/*****************************************************************************
 * 
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CoreCodec, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CoreCodec, Inc. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CoreCodec, Inc. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __HELPER_H
#define __HELPER_H

#ifdef TARGET_LINUX
#include <stddef.h>
#define OFS(name,item) offsetof(name,item)
#else
#define OFS(name,item) ((uintptr_t)&(((name*)NULL)->item))
#endif

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
#if defined(CONFIG_UNALIGNED_ACCESS)
#define LOAD16LE(ptr)		INT16LE(*(uint16_t*)(ptr))
#define LOAD16BE(ptr)		INT16BE(*(uint16_t*)(ptr))
#define LOAD32LE(ptr)		INT32LE(*(uint32_t*)(ptr))
#define LOAD32BE(ptr)		INT32BE(*(uint32_t*)(ptr))
#define LOAD64LE(ptr)		INT64LE(*(uint64_t*)(ptr))
#define LOAD64BE(ptr)		INT64BE(*(uint64_t*)(ptr))
#else
#define LOAD16LE(ptr)		((uint16_t)((LOAD8(ptr,1)<<8)|LOAD8(ptr,0)))
#define LOAD16BE(ptr)		((uint16_t)((LOAD8(ptr,0)<<8)|LOAD8(ptr,1)))
#define LOAD32LE(ptr)		((LOAD8(ptr,3)<<24)|(LOAD8(ptr,2)<<16)|(LOAD8(ptr,1)<<8)|LOAD8(ptr,0))
#define LOAD32BE(ptr)		((LOAD8(ptr,0)<<24)|(LOAD8(ptr,1)<<16)|(LOAD8(ptr,2)<<8)|LOAD8(ptr,3))
#define LOAD64LE(ptr)		((((uint64_t)LOAD8(ptr,0))    )|(((uint64_t)LOAD8(ptr,1))<< 8)|(((uint64_t)LOAD8(ptr,2))<<16)|(((uint64_t)LOAD8(ptr,3))<<24)| \
							 (((uint64_t)LOAD8(ptr,4))<<32)|(((uint64_t)LOAD8(ptr,5))<<40)|(((uint64_t)LOAD8(ptr,6))<<48)|(((uint64_t)LOAD8(ptr,0))<<56))
#define LOAD64BE(ptr)		((((uint64_t)LOAD8(ptr,0))<<56)|(((uint64_t)LOAD8(ptr,1))<<48)|(((uint64_t)LOAD8(ptr,2))<<40)|(((uint64_t)LOAD8(ptr,3))<<32)| \
							 (((uint64_t)LOAD8(ptr,4))<<24)|(((uint64_t)LOAD8(ptr,5))<<16)|(((uint64_t)LOAD8(ptr,6))<< 8)|(((uint64_t)LOAD8(ptr,0))    ))
#endif


#define STORE8(p, o, i)      ((uint8_t *) (p))[o] = (uint8_t) ((i) & 0xFF)

#if defined(CONFIG_UNALIGNED_ACCESS) && defined(IS_BIG_ENDIAN)

#define STORE16BE(p, i)      *((uint16_t*)(p))=i
#define STORE32BE(p, i)      *((uint32_t*)(p))=i
#define STORE64BE(p, i)      *((uint64_t*)(p))=i
#else
#define STORE16BE(p, i)      STORE8(p, 1, i), STORE8(p, 0, ((uint16_t)i) >> 8)
#define STORE32BE(p, i)      STORE8(p, 3, i), STORE8(p, 2, ((uint32_t)i) >> 8), STORE8(p, 1, ((uint32_t) i) >> 16), STORE8(p, 0, ((uint32_t) i) >> 24)
#define STORE64BE(p, i)      STORE8(p, 7, i), STORE8(p, 6, ((uint64_t)i) >> 8), STORE8(p, 5, ((uint64_t)i) >> 16), STORE8(p, 4, ((uint64_t)i) >> 24), \
                                 STORE8(p, 3, ((uint64_t)i) >> 32), STORE8(p, 2, ((uint64_t)i) >> 40), STORE8(p, 1, ((uint64_t)i) >> 48), STORE8(p, 0, ((uint64_t)i) >> 56)
#endif

#if defined(CONFIG_UNALIGNED_ACCESS) && defined(IS_LITTLE_ENDIAN)
#define STORE16LE(p, i)      *((uint16_t*)(p))=i
#define STORE32LE(p, i)      *((uint32_t*)(p))=i
#define STORE64LE(p, i)      *((uint64_t*)(p))=i
#else
#define STORE16LE(p, i)      STORE8(p, 0, i), STORE8(p, 1, ((uint16_t)i) >> 8)
#define STORE32LE(p, i)      STORE8(p, 0, i), STORE8(p, 1, ((uint32_t)i) >> 8), STORE8(p, 2, ((uint32_t)i) >> 16), STORE8(p, 3, ((uint32_t)i) >> 24)
#define STORE64LE(p, i)      STORE8(p, 0, i), STORE8(p, 1, ((uint64_t)i) >> 8), STORE8(p, 2, ((uint64_t)i) >> 16), STORE8(p, 3, ((uint64_t)i) >> 24), \
                                 STORE8(p, 4, ((uint64_t)i) >> 32), STORE8(p, 5, ((uint64_t)i) >> 40), STORE8(p, 6, ((uint64_t)i) >> 48), STORE8(p, 7, ((uint64_t)i) >> 56)
#endif

#ifdef IS_BIG_ENDIAN
#define LOAD16(ptr) LOAD16BE(ptr)
#define LOAD32(ptr) LOAD32BE(ptr)
#define LOAD64(ptr) LOAD64BE(ptr)
#define LOAD16SW(ptr) LOAD16LE(ptr)
#define LOAD32SW(ptr) LOAD32LE(ptr)
#define LOAD64SW(ptr) LOAD64LE(ptr)
#else
#define LOAD16(ptr) LOAD16LE(ptr)
#define LOAD32(ptr) LOAD32LE(ptr)
#define LOAD64(ptr) LOAD64LE(ptr)
#define LOAD16SW(ptr) LOAD16BE(ptr)
#define LOAD32SW(ptr) LOAD32BE(ptr)
#define LOAD64SW(ptr) LOAD64BE(ptr)
#endif

#if defined(__GNUC__) && defined(MIPS)
#undef LOAD32
static INLINE uint32_t LOAD32(const void* ptr) { uint32_t v; asm ("ulw %0,0(%1)\n" : "=&r" (v) : "r" (ptr)); return v; }
#endif

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

static INLINE int Fix16Mul(int a,int b)
{
    return (int)(((int64_t)(a<<8)*(int64_t)(b<<8))>>32);
}

static INLINE uint32_t _ones(uint32_t x)
{
    x -= ((x >> 1) & 0x55555555);
    x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
    x = (((x >> 4) + x) & 0x0f0f0f0f);
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x0000003f;
}

static INLINE uint32_t _floor_log2(uint32_t x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return _ones(x) - 1;
}

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
