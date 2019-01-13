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
#include "rtsp_connection.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspConnection class

int RtspConnection::gConnectionCount = 0;
static int gConnectionIdConter = 0;

/**
 * 构建一个新的 RtspConnection 对象.
 * @param rtspServer 所属的 RtspServer.
 */
RtspConnection::RtspConnection(IRtspContext* context)
{
	fCSeq				= 1;
	fConnectionClose	= 0;
	fConnectionId		= ++gConnectionIdConter;
	fIsAuthed			= FALSE;
	fRtspContext		= context;

	fRtspHandler.SetRtspContext(context);
	fRtspSocket = new RtspSocket();
	gConnectionCount++;
}

RtspConnection::~RtspConnection()
{
	gConnectionCount--;
}

/** 关闭这个 RTSP 连接并释放相关的所有资源. */
void RtspConnection::Close()
{
	RtspSocketPtr rtspSocket = fRtspSocket;
	if (rtspSocket) {
		fRtspSocket = NULL;

		rtspSocket->Close();
	}

	fRtspHandler.Close();
	CloseRtspSession();
}

/** 关闭这个连接相关的 RTSP 会话. */
void RtspConnection::CloseRtspSession()
{
	RtspSessionPtr rtspSession = fRtspSession;
	if (rtspSession == NULL) {
		return;
	}

	// 因为组播会话可能关联到多个客户端, 所以不需要关闭.
	if (rtspSession->IsMulticast()) {
		fRtspSession = NULL;
		return;
	}

	RtspContext* rtspContext = dynamic_cast<RtspContext*>(fRtspContext);
	if (rtspContext) {
		rtspContext->OnCloseRtspSession(rtspSession);
	}

	// 关闭这个会话
	rtspSession->Close();
	rtspSession->SetConnection(NULL);
	rtspSession->SetMediaSource(NULL);
	rtspSession->SetSessionState(kRtspStateInit);

	fRtspSession = NULL;
}

/** 返回 Content-Base 的值. */
String RtspConnection::GetContentBase( Uri &url )
{
	String host = url.GetHostName();
	UINT port = url.GetPort();

	if (host.IsEmpty()) {
		host = fLocalAddress.GetAddress();
		port = fLocalAddress.GetPort();
	}

	String contentBase;
	if (port == 0 || port == kRtspDefaultPort) {
		contentBase = "rtsp://" + host;
	} else {
		contentBase.Format("rtsp://%s:%d", host.c_str(), port);
	}

	if (url.GetPathName().IsEmpty()) {
		contentBase += "/";

	} else {
		contentBase += url.GetPathName();
	}

	return contentBase;
}

/** 返回这个连接的本地的 IP 地址. */
String RtspConnection::GetLocalAddress()
{
	return fLocalAddress.GetAddress();
}

/** 
 * 返回这个连接的对方的 IP 地址.
 * Receives a IP address of the peer socket to which this socket is connected. 
 */
String RtspConnection::GetPeerAddress()
{
	return fPeerAddress.GetAddress();
}

/** 返回这个连接的对方的端口. */
UINT RtspConnection::GetPeerPort()
{
	return fPeerAddress.fPort;
}

/** 返回这个连接远端用户的名称. */
String RtspConnection::GetRemoteUser()
{
	return fRemoteUser;
}

/** 返回指向这个连接相关的 RTSP 会话对象的指针. */
RtspSessionPtr RtspConnection::GetRtspSession(BOOL create)
{
	if (fRtspSession == NULL && create) {
		fRtspSession = new RtspSession();
		fRtspSocket->fRtspWriter.SetRtspSession(fRtspSession);

		LOG_D("%s\r\n", fRtspSession->GetSessionId().c_str());
	}

	return fRtspSession;
}

/** 返回这个连接相关的会话的 ID. */
String RtspConnection::GetRtspSessionId()
{
	if (fRtspSession != NULL) {
		return fRtspSession->GetSessionId();
	}
	return "";
}

RtspSocketPtr RtspConnection::GetRtspSocket()
{
	return fRtspSocket;
}

BOOL RtspConnection::IsAuthed()
{
	return fIsAuthed;
}

BOOL RtspConnection::IsConnectionClose()
{
	return fConnectionClose;
}

BOOL RtspConnection::IsLiveStream()
{
	return (fRtspSession && fRtspSession->IsLiveStreaming());
}

BOOL RtspConnection::IsValidConnection()
{
	if (fRtspSocket == NULL || fRtspSocket->sockfd <= 0) {
		return FALSE;
	}

	return TRUE;
}

