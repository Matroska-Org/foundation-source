/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "parser.h" // struct strtab
#include "strtab.h"
#include <corec/str/str.h>
#include <corec/memheap.h>

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
}

static NOINLINE void StrTab_Clear(strtab* p)
{
	stringdef **i;
	for (i=ARRAYBEGIN(p->Table,stringdef*);i!=ARRAYEND(p->Table,stringdef*);++i)
        MemHeap_Free(p->Heap,*i,sizeof(stringdef)+tcsbytes((tchar_t*)(*i+1)));
    ArrayClear(&p->Table);
}

void StrTab_Done(strtab* p)
{
    StrTab_Clear(p);
}

static int CmpDef(const void* UNUSED_PARAM(p), const void* vpa, const void* vpb)
{
    const stringdef* const* pa = vpa;
    const stringdef* const* pb = vpb;
	const stringdef* a = *pa;
	const stringdef* b = *pb;
	if (a->Class < b->Class || (a->Class == b->Class && a->Id < b->Id))
		return -1;
	if (a->Class > b->Class || a->Id > b->Id)
		return 1;
	return 0;
}

static NOINLINE size_t FindPos(strtab *p, fourcc_t Class, int Id)
{
    size_t Pos;
    bool_t Found;
	stringdef Def;
	stringdef* Ptr = &Def;
	Def.Class = Class;
	Def.Id = Id;

	Pos = ArrayFind(&p->Table,stringdef*,&Ptr,CmpDef,NULL,&Found);
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
    return FindPos(p,Class,Id);
}

const tchar_t *StrTab_GetPos(strtab *p, size_t Pos)
{
    return GetPos(p,Pos);
}

const tchar_t* StrTab_Find(strtab* p, fourcc_t Class, int Id)
{
    return GetPos(p,FindPos(p,Class,Id));
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
