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
#include "node_internal.h"

#define NODE_MAGIC    0xF0DE0A6C
#define DYNDATA_SHIFT 8

static const uint16_t ParamSize[MAX_PARAMTYPE] = 
{
	0,					//TYPE_NONE
	sizeof(bool_t),		//TYPE_BOOLEAN
	sizeof(int),		//TYPE_INT
	sizeof(cc_fraction),//TYPE_FRACTION
	MAXDATA,			//TYPE_STRING
	sizeof(cc_rect),	//TYPE_RECT
	sizeof(cc_point),	//TYPE_POINT
	sizeof(rgbval_t),	//TYPE_RGB
	sizeof(fourcc_t),	//TYPE_FOURCC
	sizeof(filepos_t),	//TYPE_FILEPOS
	sizeof(node*),		//TYPE_NODE
	sizeof(metanotify),	//TYPE_META
	sizeof(pin),		//TYPE_PACKET
	sizeof(tick_t),		//TYPE_TICK
	sizeof(nodenotify), //TYPE_NODENOTIFY
	sizeof(void*),		//TYPE_PTR
	MAXDATA,			//TYPE_BINARY
	sizeof(notify),		//TYPE_NOTIFY
	sizeof(int8_t),		//TYPE_INT8
	sizeof(int16_t),	//TYPE_INT16
	sizeof(int32_t),	//TYPE_INT32
	sizeof(int64_t),	//TYPE_INT64
	sizeof(nodefunc),	//TYPE_FUNC
    sizeof(node*),      //TYPE_NODE_REF
    sizeof(bool_t),     //TYPE_BOOL_BIT
    sizeof(pin),        //TYPE_PIN
    0,                  //TYPE_EVENT
    MAXDATA,            //TYPE_EXPR
    sizeof(cc_point16), //TYPE_POINT16
    sizeof(int16_t)*4,  //TYPE_RECT16
    sizeof(array),      //TYPE_ARRAY
	MAXDATA,			//TYPE_EXPRSTRING
	MAXDATA,			//TYPE_EXPRPARAM
	sizeof(datetime_t), //TYPE_DATETIME
	sizeof(int_fast32_t), //TYPE_DBNO
    sizeof(cc_guid),    //TYPE_GUID
    sizeof(int),        //TYPE_FIX16
    sizeof(int),        //TYPE_LUA_REF
	sizeof(notifyex),   //TYPE_NOTIFYEX
	sizeof(dataenum),   //TYPE_ENUM
    sizeof(multi_enum_set), //TYPE_ENUM_MULTI_SET
    sizeof(size_t),     //TYPE_SIZE
};

static const tchar_t* ParamName[MAX_PARAMTYPE] = 
{
	T("none"),			//TYPE_NONE
	T("boolean"),		//TYPE_BOOLEAN
	T("integer"),		//TYPE_INT
	T("fraction"),      //TYPE_FRACTION
	T("string"),		//TYPE_STRING
	T("rectangle"),	    //TYPE_RECT
	T("point"),	        //TYPE_POINT
	T("rgb"),       	//TYPE_RGB
	T("fourcc"),	    //TYPE_FOURCC
	T("filepos"),	    //TYPE_FILEPOS
	T("node"),  		//TYPE_NODE
	NULL,	            //TYPE_META
	T("packet"),		//TYPE_PACKET
	T("tick"),		    //TYPE_TICK
	NULL,               //TYPE_NODENOTIFY
	T("pointer"),		//TYPE_PTR
	NULL,		        //TYPE_BINARY
	NULL,		        //TYPE_NOTIFY
	T("integer_8"),		//TYPE_INT8
	T("integer_16"),    //TYPE_INT16
	T("integer_32"),	//TYPE_INT32
	T("integer_64"),	//TYPE_INT64
	NULL,	            //TYPE_FUNC
    T("node_ref"),      //TYPE_NODE_REF
    NULL,               //TYPE_BOOL_BIT
    T("pin"),           //TYPE_PIN
    T("event"),         //TYPE_EVENT
    NULL,               //TYPE_EXPR
    T("point_16"),      //TYPE_POINT16
    T("rect_16"),       //TYPE_RECT16
    NULL,               //TYPE_ARRAY
	NULL,			    //TYPE_EXPRSTRING
	NULL,			    //TYPE_EXPRPARAM
	T("datetime"),      //TYPE_DATETIME
    T("db_no"),         //TYPE_DBNO
    T("guid"),          //TYPE_GUID
    T("fix_16"),        //TYPE_FIX16
    NULL,               //TYPE_LUA_REF
	NULL,               //TYPE_NOTIFYEX
	NULL,               //TYPE_ENUM
	NULL,               //TYPE_ENUM_MULTI_SET
	T("size"),          //TYPE_SIZE
};

static const tchar_t* ParamFormat[(TUNIT_MASK>>TUNIT_SHIFT)+1] = 
{
    NULL,               
    T("kbyte"),         //TUNIT_KBYTE
    T("second"),        //TUNIT_SECOND
    T("mhz"),           //TUNIT_MHZ
    T("xcoord"),        //TUNIT_XCOORD
    T("ycoord"),        //TUNIT_YCOORD
    T("byterate"),      //TUNIT_BYTERATE
    T("folder"),        //TUNIT_FOLDER
    T("number"),        //TUNIT_NUMBER
    T("fix16"),         //TUNIT_FIX16
    T("ip"),            //TUNIT_IP
    T("coord"),         //TUNIT_COORD
    T("password"),      //TUNIT_PASSWORD

    T("upper"),         //TUNIT_UPPER
    T("hotkey"),        //TUNIT_HOTKEY
    T("checklist"),     //TUNIT_CHECKLIST
    T("percent"),       //TUNIT_PERCENT
    T("hex"),           //TUNIT_HEX
    T("task"),          //TUNIT_TASK
};

static INLINE void Node_ValidatePtr(anynode* Node)
{
#ifdef CONFIG_DEBUGCHECKS
    assert(((node*)Node)->Magic==NODE_MAGIC);
#endif
}

NOINLINE bool_t NodeClass_IsPartOf(const nodeclass* p, fourcc_t PartOfClass)
{
	for (;p;p=p->ParentClass)
		if (NodeClass_ClassId(p) == PartOfClass)
			return 1;
	return 0;
}

static INLINE nodeclass* NodeGetClass(const node* p)
{
    return ((nodeclass*)p->VMT)-1;
}

static INLINE bool_t CheckLoadModule(nodecontext* p,nodemodule* Module)
{
	if (&p->Base!=Module && p->LoadModule && !Module->Module)
        return p->LoadModule(p,Module);
    return 0;
}

static NOINLINE int CmpClass(const void* UNUSED_PARAM(p), const nodeclass* const* a, const nodeclass* const* b)
{
	fourcc_t AClass = NodeClass_ClassId(*a);
	fourcc_t BClass = NodeClass_ClassId(*b);
	if (AClass > BClass) return 1;
	if (AClass < BClass) return -1;
	if ((*a)->Module != (*b)->Module)
		return ((*a)->Module > (*b)->Module) ? 1:-1;
	return 0;
}

static NOINLINE int CmpClassNoModule(const void* UNUSED_PARAM(p), const nodeclass* const* a, const nodeclass* const* b)
{
	fourcc_t AClass = NodeClass_ClassId(*a);
	fourcc_t BClass = NodeClass_ClassId(*b);
	if (AClass > BClass) return 1;
	if (AClass < BClass) return -1;
	return 0;
}

static NOINLINE int CmpClassPri(const void* UNUSED_PARAM(p), const nodeclass* const* a, const nodeclass* const* b)
{
	int APriority = (*a)->Priority;
	int BPriority = (*b)->Priority;
	if (APriority > BPriority) return -1;
	if (APriority < BPriority) return 1;
	return CmpClass(NULL,a,b);
}

typedef struct nodeclassrated {
    const nodeclass *Class;
    int Rating;
} nodeclassrated;

static NOINLINE int CmpRatedClassPri(const void* UNUSED_PARAM(p), const nodeclassrated* a, const nodeclassrated* b)
{
	if (a->Rating > b->Rating) return -1;
	if (a->Rating < b->Rating) return 1;
    return CmpClassPri(NULL,&a->Class,&b->Class);
}

static NOINLINE int CmpNode(const void* p, const node* const* a, const node* const* b)
{
    const nodeclass* ca = NodeGetClass(*a);
    const nodeclass* cb = NodeGetClass(*b);
    return CmpClass(p,&ca,&cb);
}

static NOINLINE int CmpNodeClass(void* UNUSED_PARAM(p), const node* const* a, const fourcc_t* b)
{
	fourcc_t AClass = NodeClass_ClassId(NodeGetClass(*a));
	if (AClass > *b) return 1;
	if (AClass < *b) return -1;
	return 0;
}

NOINLINE const nodeclass* NodeContext_FindClass(anynode* Any,fourcc_t ClassId)
{
	size_t Pos;
	bool_t Found;
	nodeclass_with_vmt Item;
	const nodeclass* Ptr;
    nodecontext* p = Node_Context(Any);

    if (ClassId == 0)
        return NULL;

    Ptr = (const nodeclass*)p->NodeCache;
    if (Ptr && NodeClass_ClassId(Ptr) == ClassId)
        return Ptr;

	LockEnter(p->NodeLock);
	Item.VMT.ClassId = ClassId;
	Ptr = &Item.Class;
	Pos = ArrayFind(&p->NodeClass,nodeclass*,&Ptr,(arraycmp)CmpClassNoModule, NULL, &Found);
	if (Found)
	{
		if (ARRAYBEGIN(p->NodeClass,const nodeclass*)[Pos]->State<CLASS_REGISTERED)
		{
			// try searching for a valid
			size_t Count = ARRAYCOUNT(p->NodeClass,const nodeclass*);

			// find first with same id
			for (;Pos>0 && NodeClass_ClassId(ARRAYBEGIN(p->NodeClass,const nodeclass*)[Pos-1]) == ClassId;--Pos) {}

			Ptr = NULL;
			for (;Pos<Count && NodeClass_ClassId(ARRAYBEGIN(p->NodeClass,const nodeclass*)[Pos]) == ClassId;++Pos)
				if (ARRAYBEGIN(p->NodeClass,const nodeclass*)[Pos]->State>=CLASS_REGISTERED)
				{
					Ptr = ARRAYBEGIN(p->NodeClass,const nodeclass*)[Pos];
					break;
				}

		}
		else
			Ptr = ARRAYBEGIN(p->NodeClass,const nodeclass*)[Pos];

        p->NodeCache = Ptr;
	}
	else
	{
		Ptr = NULL;
		//DebugMessage(T("Class %c%c%c%c (%d) not found"),(ClassId>>0)&0xFF,(ClassId>>8)&0xFF,(ClassId>>16)&0xFF,(ClassId>>24)&0xFF,ClassId);
	}

	LockLeave(p->NodeLock);
	return Ptr;
}

static NOINLINE const nodeclass* FindModuleClass(nodecontext* p,fourcc_t ClassId,nodemodule* Module)
{
    const nodeclass** i;
	for (i=ARRAYBEGIN(p->NodeClass,const nodeclass*);i!=ARRAYEND(p->NodeClass,const nodeclass*);++i)
		if (NodeClass_ClassId(*i) == ClassId && (*i)->Module == Module && (*i)->State>=CLASS_REGISTERED)
			return *i;
    return NULL;
}

NOINLINE const nodeclass* NodeContext_FindClassEx(anynode* AnyNode,fourcc_t ClassId,nodemodule* Module)
{
    const nodeclass* Class = NULL;
	if (ClassId)
	{
        nodecontext* p = Node_Context(AnyNode);
		LockEnter(p->NodeLock);

		Class = FindModuleClass(p,ClassId,Module);
		if (!Class)
			Class = NodeContext_FindClass(p,ClassId);

		LockLeave(p->NodeLock);
	}
    return Class;
}

#if defined(CONFIG_DEBUG_LEAKS)
extern void DebugMessage(const tchar_t*,...);

typedef struct class_ref_t
{
    const nodeclass* Class;
    size_t Count;

} class_ref_t;

static int CmpClassRef(void* UNUSED_PARAM(p), const class_ref_t *a, const class_ref_t *b)
{
    return (a->Class->FourCC - b->Class->FourCC);
}

static void AddClassRef(const nodeclass* Class)
{
    bool_t Found;
    intptr_t Pos=-1;
    array *Refs = &Class->Module->ClassRefs;
    class_ref_t Item;
    
    if (!Class->Module->LockRefs)
        Class->Module->LockRefs = LockCreate();

    LockEnter(Class->Module->LockRefs);

    Item.Class = Class;
    Item.Count = 1;
    Pos = ArrayFind(Refs,class_ref_t,&Item,(arraycmp)CmpClassRef,NULL,&Found);
    if (Found)
    {
        ARRAYBEGIN(*Refs,class_ref_t)[Pos].Count++;
    }
    else if (ArrayAdd(Refs,class_ref_t,&Item,(arraycmp)CmpClassRef,NULL,0)==-1)
    {
        DebugMessage(T("AddClassRef %p class %r/%p could not be added !"),Refs,Class->FourCC,Class);
    }

    LockLeave(Class->Module->LockRefs);
}

