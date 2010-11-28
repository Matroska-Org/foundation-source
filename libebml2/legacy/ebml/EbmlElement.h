/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Matroska assocation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY the Matroska association ``AS IS'' AND ANY
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

#ifndef _EBML2_EBML_ELEMENT_H
#define _EBML2_EBML_ELEMENT_H

#include "ebml/EbmlConfig.h"
#include "ebml/EbmlId.h"
#include "ebml/IOCallback.h"
#include "ebml/ebml.h"

#define EBML_BINARY_LEGACY_CLASS    FOURCC('E','B','B','C')
#define EBML_DATE_LEGACY_CLASS      FOURCC('E','B','B','D')
#define EBML_INTEGER_LEGACY_CLASS   FOURCC('E','B','B','I')
#define EBML_SINTEGER_LEGACY_CLASS  FOURCC('E','B','B','S')
#define EBML_FLOAT_LEGACY_CLASS     FOURCC('E','B','B','F')
#define EBML_MASTER_LEGACY_CLASS    FOURCC('E','B','B','M')
#define EBML_STRING_LEGACY_CLASS    FOURCC('E','B','B','G')
#define EBML_UNISTRING_LEGACY_CLASS FOURCC('E','B','B','U')

#define DECLARE_EBML_CONTEXT(x) 
#define DECLARE_EBML_MASTER(x)    DECLARE_EBML_CONTEXT(x) \
  class x : public EbmlMaster { \
  public: x(ebml_element *WithNode = NULL); \
  EBML_CONCRETE_CLASS(x)
#define DECLARE_EBML_UINTEGER(x)    DECLARE_EBML_CONTEXT(x) \
  class x : public EbmlUInteger { \
  public: x(ebml_element *WithNode = NULL); \
  EBML_CONCRETE_CLASS(x)
#define DECLARE_EBML_STRING(x)      DECLARE_EBML_CONTEXT(x) \
  class x : public EbmlString { \
  public: x(ebml_element *WithNode = NULL); \
  EBML_CONCRETE_CLASS(x)
#define DECLARE_EBML_BINARY(x)      DECLARE_EBML_CONTEXT(x) \
  class x : public EbmlBinary { \
  public: x(ebml_element *WithNode = NULL); \
  EBML_CONCRETE_CLASS(x)
#define DECLARE_EBML_VOID(x)      DECLARE_EBML_CONTEXT(x) \
  class x : public EbmlElement { \
  public: x(ebml_element *WithNode = NULL); \
  EBML_CONCRETE_CLASS(x)

#define DEFINE_SEMANTIC_CONTEXT(x)
#define DEFINE_START_SEMANTIC(x)     static const ebml_semantic EBML_Semantic##x[] = {
#define DEFINE_END_SEMANTIC(x)       {0, 0, NULL}}; // end of the table
#define DEFINE_SEMANTIC_ITEM(m,u,c)  {m, u, &c::EBML_Context##c},

#define EBML_CONCRETE_CLASS(x) \
    public: \
        operator const EbmlId (void) const { return EbmlId(EBML_Context##x.Id); } \
        virtual EbmlElement * Clone() const { return new x(*this); } \
        virtual operator const ebml_context &() const { return EBML_Context##x; } \
        static const ebml_context EBML_Context##x; \
        static const ebml_context & GetContext() { return EBML_Context##x; } \
        static void PostCreate(ebml_element *p, const void *Cookie) { if (!Cookie) Cookie=new x(p); Node_Set(p,EBML_ELEMENT_OBJECT,&Cookie,sizeof(Cookie)); }

#define DEFINE_xxx_CONTEXT(x,global) \
    const ebml_context EBML_Context##x = {0, EBML_MASTER_LEGACY_CLASS, 0, 0, "##x", EBML_Semantic##x, NULL};

#define DEFINE_EBML_MASTER(x,id,parent,name) DEFINE_xxx_MASTER(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_MASTER(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_MASTER_LEGACY_CLASS, 0, 0, name, EBML_Semantic##x, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlMaster(EBML_Context##x, WithNode) {}

#define DEFINE_EBML_MASTER_CONS(x,id,parent,name) DEFINE_xxx_MASTER_CONS(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_MASTER_CONS(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_MASTER_LEGACY_CLASS, 0, 0, name, EBML_Semantic##x, global, x::PostCreate}; \

#define DEFINE_xxx_MASTER_ORPHAN(x,id,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_MASTER_LEGACY_CLASS, 0, 0, name, EBML_Semantic##x, global, x::PostCreate}; \

#define DEFINE_EBML_UINTEGER(x,id,parent,name) DEFINE_xxx_UINTEGER(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_UINTEGER(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_INTEGER_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlUInteger(EBML_Context##x, WithNode) {}

#define DEFINE_EBML_UINTEGER_DEF(x,id,parent,name,defval) DEFINE_xxx_UINTEGER_DEF(x,id,parent,name,global,defval)
#define DEFINE_xxx_UINTEGER_DEF(x,id,parent,name,global,defval) \
    const ebml_context x::EBML_Context##x = {id, EBML_INTEGER_LEGACY_CLASS, 1, defval, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlUInteger(EBML_Context##x, defval, WithNode) {}

#define DEFINE_EBML_SINTEGER(x,id,parent,name) DEFINE_xxx_SINTEGER(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_SINTEGER(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_SINTEGER_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlSInteger(EBML_Context##x, WithNode) {}

#define DEFINE_EBML_SINTEGER_CONS(x,id,parent,name) DEFINE_xxx_SINTEGER_CONS(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_SINTEGER_CONS(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_SINTEGER_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \

#define DEFINE_EBML_FLOAT(x,id,parent,name) DEFINE_xxx_FLOAT(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_FLOAT(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_FLOAT_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlFloat(EBML_Context##x, EbmlFloat::FLOAT_32, WithNode) {}

#define DEFINE_EBML_FLOAT64(x,id,parent,name) DEFINE_xxx_FLOAT64(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_FLOAT64(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_FLOAT_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlFloat(EBML_Context##x, EbmlFloat::FLOAT_64, WithNode) {}

#define DEFINE_EBML_FLOAT_DEF(x,id,parent,name,defval) DEFINE_xxx_FLOAT_DEF(x,id,parent,name,EBML_SemanticGlobal,defval)
#define DEFINE_xxx_FLOAT_DEF(x,id,parent,name,global,defval) \
    const ebml_context x::EBML_Context##x = {id, EBML_FLOAT_LEGACY_CLASS, 1, (intptr_t)defval, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlFloat(EBML_Context##x, defval, EbmlFloat::FLOAT_32, WithNode) {}

#define DEFINE_EBML_UNISTRING(x,id,parent,name) DEFINE_xxx_UNISTRING(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_UNISTRING(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_UNISTRING_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlUnicodeString(EBML_Context##x, WithNode) {}

#define DEFINE_EBML_STRING(x,id,parent,name) DEFINE_xxx_STRING(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_STRING(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_STRING_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlString(EBML_Context##x, WithNode) {}

#define DEFINE_EBML_STRING_DEF(x,id,parent,name,defval) DEFINE_xxx_STRING_DEF(x,id,parent,name,EBML_SemanticGlobal,defval)
#define DEFINE_xxx_STRING_DEF(x,id,parent,name,global,defval) \
    const ebml_context x::EBML_Context##x = {id, EBML_STRING_LEGACY_CLASS, 1, (intptr_t)defval, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlString(EBML_Context##x, defval, WithNode) {}

#define DEFINE_xxx_BINARY(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_BINARY_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlBinary(EBML_Context##x, WithNode) {}

#define DEFINE_xxx_BINARY_CONS(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_BINARY_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \

#define DEFINE_EBML_BINARY_GLOBAL(x,id,name) DEFINE_xxx_BINARY_GLOBAL(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_BINARY_GLOBAL(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_BINARY_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlBinary(EBML_Context##x, WithNode) {}

#define DEFINE_EBML_VOID_GLOBAL(x,id,name) \
    const ebml_context x::EBML_Context##x = {id, EBML_VOID_CLASS, 0, 0, name, NULL, EBML_SemanticGlobal, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlElement(EBML_Context##x, WithNode) {}

#define DEFINE_EBML_DATE(x,id,parent,name) DEFINE_xxx_DATE(x,id,parent,name,EBML_SemanticGlobal)
#define DEFINE_xxx_DATE(x,id,parent,name,global) \
    const ebml_context x::EBML_Context##x = {id, EBML_DATE_LEGACY_CLASS, 0, 0, name, NULL, global, x::PostCreate}; \
    x::x(ebml_element *WithNode) :EbmlDate(EBML_Context##x, WithNode)  {}

#define EBML_DEF_SEP ,
#define EBML_DEF_CONS           const ebml_context &ec
#define EBML_DEF_PARAM          ec
#define EBML_DEF_BINARY_INIT    EbmlBinary(ec, WithNode)
#define EBML_DEF_BINARY_CTX(x)  EBML_Context##x
#define EBML_DEF_BINARY(x)      EbmlBinary(EBML_Context##x, WithNode)
#define EBML_DEF_SINTEGER(x)    EbmlSInteger(EBML_Context##x, WithNode)
#define EBML_EXTRA_PARAM        ebml_element *WithNode = NULL
#define EBML_EXTRA_CALL         WithNode
#define EBML_EXTRA_DEF          ebml_element *WithNode

#define EBML_ID(ref)               ref::EBML_Context##ref.Id
#define EBML_INFO(ref)             ref::EBML_Context##ref
#define EBML_CLASS_SEMCONTEXT(ref) EBML_Context##ref
#define EBML_CLASS_CALLBACK(ref)   ref::EBML_Context##ref
#define EBML_CONTEXT(e)            static_cast<const ebml_context &>(*(e))
#define EBML_NAME(e)               (e)->DebugName()

#define EBML_INFO_ID(cb)      (cb).Id
#define EBML_INFO_CONTEXT(cb) cb
#define EBML_INFO_NAME(cb)    (cb).ElementName

#define EBML_CLASS_CONTEXT(ref) ref::EBML_Context##ref

#define EBML_SEM_UNIQUE(s)  (s).Unique
#define EBML_SEM_CONTEXT(s) ((const EbmlSemanticContext)*(s).eClass) // EbmlSemantic/ebml_semantic -> EbmlSemanticContext
#define EBML_SEM_CREATE(s)  CreateEbmlElement(s)

#define EBML_CTX_SIZE(c)       (c).GetSize()                  // EbmlSemanticContext -> size_t
#define EBML_CTX_IDX(c,i)      (c).GetSemantic(i)             // EbmlSemanticContext -> EbmlSemantic  / ebml_semantic
#define EBML_CTX_IDX_INFO(c,i) *(c).GetSemantic(i).eClass     // EbmlSemanticContext -> EbmlCallbacks / ebml_context
#define EBML_CTX_IDX_ID(c,i)   (c).GetSemantic(i).eClass->Id  // EbmlSemanticContext -> EbmlId

namespace LIBEBML_NAMESPACE {

    typedef int ScopeMode;

    class EbmlSemanticContext;
    class EbmlElement;
    class EbmlStream;

    EbmlElement & CreateEbmlElement(const ebml_semantic &);

    typedef ebml_context EbmlCallbacks;
    typedef ebml_semantic EbmlSemantic;

    class EbmlSemanticContext {
    public:
        EbmlSemanticContext(const ebml_context &);
		bool operator!=(const EbmlSemanticContext & Elt) const;
        ~EbmlSemanticContext();

        inline size_t GetSize() const { return Size; }

        const ebml_context * GetContext() const;
        const EbmlSemantic & GetSemantic(size_t i) const;

    private:
        size_t Size;
        const ebml_context & Context;
//        ebml_parser_context *pContext;
    };

    class EbmlElement {
    public:
        virtual ~EbmlElement();
        void SetSizeLength(size_t);
        filepos_t Render(IOCallback & output, bool bWithDefault = false, bool bKeepPosition = false, bool bForceRender = false);
        virtual bool SetSizeInfinite(bool bIsInfinite = true);
        filepos_t GetElementPosition() const;
        size_t HeadSize() const;
        filepos_t ElementSize(bool bWithDefault = false) const;
        bool IsFiniteSize() const;
        virtual filepos_t GetSize() const;
        size_t GetSizeLength() const;
        virtual operator const EbmlId () const = 0;
        virtual EbmlElement * Clone() const = 0;

		/*!
			\brief Force the size of an element
			\warning only possible if the size is "undefined"
		*/
		bool ForceSize(filepos_t NewSize);
		/*!
			\brief set the default size of an element
		*/
		virtual void SetDefaultSize(filepos_t aDefaultSize);
        filepos_t OverwriteHead(IOCallback & output, bool bKeepPosition = false);
        virtual void Read(EbmlStream & inDataStream, const EbmlSemanticContext & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt = false, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual void Read(EbmlStream & inDataStream, const ebml_parser_context & Context, int & UpperEltFound, EbmlElement * & FoundElt, bool AllowDummyElt = false, ScopeMode ReadFully = SCOPE_ALL_DATA);
        EbmlElement * SkipData(EbmlStream & DataStream, const EbmlSemanticContext & Context, EbmlElement * TestReadElt = NULL, bool AllowDummyElt = false);

        virtual bool IsSmallerThan(const EbmlElement *Cmp) const;

        void SetValueIsSet(bool Set = true);
        void SetSize_(filepos_t);
        bool ValueIsSet() const;

        const char* DebugName() const;

        inline ebml_element *GetNode() { return Node; }
        inline const ebml_element *GetNode() const { return Node; }

        virtual operator const ebml_context &() const = 0;


        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA) = 0;
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false) = 0;
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false) = 0; /// update the Size of the Data stored before rendering
    protected:
        EbmlElement(const ebml_context &, ebml_element *WithNode = NULL);
        ebml_element *Node;
        static err_t Deleting(EbmlElement *p, nodeevt *Evt);
    };

    size_t CodedSizeLength(filepos_t Length, size_t SizeLength, bool bSizeIsFinite = true);
    size_t CodedSizeLengthSigned(filepos_t Length, size_t SizeLength);
    size_t CodedValueLengthSigned(filepos_t Length, size_t CodedSize, binary * OutBuffer);
    size_t CodedValueLength(filepos_t Length, size_t CodedSize, binary * OutBuffer);
    filepos_t ReadCodedSizeValue(const binary * InBuffer, uint32_t & BufferSize, uint64_t & SizeUnknown);
    filepos_t ReadCodedSizeSignedValue(const binary * InBuffer, uint32_t & BufferSize, uint64_t & SizeUnknown);

    extern const ebml_semantic EBML_SemanticGlobal[];

};

#endif // _EBML2_EBML_ELEMENT_H
