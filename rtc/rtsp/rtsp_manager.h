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
#ifndef _NS_VISION_RTSP_MANAGER_H
#define _NS_VISION_RTSP_MANAGER_H

#include "rtsp_context.h"
#include "rtsp_server.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspManager class

/** 
 * RtspManager 代表一个 RTSP 服务端.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspManager : public IRtspManager, public Thread
{
// Construction/Destruction -----------------------------------
public:
	RtspManager();
	virtual ~RtspManager();

// Attributes -------------------------------------------------
public:
	String GetStatusText(LPCSTR type);
	UINT   GetState();
	void   SetListener(IRtspManagerListener *listener);

// Operations -------------------------------------------------
public:
	int  Close();
	void OnDump(String& dump);
	int  Run();
	int  Start();
	int  Stop();
	int  UpdateSettings(IPreferences* profile);

// Implementation ---------------------------------------------
private:
	void DispatchEvent(EventObject* event);
	void PostEvent(EventObject* event);

// Data Members -----------------------------------------------
private:
	RtspServer fRtspServer;		///< 
	INT64   fLastActiveTime;	///< 记录最后正常运行的时间, 用来检测这个线程是否死锁
	Mutex	fMutex;				///< Mutex.
	int		fState;				///< 当前服务运行状态
};

}; // namespace rtsp


#endif // !defined(_NS_VISION_RTSP_MANAGER_H)
