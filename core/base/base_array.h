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
#ifndef _NS_VISION_CORE_BASE_ARRAY_H
#define _NS_VISION_CORE_BASE_ARRAY_H

#include "base_types.h"

/** 工具集合. */
namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// ArrayList class

/** 
 * 一个简单的数组模板类, 大小可变数组的实现.
 * 提供一些方法，用于创建、处理、搜索数组以及对数组进行排序等.
 *
 * @author ChengZhen (anyou@msn.com)
 */
template <typename TYPE> 
class ArrayList
{
public:
	ArrayList();
	ArrayList(UINT capacity);
	ArrayList(TYPE* data, UINT size);
	ArrayList(const ArrayList& other);
	virtual ~ArrayList();

// Attributes -------------------------------------------------
public:
	UINT  GetCapacity() const { return fCapacity; }	///< 返回这个数组当前可包含的元素数。
	TYPE* GetData()     const { return fValues;	}	///< 返回这个数组管理的元素的原始数组的指针
	UINT  GetLength()   const { return fLength;	}	///< 返回这个数组实际包含的元素的个数

	BOOL IsEmpty();

// Operations -------------------------------------------------
public:
	const TYPE& Get(UINT index) const;
	const TYPE& Get(UINT index, const TYPE& defaultValue) const;

	TYPE& Get(UINT index);
	TYPE& Get(UINT index, TYPE& defaultValue);

	const TYPE& operator[] (UINT index) const { return Get(index); }
	TYPE& operator[] (UINT index) { return Get(index); }

	ArrayList& operator = (const ArrayList& other);

public:
	void Add(const TYPE& item);
	void Clear();
	BOOL Contains(const TYPE& item);
	int  IndexOf(const TYPE& item);
	int  LastIndexOf(const TYPE& item);
	void Remove(UINT index);
	void RemoveLast();
	void Resize(UINT size, TYPE fill = TYPE());
	void Set(UINT index, const TYPE& item);
	void TrimSize();

public:
	static int CopyArray(TYPE* src, TYPE* desc, UINT count);

// Implementation ---------------------------------------------
protected:
	void ResizeIfNecessary();
	BOOL IsValidIndex(const UINT index) const;

// Data Members -----------------------------------------------
private:
	UINT  fCapacity;		///< 最大元素数量
	BOOL  fIsFixedSize;		///< 指出数组大小是否是固定的
	BOOL  fIsReadOnly;		///< 指出数组是否是只读的
	UINT  fLength;			///< 有效元素数量
	TYPE  fNullValue;		///< 
	TYPE* fValues;			///< 这个数组的元素列表
};

///////////////////////////////////////////////////////////////////////////////
// 


/** 初始化 ArrayList 类的新实例，该实例为空并且具有默认初始容量。. */
template <class TYPE>
ArrayList<TYPE>::ArrayList()
{
	fCapacity		= 0;
	fIsFixedSize	= FALSE;
	fIsReadOnly		= FALSE;
	fLength			= 0;
	fNullValue		= TYPE();
	fValues			= NULL;
}

/** 默认复制构建方法. */
template <class TYPE>
ArrayList<TYPE>::ArrayList( const ArrayList& other )
{
	fValues		= NULL;
	fLength		= 0;
	fCapacity	= 0;
	if (this == &other) {
		return;
	}

	fCapacity		= other.fCapacity;
	fIsFixedSize	= FALSE;
	fIsReadOnly		= FALSE;
	fLength			= other.fLength;
	fNullValue		= TYPE();
	fValues			= new TYPE[fLength];

	CopyArray(other.fValues, fValues, fLength);
}

template <class TYPE> ArrayList<TYPE>& 
ArrayList<TYPE>::operator =( const ArrayList& other )
{
	if (this == &other) {
		return *this;
	}

	Clear();

	fCapacity		= other.fCapacity;
	fIsFixedSize	= FALSE;
	fIsReadOnly		= FALSE;
	fLength			= other.fLength;
	fNullValue		= TYPE();
	fValues			= new TYPE[other.fLength];

	CopyArray(other.fValues, fValues, fLength);

	return *this;
}

