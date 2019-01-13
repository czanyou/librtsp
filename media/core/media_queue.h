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
#ifndef _NS_VISION_MEDIA_QUEUE_H
#define _NS_VISION_MEDIA_QUEUE_H

#include "media/vision_media.h"

namespace media {

/** IMediaSample 列表类型 */
typedef RingList<IMediaSamplePtr, IMediaSample*> IMediaSampleRing;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaSampleQueue class

/**
 * 代表一个音视频帧缓冲区.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MediaSampleQueue : public Object
{
public:
	MediaSampleQueue();
	MediaSampleQueue(UINT mediaType);

// Attributes -------------------------------------------------
public:
	UINT GetTotalBufferSize();
	UINT GetLostPacketCount();
	UINT GetTotalBufferCount();
	UINT GetSampleCount();
	BOOL IsEmpty();

// Operations -------------------------------------------------
public:
	IMediaSamplePtr RemoveHead();
	IMediaSamplePtr RemoveTail();

public:
	BOOL AddAudioSample(IMediaSample* mediaSample);
	void Clear();
	BOOL DeliverSample(IMediaSample* mediaSample);
	void OnDump(String& dump);

// Implementation ---------------------------------------------
protected:
	BOOL AddVideoSample(IMediaSample* mediaSample);
	void OnDrop(IMediaSample* mediaSample);
	void SetQueueSize(UINT queueSize);

// Data Members -----------------------------------------------
private:
	IMediaSampleRing fMediaSamples;	///< Frame list.
	Mutex	fMutex;				///< Mutex object	
	UINT    fMediaType;			///< 缓冲区类型
	UINT    fBufferSize;		///< 
	UINT	fLostPacketCount;	///< 总共丢失的包
	UINT    fMaxBufferSize;		///< 
	BOOL	fDropPFrame;		///< Drop P Frame flag 
};

typedef SmartPtr<MediaSampleQueue> MediaSampleQueuePtr;

}; // namespace rtsp

#endif // !defined(_NS_VISION_MEDIA_QUEUE_H)
