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
#ifndef _NS_VISION_CORE_BASE_STRING_H
#define _NS_VISION_CORE_BASE_STRING_H

#include "base_types.h"

namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// String functions 

/** 整数转换成字符串. */
char* itoa(int val, char* buffer, int r = 10);

/** 指出指定的字符串是否为空字符串. */
inline bool isempty(LPCSTR str) 
{
	return (str == NULL) || (*str == '\0');
}

/** 指出指定的两个字符串是否相等. */
inline bool isequal(LPCSTR str1, LPCSTR str2) 
{
	if (str1 == NULL) {
		return (str2 == NULL);

	} else if (str2 == NULL) {
		return false;

	} else {
		return strcmp(str1, str2) == 0;
	}
}

/** 指出指定的字符串是否以指定的字符串开始. */
inline bool startsWith(LPCSTR str, LPCSTR start)
{
	if (start == NULL || str == NULL) {
		return false;
	}

	while (*start) {
		if (*start++ != *str++) {
			return false;
		}
	}

	return true;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// String class

/**
 * 代表一个单字节的字符串, 字符串处理类的简单实现.
 * 这个类封装了一些比较简单的字符串处理的方法来简化字符串的处理. 
 * 
 * The String class represents character strings. A string is represented by a 
 * list of 8-bit character codes accessible through the charCodeAt or the 
 * charCodes method.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class String
{
public:
	typedef int   size_type;		///< 字符串长度类型
	typedef char  TCHAR;			///< 字符类型
	typedef char* pointer;			///< 字符指针类型
	static const  int npos;			///< 无效的字符串长度或位置

public:
	String();
	String(const String& str);
	String(const String& str, int pos, int n);
	String(LPCSTR str, int length = npos);
	virtual ~String();

// ------------------------------------------------------------
public:
	String& Append(char ch);
	String& Append(LPCSTR str);
	String& Append(LPCSTR str, UINT n);
	String& Append(const String& str);

	void   Clear();
	int    Compare(LPCSTR str) const;
	int    Compare(const String& str) const;
	int    CompareNoCase(LPCSTR str) const;
	BOOL   Contains(LPCSTR str) const;
	LPCSTR c_str() const;
	LPCSTR GetChars() const;

	BOOL   EndsWith(LPCSTR str) const;
	int    FindFirstOf(LPCSTR str, int pos = 0) const;
	int    Format(LPCSTR fmt, ...);
	
	const char& GetAt(UINT n) const;
	char&  GetAt(UINT n);
	UINT   GetCapacity() const;		
	UINT   GetLength() const;			

	int    IndexOf(LPCSTR str, int pos = 0) const;
	int    IndexOf(const char str, int pos = 0) const;
	BOOL   IsEmpty() const;

	BOOL   StartsWith(LPCSTR str) const;
	String SubString(int pos = 0, int n = npos) const;

	void   Replace(char target, char replacement);
	String Replace(LPCSTR target, LPCSTR replacement);
	int    Resize(int newSize);

	void   ToUpper();
	void   ToLower();
	String Tokenize(String separator, int& pos) const;
	String Trim();

// 操作符重载 ------------------------------------------------
public:
	String& operator =  (char ch);
	String& operator =  (LPCSTR str);
	String& operator =  (const String& str);
	String& operator += (const String& s); 
	String& operator += (LPCSTR s);
	String& operator += (const char ch);	

	char operator[] (int n) const;
	operator LPCSTR () const;	///< 重载

// ------------------------------------------------------------
private:
	int  Realloc(UINT size);
	void Copy(LPCSTR str, int len, int alloc_len);

// Attributes -------------------------------------------------
private:
	UINT  fAllocSize;		///< 当前分配的内存长度
	UINT  fLength;			///< 当前字符串的长度
	char* fStringData;		///< 存储字符串的内存空间
};

/** 基本的单字节字符串类型. */
//typedef String String;

String operator +(const String& s1, LPCSTR s2);				///< 操作符重载
String operator +(LPCSTR s1, const String& s2);				///< 操作符重载
String operator +(const String& s1, const String& s2);		///< 操作符重载

bool operator == (const String& s1, LPCSTR s2);				///< 操作符重载
bool operator != (const String& s1, LPCSTR str);			///< 操作符重载
bool operator >  (const String& s1, LPCSTR str);			///< 操作符重载
bool operator <  (const String& s1, LPCSTR str);			///< 操作符重载
bool operator >= (const String& s1, LPCSTR str);			///< 操作符重载
bool operator <= (const String& s1, LPCSTR str);			///< 操作符重载

bool operator == (LPCSTR s1, const String& s2);				///< 操作符重载
bool operator != (LPCSTR s1, const String& s2);				///< 操作符重载
bool operator <  (LPCSTR s1, const String& s2);				///< 操作符重载
bool operator >  (LPCSTR s1, const String& s2) ;			///< 操作符重载
bool operator >= (LPCSTR s1, const String& s2);				///< 操作符重载
bool operator <= (LPCSTR s1, const String& s2);				///< 操作符重载

bool operator == (const String& s1, const String& s2);		///< 操作符重载
bool operator != (const String& s1, const String& s2);		///< 操作符重载
bool operator >  (const String& s1, const String& s2);		///< 操作符重载
bool operator <  (const String& s1, const String& s2);		///< 操作符重载
bool operator >= (const String& s1, const String& s2);		///< 操作符重载
bool operator <= (const String& s1, const String& s2);		///< 操作符重载;

};

#endif // _NS_VISION_CORE_BASE_STRING_H
