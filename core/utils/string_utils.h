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
#ifndef _NS_VISION_CORE_UTIL_STRING_UTILS_H_
#define _NS_VISION_CORE_UTIL_STRING_UTILS_H_

#include "core/base/base_map.h"

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Attributes class

/**
 * 属性列表实现类.
 * 用于管理(名称=值)属性元素.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Attributes : public Object
{
public:
	Attributes();
	Attributes(const Attributes& attributes);

	typedef HashMap<String, String> StringMap;

// Attributes -------------------------------------------------
public:
	String GetAttribute(LPCSTR key, LPCSTR defaultValue = "");
	int    GetAttributeInt(LPCSTR key, int defaultValue = 0);
	int    GetChangeFlags();
	int    SetAttribute(LPCSTR key, LPCSTR value);
	void   SetSeparator(LPCSTR separator);

// Operations -------------------------------------------------
public:
	void   Clear();
	void   ClearChangeFlags();
	int    HasAttribute(LPCSTR key);
	int    RemoveAttribute(LPCSTR key);
	void   ParseString(LPCSTR query);
	String ToString();

	Attributes& operator = (const Attributes& attributes);

// Data members -----------------------------------------------
private:
	int fChangeFlags;		///< 修改标记
	String fSeparator;		///< 属性分隔符
	StringMap fStringMap;	///< 属性 Map 表
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// StringUtils class 

/**
 * 字符串工具类, 集成了一些常用的字符串处理工具方法. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class StringUtils
{
public:
	static int    Base64Decode(const char* str, BYTE* space, int size);
	static char*  Base64Encode(const BYTE* p, int size, char *buf, int len);

	static String FormatBytes(INT64 bytes);
	static String FormatBitrate(INT64 bytes);

	static int    HexChar2Int(char ch);
	static int    HexDecode(LPCSTR text, BYTE* buf, UINT buflen);
	static String HexEncode(void* value, UINT len);
	static int    HexString2Int(String text);
	static INT64  HexString2Int64(String text);

	static String Trim(LPCSTR text);
	static String ValueOf(int value);
	static String RandomString(int length);
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Lex class

/** 
 * 代表一个基本的简单的词法解析器. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Lex
{
public:
	Lex(LPCSTR text);

// Operations -------------------------------------------------
public:
	void   Clear();
	int    Find(LPCSTR subString);
	String GetRemains();
	BOOL   HasMore();
	int    LookChar();
	BOOL   LookToken(LPCSTR token, BOOL caseSensitive = FALSE);
	int    NextChar();
	BOOL   NextNumber(int &value);
	BOOL   NextToken( char endChar, String &token );
	void   ResetPos();
	void   SkipSpace();

// Data members -----------------------------------------------
public:
	String fBuffer;			///< 要解析的字符串
	size_t fPosition;		///< 当前位置
	size_t fSavePosition;	///< 保存的位置
};

};
};

#endif // _NS_VISION_CORE_UTIL_STRING_UTILS_H_
