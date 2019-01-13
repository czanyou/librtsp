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

#include "rtsp_session_track.h"
#include "rtsp_session.h"
#include "rtsp_context.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSession class

int RtspSession::gSessionCount = 0;

/**
 * 构建一个新的 RtspSession 对象. 
 * @param sessionId 这个会话的 ID.
 */
RtspSession::RtspSession(LPCSTR sessionId)
{
	if (isempty(sessionId)) {
		fSessionId = CreateSessionId();

	} else {
		fSessionId = sessionId;
	}

	fChangeFlags		= 0;
	fConnection			= NULL;	
	fRtspContext		= NULL;
	fSessionState		= kRtspStateInit;
	fSsrc				= (UINT)random() ^ 0x12345678;
	fStartTime			= 0;

	fAudioTrack.fFrameBuffer = new MediaSampleQueue(kMediaTypeAudio);
	fVideoTrack.fFrameBuffer = new MediaSampleQueue(kMediaTypeVideo);

	gSessionCount++;
}

RtspSession::~RtspSession()
{
	gSessionCount--;
}

/** Clear all parameter value. */
void RtspSession::Clear()
{
	// Reset state
	SetSessionState(kRtspStateInit);	

	fVideoTrack.Clear();
	fAudioTrack.Clear();
	fStartTime			= 0;
	fLocalAddress		= "";

	fMutex.Lock();
	fFirstTimestamp		= 0;
	fSsrc				= (UINT)random();
	fMutex.UnLock();
}

/** Close session */
void RtspSession::Close()
{
	LOG_D("RtspSession %s\r\n", GetSessionId().c_str());
	fConnection = NULL;

	// 关闭相关的 RTP 传输通道.
	fAudioTrack.Close();
	fVideoTrack.Close();

	if (fMediaSource) {
		fMediaSource->RemoveMediaSink(this);
		fMediaSource = NULL;
	}

	Clear();
	SetSessionState(kRtspStateInit);
}

/** 随机生成一个 SessionID. */
String RtspSession::CreateSessionId()
{
	char buf[MAX_PATH + 1];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, MAX_PATH, "%x%x%u", UINT(time(NULL) & 0xFFFFFF), 
		UINT(GetSysTickCount() & 0xFFFFFF),  UINT(gSessionCount) );
	return buf;
}

/** 返回指定的 track 当前的传输码率. */
int RtspSession::GetBitrate( int mediaType )
{
	if (mediaType == kMediaTypeAudio) {
		return fAudioTrack.fStat.GetAvgBitrate();

	} else if (mediaType == kMediaTypeVideo) {
		return fVideoTrack.fStat.GetAvgBitrate();

	} else {
		return 0;
	}
}

/** Gets the pointer of RtspConnection for the session. */
IRtspConnectionPtr RtspSession::GetConnection()
{
	return fConnection;
}

double RtspSession::GetDuration()
{
	if (fMediaSource) {
		return fMediaSource->GetDuration();
	}

	return 0.0;
}

/** 返回指定的 track 当前的传输帧率. */
int RtspSession::GetFrameRate( int mediaType )
{
	if (mediaType == kMediaTypeAudio) {
		return fAudioTrack.fStat.GetFrameRate();

	} else if (mediaType == kMediaTypeVideo) {
		return fVideoTrack.fStat.GetFrameRate();

	} else {
		return 0;
	}
}

/** 返回当前会话本地的 IP 地址. */
String RtspSession::GetLocalAddress()
{
	return fLocalAddress;
}

/** 准备下一帧要发送的媒体数据. */
IMediaSamplePtr RtspSession::GetNextMediaSample()
{
	IMediaSamplePtr mediaSample = NULL;
	if (fAudioTrack.IsEmpty() && fVideoTrack.IsEmpty()) {
		if (fConnection) {
			fConnection->OnFillSendBuffer();
		}
	}

	// 从发送队列中取得下一音频帧
	if (fFirstTimestamp > 0) {
		mediaSample = fAudioTrack.GetNextSample();
		if (mediaSample != NULL) {
			return mediaSample;
		}
	}

	// 从发送队列中取得下一视频帧
	mediaSample = fVideoTrack.GetNextSample();
	if (mediaSample == NULL) {
		return NULL;
	}

	// Remember the timestamp of the first video frame that sent.
	if (fFirstTimestamp <= 0) {
		// The first frame must I Frame.
		if (!mediaSample->IsSyncPoint()) {
			mediaSample = NULL;
			return NULL;
		}

		fFirstTimestamp = mediaSample->GetSampleTime();
	}

	return mediaSample;
}

/** 返回通过 STUN 获取的公网地址. */
String RtspSession::GetPublicAddress()
{
	return "";
}

