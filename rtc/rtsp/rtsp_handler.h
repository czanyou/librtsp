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
#ifndef _NS_VISION_RTSP_HANDLER
#define _NS_VISION_RTSP_HANDLER

namespace rtsp {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspSnapshotHandler class

class RtspSnapshotHandler : public IMediaSink
{
public:
	int  OnMediaDistribute(int channel, IMediaSample* mediaSample);
	int  OnMediaSourceChange(UINT flags);

public:
	void Close();
	void SetSnapshot(RtspMessage* snapshotResponse);

public:
	RtspMessagePtr fSnapshotResponse;	///< 

};

typedef SmartPtr<RtspSnapshotHandler> RtspSnapshotHandlerPtr;

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// RtspHandler class

/**
 * RTSP 消息处理器
 *
 * @author ChengZhen (anyou@msn.com)
 */
class RtspHandler
{
public:
	RtspHandler();
	typedef LinkedList<RtspMessagePtr> RtspMessageList;

// Attributes -------------------------------------------------
public:
	String GetAllowedMethods();
	void   SetRtspContext(IRtspContext* context);

// Operations -------------------------------------------------
public:
	void Close();
	void HandleRequest		 (RtspMessage* request);
	void HandleResponse		 (RtspMessage* response);
	void HandleLiveRequest	 (RtspMessage* request, RtspMessage* response);

// Implementation ---------------------------------------------
protected:
	void HandleUnautherticate(RtspMessage* request, RtspMessage* response);
	void HandleNotImplement  (RtspMessage* request, RtspMessage* response);
	void HandleGET_PARAMETER (RtspMessage* request, RtspMessage* response);
	void HandleSET_PARAMETER (RtspMessage* request, RtspMessage* response);
	void HandleOPTIONS		 (RtspMessage* request, RtspMessage* response);
	void HandleDESCRIBE		 (RtspMessage* request, RtspMessage* response);
	void HandleSETUP		 (RtspMessage* request, RtspMessage* response);
	void HandlePLAY			 (RtspMessage* request, RtspMessage* response);
	void HandleTEARDOWN		 (RtspMessage* request, RtspMessage* response);
	void HandlePAUSE		 (RtspMessage* request, RtspMessage* response);
	void HandleGET			 (RtspMessage* request, RtspMessage* response);

// Data Members -----------------------------------------------
private:
	RtspSnapshotHandlerPtr fSnapshotHandler;
	RtspMessageList fRequestList;	///< 
	IRtspContext* fRtspContext;		///< 这个连接所属的 RtspServer 对象.
	UINT fLastMediaSourceId;		///< 
	int	 fSetupCounter;				///< 这个连接当前接收到的 SETUP 请求数目

};

}

#endif // _NS_VISION_RTSP_HANDLER

