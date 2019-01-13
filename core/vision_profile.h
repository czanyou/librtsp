/***
 * The content of this file or document is CONFIDENTIAL and PROPRIETARY
 * to ChengZhen(Anyou).  It is subject to the terms of a
 * License Agreement between Licensee and ChengZhen(Anyou).
 * restricting among other things, the use, reproduction, distribution
 * and transfer.  Each of the embodiments, including this information and
 * any derivative work shall retain this copyright notice.
 *
 * Copyright (c) 2013-2014 ChengZhen(Anyou). All Rights Reserved.
 *
 */
#ifndef _NS_VISION_INC_PROFILE_H
#define _NS_VISION_INC_PROFILE_H

namespace core {
namespace vision {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// IPreferences class

/**
 * 用于读取和修改配置文件. 所有的参数都以 "名称=值" 对的形式保存.
 *
 * @author ChengZhen (anyou@msn.com)
 */
class IPreferences : public Object
{
public:
	/** 
	 * 立即保存对 Profile 的修改. 
	 * Commit your preferences changes back from this Editor to the IPreferences
	 * object it is editing.
	 */
	virtual int Commit() = 0;

	virtual int Reload() = 0;

	virtual BOOL IsContentChanged() = 0;

	/**
	 * 返回指定的名称的参数的整数值. 
	 * @param name 参数的名称
	 * @param defaultValue 如果指定的参数不存在的话则返回这个默认值.
	 */
	virtual int GetInt(LPCSTR name, int defaultValue = 0) = 0;

	/** 返回当前 Profile 参数文件的最后修改时间. */
	virtual time_t GetModifiedTime() = 0;

	/** 
	 * 返回指定的名称的参数的值. 
	 * @param name 参数的名称
  	 * @param defaultValue 如果指定的参数不存在的话则返回这个默认值.
	 */
	virtual String GetString(LPCSTR name, LPCSTR defaultValue = "") = 0;

	/** 
	 * 设置指定的名称的参数的值. 
	 * @param name 参数的名称.
	 * @param value 参数的值.
	 */
	virtual int PutString(LPCSTR name, LPCSTR value) = 0;

};

typedef SmartPtr<IPreferences> IPreferencesPtr;

}; // 
}; // 

using namespace core::vision;

#endif // !defined(_NS_VISION_INC_PROFILE_H)
