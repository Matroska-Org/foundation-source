/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_BINARY_H
#define _EBML2_EBML_BINARY_H

#include "ebml/EbmlElement.h"

namespace libebml {

    class EbmlBinary : public EbmlElement {
    public:
        EbmlBinary(const ebml_context &ec, ebml_element *WithNode = NULL) :EbmlElement(ec,WithNode) {}
        void CopyBuffer(const void *Buffer, size_t BufferSize);
        void SetBuffer(const binary *Buffer, size_t BufferSize);

        virtual EbmlElement * Clone() const;

        const binary* GetBuffer() const;
        binary* GetBuffer();

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };

};

#endif // _EBML2_EBML_BINARY_H
