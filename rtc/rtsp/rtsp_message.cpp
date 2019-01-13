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

#include "rtsp_message.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspMessage class

/** 状态短语列表. */
LPCSTR RtspMessage::fStatusTexts[] =
{
    "Continue",                              // 100 Continue

    "OK",                                    // 200 SuccessOK
    "Created",                               // 201 SuccessCreated
    "Accepted",                              // 202 SuccessAccepted
    "No Content",                            // 204 SuccessNoContent
    "Partial Content",                       // 206 SuccessPartialContent
    "Low on Storage Space",                  // 250 SuccessLowOnStorage
	"No Response",							 // 255 No Response

    "Multiple Choices",                      // 300 MultipleChoices
    "Moved Permanently",                     // 301 RedirectPermMoved
    "Found",                                 // 302 RedirectTempMoved
    "See Other",                             // 303 RedirectSeeOther
    "Not Modified",                          // 304 RedirectNotModified
    "Use Proxy",                             // 305 UseProxy
	
    "Bad Request",                           // 400 ClientBadRequest
    "Unauthorized",                          // 401 ClientUnAuthorized
    "Payment Required",                      // 402 PaymentRequired
    "Forbidden",                             // 403 ClientForbidden
    "Not Found",                             // 404 ClientNotFound
    "Method Not Allowed",                    // 405 ClientMethodNotAllowed
    "Not Acceptable",                        // 406 NotAcceptable
    "Proxy Authentication Required",         // 407 ProxyAuthenticationRequired
    "Request Time-out",                      // 408 RequestTimeout
    "Conflict",                              // 409 ClientConflict

    "Gone",                                  // 410 Gone
    "Length Required",                       // 411 LengthRequired
    "Precondition Failed",                   // 412 PreconditionFailed
    "Request Entity Too Large",              // 413 RequestEntityTooLarge
    "Request-URI Too Large",                 // 414 RequestURITooLarge
    "Unsupported Media Type",                // 415 UnsupportedMediaType

    "Parameter Not Understood",              // 451 ClientParameterNotUnderstood
    "Conference Not Found",                  // 452 ClientConferenceNotFound
    "Not Enough Bandwidth",                  // 453 ClientNotEnoughBandwidth
    "Session Not Found",                     // 454 ClientSessionNotFound
    "Method Not Valid in this State",        // 455 ClientMethodNotValidInState
    "Header Field Not Valid For Resource",   // 456 ClientHeaderFieldNotValid
    "Invalid Range",                         // 457 ClientInvalidRange
    "Parameter Is Read-Only",                // 458 ClientReadOnlyParameter
    "Aggregate Option Not Allowed",          // 459 ClientAggregateOptionNotAllowed
    "Only Aggregate Option Allowed",         // 460 ClientAggregateOptionAllowed
    "Unsupported Transport",                 // 461 ClientUnsupportedTransport
    "Destination Unreachable",               // 462 ClientDestinationUnreachable

    "Internal Server Error",                 // 500 ServerInternal
    "Not Implemented",                       // 501 ServerNotImplemented
    "Bad Gateway",                           // 502 ServerBadGateway
    "Service Unavailable",                   // 503 ServerUnavailable
    "Gateway Timeout",                       // 504 ServerGatewayTimeout
    "RTSP Version not supported",            // 505 RTSPVersionNotSupported
    "Option Not Supported"                   // 551 OptionNotSupported
};

const int RtspMessage::fStatusCodes[] =
{
    100,        ///< Continue

    200,        ///< SuccessOK
    201,        ///< SuccessCreated
    202,        ///< SuccessAccepted
    204,        ///< SuccessNoContent
    206,        ///< SuccessPartialContent
    250,        ///< SuccessLowOnStorage
	255,        ///< NoResponse

    300,        ///< MultipleChoices
    301,        ///< RedirectPermMoved
    302,        ///< RedirectTempMoved
    303,        ///< RedirectSeeOther
    304,        ///< RedirectNotModified
    305,        ///< UseProxy

    400,        ///< ClientBadRequest
    401,        ///< ClientUnAuthorized
    402,        ///< PaymentRequired
    403,        ///< ClientForbidden
    404,        ///< ClientNotFound
    405,        ///< ClientMethodNotAllowed
    406,        ///< NotAcceptable
    407,        ///< ProxyAuthenticationRequired
    408,        ///< RequestTimeout
    409,        ///< ClientConflict
    410,        ///< Gone
    411,        ///< LengthRequired
    412,        ///< PreconditionFailed
    413,        ///< RequestEntityTooLarge
    414,        ///< RequestURITooLarge
    415,        ///< UnsupportedMediaType
    451,        ///< ClientParameterNotUnderstood
    452,        ///< ClientConferenceNotFound
    453,        ///< ClientNotEnoughBandwidth
    454,        ///< ClientSessionNotFound
    455,        ///< ClientMethodNotValidInState
    456,        ///< ClientHeaderFieldNotValid
    457,        ///< ClientInvalidRange
    458,        ///< ClientReadOnlyParameter
    459,        ///< ClientAggregateOptionNotAllowed
    460,        ///< ClientAggregateOptionAllowed
    461,        ///< ClientUnsupportedTransport
    462,        ///< ClientDestinationUnreachable

    500,        ///< ServerInternal
    501,        ///< ServerNotImplemented
    502,        ///< ServerBadGateway
    503,        ///< ServerUnavailable
    504,        ///< ServerGatewayTimeout
    505,        ///< RTSPVersionNotSupported
    551         ///< ServerOptionNotSupported
};

RtspMessage::RtspMessage( void ) : BaseMessage()
{

}

RtspMessage::RtspMessage( LPCSTR method, LPCSTR url, LPCSTR protocol )
: BaseMessage(method, url, protocol)
{

}

