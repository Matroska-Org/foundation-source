/*
 * $Id: legacy.cpp 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Steve Lhomme
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

#include <cassert>

#include "ebml/EbmlElement.h"
#include "ebml/EbmlMaster.h"
#include "ebml/EbmlString.h"
#include "ebml/EbmlUnicodeString.h"
#include "ebml/EbmlUInteger.h"
#include "ebml/EbmlSInteger.h"
#include "ebml/EbmlFloat.h"
#include "ebml/EbmlDate.h"
#include "ebml/EbmlStream.h"
#include "ebml/EbmlVoid.h"
#include "ebml/EbmlCrc32.h"
#include "ebml/EbmlHead.h"
#include "ebml/EbmlSubHead.h"
#include "ebml/EbmlContexts.h"

#include "ebml2_legacy_project.h"
#include "ebml/EbmlVersion.h"

#include "ebml/ebml.h"

#define IOCALLBACK_STREAM_CLASS FOURCC('I','O','C','B')

#define Ebml_Children(x)  (ebml_element*)NodeTree_Children(x)
#define Ebml_Next(x)      (ebml_element*)NodeTree_Next(x)

fourcc_t GetEbmlFourCC(const ebml_element* p)
{
    return p->Context->Id;
}

fourcc_t GetMasterFourCC()
{
    return EBML_MASTER_CLASS;
}

fourcc_t GetUIntegerFourCC()
{
    return EBML_INTEGER_CLASS;
}

fourcc_t GetSIntegerFourCC()
{
    return EBML_SINTEGER_CLASS;
}

fourcc_t GetStringFourCC()
{
    return EBML_STRING_CLASS;
}

fourcc_t GetUniStringFourCC()
{
    return EBML_UNISTRING_CLASS;
}

fourcc_t GetBinaryFourCC()
{
    return EBML_BINARY_CLASS;
}

fourcc_t GetFloatFourCC()
{
    return EBML_FLOAT_CLASS;
}

fourcc_t GetDateFourCC()
{
    return EBML_DATE_CLASS;
}

const ebml_semantic & GetGlobalEBMLSemantic()
{
    return *EBML_SemanticGlobals;
}

static nodecontext ccContext;

using namespace LIBEBML_NAMESPACE;

struct stream_io
{
    stream Base;
    IOCallback *cpp;
};

static filepos_t Seek(stream_io* p,filepos_t Pos,int SeekMode) 
{
    if (SeekMode == SEEK_CUR)
    {
        if (Pos!=0)
            p->cpp->setFilePointer(Pos,seek_current);
        return (filepos_t)p->cpp->getFilePointer();
    }
    if (SeekMode == SEEK_SET)
    {
        p->cpp->setFilePointer(Pos);
        return Pos;
    }
    if (SeekMode == SEEK_END)
    {
        p->cpp->setFilePointer(Pos,seek_end);
        return (filepos_t)p->cpp->getFilePointer();
    }
    return INVALID_FILEPOS_T;
}

static err_t Write(stream_io* p,const void* Data,size_t Size, size_t* Written)
{
    size_t _Written;
    if (!Written)
        Written = &_Written;
    _Written = p->cpp->write(Data,Size);
    if (Written)
        *Written = _Written;
    return (_Written==Size)?ERR_NONE:ERR_DEVICE_ERROR;
}

META_START(IOCallback_Class,IOCALLBACK_STREAM_CLASS)
META_CLASS(FLAGS,CFLAG_LOCAL)
META_CLASS(SIZE,sizeof(stream_io))
META_VMT(TYPE_FUNC,stream_vmt,Seek,Seek)
META_VMT(TYPE_FUNC,stream_vmt,Write,Write)
META_END(STREAM_CLASS)

void ebml_init()
{
    NodeContext_Init(&ccContext,NULL,NULL,NULL);
    EBML_Init(&ccContext);
    NodeRegisterClassEx((nodemodule*)&ccContext,IOCallback_Class);

    EbmlHead::EBML_ContextEbmlHead.PostCreate = EbmlHead::PostCreate;
    ::EBML_ContextHead.PostCreate = EbmlHead::PostCreate;
    EDocType::EBML_ContextEDocType.PostCreate = EDocType::PostCreate;
    ::EBML_ContextDocType.PostCreate = EDocType::PostCreate;
    EVersion::EBML_ContextEVersion.PostCreate = EVersion::PostCreate;
    ::EBML_ContextVersion.PostCreate = EVersion::PostCreate;
    EReadVersion::EBML_ContextEReadVersion.PostCreate = EReadVersion::PostCreate;
    ::EBML_ContextReadVersion.PostCreate = EReadVersion::PostCreate;
    EDocTypeVersion::EBML_ContextEDocTypeVersion.PostCreate = EDocTypeVersion::PostCreate;
    ::EBML_ContextDocTypeVersion.PostCreate = EDocTypeVersion::PostCreate;
    EDocTypeReadVersion::EBML_ContextEDocTypeReadVersion.PostCreate = EDocTypeReadVersion::PostCreate;
    ::EBML_ContextDocTypeReadVersion.PostCreate = EDocTypeReadVersion::PostCreate;
    EMaxSizeLength::EBML_ContextEMaxSizeLength.PostCreate = EMaxSizeLength::PostCreate;
    ::EBML_ContextMaxSizeLength.PostCreate = EMaxSizeLength::PostCreate;
    EMaxIdLength::EBML_ContextEMaxIdLength.PostCreate  = EMaxIdLength::PostCreate;
    ::EBML_ContextMaxIdLength.PostCreate  = EMaxIdLength::PostCreate;
}

void ebml_done()
{
    EBML_Done(&ccContext);
    NodeContext_Done(&ccContext);
}


namespace LIBEBML_NAMESPACE
{

EbmlElement & DummyCreate()
{
assert(0);
return *static_cast<EbmlElement*>(NULL);
}

DEFINE_EBML_BINARY_GLOBAL(EbmlCrc32, 0xBF, 1, "EBMLCrc32\0ratamadabapa");
DEFINE_EBML_VOID_GLOBAL(EbmlVoid,  0xEC, 1, "EBMLVoid");

ebml_context EbmlHead::EBML_ContextEbmlHead                       = ::EBML_ContextHead; // dirty copy
ebml_context EDocType::EBML_ContextEDocType                       = ::EBML_ContextDocType; // dirty copy
ebml_context EVersion::EBML_ContextEVersion                       = ::EBML_ContextVersion; // dirty copy
ebml_context EReadVersion::EBML_ContextEReadVersion               = ::EBML_ContextReadVersion; // dirty copy
ebml_context EDocTypeVersion::EBML_ContextEDocTypeVersion         = ::EBML_ContextDocTypeVersion; // dirty copy
ebml_context EDocTypeReadVersion::EBML_ContextEDocTypeReadVersion = ::EBML_ContextDocTypeReadVersion; // dirty copy
ebml_context EMaxSizeLength::EBML_ContextEMaxSizeLength           = ::EBML_ContextMaxSizeLength; // dirty copy
ebml_context EMaxIdLength::EBML_ContextEMaxIdLength               = ::EBML_ContextMaxIdLength; // dirty copy

const ebml_context & EbmlHead::GetContext() { return EBML_ContextEbmlHead; }
const ebml_context & EDocType::GetContext() { return EBML_ContextEDocType; }
const ebml_context & EVersion::GetContext() { return EBML_ContextEVersion; }
const ebml_context & EReadVersion::GetContext() { return EBML_ContextEReadVersion; }
const ebml_context & EDocTypeVersion::GetContext() { return EBML_ContextEDocTypeVersion; }
const ebml_context & EDocTypeReadVersion::GetContext() { return EBML_ContextEDocTypeReadVersion; }
const ebml_context & EMaxSizeLength::GetContext() { return EBML_ContextEMaxSizeLength; }
const ebml_context & EMaxIdLength::GetContext() { return EBML_ContextEMaxIdLength; }

void EbmlHead::PostCreate(ebml_element *p, const void *Cookie)            { if (!Cookie) Cookie=new EbmlHead(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }
void EDocType::PostCreate(ebml_element *p, const void *Cookie)            { if (!Cookie) Cookie=new EDocType(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }
void EVersion::PostCreate(ebml_element *p, const void *Cookie)            { if (!Cookie) Cookie=new EVersion(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }
void EReadVersion::PostCreate(ebml_element *p, const void *Cookie)        { if (!Cookie) Cookie=new EReadVersion(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }
void EDocTypeVersion::PostCreate(ebml_element *p, const void *Cookie)     { if (!Cookie) Cookie=new EDocTypeVersion(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }
void EDocTypeReadVersion::PostCreate(ebml_element *p, const void *Cookie) { if (!Cookie) Cookie=new EDocTypeReadVersion(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }
void EMaxSizeLength::PostCreate(ebml_element *p, const void *Cookie)      { if (!Cookie) Cookie=new EMaxSizeLength(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }
void EMaxIdLength::PostCreate(ebml_element *p, const void *Cookie)        { if (!Cookie) Cookie=new EMaxIdLength(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }

size_t CodedSizeLength(filepos_t Length, size_t SizeLength, bool bSizeIsFinite)
{
assert(0);
    return 0;
}

size_t CodedValueLength(filepos_t Length, size_t CodedSize, binary * OutBuffer)
{
assert(0);
    return 0;
}

size_t CodedSizeLengthSigned(filepos_t Length, size_t SizeLength)
{
assert(0);
    return 0;
}

size_t CodedValueLengthSigned(filepos_t Length, size_t CodedSize, binary * OutBuffer)
{
assert(0);
    return 0;
}

filepos_t ReadCodedSizeValue(const binary * InBuffer, uint32_t & BufferSize, uint64_t & SizeUnknown)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t ReadCodedSizeSignedValue(const binary * InBuffer, uint32_t & BufferSize, uint64_t & SizeUnknown)
{
assert(0);
    return INVALID_FILEPOS_T;
}

const EbmlSemanticContext & GetEbmlGlobal_Context()
{
assert(0);
    return *((EbmlSemanticContext*)NULL);
}

big_int16::big_int16(int16_t)
{
assert(0);
}

big_int16::big_int16()
{
assert(0);
}

big_int16::operator int16() const
{
assert(0);
    return 0;
}

void big_int16::Fill(binary *Buffer) const
{
assert(0);
}

void big_int16::Eval(const binary *Buffer)
{
assert(0);
}



/*****************
 * EbmlElement
 ****************/
