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
#include "mkclean_stdafx.h"
#include "mkclean_project.h"
#define CONFIG_EBML_UNICODE
#include "matroska/matroska.h"

/*!
 * \todo verify that no lacing is used when lacing is disabled in the SegmentInfo
 * \todo forbid the use of SimpleBlock in v1 (strict profiling)
 * \todo start a new cluster boundary with each video keyframe
 * \todo change the Segment UID (when key parts are altered)
 * \todo optionally add a CRC32 on level1 elements
 * \todo error when an unknown codec (for the profile) is found (option to turn into a warning)
 * \todo force keeping some forbidden elements in a profile (chapters in 'test')
 * \todo allow creating/replacing Tags
 * \todo allow creating/replacing Chapters
 * \todo allow creating/replacing Attachments
 */

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

static int DocVersion = 1;
static int Profile = 0;
static textwriter *StdErr = NULL;
static uint8_t Test[5] = {0x77, 0x65, 0x62, 0x6D, 0};

static void ReduceSize(ebml_element *Element)
{
    Element->SizeLength = 0; // reset
    if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    {
        ebml_element *i;
		EBML_MasterMandatory(Element,1);
        for (i=EBML_MasterChildren(Element);i;i=EBML_MasterNext(i))
            ReduceSize(i);
    }
}

static void SettleClustersWithCues(array *Clusters, filepos_t ClusterStart, ebml_element *Cues, ebml_element *Segment)
{
    ebml_element **Cluster;
    ebml_element *Cue;
    filepos_t OriginalSize = Cues->DataSize;
    filepos_t ClusterPos = ClusterStart + EBML_ElementFullSize(Cues,0);
    // reposition all the Clusters
    for (Cluster=ARRAYBEGIN(*Clusters,ebml_element*);Cluster!=ARRAYEND(*Clusters,ebml_element*);++Cluster)
    {
        (*Cluster)->ElementPosition = ClusterPos;
        EBML_ElementUpdateSize(*Cluster,0,0);
        ClusterPos += EBML_ElementFullSize(*Cluster,0);
    }

    // reevaluate the size needed for the Cues
    for (Cue=EBML_MasterChildren(Cues);Cue;Cue=EBML_MasterNext(Cue))
        MATROSKA_CuePointUpdate((matroska_cuepoint*)Cue, Segment);
    ClusterPos = EBML_ElementUpdateSize(Cues,0,0);
    if (ClusterPos != OriginalSize)
        SettleClustersWithCues(Clusters,ClusterStart,Cues,Segment);
}

static void ShowProgress(const ebml_element *RCluster, const ebml_element *RSegment, int phase)
{
    TextPrintf(StdErr,T("Progress %d/3: %3d%%\r"),phase,Scale32(100,RCluster->ElementPosition,RSegment->DataSize)+1);
}

static void EndProgress(const ebml_element *RSegment, int phase)
{
    TextPrintf(StdErr,T("Progress %d/3: 100%%\r\n"),phase);
}

static matroska_cluster **LinkCueCluster(matroska_cuepoint *Cue, array *Clusters, matroska_cluster **StartCluster, const ebml_element *RSegment)
{
    matroska_cluster **Cluster;
    matroska_block *Block;
    int16_t CueTrack;
    timecode_t CueTimecode;
    size_t StartBoost = 7;

    CueTrack = MATROSKA_CueTrackNum(Cue);
    CueTimecode = MATROSKA_CueTimecode(Cue);
    if (StartCluster)
    {
        for (Cluster=StartCluster;StartBoost && Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster,--StartBoost)
        {
            Block = MATROSKA_GetBlockForTimecode(*Cluster, CueTimecode, CueTrack);
            if (Block)
            {
                MATROSKA_LinkCuePointBlock(Cue,Block);
                ShowProgress((ebml_element*)(*Cluster),RSegment,2);
                return Cluster;
            }
        }
    }

    for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
    {
        Block = MATROSKA_GetBlockForTimecode(*Cluster, CueTimecode, CueTrack);
        if (Block)
        {
            MATROSKA_LinkCuePointBlock(Cue,Block);
            ShowProgress((ebml_element*)(*Cluster),RSegment,2);
            return Cluster;
        }
    }

    TextPrintf(StdErr,T("Could not find the matching block for timecode %0.3f s\r\n"),CueTimecode/1000000000.0);
    return NULL;
}

