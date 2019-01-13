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
#include "rtp_rtcp.h"

namespace rtp { 

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtcpPacket class

RtcpPacket::RtcpPacket()
{
	fCurPacket			= 0;
	fIsExternalBuffer	= TRUE;
	fPacketCount		= 0;
	fPacketData			= NULL;
	fPacketLength		= 0;
	fPacketType			= 0;
	fAllocSize			= 0;

	memset(fPacketOffset, 0, sizeof(fPacketOffset));
}

RtcpPacket::~RtcpPacket()
{
	if (fPacketData) {
		delete[] fPacketData;
		fPacketData = NULL;
	}
}

/** 添加一个指定的类型的 RTCP 包. */
int RtcpPacket::AddPacket( BYTE type, BYTE count )
{
	// 分配缓存区
	if (fPacketData == NULL) {
		fAllocSize = RTCP_DEFAULT_SIZE;
		fPacketData = new BYTE[fAllocSize];
		fIsExternalBuffer = FALSE;
	}

	int length = 0;
	switch (type) {
	case RTCP_TYPE_SR: 
		length = 6 + count * 6;
		break;

	case RTCP_TYPE_SDES:
		length = count * 6;
		break;

	default:
		return -1;
		break;
	}

	// 检测是否超过包的大小
	if (fAllocSize - fPacketLength < (length + 1) * 4) {
		return -1;
	}

	// 设置公共属性的值
	RtcpCommonHeader* header = GetCommonHeader();
	if (header) {
		header->fHead		= 0x80 | (count & 0x1F);
		header->fPacketType	= type;
		header->fPacketLength		= htons(length);
	}

	fPacketCount++;

	fPacketType	= type;
	fCurPacket	= fPacketCount - 1;

	fPacketOffset[fCurPacket] = fPacketLength;
	fPacketLength += (length + 1) * 4;

	return 0;
}

/** 返回这个包的公共的头. */
RtcpCommonHeader* RtcpPacket::GetCommonHeader()
{
	if (fCurPacket >= fPacketCount) {
		return NULL;
	}

	BYTE* offset = fPacketData + fPacketOffset[fCurPacket];
	return reinterpret_cast<RtcpCommonHeader*>(offset);
}

BYTE* RtcpPacket::GetPacketData()
{
	return fPacketData;
}

int RtcpPacket::GetPacketLength() const
{
	return fPacketLength;
}

int RtcpPacket::GetPacketType() const
{
	return fPacketType;
}

/** 返回这个包的发送者报告部分. */
RtcpReportBlock* RtcpPacket::GetReportBlock()
{
	if (fPacketData == NULL || fPacketType != RTCP_TYPE_SR) {
		return NULL;
	}

	RtcpCommonHeader* header = GetCommonHeader();
	BYTE *offset = (BYTE*)header;
	if (offset == NULL ) {
		return NULL;
	}

	// 检查是否包含报告块
	BYTE count = header->fHead & 0x1F;
	if (count <= 0) {
		return NULL;
	}

	return reinterpret_cast<RtcpReportBlock*>(offset + 7);
}

/** 返回这个包的发送者信息部分. */
RtcpSenderInfo* RtcpPacket::GetSenderInfo()
{
	if (fPacketData == NULL || fPacketType != RTCP_TYPE_SR) {
		return NULL;
	}

	BYTE *offset = (BYTE*)GetCommonHeader();
	if (offset == NULL) {
		return NULL;
	}

	return reinterpret_cast<RtcpSenderInfo*>(offset + sizeof(RtcpCommonHeader));
}

/** 解析指定的 RTCP 原始数据包内容. */
BOOL RtcpPacket::Parse( char* packetData, int length )
{
	if (packetData == NULL || length <= 1) {
		return FALSE;
	}

	fIsExternalBuffer	= TRUE;
	fPacketData			= (BYTE*)packetData;
	fPacketLength		= length;

	return TRUE;
}

void RtcpPacket::SetPacketType( int type )
{
	fPacketType = type;
}

};