static void DelClassRef(const nodeclass* Class)
{
    bool_t Found;
    intptr_t Pos;
    array *Refs = &Class->Module->ClassRefs;
    class_ref_t Item;

    if (!Class->Module->LockRefs)
        Class->Module->LockRefs = LockCreate();

    LockEnter(Class->Module->LockRefs);

    Item.Class = Class;
    Pos = ArrayFind(Refs,class_ref_t,&Item,(arraycmp)CmpClassRef,NULL,&Found);
    if (Found)
    {
        if (--ARRAYBEGIN(*Refs,class_ref_t)[Pos].Count==0)
        {
            //DebugMessage(T("class %r No more used !"),Class->FourCC);
            ArrayRemove(Refs,class_ref_t,&Item,(arraycmp)CmpClassRef,NULL);
        }
    }
    else
	{
        class_ref_t *r;
        DebugMessage(T("DelClassRef %p class %r/%p Not found !"),Refs,Class->FourCC,Class);
        for (r=ARRAYBEGIN(*Refs,class_ref_t);r!=ARRAYEND(*Refs,class_ref_t);++r)
        {
            DebugMessage(T("%r/%p=%d"),r->Class->FourCC,r->Class,r->Count);
        }
        Pos = ArrayFind(Refs,class_ref_t,&Item,(arraycmp)CmpClassRef,NULL,&Found);
	}
    LockLeave(Class->Module->LockRefs);
}
#else
#define AddClassRef(c)
#define DelClassRef(c)
#endif

static void UnInitClass(nodecontext* p,nodeclass* Class, bool_t IncludingModule);

static void UnRegisterModule(nodecontext* p, nodemodule* Module, bool_t IncludingModule)
{
	nodeclass** i;
    node** j;

    // release singleton objects
    for (j=ARRAYBEGIN(p->NodeSingleton,node*);j!=ARRAYEND(p->NodeSingleton,node*);++j)
    {
        if (NodeGetClass(*j)->Module == Module)
        {
            node* Node = *j;
            ArrayDelete(&p->NodeSingleton,(uint8_t*)j-ARRAYBEGIN(p->NodeSingleton,uint8_t),sizeof(node*));
            Node_Release(Node);
            j = ARRAYBEGIN(p->NodeSingleton,node*)-1; 
        }
    }

    // node memory leak in this module? (root module checked in NodeContext_Done)
    assert(Module==&p->Base || Module->Base.RefCount==1);

	for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
		if ((*i)->Meta && (*i)->Module == Module)
        {
            if ((*i)->State>=CLASS_INITED)
                UnInitClass(p,(*i),IncludingModule);
	        (*i)->Meta = NULL;
        }
}

NOINLINE bool_t NodeContext_Cleanup(nodecontext* p,bool_t Force)
{
    // at the moment we only release modules without any object (not even singleton objects)
    // because cleanup and singleton object releasing could have multithreading issues...

	bool_t Found=0;
    if (p->FreeModule)
    {
        nodemodule* i;
    	LockEnter(p->NodeLock);
	    for (i=p->Base.Next;i;i=i->Next)
		    if (i->Module && (Force || (!i->Config && i->Base.RefCount==1)))
		    {
                if (!Force)
                    NodeSingletonEvent(p,NODE_SINGLETON_SHUTDOWN,i);
                UnRegisterModule(p,i,0);
                p->FreeModule(p,i);
			    Found = 1;
		    }
    	LockLeave(p->NodeLock);
    }
	return Found;
}

static NOINLINE void UnlockModules(const nodeclass* Class)
{
	for (;Class;Class=Class->ParentClass)
    {
        DelClassRef(Class);
		--Class->Module->Base.RefCount;
        assert(Class->Module->Base.RefCount>=1);
    }
}

static void UnlockModulesWithLock(nodecontext* p,const nodeclass* Class)
{
    LockEnter(p->NodeLock);
    UnlockModules(Class);
    LockLeave(p->NodeLock);
}

static NOINLINE bool_t DataFree(nodecontext* p, node* Node, nodedata** i, bool_t DeletingNode)
{
    datatype Type;
    nodedata* Data = *i;
    nodenotify *n,*m;

    Type = Data->Code & TYPE_MASK;
    if (Type == TYPE_NODENOTIFY)
    {
        n=(nodenotify*)NodeData_Data(Data);
        if (n)
        {
            if (n->Func) // the notification list is being used
            {
                if (DeletingNode)
                    n->Referer = &n->Referer; // mark the node to be removed after the notification
                else
                    n->Referer = n; // mark all data to be freed after the notification
                return 0;
            }
            else
            {
                n=n->Next;
                while (n)
                {
                    m=n;
                    n=n->Next;
                    MemHeap_Free(p->NodeHeap,m,sizeof(nodenotify));
                }
            }
        }
    }
    else
    if (Type == TYPE_EXPR && p->ExprRelease)
        p->ExprRelease((nodeexpr*)NodeData_Data(Data));
#if defined(CONFIG_CORELUA)
    else
    if (Type == TYPE_LUA_REF && p->LuaRelease)
        p->LuaRelease(p->LuaCookie,(int*)NodeData_Data(Data));
#endif
    else
    if (Type == TYPE_NODE_REF && *(node**)NodeData_Data(Data))
        Node_Release(*(node**)NodeData_Data(Data));
    else
    if (Type == TYPE_NOTIFYEX)
    {
        notifyex *n=(notifyex*)NodeData_Data(Data);
        if (n->Free)
            n->Free(n->FreeCookie,n->This);
    }

    *i = Data->Next;
    MemHeap_Free(p->NodeHeap,Data,sizeof(nodedata)+Node_DataSize(Node,Data->Code>>8,Type,NodeData_Data(Data),META_PARAM_UNSET)); // META_PARAM_UNSET is neutral for the type
    return 1;
}

NOINLINE nodedata** Node_GetDataStart(node* Node, dataid Id, datatype Type)
{
    uint_fast32_t Code = (Id<<DYNDATA_SHIFT)|Type;
    nodedata** i;
    nodecontext* p = Node_Context(Node);

    LockEnter(p->NodeLock);

    for (i=&Node->Data;*i;i=&(*i)->Next)
        if ((*i)->Code == Code)
        {
            LockLeave(p->NodeLock);
            return i;
        }

    LockLeave(p->NodeLock);
    return NULL;
}

NOINLINE void Node_RemoveData(node* Node, dataid Id, datatype Type)
{
    uint_fast32_t Code = (Id<<DYNDATA_SHIFT)|Type;
    nodedata** i;
	nodecontext* p = Node_Context(Node);

	LockEnter(p->NodeLock);

    for (i=&Node->Data;*i;i=&(*i)->Next)
        if ((*i)->Code == Code)
        {
            DataFree(p,Node,i,0);
            break;
        }

	LockLeave(p->NodeLock);
}

NOINLINE void* Node_AddData(node* Node, dataid Id, datatype Type, const void* Data)
{
    if (Node)
    {
        size_t Size = Node_DataSize(Node,Id,Type&TYPE_MASK,Data,META_PARAM_UNSET); // META_PARAM_UNSET is neutral for the type
        if (Size>0)
        {
            nodedata* Ptr;
    	    nodecontext* p = Node_Context(Node);
    	    LockEnter(p->NodeLock);

            Ptr = (nodedata*)MemHeap_Alloc(p->NodeHeap,sizeof(nodedata)+Size,0);
            if (!Ptr)
            {
        	    LockLeave(p->NodeLock);
                return NULL;
            }

            // Data list order is important. Evaluating TYPE_EXPRSTRING needs to be done in the order it was added
            // This function reverses the order, but we also use Node_Copy with templates so the final order it correct (hackish...)

            memcpy(NodeData_Data(Ptr),Data,Size);
            Ptr->Code = (Id<<DYNDATA_SHIFT)|Type;
            Ptr->Next = Node->Data;
            Node->Data = Ptr;

            if (Type == TYPE_NODE_REF && *(node**)NodeData_Data(Ptr))
                Node_AddRef(*(node**)NodeData_Data(Ptr));

#if defined(CONFIG_CORELUA)
            if (Type == TYPE_LUA_REF)
                p->LuaAddRef(p->LuaCookie,(int*)NodeData_Data(Ptr));
#endif

    	    LockLeave(p->NodeLock);

            return NodeData_Data(Ptr);
        }
    }
    return NULL;
}

static void* SetData(node* p, dataid Id, datatype Type, const void* Data)
{
    size_t i,Size = Node_DataSize(p,Id,Type&TYPE_MASK,Data,META_PARAM_UNSET); // META_PARAM_UNSET is neutral for the type

    for (i=0;i<Size;++i)
        if (((uint8_t*)Data)[i])
            break;

    if (NodeTypeSize(Type)==MAXDATA) // variable size
    {
        Node_RemoveData(p,Id,Type);

        // do not write anything if the value is 0
        if (i<Size)
            return Node_AddData(p,Id,Type,Data);
        else
            return NULL; // TODO: remove the pre-existing data with Node_RemoveData() ?
    }
    else
    {
        // do not write anything if the value is 0
        if (i<Size)
        {
            void *v = Node_GetData(p,Id,Type);
            if (!v)
                return Node_AddData(p,Id,Type,Data);
            else
            {
                assert(Size==NodeTypeSize(Type));
                memcpy(v,Data,Size);
            }
            return v;
        }
        else
            Node_RemoveData(p,Id,Type);
        return NULL;
    }
}

err_t Node_ReadData(node* p, dataid Id, datatype Type, void* Data, size_t Size)
{
    const uint8_t* Ptr = Node_GetData(p,Id,Type);
    if (Ptr)
    {
		if (Type == TYPE_STRING)
			tcscpy_s((tchar_t*)Data,Size/sizeof(tchar_t),(const tchar_t*)Ptr);
        else
        {
            size_t DataSize = Node_DataSize(p,Id,Type,Ptr,META_PARAM_GET);
            if (DataSize > Size)
                return ERR_INVALID_PARAM;
            memcpy(Data,Ptr,DataSize);
        }
    }
    else if ((Type & TYPE_MASK) == TYPE_STRING)
        *((tchar_t*)Data) = 0;
    else
        memset(Data,0,Size);

    return ERR_NONE;
}

const tchar_t* Node_GetDataStr(const node* p, dataid Id)
{
    const tchar_t* s = Node_GetData(p,Id,TYPE_STRING);
    return s?s:T("");
}

datetime_t Node_GetDataDatetime(const node* p, dataid Id)
{
    datetime_t *d = Node_GetData(p,Id,TYPE_DATETIME);
    return d?*d:INVALID_DATETIME_T;
}


void* Node_GetData(const node* p, dataid Id, datatype Type)
{
    uint_fast32_t Code = (Id<<DYNDATA_SHIFT)|Type;
    nodedata* i;
    for (i=p->Data;i;i=i->Next)
        if (i->Code == Code)
            return NodeData_Data(i);
    return NULL;
}

static NOINLINE bool_t CallDelete(nodecontext* p,node* Node,const nodeclass* Class)
{
    bool_t Result = 1;
    nodedata **ListItem;

	for (;Class;Class=Class->ParentClass)
		if (Class->Meta)
		{
			const nodemeta* m;
			for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
            {
				if (m->Meta == META_CLASS_DELETE)
					((void(*)(node*))m->Data)(Node);
                else
                if (m->Meta == META_PARAM_DATA_RELEASE)
					((nodeupdatefunc)m->Data)(Node,0);
                else
                if (m->Meta == (META_MODE_DATA | TYPE_ARRAY) && (intptr_t)m->Data>=0)
                {
                    array* Ptr = (array*)((uint8_t*)Node+m->Data);
#if defined(CONFIG_CORECDOC)
                    if (m[2].Meta == META_PARAM_DATA_FLAGS)
                    {
                        ++m;
                        ++m;
                    }
                    if (m[2].Meta == META_PARAM_DATA_RELEASE)
                    {
                        ++m;
                        ++m;
        				((nodeupdatefunc)m->Data)(Node,0);
                    }
#else
                    if (m[1].Meta == META_PARAM_DATA_FLAGS)
                        ++m;
                    if (m[1].Meta == META_PARAM_DATA_RELEASE)
                    {
                        ++m;
        				((nodeupdatefunc)m->Data)(Node,0);
                    }
#endif
                    ArrayClear(Ptr);
                }
                else
                if (m->Meta == (META_MODE_DATA | TYPE_NODE_REF) && (intptr_t)m->Data>=0)
                {
                    node** Ptr = (node**)((uint8_t*)Node+m->Data);
                    node* v = *Ptr;
                    if (v)
                    {
#if defined(CONFIG_CORECDOC)
                        if (m[2].Meta == META_PARAM_DATA_FLAGS)
                        {
                            ++m;
                            ++m;
                        }
                        if (m[2].Meta == META_PARAM_DATA_RELEASE)
                        {
                            ++m;
                            ++m;
        				    ((nodeupdatefunc)m->Data)(Node,0);
                        }
#else
                        if (m[1].Meta == META_PARAM_DATA_FLAGS)
                            ++m;
                        if (m[1].Meta == META_PARAM_DATA_RELEASE)
                        {
                            ++m;
        				    ((nodeupdatefunc)m->Data)(Node,0);
                        }
#endif
                        *Ptr = NULL;
                        Node_Release(v);
                    }
                }
            }
		}

    // no need to NodeLock around DataFree(), because object in deleting
    // (other threads should not touch this object anymore)
    ListItem = &Node->Data;
    while (*ListItem)
        if (!DataFree(p,Node,ListItem,1))
        {
            ListItem = &(*ListItem)->Next; // will be deleted on Node_NotifyInternal exit
            Result = 0;
        }

    Node->VMT = NULL;
    return Result;
}

