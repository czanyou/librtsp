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
#ifndef _NS_VISION_CORE_NIO_RESOLVER_H  
#define _NS_VISION_CORE_NIO_RESOLVER_H  

#include "core/os/thread.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// AsyncResolver class

/**
 * 异步域名解析类.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class AsyncResolver : public Thread
{  
public:  
	AsyncResolver();
	virtual ~AsyncResolver();

// Attributes -------------------------------------------------
public:  
	String GetResolvedAddress();
	INT64  GetStartTime();

// Operations -------------------------------------------------
public:  
	BOOL IsIPv4Address(LPCSTR address);
	BOOL StartResolving(LPCSTR domain);
	void Clear();

private:
	int Run();

// Data Members -----------------------------------------------
private:
	String fDomain;			///< 要解析的域名
	String fAddress;		///< 要解析的地址
	INT64  fStartTime;		///< 开始解析的时间
	Mutex  fMutex;			///< Mutex
}; 

} //
}

#endif /* _NS_VISION_CORE_NIO_RESOLVER_H */  
