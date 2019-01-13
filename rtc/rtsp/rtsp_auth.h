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
#ifndef _NS_VISION_RTSP_AUTH_H
#define _NS_VISION_RTSP_AUTH_H

#include "rtsp_common.h"
#include "rtsp_message.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspAuthorization class

/**
 * 代表 RTSP 身份认证模块.
 *
 * @author ChengZhen (anyou@msn.com)
 */ 
class RtspAuthorization
{
public:
	RtspAuthorization();

// Attributes -------------------------------------------------
public:
	String GetAuthorizationFeed();	///< 返回认证种子值
	String GetCnonce()		{ return fCnonce; }
	String GetNc()			{ return fNc; }
	String GetNonce()		{ return fNonce; }
	String GetQop()			{ return fQop; }
	String GetResponse()	{ return fResponse; }
	String GetRealm()		{ return fRealm; }
	String GetUri()			{ return fUri; }
	String GetUsername()	{ return fUsername; }
	BOOL   IsEnabled();
	void   SetEnabled( BOOL enabled );
	void   SetPassFileName(LPCSTR filename);

// Operations -------------------------------------------------
public:
	BOOL   CheckAuthorization(String address, BaseMessage* request, String& username);
	String CheckBasicAuthorization (BaseMessage* request, String& auth);
	String CheckDigestAuthorization(BaseMessage* request, String& auth);
	BOOL   ParseAuthorization(String header);
	String ReadPassfile(String user);

// Implementation ---------------------------------------------
private:
	void   SetParameter( String name, String value );

// Data Members -----------------------------------------------
private:
	String fCnonce;			///< 客户端提供的 Nonce 参数
	String fFeed;			///< RTSP 认证种子, 主要是一组随机字符, 用于提供安全性
	String fNc;				///< 客户端请求序号, 以 1 递增
	String fNonce;			///< 服务端提供的 Nonce 参数
	String fPassFileName;	///< 
	String fQop;			///< GOP
	String fRealm;			///< 
	String fResponse;		///< 客户端响应
	String fUri;			///< 验证的请求 URI
	String fUsername;		///< 验证的用户名
	BOOL   fEnabled;		///< 是否启用 RTSP 认证
};

};

#endif // _NS_VISION_RTSP_AUTH_H
