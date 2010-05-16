/*
 * $Id$
 * Copyright (c) 2010, Matroska Foundation
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
#include "mkvalidator_stdafx.h"
#include "mkvalidator_project.h"
#ifndef CONFIG_EBML_UNICODE
#define CONFIG_EBML_UNICODE
#endif
#include "matroska/matroska.h"

/*!
 * \todo handle segments with an infinite size
 * \todo warn when a top level element is not present in the main SeekHead
 * \todo optionally warn when a Cluster's first video track is not a keyframe
 * \todo optionally show the use of deprecated elements
 */

static textwriter *StdErr = NULL;
static ebml_element *RSegmentInfo = NULL, *RTrackInfo = NULL, *RChapters = NULL, *RTags = NULL, *RCues = NULL, *RAttachments = NULL, *RSeekHead = NULL, *RSeekHead2 = NULL;
static array RClusters;

#ifdef TARGET_WIN
#include <windows.h>
void DebugMessage(const tchar_t* Msg,...)
{
#if !defined(NDEBUG) || defined(LOGFILE) || defined(LOGTIME)
	va_list Args;
	tchar_t Buffer[1024],*s=Buffer;

	va_start(Args,Msg);
	vstprintf_s(Buffer,TSIZEOF(Buffer), Msg, Args);
	va_end(Args);
	tcscat_s(Buffer,TSIZEOF(Buffer),T("\r\n"));
#endif

#ifdef LOGTIME
    {
        tchar_t timed[1024];
        SysTickToString(timed,TSIZEOF(timed),GetTimeTick(),1,1,0);
        stcatprintf_s(timed,TSIZEOF(timed),T(" %s"),s);
        s = timed;
    }
#endif

#if !defined(NDEBUG)
	OutputDebugString(s);
#endif

#if defined(LOGFILE)
{
    static FILE* f=NULL;
    static char s8[1024];
    size_t i;
    if (!f)
#if defined(TARGET_WINCE)
    {
        tchar_t DocPath[MAXPATH];
        char LogPath[MAXPATH];
        charconv *ToStr = CharConvOpen(NULL,CHARSET_DEFAULT);
        GetDocumentPath(NULL,DocPath,TSIZEOF(DocPath),FTYPE_LOG); // more visible via ActiveSync
        if (!DocPath[0])
            tcscpy_s(DocPath,TSIZEOF(DocPath),T("\\My Documents"));
        if (!PathIsFolder(NULL,DocPath))
            FolderCreate(NULL,DocPath);
        tcscat_s(DocPath,TSIZEOF(DocPath),T("\\corelog.txt"));
        CharConvST(ToStr,LogPath,sizeof(LogPath),DocPath);
        CharConvClose(ToStr);
        f=fopen(LogPath,"a+b");
        if (!f)
            f=fopen("\\corelog.txt","a+b");
    }
#else
        f=fopen("\\corelog.txt","a+b");
#endif
    for (i=0;s[i];++i)
        s8[i]=(char)s[i];
    s8[i]=0;
    fputs(s8,f);
    fflush(f);
}
#endif
}
#endif

static const tchar_t *GetProfileName(size_t ProfileNum)
{
static const tchar_t *Profile[5] = {T("unknown"), T("v1"), T("v2"), T("testv1"), T("testv2") };
	switch (ProfileNum)
	{
	case PROFILE_MATROSKA_V1: return Profile[1];
	case PROFILE_MATROSKA_V2: return Profile[2];
	case PROFILE_TEST_V1:     return Profile[3];
	case PROFILE_TEST_V2:     return Profile[4];
	default:                  return Profile[0];
	}
}

static int OutputError(int ErrCode, const tchar_t *ErrString, ...)
{
	tchar_t Buffer[MAXLINE];
	va_list Args;
	va_start(Args,ErrString);
	vstprintf_s(Buffer,TSIZEOF(Buffer), ErrString, Args);
	va_end(Args);
	TextPrintf(StdErr,T("\rERR%03X: %s\r\n"),ErrCode,Buffer);
	return -ErrCode;
}

