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
#ifndef _NS_VISION_MEDIA_BUFFER_H
#define _NS_VISION_MEDIA_BUFFER_H

#include "media/vision_media.h"

namespace media {

const UINT kRtpDefaultPacketSize	= 1451;	///< 默认的 RTP 包的最大大小

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaBuffer class

/** 
 * 包含了一个媒体数据块.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MediaBuffer : public IMediaBuffer
{
// Construction/Destruction -----------------------------------
public:
	MediaBuffer();
	MediaBuffer(UINT maxLength);
	virtual ~MediaBuffer();

public:
	long AddRef();
	long Release();

// Attributes -------------------------------------------------
public:
	BYTE* GetData();
	UINT  GetFragmentCount() const;
	UINT  GetFragmentSequence() const;
	UINT  GetFrameType() const;
	UINT  GetLength() const;
	UINT  GetMaxSize() const;
	UINT  GetPayloadType() const;
	INT64 GetSampleTime() const;

	BOOL  IsMarker() const;
	BOOL  IsSyncPoint() const;

	void  SetFragmentCount(UINT total);
	void  SetFragmentSequence(UINT sequence);
	void  SetFrameType(UINT type);
	void  SetLength(UINT length);
	void  SetMarker(BOOL marker);
	void  SetPayloadType(UINT type);
	void  SetSyncPoint(BOOL syncPoint);
	void  SetSampleTime(INT64 timestamp);

// Data members -----------------------------------------------
protected:
	BYTE* fBuffer;				///< 指向这个缓存区的数据的缓存区开始位置
	UINT  fFragmentCount;		///< 这个缓存区的所属单元分片数量
	UINT  fFragmentSequence;	///< 这个缓存区的所属单元分片序号
	UINT  fFrameType;			///< 这个缓存区的类型
	BOOL  fIsSyncPoint;			///< 这个缓存区 是否是同步点
	BOOL  fIsMarker;			///< 这个缓存区的标记
	UINT  fMaxLength;			///< 这个缓存区 的缓存区的最大长度
	Mutex fMutex;				///< 这个缓存区的互斥锁
	UINT  fPayloadLength;		///< 这个缓存区的有效负载长度 (不包括头部分)
	UINT  fPayloadType;			///< 这个缓存区的有效负载类型
	long  fRefCount;			///< 这个缓存区的引用数
	INT64 fSampleTime;			///< 这个缓存区的采集时间.
};

/** RtpPacket  智能指针类型. */
typedef SmartPtr<MediaBuffer> MediaBufferPtr;

};

#endif // !defined(_NS_VISION_MEDIA_BUFFER_H)
