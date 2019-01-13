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

#ifndef _NS_VISION_RTSP_SESSION_H
#define _NS_VISION_RTSP_SESSION_H


#include "rtsp_session_track.h"
#include "rtsp_common.h"

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSession class

/** 
 * 代表一个 RTSP 会话. 
 * 这个 Session 主要实现把分发到的音视频帧通过 RTP 协议发送给客户端.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspSession : public IMediaSink
{
// Construction/Destruction -----------------------------------
public:	
	RtspSession(LPCSTR sessionId = NULL);
	virtual ~RtspSession();
	static int gSessionCount; ///< 实例数

// Attributes -------------------------------------------------
public:
	const RtspTransport& GetTransport(int mediaType) const;

	IRtspConnectionPtr GetConnection();
	RtspState GetSessionState();

	double GetDuration();
	int    GetBitrate(int mediaType);
	int    GetFrameRate(int mediaType);
	String GetLocalAddress();
	String GetPublicAddress();
	String GetRtpInfo(LPCSTR url);
	String GetSessionId();
	int    GetSourceChangeFlags();
	UINT   GetSSRC();

	BOOL   IsMulticast();
	BOOL   IsPlaying();
	BOOL   IsTcpTransport();
	BOOL   IsLiveStreaming();

	void   SetRange(double start, double end);
	void   SetScale(float scale);
	void   SetConnection(IRtspConnection* connection);
	void   SetMediaSource(IMediaSource* source);
	void   SetSessionState(RtspState state);
	void   SetTransport(RtspTransport& transport, BOOL isVideoTransport = TRUE);

// Operations -------------------------------------------------
public:	
	IMediaSamplePtr GetNextMediaSample();
	IMediaSamplePtr OnRtpNextSample(UINT type, INT64& timestamp);

	UINT   GetRtpTime(INT64 ts, int frequency);

	void   Clear();
	void   Close();

	String OnDump(UINT indent);
	int    OnMediaDistribute(int channel, IMediaSample* mediaSample);
	int    OnMediaSourceChange(UINT flags);

	void   OnRtpMediaStream(UINT mediaType);

	void   OnTimer();
	void   OnSend();
	void   RenewStream();

	int    Start(IRtspContext* context);
	int    StateMachine(LPCSTR method);

// Implementation ---------------------------------------------
protected:
	String CreateSessionId();

// Data Members -----------------------------------------------
private:
	IRtspConnectionPtr	fConnection;	///< Point to the RTSP Connection.
	IRtspContext*		fRtspContext;	///< 
	IMediaSourcePtr		fMediaSource;	///< 这个会话相关的流数据源
	RtspSessionTrack	fAudioTrack;	///< 
	RtspSessionTrack	fVideoTrack;	///< 

	int			fChangeFlags;			///< 
	INT64		fFirstTimestamp;		///< The timestamp of the first frame.
	String      fLocalAddress;			///< 这个会话的本地 IP 地址
	Mutex		fMutex;					///< Mutex object	
	RtspState	fSessionState;				///< The state of the RTSP session.
	String		fSessionId;				///< The Session ID of the RTSP session.
	UINT		fSsrc;					///< The SSRC of this session
	time_t		fStartTime;				///< 会话开始的时间
};

/** RtspSession 智能指针类型. */
typedef SmartPtr<RtspSession> RtspSessionPtr;

}; // namespace rtsp

#endif // !defined(_NS_VISION_RTSP_SESSION_H)
