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
#ifndef _NS_VISION_SDP_SESSION_H
#define _NS_VISION_SDP_SESSION_H

/**
 * SDP 文档解析模块.
 *	Session description
 *       - v=  (protocol version)
 *       - o=  (owner/creator and session identifier).
 *       - s=  (session name)
 *       - i=* (session information)
 *
 *       - u=* (URI of description)
 *       - e=* (email address)
 *       - p=* (phone number)
 *       - c=* (connection information - not required if included in all media)
 *       - b=* (bandwidth information)
 *       - One or more time descriptions (see below)
 *       - z=* (time zone adjustments)
 *       - k=* (encryption key)
 *       - a=* (zero or more session attribute lines)
 *       - Zero or more media descriptions (see below)
 *
 *	Time description
 *       - t=  (time the session is active)
 *       - r=* (zero or more repeat times)
 *
 *	Media description
 *       - m=  (media name and transport address)
 *       - i=* (media title)
 *       - c=* (connection information - optional if included at session-level)
 *       - b=* (bandwidth information)
 *       - k=* (encryption key)
 *       - a=* (zero or more media attribute lines)
 */
namespace sdp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpRtpMap class

/** 
 * SdpRtpMap 代表 SDP Media 的 rtpmap 属性. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class SdpRtpMap
{
public:
	SdpRtpMap();

// Operations -------------------------------------------------
public:
	int Parse(String& rtpmap);

// Data Members -----------------------------------------------	
public:
	int    fChannels;	///< 通道数
	String fFormat;		///< 编解码格式类型 
	int    fFrequency;	///< 时间频率
	int    fPayload;	///< 负载类型

};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpAttribute class

/**
 * 代表一个 SDP 属性
 *
 * @author ChengZhen (anyou@msn.com)
 */
class SdpAttribute : public Object
{
public:
	SdpAttribute();
	SdpAttribute(LPCSTR name, LPCSTR value);

// Data Members -----------------------------------------------	
public:
	String fName;	///< 属性名
	String fValue;	///< 属性值
};

typedef SmartPtr<SdpAttribute> SdpAttributePtr;
typedef ArrayList<SdpAttributePtr> SdpAttributeArray;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpMedia class

/** A Media represents an m= field contained within a MediaDescription. 
 * The Media identifies information about the format(s) of the media associated 
 * with the MediaDescription.
 *
 * The Media field includes:
 *   - a mediaType (e.g. audio, video, etc.)
 *   - a port number (or set of ports)
 *   - a protocol to be used (e.g. RTP/AVP)
 *   - a set of media formats which correspond to Attributes associated with the
 *		media description.
 *
 * Here is an example:
 *
 * <code>
 * m=audio 60000 RTP/AVP 0 <br/>
 * a=rtpmap:0 PCMU/8000
 * </code>
 *
 * This example identifies that the client can receive audio on port 60000 in 
 * format 0 which corresponds to PCMU/8000.
 *
 * Please refer to IETF RFC 2327 for a description of SDP.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class SdpMedia : public Object
{
public:
	SdpMedia();
	
// Operations -------------------------------------------------
public:
	int  ParseLine(char type, String& line);
	int  ParseMediaLine(String& line);

// Data Members -----------------------------------------------	
public:
	SdpAttributeArray fAttributes;
	SdpRtpMap fRtpMap;	///< 

	String fControl;	///< 
	int    fMediaPort;	///< the port of the media defined by this description
	String fMediaType;	///< the type (audio,video etc) of the media defined by this description.
	int    fPayload;	///< the payload type of the media defined by this description
	int    fPortCount;	///< the number of ports associated with this media description
	String fProtocol;	///< the protocol over which this media should be transmitted.

};

typedef SmartPtr<SdpMedia> SdpMediaPtr;
typedef ArrayList<SdpMediaPtr> SdpMediaArray;
	
//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// SdpSession class

/** SdpSession 代表了会话描述协议 (IETF RFC 2327) 定义的数据.
 * A SdpSession represents the data defined by the SDP Session Description 
 * Protocol (see IETF RFC 2327) and holds information about the originator of 
 * a session, the media types that a client can support and the host and port 
 * on which the client will listen for that media.
 *
 * The SdpSession also holds timing information for the session (e.g. start, end,
 * repeat, time zone) and bandwidth supported for the session.
 *
 * Please refer to IETF RFC 2327 for a description of SDP.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class SdpSession  
{
public:
	SdpSession();
	virtual ~SdpSession();
	
// Attributes -------------------------------------------------
public:
	SdpMediaPtr GetSdpMedia(UINT index);

	String GetConnectionAddress();
	UINT   GetSdpMediaCount();
	String GetAttribute(LPCSTR name);

// Operations -------------------------------------------------
public:
	void Clear();
	int  Parse(LPCSTR sdp);

private:
	int  ParseConnection(String& value);
	int  ParseLine(char type, String& value);

// Data Members -----------------------------------------------	
private:
	SdpMediaArray fMedias;		///< SDP Media 列表
	SdpAttributeArray fAttributes;

	String fConnectionAddress;	///< the connection information associated with this object.
	String fOrigin;				///< information about the originator of the session. This corresponds to the o= field
	String fSessionName;		///< the name of the session. This corresponds to the s= field of the SDP data.
	UINT   fVersion;			///< the version of SDP in use. This corresponds to the v= field of the SDP data.
};

}; // namespace media

#endif // !defined(_NS_VISION_SDP_SESSION_H)

