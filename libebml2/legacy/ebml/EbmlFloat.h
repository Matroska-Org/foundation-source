/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_FLOAT_H
#define _EBML2_EBML_FLOAT_H

#include "ebml/EbmlElement.h"

namespace libebml {

    class EbmlFloat : public EbmlElement {
    public:
		enum Precision {
			 FLOAT_32
			,FLOAT_64
		};

		EbmlFloat(const ebml_context &, Precision prec = FLOAT_32, ebml_element *WithNode = NULL);
		EbmlFloat(const ebml_context &, double DefaultValue, Precision prec = FLOAT_32, ebml_element *WithNode = NULL);
        operator double() const;

        double operator =(double val);
        virtual bool IsSmallerThan(const EbmlElement *Cmp) const;

        void SetPrecision(Precision prec = FLOAT_32);

        virtual EbmlElement * Clone() const;

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };

};

#endif // _EBML2_EBML_FLOAT_H
