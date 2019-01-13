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
#ifndef _NS_VISION_RTSP_WRITER_H
#define _NS_VISION_RTSP_WRITER_H

#include "rtsp_common.h"
#include "rtsp_session.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspWriter class

/**
 * 代表一个 RTSP 消息书写器.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspWriter
{
public:
	RtspWriter();
	virtual ~RtspWriter();
	typedef LinkedList<IMediaBufferPtr> MediaBufferList;  ///< IMediaBuffer 智能指针列表类型

public:
	void SetRtspSession(RtspSession* session);

// Operations -------------------------------------------------
public:
	void ClearAll();
	void ClearBufferList();
	int  OnSend(Socket& socket, BOOL& isEmptyBuffer);
	void OnDump(String& dump);
	int  WriteBuffer(const BYTE* data, UINT dataLength);
	int  WriteSample(IMediaSample* mediaSample);
	int  WriteMessage(LPCSTR message, size_t length);

// Implementation ---------------------------------------------
private:
	BOOL OnFillNextSample();
	BOOL OnFillNextRtpPacket(IMediaBuffer* mediaBuffer, UINT channel, UINT rtpTime, UINT rtpSsrc);
	BOOL OnFillNextRawPacket(IMediaBuffer* mediaBuffer);
	int  SendBufferData( Socket &socket );
	int  CheckBufferSize(UINT freeSize);

// Data Members -----------------------------------------------
private:
	RtspSessionPtr  fRtspSession;	///< 指向相关的 RTSP 会话对象
	ByteBufferPtr	fSendBuffer;	///< 发送缓存区

	Mutex	fMutex;					///< The mutex object
	UINT    fNalHeader;				///< 
	WORD    fSequence;				///< 
};

}

#endif // _NS_VISION_RTSP_WRITER_H

