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

#include "core/utils/md5.h"
#include "rtsp_manager.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//

/** 创建一个 RTSP Server 的实例. */
IRtspManagerPtr CreateRtspServer()
{
	return new rtsp::RtspManager();
}

const UINT kRtspEventProfile				= 0x0311;
const UINT kRtspEventNotify					= 0x0313;
//const UINT kRtspEventSessionStart			= 0x0315;
//const UINT kRtspEventSessionStop			= 0x0317;
//const UINT kRtspEventConnectionAttempt		= 0x0319;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspEvent

class RtspEvent : public Object
{
public:
	String content;
	String contentType;
	String eventType;
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspManager class

RtspManager::RtspManager()
{
	fState				= STATE_STOPED;
	fLastActiveTime		= GetSysTickCount();
}

RtspManager::~RtspManager()
{
	if (fState != STATE_STOPED) {
		Close();
	}
}

/** 关闭这个 RTSP 服务. */
int RtspManager::Close()
{
	if (fState != STATE_STOPED) {
		fState = STATE_STOPED;	
	}

	ThreadJoin();
	return RTSP_S_OK;
}

void RtspManager::DispatchEvent( EventObject* event )
{
	if (event == NULL) {
		return;
	}

	//LOG_D("%x\r\n", event->fType);

	Object* object = event->fSourceObject;

	switch (event->fType) {
	case kRtspEventProfile:
		fRtspServer.UpdateSettings(dynamic_cast<IPreferences*>(object));
		break;

	default:
		break;
	}
}

/** 
 * 返回这个 RTSP 服务器的当前状态. 
 *
 * @return 返回 STATE_STOPED 表示当前线程死掉或退出了.
 */
UINT RtspManager::GetState()
{
	INT64 now = GetSysTickCount();
	UINT offset = UINT((now - fLastActiveTime) / 1000);
	if (offset >= 60 * 2) {
		LOG_W("Invalid RTSP running status: timeout(%d)...\r\n", offset);
		return STATE_STOPED;
	}
	return fState;
}

/** 返回这个 RTSP 服务的当前状态. */
String RtspManager::GetStatusText(LPCSTR path)
{
	return fRtspServer.GetStatusText(path);
}

void RtspManager::OnDump( String& dump )
{
	fRtspServer.OnDump(dump);
}

void RtspManager::PostEvent( EventObject* event )
{
	Thread::PostEvent(event);

	SocketSelectorPtr selector = fRtspServer.GetConnectionManager().GetSocketSelector();
	if (selector) {
		selector->WakeUp();
	}
}

/** 进入事件循环, 这个方法将不会返回, 直到关闭了这个服务器. */
int RtspManager::Run()
{
	int ret = fRtspServer.Open();
	if (ret != RTSP_S_OK) {
		fState = STATE_STOPED;
		fRtspServer.Close();
		return ret;
	}

	fState = STATE_RUNNING;

	RtspConnectionManager& connectionManager = fRtspServer.GetConnectionManager();
	INT64 lastTime = 0;

	while (fState == STATE_RUNNING) {
		DispatchEvents();

		INT64 now = GetSysTickCount();
		if (UINT(now - lastTime) >= 1000) {
			lastTime = now;

			fLastActiveTime = GetSysTickCount();
			fRtspServer.OnTimer();
		}

		connectionManager.Poll();
	}

	fRtspServer.Close();
	return 0;
}

/** 注册指定的事件侦听器. */
void RtspManager::SetListener( IRtspManagerListener *listener )
{
	fRtspServer.SetListener(listener);
}

/** 
 * 启动这个 RTSP 服务. 
 * @return 如果成功则返回 0, 如果失败则返回一个小于 0 的错误码.
 */
int RtspManager::Start()
{
	if (fState == STATE_RUNNING) {
		return RTSP_E_INVALID_STATE;
	}

	fState = STATE_STARTING;

	SetThreadName("RTSP Server");
	StartThread();

	return RTSP_S_OK;
}

/** 停止这个 RTSP 服务. */
int RtspManager::Stop()
{
	if (fState == STATE_STOPED) {
		return RTSP_S_OK;
	}

	LOG_D("Stoping RTSP Server...\r\n");

	fState = STATE_STOPPING;
	return RTSP_S_OK;
}

/** 更新参数设置. */
int RtspManager::UpdateSettings( IPreferences* profile )
{
	if (profile) {
		SendEvent(kRtspEventProfile, 0, 0, profile);
	}

	return 0;
}

}
