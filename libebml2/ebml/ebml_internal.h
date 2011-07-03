/*
 * $Id$
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

#ifndef LIBEBML2_INTERNAL_H
#define LIBEBML2_INTERNAL_H

/*
 * ONLY INCLUDE THIS FILE IF YOU PLAN TO CREATE YOUR OWN EBML-BASED CLASS
 */

#include "corec/node/node.h"

#ifdef __cplusplus
extern "C" {
#endif

INTERNAL_C_API size_t GetIdLength(fourcc_t Id);

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
    void (*PostCreate)(ebml_element *p, const void *Cookie);
};

struct ebml_element
{
    nodetree Base;
    filepos_t DataSize; // size of the data inside the element
    filepos_t ElementPosition;
    filepos_t SizePosition; // TODO: is this needed since we have the ElementPosition and SizeLength ?
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

#endif /* LIBEBML2_INTERNAL_H */
