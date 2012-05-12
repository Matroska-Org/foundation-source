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

#ifndef __NODEBASE_H
#define __NODEBASE_H

#define MAXDATA				(MAXPATH*sizeof(tchar_t))

//----------------------------------------------------------------
// data types

#define TYPE_BOOLEAN		1		// bool_t
#define TYPE_INT			2		// int
#define TYPE_FRACTION		3		// cc_fraction
#define TYPE_STRING			4		// null terminated tchar_t[]
#define TYPE_RECT			5		// rect
#define TYPE_POINT			6		// point
#define TYPE_RGB			7		// rgbval_t
#define TYPE_FOURCC			8		// fourcc_t
#define TYPE_FILEPOS		9		// filepos_t
#define TYPE_NODE			10		// node* (format1: node class)
#define TYPE_META		    11		// metanotify 
#define TYPE_PACKET			12		// flow packet pin 
#define TYPE_TICK			13		// tick_t
#define TYPE_NODENOTIFY     14      // nodenotify (private)
#define TYPE_PTR			15		// void*
#define TYPE_BINARY			16		// binary data (format1: size)
#define TYPE_NOTIFY			17		// notify
#define TYPE_INT8			18		// int8_t
#define TYPE_INT16			19		// int16_t
#define TYPE_INT32			20		// int32_t
#define TYPE_INT64			21		// int64_t
#define TYPE_FUNC			22		// nodefunc
#define TYPE_NODE_REF       23      // node*
#define TYPE_BOOL_BIT       24      // bool_t
#define TYPE_PIN            25      // pin
#define TYPE_EVENT          26      // void
#define TYPE_EXPR           27      
#define TYPE_POINT16        28      // cc_point16
#define TYPE_RECT16         29      // int16_t[4]
#define TYPE_ARRAY          30
#define TYPE_EXPRSTRING     31
#define TYPE_EXPRPARAM      32
#define TYPE_DATETIME		33		// datetime_t
#define TYPE_DBNO     		34		// db_no
#define TYPE_GUID           35      // cc_guid
#define TYPE_FIX16          36      // int
#define TYPE_LUA_REF        37      // int
#define TYPE_NOTIFYEX       38
#define TYPE_ENUM           39      // dataenum
#define TYPE_ENUM_MULTI_SET 40      // multi_enum_set
#define TYPE_SIZE           41      // size_t

#define MAX_PARAMTYPE		42
#define TYPE_MASK           0x3F

#define PERCENT_ONE			1024
#define FIX16_SHIFT         16
#define FIX16_UNIT          (1<<FIX16_SHIFT)

#define TFLAG_DEFAULT       0x00000100
#define TFLAG_RDONLY		0x00000200
#define TFLAG_SETUP			0x00000400
#define TFLAG_SETTINGS  	0x00000800 // should be managed in a settings page
#define TFLAG_INPUT         0x00001000
#define TFLAG_OUTPUT        0x00002000
#define TFLAG_NOSAVE        0x00004000
#define TFLAG_ENUM          0x00008000 // Id|DATA_ENUM can be used
#define TFLAG_NOTIFY        0x00010000
#define TFLAG_UPDATEMODE    0x00020000
#define TFLAG_NODUMP        0x00040000
#define TFLAG_HOTKEY        0x00080000
#define TFLAG_DISPLAY       0x00100000
#define TFLAG_AVAILABLE     0x00200000 // Id|DATA_AVAILABLE can be used
// TFLAG_ENUM (sub)flags
#define TFLAG_EDITABLE_ENUM 0x00400000
#define TFLAG_MULTI_ENUM    0x00800000 // Id|DATA_ENUM_MULTI can be used
#define TFLAG_POPUP         0x01000000

