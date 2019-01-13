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
#ifndef _NS_VISION_CORE_NIO_CHANNEL_H
#define _NS_VISION_CORE_NIO_CHANNEL_H

#include "nio_network.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Pipe class 

/** 
 * 代表一对管道. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class SocketPipe
{
public:
	SocketPipe();
	~SocketPipe();

public:
	int& InPipe();
	int& OutPipe();

public:
	int Open();
	int Close();

// Data Members -----------------------------------------------
private:
	int fPipes[2];	///< 管道

};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Selector class 

/** 选择器操作类型. */
enum SelectorOperate 
{
	OP_ACCEPT	= 0x01,
	OP_CONNECT	= 0x02,
	OP_READ		= 0x04,
	OP_SEND		= 0x08
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SelectorChannel class 

class SocketSelector;

/**
 * 代表一个选择器通道.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class SelectorChannel : public Object
{
public:
	SelectorChannel() {
		fEvents		= 0;
		fSelector	= NULL;
	}

	virtual int  GetFd() = 0;
	virtual void Close() = 0;
	virtual void OnConnect(int errorCode) {}
	virtual void OnReceive(int errorCode) {}
	virtual void OnAccept (int errorCode) {}
	virtual void OnClose  (int errorCode) {}
	virtual void OnSend   (int errorCode) {}

public:
	virtual int  SendTo (LPCSTR buf, int size, LPCSTR socketAddress, 
		UINT  socketPort, int flags = 0)  { return 0; }
	virtual int  ReceiveFrom(LPCSTR buf, int size, LPSTR  socketAddress, 
		UINT& socketPort, int flags = 0)  { return 0; }

	virtual int  Send   (LPCSTR buf, int size, int flags = 0)  { return 0; }
	virtual int  Receive(LPSTR  buf, int size, int flags = 0)  { return 0; }

public:
	SocketSelector* fSelector;	///< 多路选择器
	int fEvents;	///< 当前异步等待的事件
};

typedef SmartPtr<SelectorChannel> SelectorChannelPtr;


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SocketPacket class

/**
 * 代表一个 Socket 用于接收或者发送的数据包.
 *
 */
class SocketPacket : public Object
{
public:
	SocketPacket() {
		fPacketData		= NULL;
		fPacketLength	= 0;
		fRelayed		= 0;
		fSourcePort		= 0;
	}

// Data members -----------------------------------------------
public:
	SelectorChannelPtr fTransport;	///< 相关的 Socket 通道
	SocketAddress fRelayAddress;	///< 相关的转发地址
	String fSourceAddress;			///< 这个数据包的源地址

	BYTE* fPacketData;		///< 这个数据包的内容
	UINT  fPacketLength;	///< 这个数据包的长度
	UINT  fRelayed;			///< 指出这个数据包是否是通过转发的
	UINT  fSourcePort;		///< 这个数据包的源地址端口
};

typedef SmartPtr<SocketPacket> SocketPacketPtr;


} //
}

using namespace core::net;

#endif // _NS_VISION_CORE_NIO_CHANNEL_H