static int OutputWarning(int ErrCode, const tchar_t *ErrString, ...)
{
	tchar_t Buffer[MAXLINE];
	va_list Args;
	va_start(Args,ErrString);
	vstprintf_s(Buffer,TSIZEOF(Buffer), ErrString, Args);
	va_end(Args);
	TextPrintf(StdErr,T("\rWRN%03X: %s\r\n"),ErrCode,Buffer);
	return -ErrCode;
}

static void CheckUnknownElements(ebml_element *Elt)
{
	tchar_t IdStr[32], String[MAXPATH];
	ebml_element *SubElt;
	for (SubElt = EBML_MasterChildren(Elt); SubElt; SubElt = EBML_MasterNext(SubElt))
	{
		if (Node_IsPartOf(SubElt,EBML_DUMMY_ID))
		{
			Node_FromStr(Elt,String,TSIZEOF(String),Elt->Context->ElementName);
			EBML_IdToString(IdStr,TSIZEOF(IdStr),SubElt->Context->Id);
			OutputError(12,T("Unknown element in %s %s at %lld (size %lld)"),String,IdStr,(long)SubElt->ElementPosition,(long)SubElt->DataSize);
		}
		else if (Node_IsPartOf(SubElt,EBML_MASTER_CLASS))
		{
			CheckUnknownElements(SubElt);
		}
	}
}

static int CheckCodecs(ebml_element *Tracks, int ProfileNum)
{
	ebml_element *Track, *TrackType, *TrackNum;
	ebml_string *CodecID;
	tchar_t CodecName[MAXPATH];
	int Result = 0;
	Track = EBML_MasterFindFirstElt(Tracks, &MATROSKA_ContextTrackEntry, 0, 0);
	while (Track)
	{
		TrackNum = EBML_MasterFindFirstElt(Track, &MATROSKA_ContextTrackNumber, 1, 1);
		if (TrackNum)
		{
			TrackType = EBML_MasterFindFirstElt(Track, &MATROSKA_ContextTrackType, 1, 1);
			CodecID = (ebml_string*)EBML_MasterFindFirstElt(Track, &MATROSKA_ContextTrackCodecID, 1, 1);
			if (!CodecID)
				Result |= OutputError(0x300,T("Track #%d has no CodecID defined"),(long)EBML_IntegerValue(TrackNum));
			if (!TrackType)
				Result |= OutputError(0x301,T("Track #%d has no type defined"),(long)EBML_IntegerValue(TrackNum));
			else
			{
				if (ProfileNum==PROFILE_TEST_V1 || ProfileNum==PROFILE_TEST_V2)
				{
					if (EBML_IntegerValue(TrackType) != TRACK_TYPE_AUDIO && EBML_IntegerValue(TrackType) != TRACK_TYPE_VIDEO)
						Result |= OutputError(0x302,T("Track #%d type %d not supported for profile '%s'"),(long)EBML_IntegerValue(TrackNum),(long)EBML_IntegerValue(TrackType),GetProfileName(ProfileNum));
					if (CodecID)
					{
						EBML_StringGet(CodecID,CodecName,TSIZEOF(CodecName));
						if (EBML_IntegerValue(TrackType) == TRACK_TYPE_AUDIO)
						{
							if (!tcsisame_ascii(CodecName,T("A_VORBIS")))
								Result |= OutputError(0x303,T("Track #%d codec %s not supported for profile '%s'"),(long)EBML_IntegerValue(TrackNum),CodecName,GetProfileName(ProfileNum));
						}
						else if (EBML_IntegerValue(TrackType) == TRACK_TYPE_VIDEO)
						{
							const uint8_t Test[6] = {0x56, 0x5F, 0x56, 0x50, 0x38, 0x00};
							if (memcmp(CodecID->Buffer,Test,6)!=0)
								Result |= OutputError(0x304,T("Track #%d codec %s not supported for profile '%s'"),(long)EBML_IntegerValue(TrackNum),CodecName,GetProfileName(ProfileNum));
						}
					}
				}
			}
			Track = EBML_MasterFindNextElt(Tracks, Track, 0, 0);
		}
	}
	return Result;
}

