
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
#include "rtsp_session.h"
#include "rtsp_handler.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspHandler class

RtspHandler::RtspHandler()
{
	fLastMediaSourceId	= 0;
	fRtspContext		= NULL;
	fSetupCounter		= 0;
}

void RtspHandler::Close()
{
	if (fSnapshotHandler) {
		fSnapshotHandler->Close();
		fSnapshotHandler = NULL;
	}

	fRequestList.Clear();
}

/** 返回这个连接允许的方法. */
String RtspHandler::GetAllowedMethods()
{
	return "DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, SET_PARAMETER, GET_PARAMETER";
}

/** 处理指定的收到的请求消息. */
void RtspHandler::HandleRequest(RtspMessage* request)
{
	if (request == NULL) {
		return;
	}

	RtspConnectionPtr rtspConnection = dynamic_cast<RtspConnection*>(request->GetRtspConnection().ptr);
	if (rtspConnection == NULL) {
		return;
	}

	RtspMessagePtr response = new RtspMessage();
	response->SetRequest(FALSE);
	response->SetProtocol("RTSP/1.0");
	response->SetProtocol(request->GetProtocol());
	response->SetRtspConnection(rtspConnection);

	// 
	String cSeq = request->GetHeader("CSeq");
	if (!cSeq.IsEmpty()) {
		response->SetHeader("CSeq", cSeq);
	}

	// 
	if (fRtspContext == NULL) {
		ASSERT(fRtspContext);
		response->SetStatusCode(RtspMessage::InternalError);
		response->Send();
		return;
	}

	// 
	if (LOG_IS_DEBUG) {
		if (request->GetMethod() != "INFO" && request->GetMethod() != "POST") {
			//LOG_D("<<< %s %s\r\n", 
			//	request->GetMethod().c_str(), 
			//	connection->GetPeerAddress());
			//LOG_D("%s\r\n", request->ToString().c_str());
		}
	}

	// Check Authorization 
	BOOL isAuthed = rtspConnection->IsAuthed();
	RtspContext* rtspContext = dynamic_cast<RtspContext*>(fRtspContext);

	if (!isAuthed && !rtspContext->CheckAuthorization(rtspConnection, request)) {
		HandleUnautherticate(request, response);
		response->Send();
		return;
	}

	request->SetHeader("X-Vision-Remote-User", rtspConnection->GetRemoteUser());
	String method = request->GetMethod();

	if (method == "DESCRIBE") {
		HandleDESCRIBE(request, response);

	} else if (method == "OPTIONS") {
		HandleOPTIONS(request, response);

	} else if (method == "SETUP") {
		HandleSETUP(request, response);

	} else if (method == "PLAY") {
		HandlePLAY(request, response);

	} else if (method == "TEARDOWN") {
		HandleTEARDOWN(request, response);

	} else if (method == "PAUSE") {
		HandlePAUSE(request, response);

	} else if (method == "GET_PARAMETER") {
		HandleGET_PARAMETER(request, response);

	} else if (method == "SET_PARAMETER") {
		HandleSET_PARAMETER(request, response);

	} else if (method == "GET") {
		HandleGET(request, response);

	} else {
		HandleNotImplement(request, response);
	}

	if (response->GetHeader("Connection") == "close") {
		rtspConnection->SetConnectionClose(TRUE);
	}

	UINT status = response->GetStatusCode();
	if (status >= 200 && status != RtspMessage::NoResponse) {
		response->Send();
	}
}

/** 处理指定的收到的应答消息. */
void RtspHandler::HandleResponse( RtspMessage* response )
{

}

/** 
 * Handle DESCRIBE method. 
 * 客户端发送这个请求取得服务端对某个 URL 的描述, 如果多少个流, 各是什么压缩
 * 格式. 这些信息一般采用 SDP 协议来描述. 详细信息可以参考 RFC SDP 文档.
 */
