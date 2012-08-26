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
#if defined(EBML_LIBRARY)
# if defined(EBML_LEGACY_API)
#  include "ebml2_legacy_project.h"
# else
#  include "ebml2_project.h"
# endif
#endif

extern const nodemeta BufStream_Class[];
extern const nodemeta MemStream_Class[];
extern const nodemeta Streams_Class[];
#if defined(CONFIG_EBML_UNICODE)
extern const nodemeta LangStr_Class[];
extern const nodemeta UrlPart_Class[];
# if defined(CONFIG_STDIO)
extern const nodemeta Stdio_Class[];
# endif
#endif

extern const nodemeta EBMLElement_Class[];
extern const nodemeta EBMLMaster_Class[];
extern const nodemeta EBMLBinary_Class[];
extern const nodemeta EBMLString_Class[];
extern const nodemeta EBMLInteger_Class[];
extern const nodemeta EBMLCRC_Class[];
extern const nodemeta EBMLDate_Class[];
extern const nodemeta EBMLVoid_Class[];

err_t EBML_Init(nodecontext *p)
{
    // TODO: only when used as standalone (no coremake & core-c in the rest of the project)
#if defined(EBML_LIBRARY)
    tchar_t LibName[MAXPATH];
    tcscpy_s(LibName,TSIZEOF(LibName),PROJECT_NAME T(" v") PROJECT_VERSION);
    Node_SetData(p,CONTEXT_LIBEBML_VERSION,TYPE_STRING,LibName);
#endif

    NodeRegisterClassEx((nodemodule*)p,BufStream_Class);
	NodeRegisterClassEx((nodemodule*)p,MemStream_Class);
	NodeRegisterClassEx((nodemodule*)p,Streams_Class);
#if defined(CONFIG_EBML_UNICODE)
	NodeRegisterClassEx((nodemodule*)p,LangStr_Class);
	NodeRegisterClassEx((nodemodule*)p,UrlPart_Class);
# if defined(CONFIG_STDIO)
	NodeRegisterClassEx((nodemodule*)p,Stdio_Class);
# endif
#endif

    NodeRegisterClassEx((nodemodule*)p,EBMLElement_Class);
	NodeRegisterClassEx((nodemodule*)p,EBMLMaster_Class);
	NodeRegisterClassEx((nodemodule*)p,EBMLBinary_Class);
	NodeRegisterClassEx((nodemodule*)p,EBMLString_Class);
	NodeRegisterClassEx((nodemodule*)p,EBMLInteger_Class);
	NodeRegisterClassEx((nodemodule*)p,EBMLDate_Class);
	NodeRegisterClassEx((nodemodule*)p,EBMLCRC_Class);
	NodeRegisterClassEx((nodemodule*)p,EBMLVoid_Class);

    return ERR_NONE;
}

err_t EBML_Done(nodecontext *p)
{
    return ERR_NONE;
}

CONTEXT_CONST ebml_context EBML_ContextDummy = {0xFF, EBML_DUMMY_ID, 0, 0, "DummyElement", NULL, NULL};

CONTEXT_CONST ebml_context EBML_ContextVersion            = {0x4286, EBML_INTEGER_CLASS, 1, EBML_MAX_VERSION, "EBMLVersion", NULL, EBML_SemanticGlobals};
CONTEXT_CONST ebml_context EBML_ContextReadVersion        = {0x42F7, EBML_INTEGER_CLASS, 1, EBML_MAX_VERSION, "EBMLReadVersion", NULL, EBML_SemanticGlobals};
CONTEXT_CONST ebml_context EBML_ContextMaxIdLength        = {0x42F2, EBML_INTEGER_CLASS, 1, EBML_MAX_ID, "EBMLMaxIdLength", NULL, EBML_SemanticGlobals};
CONTEXT_CONST ebml_context EBML_ContextMaxSizeLength      = {0x42F3, EBML_INTEGER_CLASS, 1, EBML_MAX_SIZE, "EBMLMaxSizeLength", NULL, EBML_SemanticGlobals};
CONTEXT_CONST ebml_context EBML_ContextDocType            = {0x4282, EBML_STRING_CLASS,  1, (intptr_t)"matroska", "EBMLDocType", NULL, EBML_SemanticGlobals};
CONTEXT_CONST ebml_context EBML_ContextDocTypeVersion     = {0x4287, EBML_INTEGER_CLASS, 1, 1, "EBMLDocTypeVersion", NULL, EBML_SemanticGlobals};
CONTEXT_CONST ebml_context EBML_ContextDocTypeReadVersion = {0x4285, EBML_INTEGER_CLASS, 1, 1, "EBMLDocTypeReadVersion", NULL, EBML_SemanticGlobals}; 

