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

#include "rtsp_writer.h"
#include "rtc/rtp/rtp_decoder.h"

namespace rtsp {

//______________________________________________________________________________
////////////////////////////////////////////////////////////////////////////////
// RtspWriter class

RtspWriter::RtspWriter()
{
	fNalHeader	= 0;
	fSendBuffer	= new ByteBuffer(1024 * 64);
	fSequence	= 0;
}

RtspWriter::~RtspWriter()
{
}

int RtspWriter::CheckBufferSize(UINT packetSize) 
{
	if (packetSize <= 0) {
		return 0;
	}

	ByteBufferPtr sendBuffer = fSendBuffer;
	if (sendBuffer == NULL) {
		UINT newCapacity = MAX(1024 * 64, packetSize);
		fSendBuffer = new ByteBuffer(newCapacity);
		return 0;
	}

	packetSize += 1024;

	UINT freeSize = sendBuffer->GetFreeSize();
	if (freeSize >= packetSize) {
		return 0;
	}

	// compact
	freeSize += sendBuffer->GetStart();
	if (freeSize >= packetSize) {
		sendBuffer->Compact();
		return 0;
	}

	// expend
	UINT capacity = sendBuffer->GetCapacity();
	UINT newCapacity = MAX(capacity * 2, capacity + packetSize);
	sendBuffer->Resize(newCapacity);

	return 0;
}

/** 重置所有的属性. */
void RtspWriter::ClearAll()
{
	fMutex.Lock();
	fRtspSession = NULL;

	if (fSendBuffer) {
		fSendBuffer->Reset();
	}
	fMutex.UnLock();
}

void RtspWriter::ClearBufferList()
{
}

void RtspWriter::OnDump(String& dump)
{
	ByteBufferPtr sendBuffer = fSendBuffer;
	if (sendBuffer == NULL) {
		return;
	}

	String text;
	text.Format(" <td>%d</td> <td>%d</td> <td>%d</td> <td>%d</td></tr>\n",
		sendBuffer->GetSize(), sendBuffer->GetCapacity(), 
		sendBuffer->GetStart(), sendBuffer->GetEnd());

	dump += text;
}

BOOL RtspWriter::OnFillNextSample()
{
	RtspSessionPtr rtspSession = fRtspSession;
	if (rtspSession == NULL) {
		return FALSE;

	} else if (rtspSession->GetSessionState() != kRtspStatePlaying) {
		return FALSE;

	} else if (!rtspSession->IsTcpTransport()) {
		return FALSE;	// 如果不是 RTP over RTSP 
	}

	///////////////////////////////////////////////////////////
	// 如果当前 RTP 包发送缓冲区为空, 则从帧发送队列中取得下一帧

	IMediaSamplePtr mediaSample = rtspSession->GetNextMediaSample();
	if (mediaSample) {
		WriteSample(mediaSample);
	}

	// end if
	return TRUE;
}

/** 
* 填充消息内容. 
* 主要在请求当前画面截图时用到, 画面截图是通过 HTTP 以及图片下载的方式传输的.
* 这个方法就是把图片数据填充到发送缓存区中.
*/
BOOL RtspWriter::OnFillNextRawPacket(IMediaBuffer* mediaBuffer)
{
	if (mediaBuffer == NULL) {
		return FALSE;
	}

	BYTE* data  = mediaBuffer->GetData();
	UINT length = mediaBuffer->GetLength();
	WriteBuffer(data, length);

	return TRUE;
}

// 取得下一个要发送的数据包
BOOL RtspWriter::OnFillNextRtpPacket(IMediaBuffer* mediaBuffer, UINT channel, 
									 UINT rtpTime, UINT rtpSsrc)
{
	if (mediaBuffer == NULL) {
		return FALSE;
	}

	const UINT kRtpHeaderSize = 12;
	const UINT kRtpIntervalSize = 4;

	BYTE* sampleData	= mediaBuffer->GetData();
	UINT  sampleLength	= mediaBuffer->GetLength();
	UINT startSize = RtpDecoder::GetStartCodeLength(sampleData);
	if (startSize) {
		sampleData   += startSize;
		sampleLength -= startSize;
	}

	UINT  totalSize		= sampleLength   + kRtpHeaderSize;
	UINT  headerSize	= kRtpHeaderSize + kRtpIntervalSize;

	///////////////////////////////////////////////////////////
	// RTSP/RTP Size Header

	BYTE header[32];
	header[0] = '$';
	header[1] = (BYTE)channel;
	header[2] = HIBYTE(totalSize);
	header[3] = LOBYTE(totalSize);

	///////////////////////////////////////////////////////////
	// Initialize RTP Header 初始化，为发送做准备
	// 如果是 RTP 数据包，则填写 RTP 头部相关字段
	// 因为 RTP 包是多个 Session 共同的, 在发送时, 头部的字段的值是和 
	// Session 相关的, 所以在发送前要重新填写相应字段的值.

	RtpHeader* rtpHeader	= (RtpHeader*)(header + kRtpIntervalSize);
	rtpHeader->fHead		= 0x80;  // 10-0-0-0000
	rtpHeader->fPayload		= (mediaBuffer->GetPayloadType() & 0x7F);
	rtpHeader->fTimestamp	= htonl(rtpTime);
	rtpHeader->fSequence	= htons(fSequence++);
	rtpHeader->fSsrc		= htonl(rtpSsrc);

	if (mediaBuffer->IsMarker()) {
		rtpHeader->fPayload	|= 0x80;
	}

	// FU Header
	UINT packetCount = mediaBuffer->GetFragmentCount();
	if (packetCount > 1) {
		BYTE* fuHeader = (header + headerSize);

		UINT packetSequence = mediaBuffer->GetFragmentSequence();
		BOOL isStart = (packetSequence == 1);
		BOOL isEnd   = (packetSequence == packetCount);

		if (isStart) {
			fNalHeader = sampleData[0];
		}

		fuHeader[0] = (fNalHeader & 0x60) | 28;	// FU indicator   
		fuHeader[1] = (fNalHeader & 0x1f);		// FU header 

		if (isStart) {
			fuHeader[1] |= 0x80;

			// 跳过开始位置的 NAL 头
			sampleData++;
			sampleLength--;
		}

		if (isEnd) {
			fuHeader[1] |= 0x40;
		}

		// 
		headerSize += 2; // FU header size
		totalSize = (sampleLength + headerSize) - kRtpIntervalSize;
		header[2] = HIBYTE(totalSize);
		header[3] = LOBYTE(totalSize);

		// LOG_D("%d/%d %02x %02x\r\n", packetSequence, packetCount, fuHeader[0], fuHeader[1]);
	}

	WriteBuffer(header, headerSize);
	WriteBuffer(sampleData, sampleLength);

	return TRUE;
}

/**
 * 处理发送事件.
 * @param socket 用于发送数据的 Socket.
 * @param isEmptyBuffer 返回参数, 表示发送缓存区是否为空.
 * @param connectionClose 是否自动关闭 Socket
 * @return  
 */
int RtspWriter::OnSend( Socket& socket, BOOL& isEmptyBuffer )
{
	ByteBufferPtr sendBuffer = fSendBuffer;
	if (sendBuffer == NULL) {
		return -1;
	}

	int ret = 0;
	while (socket.sockfd > 0) {

		// 检测发送缓存区是否为空
		if (sendBuffer->IsEmpty()) {
			if (!OnFillNextSample()) {
				// TODO: 
			}

			fMutex.Lock();
			isEmptyBuffer = sendBuffer->IsEmpty(); // 如果没有可发送的数据
			fMutex.UnLock();

			if (isEmptyBuffer) {
				// 删除 OP_SEND 标记
				if (socket.fEvents != OP_READ) {
					socket.AsyncSelect(OP_READ);
				}
				break;

			} else {
				if ((socket.fEvents & OP_SEND) == 0) {
					socket.AsyncSelect(OP_READ | OP_SEND);
				}
			}
		}

		// 发送数据发生错误或中断, 退出发送循环
		int sent = SendBufferData(socket);
		if (sent <= 0) {
			return sent;
		}

		ret += sent;
	}

	return ret;
}

/** 发送缓存区中的数据. */
int RtspWriter::SendBufferData( Socket &socket )
{
	if (socket.sockfd <= 0) {
		return -1;
	}

	int ret = 0;

	// 发送数据	
	fMutex.Lock();

	ByteBufferPtr sendBuffer = fSendBuffer;
	if (sendBuffer) {
		char* data = (char*)sendBuffer->GetData();
		UINT  size = sendBuffer->GetSize();

		ret = socket.Send(data, size, 0);
		if (ret > 0) {
			sendBuffer->Skip(ret);
		}
	}

	fMutex.UnLock();

	if (ret > 0) {
		return ret;
	}

	// 如果不是严重的错误, 如被其他进程中断，
	// 或缓冲区中的数据还没有发送完毕.
	// 当在广域网上时, 有时会碰到奇怪的 115 错误, 但好像并不是严重错误
	if (errno == EINTR || errno == EWOULDBLOCK || errno == 115) {
		// If Error occur, must break, let other session to send data first.	
		if (socket.fEvents != (OP_SEND | OP_READ)) {
			socket.AsyncSelect( OP_SEND | OP_READ);
		}

	} else {
		// Fatal error, maybe connection is closed
		LOG_D("Send failed, %d/%s\r\n", errno, strerror(errno));
		socket.Close();
	}

	return ret;
}

/** 设置相关的 RtspSession 对象. */
void RtspWriter::SetRtspSession( RtspSession* session )
{
	fRtspSession = session;
}

int RtspWriter::WriteBuffer(const BYTE* data, UINT dataLength)
{
	if (data == NULL || dataLength <= 0) {
		return 0;
	}

	int ret = -1;

	fMutex.Lock();
	ByteBufferPtr sendBuffer = fSendBuffer;
	if (sendBuffer && sendBuffer->Put(data, dataLength)) {
		ret = dataLength;
	}

	fMutex.UnLock();
	return ret;
}

/** 
 * 写指定的 RTSP 消息到发送缓存区中. 
 *
 * @param message 要发送的消息内容
 * @param length 要发送的消息长度
 * @return 返回实际写入的数据的长度, 如果发生错误则返回 -1.
 */
int RtspWriter::WriteMessage(LPCSTR message, size_t length)
{
	if (isempty(message) || length == 0) {	
		return -1; // 无效的参数
	}

	CheckBufferSize(length);
	return WriteBuffer((BYTE*)message, length);
}

/** 
 * 填充一帧到发送缓冲区中. 
 *
 */
int RtspWriter::WriteSample(IMediaSample* mediaSample)
{
	if (mediaSample == NULL) {
		return -1;
	}

	int ret = 0;

	RtspSessionPtr rtspSession = fRtspSession;

	// Channel Identifier
	UINT  channel	 = (mediaSample->GetFrameType() != kAudioFrame) ? 0 : 2;
	INT64 sampleTime =  mediaSample->GetSampleTime();
	UINT  frequency  =  mediaSample->GetFrequency();
	UINT  sampleSize =  mediaSample->GetSampleSize();
	
	CheckBufferSize(sampleSize);

	// Buffers
	int count = mediaSample->GetBufferCount();
	for (int i = 0; i < count; i++) {
		IMediaBufferPtr mediaBuffer = mediaSample->GetBuffer(i);
		if (mediaBuffer) {
			ret += mediaBuffer->GetLength();

			if (rtspSession == NULL) {
				OnFillNextRawPacket(mediaBuffer);

			} else {
				UINT rtpTime = rtspSession->GetRtpTime(sampleTime, frequency);
				UINT rtpSsrc = rtspSession->GetSSRC();
				OnFillNextRtpPacket(mediaBuffer, channel, rtpTime, rtpSsrc);
			}
		}
	}

	return ret;
}

};
