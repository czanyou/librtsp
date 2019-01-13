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
#ifndef _NS_VISION_CORE_UTIL_BYTE_BUFFER_H_
#define _NS_VISION_CORE_UTIL_BYTE_BUFFER_H_

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// ByteBuffer class 

/** 
 * 字节缓冲区实现类. 
 * 0 <= Start <= End <= Capacity
 * Size == End - Start
 * FreeSize = Capacity - End
 *
 * @author ChengZhen (anyou@msn.com)
 */
class ByteBuffer : public Object 
{
public:
	ByteBuffer();
	ByteBuffer(UINT capacity);
	ByteBuffer(const BYTE* bytes, UINT len);
	virtual ~ByteBuffer();
	
// Attributes -------------------------------------------------
public:
	BYTE* GetBuffer();		///< 返回内部缓存区的起始地址
	BYTE* GetData();		///< 返回有效数据存放的起始地址
	BYTE* GetFreeBuffer();	///< 返回空闲空间的起始地址
	UINT  GetCapacity();	///< 返回内部缓存区总共的大小
	UINT  GetEnd();			///< 返回尾指针位置
	UINT  GetFreeSize();	///< 返回空闲空间的长度
	UINT  GetSize();		///< 返回有效数据的长度.
	UINT  GetStart();		///< 返回头指针位置
	BOOL  IsEmpty();		///< 指出这个缓存区是否为空
	void  Reset();			///< 清除所有的数据

// Operations -------------------------------------------------
public:
	void  Construct(const BYTE* bytes, UINT size);
	void  Compact();
	BOOL  Extend(UINT size);
	int   GetByte();
	int   GetBytes(BYTE* buf, UINT buflen);
	short GetInt16();
	int   GetInt32();
	int   PeekByte(int n = 0);
	BOOL  Put(const BYTE* bytes, UINT len);
	int   PutInt16(short value);
	int   PutInt32(int value);
	int   PutUint8(BYTE value);
	int   PutUint16(WORD value);
	int   PutUint32(UINT value);
	BOOL  ReadLine(String &line);
	void  Resize(UINT size);
	BOOL  Skip(UINT size);

// Data Members -----------------------------------------------
private:
	BYTE* fBuffer;		/** 缓存区. */
	UINT  fCapacity;	/** 容量.   */
	UINT  fStart;		/** 头指针. */
	UINT  fEnd;			/** 尾指针. */
};

/** ByteBuffer 智能指针类型. */
typedef SmartPtr<ByteBuffer> ByteBufferPtr;

};
};

#endif // !defined(_NS_VISION_CORE_UTIL_BYTE_BUFFER_H_)
