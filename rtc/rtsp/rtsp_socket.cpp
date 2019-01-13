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

#include "rtsp_socket.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSocket class

RtspSocket::RtspSocket(void)
{
	fLastActiveTime		= 0;
	fListener			= NULL;
	fReceiveTimes		= 0;
}

RtspSocket::~RtspSocket(void)
{
	
}

void RtspSocket::Close()
{
	Socket::Close();

	fListener = NULL;
	fRtspReader.ClearAll();
	fRtspWriter.ClearAll();
}

void RtspSocket::DispatchMessage()
{
	SetLastActiveTime();

	// 解析并处理收到的消息 ------------------
	while (TRUE) {
		if (fRtspReader.IsRtpPacket()) {
			char* packet = fRtspReader.GetNextPacket();
			if (packet == NULL) {
				break;
			}

			size_t packetSize = MAKEWORD(packet[3], packet[2]);
			HandleRtpPacket(packet + 4, packetSize);

		} else {
			RtspMessagePtr message = fRtspReader.GetNextMessage();
			if (message == NULL) {
				break;
			}

			HandleMessage(message);
		}
	}
}

/** 返回这个连接最后活跃的时间. */
time_t RtspSocket::GetLastActiveTime()
{
	return fLastActiveTime;
}

void RtspSocket::HandleMessage( RtspMessage* message )
{
	if (fListener) {
		fListener->OnRtspMessage(message);
	}
}

void RtspSocket::HandleRtpPacket( char* packet, size_t buflen )
{
	if (fListener) {
		fListener->OnRtspPacket(packet, buflen);
	}
}

void RtspSocket::OnClose( int errorCode )
{
	fListener = NULL;
	fRtspReader.ClearAll();
	fRtspWriter.ClearAll();

	LOG_D("%d %x", GetFd(), fEvents);
}

void RtspSocket::OnConnect( int errorCode )
{
	SetLastActiveTime();

	if (fListener) {
		fListener->OnRtspConnect(errorCode);
	}
}

int RtspSocket::OnReadMessages()
{
	UINT freeSize = 0;
	BYTE* freeBuffer = fRtspReader.LockBuffer(freeSize);
	if (freeBuffer == NULL || freeSize <= 0) {
		return -1;
	}

	int ret = 0;
	while (TRUE) {
		ret = Receive((char*)freeBuffer, freeSize);
		if (ret > 0) {
			fRtspReader.UnlockBuffer(ret);
			return ret;
		}

		// LOG_D("Error: %d %s\r\n", errno, strerror(errno));
		if (errno == EINTR) {
			break;

		} else if (errno == EWOULDBLOCK) {
#ifdef _linux
			struct tcp_info info; 
			int len = sizeof(info); 
			getsockopt(socket.GetFd(), IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len); 
			if ((info.tcpi_state != TCP_ESTABLISHED)) { 
				socket.Close();
			}
#else
			//LOG_D("Error: %d %s\r\n", errno, strerror(errno));
#endif
			break;

		} else if (errno == 0) {
			LOG_D("Error: %d %s\r\n", errno, strerror(errno));
			break;

		} else {
			LOG_D("Error: %d %s\r\n", errno, strerror(errno));
			Close();
			break;
		}
	
		break;
	}

	fRtspReader.UnlockBuffer(0);
	return ret;
}

/** Called by framework when have data arrived. */
void RtspSocket::OnReceive(int errorCode)
{
	// 当这个方法被反复调用又没有数据可读, 则可能是 Socket 关闭了.
	fReceiveTimes++;
	if (fReceiveTimes > 15) {
		LOG_W("ReceiveTimes: %d\r\n", fReceiveTimes);
		Close();
		return;
	}

	// 
	INT64 startTime = GetSysTickCount();
	while (sockfd > 0) {
		if (OnReadMessages() <= 0) {
			break;
		}

		DispatchMessage();
		fReceiveTimes = 0;

		// 设置超时, 防止一直读数据, 使无法处理其他通道
		INT64 now = GetSysTickCount();
		UINT span = (UINT)(now - startTime);
		if (span >= 100) {
			break;
		}
	}
}

void RtspSocket::OnSend( int errorCode )
{
	// 如果发送缓存区为空
	if (fListener) {
		fListener->OnRtspSend(errorCode);
	}
}

int RtspSocket::OnSendBufferData() 
{
	// 发送缓存区中的数据
	BOOL isEmptyBuffer = FALSE;
	fRtspWriter.OnSend(*this, isEmptyBuffer);
	return isEmptyBuffer;
}

void RtspSocket::Reset()
{
	if (sockfd > 0) {
		Close();
	}
	sockfd = -1;

	fRtspReader.ClearAll();
	fRtspWriter.ClearAll();

	fReceiveTimes		= 0;
	fLastActiveTime		= 0;
}

void RtspSocket::ResetWriter()
{
}

/** 发送指定的消息. */
int RtspSocket::SendMessage(LPCSTR message, size_t length)
{
	return fRtspWriter.WriteMessage(message, length);
}

/** 发送消息内容. */
int RtspSocket::SendMessageContent( IMediaSample* mediaSample )
{
	if (mediaSample == NULL) {
		return 0;
	} 

	return fRtspWriter.WriteSample(mediaSample);
}

/** 设置最后活跃时间. */
void RtspSocket::SetLastActiveTime()
{
	fLastActiveTime = time(NULL);
}

/** 设置事件侦听器. */
void RtspSocket::SetListener( RtspSocketListener* listener )
{
	fListener = listener;
}

}
