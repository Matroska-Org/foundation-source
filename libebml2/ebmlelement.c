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

static bool_t ValidateSize(const ebml_element *p)
{
    return 1;
}

static void PostCreate(ebml_element *Element, bool_t SetDefault)
{
    Element->DefaultSize = -1;
    Element->ElementPosition = INVALID_FILEPOS_T;
    Element->SizePosition = INVALID_FILEPOS_T;
}

static bool_t NeedsDataSizeUpdate(ebml_element *Element, bool_t bWithDefault)
{
    if (!Element->bNeedDataSizeUpdate)
        return 0;
    if (!bWithDefault && EBML_ElementIsDefaultValue(Element))
        return 0;
    return 1;
}

static filepos_t UpdateDataSize(ebml_element *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory)
{
	if (!bWithDefault && EBML_ElementIsDefaultValue(Element))
		return 0;

    if (Element->DefaultSize > Element->DataSize)
		Element->DataSize = Element->DefaultSize;

    Element->bNeedDataSizeUpdate = 0;
#if !defined(NDEBUG)
    assert(!EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault));
#endif
    return Element->DataSize;
}

static err_t Create(ebml_element *Element)
{
    Element->DataSize = INVALID_FILEPOS_T;
    Element->bNeedDataSizeUpdate = 1;
    return ERR_NONE;
}

META_START(EBMLElement_Class,EBML_ELEMENT_CLASS)
META_CLASS(SIZE,sizeof(ebml_element))
META_CLASS(VMT_SIZE,sizeof(ebml_element_vmt))
META_CLASS(FLAGS,CFLAG_ABSTRACT)
META_CLASS(CREATE,Create)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreate)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateDataSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,NeedsDataSizeUpdate,NeedsDataSizeUpdate)

META_PARAM(TYPE,EBML_ELEMENT_INFINITESIZE,TYPE_BOOLEAN)
META_DYNAMIC(TYPE_BOOLEAN,EBML_ELEMENT_INFINITESIZE)

META_PARAM(TYPE,EBML_ELEMENT_OBJECT,TYPE_PTR)
META_DYNAMIC(TYPE_PTR,EBML_ELEMENT_OBJECT)

META_END_CONTINUE(NODETREE_CLASS)

META_START_CONTINUE(EBML_DUMMY_ID)
META_CLASS(SIZE,sizeof(ebml_dummy))
META_END(EBML_BINARY_CLASS)

bool_t EBML_ElementIsFiniteSize(const ebml_element *Element)
{
    return (Node_GetData((const node*)Element,EBML_ELEMENT_INFINITESIZE,TYPE_BOOLEAN) == 0);
}

void EBML_ElementSetInfiniteSize(const ebml_element *Element, bool_t Set)
{
    bool_t b = Set;
    Node_SetData((node*)Element,EBML_ELEMENT_INFINITESIZE,TYPE_BOOLEAN,&b);
}

bool_t EBML_ElementIsDummy(const ebml_element *Element)
{
    return Node_IsPartOf(Element,EBML_DUMMY_ID);
}

ebml_element *EBML_ElementSkipData(ebml_element *p, stream *Input, const ebml_parser_context *Context, ebml_element *TestReadElt, bool_t AllowDummyElt)
{
	ebml_element *Result = NULL;
	if (EBML_ElementIsFiniteSize(p)) {
		assert(TestReadElt == NULL);
		assert(p->ElementPosition < p->SizePosition);
		Stream_Seek(Input, EBML_ElementPositionEnd(p), SEEK_SET);
	} else {
		// read elements until an upper element is found
		int bUpperElement = 0; // trick to call FindNextID correctly
		Result = EBML_FindNextElement(Input, Context, &bUpperElement, AllowDummyElt);
		if (Result != NULL)
            Stream_Seek(Input, EBML_ElementPositionData(Result), SEEK_SET);
	}
	return Result;
}

const ebml_context *EBML_ElementContext(const ebml_element *Element)
{
    return Element->Context;
}

