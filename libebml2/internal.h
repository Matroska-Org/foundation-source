/*
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBEBML2_INTERNAL_H
#define LIBEBML2_INTERNAL_H

#include "ebml2/ebml_classes.h"

#ifdef __cplusplus
extern "C" {
#endif

INTERNAL_C_API size_t GetIdLength(fourcc_t Id);

extern const nodemeta EBMLElement_Class[];
extern const nodemeta EBMLMaster_Class[];
extern const nodemeta EBMLBinary_Class[];
extern const nodemeta EBMLString_Class[];
extern const nodemeta EBMLInteger_Class[];
extern const nodemeta EBMLCRC_Class[];
extern const nodemeta EBMLDate_Class[];
extern const nodemeta EBMLVoid_Class[];

#ifdef __cplusplus
}
#endif

#endif /* LIBEBML2_INTERNAL_H */
