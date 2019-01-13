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
#ifndef _NS_VISION_CORE_UTIL_LOGS_H_
#define _NS_VISION_CORE_UTIL_LOGS_H_

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// LogLevel enum

/** 日志级别. */
enum LogLevel 
{
	kLogLevelDebug		= 1,	///< 调试信息
	kLogLevelInfo		= 2,	///< 普通信息
	kLogLevelWarn		= 3,	///< 警告信息
	kLogLevelError		= 4		///< 出错信息
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Log class

/** 
 * 用于输出日志信息. 
 * API for sending log output.
 * 
 * @author ChengZhen (anyou@msn.com)
 */
class Log
{
public:
	Log();

// Attributes -------------------------------------------------
public:
	static Log& GetLog();
	static void SetLevel(UINT level);

	UINT GetLogLevel();
	UINT GetUpTime();
	void SetLogLevel(UINT level);
	void SetLogToFile(BOOL enabled);

// Operations -------------------------------------------------
public:
	void WriteSysLog(LPCSTR fmt, ...);
	void Write(UINT level, LPCSTR file, int line, LPCSTR func, LPCSTR fmt, ...);

public:
	static int  WriteLog(LPCSTR filename, LPCSTR info, UINT flags = 0);
	static void PrintHex(BYTE* data, UINT length);

// Implementation ---------------------------------------------
private:
	LPCSTR TrimFilename( LPCSTR file, char* path );
	LPCSTR GetLevelTag( UINT level );

// Data Members -----------------------------------------------
private:
	static Log fLogger;		///< 
	time_t	fBaseTime;		///< 
	UINT	fBufferSize;	///< 
	UINT	fLogLevel;		///< 当前日志输出级别
	Mutex	fLogMutex;		///< 
	BOOL	fLogToFile;		///< 
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Log APIs

/** Send an system log message. */
#define LOG_SYS Log::GetLog().WriteSysLog

/** Checks to see whether or not a log is loggable at the specified level. */
#define LOG_IS_DEBUG (Log::GetLog().GetLogLevel() <= kLogLevelDebug)

/** Set the specified log level. */
#define LOG_SET_LEVEL(level, toFile)	\
	Log::GetLog().SetLogLevel(level);	\
	Log::GetLog().SetLogToFile(toFile);

#ifdef _WIN32

/** Send an TRACE log message. */
#define TRACE(fmt, ...) \
	Log::GetLog().Write(kLogLevelInfo,  __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__);

/** Send an ERROR log message. */
#define LOG_E(fmt, ...) \
	Log::GetLog().Write(kLogLevelError, __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__);

/** Send a WARN log message. */
#define LOG_W(fmt, ...) \
	Log::GetLog().Write(kLogLevelWarn,  __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__);

/** Send an INFO log message. */
#define LOG_I(fmt, ...) \
	Log::GetLog().Write(kLogLevelInfo,  __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__);

/** Send a DEBUG log message. */
#define LOG_D(fmt, ...) \
	if (LOG_IS_DEBUG) { \
	Log::GetLog().Write(kLogLevelDebug, __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__); \
	}

#else

/** Send an TRACE log message. */
#define TRACE(fmt, args...) \
	Log::GetLog().Write(kLogLevelInfo,  __FILE__, __LINE__, __FUNCTION__, fmt, ##args);

/** Send an ERROR log message. */
#define LOG_E(fmt, args...) \
	Log::GetLog().Write(kLogLevelError, __FILE__, __LINE__, __FUNCTION__, fmt, ##args);

/** Send a WARN log message. */
#define LOG_W(fmt, args...) \
	Log::GetLog().Write(kLogLevelWarn,  __FILE__, __LINE__, __FUNCTION__, fmt, ##args);

/** Send an INFO log message. */
#define LOG_I(fmt, args...) \
	Log::GetLog().Write(kLogLevelInfo,  __FILE__, __LINE__, __FUNCTION__, fmt, ##args);

/** Send a DEBUG log message. */
#define LOG_D(fmt, args...) \
	if (LOG_IS_DEBUG) { \
	Log::GetLog().Write(kLogLevelDebug, __FILE__, __LINE__, __FUNCTION__, fmt, ##args); \
	}

#endif

#define ASSERT_PARAM(a)  LOG_D("Invalid Parameter: %s", #a)
#define ASSERT_MEMBER(a) LOG_D("Invalid Member: %s", #a)


};
};

#endif // !defined(_NS_VISION_CORE_UTIL_LOGS_H_)