void RtspHandler::HandleDESCRIBE(RtspMessage* request, RtspMessage* response)
{
	RtspConnection* rtspConnection = dynamic_cast<RtspConnection*>(request->GetRtspConnection().ptr);
	if (rtspConnection == NULL) {
		return;
	}

	rtspConnection->SetAuthed(TRUE);

	Uri url(request->GetURL());
	String path = url.GetPathName();
	if (path.IsEmpty()) {
		path += '/';
	}

	RtspContext* rtspContext = dynamic_cast<RtspContext*>(fRtspContext);

	// 取得对应指定的 URL 的 SDP 字符串
	IMediaSourcePtr mediaSource = rtspContext->GetMediaSource(path);	
	if (mediaSource == NULL) {
		if (path.GetLength() <= 4 || path.SubString(path.GetLength() - 4) != ".3gp") {
			response->SetStatusCode(RtspMessage::NotFound);
			return;
		}

		// 如果子码流不存在, 则请求主码流?
		path = path.SubString(0, path.GetLength() - 4) + ".mp4";
		mediaSource = rtspContext->GetMediaSource(path);
	}

	String sdpString;
	if (mediaSource) {
		sdpString = mediaSource->GetRtspSdpString(rtspConnection->GetLocalAddress());
		fLastMediaSourceId = mediaSource->GetSourceId();
	}

	if (sdpString.IsEmpty()) {
		response->SetStatusCode(RtspMessage::NotFound);
		return;
	}

	response->SetStatusCode(RtspMessage::OK);
	response->SetHeader("Content-Type", "application/sdp");
	response->SetHeader("Content-Base", rtspConnection->GetContentBase(url));
	response->SetContent(sdpString.c_str(), sdpString.GetLength());

	fSetupCounter = 0;
}

void RtspHandler::HandleLiveRequest( RtspMessage* request, RtspMessage* response )
{
	if (request == NULL || response == NULL) {
		return;
	}

	Uri uri(request->GetURL());

	RtspContext* rtspContext = dynamic_cast<RtspContext*>(fRtspContext);
	if (rtspContext == NULL) {
		response->SetStatusCode(404);
		return;
	}

	IMediaSourcePtr mediaSource = rtspContext->GetMediaSource(uri.GetPathName());	
	if (mediaSource == NULL) {
		response->SetStatusCode(404);
		return;
	}

	RtspSnapshotHandlerPtr snaphost = new RtspSnapshotHandler();
	if (snaphost == NULL) {
		response->SetStatusCode(RtspMessage::NotFound);
		return;
	}

	snaphost->SetSnapshot(response);

	if (mediaSource) {
		mediaSource->AddMediaSink(snaphost);
	}

	fSnapshotHandler = snaphost;
}

/** 
 * Handle GET method. 
 * 客户端可以通过浏览器访问 554 端口读取服务器当前的运行状态.
 */
void RtspHandler::HandleGET(RtspMessage* request, RtspMessage* response)
{
	String value = request->GetHeader("Connection");
	if (!value.IsEmpty()) {
		response->SetHeader("Connection", value);
	}
	response->SetHeader("Content-Length", 0);

	Uri uri(request->GetURL());
	if (uri.GetPathName().StartsWith("/live")) {
		HandleLiveRequest(request, response); 

	} else {
		HandleNotImplement(request, response);
	}
}

void RtspHandler::HandleGET_PARAMETER( RtspMessage* request, RtspMessage* response )
{
	response->SetStatusCode(RtspMessage::OK);
}

/** 处理没有没有实现的其他方法, 子类可以通过重载这个方法来处理其他扩展的方法. */
void RtspHandler::HandleNotImplement(RtspMessage* request, RtspMessage* response)
{
	RtspContext* rtspContext = (RtspContext*)fRtspContext;
	if (rtspContext) {
		rtspContext->HandleRequest(request, response);
	}

	int statusCode = response->GetStatusCode();
	if (statusCode < 200 || statusCode > 699) {
		response->SetStatusCode(RtspMessage::NotImplemented);
	}
}

/** 
 * Handle RTSP OPTIONS request-> 
 * 客户端发送这个请求来确认服务端支持哪些 RTSP 方法. 简单回复即可.
 */
void RtspHandler::HandleOPTIONS( RtspMessage* request, RtspMessage* response)
{
	response->SetStatusCode(RtspMessage::OK);
	response->SetHeader("Public", GetAllowedMethods());
}

/** 
 * Handle RTSP PLAY method. 
 * 客户端发送这个请求通知服务端开始传输数据
 */
