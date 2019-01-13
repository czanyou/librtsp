
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

#include "media_track.h"

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaSourceTrack class

MediaSourceTrack::MediaSourceTrack(LPCSTR mediaType)
{
	fMediaFormat.fMediaType			= mediaType;
	fMediaFormat.fPayloadType		= 96;
	fMediaFormat.fSampleRate		= 90000;
	fTrackId			= 0;
	fSourceId			= 0;
	fSampleSequence		= 0;
	fLastActiveTime		= 0;

	if (fMediaFormat.fMediaType == "video") {
		fMediaFormat.fCodecName		= "H264";
		fMediaFormat.fPayloadType	= 96;
		fMediaFormat.fSampleRate	= 90000;
		fTrackId		= 1;
		fProfileLevelId = "42801e";
		fParameterSet	= "";
		//fParameterSet	= "Z0LgFNoFglE=,aM4wpIA=";

	} else if (fMediaFormat.fMediaType == "audio") {
		fMediaFormat.fCodecName		= "";
		fMediaFormat.fPayloadType	= 97;
		fMediaFormat.fSampleRate	= 48000;
		fTrackId		= 2;
	}
}

MediaSourceTrack::~MediaSourceTrack()
{

}

UINT MediaSourceTrack::GetAvgBitrate()
{
	return fStat.GetAvgBitrate();
}

/** Retrieves the bitrate for the stream. */
UINT MediaSourceTrack::GetBitrate()
{
	return fStat.GetAvgBitrate();
}

/** Returns the channel count of this audio track. */
UINT MediaSourceTrack::GetChannels()
{
	return fMediaFormat.fChannelCount;
}

/** Gets the codec name of this track. */
String MediaSourceTrack::GetCodecType() const
{
	return fMediaFormat.fCodecName;
}

UINT MediaSourceTrack::GetFrameRate()
{
	return fStat.GetFrameRate();
}

/** Gets the RTP timestamp frequency of this track.. */
UINT MediaSourceTrack::GetFrequency() const
{
	return fMediaFormat.fSampleRate;
}

/** Gets the media type of this track. */
String MediaSourceTrack::GetMediaType() const
{
	return fMediaFormat.fMediaType;
}

/** Sets the RTP payload type of this track.. */
UINT MediaSourceTrack::GetPayloadType() const
{
	return fMediaFormat.fPayloadType;
}

/** Gets the SDP String */
String MediaSourceTrack::GetRtspSdpString()
{
	BYTE payload = GetPayloadType();
	String sdp;
	sdp.Resize(1024);

	if (fMediaFormat.fMediaType == "video") {
		sdp.Format(
			"m=%s 0 RTP/AVP %d\r\n"
			"a=control:track%d\r\n"
			"a=rtpmap:%d %s/%d\r\n"
			"a=width:%d\r\n"
			"a=height:%d\r\n"
			"a=fmtp:%d profile-level-id=%s;packetization-mode=1;sprop-parameter-sets=%s\r\n",
			fMediaFormat.fMediaType.c_str(), payload,
			fTrackId,
			payload, fMediaFormat.fCodecName.c_str(), GetFrequency(),
			fMediaFormat.fVideoWidth, fMediaFormat.fVideoHeight,
			payload, fProfileLevelId.c_str(), fParameterSet.c_str());

	} else if (fMediaFormat.fMediaType == "audio") {
		if (fMediaFormat.fCodecName.IsEmpty() || fMediaFormat.fCodecName == "MUTE" || fMediaFormat.fChannelCount == 0) {
			return "";
		}

		sdp.Format(
			"m=%s %d RTP/AVP %d\r\n"
			"a=rtpmap:%d %s/%d/%d\r\n"
			"a=control:track%d\r\n",
			fMediaFormat.fMediaType.c_str(), 0, payload,
			payload, fMediaFormat.fCodecName.c_str(), GetFrequency(), 
			fMediaFormat.fChannelCount,
			fTrackId);

		if (!fMediaFormat.fFmtp.IsEmpty()) {
			String fmtp;
			fmtp.Resize(256);
			fmtp.Format("a=fmtp:%d %s\r\n", payload, fMediaFormat.fFmtp.c_str());
			sdp += fmtp;
		}
	}

	//LOG_D("%s\r\n", sdp.c_str());
	return sdp;
}

