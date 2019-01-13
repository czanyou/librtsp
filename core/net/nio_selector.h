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
#ifndef _NS_VISION_CORE_NIO_SELECTOR_H
#define _NS_VISION_CORE_NIO_SELECTOR_H

#include "nio_channel.h"

#if 1
#ifdef __linux
#define EPOLL_SUPPORT 1
#endif
#endif

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketSelector class 

/**
 * 用于监视多路 Socket I/O. 实现多路复用.
 * 它使用 epoll 方法来监视频多路 socket 的状态.
 * 
 * @author ChengZhen (anyou@msn.com)
 */
class SocketSelector : public Object
{
public:
	SocketSelector();
	~SocketSelector();
	typedef HashMap<int, SelectorChannelPtr> ChannelMap;  ///< Socket Map

public:
	int  GetFd();

// Operations -------------------------------------------------
public:
	virtual int  AsyncSelect(SelectorChannel* socket, int listenEvents);

	virtual void ClearPipe();
	virtual void Close();

	virtual int  WaitEvents(int timeout = -1);
	virtual int  InitPipe();

	virtual int  Open();
	virtual int  RemoveChannel(SelectorChannel* socket);
	virtual int  WakeUp();

	String OnDump(UINT indent);

public:
	static BOOL CanRead(int fd, int timeout );
	static BOOL SetNoBlock(int fd, BOOL noBlock);

// Implementation ---------------------------------------------
protected:
	SelectorChannelPtr GetChannel(int sockfd);
	BOOL IsPipe(int fd);

	int  SelectWait(int timeout);
	void SelectHandleEvents();
	int  SelectInitfdSet();

// Data Members -----------------------------------------------
protected:
	ChannelMap	fSockets;		///< Sockets

	fd_set  fReadSet;			///< 
	fd_set  fSendSet;			///< 

	int		fEpollFd;			///< epoll handler
	UINT    fEPollMode;			///< 
	Mutex	fMutex;				///< Mutex
	SocketPipe	fPipe;				///< 管道
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// EpollSocketSelector class 

#ifdef EPOLL_SUPPORT
class EpollSocketSelector : public SocketSelector
{
public:
	EpollSocketSelector();

public:
	int  EPollAddFileDescripor(int fd, UINT events = OP_READ);
	int  EPollAsyncSelect(SelectorChannel* socket, int events);
	int  EPollControl(int fd, int op, UINT events);
	void EPollHandleEvent(SelectorChannel *socket, UINT events);
	void EPollHandleEvent(int fd, UINT events);
	int  EPollWait(int timeout);

public:
	int  AsyncSelect(SelectorChannel* socket, int events);
	void Close();
	int  InitPipe();
	int  Open();
	int  RemoveChannel( SelectorChannel* socket );
	int  WaitEvents(int timeout);

private:
	epoll_event fEvents[20];	///< 事件列表

	int		fEpollFd;			///< epoll handler
	UINT    fEPollMode;			///< 
};

#endif

typedef SmartPtr<SocketSelector> SocketSelectorPtr;

SocketSelectorPtr SocketSelectorCreate();

} //
}

#endif // _NS_VISION_CORE_NIO_SELECTOR_H
