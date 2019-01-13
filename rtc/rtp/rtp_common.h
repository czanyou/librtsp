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

#ifndef _NS_VISION_RTP_COMMON_H
#define _NS_VISION_RTP_COMMON_H

#include "rtc/rtc_context.h"

namespace rtp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RTP 相关常量定义

enum RtpDefines 
{
	RTCP_SDES_MAXITEMLENGTH			= 255,
	RTCP_SDES_NUMITEMS_NONPRIVATE	= 7,
	RTP_BYE_TIMEOUT_MULTIPLIER		= 1,
	RTP_COLLISION_TIMEOUT_MULTIPLIER = 10,
	RTP_DEFAULT_SESSION_BAND_WIDTH	= 10000,
	RTP_MAX_CSRCS					= 15,		///< CSRC 最大的数量
	RTP_MAX_PRIVITEMS				= 256,
	RTP_MEMBER_TIMEOUT_MULTIPLIER	= 5,
	RTP_MIN_PACKET_SIZE				= 600,		///< 最小的 RTP 包的大小
	RTP_NOTET_TIMEOUT_MULTIPLIER	= 25,
	RTP_PROBATION_COUNT				= 2,
	RTP_SENDER_TIMEOUT_MULTIPLIER	= 2,
	RTP_VERSION						= 2			///< RTP 的版本号
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//

/** RTCP 包类型. */
enum RtcpPacketType 
{
	RTCP_TYPE_SR			= 200,	// 发送端报告
	RTCP_TYPE_RR			= 201,	// 接收端报告
	RTCP_TYPE_SDES			= 202,	// 会话描述
	RTCP_TYPE_BYE			= 203,	// 离开会话消息
	RTCP_TYPE_APP			= 204,	// 应用程序扩展消息类型
	RTCP_TYPE_ACK			= 205	// 应用程序扩展消息类型
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//

/** RTCP SDES 类型. */
enum RtcpSdesId 
{
	RTCP_SDES_CNAME			= 1,
	RTCP_SDES_NAME			= 2,
	RTCP_SDES_EMAIL			= 3,
	RTCP_SDES_PHONE			= 4,
	RTCP_SDES_LOCATION		= 5,
	RTCP_SDES_TOOL			= 6,
	RTCP_SDES_NOTE			= 7,
	RTCP_SDES_PRIVATE		= 8
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtpHeader struct

/** 
 * 代表 RTP 固定的头字段的结构.
 * RTP fixed header fields 
 *
 * @author ChengZhen (anyou@msn.com)
 */
struct RtpHeader
{
	BYTE fHead;				///< pack head (v:2bit, p:1bit, x:1bit, cc:4bit)
	BYTE fPayload;			///< media payload format(m:1bit, payload:7bit)
	WORD fSequence;			///< sequence number
	UINT fTimestamp;		///< timestamp
	UINT fSsrc;				///< ssrc
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//

/** 
 * RtcpCommonHeader 代表 RTCP 包公共部分. 
 */
struct RtcpCommonHeader
{
	BYTE fHead;				///< 消息头部分
	BYTE fPacketType;		///< 包类型
	WORD fPacketLength;		///< 包长度
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtcpAckHeader

/** Protocol Structure - RRTP (Reliable RTP) */
struct RtcpAckHeader
{
	BYTE fHead;				///< 消息头部分
	BYTE fPacketType;		///< 包类型, 总是为 205
	WORD fLength;			/* Header length - 
							It indicates where user data begins in the packet. */
	WORD fFlags;			/* Control bits - 
							indicate what is present in the packet. Details as follows:

							 */
	WORD fCheckSum;			///< 
	WORD fSequence;			/* Sequence number - 
							When a connection is first opened, 
							each peer randomly picks an initial sequence number. 
							Each transmitter increments the sequence number before
							sending a data, null, or reset segment. */
	WORD fAckNumber;		/* Acknowledgment number - 
							The field indicates to a transmitter the last 
							in-sequence packet the receiver has received. */

};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//

/** RtcpSenderInfo 代表 RTCP 发送者报告信息. */
struct RtcpSenderInfo 
{
	UINT fSsrc;				///< Sender generating this report
	UINT fNtpMsw;			///< NTP timestamp 
	UINT fNtpLsw;			///< Least significant word    
	UINT fRtpTimstamp;		///< RTP timestamp 
	UINT fPacketsSent;		///< Packets sent 
	UINT fBytesSent;		///< Octets(bytes) sent 
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//

/** RtcpReportBlock 代表 RTCP 接收者报告. */
struct RtcpReportBlock
{
	UINT fSsrc;				///< Identifies about which SSRC's data this report is...
	BYTE fFractionLost;		///< 
	BYTE fPacketsLost[3];	///< 
	UINT fHighestSequence;	///< 
	UINT fJitter;			///< 
	UINT fLastSR;			///< 
	UINT fDelayLastSR;		///< 
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
//

/** RtcpSdesHeader 代表 RTCP SEDES 头. */
struct RtcpSdesHeader
{
	BYTE fSdesId;			// 类型 ID
	BYTE fLength;			// 数据长度
};

}; // namespace rtsp

#endif // _NS_VISION_RTP_COMMON_H
