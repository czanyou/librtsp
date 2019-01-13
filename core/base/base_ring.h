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
#ifndef _NS_VISION_CORE_BASE_RING_H
#define _NS_VISION_CORE_BASE_RING_H

#include "base_types.h"

namespace core {

//______________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RingList class

/** 
 * 环形缓存区实现类. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
template<typename T, typename ARG_T>
class RingList
{
public:
	RingList() {
		fValues		= NULL;
		fTail		= 0;
		fHead		= 0;
		fCapacity	= 0;
		Resize(4, T());
	}

public:
	void Clear() {
		while (!IsEmpty()) {
			RemoveTail();
		}

		fTail		= 0;
		fHead		= 0;
	}

	/** 在头部添加一个元素. */
	BOOL AddHead(const ARG_T& t) {
		if (IsFull()) {
			return FALSE;
		}

		fValues[fHead] = t;
		fHead = (fHead + 1) % fCapacity;
		return TRUE;
	}

	/** 返回从尾部开始指定索引位置的元素. */
	T& Get(UINT index) {
		return fValues[(fTail + index) % fCapacity];
	}

	/** 返回这个缓存区的长度. */
	int GetCapacity() const {
		return fCapacity - 1;	// 留出一个空位用来检测是否已满
	}

	/** 返回头部的元素. */
	T& GetHead() {
		return fValues[fHead];
	}

	/** 返回尾部的元素. */
	T& GetTail() {
		return fValues[fTail];
	}

	/** 指出缓存区是否为空. */
	BOOL IsEmpty() const {
		return (fTail == fHead);
	}

	/** 指出缓存区是否已满. */
	BOOL IsFull() const {
		return fTail == (fHead + 1) % fCapacity;
	}

	/** 删除一个头部的元素. */
	void RemoveHead() {
		if (IsEmpty()) {
			return;
		}

		fHead = (fHead + fCapacity - 1) % fCapacity;
		fValues[fHead] = T();
	}

	/** 删除一个尾部的元素. */
	void RemoveTail() {
		if (IsEmpty()) {
			return;
		}

		fValues[fTail] = T();
		fTail = (fTail + 1) % fCapacity;
	}

	/** 重新初始化这个缓存区的大小. */
	void Resize(UINT size, ARG_T fill) {

		size++; // 添加一个位置用于检测缓存区是否已满
		if (size <= fCapacity) {
			return; // 暂时不支持缩小缓存区
		}

		if (fValues) {
			delete[] fValues;
		}

		fCapacity = size;
		T* newValues = new T[fCapacity];
		for (UINT i = 0; i < fCapacity; i++) {
			newValues[i] = fill;
		}

		fHead = 0;
		fTail = 0;
		fValues = newValues;
	}

	/** 返回缓存区中有效元素的个数. */
	int Size() const {
		return (fHead + fCapacity - fTail) % fCapacity;
	}

// Data Members -----------------------------------------------
protected:
	T*   fValues;	///< 元素列表
	UINT fCapacity;	///< 缓存区长度
	UINT fTail;		///< 尾指针, 指向第一个元素的位置
	UINT fHead;		///< 头指针, 指向下一个空闲的位置
};

};

#endif // _NS_VISION_CORE_BASE_RING_H

