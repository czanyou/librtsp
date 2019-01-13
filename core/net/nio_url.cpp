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

#include "nio_url.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// class Uri

/** URI 中允许使用的标点或符号. */
LPCSTR Uri::fSafeChars = "-_.!~*'();/?:@&=+$,";

/** 默认构建方法. */
Uri::Uri()
{
	fPort = 0;
}

/**
 * 通过指定的 url 字符串构建一个 URI 对象.
 * @param url URI 字符串.
 */
Uri::Uri( LPCSTR url )
{
	fPort = 0;
	Parse(url);
}

/**
 * 对指定的 URI 进行解码, 转换成正常的字符串, 把 %XX 的形式字符转换
 * 成原始的字符.
 * 
 * @param stringToUnescape 要转换经过编码的 URI 字符串.
 * @return 返回转换后的字符串.
 */
String Uri::DecodeUriComponent (LPCSTR stringToUnescape ) 
{
	String ret;
	if (isempty(stringToUnescape)) {
		return ret;
	}

	int len = strlen(stringToUnescape);
	if (len >= 64 * 1024) {
		return ret;
	}

	ret.Resize(len + 10);

	while (*stringToUnescape) {
		if (*stringToUnescape == '%') {
			stringToUnescape++;

			if (*stringToUnescape  == '%') {
				ret += '%';

			} else {
				// HEX to char
				int value = 0;
				for (int i = 0; (i < 2) && *stringToUnescape; i++) {
					value <<= 4;
					if (isdigit(*stringToUnescape )) {
						value += *stringToUnescape  - '0';

					} else {
						value += toupper(*stringToUnescape ) - 'A' + 10;
					}

					stringToUnescape++;
				}
				ret += char(value);
			}

		} else if (*stringToUnescape == '+') {
			stringToUnescape++;
			ret += ' ';

		} else {
			ret += *stringToUnescape ++;
		}
	}

	return ret;
}

/**
 * 将字符串转换为它的转义表示形式。
 * EncodeUriComponent 方法将除 RFC 2396 非保留字符之外的所有字符转换为
 * 相应的十六进制表示形式。所有 Unicode 字符在转义之前都会先转换为
 * UTF-8 格式。
 *
 * @param stringToEscape 要转换的正常字符串.
 * @return 返回转换后的字符串.
 */
String Uri::EncodeUriComponent (LPCSTR stringToEscape)
{
	String ret;
	if (isempty(stringToEscape)) {
		return ret;
	}

	int len = strlen(stringToEscape);
	if (len >= 64 * 1024) {
		return ret;
	}

	ret.Resize(len * 2);
	static LPCSTR DIGITS = "0123456789ABCDEF++";

	while (*stringToEscape) {
		if (isalnum(*stringToEscape)) {
			ret += *stringToEscape++;

		} else if (strchr(fSafeChars, *stringToEscape) != NULL) {
			ret += *stringToEscape++;

		} else {
			ret += '%';
			ret += DIGITS[(*stringToEscape >> 4) & 0x0f];
			ret += DIGITS[(*stringToEscape     ) & 0x0f];
			stringToEscape++;
		} 
	}
	return ret;
}

/** 返回与此 URI 关联协议的默认端口号。*/
int Uri::GetDefaultPort()
{
	if (fProtocol == "http") {
		return 80;

	} else if (fProtocol == "https") {
		return 445;

	} else if (fProtocol == "rtsp") {
		return 554;

	} else if (fProtocol == "ftp") {
		return 21;
	}
	return 0;
}

/**  The full URL that was originally parsed. Both the protocol and host are lowercased. */
String Uri::GetHref()
{
	return fHref;
}

/** The full lowercased host portion of the URL, including port information. */
String Uri::GetHost()
{
	if (fPort == 0) {
		return fHostName;
	}

	String host = fHostName;
	host += ":";
	host += StringUtils::ValueOf(fPort);
	return host;
}

/**  Just the lowercased host name portion of the host.*/
String Uri::GetHostName()
{
	return fHostName;
}

/**
 * 返回指定的名称的查询参数的值.
 *
 * @param name 参数的名称
 * @param defaultValue 参数的默认值
 * @return 返回参数的整数值
 */
int Uri::GetParameterInt( LPCSTR name, int defaultValue )
{
	String value;
	if (GetParameter(name, value) && !value.IsEmpty()) {
		return atoi(value.c_str());
	}

	return defaultValue;
}

/**
 * 返回指定的名称的查询参数的值.
 *
 * @param name 参数的名称
 * @param value 输出参数, 返回参数的值
 * @return 如果指定的参数不存在, 则返回 FALSE, 否则返回 TRUE.
 */
BOOL Uri::GetParameter( LPCSTR name, String& value )
{
	value.Clear();
	if (name == NULL || *name == '\0' || fQuery.IsEmpty()) {
		return FALSE;
	}

	size_t name_len = strlen(name);
	LPCSTR query = fQuery.c_str();
	LPCSTR start = query;

	while (start != NULL) {
		LPCSTR end = strchr(start, '&');
		size_t param_len = end ? (end - start) : strlen(start);
		// 占位参数,没有参数值
		if ((param_len == name_len) && !strncmp(name, start, name_len)) {
			return TRUE;

			// 参数 name=value 的形式
		} else if ((param_len >= name_len) && (start[name_len] == '=') 
			&& !strncmp(name, start, name_len)) {
			start += (name_len + 1);
			param_len -= (name_len + 1);
			value = String(start, param_len);
			value = DecodeUriComponent(value.c_str());
			return TRUE;
		}
		start = end ? (end + 1) : NULL;
	}

	return FALSE;
}

