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

void ebml_init()
{
    NodeContext_Init(&ccContext,NULL,NULL,NULL);
    EBML_Init(&ccContext);
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
DEFINE_EBML_BINARY_GLOBAL(EbmlVoid,  0xEC, 1, "EBMLVoid");

const ebml_context EbmlHead::EBML_ContextEbmlHead                       = ::EBML_ContextHead; // dirty copy
const ebml_context EDocType::EBML_ContextEDocType                       = ::EBML_ContextDocType; // dirty copy
const ebml_context EVersion::EBML_ContextEVersion                       = ::EBML_ContextVersion; // dirty copy
const ebml_context EReadVersion::EBML_ContextEReadVersion               = ::EBML_ContextReadVersion; // dirty copy
const ebml_context EDocTypeVersion::EBML_ContextEDocTypeVersion         = ::EBML_ContextDocTypeVersion; // dirty copy
const ebml_context EDocTypeReadVersion::EBML_ContextEDocTypeReadVersion = ::EBML_ContextDocTypeReadVersion; // dirty copy
const ebml_context EMaxSizeLength::EBML_ContextEMaxSizeLength           = ::EBML_ContextMaxSizeLength; // dirty copy
const ebml_context EMaxIdLength::EBML_ContextEMaxIdLength               = ::EBML_ContextMaxIdLength; // dirty copy

const ebml_context & EbmlHead::GetContext() { return EBML_ContextHead; }
const ebml_context & EDocType::GetContext() { return EBML_ContextEDocType; }
const ebml_context & EVersion::GetContext() { return EBML_ContextEVersion; }
const ebml_context & EReadVersion::GetContext() { return EBML_ContextEReadVersion; }
const ebml_context & EDocTypeVersion::GetContext() { return EBML_ContextEDocTypeVersion; }
const ebml_context & EDocTypeReadVersion::GetContext() { return EBML_ContextEDocTypeReadVersion; }
const ebml_context & EMaxSizeLength::GetContext() { return EBML_ContextEMaxSizeLength; }
const ebml_context & EMaxIdLength::GetContext() { return EBML_ContextEMaxIdLength; }

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
EbmlElement::EbmlElement(const ebml_context & Context)
{
    Node = EBML_ElementCreate(&ccContext,&Context,0);
assert(0);
    if (Node)
    {
        // TODO: throw some error
    }
}

EbmlElement::EbmlElement()
{
assert(0);
    Node = EBML_ElementCreate(NULL,NULL,0);
    if (Node)
    {
        // TODO: throw some error
    }
}

EbmlElement * EbmlElement::SkipData(EbmlStream & DataStream, const EbmlSemanticContext & Context, EbmlElement * TestReadElt, bool AllowDummyElt)
{
assert(0);
    return NULL;
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
assert(0);
    return INVALID_FILEPOS_T;
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
assert(0);
    return false;
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
assert(0);
    return false;
}

void EbmlElement::SetSizeLength(size_t)
{
assert(0);
}

size_t EbmlElement::HeadSize() const
{
assert(0);
    return 0;
}

filepos_t EbmlElement::ElementSize(bool bKeepIntact) const
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlElement::GetElementPosition() const
{
    return Node->ElementPosition;
}

filepos_t EbmlElement::Render(IOCallback & output, bool bSaveDefault)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlElement::OverwriteHead(IOCallback & output, bool bKeepPosition)
{
assert(0);
    return INVALID_FILEPOS_T;
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
    while (s->eClass!=NULL)
    {
        ++Size;
        ++s;
    }
}

const EbmlSemantic EbmlSemanticContext::GetSemantic(size_t i) const
{
    assert(i < Size);
    return Context.Semantic[i];
}

bool EbmlSemanticContext::operator!=(const EbmlSemanticContext & Elt) const
{
	return (Size != Elt.Size) || (Context.Semantic != Elt.Context.Semantic); // TODO: handle more
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
 * EbmlSemantic
 ****************/
EbmlSemantic::EbmlSemantic(bool Mandatory,bool Unique,const EbmlCallbacks & GetCallbacks)
:Callbacks(&GetCallbacks)
{
assert(0); // should not exist anymore
    pSemantic = new ebml_semantic;
    pSemantic->Mandatory = Mandatory;
    pSemantic->Unique = Unique;
    pSemantic->eClass = NULL;
//    pSemantic->eClass = GetCallbacks.GetContext(); // TODO handle this after GetCallbacks is initialized
}

EbmlSemantic::EbmlSemantic(const ebml_semantic & Semantic)
:Callbacks(NULL)
,pSemantic(const_cast<ebml_semantic*>(&Semantic))
{
}

EbmlSemantic::~EbmlSemantic()
{
    delete pSemantic;
}

EbmlElement & EbmlSemantic::Create() const
{
assert(0);
return *static_cast<EbmlElement*>(NULL);
}

bool EbmlSemantic::IsUnique() const
{
    return pSemantic->Unique!=0;
}

EbmlSemantic::operator const EbmlCallbacks &() const
{
    return *Callbacks;
}

EbmlSemantic::operator const ebml_semantic &() const
{
    if (pSemantic->eClass == NULL)
    {
        pSemantic->eClass = Callbacks;
    }
    return *pSemantic;
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
EbmlMaster::EbmlMaster()
{
assert(0);
}

EbmlMaster::EbmlMaster(struct ebml_context const &Context)
:EbmlElement(Context)
{
assert(0);
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

filepos_t EbmlMaster::WriteHead(IOCallback & output, size_t SizeLength, bool bKeepIntact)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlMaster::UpdateSize(bool bKeepIntact, bool bForceRender)
{
assert(0);
    return INVALID_FILEPOS_T;
}

void EbmlMaster::Sort()
{
assert(0);
}

void EbmlMaster::RemoveAll()
{
assert(0);
}

size_t EbmlMaster::ListSize() const
{
assert(0);
return 0;
}

bool EbmlMaster::InsertElement(EbmlElement & element, size_t position)
{
assert(0);
    return false;
}

bool EbmlMaster::PushElement(EbmlElement & element)
{
assert(0);
    return false;
}

EbmlElement *EbmlMaster::AddNewElt(const EbmlCallbacks & Callbacks)
{
assert(0);
    return NULL;
}

EbmlElement *EbmlMaster::FindElt(const ebml_context & Callbacks) const
{
assert(0);
    return NULL;
}

EbmlElement *EbmlMaster::FindFirstElt(const ebml_context & Callbacks) const
{
assert(0);
    return NULL;
}

EbmlElement *EbmlMaster::FindFirstElt(const ebml_context & Callbacks, const bool bCreateIfNull) const
{
assert(0);
    return NULL;
}

EbmlElement *EbmlMaster::FindNextElt(const EbmlElement & FromElt) const
{
assert(0);
    return NULL;
}

EbmlElement *EbmlMaster::FindNextElt(const EbmlElement & FromElt, const bool bCreateIfNull)
{
assert(0);
    return NULL;
}

void EbmlMaster::Remove(size_t Index)
{
assert(0);
}

EbmlElement * EbmlMaster::operator[](size_t position)
{
assert(0);
return NULL;
}

const EbmlElement * EbmlMaster::operator[](size_t position) const
{
assert(0);
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
assert(0);
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

filepos_t EbmlBinary::UpdateSize(bool bKeepIntact, bool bForceRender)
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
EbmlString::EbmlString(const char *)
{
assert(0);
}

EbmlString::EbmlString()
{}

filepos_t EbmlString::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlString::UpdateSize(bool bKeepIntact, bool bForceRender)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlString & EbmlString::operator=(const std::string &)
{
assert(0);
return *static_cast<EbmlString*>(NULL);
}

EbmlString & EbmlString::operator=(const char *)
{
assert(0);
return *static_cast<EbmlString*>(NULL);
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
EbmlUnicodeString::EbmlUnicodeString()
{}

filepos_t EbmlUnicodeString::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlUnicodeString::UpdateSize(bool bKeepIntact, bool bForceRender)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlUnicodeString & EbmlUnicodeString::operator=(const UTFstring &)
{
assert(0);
return *static_cast<EbmlUnicodeString*>(NULL);
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
UTFstring::UTFstring(const wchar_t*)
{
assert(0);
}

UTFstring::UTFstring()
{
assert(0);
}

const wchar_t* UTFstring::c_str() const
{
assert(0);
    return NULL;
}

UTFstring::operator const wchar_t*() const
{
assert(0);
    return NULL;
}

bool UTFstring::operator==(const UTFstring&) const
{
assert(0);
    return false;
}

size_t UTFstring::length() const
{
assert(0);
    return 0;
}

/*****************
 * EbmlUInteger
 ****************/
EbmlUInteger::EbmlUInteger()
{
}

EbmlUInteger::EbmlUInteger(unsigned int DefaultValue)
{
assert(0);
}

void EbmlUInteger::SetDefaultSize(filepos_t aDefaultSize)
{
assert(0);
}

filepos_t EbmlUInteger::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlUInteger::UpdateSize(bool bKeepIntact, bool bForceRender)
{
assert(0);
    return INVALID_FILEPOS_T;
}

EbmlUInteger::operator uint8_t() const
{
assert(0);
    return 0;
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

filepos_t EbmlSInteger::UpdateSize(bool bKeepIntact, bool bForceRender)
{
assert(0);
    return INVALID_FILEPOS_T;
}

int64_t EbmlSInteger::operator =(int64_t)
{
assert(0);
return 0;
}

EbmlSInteger::operator int64_t() const
{
assert(0);
return 0;
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
EbmlFloat::EbmlFloat(double DefaultValue, Precision prec)
{
assert(0);
}

EbmlFloat::EbmlFloat(Precision prec)
{
assert(0);
    SetPrecision(prec);
}

EbmlFloat::operator double() const
{
    return reinterpret_cast<const ebml_float*>(Node)->Value;
}

void EbmlFloat::SetPrecision(Precision prec)
{
assert(0);
    reinterpret_cast<ebml_float*>(Node)->IsSimplePrecision = (prec==FLOAT_32);
}

filepos_t EbmlFloat::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlFloat::UpdateSize(bool bKeepIntact, bool bForceRender)
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
    return EBML_DateTime(reinterpret_cast<const ebml_date*>(Node));
}

void EbmlDate::SetEpochDate(datetime_t NewDate)
{
    EBML_DateSetDateTime(reinterpret_cast<ebml_date*>(Node),NewDate);
}

filepos_t EbmlDate::ReadData(IOCallback & input, ScopeMode ReadFully)
{
assert(0);
    return INVALID_FILEPOS_T;
}

filepos_t EbmlDate::UpdateSize(bool bKeepIntact, bool bForceRender)
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
filepos_t EbmlVoid::ReplaceWith(EbmlElement & EltToReplaceWith, IOCallback & output, bool ComeBackAfterward, bool bKeepIntact)
{
assert(0);
    return INVALID_FILEPOS_T;
}

void EbmlVoid::SetSize(filepos_t)
{
assert(0);
}


/*****************
 * IOCallback
 ****************/
void IOCallback::writeFully(const void* Buffer, size_t Size)
{
assert(0);
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


/*****************
 * EbmlHead
 ****************/
EbmlHead::EbmlHead()
{
assert(0);
}


};