static NOINLINE const nodeclass* LockModules(nodecontext* p, const nodeclass* Class)
{
    fourcc_t ClassId = NodeClass_ClassId(Class);
	nodemodule* Module = Class->Module;

	if (!Class->ParentClass)
	{
		if (Class->ParentId)
			return NULL;
	}
	else
	{
        int8_t State = Class->State;

		if (!LockModules(p,Class->ParentClass))
			return NULL;

        if (State<CLASS_INITED)
            Class = FindModuleClass(p,ClassId,Module); // maybe have been reallocated
	}

    AddClassRef(Class);
	++Module->Base.RefCount;

    if (CheckLoadModule(p,Module))
    {
        NodeSingletonEvent(p,NODE_SINGLETON_STARTUP,Module);
        Class = FindModuleClass(p,ClassId,Module); // maybe have been reallocated
    }

	if (Class->State<CLASS_INITED)
	{
		UnlockModules(Class);
		return NULL;
	}

	return Class;
}

static const nodeclass* LockModulesWithLock(nodecontext* p, const nodeclass* Class)
{
    LockEnter(p->NodeLock);
    Class = LockModules(p,Class);
    LockLeave(p->NodeLock);
    return Class;
}

static void MetaConst(const nodemeta* i,void* Data)
{
	size_t Size = ParamSize[i->Meta & TYPE_MASK];
	if (Size == sizeof(uintptr_t))
		*(uintptr_t*)((uint8_t*)Data+i->Id) = i->Data;
	else
    {
        memset((uint8_t*)Data+i->Id,0,Size);
		memcpy((uint8_t*)Data+i->Id,&i->Data,min(sizeof(uintptr_t),Size));
    }
}

static err_t CallCreate(nodecontext* p,node* Node,const nodeclass* Class)
{
	if (Class)
	{
		const nodemeta* i;

		if (Class->ParentId && !Class->ParentClass)
			return ERR_NOT_SUPPORTED;

		if (CallCreate(p,Node,Class->ParentClass)!=ERR_NONE)
			return ERR_NOT_SUPPORTED;

		assert(Class->Meta && Class->State>=CLASS_INITED);

		for (i=Class->Meta;i->Meta != META_CLASS_PARENT_ID;++i)
		{
			if ((i->Meta & META_MODE_MASK)==META_MODE_CONST)
				MetaConst(i,Node);
			else
            if (i->Meta == (META_MODE_DATA | TYPE_ARRAY) && (intptr_t)i->Data>=0)
            {
                array* Ptr = (array*)((uint8_t*)Node+i->Data);
                ArrayInitEx(Ptr,p->NodeHeap);
            }
            else
			if (i->Meta == META_CLASS_CREATE && ((err_t(*)(node*))i->Data)(Node) != ERR_NONE)
			{
				CallDelete(p,Node,Class->ParentClass);
				return ERR_NOT_SUPPORTED;
			}
		}
	}
	return ERR_NONE;
}

static NOINLINE size_t NodeSize(const nodeclass* Class)
{
	const nodeclass *j;
	const nodemeta* m;
    size_t Size = 0;

    if (NodeClass_ClassId(Class) == NODEMODULE_CLASS)
        return sizeof(nodemodule); // for NodeContext_Done when all classes are already released

	for (j=Class;j && !Size;j=j->ParentClass)
		for (m=j->Meta;m && m->Meta != META_CLASS_PARENT_ID;++m)
			if (m->Meta == META_CLASS_SIZE)
			{
				Size = (size_t)m->Data;
				break;
			}

    return Size;
}

void Node_Destructor(node* Node)
{
    if (Node && Node->VMT)
    {
    	const nodeclass* Class = NodeGetClass(Node);
        nodecontext* p = Node_Context(Node);

        Node_Notify(Node,NODE_DELETING);

	    CallDelete(p,Node,Class);

#ifdef CONFIG_DEBUGCHECKS
        Node->Magic = 0;
#endif

        UnlockModulesWithLock(p,Class);
    }
}

static bool_t AddSingleton(nodecontext* p, node* Node)
{
    bool_t Result;
    LockEnter(p->NodeLock);
    Result = ArrayAdd(&p->NodeSingleton,node*,&Node,(arraycmp)CmpNode,NULL,64)>=0;
    LockLeave(p->NodeLock);
    return Result;
}

err_t Node_Constructor(anynode* AnyNode, node* Node, size_t Size, fourcc_t ClassId)
{
    err_t Err;
    const nodeclass* Class;
    nodecontext* p = Node_Context(AnyNode);

    memset(Node,0,Size);
    Node->RefCount = 1;

    Class = NodeContext_FindClass(p,ClassId);
    if (Class && (Class = LockModulesWithLock(p,Class)) != NULL)
    {
#ifdef CONFIG_DEBUGCHECKS
        Node->FourCC = ClassId;
        Node->Magic = NODE_MAGIC;
#endif
        Node->VMT = Class+1;

	    Err = CallCreate(p,Node,Class);
        if (Err == ERR_NONE)
        {
            if (Class->Flags & CFLAG_SINGLETON)
            {
                assert(Class->Flags & CFLAG_OWN_MEMORY);

                if (!AddSingleton(p,Node))
                {
                    Node_Destructor(Node);
			        return ERR_OUT_OF_MEMORY;
                }

                Node_AddRef(Node);
            }
        }
        else
        {
            Node->VMT = NULL;
		    UnlockModulesWithLock(p,Class);
        }
    }
    else
        Err = ERR_NOT_SUPPORTED;

    return Err;
}

static node* NodeCreateFromClass(nodecontext* p, const nodeclass* Class, bool_t Singleton);

static NOINLINE void ReleaseMetaLookup(nodecontext* p, nodeclass* Class)
{
    if (Class->ParentClass)
    {
        if (Class->MetaGet==Class->ParentClass->MetaGet)
            Class->MetaGet=NULL;
        if (Class->MetaSet==Class->ParentClass->MetaSet)
            Class->MetaSet=NULL;
        if (Class->MetaUnSet==Class->ParentClass->MetaUnSet)
            Class->MetaUnSet=NULL;
    }

    if (Class->MetaGet)
    {
        MemHeap_Free(p->NodeConstHeap,Class->MetaGet,sizeof(nodemetalookuphead)+(Class->MetaGet->Upper+1)*sizeof(nodemetalookup));
        Class->MetaGet = NULL;
    }

    if (Class->MetaSet)
    {
        MemHeap_Free(p->NodeConstHeap,Class->MetaSet,sizeof(nodemetalookuphead)+(Class->MetaSet->Upper+1)*sizeof(nodemetalookup));
        Class->MetaSet = NULL;
    }

    if (Class->MetaUnSet)
    {
        MemHeap_Free(p->NodeConstHeap,Class->MetaUnSet,sizeof(nodemetalookuphead)+(Class->MetaUnSet->Upper+1)*sizeof(nodemetalookup));
        Class->MetaUnSet = NULL;
    }
}

static void UnInitClass(nodecontext* p,nodeclass* Class, bool_t IncludingModule)
{
    nodeclass** i;
	const nodemeta* m;

    if (!IncludingModule && NodeClass_IsPartOf(Class,NODEMODULE_CLASS))
        return;

    // uninit all child classes
   	for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
    	if ((*i)->State>=CLASS_INITED && (*i)->ParentClass == Class)
			UnInitClass(p,*i,IncludingModule);

	for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
		if (m->Meta == META_CLASS_VMT_DELETE)
			((void(*)(fourcc_t,const void*))m->Data)(NodeClass_ClassId(Class),Class+1);

    if (Class->State>CLASS_REGISTERED)
        Class->State=CLASS_REGISTERED;

    ReleaseMetaLookup(p,Class);
}

static nodeclass* NodeClassResize(nodecontext* p,nodeclass* Old,size_t VMTSize)
{
    nodeclass* New = MemHeap_ReAlloc(p->NodeHeap,Old,sizeof(nodeclass)+Old->VMTSize,sizeof(nodeclass)+VMTSize);
	if (New)
    {
        nodeclass** i;

        New->VMTSize = VMTSize;

	    for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
        {
            if (*i == Old)
                *i = New;
            if ((*i)->ParentClass == Old)
                (*i)->ParentClass = New;
        }
    }
    return New;
}

static bool_t FilterLookupGet(const nodemeta* m)
{
    return m->Meta==META_PARAM_GET || (m->Meta & META_MODE_MASK)==META_MODE_DATA;
}

static bool_t FilterLookupSet(const nodemeta* m)
{
    return m->Meta==META_PARAM_SET || m->Meta==META_PARAM_EVENT ||
#if defined(CONFIG_CORECDOC)
        ((m->Meta & META_MODE_MASK)==META_MODE_DATA && (m[2].Meta != META_PARAM_DATA_FLAGS || !((int)m[2].Data & DFLAG_RDONLY)));
#else
        ((m->Meta & META_MODE_MASK)==META_MODE_DATA && (m[1].Meta != META_PARAM_DATA_FLAGS || !((int)m[1].Data & DFLAG_RDONLY)));
#endif
}        

static bool_t FilterLookupUnSet(const nodemeta* m)
{
    return m->Meta==META_PARAM_UNSET || m->Meta==META_PARAM_EVENT ||
#if defined(CONFIG_CORECDOC)
        ((m->Meta & META_MODE_MASK)==META_MODE_DATA && (m[2].Meta != META_PARAM_DATA_FLAGS || !((int)m[2].Data & DFLAG_RDONLY)));
#else
        ((m->Meta & META_MODE_MASK)==META_MODE_DATA && (m[1].Meta != META_PARAM_DATA_FLAGS || !((int)m[1].Data & DFLAG_RDONLY)));
#endif
}        

static const nodemeta* BitLookup(const nodeclass* Class,dataid Id)
{
    const nodemeta* m;
	for (;Class;Class=Class->ParentClass)
        if (Class->Meta)
            for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
                if (m->Id == Id && m->Meta == META_PARAM_BIT)
                    return m;
    return NULL;
}

static NOINLINE int CmpLookup(const nodemetalookup* p, const nodemetalookup* a, const nodemetalookup* b)
{
    // sort by Id
	if (a->Id > b->Id) return 1;
	if (a->Id < b->Id) return -1;
    // make sure META_PARAM_BIT is after META_MODE_DATA
    if (a->Meta && b->Meta && a->Meta->Meta == META_PARAM_BIT) return 1;
    if (b->Meta && b->Meta && b->Meta->Meta == META_PARAM_BIT) return -1;
    // make sure META_PARAM_EVENT's are added in the order as they are found
    if (a==p) return 1;
    if (b==p) return -1;
	return 0;
}

static NOINLINE void AddLookup(nodeclass* Class, const nodemeta* m, array* List)
{
    bool_t Found;
    nodemetalookup Lookup;
    Lookup.Id = m->Id;
    Lookup.Meta = NULL;
    ArrayFind(List,nodemetalookup,&Lookup,(arraycmp)CmpLookup,NULL,&Found);
    if (!Found || m->Meta == META_PARAM_EVENT)
    {
        if ((m->Meta & META_MODE_MASK)==META_MODE_DATA && (m->Meta & TYPE_MASK)==TYPE_BOOL_BIT)
        {
            Lookup.Meta = BitLookup(Class,m->Id);
            assert(Lookup.Meta);
            if (!Lookup.Meta)
                return;
            ArrayAdd(List,nodemetalookup,&Lookup,(arraycmp)CmpLookup,NULL,0);
        }
        Lookup.Meta = m;
        ArrayAdd(List,nodemetalookup,&Lookup,(arraycmp)CmpLookup,&Lookup,0);
    }
}

static NOINLINE nodemetalookuphead* BuildLookup(nodecontext* p,array* List)
{
    nodemetalookuphead* v;
    nodemetalookuphead Head;
    Head.Upper = ARRAYCOUNT(*List,nodemetalookup)-1;
    v = MemHeap_Alloc(p->NodeConstHeap,sizeof(Head)+ARRAYCOUNT(*List,uint8_t),0);
    if (v)
    {
        MemHeap_Write(p->NodeConstHeap,v,&Head,0,sizeof(Head));
        MemHeap_Write(p->NodeConstHeap,v,ARRAYBEGIN(*List,nodemetalookup),sizeof(Head),ARRAYCOUNT(*List,uint8_t));
    }
    ArrayClear(List);
    return v;
}

