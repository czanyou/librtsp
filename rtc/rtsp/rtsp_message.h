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
#ifndef _NS_VISION_RTSP_MESSAGE_H
#define _NS_VISION_RTSP_MESSAGE_H

#include "core/net/nio_message.h"
#include "rtc/vision_rtsp.h"
#include "rtsp_common.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspStreamingMode enum

/** RTSP/RTP Streaming Mode. */
enum RtspStreamingMode 
{
	TRANSPORT_UNKNOWN,
	TRANSPORT_UDP,		///< (RTP/AVP), RTP Over UDP, 
	TRANSPORT_TCP,		///< (RTP/AVP/TCP), RTP Over RTSP / RTP, RTSP 交错传输方式 
	TRANSPORT_HTTP		///< HTTP 隧道穿透传输方式.
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspMessage class

/** 
 * 代表 RTSP 的相关应答状态码和状态短语. 
 * 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspMessage : public BaseMessage
{
// Attributes -------------------------------------------------
public:
	/** 
	 * RTSP 协议定义的应答状态码. 
	 * Contains the values of status codes defined for RTSP.
	 */
	enum RtspStatusCode 
	{
		Continue				= 100,      ///< Continue

		OK						= 200,      ///< SuccessOK
		Created					= 201,      ///< SuccessCreated
		Accepted				= 202,      ///< SuccessAccepted
		NoContent				= 204,      ///< SuccessNoContent
		PartialContent			= 206,      ///< SuccessPartialContent
		LowOnStorage			= 250,      ///< SuccessLowOnStorage
		NoResponse				= 255,      ///< NoResponse

		MultipleChoices			= 300,      ///< MultipleChoices
		PermMoved				= 301,      ///< RedirectPermMoved
		TempMoved				= 302,      ///< RedirectTempMoved
		SeeOther				= 303,      ///< RedirectSeeOther
		NotModified				= 304,      ///< RedirectNotModified
		UseProxy				= 305,      ///< UseProxy

		BadRequest				= 400,      ///< ClientBadRequest
		UnAuthorized			= 401,      ///< ClientUnAuthorized
		PaymentRequired			= 402,      ///< PaymentRequired
		Forbidden				= 403,      ///< ClientForbidden
		NotFound				= 404,      ///< ClientNotFound

		/** 
		 * 请求的方法不允许用于请求 URI 指定的资源. 应答消息中必须包含一个 Allow
		 * 字段指定这个资源允许的所有方法.
		 */
		MethodNotAllowed		= 405,      ///< ClientMethodNotAllowed
		NotAcceptable			= 406,      ///< NotAcceptable
		ProxyAuthenticationRequired = 407,  ///< ProxyAuthenticationRequired
		RequestTimeout			= 408,      ///< RequestTimeout
		Conflict				= 409,      ///< ClientConflict
		Gone					= 410,      ///< Gone
		LengthRequired			= 411,      ///< LengthRequired
		PreconditionFailed		= 412,      ///< PreconditionFailed
		RequestEntityTooLarge	= 413,      ///< RequestEntityTooLarge
		RequestURITooLarge		= 414,      ///< RequestURITooLarge
		UnsupportedMediaType	= 415,      ///< UnsupportedMediaType
		
		/** 请求的接受方不支持请求中的一个或多个参数. */
		ParameterNotUnderstood	= 451,      ///< ClientParameterNotUnderstood

		/** 媒体服务器不知道 Conference 字段指定的 Conference. */
		ConferenceNotFound		= 452,      ///< ClientConferenceNotFound

		/** 这个请求因为没有足够的带宽而被拒绝. */
		NotEnoughBandwidth		= 453,      ///< ClientNotEnoughBandwidth

		/** Session 字段指定的 RTSP 会话已丢失, 无效的或者已经过期了. */
		SessionNotFound			= 454,      ///< ClientSessionNotFound

		/** 客户端或服务端不存在当前状态下处理这个请求. */
		MethodNotValidInState	= 455,      ///< ClientMethodNotValidInState

		/** 服务端无法处理指定的头字段. */
		HeaderFieldNotValid		= 456,      ///< ClientHeaderFieldNotValid

		/** 给定的 Range 值超出范围. */
		InvalidRange			= 457,      ///< ClientInvalidRange

