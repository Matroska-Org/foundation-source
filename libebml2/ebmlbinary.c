/*
 * Copyright (c) 2008-2010, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "ebml2/ebml.h"
#include "internal.h"
#include <corec/helpers/file/streams.h>

static err_t ReadData(ebml_binary *Element, struct stream *Input, const ebml_parser_context *UNUSED_PARAM(ParserContext), bool_t UNUSED_PARAM(AllowDummyElt), int Scope, size_t UNUSED_PARAM(DepthCheckCRC))
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

#if MAX_FILEPOS >= SIZE_MAX
    if ((filepos_t)Element->Base.DataSize > (filepos_t)SIZE_MAX
#else
    if ((size_t)Element->Base.DataSize > (size_t)SIZE_MAX
#endif
        || !ArrayResize(&Element->Data,(size_t)Element->Base.DataSize,0))
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
static err_t RenderData(ebml_binary *Element, struct stream *Output, bool_t UNUSED_PARAM(bForceWithoutMandatory), bool_t UNUSED_PARAM(bWithDefault), int UNUSED_PARAM(ForProfile), filepos_t *Rendered)
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

static bool_t IsDefaultValue(const ebml_binary *UNUSED_PARAM(Element))
{
    return 0; // TODO: a default binary value needs a size too (use a structure to set the value in the structure)
}

static filepos_t UpdateDataSize(ebml_binary *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory, int ForProfile)
{
	Element->Base.DataSize = ARRAYCOUNT(Element->Data,uint8_t);

	return INHERITED(Element,ebml_element_vmt,EBML_BINARY_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory, ForProfile);
}

static bool_t ValidateSize(const ebml_element *p)
{
    return EBML_ElementIsFiniteSize(p);
}

static ebml_binary *Copy(const ebml_binary *Element)
{
    ebml_binary *Result = (ebml_binary*)EBML_ElementCreate(Element,Element->Base.Context,0,EBML_ANY_PROFILE);
    if (Result)
    {
        ArrayCopy(&Result->Data,&Element->Data);
        Result->Base.bValueIsSet = Element->Base.bValueIsSet;
        Result->Base.DataSize = Element->Base.DataSize;
        Result->Base.ElementPosition = Element->Base.ElementPosition;
        Result->Base.SizeLength = Element->Base.SizeLength;
        Result->Base.SizePosition = Element->Base.SizePosition;
        Result->Base.EndPosition = Element->Base.EndPosition;
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
