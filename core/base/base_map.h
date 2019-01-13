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
#ifndef _NS_VISION_CORE_BASE_MAP_H
#define _NS_VISION_CORE_BASE_MAP_H

#include "base_types.h"
#include "base_list.h"

namespace core {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// HashFunctions class

/** 
 * 包含了一些基本的常用 Hash 方法. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
class HashFunctions
{
public:
	inline static UINT Hash(char   key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(BYTE   key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(short  key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(WORD   key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(int    key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(UINT   key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(long   key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(ULONG  key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(double key)	{ return (UINT)key; }	///< Hash
	inline static UINT Hash(void*  key)	{ return (UINT)(INT64)key; }	///< Hash
	inline static UINT Hash(INT64  key);
	inline static UINT Hash(LPCSTR key);
};

inline UINT HashFunctions::Hash(INT64  key)	
{
	return UINT((key >> 31) ^ key);
}

inline UINT HashFunctions::Hash(LPCSTR key) 
{
	UINT seed = 131; // 31 131 1313 13131 131313 etc..
	UINT hash = 0;

	if (key == NULL) {
		return seed;
	}

	while (*key) {	
		hash = hash * seed + (*key++);
	}

	return UINT(hash & 0x7FFFFFFF);
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// HashMap class

/** 
 * 一个简单的 Hash 表实现. 
 *
 * @author ChengZhen (anyou@msn.com)
 */
template<class KEY, class VALUE, class ARG_KEY = const KEY&, class ARG_VALUE = const VALUE&>
class HashMap
{
public:
	/** 代表 HashTable 的一个实体. */
	struct Entry {
		Entry() {
			fKey		= KEY();
			fValue		= VALUE();
			fHashCode	= 0;
			fNextEntry	= NULL;
		}

		Entry(const KEY& key, const VALUE& value) {
			fValue		= value;
			fKey		= key;
			fHashCode	= 0;
			fNextEntry	= NULL;
		}

		UINT     fHashCode;		///< 这个条目的键的哈希值
		KEY		 fKey;			///< 这个表格条目的键(key)
		Entry*   fNextEntry;	///< 具有同样的哈希值的下一步条目(如果有的话).
		VALUE    fValue;		///< 这个条目的值
	};

	typedef Entry* EntryPtr;			///< 实体指针类型
	typedef LinkedList<EntryPtr> EntryList;	///< 实体列表类型
	typedef size_t size_type;			///< 大小类型
	typedef VALUE value_type;			///< 值类型

// ------------------------------------------------------------
	HashMap();
	HashMap(const HashMap& map) { operator = (map); }
	~HashMap();

// Operations -------------------------------------------------
public:
	void Clear();
	BOOL Contanis(const KEY& key) const;
	BOOL Get(const KEY& key, VALUE& value) const;
	BOOL GetEntrys(EntryList& list);
	UINT GetSize() const;
	BOOL IsEmpty() const { return fEntryCount <= 0; }		///< 如果此映射未包含键-值映射关系，则返回 true。
	BOOL Put(const KEY& key, const VALUE& value);
	BOOL Remove(const KEY& key);			

public:
	const VALUE GetValue(const KEY& key) const;
	const VALUE GetValue(const KEY& key, const VALUE& defaultValue) const;
	const VALUE operator[](const KEY& key) const { return value(key); }

	VALUE&   operator[] (const KEY& key);
	HashMap& operator = (const HashMap& map);

// Implementation ---------------------------------------------
private:
	EntryPtr FindEntry(const KEY& key, UINT* hashCode = NULL) const;
	EntryPtr NewEntry(UINT hashCode, const KEY& key, const VALUE& value, EntryPtr* nextEntry);

	void Create(UINT initSize);
	BOOL Expand();	
	UINT Hash(const KEY& key) const;
	UINT IndexOf(UINT tableLength, UINT hashValue) const;

// Data Members -----------------------------------------------
private:
	EntryPtr* fEntryTable;		///< Entry 表

