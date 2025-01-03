/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
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

extern const cc_memheap *MemHeap_Default;

static INLINE void *MemHeap_Alloc(const cc_memheap *p, size_t s, int Flags)
{
    return p->Alloc(p, s, Flags);
}
static INLINE void MemHeap_Free(const cc_memheap *p, void *ptr, size_t s)
{
    p->Free(p, ptr, s);
}
static INLINE void *MemHeap_ReAlloc(const cc_memheap *p, void *ptr, size_t old, size_t new)
{
    return p->ReAlloc(p, ptr, old, new);
}
static INLINE void MemHeap_Write(const cc_memheap *p, void *ptr, void *src, size_t pos, size_t size)
{
     p->Write(p, ptr, src, pos, size);
}
static INLINE uint8_t *MemHeap_Null(const cc_memheap *p)
{
     return (uint8_t*)(&(p)->Null+1);
}

#endif
