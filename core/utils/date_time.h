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
#ifndef _NS_VISION_CORE_UTIL_TIME_H_
#define _NS_VISION_CORE_UTIL_TIME_H_

#include "core/base/base.h"

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Time class 

typedef struct tm tm_t;

/** 
 * 表示特定的瞬间，精确到秒。
 *
 * @author ChengZhen (anyou@msn.com)
 */
class Date
{
public:
	Date();
	Date(const Date& time);
	Date(time_t time);

// Attributes -------------------------------------------------
public:
	time_t GetTime() const;

	int  GetDay();			///< 返回此 Time 表示的月份中的某一天
	int  GetDayOfWeek();	///< 返回此 Time 表示的一周中的某一天
	int  GetHour();			///< 返回此 Time 表示的小时
	int  GetMinute();		///< 返回此 Time 表示的分钟
	int  GetMonth();		///< 返回此 Time 表示的月份, 1 - 12
	int  GetSecond();		///< 返回此 Time 表示的秒钟
	int  GetTimeOfDay();	///< 
	int  GetTimezoneOffset();
	int  GetYear();			///< 返回此 Time 表示的年份

	int  SetDate(int year, int month, int day);
	int  SetDateTime(int year, int month, int day, int hour, int minute, int second);
	int  SetTime(int hour, int minute, int second);
	int  SetTime(time_t time);

// Operations -------------------------------------------------
public:
	time_t Parse(LPCSTR dateString);
	String Format(LPCSTR fmt = NULL);
	String FormatGmt(LPCSTR fmt = NULL);

	tm_t* GetLocalTm(tm_t* ptm) const;
	tm_t* GetGmtTm(tm_t* ptm) const;

	Date& operator =(const Date& time);
	Date& operator =(time_t time);

// Static Methods ---------------------------------------------
public:
	static time_t GetCurrentTime();
	static String FormatTime(UINT time);
	static int	  GetMonthByName(LPCSTR month);
	static time_t MakeTime (UINT year, UINT mon, UINT day, UINT hour, UINT min, UINT sec);
	static time_t ParseHttpDate(LPCSTR dateString);
	static time_t ParseRfcDate(LPCSTR dateString);
	static long   ParseTime(LPCSTR timeString);
	static String ToLocalString(time_t now);
	static String ToRfcString(time_t now);

// Data members -----------------------------------------------
protected:
	time_t fTime;	///< 从 1970 年 1 月 1 日 00:00:00 GMT 以来经过的秒数
	struct tm fLocalTm;	///< 当前日期对应的本地时间
};

};
};

#endif // !defined(_NS_VISION_CORE_UTIL_TIME_H_)
