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
#ifndef _NS_VISION_CORE_NIO_MESSAGE_H
#define _NS_VISION_CORE_NIO_MESSAGE_H

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// BaseMessage class

/**
 * 一个 HTTP/SIP 风格的消息创建, 解析实现类. 
 * 可以使用这个类来生成或解析 HTTP/SIP 等消息.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class BaseMessage : public Object
{
public:
	BaseMessage(void);
	BaseMessage(LPCSTR method, LPCSTR url, LPCSTR protocol);
	BaseMessage(int statusCode, LPCSTR statusText);
	BaseMessage(const BaseMessage& message);
	virtual ~BaseMessage(void);

	typedef LinkedList<String> StringList;	///< 字符串列表
	typedef LinkedList<String>::ListIterator StringListIter;	///< 字符串列表迭代器类型

// Attributes -------------------------------------------------
public:
	const String& GetMethod() const;	///< 返回这个消息的方法名
	const String& GetURL() const;		///< 返回这个消息的请求 URI
	const String& GetProtocol() const;	///< 返回这个消息的协议类型
	const String& GetStatusText() const;///< 返回这个消息的状态字符串

	int    GetStatusCode();				///< 返回这个消息的状态码
	LPCSTR GetContent();
	UINT   GetContentLength();
	BOOL   IsRequest();					///< 指出是否是请求消息

	void   SetMethod(LPCSTR method);	///< 设置这个消息的方法名
	void   SetProtocol(LPCSTR scheme);	///< 设置这个消息的协议类型
	void   SetRequest(BOOL isRequest);	///< 设置这个消息的类型
	void   SetStatusText(LPCSTR text);	///< 设置这个消息的状态字符串
	void   SetStatusCode(int code);		///< 设置这个消息的状态码
	void   SetURL(LPCSTR uri);			///< 设置这个消息的请求 URI

// Operations -------------------------------------------------
public:
	void AddHeader(LPCSTR name, LPCSTR value);
	void Clear();
	void ClearHeaders();
	int  GetHeaderInt(LPCSTR name, int defaultValue = 0);
	BOOL GetHeaders( LPCSTR name, StringList &values );
	BOOL Parse(LPCSTR message);
	BOOL ParseStartLine(LPCSTR startLine);
	void RemoveHeader(LPCSTR name);
	void SetContent(LPCSTR content, size_t length);
	void SetHeader(LPCSTR name, LPCSTR value);
	void SetHeader(LPCSTR name, int value);

	String EncodeWithContent();
	String EncodeWithoutContent();
	String GetHeader(LPCSTR name);
	String ToString();

public:
	virtual int Send();

public:
	String operator [](LPCSTR name);
	BaseMessage& operator =(const BaseMessage& message);

// Data Members -------------------------------------
private:
	StringList fHeaders;	///< 消息头字段列表

	char*  fContent;		///< 消息内容
	UINT   fContentLength;	///< 消息内容的长度
	BOOL   fIsRequest;		///< 指出是否为请求消息
	String fMethod;			///< 方法名, 当这是一个请求消息才有效
	String fProtocol;		///< 消息使用的协议和版本
	int    fStatusCode;		///< 状态代码, 当这是一个回应消息才有效
	String fStatusText;		///< 状态字符串, 当这是一个回应消息才有效
	String fUri;			///< 请求的 URL, 当这是一个请求消息才有效
};

typedef SmartPtr<BaseMessage> BaseMessagePtr;

};
};

using namespace core::net;

#endif // _NS_VISION_CORE_NIO_MESSAGE_H
