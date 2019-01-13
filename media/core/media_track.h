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

#ifndef _NS_VISION_MEDIA_TRACK_H
#define _NS_VISION_MEDIA_TRACK_H

#include "media/vision_media.h"
#include "media/vision_media_source.h"
#include "media_track_stat.h"

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSourceTrack class 

/**
 * 代表一路音频或视频媒体流.
 * Represents a RTSP track.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MediaSourceTrack : public IMediaSourceTrack
{
// Construction/Destruction -----------------------------------
public:
	MediaSourceTrack(LPCSTR type);
	virtual ~MediaSourceTrack();
	typedef LinkedList<IMediaSinkPtr, IMediaSinkPtr> MediaSinkList; ///< IMediaSink 列表类型

// Attributes -------------------------------------------------
public:
	MediaTrackStat& GetStat();	///< 返回当前流统计信息.
	UINT   GetAvgBitrate();
	UINT   GetBitrate();
	UINT   GetChannels();
	String GetCodecType() const;
	UINT   GetFrequency() const;
	UINT   GetFrameRate();
	String GetMediaType() const;
	UINT   GetPayloadType() const;
	String GetRtspSdpString();
	String GetSipSdpString(int mediaPort);
	UINT   GetSourceId() const;
	UINT   GetTotalSamples();
	UINT   GetTrackId() const;
	UINT   GetVideoHeight();	///< 返回图像的高度
	UINT   GetVideoWidth();		///< 返回图像的宽度
  
	BOOL   IsReady();
	BOOL   IsActive();

	void SetChannels(UINT channels);
	void SetCodecType(LPCSTR codecName);
	void SetFmtp(LPCSTR fmtp);
	void SetFrequency(UINT frequency);
	void SetMediaType(LPCSTR name);
	void SetMediaFormat(MediaFormat* format);
	void SetPayloadType(UINT payloadType);
	void SetSourceId(UINT sourceId);
	void SetTrackId(UINT trackId);
	void SetVideoHeight(UINT height);	///< 设置图像的高度
	void SetVideoWidth(UINT width);		///< 设置图像的宽度

// Virtual Operations -----------------------------------------
public:
	int  ProcessSample(IMediaSample* sample);
	void OnDump(String& dump);
	void Reset();

// Implementation ---------------------------------------------
protected:
	void PreProcessSample(IMediaSample* sample);		/** 预处理指定的帧. */
	void OnVideoSyncPoint(IMediaSample* sample);	/** 预处理指定的帧. */

// Data members -----------------------------------------------
protected:
	MediaTrackStat fStat;		///< Track rate statistics	
	MediaFormat	fMediaFormat;	///< 

	UINT	fSampleSequence;		///< Frame sequence counter
	INT64   fLastActiveTime;	///< 
	Mutex	fMutex;				///< Mutex
	String  fParameterSet;		///< The FMTP attribute value
	String  fProfileLevelId;	///< The H.264 video profile, profile level and ID.
	UINT	fSourceId;			///< Channel number of the stream
	UINT	fTrackId;			///< Specifies the RTSP track number.
};

/** RtpTrack 智能指针类型. */
typedef SmartPtr<MediaSourceTrack> MediaSourceTrackPtr;

}; // namespace rtsp

#endif // !defined(_NS_VISION_MEDIA_TRACK_H)
