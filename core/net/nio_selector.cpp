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

#include "nio_selector.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketSelector class 

/** 默认构建方法. */
SocketSelector::SocketSelector()
{
}

SocketSelector::~SocketSelector()
{
	Close();
}

/**
 * 修改指定的 Socket 要监视的事件.
 * EPOLLIN:  相关的文件 read 操作可用
 * EPOLLPRI: 相关的文件 read 操作可用 (紧急数据可用).
 * EPOLLOUT: 相关的文件 write 操作可用
 * EPOLLERR: 相关的文件发生错误
 * EPOLLHUP: 相关的文件被挂起.
 * EPOLLET:  将相关的文件设置为边缘触发行为方式. 默认为水平触发方式.
 * EPOLLONESHOT: 将相关的文件设置为一次性触发行为方式. 
 */
int SocketSelector::AsyncSelect(SelectorChannel* socket, int events)
{
	if (socket == NULL || socket->GetFd() <= 0) {
		LOG_D("Invalid socket\r\n");
		return -1;
	}

	socket->fEvents		= events;
	socket->fSelector	= this;

	int fd = socket->GetFd();
	//LOG_W("%d, %x", fd, events);

	if (GetChannel(fd) == NULL) {
		fMutex.Lock();
		fSockets.Put(fd, socket);
		fMutex.UnLock();
	}

	return 0;
}

BOOL SocketSelector::CanRead(int fd, int timeout )
{
	if (fd <= 0) {
		return FALSE;
	}

	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	int maxFd = fd;

	struct timeval tv;
	tv.tv_sec  = timeout / 1000;
	tv.tv_usec = timeout % 1000 * 1000;

	int ret = select(maxFd + 1, &rfds, NULL, NULL, &tv);
	return (ret <= 0) ? FALSE : TRUE;
}

/**
 * 关闭这个选择器并释放相关的资源.
 * @return void 
 */
void SocketSelector::Close()
{
	ChannelMap::EntryList list;

	fMutex.Lock();
	fSockets.GetEntrys(list);

	fSockets.Clear();
	fPipe.Close();
	fMutex.UnLock();

	ChannelMap::EntryList::ListIterator iter = list.GetListIterator();
	while (iter.HasNext()) {
		ChannelMap::EntryPtr entry = iter.Next();
		SelectorChannelPtr channel = entry->fValue; 
		if (channel) {
			channel->fSelector = NULL;
		}
	}
}

/**
 * 清除管道中的数据.
 * @return BOOL 
 */
void SocketSelector::ClearPipe()
{
	if (fPipe.InPipe() > 0) {
		char buffer[MAX_PATH];
		read(fPipe.InPipe(), buffer, sizeof(buffer));
	}
}


/**
 * 查找指定的 fd 的 Socket.
 */
SelectorChannelPtr SocketSelector::GetChannel(int sockfd)
{
	fMutex.Lock();
	SelectorChannelPtr channel = fSockets[sockfd];
	fMutex.UnLock();

	return channel;
}

int SocketSelector::GetFd()
{
	return fEpollFd;
}

/** 创建相关的管道, 用来唤醒阻塞的 epoll 或 select 调用. */
int SocketSelector::InitPipe()
{
	fPipe.Open();

	return 0;
}

/** 指出指定的 fd 是否是控制用的管道, 如果是则清空缓存区并返回 TRUE. */
BOOL SocketSelector::IsPipe(int fd) 
{
	if (fd > 0 && fd == fPipe.InPipe()) {
		return TRUE;
	}
	return FALSE;
}

int SocketSelector::Open()
{
	if (fEpollFd > 0) {
		return -1;
	}

	return 0;
}

String SocketSelector::OnDump(UINT indent)
{
	String dump;
	dump.Format("<table>\n"
		"  <tr>"
		"<th>Epoll</th> <th>PIPE in</th> <th>PIPE out</th>"
		"<th>Socket Count</th> </tr>\n"
		"  <tr>"
		"<td>%d</td> <td>%d</td> <td>%d</td> <td>%d</td> "
		"</tr>\n </table>\n", 
		fEpollFd, fPipe.InPipe(), fPipe.OutPipe(), fSockets.GetSize());
	return dump;
}

/** 移除指定的 SelectorChannel */
int SocketSelector::RemoveChannel( SelectorChannel* socket )
{
	if (socket == NULL) {
		return -1;
	}

	int fd = socket->GetFd();
	if (fd <= 0) {
		return -1;
	}

	fMutex.Lock();
	fSockets.Remove(fd);
	fMutex.UnLock();

	socket->fEvents = 0;
	return 0;
}

