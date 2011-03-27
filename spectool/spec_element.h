/*
 * $Id$
 * Copyright (c) 2011, Matroska (non-profit organisation)
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

#ifndef SPEC_ELEMENT_H
#define SPEC_ELEMENT_H

#ifndef CONFIG_EBML_UNICODE
#define CONFIG_EBML_UNICODE
#endif
#include "parser/parser.h"

#define SPEC_ELEMENT_CLASS  FOURCC('S','P','E','C')

typedef enum {
    EBML_unknown,
    EBML_MASTER,
    EBML_INTEGER,
    EBML_UNSIGNED_INTEGER,
    EBML_DATE,
    EBML_FLOAT,
    EBML_STRING,
    EBML_UNICODE_STRING,
    EBML_BINARY,
} ebml_type;

typedef struct SpecElement {
    node Base;

    tchar_t Name[MAXPATH];
    tchar_t CppName[MAXPATH];
    int Level;
    bool_t Recursive;
    int32_t Id;
    ebml_type Type;
    bool_t Mandatory;
    bool_t Multiple;
    tchar_t Range[MAXPATH];
    tchar_t DefaultValue[32];
    int MinVersion, MaxVersion;
    int ByteSize;
    bool_t InWebM;
    bool_t InDivX;
    tchar_t Description[MAXLINE];
} SpecElement;

void ReadElementText(parser *p, tchar_t *Out, size_t OutLen);
void ReadSpecElement(SpecElement *elt, parser *p);

#endif /* SPEC_ELEMENT_H */
