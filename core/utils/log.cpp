/***
 * The content of this file or document is CONFIDENTIAL and PROPRIETARY
 * to NewStream Technologies.  It is subject to the terms of a
 * License Agreement between Licensee and NewStream Technologies.
 * restricting among other things, the use, reproduction, distribution
 * and transfer.  Each of the embodiments, including this information and
 * any derivative work shall retain this copyright notice.
 *
 * Copyright (c) 2005-2013 NewStream Technologies. All Rights Reserved.
 *
 */
#include "stdafx.h"

#include "log.h"
#include "core/os/thread.h"

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Logger class
  
Log Log::fLogger;

Log::Log()
{
	fBaseTime		= time(NULL);
	fBufferSize		= 1524;				///< LOG 缓存区长度, 长于这个大小的信息将不会显示.
	fLogLevel		= kLogLevelDebug;	///< 默认只显示 DEBUG 级别以上的信息
	//fLogLevel		= kLogLevelInfo;	///< 默认只显示 INFO 级别以上的信息
	fLogToFile		= TRUE;
}

LPCSTR Log::GetLevelTag( UINT level )
{
#ifdef __iphone_os
	return "";
#endif

	LPCSTR tag = "";
	if (level == kLogLevelError) {
		tag = "[1;31m";
	} else if (level == kLogLevelWarn) {
		tag = "[1;35m";
	} else if (level == kLogLevelInfo) {
		tag = "[1;34m";
	}

	return tag;
}

Log& Log::GetLog()
{
	return fLogger;
}

/** 当前设置的日志信息输出级别. */
UINT Log::GetLogLevel()
{
	return fLogLevel;
}

UINT Log::GetUpTime()
{
#ifdef __linux
	return (UINT)System::GetUpTime();

#else 
	return UINT(GetSysTickCount() / 1000);
#endif
}

void Log::PrintHex( BYTE* data, UINT length )
{
	if (data == NULL || length == 0) {
		return;
	}

	for (UINT i = 0; i < length; i++) {
		if (i % 24 == 23) {
			printf("%02X\r\n", data[i]);

		} else if (i % 8 == 7) {
			printf("%02X  ", data[i]);

		} else {
			printf("%02X ", data[i]);
		}
	}

	printf("\r\n");
}

/** 设置日志级别, 低于指定级别的日志信息, 将被忽略. */
void Log::SetLevel(UINT level)
{
	GetLog().SetLogLevel(level);
}

void Log::SetLogLevel( UINT level )
{
	fLogLevel = level;
}

/** 设置是否将日志输出到硬盘中. */
void Log::SetLogToFile( BOOL enabled )
{
	fLogToFile = enabled;
}

LPCSTR Log::TrimFilename( LPCSTR file, char* path )
{
	if (file) {
		LPCSTR p = strrchr(file, '/');
		if (p) {
			file = p + 1;
		}

		p = strchr(file, '.');
		int size = p ? p - file : MAX_PATH;
		strncpy(path, file, size);
	}	return file;
}

/** 
 * 输出指定的日志信息, 结果直接打印在控制台.
 * @param level 日志级别
 * @param file 所在文件
 * @param line 源代码所在行
 * @param function 所在函数
 * @param fmt ... 要输出的内容.
 */
void Log::Write( UINT level, LPCSTR file, int line, LPCSTR function, LPCSTR fmt, ... )
{
	if (fLogLevel > level) {
		return;
	}
	
	// Path
	char path[MAX_PATH];
	memset(path, 0, sizeof(path));
	TrimFilename(file, path);

	// Prefix
	char buf[fBufferSize + 255];
	memset(buf, 0, sizeof(buf));

	UINT   upTime	= GetUpTime();
	LPCSTR tag		= GetLevelTag(level);
	LPCSTR end		= isempty(tag) ? "" : "[0;39m";

	int offset = snprintf(buf, MAX_PATH, "%4u# %s", upTime, tag);
	//fputs(prefix, stderr);

	// Info
	va_list ap;

	// Print log to string ...
	va_start(ap, fmt);
	int size = vsnprintf(buf + offset, fBufferSize, fmt, ap);
	va_end(ap);

	// Line end
	char* p = buf + offset + size - 1;
	while (*p == '\r' || *p == '\n') {
		size--;
		p--;
	};
	
	snprintf(buf + offset + size, fBufferSize - size, "%s (%s:%d/%s)\n", end, path, line, function);

	fLogMutex.Lock();

	fputs(buf, stderr);
	fLogMutex.UnLock();

#ifdef __linux
	if (level > kLogLevelDebug) {
		snprintf(buf + size, fBufferSize - size, "(%s:%d)\n", path, line);
		syslog(LOG_INFO, "%s", buf);
	}
#endif
}

/**
 * 写日志.
 * @param filename 要输出日志的文件
 * @param info 要输出的日志信息
 * @param flags 输出标记
 */
int Log::WriteLog( LPCSTR filename, LPCSTR info, UINT flags )
{
	if (isempty(filename) || isempty(info)) {
		return 0;
	}

	return 0;
}

/** 输出指定的信息到 Linux 系统日志文件. */
void Log::WriteSysLog(LPCSTR fmt, ...)
{
#ifndef NO_SYSLOG
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsyslog(LOG_INFO | LOG_USER, fmt, arg_ptr);
	va_end(arg_ptr); 
#endif

}

};
};
