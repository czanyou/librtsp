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

#include "rtsp_client.h"
#include "rtc/vision_rtsp.h"
#include "rtc/sdp/sdp.h"
#include "rtc/rtp/rtp_common.h"

using namespace sdp;
using namespace rtp;

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSocket class

IRtspClientPtr CreateRtspClient()
{
	return new rtsp::RtspClient();
}

RtspVideoDecoder::RtspVideoDecoder()
{
	fAudioCodec			= kAudioFormatPCM;
	fAudioPayload		= -1;
	fLastSampleFlags	= 1;
	fLastSequence		= 0;
	fRawPacketFlag		= FALSE;
	fSequence			= 0;
	fVideoCodec			= kVideoFormatH264;
	fVideoFrameType		= kMediaFrameUnknown;
	fVideoPayload		= -1;

	fFragmentSequence	= 1;
}

IMediaBufferPtr RtspVideoDecoder::Decode( BYTE* data, int length, UINT isMarker, UINT timestamp )
{
	const static BYTE RTP_FRAGMENT_START = 0x80;
	const static BYTE RTP_FRAGMENT_END = 0x40;
	const static BYTE NALU_TYPE_MASK = 0x1F;

	BOOL isFragmentEnd = FALSE;
	UINT fragmentCount = 0;
	UINT fragmentSequence = 0;

	BYTE* sampleData = data;
	UINT  sampleSize = length;

	BYTE nalHeader = data[0];
	BYTE nalType = data[0] & NALU_TYPE_MASK;
	if (nalType <= MAX_SINGLE_NALU_TYPE) { // Single NALU mode
		sampleData = data;
		sampleSize = length;
		nalHeader = data[0];

	} else if (nalType == STAP_A) { // 组合包 STAP-A
		sampleData = data + 3;
		sampleSize = MAKEWORD(data[2], data[1]);
		nalHeader = data[3];

	} else if (nalType == STAP_B) { // 组合包 STAP-B
		sampleData = data + 5;
		sampleSize = MAKEWORD(data[4], data[3]);
		nalHeader = data[5];

	} else if (nalType == MTAP16) { // 组合包 MTAP16
		sampleData = data + 8;
		sampleSize = MAKEWORD(data[4], data[3]);
		nalHeader = data[8];

	} else if (nalType == MTAP24) { // 组合包 MTAP24
		sampleData = data + 9;
		sampleSize = MAKEWORD(data[4], data[3]);
		nalHeader = data[9];

	} else if (nalType == FU_A || nalType == FU_B) { // RTP 分片 FU-A / FU-B
		if (data[1] & RTP_FRAGMENT_START) { // 一个分片的开始
			sampleData = data + 1;
			sampleSize = length - 1;
			nalHeader = (data[0] & 0x60) | (data[1] & NALU_TYPE_MASK);

		} else {
			sampleData = data + 2;
			sampleSize = length - 2;
			nalHeader = 0;
		}

		// 跳过 decoding order number (16bit)
		if (nalType == FU_B) {
			sampleData += 2;
			sampleSize -= 2;
		}

		if (data[1] & RTP_FRAGMENT_END) {
			isFragmentEnd = TRUE;
		}
		

	} else {
		return NULL;
	}

	if (nalHeader != 0) {
		sampleData[0] = nalHeader;

		sampleData -= 3;
		sampleSize += 3;
		sampleData[0] = 0x00;
		sampleData[1] = 0x00;
		sampleData[2] = 0x01;

		fFragmentSequence = 1;

	} else {
		fFragmentSequence++;
	}

	fragmentSequence = fFragmentSequence;
	if (isFragmentEnd) {
		fragmentSequence = fFragmentSequence;
		fragmentCount = fragmentSequence;

		fFragmentSequence = 1;
	}

	UINT  frameType = kMediaFrameUnknown;
	BOOL  isSyncPoint = FALSE;

	if (fLastSampleFlags) {
		frameType = GetFrameType(kMediaTypeVideo, sampleData);
		if (frameType == kVideoFrameI || frameType == kAudioFrame) {
			isSyncPoint = TRUE;
		}
	}

	if (frameType == (UINT)kMediaFrameUnknown) {
		frameType = fVideoFrameType;

	} else {
		fVideoFrameType = frameType;
	}

	fLastSampleFlags = isMarker;

	MediaBufferPtr mediaBuffer = new MediaBuffer(sampleSize + 32);
	mediaBuffer->SetFrameType(frameType);
	mediaBuffer->SetLength(sampleSize);
	mediaBuffer->SetMarker(isMarker);
	mediaBuffer->SetPayloadType(0);
	mediaBuffer->SetSampleTime(timestamp);
	mediaBuffer->SetSyncPoint(isSyncPoint);
	mediaBuffer->SetFragmentCount(fragmentCount);
	mediaBuffer->SetFragmentSequence(fragmentSequence);
	memcpy(mediaBuffer->GetData(), sampleData, sampleSize);

	return mediaBuffer.ptr;
}

