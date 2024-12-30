/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_VOID_H
#define _EBML2_EBML_VOID_H

#include "ebml/EbmlBinary.h"

namespace libebml {

DECLARE_EBML_VOID(EbmlVoid)
    public:
        void SetSize(filepos_t);
        filepos_t ReplaceWith(EbmlElement & EltToReplaceWith, IOCallback & output, bool ComeBackAfterward = true, bool bWithDefault = false);

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };
};

#endif // _EBML2_EBML_VOID_H
