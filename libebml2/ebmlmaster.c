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
#include "ebmlcrc.h"

ebml_element *EBML_MasterAddElt(ebml_master *Element, const ebml_context *Context, bool_t SetDefault)
{
    ebml_element *i;
    i = EBML_ElementCreate(Element,Context,SetDefault,NULL);
    if (i)
        if (EBML_MasterAppend(Element,i)!=ERR_NONE)
        {
            NodeDelete((node*)i);
            i = NULL;
        }
    return i;
}

ebml_element *EBML_MasterFindFirstElt(ebml_master *Element, const ebml_context *Context, bool_t bCreateIfNull, bool_t SetDefault)
{
    ebml_element *i;
    for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
    {
        if (i->Context->Id == Context->Id)
            break;
    }

    if (!i && bCreateIfNull)
        i = EBML_MasterAddElt(Element,Context,SetDefault);

    return i;
}

ebml_element *EBML_MasterFindNextElt(ebml_master *Element, const ebml_element *Current, bool_t bCreateIfNull, bool_t SetDefault)
{
    ebml_element *i;
    if (!Current)
        return NULL;

    for (i=EBML_MasterNext(Current);i;i=EBML_MasterNext(i))
    {
        if (i->Context->Id == Current->Context->Id)
            break;
    }

    if (!i && bCreateIfNull)
        i = EBML_MasterAddElt(Element,Current->Context,SetDefault);

    return i;
}

err_t EBML_MasterAppend(ebml_master *Element, ebml_element *Append)
{
    err_t Result;
	assert(Node_IsPartOf(Element,EBML_MASTER_CLASS));
    Result = NodeTree_SetParent(Append,Element,NULL);
    if (Result==ERR_NONE)
        Element->Base.bValueIsSet = 1;
    return Result;
}

err_t EBML_MasterRemove(ebml_master *Element, ebml_element *Append)
{
    err_t Result = NodeTree_SetParent(Append,NULL,NULL);
    return Result;
}

size_t EBML_MasterCount(const ebml_master *Element)
{
    size_t Result = 0;
    ebml_element *i;
    for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
        ++Result;
    return Result;
}

static int EbmlCmp(const ebml_element* Element, const ebml_element** a,const ebml_element** b)
{
    if ((*a)->Context->Id == (*b)->Context->Id)
        return EBML_ElementCmp(*a,*b);
    return 0;
}

void EBML_MasterSort(ebml_master *Element, arraycmp Cmp, const void* CmpParam)
{
    array Elts;
    ebml_element *i,**j;
    ArrayInit(&Elts);
    for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
        ArrayAppend(&Elts,&i,sizeof(i),64);
    if (Cmp)
        ArraySort(&Elts,ebml_element*,Cmp,CmpParam,0);
    else
        ArraySort(&Elts,ebml_element*,EbmlCmp,Element,0);

    // refill the master with the new order
    EBML_MasterClear(Element);
    i = NULL;
    for (j=ARRAYBEGIN(Elts,ebml_element*);j!=ARRAYEND(Elts,ebml_element*);++j)
    {
        NodeTree_SetParent(*j,Element,NULL);
        i = *j;
    }
    ArrayClear(&Elts);
}

void EBML_MasterClear(ebml_master *Element)
{
    ebml_element *i = EBML_MasterChildren(Element);
    while (i)
    {
        NodeTree_SetParent(i,NULL,NULL);
        i = EBML_MasterChildren(Element);
    }
}

void EBML_MasterErase(ebml_master *Element)
{
	while (Element->Base.Base.Children)
    	NodeTree_DetachAndRelease(Element->Base.Base.Children);
}

static bool_t IsDefaultValue(const ebml_element *Element)
{
    // TODO: a master element has the default value if all the sub elements are unique and have the default value
    return 0;
}

static bool_t CheckMandatory(const ebml_master *Element, bool_t bWithDefault)
{
    const ebml_semantic *i;
    for (i=Element->Base.Context->Semantic;i->eClass;++i)
    {
        if (i->Mandatory && !EBML_MasterFindChild(Element,i->eClass) && (bWithDefault || !i->eClass->HasDefault))
            return 0;
    }
    return 1;
}

bool_t EBML_MasterCheckMandatory(const ebml_master *Element, bool_t bWithDefault)
{
	ebml_element *Child;
	if (!CheckMandatory(Element, bWithDefault))
		return 0;

	for (Child = EBML_MasterChildren(Element); Child; Child = EBML_MasterNext(Child))
	{
		if (Node_IsPartOf(Child,EBML_MASTER_CLASS) && !EBML_MasterCheckMandatory((ebml_master*)Child, bWithDefault))
			return 0;
	}
    return 1;
}

