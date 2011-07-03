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

#include "parser.h"

// Don't use dataheap in win32 debug mode. It's easier to track down memory corruptions.
#if defined(NDEBUG) || !defined(TARGET_WIN32)
#define DATAHEAP_LIMIT 72
#else
#define DATAHEAP_LIMIT 1
#endif

#ifdef MIPS64
typedef uint64_t dataunit;
#else
typedef uintptr_t dataunit;
#endif

#define BUFFER_SIZE	  1024      //in dataunit

#define DATAALIGN(n) (((n)+sizeof(dataunit)-1)/sizeof(dataunit))

static INLINE size_t DataSize(dataunit* i) { return (size_t)*i & 65535; }
static INLINE size_t DataNext(dataunit* i) { return (size_t)*i >> 16; }

struct dataheap_free
{
    dataheap_free* Next;
};

typedef struct dataheap_block
{
    dataunit* Data;
    uint16_t Count;
    uint16_t MaxSize;

} dataheap_block;

#if 0
static void DataHeap_Check(dataheap* p)
{
	dataheap_block* i;
    size_t MaxSize=0;

	for (i=ARRAYBEGIN(p->Buffer,dataheap_block);i!=ARRAYEND(p->Buffer,dataheap_block);++i)
    {
        dataunit* a = i->Data;  
        dataunit* b = i->Data+BUFFER_SIZE-1;
        size_t Count=0;

        assert(i->MaxSize>=MaxSize);
        assert(!DataSize(a));
        assert(*b==0);

        while (a!=b)
        {
            if (i->MaxSize)
            {
                assert(DataSize(a)<=i->MaxSize);
                if (DataSize(a)==i->MaxSize)
                    ++Count;
            }
            assert(DataSize(a)<=DataNext(a));
            assert(a+DataNext(a)<=b);
            a += DataNext(a);
        }

        MaxSize = i->MaxSize;
        assert(!i->MaxSize || i->Count==Count);
    }
}
#else
#define DataHeap_Check(p) {}
#endif

static void DataHeap_Write(dataheap* UNUSED_PARAM(p),void* Ptr,const void* Src,size_t Pos,size_t Size)
{
    memcpy((uint8_t*)Ptr+Pos,Src,Size);
}

static void* DataHeap_Alloc(dataheap* p, size_t Size, int UNUSED_PARAM(Flags))
{
    dataheap_block Block;
	dataheap_block* i;
    dataunit* curr;
    dataunit* prev;
    size_t n,Count,MaxSize;

    if (!Size)
        return NULL;

    Size = DATAALIGN(Size);

    if (Size>=DATAHEAP_LIMIT)
        return MemHeap_Alloc(p->Heap,Size*sizeof(dataunit),0);

    LockEnter(p->Lock);

    if (Size==DATAALIGN(3*sizeof(void*)))
    {
        dataheap_free* i;
        if (!p->Free3)
        {
            Block.Count = 0;
            Block.MaxSize = 0;
            Block.Data = MemHeap_Alloc(p->Heap,BUFFER_SIZE*sizeof(dataunit),0);
	        if (!Block.Data)
                goto failed;
            
            if (!ArrayInsert(&p->Buffer,0,&Block,sizeof(Block),256))
                goto failed_array;

            Block.Data[0] = (BUFFER_SIZE-1)<<16; // head delimiter
            Block.Data[BUFFER_SIZE-1] = 0; // tail delimiter

            for (n=1;n<BUFFER_SIZE-3;n+=3)
            {
                i=(dataheap_free*)(Block.Data+n);
                i->Next = p->Free3;
                p->Free3 = i;
            }
        }

        i=p->Free3;
        p->Free3 = i->Next;
        LockLeave(p->Lock);
        return i;
    }

    DataHeap_Check(p);

    if (!ARRAYEMPTY(p->Buffer))
    {
        for (;;)
        {
            i=ARRAYEND(p->Buffer,dataheap_block)-1;
            if (i->MaxSize<Size)
                break;

            MaxSize=i->Count?i->MaxSize:0;
            Count=0;

            curr = i->Data;
            do
            {
                prev = curr;
                curr += DataNext(curr);

                if ((n = DataSize(curr)) >= Size)
                {
                    if (n==i->MaxSize)
                    {
                        assert(i->Count>0);
                        i->Count = (uint16_t)(i->Count-1);
                    }

                    if (n == Size)
                        *prev += DataNext(curr) << 16;
                    else
                    {
                        *prev += Size << 16;
                        curr[Size] = (n-Size) | ((DataNext(curr)-Size)<<16);
                    }

                    LockLeave(p->Lock);
                    return curr;
                }

                if (MaxSize<=n)
                {
                    if (MaxSize==n)
                        ++Count;
                    else
                    {
                        MaxSize=n;
                        Count=1;
                    }
                }
            }
            while (n);

            assert(i->Count==0);

            i->Count = (uint16_t)Count;
            i->MaxSize = (uint16_t)MaxSize;
            while (i!=ARRAYBEGIN(p->Buffer,dataheap_block) && i[-1].MaxSize > i->MaxSize)
            {
                SWAPVAL(dataheap_block,i[-1],i[0]);
                --i;
            }
        }
    }

    Block.Data = MemHeap_Alloc(p->Heap,BUFFER_SIZE*sizeof(dataunit),0);
	if (!Block.Data)
        goto failed;

	if (!ArrayAppend(&p->Buffer,&Block,sizeof(Block),256))
        goto failed_array;

    MaxSize = BUFFER_SIZE-2-Size;

    i = ARRAYEND(p->Buffer,dataheap_block)-1;
    i->Count = 1;
    i->MaxSize = (uint16_t)MaxSize;

    curr = i->Data;
    curr[0] = (1+Size)<<16; // head delimiter
    curr[1+Size] = MaxSize + (MaxSize<<16);
    curr[BUFFER_SIZE-1] = 0; // tail delimiter

    while (i!=ARRAYBEGIN(p->Buffer,dataheap_block) && i[-1].MaxSize > i->MaxSize)
    {
        SWAPVAL(dataheap_block,i[-1],i[0]);
        --i;
    }

    LockLeave(p->Lock);
    return curr+1;

failed_array:
    MemHeap_Free(p->Heap,Block.Data,BUFFER_SIZE*sizeof(dataunit));
failed:
    LockLeave(p->Lock);
    return NULL;
}