/**
 * 构建方法, 通过封装指定的原始数组来构建一个新数组.
 * @param data 原始数组
 * @param size 原始数组的长度(元素的个数).
 */
template <class TYPE>
ArrayList<TYPE>::ArrayList( TYPE* data, UINT size )
{
	fCapacity		= 0;
	fIsFixedSize	= FALSE;
	fIsReadOnly		= FALSE;
	fLength			= 0;
	fNullValue		= TYPE();
	fValues			= NULL;

	if (data) {
		fCapacity	= size;
		fLength		= size;
		fValues		= new TYPE[fLength];
		CopyArray(data, fValues, fLength);
	}
}

/**
 * 初始化 ArrayList 类的新实例，该实例为空并且具有指定的初始容量.
 * @param capacity 默认分配的容量, 如果预先知道并分配数组大概的容量可以提高性能.
 */
template <class TYPE>
ArrayList<TYPE>::ArrayList( UINT capacity )
{
	fCapacity		= 0;
	fIsFixedSize	= FALSE;
	fIsReadOnly		= FALSE;
	fLength			= 0;
	fNullValue		= TYPE();
	fValues			= NULL;

	if (capacity > 0) {
		fCapacity = capacity;
		fValues = new TYPE[capacity];

		for (UINT i = 0; i < capacity; i++) {
			fValues[i] = TYPE();
		}
	}
}

template <class TYPE>
ArrayList<TYPE>::~ArrayList()
{
	if (fValues) {
		delete[] fValues;
		fValues = NULL;
	}
}

/**
 * 添加一个元素到数组的尾部.
 * @param item 要添加的元素.
 */
template <class TYPE>
void ArrayList<TYPE>::Add(const TYPE& item) 
{
	if (fCapacity <= fLength) {
		ResizeIfNecessary();
	}
	
	if (fValues && (fCapacity > fLength)) {
		fValues[fLength++] = item;
	}
}

/** 移除此数组中的所有元素. */
template <class TYPE>
void ArrayList<TYPE>::Clear() 
{
	if (fValues) {
		delete[] fValues;
		fValues = NULL;
	}

	fCapacity	= 0;
	fLength		= 0;
	fNullValue	= TYPE();
}

/** 指出这个数据是否包含指定的元素. */
template <typename TYPE>
BOOL ArrayList<TYPE>::Contains( const TYPE& item )
{
	for (UINT i = 0; i < fLength; i++) {
		if (item == fValues[i]) {
			return TRUE;
		}
	}

	return FALSE;
}

/** 复制数组元素. */
template <typename TYPE>
int ArrayList<TYPE>::CopyArray( TYPE* src, TYPE* desc, UINT count )
{
	if (src == NULL || desc == NULL || count <= 0) {
		return -1;
	}

	for (UINT i = 0; i < count; i++) {
		desc[i] = src[i];
	}

	return count;
}

/**
 * 返回指定索引位置的元素的引用.
 * @param index 数组索引值, 从 0 开始.
 * @return 相应的元素的引用.
 */
template <class TYPE>
TYPE& ArrayList<TYPE>::Get( UINT index )
{
	if (!IsValidIndex(index)) {
		return fNullValue;
	}

	return fValues[index];
}

/**
 * 返回指定索引位置的元素的引用.
 * @param index 数组索引值, 从 0 开始.
 * @return 相应的元素的引用.
 */
template <class TYPE>
TYPE& ArrayList<TYPE>::Get( UINT index, TYPE& defaultValue )
{
	if (!IsValidIndex(index)) {
		return defaultValue;
	}

	return fValues[index];
}

/**
 * 返回指定索引位置的元素的引用.
 * @param index 数组索引值, 从 0 开始.
 * @return const 相应的元素的引用.
 */
template <class TYPE>
const TYPE& ArrayList<TYPE>::Get( UINT index ) const
{
	if (!IsValidIndex(index)) {
		return fNullValue;
	}

	return fValues[index];
}

