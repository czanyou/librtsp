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
#ifndef _NS_VISION_CORE_OS_THREAD_H
#define _NS_VISION_CORE_OS_THREAD_H

#include "core/base/base_ring.h"

namespace core {
namespace os {

#ifdef _WIN32
	typedef int				ThreadId;
	typedef int				ThreadKey;

#else
	typedef pthread_t		ThreadId;
	typedef pthread_key_t	ThreadKey;
#endif

class EventObject;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// EventHandler class

/**
 * 事件处理器.
 */
class EventHandler : public Object
{
public:
	virtual void HandleEvent(EventObject* event) = 0;
};

typedef SmartPtr<EventHandler> EventHandlerPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Event class

/**
 * 代表一个事件.
 */
class EventObject : public Object
{
public:
	EventObject();
	EventObject(const EventObject& event);
	EventObject(UINT type, UINT param1, UINT param2);
	EventObject(UINT type, UINT param1, UINT param2, Object* source, EventHandler* target);

// Attributes -------------------------------------------------
public:
	ObjectPtr GetSourceObject();
	EventHandlerPtr GetTargetObject();

	UINT GetParam1();
	UINT GetParam2();
	UINT GetType();

// Data Members -----------------------------------------------
public:
	ObjectPtr fSourceObject;	///< 事件源
	EventHandlerPtr fTargetObject;	///< 事件目标

	UINT fType;		///< 事件类型
	UINT fParam1;	///< 事件参数
	UINT fParam2;	///< 事件参数
};

/** Event 智能指针类型. */
typedef SmartPtr<EventObject> EventObjectPtr;

/** Event 智能指针类型列表. */
typedef RingList<EventObjectPtr, EventObject*> EventQueue;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// EventLooper class

/**
 * 管理事件队列.
 */
class EventLooper
{
public:
	EventLooper();
	~EventLooper();

public:
	EventObjectPtr GetNextEvent();
	EventObjectPtr WaitNextEvent(int timeout = -1);

public:
	void ClearEvents();
	int  FireEvent(EventObject* event);
	int  FireEvent(UINT type, UINT param1, UINT param2);
	void PushEvent(EventObject* event);
	void Wakeup();

// Data Members -----------------------------------------------
private:
	ThreadId    fOwnerThreadId;	///< 当前线程的 ID.
	EventQueue	fEventQueue;	///< 要处理的事件队列
	Mutex		fMutex;			///< 互斥锁
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Thread class

/** 
 * 代表一个操作系统线程对象. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Thread
{
public:
	Thread();
	virtual ~Thread();

// Attributes -------------------------------------------------
public:
	EventLooper& GetEventLooper();	///< 
	ThreadId GetThreadId();			///< 返回这个线程对象的 ID.
	LPCSTR GetThreadName();			///< 返回这个线程对象的名称.
	BOOL IsThreadAlive();
	void SetThreadName(LPCSTR name);
	void SendEvent(EventObject* event);
	void SendEvent(UINT type, UINT param1, UINT param2, Object* source, EventHandler* target = NULL);

// Operations -------------------------------------------------
public:
	virtual int  ThreadJoin();
	virtual int  StartThread();
	virtual int  Run();			///< 子类通过重写这个方法实现线程过程
	virtual void DispatchEvent(EventObject* event);
	virtual void DispatchEvents();
	virtual void PostEvent(EventObject* event);

public:
	static ThreadId GetCurrentThreadId();

// Implementation ---------------------------------------------
private:
	static void* ThreadProc(void* param);

// Data Members -----------------------------------------------
protected:
	EventLooper fEventLooper; ///< 
	ThreadId fThreadId;		///< 当前线程的 ID.
	char fName[MAX_PATH];	///< 当前线程的名称.
};

}; // namespace os
}; // namespace core

#endif // !defined(_NS_VISION_CORE_OS_THREAD_H)
