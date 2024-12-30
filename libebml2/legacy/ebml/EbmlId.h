/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_ID_H
#define _EBML2_EBML_ID_H

#include "ebml/EbmlConfig.h"

#define EBML_ID_LENGTH(x)  (x).GetLength()
#define EBML_ID_VALUE(x)   (fourcc_t)(x)

namespace libebml {

    class EbmlId {
    public:
        EbmlId(uint32_t Id, size_t Size);
        EbmlId(const binary aValue[4], size_t aLength);
        EbmlId(fourcc_t Id);
        void Fill(binary * Buffer) const;
        operator fourcc_t() const;
        size_t GetLength() const;
    private:
        fourcc_t Value;
    };
};

#endif // _EBML2_EBML_ID_H