static void BuildMetaLookup(nodecontext* p,nodeclass* Class)
{
    const nodeclass* c;
    array List;
    bool_t NeedGet=0;
    bool_t NeedSet=0;
    bool_t NeedUnSet=0;

    const nodemeta* m;
	for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
    {
        if (FilterLookupGet(m))
            NeedGet = 1;
        if (FilterLookupSet(m))
            NeedSet = 1;
        if (FilterLookupUnSet(m))
            NeedUnSet = 1;
    }

	if (NeedGet)
    {
        ArrayInit(&List);
	    for (c=Class;c;c=c->ParentClass)
            if (c->Meta)
                for (m=c->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
				    if (FilterLookupGet(m))
                        AddLookup(Class,m,&List);

        if (!ARRAYEMPTY(List))
            Class->MetaGet = BuildLookup(p,&List);
    }
    else
    if (Class->ParentClass)
        Class->MetaGet = Class->ParentClass->MetaGet;

	if (NeedSet)
    {
        ArrayInit(&List);
	    for (c=Class;c;c=c->ParentClass)
            if (c->Meta)
                for (m=c->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
				    if (FilterLookupSet(m))
                        AddLookup(Class,m,&List);

        if (!ARRAYEMPTY(List))
            Class->MetaSet = BuildLookup(p,&List);
    }
    else
    if (Class->ParentClass)
        Class->MetaSet = Class->ParentClass->MetaSet;

	if (NeedUnSet)
    {
        ArrayInit(&List);
	    for (c=Class;c;c=c->ParentClass)
            if (c->Meta)
                for (m=c->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
				    if (FilterLookupUnSet(m))
                        AddLookup(Class,m,&List);

        if (!ARRAYEMPTY(List))
            Class->MetaUnSet = BuildLookup(p,&List);
    }
    else
    if (Class->ParentClass)
        Class->MetaUnSet = Class->ParentClass->MetaUnSet;
}

static void InitClass(nodecontext* p,nodeclass* Class)
{
    const nodeclass* Parent = Class->ParentClass;
    if (Class->State==CLASS_REGISTERED && Class->Meta && (!Class->ParentId || (Parent && Parent->State>=CLASS_INITED)))
    {
        uintptr_t Data;
        const nodemeta* i;

        if (Parent)
        {
            fourcc_t ClassId;

            if (Class->VMTSize == DEFAULT_VMT && Parent->VMTSize > DEFAULT_VMT)
            {
                Class = NodeClassResize(p,Class,Parent->VMTSize);
                if (!Class)
                    return;
            }

            assert(Parent->VMTSize >= DEFAULT_VMT);
            assert(Class->VMTSize >= Parent->VMTSize);
            assert(NodeClass_Context(Parent) == NodeClass_Context(Class));

            ClassId = NodeClass_ClassId(Class);
            memcpy(Class+1,Parent+1,min(Class->VMTSize,Parent->VMTSize));
            NodeClass_ClassId(Class) = ClassId;
        }

        BuildMetaLookup(p,Class);

        assert(Class->VMTSize >= DEFAULT_VMT);
        Class->State = CLASS_INITED;

	    for (i=Class->Meta;i->Meta != META_CLASS_PARENT_ID;++i)
	    {
		    if ((i->Meta & META_MODE_MASK)==META_MODE_VMT)
			{
#if defined(CONFIG_CORECDOC)
				if ((i->Meta & ~META_MODE_MASK)==TYPE_FUNC) // otherwise it might be some doc items
#endif
					MetaConst(i,Class+1);
			}
		    else
		    switch (i->Meta)
		    {
		    case META_CLASS_VMT_CREATE:
			    if (((int(*)(fourcc_t,void*))i->Data)(NodeClass_ClassId(Class),Class+1) != ERR_NONE)
                {
                    ReleaseMetaLookup(p,Class);
                    p->NodeCache = NULL;
				    Class->State = CLASS_FAILED;
                }
			    break;

            case META_CLASS_META:
                if (((bool_t(*)(nodecontext*,void*,datameta,uintptr_t*))i->Data)(p,Class+1,META_CLASS_FLAGS,&Data))
    			    Class->Flags = (uint8_t)Data;
                if (((bool_t(*)(nodecontext*,void*,datameta,uintptr_t*))i->Data)(p,Class+1,META_CLASS_PRIORITY,&Data))
			        Class->Priority = (int16_t)(Data-PRI_DEFAULT);
                break;

		    case META_CLASS_FLAGS:
			    Class->Flags = (uint8_t)i->Data;
			    break;

		    case META_CLASS_PRIORITY:
			    Class->Priority = (int16_t)(i->Data-PRI_DEFAULT);
			    break;
		    }
	    }
 
	    if (Class->State>=CLASS_INITED && (Class->Flags & (CFLAG_SINGLETON|CFLAG_OWN_MEMORY))==CFLAG_SINGLETON && !NodeCreateFromClass(p,Class,1))
	    {
            p->NodeCache = NULL;
            Class->State = CLASS_FAILED;
            UnInitClass(p,Class,0); // just to call META_CLASS_VMT_DELETE
	    }

	    if (Class->State>=CLASS_INITED)
        {
            nodeclass** i;
    	    for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
	    	    if ((*i)->ParentClass == Class)
		    	    InitClass(p,*i);
        }
    }
}

void NodeContext_UpdateParents(nodecontext* p)
{
	nodeclass** i;
	for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
    {
		(*i)->ParentClass = NodeContext_FindClass(p,(*i)->ParentId);
        InitClass(p,*i);
    }
}

static node* NodeCreateFromClass(nodecontext* p, const nodeclass* Class, bool_t Singleton)
{
	node* Node;
	size_t Size;

	if (!Class || (Class->Flags & CFLAG_ABSTRACT))
		return NULL;

	if ((Class->Flags & CFLAG_SINGLETON) && (Node = NodeSingleton(p,NodeClass_ClassId(Class)))!=NULL)
	{
        if (!Singleton)
            Node_AddRef(Node);
		return Node;
	}

	Class = LockModulesWithLock(p,Class);
    if (!Class)
		return NULL;

	Size = NodeSize(Class);
	if (!Size)
	{
        UnlockModulesWithLock(p,Class);
		return NULL;
	}

    Node = (node*)MemHeap_Alloc(p->NodeHeap,Size,0);
    if (Node)
    {
		memset(Node,0,Size);
#ifdef CONFIG_DEBUGCHECKS
        Node->FourCC = NodeClass_ClassId(Class);
        Node->Magic = NODE_MAGIC;
#endif
        Node->RefCount = 1;
        Node->VMT = Class+1;

        if (Singleton && !AddSingleton(p,Node))
        {
	        UnlockModulesWithLock(p,Class);
	        return NULL;
        }

		if (CallCreate(p,Node,Class) != ERR_NONE)
		{
        	LockEnter(p->NodeLock);
            if (Singleton)
                ArrayRemove(&p->NodeSingleton,node*,&Node,NULL,NULL); // can't use CmpNode, because Node->VMT is NULL
			UnlockModules(Class);
        	LockLeave(p->NodeLock);

            MemHeap_Free(p->NodeHeap,Node,Size);
			Node = NULL;
		}
	}

	return Node;
}

nodeclass* NodeContext_CreateClass(nodecontext* p, fourcc_t ClassId, size_t VMTSize, nodemodule* Module)
{
	nodeclass* Class = NULL;
	nodeclass** i;
    size_t Size = sizeof(nodeclass)+VMTSize;

    assert(VMTSize>=DEFAULT_VMT);

    p->NodeCache = NULL;

	for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
		if (NodeClass_ClassId(*i) == ClassId && (*i)->Module == Module)
		{
			Class = *i;
			break;
		}

	if (!Class)
	{
   	    nodeclass** i;

		Class = (nodeclass*) MemHeap_Alloc(p->NodeHeap,Size,0);
		if (!Class)
			return NULL;

		memset(Class,0,Size);
#ifdef CONFIG_DEBUGCHECKS
        Class->FourCC = ClassId;
#endif
		Class->VMTSize = VMTSize;
		NodeClass_Context(Class) = p;
		NodeClass_ClassId(Class) = ClassId;
		Class->Module = Module;

	    if (ArrayAdd(&p->NodeClass,nodeclass*,&Class,(arraycmp)CmpClass,NULL,128)<0)
        {
			MemHeap_Free(p->NodeHeap,Class,Size);
			return NULL;
        }

	    // update child classes
		for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
		    if ((*i)->ParentId == NodeClass_ClassId(Class) && (!(*i)->ParentClass || (*i)->ParentClass->State<CLASS_REGISTERED || (*i)->Module == Class->Module))
				(*i)->ParentClass = Class;
	}
	else
	{
		if (Class->Meta)
			return NULL; // already registered

		if (Class->VMTSize != VMTSize)
        {
            Class = NodeClassResize(p,Class,VMTSize);
            if (!Class)
                return NULL;
        }
	}

    if (Class->State<CLASS_REGISTERED)
	    Class->State=CLASS_REGISTERED;
	Class->Priority = 0;
	Class->Flags = 0;

	return Class;
}

void NodeRegisterClassEx(nodemodule* Module,const nodemeta* Meta)
{
	nodeclass* Class;
	nodecontext* p = Node_Context(Module);
	LockEnter(p->NodeLock);

	while (Meta->Meta == META_CLASS_CLASS_ID)
	{
        const nodeclass* ParentClass;
        fourcc_t ParentId;
		fourcc_t ClassId = (fourcc_t)(Meta++)->Data;
		size_t VMTSize = DEFAULT_VMT;
        const nodemeta* i = Meta;

        if (!ClassId)
            ClassId = ++p->DynamicClass;

        for (;Meta->Meta != META_CLASS_PARENT_ID;++Meta)
            if (Meta->Meta == META_CLASS_VMT_SIZE)
			    VMTSize = (size_t)(Meta->Data);

        ParentId = (fourcc_t)Meta->Data;
        ParentClass = NodeContext_FindClassEx(p,ParentId,Module);

        if (VMTSize == DEFAULT_VMT && ParentClass)
            VMTSize = ParentClass->VMTSize;

		Class = NodeContext_CreateClass(p,ClassId,VMTSize,Module);
		if (Class)
		{
			Class->Meta = i;
			Class->ParentId = ParentId;
			Class->ParentClass = ParentClass;
            InitClass(p,Class);
		}

		if (!(Meta++)->Id) // more classes?
			break; 
	}

	LockLeave(p->NodeLock);
}

static void EraseNode(nodecontext* p,node* Node,const nodeclass* Class)
{
    size_t Size = NodeSize(Class);

#ifdef CONFIG_DEBUGCHECKS
    Node->Magic = 0;
#endif

    UnlockModulesWithLock(p,Class);

    if (!(Class->Flags & CFLAG_OWN_MEMORY))
        MemHeap_Free(p->NodeHeap,Node,Size);
}

void Node_AddRef(thisnode p)
{
    assert(p); // we may switch to virtual reference functions later
    Node_ValidatePtr(p);
    ++((node*)p)->RefCount;
}

void Node_Release(thisnode p)
{
    nodecontext* Context;
    assert(p); // we may switch to virtual reference functions later
    Node_ValidatePtr(p);

    Context = Node_Context(p);
    if (--((node*)p)->RefCount == 0)
    {
    	const nodeclass* Class = NodeGetClass(p);
        Node_Notify((node*)p,NODE_DELETING);
        
        assert(((node*)p)->RefCount == 0); // the RefCount may increase during Node_Notify but it shouldn't
        if (CallDelete(Context,p,Class))
            EraseNode(Context,p,Class);
    }
}

void NodeDelete(node* p)
{
    if (p)
        Node_Release(p);
}

nodemodule* NodeClassModule(anynode* Any,fourcc_t ClassId)
{
    nodecontext* p = Node_Context(Any);
    nodemodule* Module = NULL;
	const nodeclass* Class = NULL;
	if (ClassId)
	{
		LockEnter(p->NodeLock);
		Class = NodeContext_FindClass(p,ClassId);
		LockLeave(p->NodeLock);
	}
	if (Class)
		Module = Class->Module;
	else
		Module = &p->Base;
    return Module;
}

nodemodule* NodeModule(node* Node)
{
    return NodeGetClass(Node)->Module;
}

NOINLINE bool_t Node_IsPartOf(const void* Node, fourcc_t PartOfClass)
{
    Node_ValidatePtr(Node);
    return NodeClass_IsPartOf(NodeGetClass(Node),PartOfClass);
}

NOINLINE void NodeClassSetPriority(node* Node,int Priority)
{
    NodeGetClass(Node)->Priority = (int16_t)(Priority - PRI_DEFAULT);
}

NOINLINE int NodeClassFlags(node* Node)
{
	return NodeGetClass(Node)->Flags;
}

node* NodeCreate(anynode* Any, fourcc_t ClassId)
{
	nodecontext* p = Node_Context(Any);
	return NodeCreateFromClass(p,NodeContext_FindClass(p,ClassId),0);
}

node* NodeSingleton(anynode* Any, fourcc_t Class)
{
	node* Node = NULL;
	if (Any)
	{
		nodecontext* p = Node_Context(Any);
		size_t Pos;
        bool_t Found;

		LockEnter(p->NodeLock);

        Pos = ArrayFind(&p->NodeSingleton,node*,&Class,(arraycmp)CmpNodeClass,NULL,&Found);
    	if (Found)
            Node = ARRAYBEGIN(p->NodeSingleton,node*)[Pos];

		LockLeave(p->NodeLock);
	}
	return Node;
}

bool_t NodeIsClass(anynode* p,fourcc_t ClassId, fourcc_t PartOfClass)
{
	return NodeClass_IsPartOf(NodeContext_FindClass(p,ClassId),PartOfClass);
}

fourcc_t NodeEnumClass(anynode* Any,array* ListId, fourcc_t ClassId)
{
	return NodeEnumClassFilterRated(Any,ListId,ClassId,NULL,NULL);
}

typedef struct filterstr
{
    int Id;
    const tchar_t* Str;
} filterstr;

static int FilterStr(filterstr* i, const nodeclass* Class)
{
    //TODO: suppport rating
    return StrListIndex(i->Str,NodeClass_Str(NULL,Class,i->Id))>=0?1:0;
}

fourcc_t NodeEnumClassStr(anynode* Any,array* ListId, fourcc_t ClassId, int Id, const tchar_t* Str)
{
    filterstr i;
    if (!Str || !Str[0])
    {
        if (ListId)
            ArrayInit(ListId);
        return 0;
    }
    i.Id = Id;
    i.Str = Str;
	return NodeEnumClassFilterRated(Any,ListId,ClassId,(nodeenumclassfilterrated)FilterStr,&i);
}

static NOINLINE void NodeEnumSingletonsEx(anynode* Any,array* List, nodemodule* Module)
{
    nodecontext* p = Node_Context(Any);
    node** i;
	ArrayInit(List);

	LockEnter(p->NodeLock);
	for (i=ARRAYBEGIN(p->NodeSingleton,node*);i!=ARRAYEND(p->NodeSingleton,node*);++i)
		if (*i && (!Module || NodeGetClass(*i)->Module == Module))
			ArrayAppend(List,i,sizeof(*i),64);
	LockLeave(p->NodeLock);
}

void NodeEnumSingletons(anynode* Any,array* List)
{
    NodeEnumSingletonsEx(Any,List,NULL);
}

static uintptr_t NOINLINE NodeParamMeta(const nodeclass* Class, uint_fast32_t Meta, dataid Id, const node* Node)
{
    uintptr_t Data;
	const nodemeta* m;

    for (;Class;Class=Class->ParentClass)
    {
		if (Class->Meta) // should be registered, just fail safe
			for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
                if (m->Id == Id)
                {
				    if (m->Meta == Meta)
					    return m->Data;
                    if (m->Meta == META_PARAM_META && ((bool_t(*)(const node*,dataid,datameta,uintptr_t*))m->Data)(Node,Id,Meta,&Data))
                        return Data;
                }

        if (Meta == META_PARAM_STRING)
        {
            const nodeclass* MainClass = Class;
            nodecontext* Context = NodeClass_Context(Class);

            // only check for META_PARAM_META...
	        while ((Class=Class->ParentClass)!=NULL)
            {
		        if (Class->Meta) // should be registered, just fail safe
			        for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
                        if (m->Id == Id && m->Meta == META_PARAM_META && ((bool_t(*)(const node*,dataid,datameta,uintptr_t*))m->Data)(Node,Id,Meta,&Data))
                            return Data;
            }

            Class = MainClass;
            if (Context->ExternalStr)
            {
                do {
                    Data = (uintptr_t)Context->ExternalStr(Context,NodeClass_ClassId(Class),(int)Id);
                    if (Data!=0 && *((const tchar_t*)Data))
                        return Data;
                } while ((Class=Class->ParentClass)!=NULL);
                return (uintptr_t)T("");
            }
        }
    }
	return 0;
}

NOINLINE uintptr_t NodeClass_Meta(const nodeclass* Class,dataid Id,datameta Meta)
{
    return NodeParamMeta(Class,Meta,Id,NULL); 
}

NOINLINE int NodeClass_Priority(const nodeclass* Class)
{
    return Class?Class->Priority:PRI_MINIMUM;
}

NOINLINE fourcc_t NodeClass_Parent(const nodeclass* Class)
{
    return Class?Class->ParentId:0;
}

NOINLINE const tchar_t* NodeClass_Str(anynode* AnyNode, const nodeclass* Class,int No)
{
	const tchar_t* s = (const tchar_t*)NodeParamMeta(Class,META_PARAM_STRING,No,AnyNode); 
    return s?s:T("");
}

const tchar_t* NodeParamStr(const node* p,int No)
{
    return NodeClass_Str(p,NodeGetClass(p),No);
}

const tchar_t* NodeStr2(anynode* AnyNode,fourcc_t ClassId,int No)
{
	nodecontext* Context = Node_Context(AnyNode);
    const nodeclass* Class = NodeContext_FindClass(Context,ClassId);
    if (Class)
	    return NodeClass_Str(AnyNode,Class,No);
    if (Context->ExternalStr)
        return Context->ExternalStr(Context,ClassId,No);
    return T("");
}

//TODO: check if this can be merged with NodeStr2
const tchar_t* NodeStrEx(anynode* AnyNode,fourcc_t ClassId,int No)
{
	nodecontext* Context = Node_Context(AnyNode);
    if (Context->ExternalStr)
        return Context->ExternalStr(Context,ClassId,No);
    return T("");
}

fourcc_t NodeEnumClassFilterRated(anynode* AnyContext, array* ListId, fourcc_t ClassId, nodeenumclassfilterrated Func, void* Param)
{
    nodecontext* p;
	const nodeclass* v;
	const nodeclass **i;
	array List;

	fourcc_t BestId = 0;
	int BestPri = 0;
    int BestRate = 0;

    assert(AnyContext != NULL);
    p = Node_Context(AnyContext);

	ArrayInit(&List);

	LockEnter(p->NodeLock);
	for (i=ARRAYBEGIN(p->NodeClass,const nodeclass*);i!=ARRAYEND(p->NodeClass,const nodeclass*);++i)
    {
		if ((*i)->State>=CLASS_REGISTERED && !((*i)->Flags & CFLAG_ABSTRACT)) // skip abstract
        {
			for (v=*i;v;v=v->ParentClass)
				if (NodeClass_ClassId(v) == ClassId)
				{
                    int Rating = Func ? Func(Param,*i):1;
                    if (Rating > 0)
                    {
					    if (!ListId)
					    {
						    if (BestRate < Rating || (BestRate == Rating && BestPri < (*i)->Priority))
						    {
							    BestId = NodeClass_ClassId(*i);
							    BestPri = (*i)->Priority;
                                BestRate = Rating;
						    }
					    }
					    else
                        {
                            nodeclassrated Item;
                            Item.Class = *i;
                            Item.Rating = Rating;
    					    ArrayAppend(&List,&Item,sizeof(Item),64);
                        }
                    }
					break;
				}
        }
    }

	if (ListId)
	{
		fourcc_t* a;
		nodeclassrated* b;
		size_t Count = ARRAYCOUNT(List,nodeclassrated);
		ArraySort(&List,nodeclassrated,(arraycmp)CmpRatedClassPri, NULL, 0);
		
		ArrayInit(ListId);
		if (ArrayAppend(ListId,NULL,Count*sizeof(fourcc_t),64))
		{
			for (a=ARRAYBEGIN(*ListId,fourcc_t),b=ARRAYBEGIN(List,nodeclassrated);b!=ARRAYEND(List,nodeclassrated);++b,++a)
                *a = NodeClass_ClassId(b->Class);
		}
		ArrayClear(&List);
	}

	LockLeave(p->NodeLock);
	return BestId;
}

static INLINE void Node_NotifyInternal(node* Node,dataid Id,nodedata** i,nodecontext* Context)
{
	const nodeclass* Class = NodeGetClass(Node);
    nodenotify* Top = NodeData_Data(*i);
    nodeevt Pin;
    nodenotify *n,*m;
    bool_t PostDeleteNode = 0, PosDeleteList = 0;

    // Top->Referer = any: the item of the list being called / NULL: remove the item being called from the list / Top: delete all the notification / &Top->Referer: delete all the node
    // Top->Func = set if the notification is in use

    if (Top->Func!=NULL)
    {
        assert(Top->Func!=NULL);
        return; // already being notified, not supported
    }
    Top->Func = (notifyproc)Top; // mark the notification as in use

    Pin.Node = Node;
    Pin.Id = Id;

    n = Top->Next; // skip the dummy notify element
    assert(n!=NULL); // if there is a Top there should be at least one item in the list
    while (n)
    {
        Top->Referer = n; // tell the RemoveNotify that this item in the list is being used
        m=n;
        n->Func(n->Referer,&Pin);
        n=n->Next;

        if (Top->Referer==&Top->Referer)
        {
            PostDeleteNode = 1;
        }
        else
        if (Top->Referer==Top)
        {
            PosDeleteList = 1;
        }
        else
        if (Top->Referer==NULL) // the notification item has been (asked to be) removed
        {
            MemHeap_Free(Context->NodeHeap,m,sizeof(nodenotify));
            // i may have changed
            i = Node_GetDataStart(Node,Id,TYPE_NODENOTIFY);
        }
    }
    Top->Referer = NULL;

    if (PostDeleteNode)
    {
    	LockEnter(Context->NodeLock);
        DataFree(Context,Node,i,1);
        EraseNode(Context,Node,Class);
    	LockLeave(Context->NodeLock);
    }
    else
    if (Id==NODE_DELETING || Top->Next==NULL || PosDeleteList)
    {
    	LockEnter(Context->NodeLock);
        DataFree(Context,Node,i,0); // delete the notify data when it's not to be used anymore(NODE_DELETING) or empty
	    LockLeave(Context->NodeLock);
    }
    Top->Func = NULL;
}

bool_t Node_Notify(node* Node,dataid Id)
{
    nodedata** i = Node_GetDataStart(Node,Id,TYPE_NODENOTIFY);
    if (i && NodeData_Data(*i))
    {
        Node_NotifyInternal(Node,Id,i,Node_Context(Node));
        return 1;
    }
    return 0;
}

#if defined(CONFIG_MULTITHREAD)
bool_t Node_PostNotify(node* Node,dataid Id)
{
    nodedata** i = Node_GetDataStart(Node,Id,TYPE_NODENOTIFY);
    if (i && NodeData_Data(*i) && NodeData_Data(*i)->Next) // do not propagate empty events
    {
        nodecontext* Context = Node_Context(Node);
        register uintptr_t CurrentThreadId = ThreadId();

        if (Context->PostNotify && Context->ThreadId != CurrentThreadId) // TODO: what about the other ones in the same thread ?
        {
            if (Context->PostNotify(Context->PostNotifyParam,Node,Id))
                return 0;
            // fallback to the same thread as we don't want to lose any event sending
        }
        Node_NotifyInternal(Node,Id,i,Context);
        return 1;
    }
    return 0;
}
#endif

NOINLINE void Node_AddNotify(node* Node, dataid Id, notifyproc Func, void* Referer)
{
    if (Node)
    {
        nodecontext* p = Node_Context(Node);
        nodenotify* n;
        nodenotify* Ptr;
    	LockEnter(p->NodeLock);
        
        Ptr = Node_GetData(Node,Id,TYPE_NODENOTIFY);
        if (!Ptr)
        {
            nodenotify Origin = {NULL,NULL,NULL};
            Ptr = Node_AddData(Node,Id,TYPE_NODENOTIFY,&Origin);
            if (!Ptr)
            {
                // Something went wrong ! No more memory ?
        	    LockLeave(p->NodeLock);
                return;
            }
        }

        n = MemHeap_Alloc(p->NodeHeap,sizeof(nodenotify),0);
        if (!n)
        {
        	LockLeave(p->NodeLock);
            return;
        }

        n->Func = Func;
        n->Referer = Referer;
        n->Next = Ptr->Next;
        Ptr->Next = n;

        LockLeave(p->NodeLock);
    }
}

void Node_AddNotify_Update(node* Node, dataid Id, notifyproc Func, void* Refered)
{
    Node_AddNotify(Node,Id,Func,Refered);
    if (Func)
    {
        nodeevt Pin;
        Pin.Node = Node;
        Pin.Id = Id;
        Func(Refered,&Pin);
    }
}

void Node_RemoveNotify(node* Node, dataid Id, notifyproc Func, void* Referer)
{
    if (Node)
    {
        nodecontext* p = Node_Context(Node);
        nodenotify* n;
    	LockEnter(p->NodeLock);

        n = Node_GetData(Node,Id,TYPE_NODENOTIFY);
        if (n)
        {
            nodenotify** Ptr;
            nodenotify* Top = n;
            void *OldReferer = Top->Referer;
            
            n = Top->Next; // skip the first element which is just there to know if an item of the chain is being notified
            Ptr = &n;
            for (;(n=*Ptr)!=NULL;Ptr=&n->Next)
            {
                if (n->Func==Func && n->Referer==Referer)
                {
                    // remove the item from the list
                    if (*Ptr == Top->Next)
                        Top->Next = n->Next;
                    else
                        *Ptr = n->Next;

                    if (Top->Referer==n) // this notification item is being called
                        Top->Referer = NULL; // tell the notifier that it should remove this element after the call
                    else
                        MemHeap_Free(p->NodeHeap,n,sizeof(nodenotify));
                    break;
                }
            }

            if (Top->Next==NULL && OldReferer==Top->Referer)
                Node_RemoveData(Node,Id,TYPE_NODENOTIFY); // the notification list is empty and it's not being called
        }
    	LockLeave(p->NodeLock);
    }
}

size_t NodeTypeSize(datatype Type)
{
	assert((Type & TYPE_MASK)<MAX_PARAMTYPE);
    return ParamSize[Type & TYPE_MASK];
}

dataflags NodeContext_FindDataType(const tchar_t* Type, const tchar_t* Format)
{
    dataflags i;
    for (i=0;i<MAX_PARAMTYPE;++i)
        if (ParamName[i] && tcsisame_ascii(Type,ParamName[i]))
        {
            if (Format && Format[0])
            {
                dataflags j;
                for (j=1;(j<<TUNIT_SHIFT) & TUNIT_MASK;++j)
                    if (ParamFormat[j] && tcsisame_ascii(Format,ParamFormat[j]))
                    {
                        i |= j<<TUNIT_SHIFT;
                        break;
                    }
            }
            return i;
        }
    return 0;
}

const tchar_t *NodeContext_TypeName(datatype Type)
{
    Type &= TYPE_MASK;
    return ParamName[Type];
}

const tchar_t *NodeContext_UnitName(datatype Unit)
{
    Unit &= TUNIT_MASK;
    return ParamFormat[Unit>>TUNIT_SHIFT];
}

static const tchar_t NullString[] = T("");

static NOINLINE bool_t EqData(datatype Type, const void* a, const void* b, size_t Size)
{
    size_t i;

    switch (Type)
    {
    case TYPE_EXPRSTRING:
    case TYPE_EXPRPARAM:
    case TYPE_STRING:
        if (!a) a = NullString;
        if (!b) b = NullString;
        return tcscmp(a,b)==0;

    case TYPE_ARRAY:
        return ArrayEq(a,b);

    case TYPE_FRACTION:
        if (a && b)
            return EqFrac((const cc_fraction*)a,(const cc_fraction*)b);
        break;
    }

    if (!a && !b)
        return 1;

    if (a && b)
        return memcmp(a,b,Size)==0;

    if (!a)
        a = b;

    for (i=0;i<Size;++i)
        if (((const uint8_t*)a)[i])
            return 0;

    return 1;
}

bool_t Node_EqData(node* p, dataid Id, dataflags Type, const void* a, const void* b)
{
    if ((Type & TFLAG_MULTI_ENUM) && (Id & DATA_ENUM_MULTI))
    {
        // b is a dataenumex with all the set values
        uint8_t *v;
        int *IsSet;
        const dataenum *Values = (const dataenum *)b;
        for (v=ARRAYBEGIN(Values->Value,uint8_t),IsSet=ARRAYBEGIN(Values->Name,int);v!=ARRAYEND(Values->Value,uint8_t);v+=Values->ValueSize,++IsSet)
        {
            if (EqData(Type & TYPE_MASK,a,v,Values->ValueSize))
                return (*IsSet!=0);
        }
        return 0;
    }
    return EqData(Type & TYPE_MASK,a,b,Node_MaxDataSize(p,Id,Type,META_PARAM_GET));
}

NOINLINE size_t Node_MaxDataSize(node* p, dataid Id, dataflags Flags, int QueryType)
{
	size_t Size;
    assert(QueryType==META_PARAM_GET || QueryType==META_PARAM_SET || QueryType==META_PARAM_UNSET);
    Flags &= TYPE_MASK;
    //assert(Flags != 0);
    if (Flags == 0)
        return 0;

    if (Id < (DATA_ENUM_MULTI*2)) // a normal ID (instead of a class ID, sometimes)
    {
        if (Id & DATA_ENUM)
        {
            if (QueryType==META_PARAM_GET)
                Flags = TYPE_ENUM;
            else
            if (QueryType==META_PARAM_SET)
                Flags = TYPE_STRING;
        }
        if (Id & DATA_ENUM_MULTI)
        {
            if (QueryType==META_PARAM_GET)
                Flags = TYPE_ENUM;
            else
            if (QueryType==META_PARAM_SET)
                Flags = TYPE_ENUM_MULTI_SET;
        }
    }

	assert(Flags < MAX_PARAMTYPE);
	Size = ParamSize[Flags];
	if (Size == MAXDATA && Flags != TYPE_STRING) // we don't care about how much space the non-dynamic string needs (except in MetaSet())
	{
		Size = (size_t)Node_Meta(p,Id,META_PARAM_SIZE);
		if (!Size)
			Size = MAXDATA;
	}
	return Size;
}

NOINLINE size_t Node_DataSize(node* p, dataid Id, datatype Type, const void* Data, int QueryType)
{
    size_t Size;

    assert(QueryType==META_PARAM_GET || QueryType==META_PARAM_SET || QueryType==META_PARAM_UNSET);
    if (!Data)
        return 0;
    
    switch (Type)
    {
    case TYPE_EXPR:
        Size = Node_Context(p)->ExprSize((nodeexpr*)Data);
        break;

    case TYPE_EXPRSTRING:
    case TYPE_EXPRPARAM:
    case TYPE_STRING:
        Size = tcsbytes((const tchar_t*)Data);
        break;

    default:
        Size = Node_MaxDataSize(p,Id,Type,QueryType);
        break;
    }
    return Size;
}

static NOINLINE void MetaEnumClass(const nodeclass* Class,array* List)
{
    if (Class->ParentClass)
        MetaEnumClass(Class->ParentClass,List);
        
    if (Class->Meta)
    {
        const nodemeta* m;
        for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
		    if (m->Meta == META_PARAM_TYPE)
    	    {
                dataid Id = m->Id;
                ArrayAppend(List,&Id,sizeof(Id),256);
            }
    }
}

static void MetaEnum(node* p,array* List)
{	
	const nodeclass* Class;
    ArrayInit(List);
    Class = NodeGetClass(p);
	if (Class)
        MetaEnumClass(Class,List);
}

static uintptr_t MetaMeta(node* p,dataid Id,datameta Meta)
{
    return NodeParamMeta(NodeGetClass(p),Meta,Id,p);
}

static err_t MetaGet(node* p,dataid Id,void* Data,size_t Size) 
{ 
	const nodeclass* Class = NodeGetClass(p);
    nodemetalookuphead* Head = Class->MetaGet;
    if (Head)
    {
        nodemetalookup* Lookup = (nodemetalookup*)(Head+1);
	    intptr_t Mid;
	    intptr_t Lower = 0;
	    intptr_t Upper = Head->Upper; 

	    while (Upper >= Lower) 
	    {
		    Mid = (Upper + Lower) >> 1;

		    if (Lookup[Mid].Id>Id)
			    Upper = Mid-1;	
		    else if (Lookup[Mid].Id<Id)  		
			    Lower = Mid+1;	
		    else 
		    {	
                const nodemeta* m = Lookup[Mid].Meta;
				const uint8_t* Ptr;
				datatype Type;
                
                if (m->Meta == META_PARAM_GET)
					return ((err_t(*)(node*,dataid,void*,size_t))m->Data)(p,Id,Data,Size);

                if (m->Meta == META_PARAM_BIT)
                    m = Lookup[--Mid].Meta;

			    assert((m->Meta & META_MODE_MASK)==META_MODE_DATA);

				Type = (m->Meta & TYPE_MASK);
                if ((intptr_t)m->Data<0)
                {
                    Ptr = Node_GetData(p,Id,Type);
                    if (!Ptr)
                    {
#if defined(CONFIG_CORECDOC)
                        if (m[2].Meta == META_PARAM_DATA_FLAGS && (m[2].Data & DFLAG_NODEFAULT))
#else
                        if (m[1].Meta == META_PARAM_DATA_FLAGS && (m[1].Data & DFLAG_NODEFAULT))
#endif
                            return ERR_INVALID_DATA;

                        memset(Data,0,Size);
                        return ERR_NONE;
                    }
                }
                else
                    Ptr = (const uint8_t*)p + (intptr_t)m->Data;

				if (Type == TYPE_STRING)
					tcscpy_s(Data,Size/sizeof(tchar_t),(const tchar_t*)Ptr);
				else
                if (Type == TYPE_BOOL_BIT)
                {
                    uint32_t Bit;
                    assert(Lookup[Mid+1].Id == Id && Lookup[Mid+1].Meta->Meta==META_PARAM_BIT);
                    Bit = (uint32_t)Lookup[Mid+1].Meta->Data;
                    if (Size != sizeof(bool_t))
                        return ERR_INVALID_DATA;
                    *(bool_t*)Data = (*(const uint32_t*)Ptr & Bit) != 0;
                }
                else
				{
					size_t DataSize = Node_MaxDataSize(p,m->Id,Type,META_PARAM_GET);
					assert(Size == DataSize);
					if (DataSize != Size)
						return ERR_INVALID_DATA;

                    if (DataSize==sizeof(int))
                        *(int*)Data = *(const int*)Ptr;
                    else
    					memcpy(Data,Ptr,DataSize);
				}

				return ERR_NONE;
		    }
	    }
    }
	return ERR_INVALID_PARAM; 
}

static err_t MetaSet(node* p,dataid Id,const void* Data,size_t Size)
{ 
	const nodeclass* Class = NodeGetClass(p);
    nodemetalookuphead* Head = Class->MetaSet;
    if (Head)
    {
        nodemetalookup* Lookup = (nodemetalookup*)(Head+1);
	    intptr_t Mid;
	    intptr_t Lower = 0;
	    intptr_t Upper = Head->Upper; 

	    while (Upper >= Lower) 
	    {
		    Mid = (Upper + Lower) >> 1;

		    if (Lookup[Mid].Id>Id)
			    Upper = Mid-1;	
		    else if (Lookup[Mid].Id<Id)  		
			    Lower = Mid+1;	
		    else 
		    {	
                err_t Result;
                int DFlags;
                dataflags Type;
                intptr_t MetaData;
                const nodemeta* m = Lookup[Mid].Meta;

			    if (m->Meta == META_PARAM_SET)
				    return ((err_t(*)(node*,dataid,const void*,size_t))m->Data)(p,Id,Data,Size);

			    if (m->Meta == META_PARAM_EVENT)
			    {
                    while (Mid>0 && Lookup[Mid-1].Id==Id)
                        --Mid;
                    do
                    {
                        assert(Lookup[Mid].Meta->Meta==META_PARAM_EVENT);
				        Result = ((nodeupdatefunc)Lookup[Mid].Meta->Data)(p,Id);
				        if (Result != ERR_NONE)
					        return Result;
                        ++Mid;
                    }
                    while (Mid<=(intptr_t)Head->Upper && Lookup[Mid].Id==Id);

                    return ERR_NONE;
                }

                if (m->Meta == META_PARAM_BIT)
                    m = Lookup[--Mid].Meta;

			    assert((m->Meta & META_MODE_MASK)==META_MODE_DATA); // any variable needs either a SET method, be an event or have related DATA

                MetaData = m->Data;
				Type = m->Meta;
                DFlags = 0;

#if defined(CONFIG_CORECDOC)
                if (m[2].Meta == META_PARAM_DATA_FLAGS)
#else
                if (m[1].Meta == META_PARAM_DATA_FLAGS)
#endif
                {
                    ++m;
#if defined(CONFIG_CORECDOC)
                    ++m;
#endif
                    DFlags = (int)m->Data;
                    if (DFlags & DFLAG_RDONLY)
                        continue;

                    // TODO: pointer should not use DFLAG_CMP because it maybe a recreated different object (unless referencing is used)
                    //assert(!(DFlags & DFLAG_CMP) || (Type & TYPE_MASK) != TYPE_NODE);

                    if ((DFlags & DFLAG_VERIFY_CLASS) && ((Type & TYPE_MASK) == TYPE_NODE || (Type & TYPE_MASK) == TYPE_NODE_REF) && 
                        Data && Size==sizeof(node*) && *(const node**)Data &&
                        !Node_IsPartOf(*(const node**)Data,(fourcc_t)Node_Meta(p,Id,META_PARAM_CLASS)))
                        return ERR_INVALID_DATA;
                }

                if (MetaData < 0)
                {
                    // dynamic data
                    size_t RealSize = Node_DataSize(p,Id,(Type & TYPE_MASK),Data,META_PARAM_SET);
                    if ((Type & TYPE_MASK) != TYPE_STRING && RealSize > Size)
                        return ERR_INVALID_DATA;

                    if ((DFlags & DFLAG_CMP) && Node_EqData(p,Id,Type,Data,Node_GetData(p,Id,Type & TYPE_MASK)))
                        return ERR_NONE;

                    // we don't support release with dynamic, because object destructor or manually dynamic data manipulation would not trigger it
#if defined(CONFIG_CORECDOC)
                    assert(m[2].Meta != META_PARAM_DATA_RELEASE); 
#else
                    assert(m[1].Meta != META_PARAM_DATA_RELEASE); 
#endif

                    Node_SetData(p,Id,Type & TYPE_MASK,Data);
                }
                else
                {
                    uint32_t Bit;
					uint8_t* Ptr = (uint8_t*)p + (intptr_t)MetaData;
					size_t DataSize = Node_MaxDataSize(p,Id,Type,META_PARAM_SET);

					assert((Type & TYPE_MASK) == TYPE_STRING || !Data || Size == DataSize);
					if (Data && Size != DataSize && (Type & TYPE_MASK) != TYPE_STRING)
						return ERR_INVALID_DATA;

                    if (DFlags & DFLAG_CMP)
                    {
                        if ((Type & TYPE_MASK) == TYPE_BOOL_BIT)
                        {
                            bool_t v;
                            uint32_t Bit;
                            assert(Lookup[Mid+1].Id == Id && Lookup[Mid+1].Meta->Meta==META_PARAM_BIT);
                            Bit = (uint32_t)Lookup[Mid+1].Meta->Data;
                            v = (*(uint32_t*)Ptr & Bit) != 0;
                            if (EqData(Type & TYPE_MASK,&v,Data,DataSize))
                                return ERR_NONE;
                        }
                        else
                        {
                           if (EqData(Type & TYPE_MASK,Ptr,Data,DataSize))
                                return ERR_NONE;
                        }
                    }

#if defined(CONFIG_CORECDOC)
                    if (m[2].Meta == META_PARAM_DATA_RELEASE)
#else
                    if (m[1].Meta == META_PARAM_DATA_RELEASE)
#endif
                    {
                        ++m;
#if defined(CONFIG_CORECDOC)
                        ++m;
#endif
						((nodeupdatefunc)m->Data)(p,Id);
                    }

					switch (Type & TYPE_MASK)
					{
					case TYPE_STRING:
                        DataSize = (size_t)Node_Meta(p,Id,META_PARAM_SIZE);
                        assert(DataSize);
                        if (!Data)
                            Data = NullString;
                        tcscpy_s((tchar_t*)Ptr,DataSize/sizeof(tchar_t),(const tchar_t*)Data); 
						break;

                    case TYPE_BOOL_BIT:
                        assert(Lookup[Mid+1].Id == Id && Lookup[Mid+1].Meta->Meta==META_PARAM_BIT);
                        Bit = (uint32_t)Lookup[Mid+1].Meta->Data;
                        if (Data && *(const bool_t*)Data)
                            *(uint32_t*)Ptr |= Bit;
                        else
                            *(uint32_t*)Ptr &= ~Bit;
                        break;

                    case TYPE_NODE_REF:
                    {
                        node* v = Data?*(node**)Data:NULL;
                        node* v0 = *(node**)Ptr;
                        if (v0 != v)
                        {
                            if (v)
                                Node_AddRef(v);
                            *(node**)Ptr = v;
                            if (v0)
                                Node_Release(v0);
                        }
                        break;
                    }
                    case TYPE_ARRAY:
                        ArrayCopy((array*)Ptr,(const array*)Data);
                        break;

					default:
						if (Data)
							memcpy(Ptr,Data,DataSize);
                        else
							memset(Ptr,0,DataSize);
						break;
					}
                }

                Result = ERR_NONE;

#if defined(CONFIG_CORECDOC)
				if (m[2].Meta == META_PARAM_DATA_UPDATE)
#else
				if (m[1].Meta == META_PARAM_DATA_UPDATE)
#endif
                {
                    ++m;
#if defined(CONFIG_CORECDOC)
                    ++m;
#endif
					Result = ((nodeupdatefunc)m->Data)(p,Id);
                }

                if (DFlags & DFLAG_NOTIFY)
                    Node_PostNotify(p,Id);

				return Result;
            }
	    }
    }
	return ERR_INVALID_PARAM; 
}

static err_t MetaUnSet(node* p,dataid Id,const void* Data,size_t Size)
{ 
	const nodeclass* Class = NodeGetClass(p);
    nodemetalookuphead* Head = Class->MetaUnSet;
    if (Head)
    {
        nodemetalookup* Lookup = (nodemetalookup*)(Head+1);
	    intptr_t Mid;
	    intptr_t Lower = 0;
	    intptr_t Upper = Head->Upper; 

	    while (Upper >= Lower) 
	    {
		    Mid = (Upper + Lower) >> 1;

		    if (Lookup[Mid].Id>Id)
			    Upper = Mid-1;	
		    else if (Lookup[Mid].Id<Id)  		
			    Lower = Mid+1;	
		    else 
		    {	
                err_t Result;
                int DFlags;
                dataflags Type;
                intptr_t MetaData;
                const nodemeta* m = Lookup[Mid].Meta;

			    if (m->Meta == META_PARAM_UNSET)
				    return ((err_t(*)(node*,dataid,const void*,size_t))m->Data)(p,Id,Data,Size);

			    if (m->Meta == META_PARAM_EVENT)
			    {
                    while (Mid>0 && Lookup[Mid-1].Id==Id)
                        --Mid;
                    do
                    {
                        assert(Lookup[Mid].Meta->Meta==META_PARAM_EVENT);
				        Result = ((nodeupdatefunc)Lookup[Mid].Meta->Data)(p,Id);
				        if (Result != ERR_NONE)
					        return Result;
                        ++Mid;
                    }
                    while (Mid<=(intptr_t)Head->Upper && Lookup[Mid].Id==Id);

                    return ERR_NONE;
                }

                if (m->Meta == META_PARAM_BIT)
                    m = Lookup[--Mid].Meta;

			    assert((m->Meta & META_MODE_MASK)==META_MODE_DATA);

                MetaData = m->Data;
				Type = m->Meta;
                DFlags = 0;

#if defined(CONFIG_CORECDOC)
                if (m[2].Meta == META_PARAM_DATA_FLAGS)
#else
                if (m[1].Meta == META_PARAM_DATA_FLAGS)
#endif
                {
                    ++m;
#if defined(CONFIG_CORECDOC)
                    ++m;
#endif
                    DFlags = (int)m->Data;
                    if (DFlags & DFLAG_RDONLY)
                        continue;

                    // TODO: pointer should not use DFLAG_CMP because it maybe a recreated different object (unless referencing is used)
                    //assert(!(DFlags & DFLAG_CMP) || (Type & TYPE_MASK) != TYPE_NODE);

                    if ((DFlags & DFLAG_VERIFY_CLASS) && ((Type & TYPE_MASK) == TYPE_NODE || (Type & TYPE_MASK) == TYPE_NODE_REF) && 
                        Data && Size==sizeof(node*) && *(const node**)Data &&
                        !Node_IsPartOf(*(const node**)Data,(fourcc_t)Node_Meta(p,Id,META_PARAM_CLASS)))
                        return ERR_INVALID_DATA;
                }

                if (MetaData < 0)
                {
                    size_t RealSize = Node_DataSize(p,Id,Type,Data,META_PARAM_UNSET);
                    if ((Type & TYPE_MASK) != TYPE_STRING && RealSize > Size)
                        return ERR_INVALID_DATA;

                    if ((DFlags & DFLAG_CMP) && Node_EqData(p,Id,Type,Data,Node_GetData(p,Id,Type & TYPE_MASK)))
                        return ERR_NONE;

                    // we don't support release with dynamic, because object destructor or manually dynamic data manipulation would not trigger it
#if defined(CONFIG_CORECDOC)
                    assert(m[2].Meta != META_PARAM_DATA_RELEASE);
#else
                    assert(m[1].Meta != META_PARAM_DATA_RELEASE);
#endif

                    Node_SetData(p,Id,Type & TYPE_MASK,Data);
                }
                else
                {
                    uint32_t Bit;
					uint8_t* Ptr = (uint8_t*)p + (intptr_t)MetaData;
					size_t DataSize = Node_MaxDataSize(p,Id,Type,META_PARAM_UNSET);

					assert((Type & TYPE_MASK) == TYPE_STRING || !Data || Size == DataSize);
					if (Data && Size != DataSize && (Type & TYPE_MASK) != TYPE_STRING)
						return ERR_INVALID_DATA;

                    if (DFlags & DFLAG_CMP)
                    {
                        if ((Type & TYPE_MASK) == TYPE_BOOL_BIT)
                        {
                            bool_t v;
                            uint32_t Bit;
                            assert(Lookup[Mid+1].Id == Id && Lookup[Mid+1].Meta->Meta==META_PARAM_BIT);
                            Bit = (uint32_t)Lookup[Mid+1].Meta->Data;
                            v = (*(uint32_t*)Ptr & Bit) != 0;
                            if (EqData(Type & TYPE_MASK,&v,Data,DataSize))
                                return ERR_NONE;
                        }
                        else
                        {
                           if (EqData(Type & TYPE_MASK,Ptr,Data,DataSize))
                                return ERR_NONE;
                        }
                    }

#if defined(CONFIG_CORECDOC)
                    if (m[2].Meta == META_PARAM_DATA_RELEASE)
#else
                    if (m[1].Meta == META_PARAM_DATA_RELEASE)
#endif
                    {
                        ++m;
#if defined(CONFIG_CORECDOC)
                        ++m;
#endif
						((nodeupdatefunc)m->Data)(p,Id);
                    }

					switch (Type & TYPE_MASK)
					{
					case TYPE_STRING:
                        DataSize = (size_t)Node_Meta(p,Id,META_PARAM_SIZE);
                        assert(DataSize);
                        if (!Data)
                            Data = NullString;
                        tcscpy_s((tchar_t*)Ptr,DataSize/sizeof(tchar_t),(const tchar_t*)Data); 
						break;

                    case TYPE_BOOL_BIT:
                        assert(Lookup[Mid+1].Id == Id && Lookup[Mid+1].Meta->Meta==META_PARAM_BIT);
                        Bit = (uint32_t)Lookup[Mid+1].Meta->Data;
                        if (Data && *(const bool_t*)Data)
                            *(uint32_t*)Ptr |= Bit;
                        else
                            *(uint32_t*)Ptr &= ~Bit;
                        break;

                    case TYPE_NODE_REF:
                    {
                        node* v = Data?*(node**)Data:NULL;
                        node* v0 = *(node**)Ptr;
                        if (v0 != v)
                        {
                            if (v)
                                Node_AddRef(v);
                            *(node**)Ptr = v;
                            if (v0)
                                Node_Release(v0);
                        }
                        break;
                    }
                    case TYPE_ARRAY:
                        ArrayCopy((array*)Ptr,(const array*)Data);
                        break;

					default:
						if (Data)
							memcpy(Ptr,Data,DataSize);
                        else
							memset(Ptr,0,DataSize);
						break;
					}
                }

                Result = ERR_NONE;

#if defined(CONFIG_CORECDOC)
				if (m[2].Meta == META_PARAM_DATA_UPDATE)
#else
				if (m[1].Meta == META_PARAM_DATA_UPDATE)
#endif
                {
                    ++m;
#if defined(CONFIG_CORECDOC)
                    ++m;
#endif
					Result = ((nodeupdatefunc)m->Data)(p,Id);
                }

                if (DFlags & DFLAG_NOTIFY)
                    Node_PostNotify(p,Id);

				return Result;
            }
	    }
    }
	return ERR_INVALID_PARAM; 
}

static dataid MetaFindParam(node* p,const tchar_t* Token)
{
    if (Token)
    {
        const nodeclass* c;
        const nodemeta* m;

        for (c = NodeGetClass(p);c;c=c->ParentClass)
	        if (c->Meta) // should be registered, just fail safe
		        for (m=c->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
			        if (m->Meta == META_PARAM_NAME && tcsisame_ascii(Token,(const tchar_t*)m->Data))
                    {
                        return m->Id;
                    }
    }
    return 0;
}

META_START(Node_Class,NODE_CLASS)
META_CLASS(SIZE,sizeof(node))
META_VMT(TYPE_FUNC,node_vmt,Enum,MetaEnum)
META_VMT(TYPE_FUNC,node_vmt,Get,MetaGet)
META_VMT(TYPE_FUNC,node_vmt,Set,MetaSet)
META_VMT(TYPE_FUNC,node_vmt,UnSet,MetaUnSet)
META_VMT(TYPE_FUNC,node_vmt,Meta,MetaMeta)
META_VMT(TYPE_FUNC,node_vmt,FindParam,MetaFindParam)
META_VMT(TYPE_FUNC,node_vmt,SetData,SetData)
META_PARAM(TYPE,NODE_DELETING,TYPE_EVENT|TFLAG_NOTIFY|TFLAG_RDONLY)
META_END_CONTINUE(0)

META_START_CONTINUE(NODEMODULE_CLASS)
META_CLASS(SIZE,sizeof(nodemodule))
META_END_CONTINUE(NODE_CLASS)

META_START_CONTINUE(NODECONTEXT_CLASS)
META_CLASS(SIZE,sizeof(nodecontext))
META_PARAM(NAME,NODECONTEXT_PROJECT_NAME,T("ProjName"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_NAME,TYPE_STRING|TFLAG_SETUP|TFLAG_RDONLY)
META_DYNAMIC_RDONLY(TYPE_STRING,NODECONTEXT_PROJECT_NAME)

META_PARAM(NAME,NODECONTEXT_PROJECT_VENDOR,T("ProjVendor"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_VENDOR,TYPE_STRING|TFLAG_SETUP|TFLAG_RDONLY)
META_DYNAMIC_RDONLY(TYPE_STRING,NODECONTEXT_PROJECT_VENDOR)

META_PARAM(NAME,NODECONTEXT_PROJECT_VERSION,T("ProjVersion"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_VERSION,TYPE_STRING|TFLAG_SETUP|TFLAG_RDONLY)
META_DYNAMIC_RDONLY(TYPE_STRING,NODECONTEXT_PROJECT_VERSION)

META_PARAM(NAME,NODECONTEXT_PROJECT_HELP,T("HelpFile"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_HELP,TYPE_STRING|TFLAG_SETUP|TFLAG_RDONLY)
META_DYNAMIC_RDONLY(TYPE_STRING,NODECONTEXT_PROJECT_HELP)

#if defined(TARGET_PALMOS)
META_PARAM(NAME,NODECONTEXT_PROJECT_FOURCC,T("ProjFourCC"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_FOURCC,TYPE_FOURCC|TFLAG_SETUP|TFLAG_RDONLY)
META_DATA(TYPE_FOURCC,NODECONTEXT_PROJECT_FOURCC,nodecontext,ProjFourCC)
#endif

META_PARAM(NAME,NODECONTEXT_PROJECT_BUILD,T("BuildVersion"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_BUILD,TYPE_INT|TFLAG_SETUP|TFLAG_RDONLY)
META_DATA(TYPE_INT,NODECONTEXT_PROJECT_BUILD,nodecontext,Build)

META_PARAM(NAME,NODECONTEXT_PROJECT_MIME,T("AppMime"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_MIME,TYPE_STRING|TFLAG_SETUP|TFLAG_RDONLY)
META_DYNAMIC_RDONLY(TYPE_STRING,NODECONTEXT_PROJECT_MIME)

META_PARAM(NAME,NODECONTEXT_PROJECT_PATH,T("ProjPath"))
META_PARAM(TYPE,NODECONTEXT_PROJECT_PATH,TYPE_STRING|TFLAG_SETUP|TFLAG_RDONLY)
META_DYNAMIC_RDONLY(TYPE_STRING,NODECONTEXT_PROJECT_PATH)

META_PARAM(TYPE,NODECONTEXT_PROJECT_APPID,TYPE_INT16|TFLAG_SETUP|TFLAG_RDONLY)
META_DATA(TYPE_INT16,NODECONTEXT_PROJECT_APPID,nodecontext,AppId)
META_END(NODEMODULE_CLASS)

MEMHEAP_DEFAULT

void NodeContext_Init(nodecontext* p,const nodemeta* Custom, const cc_memheap* Heap, const cc_memheap* ConstHeap)
{
    fourcc_t ClassId = NODECONTEXT_CLASS;
    node_vmt Tmp;

    memset(p,0,sizeof(nodecontext));

    if (Heap)
    {
        ArrayInitEx(&p->Collect,Heap);
        ArrayInitEx(&p->NodeSingleton,Heap);
        ArrayInitEx(&p->NodeClass,Heap);
    }
    else
        Heap = &MemHeap_Default;

    if (!ConstHeap)
        ConstHeap=Heap;

#if defined(CONFIG_MULTITHREAD)
    p->PostNotifyParam = p;
#endif
	p->NodeLock = LockCreate();
    p->NodeHeap = Heap;
    p->NodeConstHeap = ConstHeap;

    Tmp.Context = p;
    p->Base.Base.VMT = &Tmp;
    p->Base.Found = 1;
    p->Base.Func = (void*)(uintptr_t)NodeContext_Init;

	NodeRegisterClassEx(&p->Base,Node_Class);
    NodeRegisterClassEx(&p->Base,NodeTree_Class);

    if (Custom)
    {
        NodeRegisterClassEx(&p->Base,Custom);
        ClassId = (fourcc_t)Custom->Data;
    }

    Node_Constructor(p,(node*)p,0,ClassId);
}

void NodeContext_Done(nodecontext* p)
{
	nodeclass **i;

    NodeSingletonEvent(p,NODE_SINGLETON_SHUTDOWN,NULL);

	NodeContext_Cleanup(p,1); // first unregister all plugins

	UnRegisterModule(p,&p->Base,1); // unregister common.dll classes

    // free modules
    while (p->Base.Next)
    {
        node* v = (node*)p->Base.Next;
        p->Base.Next = p->Base.Next->Next;
		Node_Release(v);
    }

    Node_Destructor((node*)p);

#if defined(CONFIG_DEBUG_LEAKS)
    if (p->Base.Base.RefCount!=1)
    {
        class_ref_t *Item;
        for (Item=ARRAYBEGIN(p->Base.ClassRefs,class_ref_t);Item!=ARRAYEND(p->Base.ClassRefs,class_ref_t);++Item)
        {
            DebugMessage(T("remaining class %r (%d times)"),Item->Class->FourCC,Item->Count);
        }
    }
#endif
    assert(p->Base.Base.RefCount==1); // node memory leaking?

    // free classes
	for (i=ARRAYBEGIN(p->NodeClass,nodeclass*);i!=ARRAYEND(p->NodeClass,nodeclass*);++i)
	{
		assert(!(*i)->Meta && !(*i)->MetaGet && !(*i)->MetaSet && !(*i)->MetaUnSet);
		MemHeap_Free(p->NodeHeap,*i,sizeof(nodeclass)+(*i)->VMTSize);
	}

    assert(ARRAYEMPTY(p->Collect));
	ArrayClear(&p->Collect);
	ArrayClear(&p->NodeSingleton);
	ArrayClear(&p->NodeClass);
	LockDelete(p->NodeLock);
	p->NodeLock = NULL;
}

void NodeEnumDef(node* p,array* List)
{
    ArrayInit(List);
    if (p)
    {
        dataid* i;
        datadef Def;
        array Id;

        Node_Enum(p,&Id);

        for (i=ARRAYBEGIN(Id,dataid);i!=ARRAYEND(Id,dataid);++i)
            if (NodeDataDef(p,*i,&Def))
                ArrayAppend(List,&Def,sizeof(Def),256);

        ArrayClear(&Id);
    }
}

bool_t NodeDataDef(node* p, dataid Id, datadef* DataDef)
{
    uintptr_t Flags = Node_Meta(p,Id,META_PARAM_TYPE);
    if (!Flags)
        return 0;
    //assert((Flags & TYPE_MASK)!=0);

    DataDef->Id = Id;
    DataDef->Flags = (dataflags)Flags;
    return 1;
}

err_t Node_Toggle(void* Node,dataid Id)
{
    bool_t Bool = 0;
    Node_GET(Node,Id,&Bool);
    Bool = !Bool;
    return Node_SET(Node,Id,&Bool);
}

NOINLINE void NodeDup_Replace(array* Dup, node** Ptr)
{
    if (Dup)
    {
        node* p = *Ptr;
        nodedup* i;
        for (i=ARRAYBEGIN(*Dup,nodedup);i!=ARRAYEND(*Dup,nodedup);++i)
            if (i->Orig == p)
            {
                *Ptr = i->Dup;
                return;
            }
    }
}

static NOINLINE void CopyData(const nodeclass* Class, node* p, node* Src, array* Dup,  uint8_t* Data)
{
    if (Class->ParentClass)
        CopyData(Class->ParentClass,p,Src,Dup,Data);

    if (Class->Meta)
    {
        const nodemeta* m;
        for (m=Class->Meta;m->Meta != META_CLASS_PARENT_ID;++m)
        {
            uintptr_t Type;
			if (m->Meta == META_PARAM_TYPE)
                Type = m->Data;
            else
            if (m->Meta != META_PARAM_META || !((bool_t(*)(node*,dataid,datameta,uintptr_t*))m->Data)(Src,m->Id,META_PARAM_TYPE,&Type)) // we need to use the Src because dynamic flags (like UIButton_Value)
                continue;

            if (!(Type & (TFLAG_RDONLY|TFLAG_NOSAVE)) && (Type & TYPE_MASK))
			{
                dataid Id = m->Id;
                Type &= TYPE_MASK;
                if (Type != TYPE_EXPR) // batch expressions are already copied
                {
                    size_t Size = Node_MaxDataSize(Src,Id,Type,META_PARAM_GET);
                    assert((Id & ~DATA_ENUM)==Id);
                    if (Size && Node_Get(Src,Id,Data,Size) == ERR_NONE)
                    {
                        if (Type == TYPE_PIN)
                            NodeDup_Replace(Dup,&((pin*)Data)->Node);

                        if (Type == TYPE_NODE)
                            NodeDup_Replace(Dup,(node**)Data);

                        Node_Set(p,Id,Data,Size);
                    }
                }
            }
        }
    }
}

void Node_Copy(node* p, node* Src, array* Dup)
{
    nodedata* i;
	const nodeclass* Class = NodeGetClass(p);
    uint8_t Data[MAXDATA];

    // at moment only same ClassId will be copied
    if (Class != NodeGetClass(Src))
        return;

    for (i=Src->Data;i;i=i->Next)
    {
        datatype Type = i->Code & TYPE_MASK;
        if (Type == TYPE_EXPRSTRING || Type == TYPE_EXPRPARAM || Type == TYPE_EXPR)
        {
            void* Data = Node_AddData(p,i->Code>>8,Type,NodeData_Data(i));
            if (Type == TYPE_EXPR && Data)
                NodeClass_Context(Class)->ExprDup(p,(nodeexpr*)Data,Dup);
        }
    }

    CopyData(Class,p,Src,Dup,Data);
}

const void* Node_InheritedVMT(node* p,fourcc_t ClassId)
{
    const nodeclass* Class = NodeGetClass(p);
    while (NodeClass_ClassId(Class) != ClassId)
    {
        assert(Class->ParentClass);
        Class=Class->ParentClass;
    }
    assert(Class->ParentClass);
    return Class->ParentClass+1;
}

void NodeReportError(anynode* AnyNode, node* Sender, fourcc_t Class, int No,...)
{
    nodecontext* Context = Node_Context(AnyNode);
	if (Context->ReportError)
	{
	    va_list Args;
	    va_start(Args,No);
		Context->ReportError(Context,Sender,Class,No,Args);
		va_end(Args);
    }
}

void NodeSingletonEvent(anynode* Any, dataid Cmd, nodemodule* Module)
{
    array List;
    node** i;

    NodeEnumSingletonsEx(Any,&List,Module);

    for (i=ARRAYBEGIN(List,node*);i!=ARRAYEND(List,node*);++i)
        Node_Trigger(*i,Cmd);

    ArrayClear(&List);
}

void* NodeHeap_Alloc(anynode* AnyNode, size_t Size)
{
    return MemHeap_Alloc(Node_Context(AnyNode)->NodeHeap,Size,0);
}

void NodeHeap_Free(anynode* AnyNode, void* Ptr, size_t Size)
{
    MemHeap_Free(Node_Context(AnyNode)->NodeHeap,Ptr,Size);
}

typedef struct memcollectitem
{
    memcollect Func;
    void* Cookie;

} memcollectitem;

bool_t NodeHibernate(anynode* AnyNode)
{
    nodecontext* p = Node_Context(AnyNode);
	bool_t Changed = 0;

	if (!p->InCollect)
	{
        int Level;
        int NextLevel;
        memcollectitem* i;
		p->InCollect = 1;
    
        for (Level=COLLECT_UNUSED;!Changed;Level=NextLevel)
        {
            NextLevel = INT_MAX;
            for (i=ARRAYBEGIN(p->Collect,memcollectitem);i!=ARRAYEND(p->Collect,memcollectitem);++i)
            {
                int v=i->Func(i->Cookie,Level);
                if (v==COLLECT_FOUND)
                    Changed=1;
                else
                if (NextLevel>v && v>Level)
                    NextLevel=v;
            }
            if (NextLevel==INT_MAX)
                break;
        }

		if (!Changed && NodeContext_Cleanup(p,0))
			Changed = 1;

		p->InCollect = 0;
	}

	return Changed;
}

void Mem_AddCollector(anynode* AnyNode, memcollect Func, void* Cookie)
{
    memcollectitem Item;
    Item.Func = Func;
    Item.Cookie = Cookie;
    ArrayAppend(&Node_Context(AnyNode)->Collect,&Item,sizeof(Item),0);
}

void Mem_RemoveCollector(anynode* AnyNode, memcollect Func, void* Cookie)
{
    memcollectitem Item;
    Item.Func = Func;
    Item.Cookie = Cookie;
    ArrayRemove(&Node_Context(AnyNode)->Collect,memcollectitem,&Item,NULL,NULL);
}
