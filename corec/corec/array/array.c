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

#include "array.h"

typedef struct datahead
{
    size_t Size;

} datahead;

#define Data_Head(Name)         ((datahead*)(Name)-1)
#define Data_HeapHead(Name)     ((dataheaphead*)(Name)-1)
#define Data_IsHeap(Name)       (Data_Head(Name)->Size & DATA_FLAG_HEAP)
#define Data_IsMemHeap(Name)    (Data_Head(Name)->Size & DATA_FLAG_MEMHEAP)
#define Data_GetSize(Name)      (Data_Head(Name)->Size & ~(DATA_FLAG_HEAP|DATA_FLAG_MEMHEAP))

size_t Data_Size(const uint8_t* a)
{
    if (!a) return 0;
    return Data_GetSize(a);
}

NOINLINE bool_t Data_ReAlloc(uint8_t** a,size_t n)
{
    uint8_t* p = *a;
    size_t oldsize;

    if (p)
    {
        if (!Data_Head(p)->Size) // const?
            return 0;
        oldsize = Data_GetSize(p);
    }
    else
        oldsize = 0;

    if (oldsize<n)
    {
        if (p && Data_IsMemHeap(p))
        {
            const cc_memheap* Heap = Data_HeapHead(p)->Heap;
            dataheaphead* Head;
            if (!oldsize)
                Head = MemHeap_Alloc(Heap,n+sizeof(dataheaphead),0);
            else
                Head = MemHeap_ReAlloc(Heap,Data_HeapHead(p),oldsize+sizeof(dataheaphead),n+sizeof(dataheaphead));
            if (!Head)
                return 0;

            Head->Heap = Heap;
            Head->Size = n|DATA_FLAG_HEAP|DATA_FLAG_MEMHEAP;
            *a = (uint8_t*)(Head+1);
        }
        else
        {
            datahead* Head;
            if (!p || !Data_IsHeap(p))
            {
                uint8_t* old = p;
                Head = malloc(n+sizeof(datahead));
                if (Head && old)
                    memcpy(Head+1,old,oldsize);
            }
            else
                Head = realloc(Data_Head(p),n+sizeof(datahead));

            if (!Head)
                return 0;

            Head->Size = n|DATA_FLAG_HEAP;
            *a = (uint8_t*)(Head+1);
        }
    }
    return 1;
}

NOINLINE void Data_Release(uint8_t** a)
{
    uint8_t* p = *a;
    if (p)
    {
        *a = NULL;
        if (Data_IsHeap(p))
        {
            if (Data_IsMemHeap(p))
            {
                if (Data_GetSize(p))
                    MemHeap_Free(Data_HeapHead(p)->Heap,Data_HeapHead(p),Data_GetSize(p)+sizeof(dataheaphead));
            }
            else
                free(Data_Head(p));
        }
    }
}

NOINLINE void Data_Clear(uint8_t** a)
{
    uint8_t* p = *a;
    if (p && Data_IsMemHeap(p))
    {
        p = MemHeap_Null(Data_HeapHead(p)->Heap);
        Data_Release(a);
        *a = p;
    }
    else
        Data_Release(a);
}

bool_t Data_Set(uint8_t** a,const uint8_t* b,size_t pos,size_t len)
{
    if (!Data_ReAlloc(a,pos+len))
        return 0;

    memcpy(*a+pos,b,len);
    return 1;
}

size_t ArraySize(const array*p)
{
    return p->_End-p->_Begin;
}

void ArrayInitEx(array* p,const cc_memheap* Heap)
{
    p->_Begin = p->_End = Heap ? MemHeap_Null(Heap):NULL;
}

NOINLINE void ArrayClear(array* p)
{
	Data_Clear(&p->_Begin);
	p->_End = p->_Begin;
}

void ArrayDrop(array* p)
{
	p->_End = p->_Begin;
}

static size_t SizeAlign(size_t Total, size_t Align)
{
    if (!Align)
    {
        for (Align=16;Align<16384;Align<<=1)
            if (Align*8 > Total)
                break;
    }
    --Align;
	return (Total + Align) & ~Align;
}

bool_t ArrayAlloc(array* p,size_t Total,size_t Align)
{
    size_t Size = ArraySize(p);
    if (!Data_ReAlloc(&p->_Begin,SizeAlign(Total,Align)))
        return 0;
	p->_End = p->_Begin + Size;
	return 1;
}

