/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_CONTEXTS_H
#define _EBML2_EBML_CONTEXTS_H

#include "ebml/EbmlElement.h"

namespace libebml {

    const EbmlSemanticContext & GetEbmlGlobal_Context();

};

#ifdef __cplusplus
extern "C" {
#endif

extern void ebml_init();
extern void ebml_done();

#ifdef __cplusplus
}
#endif

#endif // _EBML2_EBML_CONTEXTS_H
