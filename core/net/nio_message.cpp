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

#include "nio_message.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseMessage class

/** 默认构建方法. */
BaseMessage::BaseMessage(void)
{
	fIsRequest		= TRUE;
	fContent		= NULL;
	fContentLength	= 0;
	fStatusCode		= 0;
	fProtocol		= "SIP/2.0";
}

/** 默认复制构建方法. */
BaseMessage::BaseMessage( const BaseMessage& message )
{
	fIsRequest		= TRUE;
	fContent		= NULL;
	fContentLength	= 0;
	fStatusCode		= 0;
	fProtocol		= "SIP/2.0";
	operator =(message);
}

/**
 * 构建一个新的 BaseMessage 对象.
 * @param method 请求方法.
 * @param url URL
 * @param protocol 协议类型
 */
BaseMessage::BaseMessage( LPCSTR method, LPCSTR url, LPCSTR protocol )
{
	fIsRequest		= TRUE;
	fContent		= NULL;
	fContentLength	= 0;
	fStatusCode		= 0;
	fProtocol		= "SIP/2.0";

	fMethod			= method ? method : "";
	fUri			= url ? url : "";
	fProtocol		= protocol ? protocol : "SIP/2.0";
}

/**
 * 构建一个新的 BaseMessage 对象.
 * @param statusCode 应答状态码
 * @param statusText 应答状态字符串
 */
BaseMessage::BaseMessage( int statusCode, LPCSTR statusText )
{
	fIsRequest		= TRUE;
	fContent		= NULL;
	fContentLength	= 0;
	fStatusCode		= statusCode;
	fProtocol		= "SIP/2.0";
	fStatusText		= statusText ? statusText : "Unknown Status";
}

BaseMessage::~BaseMessage(void)
{
	SetContent(NULL, 0);
}

/**
 * 添加一个新的头域
 * @param name 头域的名称
 * @param value 头域的值.
 */
void BaseMessage::AddHeader( LPCSTR name, LPCSTR value )
{
	if (isempty(name) || isempty(value)) {
		return;
	}

	size_t len = strlen(name);

	String header = name;
	header += ": ";
	header += value;

	// 找到最后一个同名的头字段, 并把新的头字段插入到这个头字段的后面.
	BOOL isFound = FALSE;
	StringListIter iter = fHeaders.GetListIterator();
	while (iter.HasNext()) {
		String &line = iter.Next();
		if (len < line.GetLength()
			&& !strncasecmp(line.c_str(), name, len)
			&& line.GetAt(len) == ':') {
			isFound = TRUE;

		} else if (isFound) {
			iter.Add(header);
			return;
		}
	}

	fHeaders.AddLast(header);
}

/** 清空所有的内容 */
void BaseMessage::Clear()
{
	fHeaders.Clear();
	SetContent(NULL, 0);

	fMethod		= "";
	fUri		= "";
	fProtocol	= "";
	fStatusText	= "";

	fIsRequest	= TRUE;
	fStatusCode	= 0;
}

/** 清除所有的头域. */
void BaseMessage::ClearHeaders()
{
	fHeaders.Clear();
}

String BaseMessage::EncodeWithContent()
{
	BOOL withContent = FALSE;

	// Content --------------
	if (fContent && fContentLength > 0) {
		size_t len = strlen(fContent);
		if (len == fContentLength) {
			withContent = TRUE;
		}
	}

	String message = EncodeWithoutContent();

	if (withContent) {
		message += fContent;
	}

	/*String contentLength = GetHeader("Content-Length");

	LOG_E("%s: %d, %d, %s, %d", GetURL().c_str(),
		headerLength, fContentLength, contentLength.c_str(),
		message.GetLength());*/

	return message;
}

String BaseMessage::EncodeWithoutContent()
{
	String message;

	// Start Line -------------
	if (fIsRequest) {
		message += fMethod;
		message += " ";
		message += fUri;
		message += " ";
		message += fProtocol;
		message += "\r\n";

	} else {
		message += fProtocol;
		message += " ";
		message += StringUtils::ValueOf(fStatusCode);
		message += " ";
		message += fStatusText;
		message += "\r\n";
	}

	// 自动计算并生成 Content-Length 头
	if (fContent && fContentLength > 0) {
		SetHeader("Content-Length", fContentLength);
	}

	// Headers --------------
	StringListIter iter = fHeaders.GetListIterator();
	while (iter.HasNext()) {
		String &header = iter.Next();
		message += header;
		message += "\r\n";
	}
	message += "\r\n";
	return message;
}

