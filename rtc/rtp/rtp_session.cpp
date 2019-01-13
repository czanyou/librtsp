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
#include "rtp_session.h"

namespace rtp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtpSession class

RtpSession::RtpSession(void)
{
	fAudioCodecType		= 0;
	fIsIceEnabled		= FALSE;
	fIsIceSelected		= FALSE;
	fIsTunnel			= FALSE;
	fRtcContext			= NULL;
	fRtpSsrc			= 0;
	fSessionListener	= NULL;
	fSessionState		= 0;
	fStunBinding		= FALSE;
	fVideoCodecType		= 0;
	fIsIceControlling	= FALSE;

	fAudioSampleQueue	= new MediaSampleQueue(kMediaTypeAudio);
	fVideoSampleQueue	= new MediaSampleQueue(kMediaTypeVideo);
}

RtpSession::~RtpSession(void)
{
}

/** 构建发送端播放消息包. */
int RtpSession::BuildSourceReportPacket()
{
	RtcpPacket* packet = new RtcpPacket();
	packet->AddPacket(RTCP_TYPE_SR, 0);

	RtcpSenderInfo* senderInfo = packet->GetSenderInfo();
	if (senderInfo == NULL) {  
		return -1; // 必须包含发送者信息
	}

	UINT rtpTimestamp = 0;

	senderInfo->fSsrc				= htonl(fRtpSsrc);
	senderInfo->fNtpMsw				= 0;
	senderInfo->fNtpLsw				= 0;
	senderInfo->fRtpTimstamp		= htonl(rtpTimestamp);

	// 填充报告信息
	RtcpReportBlock* report = packet->GetReportBlock();
	if (report) {
		report->fSsrc				= 0;
		report->fFractionLost		= 0;
		report->fPacketsLost[0]		= 0;
		report->fPacketsLost[1]		= 0;
		report->fPacketsLost[2]		= 0;
		report->fHighestSequence	= 0;
		report->fJitter				= 0;
		report->fLastSR				= 0;
		report->fDelayLastSR		= 0;
	}

	packet->AddPacket(RTCP_TYPE_SDES, 0);
	RtcpCommonHeader* header = packet->GetCommonHeader();
	BYTE* offset = (BYTE*)header;
	UINT* ssrc = (UINT*)(offset + 4);
	*ssrc = 0;

	RtcpSdesHeader* sdes = reinterpret_cast<RtcpSdesHeader*>(offset + 8);
	sdes->fSdesId = RTCP_SDES_CNAME;
	sdes->fLength = 10;

	BYTE* end = offset + 8 + 12;
	*end = 0;

	return 0;
}

int RtpSession::Close()
{
	if (fAudioSampleQueue) {
		fAudioSampleQueue->Clear();
	}

	if (fVideoSampleQueue) {
		fVideoSampleQueue->Clear();
	}

	fSessionListener = NULL;
	return 0;
}

/** 分发要发送的 Sample. */
int RtpSession::DeliverMediaSample( IMediaSample* mediaSample )
{
	if (mediaSample == NULL) {
		return -1;
	}

	// Remember the timestamp of the first video mediaSample that sent.
	if (fFirstTimestamp == 0) {
		// The first frame must I Frame.
		if (!mediaSample->IsSyncPoint()) {
			mediaSample = NULL;
			return 0;
		}

		fFirstTimestamp = mediaSample->GetSampleTime();
	}

	//LOG_D("%d, %d\r\n", channel, frame->GetSequence());

	UINT mediaType = mediaSample->GetMediaType();
	if (mediaType == kMediaTypeVideo) {
		fVideoSampleQueue->DeliverSample(mediaSample);

	} else if (mediaType == kMediaTypeAudio) {
		fAudioSampleQueue->DeliverSample(mediaSample);
	}

	return 0;
}

UINT RtpSession::GetCodecType( LPCSTR codecName )
{
	String name = codecName ? codecName : "";
	if (name == "H264") {
		return kVideoFormatH264;

	} else if (name == "MJPEG") {
		return kVideoFormatMJPG;
	}

	return kMediaFrameUnknown;
}

INT64 RtpSession::GetLastActiveTime()
{
	return 0;
}

String RtpSession::GetPublicAddress()
{
	return "";
}

/** 
 * Get currently RTP style timestamp.
 * 把当前以毫秒为单位的时间戳转换成指定频率的 RTP 风格时间戳.
 */
UINT RtpSession::GetRtpTimestamp(INT64 ts, int frequency)
{
	return (UINT)((ts - fFirstTimestamp) * (frequency / 1000));
}


/** 处理收到的 RTP 隧道数据包. */
void RtpSession::HandleTunnelPacket( SocketPacket* packet )
{

}

BOOL RtpSession::IsIceSelected()
{
	return fIsIceSelected;
}