static void DataHeap_Free(dataheap* p, void* Ptr, size_t Size)
{
    if (Ptr && Size)
    {
        dataunit *curr = (dataunit*)Ptr;
        dataunit *next;
        dataunit *prev;
	    dataheap_block* i;
        
        Size = DATAALIGN(Size);

        if (Size>=DATAHEAP_LIMIT)
        {
            MemHeap_Free(p->Heap,Ptr,Size*sizeof(dataunit));
            return;
        }

        LockEnter(p->Lock);

        if (Size==3)
        {
            ((dataheap_free*)Ptr)->Next = p->Free3;
            p->Free3 = (dataheap_free*)Ptr;
            LockLeave(p->Lock);
            return;
        }

    	for (i=ARRAYEND(p->Buffer,dataheap_block);i!=ARRAYBEGIN(p->Buffer,dataheap_block);)
        {
            --i;
            if (i->Data < curr && i->Data+BUFFER_SIZE > curr)
            {
                DataHeap_Check(p);

                next = i->Data;
                do
                {
                    prev = next;
                    next += DataNext(next);
                }
                while (next<curr);

                *curr = Size + ((next - curr)<<16);
                *prev = DataSize(prev) + ((curr - prev)<<16);

                // merge with next (avoid delimiter)
                if (DataSize(next) && DataSize(curr)==DataNext(curr)) 
                    *curr += *next;

                // merge with prev (avoid delimiter)
                if (DataSize(prev) && DataSize(prev)==DataNext(prev)) 
                {
                    *prev += *curr;
                    curr = prev;
                }

                Size = DataSize(curr);
                if (Size > i->MaxSize)
                {
                    i->MaxSize = (uint16_t)Size;
                    i->Count = 1;

                    while (++i!=ARRAYEND(p->Buffer,dataheap_block) && i[-1].MaxSize > i->MaxSize)
                    {
                        SWAPVAL(dataheap_block,i[-1],i[0]);
                    }
                }
                else
                if (Size == i->MaxSize)
                    i->Count = (uint16_t)(i->Count+1);

                DataHeap_Check(p);
                break;
            }
        }

        LockLeave(p->Lock);
    }
}

static void* DataHeap_ReAlloc(dataheap* p, void* Old, size_t OldSize, size_t NewSize)
{
    size_t n;
    void* New;

    OldSize = DATAALIGN(OldSize)*sizeof(dataunit);
    NewSize = DATAALIGN(NewSize)*sizeof(dataunit);

    if (OldSize == NewSize)
        return Old;

    if (DATAHEAP_LIMIT*sizeof(dataunit)<=OldSize && DATAHEAP_LIMIT*sizeof(dataunit)<=NewSize)
        return MemHeap_ReAlloc(p->Heap,Old,OldSize,NewSize);

    New = DataHeap_Alloc(p,NewSize,0);

    if (!New && NewSize)
        return NULL; // failed

    if (Old && New)
    {
        n = min(OldSize,NewSize);
        memcpy(New,Old,n);
    }

    DataHeap_Free(p,Old,OldSize);
    return New;
}

void DataHeap_Init(dataheap* p,const cc_memheap* Heap)
{
    assert(BUFFER_SIZE-2>=DATAHEAP_LIMIT);
    p->Base.Alloc = (memheap_alloc)DataHeap_Alloc;
    p->Base.ReAlloc = (memheap_realloc)DataHeap_ReAlloc;
    p->Base.Free = (memheap_free)DataHeap_Free;
    p->Base.Write = (memheap_write)DataHeap_Write;
    p->Base.Null.Heap = &p->Base;
    p->Base.Null.Size = DATA_FLAG_MEMHEAP;
    p->Heap = Heap;
    ArrayInitEx(&p->Buffer,Heap);
    ArrayAlloc(&p->Buffer,512,1);
    p->Lock = LockCreate();
    p->Free3 = NULL;
}

void DataHeap_Done(dataheap* p)
{
	dataheap_block* i;
	for (i=ARRAYBEGIN(p->Buffer,dataheap_block);i!=ARRAYEND(p->Buffer,dataheap_block);++i)
		MemHeap_Free(p->Heap,i->Data,BUFFER_SIZE*sizeof(dataunit));
	ArrayClear(&p->Buffer);
    p->Free3 = NULL;
    LockDelete(p->Lock);
    p->Lock = NULL;
}

