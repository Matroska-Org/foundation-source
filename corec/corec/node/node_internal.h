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
