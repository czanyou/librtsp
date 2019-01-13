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

#ifndef _NS_VISION_RTSP_SERVER_H
#define _NS_VISION_RTSP_SERVER_H

#include "rtsp_context.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspServer class

/** 
 * RtspServer 代表一个 RTSP 服务端.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspServer
{
// Construction/Destruction -----------------------------------
public:
	RtspServer();
	virtual ~RtspServer();

	typedef LinkedList<RtspConnectionPtr> RtspConnectionList; ///< RtspConnection 类型

// Attributes -------------------------------------------------
public:
	RtspConnectionManager& GetConnectionManager();
	RtspSessionManager& GetSessionManager();
	IRtspManagerListener* GetListener();
	RtspContextPtr  GetRtspContext();
	IMediaSourcePtr GetMediaSource(LPCSTR path, UINT id);
	RtspSettings&  GetSettings();

	String GetBasePath();
	String GetDeviceInformation();
	String GetStatusText(LPCSTR type);
	int	   GetSessionCount();
	UINT   GetState();
	void   OnDump(String& dump);

	void   SetListener(IRtspManagerListener *listener);

// Operations -------------------------------------------------
public:
	int    Close();
	BOOL   CheckAuthorization(RtspConnection *connection, BaseMessage* request);
	String GetBasicStatus(LPCSTR remoteAddress = NULL);
	int    OnTimer();
	int    Open();
	int    UpdateSettings(IPreferences* profile);

// Implementation ---------------------------------------------
private:
	String GetConnectionStatus();
	String GetSessionStatus();

// Data Members -----------------------------------------------
private:
	RtspConnectionManager fConnectionManager;	///< RTSP 连接管理器.
	IRtspManagerListener* fRtspListener;		///< RTSP event listener
	RtspSessionManager    fSessionManager;		///< RTSP 会话管理器.
	RtspAuthorization	  fRtspAuth;			///< 身份认证模块

	RtspContextPtr  fRtspContext;				///< RTSP 上下文
	RtspSettings	fRtspSettings;				///< 参数设置

	Mutex	fMutex;								///< Mutex.
};

}; // namespace rtsp


#endif // !defined(_NS_VISION_RTSP_SERVER_H)
