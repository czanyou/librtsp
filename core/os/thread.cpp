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

#include "core/os/thread.h"

namespace core {
namespace os {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Event class

EventObject::EventObject()
{
	fType		= 0;
	fParam1		= 0;
	fParam2		= 0;
}

EventObject::EventObject( const EventObject& event )
{
	fType		= event.fType;
	fParam1		= event.fParam1;
	fParam2		= event.fParam2;
}

EventObject::EventObject( UINT t, UINT p1, UINT p2 )
{
	fType		= t;
	fParam1		= p1;
	fParam2		= p2;
}

EventObject::EventObject( UINT t, UINT p1, UINT p2, Object* sourceObject, EventHandler* targetObject )
{
	fType		= t;
	fParam1		= p1;
	fParam2		= p2;

	fSourceObject 	= sourceObject;
	fTargetObject	= targetObject;
}

UINT EventObject::GetParam1()
{
	return fParam1;
}

UINT EventObject::GetParam2()
{
	return fParam2;
}

ObjectPtr EventObject::GetSourceObject()
{
	return fSourceObject;
}

EventHandlerPtr EventObject::GetTargetObject()
{
	return fTargetObject;
}

UINT EventObject::GetType()
{
	return fType;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// EventManager class

EventLooper::EventLooper()
{
	fMutex.InitCond();
	fEventQueue.Resize(100, NULL);
	fOwnerThreadId	= 0;
}

EventLooper::~EventLooper()
{
}

void EventLooper::ClearEvents()
{
	fMutex.Lock();
	fEventQueue.Clear();
	fMutex.UnLock();
}

int EventLooper::FireEvent( EventObject* event )
{
	if (event == NULL) {
		return -1;
	}

	int ret = 0;
	PushEvent(event);
	return ret;
}

int EventLooper::FireEvent( UINT type, UINT param1, UINT param2 )
{
	EventObjectPtr event = new EventObject(type, param1, param2);
	return FireEvent(event);
}

EventObjectPtr EventLooper::GetNextEvent()
{
	EventObjectPtr event;
	fMutex.Lock();
	if (!fEventQueue.IsEmpty()) {
		event = fEventQueue.GetTail();
		fEventQueue.RemoveTail();
	}
	fMutex.UnLock();
	return event;
}

void EventLooper::PushEvent( EventObject* event )
{
	if (event == NULL) {
		return;
	}

	fMutex.Lock();
	if (fEventQueue.IsFull()) {
		fEventQueue.RemoveTail();
	}

	if (!fEventQueue.IsFull()) {
		fEventQueue.AddHead(event);
	}

	fMutex.Notify();
	fMutex.UnLock();
}

/** 等待下一个事件. */
EventObjectPtr EventLooper::WaitNextEvent(int timeout)
{
	EventObjectPtr event;
	fMutex.Lock();

	if (timeout < 0) {
		while (event == NULL) {
			if (!fEventQueue.IsEmpty()) {
				event = fEventQueue.GetTail();
				fEventQueue.RemoveTail();
				break;
			}

			fMutex.Wait();
		}

	} else {
		if (fEventQueue.IsEmpty()) {
			fMutex.Wait(timeout);
		}

		if (!fEventQueue.IsEmpty()) {
			event = fEventQueue.GetTail();
			fEventQueue.RemoveTail();
		}
	}

	fMutex.UnLock();
	return event;
}

void EventLooper::Wakeup()
{
	fMutex.Lock();
	fMutex.Notify();
	fMutex.UnLock();
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Thread class

Thread::Thread()
{
	fThreadId 	= 0;
	memset(fName, 0, sizeof(fName));
}

Thread::~Thread()
{

}

void Thread::DispatchEvent( EventObject* event )
{
	if (event == NULL) {
		return;
	}

	EventHandlerPtr handler = event->GetTargetObject();
	if (handler) {
		handler->HandleEvent(event);
	}
}

void Thread::DispatchEvents()
{
	while (TRUE) {
		EventObjectPtr event = fEventLooper.GetNextEvent();
		if (event == NULL) {
			break;
		}

		DispatchEvent(event);
	}
}

/** 返回当前正在执行线程的 ID. */
ThreadId Thread::GetCurrentThreadId()
{
	return pthread_self();
}

EventLooper& Thread::GetEventLooper()
{
	return fEventLooper;
}

LPCSTR Thread::GetThreadName()
{
	return fName;
}

ThreadId Thread::GetThreadId()
{
	return fThreadId;
}

/** 测试线程是否处于活动状态。*/
BOOL Thread::IsThreadAlive()
{
	if (fThreadId > 0) {
		return TRUE;
	}
	return FALSE;
}

void Thread::PostEvent( EventObject* event )
{
	fEventLooper.PushEvent(event);
}

void Thread::SendEvent( EventObject* event )
{
	ThreadId pid = GetCurrentThreadId();
	if (pid == fThreadId) {
		DispatchEvent(event);

	} else {
		PostEvent(event);
	}
}

void Thread::SendEvent( UINT type, UINT param1, UINT param2, Object* source, EventHandler* target )
{
	SendEvent(new EventObject(type, param1, param2, source, target));
}

/**
 * 设置这个线程的名称.
 * @param name 名称
 */
void Thread::SetThreadName( LPCSTR name )
{
	if (name == NULL) {
		name = "";
	}

	strncpy(fName, name, MAX_PATH - 1);
}

/** 启动这个线程. */
int Thread::StartThread()
{
	if (fThreadId == 0) {
		pthread_create(&fThreadId, NULL, ThreadProc, (void*)this);
	}
	return 0;
}

/** 等待该线程终止. */
int Thread::ThreadJoin()
{
	if (fThreadId > 0) {
		pthread_join(fThreadId, NULL);
	}
	return 0;
}

/** 这个线程的过程. */
void* Thread::ThreadProc( void* param )
{
	Thread* thread = (Thread*)param;
	if (thread) {
		int pid = getpid();
		int tid = (long)GetCurrentThreadId();
		LPCSTR name = thread->fName;

		LOG_D("[%d:%x] Enter Thread '%s' >>>\r\n", pid, tid, name);
		thread->Run();
		thread->fThreadId = 0;
		LOG_D("[%d:%x] Exit Thread '%s' <<<\r\n",  pid, tid, name);
	}

	return NULL;
}

int Thread::Run()
{
	return 0;
}


}; // namespace os
}; // namespace core
