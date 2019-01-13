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
#ifndef _NS_VISION_RTSP_CONNECTION
#define _NS_VISION_RTSP_CONNECTION

#include "media/vision_media.h"
#include "rtsp_session.h"
#include "rtsp_writer.h"
#include "rtsp_socket.h"
#include "rtsp_handler.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspConnection class

/** 
 * RtspConnection 代表一个 RTSP 连接.
 * 服务器通过这个类来处理客户端的 RTSP 请求, 当然也包括其他的如 HTTP 请求.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspConnection : public IRtspConnection, public RtspSocketListener
{
// Construction/Destruction -----------------------------------
public:
	RtspConnection(IRtspContext* rtspContext);
	virtual ~RtspConnection();

	static int gConnectionCount; ///< 实例数

// Attributes -------------------------------------------------
public:
	RtspSessionPtr GetRtspSession(BOOL create = FALSE);
	RtspSocketPtr GetRtspSocket();

	String GetContentBase( Uri &url );
	String GetRtspSessionId();
	String GetRemoteUser();
	String GetLocalAddress();
	String GetPeerAddress();
	UINT   GetPeerPort();

	BOOL   IsAuthed();
	BOOL   IsConnectionClose();
	BOOL   IsLiveStream();
	BOOL   IsValidConnection();

	void   SetAuthed(BOOL authed);
	void   SetConnectionClose(BOOL connectionClose);
	void   SetRemoteUser(LPCSTR user);

// Operations -------------------------------------------------
public:
	void Close();

	void OnCheckConnection();
	void OnDump(String& dump, int indent);
	void OnInit();
	void OnRtspSend(int errorCode);
	void OnSendBufferNotify(int flags);

	void ResetWriter();
	int  SendMessageContent(IMediaSample* mediaSample);
	int  SendNotifyMessage(LPCSTR content, LPCSTR contentType, LPCSTR eventType = "ipcam:event");
	int  SendResponse(RtspMessage* response);

// Implementation ---------------------------------------------
protected:
	void CloseRtspSession();
	void OnRtspMessage(RtspMessage* message);
	void OnRtspPacket(char* packet, size_t packetSize);
	void OnRtspConnect(int errorCode);
	void OnFillSendBuffer();
	int  SendMessage(LPCSTR message, size_t length);

// Data Members -----------------------------------------------
protected:
	SocketAddress	fLocalAddress;		///< Specifies the local IP address for this connection
	SocketAddress	fPeerAddress;		///< Specifies the peer IP address for this connection	
	IRtspContext*	fRtspContext;		///< 这个连接所属的 RTSP 上下文对象.
	RtspHandler		fRtspHandler;		///< 这个连接相关的 RTSP Handler
	RtspSessionPtr	fRtspSession;		///< 这个连接相关的 RTSP 会话对象
	RtspSocketPtr	fRtspSocket;		///< 这个连接相关的 Socket 对象

	BOOL	fConnectionClose;			///< Connection close flag, 
	int		fConnectionId;				///< 这个连接的 ID
	int		fCSeq;						///< 这个连接相关请求 CSeq number
	BOOL	fIsAuthed;					///< 指出这个连接是否已经通过认证
	String	fRemoteUser;				///< 这个连接相关的远端用户名
};

/** RtspConnection 智能指针类型. */
typedef SmartPtr<RtspConnection> RtspConnectionPtr;

}; // namespace rtsp

#endif // !defined(_NS_VISION_RTSP_CONNECTION)