static void LinkClusters(array *Clusters, ebml_element *RSegmentInfo, ebml_element *Tracks)
{
    matroska_cluster **Cluster;

	// link each Block/SimpleBlock with its Track and SegmentInfo
	for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
	{
		MATROSKA_LinkClusterBlocks(*Cluster, RSegmentInfo, Tracks);
		ReduceSize((ebml_element*)*Cluster);
	}
}

static void OptimizeCues(ebml_element *Cues, array *Clusters, ebml_element *RSegmentInfo, filepos_t StartPos, ebml_element *WSegment, const ebml_element *RSegment, bool_t ReLink)
{
    matroska_cluster **Cluster;
    matroska_cuepoint *Cue;

    ReduceSize(Cues);

	if (ReLink)
	{
		// link each Cue entry to the segment
		for (Cue = (matroska_cuepoint*)EBML_MasterChildren(Cues);Cue;Cue=(matroska_cuepoint*)EBML_MasterNext(Cue))
			MATROSKA_LinkCueSegmentInfo(Cue,RSegmentInfo);

		// link each Cue entry to the corresponding Block/SimpleBlock in the Cluster
		Cluster = NULL;
		for (Cue = (matroska_cuepoint*)EBML_MasterChildren(Cues);Cue;Cue=(matroska_cuepoint*)EBML_MasterNext(Cue))
			Cluster = LinkCueCluster(Cue,Clusters,Cluster,RSegment);
		EndProgress(RSegment,2);
	}

    // sort the Cues
    MATROSKA_CuesSort(Cues);

    SettleClustersWithCues(Clusters,StartPos,Cues,WSegment);
}

static ebml_element *CheckMatroskaHead(const ebml_element *Head, const ebml_parser_context *Parser, stream *Input)
{
    ebml_parser_context SubContext;
    ebml_element *SubElement;
    int UpperElement;
    tchar_t String[MAXLINE];

    SubContext.UpContext = Parser;
    SubContext.Context = Head->Context;
    SubContext.EndPosition = EBML_ElementPositionEnd(Head);
    SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
    while (SubElement)
    {
        if (SubElement->Context->Id == EBML_ContextReadVersion.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue(SubElement) > EBML_MAX_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %ld not supported"),(long)EBML_IntegerValue(SubElement));
                break;
            }
        }
        else if (SubElement->Context->Id == EBML_ContextMaxIdLength.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue(SubElement) > EBML_MAX_ID)
            {
                TextPrintf(StdErr,T("EBML Max ID Length %ld not supported"),(long)EBML_IntegerValue(SubElement));
                break;
            }
        }
        else if (SubElement->Context->Id == EBML_ContextMaxSizeLength.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue(SubElement) > EBML_MAX_SIZE)
            {
                TextPrintf(StdErr,T("EBML Max Coded Size %ld not supported"),(long)EBML_IntegerValue(SubElement));
                break;
            }
        }
        else if (SubElement->Context->Id == EBML_ContextDocType.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else
            {
                EBML_StringGet((ebml_string*)SubElement,String,TSIZEOF(String));
                if (tcscmp(String,T("matroska"))==0)
                    Profile = PROFILE_MATROSKA_V1;
                else if (memcmp(((ebml_string*)SubElement)->Buffer,Test,5)==0)
                    Profile = PROFILE_TEST;
                else
                {
                    TextPrintf(StdErr,T("EBML DocType %s not supported"),(long)EBML_IntegerValue(SubElement));
                    break;
                }
            }
        }
        else if (SubElement->Context->Id == EBML_ContextDocTypeReadVersion.Id)
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue(SubElement) > MATROSKA_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %ld not supported"),(long)EBML_IntegerValue(SubElement));
                break;
            }
            else
                DocVersion = (int)EBML_IntegerValue(SubElement);
        }
        else if (SubElement->Context->Id == MATROSKA_ContextSegment.Id)
            return SubElement;
        else
            EBML_ElementSkipData(SubElement,Input,NULL,NULL,0);
        NodeDelete((node*)SubElement);
        SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
    }

    return NULL;
}

