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
#ifndef _NS_VISION_INC_MEDIA_SOURCE_H
#define _NS_VISION_INC_MEDIA_SOURCE_H

#include "media/vision_media.h"

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IMediaSink class

#define kMediaSinkRenewStream	2

/** 
 * 媒体数据接收模块接口. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IMediaSink : public Object
{
public:
	/**
	 * 分发指定的帧.
	 * @param channel 通道 ID
	 * @param frame 要分发的帧
	 */
	virtual int  OnMediaDistribute(int channel, IMediaSample* mediaSample) = 0;

	/** 通知这个 Sink 数据源发生了改变. */
	virtual int  OnMediaSourceChange(UINT flags) = 0;

};

/** IMediaSink 智能指针类型. */
typedef SmartPtr<IMediaSink> IMediaSinkPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IMediaSourceTrack class 

/**
 * 代表一路音频或视频媒体轨道.
 * Represents a RTSP track.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IMediaSourceTrack : public Object
{
public:
	/** 返回当前轨道的平均码率. */
	virtual UINT   GetAvgBitrate() = 0;

	/** 返回当前轨道的实时码率. */
	virtual UINT   GetBitrate() = 0;

	/** 返回当前轨道的通道数(只对音频有效). */
	virtual UINT   GetChannels()	= 0;

	/** 返回当前轨道的编码类型. */
	virtual String GetCodecType() const = 0;

	/** 返回当前轨道的帧率. */
	virtual UINT   GetFrameRate() = 0;

	/** 返回当前轨道的采样率. */
	virtual UINT   GetFrequency() const = 0;

	/** 返回当前轨道的类型, 如 'video', 'audio'. */
	virtual String GetMediaType() const = 0;

	/** 返回当前轨道的 RTP 负载类型. */
	virtual UINT   GetPayloadType() const = 0;

	/** 返回当前轨道的 SDP 串(用于 RTSP). */
	virtual String GetRtspSdpString() = 0;

	/** 返回当前轨道的 SDP 串(用于 SIP). */
	virtual String GetSipSdpString(int mediaPort) = 0;

	/** 返回当前轨道的. */
	virtual UINT   GetSourceId() const = 0;

	/** 返回当前轨道的总共的帧数. */
	virtual UINT   GetTotalSamples() = 0;

	/** 返回当前轨道的 ID. */
	virtual UINT   GetTrackId() const = 0;

	/** 返回当前轨道的图像的高度. */
	virtual UINT   GetVideoHeight() = 0;

	/** 返回当前轨道的图像的宽度. */
	virtual UINT   GetVideoWidth()	= 0;

};

/** IMediaSourceTrack 智能指针类型. */
typedef SmartPtr<IMediaSourceTrack> IMediaSourceTrackPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IMediaSource class 

/**
 * 这个类声明了一个 IMediaSource 的抽象接口，表示视音频流。
 * 
 * 每一个 IMediaSource 对象可以包含零到多个视音频轨道。在一个 IMediaSource 对象的所
 * 有媒体轨道在渲染的时候必须是同步的。不同的 IMediaSource 对象不必同步。
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IMediaSource : public Object
{
public:
	/** 返回这个源相关的音频轨道. */
	virtual IMediaSourceTrackPtr GetAudioTrack() = 0;

	/** 返回这个源相关的视频轨道. */
	virtual IMediaSourceTrackPtr GetVideoTrack() = 0;

public:
	/** 添加一个 IMediaSink */
	virtual int  AddMediaSink(IMediaSink* sink) = 0;

	/** 分发指定的媒体帧 */
	virtual int  DeliverSample(IMediaSample* mediaSample) = 0;

	/** 返回这个源的时间长度, 如果是实时流则返回 0. */
	virtual UINT GetDuration() = 0;

	/** 返回这个源的唯一标签. */
	virtual String GetPath() = 0;

	/** 返回这个源的 RTSP SDP 串. */
	virtual String GetRtspSdpString(LPCSTR localAddress) = 0;

	/** 返回这个源的 SIP SDP 串. */
	virtual String GetSipSdpString(UINT type, int localPort) = 0;

	/** 返回这个源的 ID. */
	virtual UINT GetSourceId() = 0;

	/** 指出是否是实时源. */
	virtual BOOL IsLiveStreaming() = 0;

	/**  */
	virtual int  RenewStream() = 0;

	/** 移除指定的 IMediaSink */
	virtual int  RemoveMediaSink(IMediaSink* sink) = 0;

	virtual int  SetMediaFormat(UINT mediaType, MediaFormat* format) = 0;

	/** 设置回放范围. */
	virtual BOOL SetRange(double start, double end) = 0;

	/** 设置回放速率. */
	virtual BOOL SetScale(float scale) = 0;

};

/** IMediaSource 智能指针类型. */
typedef SmartPtr<IMediaSource> IMediaSourcePtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IMediaSource class 

class IAudioPlayer : public Object
{
public:
	virtual void WriteAudioSample(BYTE* data, UINT len) = 0;
	virtual void OnStart(LPCSTR format) = 0;
	virtual void OnEnd() = 0;

};

typedef SmartPtr<IAudioPlayer> IAudioPlayerPtr;


} // namespace media


#endif // _NS_VISION_INC_MEDIA_SOURCE_H

