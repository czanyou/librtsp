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
#ifndef _NS_VISION_INC_RTSP_H
#define _NS_VISION_INC_RTSP_H

#include "core/vision_profile.h"
#include "media/vision_media_source.h"
#include "core/net/nio_message.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////

const LPCSTR kRtspVersion		= "RTSP/1.0";
const UINT   kRtspDefaultPort	= 554;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspErrorCode enum

/** RTSP 错误返回码定义. */
enum RtspErrorCode 
{
	RTSP_S_OK					= 0,			///< 执行成功
	RTSP_E_INVALID_STATE		= 0xC0010010,	///< 无效的状态
	RTSP_E_INVALID_SINK			= 0xC0010013,	///< Sink 错误
	RTSP_E_INVALID_URL			= 0xC0010014,	///< 无效的 URL
	RTSP_E_INVALID_PARAMETER	= 0xC0010015,	///< 参数为空
	RTSP_E_INVALID_CHANNEL		= 0xC0010017,	///< 无效的通道号
	RTSP_E_SOCKET				= 0xC0010018,	///< Socket 错误
	RTSP_E_BIND_FAILED			= 0xC0010019,	///< Socket 绑定错误
	RTSP_E_TOO_MANY_SESSIONS	= 0xC0010020	///< 太多的会话
};


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspState enum

/** RTSP 会话状态. */
enum RtspState
{
	kRtspStateUnknown			= 0,	///< 未知状态
	kRtspStateConnecting		= 10,	///< 正在连接中
	kRtspStateInit				= 20,	///< 初始状态
	kRtspStateReady				= 30,	///< 就绪状态
	kRtspStatePlaying			= 40,	///< 播放状态
	kRtspStateRecording			= 50	///< 录像状态, 仅对 VCR 有效.
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IRtspManagerListener interface

/**
 * RTSP 服务器模块事件侦听器接口.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IRtspManagerListener
{
public:
	/** 
	 * 当请求指定的路径的数据源时, 调用这个方法. 
	 * @param path 请求的数据源路径
	 * @param id 请求的数据源 ID
	 * @return 返回相关的数据源对象.
	 */
	virtual IMediaSourcePtr OnRtspGetSource(LPCSTR path, UINT id = 0) = 0;

	/** 
	 * 当收到指定的请求消息时, 调用这个方法.
	 * 对于 RTSP 模块内部未处理的消息会通过调用这个方法来处理, 应用程序可以通过
	 * 实现这个方法来扩展 RTSP 服务器的功能.
	 * @param request 要处理的请求消息
	 * @param response 应答消息
	 */
	virtual int OnRtspRequest(BaseMessage *request, BaseMessage *response) = 0;
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IRtspManager interface

/** 
 * 代表 RTSP/RTP 服务端模块. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IRtspManager : public Object
{
// Attributes -------------------------------------------------
public:
	/**  
	 * 返回这个 RTSP 服务器的当前状态. 
	 *
	 * @return 服务器的状态, 参考 IVisionModule::GetState
	 */
	virtual UINT GetState() = 0;

	/** 返回这个 RTSP 服务器的当前状态. */
	virtual String GetStatusText(LPCSTR type = NULL) = 0;

	/** 
	 * 注册事件侦听器. 
	 * @param listener 事件回调接口
	 */
	virtual void SetListener(IRtspManagerListener *listener) = 0;
	
public:
	/** 关闭这个服务. */
	virtual int Close() = 0;

	virtual void OnDump(String& dump) = 0;

	/** 
	 * 启动 RTSP 服务. 
	 * @return 如果成功则返回 0, 否则返回一个小于 0 的错误码.
	 */
	virtual int Start() = 0;

	/** 停止 RTSP 服务. */
	virtual int Stop() = 0;

	/** 通知更新设置, 通常在检测到 Profile 文件修改后调用这个方法. */
	virtual int UpdateSettings(IPreferences* profile) = 0;
};

typedef SmartPtr<IRtspManager> IRtspManagerPtr;

/** 创建一个新的 RTSP 服务器的实例. */
IRtspManagerPtr CreateRtspServer();

}

#endif // _NS_VISION_INC_RTSP_H

