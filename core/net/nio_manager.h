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
#ifndef _NS_VISION_CORE_NIO_MANAGER_H
#define _NS_VISION_CORE_NIO_MANAGER_H

#include "nio_selector.h"
#include "nio_network.h"
#include "nio_socket.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseSocketServer class 

class BaseSocketServer : public SocketServer
{
public:
	class BaseServerSocket : public Socket 
	{
	public:
		virtual void OnAccept(int errorCode);
		
	public:
		SmartPtr<BaseSocketServer> fBaseSocketServer;
	};

public:
	BaseSocketServer();

public:
	void SetSelector(SocketSelector* selector);

public:
	virtual int Listen(UINT socketPort, LPCSTR socketAddress = NULL, int backlog = 5);
	virtual int Close();
	virtual int GetConnectionCount();
	virtual void OnAccept(int errorCode);

// Data members -----------------------------------------------
private:
	UINT fListenPort;
	SocketSelectorPtr fSelector;	///< 多路选择器	
	SmartPtr<BaseServerSocket> fSocket;
};

typedef SmartPtr<BaseSocketServer> BaseSocketServerPtr;


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseSocketConnection class 

class BaseSocketConnection : public SocketConnection
{
public:
	class BaseConnectionSocket : public Socket 
	{
	public:
		virtual void OnClose	(int errorCode);
		virtual void OnReceive	(int errorCode);
		virtual void OnSend		(int errorCode);
		virtual void OnConnect	(int errorCode);

	public:
		SmartPtr<BaseSocketConnection> fBaseSocketConnection;
	};

public:
	BaseSocketConnection();

public:
	SocketListenerPtr GetListener();
	SocketPtr GetSocket();
	void SetSelector(SocketSelector* selector);

public:
	virtual int  Close();
	virtual int  Connect(UINT socketPort, LPCSTR socketAddress = NULL);
	virtual int  End();
	virtual int  IsInvalid();
	virtual int  OnInit();
	virtual int  Write(BYTE* data, UINT size);

	void OnReceive( int errorCode );
	void OnSend( int errorCode );
	int SendData(LPCSTR data, int dataLen);

// Data members -----------------------------------------------
private:
	SmartPtr<BaseConnectionSocket> fSocket;
	SocketSelectorPtr fSelector;	///< 多路选择器	
	ByteBufferPtr fRecvBuffer;		///< 接收缓存区
	ByteBufferPtr fSendBuffer;		///< 发送缓存区
	int		fReadFailedTimes;		///<

};

typedef SmartPtr<BaseSocketConnection> BaseSocketConnectionPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseSocketManager class 

class BaseSocketManager : public SocketManager
{
public:
	BaseSocketManager();

public:
	virtual void Close();
	virtual int  Open();
	virtual SocketServerPtr CreateServer();
	virtual void WaitEvents(int timeout);

// Data members -----------------------------------------------
private:
	SocketSelectorPtr fSelector;	///< 多路选择器	

};

typedef SmartPtr<BaseSocketManager> BaseSocketManagerPtr;


} //
}

#endif // !defined(_NS_VISION_CORE_NIO_MANAGER_H)