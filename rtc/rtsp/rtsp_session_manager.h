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
#ifndef _NS_VISION_RTSP_SESSION_MANAGER_H
#define _NS_VISION_RTSP_SESSION_MANAGER_H

#include "rtsp_session.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// class RtspSessionManager 

/** 
 * 负责管理所有的 RTSP 会话. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspSessionManager
{
// Construction/Destruction -----------------------------------
public:
	RtspSessionManager();
	virtual ~RtspSessionManager();
	typedef LinkedList<RtspSessionPtr, RtspSessionPtr> RtspSessionList; ///< RtspSession 列表类型

// Attributes -------------------------------------------------
public:
	RtspSessionPtr GetSession(LPCSTR sessionId);
	RtspSessionPtr GetLastSession();
	void   GetAll(RtspSessionList& sessions);
	int    GetCount();

// Operations -------------------------------------------------
public:
	BOOL AddSession(RtspSession* session);
	void Clear();
	BOOL Contains(RtspSession* session);
	void OnDump(String& dump);
	void OnTimer();
	void Monopolize( LPCSTR sessionId );
	BOOL RemoveSession(RtspSession* session);

// Data members -----------------------------------------------
private:
	RtspSessionList	fSessions;	///< RTSP 会话列表
	Mutex fMutex;	///< Mutex object
};

}; // namespace rtsp

#endif // !defined(_NS_VISION_RTSP_SESSION_MANAGER_H)