EbmlElement & CreateEbmlElement(const ebml_semantic &Semantic)
{
assert(0);
return *static_cast<EbmlElement*>(NULL);
}

err_t EbmlElement::Deleting(EbmlElement *p, nodeevt *Evt)
{
    p->Node = NULL;
    delete p;
    return ERR_NONE;
}

EbmlElement::EbmlElement(const ebml_context & Context, ebml_element *WithNode)
:Node(WithNode)
{
    if (!WithNode)
        Node = EBML_ElementCreate(&ccContext,&Context,0,this);
    if (!Node)
    {
        // TODO: throw some error
    }
    Node_AddNotify((node*)Node,NODE_DELETING,(notifyproc)Deleting,this);

}

EbmlElement::~EbmlElement()
{
    if (Node)
    {
        Node_RemoveNotify((node*)Node,NODE_DELETING,(notifyproc)Deleting,this);
        NodeDelete((node*)Node);
    }
}

EbmlElement * EbmlElement::SkipData(EbmlStream & DataStream, const EbmlSemanticContext & Context, EbmlElement * TestReadElt, bool AllowDummyElt)
{
assert(0);
    return NULL;
}

filepos_t EbmlElement::UpdateSize(bool bWithDefault, bool bForceRender)
{
    return EBML_ElementUpdateSize(Node, bWithDefault, bForceRender);
}

