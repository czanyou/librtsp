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

#ifndef _NS_VISION_RTSP_CONTEXT_H
#define _NS_VISION_RTSP_CONTEXT_H

#include "rtsp_message.h"
#include "rtsp_common.h"
#include "rtsp_auth.h"
#include "rtsp_session_manager.h"
#include "rtsp_connection_manager.h"

namespace rtsp {

class RtspServer;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspContext class

/**
 * RTSP 上下文
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspContext : public IRtspContext
{
public:
	RtspContext();
	virtual ~RtspContext();

// Attributes -------------------------------------------------
public:
	IMediaSourcePtr GetMediaSource(LPCSTR path, UINT sourceId = 0);
	SocketSelectorPtr GetSocketSelector();

	String GetBasePath();
	String GetUserAgent();

// Operations -------------------------------------------------
public:
	int  HandleRequest(BaseMessage *request, BaseMessage *response) ;
	BOOL CheckAuthorization(RtspConnection *connection, BaseMessage* request);

	void OnAddRtspSession(RtspSession* session);
	void OnCloseRtspSession(RtspSession* session);

// Data Members -----------------------------------------------
public:
	RtspServer* fRtspServer;
};

typedef SmartPtr<RtspContext> RtspContextPtr;

}; // namespace rtsp


#endif // !defined(_NS_VISION_RTSP_CONTEXT_H)
