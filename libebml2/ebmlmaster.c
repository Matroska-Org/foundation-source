/*
 * $Id: ebmlmaster.c 1323 2008-10-05 12:07:46Z robux4 $
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

ebml_element *EBML_MasterAddElt(ebml_element *Element, const ebml_context *Context, bool_t SetDefault)
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

ebml_element *EBML_MasterFindFirstElt(ebml_element *Element, const ebml_context *Context, bool_t bCreateIfNull, bool_t SetDefault)
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

ebml_element *EBML_MasterFindNextElt(ebml_element *Element, const ebml_element *Current, bool_t bCreateIfNull, bool_t SetDefault)
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

err_t EBML_MasterAppend(ebml_element *Element, ebml_element *Append)
{
    err_t Result = NodeTree_SetParent(Append,Element,NULL);
    if (Result==ERR_NONE)
        Element->bValueIsSet = 1;
    return Result;
}

size_t EBML_MasterCount(const ebml_element *Element)
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
        return EBML_ElementCmp(a,b);
    return 0;
}

void EBML_MasterSort(ebml_element *Element, arraycmp Cmp, const void* CmpParam)
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

void EBML_MasterClear(ebml_element *Element)
{
    ebml_element *i = EBML_MasterChildren(Element);
    while (i)
    {
        NodeTree_SetParent(i,NULL,NULL);
        i = EBML_MasterChildren(Element);
    }
}

static bool_t IsDefaultValue(const ebml_element *Element)
{
    // TODO: a master element has the default value if all the sub elements are unique and have the default value
    return 0;
}

static bool_t CheckMandatory(const ebml_element *Element)
{
    const ebml_semantic *i;
    for (i=Element->Context->Semantic;i->eClass;++i)
    {
        if (i->Mandatory && !EBML_MasterFindChild(Element,i->eClass))
            return 0;
    }
    return 1;
}

filepos_t UpdateSize(ebml_element *Element, bool_t bWithDefault, bool_t bForceRender)
{
    ebml_element *i;

	Element->Size = 0;

	if (!EBML_ElementIsFiniteSize(Element))
		return INVALID_FILEPOS_T;

	if (!bForceRender) {
		assert(CheckMandatory(Element));
    }

    for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
    {
        if (!bWithDefault && EBML_ElementIsDefaultValue(i))
            continue;
        EBML_ElementUpdateSize(i,bWithDefault,bForceRender);
        if (i->Size == INVALID_FILEPOS_T)
            return INVALID_FILEPOS_T;
        Element->Size += EBML_ElementFullSize(i,bWithDefault);
    }
#ifdef TODO
	if (bChecksumUsed) {
		Element->Size += EBML_ElementFullSize(Element->Checksum,bWithDefault);
	}
#endif
	return Element->Size;
}

static void PostCreate(ebml_element *Element)
{
    const ebml_semantic *i;
    INHERITED(Element,ebml_element_vmt,EBML_MASTER_CLASS)->PostCreate(Element);
    for (i=Element->Context->Semantic;i->eClass;++i)
    {
        if (i->Mandatory && i->Unique)
            EBML_MasterFindFirstElt(Element,i->eClass,1,1); // TODO: should it force the default value ?
    }
}

static err_t ReadData(ebml_element *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
	// remove all existing elements, including the mandatory ones...
    NodeTree_Clear((nodetree*)Element);
    Element->bValueIsSet = 0;

	// read blocks and discard the ones we don't care about
	if (Element->Size > 0) {
	    ebml_element *SubElement;
        ebml_parser_context Context;
        int UpperEltFound = 0;
        filepos_t MaxSizeToRead;

        if (Stream_Seek(Input,EBML_ElementPositionData(Element),SEEK_SET)==INVALID_FILEPOS_T)
            return ERR_END_OF_FILE;

        MaxSizeToRead = Element->Size;
        Context.UpContext = ParserContext;
        Context.Context = Element->Context;
        Context.EndPosition = EBML_ElementPositionEnd(Element);
        SubElement = EBML_FindNextElement(Input,&Context,&UpperEltFound,AllowDummyElt);
		while (SubElement && UpperEltFound<=0 && EBML_ElementPositionEnd(SubElement) <= EBML_ElementPositionEnd(Element))
        {
			if (!AllowDummyElt && EBML_ElementIsDummy(SubElement)) {
                // TODO: this should never happen
                EBML_ElementSkipData(SubElement,Input,&Context,NULL,AllowDummyElt);
				NodeDelete((node*)SubElement); // forget this unknown element
			} else {
                EBML_ElementReadData(SubElement,Input,&Context,AllowDummyElt, Scope);
                EBML_MasterAppend(Element,SubElement);

				// just in case
                EBML_ElementSkipData(SubElement,Input,&Context,NULL,AllowDummyElt);
			}
			MaxSizeToRead = EBML_ElementPositionEnd(Element) - EBML_ElementPositionEnd(SubElement); // even if it's the default value

			if (UpperEltFound > 0) {
				UpperEltFound--;
				if (UpperEltFound > 0 || MaxSizeToRead <= 0)
					goto processCrc;
				continue;
			} 
			
			if (UpperEltFound < 0) {
				UpperEltFound++;
				if (UpperEltFound < 0)
					goto processCrc;
			}

			if (MaxSizeToRead <= 0) {
				goto processCrc;// this level is finished
			}
			
			SubElement = EBML_FindNextElement(Input,&Context,&UpperEltFound,AllowDummyElt);
		}
	}
processCrc:
#ifdef TODO
	for (Index=0; Index<ElementList.size(); Index++) {
		if (ElementList[Index]->Generic().GlobalId == EbmlCrc32::ClassInfos.GlobalId) {
			bChecksumUsed = true;
			// remove the element
			Checksum = *(static_cast<EbmlCrc32*>(ElementList[Index]));
			delete ElementList[Index];
			Remove(Index--);
		}
	}
#endif
    Element->bValueIsSet = 1;
    return ERR_NONE;
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
		assert(CheckMandatory(Element));
	}

#ifdef TODO
	if (!bChecksumUsed) { // old school
#endif
        for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
        {
			if (!bWithDefault && EBML_ElementIsDefaultValue(i))
				continue;
			Err = EBML_ElementRender(i,Output, bWithDefault, 0, bForceRender, &ItemRendered);
            if (Err!=ERR_NONE)
                return Err;
            *Rendered += ItemRendered;
		}
#ifdef TODO
	} else { // new school: render in memory and compute the CRC
		MemIOCallback TmpBuf(Size - 6);
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

META_START(EBMLMaster_Class,EBML_MASTER_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,PostCreate,PostCreate)
META_VMT(TYPE_FUNC,ebml_element_vmt,IsDefaultValue,IsDefaultValue)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateSize)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadData)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderData)
#endif
META_END(EBML_ELEMENT_CLASS)