	UINT fEntryCount;			///< 此映射中的键-值映射关系数
	UINT fLoadLimit;			///< 
	UINT fPrimeIndex;			///< 当前使用的素数的索引
	UINT fTableLength;			///< Entry 表的长度

private:
	static const UINT PRIMES[];	///< 素数表, 用于生成 Entry 表的长度
	static const float MAX_LOAD_FACTOR;
};


///////////////////////////////////////////////////////////////////////////////
// 

#define HASHMAP_TEMPLATE template<class KEY, class VALUE, class ARG_KEY, class ARG_VALUE>
#define HASHMAP_TYPE HashMap<KEY, VALUE, ARG_KEY, ARG_VALUE>
#define HASHMAP_PRIMES_SIZE 26

/** 表格中最多存放的实体的比例, 超过这个比例需要扩大表格的大小. */
HASHMAP_TEMPLATE const float HASHMAP_TYPE::MAX_LOAD_FACTOR = 0.65f;	

/** primes table, 用于构建比较理想的 Hash table 的长度. */
HASHMAP_TEMPLATE const UINT HASHMAP_TYPE::PRIMES[] = {
	53, 97, 193, 389,
	769, 1543, 3079, 6151,
	12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869,
	3145739, 6291469, 12582917, 25165843,
	50331653, 100663319, 201326611, 402653189,
	805306457, 1610612741
};

HASHMAP_TEMPLATE HASHMAP_TYPE::HashMap()
{
	fEntryCount		= 0;
	fEntryTable		= NULL;
	fLoadLimit		= 0;
	fPrimeIndex		= 0;
	fTableLength	= 0;

	Create(16);
}

HASHMAP_TEMPLATE HASHMAP_TYPE::~HashMap()
{
	Clear();
	if (fEntryTable) {
		free(fEntryTable);
		fEntryTable	= NULL;
	}
}

HASHMAP_TEMPLATE HASHMAP_TYPE& 
HASHMAP_TYPE::operator=( const HashMap& map )
{
	fEntryCount		= 0;
	fEntryTable		= NULL;
	fLoadLimit		= 0;
	fPrimeIndex		= 0;
	fTableLength	= 0;

	Create(map.fTableLength);

	for (UINT i = 0; i < map.fTableLength; i++) {
		EntryPtr p = map.fEntryTable[i];
		while (p) {
			Put(p->fKey, p->fValue);
			p = p->fNextEntry;
		}
	}

	return *this;
}

/**
 * 返回所有的实体.
 * @param list 
 */
HASHMAP_TEMPLATE
BOOL HASHMAP_TYPE::GetEntrys(EntryList& list) 
{
	for (UINT i = 0; i < fTableLength; i++) {
		EntryPtr p = fEntryTable[i];
		while (p) {
			list.AddLast(p);
			p = p->fNextEntry;
		}
	}
	return true;
}

/**
	* 查找指定的实体.
	* @param key 
	* @param hashCode 
	* @return EntryPtr 
	*/
HASHMAP_TEMPLATE typename HASHMAP_TYPE::EntryPtr
HASHMAP_TYPE::FindEntry(const KEY& key, UINT* hashCode) const 
{
	UINT code = Hash(key);
	if (hashCode) {
		*hashCode = code;
	}

	UINT index = IndexOf(fTableLength, code);
	EntryPtr entry = fEntryTable[index];
	while (entry != NULL) {
		// Check Hash value to short circuit heavier comparison 
		if (key == entry->fKey) {
			return entry;
		}
		entry = entry->fNextEntry;
	}
	return NULL;
}

/**
	* 创建一个新的实体.
	* @param hashCode 
	* @param key 
	* @param value 
	* @param nextEntry 
	* @return EntryPtr 
	*/
HASHMAP_TEMPLATE typename HASHMAP_TYPE::EntryPtr
HASHMAP_TYPE::NewEntry(UINT hashCode, const KEY& key, const VALUE& value, EntryPtr* nextEntry) 
{
	EntryPtr entry = new Entry(key, value);
	entry->fHashCode = hashCode;
	entry->fNextEntry = *nextEntry;
	*nextEntry = entry;

	/* This method allows duplicate keys - but they shouldn't be used */
	if (++(fEntryCount) > fLoadLimit) {
		/* Ignore the return value. If Expand fails, we should
		* still try cramming just this value into the existing table
		* -- we may not have memory for a larger table, but one more
		* element may be ok. Next time we insert, we'll try expanding again.*/
		Expand();
	}

	return entry;
}

HASHMAP_TEMPLATE UINT HASHMAP_TYPE::IndexOf(UINT tableLength, UINT hashCode) const 
{
	return (hashCode % tableLength);
}

HASHMAP_TEMPLATE UINT HASHMAP_TYPE::Hash( const KEY& key ) const
{
	/* Aim to protect against poor Hash functions by adding logic here
	* - logic taken from java 1.4 hashtable source */
	UINT i = HashFunctions::Hash(key);
	i += ~(i << 9);
	i ^=  ((i >> 14) | (i << 18)); /* >>> */
	i +=  (i << 4);
	i ^=  ((i >> 10) | (i << 22)); /* >>> */
	return i;
}

HASHMAP_TEMPLATE void HASHMAP_TYPE::Create(UINT initSize)
{
	/* Check requested hashtable isn't too large */
	if (initSize > (1u << 30)) {
		return;
	}

	/* Enforce size as prime */
	UINT pindex = 0;
	UINT size = PRIMES[0];
	UINT prime_table_length = HASHMAP_PRIMES_SIZE;
	for (pindex = 0; pindex < prime_table_length; pindex++) {
		if (PRIMES[pindex] > initSize) { 
			size = PRIMES[pindex];
			break; 
		}
	}

	fEntryTable = (EntryPtr*)malloc(sizeof(EntryPtr) * size);
	if (NULL == fEntryTable) { 
		return; 
	} /*oom*/

	memset(fEntryTable, 0, size * sizeof(EntryPtr));
	fTableLength  = size;
	fPrimeIndex   = pindex;
	fEntryCount   = 0;
	fLoadLimit    = (UINT) ceil(size * MAX_LOAD_FACTOR);
}

HASHMAP_TEMPLATE BOOL HASHMAP_TYPE::Expand()
{
	/* Check we're not hitting max capacity */
	UINT prime_table_length = HASHMAP_PRIMES_SIZE;
	if (fPrimeIndex >= (prime_table_length - 1)) {
		return false;
	}

	/* Double the size of the table to accomodate more entries */
	EntryPtr  entry;
	EntryPtr* pE;
	UINT i, index;

	UINT newsize = PRIMES[++(fPrimeIndex)];

	EntryPtr* newtable = (EntryPtr*)malloc(sizeof(EntryPtr) * newsize);
	if (newtable != NULL) {
		memset(newtable, 0, newsize * sizeof(EntryPtr));
		/* This algorithm is not 'stable'. ie. it reverses the list
		* when it transfers entries between the tables */
		for (i = 0; i < fTableLength; i++) {
			while ((entry = fEntryTable[i]) != NULL) {
				fEntryTable[i] = entry->fNextEntry;
				UINT index = IndexOf(newsize, entry->fHashCode);
				entry->fNextEntry = newtable[index];
				newtable[index] = entry;
			}
		}
		free(fEntryTable);
		fEntryTable = newtable;

	/* Plan B: realloc instead */
	} else {
		newtable = (EntryPtr*)realloc(fEntryTable, newsize * sizeof(EntryPtr));
		if (NULL == newtable) { 
			(fPrimeIndex)--; 
			return false; 
		}
		fEntryTable = newtable;
		memset(newtable[fTableLength], 0, newsize - fTableLength);
		for (i = 0; i < fTableLength; i++) {
			for (pE = &(newtable[i]), entry = *pE; entry != NULL; entry = *pE) {
				index = IndexOf(newsize,entry->fHashCode);
				if (index == i) {
					pE = &(entry->fNextEntry);
				} else {
					*pE = entry->fNextEntry;
					entry->fNextEntry = newtable[index];
					newtable[index] = entry;
				}
			}
		}
	}

	fTableLength = newsize;
	fLoadLimit   = (UINT) ceil(newsize * MAX_LOAD_FACTOR);
	return true;
}

/** 从此映射中移除所有映射关系. */
HASHMAP_TEMPLATE void HASHMAP_TYPE::Clear()
{
	fEntryCount	= 0;
	if (fEntryTable == NULL) {
		return;
	}

	for (UINT i = 0; i < fTableLength; i++) {
		EntryPtr next = fEntryTable[i];
		while (next) { 
			EntryPtr entry = next; 
			next = entry->fNextEntry;
			entry->fNextEntry = NULL;
			delete entry;
		}

		fEntryTable[i] = NULL;
	}
}

/** 返回此映射中的键-值映射关系数. */
HASHMAP_TEMPLATE UINT HASHMAP_TYPE::GetSize() const
{
	return fEntryCount;
}

/** 将指定的值与此映射中的指定键相关联 */
HASHMAP_TEMPLATE BOOL HASHMAP_TYPE::Put( const KEY& key, const VALUE& value)
{
	Remove(key);

	UINT hashCode = Hash(key);
	EntryPtr* p = fEntryTable + IndexOf(fTableLength, hashCode);
	NewEntry(hashCode, key, value, p);
	return true;
}

/** 如果此映射包含指定键的映射关系，则返回 true。*/
HASHMAP_TEMPLATE BOOL HASHMAP_TYPE::Contanis( const KEY& key ) const
{
	return (FindEntry(key) != NULL);
}

/** 返回此映射中映射到指定键的值。*/
HASHMAP_TEMPLATE const VALUE HASHMAP_TYPE::GetValue( const KEY& key ) const
{
	EntryPtr entry = FindEntry(key);
	if (entry) {
		return entry->fValue;
	}

	return VALUE();
}

/** 返回此映射中映射到指定键的值。*/
HASHMAP_TEMPLATE const VALUE HASHMAP_TYPE::GetValue( const KEY& key, const VALUE& defaultValue ) const
{
	EntryPtr entry = FindEntry(key);
	return (entry != NULL) ? entry->fValue : defaultValue;
}

/** 返回此映射中映射到指定键的值。*/
HASHMAP_TEMPLATE BOOL HASHMAP_TYPE::Get( const KEY& key, VALUE& value) const 
{
	EntryPtr entry = FindEntry(key);
	if (entry) {
		value = entry->fValue;
		return true;
	}
	return false;
}

/** 返回此映射中映射到指定键的值。*/
HASHMAP_TEMPLATE VALUE& HASHMAP_TYPE::operator[]( const KEY& key )
{
	UINT hashCode = 0;
	EntryPtr entry = FindEntry(key, &hashCode);
	if (entry) {
		return entry->fValue;

	} else {
		EntryPtr* p = fEntryTable + IndexOf(fTableLength, hashCode);
		entry = NewEntry(hashCode, key, VALUE(), p);
		return entry->fValue;
	}
}

/** 如果存在此键的映射关系，则将其从映射中移除. */
HASHMAP_TEMPLATE BOOL HASHMAP_TYPE::Remove( const KEY& key)
{
	UINT index = IndexOf(fTableLength, Hash(key));
	EntryPtr entry = fEntryTable[index];
	if (entry == NULL) {
		return false;

	} else if (entry->fKey == key) {
		fEntryCount--;
		fEntryTable[index] = entry->fNextEntry;
		entry->fNextEntry = NULL;
		delete entry;
		return true;
	}

	BOOL ret = false;
	while (entry->fNextEntry != NULL) {
		EntryPtr next = entry->fNextEntry;

		/* Check Hash value to short circuit heavier comparison */
		if (key == next->fKey) {
			entry->fNextEntry = next->fNextEntry;	// 从链表中删除这个节点
			next->fNextEntry  = NULL;
			fEntryCount--;
			delete next;
			ret = true;
			break;
		}

		entry = next;
	}

	return ret;
}

};

#endif // _NS_VISION_CORE_BASE_MAP_H

