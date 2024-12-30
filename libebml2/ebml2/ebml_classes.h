/*
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LIBEBML2_CLASSES_H
#define LIBEBML2_CLASSES_H

/*
 * ONLY INCLUDE THIS FILE IF YOU PLAN TO CREATE YOUR OWN EBML-BASED CLASS
 */

#include <corec/node/node.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ebml_context
{
    fourcc_t Id;
    fourcc_t Class; // TODO: store a pointer to make creation faster
    bool_t HasDefault;
    intptr_t DefaultValue;
    const char *ElementName;
    // TODO: create sub class so we don't have to assign it all the time
    const ebml_semantic *Semantic; // table with last element class set to NULL
    const ebml_semantic *GlobalContext; // table with last element class set to NULL
};

struct ebml_element
{
    nodetree Base;
    filepos_t DataSize; // size of the data inside the element
    filepos_t ElementPosition;
    filepos_t SizePosition; // TODO: is this needed since we have the ElementPosition and SizeLength ?
    filepos_t EndPosition;
    const ebml_context *Context;
    int DefaultSize;
    int8_t SizeLength;
    boolmem_t bValueIsSet;
    boolmem_t bNeedDataSizeUpdate;
};

struct ebml_master
{
    ebml_element Base;
    int CheckSumStatus; // 0: not set, 1: requested/invalid, 2: verified

};

struct ebml_string
{
    ebml_element Base;
    const char *Buffer; // UTF-8 internal storage

};

struct ebml_integer
{
    ebml_element Base;
    int64_t Value;

};

struct ebml_float
{
    ebml_element Base;
    double Value;

};

struct ebml_binary
{
    ebml_element Base;
    array Data;

};

struct ebml_dummy
{
    ebml_binary Base;
    ebml_context DummyContext;

};

#ifdef __cplusplus
}
#endif

#endif /* LIBEBML2_CLASSES_H */
