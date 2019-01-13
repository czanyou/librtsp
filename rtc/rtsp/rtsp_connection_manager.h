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

#ifndef _NS_VISION_RTSP_CONNECTION_MANAGER_H
#define _NS_VISION_RTSP_CONNECTION_MANAGER_H

#include "core/net/nio_selector.h"
#include "core/net/nio_resolver.h"
#include "rtsp_message.h"
#include "rtsp_connection.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////

/**
 * 代表一个 RTSP 连接管理器. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspConnectionManager
{
private:
	class ListenSocket : public Socket 
	{
	public:
		ListenSocket();

	public:
		BOOL CreateSocket(UINT port);

	public:
		virtual void OnAccept(int errorCode);

	public:
		RtspConnectionManager* manager;
		UINT fListenPort;
	};

	typedef SmartPtr<ListenSocket> ListenSocketPtr;

public:
	RtspConnectionManager();
	typedef LinkedList<RtspConnectionPtr> RtspConnectionList;

// Attributes -------------------------------------------------
public:
	SocketSelectorPtr GetSocketSelector();
	int  GetConnectionCount();
	void GetConnections(RtspConnectionList& connections);
	UINT GetListenPort();

	void SetListenPort(UINT port);

// Operations -------------------------------------------------
public:
	void Close();
	BOOL OnAccept(Socket *socket);
	void OnDump(String& dump);
	void OnTimer();
	int  Open(IRtspContext* context);
	void Poll();

// Implementation ---------------------------------------------
private:
	void OnCheckConnections();

// Data Members -----------------------------------------------
private:
	RtspConnectionList	fConnections;		///< RTSP 连接列表
	ListenSocketPtr		fListenSocket;		///< RTSP server listen socket.
	IRtspContext*		fRtspContext;		///< 所属的 RTSP 服务器
	SocketSelectorPtr	fSelector;			///< The socket selector

	UINT	fListenPort;		///< 这个 RTSP 服务器侦听的端口
	Mutex	fMutex;				///< Mutex object.	
};

}

#endif // _NS_VISION_RTSP_CONNECTION_MANAGER_H
