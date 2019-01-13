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
#ifndef _NS_VISION_CORE_BASE_OBJECT_H
#define _NS_VISION_CORE_BASE_OBJECT_H

#include "base_mutex.h"
#include "base_ptr.h"

namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Object class

#define VISION_DEC_CLASS_RUNTIME(CLASS, PARENT)	\
	virtual LPCSTR _GetClassName() { return #CLASS; }


/** 
 * Object 定义了引用计数等基本功能的基本类对象.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Object
{
public:
	Object() : fRefcount(0) {}
	virtual ~Object() {}

	VISION_DEC_CLASS_RUNTIME(Object, Object);

public:
	/** 递增引用计数. */
	virtual long AddRef() {
		fMutex.Lock();
		int ref = ++fRefcount;
		fMutex.UnLock();
		return ref;
	}

	/** 递减引用计数, 当引用计数为 0 时自动删除当前对象. */
	virtual long Release() {
		fMutex.Lock();
		long res = --fRefcount;
		fMutex.UnLock();
		if (res == 0) {
			delete this;
		}
		return res;
	}

// Data Members -----------------------------------------------
private:
	long	fRefcount;	///< 引用计数
	Mutex	fMutex;		///< Mutex
};

/** Object 智能指针类型. */
typedef SmartPtr<Object> ObjectPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseObject class

/** 
 * 用于实现基本的引用计数功能. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class BaseObject
{
public:
    BaseObject() {
        fRefCount = 0;
    }

    virtual ~BaseObject() {
    }

// Operations -------------------------------------------------
public:
	/** 递增引用计数. */
    virtual ULONG BaseAddRef() {
		fMutex.Lock();
        ULONG refCount = ++fRefCount;
        fMutex.UnLock();
        return refCount;
    }

	/** 递减引用计数. */
    virtual ULONG BaseRelease() {
        fMutex.Lock();
        ULONG refCount = --fRefCount;
        fMutex.UnLock();
        if (refCount == 0) {
            delete this;
        }
		return refCount;
    }

// Data members -----------------------------------------------
private:
    volatile ULONG fRefCount;
	Mutex fMutex;
};

#define VISION_BASEREF_METHODS(theClass)						\
	ULONG AddRef()  { return BaseObject::BaseAddRef();  }		\
	ULONG Release() { return BaseObject::BaseRelease(); }		\
	LPCSTR _GetClassName() { return #theClass; }			

}; // namespace

#endif //_NS_VISION_CORE_BASE_OBJECT_H
