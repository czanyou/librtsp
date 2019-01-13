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

#include "media/vision_media.h"
#include "media_track_stat.h"

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaTrackStat class

MediaTrackStat::MediaTrackStat()
{
	Clear();
	fAvgBitRate.Resize(60);
}

/** 重置所有的变量. */
void MediaTrackStat::Clear()
{
	fTotalFrameCount	= 0;
	fTotalByteCount		= 0;

	fFrameRate			= 0;
	fBitRate			= 0;

	fIntervalTime		= 0;
	fFrameCount			= 0;
	fFrameBytes			= 0;
}

UINT MediaTrackStat::GetAvgBitrate()
{
	return fAvgBitRate.GetAvgValue(5);
}

UINT MediaTrackStat::GetBitrate()
{
	return fBitRate;
}

String MediaTrackStat::GetBitrateText()
{
	String ret;
	double bitrate = GetBitrate() * 8;
	if (bitrate < 1024) {
		ret.Format("%dbps", int(bitrate));

	} else if (bitrate < 1024 * 1024) {
		bitrate = bitrate / 1024.0;
		ret.Format("%.1fkbps", bitrate);

	} else {
		bitrate = bitrate / (1024.0 * 1024.0);
		ret.Format("%.1fmbps", bitrate);
	}

	return ret;
}

UINT MediaTrackStat::GetFrameRate()
{
	return fFrameRate;
}

UINT MediaTrackStat::GetTotalBytes()
{
	return fTotalByteCount;
}

UINT MediaTrackStat::GetTotalFrame()
{
	return fTotalFrameCount;
}

/** 统计音频和视频的码流, 帧率等. */
void MediaTrackStat::OnMediaSample(UINT sampleSize)
{
	INT64 now = time(NULL);

	if (sampleSize > 0) {
		fTotalFrameCount++;
		fTotalByteCount += sampleSize;

		fFrameCount++;
		fFrameBytes += sampleSize;
	}

	if (UINT(now - fIntervalTime) >= 1) {
		fIntervalTime = now;

		fFrameRate	= fFrameCount;
		fBitRate	= fFrameBytes;

		fAvgBitRate.Append(fBitRate);

		// 重新开始统计
		fFrameCount = 0;
		fFrameBytes = 0;
	}	
}

}
