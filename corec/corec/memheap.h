/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __MEMHEAP_H
#define __MEMHEAP_H

#include <stdalign.h>

#if defined(_MSC_VER) && !defined(__clang__)
// MSVC lacks max_align_t in C11 mode, use the value from the C++ mode
#define max_align_t  double
#endif

typedef struct cc_memheap cc_memheap;

typedef void* (*memheap_alloc)(const void* This,size_t);
typedef void (*memheap_free)(const void* This,void*,size_t);
typedef void* (*memheap_realloc)(const void* This,void*,size_t Old,size_t New);
typedef void (*memheap_write)(const void* This,void*,const void* Src,size_t Pos,size_t Size);

#define ARRAY_POINTER_HOLDER \
        size_t Size; \
        alignas(max_align_t) char data[]

// we can't use a type with a flexible array inside another structure, so we
// use the same define everywhere
#define MEMHEAD_POINTER_HOLDER \
        const cc_memheap* Heap; \
        ARRAY_POINTER_HOLDER

struct cc_memheap
{
    memheap_alloc Alloc;
    memheap_free Free;
    memheap_realloc ReAlloc;
    memheap_write Write;

    MEMHEAD_POINTER_HOLDER;
};

const cc_memheap *MemHeap_GetDefault(void);

static INLINE void *MemHeap_Alloc(const cc_memheap *p, size_t s)
{
    return p->Alloc(p, s);
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

#endif
