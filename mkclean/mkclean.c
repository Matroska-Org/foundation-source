/*
 * $Id$
 * Copyright (c) 2010-2011, Matroska (non-profit organisation)
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
#include "mkclean_stdafx.h"
#include "mkclean_project.h"
#ifndef CONFIG_EBML_UNICODE
#define CONFIG_EBML_UNICODE
#endif
#include "matroska/matroska.h"
#include "matroska/matroska_sem.h"

/*!
 * \todo write the CRC-32 on Clusters too (make it faster in libebml2)
 * \todo discards tracks that has the same UID
 * \todo remuxing: turn a BlockGroup into a SimpleBlock in v2 profiles and when it makes sense (duration = default track duration) (optimize mode)
 * \todo error when an unknown codec (for the profile) is found (option to turn into a warning) (loose mode)
 * \todo compute the segment duration based on audio (when it's not set)
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

#define EXTRA_SEEK_SPACE       22
#define MARKER3D         (block_info*)1

typedef struct block_info
{
	matroska_block *Block;
	timecode_t DecodeTime;
	size_t FrameStartIndex;

} block_info;

#define TABLE_MARKER (uint8_t*)1

typedef struct track_info
{
	bool_t IsLaced;
//    bool_t NeedsReading;

} track_info;

static const tchar_t *GetProfileName(size_t ProfileNum)
{
static const tchar_t *Profile[7] = {T("unknown"), T("matroska v1"), T("matroska v2"), T("matroska v3"), T("webm"), T("matroska+DivX"), T("matroska v4")};
	switch (ProfileNum)
	{
	default:                  return Profile[0];
	case PROFILE_MATROSKA_V1: return Profile[1];
	case PROFILE_MATROSKA_V2: return Profile[2];
	case PROFILE_MATROSKA_V3: return Profile[3];
	case PROFILE_WEBM:        return Profile[4];
	case PROFILE_DIVX:        return Profile[5];
	case PROFILE_MATROSKA_V4: return Profile[6];
	}
}

static int GetProfileId(int Profile)
{
	switch (Profile)
	{
	default:                  return 0;
	case PROFILE_MATROSKA_V1: return 1;
	case PROFILE_MATROSKA_V2: return 2;
	case PROFILE_MATROSKA_V3: return 3;
	case PROFILE_WEBM:        return 4;
	case PROFILE_DIVX:        return 5;
	case PROFILE_MATROSKA_V4: return 6;
	}
}

static int DocVersion = 1;
static int SrcProfile = 0, DstProfile = 0;
static textwriter *StdErr = NULL;
static size_t ExtraSizeDiff = 0;
static bool_t Quiet = 0;
static bool_t Unsafe = 0;
static bool_t Live = 0;
static int TotalPhases = 2;
static int CurrentPhase = 1;

static bool_t MasterError(void *cookie, int type, const tchar_t *ClassName, const ebml_element *i)
{
	tchar_t IdString[MAXPATH];
    if (type==MASTER_CHECK_PROFILE_INVALID)
    {
    	EBML_ElementGetName(i,IdString,TSIZEOF(IdString));
        TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" is not part of profile '%s', skipping\r\n"),IdString,EBML_ElementPosition(i),GetProfileName(DstProfile));
    }
    else if (type==MASTER_CHECK_MULTIPLE_UNIQUE)
    {
    	EBML_ElementGetName(i,IdString,TSIZEOF(IdString));
        TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" has multiple versions of the unique element %s, skipping\r\n"),IdString,EBML_ElementPosition(i),ClassName);
    }
    else if (type==MASTER_CHECK_MISSING_MANDATORY)
    {
    	EBML_ElementGetName(cookie,IdString,TSIZEOF(IdString));
	    TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" is missing mandatory element %s\r\n"),IdString,EBML_ElementPosition(cookie),ClassName);
    }
    return 1;
}

static void ReduceSize(ebml_element *Element)
{
    EBML_ElementSetSizeLength(Element, 0); // reset
    if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    {
        ebml_element *i, *j;

        if (Unsafe)
            EBML_MasterUseChecksum((ebml_master*)Element,0);

        if (EBML_ElementIsType(Element, &MATROSKA_ContextBlockGroup) && !EBML_MasterCheckMandatory((ebml_master*)Element, 0))
        {
            NodeDelete((node*)Element);
            return;
        }

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
			else if (EBML_ElementIsType(i, &EBML_ContextEbmlCrc32))
			{
				NodeDelete((node*)i);
				continue;
			}
            ReduceSize(i);
		}

        if (!EBML_MasterChildren(Element) && !EBML_MasterCheckMandatory((ebml_master*)Element, 0))
        {
            NodeDelete((node*)Element);
            return;
        }

        EBML_MasterAddMandatory((ebml_master*)Element,1);

        EBML_MasterCheckContext((ebml_master*)Element, DstProfile, MasterError, Element);
	}
}

static bool_t ReadClusterData(ebml_master *Cluster, stream *Input)
{
    bool_t Changed = 0;
    err_t Result = ERR_NONE;
    ebml_element *Block, *GBlock, *NextBlock;
    // read all the Block/SimpleBlock data
    for (Block = EBML_MasterChildren(Cluster);Block;Block=NextBlock)
    {
        NextBlock = EBML_MasterNext(Block);
        if (EBML_ElementIsType(Block, &MATROSKA_ContextBlockGroup))
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(GBlock, &MATROSKA_ContextBlock))
                {
                    if ((Result = MATROSKA_BlockReadData((matroska_block*)GBlock, Input))!=ERR_NONE)
                    {
                        Changed = 1;
                        NodeDelete((node*)Block);
                    }
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
        {
            if ((Result = MATROSKA_BlockReadData((matroska_block*)Block, Input))!=ERR_NONE)
            {
                Changed = 1;
                NodeDelete((node*)Block);
            }
        }
    }
    return Changed;
}

static err_t UnReadClusterData(ebml_master *Cluster, bool_t IncludingNotRead)
{
    err_t Result = ERR_NONE;
    ebml_element *Block, *GBlock;
    for (Block = EBML_MasterChildren(Cluster);Result==ERR_NONE && Block;Block=EBML_MasterNext(Block))
    {
        if (EBML_ElementIsType(Block, &MATROSKA_ContextBlockGroup))
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(GBlock, &MATROSKA_ContextBlock))
                {
                    Result = MATROSKA_BlockReleaseData((matroska_block*)GBlock,IncludingNotRead);
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
            Result = MATROSKA_BlockReleaseData((matroska_block*)Block,IncludingNotRead);
    }
    return Result;
}

static void SetClusterPrevSize(array *Clusters, stream *Input)
{
    ebml_master **Cluster;
    ebml_element *Elt, *Elt2;
    filepos_t ClusterSize = INVALID_FILEPOS_T;

    // Write the Cluster PrevSize
    for (Cluster = ARRAYBEGIN(*Clusters,ebml_master*);Cluster != ARRAYEND(*Clusters,ebml_master*); ++Cluster)
    {
        if (Input!=NULL)
            ReadClusterData(*Cluster,Input);

        if (ClusterSize != INVALID_FILEPOS_T)
        {
            Elt = EBML_MasterGetChild(*Cluster, &MATROSKA_ContextPrevSize);
            if (Elt)
            {
                EBML_IntegerSetValue((ebml_integer*)Elt, ClusterSize);
                Elt2 = EBML_MasterFindChild(*Cluster, &MATROSKA_ContextTimecode);
                if (Elt2)
                    NodeTree_SetParent(Elt,*Cluster,NodeTree_Next(Elt2));
                ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                EBML_ElementUpdateSize(*Cluster,0,1);
            }
        } else EBML_ElementUpdateSize(*Cluster,0,1);
        EBML_ElementSetInfiniteSize((ebml_element*)*Cluster,Live);
        ClusterSize = EBML_ElementFullSize((ebml_element*)*Cluster,0);

        if (Input!=NULL)
            UnReadClusterData(*Cluster, 0);
    }
}

static void UpdateCues(ebml_master *Cues, ebml_master *Segment)
{
    ebml_master *Cue,*NextCue;

    // reevaluate the size needed for the Cues
    for (Cue=(ebml_master*)EBML_MasterChildren(Cues);Cue;Cue=NextCue)
    {
        NextCue =(ebml_master*)EBML_MasterNext(Cue);
        if (MATROSKA_CuePointUpdate((matroska_cuepoint*)Cue, (ebml_element*)Segment)!=ERR_NONE)
        {
            EBML_MasterRemove(Cues,(ebml_element*)Cue); // make sure it doesn't remain in the list
            NodeDelete((node*)Cue);
        }
    }
}

static void SettleClustersWithCues(array *Clusters, filepos_t ClusterStart, ebml_master *Cues, ebml_master *Segment, bool_t SafeClusters, stream *Input)
{
    ebml_element *Elt, *Elt2;
    ebml_master **Cluster;
    filepos_t OriginalSize = EBML_ElementDataSize((ebml_element*)Cues,0);
    filepos_t ClusterPos = ClusterStart + EBML_ElementFullSize((ebml_element*)Cues,0);
    filepos_t ClusterSize = INVALID_FILEPOS_T;

    // reposition all the Clusters
    for (Cluster=ARRAYBEGIN(*Clusters,ebml_master*);Cluster!=ARRAYEND(*Clusters,ebml_master*);++Cluster)
    {
        if (Input!=NULL)
            ReadClusterData(*Cluster,Input);

        EBML_ElementForcePosition((ebml_element*)*Cluster, ClusterPos);
        if (SafeClusters)
        {
            Elt = NULL;
            if (ClusterSize != INVALID_FILEPOS_T)
            {
                Elt = EBML_MasterGetChild(*Cluster, &MATROSKA_ContextPrevSize);
                if (Elt)
                {
                    EBML_IntegerSetValue((ebml_integer*)Elt, ClusterSize);
                    Elt2 = EBML_MasterFindChild(*Cluster, &MATROSKA_ContextTimecode);
                    if (Elt2)
                        NodeTree_SetParent(Elt,*Cluster,NodeTree_Next(Elt2)); // make sure the PrevSize is just after the ClusterTimecode
                    ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                }
            }

            // output the Cluster position as well
            if (Elt)
                Elt2 = Elt; // make sure the Cluster Position is just after the PrevSize
            else
                Elt2 = EBML_MasterFindChild(*Cluster, &MATROSKA_ContextTimecode); // make sure the Cluster Position is just after the ClusterTimecode
            if (Elt2 && DstProfile!=PROFILE_WEBM)
            {
                Elt = EBML_MasterGetChild(*Cluster, &MATROSKA_ContextPosition);
                if (Elt)
                {
                    EBML_IntegerSetValue((ebml_integer*)Elt, ClusterPos - EBML_ElementPositionData((ebml_element*)Segment));
                    NodeTree_SetParent(Elt,*Cluster,NodeTree_Next(Elt2));
                    ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                }
            }
        }
        EBML_ElementUpdateSize(*Cluster,0,0);
        ClusterSize = EBML_ElementFullSize((ebml_element*)*Cluster,0);
        ClusterPos += ClusterSize;

        if (Input!=NULL)
            UnReadClusterData(*Cluster, 0);
    }

    UpdateCues(Cues, Segment);

    ClusterPos = EBML_ElementUpdateSize(Cues,0,0);
    if (ClusterPos != OriginalSize)
        SettleClustersWithCues(Clusters,ClusterStart,Cues,Segment, SafeClusters, Input);

}

static void ShowProgress(const ebml_element *RCluster, filepos_t TotalSize)
{
    if (!Quiet)
        TextPrintf(StdErr,T("Progress %d/%d: %3d%%\r"), CurrentPhase, TotalPhases,Scale32(100,EBML_ElementPosition(RCluster),TotalSize)+1);
}

static void EndProgress()
{
    if (!Quiet)
        TextPrintf(StdErr,T("Progress %d/%d: 100%%\r\n"), CurrentPhase, TotalPhases);
}

static matroska_cluster **LinkCueCluster(matroska_cuepoint *Cue, array *Clusters, matroska_cluster **StartCluster, filepos_t TotalSize)
{
    matroska_cluster **Cluster;
    matroska_block *Block;
    int16_t CueTrack;
    timecode_t CueTimecode;
    size_t StartBoost = 7;

    CueTrack = MATROSKA_CueTrackNum(Cue);
    CueTimecode = MATROSKA_CueTimecode(Cue);
    ++CurrentPhase;
    if (StartCluster)
    {
        for (Cluster=StartCluster;StartBoost && Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster,--StartBoost)
        {
            Block = MATROSKA_GetBlockForTimecode(*Cluster, CueTimecode, CueTrack);
            if (Block)
            {
                MATROSKA_LinkCuePointBlock(Cue,Block);
                ShowProgress((ebml_element*)(*Cluster),TotalSize);
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
            ShowProgress((ebml_element*)(*Cluster),TotalSize);
            return Cluster;
        }
    }

    TextPrintf(StdErr,T("Could not find the matching block for timecode %0.3f s\r\n"),CueTimecode/1000000000.0);
    return NULL;
}

static int LinkClusters(array *Clusters, ebml_master *RSegmentInfo, ebml_master *Tracks, int DstProfile, array *WTracks, timecode_t Offset)
{
    matroska_cluster **Cluster;
	ebml_element *Block, *GBlock, *BlockTrack, *Type;
    ebml_integer *Time;
    int BlockNum;

	// find out if the Clusters use forbidden features for that DstProfile
	if (DstProfile == PROFILE_MATROSKA_V1 || DstProfile == PROFILE_DIVX)
	{
		for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
		{
			for (Block = EBML_MasterChildren(*Cluster);Block;Block=EBML_MasterNext(Block))
			{
				if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
                {
					TextPrintf(StdErr,T("Using SimpleBlock in profile '%s' at %") TPRId64 T(" try \"--doctype %d\"\r\n"),GetProfileName(DstProfile),EBML_ElementPosition(Block),GetProfileId(PROFILE_MATROSKA_V2));
					return -32;
				}
			}
		}
	}

	// link each Block/SimpleBlock with its Track and SegmentInfo
	for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
	{
        if (Offset != INVALID_TIMECODE_T)
        {
            Time = (ebml_integer*)EBML_MasterGetChild((ebml_master*)*Cluster, &MATROSKA_ContextTimecode);
            if (Time)
                EBML_IntegerSetValue(Time, Offset + EBML_IntegerValue(Time));
        }
		MATROSKA_LinkClusterBlocks(*Cluster, RSegmentInfo, Tracks, 0);
		ReduceSize((ebml_element*)*Cluster);
	}

    // mark all the audio/subtitle tracks as keyframes
	for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
	{
		for (Block = EBML_MasterChildren(*Cluster);Block;Block=EBML_MasterNext(Block))
		{
			if (EBML_ElementIsType(Block, &MATROSKA_ContextBlockGroup))
			{
				GBlock = EBML_MasterFindChild((ebml_master*)Block, &MATROSKA_ContextBlock);
				if (GBlock)
				{
					BlockTrack = MATROSKA_BlockReadTrack((matroska_block*)GBlock);
                    if (!BlockTrack) continue;
                    Type = EBML_MasterFindChild((ebml_master*)BlockTrack,&MATROSKA_ContextTrackType);
                    if (!Type) continue;
                    if (EBML_IntegerValue((ebml_integer*)Type)==TRACK_TYPE_AUDIO || EBML_IntegerValue((ebml_integer*)Type)==TRACK_TYPE_SUBTITLE)
                    {
                        MATROSKA_BlockSetKeyframe((matroska_block*)GBlock,1);
						MATROSKA_BlockSetDiscardable((matroska_block*)GBlock,0);
                    }
                    BlockNum = MATROSKA_BlockTrackNum((matroska_block*)GBlock);
                    if (MATROSKA_BlockGetFrameCount((matroska_block*)GBlock)>1)
                        ARRAYBEGIN(*WTracks,track_info)[BlockNum].IsLaced = 1;
				}
			}
			else if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
			{
				BlockTrack = MATROSKA_BlockReadTrack((matroska_block *)Block);
                if (!BlockTrack) continue;
                Type = EBML_MasterFindChild((ebml_master*)BlockTrack,&MATROSKA_ContextTrackType);
                if (!Type) continue;
                if (EBML_IntegerValue((ebml_integer*)Type)==TRACK_TYPE_AUDIO || EBML_IntegerValue((ebml_integer*)Type)==TRACK_TYPE_SUBTITLE)
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

static void OptimizeCues(ebml_master *Cues, array *Clusters, ebml_master *RSegmentInfo, filepos_t StartPos, ebml_master *WSegment, filepos_t TotalSize, bool_t ReLink, bool_t SafeClusters, stream *Input)
{
    matroska_cluster **Cluster;
    matroska_cuepoint *Cue;

    ReduceSize((ebml_element*)Cues);

	if (ReLink)
	{
		// link each Cue entry to the segment
		for (Cue = (matroska_cuepoint*)EBML_MasterChildren(Cues);Cue;Cue=(matroska_cuepoint*)EBML_MasterNext(Cue))
			MATROSKA_LinkCueSegmentInfo(Cue,RSegmentInfo);

		// link each Cue entry to the corresponding Block/SimpleBlock in the Cluster
		Cluster = NULL;
		for (Cue = (matroska_cuepoint*)EBML_MasterChildren(Cues);Cue;Cue=(matroska_cuepoint*)EBML_MasterNext(Cue))
			Cluster = LinkCueCluster(Cue,Clusters,Cluster,TotalSize);
		EndProgress();
	}

    // sort the Cues
    MATROSKA_CuesSort(Cues);

    SettleClustersWithCues(Clusters,StartPos,Cues,WSegment,SafeClusters, Input);
}

static ebml_element *CheckMatroskaHead(const ebml_element *Head, const ebml_parser_context *Parser, stream *Input)
{
    ebml_parser_context SubContext;
    ebml_element *SubElement;
    int UpperElement=0;
    tchar_t String[MAXLINE];

    SubContext.UpContext = Parser;
    SubContext.Context = EBML_ElementContext(Head);
    SubContext.EndPosition = EBML_ElementPositionEnd(Head);
    SubContext.Profile = Parser->Profile;
    SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
    while (SubElement)
    {
        if (EBML_ElementIsType(SubElement, &EBML_ContextReadVersion))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > EBML_MAX_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
        }
        else if (EBML_ElementIsType(SubElement, &EBML_ContextMaxIdLength))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > EBML_MAX_ID)
            {
                TextPrintf(StdErr,T("EBML Max ID Length %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
        }
        else if (EBML_ElementIsType(SubElement, &EBML_ContextMaxSizeLength))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > EBML_MAX_SIZE)
            {
                TextPrintf(StdErr,T("EBML Max Coded Size %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
        }
        else if (EBML_ElementIsType(SubElement, &EBML_ContextDocType))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
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
                    SrcProfile = PROFILE_WEBM;
                else
                {
                    TextPrintf(StdErr,T("EBML DocType '%s' not supported\r\n"),String);
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(SubElement, &EBML_ContextDocTypeReadVersion))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > MATROSKA_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
            else
                DocVersion = (int)EBML_IntegerValue((ebml_integer*)SubElement);
        }
        else if (EBML_ElementIsType(SubElement, &MATROSKA_ContextSegment))
            return SubElement;
        else
            EBML_ElementSkipData(SubElement,Input,NULL,NULL,0);
        NodeDelete((node*)SubElement);
        SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
    }

    return NULL;
}

static bool_t WriteCluster(ebml_master *Cluster, stream *Output, stream *Input, filepos_t PrevSize, timecode_t *PrevTimecode)
{
    filepos_t IntendedPosition = EBML_ElementPosition((ebml_element*)Cluster);
    ebml_element *Elt;
    bool_t CuesChanged = ReadClusterData(Cluster, Input);

    if (*PrevTimecode != INVALID_TIMECODE_T)
    {
        timecode_t OrigTimecode = MATROSKA_ClusterTimecode((matroska_cluster*)Cluster);
        if (*PrevTimecode >= OrigTimecode)
        {
            TextPrintf(StdErr,T("The Cluster at position %") TPRId64 T(" has the same timecode %") TPRId64 T(" as the previous cluster %") TPRId64 T(", incrementing\r\n"), EBML_ElementPosition((ebml_element*)Cluster),*PrevTimecode,OrigTimecode);
            MATROSKA_ClusterSetTimecode((matroska_cluster*)Cluster, *PrevTimecode + MATROSKA_ClusterTimecodeScale((matroska_cluster*)Cluster, 0));
            CuesChanged = 1;
        }
    }
    *PrevTimecode = MATROSKA_ClusterTimecode((matroska_cluster*)Cluster);

    EBML_ElementRender((ebml_element*)Cluster,Output,0,0,1,NULL);

    UnReadClusterData(Cluster, 1);

    if (!Live && EBML_ElementPosition((ebml_element*)Cluster) != IntendedPosition)
        TextPrintf(StdErr,T("Failed to write a Cluster at the required position %") TPRId64 T(" vs %") TPRId64 T("\r\n"), EBML_ElementPosition((ebml_element*)Cluster),IntendedPosition);
    if (!Live && PrevSize!=INVALID_FILEPOS_T)
    {
        Elt = EBML_MasterGetChild(Cluster, &MATROSKA_ContextPrevSize);
        if (Elt && PrevSize!=EBML_IntegerValue((ebml_integer*)Elt))
            TextPrintf(StdErr,T("The PrevSize of the Cluster at the position %") TPRId64 T(" is wrong: %") TPRId64 T(" vs %") TPRId64 T("\r\n"), EBML_ElementPosition((ebml_element*)Cluster),EBML_IntegerValue((ebml_integer*)Elt),PrevSize);
    }
    return CuesChanged;
}

static void MetaSeekUpdate(ebml_master *SeekHead)
{
    matroska_seekpoint *SeekPoint;
    for (SeekPoint=(matroska_seekpoint*)EBML_MasterChildren(SeekHead); SeekPoint; SeekPoint=(matroska_seekpoint*)EBML_MasterNext(SeekPoint))
        MATROSKA_MetaSeekUpdate(SeekPoint);
    EBML_ElementUpdateSize(SeekHead,0,0);
}

static ebml_master *GetMainTrack(ebml_master *Tracks, array *TrackOrder)
{
	ebml_master *Track;
	ebml_element *Elt;
	int64_t TrackNum = -1;
	size_t *order;
	
	if (TrackOrder)
		order = ARRAYBEGIN(*TrackOrder,size_t);
	else
		order = NULL;

	// find the video (first) track
	for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,&MATROSKA_ContextTrackEntry); Track; Track=(ebml_master*)EBML_MasterNextChild(Tracks,Track))
	{
		Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackType);
		if (EBML_IntegerValue((ebml_integer*)Elt) == TRACK_TYPE_VIDEO)
		{
			TrackNum = EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber));
			break;
		}
	}

	if (!Track)
	{
		// no video track found, look for an audio track
		for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,&MATROSKA_ContextTrackEntry); Track; Track=(ebml_master*)EBML_MasterNextChild(Tracks,Track))
		{
			Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackType);
			if (EBML_IntegerValue((ebml_integer*)Elt) == TRACK_TYPE_AUDIO)
			{
				TrackNum = EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber));
				break;
			}
		}
	}

	if (order)
	{
		for (Elt = EBML_MasterFindChild(Tracks,&MATROSKA_ContextTrackEntry); Elt; Elt=EBML_MasterNextChild(Tracks,Elt))
		{
			if (Elt!=(ebml_element*)Track)
				*order++ = (size_t)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextTrackNumber));
		}

		if (Track)
			*order++ = (size_t)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber));
	}

	if (!Track)
		TextPrintf(StdErr,T("Could not find an audio or video track to use as main track"));

	return Track;
}

static bool_t GenerateCueEntries(ebml_master *Cues, array *Clusters, ebml_master *Tracks, ebml_master *WSegmentInfo, ebml_element *RSegment, filepos_t TotalSize)
{
	ebml_master *Track;
	ebml_element *Elt;
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

	Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber);
	assert(Elt!=NULL);
	if (Elt)
		TrackNum = EBML_IntegerValue((ebml_integer*)Elt);

	// find all the keyframes
    ++CurrentPhase;
	for (Cluster = ARRAYBEGIN(*Clusters,ebml_element*);Cluster != ARRAYEND(*Clusters,ebml_element*); ++Cluster)
	{
        ShowProgress((ebml_element*)(*Cluster), TotalSize);
		MATROSKA_LinkClusterWriteSegmentInfo((matroska_cluster*)*Cluster,WSegmentInfo);
		for (Elt = EBML_MasterChildren(*Cluster); Elt; Elt = EBML_MasterNext(Elt))
		{
			Block = NULL;
			if (EBML_ElementIsType(Elt, &MATROSKA_ContextSimpleBlock))
			{
				if (MATROSKA_BlockKeyframe((matroska_block*)Elt))
					Block = (matroska_block*)Elt;
			}
			else if (EBML_ElementIsType(Elt, &MATROSKA_ContextBlockGroup))
			{
				ebml_element *EltB, *BlockRef = NULL;
				for (EltB = EBML_MasterChildren(Elt); EltB; EltB = EBML_MasterNext(EltB))
				{
					if (EBML_ElementIsType(EltB, &MATROSKA_ContextBlock))
						Block = (matroska_block*)EltB;
					else if (EBML_ElementIsType(EltB, &MATROSKA_ContextReferenceBlock))
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
    EndProgress();

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

static void CleanCropValues(ebml_master *Track, int64_t Width, int64_t Height)
{
    ebml_element *Left,*Right,*Top,*Bottom;
    Left = EBML_MasterGetChild(Track,&MATROSKA_ContextPixelCropLeft);
    Right = EBML_MasterGetChild(Track,&MATROSKA_ContextPixelCropRight);
    Top = EBML_MasterGetChild(Track,&MATROSKA_ContextPixelCropTop);
    Bottom = EBML_MasterGetChild(Track,&MATROSKA_ContextPixelCropBottom);
    if (EBML_IntegerValue((ebml_integer*)Top)+EBML_IntegerValue((ebml_integer*)Bottom) >= Height || EBML_IntegerValue((ebml_integer*)Left)+EBML_IntegerValue((ebml_integer*)Right) >= Width)
    {
        // invalid crop, remove the values
        NodeDelete((node*)Left);
        NodeDelete((node*)Right);
        NodeDelete((node*)Top);
        NodeDelete((node*)Bottom);
    }
    else
    {
        if (EBML_IntegerValue((ebml_integer*)Left)==0)   NodeDelete((node*)Left);
        if (EBML_IntegerValue((ebml_integer*)Right)==0)  NodeDelete((node*)Right);
        if (EBML_IntegerValue((ebml_integer*)Top)==0)    NodeDelete((node*)Top);
        if (EBML_IntegerValue((ebml_integer*)Bottom)==0) NodeDelete((node*)Bottom);
    }
}

static bool_t HasTrackUID(ebml_master *Tracks, int TrackUID, const ebml_context *UIDType)
{
    ebml_master *Track;
    for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,&MATROSKA_ContextTrackEntry);Track; Track = (ebml_master*)EBML_MasterNextChild(Tracks,Track))
    {
        ebml_element *Elt = EBML_MasterFindChild(Track,UIDType);
        if (Elt && EBML_IntegerValue((ebml_integer*)Elt)==TrackUID)
            return 1;
    }
    return 0;
}

static int CleanTracks(ebml_master *Tracks, int SrcProfile, int *DstProfile, ebml_master *Attachments, array *Alternate3DTracks)
{
    ebml_master *Track, *CurTrack, *OtherTrack;
    ebml_element *Elt, *Elt2, *DisplayW, *DisplayH;
    int TrackType, TrackNum, Width, Height;
    tchar_t CodecID[MAXPATH];
    
    for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,&MATROSKA_ContextTrackEntry); Track;)
    {
		CurTrack = Track;
		Track = (ebml_master*)EBML_MasterNextChild(Tracks,Track);
		Elt = EBML_MasterFindChild(CurTrack,&MATROSKA_ContextTrackNumber);
		if (!Elt)
		{
			TextPrintf(StdErr,T("The track at %") TPRId64 T(" has no number set!\r\n"),EBML_ElementPosition((ebml_element*)CurTrack));
			NodeDelete((node*)CurTrack);
			continue;
		}
		TrackNum = (int)EBML_IntegerValue((ebml_integer*)Elt);

		Elt = EBML_MasterFindChild(CurTrack,&MATROSKA_ContextCodecID);
		if (!Elt && !EBML_MasterFindChild(CurTrack,&MATROSKA_ContextTrackOperation))
		{
			TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has no CodecID set!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack));
			NodeDelete((node*)CurTrack);
			continue;
		}

		Elt = EBML_MasterFindChild(CurTrack,&MATROSKA_ContextTrackType);
		if (!Elt)
		{
			TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has no type set!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack));
			NodeDelete((node*)CurTrack);
			continue;
		}
        TrackType = (int)EBML_IntegerValue((ebml_integer*)Elt);

        if (ARRAYCOUNT(*Alternate3DTracks, block_info*) >= (size_t)TrackNum && TrackType!=TRACK_TYPE_VIDEO)
            ARRAYBEGIN(*Alternate3DTracks, block_info*)[TrackNum] = NULL;
		
        // clean the aspect ratio
        Elt = EBML_MasterFindChild(CurTrack,&MATROSKA_ContextVideo);
        if (Elt)
        {
            Width = (int)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextPixelWidth));
            Height = (int)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextPixelHeight));
	        if (Width==0 || Height==0)
	        {
		        TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid pixel dimensions %dx%d!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),Width,Height);
		        NodeDelete((node*)CurTrack);
		        continue;
	        }

            DisplayW = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextDisplayWidth);
            DisplayH = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextDisplayHeight);
            if (DisplayW || DisplayH)
            {
                Elt2 = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextDisplayUnit);
                if (Elt2 && EBML_IntegerValue((ebml_integer*)Elt2)==MATROSKA_DISPLAY_UNIT_DAR)
                {
                    // if the output size appears in pixel, fix it
                    if ((!DisplayW || Width==EBML_IntegerValue((ebml_integer*)DisplayW)) && (!DisplayH || Height==EBML_IntegerValue((ebml_integer*)DisplayH)))
                        EBML_IntegerSetValue((ebml_integer*)Elt2, MATROSKA_DISPLAY_UNIT_PIXEL);
                    else
                        CleanCropValues((ebml_master*)Elt, 0, 0);
                }

                if (!Elt2 || EBML_IntegerValue((ebml_integer*)Elt2)==MATROSKA_DISPLAY_UNIT_PIXEL) // pixel AR
                {
                    if (!DisplayW)
                    {
                        if (EBML_IntegerValue((ebml_integer*)DisplayH)==Height)
                        {
                            NodeDelete((node*)DisplayH);
                            DisplayH = NULL; // we don't the display values, they are the same as the pixel ones
                        }
                        else
                        {
                            DisplayW = EBML_MasterFindFirstElt((ebml_master*)Elt,&MATROSKA_ContextDisplayWidth,1,0);
                            EBML_IntegerSetValue((ebml_integer*)DisplayW,Width);
                        }
                    }
                    else if (EBML_IntegerValue((ebml_integer*)DisplayW)==0)
                    {
		                TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid display width %") TPRId64 T("!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),EBML_IntegerValue((ebml_integer*)DisplayW));
		                NodeDelete((node*)CurTrack);
		                continue;
                    }
                    else if (!DisplayH)
                    {
                        if (EBML_IntegerValue((ebml_integer*)DisplayW)==Width)
                        {
                            NodeDelete((node*)DisplayW);
                            DisplayW = NULL; // we don't the display values, they are the same as the pixel ones
                        }
                        else
                        {
                            DisplayH = EBML_MasterFindFirstElt((ebml_master*)Elt,&MATROSKA_ContextDisplayHeight,1,0);
                            EBML_IntegerSetValue((ebml_integer*)DisplayH,Height);
                        }
                    }
                    else if (EBML_IntegerValue((ebml_integer*)DisplayH)==0)
                    {
		                TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid display height %") TPRId64 T("!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),EBML_IntegerValue((ebml_integer*)DisplayH));
		                NodeDelete((node*)CurTrack);
		                continue;
                    }

                    if (DisplayW && DisplayH)
                    {
                        if (EBML_IntegerValue((ebml_integer*)DisplayH) < Height && EBML_IntegerValue((ebml_integer*)DisplayW) < Width) // Haali's non-pixel shrinking
                        {
                            int64_t DW = EBML_IntegerValue((ebml_integer*)DisplayW);
							int64_t DH = EBML_IntegerValue((ebml_integer*)DisplayH);
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
							    if (EBML_IntegerValue((ebml_integer*)DisplayW)*DH != EBML_IntegerValue((ebml_integer*)DisplayH)*DW)
							    {
								    Elt2 = EBML_MasterFindFirstElt((ebml_master*)Elt,&MATROSKA_ContextDisplayUnit, 1, 0);
								    if (Elt2)
								    {
									    EBML_IntegerSetValue((ebml_integer*)Elt2,MATROSKA_DISPLAY_UNIT_DAR);
									    EBML_IntegerSetValue((ebml_integer*)DisplayW,DW);
									    EBML_IntegerSetValue((ebml_integer*)DisplayH,DH);
								    }
							    }
                            }
                        }
                        if (DisplayH && EBML_IntegerValue((ebml_integer*)DisplayH) == Height)
                        {
                            NodeDelete((node*)DisplayH);
                            DisplayH = NULL;
                        }
                        if (DisplayW && EBML_IntegerValue((ebml_integer*)DisplayW) == Width)
                        {
                            NodeDelete((node*)DisplayW);
                            DisplayW = NULL;
                        }
                    }
                }
                Elt2 = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextDisplayUnit);
                if (EBML_IntegerValue((ebml_integer*)Elt2)!=MATROSKA_DISPLAY_UNIT_DAR)
                    CleanCropValues((ebml_master*)Elt, DisplayW?EBML_IntegerValue((ebml_integer*)DisplayW):Width, DisplayH?EBML_IntegerValue((ebml_integer*)DisplayH):Height);
            }

            if (SrcProfile==PROFILE_MATROSKA_V1 || SrcProfile==PROFILE_MATROSKA_V2 || SrcProfile==PROFILE_DIVX)
            {
                // clean the older StereoMode values
                Elt2 = EBML_MasterFindChild(Elt,&MATROSKA_ContextStereoMode);
                if (!Elt2)
                    Elt2 = EBML_MasterFindChild(Elt,&MATROSKA_ContextOldStereoMode);
                if (Elt2)
                {
                    Width = (int)EBML_IntegerValue((ebml_integer*)Elt2);
                    if (Width!=TRACK_OLD_STEREOMODE_MONO && Width <= 3) // upper values are probably the new ones
                    {
                        *DstProfile = PROFILE_MATROSKA_V3;
                        TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" is using an old StereoMode value, converting to profile '%s'\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),GetProfileName(*DstProfile));
                        if (EBML_ElementIsType(Elt2, &MATROSKA_ContextOldStereoMode))
                            // replace the old by a new
                            Elt2->Context = &MATROSKA_ContextStereoMode;

                        // replace the old values with the new ones
                        if (Width==TRACK_OLD_STEREOMODE_BOTH)
                        {
                            TextPrintf(StdErr,T("  turning 'Both Eyes' into 'side by side (left first)\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),GetProfileName(*DstProfile));
                            EBML_IntegerSetValue((ebml_integer*)Elt2,TRACK_STEREO_MODE_SIDEBYSIDE_L);
                        }
                        else
                        {
                            EBML_IntegerSetValue((ebml_integer*)Elt2,TRACK_STEREO_MODE_MONO);
                            TextPrintf(StdErr,T("  turning %s eye to mono\r\n"), Width==TRACK_OLD_STEREOMODE_LEFT?T("left"):T("right"));
                            // look for the other track
                            for (OtherTrack = (ebml_master*)EBML_MasterNextChild(Tracks,CurTrack);OtherTrack; OtherTrack = (ebml_master*)EBML_MasterNextChild(Tracks,OtherTrack))
                            {
                                ebml_element *VidElt = EBML_MasterFindChild(OtherTrack,&MATROSKA_ContextVideo);
                                if (VidElt)
                                {
                                    // TODO: only use a video track that has the same output pixels as the source
                                    ebml_element *OtherStereo = EBML_MasterFindChild(VidElt,&MATROSKA_ContextStereoMode);
                                    if (!OtherStereo)
                                        OtherStereo = EBML_MasterFindChild(VidElt,&MATROSKA_ContextOldStereoMode);
                                    if (OtherStereo && ((Width==TRACK_OLD_STEREOMODE_LEFT && EBML_IntegerValue((ebml_integer*)OtherStereo)==TRACK_OLD_STEREOMODE_RIGHT) ||
                                        (Width==TRACK_OLD_STEREOMODE_RIGHT && EBML_IntegerValue((ebml_integer*)OtherStereo)==TRACK_OLD_STEREOMODE_LEFT)))
                                    {
                                        ebml_master *CombinedTrack;
                                        int NewTrackUID;

                                        TextPrintf(StdErr,T("  turning matching %s eye to mono and creating a new combined track\r\n"), Width==TRACK_OLD_STEREOMODE_RIGHT?T("left"):T("right"));
                                        EBML_IntegerSetValue((ebml_integer*)OtherStereo,TRACK_STEREO_MODE_MONO);

                                        // create another track that is this one combined
                                        CombinedTrack = (ebml_master*)EBML_ElementCopy(CurTrack, NULL);
                                        EBML_MasterAppend(Tracks, (ebml_element*)CombinedTrack);

                                        // set a new TrackNumber
                                        NewTrackUID = TrackNum+1;
                                        while (HasTrackUID(Tracks, NewTrackUID, &MATROSKA_ContextTrackNumber))
                                            ++NewTrackUID;
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextTrackNumber);
                                        EBML_IntegerSetValue((ebml_integer*)Elt,NewTrackUID);

                                        // set a new TrackUID
                                        NewTrackUID = 1;
                                        while (HasTrackUID(Tracks, NewTrackUID, &MATROSKA_ContextTrackUID))
                                            ++NewTrackUID;
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextTrackUID);
                                        EBML_IntegerSetValue((ebml_integer*)Elt,NewTrackUID);

                                        // cleaning
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextFlagEnabled);
                                        if (Elt)
                                            EBML_IntegerSetValue((ebml_integer*)Elt,1);
                                        
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextFlagLacing);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextMinCache);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextMaxCache);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextDefaultDuration);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextTrackTimecodeScale);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextMaxBlockAdditionID);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterGetChild(CombinedTrack,&MATROSKA_ContextCodecID);
                                        EBML_StringSetValue((ebml_string*)Elt,"V_COMBINED");
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextCodecPrivate);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextAttachmentLink);
                                        NodeDelete((node*)Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,&MATROSKA_ContextTrackTranslate);
                                        NodeDelete((node*)Elt);

                                        // add the TrackCombine info
                                        Elt = EBML_MasterAddElt(CombinedTrack, &MATROSKA_ContextTrackOperation, 1);
                                        if (Elt)
                                            Elt = EBML_MasterAddElt((ebml_master*)Elt, &MATROSKA_ContextTrackCombinePlanes, 1);
                                        if (Elt)
                                        {
                                            // track 1
                                            CombinedTrack = (ebml_master*)EBML_MasterAddElt((ebml_master*)Elt, &MATROSKA_ContextTrackPlane, 1);
                                            OtherStereo = EBML_MasterFindChild(CurTrack, &MATROSKA_ContextTrackUID);
                                            if (!OtherStereo)
                                            {
                                                OtherStereo = EBML_MasterAddElt((ebml_master*)CurTrack, &MATROSKA_ContextTrackUID, 1);
                                                while (HasTrackUID(Tracks, NewTrackUID, &MATROSKA_ContextTrackUID))
                                                    ++NewTrackUID;
                                                EBML_IntegerSetValue((ebml_integer*)OtherStereo, NewTrackUID);
                                            }
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, &MATROSKA_ContextTrackPlaneUID);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, EBML_IntegerValue((ebml_integer*)OtherStereo));
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, &MATROSKA_ContextTrackPlaneType);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, Width==TRACK_OLD_STEREOMODE_LEFT? TRACK_PLANE_LEFT : TRACK_PLANE_RIGHT);

                                            // track 2
                                            CombinedTrack = (ebml_master*)EBML_MasterAddElt((ebml_master*)Elt, &MATROSKA_ContextTrackPlane, 1);
                                            OtherStereo = EBML_MasterFindChild(OtherTrack, &MATROSKA_ContextTrackUID);
                                            if (!OtherStereo)
                                            {
                                                OtherStereo = EBML_MasterAddElt((ebml_master*)OtherTrack, &MATROSKA_ContextTrackUID, 1);
                                                while (HasTrackUID(Tracks, NewTrackUID, &MATROSKA_ContextTrackUID))
                                                    ++NewTrackUID;
                                                EBML_IntegerSetValue((ebml_integer*)OtherStereo, NewTrackUID);
                                            }
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, &MATROSKA_ContextTrackPlaneUID);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, EBML_IntegerValue((ebml_integer*)OtherStereo));
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, &MATROSKA_ContextTrackPlaneType);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, Width==TRACK_OLD_STEREOMODE_RIGHT? TRACK_PLANE_LEFT : TRACK_PLANE_RIGHT);
                                        }

                                        return CleanTracks(Tracks, SrcProfile, DstProfile, Attachments, Alternate3DTracks);
                                    }
                                }
                            }

                            TextPrintf(StdErr,T("  could not find the matching %s track!\r\n"), Width==TRACK_OLD_STEREOMODE_RIGHT?T("left"):T("right"));
                        }
                    }
                }
            }
        }

        if (ARRAYCOUNT(*Alternate3DTracks, block_info*) >= (size_t)TrackNum && ARRAYBEGIN(*Alternate3DTracks, block_info*)[TrackNum])
        {
            // force the proper StereoMode value
            Elt2 = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextStereoMode);
            EBML_IntegerSetValue((ebml_integer*)Elt2, TRACK_STEREO_MODE_ALTERNATE_PACKED_L);
        }

		if (*DstProfile==PROFILE_WEBM)
		{
	        // verify that we have only VP8 and Vorbis tracks
			Elt = EBML_MasterFindChild(CurTrack,&MATROSKA_ContextCodecID);
			EBML_StringGet((ebml_string*)Elt,CodecID,TSIZEOF(CodecID));
			if (!(TrackType==TRACK_TYPE_VIDEO && tcsisame_ascii(CodecID,T("V_VP8")) || (TrackType==TRACK_TYPE_AUDIO && tcsisame_ascii(CodecID,T("A_VORBIS")))))
			{
				TextPrintf(StdErr,T("Wrong codec '%s' for profile '%s' removing track %d\r\n"),CodecID,GetProfileName(*DstProfile),TrackNum);
				NodeDelete((node*)CurTrack);
                continue;
			}
		}

        // clean the output sampling freq
        Elt = EBML_MasterFindChild(CurTrack,&MATROSKA_ContextAudio);
        if (Elt)
        {
            Elt2 = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextOutputSamplingFrequency);
            if (Elt2)
            {
                DisplayH = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextSamplingFrequency);
                assert(DisplayH!=NULL);
                if (EBML_FloatValue((ebml_float*)Elt2) == EBML_FloatValue((ebml_float*)DisplayH))
                    NodeDelete((node*)Elt2);
            }
        }

        // clean the attachment links
        Elt = EBML_MasterFindChild(CurTrack,&MATROSKA_ContextAttachmentLink);
        while (Elt)
        {
            Elt2 = NULL;
            if (!Attachments)
                Elt2 = Elt;
            else
            {
                Elt2 = EBML_MasterFindChild(Attachments,&MATROSKA_ContextAttachedFile);
                while (Elt2)
                {
                    DisplayH = EBML_MasterFindChild((ebml_master*)Elt2,&MATROSKA_ContextFileUID);
                    if (DisplayH && EBML_IntegerValue((ebml_integer*)DisplayH)==EBML_IntegerValue((ebml_integer*)Elt))
                        break;
                    Elt2 = EBML_MasterNextChild(Attachments, Elt2);
                }
                if (!Elt2) // the attachment wasn't found, delete Elt
                    Elt2 = Elt;
                else
                    Elt2 = NULL;
            }

            Elt = EBML_MasterNextChild(CurTrack, Elt);
            if (Elt2)
                NodeDelete((node*)Elt2);
        }
    }

    // disable Alternate3DTracks handling if there is no matching track
    TrackNum = -1;
    for (TrackType=0; TrackType<ARRAYCOUNT(*Alternate3DTracks, block_info*); ++TrackType)
        if (ARRAYBEGIN(*Alternate3DTracks, block_info*)[TrackType])
        {
            TrackNum = TrackType;
            break;
        }
    if (TrackNum==-1)
        ArrayClear(Alternate3DTracks);

    if (EBML_MasterFindChild(Tracks,&MATROSKA_ContextTrackEntry)==NULL)
        return -19;

    // put the TrackNumber, TrackType and CodecId at the front of the Track elements
    for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,&MATROSKA_ContextTrackEntry); Track; Track = (ebml_master*)EBML_MasterNextChild(Tracks,Track))
    {
        Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber);
        if (Elt)
            NodeTree_SetParent(Elt,Track,EBML_MasterChildren(Track));

        Elt2 = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackType);
        assert(Elt2!=NULL);
        NodeTree_SetParent(Elt2,Track,EBML_MasterNext(Elt));

        DisplayW = EBML_MasterFindChild(Track,&MATROSKA_ContextCodecID);
        if (DisplayW)
            NodeTree_SetParent(DisplayW,Track,EBML_MasterNext(Elt2));
    }

    return 0;
}

static void InitCommonHeader(array *TrackHeader)
{
    // special mark to tell the header has not been used yet
    TrackHeader->_Begin = TABLE_MARKER;
}

static bool_t BlockIsCompressed(const matroska_block *Block)
{
    ebml_master *Track = (ebml_master*)MATROSKA_BlockReadTrack(Block);
    if (Track)
    {
        ebml_master *Elt = (ebml_master*)EBML_MasterFindChild(Track, &MATROSKA_ContextContentEncodings);
        if (Elt)
        {
            Elt = (ebml_master*)EBML_MasterFindChild(Elt, &MATROSKA_ContextContentEncoding);
            if (EBML_MasterChildren(Elt))
            {
                if (EBML_MasterNext(Elt))
                    return 1; // we don't support cascased encryption/compression

                Elt = (ebml_master*)EBML_MasterFindChild(Elt, &MATROSKA_ContextContentCompression);
                if (!Elt)
                    return 1; // we don't support encryption

                Elt = (ebml_master*)EBML_MasterGetChild(Elt, &MATROSKA_ContextContentCompAlgo);
                if (EBML_IntegerValue((ebml_integer*)Elt)==MATROSKA_BLOCK_COMPR_ZLIB)
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

    if (BlockIsCompressed(Block))
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
    MATROSKA_BlockReleaseData(Block,1);
}

static void ClearCommonHeader(array *TrackHeader)
{
    if (TrackHeader->_Begin == TABLE_MARKER)
        TrackHeader->_Begin = NULL;
}

static void WriteJunk(stream *Output, size_t Amount)
{
	char Val = 0x0A;
	while (Amount--)
		Stream_Write(Output,&Val,1,NULL);
}

#if defined(TARGET_WIN) && defined(UNICODE)
int wmain(int argc, const wchar_t *argv[])
#else
int main(int argc, const char *argv[])
#endif
{
    int i,Result = 0;
    int ShowUsage = 0;
    int ShowVersion = 0;
    parsercontext p;
    textwriter _StdErr;
    stream *Input = NULL,*Output = NULL;
    tchar_t Path[MAXPATHFULL];
    tchar_t String[MAXLINE],Original[MAXLINE],*s;
    ebml_master *EbmlHead = NULL, *RSegment = NULL, *RLevel1 = NULL, **Cluster;
    ebml_master *RSegmentInfo = NULL, *RTrackInfo = NULL, *RChapters = NULL, *RTags = NULL, *RCues = NULL, *RAttachments = NULL;
    ebml_master *WSegment = NULL, *WMetaSeek = NULL, *WSegmentInfo = NULL, *WTrackInfo = NULL;
    ebml_element *Elt, *Elt2;
    matroska_seekpoint *WSeekPoint = NULL, *W1stClusterSeek = NULL;
    ebml_string *LibName, *AppName;
    array RClusters, WClusters, *Clusters, WTracks;
    ebml_parser_context RContext;
    ebml_parser_context RSegmentContext;
    int UpperElement;
    filepos_t MetaSeekBefore, MetaSeekAfter;
    filepos_t NextPos = 0, SegmentSize = 0, ClusterSize, CuesSize;
    size_t ExtraVoidSize = 0;
    timecode_t PrevTimecode;
    bool_t CuesChanged;
	bool_t KeepCues = 0, Remux = 0, CuesCreated = 0, Optimize = 0, OptimizeVideo = 1, UnOptimize = 0, ClustersNeedRead = 0, Regression = 0;
    int InputPathIndex = 1;
	int64_t TimeCodeScale = 0, OldTimeCodeScale;
    size_t MaxTrackNum = 0;
    array TrackMaxHeader; // array of uint8_t (max common header)
    filepos_t TotalSize;
    array Alternate3DTracks;

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
    ArrayInit(&Alternate3DTracks);
	Clusters = &RClusters;

    StdErr = &_StdErr;
    memset(StdErr,0,sizeof(_StdErr));
    StdErr->Stream = (stream*)NodeSingleton(&p,STDERR_ID);

#if defined(TARGET_WIN) && defined(UNICODE)
    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[0]);
#else
    Node_FromStr(&p,Path,TSIZEOF(Path),argv[0]);
#endif
    SplitPath(Path,NULL,0,String,TSIZEOF(String),NULL,0);
    UnOptimize = tcsisame_ascii(String,T("mkWDclean"));
    if (UnOptimize)
        TextPrintf(StdErr,T("Running special mkWDclean mode, please fix your player instead of valid Matroska files\r\n"));
	Path[0] = 0;

	for (i=1;i<argc;++i)
	{
#if defined(TARGET_WIN) && defined(UNICODE)
	    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[i]);
#else
	    Node_FromStr(&p,Path,TSIZEOF(Path),argv[i]);
#endif
		if (tcsisame_ascii(Path,T("--keep-cues"))) { KeepCues = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--remux"))) { Remux = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--live"))) { Live = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--doctype")) && i+1<argc-1)
		{
#if defined(TARGET_WIN) && defined(UNICODE)
		    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[++i]);
#else
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
#endif
			if (tcsisame_ascii(Path,T("1")))
				DstProfile = PROFILE_MATROSKA_V1;
			else if (tcsisame_ascii(Path,T("2")))
				DstProfile = PROFILE_MATROSKA_V2;
			else if (tcsisame_ascii(Path,T("3")))
				DstProfile = PROFILE_MATROSKA_V3;
			else if (tcsisame_ascii(Path,T("4")))
				DstProfile = PROFILE_WEBM;
			else if (tcsisame_ascii(Path,T("5")))
				DstProfile = PROFILE_DIVX;
			else if (tcsisame_ascii(Path,T("6")))
				DstProfile = PROFILE_MATROSKA_V4;
			else
			{
		        TextPrintf(StdErr,T("Unknown doctype %s\r\n"),Path);
                Path[0] = 0;
				Result = -8;
				goto exit;
			}
			InputPathIndex = i+1;
		}
		else if (tcsisame_ascii(Path,T("--timecodescale")) && i+1<argc-1)
		{
#if defined(TARGET_WIN) && defined(UNICODE)
		    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[++i]);
#else
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
#endif
			TimeCodeScale = StringToInt(Path,0);
			InputPathIndex = i+1;
		}
		else if (tcsisame_ascii(Path,T("--alt-3d")) && i+1<argc-1)
		{
            size_t TrackId;
#if defined(TARGET_WIN) && defined(UNICODE)
		    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[++i]);
#else
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
#endif
            TrackId = StringToInt(Path,0);
            if (ARRAYCOUNT(Alternate3DTracks, block_info*) < (TrackId+1))
            {
                size_t OldSize = ARRAYCOUNT(Alternate3DTracks, block_info*);
			    ArrayResize(&Alternate3DTracks, (TrackId+1)*sizeof(block_info*),64);
                memset(ARRAYBEGIN(Alternate3DTracks, block_info*)+OldSize, 0, (TrackId-OldSize+1)*sizeof(block_info*));
            }
            ARRAYBEGIN(Alternate3DTracks, block_info*)[TrackId] = MARKER3D;
			InputPathIndex = i+1;
		}
		else if (tcsisame_ascii(Path,T("--unsafe"))) { Unsafe = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--optimize"))) { Optimize = 1; OptimizeVideo = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--optimize_nv"))) { Optimize = 1; OptimizeVideo = 0; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--regression"))) { Regression = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--no-optimize"))) { UnOptimize = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--quiet"))) { Quiet = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--version"))) { ShowVersion = 1; InputPathIndex = i+1; }
        else if (tcsisame_ascii(Path,T("--help"))) {ShowVersion = 1; ShowUsage = 1; InputPathIndex = i+1; }
		else if (i<argc-2) TextPrintf(StdErr,T("Unknown parameter '%s'\r\n"),Path);
	}
    
    if (argc < (1+InputPathIndex) || ShowVersion)
    {
        TextWrite(StdErr,T("mkclean v") PROJECT_VERSION T(", Copyright (c) 2010-2011 Matroska Foundation\r\n"));
        if (argc < 2 || ShowUsage)
        {
            TextWrite(StdErr,T("Usage: mkclean [options] <matroska_src> [matroska_dst]\r\n"));
		    TextWrite(StdErr,T("Options:\r\n"));
		    TextWrite(StdErr,T("  --keep-cues   keep the original Cues content and move it to the front\r\n"));
		    TextWrite(StdErr,T("  --remux       redo the Clusters layout\r\n"));
		    TextWrite(StdErr,T("  --doctype <v> force the doctype version\r\n"));
		    TextWrite(StdErr,T("    1: 'matroska' v1\r\n"));
		    TextWrite(StdErr,T("    2: 'matroska' v2\r\n"));
		    TextWrite(StdErr,T("    3: 'matroska' v3\r\n"));
		    TextWrite(StdErr,T("    4: 'webm'\r\n"));
		    TextWrite(StdErr,T("    5: 'matroska' v1 with DivX extensions\r\n"));
		    TextWrite(StdErr,T("  --live        the output file resembles a live stream\r\n"));
		    TextWrite(StdErr,T("  --timecodescale <v> force the global TimecodeScale to <v> (1000000 is a good value)\r\n"));
		    TextWrite(StdErr,T("  --unsafe      don't output elements that are used for file recovery (saves more space)\r\n"));
		    TextWrite(StdErr,T("  --optimize    use all possible optimization for the output file\r\n"));
		    TextWrite(StdErr,T("  --optimize_nv use all possible optimization for the output file, except video tracks\r\n"));
		    TextWrite(StdErr,T("  --no-optimize disable some optimization for the output file\r\n"));
		    TextWrite(StdErr,T("  --regression  the output file is suitable for regression tests\r\n"));
            TextWrite(StdErr,T("  --alt-3d <t>  the track with ID <v> has alternate 3D fields (left first)\r\n"));
		    TextWrite(StdErr,T("  --quiet       only output errors\r\n"));
            TextWrite(StdErr,T("  --version     show the version of mkclean\r\n"));
            TextWrite(StdErr,T("  --help        show this screen\r\n"));
        }
        Path[0] = 0;
        Result = -1;
        goto exit;
    }

#if defined(TARGET_WIN) && defined(UNICODE)
    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[InputPathIndex]);
#else
    Node_FromStr(&p,Path,TSIZEOF(Path),argv[InputPathIndex]);
#endif
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Path[0] = 0;
        Result = -2;
        goto exit;
    }

    TotalSize = Stream_Seek(Input, 0, SEEK_END);
    Stream_Seek(Input, 0, SEEK_SET);

    if (InputPathIndex==argc-1)
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
#if defined(TARGET_WIN) && defined(UNICODE)
        Node_FromWcs(&p,Path,TSIZEOF(Path),argv[argc-1]);
#else
        Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
#endif
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
    RContext.Profile = 0;
    EbmlHead = (ebml_master*)EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
    if (!EbmlHead || !EBML_ElementIsType((ebml_element*)EbmlHead, &EBML_ContextHead))
    {
        TextWrite(StdErr,T("EBML head not found! Are you sure it's a matroska/webm file?\r\n"));
        Result = -4;
        goto exit;
    }

    RSegment = (ebml_master*)CheckMatroskaHead((ebml_element*)EbmlHead,&RContext,Input);
    if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==2)
        SrcProfile = PROFILE_MATROSKA_V2;
    else if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==3)
        SrcProfile = PROFILE_MATROSKA_V3;
    else if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==4)
        SrcProfile = PROFILE_MATROSKA_V4;

	if (!DstProfile)
		DstProfile = SrcProfile;

    if (DstProfile==PROFILE_WEBM)
    {
        UnOptimize = 1;
        Optimize = 0;
    }

    if (!RSegment)
    {
        Result = -5;
        goto exit;
    }
    NodeDelete((node*)EbmlHead);
    EbmlHead = NULL;

    if (Unsafe)
        ++TotalPhases;
    if (!Live)
        ++TotalPhases;

    if (EBML_ElementPositionEnd((ebml_element*)RSegment) != INVALID_FILEPOS_T)
        TotalSize = EBML_ElementPositionEnd((ebml_element*)RSegment);

    RContext.EndPosition = TotalSize; // avoid reading too far some dummy/void elements for this segment

    // locate the Segment Info, Track Info, Chapters, Tags, Attachments, Cues Clusters*
    RSegmentContext.Context = &MATROSKA_ContextSegment;
    RSegmentContext.EndPosition = EBML_ElementPositionEnd((ebml_element*)RSegment);
    RSegmentContext.UpContext = &RContext;
    RSegmentContext.Profile = SrcProfile;
	UpperElement = 0;
//TextPrintf(StdErr,T("Loading the level1 elements in memory\r\n"));
    RLevel1 = (ebml_master*)EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    while (RLevel1)
    {
        ShowProgress((ebml_element*)RLevel1, TotalSize);
        if (EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextInfo))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RSegmentInfo = RLevel1;
        }
        else if (EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextTracks))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RTrackInfo = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextChapters))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RChapters = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextTags))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RTags = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextCues) && KeepCues)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RCues = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextAttachments))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RAttachments = RLevel1;
        }
        else if (EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextCluster))
        {
			// only partially read the Cluster data (not the data inside the blocks)
            if (EBML_ElementReadData((ebml_element*)RLevel1,Input,&RSegmentContext,!Remux,SCOPE_PARTIAL_DATA,0)==ERR_NONE)
			{
                ArrayAppend(&RClusters,&RLevel1,sizeof(RLevel1),256);
				// remove MATROSKA_ContextPosition and MATROSKA_ContextPrevSize until supported
				EbmlHead = (ebml_master*)EBML_MasterFindChild(RLevel1, &MATROSKA_ContextPosition);
				if (EbmlHead)
					NodeDelete((node*)EbmlHead);
				EbmlHead = (ebml_master*)EBML_MasterFindChild(RLevel1, &MATROSKA_ContextPrevSize);
				if (EbmlHead)
					NodeDelete((node*)EbmlHead);
				EbmlHead = NULL;
                RLevel1 = (ebml_master*)EBML_ElementSkipData((ebml_element*)RLevel1, Input, &RSegmentContext, NULL, 1);
                if (RLevel1 != NULL)
                    continue;
			}
        }
        else
		{
			EbmlHead = (ebml_master*)EBML_ElementSkipData((ebml_element*)RLevel1, Input, &RSegmentContext, NULL, 1);
			assert(EbmlHead==NULL);
            NodeDelete((node*)RLevel1);
		}
        RLevel1 = (ebml_master*)EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    }
    EndProgress();

    if (!RSegmentInfo)
    {
        TextWrite(StdErr,T("The source Segment has no Segment Info section\r\n"));
        Result = -6;
        goto exit;
    }
    WSegmentInfo = (ebml_master*)EBML_ElementCopy(RSegmentInfo, NULL);
    EBML_MasterUseChecksum(WSegmentInfo,!Unsafe);

	RLevel1 = (ebml_master*)EBML_MasterGetChild(WSegmentInfo,&MATROSKA_ContextTimecodeScale);
	if (!RLevel1)
	{
		TextWrite(StdErr,T("Failed to get the TimeCodeScale handle\r\n"));
		Result = -10;
		goto exit;
	}
    OldTimeCodeScale = EBML_IntegerValue((ebml_integer*)RLevel1);
    if (TimeCodeScale==0)
    {
        // avoid using a TimeCodeScale too small
        TimeCodeScale = OldTimeCodeScale;
        while (TimeCodeScale < 100000)
            TimeCodeScale <<= 1;
    }
    EBML_IntegerSetValue((ebml_integer*)RLevel1,TimeCodeScale);
	RLevel1 = NULL;

    if (Live)
    {
	    // remove MATROSKA_ContextDuration from Live streams
	    ebml_float *Duration = (ebml_float*)EBML_MasterFindChild(WSegmentInfo, &MATROSKA_ContextDuration);
	    if (Duration)
		    NodeDelete((node*)Duration);
    }
    else if (TimeCodeScale && TimeCodeScale != OldTimeCodeScale)
    {
	    ebml_float *Duration = (ebml_float*)EBML_MasterFindChild(WSegmentInfo, &MATROSKA_ContextDuration);
        if (Duration)
        {
            double duration = EBML_FloatValue(Duration);
            EBML_FloatSetValue(Duration, (duration * OldTimeCodeScale) / TimeCodeScale);
        }
    }

    // reorder elements in WSegmentInfo
    Elt2 = EBML_MasterFindChild(WSegmentInfo, &MATROSKA_ContextTimecodeScale);
    if (Elt2)
        NodeTree_SetParent(Elt2,WSegmentInfo,EBML_MasterChildren(WSegmentInfo));
    if (!Elt2)
        Elt2 = EBML_MasterChildren(WSegmentInfo);
    Elt = EBML_MasterFindChild(WSegmentInfo, &MATROSKA_ContextDuration);
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
		Result = CleanTracks(RTrackInfo, SrcProfile, &DstProfile, RAttachments, &Alternate3DTracks);
		if (Result!=0)
		{
			TextWrite(StdErr,T("No Tracks left to use!\r\n"));
			goto exit;
		}
		WTrackInfo = (ebml_master*)EBML_ElementCopy(RTrackInfo, NULL);
		if (WTrackInfo==NULL)
		{
			TextWrite(StdErr,T("Failed to copy the track info!\r\n"));
			goto exit;
		}
        EBML_MasterUseChecksum(WTrackInfo,!Unsafe);

		// count the max track number
		for (Elt=EBML_MasterChildren(WTrackInfo); Elt; Elt=EBML_MasterNext(Elt))
		{
			if (EBML_ElementIsType(Elt, &MATROSKA_ContextTrackEntry))
			{
                EBML_MasterUseChecksum((ebml_master*)Elt,!Unsafe);
				Elt2 = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextTrackNumber);
				if (Elt2 && (size_t)EBML_IntegerValue((ebml_integer*)Elt2) > MaxTrackNum)
					MaxTrackNum = (size_t)EBML_IntegerValue((ebml_integer*)Elt2);
			}
		}

		// make sure the lacing flag is set on tracks that use it
		i = -1;
		for (Elt = EBML_MasterChildren(WTrackInfo);Elt;Elt=EBML_MasterNext(Elt))
		{
			Elt2 = EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextTrackNumber);
            if (Elt2)
			    i = max(i,(int)EBML_IntegerValue((ebml_integer*)Elt2));
		}
		ArrayResize(&WTracks,sizeof(track_info)*(i+1),0);
		ArrayZero(&WTracks);
	}

    // Write the EBMLHead
    EbmlHead = (ebml_master*)EBML_ElementCreate(&p,&EBML_ContextHead,0,NULL);
    if (!EbmlHead)
        goto exit;
    EBML_MasterUseChecksum(EbmlHead,!Unsafe);
    NodeTree_Clear((nodetree*)EbmlHead); // remove the default values
    // DocType
    RLevel1 = (ebml_master*)EBML_MasterGetChild(EbmlHead,&EBML_ContextDocType);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_STRING_CLASS));
    if (DstProfile == PROFILE_WEBM)
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
	if (DstProfile==PROFILE_MATROSKA_V2 || DstProfile==PROFILE_WEBM)
		DocVersion=2;
	if (DstProfile==PROFILE_MATROSKA_V3)
		DocVersion=3;
	if (DstProfile==PROFILE_MATROSKA_V4)
		DocVersion=4;

    if (ARRAYCOUNT(Alternate3DTracks, block_info*) && DstProfile!=PROFILE_MATROSKA_V3 && DstProfile!=PROFILE_MATROSKA_V4)
    {
        TextPrintf(StdErr,T("Using --alt-3d in profile '%s' try \"--doctype %d\"\r\n"),GetProfileName(DstProfile),GetProfileId(PROFILE_MATROSKA_V3));
        goto exit;
    }

    RLevel1 = (ebml_master*)EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    EBML_IntegerSetValue((ebml_integer*)RLevel1, DocVersion);

    // Doctype readable version
    RLevel1 = (ebml_master*)EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeReadVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    EBML_IntegerSetValue((ebml_integer*)RLevel1, DocVersion);

    if (EBML_ElementRender((ebml_element*)EbmlHead,Output,1,0,1,NULL)!=ERR_NONE)
        goto exit;
    NodeDelete((node*)EbmlHead);
    EbmlHead = NULL;
    RLevel1 = NULL;

    // Write the Matroska Segment Head
    WSegment = (ebml_master*)EBML_ElementCreate(&p,&MATROSKA_ContextSegment,0,NULL);
	if (Live)
		EBML_ElementSetInfiniteSize((ebml_element*)WSegment,1);
	else
    {
        // temporary value
        if (EBML_ElementIsFiniteSize((ebml_element*)RSegment))
		    EBML_ElementForceDataSize((ebml_element*)WSegment, EBML_ElementDataSize((ebml_element*)RSegment,0));
        else
            EBML_ElementSetSizeLength((ebml_element*)WSegment, EBML_MAX_SIZE);
    }
    if (EBML_ElementRenderHead((ebml_element*)WSegment,Output,0,NULL)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the (temporary) Segment head\r\n"));
        Result = -10;
        goto exit;
    }

    //  Compute the Segment Info size
    ReduceSize((ebml_element*)WSegmentInfo);
    // change the library names & app name
    stprintf_s(String,TSIZEOF(String),T("%s + %s"),Node_GetDataStr((node*)&p,CONTEXT_LIBEBML_VERSION),Node_GetDataStr((node*)&p,CONTEXT_LIBMATROSKA_VERSION));
    LibName = (ebml_string*)EBML_MasterFindFirstElt(WSegmentInfo, &MATROSKA_ContextMuxingApp, 1, 0);
    EBML_StringGet(LibName,Original,TSIZEOF(Original));
    if (Regression)
        EBML_UniStringSetValue(LibName,T("libebml2 + libmatroska2"));
    else
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
    if (Regression)
    {
        if (s[0])
            stprintf_s(String,TSIZEOF(String),T("mkclean regression from %s"),s);
        else
            stprintf_s(String,TSIZEOF(String),T("mkclean regression"));
    }
    else
    {
	    stprintf_s(String,TSIZEOF(String),T("mkclean %s"),PROJECT_VERSION);
	    if (Remux || Optimize || Live || UnOptimize || ARRAYCOUNT(Alternate3DTracks, block_info*))
		    tcscat_s(String,TSIZEOF(String),T(" "));
	    if (Remux)
		    tcscat_s(String,TSIZEOF(String),T("r"));
	    if (Optimize)
		    tcscat_s(String,TSIZEOF(String),T("o"));
	    if (Live)
		    tcscat_s(String,TSIZEOF(String),T("l"));
	    if (UnOptimize)
		    tcscat_s(String,TSIZEOF(String),T("u"));
	    if (ARRAYCOUNT(Alternate3DTracks, block_info*))
        {
		    tcscat_s(String,TSIZEOF(String),T("3"));
            Remux = 1;
        }
	    if (s[0])
		    stcatprintf_s(String,TSIZEOF(String),T(" from %s"),s);
    }
    EBML_UniStringSetValue(AppName,String);
	ExtraSizeDiff = tcslen(String) - ExtraSizeDiff + 2;

	if (Regression || Remux || !EBML_MasterFindChild(WSegmentInfo, &MATROSKA_ContextDateUTC))
	{
		RLevel1 = (ebml_master*)EBML_MasterGetChild(WSegmentInfo, &MATROSKA_ContextDateUTC);
        if (Regression)
            EBML_DateSetDateTime((ebml_date*)RLevel1, 1);
        else
		    EBML_DateSetDateTime((ebml_date*)RLevel1, GetTimeDate());
		RLevel1 = NULL;
	}

	if (!Live)
	{
		//  Prepare the Meta Seek with average values
		WMetaSeek = (ebml_master*)EBML_MasterAddElt(WSegment,&MATROSKA_ContextSeekHead,0);
        EBML_MasterUseChecksum(WMetaSeek,!Unsafe);
		EBML_ElementForcePosition((ebml_element*)WMetaSeek, Stream_Seek(Output,0,SEEK_CUR)); // keep the position for when we need to write it
        NextPos = 38 + 4* (Unsafe ? 17 : 23); // raw estimation of the SeekHead size
        if (RAttachments)
            NextPos += Unsafe ? 18 : 24;
        if (RChapters)
            NextPos += Unsafe ? 17 : 23;

		// segment info
		WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
		EBML_ElementForcePosition((ebml_element*)WSegmentInfo, NextPos);
		NextPos += EBML_ElementFullSize((ebml_element*)WSegmentInfo,0) + 60; // 60 for the extra string we add
		MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)WSegmentInfo);

		// track info
		if (WTrackInfo)
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
            EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
			EBML_ElementForcePosition((ebml_element*)WTrackInfo, NextPos);
            EBML_ElementUpdateSize(WTrackInfo, 0, 0);
			NextPos += EBML_ElementFullSize((ebml_element*)WTrackInfo,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)WTrackInfo);
		}
		else
		{
			TextWrite(StdErr,T("Warning: the source Segment has no Track Info section (can be a chapter file)\r\n"));
		}

		// chapters
		if (RChapters)
		{
			ReduceSize((ebml_element*)RChapters);
            if (EBML_MasterUseChecksum(RChapters,!Unsafe))
                EBML_ElementUpdateSize(RChapters, 0, 0);
			if (!EBML_MasterCheckMandatory(RChapters,0))
			{
				TextWrite(StdErr,T("The Chapters section is missing mandatory elements, skipping\r\n"));
				NodeDelete((node*)RChapters);
				RChapters = NULL;
			}
			else
			{
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
                EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
				EBML_ElementForcePosition((ebml_element*)RChapters, NextPos);
				NextPos += EBML_ElementFullSize((ebml_element*)RChapters,0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RChapters);
			}
		}

		// attachments
		if (RAttachments)
		{
			MATROSKA_AttachmentSort(RAttachments);
			ReduceSize((ebml_element*)RAttachments);
            if (EBML_MasterUseChecksum(RAttachments,!Unsafe))
                EBML_ElementUpdateSize(RAttachments, 0, 0);
			if (!EBML_MasterCheckMandatory(RAttachments,0))
			{
				TextWrite(StdErr,T("The Attachments section is missing mandatory elements, skipping\r\n"));
				NodeDelete((node*)RAttachments);
				RAttachments = NULL;
			}
			else
			{
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
                EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
				EBML_ElementForcePosition((ebml_element*)RAttachments, NextPos);
				NextPos += EBML_ElementFullSize((ebml_element*)RAttachments,0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RAttachments);
			}
		}

		// tags
		if (RTags)
		{
			ReduceSize((ebml_element*)RTags);
            if (EBML_MasterUseChecksum(RTags,!Unsafe))
                EBML_ElementUpdateSize(RTags, 0, 0);
			if (!EBML_MasterCheckMandatory(RTags,0))
			{
				TextWrite(StdErr,T("The Tags section is missing mandatory elements, skipping\r\n"));
				NodeDelete((node*)RTags);
				RTags = NULL;
			}
			else
			{
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
                EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
				EBML_ElementForcePosition((ebml_element*)RTags, NextPos);
				NextPos += EBML_ElementFullSize((ebml_element*)RTags,0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RTags);
			}
		}
	}
	else
	{
		WriteJunk(Output,134);
		NextPos += 134;
	}

    Result = LinkClusters(&RClusters,RSegmentInfo,RTrackInfo,DstProfile, &WTracks, Live?12345:INVALID_TIMECODE_T);
	if (Result!=0)
		goto exit;

    // use the output track settings for each block
    for (Cluster = ARRAYBEGIN(*Clusters,ebml_master*);Cluster != ARRAYEND(*Clusters,ebml_master*); ++Cluster)
    {
        //EBML_MasterUseChecksum((ebml_master*)*Cluster,!Unsafe);
        for (Elt = EBML_MasterChildren(*Cluster);Elt;Elt=(ebml_element*)RLevel1)
        {
            RLevel1 = (ebml_master*)EBML_MasterNext((ebml_master*)Elt);
            if (EBML_ElementIsType(Elt, &MATROSKA_ContextBlockGroup))
            {
                for (Elt2 = EBML_MasterChildren((ebml_master*)Elt);Elt2;Elt2=EBML_MasterNext((ebml_master*)Elt2))
                {
                    if (EBML_ElementIsType(Elt2, &MATROSKA_ContextBlock))
                    {
                        if (MATROSKA_LinkBlockWithWriteTracks((matroska_block*)Elt2,WTrackInfo)!=ERR_NONE)
                            NodeDelete((node*)Elt);
                        else if (MATROSKA_LinkBlockWriteSegmentInfo((matroska_block*)Elt2,WSegmentInfo)!=ERR_NONE)
                            NodeDelete((node*)Elt);
                        break;
                    }
                }
            }
            else if (EBML_ElementIsType(Elt, &MATROSKA_ContextSimpleBlock))
            {
                if (MATROSKA_LinkBlockWithWriteTracks((matroska_block*)Elt,WTrackInfo)!=ERR_NONE)
                    NodeDelete((node*)Elt);
                else if (MATROSKA_LinkBlockWriteSegmentInfo((matroska_block*)Elt,WSegmentInfo)!=ERR_NONE)
                    NodeDelete((node*)Elt);
            }
        }
	    //EBML_ElementUpdateSize(*Cluster, 0, 0);
    }

    if (Optimize && !UnOptimize)
    {
        int16_t BlockTrack;
        ebml_element *Block, *GBlock;
        matroska_cluster **ClusterR;

	    if (!Quiet) TextWrite(StdErr,T("Optimizing...\r\n"));

		ArrayResize(&TrackMaxHeader, sizeof(array)*(MaxTrackNum+1), 0);
		ArrayZero(&TrackMaxHeader);
        for (i=0;(size_t)i<=MaxTrackNum;++i) {
            bool_t OptimizeTrack = 1;
            if (!OptimizeVideo)
            {
                ebml_master *Track;
	            for (Track = (ebml_master*)EBML_MasterFindChild(WTrackInfo,&MATROSKA_ContextTrackEntry); Track; Track=(ebml_master*)EBML_MasterNextChild(WTrackInfo,Track))
	            {
		            Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber);
		            if (EBML_IntegerValue((ebml_integer*)Elt) != i)
                        continue;
		            Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackType);
		            if (EBML_IntegerValue((ebml_integer*)Elt) == TRACK_TYPE_VIDEO)
                    {
                        Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextContentEncodings);
                        NodeDelete((node*)Elt);
			            OptimizeTrack = 0;
                    }
		            break;
	            }
            }
            if (OptimizeTrack)
                InitCommonHeader(ARRAYBEGIN(TrackMaxHeader,array)+i);
        }

	    for (ClusterR=ARRAYBEGIN(RClusters,matroska_cluster*);ClusterR!=ARRAYEND(RClusters,matroska_cluster*);++ClusterR)
	    {
		    for (Block = EBML_MasterChildren(*ClusterR);Block;Block=EBML_MasterNext(Block))
		    {
			    if (EBML_ElementIsType(Block, &MATROSKA_ContextBlockGroup))
			    {
				    GBlock = EBML_MasterFindChild((ebml_master*)Block, &MATROSKA_ContextBlock);
				    if (GBlock)
				    {
					    BlockTrack = MATROSKA_BlockTrackNum((matroska_block*)GBlock);
                        ShrinkCommonHeader(ARRAYBEGIN(TrackMaxHeader,array)+BlockTrack, (matroska_block*)GBlock, Input);
				    }
			    }
			    else if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
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
		ebml_master *Track;
        matroska_block *Block1;
		timecode_t Prev = INVALID_TIMECODE_T, *Tst, BlockTime, BlockDuration, MasterEndTimecode, BlockEnd, MainBlockEnd;
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
		for (Track=(ebml_master*)EBML_MasterChildren(WTrackInfo); Track; Track=(ebml_master*)EBML_MasterNext(Track))
		{
			if (EBML_ElementIsType((ebml_element*)Track, &MATROSKA_ContextTrackEntry))
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
		BlockInfo.FrameStartIndex = 0;

		for (ClusterR=ARRAYBEGIN(RClusters,matroska_cluster*);ClusterR!=ARRAYEND(RClusters,matroska_cluster*);++ClusterR)
		{
			for (Block = EBML_MasterChildren(*ClusterR);Block;Block=EBML_MasterNext(Block))
			{
				if (EBML_ElementIsType(Block, &MATROSKA_ContextBlockGroup))
				{
					GBlock = EBML_MasterFindChild((ebml_master*)Block, &MATROSKA_ContextBlock);
					if (GBlock)
					{
						BlockTrack = MATROSKA_BlockTrackNum((matroska_block*)GBlock);
						BlockInfo.Block = (matroska_block*)GBlock;
						ArrayAppend(ARRAYBEGIN(TrackBlocks,array)+BlockTrack,&BlockInfo,sizeof(BlockInfo),1024);
					}
				}
				else if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
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
			Elt = EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber);
			assert(Elt!=NULL);
			MainTrack = (int16_t)EBML_IntegerValue((ebml_integer*)Elt);

			for (pTrackOrder=ARRAYBEGIN(TrackOrder,size_t);pTrackOrder!=ARRAYEND(TrackOrder,size_t);++pTrackOrder)
			{
				if (!ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[*pTrackOrder],block_info))
				{
					if (!Quiet) TextPrintf(StdErr,T("Track %d has no blocks! Deleting...\r\n"),(int)*pTrackOrder);
					for (Elt = EBML_MasterFindChild(WTrackInfo,&MATROSKA_ContextTrackEntry); Elt; Elt=EBML_MasterNextChild(WTrackInfo,Elt))
					{
						if (EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,&MATROSKA_ContextTrackNumber))==*pTrackOrder)
						{
							NodeDelete((node*)Elt);
							--Frame;
        					Exit = 0;
							break;
						}
					}
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
			bool_t ReachedClusterEnd = 0;
			ClusterW = (matroska_cluster*)EBML_ElementCreate(Track, &MATROSKA_ContextCluster, 0, NULL);
			ArrayAppend(&WClusters,&ClusterW,sizeof(ClusterW),256);
			MATROSKA_LinkClusterReadSegmentInfo(ClusterW, RSegmentInfo, 1);
			MATROSKA_LinkClusterWriteSegmentInfo(ClusterW, WSegmentInfo);
			MATROSKA_ClusterSetTimecode(ClusterW,*Tst); // \todo avoid having negative timecodes in the Cluster ?

			if ((Tst+1)==ARRAYEND(KeyFrameTimecodes, timecode_t))
				MasterEndTimecode = INVALID_TIMECODE_T;
			else
				MasterEndTimecode = *(Tst+1);

			while (!ReachedClusterEnd && ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack] != ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[MainTrack],block_info))
			{
				// next Block end in the master track
				if (ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack]+1 == ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array)[MainTrack],block_info))
					MainBlockEnd = INVALID_TIMECODE_T;
				else
				{
					pBlockInfo = ARRAYBEGIN(ARRAYBEGIN(TrackBlocks,array)[MainTrack],block_info) + ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack] + 1;
					MainBlockEnd = pBlockInfo->DecodeTime;
				}

                if (EBML_ElementPosition((ebml_element*)ClusterW) == INVALID_FILEPOS_T)
                    EBML_ElementForcePosition((ebml_element*)ClusterW, EBML_ElementPosition((ebml_element*)pBlockInfo->Block)); // fake average value

                // loop on all tracks in their specified order
				for (pTrackOrder=ARRAYBEGIN(TrackOrder,size_t);pTrackOrder!=ARRAYEND(TrackOrder,size_t);++pTrackOrder)
				{
					// output all the blocks until MainBlockEnd (included) for this track
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
                        {
                            if (ARRAYCOUNT(Alternate3DTracks, block_info*) && ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder])
                                ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder] = NULL;
							break; // next track around this timecode
                        }

						if (pBlockInfo->FrameStartIndex!=0)
						{
                            // use the frames left from the previous block
							if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, &MATROSKA_ContextSimpleBlock))
							{
                                Block1 = (matroska_block*)EBML_ElementCopy(pBlockInfo->Block, NULL);
								MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

								for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
								{
									if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimecode)
										break;
									MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
									MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
								}

								if (MATROSKA_BlockGetFrameCount(Block1))
									EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)Block1);
								else
									NodeDelete((node*)Block1);

								if (pBlockInfo->FrameStartIndex!=MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									break; // next track
								else
								{
									ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
									continue;
								}
							}
							else if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, &MATROSKA_ContextBlock))
							{
                                Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
								Block1 = (matroska_block*)EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextBlock);
								MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

								for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
								{
									if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimecode)
										break;
									MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
									MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
								}

								if (MATROSKA_BlockGetFrameCount(Block1))
									EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)Elt);
								else
									NodeDelete((node*)Elt);

								if (pBlockInfo->FrameStartIndex!=MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									break; // next track
								else
								{
									ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
									continue;
								}
							}
						}

						if (MainBlockEnd!=INVALID_TIMECODE_T && BlockEnd>MasterEndTimecode && *pTrackOrder!=MainTrack && MATROSKA_BlockLaced(pBlockInfo->Block))
						{
							// relacing
                            //TextPrintf(StdErr,T("\rRelacing block track %d at %") TPRId64 T(" ends %") TPRId64 T(" next cluster at %") TPRId64 T("\r\n"),*pTrackOrder,pBlockInfo->DecodeTime,BlockEnd,MasterEndTimecode);
                            if (MATROSKA_BlockReadData(pBlockInfo->Block,Input)==ERR_NONE)
                            {
						        bool_t HasDuration = MATROSKA_BlockProcessFrameDurations(pBlockInfo->Block,Input)==ERR_NONE;
							    Result = ERR_NONE;
							    if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, &MATROSKA_ContextSimpleBlock))
							    {
								    // This block needs to be split
                                    Block1 = (matroska_block*)EBML_ElementCopy(pBlockInfo->Block, NULL);
							        MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

							        for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
							        {
								        if (HasDuration && MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimecode)
									        break;
								        MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
								        MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
							        }

							        if (MATROSKA_BlockGetFrameCount(Block1)==MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
							        {
								        pBlockInfo->FrameStartIndex = 0; // all the frames are for the next Cluster
								        NodeDelete((node*)Block1);
							        }
							        else
							        {
								        if (MATROSKA_BlockGetFrameCount(Block1))
									        Result = EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)Block1);
								        else
									        NodeDelete((node*)Block1);
							        }
							        break; // next track
							    }
							    else
							    {
								    assert(EBML_ElementIsType((ebml_element*)pBlockInfo->Block, &MATROSKA_ContextBlock));
								    // This block needs to be split
                                    Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
								    Block1 = (matroska_block*)EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextBlock);
							        MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

							        for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
							        {
								        if (HasDuration && MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimecode)
									        break;
								        MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
								        MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
							        }

							        if (MATROSKA_BlockGetFrameCount(Block1)==MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
							        {
								        pBlockInfo->FrameStartIndex = 0; // all the frames are for the next Cluster
								        NodeDelete((node*)Elt);
							        }
							        else
							        {
								        if (MATROSKA_BlockGetFrameCount(Block1))
									        Result = EBML_MasterAppend((ebml_master*)ClusterW,Elt);
								        else
									        NodeDelete((node*)Elt);
							        }
							        break; // next track
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
                                MATROSKA_BlockReleaseData(pBlockInfo->Block,0);
                            }
						}

						if (MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
						{
                            block_info *prevBlock = NULL;
                            if (ARRAYCOUNT(Alternate3DTracks, block_info*) && ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder])
                            {
                                prevBlock = ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder];
                                if (prevBlock==MARKER3D)
                                {
                                    ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder] = pBlockInfo;
                                    prevBlock = NULL;
                                }
                                else if (MATROSKA_BlockGetFrameCount(prevBlock->Block) >= 2)
                                {
                                    ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder] = pBlockInfo;
                                    prevBlock = NULL;
                                }
                                else
                                {
                                    Result = MATROSKA_BlockReadData(prevBlock->Block,Input);
                                }
                            }

                            if (prevBlock)
                            {
                                // add the first frame into the previous Block
                                if (Result==ERR_NONE)
                                {
                                    Result = MATROSKA_BlockReadData(pBlockInfo->Block,Input);
                                    if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, &MATROSKA_ContextSimpleBlock))
                                    {
                                        Block1 = (matroska_block*)pBlockInfo->Block;
                                    }
                                    else
                                    {
                                        Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
								        Block1 = (matroska_block*)EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextBlock);
                                    }
                                    assert(!Block1->IsKeyframe);
						            MATROSKA_BlockGetFrame(Block1, pBlockInfo->FrameStartIndex, &FrameData, 1);
                                    MATROSKA_BlockAppendFrame(prevBlock->Block, &FrameData, *Tst);
                                    MATROSKA_BlockReleaseData(pBlockInfo->Block,0);
                                }
                                MATROSKA_BlockReleaseData(prevBlock->Block,0);
                            }
                            else
                            {
							    if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, &MATROSKA_ContextSimpleBlock))
                                {
						            Result = MATROSKA_LinkBlockWriteSegmentInfo(pBlockInfo->Block,WSegmentInfo);
                                    if (Result == ERR_NONE)
							            Result = EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)pBlockInfo->Block);
                                }
							    else
							    {
                                    assert(EBML_ElementIsType((ebml_element*)pBlockInfo->Block, &MATROSKA_ContextBlock));
                                    Result = MATROSKA_LinkBlockWriteSegmentInfo(pBlockInfo->Block,WSegmentInfo);
                                    if (Result == ERR_NONE)
                                        Result = EBML_MasterAppend((ebml_master*)ClusterW,EBML_ElementParent((ebml_element*)pBlockInfo->Block));
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
							ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
						}

						if (*pTrackOrder==MainTrack)
						{
							if (MainBlockEnd == INVALID_TIMECODE_T || BlockEnd == MasterEndTimecode)
								ReachedClusterEnd = 1;
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
        array *HeaderData = NULL;
        ebml_binary *CodecPrivate;
        ebml_master *Encodings;
        size_t TrackNum;
        tchar_t CodecID[MAXDATA];
		// fix/clean the Lacing flag for each track
		//assert(MATROSKA_ContextFlagLacing.DefaultValue==1);
		for (RLevel1 = (ebml_master*)EBML_MasterChildren(WTrackInfo); RLevel1; RLevel1=(ebml_master*)EBML_MasterNext(RLevel1))
		{
            if (EBML_ElementIsType((ebml_element*)RLevel1, &MATROSKA_ContextTrackEntry))
            {
                int encoding = MATROSKA_BLOCK_COMPR_NONE;
                int zlib_scope = MATROSKA_COMPR_SCOPE_BLOCK;

                Elt2 = EBML_MasterFindChild(RLevel1,&MATROSKA_ContextTrackNumber);
			    if (!Elt2) continue;
                TrackNum = (size_t)EBML_IntegerValue((ebml_integer*)Elt2);
			    if (ARRAYBEGIN(WTracks,track_info)[TrackNum].IsLaced)
			    {
				    // has lacing
				    Elt2 = EBML_MasterFindChild(RLevel1,&MATROSKA_ContextFlagLacing);
				    if (Elt2)
					    NodeDelete((node*)Elt2);
			    }
			    else
			    {
				    // doesn't have lacing
				    Elt2 = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextFlagLacing,1,0);
				    EBML_IntegerSetValue((ebml_integer*)Elt2,0);
			    }
                
                CodecPrivate = (ebml_binary*) EBML_MasterFindChild(RLevel1,&MATROSKA_ContextCodecPrivate);
                if (CodecPrivate && CodecPrivate->Base.DataSize==0)
                {
                    NodeDelete((node*)CodecPrivate);
                    CodecPrivate = NULL;
                }

                Encodings = (ebml_master*)EBML_MasterFindChild(RLevel1,&MATROSKA_ContextContentEncodings);
                if (UnOptimize)
                    // remove the previous track compression
                    encoding = MATROSKA_BLOCK_COMPR_NONE;
                else if (!Optimize)
                    // keep the same kind of encoding as before
                    encoding = MATROSKA_TrackGetBlockCompression((matroska_trackentry*)RLevel1);
                else
                {
                    Elt = EBML_MasterFindFirstElt(RLevel1,&MATROSKA_ContextCodecID,1,0);
                    EBML_StringGet((ebml_string*)Elt,CodecID,TSIZEOF(CodecID));
                    if (tcsisame_ascii(CodecID,T("S_USF")) || tcsisame_ascii(CodecID,T("S_VOBSUB")) || tcsisame_ascii(CodecID,T("S_HDMV/PGS")) || tcsisame_ascii(CodecID,T("B_VOBBTN"))
                        || tcsisame_ascii(CodecID,T("V_UNCOMPRESSED"))|| tcsstr(CodecID,T("A_PCM"))==CodecID)
                        encoding = MATROSKA_BLOCK_COMPR_ZLIB;
                    else
                    {
                        // don't keep the zlib compression on compressed codecs
                        //encoding = MATROSKA_TrackGetBlockCompression((matroska_trackentry*)RLevel1);
                        //if (encoding == MATROSKA_BLOCK_COMPR_NONE || encoding == MATROSKA_BLOCK_COMPR_HEADER)
                        {
                            HeaderData = ARRAYBEGIN(TrackMaxHeader,array)+TrackNum;
                            if (ARRAYCOUNT(*HeaderData,uint8_t))
                                encoding = MATROSKA_BLOCK_COMPR_HEADER;
                        }
                    }
                }
                if (encoding == MATROSKA_BLOCK_COMPR_NONE)
                    zlib_scope = 0;
                else
                {
                    if (Encodings)
                    {
                        Elt = EBML_MasterFindChild((ebml_master*)Encodings,&MATROSKA_ContextContentEncoding);
                        if (Elt)
                        {
                            Elt2 =  EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextContentEncodingScope);
                            if (Elt2)
                                zlib_scope = EBML_IntegerValue((ebml_integer*)Elt2);
                        }
                    }
                }

                // see if we can add CodecPrivate too
                if ((Optimize || encoding != MATROSKA_BLOCK_COMPR_NONE) && encoding != MATROSKA_BLOCK_COMPR_HEADER && zlib_scope != MATROSKA_COMPR_SCOPE_PRIVATE)
                {
                    if (CodecPrivate!=NULL)
                    {
                        size_t ExtraCompHeaderBytes = (encoding == MATROSKA_BLOCK_COMPR_NONE) ? 13 : 3; // extra bytes needed to add the comp header to the track
                        size_t CompressedSize = ARRAYCOUNT(CodecPrivate->Data,uint8_t);
                        uint8_t *Compressed = malloc(CompressedSize);
                        if (CompressFrameZLib(ARRAYBEGIN(CodecPrivate->Data,uint8_t), (size_t)CodecPrivate->Base.DataSize, &Compressed, &CompressedSize)==ERR_NONE
                            && (CompressedSize + ExtraCompHeaderBytes) < CodecPrivate->Base.DataSize)
                        {
                            encoding = MATROSKA_BLOCK_COMPR_ZLIB;
                            zlib_scope |= MATROSKA_COMPR_SCOPE_PRIVATE;
                        }
                        free(Compressed);
                    }
                }

                switch (encoding)
                {
                case MATROSKA_BLOCK_COMPR_ZLIB:
                case MATROSKA_BLOCK_COMPR_BZLIB: // transform bzlib into zlib
                case MATROSKA_BLOCK_COMPR_LZO1X: // transform lzo1x into zlib
                    if (MATROSKA_TrackSetCompressionZlib((matroska_trackentry*)RLevel1, zlib_scope))
						ClustersNeedRead = 1;
                    break;
                case MATROSKA_BLOCK_COMPR_HEADER:
                    if (!HeaderData || MATROSKA_TrackSetCompressionHeader((matroska_trackentry*)RLevel1, ARRAYBEGIN(*HeaderData,uint8_t), ARRAYCOUNT(*HeaderData,uint8_t)))
					    ClustersNeedRead = 1;
                    break;
                default:
                    if (MATROSKA_TrackSetCompressionNone((matroska_trackentry*)RLevel1))
					    ClustersNeedRead = 1;
                    break;
                }

                Encodings = (ebml_master*)EBML_MasterFindChild(RLevel1,&MATROSKA_ContextContentEncodings);
                if (Encodings)
                {
                    if ((Elt2 = EBML_MasterFindChild(Encodings,&MATROSKA_ContextContentEncoding)) != NULL)
                    {
                        Elt =  EBML_MasterGetChild((ebml_master*)Elt2,&MATROSKA_ContextContentEncodingScope);
                        if (EBML_IntegerValue((ebml_integer*)Elt) & MATROSKA_COMPR_SCOPE_BLOCK)
                        {
                            Elt = EBML_MasterGetChild((ebml_master*)Elt2,&MATROSKA_ContextContentCompression);
                            Elt = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextContentCompAlgo);
                            if (EBML_IntegerValue((ebml_integer*)Elt)!=MATROSKA_BLOCK_COMPR_HEADER)
                                ClustersNeedRead = 1;
                        }
                    }
                }
            }
		}
	}

	if (!Live)
	{
        // cues
        if (ARRAYCOUNT(*Clusters,ebml_element*) < 2)
        {
            NodeDelete((node*)RCues);
            RCues = NULL;
        }
		else if (RCues)
		{
			ReduceSize((ebml_element*)RCues);
			if (!EBML_MasterCheckMandatory(RCues,0))
			{
				TextWrite(StdErr,T("The original Cues are missing mandatory elements, creating from scratch\r\n"));
				NodeDelete((node*)RCues);
				RCues = NULL;
			}
            else if (EBML_MasterUseChecksum(RCues,!Unsafe))
                EBML_ElementUpdateSize(RCues,0,0);
		}

		if (!RCues && WTrackInfo && ARRAYCOUNT(*Clusters,ebml_element*) > 1)
		{
			// generate the cues
			RCues = (ebml_master*)EBML_ElementCreate(&p,&MATROSKA_ContextCues,0,NULL);
            EBML_MasterUseChecksum(RCues,!Unsafe);
			if (!Quiet) TextWrite(StdErr,T("Generating Cues from scratch\r\n"));
			CuesCreated = GenerateCueEntries(RCues,Clusters,WTrackInfo,WSegmentInfo,(ebml_element*)RSegment, TotalSize);
			if (!CuesCreated)
			{
				NodeDelete((node*)RCues);
				RCues = NULL;
			}
		}

		if (RCues)
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
            EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
			EBML_ElementForcePosition((ebml_element*)RCues, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RCues,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RCues);
		}

        ExtraVoidSize = 2 * EXTRA_SEEK_SPACE; // leave room for 2 unknown level1 elements
        if (!RTags)
            ExtraVoidSize += EXTRA_SEEK_SPACE;

        if (!RAttachments)
            ExtraVoidSize += EXTRA_SEEK_SPACE;
    
        if (!RChapters)
            ExtraVoidSize += EXTRA_SEEK_SPACE;

        // first cluster
        if (ARRAYCOUNT(RClusters,matroska_cluster*))
        {
			W1stClusterSeek = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
            EBML_MasterUseChecksum((ebml_master*)W1stClusterSeek,!Unsafe);
			EBML_ElementForcePosition(ARRAYBEGIN(*Clusters,ebml_element*)[0], NextPos + ExtraVoidSize);
			NextPos += EBML_ElementFullSize(ARRAYBEGIN(*Clusters,ebml_element*)[0],0);
			MATROSKA_LinkMetaSeekElement(W1stClusterSeek,ARRAYBEGIN(*Clusters,ebml_element*)[0]);
        }

		// first estimation of the MetaSeek size
		MetaSeekUpdate(WMetaSeek);
		MetaSeekBefore = EBML_ElementFullSize((ebml_element*)WMetaSeek,0);
        NextPos = EBML_ElementPositionData((ebml_element*)WSegment) + EBML_ElementFullSize((ebml_element*)WMetaSeek,0);

        NextPos += ExtraVoidSize;
	}

    EBML_ElementUpdateSize(WSegmentInfo,0,0);
    EBML_ElementForcePosition((ebml_element*)WSegmentInfo, NextPos);
    NextPos += EBML_ElementFullSize((ebml_element*)WSegmentInfo,0);

    //  Compute the Track Info size
    if (WTrackInfo)
    {
        ReduceSize((ebml_element*)WTrackInfo);
        EBML_ElementUpdateSize(WTrackInfo,0,0);
        EBML_ElementForcePosition((ebml_element*)WTrackInfo, NextPos);
        NextPos += EBML_ElementFullSize((ebml_element*)WTrackInfo,0);
    }

	if (!Live)
	{
        ebml_element *Void;
		//  Compute the Chapters size
		if (RChapters)
		{
			ReduceSize((ebml_element*)RChapters);
			EBML_ElementUpdateSize(RChapters,0,0);
			EBML_ElementForcePosition((ebml_element*)RChapters, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RChapters,0);
		}

		//  Compute the Attachments size
		if (RAttachments)
		{
			ReduceSize((ebml_element*)RAttachments);
			EBML_ElementUpdateSize(RAttachments,0,0);
			EBML_ElementForcePosition((ebml_element*)RAttachments, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RAttachments,0);
		}

		//  Compute the Tags size
		if (RTags)
		{
			EBML_ElementUpdateSize(RTags,0,0);
			EBML_ElementForcePosition((ebml_element*)RTags, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RTags,0);
		}

		MetaSeekUpdate(WMetaSeek);
		NextPos += EBML_ElementFullSize((ebml_element*)WMetaSeek,0) - MetaSeekBefore;

		//  Compute the Cues size
		if (WTrackInfo && RCues)
		{
            OptimizeCues(RCues,Clusters,WSegmentInfo,NextPos, WSegment, TotalSize, !CuesCreated, !Unsafe, ClustersNeedRead?Input:NULL);
			EBML_ElementForcePosition((ebml_element*)RCues, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RCues,0);
		}
        else if (ARRAYCOUNT(*Clusters,ebml_element*)==1)
            EBML_ElementForcePosition(ARRAYBEGIN(*Clusters,ebml_element*)[0], NextPos);

		// update and write the MetaSeek and the elements following
        // write without the fake Tags pointer
		Stream_Seek(Output,EBML_ElementPosition((ebml_element*)WMetaSeek),SEEK_SET);
        MetaSeekUpdate(WMetaSeek);

		EBML_ElementFullSize((ebml_element*)WMetaSeek,0);
		if (EBML_ElementRender((ebml_element*)WMetaSeek,Output,0,0,1,&MetaSeekBefore)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Meta Seek\r\n"));
			Result = -22;
			goto exit;
		}
		SegmentSize += MetaSeekBefore;

        // create a fake placeholder elements to have its position prepared in the SeekHead
        Void = EBML_ElementCreate(WMetaSeek,&EBML_ContextEbmlVoid,1,NULL);
        EBML_VoidSetFullSize(Void, ExtraVoidSize);
		if (EBML_ElementRender((ebml_element*)Void,Output,0,0,1,&ClusterSize)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the Void after Meta Seek\r\n"));
			Result = -24;
			goto exit;
		}
        NodeDelete((node*)Void);
		SegmentSize += ClusterSize;
    }
    else if (!Unsafe)
        SetClusterPrevSize(Clusters, ClustersNeedRead?Input:NULL);

    if (EBML_ElementRender((ebml_element*)WSegmentInfo,Output,0,0,1,&ClusterSize)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the Segment Info\r\n"));
        Result = -11;
        goto exit;
    }
	SegmentSize += ClusterSize;
    if (WTrackInfo)
    {
        if (EBML_ElementRender((ebml_element*)WTrackInfo,Output,0,0,1,&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Track Info\r\n"));
            Result = -12;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }
    if (!Live && RChapters)
    {
        if (EBML_ElementRender((ebml_element*)RChapters,Output,0,0,1,&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Chapters\r\n"));
            Result = -13;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }

    //  Write the Attachments
    if (!Live && RAttachments)
    {
        ReduceSize((ebml_element*)RAttachments);
        if (EBML_ElementRender((ebml_element*)RAttachments,Output,0,0,1,&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Attachments\r\n"));
            Result = -17;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }

    if (!Live && RTags)
    {
        if (EBML_ElementRender((ebml_element*)RTags,Output,0,0,1,&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Tags\r\n"));
            Result = -14;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }

    if (!Live && RCues)
    {
        if (EBML_ElementRender((ebml_element*)RCues,Output,0,0,1,&CuesSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Cues\r\n"));
            Result = -15;
            goto exit;
        }
        SegmentSize += CuesSize;
    }

    //  Write the Clusters
    ClusterSize = INVALID_FILEPOS_T;
    PrevTimecode = INVALID_TIMECODE_T;
    CuesChanged = 0;
    CurrentPhase = TotalPhases;
    for (Cluster = ARRAYBEGIN(*Clusters,ebml_master*);Cluster != ARRAYEND(*Clusters,ebml_master*); ++Cluster)
    {
        ShowProgress((ebml_element*)*Cluster, TotalSize);
        CuesChanged = WriteCluster(*Cluster,Output,Input, ClusterSize, &PrevTimecode) || CuesChanged;
        if (!Unsafe)
            ClusterSize = EBML_ElementFullSize((ebml_element*)*Cluster,0);
        SegmentSize += EBML_ElementFullSize((ebml_element*)*Cluster,0);
    }
    EndProgress();

    if (CuesChanged && !Live && RCues)
    {
        filepos_t PosBefore = Stream_Seek(Output,0,SEEK_CUR);
        Stream_Seek(Output,EBML_ElementPosition((ebml_element*)RCues),SEEK_SET);

        UpdateCues(RCues, WSegment);

        if (EBML_ElementRender((ebml_element*)RCues,Output,0,0,1,&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Cues\r\n"));
            Result = -16;
            goto exit;
        }
        if (CuesSize >= ClusterSize+2)
        {
            // the cues were shrinked, write a void element
            ebml_element *Void = EBML_ElementCreate(RCues,&EBML_ContextEbmlVoid,1,NULL);
            EBML_VoidSetFullSize(Void, CuesSize - ClusterSize);
            EBML_ElementRender(Void,Output,0,0,1,NULL);
        }
        else
        {
            assert(ClusterSize == CuesSize);
            if (ClusterSize != CuesSize)
            {
                TextWrite(StdErr,T("The Cues size changed after a Cluster timecode was altered!\r\n"));
                Result = -18;
                goto exit;
            }
        }
        Stream_Seek(Output,PosBefore,SEEK_SET);
    }

    // update the WSegment size
	if (!Live)
	{
		if (EBML_ElementDataSize((ebml_element*)WSegment,0)!=INVALID_FILEPOS_T && SegmentSize - ExtraSizeDiff - ExtraVoidSize > EBML_ElementDataSize((ebml_element*)WSegment,0))
		{
			if (EBML_CodedSizeLength(SegmentSize,EBML_ElementSizeLength((ebml_element*)WSegment),0) != EBML_CodedSizeLength(SegmentSize,EBML_ElementSizeLength((ebml_element*)WSegment),0))
			{
                // TODO: this check is always false
				TextPrintf(StdErr,T("The segment written is much bigger than the original %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),SegmentSize,EBML_ElementDataSize((ebml_element*)WSegment,0));
				Result = -20;
				goto exit;
			}
			if (!Quiet) TextPrintf(StdErr,T("The segment written is bigger than the original %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),SegmentSize,EBML_ElementDataSize((ebml_element*)WSegment,0));
		}
		if (EBML_CodedSizeLength(EBML_ElementDataSize((ebml_element*)WSegment,0),0,!Live) > EBML_CodedSizeLength(SegmentSize,0,!Live))
			EBML_ElementSetSizeLength((ebml_element*)WSegment, EBML_CodedSizeLength(EBML_ElementDataSize((ebml_element*)WSegment,0),0,!Live));
		EBML_ElementForceDataSize((ebml_element*)WSegment, SegmentSize);
		Stream_Seek(Output,EBML_ElementPosition((ebml_element*)WSegment),SEEK_SET);
		if (EBML_ElementRenderHead((ebml_element*)WSegment, Output, 0, NULL)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Segment size !\r\n"));
			Result = -21;
			goto exit;
		}

		// update the Meta Seek
		MetaSeekUpdate(WMetaSeek);
		//Stream_Seek(Output,EBML_ElementPosition(WMetaSeek,SEEK_SET);
		if (EBML_ElementRender((ebml_element*)WMetaSeek,Output,0,0,1,&MetaSeekAfter)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Meta Seek\r\n"));
			Result = -22;
			goto exit;
		}
		if (MetaSeekBefore != MetaSeekAfter)
		{
			TextPrintf(StdErr,T("The final Meta Seek size has changed %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),MetaSeekBefore,MetaSeekAfter);
			Result = -23;
			goto exit;
		}
	}

    if (!Quiet) TextPrintf(StdErr,T("Finished cleaning & optimizing \"%s\"\r\n"),Path);

exit:
    NodeDelete((node*)WSegment);

    for (Cluster = ARRAYBEGIN(RClusters,ebml_master*);Cluster != ARRAYEND(RClusters,ebml_master*); ++Cluster)
        NodeDelete((node*)*Cluster);
    ArrayClear(&RClusters);
    for (Cluster = ARRAYBEGIN(WClusters,ebml_master*);Cluster != ARRAYEND(WClusters,ebml_master*); ++Cluster)
        NodeDelete((node*)*Cluster);
    for (MaxTrackNum=0;MaxTrackNum<ARRAYCOUNT(TrackMaxHeader,array);++MaxTrackNum)
        ArrayClear(ARRAYBEGIN(TrackMaxHeader,array)+MaxTrackNum);
    ArrayClear(&Alternate3DTracks);
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

    if (Result<0 && Path[0])
        FileErase((nodecontext*)&p,Path,1,0);

    // EBML & Matroska ending
    MATROSKA_Done((nodecontext*)&p);

    // Core-C ending
	StdAfx_Done((nodemodule*)&p);
    if (!Regression) // until all the memory leaks are fixed
    ParserContext_Done(&p);

    return Result;
}
