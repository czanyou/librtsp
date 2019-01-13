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
#ifndef _NS_VISION_RTSP_READER_H
#define _NS_VISION_RTSP_READER_H

#include "rtsp_message.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspReader class

/** 
 * 这个类负责读取并解析通过 Socket 收到的 RTSP/RTP 消息. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspReader
{
public:
	RtspReader();
	virtual ~RtspReader();

// Attributes -------------------------------------------------
public:
	BOOL IsEmpty(); ///< 指出缓存区是否为空
	BOOL IsRtpPacket();
	void OnDump(String& dump);

// Operations -------------------------------------------------
public:
	RtspMessagePtr GetNextMessage();

	void  ClearAll();
	char* GetNextPacket();
	BYTE* LockBuffer( UINT& freeSize );
	void  UnlockBuffer( UINT size );

private:

// Data Members -----------------------------------------------
protected:
	ByteBufferPtr fReadBuffer;		///< 接收缓存区
	RtspMessagePtr fLastMessage;	///< RTSP message object, 最后收到的 RTSP 消息
	UINT fContentLength;			///< 要读取的 RTSP 消息内容的长度
};

}

#endif // _NS_VISION_RTSP_READER_H