bool EbmlElement::IsFiniteSize() const
{
assert(0);
    return false;
}

size_t EbmlElement::GetSizeLength() const
{
assert(0);
    return 0;
}

filepos_t EbmlElement::GetSize() const
{
    return Node->Size;
}

bool EbmlElement::ValueIsSet() const
{
    return Node->bValueIsSet!=0;
}

void EbmlElement::SetValueIsSet(bool Set)
{
    Node->bValueIsSet = Set;
}

bool EbmlElement::SetSizeInfinite(bool bIsInfinite)
{
    EBML_ElementSetInfiniteSize(Node,bIsInfinite);
    return true;
}

void EbmlElement::SetDefaultSize(filepos_t aDefaultSize)
{
assert(0);
}

void EbmlElement::SetSize_(filepos_t)
{
assert(0);
}

bool EbmlElement::ForceSize(filepos_t NewSize)
{
    return EBML_ElementInfiniteForceSize(Node,NewSize)?true:false;
}

void EbmlElement::SetSizeLength(size_t Size)
{
    Node->SizeLength = (int8_t)Size;
}

size_t EbmlElement::HeadSize() const
{
    filepos_t Result = EBML_ElementFullSize(Node,1);
    if (Result != INVALID_FILEPOS_T)
        Result -= Node->Size; 
    return Result;
}

