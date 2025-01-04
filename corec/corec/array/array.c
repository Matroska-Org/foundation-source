/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include <stdlib.h>
#include "array.h"
#include <corec/memheap.h>

#define DATA_FLAG_HEAP              (((size_t)1)<<(sizeof(size_t)*8-2))
#define DATA_FLAG_MEMHEAP           (((size_t)1)<<(sizeof(size_t)*8-1))

typedef struct
{
    ARRAY_POINTER_HOLDER;
} datahead;

typedef struct
{
    MEMHEAD_POINTER_HOLDER;
} dataheaphead;

#define container_of(ptr, type, member) \
    ((type *)((uintptr_t)ptr - offsetof(type, member)))

static INLINE dataheaphead* Data_HeapHead(const array *p)
{
    assert(p->_Begin);
    return container_of(p->_Begin, dataheaphead, data);
}

static INLINE datahead* Data_Head(const array *p)
{
    assert(p->_Begin);
    return container_of(p->_Begin, datahead, data);
}
static INLINE bool_t Data_IsHeap(const datahead* Name)
{
    return (Name->Size & DATA_FLAG_HEAP) != 0;
}
static INLINE bool_t Data_IsMemHeap(const datahead* Name)
{
    return (Name->Size & DATA_FLAG_MEMHEAP) != 0;
}
static INLINE size_t Data_GetSize(const datahead* Name)
{
    return Name->Size & ~(DATA_FLAG_HEAP|DATA_FLAG_MEMHEAP);
}

static size_t Data_Size(const array* a)
{
    if (!a->_Begin) return 0;
    const datahead *hp = Data_Head(a);
    return Data_GetSize(hp);
}

static NOINLINE bool_t Data_ReAlloc(array *a,size_t n)
{
    if (!a->_Begin)
    {
        datahead *Head = malloc(n+sizeof(datahead));
        if (!Head)
            return 0;

        Head->Size = n|DATA_FLAG_HEAP;
        a->_Begin = &Head->data;
        return 1;
    }

    datahead *hp = Data_Head(a);
    if (!hp->Size) // const?
        return 0;

    size_t oldsize = Data_GetSize(hp);
    if (oldsize<n)
    {
        if (Data_IsMemHeap(hp))
        {
            dataheaphead *dp = Data_HeapHead(a);
            const cc_memheap* Heap = dp->Heap;
            dataheaphead* Head;
            if (!oldsize)
                Head = MemHeap_Alloc(Heap,n+sizeof(dataheaphead));
            else
                Head = MemHeap_ReAlloc(Heap,dp,oldsize+sizeof(dataheaphead),n+sizeof(dataheaphead));
            if (!Head)
                return 0;

            Head->Heap = Heap;
            Head->Size = n|DATA_FLAG_HEAP|DATA_FLAG_MEMHEAP;
            a->_Begin = Head->data;
        }
        else
        {
            datahead* Head;
            if (Data_IsHeap(hp))
            {
                Head = realloc(hp,n+sizeof(datahead));
            }
            else
            {
                Head = malloc(n+sizeof(datahead));
                if (Head)
                    memcpy(&Head->data,a->_Begin,oldsize);
            }

            if (!Head)
                return 0;

            Head->Size = n|DATA_FLAG_HEAP;
            a->_Begin = Head->data;
        }
    }
    return 1;
}

static size_t ArraySize(const array*p)
{
    return p->_Used;
}

void ArrayInitEx(array* p,const cc_memheap* Heap)
{
    if (Heap == NULL)
        p->_Begin = NULL;
    else
        p->_Begin = (void*)Heap->data;
    p->_Used = 0;
}

NOINLINE void ArrayClear(array* a)
{
    if (!a->_Begin)
        return;
    datahead *hp = Data_Head(a);
    if (Data_IsMemHeap(hp))
    {
        dataheaphead *dp = Data_HeapHead(a);
        const struct cc_memheap* Heap = dp->Heap;
        if (Data_GetSize(hp))
            MemHeap_Free(Heap,dp,Data_GetSize(hp)+sizeof(dataheaphead));
        ArrayInitEx(a, Heap);
    }
    else if (Data_IsHeap(hp))
    {
        free(hp);
        a->_Begin = NULL;
        a->_Used = 0;
    }
    else
    {
        a->_Begin = NULL;
        a->_Used = 0;
    }
}

void ArrayDrop(array* p)
{
    p->_Used = 0;
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
    if (!Data_ReAlloc(p,SizeAlign(Total,Align)))
        return 0;
    p->_Used = Size;
    return 1;
}

void ArrayShrink(array* p, size_t Length)
{
    if (p->_Used > Length)
        p->_Used -= Length;
    else
        p->_Used = 0;
}

