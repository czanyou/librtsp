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

#include "string_utils.h"

namespace core {
namespace util {

	
//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// 

/** 转换方法. */
char* itoa(int val, char* buffer, int r)
{
	if (buffer == NULL) {
		return buffer;
	}

	sprintf(buffer, "%d", val);
	return buffer;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Attributes class

Attributes::Attributes()
{
	fChangeFlags	= 0;
	fSeparator		= ";";
}

Attributes::Attributes( const Attributes& attributes )
{
	operator = (attributes);
}

void Attributes::Clear()
{
	fStringMap.Clear();
}

void Attributes::ClearChangeFlags()
{
	fChangeFlags = 0;
}

String Attributes::GetAttribute( LPCSTR key, LPCSTR defaultValue )
{
	if (isempty(key)) {
		return defaultValue ? defaultValue : "";
	}

	String value;
	if (fStringMap.Get(key, value)) {
		return value;
	}

	return defaultValue ? defaultValue : "";
}

int Attributes::GetAttributeInt( LPCSTR key, int defaultValue )
{
	if (isempty(key)) {
		return defaultValue;
	}

	String value;
	if (fStringMap.Get(key, value)) {
		return atoi(value.c_str());
	}

	return defaultValue;
}

int Attributes::GetChangeFlags()
{
	return fChangeFlags;
}

int Attributes::HasAttribute( LPCSTR key )
{
	return fStringMap.Contanis(key);
}

int Attributes::RemoveAttribute( LPCSTR key )
{
	if (isempty(key)) {
		return 0;
	}

	fStringMap.Remove(key);
	return 0;
}

void Attributes::ParseString( LPCSTR query )
{
	fStringMap.Clear();
	if (isempty(query)) {
		return;
	}

	//LOG_D("query: %s\r\n", query);
	String value = query;

	String::size_type pos = 0;
	while (pos != String::npos) {
		String token = value.Tokenize(fSeparator, pos);
		String::size_type n = token.IndexOf('=');
		//LOG_D("token: %s\r\n", token.c_str());

		String name = token;
		String value = "";

		if (n != String::npos) {
			name  = token.SubString(0, n).Trim();
			value = token.SubString(n + 1).Trim();
		}

		SetAttribute(name, value);
	}
}

int Attributes::SetAttribute( LPCSTR name, LPCSTR value )
{
	if (isempty(name)) {
		return 0;

	} else if (value == NULL) {
		value = "";
	}

	String oldValue;
	String key = name;
	if (!fStringMap.Get(key, oldValue)) {
		fChangeFlags++;
		fStringMap.Put(key, value);

	} else if (oldValue != value) {
		fChangeFlags++;
		fStringMap.Put(key, value);
	}

	return 0;
}

void Attributes::SetSeparator( LPCSTR separator )
{
	fSeparator = isempty(separator) ? " " : separator;
}

Attributes& Attributes::operator = ( const Attributes& attributes )
{
	fChangeFlags	= attributes.fChangeFlags;
	fSeparator		= attributes.fSeparator;
	fStringMap		= attributes.fStringMap;
	return *this;
}

String Attributes::ToString()
{
	StringMap::EntryList tokens;
	fStringMap.GetEntrys(tokens);

	LPCSTR sep = "";
	String params;

	StringMap::EntryList::ListIterator iter = tokens.GetListIterator();
	while (iter.HasNext()) {
		StringMap::EntryPtr entry = iter.Next();

		params += sep;
		params += entry->fKey;

		String& value = entry->fValue;
		if (!value.IsEmpty()) {
			params += "=";
			params += value;
		}

		sep = fSeparator;
	}

	return params;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// StringUtils class 

/** 
 * Base-64 decoding.  This represents binary data as printable ASCII
 * characters.  Three 8-bit binary bytes are turned into four 6-bit
 * values, like so:
 *
 *   [11111111]  [22222222]  [33333333]
 *   [111111] [112222] [222233] [333333]
 *
 * Then the 6-bit values are represented using the characters "A-Za-z0-9+/".
 */
static const int kBase64DecodeTable[256] = 
{
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

/** 
 * Do base-64 decoding on a string.  Ignore any non-base64 bytes.
 * Return the actual number of bytes generated.  The decoded size will
 * be at most 3/4 the size of the encoded, and may be smaller if there
 * are padding characters (blanks, newlines).
 */
int StringUtils::Base64Decode(const char* str, BYTE* space, int size)
{
    const char* cp;
    int space_idx, phase;
    int prev_d = 0;
    unsigned char c;

	if (str == NULL || space == NULL || size <= 0) {
		return -1;
	}

    space_idx = 0;
    phase = 0;
    for (cp = str; *cp != '\0'; ++cp) {
		int d = kBase64DecodeTable[(int)*cp];
		if ( d == -1 ) {
			continue;
		}

		switch (phase) {
		case 0:
			++phase;
			break;
		case 1:
			c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
			if ( space_idx < size )	{
				space[space_idx++] = c;
			}
			++phase;
			break;
		case 2:
			c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
			if ( space_idx < size )	{
				space[space_idx++] = c;
			}
			++phase;
			break;
		case 3:
			c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
			if ( space_idx < size )	{
				space[space_idx++] = c;
			}
			phase = 0;
			break;
		}
		prev_d = d;
	}
    return space_idx;
}

/**
 * MIME Base-64 encoding
 * @param p - address of source buffer
 * @param size - size of the source buffer
 * @param buf - address of destination buffer
 * @param len - number of bytes in destination buffer
 */
char* StringUtils::Base64Encode (const BYTE *p, int size, char *buf, int len)
{
	char base64_code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		"0123456789+/=";

	unsigned int *out = (unsigned int *)buf;
	int out_len = MIN(size / 3, len / 4 - 1);

	if (p == NULL || size <= -0 || buf == NULL || len <= 0) {
		return NULL;
	}

	int i = out_len;
	while (i--) {
		unsigned int input  = 0;
		input = p[0] << 16 | p[1] << 8 | p[2];

		*out  = base64_code[(input >> 18) & 0x3F];
		*out |= base64_code[(input >> 12) & 0x3F] << 8;
		*out |= base64_code[(input >>  6) & 0x3F] << 16;
		*out |= base64_code[(input      ) & 0x3F] << 24;

		p += 3;
		out++;
	}
	*out = '\0';

	// 转换剩下的数据
	int leftover = size - out_len * 3;
	if (leftover > 0) {
		char *s = buf + (out_len << 2);

		s[2] = '=';
		s[3] = '=';

		s[0] = base64_code[(*p >> 2) & 0x3F];

		if (leftover == 1) {
			unsigned int input = (*p << 4) & 0x3F;
			s[1] = base64_code[input & 0x3F];

		} else {
			unsigned int input = (*p++ << 4) & 0x3F;
			input |= *p >> 4;
			s[1] = base64_code[input & 0x3F];

			input = *p << 2;
			s[2] = base64_code[input & 0x3F];		
		}
		s[4] = '\0';
	}
	return buf;
}

/**
 * 把指定的十六进制字符转换成相应的整数.
 * @param ch 要转换的十六进制字符, 只能包含 '0' ~ '9', 'A' ~ 'F', 'a' ~ 'f'.
 * @return 返回相应的整数值.
 */
int StringUtils::HexChar2Int(char ch) 
{
	if (ch >= '0' && ch <= '9') {
		return ch - '0';
	} else if (ch >= 'a' && ch <= 'f') {
		return 10 + (ch - 'a');
	} else if (ch >= 'A' && ch <= 'F') {
		return 10 + (ch - 'A');
	}
	return -1;
}

int StringUtils::HexString2Int(String text) 
{
	int value = 0;
	for (UINT i = 0; i < text.GetLength(); i++) {
		value = value << 4;
		value += HexChar2Int(text.GetAt(i));
	}
	return value;
}

INT64 StringUtils::HexString2Int64( String text )
{
	INT64 value = 0;
	for (UINT i = 0; i < text.GetLength(); i++) {
		value = value << 4;
		value += HexChar2Int(text.GetAt(i));
	}
	return value;
}

/**
 * 把指定的 16 进制表示字符串转换为 2 进制的 BYTE 数组.
 * @param text 要转换的字符串, 如 "FE00FBCFEC"
 * @param buf  缓存区
 * @param buflen 缓存区大小
 * @return 返回成功解析的字节的个数.
 */
int StringUtils::HexDecode(LPCSTR text, BYTE* buf, UINT buflen) 
{
	if (text == NULL || buf == NULL || buflen == 0) {
		return 0;
	}
	const char* p = text;
	int count = 0;
	for (size_t i = 0; i < buflen; i++) {
		if (p[0] == '\0' || p[1] == '\0' ) {
			break;
		}

		int a = HexChar2Int(p[0]);
		int b = HexChar2Int(p[1]);
		if (a < 0 || b < 0) {
			break;
		}

		buf[i] = (BYTE)((a << 4) | b);
		count++;

		p += 2;
	}
	return count;
}

String StringUtils::HexEncode( void* value, UINT len )
{
	String ret;
	if (value == NULL || len == 0) {
		return ret;
	}

	char buf[10];
	BYTE* p = (BYTE*)value;
	BYTE* end = p + len;
	while (p < end) {
		sprintf(buf, "%02X", *p);
		ret += buf;
		p++;
	}
	return ret;
}

/**
 * 返回指定的整数的字符串值.
 * @param i 要转换的整数
 */
String StringUtils::ValueOf( int i )
{
	char buf[32];
	snprintf(buf, 30, "%d", i);
	return buf;
}

/**
 * 删除指定的字符串两端的空白.
 * @param text 要整理的字符串
 * @return 返回整理后字符串
 */
String StringUtils::Trim( LPCSTR text )
{
	if (text == NULL || *text == '\0') {
		return "";
	}
	size_t len = strlen(text);

	// 跳过开始位置的空白字符
	LPCSTR p = text;
	while ((*p != '\0') && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) {
		p++;
	}
	if (*p == '\0') {
		return "";
	}

	// 跳过结束位置的空白字符
	LPCSTR start = p;
	p = text + len - 1;
	while ((p > start) && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) {
		p--;
	}

	return String(start, (p - start + 1));
}

String StringUtils::FormatBytes(INT64 bytes)
{
	char buffer[MAX_PATH + 1];
	memset(buffer, 0, sizeof(buffer));

	double length = bytes;
	if (length < 1024) {
		snprintf(buffer, MAX_PATH, "%.0f Bytes", length);

	} else if (length < 1024 * 1024) {
		snprintf(buffer, MAX_PATH, "%.1fK", length / 1024);

	} else if (length < 1024 * 1024 * 1024) {
		snprintf(buffer, MAX_PATH, "%.1fM", length / (1024 * 1024));

	} else {
		snprintf(buffer, MAX_PATH, "%.1fG", length / (1024 * 1024 * 1024));
	}
	return buffer;
}

String StringUtils::FormatBitrate( INT64 bytes )
{
	bytes = bytes * 8;

	char buffer[MAX_PATH + 1];
	memset(buffer, 0, sizeof(buffer));

	double rate = bytes;
	if (rate < 1024) {
		snprintf(buffer, MAX_PATH, "%.1fbps", rate);

	} else if (rate < 1024 * 1024) {
		snprintf(buffer, MAX_PATH, "%.1fkbps", rate / 1024);

	} else if (rate < 1024 * 1024 * 1024) {
		snprintf(buffer, MAX_PATH, "%.1fmbps", rate / (1024 * 1024));

	} else {
		snprintf(buffer, MAX_PATH, "%.1fgbps", rate / (1024 * 1024 * 1024));
	}
	return buffer;
}


/** 生成指定长度的随机字符串, 最长不能超过 64K. */
String StringUtils::RandomString(int length)
{
	static const char CCH[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (length >= 64 * 1024) {
		return "";		// 要求生成的字符太长了
	}

	String randomString;
	for (int i = 0; i < length; i++) {
		int x = rand() % (sizeof(CCH) - 1);  
		randomString += CCH[x];
	}
	return randomString;
}


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Lex class

/**
 * 构建一个新的 lex .
 * @param text 要解析的字符串.
 */
Lex::Lex( LPCSTR text )
{
	fBuffer			= text;
	fPosition		= 0;
	fSavePosition	= 0;
}

void Lex::Clear()
{
	fBuffer			= "";
	fPosition		= 0;
	fSavePosition	= 0;
}

/**
* 查找指定的子字符串.
* @param subString 子字符串.
* @return int 返回子字符串的位置.
*/
int Lex::Find( LPCSTR subString )
{
	if (!HasMore() || subString == NULL) {
		return -1;
	}
	return fBuffer.IndexOf(subString, fPosition);
}

/** 返回剩余的还没有解析的所有字符. */
String Lex::GetRemains()
{
	size_t temp = fPosition;
	fPosition = fBuffer.GetLength();
	return fBuffer.SubString(temp);
}

/** 指出是否还可字符没有解析. */
BOOL Lex::HasMore()
{
	return fPosition < fBuffer.GetLength();
}

int Lex::LookChar()
{
	if (fPosition < fBuffer.GetLength()) {
		return fBuffer.GetAt(fPosition);
	}
	return -1;
}

/**
 * 查看接下来是否是指定的标识
 * @param token 期望的标识.
 * @param caseSensitive 是否大小写敏感
 * @return 如果找到则返回 TRUE. 
 */
BOOL Lex::LookToken( LPCSTR token, BOOL caseSensitive )
{
	if (token == NULL || *token == '\0') {
		return FALSE;
	}

	size_t len = strlen(token);
	if (len > fBuffer.GetLength() - fPosition) {
		return FALSE;
	}

	for (size_t i = 0; i < len; i++) {
		char ch = fBuffer.GetAt(fPosition + i);
		if (caseSensitive) {
			if (ch != token[i]) {
				return FALSE;
			}

		} else {
			ch = tolower(ch);
			if (ch != tolower(token[i])) {
				return FALSE;
			}
		}
	}

	fPosition += len;
	return TRUE;
}

/** 返回下一个字符的值. */
int Lex::NextChar()
{
	if (fPosition < fBuffer.GetLength()) {
		char ch = fBuffer.GetAt(fPosition);
		fPosition++;
		return ch;
	}
	return -1;
}

/**
* 返回当前位置的数字的值, 如果当前位置不是一个数字, 则返回 FALSE.
* @param value 返回读取的整数值.
* @return 如果成功则返回 TRUE. 
*/
BOOL Lex::NextNumber( int &value )
{
	if (!HasMore()) {
		return FALSE;
	}

	size_t i = fPosition;
	char ch = fBuffer.GetAt(i);
	if (!isdigit(ch) && ch != '-' && ch != '+') {
		return FALSE;
	}
	i++;

	while (i < fBuffer.GetLength()) {
		char ch = fBuffer.GetAt(i);
		if (!isdigit(ch)) {
			break;
		}
		i++;
	}

	if (i <= fPosition) {
		return FALSE;
	}
	value = atoi(fBuffer.SubString(fPosition, i - fPosition).c_str());
	fPosition = i;
	return TRUE;
}

/**
 * 返回下一个以指定的字符结束 token, 如果失败, 则返回 FALSE.
 * @param endChar 结束字符.
 * @param token 返回解析的 Token.
 */
BOOL Lex::NextToken( char endChar, String &token )
{
	size_t i = 0;
	for (i = fPosition; i < fBuffer.GetLength(); i++) {
		char ch = fBuffer.GetAt(i);
		if (ch == endChar) {
			token = fBuffer.SubString(fPosition, i - fPosition);
			fPosition = i + 1;
			return TRUE;
		}
	}

	return FALSE;
}

/** 重置解析位置. */
void Lex::ResetPos()
{
	fPosition = 0;
}

/** 跳过所有空白字符. */
void Lex::SkipSpace()
{
	for (size_t i = fPosition; i < fBuffer.GetLength(); i++) {
		char ch = fBuffer.GetAt(i);
		if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
			break;
		}
		fPosition++;
	}
}

};
};
