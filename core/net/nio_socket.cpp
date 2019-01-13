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

#include "nio_socket.h"


namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Socket class

Socket::Socket()
{
	sockfd = -1;
}

Socket::~Socket()
{
	Close();
}


/** 
* Accepts a connection on the socket.
* Call this member function to accept a connection on a socket.
* @param connectedSocket A reference identifying a new socket that is available
*      for connection.
* @return Nonzero if the function is successful; otherwise 0.
*/
BOOL Socket::Accept(Socket &connectedSocket)
{
	int socklen = sizeof(struct sockaddr_in);
	int tempfd;
	struct sockaddr_in addr_in;	

	if ((tempfd = accept(sockfd, (struct sockaddr*)&addr_in, (socklen_t*)&socklen)) < 0) {
		return FALSE;
	}

	return connectedSocket.Attach(tempfd);
}

BOOL Socket::AsyncSelect( int events )
{
	if (fSelector && sockfd > 0) {
		fEvents = events;

		fSelector->AsyncSelect(this, events);
		fSelector->WakeUp();
	}

	return TRUE;
}

/** Attach to sockfd. */
BOOL Socket::Attach(int sock)
{
	if (sockfd > 0) {
		Close();
	}
	sockfd = sock;
	return TRUE;
}

/** 
 * Associates a local address with the socket
 * Call this member function to associate a local address with the socket.
 * @param socketPort The port identifying the socket application
 * @param socketAddress The network address, a dotted number such as "128.56.22.8". 
 * @return Nonzero if the function is successful; otherwise 0.
 */
BOOL Socket::Bind(UINT socketPort, LPCSTR socketAddress /*=NULL*/)
{
	struct sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(struct sockaddr_in));	
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = 0;

	if (socketPort > 0) {
		addr_in.sin_port = htons((WORD)socketPort);
	}

	if (socketAddress == NULL) {
		addr_in.sin_addr.s_addr = INADDR_ANY;

	} else {
		addr_in.sin_addr.s_addr = inet_addr(socketAddress);
	}

	if (bind(sockfd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr)) < 0) {
		return FALSE;	
	}

	return TRUE;	
}

/** Closes the Socket connection and releases all associated resources. */
void Socket::Close()
{
	// Close socket...
	if (sockfd <= 0) {
		return;
	}

	int fd = Detach();

	shutdown(fd, 2);
	closesocket(fd);

	fEvents = 0;
}

/**
 * 连接到指定的目的地址.
 * @param socketAddress 目的地址
 * @param socketPort 目的端口
 */
BOOL Socket::Connect(LPCSTR socketAddress, UINT socketPort)
{
	if (isempty(socketAddress)) {
		return FALSE;
	}

	struct sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(struct sockaddr_in));	
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons((WORD)socketPort);

	if (inet_aton(socketAddress, &addr_in.sin_addr) == 0) {
		return FALSE;
	}

	int len = sizeof(struct sockaddr);
	int ret = connect(sockfd, (struct sockaddr*)&addr_in, len);
	if (ret < 0) {
		LOG_E("%s:%d (%d)", socketAddress, socketPort, ret);
		return FALSE;	
	}

	OnConnect(0);
	return TRUE;	
}

/** 
 * Create a socket.
 * Call the Create member function after constructing a socket object to create 
 * the socket.
 * @param socketPort A well-known port to be used with the socket, or 0 if you 
 * want System Sockets to select a port.
 * @param socketType SOCK_STREAM or SOCK_DGRAM
 * @param socketAddress A pointer to a string containing the network address 
 * of the connected socket, a dotted number such as "128.56.22.8".
 * @return Nonzero if the function is successful; otherwise 0.
 */
BOOL Socket::Create(UINT socketPort /*=0*/, int socketType /*=SOCK_STREAM*/, 
                    LPCSTR socketAddress /*=NULL*/)
{
	if ((sockfd = socket(AF_INET, socketType, 0)) <= 0) {
		return FALSE;
	}

    if (socketPort != 0) {
        return Bind(socketPort, socketAddress);
	}

	return TRUE;
}

