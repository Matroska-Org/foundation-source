/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __MEMALLOC_H
#define __MEMALLOC_H

typedef struct cc_memalloc
{
    void* (*Alloc)(const void* This,size_t);
    void (*Free)(const void* This,void*);
    void* (*ReAlloc)(const void* This,void*,size_t);

} cc_memalloc;

#define MemAlloc_Alloc(p,a)         ((cc_memalloc*)(p))->Alloc(p,a)
#define MemAlloc_Free(p,a)          ((cc_memalloc*)(p))->Free(p,a)
#define MemAlloc_ReAlloc(p,a,b)     ((cc_memalloc*)(p))->ReAlloc(p,a,b)

#define MEMALLOC_DEFAULT \
static void* __Alloc(const void* UNUSED_PARAM(p),size_t Size) { return malloc(Size); }\
static void __Free(const void* UNUSED_PARAM(p),void* Ptr) { free(Ptr); }\
static void* __ReAlloc(const void* UNUSED_PARAM(p),void* Ptr,size_t Size) { return realloc(Ptr,Size); }\
static const cc_memalloc MemAlloc_Default = { __Alloc,__Free,__ReAlloc };

#endif
