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

#include "rtsp_server.h"
#include "core/utils/md5.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RTSP Server class

RtspServer::RtspServer()
{
	fRtspListener	= NULL;
	fRtspContext	= new RtspContext();
	fRtspContext->fRtspServer = this;
}

RtspServer::~RtspServer()
{

}

/** 
 * Check Authorization. 
 * RTSP 也使用 HTTP 规定的认证方式. 具体的细节可以参考 HTTP 协议相关章节.
 */
BOOL RtspServer::CheckAuthorization(RtspConnection *connection, BaseMessage* request)
{
	if (connection == NULL) {
		return FALSE;
	}

	String username;

	String address = connection->GetPeerAddress();
	if (!fRtspAuth.CheckAuthorization(address, request, username)) {
		return FALSE;
	}

	if (!username.IsEmpty()) {
		connection->SetRemoteUser(username.c_str());
	}

	return TRUE;
}

/** 停止这个 RTSP 服务. */
int RtspServer::Close()
{
	fConnectionManager.Close();
	return RTSP_S_OK;
}

String RtspServer::GetBasePath()
{
	return fRtspSettings.fRecordPath;
}

String RtspServer::GetBasicStatus(LPCSTR remoteAddress)
{
	String key = "";
	if (isempty(remoteAddress)) {
		remoteAddress = "";

	} else {
		MD5 md5;
		key = fRtspAuth.GetAuthorizationFeed() + remoteAddress;
		key = md5.Hash((BYTE*)key.c_str(), key.GetLength());
	}

	String status;
	status.Format(
		"{\n"
		" \"name\":\"%s\",\n"
		" \"version\":\"%s\",\n"
		" \"connection_count\":%d,\n"
		" \"session_count\":%d,\n"
		" \"remote_address\":\"%s\",\n"
		" \"access_key\":\"%s\"\n"
		"}",
		RTSP_SERVER_NAME, 
		RTSP_SERVER_VERSION,
		fConnectionManager.GetConnectionCount(),
		fSessionManager.GetCount(),
		remoteAddress, 
		key.c_str());

	return status;
}

RtspConnectionManager& RtspServer::GetConnectionManager()
{
	return fConnectionManager;
}

String RtspServer::GetConnectionStatus()
{
	String status;
	String text;

	RtspConnectionList connections;
	fConnectionManager.GetConnections(connections);
	time_t now = time(NULL);
	LPCSTR separator = " ";

	status = "[\n";
	RtspConnectionList::ListIterator iter = connections.GetListIterator();
	while (iter.HasNext()) {
		RtspConnectionPtr connection = iter.Next();
		RtspSocketPtr socket = connection->GetRtspSocket();

		if (connection == NULL) {
			break;

		} if (socket->sockfd <= 0) {
			status += separator;
			status += "{ state:\"idle\" }";
			continue;
		}


		int expires = int(socket->GetLastActiveTime() - now);
		text.Format( 
			"%s{\"id\":%d, \"address\":\"%s\", \"port\":%d, \"expires\":%d}",
			separator, socket->sockfd, connection->GetPeerAddress().c_str(), 
			connection->GetPeerPort(), expires);

		status += text;
		separator = ",\n ";
	}

	status += " ],\n";
	text.Format("\"connection_count\": %d", RtspConnection::gConnectionCount);
	status += text;
	return status;
}

RtspContextPtr RtspServer::GetRtspContext()
{
	return fRtspContext;
}

/** 返回这个 RTSP 服务器的设备 User-Agent 信息. */
String RtspServer::GetDeviceInformation()
{
	return fRtspSettings.fDeviceInfo;
}

IRtspManagerListener* RtspServer::GetListener()
{
	return fRtspListener;
}

IMediaSourcePtr RtspServer::GetMediaSource( LPCSTR path, UINT id )
{
	if (fRtspListener) {
		return fRtspListener->OnRtspGetSource(path, id);
	}
	return NULL;
}

/** 返回这个 RTSP 服务器当前创建的 RTSP/RTP 会话的数目. */
int RtspServer::GetSessionCount()
{
	return fSessionManager.GetCount();
}

/** 返回相关的 RTSP 会话管理器的引用. */
RtspSessionManager& RtspServer::GetSessionManager()
{
	return fSessionManager;
}

