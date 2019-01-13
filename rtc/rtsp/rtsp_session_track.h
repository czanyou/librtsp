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
#ifndef _NS_VISION_RTSP_SESSION_TRACK_H
#define _NS_VISION_RTSP_SESSION_TRACK_H

#include "media/vision_media_frame.h"
#include "rtsp_message.h"
#include "rtsp_common.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSessionTrack class

/**
 * 代表 RTSP 会话的一个 Track, 对应媒体流的相关 Track. 
 * 主要负责这个 Track 的帧的发送.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspSessionTrack 
{
public:
	RtspSessionTrack();

// Attributes -------------------------------------------------
public:
	IMediaSamplePtr GetNextSample();
	BOOL IsUdpTransport();
	BOOL IsEmpty();

// Operations -------------------------------------------------
public:
	void Clear();
	void Close();
	BOOL DeliverSample(IMediaSample* mediaSample);
	void OnSend();
	void OnTimer(int& flags);
	void Reset();
	int  StartTransmit(IRtspContext* context);
	String OnDump(UINT indent);

// Data Members -----------------------------------------------
public:
	MediaSampleQueuePtr	fFrameBuffer;	///< 视频帧缓冲区
	BOOL				fSetuped;		///< 
	MediaTrackStat		fStat;			///< Video and audio stream Statistics
	RtspTransport		fTransport;		///< Video Transport, 这个参数描述了如何传输视频数据
};

}; // namespace rtsp

#endif // !defined(_NS_VISION_RTSP_SESSION_TRACK_H)
