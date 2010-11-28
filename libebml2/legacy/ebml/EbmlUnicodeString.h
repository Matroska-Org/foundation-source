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

#ifndef _EBML2_EBML_UNICODE_STRING_H
#define _EBML2_EBML_UNICODE_STRING_H

#include "ebml/EbmlElement.h"

namespace LIBEBML_NAMESPACE {

    /*!
      \class UTFstring
      A class storing strings in a wchar_t (ie, in UCS-2 or UCS-4)
      \note inspired by wstring which is not available everywhere
    */
    class UTFstring {
    public:
        UTFstring();
        UTFstring(const wchar_t *);
        UTFstring(const UTFstring &);
        ~UTFstring();
        const wchar_t* c_str() const;
        bool operator==(const UTFstring&) const;
        UTFstring & operator=(const UTFstring &);
        operator const wchar_t*() const;
        size_t length() const;
    protected:
        wchar_t *Buffer;
    };

    class EbmlUnicodeString : public EbmlElement {
    public:
        //EbmlUnicodeString(const ebml_context &,const UTFstring &);
        EbmlUnicodeString(const ebml_context &, ebml_element *WithNode = NULL);
        EbmlUnicodeString & operator=(const UTFstring &);
        operator const UTFstring () const;

        virtual EbmlElement * Clone() const;

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };
};

#endif // _EBML2_EBML_UNICODE_STRING_H