/** Gets the SDP String */
String MediaSourceTrack::GetSipSdpString( int mediaPort )
{
	BYTE payload = GetPayloadType();

	String sdp;
	sdp.Resize(1024);

	if (fMediaFormat.fMediaType == "video") {
		sdp.Format(
			"m=%s %d RTP/AVP %d\r\n"
			"a=rtpmap:%d %s/%d\r\n"
			"a=width:%d\r\n"
			"a=height:%d\r\n"
			"a=fmtp:%d profile-level-id=%s;packetization-mode=1;sprop-parameter-sets=%s\r\n"
			//"a=sendrecv\r\n"
			"a=sendonly\r\n"
			"",
			fMediaFormat.fMediaType.c_str(), mediaPort, payload,
			payload, fMediaFormat.fCodecName.c_str(), GetFrequency(),
			fMediaFormat.fVideoWidth, fMediaFormat.fVideoHeight,
			payload, fProfileLevelId.c_str(), fParameterSet.c_str());

	} else if (fMediaFormat.fMediaType == "audio") {
		if (fMediaFormat.fCodecName.IsEmpty() || fMediaFormat.fCodecName == "MUTE" || fMediaFormat.fChannelCount == 0) {
			return "";
		}

		sdp.Format(
			"m=%s %d RTP/AVP %d 101\r\n"
			"a=rtpmap:%d %s/%d/%d\r\n"
			"a=fmtp:101 0-11\r\n"
			"a=rtpmap:101 telephone-event/8000\r\n"
			"a=sendrecv\r\n",
			fMediaFormat.fMediaType.c_str(), 
			mediaPort, payload,
			payload, fMediaFormat.fCodecName.c_str(), GetFrequency(), 
			fMediaFormat.fChannelCount);
	}
	return sdp;
}

/** Gets the channel number of this track. */
UINT MediaSourceTrack::GetSourceId() const
{
	return fSourceId;
}

MediaTrackStat& MediaSourceTrack::GetStat()
{
	return fStat;
}

UINT MediaSourceTrack::GetTotalSamples()
{
	return fStat.GetTotalFrame();
}

/** Gets the track ID. of this track. */
UINT MediaSourceTrack::GetTrackId() const
{
	return fTrackId;
}

UINT MediaSourceTrack::GetVideoHeight()
{
	return fMediaFormat.fVideoHeight;
}

UINT MediaSourceTrack::GetVideoWidth()
{
	return fMediaFormat.fVideoWidth;
}

BOOL MediaSourceTrack::IsActive()
{
	INT64 now = GetSysTickCount();
	if (UINT(now - fLastActiveTime) > 1000) {
		return FALSE;
	}

	return TRUE;
}

/** Indicates whether the stream is ready now. */
BOOL MediaSourceTrack::IsReady()
{
	return TRUE;
}

void MediaSourceTrack::OnDump(String& dump)
{
	String mediaType = fMediaFormat.fMediaType;
	if (!fMediaFormat.fCodecName.IsEmpty()) {
		mediaType += "/";
		mediaType += fMediaFormat.fCodecName;
	}

	DumpCell(dump, "%d: %s", fTrackId, mediaType.c_str());
	DumpCell(dump, "%d", fMediaFormat.fPayloadType);
	DumpCell(dump, "%d", fMediaFormat.fSampleRate);
	if (fMediaFormat.fVideoWidth) {
		DumpCell(dump, "%dx%d", fMediaFormat.fVideoWidth, fMediaFormat.fVideoHeight);
	} else {
		DumpCell(dump, "-");
	}

	DumpCell(dump, "%d", fSampleSequence);
}

