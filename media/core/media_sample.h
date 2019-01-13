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
#ifndef _NS_VISION_MEDIA_SAMPLE_H
#define _NS_VISION_MEDIA_SAMPLE_H

#include "media/vision_media.h"
#include "media_buffer.h"

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaSample class

/** 
 * Represents an Video / Audio Frame.
 * A object is used to hold samples and deliver them between the objects of the 
 * RTSP stream server.
 * 
 * 代表一个完整的音视频帧, 如完整的一帧图像. 它通常由多个样本组成.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MediaSample : public IMediaSample
{
// Construction/Destruction -----------------------------------
public:
	MediaSample();
	MediaSample(UINT initSize);
	virtual ~MediaSample();

	typedef ArrayList<IMediaBufferPtr> IMediaBufferArray;

// Attributes -------------------------------------------------
public:
	UINT GetBufferCount() const;
	UINT GetSampleSize() const;
	void SetReadOnly();

// Operations -------------------------------------------------
public:
	IMediaBufferPtr GetBuffer(UINT index) const;

	int  AddBuffer(IMediaBuffer* mediaBuffer);
	long AddRef();	
	void Clear();
	long Release();

// Data Members -----------------------------------------------
private:
	IMediaBufferArray fMediaBuffers; ///< 这个帧包含的数据包列表
	Mutex fMutex;			///< Mutex object
	BOOL  fReadOnly;		///< 指出这个 sample 是否是只读的
	long  fRefCount;		///< Reference count
};

/** MediaSample  智能指针类型. */
typedef SmartPtr<MediaSample> MediaSamplePtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaAllocator class

/** 
 * 代表一个媒体 Sample 分配器.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MediaAllocator
{
private:
	class MediaPoolBuffer : public MediaBuffer 
	{
	public:
		MediaPoolBuffer(MediaAllocator *allocator);
		long Release();
		void ResetRefcount();

	public:
		MediaPoolBuffer* fNext;			///< 指向下一个 Buffer, 用于单向链表
		MediaAllocator*  fAllocator;	///< Media Allocator
	};

// Construction/Destruction -----------------------------------
public:
	MediaAllocator();
	virtual ~MediaAllocator();

public:
	void Clear();

// Attributes --------------------------------------------------
public:
	UINT GetCapacity();
	UINT GetTotalCount();
	UINT GetFreeCount();
	void SetCapacity(UINT capacity);

// Operations -------------------------------------------------
public:
	IMediaSamplePtr NewMediaSample(UINT initSize);
	MediaBufferPtr NewMediaBuffer(UINT initSize);
	BOOL FreeMediaBuffer(MediaPoolBuffer* mediaBuffer);

// Data Members -----------------------------------------------
protected:
	MediaPoolBuffer* fFreeBuffers;	///< Free Buffers

	UINT	fCapacity;			///< The size to which a buffer can be set
	UINT	fFreeCount;			///< Free buffer count
	INT64   fLastTime;			///< 
	UINT	fTotalCount;		///< Total created buffers
	Mutex	fMutex;				///< Mutex object
};

};

#endif // !defined(_NS_VISION_MEDIA_SAMPLE_H)
