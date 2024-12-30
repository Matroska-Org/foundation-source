/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_SINTEGER_H
#define _EBML2_EBML_SINTEGER_H

#include "ebml/EbmlElement.h"

namespace libebml {

    class EbmlSInteger : public EbmlElement {
    public:
        EbmlSInteger(const ebml_context &ec, ebml_element *WithNode = NULL) :EbmlElement(ec,WithNode) {}
        operator int64_t() const;

        virtual EbmlElement * Clone() const;
        int64_t operator =(int64_t);

		/*!
			Set the default size of the integer (usually 1,2,4 or 8)
		*/
		void SetDefaultSize(filepos_t nDefaultSize = 1);
        virtual bool IsSmallerThan(const EbmlElement *Cmp) const;

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };

};

#endif // _EBML2_EBML_SINTEGER_H
