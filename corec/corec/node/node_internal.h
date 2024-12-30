/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __NODE_INTERNAL_H
#define __NODE_INTERNAL_H

#define DEFAULT_VMT         sizeof(node_vmt)

#define CLASS_FAILED       -2
#define CLASS_NOT_FOUND    -1
#define CLASS_REGISTERED   0
#define CLASS_INITED       1

typedef struct nodemetalookup
{
    dataid Id;
    const nodemeta* Meta;

} nodemetalookup;

typedef struct nodemetalookuphead
{
    size_t Upper;
    //nodemetalookup Lookup[]

} nodemetalookuphead;

struct nodeclass
{
#ifdef CONFIG_DEBUGCHECKS
    fourcc_t    FourCC; // help figure out memory leak
#endif
	const nodemeta* Meta;
	nodemodule* Module;
	const nodeclass* ParentClass;
    nodemetalookuphead* MetaGet;
    nodemetalookuphead* MetaSet;
    nodemetalookuphead* MetaUnSet;
	fourcc_t ParentId;
	uint8_t Flags;
	int8_t State;
	int16_t	Priority;
    size_t VMTSize;
};

typedef struct nodeclass_with_vmt
{
    nodeclass Class;
    node_vmt VMT;

} nodeclass_with_vmt;

#define NodeClass_Context(Class) ((nodeclass_with_vmt*)Class)->VMT.Context
#define NodeClass_ClassId(Class) ((nodeclass_with_vmt*)Class)->VMT.ClassId

nodeclass* NodeContext_CreateClass(nodecontext* p, fourcc_t ClassId, size_t VMTSize, nodemodule* Module);
void NodeContext_UpdateParents(nodecontext* p);

extern const nodemeta NodeTree_Class[];

typedef struct nodenotify nodenotify;

struct nodenotify
{
    nodenotify* Next;
  	notifyproc Func;
	void* Referer;
};

#endif