static void WriteCluster(ebml_element *Cluster, stream *Output, stream *Input)
{
    filepos_t IntendedPosition = Cluster->ElementPosition;
    ebml_element *Block, *GBlock;
    for (Block = EBML_MasterChildren(Cluster);Block;Block=EBML_MasterNext(Block))
    {
        if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
                {
                    MATROSKA_BlockReadData((matroska_block*)GBlock, Input);
                    break;
                }
            }
        }
        else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
            MATROSKA_BlockReadData((matroska_block*)Block, Input);
    }

    EBML_ElementRender(Cluster,Output,0,0,1,NULL,0);

    for (Block = EBML_MasterChildren(Cluster);Block;Block=EBML_MasterNext(Block))
    {
        if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
                {
                    MATROSKA_BlockReleaseData((matroska_block*)GBlock);
                    break;
                }
            }
        }
        else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
            MATROSKA_BlockReleaseData((matroska_block*)Block);
    }

    if (Cluster->ElementPosition != IntendedPosition)
    {
        TextPrintf(StdErr,T("Failed to write a Cluster at the required position %ld vs %ld"),(long)Cluster->ElementPosition,(long)IntendedPosition);
    }
}

static void MetaSeekUpdate(ebml_element *SeekHead)
{
    matroska_seekpoint *SeekPoint;
    for (SeekPoint=(matroska_seekpoint*)EBML_MasterChildren(SeekHead); SeekPoint; SeekPoint=(matroska_seekpoint*)EBML_MasterNext(SeekPoint))
        MATROSKA_MetaSeekUpdate(SeekPoint);
    EBML_ElementUpdateSize(SeekHead,0,0);
}