String RtspSession::GetRtpInfo(LPCSTR url)
{
	String track;
	int len = strlen(url);
	if (url[len - 1] != '/') {
		track = "/";
	}

	UINT seq = 0;
	UINT rtptime = 0;

	String rtpInfo;
	if (fVideoTrack.fTransport.fStreamingMode != TRANSPORT_UNKNOWN) {
		String info;
		info.Format("url=%s%strack1;seq=%u;rtptime=%u;ssrc=%u",
			url, track.c_str(), seq, rtptime, GetSSRC());

		rtpInfo += info;
	}

	if (fAudioTrack.fTransport.fStreamingMode != TRANSPORT_UNKNOWN) {
		String info;
		info.Format("url=%s%strack2;seq=%u;rtptime=%u;ssrc=%u",
			url, track.c_str(), seq, rtptime, GetSSRC());

		if (!rtpInfo.IsEmpty()) {
			rtpInfo += ",";
		}
		rtpInfo += info;
	}

	return rtpInfo;
}

/** 
 * Get currently RTP style timestamp.
 * 把当前以毫秒为单位的时间戳转换成指定频率的 RTP 风格时间戳.
 */
UINT RtspSession::GetRtpTime(INT64 sampleTime, int frequency)
{
	return (UINT)((sampleTime - fFirstTimestamp) * (frequency / 1000));
}

/** 返回当前会话的 ID. */
String RtspSession::GetSessionId()
{
	return fSessionId;
}

/** 返回当前的 RTSP 会话的状态. */
RtspState RtspSession::GetSessionState()
{
	return fSessionState;
}

int RtspSession::GetSourceChangeFlags()
{
	return fChangeFlags;
}

/** 返回这个会话的 SSRC 值. */
UINT RtspSession::GetSSRC()
{
	return fSsrc;
}

/** 返回当前会话的视频或音频的传输参数. */
const RtspTransport& RtspSession::GetTransport( int mediaType ) const
{
	if (mediaType == kMediaTypeAudio) {
		return fAudioTrack.fTransport;

	} else if (mediaType == kMediaTypeVideo) {
		return fVideoTrack.fTransport;

	} else {
		return fVideoTrack.fTransport;
	}
}

BOOL RtspSession::IsLiveStreaming()
{
	return fMediaSource && fMediaSource->IsLiveStreaming();
}

/** 指出这个会话是否是组播会话. */
BOOL RtspSession::IsMulticast()
{
	return fVideoTrack.fTransport.fIsMultcast;
}

/** 指出当前会话是否是活跃的. */
BOOL RtspSession::IsPlaying()
{
	if (IsMulticast()) {
		return TRUE;

	} else if (GetSessionState() == kRtspStatePlaying) {
		return TRUE;
	}

	return FALSE;
}

/** 指出这个会话是否是使用 TCP 传输方式. */
BOOL RtspSession::IsTcpTransport()
{
	return (fVideoTrack.fTransport.fStreamingMode == TRANSPORT_TCP);
}

/** 处理音视频帧 */
int RtspSession::OnMediaDistribute( int channel, IMediaSample* mediaSample )
{
	if (mediaSample == NULL) {
		return 0;
	}

	RtspState state = GetSessionState();
	if (state == kRtspStateInit) {
		LOG_D("%u, Is Invalid\r\n", channel);
		return -1;

	} else if (state != kRtspStatePlaying) {
		return 0;
	}

	UINT mediaType = mediaSample->GetMediaType();
	if (mediaType == kMediaTypeVideo) {
		fVideoTrack.DeliverSample(mediaSample);

	} else if (mediaType == kMediaTypeAudio) {
		fAudioTrack.DeliverSample(mediaSample);

	} else {
		return 0;
	}

	// 通知传输层, 缓存区有新的数据可发送
	IRtspConnectionPtr connection = fConnection;
	if (connection != NULL) {
		connection->OnSendBufferNotify(0);
	}

	return 0;
}

int RtspSession::OnMediaSourceChange(UINT flags)
{
	fChangeFlags++;
	return 0;
}

String RtspSession::OnDump(UINT indent)
{
	time_t now = time(NULL);
	String dump;
	dump.Format(
		"<tr>"
		"<td>%s</td> <td>%s</td> <td>%d</td>"
		"<td>%d</td> <td>%d</td>"
		"<td>%d</td>",
		fSessionId.c_str(), 
		fLocalAddress.c_str(), 
		(UINT)fFirstTimestamp, 
		fSsrc, fSessionState, 
		now - fStartTime);

	dump += fVideoTrack.OnDump(1);

	dump += "</tr><tr><td colspan='8'></td>";
	dump += fAudioTrack.OnDump(1);
	dump += "</tr>";
	return dump;
}

IMediaSamplePtr RtspSession::OnRtpNextSample( UINT type, INT64& timestamp )
{
	// 从发送队列中取得下一视频帧
	IMediaSamplePtr mediaSample = fVideoTrack.GetNextSample();
	if (mediaSample == NULL) {
		return NULL;
	}

	// Remember the timestamp of the first video frame that sent.
	if (fFirstTimestamp == 0) {
		// The first frame must I Frame.
		if (!mediaSample->IsSyncPoint()) {
			mediaSample = NULL;
			return NULL;
		}

		fFirstTimestamp = mediaSample->GetSampleTime();
	}

	UINT frequency = mediaSample->GetFrequency();
	timestamp = GetRtpTime(mediaSample->GetSampleTime(), frequency);

	return mediaSample;
}

