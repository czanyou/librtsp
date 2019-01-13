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
#ifndef _NS_VISION_RTSP_CLIENT_API_H
#define _NS_VISION_RTSP_CLIENT_API_H

#include "core/vision_profile.h"
#include "core/net/nio_message.h"
#include "core/net/nio_socket.h"
#include "media/vision_media.h"
#include "vision_rtsp.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IRtspClient interface

/** 
 * 代表一个 RTSP 客户端. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IRtspClient : public Object
{
public:

	/** RTP 传输类型. */
	enum TransportType 
	{
		TRANS_MODE_UDP  = 0,	///< RTP over UDP
		TRANS_MODE_TCP  = 1,	///< RTP over RTSP(TCP)
		TRANS_MODE_HTTP	= 2		///< RTP/RTSP over HTTP(TCP)
	};

public:
	/** 
	 * RTSP 客户端事件回调接口.
	 */
	class IRtspClientListener
	{
	public:
		/**
		 * 当接收到新的 RTP 包时会调用这个方法.
		 * @param format 格式, 0: 视频, 1: 音频
		 * @param data 数据内容
		 * @param length 长度
		 * @param flags 标志
		 * @param timestamp 时间戳
		 */
		virtual void OnMediaStream(IMediaBuffer* mediaBuffer) = 0;

		/**
		 * 当状态发生改变
		 * @param oldState 改变前的状态
		 * @param newState 改变后的状态
		 */
		virtual void OnClientStateChange(UINT oldState, UINT newState) = 0;
	};

public:
	/** 返回当前状态. */
	virtual RtspState GetRtspState() = 0;

	/** 返回指定的 Track 的属性. */
	virtual MediaFormat& GetTrackFormat(UINT format) = 0;

	/** 返回当前选择的传输方式. */
	virtual int  GetTransport() = 0;

	/** 设置要打开的 RTSP URL. */
	virtual BOOL SetDataSource(LPCSTR url) = 0;

	/** 注册事件侦听器. */
	virtual void SetListener(IRtspClientListener *listener) = 0;

	virtual void SetSelector(SocketSelector* selector) = 0;

	/** 设置传输方式. */
	virtual void SetTransport(int type) = 0;

public:
	/** 关闭这个 RTSP 客户端. */
	virtual void Close() = 0;

	/** 打开这个 RTSP 客户端. */
	virtual BOOL Open() = 0;

	virtual void OnTimer() = 0;
};

typedef SmartPtr<IRtspClient> IRtspClientPtr; ///< IRtspClientPtr 智能指针类型

/** 创建一个实现了IRtspClient 接口的实例. */
IRtspClientPtr CreateRtspClient();

}

#endif // _NS_VISION_RTSP_CLIENT_API_H