#define TUNIT_SHIFT       24
#define TUNIT_MASK        0x1F000000
#define TUNIT_KBYTE       0x01000000
#define TUNIT_SECOND      0x02000000
#define TUNIT_MHZ         0x03000000
#define TUNIT_XCOORD      0x04000000
#define TUNIT_YCOORD      0x05000000
#define TUNIT_BYTERATE    0x06000000
#define TUNIT_FOLDER      0x07000000
#define TUNIT_NUMBER      0x08000000
#define TUNIT_FIX16       0x09000000 //TODO: replace TYPE_FIX16 with TYPE_INT|TUNIT_FIX16
#define TUNIT_IP          0x0A000000
#define TUNIT_COORD       0x0B000000
#define TUNIT_PASSWORD    0x0C000000
// TODO: not really units use the old TFORMAT_ for that (and a TFORMAT_MASK as well)
#define TUNIT_UPPER       0x0D000000
#define TUNIT_HOTKEY      0x0E000000
#define TUNIT_CHECKLIST   0x0F000000 // TYPE_BOOLEAN only
#define TUNIT_PERCENT     0x10000000
#define TUNIT_HEX         0x11000000
#define TUNIT_TASK        0x12000000 // TYPE_BOOLEAN only

typedef uint_fast32_t dataid;
typedef uint_fast32_t datatype;
typedef uint_fast32_t dataflags;
typedef uint_fast32_t datameta;

typedef struct datadef
{
	dataid Id;
	dataflags Flags;

} datadef;

//---------------------------------------------------------------
// future meta info...

#define META_CLASS_SIZE			10 //size_t
#define META_CLASS_FLAGS		11 //int
#define META_CLASS_PRIORITY		12 //uint16_t
#define META_CLASS_CREATE		13 //int (*)(node*)
#define META_CLASS_DELETE		14 //void (*)(node*)
#define META_CLASS_VMT_SIZE		15 //size_t
#define META_CLASS_VMT_CREATE	16 //int (*)(fourcc_t,void*)
#define META_CLASS_VMT_DELETE	17 //void (*)(fourcc_t,void*)
#define META_CLASS_META         18 //bool_t (*)(nodecontext*,void*,datameta Meta,uintptr_t* Data)

#define META_PARAM_NAME			30 //const tchar_t*
#define META_PARAM_TYPE		    31 //uint32_t (with flags)
#define META_PARAM_MIN			32 //int (for TYPE_INT or TYPE_TICK)
#define META_PARAM_MAX			33 //int (for TYPE_INT or TYPE_TICK)
#define META_PARAM_CLASS		34 //fourcc_t (for TYPE_NODE or TYPE_FOURCC)
#define META_PARAM_ENUMLANG     36 //fourcc_t (for TYPE_INT or TYPE_FOURCC)
#define META_PARAM_ENUMNAME     37 //const tchar_t* (for TYPE_INT or TYPE_FOURCC)
#define META_PARAM_BIT          38 //uint32_t (for TYPE_BOOL_BIT)
#define META_PARAM_SIZE			39 //size_t (for TYPE_BINARY or TYPE_STRING)
#define META_PARAM_GET			40 //nodeget
#define META_PARAM_SET			41 //nodeset
#define META_PARAM_UNSET		42 //nodeunset
#define META_PARAM_STRING		44 //const tchar_t*
#define META_PARAM_EVENT		45 //nodeupdatefunc (ERR_NONE result will cause searching more event handlers)
#define META_PARAM_CUSTOM       47
#define META_PARAM_UPDATETIME   48 //tick_t
#define META_PARAM_ARRAY_TYPE	49 //dataflags
#define META_PARAM_META         50 //bool_t (*)(node*,dataid Id,datameta Meta,uintptr_t* Data)
#define META_PARAM_ENUMVALUE    53 //void* array
#define META_PARAM_ENUMVALUESIZE 54 //size_t
#define META_PARAM_ENUMFILTER   55 //bool_t (*)(node*,dataid Id,const void* Value,size_t Size)

// private
#define META_CLASS_PARENT_ID	0  //fourcc_t
#define META_CLASS_CLASS_ID		1  //fourcc_t
#define META_PARAM_DATA_FLAGS	2  //int      (must be after META_DATA)
#define META_PARAM_DATA_RELEASE	3  //nodeupdatefunc (must be after META_DATA,META_PARAM_DATA_FLAGS)
#define META_PARAM_DATA_UPDATE	4  //nodeupdatefunc (must be after META_DATA,META_PARAM_DATA_FLAGS,META_PARAM_DATA_RELEASE)

