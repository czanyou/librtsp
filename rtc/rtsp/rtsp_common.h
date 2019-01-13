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
#ifndef _NS_VISION_RTSP_COMMON_H
#define _NS_VISION_RTSP_COMMON_H

#include "core/net/nio_message.h"
#include "core/net/nio_socket.h"

#include "media/vision_media.h"
#include "rtc/vision_rtsp.h"

namespace rtsp {


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSettings interface

struct RtspSettings
{
public:
	RtspSettings() {
		fAuthEnabled	= FALSE;
		fListenPort		= kRtspDefaultPort;
	}

public:
	BOOL   fAuthEnabled;	///< 
	String fDeviceInfo;		///< 
	String fRecordPath;		///< 
	UINT   fListenPort;		///< 
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IRtspContext interface

/**
 * RTSP 上下文
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IRtspContext : public Object
{
public:
	virtual SocketSelectorPtr GetSocketSelector() = 0;

	virtual String GetUserAgent() = 0;

	virtual String GetBasePath() = 0;
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IRtspConnection interface

/** 
 * 代表一个 RTSP 连接. 
 *
 * RTSP 连接指的是 RTSP 客户端和服务端之间的一对一的连接. 
 * RTSP 一般使用 TCP 通信, 所以 RTSP 连接通常通过一条 TCP 连接进行通信.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IRtspConnection : public Object
{
public:
	/** 关闭这个连接. */
	virtual void	Close() = 0;

	/** 返回这个连接的本地地址. */
	virtual String	GetLocalAddress() = 0;

	/** 返回这个连接的远端地址. */
	virtual String	GetPeerAddress() = 0;

	/** 返回这个连接的远端端口. */
	virtual UINT	GetPeerPort() = 0;

	/** 通知有新的数据可发送. */
	virtual void	OnSendBufferNotify(int flags) = 0;

	/** 通知数据源读取更多的数据. */
	virtual void	OnFillSendBuffer() = 0;
};

typedef SmartPtr<IRtspConnection> IRtspConnectionPtr; ///< IRtspConnection 智能指针类型


}; // namespace rtsp


#endif // #ifndef _NS_VISION_RTSP_COMMON_H