void ArrayShrink(array* p, size_t Length)
{
    p->_End -= Length;
    if (p->_End < p->_Begin)
        p->_End = p->_Begin;
}

bool_t ArrayInsert(array* p, size_t Ofs, const void* Ptr, size_t Width, size_t Align)
{
	if (!ArrayAppend(p,NULL,Width,Align))
		return 0;
	memmove(p->_Begin+Ofs+Width,p->_Begin+Ofs,(p->_End-p->_Begin)-Width-Ofs);
    if (Ptr)
        memcpy(p->_Begin+Ofs,Ptr,Width);
    return 1;
}

void ArrayDelete(array* p, size_t Ofs, size_t Width)
{
	memmove(p->_Begin+Ofs,p->_Begin+Ofs+Width,(p->_End-p->_Begin)-Width-Ofs);
	p->_End -= Width;
}

bool_t ArrayAppendStr(array* p, const tchar_t* Ptr, bool_t Merge, size_t Align)
{
    if (Ptr && (Ptr[0] || !Merge))
    {
        if (Merge && !ARRAYEMPTY(*p) && ARRAYEND(*p,tchar_t)[-1]==0)
            ArrayShrink(p,sizeof(tchar_t));

        return ArrayAppend(p,Ptr,(tcslen(Ptr)+1)*sizeof(tchar_t),Align);
    }
    return 1;
}

bool_t ArrayAppend(array* p, const void* Ptr, size_t Length, size_t Align)
{
	size_t Total = p->_End - p->_Begin + Length;
	if (Total > Data_Size(p->_Begin) && !ArrayAlloc(p,Total,Align))
		return 0;
	if (Ptr)
		memcpy(p->_End,Ptr,Length);
	p->_End += Length;
	return 1;
}

bool_t ArrayEq(const array* a, const array* b)
{
    size_t an = a ? ArraySize(a):0;
    size_t bn = b ? ArraySize(b):0;
    return an == bn && (!an || memcmp(ARRAYBEGIN(*a,uint8_t),ARRAYBEGIN(*b,uint8_t),an)==0);
}

bool_t ArrayCopy(array* p, const array* q)
{
    size_t Size = ArraySize(q);
    if (!ArrayResize(p,Size,0))
        return 0;
    memcpy(ARRAYBEGIN(*p,uint8_t),ARRAYBEGIN(*q,uint8_t),Size);
    return 1;
}

bool_t ArrayResize(array* p,size_t Total, size_t Align)
{
    if (Total > Data_Size(p->_Begin) && !ArrayAlloc(p,Total,Align))
        return 0;
    p->_End = p->_Begin + Total;
    return 1;
}

void ArrayZero(array* p)
{
    memset(p->_Begin,0,p->_End-p->_Begin);
}

#define QSORTMINLEN 16

static INLINE void InQSortSwap(uint_fast32_t* a, uint_fast32_t* b)
{
    uint_fast32_t t = *a;
    *a = *b;
    *b = t;
}

static NOINLINE void InQSort(uint_fast32_t* First, uint_fast32_t* Last, arraycmp Cmp, const void* CmpParam)
{
	while (Last > First + QSORTMINLEN)
	{	
        uint_fast32_t* Mid = First + ((Last - First)>>1);
        uint_fast32_t* Ref = First;
        uint_fast32_t* Left;
        uint_fast32_t* Right;

		if (Cmp(CmpParam,First,Last) < 0) 
		{
			if (Cmp(CmpParam,Last,Mid) < 0)
				Ref = Last;
			else 
			if (Cmp(CmpParam,First,Mid) < 0)
				Ref = Mid;
		}
		else 
		if (Cmp(CmpParam,First,Mid) >= 0) 
		{
			if (Cmp(CmpParam,Last,Mid) < 0)
				Ref = Mid;
			else
				Ref = Last;
		}

		if (Ref != First)
            InQSortSwap(First,Ref);

		Left = First;
		Right = Last+1;

		for (;;)
		{ 
			while (++Left < Last && Cmp(CmpParam,First,Left) > 0) {}

			while (Cmp(CmpParam,First,--Right) < 0) {}

			if (Left >= Right)
				break;

			InQSortSwap(Left,Right);
		}

		if (Right == First)
        {
			++First;
        }
		else 
		{
			InQSortSwap(First,Right);

			--Right;

			if (Right - First < Last - Left) 
			{
				if (Right > QSORTMINLEN + First)
					InQSort(First,Right,Cmp,CmpParam);
				First = Left;
			}
			else 
            {
				if (Last > QSORTMINLEN + Left)
					InQSort(Left,Last,Cmp,CmpParam);
				Last = Right;
			}
		}
	}
}