filepos_t EbmlElement::ElementSize(bool bWithDefault) const
{
    return EBML_ElementFullSize(Node,bWithDefault);
}

filepos_t EbmlElement::GetElementPosition() const
{
    return Node->ElementPosition;
}

filepos_t EbmlElement::Render(IOCallback & output, bool bWithDefault, bool bKeepPosition, bool bForceRender)
{
    filepos_t Rendered = INVALID_FILEPOS_T;
    if (EBML_ElementRender(Node, output.GetStream(), bWithDefault, bKeepPosition, bForceRender, &Rendered)!=ERR_NONE)
        return INVALID_FILEPOS_T;
    return Rendered;
}

filepos_t EbmlElement::OverwriteHead(IOCallback & output, bool bKeepPosition)
{
    if (Node->ElementPosition == INVALID_FILEPOS_T)
		return INVALID_FILEPOS_T; // the element has not been written

	filepos_t CurrentPosition = (filepos_t)output.getFilePointer();
	output.setFilePointer(GetElementPosition());
    if (EBML_ElementRenderHead(Node, output.GetStream(), 1, 1, bKeepPosition, NULL)!=ERR_NONE)
        return INVALID_FILEPOS_T;
    if (!bKeepPosition)
    {
	    output.setFilePointer(CurrentPosition);
	    return CurrentPosition;
    }
    return Node->ElementPosition;
}

void EbmlElement::Read(EbmlStream & inDataStream, const ebml_parser_context & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt, ScopeMode ReadFully)
{
assert(0);
}

void EbmlElement::Read(EbmlStream & inDataStream, const EbmlSemanticContext & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt, ScopeMode ReadFully)
{
assert(0);
}

const char* EbmlElement::DebugName() const
{
assert(0);
    return NULL;
}

bool EbmlElement::IsSmallerThan(const EbmlElement *Cmp) const
{
	return EbmlId(*this) == EbmlId(*Cmp);
}


/*****************
 * EbmlSemanticContext
 ****************/
EbmlSemanticContext::EbmlSemanticContext(const ebml_context & _Context)
:Context(_Context)
,Size(0)
{
    const ebml_semantic *s = Context.Semantic;
    while (s && s->eClass!=NULL)
    {
        ++Size;
        ++s;
    }
}

const EbmlSemantic & EbmlSemanticContext::GetSemantic(size_t i) const
{
    assert(i < Size);
    return Context.Semantic[i];
}

bool EbmlSemanticContext::operator!=(const EbmlSemanticContext & Elt) const
{
	return (Size != Elt.Size) || (Context.Id != Elt.Context.Id) || (Context.Semantic != Elt.Context.Semantic); // TODO: handle more
}

const ebml_context * EbmlSemanticContext::GetContext() const
{
    return &Context;
}

EbmlSemanticContext::~EbmlSemanticContext()
{
//    delete pContext;
}


/*****************
 * EbmlId
 ****************/
EbmlId::EbmlId(uint32_t Id, size_t Size)
:Value(Id)
,Length(Size)
{}

EbmlId::EbmlId(const binary aValue[4], size_t aLength)
{
assert(0);
}

EbmlId::EbmlId(fourcc_t Id)
:Value(Id)
{
    // TODO: handle the size
}

EbmlId::operator fourcc_t() const
{
    return Value;
}

void EbmlId::Fill(binary * Buffer) const
{
assert(0);
}


/*****************
 * EbmlMaster_itr
 ****************/
bool EbmlMaster_itr::operator!=(const EbmlMaster_itr &) const
{
assert(0);
return false;
}

EbmlElement* EbmlMaster_itr::operator*() const
{
assert(0);
return NULL;
}

EbmlMaster_itr& EbmlMaster_itr::operator++()
{
assert(0);
    return *this;
}


/*****************
 * EbmlMaster
 ****************/
