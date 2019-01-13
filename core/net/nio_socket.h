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
#ifndef _NS_VISION_CORE_NIO_SOCKET_H
#define _NS_VISION_CORE_NIO_SOCKET_H

#include "nio_channel.h"
#include "nio_selector.h"
#include "nio_network.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Socket class

/** 
 * Socket API wrapper class. Implements the Berkeley sockets interface.
 * 这个类封装了 Socket 的主要方法.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Socket : public SelectorChannel
{
// Construction/Destruction -----------------------------------
public:
	Socket();
	virtual ~Socket();

	enum { MAX_UDP_PORT =  65535 };

// Attributes -------------------------------------------------
public:
	LPCSTR GetLastError();
	BOOL GetSockName(char* address, UINT& port);
	BOOL GetPeerName(char* address, UINT& port);
	int  GetFd() { return sockfd; }
	int  GetSockOpt(int optname, int level = SOL_SOCKET);
	
	BOOL SetSocketOption(int optname, int option, int level = SOL_SOCKET);
	BOOL SetNoBlock(BOOL noBlock);

// Operations -------------------------------------------------
public:
	static BOOL GetLocalName(char* address, LPCSTR peerAddress = NULL);
	static BOOL SetNoBlock(int fd, BOOL noBlock);

public:
	virtual BOOL Create(UINT socketPort = 0, int socketType = SOCK_STREAM, LPCSTR socketAddress = NULL);
	virtual void Close();

	BOOL Accept(Socket &connectedSocket);
	BOOL AsyncSelect(int events);
	BOOL Attach(int sockfd);
	BOOL Bind(UINT socketPort, LPCSTR socketAddress = NULL);
	BOOL Connect(LPCSTR socketAddress, UINT socketPort);

	int  Detach();
	BOOL Listen(int backlog);

// Read/Write -------------------------------------------------
public:
	int  SendTo     (LPCSTR buf, int size, LPCSTR socketAddress, UINT  socketPort, int flags = 0);
	int  ReceiveFrom(LPCSTR buf, int size, LPSTR  socketAddress, UINT& socketPort, int flags = 0);

	int  Send   (LPCSTR buf, int size, int flags = 0);
	int  Receive(LPSTR  buf, int size, int flags = 0);

// Overrides --------------------------------------------------
public:
	virtual void OnClose	(int errorCode);
	virtual void OnReceive	(int errorCode);
	virtual void OnAccept	(int errorCode);
	virtual void OnSend		(int errorCode);
	virtual void OnConnect	(int errorCode);

// Data members -----------------------------------------------
public:
	int sockfd;     ///< Socket description
};

typedef SmartPtr<Socket> SocketPtr;

} //
}

#endif // !defined(_NS_VISION_CORE_NIO_SOCKET_H)
