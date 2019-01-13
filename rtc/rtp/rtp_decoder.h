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
#ifndef _NS_VISION_RTP_DECODER_H_
#define _NS_VISION_RTP_DECODER_H_

#include "media/vision_media.h"
#include "rtp_common.h"

namespace rtp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtpH264NaluType class

/** H.264 NAL 单元类型. */
enum RtpH264NaluType 
{
	kRtp264NaluUnknown		= 0, 
	kRtp264NaluSingleMax	= 23, 
	kRtp264NaluSTAP_A		= 24, 
	kRtp264NaluSTAP_B		= 25, 
	kRtp264NaluMTAP16		= 26, 
	kRtp264NaluMTAP24		= 27, 
	kRtp264NaluFU_A			= 28, 
	kRtp264NaluFU_B			= 29
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtpDecoder class

class RtpDecoder
{
public:
	RtpDecoder();

public:


public:
	IMediaBufferPtr DecodeH264Packet ( RtpHeader* header, UINT rtpLength );
	IMediaBufferPtr DecodeAudioPacket( RtpHeader* header, UINT rtpLength );
	IMediaBufferPtr DecodeVideoPacket( RtpHeader* header, UINT rtpLength );

public:
	static UINT GetStartCodeLength(const BYTE* sample);
};


}

#endif // _NS_VISION_RTP_DECODER_H_