void ArraySortEx(array* p, size_t Count, size_t Width, arraycmp Cmp, const void* CmpParam, bool_t Unique)
{
    if (Count == ARRAY_AUTO_COUNT)
        Count = ArraySize(p)/Width;

    if (Count>1)
    {
        if (Width == sizeof(uint_fast32_t))
        {
            uint_fast32_t* End = ARRAYBEGIN(*p,uint_fast32_t)+Count;
            uint_fast32_t* i;
            uint_fast32_t* j;

		    InQSort(ARRAYBEGIN(*p,uint_fast32_t), End-1, Cmp, CmpParam);

            j = ARRAYBEGIN(*p,uint_fast32_t);
		    for (i=j+1; i!=End; ++i)
		    {
                if (Cmp(CmpParam,i,j) < 0)
                {
                    uint_fast32_t Tmp = *i;
                    do
                    {
                        j[1] = j[0];
                        if (j-- == ARRAYBEGIN(*p,uint_fast32_t))
                            break;
                    }
                    while (Cmp(CmpParam,&Tmp,j) < 0);
                    j[1] = Tmp;
                }
                j = i;
		    }

            if (Unique)
            {
		        j = ARRAYBEGIN(*p,uint_fast32_t);
		        for (i=j+1; i!=End; ++i)
		        {
			        if (Cmp(CmpParam,i,j) != 0)
                        *(++j) = *i;
		        }
		        p->_End = (uint8_t*)(j+1);
	        }
        }
        else
        {
            // dummy fallback...

            uint8_t* Tmp = (uint8_t*)alloca(Width);
            uint8_t* End = p->_Begin + Count*Width;
            uint8_t* i;
            uint8_t* j;

            j = p->_Begin;
		    for (i=j+Width; i!=End; i+=Width)
		    {
                if (Cmp(CmpParam,i,j) < 0)
                {
                    memcpy(Tmp,i,Width);
                    do
                    {
                        memcpy(j+Width,j,Width);
                        if (j == p->_Begin)
                        {
                            j -= Width;
                            break;
                        }
                        j -= Width;
                    }
                    while (Cmp(CmpParam,Tmp,j) < 0);
                    memcpy(j+Width,Tmp,Width);
                }
                j = i;
		    }

            if (Unique)
            {
		        j = p->_Begin;
		        for (i=j+Width; i!=End; i+=Width)
		        {
			        if (Cmp(CmpParam,i,j) != 0)
                    {
                        j += Width;
                        memcpy(j,i,Width);
                    }
		        }
		        p->_End = j+Width;
	        }
        }

	}
}

intptr_t ArrayFindEx(const array* p, size_t Count, size_t Width, const void* Data, arraycmp Cmp, const void* CmpParam, bool_t* Found)
{
    if (ARRAYEMPTY(*p))
    {
        *Found = 0;
        return 0;
    }

    if (Count == ARRAY_AUTO_COUNT)
    {
        Count = ArraySize(p)/Width;
        assert(Count*Width == ArraySize(p));
    }

	if (Cmp)
	{
		int i;
		intptr_t Mid = 0;
		intptr_t Lower = 0;
		intptr_t Upper = Count-1;

		while (Upper >= Lower) 
		{
			Mid = (Upper + Lower) >> 1;

			i = Cmp(CmpParam,p->_Begin+Width*Mid,Data);
			if (i>0)
				Upper = Mid-1;	
			else if (i<0)  		
				Lower = Mid+1;	
			else 
			{			        
				*Found = 1;
				return Mid;
			}
		}

		*Found = 0;

		if (Upper == Mid - 1)
			return Mid;		
		else                 
			return Lower;    
	}
	else
	{
		intptr_t No = 0;
		const uint8_t* i;
		for (i=p->_Begin;Count--;i+=Width,++No)
			if (memcmp(i,Data,Width)==0)
			{
				*Found = 1;
				return No;
			}

		*Found = 0;
		return No;
	}
}