static filepos_t UpdateSize(ebml_element *Element, bool_t bWithDefault, bool_t bForceRender)
{
    ebml_element *i;

	Element->DataSize = 0;

	if (!EBML_ElementIsFiniteSize(Element))
		return INVALID_FILEPOS_T;

	if (!bForceRender) {
		assert(CheckMandatory((ebml_master*)Element, bWithDefault));
    }

    for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
    {
        if (!bWithDefault && EBML_ElementIsDefaultValue(i))
            continue;
        EBML_ElementUpdateSize(i,bWithDefault,bForceRender);
        if (i->DataSize == INVALID_FILEPOS_T)
            return INVALID_FILEPOS_T;
        Element->DataSize += EBML_ElementFullSize(i,bWithDefault);
    }
#ifdef TODO
	if (bChecksumUsed) {
		Element->DataSize += EBML_ElementFullSize(Element->Checksum,bWithDefault);
	}
#endif
	return Element->DataSize;
}

void EBML_MasterMandatory(ebml_master *Element, bool_t SetDefault)
{
    const ebml_semantic *i;
    for (i=Element->Base.Context->Semantic;i->eClass;++i)
    {
        if (i->Mandatory && i->Unique)
            EBML_MasterFindFirstElt(Element,i->eClass,1,SetDefault);
    }
}

static void PostCreate(ebml_master *Element)
{
    INHERITED(Element,ebml_element_vmt,EBML_MASTER_CLASS)->PostCreate(Element);
	EBML_MasterMandatory(Element,1); // TODO: should it force the default value ?
}

static err_t ReadData(ebml_master *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    int UpperEltFound = 0;
    bool_t bFirst = 1;
    ebml_element *SubElement, *CRCElement = NULL;
    stream *ReadStream = Input;
    array CrcBuffer;

    // remove all existing elements, including the mandatory ones...
    NodeTree_Clear((nodetree*)Element);
    Element->Base.bValueIsSet = 0;

	// read blocks and discard the ones we don't care about
	if (Element->Base.DataSize > 0 || !EBML_ElementIsFiniteSize((ebml_element*)Element)) {
        ebml_parser_context Context;
        filepos_t MaxSizeToRead;

        if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
            return ERR_END_OF_FILE;

        MaxSizeToRead = Element->Base.DataSize;
        Context.UpContext = ParserContext;
        Context.Context = Element->Base.Context;
        Context.EndPosition = EBML_ElementPositionEnd((ebml_element*)Element);
        SubElement = EBML_FindNextElement(Input,&Context,&UpperEltFound,AllowDummyElt);
		while (SubElement && UpperEltFound<=0 && (!EBML_ElementIsFiniteSize((ebml_element*)Element) || EBML_ElementPositionEnd(SubElement) <= EBML_ElementPositionEnd((ebml_element*)Element)))
        {
			if (!AllowDummyElt && EBML_ElementIsDummy(SubElement)) {
                // TODO: this should never happen
                EBML_ElementSkipData(SubElement,ReadStream,&Context,NULL,AllowDummyElt);
				NodeDelete((node*)SubElement); // forget this unknown element
			}
            else
            {
                if (EBML_ElementReadData(SubElement,ReadStream,&Context,AllowDummyElt, Scope, DepthCheckCRC?DepthCheckCRC-1:0)==ERR_NONE)
                {
                    if (bFirst && DepthCheckCRC && Scope!=SCOPE_NO_DATA && SubElement->Context->Id==EBML_ContextEbmlCrc32.Id && CRCElement==NULL)
                    {
                        if (EBML_ElementIsFiniteSize((ebml_element*)Element))
                        {
                            // read the rest of the element in memory to avoid reading it a second time later
                            ArrayInit(&CrcBuffer);
                            if (ArrayResize(&CrcBuffer, (size_t)(EBML_ElementPositionEnd((ebml_element*)Element) - EBML_ElementPositionEnd(SubElement)), 0))
                            {
                                ReadStream = (stream*)NodeCreate(Element, MEMSTREAM_CLASS);
                                if (ReadStream==NULL)
                                {
                                    ReadStream=Input; // revert back to normal reading
                                    ArrayClear(&CrcBuffer);
                                }
                                else
                                {
                                    filepos_t Offset = EBML_ElementPositionEnd(SubElement);
                                    Node_Set(ReadStream, MEMSTREAM_DATA, ARRAYBEGIN(CrcBuffer,uint8_t), ARRAYCOUNT(CrcBuffer,uint8_t));
                                    Node_SET(ReadStream, MEMSTREAM_OFFSET, &Offset);
                                    Stream_Seek(Input,EBML_ElementPositionEnd(SubElement),SEEK_SET);
                                    if (Stream_Read(Input, ARRAYBEGIN(CrcBuffer,uint8_t), ARRAYCOUNT(CrcBuffer,uint8_t), NULL)!=ERR_NONE)
                                    {
                                        ReadStream=Input; // revert back to normal reading
                                        ArrayClear(&CrcBuffer);
                                    }
                                }
                            }
                            CRCElement = SubElement;
                        }
                        bFirst = 0;
                    }
                    if (CRCElement != SubElement)
                        EBML_MasterAppend(Element,SubElement);
			        // just in case
                    EBML_ElementSkipData(SubElement,ReadStream,&Context,NULL,AllowDummyElt);
                }
                else
                {
                    NodeDelete((node*)SubElement);
                    SubElement = NULL;
                }
			}
            if (SubElement)
			    MaxSizeToRead = EBML_ElementPositionEnd((ebml_element*)Element) - EBML_ElementPositionEnd(SubElement); // even if it's the default value

			if (UpperEltFound > 0) {
				UpperEltFound--;
				if (UpperEltFound > 0 || (EBML_ElementIsFiniteSize((ebml_element*)Element) && MaxSizeToRead <= 0))
					goto processCrc;
				continue;
			} 
			
			if (UpperEltFound < 0) {
				UpperEltFound++;
				if (UpperEltFound < 0)
					goto processCrc;
			}

			if (EBML_ElementIsFiniteSize((ebml_element*)Element) && MaxSizeToRead <= 0) {
				goto processCrc;// this level is finished
			}
			
			SubElement = EBML_FindNextElement(ReadStream,&Context,&UpperEltFound,AllowDummyElt);
		}
	}
processCrc:
    if (ReadStream!=Input)
    {
        Element->CheckSumStatus = EBML_CRCMatches(CRCElement, ARRAYBEGIN(CrcBuffer,uint8_t), ARRAYCOUNT(CrcBuffer,uint8_t))?2:1;
        StreamClose(ReadStream);
        ArrayClear(&CrcBuffer);
    }

    Element->Base.bValueIsSet = 1;
    if (UpperEltFound>0) // move back to the upper element beginning so that the next loop can find it
    {
        assert(SubElement!=NULL);
        Stream_Seek(Input,SubElement->ElementPosition,SEEK_SET);
    }
    return ERR_NONE;
}

