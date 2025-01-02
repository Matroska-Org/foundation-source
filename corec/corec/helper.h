/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __HELPER_H
#define __HELPER_H

#define SWAP32(a) ((((uint32_t)(a) >> 24) & 0x000000FF) | (((uint32_t)(a) >> 8)  & 0x0000FF00)|\
                  (((uint32_t)(a) << 8)  & 0x00FF0000) | (((uint32_t)(a) << 24) & 0xFF000000))

#define SWAP16(a) ((uint16_t)((((uint32_t)(a) >> 8) & 0xFF) | (((uint32_t)(a) << 8) & 0xFF00)))
#define SWAP64(a) (((uint64_t)SWAP32(a) << 32) | SWAP32((uint64_t)(a)>>32))

#ifdef IS_BIG_ENDIAN
#define INT64BE(a) (a)
#define INT64LE(a) SWAP64(a)
#define INT32BE(a) (a)
#define INT32LE(a) SWAP32(a)
#define INT16BE(a) (a)
#define INT16LE(a) SWAP16(a)
#else
#define INT64LE(a) (a)
#define INT64BE(a) SWAP64(a)
#define INT32LE(a) (a)
#define INT32BE(a) SWAP32(a)
#define INT16LE(a) (a)
#define INT16BE(a) SWAP16(a)
#endif

#define LOAD8(ptr,ofs)		(((uint8_t*)(ptr))[ofs])
#define LOAD16BE(ptr)		((uint16_t)((LOAD8(ptr,0)<<8)|LOAD8(ptr,1)))
#define LOAD32LE(ptr)		((LOAD8(ptr,3)<<24)|(LOAD8(ptr,2)<<16)|(LOAD8(ptr,1)<<8)|LOAD8(ptr,0))
#define LOAD32BE(ptr)		((LOAD8(ptr,0)<<24)|(LOAD8(ptr,1)<<16)|(LOAD8(ptr,2)<<8)|LOAD8(ptr,3))
#define LOAD64BE(ptr)		((((uint64_t)LOAD8(ptr,0))<<56)|(((uint64_t)LOAD8(ptr,1))<<48)|(((uint64_t)LOAD8(ptr,2))<<40)|(((uint64_t)LOAD8(ptr,3))<<32)| \
							 (((uint64_t)LOAD8(ptr,4))<<24)|(((uint64_t)LOAD8(ptr,5))<<16)|(((uint64_t)LOAD8(ptr,6))<< 8)|(((uint64_t)LOAD8(ptr,7))    ))


#define STORE8(p, o, i)      ((uint8_t *) (p))[o] = (uint8_t) ((i) & 0xFF)

#define STORE16BE(p, i)      STORE8(p, 1, i), STORE8(p, 0, ((uint16_t)i) >> 8)

#define STORE32LE(p, i)      STORE8(p, 0, i), STORE8(p, 1, ((uint32_t)i) >> 8), STORE8(p, 2, ((uint32_t)i) >> 16), STORE8(p, 3, ((uint32_t)i) >> 24)

static INLINE int64_t Scale64(int64_t v,int64_t Num,int64_t Den)
{
	if (Den)
		return (v * Num) / Den;
	return 0;
}

static INLINE int Scale32(int64_t v, int64_t Num, int64_t Den)
{
    return (int)Scale64(v,Num,Den);
}

typedef int32_t datetime_t;

#define INVALID_DATETIME_T  0

#if !defined(MIN)
#  define MIN(x,y)  ((x)>(y)?(y):(x))
#endif

#if !defined(MAX)
#  define MAX(x,y)  ((x)<(y)?(y):(x))
#endif


#endif