/**
 * 返回指定索引位置的元素的引用.
 * @param index 数组索引值, 从 0 开始.
 * @return const 相应的元素的引用.
 */
template <class TYPE>
const TYPE& ArrayList<TYPE>::Get( UINT index, const TYPE& defaultValue ) const
{
	if (!IsValidIndex(index)) {
		return defaultValue;
	}

	return fValues[index];
}

/**
 * 检查指定的索引值是否有效.
 * @param index 数组索引值, 从 0 开始.
 * @return 如果有效则返回 TRUE. 
 */
template <class TYPE>
inline BOOL ArrayList<TYPE>::IsValidIndex(const UINT index) const 
{
	if (index >= fLength) {
		return false;
	}
	return true;
}

/** 返回指定的元素在数组中的位置. */
template <typename TYPE>
int ArrayList<TYPE>::IndexOf( const TYPE& item )
{
	for (UINT i = 0; i < fLength; i++) {
		if (item == fValues[i]) {
			return i;
		}
	}

	return -1;
}

/** 指出这个数组是否为空. */
template <typename TYPE>
BOOL ArrayList<TYPE>::IsEmpty()
{
	return (fLength <= 0);
}

/** 返回指定的元素在数组中最后出现的位置. */
template <typename TYPE>
int ArrayList<TYPE>::LastIndexOf( const TYPE& item )
{
	for (int i = fLength - 1; i >= 0; i--) {
		if (item == fValues[i]) {
			return i;
		}
	}

	return -1;
}

/**
 * 重新分配数组大小. 如有必要，增加此实例的容量，以确保它至少能够容纳
 * 最小容量参数所指定的元素数。
 * @param minCapacity 所需的最小容量。
 * @param fill 要填充的值.
 */
template <class TYPE>
void ArrayList<TYPE>::Resize(UINT minCapacity, TYPE fill)
{
	if (minCapacity <= fLength) {
		return;
	}

	fCapacity = minCapacity;
	TYPE* newValues = new TYPE[fCapacity];
	if (fValues) {
		CopyArray(fValues, newValues, fLength);
		delete[] fValues;
	}

	for (UINT i = fLength; i < minCapacity; i++) {
		newValues[i] = fill;
	}

	fLength = minCapacity;
	fValues = newValues;
}

/** 重新分配数组存储空间如果需要的话. */
template <class TYPE>
void ArrayList<TYPE>::ResizeIfNecessary()
{
	if (fCapacity < fLength) {
		return;
	}

	fCapacity = (fValues == NULL) ? 2 : fCapacity * 2;
	TYPE* newValues = new TYPE[fCapacity];
	if (fValues) {
		CopyArray(fValues, newValues, fLength);
		delete[] fValues;
	}
	
	fValues = newValues;
}

/** 移除指定位置的元素. */
template <typename TYPE>
void ArrayList<TYPE>::Remove( UINT index )
{
	if (!IsValidIndex(index)) {
		return;
	}

	for (UINT i = index; i < fLength - 1; i++) {
		fValues[i] = fValues[i + 1];
	}

	RemoveLast();
}

/** 删除这个数组的最后一个元素. */
template <class TYPE>
void ArrayList<TYPE>::RemoveLast()
{
	if (fLength <= 0) {
		return;
	}

	fLength--;

	if (fLength == 0) {
		Clear();

	} else {
		fValues[fLength] = TYPE();
	}
}

/** 设置指定位置的元素的值. */
template <typename TYPE>
void ArrayList<TYPE>::Set( UINT index, const TYPE& item )
{
	if (index >= GetLength()) {
		return;
	}

	fValues[index] = item;
}

/** 整理内存, 释放多余的未使用的空间. */
template <typename TYPE>
void ArrayList<TYPE>::TrimSize()
{
	if (fLength <= 0 || fCapacity == fLength) {
		return;
	}

	fCapacity = fLength;
	TYPE* newValues = new TYPE[fCapacity];
	if (fValues) {
		CopyArray(fValues, newValues, fLength);
		delete[] fValues;
	}

	fValues = newValues;
}

};

#endif // _NS_VISION_CORE_BASE_ARRAY_H

