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

static err_t ReadDataInt(ebml_integer *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    err_t Result;
    char Buffer[8];
    int i;

    assert(Element->Base.DataSize <= 8);

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    Result = Stream_Read(Input,Buffer,(size_t)Element->Base.DataSize,NULL);
    if (Result != ERR_NONE)
        return Result;

    Element->Value = 0;
	for (i=0; i<(int)Element->Base.DataSize; i++)
	{
		Element->Value <<= 8;
		Element->Value |= (uint8_t)Buffer[i];
	}
    Element->Base.bValueIsSet = 1;
failed:
    return Result;
}

static err_t ReadDataSignedInt(ebml_integer *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    err_t Result;
    char Buffer[8];
    int i;

    assert(Element->Base.DataSize <= 8);

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    Result = Stream_Read(Input,Buffer,(size_t)Element->Base.DataSize,NULL);
    if (Result != ERR_NONE)
        goto failed;

    if (Buffer[0] & 0x80) // check wether it's a positive or negative value
        Element->Value = -1;
    else
        Element->Value = 0;
	for (i=0; i<(int)Element->Base.DataSize; i++)
	{
		Element->Value <<= 8;
		Element->Value |= Buffer[i];
	}
    Element->Base.bValueIsSet = 1;
failed:
    return Result;
}

#if defined(CONFIG_EBML_WRITING)
static err_t RenderDataSignedInt(ebml_integer *Element, stream *Output, bool_t bForceWithoutMandatory, bool_t bWithDefault, filepos_t *Rendered)
{
	uint8_t FinalData[8]; // we don't handle more than 64 bits integers
	size_t i;
	int64_t TempValue = Element->Value;
    err_t Err;
	
	if (Element->Base.SizeLength > EBML_MAX_SIZE)
		return 0; // integers larger than 64 bits are not supported

    if (Element->Base.DataSize == 0)
        return 0; // nothing to write
	
	TempValue = Element->Value;
    if (Element->Base.DefaultSize > Element->Base.DataSize)
    {
        for (i=Element->Base.DefaultSize - (int)Element->Base.DataSize - 1;i;--i)
            FinalData[i-1] = 0;
        i = Element->Base.DefaultSize - (int)Element->Base.DataSize;
    }
    else
        i=(size_t)Element->Base.DataSize;
	for (;i;--i) {
		FinalData[i-1] = (uint8_t)(TempValue & 0xFF);
		TempValue >>= 8;
	}
	
    if (Element->Base.DefaultSize > Element->Base.DataSize)
        Err = Stream_Write(Output,FinalData,(size_t)Element->Base.DefaultSize,&i);
    else
        Err = Stream_Write(Output,FinalData,(size_t)Element->Base.DataSize,&i);
    if (Rendered)
        *Rendered = i;
    return Err;
}

static err_t RenderDataInt(ebml_integer *Element, stream *Output, bool_t bForceWithoutMandatory, bool_t bWithDefault, filepos_t *Rendered)
{
	uint8_t FinalData[8]; // we don't handle more than 64 bits integers
	size_t i;
	uint64_t TempValue = Element->Value;
    err_t Err;

	if (Element->Base.SizeLength > EBML_MAX_SIZE)
		return 0; // integers larger than 64 bits are not supported

    if (Element->Base.DataSize == 0)
        return 0; // nothing to write
	
	TempValue = Element->Value;
    if (Element->Base.DefaultSize > Element->Base.DataSize)
    {
        for (i=Element->Base.DefaultSize - (int)Element->Base.DataSize - 1;i;--i)
            FinalData[i-1] = 0;
        i = Element->Base.DefaultSize - (int)Element->Base.DataSize;
    }
    else
        i=(size_t)Element->Base.DataSize;
	for (;i;--i) {
		FinalData[i-1] = (uint8_t)(TempValue & 0xFF);
		TempValue >>= 8;
	}
	
    if (Element->Base.DefaultSize > Element->Base.DataSize)
        Err = Stream_Write(Output,FinalData,(size_t)Element->Base.DefaultSize,&i);
    else
        Err = Stream_Write(Output,FinalData,(size_t)Element->Base.DataSize,&i);
    if (Rendered)
        *Rendered = i;
    return Err;
}

