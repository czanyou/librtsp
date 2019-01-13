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

#include "md5.h"

namespace core {
namespace util {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// MD5 class

/** Null data (except for first BYTE) used to finalise the checksum calculation */
BYTE MD5::PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Gets the MD5 checksum for data in a BYTE array
 * @param pBuf pointer to the BYTE array
 * @param nLength number of BYTEs of data to be checksumed
 * @return the hexadecimal MD5 checksum for the specified data
 * @note Provides an interface to the CMD5 class. Any data that can
 * be cast to a BYTE array of known length can be checksummed by this
 * function. Typically, CString and char arrays will be checksumed, 
 * although this function can be used to check the integrity of any BYTE array. 
 * A buffer of zero length can be checksummed; all buffers of zero length 
 * will return the same checksum. 
 */
LPCSTR MD5::Hash(const void* dataBuffer, UINT dataLength)
{
	if (dataBuffer == NULL || dataLength == 0) {
		return "";
	}
	//calculate and return the checksum
	UINT leftover = dataLength;
	BYTE* p = (BYTE*)dataBuffer;
	while (leftover > 0) {
		UINT size = (leftover > 64) ? 64 : leftover;
		Update(p, size);
		leftover -= size;
		p += size;
	}

	Final();
	Clear();
	return fMd5Result;
}

/**
 * Rotates the bits in a 32 bit DWORD left by a specified amount
 * @param x : the value to be rotated
 * @param n : the number of bits to rotate by
 * @return The rotated DWORD 
 */
DWORD MD5::RotateLeft(DWORD x, int n)
{
	//check that DWORD is 4 bytes long - true in Visual C++ 6 and 32 bit Windows
	ASSERT( sizeof(x) == 4 );

	//rotate and return x
	return (x << n) | (x >> (32-n));
}

/**
 * Implementation of basic MD5 transformation algorithm
 * @param A,B,C,D Current (partial) checksum
 * @param X input data
 * @param S MD5_SXX Transformation constant
 * @param T MD5_TXX Transformation constant
 */
void MD5::FF( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD F = (B & C) | (~B & D);
	A += F + X + T;
	A = RotateLeft(A, S);
	A += B;
}

/**
 * Implementation of basic MD5 transformation algorithm
 * @param A,B,C,D Current (partial) checksum
 * @param X input data
 * @param S MD5_SXX Transformation constant
 * @param T MD5_TXX Transformation constant
 */
void MD5::GG( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD G = (B & D) | (C & ~D);
	A += G + X + T;
	A = RotateLeft(A, S);
	A += B;
}

/**
 * Implementation of basic MD5 transformation algorithm
 * @param A,B,C,D Current (partial) checksum
 * @param X input data
 * @param S MD5_SXX Transformation constant
 * @param T MD5_TXX Transformation constant
 */
void MD5::HH( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD H = (B ^ C ^ D);
	A += H + X + T;
	A = RotateLeft(A, S);
	A += B;
}

/**
 * Implementation of basic MD5 transformation algorithm
 * @param A,B,C,D Current (partial) checksum
 * @param X input data
 * @param S MD5_SXX Transformation constant
 * @param T MD5_TXX Transformation constant
 */
void MD5::II( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T)
{
	DWORD I = (C ^ (B | ~D));
	A += I + X + T;
	A = RotateLeft(A, S);
	A += B;
}

/**
 * Transfers the data in an 8 bit array to a 32 bit array
 * @param output : the 32 bit (unsigned long) destination array 
 * @param input	  : the 8 bit (unsigned char) source array
 * @param nLength  : the number of 8 bit data items in the source array
 * @note Four BYTES from the input array are transferred to each DWORD entry
 * of the output array. The first BYTE is transferred to the bits (0-7) 
 * of the output DWORD, the second BYTE to bits 8-15 etc. 
 * The algorithm assumes that the input array is a multiple of 4 bytes long
 * so that there is a perfect fit into the array of 32 bit words.
 */
void MD5::ByteToDWord(DWORD* output, BYTE* input, UINT nLength)
{
	// entry invariants
	ASSERT( nLength % 4 == 0 );

	// initialisations
	UINT i=0;	//index to output array
	UINT j=0;	//index to input array

	// transfer the data by shifting and copying
	for ( ; j < nLength; i++, j += 4)
	{
		output[i] = (UINT)input[j]			| 
					(UINT)input[j+1] << 8	| 
					(UINT)input[j+2] << 16 | 
					(UINT)input[j+3] << 24;
	}
}

/**
 * MD5 basic transformation algorithm;  transforms 'm_lMD5'
 * @note An MD5 checksum is calculated by four rounds of 'Transformation'.
 * The MD5 checksum currently held in m_lMD5 is merged by the 
 * transformation process with data passed in 'Block'.  
 */
void MD5::Transform(BYTE Block[64])
{
	// initialise local data with current checksum
	UINT a = fMD5[0];
	UINT b = fMD5[1];
	UINT c = fMD5[2];
	UINT d = fMD5[3];

	//copy BYTES from input 'Block' to an array of UINTS 'X'
	UINT X[16];
	ByteToDWord( X, Block, 64 );

	// Transformation Constants - Round 1
	// Perform Round 1 of the transformation
	FF (a, b, c, d, X[ 0], MD5_S11, 0xd76aa478); // 1
	FF (d, a, b, c, X[ 1], MD5_S12, 0xe8c7b756); // 2
	FF (c, d, a, b, X[ 2], MD5_S13, 0x242070db); // 3
	FF (b, c, d, a, X[ 3], MD5_S14, 0xc1bdceee); // 4
	FF (a, b, c, d, X[ 4], MD5_S11, 0xf57c0faf); // 5
	FF (d, a, b, c, X[ 5], MD5_S12, 0x4787c62a); // 6
	FF (c, d, a, b, X[ 6], MD5_S13, 0xa8304613); // 7
	FF (b, c, d, a, X[ 7], MD5_S14, 0xfd469501); // 8
	FF (a, b, c, d, X[ 8], MD5_S11, 0x698098d8); // 9
	FF (d, a, b, c, X[ 9], MD5_S12, 0x8b44f7af); // 10
	FF (c, d, a, b, X[10], MD5_S13, 0xffff5bb1); // 11
	FF (b, c, d, a, X[11], MD5_S14, 0x895cd7be); // 12
	FF (a, b, c, d, X[12], MD5_S11, 0x6b901122); // 13
	FF (d, a, b, c, X[13], MD5_S12, 0xfd987193); // 14
	FF (c, d, a, b, X[14], MD5_S13, 0xa679438e); // 15
	FF (b, c, d, a, X[15], MD5_S14, 0x49b40821); // 16

	// Transformation Constants - Round 2
	// Perform Round 2 of the transformation
	GG (a, b, c, d, X[ 1], MD5_S21, 0xf61e2562); // 17
	GG (d, a, b, c, X[ 6], MD5_S22, 0xc040b340); // 18
	GG (c, d, a, b, X[11], MD5_S23, 0x265e5a51); // 19
	GG (b, c, d, a, X[ 0], MD5_S24, 0xe9b6c7aa); // 20
	GG (a, b, c, d, X[ 5], MD5_S21, 0xd62f105d); // 21
	GG (d, a, b, c, X[10], MD5_S22, 0x02441453); // 22
	GG (c, d, a, b, X[15], MD5_S23, 0xd8a1e681); // 23
	GG (b, c, d, a, X[ 4], MD5_S24, 0xe7d3fbc8); // 24
	GG (a, b, c, d, X[ 9], MD5_S21, 0x21e1cde6); // 25
	GG (d, a, b, c, X[14], MD5_S22, 0xc33707d6); // 26
	GG (c, d, a, b, X[ 3], MD5_S23, 0xf4d50d87); // 27
	GG (b, c, d, a, X[ 8], MD5_S24, 0x455a14ed); // 28
	GG (a, b, c, d, X[13], MD5_S21, 0xa9e3e905); // 29
	GG (d, a, b, c, X[ 2], MD5_S22, 0xfcefa3f8); // 30
	GG (c, d, a, b, X[ 7], MD5_S23, 0x676f02d9); // 31
	GG (b, c, d, a, X[12], MD5_S24, 0x8d2a4c8a); // 32

	// Transformation Constants - Round 3
	// Perform Round 3 of the transformation
	HH (a, b, c, d, X[ 5], MD5_S31, 0xfffa3942); // 33
	HH (d, a, b, c, X[ 8], MD5_S32, 0x8771f681); // 34
	HH (c, d, a, b, X[11], MD5_S33, 0x6d9d6122); // 35
	HH (b, c, d, a, X[14], MD5_S34, 0xfde5380c); // 36
	HH (a, b, c, d, X[ 1], MD5_S31, 0xa4beea44); // 37
	HH (d, a, b, c, X[ 4], MD5_S32, 0x4bdecfa9); // 38
	HH (c, d, a, b, X[ 7], MD5_S33, 0xf6bb4b60); // 39
	HH (b, c, d, a, X[10], MD5_S34, 0xbebfbc70); // 40
	HH (a, b, c, d, X[13], MD5_S31, 0x289b7ec6); // 41
	HH (d, a, b, c, X[ 0], MD5_S32, 0xeaa127fa); // 42
	HH (c, d, a, b, X[ 3], MD5_S33, 0xd4ef3085); // 43
	HH (b, c, d, a, X[ 6], MD5_S34, 0x04881d05); // 44
	HH (a, b, c, d, X[ 9], MD5_S31, 0xd9d4d039); // 45
	HH (d, a, b, c, X[12], MD5_S32, 0xe6db99e5); // 46
	HH (c, d, a, b, X[15], MD5_S33, 0x1fa27cf8); // 47
	HH (b, c, d, a, X[ 2], MD5_S34, 0xc4ac5665); // 48

	// Transformation Constants - Round 4
	// Perform Round 4 of the transformation
	II (a, b, c, d, X[ 0], MD5_S41, 0xf4292244); // 49
	II (d, a, b, c, X[ 7], MD5_S42, 0x432aff97); // 50
	II (c, d, a, b, X[14], MD5_S43, 0xab9423a7); // 51
	II (b, c, d, a, X[ 5], MD5_S44, 0xfc93a039); // 52
	II (a, b, c, d, X[12], MD5_S41, 0x655b59c3); // 53
	II (d, a, b, c, X[ 3], MD5_S42, 0x8f0ccc92); // 54
	II (c, d, a, b, X[10], MD5_S43, 0xffeff47d); // 55
	II (b, c, d, a, X[ 1], MD5_S44, 0x85845dd1); // 56
	II (a, b, c, d, X[ 8], MD5_S41, 0x6fa87e4f); // 57
	II (d, a, b, c, X[15], MD5_S42, 0xfe2ce6e0); // 58
	II (c, d, a, b, X[ 6], MD5_S43, 0xa3014314); // 59
	II (b, c, d, a, X[13], MD5_S44, 0x4e0811a1); // 60
	II (a, b, c, d, X[ 4], MD5_S41, 0xf7537e82); // 61
	II (d, a, b, c, X[11], MD5_S42, 0xbd3af235); // 62
	II (c, d, a, b, X[ 2], MD5_S43, 0x2ad7d2bb); // 63
	II (b, c, d, a, X[ 9], MD5_S44, 0xeb86d391); // 64

	// add the transformed values to the current checksum
	fMD5[0] += a;
	fMD5[1] += b;
	fMD5[2] += c;
	fMD5[3] += d;
}

/** Initialises member data. */
MD5::MD5()
{
	memset(fMd5Result, 0, sizeof(fMd5Result));

	// zero members
	memset( fBuffer, 0, 64 );
	fCount[0] = fCount[1] = 0;

	// Magic initialization constants
	// Load magic state initialization constants
	fMD5[0] = 0x67452301;	// MD5_INIT_STATE_0
	fMD5[1] = 0xefcdab89;	// MD5_INIT_STATE_1
	fMD5[2] = 0x98badcfe;	// MD5_INIT_STATE_2
	fMD5[3] = 0x10325476;	// MD5_INIT_STATE_3
}

void MD5::Clear()
{
	// zero members
	memset( fBuffer, 0, 64 );
	fCount[0] = fCount[1] = 0;

	// Magic initialization constants
	// Load magic state initialization constants
	fMD5[0] = 0x67452301;	// MD5_INIT_STATE_0
	fMD5[1] = 0xefcdab89;	// MD5_INIT_STATE_1
	fMD5[2] = 0x98badcfe;	// MD5_INIT_STATE_2
	fMD5[3] = 0x10325476;	// MD5_INIT_STATE_3
}

/**
 * Transfers the data in an 32 bit array to a 8 bit array
 * @param output  : the 8 bit destination array 
 * @param input  : the 32 bit source array
 * @param nLength  : the number of 8 bit data items in the source array
 * @note One DWORD from the input array is transferred into four BYTES 
 * in the output array. The first (0-7) bits of the first DWORD are 
 * transferred to the first output BYTE, bits bits 8-15 are transferred from
 * the second BYTE etc. 
 * 
 * The algorithm assumes that the output array is a multiple of 4 bytes long
 * so that there is a perfect fit of 8 bit BYTES into the 32 bit DWORDs.
 */
void MD5::DWordToByte(BYTE* output, DWORD* input, UINT nLength )
{
	// entry invariants
	ASSERT( nLength % 4 == 0 );

	// transfer the data by shifting and copying
	UINT i = 0;
	UINT j = 0;
	for ( ; j < nLength; i++, j += 4) {
		output[j] =   (BYTE)(input[i] & 0xff);
		output[j+1] = (BYTE)((input[i] >> 8) & 0xff);
		output[j+2] = (BYTE)((input[i] >> 16) & 0xff);
		output[j+3] = (BYTE)((input[i] >> 24) & 0xff);
	}
}

/**
 * Implementation of main MD5 checksum algorithm; ends the checksum calculation.
 * @return the final hexadecimal MD5 checksum result 
 * @note Performs the final MD5 checksum calculation ('Update' does most of the work,
 * this function just finishes the calculation.) 
 */
LPCSTR MD5::Final()
{
	// Save number of bits
	BYTE Bits[8];
	DWordToByte( Bits, fCount, 8 );

	// Pad out to 56 mod 64.
	UINT nIndex = (UINT)((fCount[0] >> 3) & 0x3f);
	UINT nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
	Update( PADDING, nPadLen );

	// Append length (before padding)
	Update( Bits, 8 );

	// Store final state in 'lpszMD5'
	const int nMD5Size = 16;
	BYTE lpszMD5[ nMD5Size ];
	DWordToByte( lpszMD5, fMD5, nMD5Size );

	// Convert the hexadecimal checksum to a CString
	char buf[16];
	memset(buf, 0, sizeof(buf));
	memset(fMd5Result, 0, sizeof(fMd5Result));

	size_t size = 0;
	for ( int i = 0; i < nMD5Size; i++) {
		sprintf(fMd5Result + size, "%02x", lpszMD5[i]);
		size += 2;
	}
	ASSERT(size == 32);
	return fMd5Result;
}

/**
 * Implementation of main MD5 checksum algorithm.
 * Computes the partial MD5 checksum for 'nInputLen' bytes of data in 'input'
 *
 * @param input input block
 * @param nInputLen length of input block
 */
void MD5::Update( BYTE* input, UINT nInputLen )
{
	// Compute number of bytes mod 64
	UINT nIndex = (UINT)((fCount[0] >> 3) & 0x3F);

	// Update number of bits
	if ( ( fCount[0] += nInputLen << 3 )  <  ( nInputLen << 3) ) {
		fCount[1]++;
	}
	fCount[1] += (nInputLen >> 29);

	// Transform as many times as possible.
	UINT i=0;		
	UINT nPartLen = 64 - nIndex;
	if (nInputLen >= nPartLen) 	
	{
		memcpy( &fBuffer[nIndex], input, nPartLen );
		Transform( fBuffer );
		for (i = nPartLen; i + 63 < nInputLen; i += 64) {
			Transform( &input[i] );
		}
		nIndex = 0;

	} else  {
		i = 0;
	}

	// Buffer remaining input
	memcpy( &fBuffer[nIndex], &input[i], nInputLen-i);
}

};
};
