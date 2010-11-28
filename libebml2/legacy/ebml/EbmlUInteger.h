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

#ifndef _EBML2_EBML_UINTEGER_H
#define _EBML2_EBML_UINTEGER_H

#include "ebml/EbmlElement.h"

namespace LIBEBML_NAMESPACE {

    class EbmlUInteger : public EbmlElement {
    public:
        EbmlUInteger(const ebml_context &,unsigned int, ebml_element *WithNode = NULL);
        EbmlUInteger(const ebml_context &, ebml_element *WithNode = NULL);
		operator uint64_t() const;
        virtual EbmlElement * Clone() const;
        uint64_t operator =(uint64_t val);

		/*!
			Set the default size of the integer (usually 1,2,4 or 8)
		*/
		void SetDefaultSize(filepos_t nDefaultSize = 0);
        virtual bool IsSmallerThan(const EbmlElement *Cmp) const;

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };

};

#endif // _EBML2_EBML_UINTEGER_H