/** 快速取得指定的帧的类型. */
UINT RtspVideoDecoder::GetFrameType(int format, BYTE* sample)
{
	if (format == kMediaTypeAudio) {
		return kAudioFrame;

	} else if (format != kMediaTypeVideo) {
		return kMediaFrameUnknown;
	}

	DWORD frameType = kVideoFrameI;
	UINT codecType = fVideoCodec;

	// 检查 MPEG4 帧类型
	if (codecType == kVideoFormatMP4V) {
		if (sample[0] != 0x00 || sample[1] != 0x00 || sample[2] != 0x01) {
			return kMediaFrameUnknown;
		}

		if (sample[3] == 0xB0 || sample[3] == 0x00)	{
			return kVideoFrameI;

		} else if (sample[3] == 0xB6) {
			return (sample[4] >> 6) & 0x03;
		}

		return kMediaFrameUnknown;

		// 检查 H.264 帧类型
	} else if (codecType == kVideoFormatH264) {
		BYTE mark = 0;

		if (sample[0] != 0x00) {
			BYTE nalType  = sample[0] & 0x1F;
			if (nalType < 24) {	
				mark = sample[0];	// 一个 RTP 包含一个 NALU

			} else if (nalType == 24) {
				mark = sample[3];	// 一个 RTP 包含多个 NALU

			} else {
				mark = sample[1];	// 一个 NALU 分多个 RTP 包, AU RTP Header
			}

		} else if (sample[2] == 0x01) {
			mark = sample[3];

		} else if (sample[2] == 0x00 && sample[3] == 0x01) {
			mark = sample[4];
		}

		if ((mark & 0x1F) == 0x07) {
			return kVideoFrameI;

		} else {
			return kVideoFrameP;
		}
	}

	return frameType;
}


