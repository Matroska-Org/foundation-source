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
#ifndef CONFIG_EBML_UNICODE
#define CONFIG_EBML_UNICODE
#endif
#include "matroska/matroska.h"

/*!
 * \todo discards tracks that has the same UID
 * \todo error when an unknown codec (for the profile) is found (option to turn into a warning) (loose mode)
 * \todo compute the segment duration based on audio (when it's not set)
 * \todo remuxing: turn a BlockGroup into a SimpleBlock in v2 profiles and when it makes sense (duration = default track duration) (optimize mode)
 * \todo remuxing: repack audio frames using lacing (no longer than the matching video frame ?) (optimize mode)
 * \todo compute the track default duration (when it's not set or not optimal) (optimize mode)
 * \todo add a batch mode to treat more than one file at once
 * \todo get the file name/list to treat from stdin too
 * \todo add an option to remove the original file
 * \todo add an option to rename the output to the original file
 * \todo add an option to show the size gained/added compared to the original
 * \todo support the japanese translation
 *
 * less important:
 * \todo (optionally) change the Segment UID (when key parts are altered/added)
 * \todo force keeping some forbidden elements in a profile (chapters/tags in 'webm') (loose mode)
 * \todo support for updating/writing the CRC32
 * \todo add an option to cut short a file with a start/end
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

typedef struct track_info
{
	bool_t IsLaced;
//    bool_t NeedsReading;

} track_info;

static const tchar_t *GetProfileName(size_t ProfileNum)
{
static const tchar_t *Profile[7] = {T("unknown"), T("matroska v1"), T("matroska v2"), T("unused webm"), T("webm"), T("matroska+DivX"), T("unused matroska+DivX") };
	switch (ProfileNum)
	{
	case PROFILE_MATROSKA_V1: return Profile[1];
	case PROFILE_MATROSKA_V2: return Profile[2];
	case PROFILE_WEBM_V2:     return Profile[4];
	case PROFILE_DIVX_V1:     return Profile[5];
	default:                  return Profile[0];
	}
}

static int GetProfileId(int Profile)
{
	switch (Profile)
	{
	case PROFILE_MATROSKA_V1: return 1;
	case PROFILE_MATROSKA_V2: return 2;
	case PROFILE_WEBM_V2:     return 4;
	case PROFILE_DIVX_V1:     return 5;
	default:                  return 0;
	}
}

static int DocVersion = 1;
static int SrcProfile = 0, DstProfile = 0;
static textwriter *StdErr = NULL;
static size_t ExtraSizeDiff = 0;
static bool_t Quiet = 0;

static void ReduceSize(ebml_element *Element)
{
    Element->SizeLength = 0; // reset
    if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    {
        ebml_element *i, *j;
		const ebml_semantic *s;
		EBML_MasterMandatory(Element,1);

        for (i=EBML_MasterChildren(Element);i;i=j)
		{
			j = EBML_MasterNext(i);
			if (Node_IsPartOf(i, EBML_VOID_CLASS))
			{
				NodeDelete((node*)i);
				continue;
			}
			else if (Node_IsPartOf(i, EBML_DUMMY_ID))
			{
				NodeDelete((node*)i);
				continue;
			}
			else if (i->Context->Id == EBML_ContextEbmlCrc32.Id)
			{
				NodeDelete((node*)i);
				continue;
			}
            ReduceSize(i);
		}

		for (i=EBML_MasterChildren(Element);i;i=i?EBML_MasterNext(i):NULL)
		{
			for (s=Element->Context->Semantic; s->eClass; ++s)
			{
				if (s->eClass->Id == i->Context->Id)
				{
			        if (Node_IsPartOf(i,EBML_MASTER_CLASS) && !EBML_MasterCheckMandatory(i,0))
			        {
						// if it's not unique we can remove it
						if (!s->Unique)
						{
							tchar_t IdString[MAXPATH];
							Node_FromStr(i,IdString,TSIZEOF(IdString),s->eClass->ElementName);
							TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" is missing mandatory elements, skipping\r\n"),IdString,i->ElementPosition);
							EBML_MasterRemove(Element,i);
							NodeDelete((node*)i);
							i=EBML_MasterChildren(Element);
							break;
						}
                    }
                    if ((s->DisabledProfile & DstProfile)!=0)
                    {
						tchar_t IdString[MAXPATH];
						Node_FromStr(i,IdString,TSIZEOF(IdString),s->eClass->ElementName);
						TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" is not part of profile '%s', skipping\r\n"),IdString,i->ElementPosition,GetProfileName(DstProfile));
						EBML_MasterRemove(Element,i);
						NodeDelete((node*)i);
						i=EBML_MasterChildren(Element);
						break;
                    }
				}
			}
		}
	}
}

static err_t ReadClusterData(ebml_element *Cluster, stream *Input)
{
    err_t Result = ERR_NONE;
    ebml_element *Block, *GBlock, *NextBlock;
    // read all the Block/SimpleBlock data
    for (Block = EBML_MasterChildren(Cluster);Result==ERR_NONE && Block;Block=NextBlock)
    {
        NextBlock = EBML_MasterNext(Block);
        if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
                {
                    if ((Result = MATROSKA_BlockReadData((matroska_block*)GBlock, Input))!=ERR_NONE)
                        NodeDelete((node*)Block);
                    break;
                }
            }
        }
        else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
        {
            if ((Result = MATROSKA_BlockReadData((matroska_block*)Block, Input))!=ERR_NONE)
                NodeDelete((node*)Block);
        }
    }
    return Result;
}

static err_t UnReadClusterData(ebml_element *Cluster)
{
    err_t Result = ERR_NONE;
    ebml_element *Block, *GBlock;
    for (Block = EBML_MasterChildren(Cluster);Result==ERR_NONE && Block;Block=EBML_MasterNext(Block))
    {
        if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
                {
                    Result = MATROSKA_BlockReleaseData((matroska_block*)GBlock);
                    break;
                }
            }
        }
        else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
            Result = MATROSKA_BlockReleaseData((matroska_block*)Block);
    }
    return Result;
}

static void SetClusterPrevSize(array *Clusters, stream *Input)
{
    ebml_element **Cluster, *Elt, *Elt2;
    filepos_t ClusterSize = INVALID_FILEPOS_T;

    // Write the Cluster PrevSize
    for (Cluster = ARRAYBEGIN(*Clusters,ebml_element*);Cluster != ARRAYEND(*Clusters,ebml_element*); ++Cluster)
    {
        if (Input!=NULL)
            ReadClusterData(*Cluster,Input);

        if (ClusterSize != INVALID_FILEPOS_T)
        {
            Elt = EBML_MasterFindFirstElt(*Cluster, &MATROSKA_ContextClusterPrevSize, 1, 1);
            if (Elt)
            {
                EBML_IntegerSetValue((ebml_integer*)Elt, ClusterSize);
                Elt2 = EBML_MasterFindFirstElt(*Cluster, &MATROSKA_ContextClusterTimecode, 0, 0);
                if (Elt2)
                    NodeTree_SetParent(Elt,*Cluster,NodeTree_Next(Elt2));
                ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                EBML_ElementUpdateSize(*Cluster,0,1);
            }
        }
        ClusterSize = EBML_ElementFullSize(*Cluster,0);

        if (Input!=NULL)
            UnReadClusterData(*Cluster);
    }
}

static void SettleClustersWithCues(array *Clusters, filepos_t ClusterStart, ebml_element *Cues, ebml_element *Segment, bool_t SafeClusters, stream *Input)
{
    ebml_element **Cluster, *Elt, *Elt2;
    ebml_element *Cue;
    filepos_t OriginalSize = Cues->DataSize;
    filepos_t ClusterPos = ClusterStart + EBML_ElementFullSize(Cues,0);
    filepos_t ClusterSize = INVALID_FILEPOS_T;

    // reposition all the Clusters
    for (Cluster=ARRAYBEGIN(*Clusters,ebml_element*);Cluster!=ARRAYEND(*Clusters,ebml_element*);++Cluster)
    {
        if (Input!=NULL)
            ReadClusterData(*Cluster,Input);

        (*Cluster)->ElementPosition = ClusterPos;
        if (SafeClusters)
        {
            Elt = NULL;
            if (ClusterSize != INVALID_FILEPOS_T)
            {
                Elt = EBML_MasterFindFirstElt(*Cluster, &MATROSKA_ContextClusterPrevSize, 1, 1);
                if (Elt)
                {
                    EBML_IntegerSetValue((ebml_integer*)Elt, ClusterSize);
                    Elt2 = EBML_MasterFindFirstElt(*Cluster, &MATROSKA_ContextClusterTimecode, 0, 0);
                    if (Elt2)
                        NodeTree_SetParent(Elt,*Cluster,NodeTree_Next(Elt2)); // make sure the PrevSize is just after the ClusterTimecode
                    ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                }
            }

            // output the Cluster position as well
            if (Elt)
                Elt2 = Elt; // make sure the Cluster Position is just after the PrevSize
            else
                Elt2 = EBML_MasterFindFirstElt(*Cluster, &MATROSKA_ContextClusterTimecode, 0, 0); // make sure the Cluster Position is just after the ClusterTimecode
            if (Elt2)
            {
                Elt = EBML_MasterFindFirstElt(*Cluster, &MATROSKA_ContextClusterPosition, 1, 1);
                if (Elt)
                {
                    EBML_IntegerSetValue((ebml_integer*)Elt, ClusterPos - EBML_ElementPositionData(Segment));
                    NodeTree_SetParent(Elt,*Cluster,NodeTree_Next(Elt2));
                    ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                }
            }
        }
        EBML_ElementUpdateSize(*Cluster,0,0);
        ClusterSize = EBML_ElementFullSize(*Cluster,0);
        ClusterPos += ClusterSize;

        if (Input!=NULL)
            UnReadClusterData(*Cluster);
    }

    // reevaluate the size needed for the Cues
    for (Cue=EBML_MasterChildren(Cues);Cue;Cue=EBML_MasterNext(Cue))
        MATROSKA_CuePointUpdate((matroska_cuepoint*)Cue, Segment);
    ClusterPos = EBML_ElementUpdateSize(Cues,0,0);
    if (ClusterPos != OriginalSize)
        SettleClustersWithCues(Clusters,ClusterStart,Cues,Segment, SafeClusters, Input);

}

static void ShowProgress(const ebml_element *RCluster, const ebml_element *RSegment, int phase, int max)
{
    if (!Quiet)
        TextPrintf(StdErr,T("Progress %d/%d: %3d%%\r"),phase,max,Scale32(100,RCluster->ElementPosition,RSegment->DataSize)+1);
}

static void EndProgress(const ebml_element *RSegment, int phase, int max)
{
    if (!Quiet)
        TextPrintf(StdErr,T("Progress %d/%d: 100%%\r\n"),phase,max);
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
                ShowProgress((ebml_element*)(*Cluster),RSegment,2,3);
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
            ShowProgress((ebml_element*)(*Cluster),RSegment,2,3);
            return Cluster;
        }
    }

    TextPrintf(StdErr,T("Could not find the matching block for timecode %0.3f s\r\n"),CueTimecode/1000000000.0);
    return NULL;
}

static int LinkClusters(array *Clusters, ebml_element *RSegmentInfo, ebml_element *Tracks, int DstProfile, array *WTracks)
{
    matroska_cluster **Cluster;
	ebml_element *Block, *GBlock, *BlockTrack, *Type;
    int BlockNum;

	// find out if the Clusters use forbidden features for that DstProfile
	if (DstProfile == PROFILE_MATROSKA_V1 || DstProfile == PROFILE_DIVX_V1)
	{
		for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
		{
			for (Block = EBML_MasterChildren(*Cluster);Block;Block=EBML_MasterNext(Block))
			{
				if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
                {
					TextPrintf(StdErr,T("Using SimpleBlock in profile '%s' try \"--doctype %d\"\r\n"),GetProfileName(DstProfile),GetProfileId(PROFILE_MATROSKA_V2));
					return -32;
				}
			}
		}
	}

	// link each Block/SimpleBlock with its Track and SegmentInfo
	for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
	{
		MATROSKA_LinkClusterBlocks(*Cluster, RSegmentInfo, Tracks, 0);
		ReduceSize((ebml_element*)*Cluster);
	}

    // mark all the audio/subtitle tracks as keyframes
	for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
	{
		for (Block = EBML_MasterChildren(*Cluster);Block;Block=EBML_MasterNext(Block))
		{
			if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
			{
				GBlock = EBML_MasterFindFirstElt(Block, &MATROSKA_ContextClusterBlock, 0, 0);
				if (GBlock)
				{
					BlockTrack = MATROSKA_BlockReadTrack((matroska_block*)GBlock);
                    if (!BlockTrack) continue;
                    Type = EBML_MasterFindFirstElt(BlockTrack,&MATROSKA_ContextTrackType,0,0);
                    if (!Type) continue;
                    if (EBML_IntegerValue(Type)==TRACK_TYPE_AUDIO || EBML_IntegerValue(Type)==TRACK_TYPE_SUBTITLE)
                    {
                        MATROSKA_BlockSetKeyframe((matroska_block*)GBlock,1);
						MATROSKA_BlockSetDiscardable((matroska_block*)GBlock,0);
                    }
                    BlockNum = MATROSKA_BlockTrackNum((matroska_block*)GBlock);
                    if (MATROSKA_BlockGetFrameCount((matroska_block*)GBlock)>1)
                        ARRAYBEGIN(*WTracks,track_info)[BlockNum].IsLaced = 1;
				}
			}
			else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
			{
				BlockTrack = MATROSKA_BlockReadTrack((matroska_block *)Block);
                if (!BlockTrack) continue;
                Type = EBML_MasterFindFirstElt(BlockTrack,&MATROSKA_ContextTrackType,0,0);
                if (!Type) continue;
                if (EBML_IntegerValue(Type)==TRACK_TYPE_AUDIO || EBML_IntegerValue(Type)==TRACK_TYPE_SUBTITLE)
                {
                    MATROSKA_BlockSetKeyframe((matroska_block*)Block,1);
                    MATROSKA_BlockSetDiscardable((matroska_block*)Block,0);
                }
                BlockNum = MATROSKA_BlockTrackNum((matroska_block*)Block);
                if (MATROSKA_BlockGetFrameCount((matroska_block*)Block)>1)
                    ARRAYBEGIN(*WTracks,track_info)[BlockNum].IsLaced = 1;
			}
		}
    }
	return 0;
}

static void OptimizeCues(ebml_element *Cues, array *Clusters, ebml_element *RSegmentInfo, filepos_t StartPos, ebml_element *WSegment, const ebml_element *RSegment, bool_t ReLink, bool_t SafeClusters, stream *Input)
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
		EndProgress(RSegment,2,3);
	}

    // sort the Cues
    MATROSKA_CuesSort(Cues);

    SettleClustersWithCues(Clusters,StartPos,Cues,WSegment,SafeClusters, Input);
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
                TextPrintf(StdErr,T("EBML Read version %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue(SubElement));
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
                TextPrintf(StdErr,T("EBML Max ID Length %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue(SubElement));
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
                TextPrintf(StdErr,T("EBML Max Coded Size %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue(SubElement));
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
                    SrcProfile = PROFILE_MATROSKA_V1;
                else if (tcscmp(String,T("webm"))==0)
                    SrcProfile = PROFILE_WEBM_V2;
                else
                {
                    TextPrintf(StdErr,T("EBML DocType %s not supported\r\n"),EBML_IntegerValue(SubElement));
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
                TextPrintf(StdErr,T("EBML Read version %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue(SubElement));
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

static void WriteCluster(ebml_element *Cluster, stream *Output, stream *Input, bool_t Live, filepos_t PrevSize)
{
    filepos_t IntendedPosition = Cluster->ElementPosition;
    ebml_element *Elt;

    ReadClusterData(Cluster, Input);

    EBML_ElementRender(Cluster,Output,0,0,1,NULL,0);

    UnReadClusterData(Cluster);

    if (!Live && Cluster->ElementPosition != IntendedPosition)
        TextPrintf(StdErr,T("Failed to write a Cluster at the required position %") TPRId64 T(" vs %") TPRId64 T("\r\n"), Cluster->ElementPosition,IntendedPosition);
    if (!Live && PrevSize!=INVALID_FILEPOS_T)
    {
        Elt = EBML_MasterFindFirstElt(Cluster, &MATROSKA_ContextClusterPrevSize, 1, 1);
        if (Elt && PrevSize!=EBML_IntegerValue(Elt))
            TextPrintf(StdErr,T("The PrevSize of the Cluster at the position %") TPRId64 T(" is wrong: %") TPRId64 T(" vs %") TPRId64 T("\r\n"), Cluster->ElementPosition,EBML_IntegerValue(Elt),PrevSize);
    }
}

static void MetaSeekUpdate(ebml_element *SeekHead)
{
    matroska_seekpoint *SeekPoint;
    for (SeekPoint=(matroska_seekpoint*)EBML_MasterChildren(SeekHead); SeekPoint; SeekPoint=(matroska_seekpoint*)EBML_MasterNext(SeekPoint))
        MATROSKA_MetaSeekUpdate(SeekPoint);
    EBML_ElementUpdateSize(SeekHead,0,0);
}

static ebml_element *GetMainTrack(ebml_element *Tracks, array *TrackOrder)
{
	ebml_element *Track, *Elt;
	int64_t TrackNum = -1;
	size_t *order;
	
	if (TrackOrder)
		order = ARRAYBEGIN(*TrackOrder,size_t);
	else
		order = NULL;

	// find the video (first) track
	for (Track = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackEntry,0,0); Track; Track=EBML_MasterFindNextElt(Tracks,Track,0,0))
	{
		Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackType,0,0);
		if (EBML_IntegerValue(Elt) == TRACK_TYPE_VIDEO)
		{
			TrackNum = EBML_IntegerValue(EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0));
			break;
		}
	}

	if (!Track)
	{
		// no video track found, look for an audio track
		for (Track = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackEntry,0,0); Track; Track=EBML_MasterFindNextElt(Tracks,Track,0,0))
		{
			Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackType,0,0);
			if (EBML_IntegerValue(Elt) == TRACK_TYPE_AUDIO)
			{
				TrackNum = EBML_IntegerValue(EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0));
				break;
			}
		}
	}

	if (order)
	{
		for (Elt = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackEntry,0,0); Elt; Elt=EBML_MasterFindNextElt(Tracks,Elt,0,0))
		{
			if (Elt!=Track)
				*order++ = (size_t)EBML_IntegerValue(EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackNumber,0,0));
		}

		if (Track)
			*order++ = (size_t)EBML_IntegerValue(EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0));
	}

	if (!Track)
		TextPrintf(StdErr,T("Could not find an audio or video track to use as main track"));

	return Track;
}

static bool_t GenerateCueEntries(ebml_element *Cues, array *Clusters, ebml_element *Tracks, ebml_element *WSegmentInfo, ebml_element *RSegment)
{
	ebml_element *Track, *Elt;
	matroska_block *Block;
	ebml_element **Cluster;
	matroska_cuepoint *CuePoint;
	int64_t TrackNum;
	timecode_t PrevTimecode = INVALID_TIMECODE_T, BlockTimecode;

	Track = GetMainTrack(Tracks, NULL);
	if (!Track)
	{
		TextPrintf(StdErr,T("Could not generate the Cue entries"));
		return 0;
	}

	Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
	assert(Elt!=NULL);
	if (Elt)
		TrackNum = EBML_IntegerValue(Elt);

	// find all the keyframes
	for (Cluster = ARRAYBEGIN(*Clusters,ebml_element*);Cluster != ARRAYEND(*Clusters,ebml_element*); ++Cluster)
	{
		MATROSKA_LinkClusterWriteSegmentInfo((matroska_cluster*)*Cluster,WSegmentInfo);
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
					TextPrintf(StdErr,T("Failed to create a new CuePoint ! out of memory ?\r\n"));
					return 0;
				}
				MATROSKA_LinkCueSegmentInfo(CuePoint,WSegmentInfo);
				MATROSKA_LinkCuePointBlock(CuePoint,Block);
				MATROSKA_CuePointUpdate(CuePoint,RSegment);

				PrevTimecode = BlockTimecode;

				break; // one Cues per Cluster is enough
			}
		}
	}

	if (!EBML_MasterChildren(Cues))
	{
		TextPrintf(StdErr,T("Failed to create the Cue entries, no Block found\r\n"));
		return 0;
	}

	EBML_ElementUpdateSize(Cues,0,0);
	return 1;
}

static int TimcodeCmp(const void* Param, const timecode_t *a, const timecode_t *b)
{
	if (*a == *b)
		return 0;
	if (*a > *b)
		return 1;
	return -1;
}

static int64_t gcd(int64_t a, int64_t b)
{
    for (;;)
    {
        int64_t c = a % b;
        if(!c) return b;
        a = b;
        b = c;
    }
}

static void CleanCropValues(ebml_element *Tracks, int64_t Width, int64_t Height)
{
    ebml_element *Left,*Right,*Top,*Bottom;
    Left = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackVideoPixelCropLeft,1,1);
    Right = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackVideoPixelCropRight,1,1);
    Top = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackVideoPixelCropTop,1,1);
    Bottom = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackVideoPixelCropBottom,1,1);
    if (EBML_IntegerValue(Top)+EBML_IntegerValue(Bottom) >= Height || EBML_IntegerValue(Left)+EBML_IntegerValue(Right) >= Width)
    {
        // invalid crop, remove the values
        NodeDelete((node*)Left);
        NodeDelete((node*)Right);
        NodeDelete((node*)Top);
        NodeDelete((node*)Bottom);
    }
    else
    {
        if (EBML_IntegerValue(Left)==0)   NodeDelete((node*)Left);
        if (EBML_IntegerValue(Right)==0)  NodeDelete((node*)Right);
        if (EBML_IntegerValue(Top)==0)    NodeDelete((node*)Top);
        if (EBML_IntegerValue(Bottom)==0) NodeDelete((node*)Bottom);
    }
}

static int CleanTracks(ebml_element *Tracks, int Profile, ebml_element *RAttachments)
{
    ebml_element *Track, *CurTrack, *Elt, *Elt2, *DisplayW, *DisplayH;
    int TrackType, TrackNum, Width, Height;
    tchar_t CodecID[MAXPATH];
    
    for (Track = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackEntry,0,0); Track;)
    {
		CurTrack = Track;
		Track = EBML_MasterNext(Track);
		Elt = EBML_MasterFindFirstElt(CurTrack,&MATROSKA_ContextTrackNumber,0,0);
		if (!Elt)
		{
			TextPrintf(StdErr,T("The track at %") TPRId64 T(" has no number set!\r\n"),CurTrack->ElementPosition);
			NodeDelete((node*)CurTrack);
			continue;
		}
		TrackNum = (int)EBML_IntegerValue(Elt);

		Elt = EBML_MasterFindFirstElt(CurTrack,&MATROSKA_ContextTrackCodecID,0,0);
		if (!Elt)
		{
			TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has no CodecID set!\r\n"), TrackNum,CurTrack->ElementPosition);
			NodeDelete((node*)CurTrack);
			continue;
		}

		Elt = EBML_MasterFindFirstElt(CurTrack,&MATROSKA_ContextTrackType,0,0);
		if (!Elt)
		{
			TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has no type set!\r\n"), TrackNum,CurTrack->ElementPosition);
			NodeDelete((node*)CurTrack);
			continue;
		}
		
		if (Profile==PROFILE_WEBM_V2)
		{
	        // verify that we have only VP8 and Vorbis tracks
			TrackType = (int)EBML_IntegerValue(Elt);
			Elt = EBML_MasterFindFirstElt(CurTrack,&MATROSKA_ContextTrackCodecID,0,0);
			EBML_StringGet((ebml_string*)Elt,CodecID,TSIZEOF(CodecID));
			if (!(TrackType==TRACK_TYPE_VIDEO && tcsisame_ascii(CodecID,T("V_VP8")) || TrackType==TRACK_TYPE_AUDIO && tcsisame_ascii(CodecID,T("A_VORBIS"))))
			{
				TextPrintf(StdErr,T("Wrong codec '%s' for profile '%s' removing track %d\r\n"),CodecID,GetProfileName(Profile),TrackNum);
				NodeDelete((node*)CurTrack);
			}
		}

        // clean the aspect ratio
        Elt = EBML_MasterFindFirstElt(CurTrack,&MATROSKA_ContextTrackVideo,0,0);
        if (Elt)
        {
            Width = (int)EBML_IntegerValue(EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoPixelWidth,0,0));
            Height = (int)EBML_IntegerValue(EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoPixelHeight,0,0));
	        if (Width==0 || Height==0)
	        {
		        TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid pixel dimensions %dx%d!\r\n"), TrackNum,CurTrack->ElementPosition,Width,Height);
		        NodeDelete((node*)CurTrack);
		        continue;
	        }

            DisplayW = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoDisplayWidth,0,0);
            DisplayH = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoDisplayHeight,0,0);
            if (DisplayW || DisplayH)
            {
                Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoDisplayUnit, 0, 0);
                if (!Elt2 || EBML_IntegerValue(Elt2)==MATROSKA_DISPLAY_UNIT_PIXEL) // pixel AR
                {
                    if (!DisplayW)
                    {
                        if (EBML_IntegerValue(DisplayH)==Height)
                        {
                            NodeDelete((node*)DisplayH);
                            DisplayH = NULL; // we don't the display values, they are the same as the pixel ones
                        }
                        else
                        {
                            DisplayW = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoDisplayWidth,1,0);
                            EBML_IntegerSetValue((ebml_integer*)DisplayW,Width);
                        }
                    }
                    else if (EBML_IntegerValue(DisplayW)==0)
                    {
		                TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid display width %") TPRId64 T("!\r\n"), TrackNum,CurTrack->ElementPosition,EBML_IntegerValue(DisplayW));
		                NodeDelete((node*)CurTrack);
		                continue;
                    }
                    else if (!DisplayH)
                    {
                        if (EBML_IntegerValue(DisplayW)==Width)
                        {
                            NodeDelete((node*)DisplayW);
                            DisplayW = NULL; // we don't the display values, they are the same as the pixel ones
                        }
                        else
                        {
                            DisplayH = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoDisplayHeight,1,0);
                            EBML_IntegerSetValue((ebml_integer*)DisplayH,Height);
                        }
                    }
                    else if (EBML_IntegerValue(DisplayH)==0)
                    {
		                TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid display height %") TPRId64 T("!\r\n"), TrackNum,CurTrack->ElementPosition,EBML_IntegerValue(DisplayH));
		                NodeDelete((node*)CurTrack);
		                continue;
                    }

                    if (DisplayW && DisplayH)
                    {
                        if (EBML_IntegerValue(DisplayH) < Height && EBML_IntegerValue(DisplayW) < Width) // Haali's non-pixel shrinking
                        {
                            int64_t DW = EBML_IntegerValue(DisplayW);
							int64_t DH = EBML_IntegerValue(DisplayH);
                            int Serious = gcd(DW,DH)==1; // shrank as much as it was possible
                            if (8*DW <= Width && 8*DH <= Height)
                                ++Serious; // shrank too much compared to the original
                            else if (2*DW >= Width && 2*DH >= Height)
                                --Serious; // may be intentional

                            if (Serious) // doesn't seem correct as pixels
                            {
							    if (DW > DH)
							    {
								    EBML_IntegerSetValue((ebml_integer*)DisplayW,Scale64(Height,DW,DH));
								    EBML_IntegerSetValue((ebml_integer*)DisplayH,Height);
							    }
							    else
							    {
								    EBML_IntegerSetValue((ebml_integer*)DisplayH,Scale64(Width,DH,DW));
								    EBML_IntegerSetValue((ebml_integer*)DisplayW,Width);
							    }

							    // check if the AR is respected otherwise force it into a DAR
							    if (EBML_IntegerValue(DisplayW)*DH != EBML_IntegerValue(DisplayH)*DW)
							    {
								    Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoDisplayUnit, 1, 0);
								    if (Elt2)
								    {
									    EBML_IntegerSetValue((ebml_integer*)Elt2,MATROSKA_DISPLAY_UNIT_DAR);
									    EBML_IntegerSetValue((ebml_integer*)DisplayW,DW);
									    EBML_IntegerSetValue((ebml_integer*)DisplayH,DH);
								    }
							    }
                            }
                        }
                        if (DisplayH && EBML_IntegerValue(DisplayH) == Height)
                        {
                            NodeDelete((node*)DisplayH);
                            DisplayH = NULL;
                        }
                        if (DisplayW && EBML_IntegerValue(DisplayW) == Width)
                        {
                            NodeDelete((node*)DisplayW);
                            DisplayW = NULL;
                        }
                    }
                }
                Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackVideoDisplayUnit, 1, 1);
                if (EBML_IntegerValue(Elt2)==MATROSKA_DISPLAY_UNIT_DAR)
                    CleanCropValues(CurTrack, 0, 0);
                else
                    CleanCropValues(CurTrack, DisplayW?EBML_IntegerValue(DisplayW):Width, DisplayH?EBML_IntegerValue(DisplayH):Height);
            }
        }

        // clean the output sampling freq
        Elt = EBML_MasterFindFirstElt(CurTrack,&MATROSKA_ContextTrackAudio,0,0);
        if (Elt)
        {
            Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackAudioOutputSamplingFreq,0,0);
            if (Elt2)
            {
                DisplayH = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackAudioSamplingFreq,0,0);
                assert(DisplayH!=NULL);
                if (((ebml_float*)Elt2)->Value == ((ebml_float*)DisplayH)->Value)
                    NodeDelete((node*)Elt2);
            }
        }

        // clean the attachment links
        Elt = EBML_MasterFindFirstElt(CurTrack,&MATROSKA_ContextTrackAttachmentLink,0,0);
        while (Elt)
        {
            Elt2 = NULL;
            if (!RAttachments)
                Elt2 = Elt;
            else
            {
                Elt2 = EBML_MasterFindFirstElt(RAttachments,&MATROSKA_ContextAttachedFile,0,0);
                while (Elt2)
                {
                    DisplayH = EBML_MasterFindFirstElt(Elt2,&MATROSKA_ContextAttachedFileUID,0,0);
                    if (DisplayH && EBML_IntegerValue(DisplayH)==EBML_IntegerValue(Elt))
                        break;
                    Elt2 = EBML_MasterFindNextElt(RAttachments, Elt2,0,0);
                }
                if (!Elt2) // the attachment wasn't found, delete Elt
                    Elt2 = Elt;
                else
                    Elt2 = NULL;
            }

            Elt = EBML_MasterFindNextElt(CurTrack, Elt, 0, 0);
            if (Elt2)
                NodeDelete((node*)Elt2);
        }
    }
    
    if (EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackEntry,0,0)==NULL)
        return -19;

    // put the TrackNumber, TrackType and CodecId at the front of the Track elements
    for (Track = EBML_MasterFindFirstElt(Tracks,&MATROSKA_ContextTrackEntry,0,0); Track; Track = EBML_MasterNext(Track))
    {
        Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
        if (Elt)
            NodeTree_SetParent(Elt,Track,EBML_MasterChildren(Track));

        Elt2 = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackType,0,0);
        assert(Elt2!=NULL);
        NodeTree_SetParent(Elt2,Track,EBML_MasterNext(Elt));

        DisplayW = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackCodecID,0,0);
        if (DisplayW)
            NodeTree_SetParent(DisplayW,Track,EBML_MasterNext(Elt2));
    }

    return 0;
}

typedef struct block_info
{
	matroska_block *Block;
	timecode_t DecodeTime;
	size_t FrameStart;

} block_info;

#define TABLE_MARKER (uint8_t*)1

static void InitCommonHeader(array *TrackHeader)
{
    // special mark to tell the header has not been used yet
    TrackHeader->_Begin = TABLE_MARKER;
}

static bool_t BlockIsCompressible(const matroska_block *Block)
{
    ebml_element *Track = MATROSKA_BlockReadTrack(Block);
    if (Track)
    {
        ebml_element *Elt = EBML_MasterFindFirstElt(Track, &MATROSKA_ContextTrackEncodings, 0, 0);
        if (Elt)
        {
            Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncoding, 0, 0);
            if (EBML_MasterChildren(Elt))
            {
                if (EBML_MasterNext(Elt))
                    return 1; // we don't support cascased encryption/compression

                Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompression, 0, 0);
                if (!Elt)
                    return 1; // we don't support encryption

                Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionAlgo, 1, 1);
                if (EBML_IntegerValue(Elt)==MATROSKA_BLOCK_COMPR_ZLIB)
                    return 1;
            }
        }
    }
    return 0;
}

static void ShrinkCommonHeader(array *TrackHeader, matroska_block *Block, stream *Input)
{
    size_t Frame,FrameCount,EqualData;
    matroska_frame FrameData;

    if (TrackHeader->_Begin != TABLE_MARKER && ARRAYCOUNT(*TrackHeader,uint8_t)==0)
        return;
    if (MATROSKA_BlockReadData(Block,Input)!=ERR_NONE)
        return;

    if (BlockIsCompressible(Block))
        return;

    FrameCount = MATROSKA_BlockGetFrameCount(Block);
    Frame = 0;
    if (FrameCount && TrackHeader->_Begin == TABLE_MARKER)
    {
        // use the first frame as the reference
        MATROSKA_BlockGetFrame(Block,Frame,&FrameData,1);
        TrackHeader->_Begin = NULL;
        ArrayAppend(TrackHeader,FrameData.Data,FrameData.Size,0);
        Frame = 1;
    }
    for (;Frame<FrameCount;++Frame)
    {
        MATROSKA_BlockGetFrame(Block,Frame,&FrameData,1);
        EqualData = 0;
        while (EqualData < FrameData.Size && EqualData < ARRAYCOUNT(*TrackHeader,uint8_t))
        {
            if (ARRAYBEGIN(*TrackHeader,uint8_t)[EqualData] == FrameData.Data[EqualData])
                ++EqualData;
            else
                break;
        }
        if (EqualData != ARRAYCOUNT(*TrackHeader,uint8_t))
            ArrayShrink(TrackHeader,ARRAYCOUNT(*TrackHeader,uint8_t)-EqualData);
        if (ARRAYCOUNT(*TrackHeader,uint8_t)==0)
            break;
    }
    MATROSKA_BlockReleaseData(Block);
}

static void ClearCommonHeader(array *TrackHeader)
{
    if (TrackHeader->_Begin == TABLE_MARKER)
        TrackHeader->_Begin = NULL;
}

int main(int argc, const char *argv[])
{
    int i,Result = 0;
    int ShowUsage = 0;
    int ShowVersion = 0;
    parsercontext p;
    textwriter _StdErr;
    stream *Input = NULL,*Output = NULL;
    tchar_t Path[MAXPATHFULL];
    tchar_t String[MAXLINE],Original[MAXLINE],*s;
    ebml_element *EbmlHead = NULL, *RSegment = NULL, *RLevel1 = NULL, **Cluster;
    ebml_element *RSegmentInfo = NULL, *RTrackInfo = NULL, *RChapters = NULL, *RTags = NULL, *RCues = NULL, *RAttachments = NULL;
    ebml_element *WSegment = NULL, *WMetaSeek = NULL, *WSegmentInfo = NULL, *WTrackInfo = NULL, *Elt, *Elt2;
    matroska_seekpoint *WSeekPoint = NULL, *WSeekPointTags = NULL;
    ebml_string *LibName, *AppName;
    array RClusters, WClusters, *Clusters, WTracks;
    ebml_parser_context RContext;
    ebml_parser_context RSegmentContext;
    int UpperElement;
    filepos_t MetaSeekBefore, MetaSeekAfter;
    filepos_t NextPos, SegmentSize = 0, ClusterSize;
	bool_t KeepCues = 0, Remux = 0, CuesCreated = 0, Live = 0, Unsafe = 0, Optimize = 0, UnOptimize = 0, ClustersNeedRead = 0;
    int InputPathIndex = 2;
	int64_t TimeCodeScale = 0;
    size_t MaxTrackNum = 0;
    array TrackMaxHeader; // array of uint8_t (max common header)

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
	StdAfx_Init((nodemodule*)&p);
    ProjectSettings((nodecontext*)&p);

    // EBML & Matroska Init
    MATROSKA_Init((nodecontext*)&p);

    ArrayInit(&RClusters);
    ArrayInit(&WClusters);
    ArrayInit(&WTracks);
	ArrayInit(&TrackMaxHeader);
	Clusters = &RClusters;

    StdErr = &_StdErr;
    memset(StdErr,0,sizeof(_StdErr));
    StdErr->Stream = (stream*)NodeSingleton(&p,STDERR_ID);

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[0]);
    SplitPath(Path,NULL,0,String,TSIZEOF(String),NULL,0);
    UnOptimize = tcsisame_ascii(String,T("mkWDclean"));
    if (UnOptimize)
        TextPrintf(StdErr,T("Running special mkWDclean mode, please fix your player instead of valid Matroska files\r\n"),Path);

	for (i=1;i<argc;++i)
	{
	    Node_FromStr(&p,Path,TSIZEOF(Path),argv[i]);
		if (tcsisame_ascii(Path,T("--keep-cues"))) KeepCues = 1;
		else if (tcsisame_ascii(Path,T("--remux"))) Remux = 1;
		else if (tcsisame_ascii(Path,T("--live"))) Live = 1;
		else if (tcsisame_ascii(Path,T("--doctype")) && i+1<argc-2)
		{
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
			if (tcsisame_ascii(Path,T("1")))
				DstProfile = PROFILE_MATROSKA_V1;
			else if (tcsisame_ascii(Path,T("2")))
				DstProfile = PROFILE_MATROSKA_V2;
			else if (tcsisame_ascii(Path,T("4")))
				DstProfile = PROFILE_WEBM_V2;
			else if (tcsisame_ascii(Path,T("5")))
				DstProfile = PROFILE_DIVX_V1;
			else
			{
		        TextPrintf(StdErr,T("Unknown doctype %s\r\n"),Path);
				Result = -8;
				goto exit;
			}
		}
		else if (tcsisame_ascii(Path,T("--timecodescale")) && i+1<argc-2)
		{
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
			TimeCodeScale = StringToInt(Path,0);
		}
		else if (tcsisame_ascii(Path,T("--unsafe"))) Unsafe = 1;
		else if (tcsisame_ascii(Path,T("--optimize"))) Optimize = 1;
		else if (tcsisame_ascii(Path,T("--no-optimize"))) UnOptimize = 1;
		else if (tcsisame_ascii(Path,T("--quiet"))) Quiet = 1;
		else if (tcsisame_ascii(Path,T("--version"))) ShowVersion = 1;
        else if (tcsisame_ascii(Path,T("--help"))) {ShowVersion = 1; ShowUsage = 1;}
		else if (i<argc-2) TextPrintf(StdErr,T("Unknown parameter '%s'\r\n"),Path);
	}
    
    if (argc > 1)
    {
        if (argc==2 || (argv[argc-2][0]=='-' && argv[argc-2][1]=='-'))
            if (argv[argc-1][0]!='-' || argv[argc-1][1]!='-')
                InputPathIndex = 1;
    }

    if (argc < (1+InputPathIndex) || ShowVersion)
    {
        TextWrite(StdErr,T("mkclean v") PROJECT_VERSION T(", Copyright (c) 2010 Matroska Foundation\r\n"));
        if (argc < 2 || ShowUsage)
        {
            TextWrite(StdErr,T("Usage: mkclean [options] <matroska_src> [matroska_dst]\r\n"));
		    TextWrite(StdErr,T("Options:\r\n"));
		    TextWrite(StdErr,T("  --keep-cues   keep the original Cues content and move it to the front\r\n"));
		    TextWrite(StdErr,T("  --remux       redo the Clusters layout\r\n"));
		    TextWrite(StdErr,T("  --doctype <v> force the doctype version\r\n"));
		    TextWrite(StdErr,T("    1: 'matroska' v1\r\n"));
		    TextWrite(StdErr,T("    2: 'matroska' v2\r\n"));
		    TextWrite(StdErr,T("    4: 'webm'\r\n"));
		    TextWrite(StdErr,T("    5: 'matroska' v1 with DivX extensions\r\n"));
		    TextWrite(StdErr,T("  --live        the output file resembles a live stream\r\n"));
		    TextWrite(StdErr,T("  --timecodescale <v> force the global TimecodeScale to <v> (1000000 is a good value)\r\n"));
		    TextWrite(StdErr,T("  --unsafe      don't output elements that are used for file recovery (saves more space)\r\n"));
		    TextWrite(StdErr,T("  --optimize    use all possible optimization for the output file\r\n"));
		    TextWrite(StdErr,T("  --no-optimize disable some optimization for the output file\r\n"));
		    TextWrite(StdErr,T("  --quiet       only output errors\r\n"));
            TextWrite(StdErr,T("  --version     show the version of mkvalidator\r\n"));
            TextWrite(StdErr,T("  --help        show this screen\r\n"));
        }
        Result = -1;
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-InputPathIndex]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Result = -2;
        goto exit;
    }

    if (InputPathIndex==1)
    {
        tchar_t Ext[MAXDATA];
        SplitPath(Path,Original,TSIZEOF(Original),String,TSIZEOF(String),Ext,TSIZEOF(Ext));
        if (!Original[0])
            Path[0] = 0;
        else
        {
            tcscpy_s(Path,TSIZEOF(Path),Original);
            AddPathDelimiter(Path,TSIZEOF(Path));
        }
        if (Ext[0])
            stcatprintf_s(Path,TSIZEOF(Path),T("clean.%s.%s"),String,Ext);
        else
            stcatprintf_s(Path,TSIZEOF(Path),T("clean.%s"),String);
    }
    else
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
    EbmlHead = EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
    if (!EbmlHead || EbmlHead->Context->Id != EBML_ContextHead.Id)
    {
        TextWrite(StdErr,T("EBML head not found! Are you sure it's a matroska/webm file?\r\n"));
        Result = -4;
        goto exit;
    }

    RSegment = CheckMatroskaHead(EbmlHead,&RContext,Input);
    if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==2)
        SrcProfile = PROFILE_MATROSKA_V2;

	if (!DstProfile)
		DstProfile = SrcProfile;

	if (DstProfile==PROFILE_MATROSKA_V2 || DstProfile==PROFILE_WEBM_V2)
		DocVersion=2;

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
        ShowProgress(RLevel1,RSegment,1,Unsafe?3:2);
        if (RLevel1->Context->Id == MATROSKA_ContextSegmentInfo.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
                RSegmentInfo = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextTracks.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
                RTrackInfo = RLevel1;
        }
        else if (!Live && RLevel1->Context->Id == MATROSKA_ContextChapters.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
                RChapters = RLevel1;
        }
        else if (!Live && RLevel1->Context->Id == MATROSKA_ContextTags.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
                RTags = RLevel1;
        }
        else if (!Live && RLevel1->Context->Id == MATROSKA_ContextCues.Id && KeepCues)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
                RCues = RLevel1;
        }
        else if (!Live && RLevel1->Context->Id == MATROSKA_ContextAttachments.Id)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA)==ERR_NONE)
                RAttachments = RLevel1;
        }
        else if (RLevel1->Context->Id == MATROSKA_ContextCluster.Id)
        {
			// only partially read the Cluster data (not the data inside the blocks)
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_PARTIAL_DATA)==ERR_NONE)
			{
                ArrayAppend(&RClusters,&RLevel1,sizeof(RLevel1),256);
				// remove MATROSKA_ContextClusterPosition and MATROSKA_ContextClusterPrevSize until supported
				EbmlHead = EBML_MasterFindFirstElt(RLevel1, &MATROSKA_ContextClusterPosition, 0, 0);
				if (EbmlHead)
					NodeDelete((node*)EbmlHead);
				EbmlHead = EBML_MasterFindFirstElt(RLevel1, &MATROSKA_ContextClusterPrevSize, 0, 0);
				if (EbmlHead)
					NodeDelete((node*)EbmlHead);
				EbmlHead = NULL;
			}
        }
        else
		{
			EbmlHead = EBML_ElementSkipData(RLevel1, Input, &RSegmentContext, NULL, 1);
			assert(EbmlHead==NULL);
            NodeDelete((node*)RLevel1);
		}
        RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    }
    EndProgress(RSegment,1,Unsafe?3:2);

    if (!RSegmentInfo)
    {
        TextWrite(StdErr,T("The source Segment has no Segment Info section\r\n"));
        Result = -6;
        goto exit;
    }
    WSegmentInfo = EBML_ElementCopy(RSegmentInfo, NULL);

    if (TimeCodeScale!=0)
	{
		RLevel1 = EBML_MasterFindFirstElt(WSegmentInfo,&MATROSKA_ContextTimecodeScale,1,1);
		if (!RLevel1)
		{
			TextWrite(StdErr,T("Failed to get the TimeCodeScale handle\r\n"));
			Result = -10;
			goto exit;
		}
		EBML_IntegerSetValue((ebml_integer*)RLevel1,TimeCodeScale);
		RLevel1 = NULL;
	}
    if (Live)
    {
	    // remove MATROSKA_ContextDuration from Live streams
	    EbmlHead = EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextDuration, 0, 0);
	    if (EbmlHead)
		    NodeDelete((node*)EbmlHead);
        EbmlHead = NULL;
    }

    // reorder elements in WSegmentInfo
    Elt2 = EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextTimecodeScale, 0, 0);
    if (Elt2)
        NodeTree_SetParent(Elt2,WSegmentInfo,EBML_MasterChildren(WSegmentInfo));
    if (!Elt2)
        Elt2 = EBML_MasterChildren(WSegmentInfo);
    Elt = EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextDuration, 0, 0);
    if (Elt)
        NodeTree_SetParent(Elt,WSegmentInfo,Elt2);

    if (!RTrackInfo && ARRAYCOUNT(RClusters,ebml_element*))
    {
        TextWrite(StdErr,T("The source Segment has no Track Info section\r\n"));
        Result = -7;
        goto exit;
    }

	if (RTrackInfo)
	{
		Result = CleanTracks(RTrackInfo, DstProfile, RAttachments);
		if (Result!=0)
		{
			TextWrite(StdErr,T("No Tracks left to use!\r\n"));
			goto exit;
		}
		WTrackInfo = EBML_ElementCopy(RTrackInfo, NULL);
		if (WTrackInfo==NULL)
		{
			TextWrite(StdErr,T("Failed to copy the track info!\r\n"));
			goto exit;
		}

		// count the max track number
		for (Elt=EBML_MasterChildren(WTrackInfo); Elt; Elt=EBML_MasterNext(Elt))
		{
			if (Elt->Context->Id && MATROSKA_ContextTrackEntry.Id)
			{
				Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackNumber,0,0);
				if ((size_t)EBML_IntegerValue(Elt2) > MaxTrackNum)
					MaxTrackNum = (size_t)EBML_IntegerValue(Elt2);
			}
		}

		// make sure the lacing flag is set on tracks that use it
		i = -1;
		for (Elt = EBML_MasterChildren(WTrackInfo);Elt;Elt=EBML_MasterNext(Elt))
		{
			Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackNumber,0,0);
			i = max(i,(int)EBML_IntegerValue(Elt2));
		}
		ArrayResize(&WTracks,sizeof(track_info)*(i+1),0);
		ArrayZero(&WTracks);
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
    if (DstProfile == PROFILE_WEBM_V2)
    {
        if (EBML_StringSetValue((ebml_string*)RLevel1,"webm") != ERR_NONE)
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
	if (Live)
		EBML_ElementSetInfiniteSize(WSegment,1);
	else
		WSegment->DataSize = RSegment->DataSize; // temporary value
    if (EBML_ElementRenderHead(WSegment,Output,0,NULL)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the (temporary) Segment head\r\n"));
        Result = -10;
        goto exit;
    }

	if (!Live)
	{
		//  Prepare the Meta Seek
		WMetaSeek = EBML_MasterAddElt(WSegment,&MATROSKA_ContextSeekHead,0);
		WMetaSeek->ElementPosition = Stream_Seek(Output,0,SEEK_CUR); // keep the position for when we need to write it
		NextPos = 100; // dumy position of the SeekHead end
		// segment info
		WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
		WSegmentInfo->ElementPosition = NextPos;
		NextPos += EBML_ElementFullSize(WSegmentInfo,0);
		MATROSKA_LinkMetaSeekElement(WSeekPoint,WSegmentInfo);
		// track info
		if (WTrackInfo)
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
			WTrackInfo->ElementPosition = NextPos;
			NextPos += EBML_ElementFullSize(WTrackInfo,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,WTrackInfo);
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
	}

	Result = LinkClusters(&RClusters,RSegmentInfo,RTrackInfo,DstProfile, &WTracks);
	if (Result!=0)
		goto exit;

    if (Optimize && !UnOptimize)
    {
        int16_t BlockTrack;
        ebml_element *Block, *GBlock;
        matroska_cluster **ClusterR;

	    if (!Quiet) TextWrite(StdErr,T("Optimizing...\r\n"));

		ArrayResize(&TrackMaxHeader, sizeof(array)*(MaxTrackNum+1), 0);
		ArrayZero(&TrackMaxHeader);
        for (i=0;(size_t)i<=MaxTrackNum;++i)
            InitCommonHeader(ARRAYBEGIN(TrackMaxHeader,array)+i);

	    for (ClusterR=ARRAYBEGIN(RClusters,matroska_cluster*);ClusterR!=ARRAYEND(RClusters,matroska_cluster*);++ClusterR)
	    {
		    for (Block = EBML_MasterChildren(*ClusterR);Block;Block=EBML_MasterNext(Block))
		    {
			    if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
			    {
				    GBlock = EBML_MasterFindFirstElt(Block, &MATROSKA_ContextClusterBlock, 0, 0);
				    if (GBlock)
				    {
					    BlockTrack = MATROSKA_BlockTrackNum((matroska_block*)GBlock);
                        ShrinkCommonHeader(ARRAYBEGIN(TrackMaxHeader,array)+BlockTrack, (matroska_block*)GBlock, Input);
				    }
			    }
			    else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
			    {
				    BlockTrack = MATROSKA_BlockTrackNum((matroska_block *)Block);
                    ShrinkCommonHeader(ARRAYBEGIN(TrackMaxHeader,array)+BlockTrack, (matroska_block*)Block, Input);
			    }
		    }
	    }

        for (i=0;(size_t)i<=MaxTrackNum;++i)
            ClearCommonHeader(ARRAYBEGIN(TrackMaxHeader,array)+i);
    }

	if (Remux && WTrackInfo)
	{
		// create WClusters
		matroska_cluster **ClusterR, *ClusterW;
	    ebml_element *Block, *GBlock;
		ebml_element *Track;
        matroska_block *Block1;
		timecode_t Prev = INVALID_TIMECODE_T, *Tst, BlockTime, BlockDuration, MasterEnd, BlockEnd, MainBlockEnd;
		size_t MainTrack, BlockTrack;
        size_t Frame, *pTrackOrder;
		bool_t Deleted;
		array KeyFrameTimecodes, TrackBlockCurrIdx, TrackOrder, *pTrackBlock;
        array TrackBlocks; // array of block_info
        matroska_frame FrameData;
		block_info BlockInfo,*pBlockInfo;
		err_t Result;

		if (!Quiet) TextWrite(StdErr,T("Remuxing...\r\n"));
		// count the number of useful tracks
		Frame = 0;
		for (Track=EBML_MasterChildren(WTrackInfo); Track; Track=EBML_MasterNext(Track))
		{
			if (Track->Context->Id && MATROSKA_ContextTrackEntry.Id)
				++Frame;
		}

		ArrayInit(&TrackBlocks);
		ArrayResize(&TrackBlocks, sizeof(array)*(MaxTrackNum+1), 0);
		ArrayZero(&TrackBlocks);

		ArrayInit(&TrackBlockCurrIdx);
		ArrayResize(&TrackBlockCurrIdx, sizeof(size_t)*(MaxTrackNum+1), 0);
		ArrayZero(&TrackBlockCurrIdx);

		ArrayInit(&TrackOrder);

		// fill TrackBlocks with all the Blocks per track
		BlockInfo.DecodeTime = INVALID_TIMECODE_T;
		BlockInfo.FrameStart = 0;

		for (ClusterR=ARRAYBEGIN(RClusters,matroska_cluster*);ClusterR!=ARRAYEND(RClusters,matroska_cluster*);++ClusterR)
		{
			for (Block = EBML_MasterChildren(*ClusterR);Block;Block=EBML_MasterNext(Block))
			{
				if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
				{
					GBlock = EBML_MasterFindFirstElt(Block, &MATROSKA_ContextClusterBlock, 0, 0);
					if (GBlock)
					{
						BlockTrack = MATROSKA_BlockTrackNum((matroska_block*)GBlock);
						BlockInfo.Block = (matroska_block*)GBlock;
						ArrayAppend(ARRAYBEGIN(TrackBlocks,array)+BlockTrack,&BlockInfo,sizeof(BlockInfo),1024);
					}
				}
				else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
				{
					BlockTrack = MATROSKA_BlockTrackNum((matroska_block *)Block);
					BlockInfo.Block = (matroska_block*)Block;
					ArrayAppend(ARRAYBEGIN(TrackBlocks,array)+BlockTrack,&BlockInfo,sizeof(BlockInfo),1024);
				}
			}
		}

		// determine what is the main track num (video track)
		for (;;)
		{
			bool_t Exit = 1;
			ArrayResize(&TrackOrder, sizeof(size_t)*Frame, 0);
			ArrayZero(&TrackOrder);
			Track = GetMainTrack(WTrackInfo,&TrackOrder);
			if (!Track)
			{
				TextWrite(StdErr,T("Impossible to remux without a proper track to use\r\n"));
				goto exit;
			}
			Elt = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
			assert(Elt!=NULL);
			MainTrack = (int16_t)EBML_IntegerValue(Elt);

			for (pTrackOrder=ARRAYBEGIN(TrackOrder,size_t);pTrackOrder!=ARRAYEND(TrackOrder,size_t);++pTrackOrder)
			{
				if (!ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[*pTrackOrder],block_info))
				{
					TextPrintf(StdErr,T("Track %d has no blocks! Deleting...\r\n"),(int)*pTrackOrder);
					for (Elt = EBML_MasterFindFirstElt(WTrackInfo,&MATROSKA_ContextTrackEntry,0,0); Elt; Elt=EBML_MasterFindNextElt(WTrackInfo,Elt,0,0))
					{
						if (EBML_IntegerValue(EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackNumber,0,0))==*pTrackOrder)
						{
							NodeDelete((node*)Elt);
							--Frame;
							break;
						}
					}
					Exit = 0;
				}
			}
			if (Exit)
				break;
		}

		// process the decoding time for all blocks
		for (pTrackBlock=ARRAYBEGIN(TrackBlocks,array);pTrackBlock!=ARRAYEND(TrackBlocks,array);++pTrackBlock)
		{
			BlockEnd = INVALID_TIMECODE_T;
			for (pBlockInfo=ARRAYBEGIN(*pTrackBlock,block_info);pBlockInfo!=ARRAYEND(*pTrackBlock,block_info);++pBlockInfo)
			{
				BlockTime = MATROSKA_BlockTimecode(pBlockInfo->Block);
				pBlockInfo->DecodeTime = BlockTime;
				if ((pBlockInfo+1)!=ARRAYEND(*pTrackBlock,block_info) && BlockEnd!=INVALID_TIMECODE_T)
				{
					BlockDuration = MATROSKA_BlockTimecode((pBlockInfo+1)->Block);
					if (BlockTime > BlockDuration)
					{
						//assert(BlockDuration > BlockEnd);
						pBlockInfo->DecodeTime = BlockEnd + ((BlockTime - BlockDuration) >> 2);
					}
				}
				BlockEnd = pBlockInfo->DecodeTime;
			}
		}

		// get all the keyframe timecodes for our main track
		ArrayInit(&KeyFrameTimecodes);
		pTrackBlock=ARRAYBEGIN(TrackBlocks,array) + MainTrack;
		for (pBlockInfo=ARRAYBEGIN(*pTrackBlock,block_info);pBlockInfo!=ARRAYEND(*pTrackBlock,block_info);++pBlockInfo)
		{
			if (MATROSKA_BlockKeyframe(pBlockInfo->Block))
			{
				assert(pBlockInfo->DecodeTime == MATROSKA_BlockTimecode(pBlockInfo->Block));
				ArrayAppend(&KeyFrameTimecodes,&pBlockInfo->DecodeTime,sizeof(pBlockInfo->DecodeTime),256);
			}
		}
		if (!ARRAYCOUNT(KeyFrameTimecodes,timecode_t))
		{
			TextPrintf(StdErr,T("Impossible to remux, no keyframe found for track %d\r\n"),(int)MainTrack);
			goto exit;
		}

		// \todo sort Blocks of all tracks (according to the ref frame when available)
		// sort the timecodes, just in case the file wasn't properly muxed
		//ArraySort(&KeyFrameTimecodes, timecode_t, (arraycmp)TimcodeCmp, NULL, 1);

		// discrimate the timecodes we want to use as cluster boundaries
		//   create a new Cluster no shorter than 1s (unless the next one is too distant like 2s)
		Prev = INVALID_TIMECODE_T;
		for (Tst = ARRAYBEGIN(KeyFrameTimecodes, timecode_t); Tst!=ARRAYEND(KeyFrameTimecodes, timecode_t);)
		{
			Deleted = 0;
			if (Prev!=INVALID_TIMECODE_T && *Tst < Prev + 1000000000)
			{
				// too close
				if (Tst+1 != ARRAYEND(KeyFrameTimecodes, timecode_t) && *(Tst+1) < Prev + 2000000000)
				{
					ArrayRemove(&KeyFrameTimecodes, timecode_t, Tst, (arraycmp)TimcodeCmp, NULL);
					Deleted = 1;
				}
			}
			if (!Deleted)
				Prev = *Tst++;
		}

		// create each Cluster
		if (!Quiet) TextWrite(StdErr,T("Reclustering...\r\n"));
		for (Tst = ARRAYBEGIN(KeyFrameTimecodes, timecode_t); Tst!=ARRAYEND(KeyFrameTimecodes, timecode_t); ++Tst)
		{
			bool_t ReachedNextCluster = 0;
			ClusterW = (matroska_cluster*)EBML_ElementCreate(Track, &MATROSKA_ContextCluster, 1, NULL);
			ArrayAppend(&WClusters,&ClusterW,sizeof(ClusterW),256);
			MATROSKA_LinkClusterReadSegmentInfo(ClusterW, RSegmentInfo, 1);
			MATROSKA_ClusterSetTimecode(ClusterW,*Tst); // \todo avoid having negative timecodes in the Cluster ?
			MATROSKA_LinkClusterWriteSegmentInfo(ClusterW, WSegmentInfo);

			if ((Tst+1)==ARRAYEND(KeyFrameTimecodes, timecode_t))
				MasterEnd = INVALID_TIMECODE_T;
			else
				MasterEnd = *(Tst+1);

			while (!ReachedNextCluster && ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack] != ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[MainTrack],block_info))
			{
				// next Block end in the master track
				if (ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack]+1 == ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[MainTrack],block_info))
					MainBlockEnd = INVALID_TIMECODE_T;
				else
				{
					pBlockInfo = ARRAYBEGIN(ARRAYBEGIN(TrackBlocks,array)[MainTrack],block_info) + ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack] + 1;
					MainBlockEnd = pBlockInfo->DecodeTime;
				}

				for (pTrackOrder=ARRAYBEGIN(TrackOrder,size_t);pTrackOrder!=ARRAYEND(TrackOrder,size_t);++pTrackOrder)
				{
					// output all the blocks (todo frames) until MainBlockEnd (included) for this track
					while (ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder] < ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[*pTrackOrder],block_info))
					{
						// End of the current Block
						if (ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]+1 == ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[*pTrackOrder],block_info))
							BlockEnd = INVALID_TIMECODE_T;
						else
						{
							pBlockInfo = ARRAYBEGIN(ARRAYBEGIN(TrackBlocks,array)[*pTrackOrder],block_info) + ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder] + 1;
							BlockEnd = pBlockInfo->DecodeTime;
						}

						pBlockInfo = ARRAYBEGIN(ARRAYBEGIN(TrackBlocks,array)[*pTrackOrder],block_info) + ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder];
						if (pBlockInfo->DecodeTime > MainBlockEnd && *pTrackOrder!=MainTrack)
							break; // next track

						if (pBlockInfo->FrameStart!=0)
						{
							if (((ebml_element*)pBlockInfo->Block)->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
							{
                                Block1 = (matroska_block*)EBML_ElementCopy(pBlockInfo->Block, NULL);
								MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

								for (; pBlockInfo->FrameStart < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStart)
								{
									if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStart) >= MasterEnd)
										break;
									MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStart, &FrameData, 1);
									MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
								}

								if (MATROSKA_BlockGetFrameCount(Block1))
									EBML_MasterAppend((ebml_element*)ClusterW,(ebml_element*)Block1);
								else
									NodeDelete((node*)Block1);

								if (pBlockInfo->FrameStart!=MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									break; // next track
								else
								{
									ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
									continue;
								}
							}
							else if (((ebml_element*)pBlockInfo->Block)->Context->Id == MATROSKA_ContextClusterBlock.Id)
							{
                                Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
								Block1 = (matroska_block*)EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextClusterBlock, 0, 0);
								MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

								for (; pBlockInfo->FrameStart < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStart)
								{
									if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStart) >= MasterEnd)
										break;
									MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStart, &FrameData, 1);
									MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
								}

								if (MATROSKA_BlockGetFrameCount(Block1))
									EBML_MasterAppend((ebml_element*)ClusterW,(ebml_element*)Elt);
								else
									NodeDelete((node*)Elt);

								if (pBlockInfo->FrameStart!=MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									break; // next track
								else
								{
									ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
									continue;
								}
							}
						}

						if (BlockEnd>=MasterEnd && *pTrackOrder!=MainTrack && MATROSKA_BlockLaced(pBlockInfo->Block))
						{
							// relacing
							if (MATROSKA_BlockProcessFrameDurations(pBlockInfo->Block,Input)==ERR_NONE)
							{
								Result = ERR_NONE;
								if (((ebml_element*)pBlockInfo->Block)->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
								{
									// This block needs to be split
									MATROSKA_BlockReadData(pBlockInfo->Block,Input);
                                    Block1 = (matroska_block*)EBML_ElementCopy(pBlockInfo->Block, NULL);
								    MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

									for (; pBlockInfo->FrameStart < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStart)
									{
										if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStart) >= MasterEnd)
											break;
										MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStart, &FrameData, 1);
										MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
									}
	                                
									if (MATROSKA_BlockGetFrameCount(Block1)==MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									{
										pBlockInfo->FrameStart = 0; // all the frames are for the next Cluster
										NodeDelete((node*)Block1);
									}
									else
									{
										if (MATROSKA_BlockGetFrameCount(Block1))
											Result = EBML_MasterAppend((ebml_element*)ClusterW,(ebml_element*)Block1);
										else
											NodeDelete((node*)Block1);
										break; // next track
									}
								}
								else
								{
									assert(((ebml_element*)pBlockInfo->Block)->Context->Id == MATROSKA_ContextClusterBlock.Id);
									// This block needs to be split
									MATROSKA_BlockReadData(pBlockInfo->Block,Input);
                                    Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
									Block1 = (matroska_block*)EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextClusterBlock, 0, 0);
								    MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

									for (; pBlockInfo->FrameStart < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStart)
									{
										if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStart) >= MasterEnd)
											break;
										MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStart, &FrameData, 1);
										MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
									}
	                                
									if (MATROSKA_BlockGetFrameCount(Block1)==MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									{
										pBlockInfo->FrameStart = 0; // all the frames are for the next Cluster
										NodeDelete((node*)Elt);
									}
									else
									{
										if (MATROSKA_BlockGetFrameCount(Block1))
											Result = EBML_MasterAppend((ebml_element*)ClusterW,Elt);
										else
											NodeDelete((node*)Elt);
										break; // next track
									}
								}
								if (Result != ERR_NONE)
								{
									if (Result==ERR_INVALID_DATA)
										TextPrintf(StdErr,T("Impossible to remux, the TimecodeScale may be too low, try --timecodescale 1000000\r\n"));
									else
										TextPrintf(StdErr,T("Impossible to remux, error appending a block\r\n"));
									Result = -46;
									goto exit;
								}
							}
						}

						if (MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
						{
							if (((ebml_element*)pBlockInfo->Block)->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
                            {
							    Result = MATROSKA_LinkBlockWriteSegmentInfo(pBlockInfo->Block,WSegmentInfo);
                                if (Result == ERR_NONE)
								    Result = EBML_MasterAppend((ebml_element*)ClusterW,(ebml_element*)pBlockInfo->Block);
                            }
							else
							{
                                assert(((ebml_element*)pBlockInfo->Block)->Context->Id == MATROSKA_ContextClusterBlock.Id);
                                Result = MATROSKA_LinkBlockWriteSegmentInfo(pBlockInfo->Block,WSegmentInfo);
                                if (Result == ERR_NONE)
                                    Result = EBML_MasterAppend((ebml_element*)ClusterW,EBML_ElementParent((ebml_element*)pBlockInfo->Block));
							}
							if (Result != ERR_NONE)
							{
								if (Result==ERR_INVALID_DATA)
									TextPrintf(StdErr,T("Impossible to remux, the TimecodeScale may be too low, try --timecodescale 1000000\r\n"));
								else
									TextPrintf(StdErr,T("Impossible to remux, error appending a block\r\n"));
								Result = -46;
								goto exit;
							}
							ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
						}

						if (*pTrackOrder==MainTrack)
						{
							if (MainBlockEnd == INVALID_TIMECODE_T || BlockEnd == MasterEnd)
								ReachedNextCluster = 1;
							break;
						}
					} 
				}
			}
		}

		ArrayClear(&KeyFrameTimecodes);
		ArrayClear(&TrackBlocks);
		ArrayClear(&TrackBlockCurrIdx);
		ArrayClear(&TrackOrder);

		Clusters = &WClusters;
		NodeDelete((node*)RCues);
		RCues = NULL;
	}

	if (WTrackInfo)
	{
        array *HeaderData;
        size_t TrackNum;
        tchar_t CodecID[MAXDATA];
		// fix/clean the Lacing flag for each track
		assert(MATROSKA_ContextTrackLacing.DefaultValue==1);
		for (RLevel1 = EBML_MasterChildren(WTrackInfo); RLevel1; RLevel1=EBML_MasterNext(RLevel1))
		{
            if (RLevel1->Context->Id == MATROSKA_ContextTrackEntry.Id)
            {
			    Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackNumber,0,0);
			    if (!Elt2) continue;
                TrackNum = (size_t)EBML_IntegerValue(Elt2);
			    if (ARRAYBEGIN(WTracks,track_info)[TrackNum].IsLaced)
			    {
				    // has lacing
				    Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackLacing,0,0);
				    if (Elt2)
					    NodeDelete((node*)Elt2);
			    }
			    else
			    {
				    // doesn't have lacing
				    Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackLacing,1,0);
				    EBML_IntegerSetValue((ebml_integer*)Elt2,0);
			    }

                if (UnOptimize)
                {
                    // remove the previous track compression
                    Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackEncodings,0,0);
                    NodeDelete((node*)Elt2);
                }
                else if (Optimize)
                {
                    Elt = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackCodecID,1,0);
                    EBML_StringGet((ebml_string*)Elt,CodecID,TSIZEOF(CodecID));
                    if (tcsisame_ascii(CodecID,T("S_USF")) || tcsisame_ascii(CodecID,T("S_VOBSUB")) || tcsisame_ascii(CodecID,T("S_HDMV/PGS")) || tcsisame_ascii(CodecID,T("B_VOBBTN")))
                    {
                        // force zlib compression
                        // remove the previous compression and the new optimized one
                        Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackEncodings,0,0);
                        NodeDelete((node*)Elt2);
                        Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackEncodings,1,1);
                        Elt2 = EBML_MasterFindFirstElt(Elt2,&MATROSKA_ContextTrackEncoding,1,1);
                        Elt =  EBML_MasterFindFirstElt(Elt2,&MATROSKA_ContextTrackEncodingCompression,1,1);
                        Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackEncodingCompressionAlgo,1,1);
                        EBML_IntegerSetValue((ebml_integer*)Elt2,MATROSKA_BLOCK_COMPR_ZLIB);
                    }
                    else
                    {
                        HeaderData = ARRAYBEGIN(TrackMaxHeader,array)+TrackNum;
                        if (ARRAYCOUNT(*HeaderData,uint8_t))
                        {
                            // remove the previous compression and the new optimized one
                            Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackEncodings,0,0);
                            NodeDelete((node*)Elt2);
                            Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackEncodings,1,1);
                            Elt2 = EBML_MasterFindFirstElt(Elt2,&MATROSKA_ContextTrackEncoding,1,1);
                            Elt =  EBML_MasterFindFirstElt(Elt2,&MATROSKA_ContextTrackEncodingCompression,1,1);
                            Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackEncodingCompressionAlgo,1,1);
                            EBML_IntegerSetValue((ebml_integer*)Elt2,MATROSKA_BLOCK_COMPR_HEADER);
                            Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackEncodingCompressionSetting,1,1);
                            EBML_BinarySetData((ebml_binary*)Elt2,ARRAYBEGIN(*HeaderData,uint8_t),ARRAYCOUNT(*HeaderData,uint8_t));
                        }
                    }
                }

                Elt = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextTrackEncodings,0,0);
                if (Elt)
                {
                    if ((Elt2 = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextTrackEncoding,0,0)) != NULL)
                    {
                        Elt = EBML_MasterFindFirstElt(Elt2,&MATROSKA_ContextTrackEncodingCompressionAlgo,1,1);
                        if (EBML_IntegerValue(Elt)!=MATROSKA_BLOCK_COMPR_HEADER)
                            ClustersNeedRead = 1;
                    }
                }
            }
		}
	}

    // use the output track settings for each block
    for (Cluster = ARRAYBEGIN(*Clusters,ebml_element*);Cluster != ARRAYEND(*Clusters,ebml_element*); ++Cluster)
    {
        for (Elt = EBML_MasterChildren(*Cluster);Elt;Elt=RLevel1)
        {
            RLevel1 = EBML_MasterNext(Elt);
            if (Elt->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
            {
                for (Elt2 = EBML_MasterChildren(Elt);Elt2;Elt2=EBML_MasterNext(Elt2))
                {
                    if (Elt2->Context->Id == MATROSKA_ContextClusterBlock.Id)
                    {
                        if (MATROSKA_LinkBlockWithWriteTracks((matroska_block*)Elt2,WTrackInfo)!=ERR_NONE)
                            NodeDelete((node*)Elt);
                        else if (MATROSKA_LinkBlockWriteSegmentInfo((matroska_block*)Elt2,WSegmentInfo)!=ERR_NONE)
                            NodeDelete((node*)Elt);
                        break;
                    }
                }
            }
            else if (Elt->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
            {
                if (MATROSKA_LinkBlockWithWriteTracks((matroska_block*)Elt,WTrackInfo)!=ERR_NONE)
                    NodeDelete((node*)Elt);
                else if (MATROSKA_LinkBlockWriteSegmentInfo((matroska_block*)Elt,WSegmentInfo)!=ERR_NONE)
                    NodeDelete((node*)Elt);
            }
        }
	    //EBML_ElementUpdateSize(*Cluster, 0, 0);
    }

    // cues
	if (!Live)
	{
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
		if (!RCues && WTrackInfo)
		{
			// generate the cues
			RCues = EBML_ElementCreate(&p,&MATROSKA_ContextCues,0,NULL);
			if (!Quiet) TextWrite(StdErr,T("Generating Cues from scratch\r\n"));
			CuesCreated = GenerateCueEntries(RCues,Clusters,WTrackInfo,WSegmentInfo,RSegment);
			if (!CuesCreated)
			{
				NodeDelete((node*)RCues);
				RCues = NULL;
			}
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

        if (!RTags)
        {
            // create a fake Tags element to have its position prepared in the SeekHead
            RTags = EBML_ElementCreate(WMetaSeek,&MATROSKA_ContextTags,1,NULL);
            RTags->ElementPosition = RSegment->DataSize;
			WSeekPointTags = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
            MATROSKA_LinkMetaSeekElement(WSeekPointTags,RTags);
        }

		// first estimation of the MetaSeek size
		MetaSeekUpdate(WMetaSeek);
		MetaSeekBefore = EBML_ElementFullSize(WMetaSeek,0);
		NextPos = WMetaSeek->ElementPosition + EBML_ElementFullSize(WMetaSeek,0);
	}

    //  Compute the Segment Info size
    ReduceSize(WSegmentInfo);
    // change the library names & app name
    stprintf_s(String,TSIZEOF(String),T("%s + %s"),Node_GetDataStr((node*)&p,CONTEXT_LIBEBML_VERSION),Node_GetDataStr((node*)&p,CONTEXT_LIBMATROSKA_VERSION));
    LibName = (ebml_string*)EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextMuxingApp, 1, 0);
    EBML_StringGet(LibName,Original,TSIZEOF(Original));
    EBML_UniStringSetValue(LibName,String);

    AppName = (ebml_string*)EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextWritingApp, 1, 0);
    EBML_StringGet(AppName,String,TSIZEOF(String));
	ExtraSizeDiff = tcslen(String);
    if (!tcsisame_ascii(String,Original)) // libavformat writes the same twice, we only need one
    {
		if (Original[0])
			tcscat_s(Original,TSIZEOF(Original),T(" + "));
        tcscat_s(Original,TSIZEOF(Original),String);
    }
    s = Original;
    if (tcsnicmp_ascii(Original,T("mkclean "),8)==0)
        s += 14;
	stprintf_s(String,TSIZEOF(String),T("mkclean %s"),PROJECT_VERSION);
	if (Remux ||Live || Optimize)
		tcscat_s(String,TSIZEOF(String),T(" "));
	if (Remux)
		tcscat_s(String,TSIZEOF(String),T("r"));
	if (Optimize)
		tcscat_s(String,TSIZEOF(String),T("o"));
	if (Live)
		tcscat_s(String,TSIZEOF(String),T("l"));
	if (UnOptimize)
		tcscat_s(String,TSIZEOF(String),T("u"));
	if (s[0])
		stcatprintf_s(String,TSIZEOF(String),T(" from %s"),s);
    EBML_UniStringSetValue(AppName,String);
	ExtraSizeDiff = tcslen(String) - ExtraSizeDiff + 2;

	if (Remux || !EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextSegmentDate, 0, 0))
	{
		RLevel1 = EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextSegmentDate, 1, 1);
		EBML_DateSetDateTime((ebml_date*)RLevel1, GetTimeDate());
		RLevel1 = NULL;
	}

    EBML_ElementUpdateSize(WSegmentInfo,0,0);
    WSegmentInfo->ElementPosition = NextPos;
    NextPos += EBML_ElementFullSize(WSegmentInfo,0);

    //  Compute the Track Info size
    if (WTrackInfo)
    {
        ReduceSize(WTrackInfo);
        EBML_ElementUpdateSize(WTrackInfo,0,0);
        WTrackInfo->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(WTrackInfo,0);
    }

	if (!Live)
	{
		//  Compute the Chapters size
		if (RChapters)
		{
			ReduceSize(RChapters);
			EBML_ElementUpdateSize(RChapters,0,0);
			RChapters->ElementPosition = NextPos;
			NextPos += EBML_ElementFullSize(RChapters,0);
		}

		//  Compute the Tags size
		if (RTags && !WSeekPointTags)
		{
			EBML_ElementUpdateSize(RTags,0,0);
			RTags->ElementPosition = NextPos;
			NextPos += EBML_ElementFullSize(RTags,0);
		}

		MetaSeekUpdate(WMetaSeek);
		NextPos += EBML_ElementFullSize(WMetaSeek,0) - MetaSeekBefore;

		//  Compute the Cues size
		if (WTrackInfo && RCues)
		{
            OptimizeCues(RCues,Clusters,WSegmentInfo,NextPos, WSegment, RSegment, !CuesCreated, !Unsafe, ClustersNeedRead?Input:NULL);
			EBML_ElementUpdateSize(RCues,0,0);
			RCues->ElementPosition = NextPos;
			NextPos += EBML_ElementFullSize(RCues,0);
		}

		// update and write the MetaSeek and the elements following
        // write without the fake Tags pointer
		Stream_Seek(Output,WMetaSeek->ElementPosition,SEEK_SET);
        MetaSeekUpdate(WMetaSeek);
        if (WSeekPointTags)
        {
            filepos_t SeekPointSize;
            ebml_element *Void;

            // remove the fake tags pointer and replace by a void of the same size
            NodeTree_SetParent(WSeekPointTags,NULL,NULL);

            // write a Void element the size of WSeekPointTags
            SeekPointSize = EBML_ElementFullSize((ebml_element*)WSeekPointTags, 0);
            Void = EBML_ElementCreate(WSeekPointTags,&EBML_ContextEbmlVoid,1,NULL);
            EBML_VoidSetSize(Void, SeekPointSize - 1 - EBML_CodedSizeLength(SeekPointSize,0,1));
            EBML_MasterAppend(WMetaSeek,Void);

            NodeDelete((node*)WSeekPointTags);
            WSeekPointTags = NULL;
            NodeDelete((node*)RTags);
            RTags = NULL;
        }

		EBML_ElementFullSize(WMetaSeek,0);
		if (EBML_ElementRender(WMetaSeek,Output,0,0,1,&MetaSeekBefore,0)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Seek Head\r\n"));
			Result = -22;
			goto exit;
		}
		SegmentSize += EBML_ElementFullSize(WMetaSeek,0);
	}
    else if (!Unsafe)
        SetClusterPrevSize(Clusters, ClustersNeedRead?Input:NULL);

    if (EBML_ElementRender(WSegmentInfo,Output,0,0,1,NULL,0)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the Segment Info\r\n"));
        Result = -11;
        goto exit;
    }
    SegmentSize += EBML_ElementFullSize(WSegmentInfo,0);
    if (WTrackInfo)
    {
        if (EBML_ElementRender(WTrackInfo,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Track Info\r\n"));
            Result = -12;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(WTrackInfo,0);
    }
    if (!Live && RChapters)
    {
        if (EBML_ElementRender(RChapters,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Chapters\r\n"));
            Result = -13;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(RChapters,0);
    }
    if (!Live && RTags)
    {
        if (EBML_ElementRender(RTags,Output,0,0,1,NULL,0)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Tags\r\n"));
            Result = -14;
            goto exit;
        }
        SegmentSize += EBML_ElementFullSize(RTags,0);
    }
    if (!Live && RCues)
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
    ClusterSize = INVALID_FILEPOS_T;
    for (Cluster = ARRAYBEGIN(*Clusters,ebml_element*);Cluster != ARRAYEND(*Clusters,ebml_element*); ++Cluster)
    {
        ShowProgress(*Cluster,RSegment,Unsafe?3:2,Unsafe?3:2);
        EBML_ElementSetInfiniteSize(*Cluster,Live);
        WriteCluster(*Cluster,Output,Input, Live, ClusterSize);
        if (!Unsafe)
            ClusterSize = EBML_ElementFullSize(*Cluster,0);
        SegmentSize += EBML_ElementFullSize(*Cluster,0);
    }
    EndProgress(RSegment,Unsafe?3:2,Unsafe?3:2);

    //  Write the Attachments
    if (!Live && RAttachments)
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
	if (!Live)
	{
		if (SegmentSize - ExtraSizeDiff > WSegment->DataSize)
		{
			if (EBML_CodedSizeLength(SegmentSize,WSegment->SizeLength,0) != EBML_CodedSizeLength(SegmentSize,WSegment->SizeLength,0))
			{
				TextPrintf(StdErr,T("The segment written is much bigger than the original %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),SegmentSize,WSegment->DataSize);
				Result = -20;
				goto exit;
			}
			TextPrintf(StdErr,T("The segment written is bigger than the original %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),SegmentSize,WSegment->DataSize);
		}
		if (EBML_CodedSizeLength(WSegment->DataSize,0,!Live) > EBML_CodedSizeLength(SegmentSize,0,!Live))
			WSegment->SizeLength = (int8_t)EBML_CodedSizeLength(WSegment->DataSize,0,!Live);
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
		//Stream_Seek(Output,WMetaSeek->ElementPosition,SEEK_SET);
		if (EBML_ElementRender(WMetaSeek,Output,0,0,1,&MetaSeekAfter,0)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Seek Head\r\n"));
			Result = -22;
			goto exit;
		}
		if (MetaSeekBefore != MetaSeekAfter)
		{
			TextPrintf(StdErr,T("The final Seek Head size has changed %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),MetaSeekBefore,MetaSeekAfter);
			Result = -23;
			goto exit;
		}
	}

    if (!Quiet) TextPrintf(StdErr,T("Finished cleaning & optimizing \"%s\"\r\n"),Path);

exit:
    NodeDelete((node*)WSegment);

    for (Cluster = ARRAYBEGIN(RClusters,ebml_element*);Cluster != ARRAYEND(RClusters,ebml_element*); ++Cluster)
        NodeDelete((node*)*Cluster);
    ArrayClear(&RClusters);
    for (Cluster = ARRAYBEGIN(WClusters,ebml_element*);Cluster != ARRAYEND(WClusters,ebml_element*); ++Cluster)
        NodeDelete((node*)*Cluster);
    for (MaxTrackNum=0;MaxTrackNum<ARRAYCOUNT(TrackMaxHeader,array);++MaxTrackNum)
        ArrayClear(ARRAYBEGIN(TrackMaxHeader,array)+MaxTrackNum);
    ArrayClear(&TrackMaxHeader);
    ArrayClear(&WClusters);
    ArrayClear(&WTracks);
    NodeDelete((node*)RAttachments);
    NodeDelete((node*)RTags);
    NodeDelete((node*)RCues);
    NodeDelete((node*)RChapters);
    NodeDelete((node*)RTrackInfo);
    NodeDelete((node*)WTrackInfo);
    NodeDelete((node*)RSegmentInfo);
    NodeDelete((node*)WSegmentInfo);
    NodeDelete((node*)RLevel1);
    NodeDelete((node*)RSegment);
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