filepos_t EBML_ElementFullSize(const ebml_element *Element, bool_t bWithDefault)
{
	if (!bWithDefault && EBML_ElementIsDefaultValue(Element))
		return INVALID_FILEPOS_T; // won't be saved
	return Element->DataSize + GetIdLength(Element->Context->Id) + EBML_CodedSizeLength(Element->DataSize, Element->SizeLength, EBML_ElementIsFiniteSize(Element));
}

filepos_t EBML_ElementDataSize(const ebml_element *Element, bool_t bWithDefault)
{
    return Element->DataSize;
}

void EBML_ElementForceDataSize(ebml_element *Element, filepos_t Size)
{
    Element->DataSize = Size;
}

uint8_t EBML_ElementSizeLength(const ebml_element *Element)
{
    return Element->SizeLength;
}

void EBML_ElementSetSizeLength(ebml_element *Element, uint8_t SizeLength)
{
    Element->SizeLength = SizeLength;
}

bool_t EBML_ElementIsType(const ebml_element *Element, const ebml_context *Context)
{
    return Element->Context->Id == Context->Id;
}

fourcc_t EBML_ElementClassID(const ebml_element *Element)
{
    return Element->Context->Id;
}

filepos_t EBML_ElementPosition(const ebml_element *Element)
{
    return Element->ElementPosition;
}

void EBML_ElementForcePosition(ebml_element *Element, filepos_t Pos)
{
    Element->ElementPosition = Pos;
}

filepos_t EBML_ElementPositionData(const ebml_element *Element)
{
    if (!EBML_ElementIsFiniteSize(Element))
    {
        if (!Element->SizeLength)
            return INVALID_FILEPOS_T;
        return Element->SizePosition + Element->SizeLength;
    }
    else
        return Element->SizePosition + EBML_CodedSizeLength(Element->DataSize,Element->SizeLength,1);
}

filepos_t EBML_ElementPositionEnd(const ebml_element *Element)
{
    if (!EBML_ElementIsFiniteSize(Element))
        return INVALID_FILEPOS_T; // the end position is unknown
    else
        return Element->SizePosition + EBML_CodedSizeLength(Element->DataSize,Element->SizeLength,1) + Element->DataSize;
}

bool_t EBML_ElementInfiniteForceSize(ebml_element *Element, filepos_t NewSize)
{
	int OldSizeLen;
	filepos_t OldSize;

    assert(!EBML_ElementIsFiniteSize(Element));
	if (EBML_ElementIsFiniteSize(Element))
		return 0;

	OldSizeLen = EBML_CodedSizeLength(Element->DataSize, Element->SizeLength, EBML_ElementIsFiniteSize(Element));
	OldSize = Element->DataSize;
	Element->DataSize = NewSize;

	if (EBML_CodedSizeLength(Element->DataSize, Element->SizeLength, EBML_ElementIsFiniteSize(Element)) == OldSizeLen)
    {
		EBML_ElementSetInfiniteSize(Element,1);
		return 1;
	}
	Element->DataSize = OldSize;

	return 0;
}

size_t GetIdLength(fourcc_t Id)
{
    if ((Id & 0xFFFFFF00)==0)
        return 1;
    if ((Id & 0xFFFF0000)==0)
        return 2;
    if ((Id & 0xFF000000)==0)
        return 3;
    return 4;
}

size_t EBML_FillBufferID(uint8_t *Buffer, size_t BufSize, fourcc_t Id)
{
    size_t i,FinalHeadSize = GetIdLength(Id);
    if (BufSize < FinalHeadSize)
        return 0;
    for (i=0;i<FinalHeadSize;++i)
        Buffer[FinalHeadSize-i-1] = (uint8_t)(Id >> (i<<3));
    return FinalHeadSize;
}

size_t EBML_IdToString(tchar_t *Out, size_t OutLen, fourcc_t Id)
{
    size_t i,FinalHeadSize = GetIdLength(Id);
	if (OutLen < FinalHeadSize*4+1)
		return 0;
	Out[0] = 0;
    for (i=0;i<4;++i)
	{
		if (Out[0] || (Id >> 8*(3-i)) & 0xFF)
			stcatprintf_s(Out,OutLen,T("[%02X]"),(Id >> 8*(3-i)) & 0xFF);
	}
	return FinalHeadSize*4;
}

