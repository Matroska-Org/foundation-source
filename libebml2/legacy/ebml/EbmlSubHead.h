/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_SUBHEAD_H
#define _EBML2_EBML_SUBHEAD_H

#include "ebml/EbmlUInteger.h"
#include "ebml/EbmlString.h"

namespace libebml {

    DECLARE_EBML_UINTEGER(EVersion)
    };

    DECLARE_EBML_UINTEGER(EReadVersion)
    };

    DECLARE_EBML_UINTEGER(EMaxIdLength)
    };

    DECLARE_EBML_UINTEGER(EMaxSizeLength)
    };

    DECLARE_EBML_STRING(EDocType)
    };

    DECLARE_EBML_UINTEGER(EDocTypeVersion)
    };

    DECLARE_EBML_UINTEGER(EDocTypeReadVersion)
    };
};

#endif // _EBML2_EBML_SUBHEAD_H
