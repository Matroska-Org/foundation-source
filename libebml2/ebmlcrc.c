/*
 * $Id$
 * Copyright (c) 2008, Matroska Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Matroska Foundation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY The Matroska Foundation ``AS IS'' AND ANY
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
#include "ebml/ebml.h"

typedef struct ebml_crc ebml_crc; 

struct ebml_crc
{
    ebml_element Base;
    uint32_t CRC;
};

static bool_t ValidateSize(const ebml_element *p)
{
    return EBML_ElementIsFiniteSize(p) && (p->DataSize == 4);
}

static err_t ReadData(ebml_crc *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    err_t Result;
    uint32_t CRCbuffer;
    Result = Stream_Read(Input,&CRCbuffer,4,NULL);
    if (Result == ERR_NONE)
    {
        Element->CRC = LOAD32BE(&CRCbuffer);
        Element->Base.bValueIsSet = 1;
    }
    return Result;
}

static ebml_crc *Copy(const ebml_crc *Element, const void *Cookie)
{
    ebml_crc *Result = (ebml_crc*)EBML_ElementCreate(Element,Element->Base.Context,0,Cookie);
    if (Result)
    {
        Result->CRC = Element->CRC;
        Result->Base.bValueIsSet = Element->Base.bValueIsSet;
        Result->Base.bDefaultIsSet = Element->Base.bDefaultIsSet;
        Result->Base.DataSize = Element->Base.DataSize;
        Result->Base.ElementPosition = Element->Base.ElementPosition;
        Result->Base.SizeLength = Element->Base.SizeLength;
        Result->Base.SizePosition = Element->Base.SizePosition;
    }
    return Result;
}

META_START(EBMLCRC_Class,EBML_CRC_CLASS)
META_CLASS(SIZE,sizeof(ebml_crc))
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,Copy)
META_END(EBML_ELEMENT_CLASS)