/** 检查这个连接是否有效或者超时. */
void RtspConnection::OnCheckConnection()
{
	RtspSocketPtr rtspSocket = fRtspSocket;
	if (rtspSocket == NULL || rtspSocket->sockfd <= 0) {
		return;
	}

	time_t now = time(NULL);
	UINT offset = now - rtspSocket->fLastActiveTime;
	if (offset > 80) {
		LOG_D("Close");
		Close();
	}
}

void RtspConnection::OnDump(String& dump, int indent)
{
	String text;
	time_t now = time(NULL);
	RtspSocketPtr rtspSocket = fRtspSocket;
	if (rtspSocket == NULL) {
		return;
	}

	if (indent == 1) {
		time_t lastActiveTime = rtspSocket->fLastActiveTime;
		int activeTime = (lastActiveTime > 0) ? now - lastActiveTime : -1;

		text.Format("  <tr>"
			"<td>%d</td> <td>%s</td> "
			"<td>%d</td> <td>%d</td> "
			"<td>%d</td>"
			"<td>%d</td> <td>%d</td>"
			"<td>%d</td> <td>%d/%d</td>"
			"</tr>\n", 
			fIsAuthed, fRemoteUser.c_str(), 
			activeTime, rtspSocket->fReceiveTimes, 
			0,
			fConnectionClose, fCSeq,
			fConnectionId, rtspSocket->sockfd, rtspSocket->fEvents);
		dump += text;

	} else {
		rtspSocket->fRtspReader.OnDump(dump);
		rtspSocket->fRtspWriter.OnDump(dump);
	}
}

/** 
 * 通知填充发送缓存区. 
 * 当 RTSP 会话发现发送缓存区为空时, 会自动调用这个方法, 实现 "拉" 数据模式.
 * 推模式时, 不需要处理这个方法.
 */
void RtspConnection::OnFillSendBuffer()
{
	
}

void RtspConnection::OnInit()
{
	RtspSocketPtr rtspSocket = fRtspSocket;
	if (rtspSocket) {

		UINT port = 0;
		char address[36];
		memset(address, 0, sizeof(address));
		rtspSocket->GetPeerName(address, port);
		fPeerAddress.SetAddress(address);
		fPeerAddress.SetPort(port);

		rtspSocket->SetListener(this);
	}
}

void RtspConnection::OnRtspConnect( int errorCode )
{

}

/** Handle RTSP/HTTP Request. */
void RtspConnection::OnRtspMessage(RtspMessage* rtspMessage)
{
	if (rtspMessage == NULL) {
		return;
	}

	rtspMessage->SetRtspConnection(this);

	RtspSocketPtr rtspSocket = fRtspSocket;
	if (rtspSocket) {
		UINT port = 0;
		char address[36];
		memset(address, 0, sizeof(address));

		rtspSocket->GetPeerName(address, port);
		fPeerAddress.SetAddress(address);
		fPeerAddress.SetPort(port);

		rtspSocket->GetSockName(address, port);
		fLocalAddress.SetAddress(address);
		fLocalAddress.SetPort(port);
	}

	// 如果是 RTSP 回复消息
	if (rtspMessage->IsRequest()) {
		fRtspHandler.HandleRequest(rtspMessage);

	} else {
		fRtspHandler.HandleResponse(rtspMessage);
	}
}

void RtspConnection::OnRtspPacket(char* packet, size_t packetSize)
{
	if (packet == NULL || packetSize == 0) {
		return;
	}
}

/** 处理发送事件. */
void RtspConnection::OnRtspSend( int errorCode )
{
	// 如果会话或者相关的媒体源已经失效
	if (fRtspSession && fRtspSession->GetSourceChangeFlags()) {
		LOG_D("GetSourceChangeFlags");
		Close();
		return;
	}

	if (fRtspSession) {
		fRtspSession->OnSend();
	}

	RtspSocketPtr rtspSocket = fRtspSocket;
	if (rtspSocket == NULL) {
		ASSERT_PARAM(fRtspSocket);
		Close();
		return;
	}

	BOOL isEmptyBuffer = rtspSocket->OnSendBufferData();
	if (isEmptyBuffer) {
		// 发送缓存区中的数据
		OnFillSendBuffer();

		UINT port = 0;
		char address[36];
		memset(address, 0, sizeof(address));
		rtspSocket->GetPeerName(address, port);
		fPeerAddress.SetAddress(address);
		fPeerAddress.SetPort(port);

		if (fPeerAddress.GetAddress() == "127.0.0.1") {
			Close();
		}
	}
}

