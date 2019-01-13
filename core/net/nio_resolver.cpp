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

#include "nio_resolver.h"  

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// AsyncResolver class

AsyncResolver::AsyncResolver() 
{  
	fStartTime = 0;
}

AsyncResolver::~AsyncResolver() 
{  
}  

void AsyncResolver::Clear()
{
	fMutex.Lock();
	fAddress	= "";
	fDomain		= "";
	fStartTime	= 0;
	fMutex.UnLock();
}

/** 返回解析后的 IP 地址. */
String AsyncResolver::GetResolvedAddress()
{
	fMutex.Lock();
	String address = fAddress;
	fMutex.UnLock();
	
	return address;
}

/** 返回开始解析的时间戳. */
INT64 AsyncResolver::GetStartTime()
{
	return fStartTime;
}

/** 指出指定的地址是否是一个有效的 IPv4 地址. */
BOOL AsyncResolver::IsIPv4Address( LPCSTR address )
{
	if (isempty(address)) {
		return FALSE;
	}

	int a = 0, b = 0, c = 0, d = 0;
	int ret = sscanf(address, "%d.%d.%d.%d", &a, &b, &c, &d);
	if (ret != 4) {
		return FALSE;

	} else if (a < 0 || a > 255) {
		return FALSE;

	} else if (b < 0 || b > 255) {
		return FALSE;

	} else if (c < 0 || c > 255) {
		return FALSE;

	} else if (d < 0 || d > 255) {
		return FALSE;
	}

	return TRUE;
}

int AsyncResolver::Run()
{
	//LOG_W("StartResolver\r\n");

	fMutex.Lock();
	String domain = fDomain;
	fMutex.UnLock();

#ifdef HI3518
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	char* hostAddress = NULL;

	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int dwRetval = getaddrinfo(domain, 0, &hints, &result);
	if (dwRetval == 0) {
		if (result->ai_family == AF_INET) {
			sockaddr_in* sockaddr = (struct sockaddr_in *) result->ai_addr;
			hostAddress = inet_ntoa(sockaddr->sin_addr);
		}
	}

#else
	struct hostent *host = gethostbyname(domain);
	if (host == NULL) {
		LOG_D("Invalid host address.\n");
		return FALSE;
	}

	char* hostAddress = inet_ntoa(*(struct in_addr*)host->h_addr);
	if (hostAddress == NULL) {
		LOG_D("Invalid host address.\n");
		return FALSE;
	}

#endif

	fMutex.Lock();
	fAddress = hostAddress;
	fMutex.UnLock();

	return 0;
}

/** 
 * 开始解析.
 * @param domain 要解析的域名
 */
BOOL AsyncResolver::StartResolving( LPCSTR domain )
{
	if (IsIPv4Address(domain)) {
		fAddress = domain;
		return TRUE;
	}

	if (isempty(domain)) {
		return FALSE;

	} else if (IsThreadAlive()) {
		return FALSE;
	}

	fMutex.Lock();
	fDomain = domain;
	fMutex.UnLock();

	fMutex.Lock();
	fStartTime = GetSysTickCount();
	fMutex.UnLock();


	if (fDomain == "localhost") {
		fAddress = "127.0.0.1";
		return TRUE;
	}

	SetThreadName("AsyncResolver");
	StartThread();
	return TRUE;
}

} //
}