static const ebml_semantic EBML_SemanticHead[] = {
    {1, 1, &EBML_ContextVersion},
    {1, 1, &EBML_ContextReadVersion},
    {1, 1, &EBML_ContextMaxIdLength},
    {1, 1, &EBML_ContextMaxSizeLength},
    {1, 1, &EBML_ContextDocType},
    {1, 1, &EBML_ContextDocTypeVersion},
    {1, 1, &EBML_ContextDocTypeReadVersion},
    {0, 0, NULL} // end of the table
};
CONTEXT_CONST ebml_context EBML_ContextHead = {0x1A45DFA3, EBML_MASTER_CLASS, 0, 0, "EBMLHead\0mfthis", EBML_SemanticHead, EBML_SemanticGlobals};


CONTEXT_CONST ebml_context EBML_ContextEbmlVoid   = {0xEC, EBML_VOID_CLASS, 0, 0, "EBMLVoid", NULL, NULL};
CONTEXT_CONST ebml_context EBML_ContextEbmlCrc32  = {0xBF, EBML_CRC_CLASS, 0, 0, "EBMLCrc32", NULL, NULL};

const ebml_semantic EBML_SemanticGlobals[] = {
    {0, 0, &EBML_ContextEbmlVoid},
    {0, 1, &EBML_ContextEbmlCrc32},
    {0, 0, NULL} // end of the table
};

static const ebml_context EBML_ContextGlobals = {0, 0, 0, 0, "GlobalContext", EBML_SemanticGlobals, EBML_SemanticGlobals};

filepos_t EBML_ReadCodedSizeValue(const uint8_t *InBuffer, size_t *BufferSize, filepos_t *SizeUnknown)
{
	uint8_t SizeBitMask = 1 << 7;
	filepos_t Result = 0x7F;
	unsigned int SizeIdx, PossibleSizeLength = 0;
	uint8_t PossibleSize[8];
    unsigned int i;

	*SizeUnknown = 0x7F; // the last bit is discarded when computing the size
	for (SizeIdx = 0; SizeIdx < *BufferSize && SizeIdx < 8; SizeIdx++) {
		if (InBuffer[0] & (SizeBitMask >> SizeIdx)) {
			// ID found
			PossibleSizeLength = SizeIdx + 1;
			SizeBitMask >>= SizeIdx;
			for (SizeIdx = 0; SizeIdx < PossibleSizeLength; SizeIdx++) {
				PossibleSize[SizeIdx] = InBuffer[SizeIdx];
			}
			for (SizeIdx = 0; SizeIdx < PossibleSizeLength - 1; SizeIdx++) {
				Result <<= 7;
				Result |= 0xFF;
			}

			Result = 0;
			Result |= PossibleSize[0] & ~SizeBitMask;
			for (i = 1; i<PossibleSizeLength; i++) {
				Result <<= 8;
				Result |= PossibleSize[i];
			}

			*BufferSize = PossibleSizeLength;

			return Result;
		}
		*SizeUnknown <<= 7;
		*SizeUnknown |= 0xFF;
	}

	*BufferSize = 0;
	return 0;
}

