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

#include "media_buffer.h"

namespace media {

static int gBufferCount = 0;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaBuffer class

MediaBuffer::MediaBuffer()
{
	fFragmentCount		= 0;
	fFragmentSequence	= 0;
	fFrameType			= 0;
	fIsMarker			= 0;
	fIsSyncPoint		= 0;
	fMaxLength			= kRtpDefaultPacketSize + 64;
	fPayloadLength		= 0;
	fPayloadType		= 0;
	fRefCount			= 0;
	fSampleTime			= 0;

	fBuffer				= (BYTE*)malloc(fMaxLength);
	memset(fBuffer, 0, fMaxLength);

	gBufferCount++;
}

MediaBuffer::MediaBuffer( UINT maxLength )
{
	fFragmentCount		= 0;
	fFragmentSequence	= 0;
	fFrameType			= 0;
	fIsMarker			= 0;
	fIsSyncPoint		= 0;
	fMaxLength			= maxLength;
	fPayloadLength		= 0;
	fPayloadType		= 0;
	fRefCount			= 0;
	fSampleTime			= 0;

	fBuffer				= (BYTE*)malloc(fMaxLength);
	memset(fBuffer, 0, fMaxLength);

	gBufferCount++;

}

MediaBuffer::~MediaBuffer()
{
	if (fBuffer) {
		free(fBuffer);
		fBuffer = NULL;
	}

	gBufferCount--;
}

/** 
 * Increments a reference count 
 * Returns the value of the reference count.
 * When an interface is fully released, the reference count is zero.
 */
long MediaBuffer::AddRef()
{
	fMutex.Lock();
	long ref = ++fRefCount;
	fMutex.UnLock();
	return ref;
}

/** 返回缓存区中有效数据的指针. */
BYTE* MediaBuffer::GetData()
{
	return fBuffer;
}

/** 返回这个样本所属的帧的类型. */
UINT MediaBuffer::GetFrameType() const
{
	return fFrameType;
}

/** 返回缓存区中有效数据的长度. */
UINT MediaBuffer::GetLength() const
{
	return fPayloadLength;
}

/** 返回缓存区的总长度. */
UINT MediaBuffer::GetMaxSize() const
{
	return fMaxLength;
}

/** 
 * 返回这个样本所属的包分片总数, 少于等于 1 表示这是完整的一包. 
 * 因为 RTP 包的最大长度, 可能不能超过 1460, 所以需要将大于 1460 的包分片
 * 传输, 这里指的是分片数量.
 */
UINT MediaBuffer::GetFragmentCount() const
{
	return fFragmentCount;
}

/** 返回这个样本在所属的包中分片序号. */
UINT MediaBuffer::GetFragmentSequence() const
{
	return fFragmentSequence;
}

/** 返回这个样本的 RTP 负载类型. */
UINT MediaBuffer::GetPayloadType() const
{
	return fPayloadType;
}

/** 返回这个样本的时间戳. */
INT64 MediaBuffer::GetSampleTime() const
{
	return fSampleTime;
}

/** 返回这个样本的标记状态. */
BOOL MediaBuffer::IsMarker() const
{
	return fIsMarker;
}

BOOL MediaBuffer::IsSyncPoint() const
{
	return fIsSyncPoint;
}

/**
 * Decrements the reference count for the specified object
 * Returns the value of the reference count.
 */
long MediaBuffer::Release()
{
	fMutex.Lock();
	long ref = --fRefCount;
	fMutex.UnLock();	

	if (ref == 0) {
		delete this;
	}
	return ref;
}

void MediaBuffer::SetFragmentCount( UINT total )
{
	fFragmentCount = total;
}

void MediaBuffer::SetFragmentSequence( UINT sequence )
{
	fFragmentSequence = sequence;
}

/** 设置这个样本所属的帧的类型. */
void MediaBuffer::SetFrameType( UINT type )
{
	fFrameType = type;
}

/** Sets the length of the payload data. */
void MediaBuffer::SetLength( UINT length )
{
	fPayloadLength = length;
}

void MediaBuffer::SetMarker( BOOL marker )
{
	fIsMarker = marker;
}

void MediaBuffer::SetPayloadType( UINT type )
{
	fPayloadType = type;
}

/** 设置这个样本的时间戳. */
void MediaBuffer::SetSampleTime( INT64 timestamp )
{
	fSampleTime = timestamp;
}

void MediaBuffer::SetSyncPoint( BOOL syncPoint )
{
	fIsSyncPoint = syncPoint;
}

};
