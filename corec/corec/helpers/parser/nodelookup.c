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

typedef struct nodelookup
{
    const tchar_t* Name;
    node* Node;

} nodelookup;

static intptr_t CmpLookup(const void* UNUSED_PARAM(p), const nodelookup* a, const nodelookup* b)
{
    return tcsicmp_ascii(a->Name,b->Name);
}

void NodeLookup_AddSingletons(array* p, nodecontext* Context)
{
    array List;
    node** i;

    NodeEnumSingletons(Context,&List);
    for (i=ARRAYBEGIN(List,node*);i!=ARRAYEND(List,node*);++i)
        NodeLookup_Add(p,*i,NodeParamStr(*i,NODE_ID));
    ArrayClear(&List);
}

bool_t NodeLookup_Exists(array* p, const tchar_t* Name)
{
    assert(Name != NULL);
    if (Name && Name[0])
    {
        bool_t Found;
        nodelookup Lookup;
        Lookup.Name = Name;
        ArrayFind(p,nodelookup,&Lookup,(arraycmp)CmpLookup,NULL,&Found);
        return Found;
    }
    return 0;
}

node* NodeLookup_FindUnique(array* p, const tchar_t* Name)
{
    assert(Name!=NULL);
    if (Name && Name[0])
    {
        bool_t Found;
        size_t Pos;
        nodelookup Lookup;
        Lookup.Name = Name;
        Pos = ArrayFind(p,nodelookup,&Lookup,(arraycmp)CmpLookup,NULL,&Found);
        if (Found)
            return ARRAYBEGIN(*p,nodelookup)[Pos].Node;
    }
    return NULL;
}

bool_t NodeLookup_Add(array* p, node* Node, const tchar_t* Name)
{
    if (Name && Name[0])
    {
        bool_t Found;
        size_t Pos;
        nodelookup Lookup;

        Lookup.Name = Name;
        Lookup.Node = Node;

        Pos = ArrayFind(p,nodelookup,&Lookup,(arraycmp)CmpLookup,NULL,&Found);
        if (!Found)
		{
            ArrayAdd(p,nodelookup,&Lookup,(arraycmp)CmpLookup,NULL,0);
			return 1;
		}
        else
            ARRAYBEGIN(*p,nodelookup)[Pos].Node = NULL;
    }
	return 0;
}

void NodeLookup_Remove(array* p, node* Node, const tchar_t* Name)
{
    if (Name && Name[0])
    {
        bool_t Found;
        size_t Pos;
        nodelookup Lookup;

        Lookup.Name = Name;
        Lookup.Node = Node;

        Pos = ArrayFind(p,nodelookup,&Lookup,(arraycmp)CmpLookup,NULL,&Found);
        if (Found)
            ArrayDelete(p,Pos*sizeof(Lookup),sizeof(Lookup));
    }
}