/**
 * 返回指定的名称的查询参数的值.
 *
 * @param name 参数的名称
 * @return 返回参数的值
 */
String Uri::GetParameter( LPCSTR name )
{
	String value;
	if (GetParameter(name, value)) {
		return value;
	}
	return "";
}

/**  Concatenation of pathname and search. */
String Uri::GetPath()
{
	String path = fPathName;
	if (!fQuery.IsEmpty()) {
		path += "?";
		path += fQuery;
	}

	return path;
}

/** The path section of the URL, that comes after the host and before the query, 
 * including the initial slash if present. 
 */
String Uri::GetPathName()
{
	return fPathName;
}

/** The port number portion of the host. */
UINT Uri::GetPort()
{
	return fPort;
}

/**  The request protocol, lowercased. */
String Uri::GetProtocol()
{
	return fProtocol;
}

/** Either the 'params' portion of the query string. */
String Uri::GetQuery()
{
	return fQuery;
}

/**
 * 指出是否存在指定名称的参数.
 * @param name 参数的名称
 * @return 如果指定的参数不存在, 则返回 FALSE, 否则返回 TRUE.
 */
BOOL Uri::HasParameter( LPCSTR name )
{
	String value;
	return GetParameter(name, value);
}

/** 
 * 解析指定的 URI 字符串.
 *
 * @param url 要解析的 URI 字符串.
 * @return 如果成功, 则返回 0.
 */
int Uri::Parse( LPCSTR url )
{
	fHref = url;
	fHostName.Clear();
	fPathName.Clear();
	fProtocol.Clear();
	fQuery.Clear();
	fUserName.Clear();
	fPassword.Clear();
	fHash .Clear();
	fPort = 0;

	if (url == NULL) {
		return -1;
	}

	// 解析 scheme 部分
	int pos = fHref.IndexOf("://");
	if (pos == String::npos || pos > 6) {
		fPathName = url;

	} else {
		fProtocol = fHref.SubString(0, pos);
		fProtocol.ToLower();
		fHostName = fHref.SubString(pos + 3);

		// 解析路径部分
		pos = fHostName.IndexOf("/");
		if (pos != String::npos) {
			fPathName = fHostName.SubString(pos);
			fHostName = fHostName.SubString(0, pos);
		}

		// 解析用户和密码部分
		pos = fHostName.IndexOf("@");
		if (pos != String::npos) {
			fUserName = fHostName.SubString(0, pos);
			fHostName = fHostName.SubString(pos + 1);
		}

		// 解析端口部分
		pos = fHostName.IndexOf(":");
		if (pos != String::npos) {
			fPort = atoi(fHostName.SubString(pos + 1));
			fHostName = fHostName.SubString(0, pos);
		}

		// 解析用户和密码部分
		pos = fUserName.IndexOf(":");
		if (pos != String::npos) {
			fPassword = fUserName.SubString(pos + 1);
			fUserName = fUserName.SubString(0, pos);
		}
	}

	// 解析查询参数部分
	pos = fPathName.IndexOf("?");
	if (pos != String::npos) {
		fQuery = fPathName.SubString(pos + 1);
		fPathName = fPathName.SubString(0, pos);
	}

	// 引用部分
	pos = fPathName.IndexOf("#");
	if (pos != String::npos) {
		fHash  = fQuery.SubString(pos + 1);
		fQuery = fQuery.SubString(0, pos);
	}

	fProtocol.ToLower();
	fHostName.ToLower();
	return 0;
}

void Uri::SetHostName( LPCSTR hostName )
{
	fHostName = hostName ? hostName : "";
}

void Uri::SetProtocol( LPCSTR protocol )
{
	fProtocol = protocol ? protocol : "";
}

void Uri::SetPathName( LPCSTR pathName )
{
	fPathName = pathName ? pathName : "";
}

void Uri::SetQuery( LPCSTR query )
{
	fQuery = query ? query : "";
}

void Uri::SetUserName( LPCSTR userName )
{
	fUserName = userName ? userName : "";
}

void Uri::SetPassword( LPCSTR password )
{
	fPassword = password ? password : "";
}

void Uri::SetHash( LPCSTR hash )
{
	fHash = hash ? hash : "";
}

void Uri::SetPort( UINT port )
{
	fPort = port;
}

/** 构造这个 URI 的字符串表示形式. */
String Uri::ToString()
{
	String url;

	// Scheme://
	url += fProtocol;
	url += "://";

	// Username:Password
	if (!fUserName.IsEmpty()) {
		url += fUserName;
		if (!fPassword.IsEmpty()) {
			url += ":";
			url += fPassword;
		}
		url += "@";
	}

	// Host:Port
	url += fHostName;
	if (fPort > 0) {
		char buffer[32];
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer, 30, "%u", fPort);

		url += ":";
		url += buffer;
	}

	// Path
	if (fPathName.IsEmpty()) {
		url += "/";
	} else {
		url += fPathName;
	}

	// Query
	if (!fQuery.IsEmpty()) {
		url += "?";
		url += fQuery;
	}

	return url;
}

};
}