String RtspServer::GetSessionStatus()
{
	String status;
	String text;

	LPCSTR separator = " ";
	status += "[\n";

	RtspSessionManager::RtspSessionList sessions;
	fSessionManager.GetAll(sessions);

	RtspSessionManager::RtspSessionList::ListIterator iter = sessions.GetListIterator();
	while (iter.HasNext()) {
		RtspSessionPtr rtspSession = iter.Next();

		SmartPtr<IRtspConnection> object = rtspSession->GetConnection();
		RtspConnection* rtspConnection = dynamic_cast<RtspConnection*>(object.get());

		if (rtspSession->GetSessionState() == kRtspStateInit) {
			text.Format("%s{ state: \"invalid\" }", separator);

		} else if (rtspConnection == NULL) {
			text.Format(
				"%s{\"id\":\"%s\", \"address\":\"%s\", \"port\":%d, \"video\":\"%dfps/%dkbps\", "
				"\"audio\":\"%dfps/%dkbps\"}",
				separator, rtspSession->GetSessionId().c_str(),
				"[SIP]", 0,
				rtspSession->GetFrameRate(kMediaTypeVideo),
				(rtspSession->GetBitrate(kMediaTypeVideo) >> 7),
				rtspSession->GetFrameRate(kMediaTypeAudio),
				(rtspSession->GetBitrate(kMediaTypeAudio) >> 7));

		} else {
			text.Format(
				"%s{\"id\":\"%s\", \"address\":\"%s\", \"port\":%d, \"video\":\"%dfps/%dkbps\", "
				"\"audio\":\"%dfps/%dkbps\"}",
				separator, rtspSession->GetSessionId().c_str(),
				rtspConnection->GetPeerAddress().c_str(),
				(int)rtspConnection->GetPeerPort(),
				rtspSession->GetFrameRate(kMediaTypeVideo),
				(rtspSession->GetBitrate(kMediaTypeVideo) >> 7),
				rtspSession->GetFrameRate(kMediaTypeAudio),
				(rtspSession->GetBitrate(kMediaTypeAudio) >> 7));
		}

		separator = ",\n ";
		status += text;
	}

	status += " ],\n";
	text.Format("\"connection_count\": %d", RtspSession::gSessionCount);
	status += text;
	return status;
}


RtspSettings& RtspServer::GetSettings()
{
	return fRtspSettings;
}

/** 返回这个 RTSP 服务的当前状态. */
String RtspServer::GetStatusText(LPCSTR path)
{
	String status;
	String type = path;
	if (type == "channels") {
		return GetSessionStatus();

	} else if (type == "connections") {
		return GetConnectionStatus();

	} else {
		return GetBasicStatus();
	}

	return status;
}

void RtspServer::OnDump(String& dump)
{
	fSessionManager.OnDump(dump);
	fConnectionManager.OnDump(dump);
}

/** 大概每隔一秒调用一次这个方法 */
int RtspServer::OnTimer()
{
	fSessionManager.OnTimer();
	fConnectionManager.OnTimer();
	return 0;
}

/** 
 * 启动这个 RTSP 服务. 
 * @return 如果成功则返回 0, 如果失败则返回一个小于 0 的错误码.
 */
int RtspServer::Open()
{
	int ret = fConnectionManager.Open(fRtspContext);
	if (ret != RTSP_S_OK) {
		LOG_E("Start connection manager failed! (%d)\r\n", ret);
		return ret;
	}

	return RTSP_S_OK;
}

/** 注册指定的事件侦听器. */
void RtspServer::SetListener( IRtspManagerListener *listener )
{
	fRtspListener = listener;
}

/** 更新参数设置. */
int RtspServer::UpdateSettings( IPreferences* preferences )
{
	if (preferences == NULL) {
		return 0;
	}

	String macAddress				= preferences->GetString("eth_mac", "IPCAM");
	String hardwareVersion			= preferences->GetString("device_model", "IPCAM");
	String firmwareVersion			= preferences->GetString("firmware_version", "IPCAM");

	fRtspSettings.fAuthEnabled		= preferences->GetInt("rtsp_auth", 0);
	fRtspSettings.fListenPort		= preferences->GetInt("rtsp_port", kRtspDefaultPort);

	fRtspSettings.fDeviceInfo.Format("Linux/2.6.14,%s/%s,(MAC:%s)", 
		hardwareVersion.c_str(), firmwareVersion.c_str(), macAddress.c_str());

	fRtspAuth.SetEnabled(fRtspSettings.fAuthEnabled);

	return RTSP_S_OK;
}

}
