/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_STREAM_H
#define _EBML2_EBML_STREAM_H

#include "ebml/IOCallback.h"

namespace libebml {
    class EbmlStream {
    public:
        EbmlStream(IOCallback &);

        EbmlElement * FindNextID(const ebml_context & ClassInfos, filepos_t MaxDataSize);
        EbmlElement * FindNextElement(const ebml_context & Context, int & UpperLevel, filepos_t MaxDataSize, bool AllowDummyElt, size_t MaxLowerLevel = 1);

        IOCallback & I_O();
    private:
        IOCallback &mIO;
    };
};

#endif // _EBML2_EBML_STREAM_H