void RtspHandler::HandlePLAY(RtspMessage* request, RtspMessage* response)
{
	RtspConnectionPtr rtspConnection = dynamic_cast<RtspConnection*>(request->GetRtspConnection().ptr);

	RtspSessionPtr	rtspSession	= rtspConnection->GetRtspSession();
	if (rtspSession == NULL) {
		response->SetStatusCode(RtspMessage::SessionNotFound);
		return;
	}

	if (rtspSession->IsMulticast()) {
		response->SetStatusCode(RtspMessage::OK);
		return;

	} else if (rtspSession->GetSessionState() > kRtspStateReady) {
		// 会话已经启动了
		response->SetStatusCode(RtspMessage::OK);
		String value = request->GetHeader("Scale");
		if (!value.IsEmpty()) {
			float scale = (float)atof(value.c_str());
			rtspSession->SetScale(scale);
			response->SetHeader("Scale", value);
		}
		return;
	}

	rtspSession->SetConnection(rtspConnection);
	if (rtspSession->Start(fRtspContext) < 0) {
		response->SetStatusCode(RtspMessage::InternalError); // 会话启动失败
		return;
	}

	// 取得客户端请求的通道
	rtspSession->StateMachine("PLAY");

	RtspContext* rtspContext = (RtspContext*)fRtspContext;
	if (rtspContext) {
		rtspContext->OnAddRtspSession(rtspSession);
	}

	response->SetStatusCode(RtspMessage::OK);

	if (!rtspSession->IsLiveStreaming()) {
		// Range
		String value = request->GetHeader("Range");
		RtspRange range;
		if (!value.IsEmpty()) {
			if (range.Parse(value)) {
				rtspSession->SetRange(range.fStart, range.fEnd);
				range.fEnd = rtspSession->GetDuration();
				response->SetHeader("Range", range.ToString());

			} else {
				response->SetStatusCode(RtspMessage::InvalidRange);
			}
		}
		range.fEnd = rtspSession->GetDuration();
		response->SetHeader("Range", range.ToString());

		// Scale
		value = request->GetHeader("Scale");
		if (!value.IsEmpty()) {
			float scale = (float)atof(value.c_str());
			rtspSession->SetScale(scale);
			response->SetHeader("Scale", value);
		}
	}

	rtspSession->RenewStream();

	// 生成 RTP-Info 消息头.
	Uri uri(request->GetURL());
	String url = rtspConnection->GetContentBase(uri);
	//response->SetHeader("RTP-Info", rtspSession->GetRtpInfo(url));

	// Range
	String range = request->GetHeader("Range");
	if (!range.IsEmpty()) {
		response->SetHeader("Range", range);
	}
}

/**
 * 处理暂停请求.
 * RTSP PAUSE 要求服务端暂时停止发送数据, 并且状态机进入 PAUSE 状态.
 */
void RtspHandler::HandlePAUSE(RtspMessage* request, RtspMessage* response) 
{
	RtspConnection* rtspConnection = dynamic_cast<RtspConnection*>(request->GetRtspConnection().ptr);
	RtspSessionPtr rtspSession = rtspConnection->GetRtspSession();

	if (rtspSession == NULL) {
		response->SetStatusCode(RtspMessage::SessionNotFound);
		return;
	}
	
	if (!rtspSession->IsMulticast()) {
		rtspSession->StateMachine("PAUSE");
	}

	rtspConnection->ResetWriter();
	response->SetStatusCode(RtspMessage::OK);
}

void RtspHandler::HandleSET_PARAMETER(RtspMessage* request, RtspMessage* response)
{
	RtspConnection* rtspConnection = dynamic_cast<RtspConnection*>(request->GetRtspConnection().ptr);

	response->SetStatusCode(200);
	String value = request->GetHeader("X-Request-Key-Frame");
	if (!value.IsEmpty()) {
		if (rtspConnection == NULL) {
			return;
		}

		RtspSessionPtr rtspSession = rtspConnection->GetRtspSession();
		if (rtspSession) {
			rtspSession->RenewStream();
		}

		return;
	}

	HandleNotImplement(request, response);
}

/** 
 * Handle SETUP method. 
 * 客户端发送这个请求来协商流的传输方式等.
 */