static int CheckProfileViolation(ebml_element *Elt, int ProfileMask)
{
	int Result = 0;
	tchar_t String[MAXPATH],Invalid[MAXPATH];
	ebml_element *SubElt;

	Node_FromStr(Elt,String,TSIZEOF(String),Elt->Context->ElementName);
	if (Node_IsPartOf(Elt,EBML_MASTER_CLASS))
	{
		for (SubElt = EBML_MasterChildren(Elt); SubElt; SubElt = EBML_MasterNext(SubElt))
		{
			if (!Node_IsPartOf(SubElt,EBML_DUMMY_ID))
			{
				const ebml_semantic *i;
				for (i=Elt->Context->Semantic;i->eClass;++i)
				{
					if (i->eClass->Id==SubElt->Context->Id)
					{
						if ((i->DisabledProfile & ProfileMask)!=0)
						{
							Node_FromStr(Elt,Invalid,TSIZEOF(Invalid),i->eClass->ElementName);
							Result |= OutputError(0x201,T("Invalid %s for profile '%s' at %lld in %s"),Invalid,GetProfileName(ProfileMask),(long)SubElt->ElementPosition,String);
						}
						break;
					}
				}
				if (Node_IsPartOf(SubElt,EBML_MASTER_CLASS))
					Result |= CheckProfileViolation(SubElt, ProfileMask);
			}
		}
	}

	return Result;
}

static int CheckMandatory(ebml_element *Elt, int ProfileMask)
{
	int Result = 0;
	tchar_t String[MAXPATH],Missing[MAXPATH];
	ebml_element *SubElt;

	Node_FromStr(Elt,String,TSIZEOF(String),Elt->Context->ElementName);
	if (Node_IsPartOf(Elt,EBML_MASTER_CLASS))
	{
		const ebml_semantic *i;
		for (i=Elt->Context->Semantic;i->eClass;++i)
		{
			if ((i->DisabledProfile & ProfileMask)==0 && i->Mandatory && !i->eClass->HasDefault && !EBML_MasterFindChild(Elt,i->eClass))
			{
				Node_FromStr(Elt,Missing,TSIZEOF(Missing),i->eClass->ElementName);
				Result |= OutputError(0x200,T("Missing element %s in %s at %lld"),Missing,String,(long)Elt->ElementPosition);
			}
		}

		for (SubElt = EBML_MasterChildren(Elt); SubElt; SubElt = EBML_MasterNext(SubElt))
		{
			if (Node_IsPartOf(SubElt,EBML_MASTER_CLASS))
				Result |= CheckMandatory(SubElt, ProfileMask);
		}
	}

	return Result;
}