filepos_t EBML_ReadCodedSizeSignedValue(const uint8_t *InBuffer, size_t *BufferSize, filepos_t *SizeUnknown)
{
	filepos_t Result = EBML_ReadCodedSizeValue(InBuffer, BufferSize, SizeUnknown);

	if (*BufferSize != 0)
	{
		switch (*BufferSize)
		{
		case 1:
			Result -= 63;
			break;
		case 2:
			Result -= 8191;
			break;
		case 3:
			Result -= 1048575L;
			break;
		case 4:
			Result -= 134217727L;
			break;
		}
	}

	return Result;
}

static fourcc_t EBML_IdFromBuffer(const uint8_t *PossibleId, int8_t IdLength)
{
    if (IdLength == 1)
        return FOURCCBE(0,0,0,PossibleId[0]);
    if (IdLength == 2)
        return FOURCCBE(0,0,PossibleId[0],PossibleId[1]);
    if (IdLength == 3)
        return FOURCCBE(0,PossibleId[0],PossibleId[1],PossibleId[2]);
    return FOURCCBE(PossibleId[0],PossibleId[1],PossibleId[2],PossibleId[3]);
}

static bool_t EBML_IdMatch(const uint8_t *PossibleId, int8_t IdLength, fourcc_t ContextId)
{
    return ContextId == EBML_IdFromBuffer(PossibleId,IdLength);
}

ebml_element *EBML_ElementCreate(anynode *Any, const ebml_context *Context, bool_t SetDefault, const void *Cookie)
{
    ebml_element *Result;
    Result = (ebml_element*)NodeCreate(Any,Context->Class);
    if (Result!=NULL)
    {
        Result->Context = Context;
#if defined(EBML_LEGACY_API)
        assert(Context->PostCreate); // for projects with legacy access
#endif
        if (Context->PostCreate)
            Context->PostCreate(Result,Cookie);
        VMT_FUNC(Result,ebml_element_vmt)->PostCreate(Result, SetDefault);
    }
    return Result;
}

static ebml_element *CreateElement(anynode *Any, const uint8_t *PossibleId, int8_t IdLength, const ebml_context *Context, ebml_master *Parent)
{
    ebml_element *Result;
    assert(Context!=NULL);
    if (EBML_IdMatch(PossibleId, IdLength, Context->Id))
    {
        Result = EBML_ElementCreate(Any,Context,0,NULL);
    }
    else
    {
        Result = EBML_ElementCreate(Any,&EBML_ContextDummy,0,NULL);
        if (Result!=NULL)
        {
            // Fill a temp context
            ebml_dummy *Dummy = (ebml_dummy*)Result;
            memcpy(&Dummy->DummyContext,&EBML_ContextDummy,sizeof(Dummy->DummyContext));
            Dummy->DummyContext.Id = EBML_IdFromBuffer(PossibleId,IdLength);
            Result->Context = &Dummy->DummyContext;
        }
    }
    if (Result && Parent)
    {
        assert(Node_IsPartOf(Parent,EBML_MASTER_CLASS));
        EBML_MasterAppend(Parent,Result);
    }
    return Result;
}

