/*
 * $Id$
 * Copyright (c) 2008, Matroska Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Matroska Foundation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY The Matroska Foundation ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL The Matroska Foundation BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBEBML2_H
#define LIBEBML2_H

#include "node/node.h"
#include "file/file.h"

#if defined(CONFIG_EBML_UNICODE)
#include "parser/parser.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CONTEXT_LIBEBML_VERSION  0x400

#define EBML_MAX_VERSION    1
#define EBML_MAX_ID         4
#define EBML_MAX_SIZE       8

#define SCOPE_PARTIAL_DATA  0  // read all data, except inside some binary elements (useful for binary data with a (internal) header)
#define SCOPE_ALL_DATA      1
#define SCOPE_NO_DATA       2

// base classes
#define EBML_ELEMENT_CLASS   FOURCC('E','B','E','L')
#define EBML_BINARY_CLASS    FOURCC('E','B','I','N')
#define EBML_DATE_CLASS      FOURCC('E','B','D','T')
#define EBML_INTEGER_CLASS   FOURCC('E','B','I','T')
#define EBML_SINTEGER_CLASS  FOURCC('E','B','S','I')
#define EBML_FLOAT_CLASS     FOURCC('E','B','F','L')
#define EBML_MASTER_CLASS    FOURCC('E','B','M','T')
#define EBML_STRING_CLASS    FOURCC('E','B','S','G')
#define EBML_UNISTRING_CLASS FOURCC('E','B','S','U')
#define EBML_BOOLEAN_CLASS   FOURCC('E','B','B','O')

// global classes
#define EBML_CRC_CLASS       FOURCC('E','B','C','R')
#define EBML_VOID_CLASS      FOURCC('E','B','V','D')

#define EBML_DUMMY_ID        FOURCC('E','B','D','U')

#define EBML_ELEMENT_INFINITESIZE   0x100
#define EBML_ELEMENT_OBJECT         0x101

#if defined(EBML2_EXPORTS)
#define EBML_DLL DLLEXPORT
#elif defined(EBML2_IMPORTS)
#define EBML_DLL DLLIMPORT
#else
#define EBML_DLL
#endif

typedef struct ebml_context ebml_context;
typedef struct ebml_parser_context ebml_parser_context;
typedef struct ebml_semantic ebml_semantic;
typedef struct ebml_element ebml_element;
typedef struct ebml_crc ebml_crc;

struct ebml_semantic
{
    bool_t Mandatory;
    bool_t Unique;
    const ebml_context *eClass;
    int DisabledProfile; // PROFILE_MATROSKA_V1 and others
};

struct ebml_context
{
    fourcc_t Id;
    fourcc_t Class; // TODO: store a pointer to make creation faster 
    bool_t HasDefault;
    intptr_t DefaultValue;
    const char *ElementName;
    // TODO: create sub class so we don't have to assign it all the time
    const ebml_semantic *Semantic; // table with last element class set to NULL
    const ebml_semantic *GlobalContext; // table with last element class set to NULL
    void (*PostCreate)(ebml_element *p, const void *Cookie);
};

struct ebml_parser_context
{
    const ebml_context *Context;
    const ebml_parser_context *UpContext;
    filepos_t EndPosition;
};

struct ebml_element
{
    nodetree Base;
    filepos_t DataSize; // size of the data inside the element
    filepos_t ElementPosition;
    filepos_t SizePosition; // TODO: is this needed since we have the ElementPosition and SizeLength ?
    const ebml_context *Context;
    int DefaultSize;
    int8_t SizeLength;
    boolmem_t bValueIsSet;
    boolmem_t bDefaultIsSet;
    boolmem_t bNeedDataSizeUpdate;
};

typedef struct ebml_element_vmt
{
    nodetree_vmt Base;
    bool_t (*ValidateSize)(const void*);
    err_t (*ReadData)(void*, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC);
#if defined(CONFIG_EBML_WRITING)
    err_t (*RenderData)(void*, stream *Output, bool_t bForceRender, bool_t bWithDefault, filepos_t *Rendered);
#endif
    bool_t (*IsDefaultValue)(const void*);
    bool_t (*DefaultIsSet)(const void*);
    filepos_t (*UpdateDataSize)(void*, bool_t bWithDefault, bool_t bForceRender);
    bool_t (*NeedsDataSizeUpdate)(const void*, bool_t bWithDefault);
    int (*Cmp)(const void*, const void*);
    ebml_element *(*Copy)(const void*, const void *Cookie);
    
    // internal call only
    void (*PostCreate)(void*);

} ebml_element_vmt;

#define EBML_ElementValidateSize(p)          VMT_FUNC(p,ebml_element_vmt)->ValidateSize(p)
#define EBML_ElementReadData(p,i,c,d,s,r)    VMT_FUNC(p,ebml_element_vmt)->ReadData(p,i,c,d,s,r)
#define EBML_ElementRenderData(p,s,f,k,r)    VMT_FUNC(p,ebml_element_vmt)->RenderData(p,s,f,k,r)
#define EBML_ElementIsDefaultValue(p)        VMT_FUNC(p,ebml_element_vmt)->IsDefaultValue(p)
#define EBML_ElementUpdateSize(p,k,f)        VMT_FUNC(p,ebml_element_vmt)->UpdateDataSize(p,k,f)
#define EBML_ElementNeedsDataSizeUpdate(p,d) VMT_FUNC(p,ebml_element_vmt)->NeedsDataSizeUpdate(p,d)
#define EBML_ElementCmp(p,e)                 VMT_FUNC(p,ebml_element_vmt)->Cmp(p,e)
#define EBML_ElementCopy(p,c)                VMT_FUNC(p,ebml_element_vmt)->Copy(p,c)

typedef struct ebml_master
{
    ebml_element Base;
    int CheckSumStatus; // 0: not set, 1: requested/invalid, 2: verified

} ebml_master;

typedef struct ebml_string
{
    ebml_element Base;
    const char *Buffer; // UTF-8 internal storage

} ebml_string;

typedef struct ebml_integer
{
    ebml_element Base;
    int64_t Value;

} ebml_integer;

typedef struct ebml_float
{
    ebml_element Base;
    double Value;

} ebml_float;

typedef struct ebml_binary
{
    ebml_element Base;
    array Data;

} ebml_binary;

typedef ebml_integer ebml_date;

typedef struct ebml_dummy
{
    ebml_binary Base;
    ebml_context DummyContext;

} ebml_dummy;

EBML_DLL err_t EBML_Init(nodecontext *p);
EBML_DLL err_t EBML_Done(nodecontext *p);

EBML_DLL ebml_element *EBML_ElementCreate(anynode *Any, const ebml_context *Context, bool_t SetDefault, const void *Cookie);

EBML_DLL ebml_element *EBML_FindNextId(stream *Input, const ebml_context *Context, size_t MaxDataSize);
EBML_DLL ebml_element *EBML_FindNextElement(stream *Input, const ebml_parser_context *Context, int *UpperLevels, bool_t AllowDummy);
EBML_DLL int EBML_CodedSizeLength(filepos_t Length, uint8_t SizeLength, bool_t bSizeIsFinite); // TODO: turn into a macro ?
EBML_DLL int EBML_CodedSizeLengthSigned(filepos_t Length, uint8_t SizeLength); // TODO: turn into a macro ?
EBML_DLL int EBML_CodedValueLength(filepos_t Length, size_t CodedSize, uint8_t *OutBuffer, bool_t bSizeIsFinite); // TODO: turn into a macro ?
EBML_DLL int EBML_CodedValueLengthSigned(filepos_t Length, size_t CodedSize, uint8_t * OutBuffer); // TODO: turn into a macro ?
EBML_DLL filepos_t EBML_ReadCodedSizeValue(const uint8_t *InBuffer, size_t *BufferSize, filepos_t *SizeUnknown);
EBML_DLL filepos_t EBML_ReadCodedSizeSignedValue(const uint8_t *InBuffer, size_t *BufferSize, filepos_t *SizeUnknown);

EBML_DLL void EBML_ElementGetName(const ebml_element *Element, tchar_t *Out, size_t OutLen);
EBML_DLL const char *EBML_ElementGetClassName(const ebml_element *Element);

EBML_DLL filepos_t EBML_ElementFullSize(const ebml_element *Element, bool_t bWithDefault);

EBML_DLL ebml_element *EBML_ElementSkipData(ebml_element *Element, stream *Input, const ebml_parser_context *Context, ebml_element *TestReadElt, bool_t AllowDummy);
EBML_DLL bool_t EBML_ElementIsFiniteSize(const ebml_element *Element);
EBML_DLL void EBML_ElementSetInfiniteSize(const ebml_element *Element, bool_t Set);
EBML_DLL bool_t EBML_ElementInfiniteForceSize(ebml_element *Element, filepos_t NewSize);
EBML_DLL bool_t EBML_ElementIsDummy(const ebml_element *Element);

EBML_DLL fourcc_t EBML_ElementClassID(const ebml_element *Element);
static INLINE filepos_t EBML_ElementPositionData(const ebml_element *Element)
{
    if (!EBML_ElementIsFiniteSize(Element))
    {
        if (!Element->SizeLength)
            return INVALID_FILEPOS_T;
        return Element->SizePosition + Element->SizeLength;
    }
    else
        return Element->SizePosition + EBML_CodedSizeLength(Element->DataSize,Element->SizeLength,1);
}
static INLINE filepos_t EBML_ElementPositionEnd(const ebml_element *Element)
{
    if (!EBML_ElementIsFiniteSize(Element))
        return INVALID_FILEPOS_T; // the end position is unknown
    else
        return Element->SizePosition + EBML_CodedSizeLength(Element->DataSize,Element->SizeLength,1) + Element->DataSize;
}

#if defined(CONFIG_EBML_WRITING)
// TODO: replace the list of bools by flags ?
EBML_DLL err_t EBML_ElementRender(ebml_element *Element, stream *Output, bool_t bWithDefault, bool_t bKeepPosition, bool_t bForceRender, filepos_t *Rendered);
EBML_DLL err_t EBML_ElementRenderHead(ebml_element *Element, stream *Output, bool_t bKeepPosition, filepos_t *Rendered);
#endif

// type specific routines
EBML_DLL ebml_element *EBML_MasterFindFirstElt(ebml_master *Element, const ebml_context *Context, bool_t bCreateIfNull, bool_t SetDefault);
EBML_DLL err_t EBML_MasterAppend(ebml_master *Element, ebml_element *Append);
EBML_DLL err_t EBML_MasterRemove(ebml_master *Element, ebml_element *Remove);
EBML_DLL ebml_element *EBML_MasterFindNextElt(ebml_master *Element, const ebml_element *Current, bool_t bCreateIfNull, bool_t SetDefault);
EBML_DLL ebml_element *EBML_MasterAddElt(ebml_master *Element, const ebml_context *Context, bool_t SetDefault);
EBML_DLL size_t EBML_MasterCount(const ebml_master *Element);
EBML_DLL void EBML_MasterClear(ebml_master *Element); // clear the list (the children and not freed)
EBML_DLL void EBML_MasterErase(ebml_master *Element);
EBML_DLL void EBML_MasterMandatory(ebml_master *Element, bool_t SetDefault); // add the mandatory elements
EBML_DLL bool_t EBML_MasterCheckMandatory(const ebml_master *Element, bool_t SetDefault);
EBML_DLL void EBML_MasterSort(ebml_master *Element, arraycmp Cmp, const void* CmpParam);
EBML_DLL bool_t EBML_MasterUseChecksum(ebml_master *Element, bool_t Use);
EBML_DLL bool_t EBML_MasterIsChecksumValid(const ebml_master *Element);
#define EBML_MasterGetChild(e,c)   EBML_MasterFindFirstElt(e,c,1,1)
#define EBML_MasterFindChild(e,c)  EBML_MasterFindFirstElt((ebml_master*)e,c,0,0)
#define EBML_MasterNextChild(e,c)  EBML_MasterFindNextElt((ebml_master*)e,(ebml_element*)c,0,0)
#define EBML_MasterChildren(p)     ((ebml_element*)NodeTree_Children(p))
#define EBML_MasterNext(p)         ((ebml_element*)NodeTree_Next(p))
#define EBML_ElementParent(p)      ((ebml_element*)NodeTree_Parent(p))

#if !defined(NDEBUG)
#define EBML_IntegerValue(p)       (assert(((ebml_element*)p)->bValueIsSet), ((ebml_integer*)p)->Value)
#else
#define EBML_IntegerValue(p)       ((ebml_integer*)p)->Value
#endif
EBML_DLL err_t EBML_IntegerSetValue(ebml_integer *Element, int64_t Value);

EBML_DLL err_t EBML_StringSetValue(ebml_string *Element,const char *Value);
#if defined(CONFIG_EBML_UNICODE)
EBML_DLL err_t EBML_UniStringSetValue(ebml_string *Element,const tchar_t *Value);
#endif
EBML_DLL void EBML_StringGet(ebml_string *Element,tchar_t *Out, size_t OutLen);

EBML_DLL datetime_t EBML_DateTime(const ebml_date *Element);
EBML_DLL err_t EBML_DateSetDateTime(ebml_date *Element, datetime_t Date);

EBML_DLL err_t EBML_BinarySetData(ebml_binary *Element, const uint8_t *Data, size_t DataSize);
EBML_DLL const uint8_t *EBML_BinaryGetData(ebml_binary *Element);

#if defined(CONFIG_EBML_WRITING)
EBML_DLL void EBML_VoidSetSize(ebml_element *Void, filepos_t);
EBML_DLL filepos_t EBML_VoidReplaceWith(ebml_element *Void, ebml_element *Replaced, stream *Output, bool_t ComeBackAfterward, bool_t bWithDefault);
#endif
EBML_DLL size_t EBML_FillBufferID(uint8_t *Buffer, size_t BufSize, fourcc_t Id);
EBML_DLL size_t EBML_IdToString(tchar_t *Out, size_t OutLen, fourcc_t Id);
EBML_DLL fourcc_t EBML_BufferToID(const uint8_t *Buffer);

INTERNAL_C_API size_t GetIdLength(fourcc_t Id);

#if defined(EBML_LEGACY_API)
#define CONTEXT_CONST
#else
#define CONTEXT_CONST const
#endif

// EBML contexts
extern CONTEXT_CONST ebml_context EBML_ContextHead;
extern CONTEXT_CONST ebml_context EBML_ContextDummy;
extern CONTEXT_CONST ebml_context EBML_ContextVersion;
extern CONTEXT_CONST ebml_context EBML_ContextReadVersion;
extern CONTEXT_CONST ebml_context EBML_ContextMaxIdLength;
extern CONTEXT_CONST ebml_context EBML_ContextMaxSizeLength;
extern CONTEXT_CONST ebml_context EBML_ContextDocType;
extern CONTEXT_CONST ebml_context EBML_ContextDocTypeVersion;
extern CONTEXT_CONST ebml_context EBML_ContextDocTypeReadVersion;

extern CONTEXT_CONST ebml_context EBML_ContextEbmlVoid;
extern CONTEXT_CONST ebml_context EBML_ContextEbmlCrc32;

//extern const ebml_context EBML_ContextGlobals;
extern const ebml_semantic EBML_SemanticGlobals[];

#ifdef __cplusplus
}
#endif

#endif /* LIBEBML2_H */
