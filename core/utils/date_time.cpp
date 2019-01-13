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

#include "date_time.h"
#include "utils.h"

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Time class

Date::Date()
{
	fTime = 0;
	memset(&fLocalTm, 0, sizeof(fLocalTm));
}

/** 用给定的 Time 值设置此 Time 的当前时间值。*/
Date::Date( const Date& time )
{
	fTime = 0;
	memset(&fLocalTm, 0, sizeof(fLocalTm));
	SetTime(time.fTime);
}

/** 用给定的 time_t 值设置此 Time 的当前时间值。*/
Date::Date( time_t time )
{
	fTime = 0;
	memset(&fLocalTm, 0, sizeof(fLocalTm));
	SetTime(time);
}

/** 格式化此日期所表示的本地时间. 同 strftime. */
String Date::Format( LPCSTR fmt /*= NULL*/ )
{
	if (isempty(fmt)) {
		fmt = "%F %T";
	}

	char date[MAX_PATH + 1];
	memset(date, 0, sizeof(date));
	strftime(date, MAX_PATH, fmt, &fLocalTm);
	return date;
}

/** 格式化此日期所表示的 GMT 时间. 同 strftime.  */
String Date::FormatGmt( LPCSTR fmt /*= NULL*/ )
{
	if (isempty(fmt)) {
		fmt = "%F %T";
	}

	struct tm tmval;
	GetGmtTm(&tmval);

	char date[MAX_PATH + 1];
	memset(date, 0, sizeof(date));
	strftime(date, MAX_PATH, fmt, &tmval);
	return date;
}

/** 格式化时间. */
String Date::FormatTime(UINT time)
{
	UINT ONE_HOUR = 3600;
	UINT ONE_MINUTE = 60;
	UINT ONE_DAY  = ONE_HOUR * 24;
	char buffer[MAX_PATH + 1];

	int value  = time;
	int days    = value / ONE_DAY;
	value       = value % ONE_DAY;

	int hours   = value / ONE_HOUR;	
	value		= value % ONE_HOUR;

	int minutes = value / ONE_MINUTE;
	int seconds = value % ONE_MINUTE;

	if (time >= ONE_DAY ) {
		snprintf(buffer, MAX_PATH, "%d,%02d:%02d:%02d", 
			days, hours, minutes, minutes);

	} else if (time >= 3600) {
		snprintf(buffer, MAX_PATH, "%d:%02d:%02d", 
			hours, minutes, seconds);

	} else {
		snprintf(buffer, MAX_PATH, "%02d:%02d", minutes, seconds);
	}
	return buffer;
}

/** 返回当前系统时间. */
time_t Date::GetCurrentTime()
{
	return time(NULL);
}

int Date::GetDay()
{
	return fLocalTm.tm_mday;
}

int Date::GetDayOfWeek()
{
	return fLocalTm.tm_wday;
}

/** 返回 GMT 时间. */
tm_t* Date::GetGmtTm(tm_t* ptm) const
{
	if (ptm) {
		memset(ptm, 0, sizeof(tm_t));
		return gmtime_r(&fTime, ptm);
	}
	return ptm;
}

int Date::GetHour()
{
	return fLocalTm.tm_hour;
}

/** 返回本地时间. */
tm_t* Date::GetLocalTm(tm_t* ptm) const
{
	if (ptm) {
		memset(ptm, 0, sizeof(tm_t));
		return localtime_r(&fTime, ptm); // localtime 是非线程安全的
	}
	return ptm;
}

int Date::GetMinute()
{
	return fLocalTm.tm_min;
}

int Date::GetMonth()
{
	return fLocalTm.tm_mon + 1;
}

/** Convert month to the month number. Return -1 on error, or month number */
int Date::GetMonthByName(LPCSTR month)
{
	if (isempty(month)) {
		return -1;
	}

	static LPCSTR monthNames[] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL
	};

	for (UINT i = 0; i < 12; i++) {
		if (isequal(month, monthNames[i])) {
			return i;
		}
	}

	return -1;
}

int Date::GetSecond()
{
	return fLocalTm.tm_sec;
}

/** 
 * 返回此日期所表示的日期在当天 0 点开始以来已经过去的秒数 （用本地时区进行解
 * 释）。返回值在 0 和 24 * 3600 之间。 
 */
int Date::GetTimeOfDay()
{
	return fLocalTm.tm_sec + (fLocalTm.tm_min * 60) + (fLocalTm.tm_hour * 3600);
}

/** 返回此日期所表示的时间自 1970 年 1 月 1 日 00:00:00 GMT 以来的秒数 */
time_t Date::GetTime() const
{
	return fTime;
}

