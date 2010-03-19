/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
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

#ifndef _EBML2_EBML_ELEMENT_H
#define _EBML2_EBML_ELEMENT_H

#include "ebml/EbmlConfig.h"
#include "ebml/EbmlId.h"
#include "ebml/IOCallback.h"

typedef struct ebml_element ebml_element;
fourcc_t GetEbmlFourCC(ebml_element *);

#define EBML_CONCRETE_CLASS(x) \
    public: \
        operator const EbmlId (void) const { return EbmlId(GetEbmlFourCC(Node)); } \
        static const EbmlCallbacks ClassInfos; \
        static EbmlElement & Create() {return *(new x);} \
        virtual EbmlElement * Clone() const { return new x(*this); } \

#define EBML_INFO(ref)  ref::ClassInfos        // TODO
#define EBML_ID(ref)    ((const EbmlId &)ref::ClassInfos)
#define EBML_CONTEXT(e) ((const EbmlSemanticContext &)(*e))
#define EBML_NAME(e)    (e)->DebugName()

#define EBML_INFO_ID(cb)      ((const EbmlId &)(cb))
#define EBML_INFO_NAME(cb)    (cb).ClassName()
#define EBML_INFO_CONTEXT(cb) ((const EbmlSemanticContext &)(cb))

#define EBML_SEM_UNIQUE(s)  (s).IsUnique()
#define EBML_SEM_INFO(s)    (const EbmlCallbacks &)(s)
#define EBML_SEM_ID(s)      ((const EbmlId &)(const EbmlCallbacks &)(s))
#define EBML_SEM_CONTEXT(s) ((const EbmlSemanticContext &)(const EbmlCallbacks &)(s))
#define EBML_SEM_CREATE(s)  (s).Create()

#define EBML_CTX_SIZE(c)    (c).GetSize()

namespace LIBEBML_NAMESPACE {

    enum ScopeMode {
	    SCOPE_PARTIAL_DATA = 0,
	    SCOPE_ALL_DATA,
	    SCOPE_NO_DATA
    };

    class EbmlSemanticContext;
    class EbmlElement;
    class EbmlStream;

    class EbmlCallbacks {
    public:
        EbmlCallbacks(EbmlElement & (*_Create)(),const EbmlId &_GlobalId, const char *_Debug, const EbmlSemanticContext &_Context);

        inline operator const EbmlId &() const { return EbmlID; }
        inline operator const EbmlSemanticContext &() const { return mContext; }
        const char* ClassName() const;

    private:
        const EbmlId & EbmlID;
        const EbmlSemanticContext & mContext;
    };

    class EbmlSemantic {
    public:
        EbmlSemantic(bool _Mandatory,bool _Unique,const EbmlCallbacks & _GetCallbacks);

        EbmlElement & Create() const;
        operator const EbmlCallbacks &() const;
        bool IsUnique() const;
    };

    class EbmlSemanticContext {
    public:
        EbmlSemanticContext(size_t,const EbmlSemantic*,const EbmlSemanticContext *,const EbmlSemanticContext & (*global)(), const EbmlCallbacks*);
		bool operator!=(const EbmlSemanticContext & Elt) const;

        inline size_t GetSize() const { return Size; }

        const EbmlSemantic *MyTable; // needs to be public for legacy reasons

    private:
        size_t Size;
    };

    class EbmlElement {
    public:
        void SetSizeLength(size_t);
        filepos_t Render(IOCallback & output, bool bSaveDefault = false);
        virtual bool SetSizeInfinite(bool bIsInfinite = true);
        filepos_t GetElementPosition() const;
        size_t HeadSize() const;
        filepos_t ElementSize(bool bKeepIntact = false) const;
        bool IsFiniteSize() const;
        virtual filepos_t GetSize() const;
        size_t GetSizeLength() const;
        virtual operator const EbmlId () const = 0;
		virtual filepos_t UpdateSize(bool bKeepIntact = false, bool bForceRender = false) = 0; /// update the Size of the Data stored
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA) = 0;
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
        EbmlElement * SkipData(EbmlStream & DataStream, const EbmlSemanticContext & Context, EbmlElement * TestReadElt = NULL, bool AllowDummyElt = false);

        void SetValueIsSet(bool Set = true);
        void SetSize_(filepos_t);
        bool ValueIsSet() const;

        const char* DebugName() const;
        operator const EbmlSemanticContext &() const;

    protected:
        EbmlElement();
        EbmlElement(const EbmlSemanticContext &);
        // TODO: add an EBML_LEGACY_STRICT define to avoid direct access to the class variables
#if 0
        bool bValueIsSet;
        filepos_t Size;
		bool bSizeIsFinite;
#endif
        ebml_element *Node;
    };

    size_t CodedSizeLength(filepos_t Length, size_t SizeLength, bool bSizeIsFinite = true);
    size_t CodedSizeLengthSigned(filepos_t Length, size_t SizeLength);
    size_t CodedValueLengthSigned(filepos_t Length, size_t CodedSize, binary * OutBuffer);
    size_t CodedValueLength(filepos_t Length, size_t CodedSize, binary * OutBuffer);
    filepos_t ReadCodedSizeValue(const binary * InBuffer, uint32_t & BufferSize, uint64_t & SizeUnknown);
    filepos_t ReadCodedSizeSignedValue(const binary * InBuffer, uint32_t & BufferSize, uint64_t & SizeUnknown);
};

#endif // _EBML2_EBML_ELEMENT_H