void SocketSelector::SelectHandleEvents()
{
	// check pipe
	int fd = fPipe.InPipe();
	if ((fd > 0) && FD_ISSET(fd, &fReadSet)) {
		ClearPipe();
	}

	// check sockets
	ChannelMap::EntryList list;
	fMutex.Lock();
	fSockets.GetEntrys(list);
	fMutex.UnLock();

	ChannelMap::EntryList::ListIterator iter = list.GetListIterator();
	while (iter.HasNext()) {
		ChannelMap::EntryPtr entry = iter.Next();
		SelectorChannelPtr channel = entry->fValue; 
		if (channel == NULL) {
			continue;
		}

		fd = channel->GetFd();
		if (FD_ISSET(fd, &fReadSet)) {
			if (channel->fEvents & OP_ACCEPT) {
				channel->OnAccept(0);

			} else if (channel->fEvents & OP_CONNECT) {
				channel->OnConnect(0);

			} else if (channel->fEvents & OP_READ) {
				channel->OnReceive(0);

			} else {
				channel->OnReceive(0);
			}
		}

		if (FD_ISSET(fd, &fSendSet)) {

			if (channel->fEvents & OP_SEND) {
				channel->OnSend(0);
			}
		}
	}
}

int SocketSelector::SelectInitfdSet()
{
	FD_ZERO(&fReadSet);
	FD_ZERO(&fSendSet);

	int maxFd = 0;

	// pipe
	int fd = fPipe.InPipe();
	if (fd > 0) {
		maxFd = MAX(fd, maxFd);
		FD_SET(fd, &fReadSet);
	}

	// sockets
	ChannelMap::EntryList list;
	fMutex.Lock();
	fSockets.GetEntrys(list);
	fMutex.UnLock();

	ChannelMap::EntryList::ListIterator iter = list.GetListIterator();
	while (iter.HasNext()) {
		ChannelMap::EntryPtr entry = iter.Next();
		SelectorChannelPtr channel = entry->fValue; 
		if (channel == NULL) {
			continue;
		}

		int fd = channel->GetFd();
		int events = channel->fEvents;

		if (events & (OP_READ | OP_ACCEPT | OP_CONNECT)) {
			maxFd = MAX(fd, maxFd);
			FD_SET(fd, &fReadSet);

			// LOG_I("fReadSet: %d, %d", fd, events);
		}

		if (events & OP_SEND) {
			maxFd = MAX(fd, maxFd);
			FD_SET(fd, &fSendSet);
		}
	}	

	return maxFd;
}

/** Socket Select Loop */
int SocketSelector::SelectWait(int timeout)
{
	int maxFd = SelectInitfdSet();

	// timeout
	struct timeval tv;
	tv.tv_sec  = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	// select
	int ret = select(maxFd + 1, &fReadSet, &fSendSet, NULL, &tv);
	if (ret <= 0) {
		return ret;
	}

	SelectHandleEvents();
	return ret;
}

BOOL SocketSelector::SetNoBlock( int fd, BOOL noBlock )
{
#ifdef _WIN32
	UINT status = noBlock ? 1 : 0;
	ioctlsocket(fd, FIONBIO, &status);

#else
	long fileFlags = fcntl(fd, F_GETFL);
	if (!noBlock) {
		fileFlags &= ~O_NONBLOCK;

	} else {
		fileFlags |= O_NONBLOCK;
	}

	fcntl(fd, F_SETFL, fileFlags);
#endif

	return TRUE;
}

/** Socket Select Loop */
int SocketSelector::WaitEvents(int timeout)
{
	return SelectWait(timeout);
}

/** 立即唤醒阻塞的 epoll 或 select 方法调用. */
int SocketSelector::WakeUp()
{
	// 写数据到管道用来唤醒
	int ret = 0;
	if (fPipe.OutPipe() > 0) {
		char buffer[MAX_PATH] = { 0x01 };
		ret = write(fPipe.OutPipe(), buffer, 1);
	}

	return ret;
}


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// EpollSocketSelector class 


#ifdef EPOLL_SUPPORT

EpollSocketSelector::EpollSocketSelector()
{
	fEpollFd	= -1;
	fEPollMode	= EPOLLET;
}

int EpollSocketSelector::AsyncSelect(SelectorChannel* socket, int events)
{
	return EPollAsyncSelect(socket, events);
}

void EpollSocketSelector::Close()
{
	SocketSelector::Close();

	close(fEpollFd);
	fEpollFd = -1;
}

/**
 * 添加指定的句柄
 * @param fd 
 * @param events 
 * @return int 
 */
int EpollSocketSelector::EPollAddFileDescripor( int fd, UINT events )
{
	UINT pollEvents	= fEPollMode;

	if (events & OP_ACCEPT) {
		pollEvents |= EPOLLIN;

	} else if (events & OP_CONNECT) {
		pollEvents |= (EPOLLIN | EPOLLOUT);

	}  else if (events & OP_READ) {
		pollEvents |= EPOLLIN;
	} 

	if (events & OP_SEND) {
		pollEvents |= EPOLLOUT;
	} 

	return EPollControl(fd, EPOLL_CTL_ADD, pollEvents);

	return 0;
}