/** 返回时区偏移时间. */
int Date::GetTimezoneOffset()
{
	struct tm gmtTime;
	memset(&gmtTime, 0, sizeof(gmtTime));
	GetGmtTm(&gmtTime);
	return 0;
}

int Date::GetYear()
{
	return fLocalTm.tm_year + 1900;
}

/** 创建一个 time_t 值. */
time_t Date::MakeTime (UINT year, UINT mon, UINT day, UINT hour, UINT min, UINT sec)
{
	if (0 >= (int) (mon -= 2)) {    /**//* 1..12 -> 11,12,1..10 */
		mon += 12;      /**//* Puts Feb last since it has leap day */
		year -= 1;
	}

	return (((
		(time_t) (year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day) +
		year * 365 - 719499
		) * 24 + hour /**//* now have hours */
		) * 60 + min /**//* now have minutes */
		) * 60 + sec; /**//* finally seconds */
}

/** 重载 = 操作符. */
Date& Date::operator=( time_t time )
{
	SetTime(time); 
	return *this;
}

/** 重载 = 操作符. */
Date& Date::operator=( const Date& time )
{
	SetTime(time.GetTime()); 
	return *this;
}

/** 解析 yyyy-mm-dd HH:MM:SS 格式的日期字符串. */
time_t Date::Parse( LPCSTR dateString )
{
	if (isempty(dateString)) {
		return 0;
	}

	BOOL isGmtTime = FALSE;

	struct tm lt;
	memset(&lt, 0, sizeof(lt));
	int value = 0;
	Lex lex(StringUtils::Trim(dateString));

	do {
		// 年份
		if (!lex.NextNumber(value) || !lex.LookToken("-")) {
			return 0;

		} else if (value < 1970 || value > 9999) {
			return 0;
		}
		lt.tm_year = value - 1900;

		// 月份
		if (!lex.NextNumber(value) || !lex.LookToken("-")) {
			return 0;

		} else if (value < 1 || value > 12) {
			return 0;
		}
		lt.tm_mon = value - 1;

		// 日期
		if (!lex.NextNumber(value)) {
			return 0;

		} else if (value < 1 || value > 31) {
			return 0;
		}
		lt.tm_mday = value;

		if (!lex.LookToken(" ") && !lex.LookToken("T")) {
			break;
		}

		// 小时
		if (!lex.NextNumber(value)) {
			break;

		} else if (value < 0 || value > 23) {
			return 0;
		}
		lt.tm_hour	= value;

		// 分钟
		if (!lex.LookToken(":") || !lex.NextNumber(value)) {
			break;

		} else if (value < 0 || value > 59) {
			return 0;
		}
		lt.tm_min	= value;

		// 秒钟
		if (!lex.LookToken(":") || !lex.NextNumber(value)) {
			break;

		} else if (value < 0 || value > 59) {
			return 0;
		}
		lt.tm_sec	= value;

		if (lex.LookToken("Z")) {
			isGmtTime = TRUE;
		}

	} while (FALSE);

	if (isGmtTime) {
		fTime = MakeTime(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday,
			lt.tm_hour, lt.tm_min, lt.tm_sec);

	} else {
		SetTime(mktime(&lt));
	}

	return fTime;
}

/** 解析 RFC 定义格式的日期字符串. */
time_t Date::ParseRfcDate(LPCSTR dateString)
{
	String date = dateString;
	LPCSTR monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	String::size_type pos = date.IndexOf(',');
	if (pos == String::npos) {
		return 0;
	}

	date = StringUtils::Trim(date.SubString(pos + 1).c_str());

	struct tm gmt_tm;
	memset(&gmt_tm, 0, sizeof(gmt_tm));

	// date
	int state = 0;
	pos = 0;
	while (pos != String::npos ) {
		String value = date.Tokenize(" ", pos);
		if (state == 0) {
			gmt_tm.tm_mday = atoi(value.c_str());

		} else if (state == 1) {
			for (int i = 0; i < 12; i++) {
				if (value == monthNames[i]) {
					gmt_tm.tm_mon = i;
					break;
				}
			}

		} else if (state == 2) {
			gmt_tm.tm_year = atoi(value.c_str()) - 1900;

		} else if (state == 3) {
			date = value;
			break;

		} else {
			break;
		}
		state++;
	}

	// time
	state = 0;
	pos = 0;
	while (pos != String::npos ) {
		String value = date.Tokenize(":", pos);
		if (state == 0) {
			gmt_tm.tm_hour = atoi(value.c_str());

		} else if (state == 1) {
			gmt_tm.tm_min = atoi(value.c_str());

		} else if (state == 2) {
			gmt_tm.tm_sec = atoi(value.c_str());
			break;

		} else {
			break;
		}
		state++;
	}

	return MakeTime(gmt_tm.tm_year + 1900, gmt_tm.tm_mon + 1, gmt_tm.tm_mday,
		gmt_tm.tm_hour, gmt_tm.tm_min, gmt_tm.tm_sec);
}


