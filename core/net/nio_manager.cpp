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

#include "nio_manager.h"


namespace core {
namespace net {



SocketManagerPtr SocketManagerCreate()
{
	return new BaseSocketManager();
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseSocketServer class 

void BaseSocketServer::BaseServerSocket::OnAccept( int errorCode )
{
	BaseSocketServerPtr server = fBaseSocketServer;
	if (server) {
		server->OnAccept(errorCode);
	}
}

BaseSocketServer::BaseSocketServer()
{
	fListenPort = 0;
}

int BaseSocketServer::Close()
{
	SmartPtr<BaseServerSocket> socket = fSocket;
	fSocket = NULL;

	if (socket) {
		socket->fBaseSocketServer = NULL;
		socket->Close();
		socket = NULL;
	}

	fListener = NULL;

	return 0;
}

int BaseSocketServer::GetConnectionCount()
{
	return 0;
}

int BaseSocketServer::Listen( UINT socketPort, LPCSTR socketAddress /*= NULL*/, int backlog /*= 5*/ )
{
	if (fSocket != NULL) {
		return -1;
	}

	fListenPort = socketPort;

	SmartPtr<BaseServerSocket> socket = new BaseServerSocket();
	socket->fBaseSocketServer = this;

	// Create the listen socket. -------------------------
	if (!socket->Create()) {
		LOG_W("Create HTTP listen socket (%d) failed. %s.\r\n", fListenPort,
			strerror(errno));
		return -1;
	}

	if (socket->SetSocketOption(SO_REUSEADDR, 1) < 0) {
		LOG_W("Ignoring error on setsockopt SO_REUSEADDR: %s\r\n", strerror(errno));
	}

	// Bind listen port
	if (!socket->Bind(fListenPort)) {
		LOG_E("Unable bind to (HTTP:%d) port. %s\r\n", fListenPort, strerror(errno));
		socket->Close();
		return -1;
	}

	// Start listen
	if (!socket->Listen(backlog)) {
		LOG_E("Error when attempting to listen on (HTTP:%d). %s\r\n", fListenPort,
			strerror(errno));
		socket->Close();
		return -1;
	}

	LOG_D("The server listen at (%d)\r\n", fListenPort);
	socket->SetNoBlock(TRUE);

	SocketSelectorPtr selector = fSelector;
	if (selector) {
		selector->AsyncSelect(socket, OP_ACCEPT);
	}

	fSocket = socket;
	return 0;
}

void BaseSocketServer::OnAccept( int errorCode )
{
	SmartPtr<BaseServerSocket> server = fSocket;
	if (server == NULL) {
		Close();
		return;
	}

	// 创建新的连接
	while (TRUE) {
		BaseSocketConnectionPtr connection = new BaseSocketConnection();
		SocketPtr socket = connection->GetSocket();
		if (!server->Accept(*socket)) {
			return;
		}

		socket->SetNoBlock(TRUE);
		connection->OnInit();

		SocketSelectorPtr selector = fSelector;
		if (selector) {
			selector->AsyncSelect(socket, OP_READ);
		}

		SocketListenerPtr listener = fListener;
		if (listener) {
			listener->OnEvent(kSocketEventConnection, 0, connection);
		}
	}
}

void BaseSocketServer::SetSelector( SocketSelector* selector )
{
	fSelector = selector;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseSocketConnection class 


void BaseSocketConnection::BaseConnectionSocket::OnClose( int errorCode )
{
	BaseSocketConnectionPtr connection = fBaseSocketConnection;
	SocketListenerPtr listener = connection ? connection->GetListener() : NULL;
	if (listener) {
		listener->OnEvent(kSocketEventClose, 0, connection);
	}
}

void BaseSocketConnection::BaseConnectionSocket::OnReceive( int errorCode )
{
	BaseSocketConnectionPtr connection = fBaseSocketConnection;
	if (connection) {
		connection->OnReceive(errorCode);
	}
}

void BaseSocketConnection::BaseConnectionSocket::OnSend( int errorCode )
{
	BaseSocketConnectionPtr connection = fBaseSocketConnection;
	if (connection) {
		connection->OnSend(errorCode);
	}	
}

void BaseSocketConnection::BaseConnectionSocket::OnConnect( int errorCode )
{
	BaseSocketConnectionPtr connection = fBaseSocketConnection;
	SocketListenerPtr listener = connection ? connection->GetListener() : NULL;
	if (listener) {
		listener->OnEvent(kSocketEventConnection, 0, connection);
	}
}

BaseSocketConnection::BaseSocketConnection()
{
	fSocket = new BaseConnectionSocket();
	fSocket->fBaseSocketConnection = this;

	fRecvBuffer	= new ByteBuffer(1024 * 16);
	fSendBuffer	= new ByteBuffer(1024 * 64);

	fReadFailedTimes = 0;
}

int BaseSocketConnection::Connect( UINT socketPort, LPCSTR socketAddress /*= NULL*/ )
{
	return 0;
}

int BaseSocketConnection::Close()
{
	SmartPtr<BaseConnectionSocket> socket = fSocket;
	fSocket = NULL;

	if (socket) {
		socket->fBaseSocketConnection = NULL;
		socket->Close();
		socket = NULL;
	}

	if (fRecvBuffer) {
		fRecvBuffer = NULL;
	}

	if (fSendBuffer) {
		fSendBuffer	= NULL;
	}

	fListener = NULL;

	return 0;
}

int BaseSocketConnection::End()
{
	return 0;
}

SocketListenerPtr BaseSocketConnection::GetListener()
{
	return fListener;
}

SocketPtr BaseSocketConnection::GetSocket()
{
	return fSocket.ptr;
}

int BaseSocketConnection::OnInit()
{
	SmartPtr<BaseConnectionSocket> socket = fSocket;
	if (socket == NULL) {
		Close();
		return 0;
	}

	char address[32];
	memset(address, 0, sizeof(address));
	UINT port = 0;
	socket->GetPeerName(address, port);

	fRemoteAddress.SetAddress(address);
	fRemoteAddress.SetPort(port);
	return 0;
}

void BaseSocketConnection::OnReceive( int errorCode )
{
	SmartPtr<BaseConnectionSocket> socket = fSocket;
	if (socket == NULL) {
		Close();
		return;
	}

	ByteBufferPtr recvBuffer = fRecvBuffer;
	if (recvBuffer == NULL) {
		LOG_D("Invalid HTTP recv buffer!\r\n");
		Close();
		return;
	}

	while (TRUE) {
		// Compact buffer
		if (recvBuffer->GetFreeSize() < 256) {
			recvBuffer->Compact();
			if (recvBuffer->GetFreeSize() <= 0) {
				// 没有足够的缓存区空间, 可能收到了非法的数据包
				LOG_D("HTTP recv buffer too full!\r\n");
				Close();
				break;
			}
		}

		// 接收数据
		char* buffer = (char*)recvBuffer->GetFreeBuffer();
		UINT freeSize = recvBuffer->GetFreeSize();
		int size = socket->Receive(buffer, freeSize);
		if (size <= 0) {
			if (fReadFailedTimes++ > 10) {
				Close();
			}
			break;
		}

		fReadFailedTimes = 0;
		recvBuffer->Extend(size);

		buffer = (char*)recvBuffer->GetFreeBuffer();
		*buffer = '\0';

		SocketListenerPtr listener = fListener;
		if (listener) {
			listener->OnRead(0, recvBuffer, this);
		}
	}
}

void BaseSocketConnection::OnSend( int errorCode )
{
	SmartPtr<BaseConnectionSocket> socket = fSocket;
	if (socket == NULL) {
		Close();
		return;
	}

	ByteBufferPtr sendBuffer = fSendBuffer;
	if (sendBuffer == NULL) {
		Close();
		return;
	}

	while (TRUE) {
		// Fill buffer
		if (sendBuffer->GetSize() <= 0) {
			// TODO: 
			SocketListenerPtr listener = fListener;
			if (listener) {
				listener->OnSend(0, sendBuffer, this);
			}
		}

		int leftover = sendBuffer->GetSize();
		if (leftover <= 0) {
			break;
		}

		// Send data
		char *data = (char*)sendBuffer->GetData();
		int ret = SendData(data, leftover);
		if (ret <= 0) {
			break;
		}
		sendBuffer->Skip(ret);
	}

	// Send flag
	if (sendBuffer->IsEmpty()) {
		if ((socket->fEvents & OP_SEND) == OP_SEND) {
			socket->AsyncSelect(OP_READ);
		}
	}
}
/** 
 * 发送指定的长度的数据.
 * @param data 要发送的数据
 * @param data_len 要发送的数据的长度.
 * @return 返回成功发送的数据的长度. 如果发生严重的错误, 则返回 -1.
 */
int BaseSocketConnection::SendData(LPCSTR data, int dataLen)
{
	SmartPtr<BaseConnectionSocket> socket = fSocket;
	if (socket == NULL) {
		Close();
		return 0;
	}

	if (data == NULL || dataLen <= 0) {
		return 0;
	}

	// Send
	int ret = socket->Send(data, dataLen);
	if (ret > 0) {
		return ret;
	}

	// 如果不是严重的错误, 如被其他进程中断，
	// 或缓冲区中的数据还没有发送完毕.
	// If Error occur, must break, let other session to send data first.						
	if (errno == EINTR || errno == EWOULDBLOCK) {					
		socket->AsyncSelect(socket->fEvents | OP_SEND);
		return 0;

	} else {
		LOG_D("Send failed %s %d\r\n", strerror(errno), errno);
		Close(); // Fatal error, maybe connection is closed
		return ret;
	}
}

void BaseSocketConnection::SetSelector( SocketSelector* selector )
{
	fSelector = selector;
}

int BaseSocketConnection::Write( BYTE* message, UINT len )
{
	if (message == NULL || len <= 0) {
		return -1;
	}

	SmartPtr<BaseConnectionSocket> socket = fSocket;
	if (socket == NULL) {
		Close();
		return 0;
	}

	ByteBufferPtr sendBuffer = fSendBuffer;
	if (sendBuffer == NULL) {
		LOG_D("Invalid HTTP send buffer!\r\n");
		Close();
		return -1;
	}

	// Compact buffer
	if (sendBuffer->GetFreeSize() < len) {
		sendBuffer->Compact();

		if (sendBuffer->GetFreeSize() < len) {
			LOG_D("Send buffer too full!\r\n");
			Close();
			return -1;
		}
	}

	sendBuffer->Put((BYTE*)message, len);
	socket->AsyncSelect(socket->fEvents | OP_SEND);

	return len;
}

int BaseSocketConnection::IsInvalid()
{
	if (fSocket == NULL) {
		return 1;

	} else if (fSocket->sockfd <= 0) {
		return 1;
	}

	return 0;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseSocketManager class 

BaseSocketManager::BaseSocketManager()
{
	fSelector = SocketSelectorCreate();
}

void BaseSocketManager::Close()
{
	if (fSelector != NULL) {
		fSelector->Close();
		fSelector = NULL;
	}
}

SocketServerPtr BaseSocketManager::CreateServer()
{
	BaseSocketServerPtr server = new BaseSocketServer();
	server->SetSelector(fSelector);
	return server.ptr;
}

int BaseSocketManager::Open()
{
	SocketSelectorPtr selector = fSelector;
	if (selector) {
		selector->Open();

	} else {
		LOG_W("Invalid selector");
	}

	return 0;
}

void BaseSocketManager::WaitEvents( int timeout )
{
	SocketSelectorPtr selector = fSelector;
	if (selector) {
		selector->WaitEvents(timeout);

	} else {
		LOG_W("Invalid selector");
	}
}



} //
}

