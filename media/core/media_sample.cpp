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

#include "media_sample.h"

namespace media {

static int gSampleCount  = 0;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaSample class

/** 构建方法. */
MediaSample::MediaSample()
{
	IMediaSample::Clear();
	fRefCount = 0;
	fReadOnly = FALSE;
	gSampleCount++;
};

/** 构建方法. */
MediaSample::MediaSample( UINT initSize )
{
	IMediaSample::Clear();
	fRefCount = 0;
	fReadOnly = FALSE;
	gSampleCount++;
}

MediaSample::~MediaSample()
{
	fMediaBuffers.Clear();
	gSampleCount--;
}

/** 添加一个 RTP 包. */
int MediaSample::AddBuffer( IMediaBuffer* mediaBuffer )
{
	if (mediaBuffer == NULL) {
		return -1;

	 } else if (fReadOnly) {
		 return -1;
	 }

	fMediaBuffers.Add(mediaBuffer);
	return 0;
}

/** 
 * Increments a reference count 
 * Returns the value of the reference count.
 * When an interface is fully released, the reference count is zero.
 */
long MediaSample::AddRef()
{
	fMutex.Lock();
	long ref = ++fRefCount;
	fMutex.UnLock();

	return ref;
}

/** 重置主要的成员变量的值. */
void MediaSample::Clear()
{
	IMediaSample::Clear();

	fMediaBuffers.Clear();
	fReadOnly = FALSE;
}

/** 返回指定的索引的 RTP 包. */
IMediaBufferPtr MediaSample::GetBuffer( UINT index ) const
{
	if (index >= fMediaBuffers.GetLength()) {
		return NULL;
	}

	return fMediaBuffers.Get(index);
}

/** 返回包含的数据包的个数. */
UINT MediaSample::GetBufferCount() const
{
	return fMediaBuffers.GetLength();
}

UINT MediaSample::GetSampleSize() const
{
	UINT sampleSize = 0;
	UINT count = fMediaBuffers.GetLength();
	for (UINT i = 0; i < count; i++) {
		IMediaBufferPtr mediaBuffer = fMediaBuffers.Get(i);
		sampleSize += mediaBuffer->GetLength();
	}

	return sampleSize;
}

/**
 * Decrements the reference count for the specified object
 * Returns the value of the reference count.
 */
long MediaSample::Release()
{
	fMutex.Lock();
	long ref = --fRefCount;
	fMutex.UnLock();

	if (ref == 0) {
		Clear();
		delete this;
	}

	return ref;
}

void MediaSample::SetReadOnly()
{
	fReadOnly = TRUE;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaPoolBuffer class

MediaAllocator::MediaPoolBuffer::MediaPoolBuffer( MediaAllocator* allocator )
	: MediaBuffer()
{
	if (allocator == NULL) {
		LOG_E("Invalid allocator.\r\n");
	}

	fNext		= NULL;
	fAllocator	= allocator;
}

/**
 * 递减引用计数.
 * 当引用计数为 0 时, 并不是直接删除当前对象, 而是尝试回收到 Packet 池中.
 */
long MediaAllocator::MediaPoolBuffer::Release()
{
	fMutex.Lock();
	long ref = --fRefCount;
	fMutex.UnLock();	

	if (ref == 0) {
		fRefCount		= 0;
		fPayloadLength	= 0;

		if (fAllocator) {
			fAllocator->FreeMediaBuffer(this);

		} else {
			delete this;
		}
	}

	return ref;
}

/** 重置引用计数. */
void MediaAllocator::MediaPoolBuffer::ResetRefcount()
{
	fRefCount = 0;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaAllocator class

MediaAllocator::MediaAllocator()
{
	fCapacity		= 64;	// Default capacity
	fFreeBuffers	= NULL;
	fFreeCount		= 0;
	fLastTime		= GetSysTickCount();
	fTotalCount		= 0;
}	

MediaAllocator::~MediaAllocator()
{
	Clear();
}

/** Clear free buffers. */
void MediaAllocator::Clear()
{
	fMutex.Lock();
	while (fFreeBuffers != NULL) {
		MediaPoolBuffer* mediaBuffer = fFreeBuffers;
		fFreeBuffers = fFreeBuffers->fNext;

		delete mediaBuffer;
		mediaBuffer = NULL;
		fTotalCount--;
	}

	fFreeCount = 0;
	fMutex.UnLock();
}

/** Free the packet: push it into the free packet queue. */ 
BOOL MediaAllocator::FreeMediaBuffer(MediaPoolBuffer *mediaBuffer)
{
	ASSERT(mediaBuffer);
	if (mediaBuffer == NULL) {
		return FALSE;
	}

	mediaBuffer->ResetRefcount();

	fMutex.Lock();
	// The buffer is too full.
	if (fTotalCount > fCapacity) {
		delete mediaBuffer;
		mediaBuffer = NULL;
		fTotalCount--;

	} else {
		// push it into free packet queue
		mediaBuffer->fNext = fFreeBuffers;
		fFreeBuffers = mediaBuffer;
		fFreeCount++;
	}
	fMutex.UnLock();

	// 定时清理空闲的数据
	INT64 now = GetSysTickCount();
	if (UINT(now - fLastTime) > 1000 * 10) {
		fLastTime = now;
		Clear();
	}
	return TRUE;
}

UINT MediaAllocator::GetCapacity()
{
	return fCapacity;
}

/** Retrieves the count of free RTP packets. */
UINT MediaAllocator::GetFreeCount()
{
	fMutex.Lock();
	int count = fFreeCount;
	fMutex.UnLock();

	return count; 
}

UINT MediaAllocator::GetTotalCount()
{
	return fTotalCount;
}

/**
 * 创建一个新的 Buffer
 */
MediaBufferPtr MediaAllocator::NewMediaBuffer(UINT initSize)
{
	MediaPoolBuffer* mediaBuffer = NULL;

	fMutex.Lock();
	if (fFreeBuffers) {
		mediaBuffer = fFreeBuffers;
		fFreeBuffers = mediaBuffer->fNext;
		fFreeCount--;

		mediaBuffer->fNext = NULL;

	} else if (fTotalCount <= fCapacity) {
		mediaBuffer = new MediaPoolBuffer(this);
		fTotalCount++;
	}
	fMutex.UnLock();
	return mediaBuffer;
}

/** 创建一个新的 IMediaFrame. */
IMediaSamplePtr MediaAllocator::NewMediaSample(UINT initSize)
{
	IMediaSamplePtr mediaSample = new MediaSample(initSize);
	return mediaSample;
}

/**
 * 设置这个池的最大容量.
 * @param capacity 最大容量, 单位为字节.
 */
void MediaAllocator::SetCapacity( UINT capacity )
{
	fCapacity = capacity / 1500; 
}

};
