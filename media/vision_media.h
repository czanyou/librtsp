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
#ifndef _NS_VISION_MEDIA_API_H
#define _NS_VISION_MEDIA_API_H

#include "core/base/base.h"

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaMajorTypes enum

/** 
 * Major media format type. 
 * 常见的媒体格式.
 * In a media type, the major type describes the overall category of the data, 
 * such as audio or video. 
 */
enum MediaMajorTypes 
{
	kMediaTypeVideo		= 0,	///< 视频类型, Video media type.
	kMediaTypeAudio		= 1,	///< 音频类型, Audio media type.
	kMediaTypeText		= 2,	///< 文件类型, Text stream
	kMediaTypeImage		= 3,	///< 图像类型, Still image stream.
	kMediaTypeHTML		= 4,	///< HTML 流数据, HTML stream.
	kMediaTypeBinary	= 5,	///< 原始数据类型, Binary stream.
	kMediaTypeTunnel	= 100	///< 原始数据类型, Binary stream.
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaSubTypes enum

/** 
 * 常见的音视频编码类型. 
 * The subtype GUID defines a specific media format type within a major type.
 * For example, within video, the subtypes include RGB-24, RGB-32, UYVY, AYUV, 
 * and so forth. Within audio, the subtypes include PCM audio, AAC, and so forth.
 */
enum MediaSubTypes
{
	// Audio Formats
	kAudioFormatAAC = 1,	// Advanced Audio Coding (AAC).
	kAudioFormatADPCM_IMA,
	kAudioFormatADPCM_MS,
	kAudioFormatAMR_NB,		// Adaptative Multi-Rate audio
	kAudioFormatAMR_WB,
	kAudioFormatG726_16,	// 
	kAudioFormatG726_24,
	kAudioFormatG726_32,
	kAudioFormatG726_40,
	kAudioFormatMP3,		// MPEG Audio Layer-3 (MP3).
	kAudioFormatPCM,
	kAudioFormatPCMA,		// 
	kAudioFormatPCMU,		// 

	// Video Encode Formats
	kVideoFormatH263,		// 'H263' H.263 video.
	kVideoFormatH264,		// 'H264' H.264 video.
	kVideoFormatMJPG,		// 'MJPG' Motion JPEG.
	kVideoFormatMP4V,		// 'MP4V' MPEG-4 part 2 video.

	// Pixel Formats
	kPixelFormatRGB24,		// RGB, 24 bpp.
	kPixelFormatRGB32,		// RGB, 32 bpp.
	kPixelFormatRGB565,		// RGB 565, 16 bpp
	kPixelFormatYV12,		// 'YV12' YUV 4:2:0 Planar

	kMediaSubTypeCount		// 
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaFrameTypes enum

/** 
 * Video / Audio frame type. 
 * 常见的音视频帧类型.
 */
enum MediaFrameTypes 
{
	kMediaFrameUnknown		= -1,	///< 未知的媒体帧

	kVideoFrameI			= 0,	///< 关键帧
	kVideoFrameP			= 1,	///< 向前预测帧
	kVideoFrameB			= 2,	///< 前后预测帧
	kVideoFrameRB			= 3,	///< 前后预测帧

	kAudioFrame				= 10,	///< 音频帧

