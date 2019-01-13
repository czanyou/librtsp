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
#ifndef _NS_VISION_RTP_RTCP_PACKET_H_
#define _NS_VISION_RTP_RTCP_PACKET_H_

#include "media/vision_media.h"
#include "rtp_common.h"
#include "core/net/nio_socket.h"
#include "media/vision_media_frame.h"

namespace rtp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RTCP Packet class

/** 
 * 代表一个 RTCP 数据包. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtcpPacket
{
public:
	RtcpPacket();
	virtual ~RtcpPacket();

	enum {
		RTCP_DEFAULT_SIZE = 1500,
		RTCP_PACKET_MAX_COUNT = 100
	};

// Operations -------------------------------------------------
public:
	RtcpCommonHeader* GetCommonHeader();
	RtcpReportBlock*  GetReportBlock();
	RtcpSenderInfo*   GetSenderInfo();

	BYTE* GetPacketData();
	int   GetPacketLength() const;
	int   GetPacketType() const;

	void  SetPacketType(int type);

public:
	int  AddPacket(BYTE type, BYTE count);
	BOOL Parse(char* packetData, int length);
	
// Data members -----------------------------------------------
protected:
	int   fAllocSize;			///< 数据包缓存区分配的大小
	BYTE  fCurPacket;			///< 当前正在编辑的包的索引
	BOOL  fIsExternalBuffer;	///< 指出缓存区是否是外部分配的.
	BYTE  fPacketCount;			///< 如果这是一个复合包, 则指出包含的包的数目
	BYTE* fPacketData;			///< 数据包缓存区
	int   fPacketLength;		///< 数据包的长度
	BYTE  fPacketType;			///< 当前正在编辑的 RTCP 包的类型
	WORD  fPacketOffset[RTCP_PACKET_MAX_COUNT]; ///< 每一个子 RTCP 包的偏移位置
};

}

#endif // _NS_VISION_RTP_RTCP_PACKET_H_
