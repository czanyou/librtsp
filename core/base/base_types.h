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
#ifndef _NS_VISION_CORE_BASE_TYPES_H
#define _NS_VISION_CORE_BASE_TYPES_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32

#include <WinSock2.h>

#else

#include <arpa/inet.h>		// inet_addr,inet_aton
#include <dirent.h>
#include <pthread.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <unistd.h>

#if __linux
#include <linux/soundcard.h>
#include <net/if_arp.h>
#include <sys/epoll.h>
#include <sys/reboot.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <mntent.h>
#endif

#endif

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// API

#if defined(_WIN32)

#define closesocket			close
#define epoll_event			int
#define pthread_cond_t		int
#define pthread_mutex_t		int
#define sighandler_t		int
#define snprintf			_snprintf
#define strncasecmp			_strnicmp


#elif __linux

#define closesocket close

#elif __IPHONE_OS_VERSION_MAX_ALLOWED
#define __iphone_os			1
#define epoll_event			int
#define closesocket			close

#elif defined(__APPLE__)
#define __mac_os			1
#define epoll_event			int
#define closesocket			close
#define NO_SERIAL_COMM		1

typedef void (*sighandler_t)(int value);

#else 

#endif

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Debug APIs 

#if 1
#define ASSERT(x) if (!(x)) { LOG_D("!ASSERT: (%s). (%s)\r\n", #x, __FILE__); }
#endif

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Types (platform dependent)
// 定义 WINAPI 风格的数据类型
#ifndef _WIN32

typedef unsigned long long	QWORD;	///< 64bit uint
typedef unsigned int        DWORD;	///< 32bit uint
typedef unsigned short      WORD;	///< 16bit uint
typedef unsigned char       BYTE;	///< 8bit uint

typedef unsigned long		ULONG;
typedef unsigned int        UINT;	///< 32bit uint

typedef signed int          INT;	///< 32bit int
typedef signed long long	INT64;	///< 64bit int

typedef const char *		LPCSTR;
typedef char *				LPSTR;

#ifdef __linux
typedef int                 BOOL;

#else
typedef signed char         BOOL;
#endif

#endif

typedef struct timeval		TIMEVAL;

#ifdef __cplusplus
typedef wchar_t				WCHAR;
typedef const wchar_t *		LPCWSTR;		
typedef wchar_t *			LPWSTR;	
#endif

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Quick functions

#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD) ((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)((w) & 0xFF))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef RANGE
#define RANGE(p, min, max) ((p) < (min) ? (min) : ((p) > (max) ? (max) : (p) ))
#endif

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Constants

#ifndef FALSE
#  define FALSE             0
#endif

#ifndef TRUE
#  define TRUE              1
#endif

#define kInvalidSocket		-1
#define kSocketError		-1

#ifndef IN
#  define IN
#endif

#ifndef OUT
#  define OUT
#endif

#ifndef MAX_PATH
#  define MAX_PATH			256
#endif

#endif // !defined(_NS_VISION_CORE_BASE_TYPES_H)
