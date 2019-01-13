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
#ifndef _NS_VISION_CORE_NIO_URL_H
#define _NS_VISION_CORE_NIO_URL_H

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Uri class

/** 
 * 提供统一资源标识符 (URI) 的对象表示形式和对 URI 各部分的轻松访问。
 *
 * URL 也分为相对 URL 和绝对 URL, 相对 URL 不包含 URL 的所有部分.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Uri
{
public:
	Uri();
	Uri(LPCSTR url);

// Attributes -------------------------------------------------
public:
	String GetHost();
	String GetHostName();
	String GetHref();
	String GetPath();
	String GetPathName();
	String GetProtocol();
	String GetQuery();
	UINT   GetPort();

	void SetHostName(LPCSTR hostName);
	void SetHref(LPCSTR href);
	void SetProtocol(LPCSTR protocol);
	void SetPathName(LPCSTR pathName);
	void SetQuery(LPCSTR query);
	void SetUserName(LPCSTR userName);
	void SetPassword(LPCSTR password);
	void SetHash(LPCSTR hash);
	void SetPort(UINT port);

// Operations -------------------------------------------------
public:
	int    GetDefaultPort();
	String GetParameter( LPCSTR name );
	BOOL   GetParameter( LPCSTR name, String& value );
	int	   GetParameterInt( LPCSTR name, int defaultValue = -1 );
	BOOL   HasParameter( LPCSTR name );
	int    Parse( LPCSTR url  );
	String ToString();

// Static Methods ---------------------------------------------
public:
	static String DecodeUriComponent( LPCSTR fromConvert );
	static String EncodeUriComponent( LPCSTR toConvert );

// Data members -----------------------------------------------
public:
	String fHref;		///< 这个 URI 的原始字符串.
	String fProtocol;	///< 这个 URI 的方案名称
	String fHostName;	///< 这个 URI 的主机地址部分
	String fPathName;	///< 这个 URI 的路径部分
	String fQuery;		///< 这个 URI 的包含的任何查询信息
	String fUserName;	///< 这个 URI 的用户名信息
	String fPassword;	///< 这个 URI 的密码信息
	String fHash;		///< 这个 URI 的锚点（也称为“引用”）
	UINT   fPort;		///< 这个 URI 的端口号

private:
	static LPCSTR fSafeChars; ///< URL 字符串中允许出现的符号
};

};
};

using namespace core::net;

#endif // _NS_VISION_CORE_NIO_URL_H
