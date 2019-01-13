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

#ifndef _NS_VISION_RTSP_CLIENT_H
#define _NS_VISION_RTSP_CLIENT_H

#include "core/net/nio_url.h"
#include "rtsp_common.h"
#include "rtsp_reader.h"
#include "rtsp_socket.h"
#include "rtc/vision_rtsp_client.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspClient class

class RtspVideoDecoder
{
public:
	/** H.264 NAL 单元类型. */
	enum NalUnitType 
	{
		INVLIAD_NALU = 0, 
		MAX_SINGLE_NALU_TYPE = 23, 
		STAP_A	= 24, 
		STAP_B	= 25, 
		MTAP16	= 26, 
		MTAP24	= 27, 
		FU_A	= 28, 
		FU_B	= 29
	};

public:
	RtspVideoDecoder();

public:
	IMediaBufferPtr Decode(BYTE* data, int length, UINT flags, UINT timestamp); 
	void Reset();
	UINT GetFrameType(int format, BYTE* sample);

public:
	UINT fAudioCodec;		///< 
	UINT fVideoCodec;		///< 

	int	 fAudioPayload;		///< 
	UINT fLastSampleFlags;	///< 上一个 Sample 的标识
	UINT fLastSequence;		///< 
	BOOL fRawPacketFlag;	///< 指出是否输出原始的 RTP 包内容
	UINT fSequence;			///< 
	UINT fVideoFrameType;	///< 
	int  fVideoPayload;		///< 视频流的 Payload 类型, 用于检测指定的包是属于音频还是视频

	UINT fFragmentSequence;
};

/** 
 * RtspClient 代表一个 RTSP 客户端.
 * 
 * @author ChengZhen (anyou@msn.com)
 */
class RtspClient :  public IRtspClient, public RtspSocketListener
{
public:
	RtspClient(void);
	~RtspClient(void);

// Attributes -------------------------------------------------
public:
	MediaFormat& GetTrackFormat(UINT mediaType);
	RtspState GetRtspState();	///< 返回当前状态

	time_t GetLastActiveTime();
	time_t GetLastHeartBeatTime();
	int  GetTransport() { return fTransport; }
	BOOL IsTimeout();

	void SetTransport(int type);
	void SetListener(IRtspClientListener *notify);
	BOOL SetDataSource(LPCSTR url);
	void SetSelector(SocketSelector* selector);

// Operations -------------------------------------------------
public:
	void Close();
	void HandleResponse(BaseMessage* response);

	void ProcessPLAYResponse();

	void ProcessSETUPResponse( BaseMessage* response );

	void ProcessDESCRIBEResponse( BaseMessage* response );

	int  HandleRtpPacket(char* packet, size_t buflen);
	BOOL Open();

// Implements -------------------------------------------------	
private:
	BOOL Connect(int timeout = 5);
	void HandleAudioPacket(BYTE* data, int length, UINT flags, UINT timestamp); 
	void HandleVideoPacket(BYTE* data, int length, UINT flags, UINT timestamp); 
	int  ParseSDP(LPCSTR sdp);

	void OnRtspConnect(int errorCode);
	void OnRtspMessage(RtspMessage* message);
	void OnRtspPacket (char* packet, size_t buflen);
	void OnRtspSend   (int errorCode);
	void OnTimer();

	BOOL SendDESCRIBE();
	BOOL SendGET_PARAMETER();
	BOOL SendMessage(BaseMessage& message);
	BOOL SendOPTIONS();
	BOOL SendPLAY();
	BOOL SendSETUP();
	BOOL SendSUBSCRIBE();
	BOOL SendTEARDOWN();

	void SetLastActiveTime();
	void SetRtspState(RtspState state);

// Data Members -----------------------------------------------
private:
	IRtspClientListener* fListener;		///< 注册的事件侦听器
	SocketSelectorPtr fSelector;		///< 
	RtspVideoDecoder  fVideoDecoder;	///< 
	MediaFormat		fAudioFormat;		///< 音频 Track 属性
	MediaFormat		fVideoFormat;		///< 视频 Track 属性
	RtspState		fRtspState;			///< 当前 RTSP 客户端状态
	RtspSocketPtr	fRtspSocket;		///< 相关的 Socket 对象

	String  fLastRequest;			///< 最后一个发送的请求消息
	int		fLastRequestCSeq;		///< 最后一个发送的请求消息的 CSeq 值
	time_t  fLastActiveTime;		///< 最后活跃的时间
	time_t	fLastHeartBeatTime;		///< 最后心跳时间
	String	fSessionId;				///< RTSP Session ID

	int     fClientPort;			///< 
	int		fCSeq;					///< RTSP Method CSeq Number
	int		fSetupIndex;			///< 
	int		fTransport;				///< 当前 RTP 数据包传输类型.
	Uri		fUrl;					///< 当前 URL 地址
};

}

#endif // define _NS_VISION_RTSP_CLIENT_H
