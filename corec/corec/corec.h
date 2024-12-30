/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef _COREC_H
#define _COREC_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#pragma warning(push, 4)
#pragma warning(disable : 4100 4710 4514 4201 4714 4115 4206 4055 4214 4998 4273 4127 4114 4512)
#endif

#include "portab.h"
#include "memalloc.h"
#include "err.h"
#include "helper.h"

#if defined(NDEBUG) && defined(CONFIG_DEBUGCHECKS)
#undef CONFIG_DEBUGCHECKS
#endif
#if defined(NDEBUG) && defined(CONFIG_DEBUG_LEAKS)
#undef CONFIG_DEBUG_LEAKS
#endif

#ifdef __cplusplus
}
#endif

#endif /* _COREC_H */