/** Detach from sockfd. */
int Socket::Detach()
{
	SocketSelectorPtr selector = fSelector;
	fSelector = NULL;

	if (selector != NULL) {
		selector->RemoveChannel(this);
		selector = NULL;
	}

	int fd = sockfd;
	sockfd = -1;
	return fd;
}

/** Get the last error. */
LPCSTR Socket::GetLastError()
{
	return strerror(errno);
}

/**
* 取得本地的 IP 地址
*/
BOOL Socket::GetLocalName(char* address, LPCSTR peerAddress)
{
	sockaddr_in localAddress;
	sockaddr_in remoteAddress;
	int addressSize = sizeof(sockaddr_in);

	if (address == NULL) {
		LOG_E("Invalid address\r\n");
		return FALSE;
	}

	/* 开始地址*/
	localAddress.sin_addr.s_addr = INADDR_ANY;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == kInvalidSocket) {
		LOG_E("Invalid socket\r\n");
		return FALSE;
	}

	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_port   = htons(80);
	remoteAddress.sin_addr.s_addr = inet_addr("10.0.0.10");
	if (!isempty(peerAddress)) {
		inet_aton(peerAddress, &remoteAddress.sin_addr);
	}

	int ret = connect(fd, (struct sockaddr*)&remoteAddress, sizeof(sockaddr_in));
	if (ret != kSocketError) {
		//LOG_E("Invalid connect\r\n");
	}

	getsockname(fd, (struct sockaddr*)&localAddress, (socklen_t*)&addressSize);
	closesocket(fd);

	strcpy(address, (inet_ntoa(localAddress.sin_addr)));
	return TRUE;
}

/**
* Gets the address of the peer socket to which the socket is connected.
*/
BOOL Socket::GetPeerName(char *address, UINT &port)
{
	struct sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(struct sockaddr_in));	
	addr_in.sin_family = AF_INET;

	socklen_t len = sizeof(addr_in);
	if (getpeername(sockfd, (struct sockaddr*)(&addr_in), &len) != 0) {
		return FALSE;
	}

	if (address) {
		strcpy(address, inet_ntoa(addr_in.sin_addr));
	}

	port = ntohs(addr_in.sin_port);
	return TRUE;
}

/** Retrieves a socket option */
int Socket::GetSockOpt(int optname, int level)
{
	int option = -1;
	socklen_t optlen = sizeof(option);
	getsockopt(sockfd, level, optname, (char*)&option, &optlen);
	return option;
}

/** Gets the local name for a socket. */
BOOL Socket::GetSockName(char *address, UINT &port)
{
	struct sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(struct sockaddr_in));	
	addr_in.sin_family = AF_INET;

	socklen_t len = sizeof(addr_in);
	if (getsockname(sockfd, (struct sockaddr*)(&addr_in), &len) != 0) {
		return FALSE;
	}

	if (address) {
		strcpy(address, inet_ntoa(addr_in.sin_addr));
	}

	port = ntohs(addr_in.sin_port);
	return TRUE;
}

/** 
 * Establishes a socket to listen for incoming connection requests.
 * Call this member function to listen for incoming connection requests.
 * @param backlog The maximum length to which the queue of pending connections can grow. 
 * @return Nonzero if the function is successful; otherwise 0.
 */
BOOL Socket::Listen(int backlog)
{
	if (listen(sockfd, backlog) == -1) {
		return FALSE;		
	}
	return TRUE;
}

/** 当收到新的连接请求时调用这个方法. */
void Socket::OnAccept(int errorCode)
{
}

/** 当这个套接字被关闭时调用这个方法 */
void Socket::OnClose(int errorCode)
{
}

/** 当成功连接到指定的目录主机时调用这个方法. */
void Socket::OnConnect(int errorCode)
{
}

/** 当接收缓存区有新的数据时调用这个方法. */
void Socket::OnReceive(int errorCode)
{
}

/** 当发送缓存区为空时调用这个方法. */
void Socket::OnSend(int errorCode)
{
}

