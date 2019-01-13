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
#ifndef _NS_VISION_MEDIA_MEPG_TS_H
#define _NS_VISION_MEDIA_MEPG_TS_H

#include "core/utils/debug.h"

using namespace core::util;

namespace media {

class MediaStreamMuxer
{
public:
	MediaStreamMuxer();
	~MediaStreamMuxer();

// Operations -------------------------------------------------
public:
	void Close();
	int  Write(IMediaBuffer *sample);
	void SetFilename(LPCSTR filename);

// Implementation ---------------------------------------------
private:
	int  OpenOutStream();

	int  WriteESSample(BYTE* data, UINT length, INT64 pts, BOOL isEnd);
	int  WritePATPacket();
	int  WritePCR(BYTE* buffer, INT64 pcr);
	int  WritePESHeader(BYTE* buffer, INT64 pts);
	int  WritePESPacket(BYTE* data, UINT length, INT64 pts);
	int  WritePMTPacket();
	int  WriteTSPacket(BYTE* data, UINT length);

// Data Members -----------------------------------------------
private:
	String fFileName;
	FILE* fFile;		
	BYTE* fFrameBuffer;
	UINT fFrameSize;
	UINT fIndex;
	BOOL fIsStop;
	BYTE fContinuityCounter;
	BYTE fPATContinuityCounter;
	BYTE fPMTContinuityCounter;
};

}

#endif // _NS_VISION_MEDIA_MEPG_TS_H