static ebml_element *EBML_ElementCreateUsingContext(void *AnyNode, const uint8_t *PossibleId, int8_t IdLength, const ebml_parser_context *Context,
                                                    int *LowLevel, bool_t IsGlobalContext, bool_t bAllowDummy)
{
    int MaxLowerLevel=1; //TODO: remove ?
//	unsigned int ContextIndex;
	ebml_element *Result = NULL;
    const ebml_semantic *Semantic;

    if (!Context || !Context->Context || !Context->Context->Semantic)
        return NULL;

	// elements at the current level
    for (Semantic=Context->Context->Semantic;Semantic->eClass;Semantic++)
    {
        if (EBML_IdMatch(PossibleId, IdLength, Semantic->eClass->Id)) // && (bAllowDummy || bAllowOutOfProfile || !(Context->Profile & Semantic->DisabledProfile)))
        {
            Result = EBML_ElementCreate(AnyNode,Semantic->eClass,0,NULL);
			return Result;
		}
	}

	// global elements
	assert(Context->Context->GlobalContext != NULL); // global should always exist, at least the EBML ones
	if (Context->Context->GlobalContext == Context->Context->Semantic)
		return NULL;
    else
    {
        ebml_context ContextGlobals;
        ebml_parser_context GlobalContext;

        ContextGlobals.Semantic = Context->Context->GlobalContext;
        ContextGlobals.GlobalContext = Context->Context->GlobalContext;

        GlobalContext.Context = &ContextGlobals;
        GlobalContext.UpContext = Context;
        GlobalContext.EndPosition = INVALID_FILEPOS_T;
        GlobalContext.Profile = Context->Profile;
		(*LowLevel)--;
		MaxLowerLevel--;
		// recursive is good, but be carefull...
        Result = EBML_ElementCreateUsingContext(AnyNode,PossibleId,IdLength,&GlobalContext,LowLevel,1,bAllowDummy);
        if (Result)
            return Result;
		(*LowLevel)++;
		MaxLowerLevel++;
	}

#ifdef TODO
	// parent elements
	if (Context.MasterElt != NULL && aID == Context.MasterElt->GlobalId) {
		(*LowLevel)++; // already one level up (same as context)
		return &Context.MasterElt->Create();
	}
#endif

	// check wether it's not part of an upper context
	if (Context->UpContext != NULL) {
		(*LowLevel)++;
		MaxLowerLevel++;
		return EBML_ElementCreateUsingContext(AnyNode, PossibleId, IdLength, Context->UpContext, LowLevel, IsGlobalContext, bAllowDummy);
	}

    // dummy fallback
	if (!IsGlobalContext && bAllowDummy && IdLength!=0) {
		(*LowLevel) = 0;
        Result = CreateElement(AnyNode,PossibleId,IdLength,Context->Context,NULL);
	}

	return Result;
}

ebml_element *EBML_FindNextId(stream *Input, const ebml_context *Context, size_t MaxDataSize)
{
    filepos_t aElementPosition, aSizePosition;
    filepos_t SizeFound=0, SizeUnknown;
    int ReadSize;
    uint8_t BitMask;
    uint8_t PossibleId[4];
    uint8_t PossibleSize[8]; // we don't support size stored in more than 64 bits
    bool_t bElementFound = 0;
    int8_t PossibleID_Length = 0;
    size_t _SizeLength;
    uint8_t PossibleSizeLength = 0;
    ebml_element *Result = NULL;

    while (!bElementFound)
    {
        aElementPosition = Stream_Seek(Input,0,SEEK_CUR);
        ReadSize = 0;
        BitMask = 1 << 7;
        for (;;)
        {
            if (Stream_ReadOneOrMore(Input,&PossibleId[PossibleID_Length], 1, NULL)!=ERR_NONE)
                break;
            ReadSize++;
            if (ReadSize == PossibleID_Length)
                return NULL; // No more data ?
			if (++PossibleID_Length > 4)
				return NULL; // we don't support element IDs over class D
            if (PossibleId[0] & BitMask)
            {
                bElementFound = 1;
                break;
            }
            BitMask >>= 1;
        }

	    // read the data size
	    aSizePosition = Stream_Seek(Input,0,SEEK_CUR);
	    do {
		    if (PossibleSizeLength >= 8)
			    // Size is larger than 8 bytes
			    return NULL;

            if (Stream_ReadOneOrMore(Input,&PossibleSize[PossibleSizeLength++], 1, NULL)!=ERR_NONE)
                break;
		    ReadSize++;
		    _SizeLength = PossibleSizeLength;
		    SizeFound = EBML_ReadCodedSizeValue(&PossibleSize[0], &_SizeLength, &SizeUnknown);
	    } while (_SizeLength == 0);
    }

    // look for the ID in the provided context
    Result = CreateElement(Input, PossibleId, PossibleID_Length, Context,NULL);
    assert(Result != NULL);
#if 0
if (PossibleID_Length==4)
printf("Elt: size %d id %d %02X%02X%02X%02X\n",SizeFound,PossibleID_Length,PossibleId[0],PossibleId[1],PossibleId[2],PossibleId[3]);
else if (PossibleID_Length==3)
printf("Elt: size %d id %d %02X%02X%02X\n",SizeFound,PossibleID_Length,PossibleId[0],PossibleId[1],PossibleId[2]);
else if (PossibleID_Length==2)
printf("Elt: size %d id %d %02X%02X\n",SizeFound,PossibleID_Length,PossibleId[0],PossibleId[1]);
else if (PossibleID_Length==1)
printf("Elt: size %d id %d %02X\n",SizeFound,PossibleID_Length,PossibleId[0]);
#endif
	Result->SizeLength = PossibleSizeLength;
	Result->DataSize = SizeFound;
    if (!EBML_ElementValidateSize(Result) || (SizeFound != SizeUnknown && MaxDataSize < (size_t)Result->DataSize))
    {
        NodeDelete((node*)Result);
        return NULL;
    }
    Result->ElementPosition = aElementPosition;
    Result->SizePosition = aSizePosition;

    return Result;
}