void RtspVideoDecoder::Reset()
{
	
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspClient class

RtspClient::RtspClient(void)
{
	fTransport			= TRANS_MODE_TCP;
	fRtspState			= kRtspStateUnknown;
    fCSeq				= 1;
	fListener			= NULL;
	fLastRequestCSeq	= 0;
	fLastHeartBeatTime	= 0;
	fSetupIndex			= 0;
	fClientPort			= 50000;
}

RtspClient::~RtspClient(void)
{
}

/** 关闭这个 RTSP 客户端. */
void RtspClient::Close()
{
	LOG_W("RtspClient");

	if (fRtspSocket->sockfd > 0) {
		SendTEARDOWN();

		fRtspSocket->SetListener(NULL);
		fRtspSocket->Close();
		fRtspSocket = NULL;
	}

	fSessionId	= "";
	fCSeq		= 1;

	fLastActiveTime = 0;

	SetRtspState(kRtspStateUnknown); 

	fVideoFormat.Clear();
	fAudioFormat.Clear();
}

/**
 * 尝试连接指定的服务器.
 * @param timeout 连接超时时间.
 * @return 如果连接成功则返回 TRUE, 否则返回 FALSE. 
 */
BOOL RtspClient::Connect(int timeout)
{
	if (fRtspSocket != NULL) {
		LOG_W("Invalid fRtspSocket");
		return FALSE;
	}

	SetRtspState(kRtspStateConnecting);

	LOG_I("%s:%d", fUrl.GetHostName().c_str(), fUrl.GetPort());

	fRtspSocket	= new RtspSocket();
	fRtspSocket->Create();
	fRtspSocket->SetListener(this);

	LOG_W("RtspClient (%d)", fRtspSocket->sockfd);
	fRtspSocket->Connect(fUrl.GetHostName().c_str(), fUrl.GetPort());

	if (fSelector) {
		fRtspSocket->fEvents = OP_CONNECT;
		fSelector->AsyncSelect(fRtspSocket, OP_CONNECT);
	}

	fRtspSocket->SetNoBlock(TRUE);
	return TRUE;
}

/** 返回最后收到数据的时间. */
time_t RtspClient::GetLastActiveTime()
{
	return fLastActiveTime;
}

/** 返回最后发送心跳包的时间. */
time_t RtspClient::GetLastHeartBeatTime()
{
	return fLastHeartBeatTime;
}

RtspState RtspClient::GetRtspState()
{
	return fRtspState;
}

MediaFormat& RtspClient::GetTrackFormat( UINT mediaType )
{
	return (mediaType == kMediaTypeVideo) ? fVideoFormat : fAudioFormat;
}

/** 指出当前 RTSP 连接是否超时. */
BOOL RtspClient::IsTimeout()
{
	time_t now = time(NULL);
	return abs(long(now - GetLastActiveTime())) > 20;
}

void RtspClient::HandleAudioPacket( BYTE* data, int length, UINT flags, UINT timestamp )
{
	if (fListener) {
		//fListener->OnClientPacket(kMediaTypeAudio, data,  length, flags, timestamp);
	}
}

/**
 * 处理指定的应答消息
 * @param message 应答消息
 */
void RtspClient::HandleResponse(BaseMessage* response) 
{
	// LOG_D("%s\r\n", response->ToString().c_str());

	int responseCSeq = atoi(response->GetHeader("CSeq"));
	if (responseCSeq < fLastRequestCSeq) {
		return; // 忽略所有过期的应答消息

	} else if (responseCSeq == fLastRequestCSeq) {
		String method = fLastRequest;
		if (method == "OPTIONS") {
			SendDESCRIBE();

		} else if (method == "DESCRIBE") { 
			ProcessDESCRIBEResponse(response);

		} else if (method == "SETUP") {
			ProcessSETUPResponse(response);

		} else if (method == "PLAY") {
			ProcessPLAYResponse();
		}
	}
}

/**
 * 读取 RTP 包.
 * @param packet 读取的数据存放的缓存区
 * @param buflen 缓存区的长度.
 * @return 
 */
int RtspClient::HandleRtpPacket(char* packet, size_t packetSize)
{
	//LOG_D("%d\r\n", packetSize);

	if (packet == NULL || packetSize == 0) {
		return 0;
	}

	RtpHeader* header = reinterpret_cast<RtpHeader*>(packet);
	if (header->fHead != 0x80) {
		return 0;
	}

	UINT timestamp = ntohl(header->fTimestamp);
	BYTE* data	= (BYTE*)(packet + 12);
	UINT paylod	=  header->fPayload & 0x7F;
	UINT isEnd	= (header->fPayload & 0x80) == 0x80;

	SetLastActiveTime();

	// Video Stream
	if (paylod == fVideoFormat.fPayloadType) {
		timestamp = timestamp / 90;
		int dataLength = packetSize - 12;
		IMediaBufferPtr mediaBuffer = fVideoDecoder.Decode(data, dataLength, isEnd, timestamp);
		if (fListener && mediaBuffer) {
			fListener->OnMediaStream(mediaBuffer);
		}

		// Audio Stream
	} else if (paylod == fAudioFormat.fPayloadType) {
		fAudioFormat.fSampleRate = fAudioFormat.fSampleRate == 0 ? 8000 : fAudioFormat.fSampleRate; 
		timestamp = timestamp / (fAudioFormat.fSampleRate / 1000);
		HandleAudioPacket(data, packetSize - 12, isEnd, timestamp);
	}

	return 0;
}

/**
 * 处理指定的 RTP 包.
 * @param format 
 * @param data 
 * @param length 
 * @param flags 
 * @param timestamp 
 */
void RtspClient::HandleVideoPacket( BYTE* data, int length, UINT flags, UINT timestamp )
{

}

/** 当连接成功. */
void RtspClient::OnRtspConnect( int errorCode )
{
	LOG_I("%d", GetRtspState());

	if (fSelector && fRtspSocket) {
		fRtspSocket->fEvents = OP_READ;
		fSelector->AsyncSelect(fRtspSocket, OP_READ);
	}

	SetRtspState(kRtspStateInit);
	SendDESCRIBE();
}

void RtspClient::OnRtspMessage( RtspMessage* message )
{
	HandleResponse(message);
}

void RtspClient::OnRtspPacket( char* packet, size_t buflen )
{
	HandleRtpPacket(packet, buflen);
}

void RtspClient::OnRtspSend( int errorCode )
{
	RtspSocketPtr rtspSocket = fRtspSocket;
	if (rtspSocket == NULL) {
		ASSERT_PARAM(fRtspSocket);
		Close();
		return;
	}

	rtspSocket->OnSendBufferData();
}

void RtspClient::OnTimer()
{
	time_t now = time(NULL);
	UINT span = now - fLastHeartBeatTime;
	if (span > 40) {
		if (GetRtspState() >= kRtspStateReady) {
			SendGET_PARAMETER();
		}
	}

	if (IsTimeout()) {
		Close();
		Open();
	}
}


/** 
 * 打开当前的 RTSP URL.
 * @return 如果成功则返回 TRUE.
 *	如果当前状态不为 StopedState 返回 FALSE.
 */
BOOL RtspClient::Open()
{
	if (GetRtspState() != kRtspStateUnknown) {
		LOG_W("Invalid state");
		return FALSE; // 无效的状态
	}

	SetLastActiveTime();

	if (fRtspSocket) {
		fRtspSocket->SetListener(NULL);
		fRtspSocket->Close();
		fRtspSocket = NULL;
	}

	fSetupIndex = 0;
	fClientPort = 50000 + time(NULL) % 10000;

	fVideoFormat.Clear();
	fAudioFormat.Clear();

	return Connect();
}

void RtspClient::ProcessDESCRIBEResponse( BaseMessage* response )
{
	ParseSDP(response->GetContent());
	SendSETUP();
}

void RtspClient::ProcessSETUPResponse( BaseMessage* response )
{
	if (fSessionId.IsEmpty()) {
		fSessionId = response->GetHeader("Session");
	}

	if (fSetupIndex <= 1 && fAudioFormat.fSampleRate > 0) {
		SendSETUP();
		return;
	}

	if (GetRtspState() == kRtspStateInit) {
		SetRtspState(kRtspStateReady);
		SendPLAY();
	}
}

void RtspClient::ProcessPLAYResponse()
{
	if (GetRtspState() != kRtspStatePlaying) {
		SetLastActiveTime();
		fLastHeartBeatTime = time(NULL);
		SetRtspState(kRtspStatePlaying);
	}
}

/**
 * 解析指定的 SDP 字符串.
 * @param sdpString SDP 字符串.
 * @return 如果成功则返回 0, 否则返回一个小于 0 的错误码.
 */
int RtspClient::ParseSDP(LPCSTR sdpString)
{
	if (isempty(sdpString)) {
		LOG_W("Null SDP String\r\n");
		return -1;
	}

	SdpSession sdpSession;
	sdpSession.Parse(sdpString); 

	int mediaCount = sdpSession.GetSdpMediaCount(); 
	for (int i = 0; i < mediaCount; i++) {
		SdpMediaPtr sdpMedia = sdpSession.GetSdpMedia(i);
		if (sdpMedia == NULL) {
			break;
		}

		if (sdpMedia->fMediaType == "audio") {
			fAudioFormat.fChannelCount  = sdpMedia->fRtpMap.fChannels;
			fAudioFormat.fCodecName		= sdpMedia->fRtpMap.fFormat;
			fAudioFormat.fFmtp			= sdpMedia->fControl;
			fAudioFormat.fPayloadType	= sdpMedia->fPayload;
			fAudioFormat.fSampleRate	= sdpMedia->fRtpMap.fFrequency;

			if (fAudioFormat.fChannelCount <= 0) {
				fAudioFormat.fChannelCount = 1;
			}

			if (fAudioFormat.fSampleRate == 0) {
				fAudioFormat.fSampleRate = 8000;
			}

		} else if (sdpMedia->fMediaType == "video") {
			fVideoFormat.fChannelCount	= sdpMedia->fRtpMap.fChannels;
			fVideoFormat.fCodecName		= sdpMedia->fRtpMap.fFormat;
			fVideoFormat.fFmtp			= sdpMedia->fControl;
			fVideoFormat.fPayloadType	= sdpMedia->fPayload;
			fVideoFormat.fSampleRate	= sdpMedia->fRtpMap.fFrequency;

			if (fVideoFormat.fSampleRate == 0) {
				fVideoFormat.fSampleRate = 90000;
			}

			LOG_W("control=%s", fVideoFormat.fFmtp.c_str());
		}
	}

	return 0;
}

/** 发送 DESCRIBE 请求. */
BOOL RtspClient::SendDESCRIBE()
{
	BaseMessage request("DESCRIBE", fUrl.GetHref(), kRtspVersion);
	return SendMessage(request);
}

/** 发送 SET_PARAMETER 请求. */
BOOL RtspClient::SendGET_PARAMETER()
{
	BaseMessage request("GET_PARAMETER", fUrl.GetHref(), kRtspVersion);
	fLastHeartBeatTime = time(NULL);
	return SendMessage(request);
}

/** 发送 OPTIONS 请求. */
BOOL RtspClient::SendOPTIONS()
{
	BaseMessage request("OPTIONS", fUrl.GetHref(), kRtspVersion);
	return SendMessage(request);
}

/**
 * 发送指定的 RTSP 消息
 * @param message 要发送的 RTSP 消息.
 * @return 如果成功则返回 TRUE. 
 */
BOOL RtspClient::SendMessage(BaseMessage& message)
{
	RtspState state = GetRtspState();
	if (state == kRtspStateUnknown || state == kRtspStateConnecting) {
		LOG_D("Invalid State %d\r\n", state);
		return FALSE; // 无效的状态
	}

	if (!fSessionId.IsEmpty()) {
		message.SetHeader("Session", fSessionId.c_str());
	}

	message.SetHeader("CSeq", fCSeq++);

	// 客户端通过发送大量 POST 请求发送音频消息, 所以不处理 POST 应答
	if (!isequal(message.GetMethod(), "POST")) {
		fLastRequestCSeq = atoi(message["CSeq"].c_str());
		fLastRequest = message.GetMethod();
	}

	BYTE* data = new BYTE[1024 * 4];
	String headers = message.EncodeWithoutContent();
	int len = headers.GetLength();
	memcpy(data, headers.c_str(), len);
	if (message.GetContent()) {
		int clen = message.GetContentLength();
		memcpy(data + len, message.GetContent(), clen);
		len += clen;
	}

	// LOG_W("RtspClient: %s", headers.c_str());

	BOOL flag = TRUE;
	if (fRtspSocket) {
		int ret = fRtspSocket->SendMessage((LPCSTR)data, len);
		fRtspSocket->OnSendBufferData();

		flag = (ret == len); 
	}

	delete[] data;
	return flag;
}

/** 发送 PLAY 请求. */
BOOL RtspClient::SendPLAY()
{
	RtspState state = GetRtspState();
	if (state != kRtspStateReady && state != kRtspStatePlaying) {
		return FALSE; // 无效的状态
	}

	BaseMessage request("PLAY", fUrl.GetHref(), kRtspVersion);
	request.SetHeader("Session", fSessionId);
	return SendMessage(request);
}

/** 发送 SETUP 请求. */
BOOL RtspClient::SendSETUP()
{
	MediaFormat& format = fSetupIndex ? fAudioFormat : fVideoFormat;
	String url = fUrl.GetHref();
	if (!format.fFmtp.IsEmpty()) {
		if (url.GetAt(url.GetLength() - 1) != '/') {
			url += "/";
		}
		url += format.fFmtp;
	}

	LOG_W("url=%s(%d)", url.c_str(), fSetupIndex);

	BaseMessage request("SETUP", url, kRtspVersion);
	char buffer[MAX_PATH + 1];
	memset(buffer, 0, sizeof(buffer));

	switch(fTransport) {
	case TRANS_MODE_TCP: 
	{
		int interleaved = 0 + fSetupIndex * 2;
		snprintf(buffer, MAX_PATH, "RTP/AVP/TCP;unicast;interleaved=%d-%d", 
			interleaved, interleaved + 1);
		break;
	}

	case TRANS_MODE_UDP:
	{
		int port = fClientPort + fSetupIndex * 2;
		snprintf(buffer, MAX_PATH, "RTP/AVP;unicast;client_port=%d-%d", 
			port, port + 1);
		break;
	}

	default: 
		return FALSE;
	}

	fSetupIndex++;
	request.SetHeader("Transport", buffer);
	return SendMessage(request);
}

/**
 * 请求订阅事件消息. 只有请求订阅事件, 设备才会主动地向客户端发送事件变动消息.
 * @param eventName 要订阅的事件的名称, 目前没有用到.
 */
BOOL RtspClient::SendSUBSCRIBE()
{
	if (fSessionId.IsEmpty()) {
		LOG_W("Invalid RTSP Session.\r\n");
		return -1;
	}

	BaseMessage request("SUBSCRIBE", fUrl.GetHref(), kRtspVersion);
	request.SetHeader("ST", "ipcam:event");
	request.SetHeader("CALLBACK", "*");
	request.SetHeader("TIMEOUT", "second-1800");
	return SendMessage(request);
}

/** 发送 PLAY 请求. */
BOOL RtspClient::SendTEARDOWN()
{
	BaseMessage request("TEARDOWN", fUrl.GetHref(), kRtspVersion);
	request.SetHeader("Session", fSessionId);
	return SendMessage(request);
}

/** 设置最后活跃时间. */
void RtspClient::SetLastActiveTime()
{
	fLastActiveTime = time(NULL);
}

/** 注册事件侦听器. */
void RtspClient::SetListener( IRtspClientListener* listener )
{
	fListener = listener;
}

/** 设置当前 RTSP 客户端的状态. */
void RtspClient::SetRtspState( RtspState newState )
{
	if (fRtspState == newState) {
		return;
	}

	// LOG_D("%d", newState);

	RtspState oldState = fRtspState;
	fRtspState = newState;

	if (fListener) {
		fListener->OnClientStateChange(oldState, newState);
	}
}

void RtspClient::SetSelector( SocketSelector* selector )
{
	fSelector = selector;
}

void RtspClient::SetTransport( int type )
{
	fTransport = type;
}

/** 
 * 设置要打开的 URL 地址. 
 * @param url The URL to open, ex: "rtsp://192.168.1.1:554/live/1"
 */
BOOL RtspClient::SetDataSource(LPCSTR url) 
{
	if (isempty(url)) {
		LOG_D("Null URL\r\n");
		return FALSE;
	}

	// 
	String strURL;
	if (strncmp(url, "rtsp://", 7) == 0) {
		strURL = url;

	} else {
		strURL = "rtsp://";
		strURL += url;
	}

	fUrl.Parse(strURL.c_str());
	if (fUrl.GetPort() <= 0) {
		fUrl.fPort = kRtspDefaultPort;
	}

	if (fUrl.GetHostName().IsEmpty()) {
		return FALSE;
	}

	LOG_D("%s:%d\r\n", fUrl.GetHostName().c_str(), fUrl.GetPort());
	return TRUE;
}

}
