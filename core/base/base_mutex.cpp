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
#include "base_mutex.h"

namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Mutex class

Mutex::Mutex()
{
	pthread_mutex_init(&fMutex, NULL);
	fCondInited = FALSE;
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&fMutex);

	if (fCondInited) {
		pthread_cond_destroy(&fCond);
	}
}

/** Init pthread cond. */
int Mutex::InitCond()
{
	if (fCondInited) {
		return -1;
	}

	pthread_cond_init(&fCond, NULL);
	fCondInited = TRUE;	
	return 0;
}

/** lock */
int Mutex::Lock()
{
	pthread_mutex_lock(&fMutex);
	return 0;
}

/** notify all waiting thread */
int Mutex::NotifyAll()
{
	if (!fCondInited) {
		return -1;
	}

	return pthread_cond_broadcast(&fCond);
}

/** notify first waiting thread */
int Mutex::Notify()
{
	if (!fCondInited) {
		return -1;
	}

	return pthread_cond_signal(&fCond);
}

/** unlock */
int Mutex::UnLock()
{
	pthread_mutex_unlock(&fMutex);
	return 0;
}

/** wait until another thread send a signal */
int Mutex::Wait()
{
	if (!fCondInited) {
		return -1;
	}

	return pthread_cond_wait(&fCond, &fMutex);
}

/** wait but a short time */
int Mutex::Wait(int timeout)
{
	if (!fCondInited) {
		return -1;
	}

	if (timeout < 0) {
		return pthread_cond_wait(&fCond, &fMutex);
	}

	// now
	struct timeval now;
	gettimeofday(&now, NULL);

	// time out
	timeout = timeout * 1000 + now.tv_usec;
	now.tv_sec += timeout / 1000000;
	now.tv_usec = timeout % 1000000;

	// wait time
	struct timespec abstime;
	memset(&abstime, 0, sizeof(abstime));
	abstime.tv_sec  = now.tv_sec;
	abstime.tv_nsec = now.tv_usec * 1000;

	return pthread_cond_timedwait(&fCond, &fMutex, &abstime);
}

}