RtspMessage::RtspMessage( int statusCode, LPCSTR statusText )
: BaseMessage(statusCode, statusText)
{

}

IRtspConnectionPtr RtspMessage::GetRtspConnection()
{
	return fRtspConnection;
}

/** Get specified status string 
 * @param code status code
 * @return Returns the status string if specified status exist, Otherwise returns NULL.
 */
LPCSTR RtspMessage::GetStatusString(int code)
{
	int size = sizeof(fStatusCodes) / sizeof(fStatusCodes[0]);
	for (int i = 0; i < size; i++) {
		if (code == fStatusCodes[i]) {
			return fStatusTexts[i];
		}
	}

	return NULL;
}


int RtspMessage::Send()
{
	//RtspConnectionPtr rtspConnection = dynamic_cast<RtspConnection*>(fRtspConnection.ptr);
	//if (rtspConnection == NULL) {
	//	LOG_D("Invalid fRtspConnection");
	//	return -1;
	//}

	//return rtspConnection->SendResponse(this);

	return 0;
}

void RtspMessage::SetRtspConnection( IRtspConnection* rtspConnection )
{
	//fRtspConnection = rtspConnection;
}


//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
// RtspTransport class

/** 构建一个新的 RtspTransport 对象. */
RtspTransport::RtspTransport() 
{
	Clear();
}

/** 构建一个新的 RtspTransport 对象. */
RtspTransport::RtspTransport( const RtspTransport& transport )
{
	operator=(transport);
}

/** 重置所有的属性. */
void RtspTransport::Clear()
{
	fClientPort		= 0;
	fServerPort		= 0;
	fInterleaved		= 0;
	fTtl				= 0;
	fIsMultcast		= FALSE;
	fStreamingMode	= TRANSPORT_UNKNOWN;
	memset(fDestination, 0, sizeof(fDestination));
}

/** 重载 = 操作符. */
RtspTransport& RtspTransport::operator=(const RtspTransport& transport) 
{
	fClientPort		= transport.fClientPort;
	fServerPort		= transport.fServerPort;
	fInterleaved		= transport.fInterleaved;
	fTtl				= transport.fTtl;
	fIsMultcast		= transport.fIsMultcast;
	fStreamingMode	= transport.fStreamingMode;
	strncpy(fDestination, transport.fDestination, sizeof(fDestination));
	return *this;
}

/** 解析指定的 RTSP Transport 头. */
int RtspTransport::Parse(LPCSTR buf)
{
	// Initialize the result parameters to default values:
	if (isempty(buf)) {
		return -1;
	}

	Clear();

	// Then, run through each of the fields, looking for ones we handle:
	String params = buf;
	String::size_type pos = 0;
	int count = 0;
	while (pos != String::npos) {
		String param = params.Tokenize(";", pos);	
		if (param.IsEmpty()) {
			continue;
		}

		count++;
		if (count == 1) {
			// 传输类型
			if (param == "RTP/AVP/TCP") {
				fStreamingMode = TRANSPORT_TCP;

			} else if (param == "RTP/AVP") {
				fStreamingMode = TRANSPORT_UDP;

			} else if (param == "RTP/AVP/UDP") {
				fStreamingMode = TRANSPORT_UDP;

			}

			continue;
		}

		String value;
		String::size_type param_pos = param.IndexOf("=", 0);
		if (param_pos != String::npos) {
			value = param.SubString(param_pos + 1);
			param = param.SubString(0, param_pos);
		}

		if (param == "multicast") {
			fIsMultcast = TRUE;

		} else if (param == "unicast") {
			fIsMultcast = FALSE;

		} else if (param == "destination") {
			memcpy(fDestination, value.c_str(), sizeof(fDestination) - 1);

		} else if (param == "ttl") {
			fTtl = (BYTE)atoi(value.c_str());

		} else if (param == "client_port") {
			fClientPort = atoi(value.c_str());

		} else if (param == "server_port") {
			fServerPort = atoi(value.c_str());

		} else if (param == "interleaved") {
			fInterleaved = atoi(value.c_str());
		}
	}
	
	return 0;
}

String RtspTransport::ToString()
{
	String text;
	if (TRANSPORT_UDP == fStreamingMode) {
		text.Format("RTP/AVP;%s;client_port=%d-%d;server_port=%d-%d", 
			fIsMultcast ? "multicast" : "unicast", 
			fClientPort, fClientPort + 1, 
			fServerPort, fServerPort + 1);

	} else {
		text.Format("RTP/AVP/TCP;unicast;interleaved=%d-%d", 
		fInterleaved, fInterleaved + 1);
	}

	return text;
}

//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
// RtspRange class

RtspRange::RtspRange()
{
	fStart = 0;
	fEnd = 0;
}

BOOL RtspRange::Parse( LPCSTR value )
{
	if (isempty(value)) {
		return FALSE;

	} else if (startsWith(value, "bytes=")) {
		LPCSTR p = value + 6;
		fStart = atoi(p);

		p = strstr(p, "-");
		if (p == NULL) {
			fEnd = 0;

		} else {
			fEnd = atoi(p);
		}

	} else if (startsWith(value, "npt=")) {
		LPCSTR p = value + 4;
		fStart = atof(p);

		p = strstr(p, "-");
		if (p == NULL) {
			fEnd = 0;

		} else {
			fEnd = atof(p);
		}

	} else {
		return FALSE;
	}

	return TRUE;
}

String RtspRange::ToString()
{
	String value;
	if (fEnd > 0) {
		value.Format("npt=%.3f-%.3f", fStart, fEnd);
	} else {
		value.Format("npt=%.3f-", fStart);
	}

	return value;
}

}