static err_t RenderDataFloat(ebml_float *Element, stream *Output, bool_t bForceWithoutMandatory, bool_t bWithDefault, filepos_t *Rendered)
{
    err_t Err;
	size_t i = 0;
    if (Element->Base.DataSize == 8)
    {
        union {
            uint32_t i;
            double f;
        } data;
        uint64_t Buf;
        data.f = Element->Value;
        Buf = LOAD64BE(&data.i);
        Err = Stream_Write(Output,&Buf,8,&i);
    }
    else
    {
        union {
            uint32_t i;
            float f;
        } data;
        uint32_t Buf;
        data.f = (float)Element->Value;
        Buf = LOAD32BE(&data.i);
        Err = Stream_Write(Output,&Buf,4,&i);
    }
    if (Rendered)
        *Rendered = i;
    return Err;
}
#endif

static bool_t ValidateSizeInt(const ebml_element *p)
{
    return EBML_ElementIsFiniteSize(p) && (p->DataSize <= 8);
}

static bool_t ValidateSizeFloat(const ebml_element *p)
{
    return EBML_ElementIsFiniteSize(p) && (p->DataSize == 8 || p->DataSize == 4);
}

static err_t ReadDataFloat(ebml_float *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    uint8_t Value[8];
    err_t Result;

	assert(Element->Base.DataSize == 8 || Element->Base.DataSize == 4);

    Element->Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

    Result = Stream_Read(Input,Value,min((size_t)Element->Base.DataSize,sizeof(Value)),NULL); // min is for code safety
    if (Result != ERR_NONE)
        goto failed;
	
	if (Element->Base.DataSize == 4) {
        union {
            float f;
            uint32_t i;
        } data;
        data.i = LOAD32BE(Value);
        Element->Value = data.f;
        Element->Base.bValueIsSet = 1;
	} else if (Element->Base.DataSize == 8) {
        union {
            double f;
            uint64_t i;
        } data;
        data.i = LOAD64BE(Value);
        Element->Value = data.f;
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

static filepos_t UpdateSizeSignedInt(ebml_integer *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory)
{
    if (EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault))
    {
	    if (Element->Value <= 0x7F && Element->Value >= (-0x80)) {
		    Element->Base.DataSize = 1;
	    } else if (Element->Value <= 0x7FFF && Element->Value >= (-0x8000)) {
		    Element->Base.DataSize = 2;
	    } else if (Element->Value <= 0x7FFFFF && Element->Value >= (-0x800000)) {
		    Element->Base.DataSize = 3;
	    } else if (Element->Value <= (int64_t)(0x7FFFFFFF) && Element->Value >= -(int64_t)(0x80000000)) {
		    Element->Base.DataSize = 4;
	    } else if (Element->Value <= 0x7FFFFFFFFF && Element->Value >= (-0x8000000000)) {
		    Element->Base.DataSize = 5;
	    } else if (Element->Value <= 0x7FFFFFFFFFFF && Element->Value >= (-0x800000000000)) {
		    Element->Base.DataSize = 6;
	    } else if (Element->Value <= 0x7FFFFFFFFFFFFF && Element->Value >= (-0x80000000000000)) {
		    Element->Base.DataSize = 7;
	    } else
		    Element->Base.DataSize = 8;
    }

	return INHERITED(Element,ebml_element_vmt,EBML_SINTEGER_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory);
}

static filepos_t UpdateSizeInt(ebml_integer *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory)
{
    if (EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault))
    {
	    if ((uint64_t)Element->Value <= 0xFF) {
		    Element->Base.DataSize = 1;
	    } else if ((uint64_t)Element->Value <= 0xFFFF) {
		    Element->Base.DataSize = 2;
	    } else if ((uint64_t)Element->Value <= 0xFFFFFF) {
		    Element->Base.DataSize = 3;
	    } else if ((uint64_t)Element->Value <= 0xFFFFFFFF) {
		    Element->Base.DataSize = 4;
	    } else if ((uint64_t)Element->Value <= 0xFFFFFFFFFF) {
		    Element->Base.DataSize = 5;
	    } else if ((uint64_t)Element->Value <= 0xFFFFFFFFFFFF) {
		    Element->Base.DataSize = 6;
	    } else if ((uint64_t)Element->Value <= 0xFFFFFFFFFFFFFF) {
		    Element->Base.DataSize = 7;
	    } else
		    Element->Base.DataSize = 8;
    }

	return INHERITED(Element,ebml_element_vmt,EBML_INTEGER_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory);
}

static void PostCreateInt(ebml_element *Element, bool_t SetDefault)
{
    INHERITED(Element,ebml_element_vmt,EBML_INTEGER_CLASS)->PostCreate(Element, SetDefault);
    Element->DefaultSize = 1;
    if (SetDefault && Element->Context->HasDefault)
        EBML_IntegerSetValue((ebml_integer*)Element, Element->Context->DefaultValue);
}