/** 
 * Receives data from the socket.
 * Call this member function to receive data from a socket.
 * @param buf A buffer for the incoming data.
 * @param size The length of the data in buf in bytes
 * @param flags Specifies the way in which the call is made
 * @return If no error occurs, Receive returns the number of bytes received. 
 * If the connection has been closed, it returns 0. Otherwise, a value of 
 * SOCKET_ERROR is returned, 
 */
int Socket::Receive(LPSTR buf, int size, int flags)
{
	if (buf == NULL || size <= 0) {
		return 0;
	}
	return recv(sockfd, buf, size, flags);
}

/** 
 * Receives a datagram and stores the source address.
 * Call this member function to receive a datagram and get the source address.
 * @param buf A buffer for the incoming data.
 * @param size The length of the data in buf in bytes
 * @param socketAddress
 * @param socketPort
 * @param flags
 */
int Socket::ReceiveFrom(LPCSTR buf, int size, LPSTR socketAddress, UINT& socketPort, int flags)
{
	if (buf == NULL || size <= 0) {
		return 0;
	}

	struct sockaddr_in addr_in;	
	memset(&addr_in, 0, sizeof(struct sockaddr_in));
	socklen_t len = sizeof(addr_in);

	int ret = recvfrom(sockfd, (char*)buf, size, flags, (struct sockaddr *)&addr_in, &len);
	if (ret >= 0) {
		socketPort = ntohs(addr_in.sin_port);
		if (socketAddress != NULL) {
			strcpy(socketAddress, inet_ntoa(addr_in.sin_addr));
		}
	}
	return ret;
}

/** 
 * Sends data to a connected socket.
 * Call this member function to send data on a connected socket.
 * @param buf A buffer containing the data to be transmitted.
 * @param size The length of the data in buf in bytes
 * @param flags Specifies the way in which the call is made
 * @return If no error occurs, Send returns the total number of characters sent. 
 * (Note that this can be less than the number indicated by size.) Otherwise, 
 * a value of SOCKET_ERROR is returned.
 */
int Socket::Send(LPCSTR buf, int size, int flags)
{
	if (buf == NULL || size <= 0) {
		return 0;
	}
	return send(sockfd, buf, size, flags);
}

/** 
 * Sends data to a specific destination.
 * Call this member function to send data to a specific destination.
 * @param buf A buffer containing the data to be transmitted.
 * @param size The length of the data in buf in bytes
 * @param hostAddress The network address, a dotted number such as "128.56.22.8". 
 * @param hostPort The port identifying the socket application
 * @param flags Specifies the way in which the call is made
 * @return If no error occurs, Send returns the total number of characters sent. 
 * (Note that this can be less than the number indicated by size.) Otherwise, 
 * a value of SOCKET_ERROR is returned.
 */
int Socket::SendTo(LPCSTR buf, int size, LPCSTR hostAddress, UINT hostPort, int flags)
{
	if (buf == NULL || size <= 0) {
		return -1;

	} else if (isempty(hostAddress) || hostPort == 0) {
		return -2;
	}

	struct sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(struct sockaddr_in));	
	if (inet_aton(hostAddress, &addr_in.sin_addr) == 0) {
		return -3;
	}

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons((WORD)hostPort);
	return sendto(sockfd, (char*)buf, size, flags, (struct sockaddr *)&addr_in, sizeof(addr_in));
}

/** Set noblock mode. */
BOOL Socket::SetNoBlock(int fd, BOOL noBlock)
{
	long file_flags;
	file_flags = fcntl(fd, F_GETFL);

	if (!noBlock) {
		file_flags &= ~O_NONBLOCK;
	} else {
		file_flags |= O_NONBLOCK;
	}

	fcntl(fd, F_SETFL, file_flags);

	return TRUE;
}

/** Set noblock mode. */
BOOL Socket::SetNoBlock(BOOL noBlock)
{
	return Socket::SetNoBlock(sockfd, noBlock);
}

/** Sets a socket option. */
BOOL Socket::SetSocketOption(int optname, int option, int level)
{
	int optlen = sizeof(option);
	return (setsockopt(sockfd, level, optname, (const char*)(&option), optlen) == 0);
}

} //
}
