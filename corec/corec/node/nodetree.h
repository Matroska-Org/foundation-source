/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __NODETREE_H
#define __NODETREE_H

#define NODETREE_CLASS		FOURCC('T','R','E','E')

#define NODETREE_PARENT     20
#define NODETREE_NEXT       21

typedef struct nodetree nodetree;

typedef struct nodetree_vmt
{
    node_vmt Base;

    err_t (*SetParent)(thisnode,void* Parent,void* Before);
    err_t (*AddChild)(thisnode,void* Child,void* Before); //private
    void (*RemoveChild)(thisnode,void* Child); //private
    nodetree* (*AddData)(thisnode,const tchar_t* Name,dataflags Flags);
    nodetree* (*FindChild)(thisnode,const tchar_t* Name);

} nodetree_vmt;

struct nodetree
{
    node Base;
	nodetree* Parent;
	nodetree* Next;
	nodetree* Children;
};

#define NodeTree_SetParent(p,a,b) VMT_FUNC(p,nodetree_vmt)->SetParent(p,a,b)
#define NodeTree_AddData(p,a,b)   VMT_FUNC(p,nodetree_vmt)->AddData(p,a,b)
#define NodeTree_FindChild(p,a)   VMT_FUNC(p,nodetree_vmt)->FindChild(p,a)
#define NodeTree_Parent(p)        ((nodetree*)(p))->Parent
#define NodeTree_Children(p)      ((nodetree*)(p))->Children
#if defined(NDEBUG)
#define NodeTree_Next(p)          ((nodetree*)(p))->Next
#else
#define NodeTree_Next(p)          (assert((nodetree*)(p) != ((nodetree*)(p))->Next),((nodetree*)(p))->Next)
#endif

NODE_DLL nodetree* NodeTree_CreateChild(void* p, const tchar_t* Name, fourcc_t ClassId, void* Before);
NODE_DLL nodetree* NodeTree_ChildByName(const void* p,const tchar_t* Name, fourcc_t Class, bool_t Recursive);
NODE_DLL nodetree* NodeTree_ChildByClass(const void* p,fourcc_t Class);
NODE_DLL nodetree* NodeTree_DetachAndRelease(void* p);
NODE_DLL void NodeTree_MoveBefore(void* p, void* Before);
NODE_DLL void NodeTree_Clear(nodetree* p);

#endif
