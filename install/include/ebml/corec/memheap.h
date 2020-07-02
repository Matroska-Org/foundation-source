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

#ifndef __MEMHEAP_H
#define __MEMHEAP_H

#define MEMHEAP_OPTIONAL        0x0001

typedef struct cc_memheap cc_memheap;

typedef void* (*memheap_alloc)(const void* This,size_t,int Flags);
typedef void (*memheap_free)(const void* This,void*,size_t);
typedef void* (*memheap_realloc)(const void* This,void*,size_t Old,size_t New);
typedef void (*memheap_write)(const void* This,void*,const void* Src,size_t Pos,size_t Size);

#define DATA_FLAG_HEAP              (((size_t)1)<<(sizeof(size_t)*8-1))
#define DATA_FLAG_MEMHEAP           (((size_t)1)<<(sizeof(size_t)*8-2))

typedef struct dataheaphead
{
    const cc_memheap* Heap;
    size_t Size;

} dataheaphead;

struct cc_memheap
{
    memheap_alloc Alloc;
    memheap_free Free;
    memheap_realloc ReAlloc;
    memheap_write Write;
    dataheaphead Null;
};

#define MemHeap_Alloc(p,a,b)       ((cc_memheap*)(p))->Alloc(p,a,b)
#define MemHeap_Free(p,a,b)        ((cc_memheap*)(p))->Free(p,a,b)
#define MemHeap_ReAlloc(p,a,b,c)   ((cc_memheap*)(p))->ReAlloc(p,a,b,c)
#define MemHeap_Write(p,a,b,c,d)   ((cc_memheap*)(p))->Write(p,a,b,c,d)
#define MemHeap_Null(p)            ((uint8_t*)(&((cc_memheap*)(p))->Null+1))

#define MEMHEAP_DEFAULT \
static void* __HAlloc(const void* UNUSED_PARAM(p),size_t Size,int UNUSED_PARAM(Flags)) { return malloc(Size); }\
static void __HFree(const void* UNUSED_PARAM(p),void* Ptr,size_t UNUSED_PARAM(Size)) { free(Ptr); }\
static void* __HReAlloc(const void* UNUSED_PARAM(p),void* Ptr,size_t UNUSED_PARAM(OldSize),size_t Size) { return realloc(Ptr,Size); }\
static void __HWrite(const void* UNUSED_PARAM(p),void* Ptr,const void* Src,size_t Pos,size_t Size) { memcpy((uint8_t*)Ptr+Pos,Src,Size); }\
static const cc_memheap MemHeap_Default = { __HAlloc,__HFree,__HReAlloc,__HWrite,{ &MemHeap_Default, DATA_FLAG_MEMHEAP } };

#endif
