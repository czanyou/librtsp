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

#include "core/net/nio_url.h"
#include "core/utils/md5.h"
#include "rtsp_auth.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspAuth class

RtspAuthorization::RtspAuthorization()
{
	fEnabled		= FALSE;
	fFeed			= StringUtils::RandomString(64);
	fPassFileName	= "/etc/passfile";

#ifdef HI3518E
	fPassFileName	= "/system/etc/passfile";

#endif
}

/**
 * RTSP 也使用 HTTP 规定的认证方式. 具体的细节可以参考 HTTP 协议相关章节.
 * @param connection 
 * @param request 
 * @return 如果通过认证则返回 TRUE.
 */
BOOL RtspAuthorization::CheckAuthorization(String address, BaseMessage* request, String& username)
{
	if (!IsEnabled()) {
		return TRUE; // 没有启用身份认证

	} else if (address == "127.0.0.1") {
		return TRUE; // 对于本地连接不做认证

	} else if (request->GetMethod() == "OPTIONS") {
		return TRUE; // 对于 OPTIONS 不做认证
	}

	Uri url(request->GetURL());
	String k = url.GetParameter("k");
	if (!k.IsEmpty() && !address.IsEmpty()) {
		String key = fFeed + address;

		MD5 md5;
		key = md5.Hash((BYTE*)key.c_str(), key.GetLength());
		if (key == k) {
			return TRUE;
		}
	}

	String auth = request->GetHeader("Authorization");
	if (auth.StartsWith("Basic ")) {
		username = CheckBasicAuthorization(request, auth);		

	} else if (auth.StartsWith("Digest ")) {
		username = CheckDigestAuthorization(request, auth);		
	}

	if (!username.IsEmpty()) {
		return TRUE;
	}

	return FALSE;
}

/** Basic 认证. */
String RtspAuthorization::CheckBasicAuthorization(BaseMessage* request, String& auth)
{
	MD5 md5;
	char authinfo[500];

	/* Decode it by base64. */
	int l = StringUtils::Base64Decode(auth.SubString(6).c_str(), (BYTE*)authinfo, sizeof(authinfo));
	authinfo[l] = '\0';
	auth = authinfo;

	int pos = auth.IndexOf(":", 0);
	if (pos == String::npos) {
		return "";
	}
	String username = auth.SubString(0, pos);
	String password = auth.SubString(pos + 1);

	String ha1 = ReadPassfile(username);
	if (ha1.IsEmpty()) {
		return "";
	}

	String k = username + ":ipcam:" + password;
	k = md5.Hash((BYTE*)k.c_str(), k.GetLength());
	if (k != ha1) {
		return "";
	}

	return username;
}

/** Digest 认证. */
String RtspAuthorization::CheckDigestAuthorization(BaseMessage* request, String& auth)
{
	MD5 md5;

	if (!ParseAuthorization(auth)) {
		return "";
	}

	String ha1 = ReadPassfile(fUsername);
	if (ha1.IsEmpty()) {
		return "";
	}

	String a2 = request->GetMethod() + ":" + fUri;
	String ha2 = md5.Hash((BYTE*)a2.c_str(), a2.GetLength());

	String a;
	if (fCnonce.IsEmpty()) {
		a = ha1 + ":" + fNonce + ":" + ha2;

	} else {
		a = ha1 + ":" + fNonce + ":" + fNc 
			+ ":" + fCnonce + ":" + fQop + ":" + ha2;
	}

	String hak = md5.Hash((BYTE*)a.c_str(), a.GetLength());
	if (hak != fResponse) {
		return "";
	}

	return fUsername;
}

String RtspAuthorization::GetAuthorizationFeed()
{
	return fFeed;
}

/** 指出是否启用了 RTSP 身份认证. */
BOOL RtspAuthorization::IsEnabled()
{
	return fEnabled;
}

/**
 * 解析指定的 头域的值.
 * @param header 头域的值.
 */
BOOL RtspAuthorization::ParseAuthorization( String header )
{
	fCnonce	  = "";
	fNc		  = "";
	fNonce	  = "";
	fQop	  = "";
	fRealm	  = "";
	fResponse = "";
	fUri	  = "";
	fUsername = "";

	if (header.IsEmpty() || !header.StartsWith("Digest ")) {
		return FALSE;
	}

	Lex lex(header.SubString(7));
	while (lex.HasMore()) {
		String name;
		String value;
		lex.SkipSpace();

		if (!lex.NextToken('=', name)) {
			break;
		}
		
		name = name.Trim();

		lex.SkipSpace();
		if (lex.LookChar() == '"') {
			lex.NextChar();
			if (!lex.NextToken('"', value)) {
				break;
			}
			lex.SkipSpace();
			lex.NextChar();

		} else {
			if (!lex.NextToken(',', value)) {
				break;
			}
		}

		SetParameter(name, value.Trim());
	}

	return TRUE;
}

/**
 * 返回指定的用户的 HA1 码.
 * @param user 用户名
 * @return 返回这个用户的 HA1 码, 如果不存在则返回一个空串.
 */
String RtspAuthorization::ReadPassfile(String user)
{
	char line[256];
	FILE* file = fopen(fPassFileName, "r");
	if (file == NULL) {
		return "";
	}

	String result;

	// 每一行对应一个用户的所有信息, 以 ':' 隔开
	while (fgets(line, sizeof(line), file) != NULL) {
		char* p = strchr(line, ':');
		if (p == NULL) {
			continue;
		}

		// 第一个参数是用户名
		*p = '\0';
		if (user != line) {
			continue;
		}

		// 第二个参数是域名
		p++;
		p = strchr(p, ':');
		if (p == NULL) {
			continue;
		}

		// 第三个参数是 HA1 码
		p++;
		int len = strlen(p);
		for (int i = len - 1; i > 0; i--) {
			// 删除结尾的空白字符
			if (p[i] == '\r' || p[i] == '\n' || p[i] == ' ') {
				p[i] = '\0';
			}
		}
		result = p;
		break;
	}

	fclose(file);
	return result;
}

/** 设置是否启用 RTSP 身份认证. */ 
void RtspAuthorization::SetEnabled( BOOL enabled )
{
	if (fEnabled != enabled) {
		fEnabled = enabled;
	}
}

void RtspAuthorization::SetParameter( String name, String value )
{
	if (name == "username") {
		fUsername = value;

	} else if (name == "cnonce") {
		fCnonce = value;

	} else if (name == "response") {
		fResponse = value;

	} else if (name == "uri") {
		fUri = value;

	} else if (name == "qop") {
		fQop = value;

	} else if (name == "nc") {
		fNc = value;

	} else if (name == "nonce") {
		fNonce = value;

	} else if (name == "realm") {
		fRealm = value;
	}
}

void RtspAuthorization::SetPassFileName( LPCSTR filename )
{
	fPassFileName = filename ? filename : "";
}

};
