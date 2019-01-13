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

#include "rtc/sdp/sdp.h"
#include "rtsp_connection.h"
#include "rtsp_session_manager.h"

using namespace sdp;

namespace rtsp {

//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
// RtspSessionManager class

RtspSessionManager::RtspSessionManager()
{
}

RtspSessionManager::~RtspSessionManager()
{
	Clear();
}

/** 
 * 添加一个新的 RTSP Session. 
 * @param session 要添加的会话.
 */
BOOL RtspSessionManager::AddSession(RtspSession* session)
{
	if (session == NULL || Contains(session)) {
		return FALSE;
	}

	fMutex.Lock();
	fSessions.AddFirst(session);
	fMutex.UnLock();
	return TRUE;
}

/** Clear all RTSP session. */
void RtspSessionManager::Clear()
{
	fMutex.Lock();
	fSessions.Clear();
	fMutex.UnLock();
}

/** 指出是否包含了指定的 RtspSession. */
BOOL RtspSessionManager::Contains(RtspSession* findSession)
{
	if (findSession == NULL) {
		return FALSE;
	}

	BOOL bRet = FALSE;	
	fMutex.Lock();
	RtspSessionList::ListIterator iter = fSessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();
		if (rtspSession.ptr == findSession) {
			bRet = TRUE;
			break;
		}
	}
	fMutex.UnLock();
	return bRet;
}

/** 返回所有的 RTSP 会话. */
void RtspSessionManager::GetAll( RtspSessionList& sessions )
{
	fMutex.Lock();
	RtspSessionList::ListIterator iter = fSessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();
		sessions.AddLast(rtspSession);
	}
	fMutex.UnLock();
}

/** 返回当前 RTSP 会话的数目. */
int RtspSessionManager::GetCount()
{
	fMutex.Lock();
	int count = fSessions.GetSize();
	fMutex.UnLock();
	return count;
}

/** 返回最后创建的一个会话. */
RtspSessionPtr RtspSessionManager::GetLastSession()
{
	RtspSessionPtr findSession = NULL;
	fMutex.Lock();
	if (!fSessions.IsEmpty()) {
		findSession = fSessions.GetFirst();
	}
	fMutex.UnLock();
	return findSession;
}

/** 
 * Find the RTP Session by Session Id. 
 * @param sessionId RTSP 会话 ID.
 * @return 如果 sessionId 为空, 或者没有这个 ID 的 RTSP 会话存在则返回 NULL.
 */
RtspSessionPtr RtspSessionManager::GetSession(LPCSTR sessionId)
{
	if (isempty(sessionId)) {
		return NULL;
	}

	RtspSessionPtr foundSession = NULL;

	fMutex.Lock();
	RtspSessionList::ListIterator iter = fSessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();
		if (rtspSession->GetSessionId() == sessionId) {
			foundSession = rtspSession;
			break;
		}
	}
	fMutex.UnLock();
	return foundSession;
}

/** 
 * 关闭除指定的 ID 的会话的其他一般性会话. 这样可以释放多人同时访问占用的资源.
 *
 * @param sessionId 要保留的会话的 ID.
 */
void RtspSessionManager::Monopolize( LPCSTR sessionId )
{
	RtspSessionManager::RtspSessionList sessions;
	GetAll(sessions);

	RtspSessionList::ListIterator iter = sessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();
		if (rtspSession == NULL) {
			continue;

		} else if (rtspSession->GetSessionId() == sessionId) {
			continue;

		} else if (rtspSession->IsMulticast()) {
			continue;
		}

		IRtspConnectionPtr connection = rtspSession->GetConnection();
		if (connection) {
			LOG_D("Monopolize Close");
			connection->Close();
		}
	}
}

void RtspSessionManager::OnDump(String& dump)
{
	String text;
	text.Format("<fieldset>\n <legend>RTSP Sessions (%d)</legend>\n\n", 
		fSessions.GetSize());

	dump += text;
	dump += "<table>\n"
		"<tr>\n"
		" <th>Session ID</th> <th>Local Address</th> <th>First Time</th>\n"
		" <th>Channel ID</th> <th>SSRC</th>	      <th>State</th>\n"
		" <th>STUN</th>    <th>Start Time</th>\n"
		" <th>Setuped</th> <th>Buffer Samples</th>  \n"
		" <th>Buffer Bytes</th> <th>Transport</th>\n"
		"</tr>\n";

	RtspSessionManager::RtspSessionList sessions;
	GetAll(sessions);

	RtspSessionList::ListIterator iter = sessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();
		if (rtspSession) {
			dump += rtspSession->OnDump(1);
		}
	}

	dump += "</table>\n</fieldset>\n";
}

/**
 * 处理定时事件.
 */
void RtspSessionManager::OnTimer()
{
	RtspSessionManager::RtspSessionList sessions;
	GetAll(sessions);

	RtspSessionList::ListIterator iter = sessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();
		if (rtspSession) {
			rtspSession->OnTimer();
		}
	}
}

/** 
 * 移除指定的 RTSP Session. 
 * @param removedSession 要移除的 RTSP 会话.
 */
BOOL RtspSessionManager::RemoveSession(RtspSession* removedSession)
{
	if (removedSession == NULL) {
		return FALSE;
	}

	fMutex.Lock();
	RtspSessionList::ListIterator iter = fSessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();
		if (rtspSession.ptr == removedSession) {
			iter.Remove();
			break;
		}
	}

	fMutex.UnLock();
	return TRUE;
}

};
