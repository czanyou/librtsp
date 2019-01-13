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
#ifndef _NS_VISION_MEDIA_TRACK_STATISTICS_H
#define _NS_VISION_MEDIA_TRACK_STATISTICS_H

#include "core/utils/debug.h"

using namespace core::util;

namespace media {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MediaTrackStat class

/** 
 * 代表 RTP Session Track Statistics. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class MediaTrackStat
{
public:
	MediaTrackStat();

// Attributes -------------------------------------------------
public:
	String GetBitrateText();

	UINT GetAvgBitrate();	///< 返回平均码率
	UINT GetBitrate();		///< 返回当前码率
	UINT GetFrameRate();	///< 返回当前帧率
	UINT GetTotalBytes();	///< 返回总共的字节
	UINT GetTotalFrame();	///< 返回总共的帧数

// Operations --------------------------------------------------
public:	
	void OnMediaSample(UINT sampleSize);
	void Clear();

// Data Members -----------------------------------------------
private:
	AvgNumber fAvgBitRate;		///< 统计平均码率
	UINT  fBitRate;				///< Current Bit Rate

	UINT  fFrameBytes;			///< 字节数
	UINT  fFrameCount;			///< 帧数
	UINT  fFrameRate;			///< Current Frame Rate
	INT64 fIntervalTime;		///< 统计间隔

	UINT  fTotalByteCount;		///< 总共统计的字节数
	UINT  fTotalFrameCount;		///< 总共统计的帧数
};

}; // namespace media

#endif // _NS_VISION_MEDIA_TRACK_STATISTICS_H
