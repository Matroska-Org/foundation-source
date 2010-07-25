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

#include "node.h"

static NOINLINE err_t SetParent(nodetree* p,nodetree* Parent,nodetree* Before)
{
    err_t Result = ERR_NONE;

    assert(!Before || Before->Parent == Parent); // make sure Before is in the right tree

	if (p!=Before && (p->Parent != Parent || p->Next != Before))
    {
	    if (p->Parent)
            VMT_FUNC(p->Parent,nodetree_vmt)->RemoveChild(p->Parent,p);

	    if (Parent)
            Result = VMT_FUNC(Parent,nodetree_vmt)->AddChild(Parent,p,Before);
        assert(p->Next != p);
    }
    return Result;
}

nodetree* NodeTree_DetachAndRelease(void* p)
{
    nodetree* Next = NodeTree_Next(p);
    NodeTree_SetParent(p,NULL,NULL);
    Node_Release(p);
    return Next;
}

void NodeTree_Clear(nodetree* p)
{
	while (p->Children)
    	NodeTree_DetachAndRelease(p->Children);

	SetParent(p,NULL,NULL);
}

static err_t AddChild(nodetree* p,nodetree* Child, nodetree* Before)
{
    nodetree** i;
	LockEnter(Node_Context(p)->NodeLock);

    for (i=&p->Children;*i;i=&(*i)->Next)
        if (*i == Before)
            break;

    Child->Parent = p;
    Child->Next = Before;
    *i = Child;

    LockLeave(Node_Context(p)->NodeLock);
    return ERR_NONE;
}

static void RemoveChild(nodetree* p,nodetree* Child)
{
	nodetree** i;
	LockEnter(Node_Context(p)->NodeLock);

	for (i=&p->Children;*i;i=&(*i)->Next)
		if (*i==Child)
		{
			*i=Child->Next;
			break;
		}

	Child->Next = NULL;
    Child->Parent = NULL;

    LockLeave(Node_Context(p)->NodeLock);
}

void NodeTree_MoveBefore(void* p, void* Before)
{
    if (p != Before)
    {
        nodetree* Parent;
	    LockEnter(Node_Context(p)->NodeLock);
        Parent = NodeTree_Parent(p);
        RemoveChild(Parent,p);
        AddChild(Parent,p,Before);
        LockLeave(Node_Context(p)->NodeLock);
    }
}

nodetree* NodeTree_ChildByName(const void* p,const tchar_t* Name, fourcc_t Class, bool_t Recursive)
{
    if (p && Name && Name[0])
    {
        nodetree* i;
        for (i=NodeTree_Children(p);i;i=NodeTree_Next(i))
            if (Node_IsPartOf(i,Class))
            {
                const tchar_t* s = (const tchar_t*)Node_GetData((node*)i,NODE_ID,TYPE_STRING);
                if (s && tcsisame_ascii(s,Name))
                    return i;
            }

        if (Recursive)
        {
            nodetree* j;
            for (i=NodeTree_Children(p);i;i=NodeTree_Next(i))
                if ((j = NodeTree_ChildByName(i,Name,Class,1))!=NULL)
                    return j;
        }
    }
    return NULL;
}

NOINLINE nodetree* NodeTree_CreateChild(void* p, const tchar_t* Name, fourcc_t ClassId, void* Before)
{
    nodetree* Child = (nodetree*)NodeCreate(p,ClassId);
    if (Child)
    {
        NodeTree_SetParent(Child,p,Before);
        if (Name && Name[0])
            Node_SetData((node*)Child,NODE_ID,TYPE_STRING,Name);
    }
    return Child;
}

NOINLINE nodetree* NodeTree_ChildByClass(const void *p, fourcc_t Class)
{
    if (Node_IsPartOf(p,Class))
        return (nodetree*)p;
    for (p=NodeTree_Children(p);p;p=NodeTree_Next(p))
    {
        nodetree *Result = NodeTree_ChildByClass(p,Class);
        if (Result)
            return Result;
    }
    return NULL;
}

static nodetree* FindChild(nodetree* p, const tchar_t* Name)
{
    nodetree* Result;
    for (p=NodeTree_Children(p);p;p=NodeTree_Next(p))
    {
        const tchar_t* s = Node_GetData((node*)p,NODE_ID,TYPE_STRING);
        if (s)
        {
            if (tcsisame_ascii(Name,s))
                return p;
        }
        else
        if ((Result = NodeTree_FindChild(p,Name))!=NULL)
            return Result;
    }
    return NULL;
}

static nodetree* AddData(nodetree* UNUSED_PARAM(i), const tchar_t* UNUSED_PARAM(Token), dataflags UNUSED_PARAM(Flags))
{
    return NULL;
}

META_START(NodeTree_Class,NODETREE_CLASS)
META_CLASS(VMT_SIZE,sizeof(nodetree_vmt))
META_CLASS(SIZE,sizeof(nodetree))
META_CLASS(DELETE,NodeTree_Clear)
META_PARAM(NAME,NODETREE_PARENT,T("Parent"))
META_PARAM(TYPE,NODETREE_PARENT,TYPE_NODE|TFLAG_DEFAULT|TFLAG_RDONLY)
META_DATA_RDONLY(TYPE_NODE,NODETREE_PARENT,nodetree,Parent)
META_PARAM(NAME,NODETREE_NEXT,T("Next"))
META_PARAM(TYPE,NODETREE_NEXT,TYPE_NODE|TFLAG_DEFAULT|TFLAG_RDONLY)
META_DATA_RDONLY(TYPE_NODE,NODETREE_NEXT,nodetree,Next)
META_VMT(TYPE_FUNC,nodetree_vmt,SetParent,SetParent)
META_VMT(TYPE_FUNC,nodetree_vmt,AddChild,AddChild)
META_VMT(TYPE_FUNC,nodetree_vmt,RemoveChild,RemoveChild)
META_VMT(TYPE_FUNC,nodetree_vmt,AddData,AddData)
META_VMT(TYPE_FUNC,nodetree_vmt,FindChild,FindChild)
META_END(NODE_CLASS)
