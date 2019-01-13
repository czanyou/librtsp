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
#ifndef _NS_VISION_CORE_UTIL_MD5_H_
#define _NS_VISION_CORE_UTIL_MD5_H_

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MD5 class

/**
 * MD5 摘要算法. 
 *
 */
class MD5
{
public:
	// constructor/destructor
	MD5();
	virtual ~MD5() {};

public:
	LPCSTR Hash(const void* buf, UINT length);

// Implements -------------------------------------------------
private:
	/** Constants for Transform routine. */
	enum MD5TransformConstants {
		MD5_S11 =  7,
		MD5_S12 = 12,
		MD5_S13 = 17,
		MD5_S14 = 22,
		MD5_S21 =  5,
		MD5_S22 =  9,
		MD5_S23 = 14,
		MD5_S24 = 20,
		MD5_S31 =  4,
		MD5_S32 = 11,
		MD5_S33 = 16,
		MD5_S34 = 23,
		MD5_S41 =  6,
		MD5_S42 = 10,
		MD5_S43 = 15,
		MD5_S44 = 21
	};

	// RSA MD5 implementation
	void Transform(BYTE block[64]);
	void Update(BYTE* input, UINT inputLen);
	LPCSTR Final();
	void Clear();

// Implements -------------------------------------------------
	inline DWORD RotateLeft(DWORD x, int n);
	inline void FF(DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void GG(DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void HH(DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	inline void II(DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);

	// utility functions
	void DWordToByte(BYTE* output, DWORD* input, UINT length);
	void ByteToDWord(DWORD* output, BYTE* input, UINT length);

// Data Members -----------------------------------------------
private:
	static BYTE PADDING[64];
	char fMd5Result[64 + 1];

	BYTE fBuffer[64];		// input buffer
	UINT fCount[2];		// number of bits, modulo 2^64 (lsb first)
	UINT fMD5[4];			// MD5 checksum
};

};
};

#endif // !defined(_NS_VISION_CORE_UTIL_MD5_H_)
