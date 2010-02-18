/*
 * $Id: ebmlnumber.c 1319 2008-09-19 16:08:57Z robux4 $
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

static err_t ReadDataInt(ebml_integer *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    err_t Result;
    char Buffer[8];
    int i;

    assert(Element->Base.Size <= 8);

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    Result = Stream_Read(Input,Buffer,(size_t)Element->Base.Size,NULL);
    if (Result != ERR_NONE)
        return Result;

    Element->Value = 0;
	for (i=0; i<(int)Element->Base.Size; i++)
	{
		Element->Value <<= 8;
		Element->Value |= (uint8_t)Buffer[i];
	}
    Element->Base.bValueIsSet = 1;
failed:
    return Result;
}

static err_t ReadDataSignedInt(ebml_integer *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    err_t Result;
    char Buffer[8];
    int i;

    assert(Element->Base.Size <= 8);

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    Result = Stream_Read(Input,Buffer,(size_t)Element->Base.Size,NULL);
    if (Result != ERR_NONE)
        goto failed;

    if (Buffer[0] & 0x80) // check wether it's a positive or negative value
        Element->Value = -1;
    else
        Element->Value = 0;
	for (i=0; i<(int)Element->Base.Size; i++)
	{
		Element->Value <<= 8;
		Element->Value |= Buffer[i];
	}
    Element->Base.bValueIsSet = 1;
failed:
    return Result;
}

static err_t RenderDataSignedInt(ebml_integer *Element, stream *Output, bool_t bForceRender, bool_t bKeepIntact, filepos_t *Rendered)
{
	uint8_t FinalData[8]; // we don't handle more than 64 bits integers
	size_t i;
	int64_t TempValue = Element->Value;
    err_t Err;
	
	if (Element->Base.SizeLength > 8)
		return 0; // integers larger than 64 bits are not supported

    if (Element->Base.Size == 0)
        return 0; // nothing to write
	
	TempValue = Element->Value;
	for (i=(size_t)Element->Base.Size;i;--i) {
		FinalData[i-1] = (uint8_t)(TempValue & 0xFF);
		TempValue >>= 8;
	}
	
    Err = Stream_Write(Output,FinalData,(size_t)Element->Base.Size,&i);
    if (Rendered)
        *Rendered = i;
    return Err;
}

static err_t RenderDataInt(ebml_integer *Element, stream *Output, bool_t bForceRender, bool_t bKeepIntact, filepos_t *Rendered)
{
	uint8_t FinalData[8]; // we don't handle more than 64 bits integers
	size_t i;
	uint64_t TempValue = Element->Value;
    err_t Err;
	
	if (Element->Base.SizeLength > 8)
		return 0; // integers larger than 64 bits are not supported

    if (Element->Base.Size == 0)
        return 0; // nothing to write
	
	TempValue = Element->Value;
	for (i=(size_t)Element->Base.Size;i;--i) {
		FinalData[i-1] = (uint8_t)(TempValue & 0xFF);
		TempValue >>= 8;
	}
	
    Err = Stream_Write(Output,FinalData,(size_t)Element->Base.Size,&i);
    if (Rendered)
        *Rendered = i;
    return Err;
}

static bool_t ValidateSizeInt(ebml_element *p)
{
    return (p->Size <= 8);
}

static bool_t ValidateSizeFloat(ebml_element *p)
{
    return (p->Size == 8 || p->Size == 4);
}

static err_t ReadDataFloat(ebml_float *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    uint8_t Value[8];
    err_t Result;

	assert(Element->Base.Size == 8 || Element->Base.Size == 4);

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    Result = Stream_Read(Input,Value,min((size_t)Element->Base.Size,sizeof(Value)),NULL); // min is for code safety
    if (Result != ERR_NONE)
        goto failed;
	
	if (Element->Base.Size == 4) {
        float Val;
        Element->IsSimplePrecision = 1;
#ifdef IS_BIG_ENDIAN
        memcpy(&Val,Value,4);
#else
        ((uint8_t*)&Val)[0] = Value[3];
        ((uint8_t*)&Val)[1] = Value[2];
        ((uint8_t*)&Val)[2] = Value[1];
        ((uint8_t*)&Val)[3] = Value[0];
#endif
        Element->Value = Val;
        Element->Base.bValueIsSet = 1;
	} else if (Element->Base.Size == 8) {
        Element->IsSimplePrecision = 0;
#ifdef IS_BIG_ENDIAN
        memcpy(&Element->Value,Value,8);
#else
        ((uint8_t*)&Element->Value)[0] = Value[7];
        ((uint8_t*)&Element->Value)[1] = Value[6];
        ((uint8_t*)&Element->Value)[2] = Value[5];
        ((uint8_t*)&Element->Value)[3] = Value[4];
        ((uint8_t*)&Element->Value)[4] = Value[3];
        ((uint8_t*)&Element->Value)[5] = Value[2];
        ((uint8_t*)&Element->Value)[6] = Value[1];
        ((uint8_t*)&Element->Value)[7] = Value[0];
#endif
        Element->Base.bValueIsSet = 1;
	} else
        Result = ERR_INVALID_PARAM;
failed:
    return Result;
}

static bool_t IsDefaultValueInt(const ebml_integer *Element)
{
    return Element->Base.Context->HasDefault && (!Element->Base.bValueIsSet || (Element->Value == (int64_t)Element->Base.Context->DefaultValue));
}

static bool_t IsDefaultValueFloat(const ebml_float *Element)
{
    return Element->Base.Context->HasDefault && (!Element->Base.bValueIsSet || (Element->Value == (double)Element->Base.Context->DefaultValue));
}

static filepos_t UpdateSizeSignedInt(ebml_integer *Element, bool_t bKeepIntact, bool_t bForceRender)
{
	if (!bKeepIntact && IsDefaultValueInt(Element))
		return 0;

	if (Element->Value <= 0x7F && Element->Value >= (-0x80)) {
		Element->Base.Size = 1;
	} else if (Element->Value <= 0x7FFF && Element->Value >= (-0x8000)) {
		Element->Base.Size = 2;
	} else if (Element->Value <= 0x7FFFFF && Element->Value >= (-0x800000)) {
		Element->Base.Size = 3;
	} else if (Element->Value <= (int64_t)(0x7FFFFFFF) && Element->Value >= (int64_t)(-0x80000000)) {
		Element->Base.Size = 4;
	} else if (Element->Value <= 0x7FFFFFFFFF && Element->Value >= (-0x8000000000)) {
		Element->Base.Size = 5;
	} else if (Element->Value <= 0x7FFFFFFFFFFF && Element->Value >= (-0x800000000000)) {
		Element->Base.Size = 6;
	} else if (Element->Value <= 0x7FFFFFFFFFFFFF && Element->Value >= (-0x80000000000000)) {
		Element->Base.Size = 7;
	} else
		Element->Base.Size = 8;

	if (Element->Base.DefaultSize > Element->Base.Size) {
		Element->Base.Size = Element->Base.DefaultSize;
	}

	return Element->Base.Size;
}

static filepos_t UpdateSizeInt(ebml_integer *Element, bool_t bKeepIntact, bool_t bForceRender)
{
	if (!bKeepIntact && IsDefaultValueInt(Element))
		return 0;

	if (Element->Value <= 0xFF) {
		Element->Base.Size = 1;
	} else if (Element->Value <= 0xFFFF) {
		Element->Base.Size = 2;
	} else if (Element->Value <= 0xFFFFFF) {
		Element->Base.Size = 3;
	} else if (Element->Value <= 0xFFFFFFFF) {
		Element->Base.Size = 4;
	} else if (Element->Value <= 0xFFFFFFFFFF) {
		Element->Base.Size = 5;
	} else if (Element->Value <= 0xFFFFFFFFFFFF) {
		Element->Base.Size = 6;
	} else if (Element->Value <= 0xFFFFFFFFFFFFFF) {
		Element->Base.Size = 7;
	} else
		Element->Base.Size = 8;

	if (Element->Base.DefaultSize > Element->Base.Size) {
		Element->Base.Size = Element->Base.DefaultSize;
	}

	return Element->Base.Size;
}

static void PostCreateInt(ebml_element *Element)
{
    INHERITED(Element,ebml_element_vmt,EBML_INTEGER_CLASS)->PostCreate(Element);
    Element->DefaultSize = 1;
}


META_START(EBMLInteger_Class,EBML_INTEGER_CLASS)
META_CLASS(SIZE,sizeof(ebml_integer))
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValueInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadDataInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderDataInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateSizeInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreateInt)
META_END_CONTINUE(EBML_ELEMENT_CLASS)

META_START_CONTINUE(EBML_SINTEGER_CLASS)
META_CLASS(SIZE,sizeof(ebml_integer))
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValueInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadDataSignedInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderDataSignedInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateSizeSignedInt)
META_END_CONTINUE(EBML_ELEMENT_CLASS)

META_START_CONTINUE(EBML_BOOLEAN_CLASS)
META_END_CONTINUE(EBML_INTEGER_CLASS)

META_START_CONTINUE(EBML_FLOAT_CLASS)
META_CLASS(SIZE,sizeof(ebml_float))
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValueFloat)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeFloat)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadDataFloat)
META_END(EBML_ELEMENT_CLASS)