BOOL RtpSession::IsIceControlling()
{
	return fIsIceControlling;
}

BOOL RtpSession::IsStunBinding()
{
	return fStunBinding;
}

/** 指出这是否是一个 RTP 隧道. */
BOOL RtpSession::IsTunnel()
{
	return fIsTunnel;
}

void RtpSession::OnDump( String& dump, int flags )
{

}

/** 处理 STUN 应答消息. */
void RtpSession::OnRtpIceBinding(UINT mediaType, int flags)
{

}

/** 
 * 当选好了 ICE 通信地址. 
 * @param mediaType 轨道类型
 */
void RtpSession::OnRtpIceSelected( UINT mediaType, LPCSTR remoteAddress, UINT remotePort )
{

}

/** 返回下一帧要发送的帧. */
IMediaSamplePtr RtpSession::OnRtpNextSample( UINT mediaType, INT64& timestamp )
{
	// 从发送队列中取得下一视频帧
	IMediaSamplePtr meidaSample = NULL;
	
	if (mediaType == kMediaTypeAudio) {
		meidaSample = fAudioSampleQueue->RemoveTail();

	} else if (mediaType == kMediaTypeVideo) {
		meidaSample = fVideoSampleQueue->RemoveTail();

	} else {
		return NULL;
	}

	if (meidaSample == NULL) {
		return NULL;
	}

	UINT frequency = meidaSample->GetFrequency();
	timestamp = GetRtpTimestamp(meidaSample->GetSampleTime(), frequency);

	return meidaSample;
}

/**
 * 解析指定的 RTP 包, 并返回包含原始媒体流的数据包.
 */
IMediaBufferPtr RtpSession::OnRtpDecodeStream( UINT mediaType, BYTE* packetData, UINT packetLen )
{
	if (packetData == NULL) {
		return 0;
	}

	RtpHeader* rtpHeader = reinterpret_cast<RtpHeader*>(packetData);
	IMediaBufferPtr mediaBuffer;

	if (mediaType == kMediaTypeVideo) {
		if (fVideoCodecType == kVideoFormatH264) {
			mediaBuffer = fVideoRtpDecoder.DecodeH264Packet(rtpHeader, packetLen);

		} else {
			mediaBuffer = fVideoRtpDecoder.DecodeVideoPacket(rtpHeader, packetLen);
		}

	} else if (mediaType == kMediaTypeAudio) {
		mediaBuffer = fAudioRtpDecoder.DecodeAudioPacket(rtpHeader, packetLen);

	}

	return mediaBuffer;
}

/** 处理 RTP 流. */
void RtpSession::OnRtpMediaStream( UINT mediaType)
{

}

void RtpSession::OnSendMediaStream()
{

}

/** 处理定时事件. */
void RtpSession::OnTransmitterTimer()
{

}

/** 设置指定的媒体流的编码类型. */
void RtpSession::SetCodecType( UINT mediaType, int codecType )
{
	if (mediaType == kMediaTypeVideo) {
		fVideoCodecType = codecType;

	} else if (mediaType == kMediaTypeAudio) {
		fAudioCodecType = codecType;
	}
}

void RtpSession::SetIceControlling( BOOL isControlling )
{
	fIsIceControlling = isControlling;
}

/** 设置是否启用 ICE */
void RtpSession::SetIceEnabled( BOOL enabled )
{
	fIsIceEnabled = enabled;
}

/** 设备事件侦听器. */
void RtpSession::SetListener( RtpSessionListener* listener )
{
	fSessionListener = listener;
}

void RtpSession::SetRtcContext( RtcContext* rtcContext )
{
	fRtcContext = rtcContext;
}

/** 设置是否启用 RTP 隧道模式, 隧道模式下多个媒体流复用同一个端口. */
void RtpSession::SetTunnel( BOOL isTunnel )
{
	fIsTunnel = isTunnel;
}

/** 
 * 启动这个 RTP 会话. 
 * @param localAddress
 * @param videoPort
 * @param audioPort
 */
int RtpSession::Start(LPCSTR localAddress, UINT videoPort, UINT audioPort)
{
	if (isempty(localAddress)) {
		LOG_D("Inavlid localAddress");
		return -1;
	}

	fLocalAddress = localAddress;

	if (fIsIceEnabled) {
		fStunBinding = TRUE;
	}

	int ret = StartTransmitter(kMediaTypeVideo, videoPort);
	if (audioPort > 0) {
		StartTransmitter(kMediaTypeAudio, audioPort);
		LOG_D("video=%d, audio=%d\r\n", videoPort, audioPort);
	}

	return ret;
}

/** 
 * 启动指定媒体轨道的 RTP 传输对象. 
 * @param mediaType 媒体轨道类型
 * @param mediaPort 本地通信端口
 */
int RtpSession::StartTransmitter( UINT mediaType, UINT mediaPort )
{

	return 0;
}

};