/** 返回这个消息的内容. */
LPCSTR BaseMessage::GetContent()
{
	return fContent;
}

/** 返回这个消息的内容的长度. */
UINT BaseMessage::GetContentLength()
{
	return fContentLength;
}

String BaseMessage::GetHeader( LPCSTR name )
{
	if (isempty(name)) {
		return "";
	}

	size_t len = strlen(name);

	StringListIter iter = fHeaders.GetListIterator();
	while (iter.HasNext()) {
		String &header = iter.Next();

		if (len >= header.GetLength()) {
			continue;
		}

		if (!strncasecmp(header.c_str(), name, len)	&& (header.GetAt(len) == ':')) {			
			LPCSTR value = header.c_str() + len + 1;
			if (value[0] == ' ') {
				value++;
			}
			return value;
		}
	}

	return "";
}

int BaseMessage::GetHeaderInt( LPCSTR name, int defaultValue )
{
	String value = GetHeader(name);
	if (value.IsEmpty()) {
		return defaultValue;
	}

	return atoi(value);
}

/**
 * 返回指定的名称的消息头域所有的值.
 * @param name 头域的名称
 * @param values 输出参数
 * @return 如果指定的头域不存在则返回 FALSE.
 */
BOOL BaseMessage::GetHeaders( LPCSTR name, StringList &values )
{
	if (isempty(name)) {
		return FALSE;
	}

	BOOL bRet = FALSE;
	size_t len = strlen(name);

	StringListIter iter = fHeaders.GetListIterator();
	while (iter.HasNext()) {
		String &header = iter.Next();

		if (len >= header.GetLength()) {
			continue;

		} else if (!strncasecmp(header.c_str(), name, len)	&& (header.GetAt(len) == ':')) {
			LPCSTR value = header.c_str() + len + 1;
			if (value[0] == ' ') {
				value++;
			}

			bRet = TRUE;
			values.AddLast(value);

		} else if (bRet) {
			break;
		}
	}

	return bRet;
}

/** 这个消息的请求方法名, 如 'GET', 'POST' . */
const String& BaseMessage::GetMethod() const
{
	return fMethod;
}

/** 这个消息的协议名 . */
const String& BaseMessage::GetProtocol() const
{
	return fProtocol;
}

/** 这个消息的应答状态码 . */
int BaseMessage::GetStatusCode()
{
	return fStatusCode;
}

/** 这个消息的应答状态字符串 . */
const String& BaseMessage::GetStatusText() const
{
	return fStatusText;
}

/** 这个消息的请求 URL . */
const String& BaseMessage::GetURL() const
{
	return fUri;
}

/** 这个消息是否是一个请求消息 . */
BOOL BaseMessage::IsRequest()
{
	return fIsRequest;
}

/**
 * 返回指定的名称的消息头域的值.
 * @param name 头域的名称
 * @return 返回指定的头域的值, 如果不存在则返回空字符串.
 */
String BaseMessage::operator [](LPCSTR name)
{
	return GetHeader(name);
}

/**
 * 解析指定的字符串表示的消息.
 * @param message 要解析的消息字符串.
 * @return 如果解析成功则返回 TRUE, 否则返回 FALSE.
 */
BOOL BaseMessage::Parse( LPCSTR message )
{
	if (isempty(message)) {
		return FALSE;
	}

	fHeaders.Clear();
	String text = message;
	String::size_type pos = 0;	
	int count = 0;

	while (pos != String::npos) {
		String line = text.Tokenize("\r\n", pos);

		// 如果找到空行
		if (line.IsEmpty()) {
			String contentString = text.SubString(pos);
			SetContent(contentString.c_str(), contentString.GetLength());
			break;
		}

		// 如果是第一行
		if (count == 0) {
			// 如果没有以 \r\n 结尾或格式错误
			if (pos == String::npos || !ParseStartLine(line.c_str())) {
				return FALSE;
			}

		} else {
			fHeaders.AddLast(line);
		}
		count++;
	}

	return TRUE;
}

/** 
 * 解析指定的消息开始行. 
 * @param startLine 要解析的开始行字符串.
 * @return 如果解析成功则返回 TRUE, 否则返回 FALSE.
 */
