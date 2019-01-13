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
#ifndef _NS_VISION_CORE_UTIL_UTILS_H_
#define _NS_VISION_CORE_UTIL_UTILS_H_

#include "core/vision_version.h"

#include "core/base/base.h"
#include "core/os/file.h"
#include "core/os/thread.h"

#include "core/utils/byte_buffer.h"
#include "core/utils/date_time.h"
#include "core/utils/debug.h"
#include "core/utils/log.h"
#include "core/utils/string_utils.h"

namespace core {

/**
 * 工具类集合.
 * 这个名称空间包含了一些常用的工具类.
 * @author ChengZhen (anyou@msn.com)
 */
namespace util {

/** 返回指定的数组占用的内存空间大小. */
#define ARRAY_SIZE(x) ( static_cast<int>(( sizeof(x) / sizeof(x[0]) )) )

/** Retrieves the number of milliseconds that have elapsed since the system was started */
INT64 GetSysTickCount(void);

/** 默认的 linux 信号处理方法. */
void  VisionDefaultSignals(sighandler_t handler);

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Bit APIs

#define BIT(bit)			(0x01 << (bit))
#define BIT_GET(x, bit)		(((x) >> (bit)) & 0x01)
#define BIT_SET(x, bit)		((x) |= BIT(bit))
#define BIT_CLR(x, bit)		((x) &= ~(BIT(bit)))

};
};

using namespace core::util;
using namespace core::os;

#endif // !defined(_NS_VISION_CORE_UTIL_UTILS_H_)