fourcc_t EBML_BufferToID(const uint8_t *Buffer)
{
	if (Buffer[0] & 0x80)
		return (fourcc_t)Buffer[0];
	if (Buffer[0] & 0x40)
		return (fourcc_t)((Buffer[0] << 8) + Buffer[1]);
	if (Buffer[0] & 0x20)
		return (fourcc_t)((Buffer[0] << 16) + (Buffer[1] << 8) + Buffer[2]);
	if (Buffer[0] & 0x10)
		return (fourcc_t)((Buffer[0] << 24) + (Buffer[1] << 16) + (Buffer[2] << 8) + Buffer[3]);
	return 0;
}

#if defined(CONFIG_EBML_WRITING)
err_t EBML_ElementRender(ebml_element *Element, stream *Output, bool_t bWithDefault, bool_t bKeepPosition, bool_t bForceWithoutMandatory, filepos_t *Rendered)
{
    err_t Result;
    filepos_t _Rendered,WrittenSize;
#if !defined(NDEBUG)
    filepos_t SupposedSize;
#endif

    if (!Rendered)
        Rendered = &_Rendered;
    *Rendered = 0;

    assert(Element->bValueIsSet || (bWithDefault && EBML_ElementIsDefaultValue(Element))); // an element is been rendered without a value set !!!
		                 // it may be a mandatory element without a default value

    if (!(Element->bValueIsSet || (bWithDefault && EBML_ElementIsDefaultValue(Element))))
		return ERR_INVALID_DATA;

	if (!bWithDefault && EBML_ElementIsDefaultValue(Element))
		return ERR_INVALID_DATA;

#if !defined(NDEBUG)
    if (EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault))
	    SupposedSize = EBML_ElementUpdateSize(Element, bWithDefault, bForceWithoutMandatory);
    else
        SupposedSize = Element->DataSize;
#else
    if (EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault))
	    EBML_ElementUpdateSize(Element, bWithDefault, bForceWithoutMandatory);
#endif
	Result = EBML_ElementRenderHead(Element, Output, bKeepPosition, &WrittenSize);
    *Rendered += WrittenSize;
    if (Result != ERR_NONE)
        return Result;

    Result = EBML_ElementRenderData(Element, Output, bForceWithoutMandatory, bWithDefault, &WrittenSize);
#if !defined(NDEBUG)
    if (SupposedSize != INVALID_FILEPOS_T) assert(WrittenSize == SupposedSize);
#endif
    *Rendered += WrittenSize;

    return Result;
}

err_t EBML_ElementRenderHead(ebml_element *Element, stream *Output, bool_t bKeepPosition, filepos_t *Rendered)
{
    err_t Err;
	uint8_t FinalHead[4+8]; // Class D + 64 bits coded size
	size_t i,FinalHeadSize;
    int CodedSize;
    filepos_t PosAfter,PosBefore = Stream_Seek(Output,0,SEEK_CUR);
	
	FinalHeadSize = EBML_FillBufferID(FinalHead,sizeof(FinalHead),Element->Context->Id);

	CodedSize = EBML_CodedSizeLength(Element->DataSize, Element->SizeLength, EBML_ElementIsFiniteSize(Element));
	EBML_CodedValueLength(Element->DataSize, CodedSize, &FinalHead[FinalHeadSize], EBML_ElementIsFiniteSize(Element));
	FinalHeadSize += CodedSize;
	
	Err = Stream_Write(Output, FinalHead, FinalHeadSize, &i);
    PosAfter = Stream_Seek(Output,0,SEEK_CUR);
	if (!bKeepPosition) {
		Element->ElementPosition = PosAfter - FinalHeadSize;
		Element->SizePosition = Element->ElementPosition + GetIdLength(Element->Context->Id);
	}
    if (Rendered)
        *Rendered = PosAfter - PosBefore;
	return Err;
}
#endif

void EBML_ElementGetName(const ebml_element *Element, tchar_t *Out, size_t OutLen)
{
    Node_FromStr(Element,Out,OutLen,Element->Context->ElementName);
}

const char *EBML_ElementGetClassName(const ebml_element *Element)
{
    return Element->Context->ElementName;
}