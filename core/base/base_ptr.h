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
#ifndef _NS_VISION_CORE_BASE_PTR_H
#define _NS_VISION_CORE_BASE_PTR_H

#include "base_types.h"

namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SmartPtr class

/** 
 * SmartPtr 代表一个智能指针.
 * 这个类可以自动管理相应对象的引用计数, 它需要对象本身提供 AddRef 和 Release 
 * 这两个方法. 
 */
template <class T> 
class SmartPtr
{
public:
	/** 构建一个智能指针对象. */
	SmartPtr() : ptr(NULL) {}

	/** 通过指定的指针构建一个智能指针对象. */
	SmartPtr(const SmartPtr<T>& p) : ptr(p.ptr) {if (ptr) ptr->AddRef(); }

	/** 通过指定的指针构建一个智能指针对象. */
	SmartPtr(T* p) : ptr(p) { if (ptr) ptr->AddRef(); }
	virtual ~SmartPtr() { operator =(NULL); }

// Operations -------------------------------------------------
public:
	/** 重载 = 操作符 */
	T* operator =(const SmartPtr<T>& p) {
		operator = (p.ptr);
		return ptr;
	}

	/** 重载 = 操作符 */
	T* operator =(T* p);

	/** 返回对象的指针. */
	operator T* () { return ptr; }

	/** 返回对象的指针. */
	T* get() { return ptr; }
	
	/** 重载 == 操作符 */
	BOOL operator == (const SmartPtr<T>& p) const { return (ptr == p.ptr); }

	/** 重载 != 操作符 */
	BOOL operator != (const SmartPtr<T>& p) const { return (ptr != p.ptr); }

	/** 重载 == 操作符 */
	BOOL operator == (const T* t) const { return (ptr == t); }

	/** 重载 != 操作符 */
	BOOL operator != (const T* t) const { return (ptr != t); }

	/** 重载 ! 操作符 */
	BOOL operator ! () const { return (ptr == NULL); }

	/** 重载 * 操作符, 返回对象的引用. */
	T& operator *()  const { return *ptr; }

	/** 重载 -> 操作符, 返回对象的指针. */
	T* operator ->() const { return ptr; }

	/** 绑定指定的指针, 这个方法不会影响 p2 引用计数. */
	void Attach(T* p2) {
		if (ptr) {
			ptr->Release();
		}
		ptr = p2;
	}

	/** 解除指针的绑定, 这个方法不会影响引用计数. */
	T* Detach() {
		T* pt = ptr;
		ptr = NULL;
		return pt;
	}

// Data Members -----------------------------------------------
public:
	T* ptr;		///< The original pointer
};

// Implements -------------------------------------------------

/** 给指针赋值. 当 p == NULL 时表示释放当前指向的对象. */
template <class T>
T* SmartPtr<T>::operator =(T* p) 
{
	if (ptr == p) { 
		return ptr; 
	}

	if (ptr) { 
		ptr->Release(); 
	}

	ptr = p;
	if (ptr) { 
		ptr->AddRef();
	}
	return ptr;
}

};

#endif // _NS_VISION_CORE_BASE_PTR_H
