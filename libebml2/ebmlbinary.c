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

static err_t ReadData(ebml_binary *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    err_t Result;

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    if (!ArrayResize(&Element->Data,(size_t)Element->Base.DataSize,0))
    {
        Result = ERR_OUT_OF_MEMORY;
        goto failed;
    }

    Result = Stream_Read(Input,ARRAYBEGIN(Element->Data,void),(size_t)Element->Base.DataSize,NULL);
    if (Result == ERR_NONE)
        Element->Base.bValueIsSet = 1;
failed:
    return Result;
}

#if defined(CONFIG_EBML_WRITING)
static err_t RenderData(ebml_binary *Element, stream *Output, bool_t bForceWithoutMandatory, bool_t bWithDefault, filepos_t *Rendered)
{
    size_t Written;
    err_t Err = Stream_Write(Output,ARRAYBEGIN(Element->Data,uint8_t),ARRAYCOUNT(Element->Data,uint8_t),&Written);
    if (Rendered)
        *Rendered = Written;
    return Err;
}
#endif

static void Delete(ebml_binary *Element)
{
    ArrayClear(&Element->Data);
}

static bool_t IsDefaultValue(const ebml_binary *Element)
{
    return 0; // TODO: a default binary value needs a size too (use a structure to set the value in the structure)
}

static filepos_t UpdateDataSize(ebml_binary *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory)
{
	Element->Base.DataSize = ARRAYCOUNT(Element->Data,uint8_t);

	return INHERITED(Element,ebml_element_vmt,EBML_BINARY_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory);
}

static bool_t ValidateSize(const ebml_element *p)
{
    return EBML_ElementIsFiniteSize(p);
}

static ebml_binary *Copy(const ebml_binary *Element, const void *Cookie)
{
    ebml_binary *Result = (ebml_binary*)EBML_ElementCreate(Element,Element->Base.Context,0,Cookie);
    if (Result)
    {
        ArrayCopy(&Result->Data,&Element->Data);
        Result->Base.bValueIsSet = Element->Base.bValueIsSet;
        Result->Base.DataSize = Element->Base.DataSize;
        Result->Base.ElementPosition = Element->Base.ElementPosition;
        Result->Base.SizeLength = Element->Base.SizeLength;
        Result->Base.SizePosition = Element->Base.SizePosition;
        Result->Base.bNeedDataSizeUpdate = Element->Base.bNeedDataSizeUpdate;
    }
    return Result;
}

META_START(EBMLBinary_Class,EBML_BINARY_CLASS)
META_CLASS(SIZE,sizeof(ebml_binary))
META_CLASS(DELETE,Delete)
META_DATA(TYPE_ARRAY,0,ebml_binary,Data)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValue)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateDataSize)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderData)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,Copy)
META_END(EBML_ELEMENT_CLASS)

err_t EBML_BinarySetData(ebml_binary *Element, const uint8_t *Data, size_t DataSize)
{
    if (!ArrayResize(&Element->Data,DataSize,0))
        return ERR_OUT_OF_MEMORY;
    memcpy(ARRAYBEGIN(Element->Data,void),Data,DataSize);
    Element->Base.DataSize = DataSize;
    Element->Base.bNeedDataSizeUpdate = 0;
    Element->Base.bValueIsSet = 1;
    return ERR_NONE;
}

const uint8_t *EBML_BinaryGetData(ebml_binary *Element)
{
    if (!ARRAYCOUNT(Element->Data,uint8_t))
        return NULL;
    return ARRAYBEGIN(Element->Data,uint8_t);
}
