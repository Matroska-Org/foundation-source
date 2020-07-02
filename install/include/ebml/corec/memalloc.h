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
