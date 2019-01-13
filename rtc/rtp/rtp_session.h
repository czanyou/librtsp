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
#ifndef _NS_VISION_RTP_SESSION_H_
#define _NS_VISION_RTP_SESSION_H_

#include "core/net/nio_socket.h"
#include "media/vision_media.h"
#include "media/vision_media_frame.h"
#include "rtp_common.h"
#include "rtp_decoder.h"
#include "rtp_rtcp.h"

using namespace rtp;

namespace rtp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtpSessionListener class

/**
 * RTP 会话事件回调接口
 * 
 * @author ChengZhen (anyou@msn.com)
 */
class RtpSessionListener
{
public:
	/**
	 * 处理接收到的 STUN 消息.
	 * @param mediaType
	 * @param flags
	 */
	virtual void OnRtpIceBinding(int flags) {}


	virtual void OnRtpIceSelected(int flags) {}

	/**
	 * 处理接收到的媒体流数据.
	 * @param mediaType
	 * @param mediaBuffer
	 */
	virtual void OnRtpMediaStream(UINT mediaType, IMediaBuffer* mediaBuffer) {}
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtpSession class

/** 
 * 代表一个 RTP 会话. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtpSession
{
public:
	RtpSession(void);
	virtual ~RtpSession(void);

// Attributes -------------------------------------------------
public:
	INT64  GetLastActiveTime();
	String GetPublicAddress();
	UINT   GetRtpTimestamp(INT64 ts, int frequency);

	BOOL  IsIceSelected();
	BOOL  IsIceControlling();
	BOOL  IsStunBinding();
	BOOL  IsTunnel();

	void  SetCodecType(UINT mediaType, int codecType);
	void  SetIceControlling(BOOL isControling);
	void  SetIceEnabled(BOOL enabled);
	void  SetListener(RtpSessionListener* listener);
	void  SetRtcContext(RtcContext* rtcContext);
	void  SetTunnel(BOOL isTunnel);

// Operations -------------------------------------------------
public:
	IMediaSamplePtr OnRtpNextSample( UINT type, INT64& timestamp );
	IMediaBufferPtr OnRtpDecodeStream( UINT mediaType, BYTE* packetData, UINT rtpLength );

public:
	int  Close();
	int  DeliverMediaSample(IMediaSample* mediaSample);
	void HandleTunnelPacket(SocketPacket* packet);

	void OnDump(String& dump, int flags);
	void OnSendMediaStream();
	void OnTransmitterTimer();

	int  Start(LPCSTR localAddress, UINT videoPort, UINT audioPort);

public:
	static UINT GetCodecType(LPCSTR codecName);

protected:
	int  BuildSourceReportPacket();
	void OnRtpIceSelected(UINT mediaType, LPCSTR remoteAddress, UINT remotePort );
	void OnRtpMediaStream(UINT mediaType);
	void OnRtpIceBinding(UINT mediaType, int flags);
	int  StartTransmitter( UINT type, UINT port );


// Data Members -----------------------------------------------
protected:
	MediaSampleQueuePtr	fAudioSampleQueue;	///< 音频帧缓冲区
	RtpSessionListener* fSessionListener;	///< 事件回调接口
	MediaSampleQueuePtr	fVideoSampleQueue;	///< 视频帧缓冲区
	RtcContextPtr		fRtcContext;		///< 相关的 RTC 上下文

	RtpDecoder fAudioRtpDecoder;	///< RTP 音频流解析器
	RtpDecoder fVideoRtpDecoder;	///< RTP 音频流解析器

	UINT   fAudioCodecType;		///< 这个会话的音频编码类型
	INT64  fFirstTimestamp;		///< The timestamp of the first frame.
	BOOL   fIsIceControlling;	///< 
	BOOL   fIsIceEnabled;		///< 是否启用 ICE
	BOOL   fIsIceSelected;		///< 指出 ICE 连通性检查和选择已经完成
	BOOL   fIsTunnel;			///< 指出这个会话是否是一个 RTP 隧道
	BOOL   fStunBinding;		///< 
	String fLocalAddress;		///< 这个会话的本地地址
	UINT   fRtpSsrc;			///< 这个会话的相关的 SSRC
	UINT   fSessionState;		///< 这个会话的状态
	UINT   fVideoCodecType;		///< 这个会话的视频编码类型
};

}

#endif // _NS_VISION_RTP_SESSION_H_
