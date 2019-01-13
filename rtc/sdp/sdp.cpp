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

#include "sdp.h"
#include "core/utils/string_utils.h"

namespace sdp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpRtpMap class

SdpRtpMap::SdpRtpMap()
{
	fChannels		= 0;
	fFrequency		= 1000;
	fPayload		= 0;
}

/**
 * 解析指定的 rtpmap 字符串.
 * @param rtpmap 
 * @return int 
 */
int SdpRtpMap::Parse( String& rtpmap )
{
	// a=rtpmap:96 H264/90000
	// a=rtpmap:97 MP4A-LATM/48000/2  rtpmap:8 PCMA/8000/1
	Lex lex(rtpmap);

	lex.SkipSpace();
	if (!lex.NextNumber(fPayload)) {
		return -1;
	}

	lex.SkipSpace(); 
	if (lex.NextToken('/', fFormat)) {
		lex.LookToken("/");
		lex.NextNumber(fFrequency);
		if (lex.LookToken("/")) {
			lex.NextNumber(fChannels); 
		}

	} else {
		fFormat = lex.GetRemains();
	}

	return 0;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpAttribute class

SdpAttribute::SdpAttribute()
{

}

SdpAttribute::SdpAttribute( LPCSTR name, LPCSTR value )
{
	fName	= name ? name : "";
	fValue	= value ? value : "";
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpMedia class

SdpMedia::SdpMedia()
{
	fMediaPort		= 0;
	fPayload		= 0;
	fPortCount		= 0;
}

/**
 * 解析 m= 行.
 * @param value m= 行的值.
 * @return SdpMedia* 返回新创建的 SdpMedia 对象的指针.
 */
int SdpMedia::ParseMediaLine(String& line)
{
	//m=<media> <port>/<number of ports> <transport> <fmt list>
	int index = 0;
	
	String::size_type pos = 0;
	while (pos != String::npos) {
		String token = line.Tokenize(" ", pos);

		switch (index) {
		case 0: fMediaType	= token;		break;
		case 1: fMediaPort	= atoi(token);	break;
		case 2: fProtocol	= token;		break;
		case 3: fPayload	= atoi(token);	break;
		}

		index++;
	}

	return 0;
}

int SdpMedia::ParseLine( char type, String& line )
{
	if (type == 'a') {
		int pos = 0;
		String name  = line.Tokenize(":", pos).Trim();
		String value = line.Tokenize(":", pos).Trim();

		if (name == "rtpmap") {
			fRtpMap.Parse(value);

		} else if (name == "control") {
			fControl = value;
		}

		SdpAttributePtr attribute = new SdpAttribute(name, value);
		fAttributes.Add(attribute);
	}

	return 0;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpSession class

SdpSession::SdpSession()
{
	fVersion		= 0;
}

SdpSession::~SdpSession()
{
	Clear();
}

/** 清空所有的内容和属性. */
void SdpSession::Clear()
{
	fVersion	= 0;
	fConnectionAddress.Clear();
	fMedias.Clear();
	fOrigin.Clear();
	fSessionName.Clear();
	fAttributes.Clear();
}

String SdpSession::GetAttribute( LPCSTR name )
{
	if (isempty(name)) {
		return "";
	}

	for (UINT i = 0; i < fAttributes.GetLength(); i++) {
		SdpAttributePtr attribute = fAttributes.Get(i);
		if (attribute == NULL) {
			continue;

		} else if (attribute->fName == name) {
			return attribute->fValue;
		}
	}

	return "";
}



String SdpSession::GetConnectionAddress()
{
	return fConnectionAddress;
}

/** 
 * 返回指定的索引的 Media 对象. 
 * @param index 要返回的 Media 对象的索引, 从 0 开始.
 * @return Media 对象. 
 */
SdpMediaPtr SdpSession::GetSdpMedia(UINT index) 
{
	if (index >= fMedias.GetLength()) {
		return NULL;
	}
	
	return fMedias[index];
} 

/** 返回当前会话包含的 Media 项的数目. */
UINT SdpSession::GetSdpMediaCount()
{
	return fMedias.GetLength();
}

/** 
 * 解析指定的 SDP 字符串. 
 * @param sdp 要解析的 SDP 字符串. 
 * @return 如果成功则返回 0.
 */
int SdpSession::Parse(LPCSTR sdp)
{
	if (isempty(sdp)) {
		LOG_D("empty sdp string\r\n")
		return -1;
	}
	
	Clear();

	String sdpString = sdp;

	int ret = 0;
	SdpMediaPtr sdpMedia;

	int pos = 0;
	while (pos >= 0) {
		String line = sdpString.Tokenize("\n", pos).Trim();
		if (line.IsEmpty()) {
			continue;
		}

		char type = line.GetAt(0);
		if (!isalpha(type) || (line.GetAt(1) != '=')) {
			break;
		}

		String value = line.SubString(2);
		if (type == 'm') {
			sdpMedia = new SdpMedia();
			sdpMedia->ParseMediaLine(value);
			fMedias.Add(sdpMedia);

		} else if (sdpMedia) {
			// Media level parameters
			sdpMedia->ParseLine(type, value);

		} else {
			// Session level parameters
			ParseLine(type, value);
		}
	}

	return ret;
}

int SdpSession::ParseConnection( String& line )
{
	int pos = 0; 
	line.Tokenize(" ", pos);
	line.Tokenize(" ", pos);
	fConnectionAddress = line.Tokenize(" ", pos);
	return 0;
}

int SdpSession::ParseLine( char type, String& value )
{
	switch (type) {
	case 'v': fVersion		= atoi(value);	break;	// v=<version>
	case 'o': fOrigin		= value;		break;	// o=<Origin Info>
	case 's': fSessionName	= value;		break;	// s=<Session Name>
	case 'c': ParseConnection(value);		break;	// c=<Connection Info>
	case 't': /* TODO: */					break;	// t=<Time>
	case 'a':
		{
			int pos = 0;
			String name = value.Tokenize(":", pos);
			String attr = value.Tokenize(":", pos).Trim();
			SdpAttributePtr attribute = new SdpAttribute(name, attr);
			fAttributes.Add(attribute);
			break;
		}
	}

	return 0;
}

};
