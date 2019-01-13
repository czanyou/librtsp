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

#include "rtsp_reader.h"

namespace rtsp {

const UINT kRtspIntervalHeaderSize	= 4;			///< RTP over RTSP 头的长度
const UINT kRtspReadBufferSize		= 1024 * 64;	///< 默认的读缓存区的大小

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspReader class

RtspReader::RtspReader()
{
	fReadBuffer		= new ByteBuffer(kRtspReadBufferSize);
	fContentLength	= 0;
	fLastMessage	= NULL;

}

RtspReader::~RtspReader()
{
	if (fReadBuffer) {
		fReadBuffer = NULL;
	}
}

/** 重置所有的成员变量的值. */
void RtspReader::ClearAll()
{
	ByteBufferPtr buffer = fReadBuffer;
	if (buffer) {
		buffer->Reset();
	}

	fLastMessage = NULL;
	fContentLength	= 0;
}

/** 
 * 解析并返回缓存区中的下一条 RTSP 消息. 解析成功后, 把指针移到下一个消息 
 * 开始的位置. 
 */
RtspMessagePtr RtspReader::GetNextMessage()
{
	if (IsEmpty()) {
		return NULL;
	}

	ByteBufferPtr buffer = fReadBuffer;
	if (buffer == NULL) {
		return NULL;
	}

	char* data = (char*)buffer->GetData();
	if (data[0] == '$') {		
		return NULL; // 检查是否 RTP 数据包
	} 

	// 解析 RTSP 消息头部分
	if (fContentLength == 0) {
		char* end = strstr(data, "\r\n\r\n");
		if (end == NULL) {
			return 0;
		}

		int headerLength = (end - data) + 4;
		end[2] = '\0';

		RtspMessagePtr rtspMessage = new RtspMessage();
		if (rtspMessage->Parse(data) < 0) {
			LOG_D("Bad Request: %s. \r\n", data);
			return NULL;
		}
		buffer->Skip(headerLength);
		fLastMessage = rtspMessage;

		int contentLength = rtspMessage->GetHeaderInt("Content-Length");
		if (contentLength <= 0) {
			rtspMessage->SetContent(NULL, 0);
			return rtspMessage;

		} else {
			fContentLength = contentLength;
		}
	}

	// 检查 RTSP 内容是否收取完毕 -------------------------
	if (fContentLength > 0) {
		UINT dataLength = buffer->GetSize();
		if (dataLength < fContentLength) {
			return NULL;
		}

		RtspMessagePtr rtspMessage = fLastMessage;
		if (rtspMessage != NULL) {
			rtspMessage->SetContent((char*)buffer->GetData(), fContentLength);
		}

		buffer->Skip(fContentLength);
		fContentLength = 0;
		return rtspMessage;
	}

	return NULL;
}

/** 
 * 返回下一个要解析的 RTP 数据包的开始位置的指针. 如果不存在或没有收取
 * 完全, 则返回 NULL. 解析成功后, 把指针移到下一个消息开始的位置.
 */
char* RtspReader::GetNextPacket()
{
	ByteBufferPtr buffer = fReadBuffer;
	if (buffer == NULL) {
		return NULL;
	}

	if (IsEmpty()) {
		return NULL;
	}

	BYTE *data = buffer->GetData();
	if (data[0] != '$') {
		return NULL;
	}

	int dataLength = buffer->GetSize();
	int packetSize = MAKEWORD(data[3], data[2]);
	if (dataLength < int(packetSize + kRtspIntervalHeaderSize)) {
		return NULL;
	}

	buffer->Skip(packetSize + kRtspIntervalHeaderSize);
	return (char*)data;
}

BOOL RtspReader::IsEmpty()
{
	ByteBufferPtr buffer = fReadBuffer;
	if (buffer == NULL) {
		return TRUE;
	}

	return buffer->IsEmpty();
}

/** 指出缓存区中下一个要解析的数据包是否是 RTP 数据包. */
BOOL RtspReader::IsRtpPacket()
{
	ByteBufferPtr buffer = fReadBuffer;
	if (buffer == NULL) {
		return FALSE;
	}

	return buffer->PeekByte() == '$'; // RTP 包以 '$' 开始
}

BYTE* RtspReader::LockBuffer( UINT& freeSize )
{
	freeSize = 0;

	ByteBufferPtr buffer = fReadBuffer;
	if (buffer == NULL) {
		return NULL;
	}

	// 当缓存区空闲空间不够时尝试压紧这个缓存区
	if (buffer->GetFreeSize() < 512) {
		buffer->Compact();
	}

	freeSize = buffer->GetFreeSize();
	if (freeSize <= 0) {
		LOG_D("Buffer is too full...\r\n");
		return NULL;
	}

	BYTE *freeBuffer = buffer->GetFreeBuffer();
	return freeBuffer;
}

void RtspReader::OnDump(String& dump)
{
	String text;
	text.Format("<tr><td>%d</td> <td>%d</td> <td></td> \n",
		fReadBuffer->GetSize(), fContentLength);

	dump += text;
}

void RtspReader::UnlockBuffer( UINT size )
{
	ByteBufferPtr buffer = fReadBuffer;
	if (buffer) {
		buffer->Extend(size);
	}
}

}
