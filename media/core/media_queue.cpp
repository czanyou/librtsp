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

#include "media_queue.h"
#include "core/utils/debug.h"

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaSampleQueue class

const UINT MEDIA_QUEUE_MAX_SIZE = (1024 * 1024 * 8);

MediaSampleQueue::MediaSampleQueue()
{
	fBufferSize			= 1500;
	fDropPFrame			= FALSE;
	fLostPacketCount	= 0;
	fMaxBufferSize		= MEDIA_QUEUE_MAX_SIZE;
	fMediaType			= 0;

	SetQueueSize(512);
}

MediaSampleQueue::MediaSampleQueue( UINT mediaType )
{
	fBufferSize			= 1500;
	fDropPFrame			= FALSE;
	fLostPacketCount	= 0;
	fMaxBufferSize		= MEDIA_QUEUE_MAX_SIZE;
	fMediaType			= mediaType;

	if (fMediaType == kMediaTypeVideo) {
		SetQueueSize(512);

	} else if (fMediaType == kMediaTypeAudio) {
		SetQueueSize(10);
	}
}

BOOL MediaSampleQueue::AddAudioSample(IMediaSample* mediaSample)
{
	if (mediaSample == NULL) {
		return FALSE;

	} else if (mediaSample->GetBufferCount() <= 0) {
		return FALSE;
	}

	BOOL ret = FALSE;
	fMutex.Lock();
	if (fMediaSamples.AddHead(mediaSample)) {
		ret = TRUE;
	}
	fMutex.UnLock();

	return ret;
}

/**
 * 写入新的一帧.
 * @param stream 产生这一帧的媒体流.
 * @param mediaSample 新采集的帧
 * @note 当缓冲区过满时, 会采取丢帧措施.
 */
BOOL MediaSampleQueue::AddVideoSample(IMediaSample* mediaSample)
{
	if (mediaSample == NULL) {
		return FALSE;

	} else if (mediaSample->GetBufferCount() <= 0) {
		return FALSE;
	}

	// 检查缓存池是否过满
	if (GetTotalBufferSize() > fMaxBufferSize) {
		LOG_E("The video pool is too full: %dkB.\r\n", GetTotalBufferSize() >> 10);
		fDropPFrame = TRUE;
		OnDrop(mediaSample);
		return FALSE;
	}

	// 处理丢帧 -----------------------------------------------
	UINT sampleCount = GetSampleCount();
	UINT frameType = mediaSample->GetFrameType();
	if (frameType == kVideoFrameI) {
		// 如果下一个 I Frame 到来, 丢弃之前的还没有来得及发送的帧.
		if (sampleCount >= 5) {
			while (GetSampleCount() > 0) {
				IMediaSamplePtr mediaSample = RemoveHead();
				OnDrop(mediaSample);
			}
		}

		// 当下一个 I 帧到来, 则停止丢弃 P 帧.
		if (fDropPFrame) {
			fDropPFrame = FALSE;
		}

	} else if (frameType == kVideoFrameP) {
		if (fDropPFrame) {
			OnDrop(mediaSample);
			return FALSE;

		} else if (sampleCount > 50) {
			// 如果缓存的帧比较多, 则丢弃这一 GOP 组中还没有发送的所有的 P 帧.
			fDropPFrame = TRUE;
		}

	} else {
		return FALSE; // 未知的帧类型
	}

	return AddAudioSample(mediaSample);
}

/** 释放缓冲区中缓存的所有帧 */
void MediaSampleQueue::Clear()
{
	fMutex.Lock();
	while (!fMediaSamples.IsEmpty()) {
		fMediaSamples.RemoveTail();
	}
	fMutex.UnLock();
}

/**
 * 写入新的一帧.
 * @param stream 产生这一帧的媒体流.
 * @param mediaSample 新的一帧
 */
BOOL MediaSampleQueue::DeliverSample(IMediaSample* mediaSample)
{
	if (fMediaType == kMediaTypeAudio) {
		return AddAudioSample(mediaSample);

	} else if (fMediaType == kMediaTypeVideo) {
		return AddVideoSample(mediaSample);
	}

	return TRUE;
}

/** 返回总共丢弃的 RTP 包的数目. */
UINT MediaSampleQueue::GetLostPacketCount()
{
	return fLostPacketCount;
}

/** 返回缓存池中总共的帧数. */
UINT MediaSampleQueue::GetSampleCount()
{
	fMutex.Lock();
	UINT ret = fMediaSamples.Size();
	fMutex.UnLock();
	return ret;
}

/** 返回缓存池中总共的 RTP 包的数目. */
UINT MediaSampleQueue::GetTotalBufferCount()
{
	fMutex.Lock();
	UINT bufferCount = 0;
	UINT sampleCount = fMediaSamples.Size();
	for (UINT i = 0; i < sampleCount; i++) {
		IMediaSamplePtr mediaSample = fMediaSamples.Get(i);
		if (mediaSample) {
			bufferCount += mediaSample->GetBufferCount();
		}
	}
	fMutex.UnLock();
	return bufferCount;
}

/** 返回缓存区缓存的数据的大小. */
UINT MediaSampleQueue::GetTotalBufferSize()
{
	return GetTotalBufferCount() * fBufferSize;
}

/** 指出缓存区是否为空. */
BOOL MediaSampleQueue::IsEmpty()
{
	fMutex.Lock();
	BOOL ret = fMediaSamples.IsEmpty();
	fMutex.UnLock();
	return ret;
}

/** 当将要丢弃指定的帧. */
void MediaSampleQueue::OnDrop(IMediaSample* mediaSample)
{
	if (mediaSample == NULL) {
		return;
	}

	fLostPacketCount += mediaSample->GetBufferCount();
}


void MediaSampleQueue::OnDump(String& dump)
{
	DumpCell(dump, "%d", GetSampleCount());
	DumpCell(dump, "%d", GetTotalBufferCount());
	DumpCell(dump, "%d", GetLostPacketCount());
	DumpCell(dump, "%d", GetTotalBufferSize());
}

/**
 * 从缓冲区头部中取出一帧.
 * @return 返回缓冲区中最晚添加的一帧, 如果缓冲区为空则返回 NULL.
 */
IMediaSamplePtr MediaSampleQueue::RemoveHead()
{
	IMediaSamplePtr mediaSample = NULL;
	fMutex.Lock();
	if (!fMediaSamples.IsEmpty()) {
		mediaSample = fMediaSamples.GetHead();
		fMediaSamples.RemoveHead();
	}

	fMutex.UnLock();
	return mediaSample;
}

/**
 * 从缓冲区尾部中取出一帧.
 * @return 返回缓冲区中最早添加的一帧, 如果缓冲区为空则返回 NULL.
 */
IMediaSamplePtr MediaSampleQueue::RemoveTail()
{
	IMediaSamplePtr mediaSample = NULL;
	fMutex.Lock();
	if (!fMediaSamples.IsEmpty()) {
		mediaSample = fMediaSamples.GetTail();
		fMediaSamples.RemoveTail();
	}

	fMutex.UnLock();
	return mediaSample;
}

void MediaSampleQueue::SetQueueSize( UINT queueSize )
{
	fMutex.Lock();
	fMediaSamples.Resize(queueSize, NULL);
	fMutex.UnLock();
}

}
