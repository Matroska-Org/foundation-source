/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_UNICODE_STRING_H
#define _EBML2_EBML_UNICODE_STRING_H

#include "ebml/EbmlElement.h"

namespace libebml {

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
