/*
 * Copyright (c) 2008-2010, Matroska (non-profit organisation)
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdlib.h>
#include "ebml2/ebml.h"
#include "internal.h"
#include <corec/helpers/file/streams.h>
#include <corec/helpers/parser/parser.h>
#include <string.h>

err_t EBML_UniStringSetValue(ebml_string *Element,const tchar_t *Value)
{
    char Data[2048];
    if (!Node_IsPartOf(Element,EBML_UNISTRING_CLASS))
        return ERR_INVALID_DATA;
    Node_ToUTF8(Element,Data,sizeof(Data)-1,Value);
    Data[sizeof(Data)-1] = 0;
    return EBML_StringSetValue(Element,Data);
}

err_t EBML_StringSetValue(ebml_string *Element,const char *Value)
{
    if (Element->Base.bValueIsSet && Element->Buffer)
        free((char*)Element->Buffer);
    Element->Base.DataSize = strlen(Value);
    Element->Buffer = malloc(Element->Base.DataSize + 1);
    if (Element->Buffer == NULL)
        return ERR_OUT_OF_MEMORY;
    strncpy((char*)Element->Buffer, Value, Element->Base.DataSize + 1);
    Element->Base.bValueIsSet = 1;
    Element->Base.bNeedDataSizeUpdate = 0;
    return ERR_NONE;
}

void EBML_StringGet(ebml_string *Element,tchar_t *Out, size_t OutLen)
{
    if (!Element->Buffer)
    {
        if (OutLen)
            *Out = 0;
    }
    else
    {
        if (Node_IsPartOf(Element,EBML_UNISTRING_CLASS))
            Node_FromUTF8(Element,Out,OutLen,Element->Buffer);
        else
            Node_FromStr(Element,Out,OutLen,Element->Buffer);
    }
}

static err_t ReadData(ebml_string *Element, struct stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    err_t Result;
    char *Buffer = NULL;

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }
#if MAX_FILEPOS >= SIZE_MAX
    if ((filepos_t)Element->Base.DataSize > (filepos_t)(SIZE_MAX - 1))
#else
    if ((size_t)Element->Base.DataSize > (size_t)(SIZE_MAX - 1))
#endif
    {
        Result = ERR_OUT_OF_MEMORY;
        goto failed;
    }

    Buffer = malloc((size_t)Element->Base.DataSize+1);
    if (!Buffer)
        return ERR_OUT_OF_MEMORY;

    Result = Stream_Read(Input,Buffer,(size_t)Element->Base.DataSize,NULL);
    if (Result != ERR_NONE)
        goto failed;

    Buffer[Element->Base.DataSize] = 0;
    Element->Buffer = Buffer;
    Element->Base.bValueIsSet = 1;
    return ERR_NONE;

failed:
    free(Buffer);
    Element->Buffer = NULL;
    return Result;
}

#if defined(CONFIG_EBML_WRITING)
static err_t RenderData(ebml_string *Element, struct stream *Output, bool_t bForceWithoutMandatory, bool_t bWithDefault, int ForProfile, filepos_t *Rendered)
{
    size_t Written;
    err_t Err = Stream_Write(Output,Element->Buffer,(size_t)Element->Base.DataSize,&Written);
    if (Rendered)
        *Rendered = Written;
    if ((Err == ERR_NONE) && (Element->Base.DefaultSize > (int)Element->Base.DataSize))
    {
        char *Padding = malloc(Element->Base.DefaultSize - (int)Element->Base.DataSize);
        if (!Padding)
            return ERR_OUT_OF_MEMORY;
        memset(Padding,0,Element->Base.DefaultSize - (int)Element->Base.DataSize);
        Err = Stream_Write(Output,Padding,Element->Base.DefaultSize - (int)Element->Base.DataSize,&Written);
        if (Rendered)
            *Rendered += Written;
        free(Padding);
    }
    return Err;
}
#endif

#if 0
err_t EBML_AsciiStringRead(ebml_string *Element, struct stream *Input, tchar_t *Out, size_t OutLen)
{
    if (Node_IsPartOf(Element,EBML_STRING_CLASS))
        return ERR_INVALID_DATA;
    else
    {
        err_t Result = EBML_ElementReadData(Element, Input);
        if (Result != ERR_NONE)
            return Result;
        Node_FromStr(Element,Out,OutLen,Element->Buffer);
        return Result;
    }
}

err_t EBML_UnicodeStringRead(ebml_string *Element, struct stream *Input, tchar_t *Out, size_t OutLen)
{
    if (Node_IsPartOf(Element,EBML_UNISTRING_CLASS))
        return ERR_INVALID_DATA;
    else
    {
        err_t Result = EBML_ElementReadData(Element, Input);
        if (Result != ERR_NONE)
            return Result;
        Node_FromUTF8(Element,Out,OutLen,Element->Buffer);
        return Result;
    }
}
#endif

static void Delete(ebml_string *p)
{
    if (p->Buffer)
        free((char*)p->Buffer);
}

static filepos_t UpdateDataSize(ebml_string *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory, int ForProfile)
{
    if (EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault))
        Element->Base.DataSize = strlen(Element->Buffer);

    return INHERITED(Element,ebml_element_vmt,EBML_STRING_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory, ForProfile);
}

static filepos_t UpdateDataSizeUni(ebml_string *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory, int ForProfile)
{
    if (EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault))
        Element->Base.DataSize = strlen(Element->Buffer);

    return INHERITED(Element,ebml_element_vmt,EBML_UNISTRING_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory, ForProfile);
}

static bool_t IsDefaultValue(const ebml_string *Element)
{
    return Element->Base.Context->HasDefault && (!Element->Base.bValueIsSet || strcmp(Element->Buffer,(const char*)Element->Base.Context->DefaultValue)==0);
}

static void PostCreateString(ebml_element *Element, bool_t SetDefault, int ForProfile)
{
    INHERITED(Element,ebml_element_vmt,EBML_STRING_CLASS)->PostCreate(Element, SetDefault, ForProfile);
    if (SetDefault && Element->Context->HasDefault)
        EBML_StringSetValue((ebml_string*)Element, (const char *)Element->Context->DefaultValue);
}

static void PostCreateUniString(ebml_element *Element, bool_t SetDefault, int ForProfile)
{
    INHERITED(Element,ebml_element_vmt,EBML_UNISTRING_CLASS)->PostCreate(Element, SetDefault, ForProfile);
    if (SetDefault && Element->Context->HasDefault)
        EBML_StringSetValue((ebml_string*)Element, (const char *)Element->Context->DefaultValue);
}

static ebml_string *Copy(const ebml_string *Element)
{
    ebml_string *Result = (ebml_string*)EBML_ElementCreate(Element,Element->Base.Context,0,EBML_ANY_PROFILE);
    if (Result)
    {
        Result->Buffer = malloc(Element->Base.DataSize + 1);
        if (Result->Buffer == NULL)
        {
            NodeDelete((node*)Result);
            return NULL;
        }
        strncpy((char*)Result->Buffer, Element->Buffer, Element->Base.DataSize + 1);
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

META_START(EBMLString_Class,EBML_STRING_CLASS)
META_CLASS(SIZE,sizeof(ebml_string))
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValue)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateDataSize)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderData)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreateString)
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,Copy)
META_END_CONTINUE(EBML_ELEMENT_CLASS)

META_START_CONTINUE(EBML_UNISTRING_CLASS)
META_CLASS(SIZE,sizeof(ebml_string))
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValue)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateDataSizeUni)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderData)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreateUniString)
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,Copy)
META_END(EBML_ELEMENT_CLASS)