uint8_t EBML_CodedSizeLength(filepos_t Length, uint8_t SizeLength, bool_t bSizeIsFinite)
{
	int CodedSize;
	if (!bSizeIsFinite)
		CodedSize = 1;
	else
    {
		// prepare the head of the size (000...01xxxxxx)
		// optimal size
		if (Length < 127) // 2^7 - 1
			CodedSize = 1;
		else if (Length < 16383) // 2^14 - 1
			CodedSize = 2;
		else if (Length < 2097151) // 2^21 - 1
			CodedSize = 3;
		else if (Length < 268435455) // 2^28 - 1
			CodedSize = 4;
		else CodedSize = 5;
	}

	if (SizeLength && CodedSize < SizeLength)
		// defined size
		CodedSize = SizeLength;

	return (uint8_t)CodedSize;
}

uint8_t EBML_CodedSizeLengthSigned(filepos_t Length, uint8_t SizeLength)
{
	int CodedSize;
	// prepare the head of the size (000...01xxxxxx)
	// optimal size
	if (Length > -64 && Length < 64) // 2^6
		CodedSize = 1;
	else if (Length > -8192 && Length < 8192) // 2^13
		CodedSize = 2;
	else if (Length > -1048576 && Length < 1048576) // 2^20
		CodedSize = 3;
	else if (Length > -134217728 && Length < 134217728) // 2^27
		CodedSize = 4;
	else CodedSize = 5;

	if (SizeLength > 0 && CodedSize < SizeLength)
		// defined size
		CodedSize = SizeLength;

	return (uint8_t)CodedSize;
}


uint8_t EBML_CodedValueLength(filepos_t Length, size_t CodedSize, uint8_t *OutBuffer, bool_t bSizeIsFinite)
{
	int _SizeMask = 0xFF;
    size_t i;
#if 0
if (CodedSize==3)
printf("%08X ",(int)Length);
#endif
    if (!bSizeIsFinite)
        Length=MAX_FILEPOS;
	OutBuffer[0] = (uint8_t)(1 << (8 - CodedSize));
	for (i=1; i<CodedSize; ++i)
    {
		OutBuffer[CodedSize-i] = (uint8_t)(Length & 0xFF);
		Length >>= 8;
		_SizeMask >>= 1;
	}
	// first one use a OR with the "EBML size head"
	OutBuffer[0] |= Length & 0xFF & _SizeMask;
#if 0
if (CodedSize==3)
printf("%02X%02X%02X\n",OutBuffer[0],OutBuffer[1],OutBuffer[2]);
#endif
	return (uint8_t)CodedSize;
}

