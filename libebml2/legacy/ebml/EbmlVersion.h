/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EBML2_EBML_VERSION_H
#define _EBML2_EBML_VERSION_H

#include <string>

static const std::string EbmlCodeVersion = "1.0.0";
static const std::string EbmlCodeDate = __TIMESTAMP__;

#define LIBEBML_VERSION  0x010000

#if defined(EBML2_LEGACY_PROJECT_VERSION)
# if (EBML2_LEGACY_PROJECT_VERSION > LIBEBML_VERSION)
#  error mismatching LIBEBML_VERSION
# endif
#endif

#endif // _EBML2_EBML_VERSION_H
