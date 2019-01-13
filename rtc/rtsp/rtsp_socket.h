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
#ifndef _NS_VISION_RTSP_SOCKET
#define _NS_VISION_RTSP_SOCKET

#include "core/net/nio_socket.h"
#include "core/net/nio_selector.h"
#include "core/net/nio_url.h"

#include "rtsp_common.h"
#include "rtsp_message.h"
#include "rtsp_reader.h"
#include "rtsp_writer.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSocketListener class

/**
 * RTSP Socket 事件侦听接口
 */
class RtspSocketListener
{
public:
	virtual void OnRtspConnect(int errorCode) = 0;
	virtual void OnRtspMessage(RtspMessage* message) = 0;
	virtual void OnRtspPacket (char* packet, size_t buflen) = 0;
	virtual void OnRtspSend   (int errorCode) = 0;
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSocket class

/**
 * RTSP Socket
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspSocket : public Socket
{
public:
	RtspSocket(void);
	virtual ~RtspSocket(void);

// Attributes -------------------------------------------------
public:
	time_t GetLastActiveTime();
	void   SetLastActiveTime();
	void   SetListener(RtspSocketListener* listener);

// Operations -------------------------------------------------
public:
	void   Close();
	void   HandleMessage(RtspMessage* message);
	void   HandleRtpPacket(char* packet, size_t buflen);

	void   OnClose  (int errorCode);
	void   OnConnect(int errorCode);
	void   OnReceive(int errorCode);
	void   OnSend   (int errorCode);
	int    OnSendBufferData();

	void   Reset();
	void   ResetWriter();
	int    SendMessage(LPCSTR message, size_t length);
	int	   SendMessageContent( IMediaSample* mediaSample );

protected:
	void   DispatchMessage();
	int    OnReadMessages();

// Data Members -----------------------------------------------
public:
	RtspSocketListener* fListener;	///< 事件侦听器
	RtspReader fRtspReader;		///< RTSP message Reader
	RtspWriter fRtspWriter;		///< 这个连接相关的 RTSP Writer
	time_t  fLastActiveTime;	///< 最后活跃时间, 用来检测这个连接是否超时.
	int		fReceiveTimes;		///< 用于监视接收是否进入了死循环
};

typedef SmartPtr<RtspSocket> RtspSocketPtr;

}

#endif // _NS_VISION_RTSP_SOCKET
