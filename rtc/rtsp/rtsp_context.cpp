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

#include "rtsp_context.h"
#include "rtsp_server.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspContext class

RtspContext::RtspContext()
{
	fRtspServer = NULL;
}

RtspContext::~RtspContext()
{

}

void RtspContext::OnAddRtspSession( RtspSession* session )
{
	if (fRtspServer == NULL) {
		return;
	}

	fRtspServer->GetSessionManager().AddSession(session);
}

BOOL RtspContext::CheckAuthorization( RtspConnection *connection, BaseMessage* request )
{
	if (fRtspServer == NULL) {
		return FALSE;
	}

	return fRtspServer->CheckAuthorization(connection, request);
}

String RtspContext::GetBasePath()
{
	if (fRtspServer == NULL) {
		return "";
	}

	return fRtspServer->GetBasePath();
}

IMediaSourcePtr RtspContext::GetMediaSource( LPCSTR path, UINT sourceId )
{
	if (fRtspServer == NULL) {
		return NULL;
	}

	return fRtspServer->GetMediaSource(path, sourceId);
}

SocketSelectorPtr RtspContext::GetSocketSelector()
{
	return fRtspServer->GetConnectionManager().GetSocketSelector();
}

String RtspContext::GetUserAgent()
{
	if (fRtspServer == NULL) {
		return "";
	}
	return fRtspServer->GetDeviceInformation();
}

int RtspContext::HandleRequest( BaseMessage *request, BaseMessage *response)
{
	if (fRtspServer == NULL) {
		return 0;
	}

	IRtspManagerListener* listener = fRtspServer->GetListener();
	if (listener) {
		listener->OnRtspRequest(request, response);
	}
	return 0;
}

void RtspContext::OnCloseRtspSession( RtspSession* session )
{
	if (fRtspServer) {
		fRtspServer->GetSessionManager().RemoveSession(session);
	}
}

}