/** 通知有新的数据可以发送. */
void RtspConnection::OnSendBufferNotify(int flags)
{
	if (fRtspSocket == NULL) {
		ASSERT_PARAM(fRtspSocket);
		Close();
		return;
	}

	if ((fRtspSocket->fEvents & OP_SEND) == 0) {
		fRtspSocket->AsyncSelect(OP_SEND | OP_READ);
	}
}

void RtspConnection::ResetWriter()
{
	if (fRtspSocket) {
		fRtspSocket->ResetWriter();
	}
}

/** 
 * 发送 NOTIFY 消息. 注意这是一个扩展消息, 用于从服务端主动向客户端发送
 * 事件通知. 这个消息只可用于控制用的连接, 不可用于 RTSP 连接.
 * 消息的长度不可以超过 4 K.
 *
 * @param content 要发送的消息内容, 如果是二进制数据一般需采用 base64 编码.
 * @param contentType 要发送的消息内容的类型.
 * @return 返回发送的字节数.
 */
int RtspConnection::SendNotifyMessage( LPCSTR content, LPCSTR contentType, LPCSTR eventType )
{
	RtspMessagePtr request = new RtspMessage("NOTIFY", "*", kRtspVersion);
	request->AddHeader("Event",	"ipcam");
	request->AddHeader("CSeq",	StringUtils::ValueOf(fCSeq++));
	request->SetHeader("Date",	Date::ToRfcString(Date::GetCurrentTime()));
	String sessionId = GetRtspSessionId();
	if (!sessionId.IsEmpty()) {
		request->SetHeader("Session", sessionId);
	}

	int contentLength = content ? strlen(content) : 0;
	if (contentLength > 0) {
		if (isempty(contentType)) {
			contentType = "text/plain";
		}

		request->AddHeader("Content-Type", contentType);
		request->SetContent(content, contentLength);
	}

	String message = request->ToString();
	return SendMessage(message, message.GetLength());
}

/** 
 * 发送指定的 RTSP 消息.
 *
 * @param message 要发送的 RTSP 消息内容.
 * @param length 要发送的 RTSP 消息的长度.
 * @return 如果参数无效则返回 -1, 否则返回发送的数据的长度.
 */
int RtspConnection::SendMessage(LPCSTR message, size_t length)
{
	if (fRtspSocket == NULL) {
		return 0;
	}
	//LOG_D("%s\r\n", message);

	OnSendBufferNotify(1);
	return fRtspSocket->SendMessage(message, length);
}

/**
 * 发送指定的一帧数据, 一般是图片
 *
 * @param mediaSample 要发送的图片数据内容
 */
int RtspConnection::SendMessageContent( IMediaSample* mediaSample )
{
	if (mediaSample == NULL) {
		return 0;

	} else if (fRtspSession != NULL) {
		return 0; // 已经创建了 RTSP 会话

	} else if (fRtspSocket == NULL) {
		return 0;

	} else {
		OnSendBufferNotify(1);
		return fRtspSocket->SendMessageContent(mediaSample);
	}

	return 0;
}

/** 
 * 发送指定的 RTSP 应答消息. 
 *
 * @param response 要发送的 RTSP 应答消息.
 * @return 如果参数无效则返回 -1, 否则返回发送的数据的长度.
 */
int RtspConnection::SendResponse(RtspMessage* response)
{
	if (response->GetStatusCode() < 100 || response->GetStatusCode() > 699) {
		response->SetStatusCode(RtspMessage::InternalError);
	}

	response->SetRequest(FALSE);
	if (response->GetStatusText().IsEmpty()) {
		response->SetStatusText(RtspMessage::GetStatusString(response->GetStatusCode()));
	}

	if (response->GetProtocol().IsEmpty()) {
		response->SetProtocol(kRtspVersion);
	}

	response->SetHeader("Server", RTSP_SERVER_NAME);
	response->SetHeader("Date", Date::ToRfcString(Date::GetCurrentTime()));

	String sessionId = GetRtspSessionId();
	if (!sessionId.IsEmpty()) {
		response->SetHeader("Session", sessionId);
	}

	String message = response->ToString();

	//LOG_E("%d\r\n", message.GetLength());
	//LOG_E("%s\r\n", message.c_str());

	return SendMessage(message, message.GetLength());
}

void RtspConnection::SetAuthed( BOOL authed )
{
	fIsAuthed = authed;
}

void RtspConnection::SetConnectionClose( BOOL connectionClose )
{
	fConnectionClose = connectionClose;
}

/** 设置这个连接远端用户的名称. */
void RtspConnection::SetRemoteUser( LPCSTR user )
{
	fRemoteUser = user ? user : "";
}


}