int EpollSocketSelector::EPollAsyncSelect(SelectorChannel* socket, int events)
{
	if (socket == NULL || socket->GetFd() <= 0) {
		LOG_D("Invalid socket\r\n");
		return -1;
	}

	socket->fEvents		= events;
	socket->fSelector	= this;

	int fd = socket->GetFd();

	UINT pollEvents	= fEPollMode;

	if (events & OP_ACCEPT) {
		pollEvents |= EPOLLIN;

	} else if (events & OP_CONNECT) {
		pollEvents |= (EPOLLIN | EPOLLOUT);

	}  else if (events & OP_READ) {
		pollEvents |= EPOLLIN;
	} 

	if (events & OP_SEND) {
		pollEvents |= EPOLLOUT;
	} 

	if (GetChannel(fd) == NULL) {
		fMutex.Lock();
		fSockets.Put(fd, socket);
		fMutex.UnLock();

		int ret = EPollControl(fd, EPOLL_CTL_ADD, pollEvents);
		if (ret == EEXIST) {
			ret = EPollControl(fd, EPOLL_CTL_MOD, pollEvents);
		}

		if (ret < 0) {
			LOG_W("epoll control failed: %d\r\n");
			RemoveChannel(socket);
		}
		return ret;

	} else {
		int ret = EPollControl(fd, EPOLL_CTL_MOD, pollEvents);
		if (ret == ENOENT) {
			ret = EPollControl(fd, EPOLL_CTL_ADD, pollEvents);
		}

		if (ret < 0) {
			LOG_W("epoll control failed: %d\r\n");
			RemoveChannel(socket);
		}
		return ret;
	}

	return 0;
}

int EpollSocketSelector::EPollControl( int fd, int op, UINT events )
{
	int ret = -1;

	if (fd <= 0) {
		LOG_D("Invalid fd\r\n");
		return ret;
	}

	// EPOLLIN The associated file is available for read(2) operations.
	// EPOLLOUT The associated file is available for write(2) operations.
	// EPOLLERR Error condition happened on the associated file descriptor. 
	//		epoll_wait(2) will always wait for this event; it is not necessary to set it in events.
	// EPOLLET  Sets the Edge Triggered behavior for the associated file descriptor. 
	//		The default behavior for epoll is Level Triggered. 
	epoll_event event;
	memset(&event, 0, sizeof(event));
	event.data.fd = fd;
	event.events  = events;
	ret = epoll_ctl(fEpollFd, op, fd, &event);

	return ret;
}

/**
 * 处理指定的事件. 
 * @param fd 有 I/O 事件的文件描述符
 * @param events 发生的事件
 */
void EpollSocketSelector::EPollHandleEvent(int fd, UINT events)
{
	if (fd <= 0) {
		return;

	} else if (IsPipe(fd)) {
		ClearPipe();
		return;
	}

	SelectorChannelPtr channel = GetChannel(fd);
	if (channel) {
		EPollHandleEvent(channel, events);
	}
}

/**
 * 处理指定的事件.
 * @param socket 发生事件的 Socket.
 * @param events 发生的事件.
 */
void EpollSocketSelector::EPollHandleEvent(SelectorChannel* channel, UINT events)
{
	if (channel == NULL) {
		return;
	}


	// 发生错误
	if (events & EPOLLERR) {
		RemoveChannel(channel);
		channel->Close();
		channel->OnClose(0);
		return;
	}

	// 可写
	if (events & EPOLLOUT) {

		if (channel->fEvents & OP_CONNECT) {
			int error = -1;
			socklen_t len = sizeof(error);
			getsockopt(channel->GetFd(), SOL_SOCKET, SO_ERROR, (char*)&error, &len);
			channel->OnConnect(error);

		} else if (channel->fEvents & OP_SEND) {
			channel->OnSend(0);

		} else {
			channel->OnSend(0);
		}

	} 

	// 可读
	if (events & EPOLLIN) {

		if (channel->fEvents & OP_ACCEPT) {
			channel->OnAccept(0);

		} else if (channel->fEvents & OP_READ) {
			channel->OnReceive(0);

		} else {
			channel->OnReceive(0);
		}
	} 
}

/** Socket Select Loop */
int EpollSocketSelector::EPollWait(int timeout)
{
	int count = 0;

	// 处理所发生的所有事件
	count = epoll_wait(fEpollFd, fEvents, 20, timeout);
	for (int i = 0; i < count; i++) {
		epoll_event& event = fEvents[i];
		EPollHandleEvent(event.data.fd, event.events);
	}

	return count;
}

int EpollSocketSelector::InitPipe()
{
	SocketSelector::InitPipe();

	EPollAddFileDescripor(fPipe.InPipe());
	return 0;
}

int EpollSocketSelector::Open()
{
	if (fEpollFd > 0) {
		return -1;
	}

	SocketSelector::Open();

	fEpollFd = epoll_create(256);

	return 0;
}

/** 移除指定的 SelectorChannel */
int EpollSocketSelector::RemoveChannel( SelectorChannel* socket )
{
	SocketSelector::RemoveChannel(socket);

	int fd = socket->GetFd();
	if (fd <= 0) {
		return -1;
	}

	return EPollControl(fd, EPOLL_CTL_DEL, EPOLLIN | fEPollMode);
}

/** Socket Select Loop */
int EpollSocketSelector::WaitEvents(int timeout)
{
	return EPollWait(timeout);
}

#endif

SocketSelectorPtr SocketSelectorCreate()
{
#ifdef EPOLL_SUPPORT
	return new EpollSocketSelector();
#else
	return new SocketSelector();
#endif
}

} //
}
