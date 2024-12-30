/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __HELPER_H
#define __HELPER_H

#ifdef __cplusplus
#define OFS(name,item) offsetof(name,item)
#else // !__cplusplus
#ifdef TARGET_LINUX
#include <stddef.h>
#define OFS(name,item) offsetof(name,item)
#else
#define OFS(name,item) ((uintptr_t)&(((name*)NULL)->item))
#endif
#endif // !__cplusplus

#define ALIGN64(x) (((x) + 63) & ~63)
#define ALIGN16(x) (((x) + 15) & ~15)
#define ALIGN8(x) (((x) + 7) & ~7)
#define ALIGN4(x) (((x) + 3) & ~3)
#define ALIGN2(x) (((x) + 1) & ~1)

#define GET_R(x)   ((uint8_t)(((x) >> 0) & 255))
#define GET_G(x)   ((uint8_t)(((x) >> 8) & 255))
#define GET_B(x)   ((uint8_t)(((x) >> 16) & 255))

#define SWAP32(a) ((((uint32_t)(a) >> 24) & 0x000000FF) | (((uint32_t)(a) >> 8)  & 0x0000FF00)|\
                  (((uint32_t)(a) << 8)  & 0x00FF0000) | (((uint32_t)(a) << 24) & 0xFF000000))

#define SWAP16(a) ((uint16_t)((((uint32_t)(a) >> 8) & 0xFF) | (((uint32_t)(a) << 8) & 0xFF00)))
#define SWAP64(a) (((uint64_t)SWAP32(a) << 32) | SWAP32((uint64_t)(a)>>32))

#define LSHIFT(v,n)			((v)<<(n))
#define RSHIFT(v,n)			((v)>>(n))
#define RLSHIFT(v,n)		(((n)>=0)?RSHIFT(v,n):LSHIFT(v,-(n)))
#define RSHIFT_ROUND(v,n)	(((v)+(1<<(n-1)))>>(n))
#define RSHIFT_ROUND_COND(v,n)	((n)>0 ? RSHIFT_ROUND(v,n) : v)

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

// a=(a+c+1)/2
// b=(b+d+1)/2
#define AVG32R(a,b,c,d) \
	c^=a; \
	d^=b; \
	a|=c; \
	b|=d; \
	c &= 0xFEFEFEFE; \
	d &= 0xFEFEFEFE; \
	a-=c>>1; \
	b-=d>>1;

#define AVG64R(a,b,c,d) \
	c^=a; \
	d^=b; \
	a|=c; \
	b|=d; \
	c &= 0xFEFEFEFEFEFEFEFE; \
	d &= 0xFEFEFEFEFEFEFEFE; \
	a-=c>>1; \
	b-=d>>1;

// a=(a+c)/2
// b=(b+d)/2
#ifdef ARM
#define AVG32NR(a,b,c,d) \
	c^=a; \
	d^=b; \
	a &= ~c; \
	b &= ~d; \
	c &= 0xFEFEFEFE; \
	d &= 0xFEFEFEFE; \
	a+=c>>1; \
	b+=d>>1;
#else
#define AVG32NR(a,b,c,d) \
	c^=a; \
	d^=b; \
	a|=c; \
	b|=d; \
	a-=c & 0x01010101; \
	b-=d & 0x01010101; \
	c &= 0xFEFEFEFE; \
	d &= 0xFEFEFEFE; \
	a-=c>>1; \
	b-=d>>1;
#endif

#define AVG64NR(a,b,c,d) \
	c^=a; \
	d^=b; \
	a|=c; \
	b|=d; \
	a-=c & 0x0101010101010101; \
	b-=d & 0x0101010101010101; \
	c &= 0xFEFEFEFEFEFEFEFE; \
	d &= 0xFEFEFEFEFEFEFEFE; \
	a-=c>>1; \
	b-=d>>1;

#define _abs(a) (((a)>=0)?(a):-(a))

static INLINE int MIDDLE(int a, int b, int c)
{
    int bb = b;

    if (a > b)
    {
        bb = a;
        a = b;
    }

    if (bb > c)
        bb = c;

    if (a > bb)
        bb = a;

    return bb;
}

static INLINE size_t _log2(uint32_t data)
{
	size_t i;
	if (!data) ++data;
	for (i=0;data;++i)
		data >>= 1;
    return i;
}

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

#define SWAPVAL(type,a,b) { type __t = (a); (a)=(b); (b)=__t; }

typedef int32_t datetime_t;

#define INVALID_DATETIME_T  0
#define MIN_DATETIME_T  ((datetime_t) 0xFFFFFFFF)
#define MAX_DATETIME_T  ((datetime_t) 0x7FFFFFFF)

#if !defined(min) && !defined(NOMINMAX)
#  define min(x,y)  ((x)>(y)?(y):(x))
#endif

#if !defined(max) && !defined(NOMINMAX)
#  define max(x,y)  ((x)<(y)?(y):(x))
#endif

#ifndef sign
#  define sign(x) ((x)<0?-1:1)
#endif

#define abs_diff(x,y)  ((x)>(y)?((x)-(y)):((y)-(x)))

#endif
