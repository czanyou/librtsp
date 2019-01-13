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
#include "core/base/base_string.h"


namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// String class

const int String::npos = -1;

/** 默认构建方法. */
String::String()
: fAllocSize(0), fLength(0), fStringData(NULL)
{
}

/** 拷贝构建方法. */
String::String(const String& str) 
: fAllocSize(0), fLength(0), fStringData(NULL)
{
	if (str.GetLength() > 0) {
		Copy(str.fStringData, str.fLength, str.fLength);
	}
}

/** 拷贝构建方法. */
String::String(const String& str, int pos, int count) 
: fAllocSize(0), fLength(0), fStringData(NULL)
{
	if (str.GetLength() > 0 && pos < (int)str.fLength) {
		if (count == npos) {
			count = str.fLength - pos;
		} else {
			count = MIN(int(str.fLength - pos), count);
		}

		Copy(str.fStringData + pos, count, count);
	}
}

/** 拷贝构建方法. */
String::String(LPCSTR str, int length) 
: fAllocSize(0), fLength(0), fStringData(NULL)
{
	if (str) {
		if (length == npos) {
			length = strlen(str);
		} else {
			length = MIN(strlen(str), (UINT)length);
		}

		Copy(str, length, length);
	}
}

String::~String() {
	Clear();
}

/**
 * 连接字符串
 * @param str 
 * @param n 
 * @return String& 
 */
String& String::Append( LPCSTR str, UINT n )
{
	if (isempty(str) || n <= 0) {
		return *this;
	}

	UINT appendLength = MIN(strlen(str), n);
	UINT newlength = fLength + appendLength;

	if (newlength + 1 >= fAllocSize) {
		Realloc(newlength);
	}

	if (fStringData) {
		strncat(fStringData, str, appendLength);
		fLength = newlength;
		fStringData[fLength] = '\0';
	}

	return *this;
}


/**
 * 连接字符串
 * @param str 
 * @return String& 
 */
String& String::Append( LPCSTR str )
{
	Append(str, strlen(str));
	return *this;
}


/**
 * 连接字符串
 * @param str 
 * @return String& 
 */
String& String::Append( const String& str )
{
	Append(str.fStringData, str.fLength);
	return *this;
}

/**
 * 连接字符串
 * @param ch 
 * @return String& 
 */
String& String::Append( char ch )
{
	char str[] = {ch, '\0'};
	Append(str);
	return *this;
}

/**
 * 清空这个字符串.
 * @return void 
 */
void String::Clear() 
{
	if (fStringData) {
		free(fStringData);
		fStringData = NULL;
	}

	fLength = 0;
	fAllocSize = 0;
}

/** 比较两个字符串的大小. */
int String::Compare(LPCSTR right) const 
{
	if (right == NULL) {
		return fStringData ? 1 : 0;
	}

	LPCSTR left = fStringData;
	if (left == NULL) {
		left = "";
	}

	return strcmp(left, right);
}

/** 比较两个字符串的大小(忽略大小写). */
int String::CompareNoCase( LPCSTR right ) const
{
	if (right == NULL) {
		return fStringData ? 1 : 0;
	}

	LPCSTR left = fStringData;
	if (left == NULL) {
		left = "";
	}

	return strcasecmp(left, right);
}

/**
 * 比较两个字符串的大小.
 * @param s 
 * @return int 
 */
int String::Compare( const String& str ) const
{
	return Compare(str.c_str());
}

/** 返回一个值，该值指示指定的子串是否出现在此字符串中。*/
BOOL String::Contains( LPCSTR str ) const
{
	if (isempty(str)) {
		return FALSE;
	}

	return IndexOf(str) != npos;
}

/** 复制指定内容和长度的数据. */
void String::Copy(LPCSTR stringData, int stringLength, int allocLength)
{
	if (stringData == NULL || stringLength == 0 || allocLength == 0) {
		return;
	}

	Realloc(allocLength);
	fLength = stringLength;
	memcpy(fStringData, stringData, stringLength * sizeof(char));
	fStringData[fLength] = '\0';
}

/** 返回该字符串包含的 C 风格字符串. */
LPCSTR String::c_str() const
{
	return fStringData ? fStringData : "";
}

/** 
 * 确定此字符串实例的结尾是否与指定的字符串匹配。
 * @param value 要与此实例末尾的子字符串进行比较的字符串。
 */
BOOL String::EndsWith( LPCSTR value ) const
{
	if (value == NULL || fStringData == NULL) {
		return FALSE;
	}

	size_t len = strlen(value);
	if (len == 0 || len > GetLength()) {
		return FALSE;
	}

	LPCSTR p = value;
	LPCSTR data = fStringData + (GetLength() - len);

	while (*p != '\0') {
		if (*data == '\0') {
			return FALSE;

		} else if (*p != *data) {
			return FALSE;
		} 

		p++;
		data++;
	}
	return TRUE;
}

/**
 * 查找第一个字符.
 * @param str 
 * @param pos 
 * @return size_t 
 */