	kMediaFrameText			= 20,	///< 表示这是一个包含文本信息的帧
	kMediaFrameMeta			= 100,	///< 元数据帧, 用于描述当前流的新的属性
	kMediaFrameRaw			= 119,	///< 包含原始数据的帧
	kMediaFrameEnd			= 9999	///< 表示这是一个当前流结束标记帧
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaFormat class

/** 
 * 代表媒体格式信息. 
 * Encapsulates the information describing the format of media data, be it audio or video.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MediaFormat
{
public:
	MediaFormat() 
	{
		fBitRate		= 0;
		fChannelCount	= 0;
		fCodecType		= 0;
		fFrameRate		= 0;
		fPayloadType	= 0;

		fSampleRate		= 0;
		fVideoHeight	= 0;
		fVideoWidth		= 0;
	}

	virtual ~MediaFormat() 
	{

	}

public:
	virtual void Clear() {
		fBitRate		= 0;
		fChannelCount	= 0;
		fCodecType		= 0;
		fFrameRate		= 0;
		fPayloadType	= 0;

		fSampleRate		= 0;
		fVideoHeight	= 0;
		fVideoWidth		= 0;

		fCodecName.Clear();
		fFmtp.Clear();
		fMediaType.Clear();
	}

// Data Members -----------------------------------------------
public:
	String fCodecName;		///< Track format name / 当前媒体格式名称
	String fFmtp;			///< RTP SDP FMTP 属性
	String fMediaType;		///< Media type

	UINT fBitRate;			///< 
	UINT fChannelCount;		///< Audio Channel Count / 音频通道数
	UINT fCodecType;		///< Codec Type, 编码类型
	UINT fFrameRate;		///< 
	UINT fPayloadType;		///< RTP 负载格式
	UINT fSampleRate;		///< RTP 频率
	UINT fVideoHeight;		///< Video height
	UINT fVideoWidth;		///< Video width
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IMediaBuffer interface

/** 
 * 代表一个媒体样本, 包含了一个媒体数据块, 用于在各个模块中共享和传递媒体数据.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IMediaBuffer
{
public:
	/** 递增引用计数. */
	virtual long AddRef() = 0;

	/** 递减引用计数. */
	virtual long Release() = 0;

// Attributes -------------------------------------------------
public:
	/** Retrieves the pointer to the actual payload data. */
	virtual BYTE* GetData() = 0;

	/** Retrieves the total count of the fragments. */
	virtual UINT  GetFragmentCount() const = 0;

	/** Retrieves the sequence of the fragment. */
	virtual UINT  GetFragmentSequence() const = 0;

	/** Returns the type of the packet. */
	virtual UINT  GetFrameType() const = 0;

	/** Retrieves the length of the payload data. */
	virtual UINT  GetLength() const = 0;

	/** Retrieves the maximum size to which a buffer can be set. */
	virtual UINT  GetMaxSize() const = 0;

	/** Returns the payload type of the packet. */
	virtual UINT  GetPayloadType() const = 0;

	/** Returns the timestamp of the packet. */
	virtual INT64 GetSampleTime() const = 0;

	/** Returns TRUE if the marker bit was set and FALSE otherwise. */
	virtual BOOL  IsMarker() const = 0;

	/** Returns TRUE if it's sync point. */
	virtual BOOL  IsSyncPoint() const = 0;

	/** Sets the count of the packet. */
	virtual void  SetFragmentCount(UINT total) = 0;

	/** Sets the sequence of the packet. */
	virtual void  SetFragmentSequence(UINT sequence) = 0;

	/** Sets the type of the packet. */
	virtual void  SetFrameType(UINT type) = 0;

	/** Sets the length of the payload data. */
	virtual void  SetLength(UINT length) = 0;

	/** Sets TRUE if the marker bit was set and FALSE otherwise. */
	virtual void  SetMarker(BOOL marker) = 0;

	/** Sets the payload type of the packet. */
	virtual void  SetPayloadType(UINT type) = 0;

	/** Sets TRUE if it's sync point. */
	virtual void  SetSyncPoint(BOOL syncPoint) = 0;

	/** Sets the timestamp of the packet. */
	virtual void  SetSampleTime(INT64 timestamp) = 0;

};

typedef SmartPtr<IMediaBuffer> IMediaBufferPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IMediaSample interface

/** 
 * 代表一个完整的音视频帧, 如一帧完整的图像. 它通常由多个媒体样本组成.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IMediaSample
{
public:
	IMediaSample() { Clear(); }

// Properties ------------------------------------------------
public:
	/** 返回当前帧指定索引的媒体数据块. */
	virtual IMediaBufferPtr GetBuffer(UINT index) const = 0;

	/** 返回当前帧包含的媒体样本的个数. */
	virtual UINT GetBufferCount() const = 0;

	/** 返回当前帧总共长度. */
	virtual UINT GetSampleSize() const = 0;

public:
	/** 返回当前帧的应用程序数据. */
	UINT GetTag() const			{ return fSampleTag;		}

	/** 返回当前帧的类型, 参考 MediaFrameTypes enum. */
	UINT GetFrameType() const		{ return fSampleType;	}