bool_t ArrayInsert(array* p, size_t Ofs, const void* Ptr, size_t Width, size_t Align)
{
    if (!ArrayAppend(p,NULL,Width,Align))
        return 0;
    memmove(ARRAYBEGIN(*p,uint8_t)+Ofs+Width, ARRAYBEGIN(*p,uint8_t)+Ofs, ArraySize(p)-Width-Ofs);
    if (Ptr)
        memcpy(ARRAYBEGIN(*p,uint8_t)+Ofs, Ptr, Width);
    return 1;
}

void ArrayDelete(array* p, size_t Ofs, size_t Width)
{
    memmove(ARRAYBEGIN(*p,uint8_t)+Ofs, ARRAYBEGIN(*p,uint8_t)+Ofs+Width, ArraySize(p)-Width-Ofs);
    p->_Used -= Width;
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
    size_t Total = ArraySize(p) + Length;
    if (Total > Data_Size(p) && !ArrayAlloc(p,Total,Align))
        return 0;
    if (Ptr)
        memcpy((uint8_t*)p->_Begin + p->_Used,Ptr,Length);
    p->_Used += Length;
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
    if (Total > Data_Size(p) && !ArrayAlloc(p,Total,Align))
        return 0;
    p->_Used = Total;
    return 1;
}

void ArrayZero(array* p)
{
    memset(p->_Begin,0,ArraySize(p));
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

static void SlowSort(array* p, size_t Count, size_t Width, arraycmp Cmp, const void* CmpParam, bool_t Unique)
{
#if defined(_MSC_VER) && !defined (__clang__)
    uint8_t* Tmp = (uint8_t*)_alloca(Width);
#else
    uint8_t Tmp[Width];
#endif
    uint8_t* End = ARRAYBEGIN(*p,uint8_t) + Count*Width;
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
        p->_Used = j - (uint8_t*)p->_Begin + Width;
    }
}

void ArraySortEx(array* p, size_t Count, size_t Width, arraycmp Cmp, const void* CmpParam, bool_t Unique)
{
    if (Count == ARRAY_AUTO_COUNT)
        Count = ArraySize(p)/Width;

    if (Count<=1)
        return;

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
            p->_Used = ((j+1)-ARRAYBEGIN(*p,uint_fast32_t)) * sizeof(uint_fast32_t);
        }
        return;
    }

    SlowSort(p, Count, Width, Cmp, CmpParam, Unique);
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

            i = Cmp(CmpParam, ARRAYBEGIN(*p,uint8_t) + Width*Mid, Data);
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
        memcpy(ARRAYBEGIN(*p,uint8_t) + Width*Pos, Data, Width);

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

void Fifo_Clear(cc_fifo* p)
{
    ArrayClear(&p->_Base);
    p->_Read = 0;
}

bool_t Fifo_Alloc(cc_fifo* p, size_t Total, size_t Align)
{
    size_t n = p->_Read;
    if (!ArrayAlloc(&p->_Base,Total,Align))
        return 0;
    p->_Read = n;
    return 1;
}

void Fifo_Drop(cc_fifo* p)
{
    ArrayDrop(&p->_Base);
    p->_Read = 0;
}

uint8_t* Fifo_Write(cc_fifo* p, const void* Ptr, size_t Length, size_t Align)
{
    size_t Total = Data_Size(&p->_Base);
    size_t Read = p->_Read;
    size_t End = ArraySize(&p->_Base) + Length + SAFETAIL;
    uint8_t* Result;

    if (End>Total && Read>0)
    {
        memmove(p->_Base._Begin, FIFO_BEGIN(*p), FIFO_SIZE(*p));
        p->_Read = 0;
        p->_Base._Used -= Read;
        End -= Read;
        Read = 0;
    }

    if (End>Total)
    {
        if (!ArrayAlloc(&p->_Base,End,Align))
            return NULL;
        p->_Read = Read;
    }

    Result = FIFO_END(*p);
    p->_Base._Used += Length;

    if (Ptr)
        memcpy(Result,Ptr,Length);
    return Result;
}

static void* __HAlloc(const void* UNUSED_PARAM(p),size_t Size) { return malloc(Size); }\
static void __HFree(const void* UNUSED_PARAM(p),void* Ptr,size_t UNUSED_PARAM(Size)) { free(Ptr); }\
static void* __HReAlloc(const void* UNUSED_PARAM(p),void* Ptr,size_t UNUSED_PARAM(OldSize),size_t Size) { return realloc(Ptr,Size); }\
static void __HWrite(const void* UNUSED_PARAM(p),void* Ptr,const void* Src,size_t Pos,size_t Size) { memcpy((uint8_t*)Ptr+Pos,Src,Size); }\
static const cc_memheap MemHeap_Default_ = { __HAlloc,__HFree,__HReAlloc,__HWrite, &MemHeap_Default_, DATA_FLAG_MEMHEAP };
const cc_memheap *MemHeap_Default = &MemHeap_Default_;