int String::FindFirstOf( LPCSTR str, int pos /*= 0*/ ) const
{
	if (str == NULL) {
		return npos;
	}

	int len = strlen(str);
	int minpos = 0x7FFFFFFF;
	for (int i = 0; i < len; i++) {
		int idx = IndexOf(str[i], pos);
		if (idx >= 0) {
			minpos = MIN(idx, minpos);
		}
	}

	if (minpos == 0x7FFFFFFF) {
		return npos;
	}
	return minpos;
}

/**
 * 格式化字符串
 * @param fmt 格式字符串
 * @param ... 可变参数列表.
 * @return 返回格式化后的字符串的长度.
 */
int String::Format( LPCSTR fmt, ... )
{
	if (fAllocSize < 1024) {
		Realloc(1024 + 1);
	}

	va_list ap;
	// print log to string ...
	va_start(ap, fmt);
	int len = vsnprintf(fStringData, fAllocSize - 1, fmt, ap);
	va_end(ap);

	fStringData[len] = '\0';
	fLength = len;
	return len;
}

/**
 * 返回指定索引位置的字符.
 * @param n 
 * @return const char& 
 */
const char& String::GetAt(UINT n) const 
{
	if (fStringData == NULL || n >= fLength) {
		static char null = 0;
		return null;
	}

	return fStringData[n]; 
};

/**
 * 返回指定索引位置的字符.
 * @param n 
 * @return char& 
 */
char& String::GetAt(UINT n) 
{
	if (fStringData == NULL || n >= fLength) {
		static char null = 0;
		return null;
	}

	return fStringData[n]; 
};

/** 返回这个字符串的容量 */
UINT String::GetCapacity() const
{
	return fAllocSize;
}

/** 返回该字符串包含的 C 风格字符串. */
LPCSTR String::GetChars() const
{
	return fStringData ? fStringData : "";
}

/** 返回这个字符串的长度 */
UINT String::GetLength() const
{
	return fLength;
}

/** 在指定的位置开始查找指定的子字符串. */
int String::IndexOf(LPCSTR value, int pos) const 
{
	if (isempty(value) || fStringData == NULL) {
		return npos;
	}

	if (pos < 0 || pos >= (int)fLength) {
		return npos;
	}

	LPCSTR find = strstr(fStringData + pos, value);
	if (find) {
		return find - fStringData;
	}

	return npos;
}

/** 在指定的位置开始查找指定的子字符串. */
int String::IndexOf(const char value, int pos) const 
{
	char str[2] = {0, 0};
	str[0] = value;
	return IndexOf(str, pos);	
}

/** 指出这个字符串是否为空 */
BOOL String::IsEmpty() const
{
	return (fLength == 0);
}

/** 替换指定的字符. */
void String::Replace(char target, char replacement)
{
	for (UINT i = 0; i < fLength; i++) {
		if (fStringData[i] == target) {
			fStringData[i] = replacement;
		}
	}
}

String String::Replace( LPCSTR target, LPCSTR replacement )
{
	if (isempty(target)) {
		return *this;
	}

	int srcLength = strlen(target);

	String ret;
	int start = 0;
	int pos = IndexOf(target, start);

	while (pos != String::npos) {
		ret += SubString(start, pos - start);

		if (replacement) {
			ret += replacement;
		}

		start = pos + srcLength;
		pos = IndexOf(target, start);
	}

	ret += SubString(start);
	return ret;
}

/** 重新分配内存 */
int String::Realloc(UINT allocLength) 
{	
	allocLength = MAX(allocLength, fLength) + 1;

	UINT newSize = 4;
	while (newSize < allocLength) {
		newSize <<= 1;
	}

	if (newSize == fAllocSize) {
		return fAllocSize;
	}

	// 分配新的空间
	fStringData = (char*)realloc(fStringData, newSize);
	if (fStringData) {
		fStringData[fLength] = '\0';
		fAllocSize = newSize;

	} else {
		fAllocSize = 0;
	}

	return fAllocSize;
}

/** 重新分配字符串缓存区大小. */
int String::Resize(int newSize)
{
	return Realloc(newSize);
}

/**
 * 确定此字符串实例的开头是否与指定的字符串匹配。
 * @param str 要比较的字符串
 */
BOOL String::StartsWith(LPCSTR value) const 
{
	if (isempty(value) || fStringData == NULL) {
		return FALSE;
	}

	LPCSTR p = value;
	LPCSTR data = fStringData;

	while (*p != '\0') {
		if (*data == '\0') {
			return FALSE;

		} else if (*p != *data) {
			return FALSE;
		} 

		p++;
		data++;
	}
	return TRUE;
}

/** 
 * 返回指定的子字符串. 
 * @param startIndex 子字符串的起始字符位置（从零开始）
 * @param length 子字符串的最大长度
 */