EbmlMaster::EbmlMaster(struct ebml_context const &Context, ebml_element *WithNode)
:EbmlElement(Context, WithNode)
{
}

EbmlMaster_itr EbmlMaster::begin() const
{
assert(0);
return *static_cast<EbmlMaster_itr*>(NULL);
}

EbmlMaster_itr EbmlMaster::end() const
{
assert(0);
return *static_cast<EbmlMaster_itr*>(NULL);
}

bool EbmlMaster::CheckMandatory() const
{
assert(0);
    return true;
}

filepos_t EbmlMaster::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlMaster::WriteHead(IOCallback & output, size_t SizeLength, bool bWithDefault)
{
    filepos_t Rendered = INVALID_FILEPOS_T;
    Node->SizeLength = (int8_t)SizeLength;
    if (EBML_ElementRenderHead(Node, output.GetStream(), 1, bWithDefault, 0, &Rendered)!=ERR_NONE)
        return INVALID_FILEPOS_T;
    return Rendered;
}

static int EbmlCmp(const ebml_element* Element, const ebml_element** a,const ebml_element** b)
{
    EbmlElement *A=NULL;
    EbmlElement *B=NULL;
    if (Node_Get((node*)*a,EBML_ELEMENT_OBJECT,&A,sizeof(A))!=ERR_NONE)
    {
        assert(0);
    }
    if (Node_Get((node*)*b,EBML_ELEMENT_OBJECT,&B,sizeof(B))!=ERR_NONE)
    {
        assert(0);
    }
    if (B->IsSmallerThan(A))
        return -1;
    else
        return 0;
}

void EbmlMaster::Sort()
{
    EBML_MasterSort(Node, (arraycmp)EbmlCmp, this);
}

void EbmlMaster::RemoveAll()
{
    EBML_MasterClear(Node);
}

size_t EbmlMaster::ListSize() const
{
    return EBML_MasterCount(Node);
}

bool EbmlMaster::InsertElement(EbmlElement & element, size_t position)
{
assert(0);
    return false;
}

bool EbmlMaster::PushElement(EbmlElement & element)
{
    return EBML_MasterAppend(Node,element.GetNode())==ERR_NONE;
}

EbmlElement *EbmlMaster::AddNewElt(const EbmlCallbacks & Kind)
{
    ebml_element *i = EBML_MasterAddElt(Node,&Kind,0);
    if (i)
    {
        EbmlElement *Result=NULL;
        if (Node_Get(i,EBML_ELEMENT_OBJECT,&Result,sizeof(Result))!=ERR_NONE)
        {
            assert(0);
        }
        return Result;
    }
    return NULL;
}

EbmlElement *EbmlMaster::FindElt(const ebml_context & Kind) const
{
    ebml_element *i = EBML_MasterFindChild(Node,&Kind);
    if (i)
    {
        EbmlElement *Result=NULL;
        if (Node_Get(i,EBML_ELEMENT_OBJECT,&Result,sizeof(Result))!=ERR_NONE)
        {
            assert(0);
        }
        return Result;
    }
    return NULL;
}

EbmlElement *EbmlMaster::FindFirstElt(const ebml_context & Kind) const
{
    ebml_element *i = EBML_MasterFindFirstElt(Node,&Kind,0,0);
    if (i)
    {
        EbmlElement *Result=NULL;
        if (Node_Get(i,EBML_ELEMENT_OBJECT,&Result,sizeof(Result))!=ERR_NONE)
        {
            assert(0);
        }
        return Result;
    }
    return NULL;
}

EbmlElement *EbmlMaster::FindFirstElt(const ebml_context & Kind, const bool bCreateIfNull) const
{
    ebml_element *i = EBML_MasterFindFirstElt(Node,&Kind,bCreateIfNull,0);
    if (i)
    {
        EbmlElement *Result=NULL;
        if (Node_Get(i,EBML_ELEMENT_OBJECT,&Result,sizeof(Result))!=ERR_NONE)
        {
            assert(0);
        }
        return Result;
    }
    return NULL;
}

EbmlElement *EbmlMaster::FindNextElt(const EbmlElement & Past) const
{
assert(0);
    return NULL;
}