		/** 在 SET_PARAMETER 请求中要设置的参数只能读不能修改. */
		ReadOnlyParameter		= 458,      ///< ClientReadOnlyParameter
		AggregateOptionNotAllowed = 459,    ///< ClientAggregateOptionNotAllowed
		AggregateOptionAllowed	= 460,      ///< ClientAggregateOptionAllowed

		/** Transport 头字段中不包含支持的传输方式. */
		UnsupportedTransport	= 461,      ///< ClientUnsupportedTransport

		/** 客户端地址无法到达, 所以无法建立数据传输通道. */
		DestinationUnreachable	= 462,      ///< ClientDestinationUnreachable

		InternalError			= 500,      ///< ServerInternal
		NotImplemented			= 501,      ///< ServerNotImplemented
		BadGateway				= 502,      ///< ServerBadGateway
		ServerUnavailable		= 503,      ///< ServerUnavailable
		GatewayTimeout			= 504,		///< ServerGatewayTimeout
		RTSPVersionNotSupported	= 505,		///< RTSPVersionNotSupported

		/** 不支持 Require 或 Proxy-Require 头字段中给定的选项. */
		OptionNotSupported		= 551		///< OptionNotSupported
	};

public:
	RtspMessage(void);
	RtspMessage(LPCSTR method, LPCSTR url, LPCSTR protocol);
	RtspMessage(int statusCode, LPCSTR statusText);

// Operations -------------------------------------------------
public:
	static LPCSTR GetStatusString(int code);

public:
	IRtspConnectionPtr GetRtspConnection();

	int  Send();
	void SetRtspConnection(IRtspConnection* rtspConnection);

// Data Members -----------------------------------------------
private:
    static const char*	fStatusTexts[];		///< RTSP status code strings
	static const int	fStatusCodes[];		///< RTSP status codes

	IRtspConnectionPtr fRtspConnection;
};

typedef SmartPtr<RtspMessage> RtspMessagePtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspTransport class

/** 
 * 代表一个 RTSP 传输头字段. 
 * - ttl: Multicast time-to-live
 * - unicast | multicast: 指出是使用单播还是多播
 * - destination: 数据流发送的目的地址.
 * - mode: 指出这个会话支持的 RTSP 方法, 有效的值为 PLAY 和 RECORD, 默认为 PLAY
 * - interleaved: 表示混合传输时, 即 RTSP/TCP 方式时, $ 的参数值.
 * - port: 指出了多播的 RTP 和 RTCP 端口.
 * - client_port: 这个参数提供了单播的客户端用于接收 RTP/RTCP 信息的端口.
 * - server_port: 这个参数提供了单播的服务端用于接收 RTP/RTCP 信息的端口.
 * - ssrc: 指定了 RTP 的 SSRC 值.
 * 
 * @author ChengZhen (anyou@msn.com)
 */
class RtspTransport
{
public:
	RtspTransport();
	RtspTransport(const RtspTransport& transport);

// Operations --------------------------------------------------
public:
	RtspTransport& operator = (const RtspTransport& transport);
	int  Parse(LPCSTR buf);
	void Clear();
	String ToString();

// Data Members -----------------------------------------------
public:
	RtspStreamingMode fStreamingMode;	///< stream, 传输模式
	int  fClientPort;		///< client_port, 客户端 RTP 端口
	char fDestination[36];	///< destination, 目的地址
	int  fInterleaved;		///< interleaved, RTP 通道号. (TCP)
	BOOL fIsMultcast;		///< multcast/unicast, 指出是否为组播还是单播
	int  fServerPort;		///< server_port, 服务端 RTP 端口
	int  fTtl;				///< ttil, TTL
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspRange class

/** 
 * 代表 RTSP 的 Range 头. 
 * 
 * @author ChengZhen (anyou@msn.com)
 */
class RtspRange 
{
public:
	RtspRange();

// Operations --------------------------------------------------
public:
	BOOL Parse(LPCSTR value);
	String ToString();

// Data Members -----------------------------------------------
public:
	double fStart;		///< 开始时间
	double fEnd;		///< 结束时间
};

}; // namespace rtsp


#endif // #ifndef _NS_VISION_RTSP_MESSAGE_H
