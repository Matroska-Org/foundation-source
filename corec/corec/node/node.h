/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __NODE_H
#define __NODE_H

#include "corec/corec.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(corec_EXPORTS)
#define NODE_DLL DLLEXPORT
#elif defined(NODE_IMPORTS)
#define NODE_DLL DLLIMPORT
#else
#define NODE_DLL
#endif

#include "nodebase.h"
#include "nodetree.h"

#ifdef __cplusplus
}
#endif

#endif /* __NODE_H */
