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

#include "byte_buffer.h"

namespace core {
namespace util {

#define BYTE_BUFFER_MAX_SIZE 16 * 1024 * 1024

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// ByteBuffer class

/** 构建一个新的 ByteBuffer. */
ByteBuffer::ByteBuffer()
{
	Construct(NULL, 0);
}

/** 
 * 构建一个新的 ByteBuffer. 
 * @param capacity 缓存区初始容量, 单位为字节.
 */
ByteBuffer::ByteBuffer(UINT capacity)
{
	Construct(NULL, 0);
	Resize(capacity);
}

ByteBuffer::ByteBuffer( const BYTE* bytes, UINT len )
{
	Construct(bytes, len);
}

/** 析构方法. */
ByteBuffer::~ByteBuffer()
{
	if (fBuffer != NULL) {
		delete[] fBuffer;
		fBuffer = NULL;
	}
}

/** 压缩这个缓存区, 把有效数据移动缓存区的最左边的开始位置. */
void ByteBuffer::Compact()
{
	if (fStart > 0) {
		int size = GetSize();
		memmove(fBuffer, fBuffer + fStart, size);
		fStart = 0;
		fEnd = size;
	}
}

void ByteBuffer::Construct( const BYTE* bytes, UINT size )
{
	fBuffer		= NULL;
	fCapacity	= 0;
	fStart		= 0;
	fEnd		= 0;

	if (bytes && size > 0) {
		Resize(size + 16);
		Put(bytes, size);
	}
}

/** 
 * 扩展有效数据区的大小. 
 * @param length 要扩展的数据的长度
 * @return 如果成功返回 TRUE, 如果缓存区没有足够的空间则返回 FALSE, 并且
 * 不会移动指针的位置.
 */
BOOL ByteBuffer::Extend( UINT length )
{
	UINT pos = fEnd + length;
	if (pos > fCapacity) {
		return FALSE;
	}

	fEnd = pos;
	fBuffer[fEnd] = 0;
	return TRUE;
}

/** 读取并返回下一个字节, 并移动开始指针的位置, 如果缓存区为空则返回 -1. */
int ByteBuffer::GetByte()
{
	if (fStart >= fEnd) {
		return -1;
	}
	return fBuffer[fStart++];
}

/** 
 * 从缓存区开始位置读取并返回指定个字节的数据. 
 * @param data 输出参数, 存放读取的数据的缓存区的地址.
 * @param length 要读取的数据的长度.
 * @return 返回实际复制的数据的长度, 如果没有足够长度的数据可读则返回 -1,
 *  并且不会复制任何数据.
 */
int ByteBuffer::GetBytes( BYTE* data, UINT length )
{
	if ((length == 0) || (length > GetSize())) {
		return -1;
	}

	memcpy(data, fBuffer + fStart, length);
	fStart += length;
	return length;
}

BYTE* ByteBuffer::GetBuffer()
{
	return fBuffer;
}

UINT ByteBuffer::GetCapacity()
{
	return fCapacity;
}

BYTE* ByteBuffer::GetData()
{
	return fBuffer ? (fBuffer + fStart) : NULL;
}

UINT ByteBuffer::GetEnd()
{
	return fEnd;
}

BYTE* ByteBuffer::GetFreeBuffer()
{
	return fBuffer ? (fBuffer + fEnd) : NULL;
}

UINT ByteBuffer::GetFreeSize()
{
	return fCapacity - fEnd;
}

short ByteBuffer::GetInt16()
{
	BYTE buffer[2];
	int ret = GetBytes(buffer, 2);
	if (ret != 2) {
		return -1;
	}

	return (short)((buffer[1] << 8) | buffer[0]);
}

int ByteBuffer::GetInt32()
{
	BYTE buffer[4];
	int ret = GetBytes(buffer, 4);
	if (ret != 4) {
		return -1;
	}

	return (int)((buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0]);
}

UINT ByteBuffer::GetSize()
{
	return fEnd - fStart;
}

UINT ByteBuffer::GetStart()
{
	return fStart;
}

BOOL ByteBuffer::IsEmpty()
{
	return fStart == fEnd;
}

/** 返回缓存区开始位置第 N 个字节的值, 但不改变指针的位置. */
int ByteBuffer::PeekByte( int n )
{
	UINT pos = fStart + n;
	if (pos >= fStart || pos < fEnd) {
		return fBuffer[pos];
	}
	return -1;
}

/** 
 * 复制指定长度的数据到缓存区中. 
 * @param data 要复制到缓存区中的数据.
 * @param length 要复制的数据的长度.
 * @return 如果所成功返回 TRUE, 如果没有参数错误或者没有足够的空闲空间则
 * 返回 FALSE, 并且不会复制任何数据.
 */
BOOL ByteBuffer::Put(const BYTE* data, UINT length) 
{
	if (data == NULL || length == 0) {
		return FALSE;
	}

	if (GetFreeSize() <= length) {
		Compact();
	}

	UINT freeSize = GetFreeSize();
	if (freeSize >= length) {
		memcpy(GetFreeBuffer(), data, length);
		Extend(length);
		return TRUE;
	}

	return FALSE;
}

int ByteBuffer::PutInt16( short value )
{
	BYTE data[2];
	data[1] = (BYTE)(value >> 8); 
	data[0] = (BYTE)(value & 0xFF); 
	return Put(data, 2);
}

int ByteBuffer::PutInt32( int value )
{
	BYTE data[4];
	data[3] = (BYTE)(value >> 24);
	data[2] = (BYTE)(value >> 16);
	data[1] = (BYTE)(value >> 8);
	data[0] = (BYTE)(value & 0xFF);		 
	return Put(data, 4);
}

int ByteBuffer::PutUint8( BYTE value )
{
	BYTE data[2];
	data[0] = value; 
	return Put(data, 1);
}

int ByteBuffer::PutUint16( WORD value )
{
	BYTE data[2];
	data[1] = (BYTE)(value >> 8); 
	data[0] = (BYTE)(value & 0xFF); 
	return Put(data, 2);
}

int ByteBuffer::PutUint32( UINT value )
{
	BYTE data[4];
	data[3] = (BYTE)(value >> 24);
	data[2] = (BYTE)(value >> 16);
	data[1] = (BYTE)(value >> 8);
	data[0] = (BYTE)(value & 0xFF);
	return Put(data, 4);
}

/** 返回下一行以 \n 结束的字符串. 不存在则返回 FALSE. */
BOOL ByteBuffer::ReadLine(String &line)
{
	for (UINT i = fStart; i < fEnd; i++) {
		if (fBuffer[i] == '\n') {
			UINT length = i - fStart + 1;
			line = String((char*)(fBuffer + fStart), length);
			Skip(length);
			return TRUE;
		}
	}
	return FALSE;
}

void ByteBuffer::Reset()
{
	fStart = 0; fEnd = 0;
}

/** 
 * 重新分配缓存区的大小. 
 * @param capacity 缓存区初始容量, 单位为字节.
 */
void ByteBuffer::Resize(UINT newCapacity)
{
	if (newCapacity >= BYTE_BUFFER_MAX_SIZE) {
		LOG_E("Capacity too large (%u)\r\n", newCapacity);
		return;

	} else if (newCapacity <= fCapacity) {
		return;
	}

	BYTE* newBuffer	= new BYTE[newCapacity + 10];
	UINT newSize = 0;
	if (fBuffer) {
		if (fEnd > fStart) {
			newSize = fEnd - fStart;
			memcpy(newBuffer, fBuffer + fStart, newSize);
		}

		delete[] fBuffer;
		fBuffer = NULL;
	}

	fStart		= 0;
	fEnd		= newSize;
	fBuffer		= newBuffer;
	fCapacity	= newCapacity;
}

/** 
 * 跳过指定个字节的数据, (移动头指针). 
 * @param length 要跳过的数据的长度
 * @return 如果成功返回 TRUE, 如果缓存区没有足够的数据则返回 FALSE, 
 * 并且不会移动指针的位置.
 */
BOOL ByteBuffer::Skip( UINT length )
{
	if (length > GetSize()) {
		return FALSE;
	}

	fStart += length;
	if (fStart == fEnd) {
		fStart = 0;
		fEnd = 0;
	}
	return TRUE;
}

};
};
