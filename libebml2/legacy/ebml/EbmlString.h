/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_STRING_H
#define _EBML2_EBML_STRING_H

#include <string>

#include "ebml/EbmlElement.h"

namespace libebml {

    class EbmlString : public EbmlElement {
    public:
        EbmlString(const ebml_context &,const char *, ebml_element *WithNode = NULL);
        EbmlString(const ebml_context &, ebml_element *WithNode = NULL);
        EbmlString & operator=(const std::string &);
        EbmlString & operator=(const char *);
        operator const std::string () const;

        virtual EbmlElement * Clone() const;

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };

};

#endif // _EBML2_EBML_STRING_H