void RtspSession::OnRtpMediaStream( UINT mediaType )
{

}

void RtspSession::OnSend()
{
	fAudioTrack.OnSend();
	fVideoTrack.OnSend();
}

void RtspSession::OnTimer()
{
	if (IsTcpTransport()) {
		// 填充发送缓存区
		if (fConnection && GetSessionState() == kRtspStatePlaying) {
			if (fAudioTrack.IsEmpty() || fVideoTrack.IsEmpty()) {
				fConnection->OnFillSendBuffer();
			}
		}
		return;
	}

	if (fConnection && GetSessionState() == kRtspStatePlaying) {
		// 填充发送缓存区
		if (fAudioTrack.IsEmpty() || fVideoTrack.IsEmpty()) {
			fConnection->OnFillSendBuffer();
		}
	}	

	int flags = 1;

	fVideoTrack.OnTimer(flags);
	fAudioTrack.OnTimer(flags);
}

void RtspSession::RenewStream()
{
	if (fMediaSource) {
		fMediaSource->RenewStream();
	}
}

/** 设置相关的 RTSP 连接对象. */
void RtspSession::SetConnection( IRtspConnection* connection )
{
	fConnection = connection;
}

void RtspSession::SetMediaSource( IMediaSource* mediaSource )
{
	if (fMediaSource) {
		fMediaSource->RemoveMediaSink(this);
		fMediaSource = NULL;
	}

	fMediaSource = mediaSource;
	if (fMediaSource) {
		LOG_D("(%d) AddMediaSink\r\n", fMediaSource->GetSourceId());
		fMediaSource->AddMediaSink(this);
	}
}

void RtspSession::SetRange( double start, double end )
{
	if (fMediaSource) {
		fMediaSource->SetRange(start, end);
	}
}

void RtspSession::SetScale( float scale )
{
	if (fMediaSource) {
		fMediaSource->SetScale(scale);
	}
}

/** 设置当前 RTSP 会话的状态. */
void RtspSession::SetSessionState(RtspState state )
{
	if (fSessionState == state) {
		return;
	}

	RtspState lastState = fSessionState;
	fSessionState = state;

	if (lastState == kRtspStatePlaying) {
		fFirstTimestamp	= 0;
		fVideoTrack.Reset();
		fAudioTrack.Reset();

	} else if (lastState == kRtspStateReady) {
		fFirstTimestamp	= 0;
		fVideoTrack.Reset();
		fAudioTrack.Reset();

	} else if (lastState == kRtspStateInit) {
		
	}
}

/** 设置当前会话的视频或音频的传输方式. */
void RtspSession::SetTransport( RtspTransport& transport, BOOL isVideoTransport /*= TRUE*/ )
{
	if (isVideoTransport) {
		fVideoTrack.fTransport = transport;

	} else {
		fAudioTrack.fTransport = transport;
	}
}

/** 
 * 启动这个会话. 
 */
int RtspSession::Start(IRtspContext* context)
{
	fRtspContext = context;

	char localAddress[64];
	memset(localAddress, 0, sizeof(localAddress));
	Socket::GetLocalName(localAddress, fVideoTrack.fTransport.fDestination);
	fLocalAddress	= localAddress;
	fStartTime		= time(NULL);

	return 0;
}

/** 
 * RTSP 服务端状态机实现. 
 * 客户端在服务端当前状态下请求不同的方法会影响服务端的状态, 
 * 具体的细节请参考 RTSP RFC 文档相关章节. 
 *
 * @param method 客户端请求的方法.
 */
int RtspSession::StateMachine(LPCSTR method)
{
	if (isempty(method)) {
		return 0;
	}

	switch (fSessionState) {
	case kRtspStateInit:
		if (isequal(method, "SETUP")) {
			SetSessionState(kRtspStateReady);
		}
		break;

	case kRtspStateReady:
		if (isequal(method, "PLAY")) {
			SetSessionState(kRtspStatePlaying);

		} else if (isequal(method, "TEARDOWN")) {
			SetSessionState(kRtspStateInit);

		} else if (isequal(method, "RECORD")) {
			SetSessionState(kRtspStateRecording);
		}
		break;

	case kRtspStatePlaying: 
		if (isequal(method, "PAUSE")) {
			SetSessionState(kRtspStateReady);

		} else if (isequal(method, "TEARDOWN")) {
			SetSessionState(kRtspStateInit);
		}
		break;

	case kRtspStateRecording:
		if (!strcmp(method, "PAUSE")) {
			SetSessionState(kRtspStateReady);

		} else if (!strcmp(method, "TEARDOWN")) {
			SetSessionState(kRtspStateInit);
		}
		break;

	default:
		break;
	}

	return 0;
}

};