EbmlElement *EbmlMaster::FindNextElt(const EbmlElement & Past, const bool bCreateIfNull)
{
    ebml_element *i = EBML_MasterFindNextElt(Node,Past.GetNode(),bCreateIfNull,0);
    if (i)
    {
        EbmlElement *Result=NULL;
        if (Node_Get(i,EBML_ELEMENT_OBJECT,&Result,sizeof(Result))!=ERR_NONE)
        {
            assert(0);
        }
        return Result;
    }
    return NULL;
}

void EbmlMaster::Remove(size_t Index)
{
assert(0);
}

EbmlElement * EbmlMaster::operator[](size_t position)
{
    ebml_element *i = Ebml_Children(Node);
    while(i && position--)
        i = Ebml_Next(i);
    if (i)
    {
        EbmlElement *Result=NULL;
        if (Node_Get(i,EBML_ELEMENT_OBJECT,&Result,sizeof(Result))!=ERR_NONE)
        {
            assert(0);
        }
        return Result;
    }
    return NULL;
}

const EbmlElement * EbmlMaster::operator[](size_t position) const
{
    ebml_element *i = Ebml_Children(Node);
    while(i && position--)
        i = Ebml_Next(i);
    if (i)
    {
        EbmlElement *Result=NULL;
        if (Node_Get(i,EBML_ELEMENT_OBJECT,&Result,sizeof(Result))!=ERR_NONE)
        {
            assert(0);
        }
        return Result;
    }
    return NULL;
}

EbmlElement * EbmlMaster::Clone() const
{
assert(0);
    return NULL;
}


/*****************
 * EbmlBinary
 ****************/
void EbmlBinary::CopyBuffer(const void *Buffer, size_t BufferSize)
{
    EBML_BinarySetData((ebml_binary*)Node, (const uint8_t*)Buffer, BufferSize);
}

void EbmlBinary::SetBuffer(const binary *Buffer, size_t BufferSize)
{
assert(0);
}

binary* EbmlBinary::GetBuffer() const
{
assert(0);
return NULL;
}

filepos_t EbmlBinary::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlElement * EbmlBinary::Clone() const
{
assert(0);
    return NULL;
}


/*****************
 * EbmlString
 ****************/
EbmlString::EbmlString(const ebml_context &ec, const char *DefaultValue, ebml_element *WithNode)
:EbmlElement(ec, WithNode)
{
    Node->bDefaultIsSet = 1;
}

EbmlString::EbmlString(const ebml_context &ec, ebml_element *WithNode)
:EbmlElement(ec, WithNode)
{
}

filepos_t EbmlString::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlString & EbmlString::operator=(const std::string & Value)
{
    EBML_StringSetValue((ebml_string*)Node,Value.c_str());
    return *this;
}

EbmlString & EbmlString::operator=(const char *Value)
{
    EBML_StringSetValue((ebml_string*)Node,Value);
    return *this;
}

EbmlString::operator const std::string() const
{
assert(0);
return *static_cast<std::string*>(NULL);
}

EbmlElement * EbmlString::Clone() const
{
assert(0);
    return NULL;
}


/*****************
 * EbmlUnicodeString
 ****************/
EbmlUnicodeString::EbmlUnicodeString(const ebml_context &ec, ebml_element *WithNode)
:EbmlElement(ec, WithNode)
{
}

filepos_t EbmlUnicodeString::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlUnicodeString & EbmlUnicodeString::operator=(const UTFstring &val)
{
    EBML_UniStringSetValue((ebml_string*)Node,val);
    return *this;
}

EbmlUnicodeString::operator const UTFstring() const
{
assert(0);
return *static_cast<UTFstring*>(NULL);
}

EbmlElement * EbmlUnicodeString::Clone() const
{
assert(0);
    return NULL;
}


/*****************
 * UTFstring
 ****************/
UTFstring::UTFstring(const wchar_t* value)
:Buffer(NULL)
{
    size_t Size = wcslen(value);
    Buffer = new wchar_t[(Size+1)*sizeof(wchar_t)];
    if (Buffer)
        memcpy(Buffer,value,(Size+1)*sizeof(wchar_t));
}

UTFstring::UTFstring()
:Buffer(NULL)
{
}

UTFstring::UTFstring(const UTFstring &Clone)
:Buffer(NULL)
{
    size_t Size = wcslen(Clone.Buffer);
    Buffer = new wchar_t[(Size+1)*sizeof(wchar_t)];
    if (Buffer)
        memcpy(Buffer,Clone.Buffer,(Size+1)*sizeof(wchar_t));
}