void RtspHandler::HandleSETUP(RtspMessage* request, RtspMessage* response)
{
	RtspConnection* rtspConnection = dynamic_cast<RtspConnection*>(request->GetRtspConnection().ptr);

	RtspTransport transport;
	transport.Parse(request->GetHeader("Transport"));

	srand((int)GetSysTickCount());
	int port = (random() % 500) * 2 + 10000; // 随机生成一个 10000 ~ 12000 间的端口

	strncpy(transport.fDestination, rtspConnection->GetPeerAddress(), 32);
	transport.fServerPort = port;

	// 如果请求的是多播的传输方式
	if (transport.fIsMultcast) {
		response->SetStatusCode(RtspMessage::NotImplemented);
		return;

		// 如果请求的是单播的传输方式
	} else {
		RtspSessionPtr rtspSession = rtspConnection->GetRtspSession(TRUE);
		if (rtspSession == NULL) {
			response->SetStatusCode(RtspMessage::NotFound);
			return;
		}

		RtspContext* rtspContext = dynamic_cast<RtspContext*>(fRtspContext);
		IMediaSourcePtr mediaSource = rtspContext->GetMediaSource(NULL, fLastMediaSourceId);	
		if (mediaSource) {
			rtspSession->SetMediaSource(mediaSource);
		}

		rtspSession->SetTransport(transport, (fSetupCounter == 0));
		rtspSession->StateMachine("SETUP");
	}

	fSetupCounter++;

	char transportHeader[MAX_PATH + 1];
	memset(transportHeader, 0, sizeof(transportHeader));
	int bufferSize = sizeof(transportHeader) - 1;

	if (transport.fIsMultcast) {
		snprintf(transportHeader, bufferSize, 
			"RTP/AVP;multicast;destination=%s;port=%d-%d;ttl=%d;mode=\"PLAY\"",
			(LPCSTR)transport.fDestination,
			transport.fClientPort, transport.fClientPort + 1, transport.fTtl);

	} else if (transport.fStreamingMode == TRANSPORT_TCP) {
		snprintf(transportHeader, bufferSize, 
			"RTP/AVP/TCP;unicast;interleaved=%d-%d;mode=\"PLAY\"",
			transport.fInterleaved, transport.fInterleaved + 1);

	} else if (transport.fStreamingMode == TRANSPORT_UDP) {
		snprintf(transportHeader, bufferSize, 
			"RTP/AVP;unicast;server_port=%d-%d;client_port=%d-%d;destination=%s;mode=\"PLAY\"",
			transport.fServerPort, transport.fServerPort + 1,
			transport.fClientPort, transport.fClientPort + 1,
			(LPCSTR)transport.fDestination);

	} else {
		response->SetStatusCode(RtspMessage::UnsupportedTransport); // 不支持的传输类型.
		return;
	}

	response->SetStatusCode(RtspMessage::OK);
	response->SetHeader("Transport", transportHeader);
}

/** 
 * Handle RTSP TEARDOWN method. 
 * 客户端发送这个请求来通知服务端停止发送流.
 */
void RtspHandler::HandleTEARDOWN(RtspMessage* request, RtspMessage* response)
{	
	RtspConnection* rtspConnection = dynamic_cast<RtspConnection*>(request->GetRtspConnection().ptr);
	RtspSessionPtr rtspSession = rtspConnection->GetRtspSession();

	if (rtspSession == NULL) {
		response->SetStatusCode(RtspMessage::SessionNotFound);
		return;
	}

	if (!rtspSession->IsMulticast()) {
		rtspSession->StateMachine("TEARDOWN");
		rtspSession->Close();
	}

	rtspConnection->ResetWriter();
	response->SetStatusCode(RtspMessage::OK);
}

/**
 * 回复未通过身份认证的请求.
 *
 * @param request 要回复的请求消息.
 */
void RtspHandler::HandleUnautherticate(RtspMessage* request, RtspMessage* response)
{
	response->SetStatusCode(RtspMessage::UnAuthorized);

	String auth;
	auth.Format("Digest qop=\"auth\",realm=\"ipcam\",nonce=\"%lu\"", 
		(ULONG) GetSysTickCount());
	response->SetHeader("WWW-Authenticate", auth);

	if (request->GetProtocol().StartsWith("HTTP/")) {
		String html = "<html><head><title>401 Unauthorized</title></head>"
			"<body><h1>401 Unauthorized</h1><hr/></body></html>\r\n";
		response->SetContent(html, html.GetLength());
	}
}

void RtspHandler::SetRtspContext( IRtspContext* context )
{
	fRtspContext = context;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSnapshotHandler class

void RtspSnapshotHandler::Close()
{
	fSnapshotResponse = NULL;
}

int RtspSnapshotHandler::OnMediaDistribute( int channel, IMediaSample* mediaSample )
{
	if (fSnapshotResponse == NULL) {
		return -1;
	}

	RtspMessagePtr response = fSnapshotResponse;

	// 通知传输层, 缓存区有新的数据可发送
	RtspConnectionPtr connection = dynamic_cast<RtspConnection*>(response->GetRtspConnection().ptr);
	if (connection == NULL) {
		LOG_D("Invalid fConnection");
		return -1;
	}

	String length = StringUtils::ValueOf(mediaSample->GetSampleSize());

	response->SetStatusCode(200);
	response->SetHeader("Content-Type", "image/jpeg");
	response->SetHeader("Content-Length", length);
	response->Send();

	connection->SendMessageContent(mediaSample);
	return -1;
}

int RtspSnapshotHandler::OnMediaSourceChange( UINT flags )
{
	return 0;
}

void RtspSnapshotHandler::SetSnapshot( RtspMessage* snapshotResponse )
{
	fSnapshotResponse = snapshotResponse;
}

}