#define DFLAG_RDONLY	        1
#define DFLAG_CMP 	            2
#define DFLAG_NOTIFY            4
#define DFLAG_VERIFY_CLASS      8
#define DFLAG_NODEFAULT         16

#define META_MODE_MASK		192
#define META_MODE_DATA		64	   // param with data
#define META_MODE_CONST		128    // constant in object (set during NodeCreate)
#define META_MODE_VMT		192	   // constant in vmt (set during NodeRegisterClass)

#if defined(CONFIG_CORECDOC)
// assume that only TYPE_FUNC is used with META_MODE_VMT so far (so we have up to 22 that are safe)
#define TYPE_DOC_PARAM_NO      (META_MODE_VMT|0)
#define TYPE_DOC_CLASS_ID      (META_MODE_VMT|1)
#define TYPE_DOC_VMT_STRUCT    (META_MODE_VMT|2)
#define TYPE_DOC_VMT_FUNC      (META_MODE_VMT|3)

#define DOC_PARAM_DESCRIPTION  (META_MODE_VMT|4)
#define DOC_PARAM_NAME         (META_MODE_VMT|5)
#define DOC_CLASS_DESCRIPTION  (META_MODE_VMT|6)
#endif

typedef struct nodemeta
{
	uint32_t Meta:8;
	uint32_t Id:24;
	uintptr_t Data;

} nodemeta;

#ifdef __cplusplus
#define META_START(name,id) extern "C" { extern const nodemeta name[]; } const nodemeta name[] = { META_START_CONTINUE(id)
#else
#define META_START(name,id) const nodemeta name[] = { META_START_CONTINUE(id)
#endif
#define META_START_CONTINUE(id) META_CLASS(CLASS_ID,id) 
#define META_CLASS(meta,data) { META_CLASS_##meta,0,(uintptr_t)(data) },
#define META_PARAM(meta,no,data) { META_PARAM_##meta,no,(uintptr_t)(data) },
#define META_DATA(type,no,name,param) { META_MODE_DATA|(type),no,(uintptr_t)(OFS(name,param)) },
#define META_DATA_FLAGS(type,no,name,param,flags) META_DATA(type,no,name,param) META_PARAM(DATA_FLAGS,no,flags)
#define META_DATA_RDONLY(type,no,name,param) META_DATA_FLAGS(type,no,name,param,DFLAG_RDONLY)
#define META_DATA_UPDATE(type,no,name,param,update) META_DATA(type,no,name,param) META_PARAM(DATA_UPDATE,no,update)
#define META_DATA_UPDATE_FLAGS(type,no,name,param,update,flags) META_DATA(type,no,name,param) META_PARAM(DATA_FLAGS,no,flags) META_PARAM(DATA_UPDATE,no,update)
#define META_DATA_UPDATE_CMP(type,no,name,param,update) META_DATA_UPDATE_FLAGS(type,no,name,param,update,DFLAG_CMP)
#define META_DATA_UPDATE_RELEASE(type,no,name,param,update,release,flags) META_DATA(type,no,name,param) META_PARAM(DATA_FLAGS,no,flags) META_PARAM(DATA_RELEASE,no,release) META_PARAM(DATA_UPDATE,no,update)
#define META_DYNAMIC(type,no) { META_MODE_DATA|(type),no,(uintptr_t)-1 },
#define META_DYNAMIC_FLAGS(type,no,flags) META_DYNAMIC(type,no) META_PARAM(DATA_FLAGS,no,flags)
#define META_DYNAMIC_RDONLY(type,no) META_DYNAMIC_FLAGS(type,no,DFLAG_RDONLY)
#define META_DYNAMIC_UPDATE(type,no,update) META_DYNAMIC(type,no) META_PARAM(DATA_UPDATE,no,update)
#define META_DYNAMIC_UPDATE_FLAGS(type,no,update,flags) META_DYNAMIC(type,no) META_PARAM(DATA_FLAGS,no,flags) META_PARAM(DATA_UPDATE,no,update)
#define META_DYNAMIC_UPDATE_CMP(type,no,update) META_DYNAMIC_UPDATE_FLAGS(type,no,update,DFLAG_CMP)
#define META_VMT(type,name,param,value) { META_MODE_VMT|(type),OFS(name,param),(uintptr_t)(value) },
#define META_CONST(type,name,param,value) { META_MODE_CONST|(type),OFS(name,param),(uintptr_t)(value) },
#define META_END(parentid) { META_CLASS_PARENT_ID,0,(uintptr_t)(parentid) }};
#define META_END_CONTINUE(parentid) { META_CLASS_PARENT_ID,1,(uintptr_t)(parentid) },