/**
 * 这个方法主要用于过滤 I 帧, 取得 fmtp 的相应的参数值.
 * @param mediaSample 要处理的帧
 */
void MediaSourceTrack::OnVideoSyncPoint( IMediaSample* mediaSample )
{
	// 提取 sprop-parameter-sets 参数的值
	if (mediaSample == NULL) {
		return;

	} else if (mediaSample->GetBufferCount() <= 0) {
		fParameterSet = "";
		return;

	} else if (fMediaFormat.fCodecName != "H264") {
		fParameterSet = "";
		return;
	}

	char buffer[MAX_PATH + 1];
	memset(buffer, 0, sizeof(buffer));

	String picSet, seqSet;

	int count = mediaSample->GetBufferCount();
	for (int i = 0; i < count; i++) {
		IMediaBufferPtr mediaBuffer = mediaSample->GetBuffer(i);
		if (mediaBuffer == NULL) {
			break;
		}

		BYTE* data = mediaBuffer->GetData();
		int   size = mediaBuffer->GetLength();
		if (data == NULL || size <= 0) {
			break;
		}

		if (data[0] == 0x00 && data[1] == 0x00) {
			UINT startSize = 0;

			if (data[2] == 0x01) {
				startSize = 3;

			} else if (data[2] == 0x00 && data[3] == 0x01) {
				startSize = 4;
			}

			if (startSize) {
				data += startSize;
				size -= startSize;
			}
		}

		BYTE naluType = data[0] & 0x1f;
		if (naluType == 0x07) { // 序列参数集
			// Profile Level 
			snprintf(buffer, MAX_PATH, "%02x%02x%02x", data[1], data[2], data[3]);
			fProfileLevelId = buffer;
			seqSet = StringUtils::Base64Encode(data, size, buffer, MAX_PATH);

		} else if (naluType == 0x08) { // 图像参数集
			picSet = StringUtils::Base64Encode(data, size, buffer, MAX_PATH);
			break;
		}
	}

	fParameterSet = seqSet;
	fParameterSet += ",";
	fParameterSet += picSet;
}

/** 
 * 预处理指定的一帧数据. 主要是初始化 RTP 头的信息等.
 * @param mediaSample 要预处理的数据帧
 */
void MediaSourceTrack::PreProcessSample(IMediaSample* mediaSample)
{
	ASSERT(mediaSample != NULL);
	if (mediaSample == NULL) {
		return;
	}

	// Initialize packet header
	int count = mediaSample->GetBufferCount();
	for (int i = 0; i < count; i++) {
		IMediaBufferPtr mediaBuffer = mediaSample->GetBuffer(i);
		if (mediaBuffer) {
			mediaBuffer->SetPayloadType(fMediaFormat.fPayloadType);
		}
	}

	// 
	if (mediaSample->IsSyncPoint()) {
		if (fMediaFormat.fMediaType == "video") {
			OnVideoSyncPoint(mediaSample);
		}
	}
}

/** 分发指定的帧. */
int MediaSourceTrack::ProcessSample(IMediaSample* mediaSample)
{
	if (mediaSample == NULL) {
		fStat.OnMediaSample(0);
		return 0;
	}

	mediaSample->SetFrequency(fMediaFormat.fSampleRate);

	// 预处理这一帧
	PreProcessSample(mediaSample);

	// Stat...
	fStat.OnMediaSample(mediaSample->GetSampleSize());
	fLastActiveTime = GetSysTickCount();

	return 1;
}

void MediaSourceTrack::Reset()
{
	fSampleSequence	= 0;
}

/** Sets the channel count of this audio track. */
void MediaSourceTrack::SetChannels( UINT channels ) 
{
	fMediaFormat.fChannelCount = channels;
}

