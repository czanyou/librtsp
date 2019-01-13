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
#include "rtsp_context.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSessionTrack class

RtspSessionTrack::RtspSessionTrack()
{
	fSetuped = FALSE;
}

void RtspSessionTrack::Clear()
{
	if (fFrameBuffer) {
		fFrameBuffer->Clear();
	}

	fStat.Clear();
	fTransport.Clear();
	fSetuped		= FALSE;
}

void RtspSessionTrack::Close()
{

}

BOOL RtspSessionTrack::DeliverSample( IMediaSample* mediaSample )
{
	if (fTransport.fStreamingMode == TRANSPORT_UNKNOWN) {
		return FALSE;

	} else if (fFrameBuffer == NULL) {
		return FALSE;

	} else {
		return fFrameBuffer->DeliverSample(mediaSample);
	}
}

IMediaSamplePtr RtspSessionTrack::GetNextSample()
{
	if (fFrameBuffer == NULL) {
		return NULL;
	}

	IMediaSamplePtr mediaSample = fFrameBuffer->RemoveTail();
	if (mediaSample) {
		fStat.OnMediaSample(mediaSample->GetSampleSize());
	}
	return mediaSample;
}

/** 指出发送缓存区是否为空. */
BOOL RtspSessionTrack::IsEmpty()
{
	return (fFrameBuffer == NULL) || fFrameBuffer->IsEmpty();
}

/** 指出是否为 UDP 传输模式. */
BOOL RtspSessionTrack::IsUdpTransport()
{
	return (fTransport.fIsMultcast || fTransport.fStreamingMode == TRANSPORT_UDP);
}

String RtspSessionTrack::OnDump(UINT indent)
{
	String dump;
	dump.Format("<td>%d</td> <td>%d</td> <td>%d</td> <td>%s</td>",
		fSetuped, 
		fFrameBuffer->GetTotalBufferCount(), 
		fFrameBuffer->GetTotalBufferSize(),
		fTransport.ToString().c_str());

	return dump;
}

void RtspSessionTrack::OnSend()
{

}

void RtspSessionTrack::OnTimer( int& flags )
{

}

void RtspSessionTrack::Reset()
{
	if (fFrameBuffer) {
		fFrameBuffer->Clear();
	}

	fStat.Clear();
}

/** 初始化指定的 RTP 传输通道. */
int RtspSessionTrack::StartTransmit(IRtspContext* context)
{
	return 0;
}

};
