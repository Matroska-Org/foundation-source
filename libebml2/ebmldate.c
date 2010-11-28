/*
 * $Id$
 * Copyright (c) 2008-2010, Matroska (non-profit organisation)
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
#include "ebml/ebml.h"
#include "ebml/ebml_internal.h"

datetime_t EBML_DateTime(const ebml_date *Element)
{
    assert(Node_IsPartOf(Element,EBML_DATE_CLASS));
    if (!Element->Base.bValueIsSet)
        return 0;
    return (datetime_t)Scale32(Element->Value,1,1000000000); // nanoseconds to seconds
}

err_t EBML_DateSetDateTime(ebml_date *Element, datetime_t Date)
{
    if (Date == INVALID_DATETIME_T)
        return ERR_INVALID_PARAM;
    EBML_IntegerSetValue(Element, Scale64(Date,1000000000,1)); // seconds to nanoseconds
    return ERR_NONE;
}

static bool_t ValidateSize(const ebml_element *p)
{
    return EBML_ElementIsFiniteSize(p) && (p->DataSize == 8 || p->DataSize == 0);
}

static err_t ReadData(ebml_date *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    err_t Result;
    int DataSize;
    uint8_t Value[8];

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    assert(Element->Base.DataSize<=8);
    Result = Stream_Read(Input,Value,(size_t)Element->Base.DataSize,NULL);
    if (Result != ERR_NONE)
        goto failed;

#ifdef IS_BIG_ENDIAN
    memcpy(&Element->Value,Value,Element->Base.DataSize);
#else
    Element->Value = 0;
    for (DataSize=0;DataSize<Element->Base.DataSize;++DataSize)
        ((uint8_t*)&Element->Value)[DataSize] = Value[Element->Base.DataSize-DataSize-1];
#endif
    Element->Base.bValueIsSet = 1;
failed:
    return Result;
}

static void PostCreate(ebml_date *Element, bool_t SetDefault)
{
    INHERITED(Element,ebml_element_vmt,EBML_DATE_CLASS)->PostCreate(Element, SetDefault);
    Element->Base.DefaultSize = 8;
    Element->Base.bNeedDataSizeUpdate = 0;
}

META_START(EBMLDate_Class,EBML_DATE_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreate)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_END(EBML_SINTEGER_CLASS)