static int CheckSeekHead(ebml_element *SeekHead)
{
	int Result = 0;
	ebml_element *RLevel1 = EBML_MasterFindFirstElt(SeekHead, &MATROSKA_ContextSeek, 0, 0);
	while (RLevel1)
	{
		filepos_t Pos = MATROSKA_MetaSeekAbsolutePos((matroska_seekpoint*)RLevel1);
		fourcc_t SeekId = MATROSKA_MetaSeekID((matroska_seekpoint*)RLevel1);
		tchar_t IdString[32];

		EBML_IdToString(IdString,TSIZEOF(IdString),SeekId);
		if (Pos == INVALID_FILEPOS_T)
			Result |= OutputError(0x60,T("The SeekPoint at %lld has an unknown position (ID %s)"),(long)RLevel1->ElementPosition,IdString);
		else if (SeekId==0)
			Result |= OutputError(0x61,T("The SeekPoint at %lld has no ID defined (position %lld)"),(long)RLevel1->ElementPosition,(long)Pos);
		else if (SeekId == MATROSKA_ContextSegmentInfo.Id)
		{
			if (!RSegmentInfo)
				Result |= OutputError(0x62,T("The SeekPoint at %lld references an unknown SegmentInfo at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
			else if (RSegmentInfo->ElementPosition != Pos)
				Result |= OutputError(0x63,T("The SeekPoint at %lld references a SegmentInfo at wrong position %lld (real %lld)"),(long)RLevel1->ElementPosition,(long)Pos,(long)RSegmentInfo->ElementPosition);
		}
		else if (SeekId == MATROSKA_ContextTracks.Id)
		{
			if (!RTrackInfo)
				Result |= OutputError(0x64,T("The SeekPoint at %lld references an unknown TrackInfo at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
			else if (RTrackInfo->ElementPosition != Pos)
				Result |= OutputError(0x65,T("The SeekPoint at %lld references a TrackInfo at wrong position %lld (real %lld)"),(long)RLevel1->ElementPosition,(long)Pos,(long)RTrackInfo->ElementPosition);
		}
		else if (SeekId == MATROSKA_ContextCues.Id)
		{
			if (!RCues)
				Result |= OutputError(0x66,T("The SeekPoint at %lld references an unknown Cues at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
			else if (RCues->ElementPosition != Pos)
				Result |= OutputError(0x67,T("The SeekPoint at %lld references a Cues at wrong position %lld (real %lld)"),(long)RLevel1->ElementPosition,(long)Pos,(long)RCues->ElementPosition);
		}
		else if (SeekId == MATROSKA_ContextTags.Id)
		{
			if (!RTags)
				Result |= OutputError(0x68,T("The SeekPoint at %lld references an unknown Tags at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
			else if (RTags->ElementPosition != Pos)
				Result |= OutputError(0x69,T("The SeekPoint at %lld references a Tags at wrong position %lld (real %lld)"),(long)RLevel1->ElementPosition,(long)Pos,(long)RTags->ElementPosition);
		}
		else if (SeekId == MATROSKA_ContextChapters.Id)
		{
			if (!RChapters)
				Result |= OutputError(0x6A,T("The SeekPoint at %lld references an unknown Chapters at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
			else if (RChapters->ElementPosition != Pos)
				Result |= OutputError(0x6B,T("The SeekPoint at %lld references a Chapters at wrong position %lld (real %lld)"),(long)RLevel1->ElementPosition,(long)Pos,(long)RChapters->ElementPosition);
		}
		else if (SeekId == MATROSKA_ContextAttachments.Id)
		{
			if (!RAttachments)
				Result |= OutputError(0x6C,T("The SeekPoint at %lld references an unknown Attachments at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
			else if (RAttachments->ElementPosition != Pos)
				Result |= OutputError(0x6D,T("The SeekPoint at %lld references a Attachments at wrong position %lld (real %lld)"),(long)RLevel1->ElementPosition,(long)Pos,(long)RAttachments->ElementPosition);
		}
		else if (SeekId == MATROSKA_ContextSeekHead.Id)
		{
			if (SeekHead->ElementPosition == Pos)
				Result |= OutputError(0x6E,T("The SeekPoint at %lld references references its own SeekHead"),(long)RLevel1->ElementPosition);
			else if (SeekHead == RSeekHead && !RSeekHead2)
				Result |= OutputError(0x6F,T("The SeekPoint at %lld references an unknown secondary SeekHead at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
			else if (SeekHead == RSeekHead2 && Pos!=RSeekHead->ElementPosition)
				Result |= OutputError(0x70,T("The SeekPoint at %lld references an unknown extra SeekHead at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
		}
		else if (SeekId == MATROSKA_ContextCluster.Id)
		{
			ebml_element **Cluster;
			for (Cluster = ARRAYBEGIN(RClusters,ebml_element*);Cluster != ARRAYEND(RClusters,ebml_element*); ++Cluster)
			{
				if ((*Cluster)->ElementPosition == Pos)
					break;
			}
			if (Cluster == ARRAYEND(RClusters,ebml_element*) && Cluster != ARRAYBEGIN(RClusters,ebml_element*))
				Result |= OutputError(0x71,T("The SeekPoint at %lld references a Cluster not found at %lld"),(long)RLevel1->ElementPosition,(long)Pos);
		}
		else
			Result |= OutputWarning(0x860,T("The SeekPoint at %lld references an element that is not a known level 1 ID %s at %lld)"),(long)RLevel1->ElementPosition,IdString,(long)Pos);
		RLevel1 = EBML_MasterFindNextElt(SeekHead, RLevel1, 0, 0);
	}
	return Result;
}

static void LinkClusterBlocks()
{
	matroska_cluster **Cluster;
	for (Cluster=ARRAYBEGIN(RClusters,matroska_cluster*);Cluster!=ARRAYEND(RClusters,matroska_cluster*);++Cluster)
		MATROSKA_LinkClusterBlocks(*Cluster, RSegmentInfo, RTrackInfo);
}

static bool_t TrackIsLaced(int16_t TrackNum)
{
    ebml_element *TrackData, *Track = EBML_MasterFindFirstElt(RTrackInfo, &MATROSKA_ContextTrackEntry, 0, 0);
    while (Track)
    {
        TrackData = EBML_MasterFindFirstElt(Track, &MATROSKA_ContextTrackNumber, 1, 1);
        if (EBML_IntegerValue(TrackData) == TrackNum)
        {
            TrackData = EBML_MasterFindFirstElt(Track, &MATROSKA_ContextTrackLacing, 1, 1);
            return EBML_IntegerValue(TrackData) != 0;
        }
        Track = EBML_MasterFindNextElt(RTrackInfo, Track, 0, 0);
    }
    return 1;
}

static int CheckLacing()
{
	int Result = 0;
	matroska_cluster **Cluster;
    ebml_element *Block, *GBlock;

	for (Cluster=ARRAYBEGIN(RClusters,matroska_cluster*);Cluster!=ARRAYEND(RClusters,matroska_cluster*);++Cluster)
    {
	    for (Block = EBML_MasterChildren(*Cluster);Block;Block=EBML_MasterNext(Block))
	    {
		    if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
		    {
			    for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
			    {
				    if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
				    {
                        //MATROSKA_ContextTrackLacing
                        if (MATROSKA_BlockLaced((matroska_block*)GBlock) && !TrackIsLaced(MATROSKA_BlockTrackNum((matroska_block*)GBlock)))
                            Result |= 0;
					    break;
				    }
			    }
		    }
		    else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
		    {
                if (MATROSKA_BlockLaced((matroska_block*)Block) && !TrackIsLaced(MATROSKA_BlockTrackNum((matroska_block*)Block)))
                    Result |= 0;
		    }
	    }
    }
	return Result;
}

static int CheckCueEntries(ebml_element *Cues)
{
	int Result = 0;
	timecode_t TimecodeEntry, PrevTimecode = INVALID_TIMECODE_T;
	int16_t TrackNumEntry;
	matroska_cluster **Cluster;
	matroska_block *Block;
    int ClustNum = 0;

	if (!RSegmentInfo)
		Result |= OutputError(0x310,T("A Cues (index) is defined but no SegmentInfo was found"));
	else if (ARRAYCOUNT(RClusters,matroska_cluster*))
	{
		matroska_cuepoint *CuePoint = (matroska_cuepoint*)EBML_MasterFindFirstElt(Cues, &MATROSKA_ContextCuePoint, 0, 0);
		while (CuePoint)
		{
            if (ClustNum++ % 24 == 0)
                TextWrite(StdErr,T("."));
			MATROSKA_LinkCueSegmentInfo(CuePoint,RSegmentInfo);
			TimecodeEntry = MATROSKA_CueTimecode(CuePoint);
			TrackNumEntry = MATROSKA_CueTrackNum(CuePoint);

			if (TimecodeEntry < PrevTimecode && PrevTimecode != INVALID_TIMECODE_T)
				Result |= OutputError(0x311,T("The Cues entry for timecode %lld ms is listed after entry %lld ms"),(long)Scale64(TimecodeEntry,1,1000000),(long)Scale64(PrevTimecode,1,1000000));

			// find a matching Block
			for (Cluster = ARRAYBEGIN(RClusters,matroska_cluster*);Cluster != ARRAYEND(RClusters,matroska_cluster*); ++Cluster)
			{
				Block = MATROSKA_GetBlockForTimecode(*Cluster, TimecodeEntry, TrackNumEntry);
				if (Block)
					break;
			}
			if (Cluster == ARRAYEND(RClusters,matroska_cluster*))
				Result |= OutputError(0x312,T("CueEntry Track #%d and timecode %lld ms not found"),TrackNumEntry,(long)Scale64(TimecodeEntry,1,1000000));
			PrevTimecode = TimecodeEntry;
			CuePoint = (matroska_cuepoint*)EBML_MasterFindNextElt(Cues, (ebml_element*)CuePoint, 0, 0);
		}
	}
	return Result;
}

int main(int argc, const char *argv[])
{
    int Result = 0;
    parsercontext p;
    textwriter _StdErr;
    stream *Input = NULL;
    tchar_t Path[MAXPATHFULL];
    tchar_t String[MAXLINE];
    ebml_element *EbmlHead = NULL, *RSegment = NULL, *RLevel1 = NULL, **Cluster;
	ebml_element *EbmlDocVer, *EbmlReadDocVer;
    ebml_string *LibName, *AppName;
    ebml_parser_context RContext;
    ebml_parser_context RSegmentContext;
    int UpperElement;
	uint8_t Test[5] = {0x77, 0x65, 0x62, 0x6D, 0};
	int MatroskaProfile = 0;

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
	StdAfx_Init((nodemodule*)&p);
    ProjectSettings((nodecontext*)&p);

    // EBML & Matroska Init
    MATROSKA_Init((nodecontext*)&p);

    ArrayInit(&RClusters);

    StdErr = &_StdErr;
    memset(StdErr,0,sizeof(_StdErr));
    StdErr->Stream = (stream*)NodeSingleton(&p,STDERR_ID);

    if (argc < 2)
    {
        TextWrite(StdErr,T("mkvalidator v") PROJECT_VERSION T(", Copyright (c) 2010 Matroska Foundation\r\n"));
        Result = OutputError(1,T("Usage: mkvalidator <matroska_src>"));
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Result = -2;
        goto exit;
    }

    // parse the source file to determine if it's a Matroska file and determine the location of the key parts
    RContext.Context = &MATROSKA_ContextStream;
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
    EbmlHead = EBML_FindNextElement(Input, &RContext, &UpperElement, 1);
    if (!EbmlHead)
    {
        Result = OutputError(3,T("Could not find an EBML head"));
        goto exit;
    }

    TextWrite(StdErr,T("."));

	if (EBML_ElementReadData(EbmlHead,Input,&RContext,0,SCOPE_ALL_DATA)!=ERR_NONE)
    {
        Result = OutputError(4,T("Could not read the EBML head"));
        goto exit;
    }

	CheckUnknownElements(EbmlHead);

	RLevel1 = EBML_MasterFindFirstElt(EbmlHead,&EBML_ContextReadVersion,1,1);
	if (EBML_IntegerValue(RLevel1) > EBML_MAX_VERSION)
		OutputError(5,T("The EBML read version is not supported: %d"),(int)EBML_IntegerValue(RLevel1));

	RLevel1 = EBML_MasterFindFirstElt(EbmlHead,&EBML_ContextMaxIdLength,1,1);
	if (EBML_IntegerValue(RLevel1) > EBML_MAX_ID)
		OutputError(6,T("The EBML max ID length is not supported: %d"),(int)EBML_IntegerValue(RLevel1));

	RLevel1 = EBML_MasterFindFirstElt(EbmlHead,&EBML_ContextMaxSizeLength,1,1);
	if (EBML_IntegerValue(RLevel1) > EBML_MAX_SIZE)
		OutputError(7,T("The EBML max size length is not supported: %d"),(int)EBML_IntegerValue(RLevel1));

	RLevel1 = EBML_MasterFindFirstElt(EbmlHead,&EBML_ContextDocType,1,1);
    EBML_StringGet((ebml_string*)RLevel1,String,TSIZEOF(String));
    if (tcscmp(String,T("matroska"))!=0 && memcmp(((ebml_string*)RLevel1)->Buffer,Test,5)!=0)
	{
		Result = OutputError(8,T("The EBML doctype is not supported: %s"),String);
		goto exit;
	}

	EbmlDocVer = EBML_MasterFindFirstElt(EbmlHead,&EBML_ContextDocTypeVersion,1,1);
	EbmlReadDocVer = EBML_MasterFindFirstElt(EbmlHead,&EBML_ContextDocTypeReadVersion,1,1);

	if (EBML_IntegerValue(EbmlDocVer) > EBML_IntegerValue(EbmlReadDocVer))
		OutputError(9,T("The read DocType version %d is higher than the Doctype version %d"),EBML_IntegerValue(EbmlReadDocVer),EBML_IntegerValue(EbmlDocVer));

	if (tcscmp(String,T("matroska"))==0)
	{
		if (EBML_IntegerValue(EbmlReadDocVer)==2)
			MatroskaProfile = PROFILE_MATROSKA_V2;
		else if (EBML_IntegerValue(EbmlReadDocVer)==1)
			MatroskaProfile = PROFILE_MATROSKA_V1;
		else
			Result |= OutputError(10,T("Unknown Matroska profile %d/%d"),EBML_IntegerValue(EbmlDocVer),EBML_IntegerValue(EbmlReadDocVer));
	}
	else if (EBML_IntegerValue(EbmlReadDocVer)==1)
		MatroskaProfile = PROFILE_TEST_V1;
	else if (EBML_IntegerValue(EbmlReadDocVer)==2)
		MatroskaProfile = PROFILE_TEST_V2;

	if (MatroskaProfile==0)
		Result |= OutputError(11,T("Matroska profile not supported"));

    TextWrite(StdErr,T("."));

	// find the segment
	RSegment = EBML_FindNextElement(Input, &RContext, &UpperElement, 1);
    RSegmentContext.Context = &MATROSKA_ContextSegment;
    RSegmentContext.EndPosition = EBML_ElementPositionEnd(RSegment);
    RSegmentContext.UpContext = &RContext;
	UpperElement = 0;
//TextPrintf(StdErr,T("Loading the level1 elements in memory\r\n"));
    RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    while (RLevel1)
	{
        if (RLevel1->Context->Id == MATROSKA_ContextCluster.Id)
        {
            //TextWrite(StdErr,T("."));

            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_PARTIAL_DATA)==ERR_NONE)
			{
                ArrayAppend(&RClusters,&RLevel1,sizeof(RLevel1),256);
				NodeTree_SetParent(RLevel1, RSegment, NULL);
				CheckUnknownElements(RLevel1);
				Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
				Result |= CheckMandatory(RLevel1, MatroskaProfile);
			}
			else
			{
				Result = OutputError(0x180,T("Failed to read the Cluster at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextSeekHead.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
			{
				if (!RSeekHead)
					RSeekHead = RLevel1;
				else if (!RSeekHead2)
					RSeekHead2 = RLevel1;
				else
					Result |= OutputError(0x101,T("Extra SeekHead found at %lld (size %lld)"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				NodeTree_SetParent(RLevel1, RSegment, NULL);
				CheckUnknownElements(RLevel1);
				Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
				Result |= CheckMandatory(RLevel1, MatroskaProfile);
			}
			else
			{
				Result = OutputError(0x100,T("Failed to read the SeekHead at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
		}
        else if (RLevel1->Context->Id == MATROSKA_ContextSegmentInfo.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
			{
				if (RSegmentInfo != NULL)
					Result |= OutputError(0x110,T("Extra SegmentInfo found at %lld (size %lld)"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				else
				{
					RSegmentInfo = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
					Result |= CheckMandatory(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x111,T("Failed to read the SegmentInfo at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
		}
        else if (RLevel1->Context->Id == MATROSKA_ContextTracks.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
			{
				if (RTrackInfo != NULL)
					Result |= OutputError(0x120,T("Extra TrackInfo found at %lld (size %lld)"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				else
				{
					RTrackInfo = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
					Result |= CheckMandatory(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x121,T("Failed to read the TrackInfo at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
		}
        else if (RLevel1->Context->Id == MATROSKA_ContextCues.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
			{
				if (RCues != NULL)
					Result |= OutputError(0x130,T("Extra Cues found at %lld (size %lld)"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				else
				{
					RCues = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
					Result |= CheckMandatory(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x131,T("Failed to read the Cues at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
		}
        else if (RLevel1->Context->Id == MATROSKA_ContextChapters.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
			{
				if (RChapters != NULL)
					Result |= OutputError(0x140,T("Extra Chapters found at %lld (size %lld)"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				else
				{
					RChapters = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
					Result |= CheckMandatory(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x141,T("Failed to read the Chapters at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
		}
        else if (RLevel1->Context->Id == MATROSKA_ContextTags.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
			{
				if (RTags != NULL)
					Result |= OutputError(0x150,T("Extra Tags found at %lld (size %lld)"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				else
				{
					RTags = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
					Result |= CheckMandatory(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x151,T("Failed to read the Tags at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
		}
        else if (RLevel1->Context->Id == MATROSKA_ContextAttachments.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
			{
				if (RAttachments != NULL)
					Result |= OutputError(0x160,T("Extra Attachments found at %lld (size %lld)"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				else
				{
					RAttachments = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
					Result |= CheckMandatory(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x161,T("Failed to read the Attachments at %lld size %lld"),(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
				goto exit;
			}
		}
		else
		{
			if (Node_IsPartOf(RLevel1,EBML_DUMMY_ID))
			{
				tchar_t Id[32];
				EBML_IdToString(Id,TSIZEOF(Id),RLevel1->Context->Id);
				Result |= OutputError(0x80,T("Unknown element %s at %lld size %lld"),Id,(long)RLevel1->ElementPosition,(long)RLevel1->DataSize);
			}
			EBML_ElementSkipData(RLevel1, Input, &RSegmentContext, NULL, 1);
            NodeDelete((node*)RLevel1);
		}
        //TextWrite(StdErr,T("."));

		RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
	}

	if (!RSegmentInfo)
	{
		Result = OutputError(0x40,T("The segment is missing a SegmentInfo"));
		goto exit;
	}

	if (!RSeekHead)
		OutputWarning(0x800,T("The segment has no SeekHead section"));
	else
		Result |= CheckSeekHead(RSeekHead);
	if (RSeekHead2)
		Result |= CheckSeekHead(RSeekHead2);

	if (ARRAYCOUNT(RClusters,ebml_element*))
	{
        TextWrite(StdErr,T("."));
		LinkClusterBlocks();

        Result |= CheckLacing();
		if (!RCues)
			OutputWarning(0x800,T("The segment has Clusters but no Cues section (bad for seeking)"));
		else
			CheckCueEntries(RCues);
		if (!RTrackInfo)
		{
			Result = OutputError(0x41,T("The segment has Clusters but no TrackInfo section"));
			goto exit;
		}
	}

    TextWrite(StdErr,T("."));
	if (RTrackInfo)
		CheckCodecs(RTrackInfo, MatroskaProfile);

	if (Result==0)
        TextPrintf(StdErr,T("\r%s %s: the file appears to be valid\r\n"),PROJECT_NAME,PROJECT_VERSION);

exit:
	if (RSegmentInfo)
	{
		tchar_t App[MAXPATH];
		App[0] = 0;
		LibName = (ebml_string*)EBML_MasterFindFirstElt(RSegmentInfo,&MATROSKA_ContextMuxingApp,0,0);
		AppName = (ebml_string*)EBML_MasterFindFirstElt(RSegmentInfo,&MATROSKA_ContextWritingApp,0,0);
		if (AppName)
		{
			EBML_StringGet(AppName,String,TSIZEOF(String));
			tcscat_s(App,TSIZEOF(App),String);
		}
		if (LibName)
		{
			EBML_StringGet(LibName,String,TSIZEOF(String));
			if (App[0])
				tcscat_s(App,TSIZEOF(App),T(" / "));
			tcscat_s(App,TSIZEOF(App),String);
		}
		if (App[0]==0)
			tcscat_s(App,TSIZEOF(App),T("<unknown>"));
		TextPrintf(StdErr,T("\r\tfile created with %s\r\n"),App);
	}

    for (Cluster = ARRAYBEGIN(RClusters,ebml_element*);Cluster != ARRAYEND(RClusters,ebml_element*); ++Cluster)
        NodeDelete((node*)*Cluster);
    ArrayClear(&RClusters);
    if (RAttachments)
        NodeDelete((node*)RAttachments);
    if (RTags)
        NodeDelete((node*)RTags);
    if (RCues)
        NodeDelete((node*)RCues);
    if (RChapters)
        NodeDelete((node*)RChapters);
    if (RTrackInfo)
        NodeDelete((node*)RTrackInfo);
    if (RSegmentInfo)
        NodeDelete((node*)RSegmentInfo);
    if (RLevel1)
        NodeDelete((node*)RLevel1);
    if (RSegment)
        NodeDelete((node*)RSegment);
    if (EbmlHead)
        NodeDelete((node*)EbmlHead);
    if (Input)
        StreamClose(Input);

    // EBML & Matroska ending
    MATROSKA_Done((nodecontext*)&p);

    // Core-C ending
	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);

    return Result;
}
