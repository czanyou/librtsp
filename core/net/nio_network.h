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
#ifndef _NS_VISION_CORE_NIO_NETWORK_H
#define _NS_VISION_CORE_NIO_NETWORK_H

namespace core {
namespace net {

#ifndef kSocketError
	const UINT kSocketError		= -1;
	const UINT kInvalidSocket	= -1;
#endif

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketAddress class

/**
 * Socket address
 * 代表一个 Socket 地址, 由 IP 地址和端口组成.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class SocketAddress
{
public:
	SocketAddress();
	SocketAddress(LPCSTR address);
	SocketAddress(const SocketAddress& address);

// Attributes -------------------------------------------------
public:
	String GetAddress();
	BYTE*  GetAddressBytes();
	UINT   GetPort();

	BOOL IsLocalAddress();
	BOOL IsValidAddress();

	void SetAddress(const BYTE* address);
	void SetAddress(LPCSTR address);
	void SetPort(UINT port);

public:
	static BOOL IsIpAddress(LPCSTR hostName);

// Operations -------------------------------------------------
public:
	void Clear();
	String ToString();
	SocketAddress& operator = (const SocketAddress& address);

// Data members -----------------------------------------------
public:
	BYTE fAddress[32];	///< Specifies the IP address for this connection	
	UINT fPort;			///< The Port
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketListener class

#define kSocketEventConnection	114
#define kSocketEventRead		115
#define kSocketEventClose		116

class SocketConnection;

/**
 * Socket 事件侦听接口, 应用程序通过实现和注册这个接口来处理网络事件
 */
class SocketListener : public Object
{
public:
	SocketListener();

// Operations -------------------------------------------------
public:
	virtual void OnEvent(int type, int result, SocketConnection* connection) = 0;
	virtual void OnRead(int type, ByteBuffer* buffer, SocketConnection* connection);
	virtual void OnSend(int type, ByteBuffer* buffer, SocketConnection* connection);
};

typedef SmartPtr<SocketListener> SocketListenerPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketConnection class

/**
 * 代表一个 Socket 连接
 */
class SocketConnection : public Object
{
public:
	SocketConnection();

// Attributes -------------------------------------------------
public:
	virtual int  GetBufferSize();
	virtual int  IsInvalid();
	virtual int  SetListener(SocketListener* listener);
	virtual int  SetTimeout(int timeout);

// Operations -------------------------------------------------
public:
	virtual int  Close();
	virtual int  Connect(UINT socketPort, LPCSTR socketAddress = NULL);
	virtual int  End();
	virtual int  Write(BYTE* data, UINT size);

public:
	virtual SocketAddress GetLocalAddress();
	virtual SocketAddress GetRemoteAddress();

// Data members -----------------------------------------------
protected:
	int fTimeout;
	int fBufferSize;
	SocketAddress fLocalAddress;
	SocketAddress fRemoteAddress;
	SocketListenerPtr fListener;
};

typedef SmartPtr<SocketConnection> SocketConnectionPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketServer class

/**
 * 代表一个服务端 Socket
 */
class SocketServer : public Object
{
public:
	SocketServer();

// Attributes -------------------------------------------------
public:
	virtual int GetConnectionCount();
	virtual int SetListener(SocketListener* listener);

// Operations -------------------------------------------------
public:
	virtual int Close();
	virtual int Listen(UINT socketPort, LPCSTR socketAddress = NULL, int backlog = 5);

// Data members -----------------------------------------------
protected:
	SocketListenerPtr fListener;
};

typedef SmartPtr<SocketServer> SocketServerPtr;


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketManager class

/**
 * Socket 管理器
 */
class SocketManager : public Object
{
public:
	SocketManager();

// Operations -------------------------------------------------
public:
	virtual int  Open();
	virtual void Close();
	virtual void WaitEvents(int timeout);

public:
	virtual SocketServerPtr CreateServer();
};


typedef SmartPtr<SocketManager> SocketManagerPtr;

SocketManagerPtr SocketManagerCreate();


} //
}

#endif // !defined(_NS_VISION_CORE_NIO_NETWORK_H)