	/** 返回当前帧的媒体采样频率. */
	UINT GetFrequency() const		{ return fFrequency;	}

	/** 返回当前帧的媒体格式. */
	UINT GetMediaType() const		{ return fMediaType;	}

	/** 返回当前帧的时间戳, 单位为毫秒. */
	INT64 GetSampleTime() const		{ return fSampleTime;	}

	/** . */
	BOOL IsSyncPoint() const		{ return fIsSyncPoint; }

	/** 设置当前帧的应用程序数据. */
	void SetTag(UINT cookie)		{ fSampleTag	= cookie;	}

	/** 设置当前帧的类型, 参考 enumFrameType. */
	void SetFrameType(UINT type)	{ fSampleType	= type;		}

	/** 设置当前帧的频率. */
	void SetFrequency(UINT value)	{ fFrequency	= value;	}

	/** 设置当前帧的格式. */
	void SetMediaType(UINT format)	{ fMediaType	= format;	}

	/** 设置当前帧的时间戳, 单位为毫秒. */
	void SetSampleTime(INT64 value)	{ fSampleTime	= value;	}

	/** . */
	void SetSyncPoint(BOOL value)	{ fIsSyncPoint	= value;	}

// Operations -------------------------------------------------
public:
	/** 递增引用计数. */
	virtual long AddRef() = 0;	

	/** 添加一个媒体样本. */
	virtual int  AddBuffer(IMediaBuffer* mediaBuffer) = 0;

	/** 清空所有内容. */
	virtual void Clear() {
		fSampleTag		= 0;
		fFrequency		= 0;
		fIsSyncPoint	= 0;
		fMediaType		= 0;
		fSampleTime		= 0;
		fSampleType		= 0;
	}

	/** 递减引用计数. */
	virtual long Release() = 0;

// Data Members -----------------------------------------------
private:
	INT64 fSampleTime;		///< 采集这一帧的时间, 单位为毫秒
	BOOL  fIsSyncPoint;		///< 指出是否是一个同步点
	UINT  fSampleTag;		///< 用户数据
	UINT  fFrequency;		///< Timestamp frequency of the frame. (Not the RTP timestamp frequency)
	UINT  fMediaType;		///< Media compress format (ex: MPEG4, PCM, ....)
	UINT  fSampleType;		///< Frame type (ex: I FRAME, P FRAME ....)
};

typedef SmartPtr<IMediaSample> IMediaSamplePtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtpPayloadType class

/** 常见的 RTP 负载类型. */
enum RtpPayloadTypes 
{
	RTP_PT_PCMU				= 0,
	RTP_PT_TENSIXTEEN		= 1,
	RTP_PT_G726_32			= 2,
	RTP_PT_GSM				= 3,
	RTP_PT_G723				= 4,
	RTP_PT_DVI4_8000		= 5,
	RTP_PT_DVI4_16000		= 6,
	RTP_PT_LPC				= 7,
	RTP_PT_PCMA				= 8,
	RTP_PT_G722				= 9,
	RTP_PT_L16_2CH			= 10,
	RTP_PT_L16_1CH			= 11,
	RTP_PT_QCELP			= 12,
	RTP_PT_CN				= 13,
	RTP_PT_MPA				= 14,
	RTP_PT_G728				= 15,
	RTP_PT_DVI4_11025		= 16,
	RTP_PT_DVI4_22050		= 17,
	RTP_PT_729				= 18,
	RTP_PT_CN_DEPRECATED	= 19,
	RTP_PT_G726_40			= 21,
	RTP_PT_G726_24			= 22,
	RTP_PT_G726_16			= 23,
	RTP_PT_CELB				= 25,
	RTP_PT_JPEG				= 26,
	RTP_PT_NV				= 28,
	RTP_PT_H261				= 31,
	RTP_PT_MPV				= 32,
	RTP_PT_MP2T				= 33,
	RTP_PT_H263				= 34,
	RTP_PT_AMR				= 97,
	RTP_PT_AAC				= 98,
	RTP_PT_H264				= 99,
	RTP_PT_MAX_LENGTH
};

} // namespace media

using namespace media;

#endif // _NS_VISION_MEDIA_API_H

