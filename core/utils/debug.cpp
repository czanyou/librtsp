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

#include "debug.h"

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// AvgNumber class

/** 构建一个新的 AvgNumber. */
AvgNumber::AvgNumber()
{
	fValues			= NULL;
	fMaxLength		= 0;
	fPosition		= 0;
	fValidCount		= 0;

	Resize(5);
}

AvgNumber::~AvgNumber()
{
	if (fValues) {
		free(fValues);
		fValues = NULL;
	}
}

/**
 * 添加一个新的统计样本. 当数组已满时添加新的样本会覆盖最老的样本.
 * @param newValue 要添加的新的样本.
 */
void AvgNumber::Append(int newValue)
{
	if (fValues == NULL) {
		return;
	}

	fValues[fPosition] = newValue;
	fPosition++;
	if (fPosition >= fMaxLength) {
		fPosition = 0;
	}

	if (fValidCount < fMaxLength) {
		fValidCount++;
	}
}

/**
 * 返回统计的平均值.
 * @param statLength 用于统计的最多的样本个数, 如果 -1 则表示使用所有的样本.
 * @return 返回计算后的平均值. 
 */
int AvgNumber::GetAvgValue(int statLength)
{
	if (fValues == NULL) {
		return 0;
	}

	if (statLength == -1) {
		statLength = fValidCount;
	}

	statLength = MIN((UINT)statLength, fValidCount);
	if (statLength <= 0) {
		return 0;
	}

	long avgNumber = 0;
	int pos = fPosition - 1;
	for (int i = 0; i < statLength; i++) {
		if (pos < 0) {
			pos = fMaxLength - 1;
		}

		avgNumber += fValues[pos--];
	}
	return avgNumber / statLength;
}

///< 返回这个对象可统计的数据的最大长度
UINT AvgNumber::GetMaxLength()
{
	return fMaxLength;
}

///< 返回有效的统计样本的个数
UINT AvgNumber::GetValidCount()
{
	return fValidCount;
}

/** 重新开始统计. */
void AvgNumber::Reset()
{
	fValidCount = 0;
	fPosition	= 0;

	if (fValues) {
		for (UINT i = 0; i < fMaxLength; i++) {
			fValues[i] = 0;
		}
	}
}

/**
 * 重新分配统计数组的大小.
 * @param maxLength 新的统计数组大小.
 */
void AvgNumber::Resize(UINT maxLength)
{
	fMaxLength = maxLength;

	if (maxLength == 0) {
		free(fValues);
		fValues = NULL;

	} else {
		int allocSize = fMaxLength * sizeof(int);
		int* values = (int*)realloc(fValues, allocSize);
		if (values != fValues) {
			fValues = values;
		}
	}

	Reset();
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Dump functions

void DumpHeader(String& dump)
{
	dump = "<html>\n<head>\n";
	dump += " <title>Stream Server Status</title>\n";
	dump += " <style type=\"text/css\">\n";
	dump += "  * { font: 12px Verdana; }\n";
	dump += "  table { border: 1px solid #ddd; border-collapse: collapse; border-spacing: 0; margin: 3px 1px; } \n";
	dump += "  td, th { padding: 2px 5px; }\n";
	dump += "  th { border-bottom: 1px solid #ccc; color: #888; background: #eee; border-right: 1px solid #ddd; }\n";
	dump += "  td { background: #fff; color: #777; border-bottom: 1px solid #ddd; border-right: 1px solid #eee; }\n";
	dump += "  fieldset { border: none; border-top: 1px solid #ccf; margin: 8px 8px; } \n";
	dump += "  legend { margin: 0 5px; color: #69f; } \n";
	dump += " </style>\n";
	dump += "</head>\n<body>\n";
}

void DumpCell(String& dump, LPCSTR fmt, ... )
{
	char buffer[255];
	memset(buffer, 0, sizeof(buffer));

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buffer, 250, fmt, ap);
	va_end(ap);

	dump += "<td>";
	dump += buffer;
	dump += "</td>";
}

void DumpFooter( String& dump )
{
	dump += "</body>\n</html>";
}

};
};