UTFstring::~UTFstring()
{
    delete[] Buffer;
}

UTFstring & UTFstring::operator=(const UTFstring & Clone)
{
    delete[] Buffer;
    size_t Size = wcslen(Clone.Buffer);
    Buffer = new wchar_t[(Size+1)*sizeof(wchar_t)];
    if (Buffer)
        memcpy(Buffer,Clone.Buffer,(Size+1)*sizeof(wchar_t));
    return *this;
}

const wchar_t* UTFstring::c_str() const
{
assert(0);
    return NULL;
}

UTFstring::operator const wchar_t*() const
{
    return Buffer;
}

bool UTFstring::operator==(const UTFstring &Cmp) const
{
    size_t Size1 = wcslen(Buffer);
    size_t Size2 = wcslen(Cmp.Buffer);
    if (Size1 != Size2)
        return false;
    return memcmp(Buffer,Cmp.Buffer,(Size1+1)*sizeof(wchar_t))==0;
}

size_t UTFstring::length() const
{
assert(0);
    return 0;
}

/*****************
 * EbmlUInteger
 ****************/
EbmlUInteger::EbmlUInteger(const ebml_context &ec, ebml_element *WithNode)
:EbmlElement(ec, WithNode)
{
}

EbmlUInteger::EbmlUInteger(const ebml_context &ec, unsigned int DefaultValue, ebml_element *WithNode)
:EbmlElement(ec, WithNode)
{
    Node->bDefaultIsSet = 1;
}

void EbmlUInteger::SetDefaultSize(filepos_t aDefaultSize)
{
    Node->DefaultSize = aDefaultSize;
}

filepos_t EbmlUInteger::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlUInteger::operator uint64_t() const
{
    return reinterpret_cast<ebml_integer*>(Node)->Value;
}

uint64_t EbmlUInteger::operator =(uint64_t val)
{
    reinterpret_cast<ebml_integer*>(Node)->Value = val;
    Node->bValueIsSet = 1;
    return val;
}

EbmlElement * EbmlUInteger::Clone() const
{
assert(0);
    return NULL;
}

bool EbmlUInteger::IsSmallerThan(const EbmlElement *Cmp) const
{
	if (EbmlId(*this) == EbmlId(*Cmp))
    {
        const EbmlUInteger *_Cmp = static_cast<const EbmlUInteger *>(Cmp);
        return reinterpret_cast<const ebml_integer*>(Node)->Value < reinterpret_cast<const ebml_integer*>(_Cmp->Node)->Value;
    }
	else
		return false;
}


/*****************
 * EbmlSInteger
 ****************/
void EbmlSInteger::SetDefaultSize(filepos_t aDefaultSize)
{
assert(0);
}

filepos_t EbmlSInteger::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

int64_t EbmlSInteger::operator =(int64_t val)
{
assert(0);
return 0;
}

EbmlSInteger::operator int64_t() const
{
    return reinterpret_cast<ebml_integer*>(Node)->Value;
}

EbmlElement * EbmlSInteger::Clone() const
{
assert(0);
    return NULL;
}

bool EbmlSInteger::IsSmallerThan(const EbmlElement *Cmp) const
{
	if (EbmlId(*this) == EbmlId(*Cmp))
    {
        const EbmlSInteger *_Cmp = static_cast<const EbmlSInteger *>(Cmp);
        return reinterpret_cast<const ebml_integer*>(Node)->Value < reinterpret_cast<const ebml_integer*>(_Cmp->Node)->Value;
    }
	else
		return false;
}


/*****************
 * EbmlFloat
 ****************/
EbmlFloat::EbmlFloat(const ebml_context &ec, double DefaultValue, Precision prec, ebml_element *WithNode)
:EbmlElement(ec, WithNode)
{
    SetPrecision(prec);
    Node->bDefaultIsSet = 1;
}

EbmlFloat::EbmlFloat(const ebml_context &ec, Precision prec, ebml_element *WithNode)
:EbmlElement(ec, WithNode)
{
    SetPrecision(prec);
}

EbmlFloat::operator double() const
{
    return reinterpret_cast<const ebml_float*>(Node)->Value;
}

