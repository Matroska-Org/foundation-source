/*
 * Copyright (c) 2008-2010, Matroska (non-profit organisation)
 * Copyright (C) the Matroska association
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "ebml2/ebml.h"
#include "internal.h"
#include <corec/helpers/file/streams.h>

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

static err_t ReadData(ebml_date *Element, struct stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
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
    if (Element->Base.DataSize > 8)
    {
        Result = ERR_INVALID_DATA;
        goto failed;
    }

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

static void PostCreate(ebml_date *Element, bool_t SetDefault, int ForProfile)
{
    INHERITED(Element,ebml_element_vmt,EBML_DATE_CLASS)->PostCreate(Element, SetDefault, ForProfile);
    Element->Base.DefaultSize = 8;
    Element->Base.bNeedDataSizeUpdate = 0;
}

META_START(EBMLDate_Class,EBML_DATE_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreate)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_END(EBML_SINTEGER_CLASS)