time_t Date::ParseHttpDate(LPCSTR date)
{
	tm tmval;
	char	mon[32];
	int		sec, min, hour, mday, month, year;

	memset(&tmval, 0, sizeof(tmval));
	sec = min = hour = mday = month = year = 0;

	if (((sscanf(date, "%d/%3s/%d %d:%d:%d",		
		&mday, mon, &year, &hour, &min, &sec) == 6) ||
		(sscanf(date, "%d %3s %d %d:%d:%d",		
		&mday, mon, &year, &hour, &min, &sec) == 6) ||
		(sscanf(date, "%*3s, %d %3s %d %d:%d:%d",  
		&mday, mon, &year, &hour, &min, &sec) == 6) ||
		(sscanf(date, "%d-%3s-%d %d:%d:%d",		
		&mday, mon, &year, &hour, &min, &sec) == 6)) &&
		(month = GetMonthByName(mon)) != -1) {
			tmval.tm_mday	= mday;
			tmval.tm_mon	= month;
			tmval.tm_year	= year;
			tmval.tm_hour	= hour;
			tmval.tm_min	= min;
			tmval.tm_sec	= sec;
	}

	if (tmval.tm_year > 1900) {
		tmval.tm_year -= 1900;

	} else if (tmval.tm_year < 70) {
		tmval.tm_year += 100;
	}

	time_t currentTime = time(NULL); 

	/* Set Daylight Saving Time field */
	tm tmtmp;
	localtime_r(&currentTime, &tmtmp);
	tmval.tm_isdst = tmtmp.tm_isdst;

	return (mktime(&tmval));
}

/** 解析 HH:MM:SS 表示的时间. */
long Date::ParseTime(LPCSTR timeString)
{
	if (isempty(timeString)) {
		return -1;
	}

	int hour	= 0;
	int minute	= 0;
	int second	= 0;

	Lex lex(timeString);
	do {
		if (!lex.NextNumber(hour)) {
			break;
		}

		if (!lex.LookToken(":") || !lex.NextNumber(minute)) {
			break;
		}

		if (!lex.LookToken(":") || !lex.NextNumber(second)) {
			break;
		}

	} while (FALSE);

	return hour * 3600 + minute * 60 + second;	
}

/** 设置此 Time 为指定日期表示的时间. */
int Date::SetDate( int year, int month, int day )
{
	fLocalTm.tm_year	= year - 1900;
	fLocalTm.tm_mon		= month - 1;
	fLocalTm.tm_mday	= day;
	fTime = mktime(&fLocalTm);
	return 0;
}

/** 设置此 Time 为指定日期表示的时间. */
int Date::SetDateTime( int year, int month, int day, int hour, int minute, int second )
{
	fLocalTm.tm_year	= year - 1900;
	fLocalTm.tm_mon		= month - 1;
	fLocalTm.tm_mday	= day;
	fLocalTm.tm_hour	= hour;
	fLocalTm.tm_min		= minute;
	fLocalTm.tm_sec		= second;
	fTime = mktime(&fLocalTm);
	return 0;
}

/** 
 * 用给定的 time_t 值设置此 Time 的当前时间值。
 * @param time 自 1970 年 1 月 1 日 00:00:00 GMT 以来的秒数
 */
int Date::SetTime( time_t time )
{
	fTime = time;
	memset(&fLocalTm, 0, sizeof(fLocalTm));
	localtime_r(&fTime, &fLocalTm);
	return 0;
}

/** 设置此 Time 为指定日期表示的时间. */
int Date::SetTime( int hour, int minute, int second )
{
	fLocalTm.tm_hour	= hour;
	fLocalTm.tm_min		= minute;
	fLocalTm.tm_sec		= second;
	fTime = mktime(&fLocalTm);
	return 0;
}

/** 生成当前日期和时间字符串. */
String Date::ToRfcString(time_t now)
{
	char buffer[MAX_PATH + 1];
	memset(buffer, 0, sizeof(buffer));

	tm tmval;
	memset(&tmval, 0, sizeof(tmval));
	localtime_r(&now, &tmval);
	strftime(buffer, MAX_PATH, "%a, %d %b %Y %H:%M:%S %z", &tmval);
	return buffer;
}

String Date::ToLocalString(time_t now)
{
	char buffer[MAX_PATH + 1];
	memset(buffer, 0, sizeof(buffer));

	tm tmval;
	memset(&tmval, 0, sizeof(tmval));
	localtime_r(&now, &tmval);
	strftime(buffer, 100, "%F %T %Z%z", &tmval);
	return buffer;
}

};
};
