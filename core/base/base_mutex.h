/***
 * The content of this file or document is CONFIDENTIAL and PROPRIETARY
 * to ChengZhen(Anyou).  It is subject to the terms of a
 * License Agreement between Licensee and ChengZhen(Anyou).
 * restricting among other things, the use, reproduction, distribution
 * and transfer.  Each of the embodiments, including this information and
 * any derivative work shall retain this copyright notice.
 *
 * Copyright (c) 2014-2015 ChengZhen(Anyou). All Rights Reserved.
 */
#ifndef _NS_VISION_CORE_BASE_MUTEX_H
#define _NS_VISION_CORE_BASE_MUTEX_H

#include "base_types.h"

namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Mutex class

/** 
 * 代表了一个操作系统级别的互斥锁对象.
 * POSIX thread mutex and cond wrapper class 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Mutex  
{
// Construction/Destruction -----------------------------------
public:
	Mutex();
	virtual ~Mutex();

// Operations -------------------------------------------------
public:
	int InitCond();
	int Lock();
	int Notify();
	int NotifyAll();
	int UnLock();
	int Wait();
	int Wait(int timeout);

// Data members -----------------------------------------------
private:
	pthread_mutex_t	fMutex;	///< mutex 
	pthread_cond_t	fCond;	///< cond
	BOOL fCondInited;		///< 指出是否已经初始化了 cond.
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MutexLocker class

/**
 * 自动锁. 
 * The MutexLocker class is a convenience class that simplifies locking and 
 * unlocking mutexes.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MutexLocker
{
public:
	MutexLocker(Mutex* mutex) { 
		fMutex = mutex; 
		if (fMutex) { 
			fMutex->Lock(); 
		} 
	}

	~MutexLocker() { 
		if (fMutex) { 
			fMutex->UnLock(); 
		} 
	}

// Data members -----------------------------------------------
private:
	Mutex* fMutex;
};

}

#endif // _NS_VISION_CORE_BASE_MUTEX_H
