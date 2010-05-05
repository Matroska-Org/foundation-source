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
#include "matroska/matroska.h"

/*!
 * \todo change the Segment UID (when key parts are altered)
 * \todo optionally add a CRC32 on level1 elements
 * \todo start a new cluster boundary with each video keyframe
 * \todo add error types (numbers) to show to that each type can be disabled on demand
 * \todo forbid the use of SimpleBlock in v1
 * \todo verify that no lacing is used when lacing is disabled in the SegmentInfo
 * \todo allow adding/replacing Tags
 * \todo allow adding/replacing Chapters
 * \todo allow adding/replacing Attachments
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
static textwriter *StdErr = NULL;

static void ReduceSize(ebml_element *Element)
{
    Element->SizeLength = 0; // reset
    if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    {
        ebml_element *i;
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
    TextWrite(StdErr,T("\n"));
}

static void OptimizeCues(ebml_element *Cues, array *Clusters, ebml_element *Tracks, ebml_element *RSegmentInfo, filepos_t StartPos, ebml_element *WSegment, ebml_element *RSegment)
{
    matroska_cluster **Cluster;
    ebml_element *Block, *GBlock;
    matroska_cuepoint *Cue;
    int16_t CueTrack;
    timecode_t CueTimecode;
    bool_t Found;

    ReduceSize(Cues);

    // link each Block/SimpleBlock with its Track and SegmentInfo
    for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
    {
        ReduceSize((ebml_element*)*Cluster);
        MATROSKA_LinkClusterSegmentInfo(*Cluster,RSegmentInfo);
        for (Block = EBML_MasterChildren(*Cluster);Block;Block=EBML_MasterNext(Block))
        {
            if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
            {
                for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
                {
                    if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
                    {
                        MATROSKA_LinkBlockTrack((matroska_block*)GBlock,Tracks);
                        MATROSKA_LinkBlockSegmentInfo((matroska_block*)GBlock,RSegmentInfo);
                        break;
                    }
                }
            }
            else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
            {
                MATROSKA_LinkBlockTrack((matroska_block*)Block,Tracks);
                MATROSKA_LinkBlockSegmentInfo((matroska_block*)Block,RSegmentInfo);
            }
        }
    }

    // link each Cue entry to the segment
    for (Cue = (matroska_cuepoint*)EBML_MasterChildren(Cues);Cue;Cue=(matroska_cuepoint*)EBML_MasterNext(Cue))
        MATROSKA_LinkCueSegmentInfo(Cue,RSegmentInfo);

    // link each Cue entry to the corresponding Block/SimpleBlock in the Cluster
    for (Cue = (matroska_cuepoint*)EBML_MasterChildren(Cues);Cue;Cue=(matroska_cuepoint*)EBML_MasterNext(Cue))
    {
        CueTrack = MATROSKA_CueTrackNum(Cue);
        CueTimecode = MATROSKA_CueTimecode(Cue);
        Found = 0;
        for (Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);!Found && Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
        {
            for (Block = EBML_MasterChildren(*Cluster);!Found && Block;Block=EBML_MasterNext(Block))
            {
                if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
                {
                    for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
                    {
                        if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
                        {
                            if (MATROSKA_BlockTrackNum((matroska_block*)GBlock) == CueTrack &&
                                MATROSKA_BlockTimecode((matroska_block*)GBlock) == CueTimecode)
                            {
                                MATROSKA_LinkCuePointBlock(Cue,(matroska_block*)GBlock);
                                Found = 1;
                            }
                            ShowProgress((ebml_element*)(*Cluster),RSegment,2);
                            break;
                        }
                    }
                }
                else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
                {
                    if (MATROSKA_BlockTrackNum((matroska_block*)Block) == CueTrack &&
                        MATROSKA_BlockTimecode((matroska_block*)Block) == CueTimecode)
                    {
                        MATROSKA_LinkCuePointBlock(Cue,(matroska_block*)Block);
                        Found = 1;
                        ShowProgress((ebml_element*)(*Cluster),RSegment,2);
                        break;
                    }
                }
            }
        }
        if (!Found)
        {
            TextPrintf(StdErr,T("Could not find the matching block for timecode %0.3f s\r\n"),CueTimecode/1000000000.0);
        }
    }
    EndProgress(RSegment,2);

    // TODO: sort the Cues

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
            else if (((ebml_integer*)SubElement)->Value > EBML_MAX_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
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
            else if (((ebml_integer*)SubElement)->Value > EBML_MAX_ID)
            {
                TextPrintf(StdErr,T("EBML Max ID Length %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
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
            else if (((ebml_integer*)SubElement)->Value > EBML_MAX_SIZE)
            {
                TextPrintf(StdErr,T("EBML Max Coded Size %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
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
                Node_FromStr(Input,String,TSIZEOF(String),((ebml_string*)SubElement)->Buffer);
                if (tcscmp(String,T("matroska"))!=0)
                {
                    TextPrintf(StdErr,T("EBML DocType %s not supported"),(long)((ebml_integer*)SubElement)->Value);
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
            else if (((ebml_integer*)SubElement)->Value > MATROSKA_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %ld not supported"),(long)((ebml_integer*)SubElement)->Value);
                break;
            }
            else
                DocVersion = (int)((ebml_integer*)SubElement)->Value;
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

int main(int argc, const char *argv[])
{
    int Result = 0;
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

    if (argc != 3)
    {
        TextWrite(StdErr,T("mkclean v") PROJECT_VERSION T(", Copyright (c) 2010 Matroska Foundation\r\n"));
        TextWrite(StdErr,T("Usage: mkclean [matroska_src] [matroska_dst]\r\n"));
        Result = -1;
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[1]);
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Result = -2;
        goto exit;
    }

    Node_FromStr(&p,Path,TSIZEOF(Path),argv[2]);
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
//TextPrintf(StdErr,T("Loading the level1 elements in memory\r\n"));
    RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    while (RLevel1)
    {
        EbmlHead = EBML_ElementSkipData(RLevel1, Input, &RSegmentContext, NULL, 1);
        ShowProgress(RLevel1,RSegment,1);
        assert(EbmlHead==NULL);
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
        else if (RLevel1->Context->Id == MATROSKA_ContextCues.Id)
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
            NodeDelete((node*)RLevel1);
        RLevel1 = EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
    }
    EndProgress(RSegment,1);

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
    if (EBML_StringSetValue((ebml_string*)RLevel1,"matroska") != ERR_NONE)
        goto exit;

    // Doctype version
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    ((ebml_integer*)RLevel1)->Value = DocVersion;

    // Doctype readable version
    RLevel1 = EBML_MasterGetChild(EbmlHead,&EBML_ContextDocTypeReadVersion);
    if (!RLevel1)
        goto exit;
    assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    ((ebml_integer*)RLevel1)->Value = DocVersion;

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
    WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
    RTrackInfo->ElementPosition = NextPos;
    NextPos += EBML_ElementFullSize(RTrackInfo,0);
    MATROSKA_LinkMetaSeekElement(WSeekPoint,RTrackInfo);
    // chapters
    if (RChapters)
    {
        WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        RChapters->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RChapters,0);
        MATROSKA_LinkMetaSeekElement(WSeekPoint,RChapters);
    }
    // tags
    if (RTags)
    {
        WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        RTags->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RTags,0);
        MATROSKA_LinkMetaSeekElement(WSeekPoint,RTags);
    }
    // cues
    WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
    RCues->ElementPosition = NextPos;
    NextPos += EBML_ElementFullSize(RCues,0);
    MATROSKA_LinkMetaSeekElement(WSeekPoint,RCues);
    // attachements
    if (RAttachments)
    {
        WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,&MATROSKA_ContextSeek,0);
        RAttachments->ElementPosition = EBML_ElementPositionEnd(RSegment) - EBML_ElementFullSize(RAttachments,0); // virutally position the attachment at the end of the segment
        NextPos += EBML_ElementFullSize(RAttachments,0);
        MATROSKA_LinkMetaSeekElement(WSeekPoint,RAttachments);
    }

    MetaSeekUpdate(WMetaSeek);

    NextPos = WMetaSeek->ElementPosition + EBML_ElementFullSize(WMetaSeek,0);

    //  Write the Segment Info
    ReduceSize(RSegmentInfo);
    // change the library names & app name
    LibName = (ebml_string*)EBML_MasterFindFirstElt(RSegmentInfo, &MATROSKA_ContextMuxingApp, 1, 0);
    stprintf_s(String,TSIZEOF(String),T("%s + %s"),Node_GetDataStr((node*)&p,CONTEXT_LIBEBML_VERSION),Node_GetDataStr((node*)&p,CONTEXT_LIBMATROSKA_VERSION));
    Node_FromUTF8(RSegmentInfo,Original,TSIZEOF(Original),LibName->Buffer);
    EBML_UniStringSetValue(LibName,String);

    AppName = (ebml_string*)EBML_MasterFindFirstElt(RSegmentInfo, &MATROSKA_ContextWritingApp, 1, 0);
    Node_FromUTF8(RSegmentInfo,String,TSIZEOF(String),AppName->Buffer);
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

    //  Write the Track Info
    ReduceSize(RTrackInfo);
    EBML_ElementUpdateSize(RTrackInfo,0,0);
    RTrackInfo->ElementPosition = NextPos;
    NextPos += EBML_ElementFullSize(RTrackInfo,0);

    //  Write the Chapters
    if (RChapters)
    {
        ReduceSize(RChapters);
        EBML_ElementUpdateSize(RChapters,0,0);
        RChapters->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RChapters,0);
    }

    //  Write the Tags
    if (RTags)
    {
        ReduceSize(RTags);
        EBML_ElementUpdateSize(RTags,0,0);
        RTags->ElementPosition = NextPos;
        NextPos += EBML_ElementFullSize(RTags,0);
    }

    //  Write the Cues
    OptimizeCues(RCues,&RClusters,RTrackInfo,RSegmentInfo,NextPos, WSegment, RSegment);
    EBML_ElementUpdateSize(RCues,0,0);
    RCues->ElementPosition = NextPos;
    NextPos += EBML_ElementFullSize(RCues,0);

    // write the MetaSeek and the elements following
    MetaSeekUpdate(WMetaSeek);
    MetaSeekBefore = EBML_ElementFullSize(WMetaSeek,0);
    Stream_Seek(Output,WMetaSeek->ElementPosition,SEEK_SET);
    if (EBML_ElementRender(WMetaSeek,Output,0,0,1,NULL,0)!=ERR_NONE)
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
    if (EBML_ElementRender(RTrackInfo,Output,0,0,1,NULL,0)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the Segment Info\r\n"));
        Result = -12;
        goto exit;
    }
    SegmentSize += EBML_ElementFullSize(RTrackInfo,0);
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
    if (EBML_ElementRender(RCues,Output,0,0,1,NULL,0)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the Cues\r\n"));
        Result = -15;
        goto exit;
    }
    SegmentSize += EBML_ElementFullSize(RCues,0);

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
