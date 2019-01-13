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
#ifndef _NS_VISION_CORE_UTIL_DEBUG_H_
#define _NS_VISION_CORE_UTIL_DEBUG_H_

#include "core/base/base_array.h"

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// AvgNumber class

/** 
 * AvgNumber 用来统计一个时刻变化的数值的平均值. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class AvgNumber
{
public:
	AvgNumber();
	~AvgNumber();

// Attributes -------------------------------------------------
public:
	UINT GetMaxLength();	
	UINT GetValidCount();	

// Operations -------------------------------------------------
public:
	void Reset();
	void Resize(UINT maxLength);
	void Append(int newValue);
	int  GetAvgValue(int statLength = -1);

// Data Members -----------------------------------------------
private:
	UINT fValidCount;	///< 当前有效的值的个数
	UINT fPosition;		///< 当前指针
	UINT fMaxLength;	///< 统计数组的长度
	int* fValues;		///< 统计样本表
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Dump functions

void DumpCell(String& dump, LPCSTR fmt, ... );
void DumpHeader(String& dump);
void DumpFooter(String& dump);

};
};

#endif // _NS_VISION_CORE_UTIL_DEBUG_H_