double EbmlFloat::operator =(double val)
{
    ((ebml_float*)Node)->Value = val;
    Node->bValueIsSet = 1;
    return val;
}

void EbmlFloat::SetPrecision(Precision prec)
{
    if (prec==FLOAT_32)
        Node->Size = 4;
    else
        Node->Size = 8;
}

bool EbmlFloat::IsSmallerThan(const EbmlElement *Cmp) const
{
	if (EbmlId(*this) == EbmlId(*Cmp))
    {
        const EbmlFloat *_Cmp = static_cast<const EbmlFloat *>(Cmp);
        return reinterpret_cast<const ebml_float*>(Node)->Value < reinterpret_cast<const ebml_float*>(_Cmp->Node)->Value;
    }
	else
		return false;
}

filepos_t EbmlFloat::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlElement * EbmlFloat::Clone() const
{
assert(0);
    return NULL;
}


/*****************
 * EbmlDate
 ****************/
datetime_t EbmlDate::GetEpochDate() const
{
    return EBML_DateTime(reinterpret_cast<const ebml_date*>(Node)) + 978307200; // 1st January 2001 to January 1, 1970
}

void EbmlDate::SetEpochDate(int32_t NewDate)
{
    EBML_DateSetDateTime(reinterpret_cast<ebml_date*>(Node),NewDate - 978307200); // January 1, 1970 to 1st January 2001
}

filepos_t EbmlDate::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

bool EbmlDate::IsSmallerThan(const EbmlElement *Cmp) const
{
	if (EbmlId(*this) == EbmlId(*Cmp))
    {
        const EbmlDate *_Cmp = static_cast<const EbmlDate *>(Cmp);
        return reinterpret_cast<const ebml_date*>(Node)->Value < reinterpret_cast<const ebml_date*>(_Cmp->Node)->Value;
    }
	else
		return false;
}

EbmlElement * EbmlDate::Clone() const
{
assert(0);
    return NULL;
}


/*****************
 * EbmlVoid
 ****************/
filepos_t EbmlVoid::ReadData(IOCallback & input, ScopeMode ReadFully)
{
    return 0;
}

filepos_t EbmlVoid::ReplaceWith(EbmlElement & EltToReplaceWith, IOCallback & output, bool ComeBackAfterward, bool bWithDefault)
{
    return EBML_VoidReplaceWith(Node, EltToReplaceWith.GetNode(), output.GetStream(), ComeBackAfterward, bWithDefault);
}

void EbmlVoid::SetSize(filepos_t Size)
{
    EBML_VoidSetSize(Node,Size);
}


/*****************
 * IOCallback
 ****************/
IOCallback::IOCallback()
:Stream(NULL)
{
}

IOCallback::~IOCallback()
{
    if (Stream)
        NodeDelete((node*)Stream);
}

void IOCallback::writeFully(const void* Buffer, size_t Size)
{
assert(0);
}

stream *IOCallback::GetStream()
{
    if (Stream==NULL)
    {
        Stream = (stream_io*)NodeCreate(&ccContext,IOCALLBACK_STREAM_CLASS);
        Stream->cpp = this;
    }
    return (stream *)Stream;
}


/*****************
 * EbmlStream
 ****************/
EbmlStream::EbmlStream(IOCallback &)
{
assert(0);
}

IOCallback & EbmlStream::I_O()
{
assert(0);
    return *((IOCallback*)NULL);
}

#if 1
EbmlElement * EbmlStream::FindNextID(const ebml_context & ClassInfos, filepos_t MaxDataSize)
{
assert(0);
    return NULL;
}

EbmlElement * EbmlStream::FindNextElement(const ebml_context & Context, int & UpperLevel, filepos_t MaxDataSize, bool AllowDummyElt, size_t MaxLowerLevel)
{
assert(0);
    return NULL;
}
#else
EbmlElement * EbmlStream::FindNextID(const EbmlCallbacks & ClassInfos, filepos_t MaxDataSize)
{
assert(0);
    return NULL;
}

EbmlElement * EbmlStream::FindNextElement(const EbmlSemanticContext & Context, int & UpperLevel, filepos_t MaxDataSize, bool AllowDummyElt, size_t MaxLowerLevel)
{
assert(0);
    return NULL;
}
#endif


};
