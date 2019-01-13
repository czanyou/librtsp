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
#ifndef _NS_VISION_RTC_CONTEXT_H
#define _NS_VISION_RTC_CONTEXT_H

#include "core/net/nio_message.h"
#include "core/net/nio_socket.h"

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtcContext enum


/**
 * RTC 上下文
 */
class RtcContext
{
public:
	/** 转发地址 */
	virtual String GetRelayAddress() = 0;

	/** STUN 地址 */
	virtual String GetStunServer() = 0;

	/** UPnP 地址 */
	virtual UINT   GetUpnpPort() = 0;

	virtual SelectorChannelPtr GetCommonTransport() = 0;
};

typedef RtcContext* RtcContextPtr;

#endif // _NS_VISION_RTC_CONTEXT_H