uint8_t EBML_CodedValueLengthSigned(filepos_t Length, size_t CodedSize, uint8_t * OutBuffer)
{
	if (Length > -64 && Length < 64) // 2^6
		Length += 63;
	else if (Length > -8192 && Length < 8192) // 2^13
		Length += 8191;
	else if (Length > -1048576 && Length < 1048576) // 2^20
		Length += 1048575;
	else if (Length > -134217728 && Length < 134217728) // 2^27
		Length += 134217727;

	return EBML_CodedValueLength(Length, CodedSize, OutBuffer, 1);
}


ebml_element *EBML_FindNextElement(stream *Input, const ebml_parser_context *pContext, int *UpperLevels, bool_t AllowDummyElt)
{
	uint8_t PossibleID_Length = 0;
	uint8_t PossibleIdNSize[16];
	int PossibleSizeLength;
	filepos_t SizeUnknown;
	int8_t SizeIdx,ReadIndex = 0; // trick for the algo, start index at 0
	uint32_t ReadSize = 0;
	filepos_t SizeFound;
	bool_t bFound;
	int UpperLevel_original = *UpperLevels;
    filepos_t CurrentPos;
    filepos_t StartPos = Stream_Seek(Input,0,SEEK_CUR);
	ebml_parser_context OrigContext;
	const ebml_parser_context *Context = &OrigContext;

	if (StartPos == INVALID_FILEPOS_T)
		return NULL;

    assert(Context != NULL);
	OrigContext = *pContext;
	
    // adjust the Context to allow the StartPos to make sense
    while (Context && Context->EndPosition != INVALID_FILEPOS_T && (StartPos >= Context->EndPosition))
    {
        if (Context->UpContext==NULL)
            break;
        Context = Context->UpContext;
        (*UpperLevels)++;
    }

    do {
		size_t _SizeLength;

		// read a potential ID
		do {
			uint8_t IdBitMask = 1 << 7;

            assert(ReadIndex < 16);
			// build the ID with the current Read Buffer
			bFound = 0;
			for (SizeIdx = 0; SizeIdx < ReadIndex && SizeIdx < 4; SizeIdx++) {
				if (PossibleIdNSize[0] & (IdBitMask >> SizeIdx)) {
					// ID found
					PossibleID_Length = SizeIdx + 1;
					IdBitMask >>= SizeIdx;
					bFound = 1;
					break;
				}
			}
			if (bFound)
				break;
            if (Context->EndPosition == StartPos+ReadSize)
                break; // we should not read further than our limit

			if (ReadIndex >= 4) {
				// ID not found
				// shift left the read octets
				memmove(&PossibleIdNSize[0],&PossibleIdNSize[1], --ReadIndex);
			}

            if (Stream_ReadOneOrMore(Input,&PossibleIdNSize[ReadIndex++], 1, NULL)!=ERR_NONE)
				return NULL; // no more data ?
			ReadSize++;

        } while (!bFound);

		SizeIdx = ReadIndex;
		ReadIndex = ReadIndex - PossibleID_Length;

		// read the data size
		PossibleSizeLength = ReadIndex;
        bFound = 0;
		while (1)
		{
			_SizeLength = PossibleSizeLength;
			SizeFound = EBML_ReadCodedSizeValue(&PossibleIdNSize[PossibleID_Length], &_SizeLength, &SizeUnknown);
			if (_SizeLength != 0) {
				bFound = 1;
				break;
			}
			if (PossibleSizeLength >= 8)
				break;
            if (Context->EndPosition == StartPos+ReadSize)
                break; // we should not read further than our limit
            if (Stream_ReadOneOrMore(Input,&PossibleIdNSize[SizeIdx++], 1, NULL)!=ERR_NONE)
                return NULL;
			ReadSize++;
			PossibleSizeLength++;
		}

        CurrentPos = Stream_Seek(Input,0,SEEK_CUR);
		if (bFound)
        {
            // make sure the element we found is contained in the Context
            if (SizeFound != SizeUnknown)
            {
                while (Context && Context->EndPosition != INVALID_FILEPOS_T && (CurrentPos + SizeFound - SizeIdx > Context->EndPosition))
                {
                    if (AllowDummyElt || Context->UpContext==NULL)
                    {
                        bFound = 0;
                        break;
                    }
                    Context = Context->UpContext;
                    (*UpperLevels)++;
                }
            }
        }

        if (bFound)
        {
			// find the element in the context and use the correct creator
            int LevelChange = 0;
			ebml_element *Result = EBML_ElementCreateUsingContext(Input, PossibleIdNSize, PossibleID_Length, Context, &LevelChange, 0, AllowDummyElt);
			if (Result != NULL)
            {
				if (AllowDummyElt || !EBML_ElementIsDummy(Result)) {
                    assert(_SizeLength <= 8);
                    Result->SizeLength = (int8_t)_SizeLength;
					Result->DataSize = SizeFound;

					if (AllowDummyElt && !EBML_ElementValidateSize(Result) && !EBML_ElementIsDummy(Result))
					{
						// the element has a good ID but wrong size, so replace with a dummy
						NodeDelete((node*)Result);
						Result = CreateElement(Input, PossibleIdNSize, PossibleID_Length, &EBML_ContextDummy, NULL);
						Result->SizeLength = (int8_t)_SizeLength;
						Result->DataSize = SizeFound;
					}

					// LevelChange values
					// -1 : global element
					//  0 : child
					//  1 : same level
					//  + : further parent
					if (EBML_ElementValidateSize(Result))
                    {
						if (SizeFound == SizeUnknown)
                        {
                            EBML_ElementSetInfiniteSize(Result, 1);
                            Result->DataSize = INVALID_FILEPOS_T;
                        }

                        if (LevelChange > 0)
                            *UpperLevels += LevelChange;
						Result->SizePosition = CurrentPos - SizeIdx + PossibleID_Length;
						Result->ElementPosition = Result->SizePosition - PossibleID_Length;
						// place the file at the beggining of the data
						Stream_Seek(Input,Result->SizePosition + _SizeLength,SEEK_SET);
						return Result;
					}
				}
				NodeDelete((node*)Result);
			}
		}

        if (Context->EndPosition!=INVALID_FILEPOS_T && Context->EndPosition <= CurrentPos)
        {
            if (AllowDummyElt)
            {
                int LevelChange = 0;
			    ebml_element *Result = EBML_ElementCreateUsingContext(Input, PossibleIdNSize, PossibleID_Length, Context, &LevelChange, 0, 1);
			    if (Result != NULL)
                {
                    if (LevelChange > 0)
                        *UpperLevels += LevelChange;
				    Result->SizePosition = CurrentPos - SizeIdx + PossibleID_Length;
				    Result->ElementPosition = Result->SizePosition - PossibleID_Length;
				    Result->DataSize = 0;
                    Result->SizeLength = (int8_t)(Context->EndPosition - Result->SizePosition);
				    // place the file at the end of the element
				    Stream_Seek(Input,Context->EndPosition,SEEK_SET);
				    return Result;
			    }
            }
            break; // we should not read further than our limit
        }

		// recover all the data in the buffer minus one byte
		ReadIndex = SizeIdx - 1;
		memmove(&PossibleIdNSize[0], &PossibleIdNSize[1], ReadIndex);
		*UpperLevels = UpperLevel_original;
		OrigContext = *pContext;
		Context = &OrigContext;
    } while (Context->EndPosition==INVALID_FILEPOS_T || (Context->EndPosition > CurrentPos - SizeIdx + PossibleID_Length));

	return NULL;
}