String String::SubString(int startIndex, int length) const 
{
	if (startIndex < 0 || startIndex >= (int)fLength) {
		return "";

	} else if (length == 0) {
		return "";

	} else if (length < 0) {
		return fStringData + startIndex;

	} else {
		if (length > int(fLength - startIndex)) {
			length = fLength - startIndex;
		}

		return String(fStringData + startIndex, length);
	}
}

/**
 * 分解指定的字符串.
 * @param separator 
 * @param pos 
 * @return util::String 
 */
String String::Tokenize( String separator, int& pos ) const
{
	if (pos == npos) {
		return "";
	}

	String dest;
	int findPos = IndexOf(separator, pos);
	if (findPos == npos) {
		dest = SubString(pos);
		pos  = npos;
		return dest;
	}

	dest = SubString(pos, findPos - pos);
	pos  = findPos + separator.GetLength();
	return dest;
}

/** 转为小写. */
void String::ToLower()
{
	for (UINT i = 0; i < fLength; i++) {
		fStringData[i] = tolower((unsigned char)fStringData[i]);
	}
}

/** 转为大写. */
void String::ToUpper()
{
	for (UINT i = 0; i < fLength; i++) {
		fStringData[i] = toupper((unsigned char)fStringData[i]);
	}
}

/**
 * 删除这个字符串头尾的空白字符. 
 * @return 返回清理后的字符串.
 */
String String::Trim()
{
	if (IsEmpty()) {
		return "";
	}

	// Trim Right
	int end = GetLength() - 1;
	for (; end >= 0; end--) {
		if (!isspace(fStringData[end])) {
			break;
		}
	}

	// Trim Left
	String ret;
	int start = 0;
	for (start = 0; start <= end; start++) {
		if (!isspace(fStringData[start])) {
			break;
		}
	}

	// Return
	if (start <= end) {
		return SubString(start, end - start + 1);
	}

	return "";
}

/** 重载 [] 操作符. */
char String::operator [] ( int n ) const
{
	return GetAt(n);
}

String::operator LPCSTR () const
{
	return fStringData ? fStringData : "";
}

/** 重载 = 操作符. */
String& String::operator = (LPCSTR str) 
{
	Clear();

	if (str) {
		int newlen = strlen(str);
		Copy(str, newlen, newlen);
	}
	return *this;
}

/** 重载 = 操作符. */
String& String::operator = (char ch) 
{
	char buf[] = {ch, '\0'};
	operator=(buf);
	return *this;
}

/** 重载 = 操作符. */
String& String::operator = ( const String& str )
{
	return operator=(str.fStringData);
}

String& String::operator += ( const char ch )
{
	return Append(ch);
}

String& String::operator += ( LPCSTR s )
{
	return Append(s);
}

String& String::operator += ( const String& s )
{
	return Append(s);
}

///////////////////////////////////////////////////////////////////////////////
// 

String operator + (const String& s1, LPCSTR s2) {
	String str = s1;
	if (s2) {
		str += s2;
	}

	return str;
}

bool operator == (const String& s1, LPCSTR s2) {
	return s1.Compare(s2) == 0;
}

bool operator != (const String& s1, LPCSTR str) {
	return s1.Compare(str) != 0;
}

bool operator > (const String& s1, LPCSTR str) {
	return s1.Compare(str) > 0;
}

bool operator < (const String& s1, LPCSTR str) {
	return s1.Compare(str) < 0;
}

bool operator >= (const String& s1, LPCSTR str) {
	return s1.Compare(str) >= 0;
}

bool operator <= (const String& s1, LPCSTR str) {
	return s1.Compare(str) <= 0;
}

String operator + (LPCSTR s1, const String& s2) {
	String str = s1;
	str += s2;
	return str;
}

bool operator == (LPCSTR s1, const String& s2) {
	return s2.Compare(s1) == 0;
}

bool operator != (LPCSTR s1, const String& s2) {
	return s2.Compare(s1) != 0;
}

bool operator > (LPCSTR s1, const String& s2) {
	return s2.Compare(s1) < 0;
}

bool operator < (LPCSTR s1, const String& s2) {
	return s2.Compare(s1) > 0;
}

bool operator >= (LPCSTR s1, const String& s2) {
	return s2.Compare(s1) <= 0;
}

bool operator <= (LPCSTR s1, const String& s2) {
	return s2.Compare(s1) >= 0;
}

String operator + (const String& s1, const String& s2)
{
	String str = s1;
	str += s2;
	return str;
}

bool operator == (const String& s1, const String& s2) {
	return s1.Compare(s2.c_str()) == 0;
}

bool operator != (const String& s1, const String& s2) {
	return s1.Compare(s2.c_str()) != 0;
}

bool operator > (const String& s1, const String& s2) {
	return s1.Compare(s2.c_str()) > 0;
}


bool operator < (const String& s1, const String& s2) {
	return s1.Compare(s2.c_str()) < 0;
}

bool operator >= (const String& s1, const String& s2) {
	return s1.Compare(s2.c_str()) >= 0;
}

bool operator <= (const String& s1, const String& s2) {
	return s1.Compare(s2.c_str()) <= 0;
}

};