static void PostCreateSignedInt(ebml_element *Element, bool_t SetDefault)
{
    INHERITED(Element,ebml_element_vmt,EBML_SINTEGER_CLASS)->PostCreate(Element, SetDefault);
    Element->DefaultSize = 1;
    if (SetDefault && Element->Context->HasDefault)
        EBML_IntegerSetValue((ebml_integer*)Element, Element->Context->DefaultValue);
}

static void PostCreateFloat(ebml_element *Element, bool_t SetDefault)
{
    INHERITED(Element,ebml_element_vmt,EBML_FLOAT_CLASS)->PostCreate(Element, SetDefault);
    Element->DefaultSize = 4;
    if (SetDefault && Element->Context->HasDefault)
        EBML_FloatSetValue((ebml_float*)Element, Element->Context->DefaultValue);
}

static ebml_integer *CopyInt(const ebml_integer *Element, const void *Cookie)
{
    ebml_integer *Result = (ebml_integer*)EBML_ElementCreate(Element,Element->Base.Context,0,Cookie);
    if (Result)
    {
        Result->Value = Element->Value;
        Result->Base.bValueIsSet = Element->Base.bValueIsSet;
        Result->Base.DataSize = Element->Base.DataSize;
        Result->Base.ElementPosition = Element->Base.ElementPosition;
        Result->Base.SizeLength = Element->Base.SizeLength;
        Result->Base.SizePosition = Element->Base.SizePosition;
        Result->Base.bNeedDataSizeUpdate = Element->Base.bNeedDataSizeUpdate;
    }
    return Result;
}

static ebml_float *CopyFloat(const ebml_float *Element, const void *Cookie)
{
    ebml_float *Result = (ebml_float*)EBML_ElementCreate(Element,Element->Base.Context,0,Cookie);
    if (Result)
    {
        Result->Value = Element->Value;
        Result->Base.bValueIsSet = Element->Base.bValueIsSet;
        Result->Base.DataSize = Element->Base.DataSize;
        Result->Base.ElementPosition = Element->Base.ElementPosition;
        Result->Base.SizeLength = Element->Base.SizeLength;
        Result->Base.SizePosition = Element->Base.SizePosition;
        Result->Base.bNeedDataSizeUpdate = Element->Base.bNeedDataSizeUpdate;
    }
    return Result;
}

META_START(EBMLInteger_Class,EBML_INTEGER_CLASS)
META_CLASS(SIZE,sizeof(ebml_integer))
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValueInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadDataInt)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderDataInt)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateSizeInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreateInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,CopyInt)
META_END_CONTINUE(EBML_ELEMENT_CLASS)

META_START_CONTINUE(EBML_SINTEGER_CLASS)
META_CLASS(SIZE,sizeof(ebml_integer))
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValueInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadDataSignedInt)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderDataSignedInt)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateSizeSignedInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreateSignedInt)
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,CopyInt)
META_END_CONTINUE(EBML_ELEMENT_CLASS)

META_START_CONTINUE(EBML_BOOLEAN_CLASS)
META_END_CONTINUE(EBML_INTEGER_CLASS)

META_START_CONTINUE(EBML_FLOAT_CLASS)
META_CLASS(SIZE,sizeof(ebml_float))
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValueFloat)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeFloat)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadDataFloat)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreateFloat)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderDataFloat)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,CopyFloat)
META_END(EBML_ELEMENT_CLASS)

void EBML_IntegerSetValue(ebml_integer *Element, int64_t Value)
{
    Element->Value = Value;
    Element->Base.bValueIsSet = 1;
    Element->Base.bNeedDataSizeUpdate = 1;
}

int64_t EBML_IntegerValue(const ebml_integer *Element)
{
#if !defined(NDEBUG)
    assert(Node_IsPartOf(Element,EBML_INTEGER_CLASS) || Node_IsPartOf(Element,EBML_SINTEGER_CLASS));
    assert(Element->Base.bValueIsSet);
#endif
    return Element->Value;
}

double EBML_FloatValue(const ebml_float *Element)
{
#if !defined(NDEBUG)
    assert(Element->Base.bValueIsSet);
#endif
    return Element->Value;
}

void EBML_FloatSetValue(ebml_float *Element, double Value)
{
    Element->Value = Value;
    Element->Base.bValueIsSet = 1;
    Element->Base.bNeedDataSizeUpdate = 1;
}