#define META_PARAM_DOC(type,no,data)
#define META_CLASS_DOC(type,data)

#if defined(CONFIG_CORECDOC)
#undef META_START
#undef META_START_CONTINUE
#undef META_PARAM
#undef META_DATA
#undef META_DYNAMIC
#undef META_VMT
#undef META_PARAM_DOC
#undef META_CLASS_DOC
#define META_START_CONTINUE(id) META_CLASS(CLASS_ID,id) \
	                            { TYPE_DOC_CLASS_ID,id,(uintptr_t)T(#id) },
#ifdef __cplusplus
#define META_START(name,id) extern "C" { extern const nodemeta name[]; } const nodemeta name[] = { META_CLASS(CLASS_ID,id) \
	                            { TYPE_DOC_CLASS_ID,id,(uintptr_t)T(#id) },
#else
#define META_START(name,id) const nodemeta name[] = { META_CLASS(CLASS_ID,id) \
	                            { TYPE_DOC_CLASS_ID,id,(uintptr_t)T(#id) },
#endif
#define META_PARAM(meta,no,data) { META_PARAM_##meta,no,(uintptr_t)(data) },\
                                 { TYPE_DOC_PARAM_NO,no,(uintptr_t)T(#no) },
#define META_DATA(type,no,name,param) { META_MODE_DATA|(type),no,(uintptr_t)(OFS(name,param)) },\
                                      { TYPE_DOC_PARAM_NO,no,(uintptr_t)T(#no) },
#define META_DYNAMIC(type,no) { META_MODE_DATA|(type),no,(uintptr_t)-1 },\
                              { TYPE_DOC_PARAM_NO,no,(uintptr_t)T(#no) },
#define META_VMT(type,name,param,value) { META_MODE_VMT|(type),OFS(name,param),(uintptr_t)(value) }, \
                                        { TYPE_DOC_VMT_STRUCT,0,(uintptr_t)T(#name) },   \
                                        { TYPE_DOC_VMT_FUNC,0,(uintptr_t)T(#param) },

#define META_PARAM_DOC(type,no,data)  {type,no,(uintptr_t)data},
#define META_CLASS_DOC(type,data)     {type,0,(uintptr_t)data},
#endif

#define META_PARAM_DOC_DESCRIPTION(no,data) META_PARAM_DOC(DOC_PARAM_DESCRIPTION,no,data)
#define META_CLASS_DOC_DESCRIPTION(data) META_CLASS_DOC(DOC_CLASS_DESCRIPTION,data)


#define CFLAG_SINGLETON		0x01
#define CFLAG_SETTINGS		0x02  // has elements with TFLAG_SETTINGS
#define CFLAG_CONFIG        0x04  // save the paramaters
#define CFLAG_ABSTRACT		0x08
#define CFLAG_LOCAL         0x10
#define CFLAG_OWN_MEMORY    0x20

//---------------------------------------------------------------
// node variable data

typedef struct nodedata nodedata;

struct nodedata
{
    nodedata* Next;
	size_t Code; // Type+(No<<8)
    //...
};

#define NodeData_Data(p)  (void*)((nodedata*)p+1)

//---------------------------------------------------------------

// dataid modifiers (max is 0x800000)
#define DATA_ENUM           0x10000 // get:dataenum / set:add named_value / unset:delete value
#define DATA_ICON           0x20000 // tchar_t*
#define DATA_AVAILABLE      0x40000 // bool_t
#define DATA_UPDATEMODE     0x80000 // bool_t
#define DATA_DYNNAME       0x100000 // tchar_t*
#define DATA_ENUM_MULTI    0x200000 // get:dataenum (with .Name a int flag to tell the status of each value) / set:individual value via multi_enum_set
// TODO: use a new dataenumex type for DATA_ENUM_MULTI

#define DataidBase(Id) ((Id) & ~(DATA_ENUM|DATA_ICON|DATA_AVAILABLE|DATA_UPDATEMODE|DATA_DYNNAME|DATA_ENUM_MULTI))
#define DataidMask(Id) ((Id) &  (DATA_ENUM|DATA_ICON|DATA_AVAILABLE|DATA_UPDATEMODE|DATA_DYNNAME|DATA_ENUM_MULTI))

static INLINE bool_t IsExtendedId(dataid Id, dataid ExtensionStart, size_t ExtensionSize)
{
    Id = DataidBase(Id);
    return (Id >= ExtensionStart && Id < ExtensionStart+ExtensionSize);
}

typedef struct dataenum
{
    size_t ValueSize;
    array Name;  // tchar_t
    array Value; // any type

} dataenum;

typedef struct multi_enum_set
{
    void *Element;
    size_t ElementSize;
    int Value;
} multi_enum_set;

//---------------------------------------------------------------
// node

#define NODE_CLASS			FOURCC('N','O','D','E')

// strings
#define NODE_NAME				0
#define NODE_CONTENTTYPE		1
#define NODE_EXTS				2
#define NODE_PROBE				3
#define NODE_PROTOCOL           4
#define NODE_ID                 5

// events for singleton objects
#define NODE_SINGLETON_INSTALL            6
#define NODE_SINGLETON_UNINSTALL          7
#define NODE_SINGLETON_STARTUP            8  // after the module's classes registered and singleton objects are created
#define NODE_SINGLETON_CONFIGURED         9  // after configuration loaded for CFLAG_CONFIG objects
#define NODE_SINGLETON_SHUTDOWN           10 // before the module's singleton objects are deleted

// events for any class
#define NODE_DELETING           11 // can be used as notify
#define NODE_SETTINGS_CHANGED   12 // can be used as notify

// settings
#define NODE_GAP                13
#define NODE_SELECT_ALL         14 //TODO: remove
#define NODE_SELECT_NONE        15 //TODO: remove

// for debug/dump (array)
#define NODE_CHILDREN			13

// default param
#define NODE_DEFAULT_DATA       14 //TODO: fix collision with NODE_SELECT_ALL

typedef	err_t (*nodefunc)(void* This);
typedef	err_t (*nodeupdatefunc)(void* This,dataid Id);
typedef bool_t (*nodeenumfilter)(void* This,dataid Id,const void* Value,size_t Size);

typedef const void anynode;
typedef void* thisnode;

typedef struct nodecontext nodecontext;

typedef struct node
{
#ifdef CONFIG_DEBUGCHECKS
    fourcc_t    FourCC; // help figure out memory leak
    uint32_t    Magic;
#endif
    const void* VMT;
    nodedata*   Data;
    size_t      RefCount;

} node;

typedef struct node_vmt
{
	nodecontext* Context;
	fourcc_t ClassId;

    void (*Enum)(thisnode,array* List);
    err_t (*Get)(thisnode,dataid Id,void* Data,size_t Size);
    err_t (*Set)(thisnode,dataid Id,const void* Data,size_t Size);
    err_t (*UnSet)(thisnode,dataid Id,const void* Data,size_t Size);
    uintptr_t (*Meta)(thisnode,dataid Id,datameta Meta);
    dataid (*FindParam)(thisnode,const tchar_t* Token);
    void* (*SetData)(thisnode,dataid Id, datatype Type,const void* Data);

} node_vmt;

#define Node_Enum(p,a) VMT_FUNC(p,node_vmt)->Enum(p,a)
#define Node_Get(p,a,b,c) VMT_FUNC(p,node_vmt)->Get(p,a,b,c)
#define Node_GET(p,a,b) VMT_FUNC(p,node_vmt)->Get(p,a,b,sizeof(*(b)))
#define Node_Set(p,a,b,c) VMT_FUNC(p,node_vmt)->Set(p,a,b,c)
#define Node_SET(p,a,b) VMT_FUNC(p,node_vmt)->Set(p,a,b,sizeof(*(b)))
#define Node_Trigger(p,a) VMT_FUNC(p,node_vmt)->Set(p,a,NULL,0)
#define Node_UnSet(p,a,b,c) VMT_FUNC(p,node_vmt)->UnSet(p,a,b,c)
#define Node_Meta(p,a,b)  VMT_FUNC(p,node_vmt)->Meta(p,a,b)
#define Node_FindParam(p,a) VMT_FUNC(p,node_vmt)->FindParam(p,a)
#define Node_SetData(p,i,t,d) VMT_FUNC(p,node_vmt)->SetData(p,i,t,d)

//-----------------------------------------------------------------

typedef struct pin
{
	node* Node;			
	dataid Id;

} pin;

typedef pin nodeevt;

typedef err_t (*notifyproc)(void* This, nodeevt* Evt);

typedef	void (*metafunc)(void* Referer, fourcc_t Meta, int32_t Stream, const tchar_t* Value);

typedef struct metanotify
{
  	metafunc Func;
	void* Referer;
    int Stream;

} metanotify;

typedef	err_t (*notifyfunc)(void* This,intptr_t Param,intptr_t Param2);

typedef	void (*freefunc)(void* This,void *Ptr);

typedef struct notify
{
	notifyfunc Func;
	void* This;

} notify;

typedef struct notifyex
{
	notifyfunc Func;
	void* This;
    freefunc Free;
    void* FreeCookie;

} notifyex;

typedef struct nodeexpr nodeexpr;

//---------------------------------------------------------------
// node priority classes

#define PRI_MINIMUM			1
#define PRI_DEFAULT		 1000
#define PRI_MAXIMUM		10000

//---------------------------------------------------------------
// functions managing node meta information

#define NODEMODULE_CLASS		FOURCC('N','M','O','D')
#define NODEMODULE_PATH         0x80

typedef struct nodeclass nodeclass;
typedef struct nodemodule nodemodule;

struct nodemodule
{
    node Base;
    nodemodule* Next;
	void* Module;
	void* Db;
	void* Func;
	uint8_t* Min;
	uint8_t* Max;
	datetime_t Stamp;
#if defined(CONFIG_DEBUG_LEAKS)
    array ClassRefs;
    void *LockRefs;
#endif
	uint8_t Found;
    uint8_t Config;
    uint8_t Changed;
};

struct nodecontext
{
    nodemodule Base;
	void* NodeLock;
    const void* NodeCache;
	array NodeSingleton; 
	array NodeClass; // ordered by id
    const cc_memheap* NodeHeap;
    const cc_memheap* NodeConstHeap;
    bool_t (*LoadModule)(nodecontext*,nodemodule*);
    void (*FreeModule)(nodecontext*,nodemodule*);
#if defined(CONFIG_MULTITHREAD)
    uintptr_t ThreadId;
    void* PostNotifyParam;
    bool_t (*PostNotify)(nodecontext*,node*,dataid); // returns if the message has been queued
#endif
    const tchar_t* (*ExternalStr)(nodecontext*,fourcc_t,int);
    //TODO: runtime datatype meta information handling...
    void (*ExprRelease)(nodeexpr*);
    size_t (*ExprSize)(nodeexpr*);
    void (*ExprDup)(node* Node, nodeexpr*, array* Dup);
#if defined(CONFIG_CORELUA)
    void *LuaCookie;
    void (*LuaRelease)(void* Cookie, int* Ref);
    void (*LuaAddRef)(void* Cookie, int* Ref);
#endif
	void (*ReportError)(nodecontext*, node* Node, fourcc_t MsgClass, int MsgNo, va_list Args);
#if defined(TARGET_PALMOS)
	fourcc_t ProjFourCC;
#endif
    int Build;
    int Revision;
    array Collect;
    bool_t InCollect;
    fourcc_t DynamicClass;
#if defined(TARGET_SYMBIAN)
    void *FsSession;
#endif
    uint16_t AppId;
};

#define NODECONTEXT_CLASS		        FOURCC('N','C','T','X')
#define NODECONTEXT_PROJECT_NAME        0x100
#define NODECONTEXT_PROJECT_VENDOR      0x101
#define NODECONTEXT_PROJECT_VERSION     0x102
#define NODECONTEXT_PROJECT_FOURCC      0x103
#define NODECONTEXT_PROJECT_HELP        0x104
#define NODECONTEXT_PROJECT_BUILD       0x105
#define NODECONTEXT_PROJECT_MIME        0x106
#define NODECONTEXT_PROJECT_APPID       0x107
#define NODECONTEXT_PROJECT_PATH        0x108

// notify
#define NODECONTEXT_CRASH               0x201

NODE_DLL void NodeContext_Init(nodecontext*,const nodemeta* Custom, const cc_memheap* Heap, const cc_memheap* ConstHeap);
NODE_DLL void NodeContext_Done(nodecontext*);
NODE_DLL bool_t NodeContext_Cleanup(nodecontext* p,bool_t Force);
NODE_DLL dataflags NodeContext_FindDataType(const tchar_t* Type, const tchar_t* Format);
NODE_DLL size_t NodeTypeSize(datatype); // TODO: use nodecontext* when switching to runtime datatype meta information handling...
NODE_DLL const tchar_t *NodeContext_TypeName(datatype Type);
NODE_DLL const tchar_t *NodeContext_UnitName(datatype Unit);

NODE_DLL void NodeRegisterClassEx(nodemodule*,const nodemeta*);
NODE_DLL fourcc_t NodeEnumClass(anynode*,array* List, fourcc_t Class); // List=NULL just returns the first class
NODE_DLL fourcc_t NodeEnumClassStr(anynode*,array* ListId, fourcc_t ClassId, int Id, const tchar_t* Str);
NODE_DLL void NodeEnumSingletons(anynode*,array* List);
NODE_DLL void NodeSingletonEvent(anynode*, dataid Cmd, nodemodule* Module); // Module = NULL for all modules

typedef	int (*nodeenumclassfilterrated)(void* Cookie, const nodeclass* Class);
NODE_DLL fourcc_t NodeEnumClassFilterRated(anynode*,array* List, fourcc_t Class, nodeenumclassfilterrated Func, void* Cookie);

NODE_DLL bool_t NodeIsClass(anynode*,fourcc_t Class, fourcc_t PartOfClass);
NODE_DLL const tchar_t* NodeStr2(anynode*,fourcc_t ClassId,int No);
NODE_DLL const tchar_t* NodeStrEx(anynode*,fourcc_t ClassId,int No);
NODE_DLL node* NodeCreate(anynode*,fourcc_t Class);
NODE_DLL node* NodeSingleton(anynode*,fourcc_t Class);

NODE_DLL const tchar_t* NodeClass_Str(anynode* AnyNode, const nodeclass*, int No);
NODE_DLL uintptr_t NodeClass_Meta(const nodeclass*,dataid Id,datameta Meta);
NODE_DLL int NodeClass_Priority(const nodeclass*);
NODE_DLL fourcc_t NodeClass_Parent(const nodeclass*);

NODE_DLL const nodeclass* NodeContext_FindClass(anynode*, fourcc_t Class);
NODE_DLL const nodeclass* NodeContext_FindClassEx(anynode*, fourcc_t Class, nodemodule* Prefered);

// functions for existing nodes

NODE_DLL void NodeDelete(node*); //TODO: use Node_Release() instead (but Node_Release doesn't support NULL)
NODE_DLL void Node_AddRef(thisnode This);
NODE_DLL void Node_Release(thisnode This);
NODE_DLL int NodeClassFlags(node*);
NODE_DLL void NodeClassSetPriority(node*,int Priority);
NODE_DLL nodemodule* NodeModule(node*);
NODE_DLL nodemodule* NodeClassModule(anynode*,fourcc_t Class);
NODE_DLL bool_t NodeFindDef(node*,const tchar_t* Token,datadef* Out);
NODE_DLL bool_t NodeDataDef(node* p, dataid Id, datadef* Out);
NODE_DLL void NodeEnumDef(node*,array* Out);
NODE_DLL void NodeParamName(node* p, dataid Id, tchar_t* Name, size_t NameLen);
NODE_DLL const tchar_t* NodeParamStr(const node* p,int No);
NODE_DLL void NodeReportError(anynode*, node* Node,fourcc_t MsgClass,int MsgNo,...);

NODE_DLL err_t Node_Constructor(anynode*,node* Node,size_t Size, fourcc_t ClassId);
NODE_DLL void Node_Destructor(node* Node);

NODE_DLL bool_t Node_Notify(node* Node, dataid Id); /// returns wether there were some receivers
#if defined(CONFIG_MULTITHREAD)
NODE_DLL bool_t Node_PostNotify(node* Node, dataid Id); // supports threading
#else
#define Node_PostNotify(x,y) Node_Notify(x,y)
#endif
NODE_DLL void Node_AddNotify(node*, dataid Id, notifyproc Func, void* Refered);
NODE_DLL void Node_AddNotify_Update(node*, dataid Id, notifyproc Func, void* Refered);
NODE_DLL void Node_RemoveNotify(node*, dataid Id, notifyproc Func, void* Refered);

NODE_DLL size_t Node_MaxDataSize(node*, dataid Id, dataflags Flags, int QueryType);
NODE_DLL bool_t Node_EqData(node*, dataid Id, dataflags Flags, const void* a, const void* b);
NODE_DLL size_t Node_DataSize(node*, dataid Id, datatype Type, const void* Data, int QueryType);
NODE_DLL void Node_RemoveData(node*, dataid Id, datatype Type);
NODE_DLL void* Node_AddData(node*, dataid Id, datatype Type, const void* Data);
NODE_DLL void* Node_GetData(const node*, dataid Id, datatype Type);
NODE_DLL err_t Node_ReadData(node* p, dataid Id, datatype Type, void* Data, size_t Size); /// fills with 0 if the dynamic data doesn't exist
NODE_DLL const tchar_t* Node_GetDataStr(const node*, dataid Id);
NODE_DLL datetime_t Node_GetDataDatetime(const node*, dataid Id);

// functions which should be exported in node DLLs

DLLEXPORT err_t DLLRegister(nodemodule*);
DLLEXPORT void DLLUnRegister(nodemodule*);
DLLEXPORT void DLLTest(void);
DLLEXPORT void DLLTest2(void);

NODE_DLL bool_t Node_IsPartOf(const void*, fourcc_t PartOfClass);
NODE_DLL err_t Node_Toggle(void* Node,dataid Id);
NODE_DLL void Node_Copy(node* Dst,node* Src,array* Dup);

typedef struct nodedup
{
    node* Orig;
    node* Dup;

} nodedup;

void NodeDup_Replace(array* Dup, node** Ptr);

#ifdef CONFIG_DEBUGCHECKS
#define VMT_FUNC(p,class_vmt) (assert((const void*)(p)!=NULL),(class_vmt*)((node*)(p))->VMT)
#else
#define VMT_FUNC(p,class_vmt) ((class_vmt*)((node*)(p))->VMT)
#endif

#define Node_Context(p) (VMT_FUNC(p,node_vmt)->Context)
#define Node_ClassId(p) (VMT_FUNC(p,node_vmt)->ClassId)

NODE_DLL const void* Node_InheritedVMT(node* p,fourcc_t ClassId);

#define INHERITED(p,class_vmt,classid) ((const class_vmt*)Node_InheritedVMT((node*)(p),classid))

void* NodeHeap_Alloc(anynode*, size_t Size);
void NodeHeap_Free(anynode*, void* Ptr, size_t Size);

typedef	int (*memcollect)(void* Cookie, int Level);

// collect levels

#define COLLECT_UNUSED	        0
#define COLLECT_SOFT		    100
#define COLLECT_HARD		    200

#define COLLECT_FOUND           -1 // return value when memory was collected on that level

NODE_DLL bool_t NodeHibernate(anynode*); //TODO: rename
NODE_DLL void Mem_AddCollector(anynode*, memcollect Func, void* Cookie);
NODE_DLL void Mem_RemoveCollector(anynode*, memcollect Func, void* Cookie);

#endif
