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

#ifndef __ARRAY_H
#define __ARRAY_H

#include "corec/corec.h"
#include "corec/memheap.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARRAY_EXPORTS)
#define ARRAY_DLL DLLEXPORT
#elif defined(ARRAY_IMPORTS)
#define ARRAY_DLL DLLIMPORT
#else
#define ARRAY_DLL
#endif

#define Data_Var(type,name,len)  struct { size_t n; type p[len]; } __##name; type* name = (__##name.n = sizeof(__##name.p),__##name.p)
ARRAY_DLL bool_t Data_ReAlloc(uint8_t** a,size_t n);
ARRAY_DLL size_t Data_Size(const uint8_t* a);
ARRAY_DLL void Data_Release(uint8_t** a);
ARRAY_DLL void Data_Clear(uint8_t** a); // release memory, but keep heap reference
ARRAY_DLL bool_t Data_Set(uint8_t** a,const uint8_t* b,size_t pos,size_t len);

typedef struct array
{
	// these are private members, use ARRAY macros to access them
	uint8_t* _Begin;
	uint8_t* _End;

} array;

typedef	int (*arraycmp)(const void* Param, const void* a,const void* b);

#define ARRAY_AUTO_COUNT    ((size_t)-1)

static INLINE void ArrayInit(array* p) { p->_Begin = NULL; p->_End = NULL; }
ARRAY_DLL void ArrayInitEx(array*,const cc_memheap*);
ARRAY_DLL void ArrayClear(array*);
ARRAY_DLL void ArrayDrop(array*);
ARRAY_DLL size_t ArraySize(const array*);
ARRAY_DLL bool_t ArrayEq(const array* a, const array* b);
ARRAY_DLL bool_t ArrayCopy(array*,const array* In);
ARRAY_DLL bool_t ArrayResize(array*,size_t Size,size_t Align);
ARRAY_DLL void ArrayZero(array*);
ARRAY_DLL intptr_t ArrayFindEx(const array* p, size_t Count, size_t Width, const void* Data, arraycmp Cmp, const void* CmpParam, bool_t* Found);
ARRAY_DLL bool_t ArrayAlloc(array* p,size_t Total,size_t Align);
ARRAY_DLL bool_t ArrayAppend(array* p, const void* Ptr, size_t Length, size_t Align);
ARRAY_DLL bool_t ArrayAppendStr(array* p, const tchar_t* Ptr, bool_t Merge, size_t Align);
ARRAY_DLL void ArrayShrink(array* p, size_t Length);
ARRAY_DLL intptr_t ArrayAddEx(array* p, size_t Count, size_t Width, const void* Data, arraycmp Cmp, const void* CmpParam, size_t Align);
ARRAY_DLL bool_t ArrayRemoveEx(array* p, size_t Count, size_t Width, const void* Data, arraycmp Cmp, const void* CmpParam);
ARRAY_DLL void ArraySortEx(array* p, size_t Count, size_t Width, arraycmp Cmp, const void* CmpParam, bool_t Unique);
ARRAY_DLL bool_t ArrayInsert(array* p, size_t Ofs, const void* Ptr, size_t Length, size_t Align);
ARRAY_DLL void ArrayDelete(array* p, size_t Ofs,  size_t Length);
ARRAY_DLL void ArrayRandomize(array* Array,size_t Width,uint32_t RndSeed);

#define ArrayAdd(p,type,Data,Cmp,CmpParam,Align)  ArrayAddEx(p,ARRAYCOUNT(*p,type),sizeof(type),Data,Cmp,CmpParam,Align)
#define ArrayRemove(p,type,Data,Cmp,CmpParam)     ArrayRemoveEx(p,ARRAYCOUNT(*p,type),sizeof(type),Data,Cmp,CmpParam)
#define ArrayFind(p,type,Data,Cmp,CmpParam,Found) ArrayFindEx(p,ARRAYCOUNT(*p,type),sizeof(type),Data,Cmp,CmpParam,Found)
#define ArraySort(p,type,Cmp,CmpParam,Unique)     ArraySortEx(p,ARRAYCOUNT(*p,type),sizeof(type),Cmp,CmpParam,Unique)

#ifdef CONFIG_DEBUGCHECKS
#define ARRAYBEGIN(Array,Type)		(assert(&(Array)!=NULL),(Type*)((Array)._Begin))
#define ARRAYEND(Array,Type)		(assert(&(Array)!=NULL),(Type*)((Array)._End))
#define ARRAYEMPTY(Array)			(assert(&(Array)!=NULL),(Array)._Begin==(Array)._End)
#else
#define ARRAYBEGIN(Array,Type)		((Type*)((Array)._Begin))
#define ARRAYEND(Array,Type)		((Type*)((Array)._End))
#define ARRAYEMPTY(Array)			((Array)._Begin==(Array)._End)
#endif
#define ARRAYCOUNT(Array,Type)		((size_t)(ARRAYEND(Array,Type)-ARRAYBEGIN(Array,Type)))

// TODO: move this to base/mem and depend on "mem" platform dependently(?)
typedef struct block
{
	const uint8_t* Ptr;
	uintptr_t Id;

} block;
//end TODO

#ifdef TARGET_PALMOS

typedef struct arrayblock
{
	array Array;
	block Block;

} arrayblock;

ARRAY_DLL void ArrayBlockClear(arrayblock*);
ARRAY_DLL void ArrayBlockLock(arrayblock*);

#else

typedef struct arrayblock
{
	array Array;

} arrayblock;

#define ArrayBlockClear(a) ArrayClear(&(a)->Array)
#define ArrayBlockLock(a) {}

#endif

#define SAFETAIL	256

typedef struct cc_fifo
{
	// private members
	array _Base;
	uint8_t* _Read;

} cc_fifo;

static INLINE void Fifo_Init(cc_fifo* p) { ArrayInit(&p->_Base); p->_Read = NULL; }
ARRAY_DLL void Fifo_Clear(cc_fifo*);
ARRAY_DLL void Fifo_Drop(cc_fifo*);
ARRAY_DLL bool_t Fifo_Alloc(cc_fifo* p, size_t Size, size_t Align);
ARRAY_DLL uint8_t* Fifo_Write(cc_fifo*, const void* Ptr, size_t Length, size_t Align);

static INLINE void Fifo_Readed(cc_fifo* p, size_t Length) 
{ 
    p->_Read += Length; 
}

#define FIFO_SIZE(p)  (ARRAYEND((p)._Base,uint8_t)-(p)._Read)
#define FIFO_BEGIN(p) ((p)._Read)
#define FIFO_END(p)   ARRAYEND((p)._Base,uint8_t)

#ifdef __cplusplus
}
#endif

#endif