static void GenerateCueEntries(ebml_element *Cues, array *RClusters, ebml_element *RTrackInfo, ebml_element *RSegmentInfo, ebml_element *RSegment)
{
	ebml_element *Track, *Elt;
	matroska_block *Block;
	ebml_element **Cluster;
	matroska_cuepoint *CuePoint;
	int64_t TrackNum;
	timecode_t PrevTimecode = INVALID_TIMECODE_T, BlockTimecode;

	// find the video (first) track
	for (Track = EBML_MasterFindFirstElt(RTrackInfo,&MATROSKA_ContextTrackEntry,0,0); Track; Track=EBML_MasterFindNextElt(RTrackInfo,Track,0,0))
	{
		Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackType,0,0);
		if (Elt && EBML_IntegerValue(Elt) == TRACK_TYPE_VIDEO)
		{
			Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
			if (Elt)
			{
				TrackNum = EBML_IntegerValue(Elt);
				break;
			}
		}
	}

	if (!Track)
	{
		// no video track found, look for an audio track
		for (Track = EBML_MasterFindFirstElt(RTrackInfo,&MATROSKA_ContextTrackEntry,0,0); Track; Track=EBML_MasterFindNextElt(RTrackInfo,Track,0,0))
		{
			Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackType,0,0);
			if (Elt && EBML_IntegerValue(Elt) == TRACK_TYPE_AUDIO)
			{
				Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
				if (Elt)
				{
					TrackNum = EBML_IntegerValue(Elt);
					break;
				}
			}
		}
	}

	if (!Track)
	{
		TextPrintf(StdErr,T("Could not find an audio or video track to generate the Cue entries"));
		return;
	}

	// find all the keyframes
	for (Cluster = ARRAYBEGIN(*RClusters,ebml_element*);Cluster != ARRAYEND(*RClusters,ebml_element*); ++Cluster)
	{
		MATROSKA_LinkClusterSegmentInfo((matroska_cluster*)*Cluster,RSegmentInfo);
		for (Elt = EBML_MasterChildren(*Cluster); Elt; Elt = EBML_MasterNext(Elt))
		{
			Block = NULL;
			if (Elt->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
			{
				if (MATROSKA_BlockKeyframe((matroska_block*)Elt))
					Block = (matroska_block*)Elt;
			}
			else if (Elt->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
			{
				ebml_element *EltB, *BlockRef = NULL;
				for (EltB = EBML_MasterChildren(Elt); EltB; EltB = EBML_MasterNext(EltB))
				{
					if (EltB->Context->Id == MATROSKA_ContextClusterBlock.Id)
						Block = (matroska_block*)EltB;
					else if (EltB->Context->Id == MATROSKA_ContextClusterReferenceBlock.Id)
						BlockRef = EltB;
				}
				if (BlockRef && Block)
					Block = NULL; // not a keyframe
			}

			if (Block && MATROSKA_BlockTrackNum(Block) == TrackNum)
			{
				BlockTimecode = MATROSKA_BlockTimecode(Block);
				if ((BlockTimecode - PrevTimecode) < 800000000 && PrevTimecode != INVALID_TIMECODE_T)
					break; // no more than 1 Cue per Cluster and per 800 ms

				CuePoint = (matroska_cuepoint*)EBML_MasterAddElt(Cues,&MATROSKA_ContextCuePoint,1);
				if (!CuePoint)
				{
					TextPrintf(StdErr,T("Failed to create a new CuePoint ! out of memory ?"));
					return;
				}
				MATROSKA_LinkCueSegmentInfo(CuePoint,RSegmentInfo);
				MATROSKA_LinkCuePointBlock(CuePoint,Block);
				MATROSKA_CuePointUpdate(CuePoint,RSegment);

				PrevTimecode = BlockTimecode;

				break; // one Cues per Cluster is enough
			}
		}
	}

	EBML_ElementUpdateSize(Cues,0,0);
}

int main(int argc, const char *argv[])
{
    int i,Result = 0;
    parsercontext p;
    textwriter _StdErr;
    stream *Input = NULL,*Output = NULL;
    tchar_t Path[MAXPATHFULL];
    tchar_t String[MAXLINE],Original[MAXLINE],*s;
    ebml_element *EbmlHead = NULL, *RSegment = NULL, *RLevel1 = NULL, **Cluster;
    ebml_element *RSegmentInfo = NULL, *RTrackInfo = NULL, *RChapters = NULL, *RTags = NULL, *RCues = NULL, *RAttachments = NULL;
    ebml_element *WSegment = NULL, *WMetaSeek = NULL;
    matroska_seekpoint *WSeekPoint = NULL;
    ebml_string *LibName, *AppName;
    array RClusters;
    ebml_parser_context RContext;
    ebml_parser_context RSegmentContext;
    int UpperElement;
    filepos_t MetaSeekBefore, MetaSeekAfter;
    filepos_t NextPos, SegmentSize = 0;
	bool_t KeepCues = 0, CuesCreated = 0;

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

    if (argc < 3)
    {
        TextWrite(StdErr,T("mkclean v") PROJECT_VERSION T(", Copyright (c) 2010 Matroska Foundation\r\n"));
        TextWrite(StdErr,T("Usage: mkclean [--keep-cues] <matroska_src> <matroska_dst>\r\n"));
		TextWrite(StdErr,T("  --keep-cues: keep the original Cues content and move it to the front\r\n"));
        Result = -1;
        goto exit;
    }

	for (i=1;i<argc-2;++i)
	{
	    Node_FromStr(&p,Path,TSIZEOF(Path),argv[i]);
		if (tcsisame_ascii(Path,T("--keep-cues")))
			KeepCues = 1;
	}

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-2]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Result = -2;
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
    Output = StreamOpen(&p,Path,SFLAG_WRONLY|SFLAG_CREATE);
    if (!Output)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for writing\r\n"),Path);
        Result = -3;
        goto exit;
    }

    // parse the source file to determine if it's a Matroska file and determine the location of the key parts
    RContext.Context = &MATROSKA_ContextStream;
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
    EbmlHead = EBML_FindNextElement(Input, &RContext, &UpperElement, 1);
    if (!EbmlHead)
    {
        TextWrite(StdErr,T("Could not find an EBML head!\r\n"));
        Result = -4;
        goto exit;
    }

    RSegment = CheckMatroskaHead(EbmlHead,&RContext,Input);
    if (Profile==PROFILE_MATROSKA_V1 && DocVersion==2)
        Profile = PROFILE_MATROSKA_V2;
    if (!RSegment)
    {
        Result = -5;
        goto exit;
    }
    NodeDelete((node*)EbmlHead);
    EbmlHead = NULL;

    // locate the Segment Info, Track Info, Chapters, Tags, Attachments, Cues Clusters*
    RSegmentContext.Context = &MATROSKA_ContextSegment;
    RSegmentContext.EndPosition = EBML_ElementPositionEnd(RSegment);
    RSegmentContext.UpContext = &RContext;
	UpperElement = 0;
