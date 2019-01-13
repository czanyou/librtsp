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

#include "utils.h"
#include "debug.h"
#include "string_utils.h"


namespace core {
namespace util {

#ifdef __linux

void VisionDefaultSignals(sighandler_t handler)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT,  handler);
	signal(SIGILL,  handler);
	signal(SIGABRT, handler);
	signal(SIGSEGV, handler);
	signal(SIGTERM, handler);
	signal(SIGQUIT, handler); 
	signal(SIGKILL, handler);
	signal(SIGSTOP, handler);
	signal(SIGALRM, handler);
	signal(SIGUSR1, handler);
	signal(SIGUSR2, handler);
	signal(SIGBUS,  handler);
	signal(SIGHUP,  handler);
}
#else 
void VisionDefaultSignals(sighandler_t handler)
{
	
}
#endif

/** 返回当前系统时钟嘀嗒数, 单位为毫秒. */
INT64 GetSysTickCount()
{
	INT64 mtime = 0;

#if _WIN32
	mtime = GetTickCount();

#elif __linux
	mtime = times(NULL);
	mtime *= 10;
	mtime += 1000L * 3600L * 24L;

#else
	struct timeval current;
	gettimeofday(&current, NULL);
	mtime = current.tv_sec * 1000 + current.tv_usec / 1000;

	static INT64 startMtime = 0;
	if (startMtime == 0) {
		startMtime = mtime;
	}

	mtime -= startMtime;

#endif
	return mtime;
}

};
};