BOOL BaseMessage::ParseStartLine( LPCSTR startLine )
{
	if (isempty(startLine)) {
		return FALSE;
	}

	String values[3];
	String text = startLine;
	String::size_type pos = 0;	
	int count = 0;

	while ((pos != String::npos) && (count < 2)) {
		values[count] = text.Tokenize(" ", pos);
		count++;
	}

	if (pos != String::npos) {
		values[count++] = text.SubString(pos);
	}

	if (count < 3) {
		return FALSE;
	}

	// 回应消息
	if (values[0].IndexOf('/') != String::npos) {
		fProtocol	= values[0];
		fStatusCode	= atoi(values[1].c_str());
		fStatusText	= values[2];
		fIsRequest	= FALSE;

		if (fProtocol.IsEmpty() || fStatusCode < 100 || fStatusCode >= 700) {
			return FALSE;
		}

	} else {
		fMethod		= values[0];
		fUri		= values[1];
		fProtocol	= values[2];
		fIsRequest	= TRUE;

		if (fMethod.IsEmpty() || fUri.IsEmpty() || fProtocol.IsEmpty()) {
			return FALSE;
		}
	}

	return TRUE;
}

/**
 * 删除指定名称的头字段.
 * @param name 字段名
 */
void BaseMessage::RemoveHeader( LPCSTR name )
{
	if (isempty(name)) {
		return;
	}

	size_t len = strlen(name);

	StringList::ListIterator iter = fHeaders.GetListIterator();
	while (iter.HasNext()) {
		String &line = iter.Next();

		if (len < line.GetLength()
			&& !strncasecmp(line.c_str(), name, len)
			&& line.GetAt(len) == ':') {

			iter.Remove();
		}
	}
}

int BaseMessage::Send()
{
	return 0;
}

/**
 * 设置消息的内容.
 * @param content 消息的内容
 * @param length 消息的内容的长度
 */
void BaseMessage::SetContent( LPCSTR content, size_t length )
{
	if (fContent != NULL) {
		delete[] fContent;
		fContent = NULL;
	}
	fContentLength = 0;

	if (content && length > 0 && length <= 1024 * 64) {
		fContent = new char[length + 1];
		memcpy(fContent, content, length);
		fContent[length] = '\0';
		fContentLength = length;
	}
}

/**
 * 设置指定的头字段的值, 如果已经存在多个同名的头字段, 则全部覆盖它们.
 * @param name 字段名
 * @param value 字段的值.
 */
void BaseMessage::SetHeader( LPCSTR name, LPCSTR value )
{
	if (isempty(name) || isempty(value)) {
		return;
	}

	RemoveHeader(name);

	String header = name;
	header += ": ";
	header += value;

	fHeaders.AddLast(header);
}


/**
 * 设置指定的头字段的值, 如果已经存在多个同名的头字段, 则全部覆盖它们.
 * @param name 字段名
 * @param value 字段的值.
 */
void BaseMessage::SetHeader( LPCSTR name, int value )
{
	char buf[64];
	sprintf(buf, "%d", value);
	SetHeader(name, buf);
}

void BaseMessage::SetMethod( LPCSTR method )
{
	fMethod = method ? method : "";
}

void BaseMessage::SetProtocol( LPCSTR scheme )
{
	fProtocol = scheme ? scheme : "";
}

void BaseMessage::SetRequest( BOOL isRequest )
{
	fIsRequest = isRequest;
}

void BaseMessage::SetStatusText( LPCSTR text )
{
	fStatusText = text ? text : "";
}

void BaseMessage::SetStatusCode( int code )
{
	fStatusCode = code;
}

void BaseMessage::SetURL( LPCSTR uri )
{
	fUri = uri ? uri : "";
}

/** 返回代表这个消息的字符串 */
String BaseMessage::ToString()
{
	return EncodeWithContent();
}

/** 重载 = 操作符. */
BaseMessage& BaseMessage::operator=( const BaseMessage& message )
{
	fIsRequest	= message.fIsRequest;
	fMethod		= message.fMethod;
	fProtocol	= message.fProtocol;
	fStatusCode	= message.fStatusCode;
	fStatusText	= message.fStatusText;
	fUri		= message.fUri;

	fHeaders.Clear();

	StringList* list = (StringList*)&(message.fHeaders);
	StringListIter iter = (StringListIter)list->GetListIterator();
	while (iter.HasNext()) {
		fHeaders.AddLast(iter.Next());
	}

	SetContent(message.fContent, message.fContentLength);

	return *this;
}

};
}