//TextPrintf(StdErr,T("Loading the level1 elements in memory\r\n"));
    RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    while (RLevel1)    {
        ShowProgress(RLevel1,RSegment,1);
        if (RLevel1->Context->Id == MATROSKA_ContextSegmentInfo.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RSegmentInfo = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextTracks.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RTrackInfo = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextChapters.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RChapters = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextTags.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RTags = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextCues.Id && KeepCues)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RCues = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextAttachments.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_ALL_DATA)==ERR_NONE)
                RAttachments = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextCluster.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,0,SCOPE_PARTIAL_DATA)==ERR_NONE)
                ArrayAppend(&RClusters,&RLevel1,sizeof(RLevel1),256);
        }
        else
		{
			EbmlHead = EBML_ElementSkipData(RLevel1, Input, &RSegmentContext, NULL, 1);
			assert(EbmlHead==NULL);
            NodeDelete((node*)RLevel1);
		}
        RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    }
    EndProgress(RSegment,1);

    if (!RSegmentInfo)
    {
        TextWrite(StdErr,T("The source Segment has no Segment Info section\r\n"));
        Result = -6;
        goto exit;
    }
    if (!RTrackInfo && ARRAYCOUNT(RClusters,ebml_element*))
    {
        TextWrite(StdErr,T("The source Segment has no Track Info section\r\n"));
        Result = -7;
        goto exit;
    }

    // Write the EBMLHead
    EbmlHead = EBML_ElementCreate(&p,&EBML_ContextHead,0,NULL);
    if (!EbmlHead)
        goto exit;
    NodeTree_Clear((nodetree*)EbmlHead); // remove the default values
    // DocType
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocType);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_STRING_CLASS));
    if (Profile == PROFILE_TEST)
    {
        if (EBML_StringSetValue((ebml_string*)RLevel1,(const char *)Test) != ERR_NONE)
            goto exit;
    }
    else
    {
        if (EBML_StringSetValue((ebml_string*)RLevel1,"matroska") != ERR_NONE)
            goto exit;
    }

    // Doctype version
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    EBML_IntegerSetValue((ebml_integer*)RLevel1, DocVersion);

    // Doctype readable version
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeReadVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    EBML_IntegerSetValue((ebml_integer*)RLevel1, DocVersion);

    if (EBML_ElementRender(EbmlHead,Output,1,0,1,NULL,1)!=ERR_NONE)
        goto exit;
    NodeDelete((node*)EbmlHead);
    EbmlHead = NULL;
    RLevel1 = NULL;

    // Write the Matroska Segment Head
    WSegment = EBML_ElementCreate(&p,&MATROSKA_ContextSegment,0,NULL);
    WSegment->DataSize = RSegment->DataSize; // temporary value
    if (EBML_ElementRenderHead(WSegment,Output,0,NULL)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the (temporary) Segment head\r\n"));
        Result = -10;
        goto exit;
    }

    //  Prepare the Meta Seek
    WMetaSeek = EBML_MasterAddElt(WSegment,&MATROSKA_ContextSeekHead,0);
    WMetaSeek->ElementPosition = Stream_Seek(Output,0,SEEK_CUR); // keep the position for when we need to write it
    NextPos = 100; // dumy position of the SeekHead end
    // segment info
    WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
    RSegmentInfo->ElementPosition = NextPos;
    NextPos += EBML_ElementFullSize(RSegmentInfo,0);
    MATROSKA_LinkMetaSeekElement(WSeekPoint,RSegmentInfo);
    // track info
    if (RTrackInfo)
    {
        WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        RTrackInfo->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RTrackInfo,0);
        MATROSKA_LinkMetaSeekElement(WSeekPoint,RTrackInfo);
    }
    else
    {
        TextWrite(StdErr,T("Warning: the source Segment has no Track Info section (can be a chapter file)\r\n"));
    }
    // chapters
    if (RChapters)
    {
		ReduceSize(RChapters);
		if (!EBML_MasterCheckMandatory(RChapters,0))
		{
			TextWrite(StdErr,T("The Chapters section is missing mandatory elements, skipping\r\n"));
			NodeDelete((node*)RChapters);
			RChapters = NULL;
		}
		else
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
			RChapters->ElementPosition = NextPos;
			NextPos += EBML_ElementFullSize(RChapters,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,RChapters);
		}
    }
    // tags
    if (RTags)
    {
		RLevel1 = EBML_MasterFindFirstElt(RTags, &MATROSKA_ContextTag, 0, 0);
		while (RLevel1)
		{
			EbmlHead = EBML_MasterFindFirstElt(RLevel1, &MATROSKA_ContextSimpleTag, 0, 0);
			if (!EbmlHead)
			{
				TextPrintf(StdErr,T("The Tag element at %lld has no SimpleTag defined, skipping\r\n"),(long)RLevel1->ElementPosition);
				EBML_MasterRemove(RTags,RLevel1);
				NodeDelete((node*)RLevel1);
				RLevel1 = EBML_MasterFindFirstElt(RTags, &MATROSKA_ContextTag, 0, 0);
				continue;
			}
			RLevel1 = EBML_MasterFindNextElt(RTags, RLevel1, 0, 0);
		}
		RLevel1 = NULL;

		ReduceSize(RTags);
		if (!EBML_MasterCheckMandatory(RTags,0))
		{
			TextWrite(StdErr,T("The Tags section is missing mandatory elements, skipping\r\n"));
			NodeDelete((node*)RTags);
			RTags = NULL;
		}
		else
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
			RTags->ElementPosition = NextPos;
			NextPos += EBML_ElementFullSize(RTags,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,RTags);
		}
    }

	LinkClusters(&RClusters,RSegmentInfo,RTrackInfo);

    // cues
	if (RCues)
	{
		ReduceSize(RCues);
		if (!EBML_MasterCheckMandatory(RCues,0))
		{
			TextWrite(StdErr,T("The original Cues are missing mandatory elements, creating from scratch\r\n"));
			NodeDelete((node*)RCues);
			RCues = NULL;
		}
	}
	if (!RCues)
	{
		// generate the cues
		RCues = EBML_ElementCreate(&p,&MATROSKA_ContextCues,0,NULL);
		GenerateCueEntries(RCues,&RClusters,RTrackInfo,RSegmentInfo,RSegment);
		CuesCreated = 1;
	}
    if (RCues)
    {
        WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        RCues->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RCues,0);
        MATROSKA_LinkMetaSeekElement(WSeekPoint,RCues);
    }
    // attachements
    if (RAttachments)
    {
		ReduceSize(RAttachments);
		if (!EBML_MasterCheckMandatory(RAttachments,0))
		{
			TextWrite(StdErr,T("The Attachments section is missing mandatory elements, skipping\r\n"));
			NodeDelete((node*)RAttachments);
			RAttachments = NULL;
		}
		else
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
			RAttachments->ElementPosition = EBML_ElementPositionEnd(RSegment) - EBML_ElementFullSize(RAttachments,0); // virutally position the attachment at the end of the segment
			NextPos += EBML_ElementFullSize(RAttachments,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,RAttachments);
		}
    }

	// first estimation of the MetaSeek size
    MetaSeekUpdate(WMetaSeek);
	MetaSeekBefore = EBML_ElementFullSize(WMetaSeek,0);
    NextPos = WMetaSeek->ElementPosition + EBML_ElementFullSize(WMetaSeek,0);

    //  Compute the Segment Info size
    ReduceSize(RSegmentInfo);
    // change the library names & app name
    LibName = (ebml_string*)EBML_MasterFindFirstElt(RSegmentInfo, &MATROSKA_ContextMuxingApp, 1, 0);
    stprintf_s(String,TSIZEOF(String),T("%s + %s"),Node_GetDataStr((node*)&p,CONTEXT_LIBEBML_VERSION),Node_GetDataStr((node*)&p,CONTEXT_LIBMATROSKA_VERSION));
    EBML_StringGet(LibName,Original,TSIZEOF(Original));
    EBML_UniStringSetValue(LibName,String);

    AppName = (ebml_string*)EBML_MasterFindFirstElt(RSegmentInfo, &MATROSKA_ContextWritingApp, 1, 0);
    EBML_StringGet(AppName,String,TSIZEOF(String));
    if (!tcsisame_ascii(String,Original)) // libavformat writes the same twice, we only need one
    {
        tcscat_s(Original,TSIZEOF(Original),T(" + "));
        tcscat_s(Original,TSIZEOF(Original),String);
    }
    s = Original;
    if (tcsnicmp_ascii(Original,T("mkclean "),8)==0)
        s += 14;
    stprintf_s(String,TSIZEOF(String),T("mkclean %s from %s"),PROJECT_VERSION,s);
    EBML_UniStringSetValue(AppName,String);
    EBML_ElementUpdateSize(RSegmentInfo,0,0);
    RSegmentInfo->ElementPosition = NextPos;
    NextPos += EBML_ElementFullSize(RSegmentInfo,0);

    //  Compute the Track Info size
    if (RTrackInfo)
    {
        ReduceSize(RTrackInfo);
        EBML_ElementUpdateSize(RTrackInfo,0,0);
        RTrackInfo->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RTrackInfo,0);
    }

    //  Compute the Chapters size
    if (RChapters)
    {
        ReduceSize(RChapters);
        EBML_ElementUpdateSize(RChapters,0,0);
        RChapters->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RChapters,0);
    }

    //  Compute the Tags size
    if (RTags)
    {
		EBML_ElementUpdateSize(RTags,0,0);
		RTags->ElementPosition = NextPos;
		NextPos += EBML_ElementFullSize(RTags,0);
    }

	MetaSeekUpdate(WMetaSeek);
	NextPos += EBML_ElementFullSize(WMetaSeek,0) - MetaSeekBefore;

	//  Compute the Cues size
    if (RTrackInfo && RCues)
    {
        OptimizeCues(RCues,&RClusters,RSegmentInfo,NextPos, WSegment, RSegment, !CuesCreated);
        EBML_ElementUpdateSize(RCues,0,0);
        RCues->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RCues,0);
    }

    // update and write the MetaSeek and the elements following
    MetaSeekUpdate(WMetaSeek);
    EBML_ElementFullSize(WMetaSeek,0);
    Stream_Seek(Output,WMetaSeek->ElementPosition,SEEK_SET);
    if (EBML_ElementRender(WMetaSeek,Output,0,0,1,&MetaSeekBefore,0)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the final Seek Head\r\n"));
        Result = -22;
        goto exit;
    }
    SegmentSize += EBML_ElementFullSize(WMetaSeek,0);
    if (EBML_ElementRender(RSegmentInfo,Output,0,0,1,NULL,0)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the Segment Info\r\n"));
        Result = -11;
        goto exit;
    }
    SegmentSize += EBML_ElementFullSize(RSegmentInfo,0);
    if (RTrackInfo)
    {
        if (EBML_ElementRender(RTrackInfo,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Segment Info\r\n"));
            Result = -12;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(RTrackInfo,0);
    }
    if (RChapters)
    {
        if (EBML_ElementRender(RChapters,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Chapters\r\n"));
            Result = -13;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(RChapters,0);
    }
    if (RTags)
    {
        if (EBML_ElementRender(RTags,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Tags\r\n"));
            Result = -14;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(RTags,0);
    }
    if (RCues)
    {
        if (EBML_ElementRender(RCues,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Cues\r\n"));
            Result = -15;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(RCues,0);
    }

    //  Write the Clusters
    for (Cluster = ARRAYBEGIN(RClusters,ebml_element*);Cluster != ARRAYEND(RClusters,ebml_element*); ++Cluster)
    {
        ShowProgress(*Cluster,RSegment,3);
        WriteCluster(*Cluster,Output,Input);
        SegmentSize += EBML_ElementFullSize(*Cluster,0);
    }
    EndProgress(RSegment,3);

    //  Write the Attachments
    if (RAttachments)
    {
        ReduceSize(RAttachments);
        if (EBML_ElementRender(RAttachments,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Attachments\r\n"));
            Result = -17;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(RAttachments,0);
    }

    // update the WSegment size
    if (SegmentSize > WSegment->DataSize)
    {
        if (EBML_CodedSizeLength(SegmentSize,WSegment->SizeLength,0) != EBML_CodedSizeLength(SegmentSize,WSegment->SizeLength,0))
        {
            TextPrintf(StdErr,T("The segment written is much bigger than the original %ld vs %ld !\r\n"),(long)SegmentSize,(long)WSegment->DataSize);
            Result = -20;
            goto exit;
        }
        TextPrintf(StdErr,T("The segment written is bigger than the original %ld vs %ld !\r\n"),(long)SegmentSize,(long)WSegment->DataSize);
    }
    WSegment->DataSize = SegmentSize;
    Stream_Seek(Output,WSegment->ElementPosition,SEEK_SET);
    if (EBML_ElementRenderHead(WSegment, Output, 0, NULL)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the final Segment size !\r\n"));
        Result = -21;
        goto exit;
    }

    // update the Meta Seek
    MetaSeekUpdate(WMetaSeek);
    Stream_Seek(Output,WMetaSeek->ElementPosition,SEEK_SET);
    if (EBML_ElementRender(WMetaSeek,Output,0,0,1,&MetaSeekAfter,0)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the final Seek Head\r\n"));
        Result = -22;
        goto exit;
    }
    if (MetaSeekBefore != MetaSeekAfter)
    {
        TextPrintf(StdErr,T("The final Seek Head size has changed %ld vs %ld !\r\n"),(long)MetaSeekBefore,(long)MetaSeekAfter);
        Result = -23;
        goto exit;
    }
    TextPrintf(StdErr,T("Finished cleaning & optimizing \"%s\"\r\n"),Path);

exit:
    if (WSegment)
        NodeDelete((node*)WSegment);

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
    if (Output)
        StreamClose(Output);

    // EBML & Matroska ending
    MATROSKA_Done((nodecontext*)&p);

    // Core-C ending
	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);

    return Result;
}