/** Sets the codec name of this track. */
void MediaSourceTrack::SetCodecType( LPCSTR subType )
{
	fMediaFormat.fCodecName = subType ? subType : "";
}

/** Sets the value of the fmtp SDP attribute. */
void MediaSourceTrack::SetFmtp( LPCSTR fmtp )
{
	fMediaFormat.fFmtp = fmtp ? fmtp : "";
}

/** Sets the RTP timestamp frequency of this track.. */
void MediaSourceTrack::SetFrequency( UINT frequency )
{
	fMediaFormat.fSampleRate = frequency;
}

/** 设置这个 Track 的相关属性. */
void MediaSourceTrack::SetMediaFormat( MediaFormat* format )
{
	if (format == NULL) {
		return;
	}

	SetChannels	  (format->fChannelCount);
	SetFmtp		  (format->fFmtp);
	SetFrequency  (format->fSampleRate);
	SetPayloadType(format->fPayloadType);
	SetCodecType  (format->fCodecName);
	SetVideoHeight(format->fVideoHeight);
	SetVideoWidth (format->fVideoWidth);

	//LOG_I("%s (%d-%d) %d,%d\r\n", propertis->format, 
	//	fMediaFormat.fRtpPayload, fChannelId, fWidth, fHeight);

	if (fMediaFormat.fMediaType == "audio") {
		if (fMediaFormat.fCodecName == "G726-16") {
			SetPayloadType(RTP_PT_G726_16);

		} else if (fMediaFormat.fCodecName == "G726-24") {
			SetPayloadType(RTP_PT_G726_24);

		} else if (fMediaFormat.fCodecName == "G726-32") {
			SetPayloadType(RTP_PT_G726_32);

		} else if (fMediaFormat.fCodecName == "G726-40") {
			SetPayloadType(RTP_PT_G726_40);

		} else if (fMediaFormat.fCodecName == "PCMA") {
			SetPayloadType(RTP_PT_PCMA);

		} else if (fMediaFormat.fCodecName == "PCMU") {
			SetPayloadType(RTP_PT_PCMU);

		} else if (fMediaFormat.fCodecName == "AMR" || fMediaFormat.fCodecName == "AMR-DTX") {
			SetPayloadType(RTP_PT_AMR);
			SetFmtp("octet-align=1");

		} else if (fMediaFormat.fCodecName == "AAC" || fMediaFormat.fCodecName == "mpeg4-generic") {
			SetPayloadType(RTP_PT_AAC);
			SetFmtp("streamtype=5; profile-level-id=15; mode=AAC-hbr; "
				"config=1210; sizeLength=13; indexLength=3; indexDeltaLength=3; "
				"CTSDeltaLength=0; DTSDeltaLength=0;");

		} else {
			SetPayloadType(96);
		}

	} else if (fMediaFormat.fMediaType == "video") {
		SetPayloadType(RTP_PT_H264);
		// TODO:
	}
}

/** Sets the media type of this track. */
void MediaSourceTrack::SetMediaType( LPCSTR mediaType )
{
	fMediaFormat.fMediaType = mediaType ? mediaType : "";
}

/** Gets the RTP payload type of this track.. */
void MediaSourceTrack::SetPayloadType( UINT payloadType )
{
	fMediaFormat.fPayloadType = payloadType;
}

/** Sets the channel number of this track. */
void MediaSourceTrack::SetSourceId( UINT sourceId )
{
	fSourceId = sourceId;
}

/** Sets the track ID. of this track. */
void MediaSourceTrack::SetTrackId( UINT trackId )
{
	fTrackId = trackId;
}

void MediaSourceTrack::SetVideoHeight( UINT height )
{
	fMediaFormat.fVideoHeight = height;
}

void MediaSourceTrack::SetVideoWidth( UINT width )
{
	fMediaFormat.fVideoWidth  = width;
}

};