intptr_t ArrayAddEx(array* p,size_t Count, size_t Width, const void* Data, arraycmp Cmp, const void* CmpParam, size_t Align)
{
	intptr_t Pos;
	bool_t Found;

	Pos = ArrayFindEx(p,Count,Width,Data,Cmp,CmpParam,&Found);
	if (!Found)
    {
        if (!ArrayInsert(p,Width*Pos,Data,Width,Align))
            return -1;
    }
    else
    	memcpy(p->_Begin+Width*Pos,Data,Width);

	return Pos;
}

bool_t ArrayRemoveEx(array* p, size_t Count, size_t Width, const void* Data, arraycmp Cmp, const void* CmpParam)
{
	bool_t Found;
	size_t Pos = ArrayFindEx(p,Count,Width,Data,Cmp,CmpParam,&Found);
	if (Found)
        ArrayDelete(p,Pos*Width,Width);
	return Found;
}

static INLINE uint32_t Rand(uint32_t RndSeed)
{ 
	return RndSeed*0x8088405U + 0x251001U;
}

void ArrayRandomize(array* Array,size_t Width,uint32_t RndSeed)
{
    size_t i,j,Count = ArraySize(Array)/Width;
    uint8_t *Buf=alloca(Width);
    for (i=0;i<Count;++i)
    {
        RndSeed = Rand(RndSeed);
        j = RndSeed % Count;
        memcpy(Buf,ARRAYBEGIN(*Array,uint8_t)+i*Width,Width);
        memcpy(ARRAYBEGIN(*Array,uint8_t)+i*Width,ARRAYBEGIN(*Array,uint8_t)+j*Width,Width);
        memcpy(ARRAYBEGIN(*Array,uint8_t)+j*Width,Buf,Width);
    }
}


#ifdef TARGET_PALMOS

// TODO: move this to base/mem and depend on "mem" platform dependently(?)
#include "common.h"
// end TODO

void ArrayBlockClear(arrayblock* p)
{
	if (p->Block.Ptr)
    {
		FreeBlock(NULL,&p->Block);
        p->Array._Begin = NULL;
        p->Array._End = NULL;
    }
    else
        ArrayClear(&p->Array);
}

void ArrayBlockLock(arrayblock* p)
{
	if (!p->Block.Ptr && p->Array._End != p->Array._Begin)
	{
		size_t n = p->Array._End-p->Array._Begin;
		if (AllocBlock(NULL,n,&p->Block,1,HEAP_STORAGE))
		{
			WriteBlock(&p->Block,0,p->Array._Begin,n);
			ArrayClear(&p->Array);
			p->Array._Begin = (uint8_t*)p->Block.Ptr;
			p->Array._End = p->Array._Begin + n;
		}
	}
}

#endif

void Fifo_Clear(cc_fifo* p)
{
	ArrayClear(&p->_Base);
    p->_Read = NULL;
}

bool_t Fifo_Alloc(cc_fifo* p, size_t Total, size_t Align)
{
    size_t n = p->_Read - p->_Base._Begin;
    if (!ArrayAlloc(&p->_Base,Total,Align))
        return 0;
    p->_Read = p->_Base._Begin+n;
    return 1;
}

void Fifo_Drop(cc_fifo* p)
{
	ArrayDrop(&p->_Base);
	p->_Read = p->_Base._Begin;
}

uint8_t* Fifo_Write(cc_fifo* p, const void* Ptr, size_t Length, size_t Align)
{
    size_t Total = Data_Size(p->_Base._Begin);
    size_t Read = p->_Read - p->_Base._Begin;
    size_t End = p->_Base._End - p->_Base._Begin + Length + SAFETAIL;
    uint8_t* Result;

    if (End>Total && Read>0)
    {
        memmove(p->_Base._Begin,p->_Read,FIFO_SIZE(*p));
        p->_Read = p->_Base._Begin;
        p->_Base._End -= Read;
        End -= Read;
        Read = 0;
    }

    if (End>Total)
    {
        if (!ArrayAlloc(&p->_Base,End,Align))
            return NULL;
        p->_Read = p->_Base._Begin+Read;
    }

    Result = p->_Base._End;
    p->_Base._End += Length;

    if (Ptr)
        memcpy(Result,Ptr,Length);
    return Result;
}
