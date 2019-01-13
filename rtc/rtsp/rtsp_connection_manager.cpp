/***
 * The content of this file or document is CONFIDENTIAL and PROPRIETARY
 * to ChengZhen(Anyou).  It is subject to the terms of a
 * License Agreement between Licensee and ChengZhen(Anyou).
 * restricting among other things, the use, reproduction, distribution
 * and transfer.  Each of the embodiments, including this information and
 * any derivative work shall retain this copyright notice.
 *
 * Copyright (c) 2014-2015 ChengZhen(Anyou). All Rights Reserved.
 *
 */
#include "stdafx.h"

#include "rtsp_context.h"
#include "rtsp_connection_manager.h"

namespace rtsp { 

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// ListenSocket class 

RtspConnectionManager::ListenSocket::ListenSocket() : manager(NULL)
{
	fListenPort = 0;
}

/** 有新的连接请求. */
void RtspConnectionManager::ListenSocket::OnAccept(int errorCode) 
{
	if (manager) {
		manager->OnAccept(this);
	}
}

BOOL RtspConnectionManager::ListenSocket::CreateSocket( UINT port )
{
	fListenPort = port;

	if (!Socket::Create()) {
		LOG_W("Create listen socket failed: %s.\r\n", strerror(errno));
		return FALSE;
	}

	if (SetSocketOption(SO_REUSEADDR, 1) < 0) {
		LOG_W("Ignoring error on setsockopt SO_REUSEADDR: %s\r\n", strerror(errno));
	}

	// Bind listen port
	if (!Bind(fListenPort)) {
		fListenPort = port * 10;

		if (!Bind(fListenPort)) {
			LOG_W("Unable to bind to tcp port(%d): %s\r\n", port, strerror(errno));

			Close();
			return FALSE;
		}
	}

	// Start listen
	// 发现一个奇怪的问题, 当客户端同时发两个连接, 服务端就会死锁
	// 把 Listen 参数设为 0, 奇怪地好像就没有这个问题了....
	//if (!Listen(0)) {

	if (!Listen(10)) {
		LOG_W("Error when attempting to listen on tcp socket: %s\r\n", strerror(errno));
		Close();
		return FALSE;
	}

	return TRUE;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspConnectionManager class

RtspConnectionManager::RtspConnectionManager()
{
	fRtspContext		= NULL;
	fListenPort			= kRtspDefaultPort;
	fSelector			= SocketSelectorCreate();

	fSelector->Open();
}

void RtspConnectionManager::Close()
{
	// listen socket
	if (fListenSocket) {
		if (fListenSocket->sockfd > 0) {
			fListenSocket->Close();
		}

		fListenSocket = NULL;
	}

	// connections
	RtspConnectionList connections;
	GetConnections(connections);
	RtspConnectionList::ListIterator iter = connections.GetListIterator();
	while (iter.HasNext()) {
		RtspConnectionPtr connection = iter.Next();
		if (connection) {
			connection->Close();
		}
	}
}

int RtspConnectionManager::GetConnectionCount()
{
	return fConnections.GetSize();
}

/** 返回所有的连接. */
void RtspConnectionManager::GetConnections( RtspConnectionList& connections )
{
	connections.Clear();
	fMutex.Lock();
	RtspConnectionList::ListIterator iter = fConnections.GetListIterator();
	while (iter.HasNext()) {
		RtspConnectionPtr connection = iter.Next();
		connections.AddLast(connection);
	}
	fMutex.UnLock();
}

UINT RtspConnectionManager::GetListenPort()
{
	return fListenPort;
}

SocketSelectorPtr RtspConnectionManager::GetSocketSelector()
{
	return fSelector;
}

/** 接受新的连接. */
BOOL RtspConnectionManager::OnAccept( Socket *listenSocket )
{
	if (listenSocket == NULL) {
		return FALSE;
	}

	RtspConnectionPtr connection = new RtspConnection(fRtspContext);
	if (connection == NULL) {
		return FALSE;
	}

	usleep(1000);
	RtspSocketPtr socket = connection->GetRtspSocket();

	if (!listenSocket->Accept(*socket)) {
		LOG_D("%s\r\n", strerror(errno));
		return FALSE;
	}

	fMutex.Lock();
	fConnections.AddLast(connection);
	fMutex.UnLock();

	connection->OnInit();

	socket->SetNoBlock(TRUE);

	SocketSelectorPtr selector = fSelector;
	if (selector) {
		selector->AsyncSelect(socket, OP_READ);
	}

	return TRUE;
}

/** 回收无效的连接. */
void RtspConnectionManager::OnCheckConnections()
{
	fMutex.Lock();
	RtspConnectionList::ListIterator iter = fConnections.GetListIterator();
	while (iter.HasNext()) {
		RtspConnectionPtr connection = iter.Next();
		if (connection == NULL) {
			iter.Remove();
			break;
		}

		// 检查这个连接是否有效
		connection->OnCheckConnection();

		if (!connection->IsValidConnection()) {
			connection->Close();

			LOG_D("connection");

			iter.Remove();
			continue;
		}
	}
	fMutex.UnLock();
}

void RtspConnectionManager::OnDump(String& dump)
{
	dump += "<fieldset>\n <legend>RTSP Connection Manager</legend>\n ";

	String text;
	text.Format(
		"<table>"
		"  <tr>"
		"<th>Listen Port</th>"
		"<th>Connections</th> </tr>\n"
		"  <tr>"
		"<td>%d</td>"
		"<td>%d</td>"
		"</tr>\n </table>\n", 
		fListenPort,
		fConnections.GetSize());

	dump += text;
	dump += "</fieldset>\n";

	dump += "<fieldset>\n <legend>RTSP Selector</legend>\n ";
	dump += fSelector->OnDump(2);
	dump += "</fieldset>\n";

	RtspConnectionList connections;
	GetConnections(connections);

	dump += "<fieldset>\n <legend>RTSP Connections</legend>\n ";
	dump += "<table>\n<tr>";
	dump += "<th>Authed</th> <th>User</th>";
	dump += "<th>Active</th> <th>Receive</th>";
	dump += "<th>Subscribe</th> <th>Notify</th>";
	dump += "<th>Connection Close</th>  <th>CSeq</th> <th>Is Send File</th>";
	dump += "<th>Connection Id</th> <th>sockfd/events</th> </tr>\n";

	RtspConnectionList::ListIterator iter = connections.GetListIterator();
	while (iter.HasNext()) {
		RtspConnectionPtr connection = iter.Next();
		connection->OnDump(dump, 1);
	}

	dump += "</table>\n";
	dump += "</fieldset>\n";

	// 
	dump += "<fieldset>\n <legend>RTSP Reader/Writer</legend>\n ";
	dump += "  <table>\n";
	dump += "   <tr><th>Read Buffer Size</th><th>Content Length</th><th>Last Message</th>\n";
	dump += "   <th>Send Buffer Size</th><th>RTP Timestamp</th>";
	dump += "<th>Channel Identifier</th><th>Sample Count</th></tr>\n";

	iter = connections.GetListIterator();
	while (iter.HasNext()) {
		RtspConnectionPtr connection = iter.Next();
		connection->OnDump(dump, 2);
	}

	dump += "</table>\n";
	dump += "</fieldset>\n";
}

void RtspConnectionManager::OnTimer()
{
	OnCheckConnections();
}

int RtspConnectionManager::Open(IRtspContext* context)
{
	fRtspContext = context;

	if (fListenSocket == NULL) {
		fListenSocket = new ListenSocket();
	}
	fListenSocket->manager = this;

	// 服务器将侦听默认的 554 端口
	if (!fListenSocket->CreateSocket(fListenPort)) {
		LOG_E("Create listen socket failed: %s.\r\n", strerror(errno));
		fListenSocket->Close();
		Close();
		return RTSP_E_SOCKET;
	}

	SocketSelectorPtr selector = fSelector;
	if (selector) {
		selector->AsyncSelect(fListenSocket, OP_ACCEPT);
	}
	
	return RTSP_S_OK;
}

void RtspConnectionManager::Poll()
{
	SocketSelectorPtr selector = fSelector;
	if (selector) {
		selector->WaitEvents(200);
	}
}

/** 设置侦听端口. */
void RtspConnectionManager::SetListenPort( UINT port )
{
	fMutex.Lock();
	if (fListenPort != port) {
		fListenPort = port;
	}
	fMutex.UnLock();

}

};
