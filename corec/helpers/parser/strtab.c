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

#define TABLEALIGN	1024

typedef struct stringdef
{	
	fourcc_t Class;
	int32_t Id;
	// tchar_t s[]
} stringdef;

void StrTab_Init(strtab* p, const cc_memheap* Heap, size_t Alloc)
{
    assert(STRTAB_ITEMSIZE == sizeof(stringdef));
    p->Heap = Heap;
    ArrayInit(&p->Table); // not using Heap, because we need direct write acess
	ArrayAlloc(&p->Table,Alloc,TABLEALIGN); 
	p->Lock = LockCreate();
}

NOINLINE void StrTab_Clear(strtab* p)
{
	stringdef **i;
	LockEnter(p->Lock);
	for (i=ARRAYBEGIN(p->Table,stringdef*);i!=ARRAYEND(p->Table,stringdef*);++i)
        MemHeap_Free(p->Heap,*i,sizeof(stringdef)+tcsbytes((tchar_t*)(*i+1)));
    ArrayClear(&p->Table);
	LockLeave(p->Lock);
}

void StrTab_Done(strtab* p)
{
    StrTab_Clear(p);
	LockDelete(p->Lock);
    p->Lock = NULL;
}

static int CmpDef(const void* UNUSED_PARAM(p), const stringdef* const* pa, const stringdef* const* pb)
{
	const stringdef* a = *pa;
	const stringdef* b = *pb;
	if (a->Class < b->Class || (a->Class == b->Class && a->Id < b->Id))
		return -1;
	if (a->Class > b->Class || a->Id > b->Id)
		return 1;
	return 0;
}

void StrTab_Add(strtab* p, bool_t Secondary, fourcc_t Class, int32_t Id, const tchar_t* s)
{
	size_t Pos;
	bool_t Found;

	stringdef Def;
	stringdef* Ptr = &Def;
	Def.Class = Class;
	Def.Id = Id;

	if (s && !*s)
        s = NULL;

	LockEnter(p->Lock);

	// already the same?
	Pos = ArrayFind(&p->Table,stringdef*,&Ptr,(arraycmp)CmpDef,NULL,&Found);

    if (Found)
    {
        Ptr = ARRAYBEGIN(p->Table,stringdef*)[Pos];

        if (Secondary || (s && tcscmp(s,(tchar_t*)(Ptr+1))==0))
            s = NULL; // keep old
        else
        {
            MemHeap_Free(p->Heap,Ptr,sizeof(stringdef)+tcsbytes((tchar_t*)(Ptr+1)));
            ArrayDelete(&p->Table,Pos*sizeof(stringdef*),sizeof(stringdef*));
        }
    }

	if (s)
	{
        size_t Size = tcsbytes(s);
        Ptr = MemHeap_Alloc(p->Heap,sizeof(stringdef)+Size,0);
        if (Ptr)
        {
            MemHeap_Write(p->Heap,Ptr,&Def,0,sizeof(stringdef));
            MemHeap_Write(p->Heap,Ptr,s,sizeof(stringdef),Size);
	        ArrayAdd(&p->Table,stringdef*,&Ptr,(arraycmp)CmpDef,NULL,TABLEALIGN);
        }
    }

	LockLeave(p->Lock);
}

int StrTab_Enum(strtab* p, fourcc_t Class, size_t Count)
{
	int Result = -1;
	stringdef **i;

	LockEnter(p->Lock);

	for (i=ARRAYBEGIN(p->Table,stringdef*);i!=ARRAYEND(p->Table,stringdef*);++i)
		if ((*i)->Class==Class && Count--==0)
		{
			Result = (*i)->Id;
			break;
		}

	LockLeave(p->Lock);
	return Result;
}

static NOINLINE size_t FindPos(strtab *p, fourcc_t Class, int Id)
{
    size_t Pos;
    bool_t Found;
	stringdef Def;
	stringdef* Ptr = &Def;
	Def.Class = Class;
	Def.Id = Id;

	Pos = ArrayFind(&p->Table,stringdef*,&Ptr,(arraycmp)CmpDef,NULL,&Found);
    if (!Found) Pos = STRTAB_INVALID_POS;
    return Pos;
}

static NOINLINE const tchar_t *GetPos(strtab *p, size_t Pos)
{
    if (Pos==STRTAB_INVALID_POS || Pos>=ARRAYCOUNT(p->Table,stringdef*))
        return T("");
    return (const tchar_t*)(ARRAYBEGIN(p->Table,stringdef*)[Pos]+1);
}

size_t StrTab_Pos(strtab *p, fourcc_t Class, int Id)
{
    size_t Pos;
	LockEnter(p->Lock);
    Pos = FindPos(p,Class,Id);
	LockLeave(p->Lock);
    return Pos;
}

const tchar_t *StrTab_GetPos(strtab *p, size_t Pos)
{
    const tchar_t *s;
	LockEnter(p->Lock);
    s = GetPos(p,Pos);
	LockLeave(p->Lock);
    return s;
}

const tchar_t* StrTab_Find(strtab* p, fourcc_t Class, int Id)
{
    const tchar_t *s;
	LockEnter(p->Lock);
    s = GetPos(p,FindPos(p,Class,Id));
	LockLeave(p->Lock);
    return s;
}

bool_t StrTab_Get(strtab* p, fourcc_t Class, int Id, tchar_t* Out, size_t OutLen)
{
    size_t Pos;
	LockEnter(p->Lock);
    Pos = FindPos(p,Class,Id);
    tcscpy_s(Out,OutLen,GetPos(p,Pos));
	LockLeave(p->Lock);
    return (Pos!=STRTAB_INVALID_POS);
}

void StrTab_GetPosName(strtab *p, size_t Pos, tchar_t *Out, size_t OutLen)
{
    *Out = 0;
    if (Pos < ARRAYCOUNT(p->Table,stringdef*))
    {
        stringdef *i = ARRAYBEGIN(p->Table,stringdef*)[Pos];
        FourCCToString(Out,OutLen,i->Class);
        stprintf_s(Out+4,OutLen-4,T("%04X"),i->Id);
    }
}