void EBML_MasterUseChecksum(ebml_master *Element, bool_t Use)
{
    Element->CheckSumStatus = 1;
}

bool_t EBML_MasterIsChecksumValid(const ebml_master *Element)
{
    return (Element->CheckSumStatus!=1);
}

#if defined(CONFIG_EBML_WRITING)
static err_t RenderData(ebml_element *Element, stream *Output, bool_t bForceRender, bool_t bWithDefault, filepos_t *Rendered)
{
    ebml_element *i;
    filepos_t _Rendered;
    filepos_t ItemRendered;
    err_t Err = ERR_NONE;

    if (!Rendered)
        Rendered = &_Rendered;
    *Rendered = 0;

	if (!bForceRender) {
		assert(CheckMandatory((ebml_master*)Element, bWithDefault));
	}

#ifdef TODO
	if (!Element->bChecksumUsed) { // old school
#endif
        for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
        {
			if (!bWithDefault && EBML_ElementIsDefaultValue(i))
				continue;
			Err = EBML_ElementRender(i,Output, bWithDefault, 0, bForceRender, &ItemRendered,0);
            if (Err!=ERR_NONE)
                return Err;
            *Rendered += ItemRendered;
		}
#ifdef TODO
	} else { // new school: render in memory and compute the CRC
		MemIOCallback TmpBuf(DataSize - 6);
		for (Index = 0; Index < ElementList.size(); Index++) {
			if (!bWithDefault && (ElementList[Index])->IsDefaultValue())
				continue;
			(ElementList[Index])->Render(TmpBuf, bWithDefault, false ,bForceRender);
		}
		Checksum.FillCRC32(TmpBuf.GetDataBuffer(), TmpBuf.GetDataBufferSize());
		Result += Checksum.Render(output, true, false ,bForceRender);
		output.writeFully(TmpBuf.GetDataBuffer(), TmpBuf.GetDataBufferSize());
		Result += TmpBuf.GetDataBufferSize();
	}
#endif

	return Err;
}
#endif

static ebml_element *Copy(const ebml_master *Element, const void *Cookie)
{
    ebml_element *i, *Elt;
    ebml_master *Result = (ebml_master*)EBML_ElementCreate(Element,Element->Base.Context,0,Cookie);
    if (Result)
    {
        EBML_MasterErase(Result); // delete the children elements created by default
        Result->Base.bValueIsSet = Element->Base.bValueIsSet;
        Result->Base.bDefaultIsSet = Element->Base.bDefaultIsSet;
        Result->Base.DataSize = Element->Base.DataSize;
        Result->Base.ElementPosition = Element->Base.ElementPosition;
        Result->Base.SizeLength = Element->Base.SizeLength;
        Result->Base.SizePosition = Element->Base.SizePosition;
        Result->CheckSumStatus = Element->CheckSumStatus;
        for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
        {
            Elt = EBML_ElementCopy(i,Cookie);
            if (!Elt || EBML_MasterAppend(Result, Elt)!=ERR_NONE)
            {
                NodeDelete((node*)Result);
                Result = NULL;
                break;
            }
        }
    }
    return (ebml_element*)Result;
}

META_START(EBMLMaster_Class,EBML_MASTER_CLASS)
META_CLASS(SIZE,sizeof(ebml_master))
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreate)
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValue)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,Copy)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderData)
#endif
META_END(EBML_ELEMENT_CLASS)
