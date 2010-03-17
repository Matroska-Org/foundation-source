/*
 * $Id: ebmlstring.c 1320 2008-10-04 15:03:47Z robux4 $
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

#if defined(CONFIG_EBML_UNICODE)
err_t EBML_UniStringSetValue(ebml_string *Element,const tchar_t *Value)
{
    char Data[2048];
    if (!Node_IsPartOf(Element,EBML_UNISTRING_CLASS))
        return ERR_INVALID_DATA;
    Node_ToUTF8(Element,Data,sizeof(Data)-1,Value);
    Data[sizeof(Data)-1] = 0;
    return EBML_StringSetValue(Element,Data);
}
#endif

err_t EBML_StringSetValue(ebml_string *Element,const char *Value)
{
    Element->Buffer = strdup(Value);
    Element->Base.bValueIsSet = 1;
    Element->Base.Size = strlen(Element->Buffer);
    return ERR_NONE;
}

static err_t ReadData(ebml_string *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    err_t Result;
    char *Buffer;

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    Buffer = malloc((size_t)Element->Base.Size+1);
    if (!Buffer)
        return ERR_OUT_OF_MEMORY;

    Result = Stream_Read(Input,Buffer,(size_t)Element->Base.Size,NULL);
    if (Result != ERR_NONE)
        goto failed;

    Buffer[Element->Base.Size] = 0;
    Element->Buffer = Buffer;
    Element->Base.bValueIsSet = 1;
    return ERR_NONE;

failed:
    free(Buffer);
    Element->Buffer = NULL;
    return Result;
}

#if defined(CONFIG_EBML_WRITING)
static err_t RenderData(ebml_string *Element, stream *Output, bool_t bForceRender, bool_t bKeepIntact, filepos_t *Rendered)
{
    size_t Written;
    err_t Err = Stream_Write(Output,Element->Buffer,(size_t)Element->Base.Size,&Written);
    if (Rendered)
        *Rendered = Written;
    if ((Err == ERR_NONE) && (Element->Base.DefaultSize > (int)Element->Base.Size))
    {
        char *Padding = malloc(Element->Base.DefaultSize - (int)Element->Base.Size);
        if (!Padding)
            return ERR_OUT_OF_MEMORY;
        memset(Padding,0,Element->Base.DefaultSize - (int)Element->Base.Size);
        Err = Stream_Write(Output,Padding,Element->Base.DefaultSize - (int)Element->Base.Size,&Written);
        if (Rendered)
            *Rendered += Written;
        free(Padding);
    }
    return Err;
}
#endif

#if 0
err_t EBML_AsciiStringRead(ebml_string *Element, stream *Input, tchar_t *Out, size_t OutLen)
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

err_t EBML_UnicodeStringRead(ebml_string *Element, stream *Input, tchar_t *Out, size_t OutLen)
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

static filepos_t UpdateSize(ebml_string *Element, bool_t bKeepIntact, bool_t bForceRender)
{
	if (!bKeepIntact && EBML_ElementIsDefaultValue(Element))
		return 0;

	Element->Base.Size = strlen(Element->Buffer);

	if (Element->Base.DefaultSize > Element->Base.Size) {
		Element->Base.Size = Element->Base.DefaultSize;
	}

	return Element->Base.Size;
}

static bool_t IsDefaultValue(const ebml_string *Element)
{
    return Element->Base.Context->HasDefault && (!Element->Base.bValueIsSet || strcmp(Element->Buffer,(const char*)Element->Base.Context->DefaultValue)==0);
}

META_START(EBMLString_Class,EBML_STRING_CLASS)
META_CLASS(SIZE,sizeof(ebml_string))
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValue)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateSize)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderData)
#endif
META_END_CONTINUE(EBML_ELEMENT_CLASS)

META_START_CONTINUE(EBML_UNISTRING_CLASS)
META_CLASS(SIZE,sizeof(ebml_string))
META_CLASS(DELETE,Delete)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValue)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateSize)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderData)
#endif
META_END(EBML_ELEMENT_CLASS)
