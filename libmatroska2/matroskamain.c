/*
 * $Id$
 * Copyright (c) 2008-2011, Matroska (non-profit organisation)
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
#include "matroska/matroska.h"
#include "matroska/matroska_sem.h"
#include "matroska/matroska_internal.h"
#if defined(HAVE_ZLIB)
#include "zlib/zlib.h"
#elif defined(CONFIG_ZLIB)
#include "zlib.h"
#endif
#if defined(CONFIG_BZLIB)
#include <bzlib.h>
#endif
#if defined(CONFIG_LZO1X)
#include "minilzo.h"
#endif
#if defined(MATROSKA_LIBRARY)
#include "matroska2_project.h"
#endif

const ebml_semantic EBML_SemanticMatroska[] = {
    {1, 0, &EBML_ContextHead        ,0},
    {1, 0, &MATROSKA_ContextSegment ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextStream = {FOURCC('M','K','X','_'), EBML_MASTER_CLASS, 0, 0, "Matroska Stream", EBML_SemanticMatroska, EBML_SemanticGlobals, NULL};

err_t MATROSKA_Init(nodecontext *p)
{
#if defined(MATROSKA_LIBRARY)
    tchar_t LibName[MAXPATH];
#endif
    err_t Err = EBML_Init(p);
    if (Err == ERR_NONE)
    {
#if defined(MATROSKA_LIBRARY)
        tcscpy_s(LibName,TSIZEOF(LibName),PROJECT_NAME T(" v") PROJECT_VERSION);
        Node_SetData(p,CONTEXT_LIBMATROSKA_VERSION,TYPE_STRING,LibName);
#endif
    }
    return Err;
}

err_t MATROSKA_Done(nodecontext *p)
{
    return EBML_Done(p);
}


#define MATROSKA_CUE_SEGMENTINFO     0x100
#define MATROSKA_CUE_BLOCK           0x101

#define MATROSKA_CLUSTER_READ_SEGMENTINFO  0x100
#define MATROSKA_CLUSTER_WRITE_SEGMENTINFO 0x101

#define MATROSKA_SEEKPOINT_ELEMENT   0x100

#define LACING_NONE  0
#define LACING_XIPH  1
#define LACING_FIXED 2
#define LACING_EBML  3
#define LACING_AUTO  4

struct matroska_cuepoint
{
    ebml_master Base;
    ebml_master *SegInfo;
    matroska_block *Block;
};

struct matroska_cluster
{
    ebml_master Base;
    ebml_master *ReadSegInfo;
    ebml_master *WriteSegInfo;
    timecode_t GlobalTimecode;
};

struct matroska_seekpoint
{
    ebml_master Base;
    ebml_element *Link;
};

struct matroska_trackentry
{
    ebml_master Base;
    bool_t CodecPrivateCompressed;
};

static err_t BlockTrackChanged(matroska_block *Block)
{
	Block->Base.Base.bNeedDataSizeUpdate = 1;
	return ERR_NONE;
}

static err_t ClusterTimeChanged(matroska_cluster *Cluster)
{
    timecode_t ClusterTimecode;
#if defined(CONFIG_EBML_WRITING)
    timecode_t BlockTimecode;
    ebml_element *Elt, *GBlock;
#endif

	Cluster->Base.Base.bNeedDataSizeUpdate = 1;
    ClusterTimecode = MATROSKA_ClusterTimecode(Cluster);
    MATROSKA_ClusterSetTimecode(Cluster, ClusterTimecode);
#if defined(CONFIG_EBML_WRITING)
    for (Elt = EBML_MasterChildren(Cluster); Elt; Elt = EBML_MasterNext(Elt))
    {
        if (EBML_ElementIsType(Elt, &MATROSKA_ContextBlockGroup))
        {
            for (GBlock = EBML_MasterChildren(Elt);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(GBlock, &MATROSKA_ContextBlock))
                {
                    BlockTimecode = MATROSKA_BlockTimecode((matroska_block*)GBlock);
                    if (BlockTimecode!=INVALID_TIMECODE_T)
                        MATROSKA_BlockSetTimecode((matroska_block*)GBlock, BlockTimecode, ClusterTimecode);
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(Elt, &MATROSKA_ContextSimpleBlock))
        {
            BlockTimecode = MATROSKA_BlockTimecode((matroska_block*)Elt);
            if (BlockTimecode!=INVALID_TIMECODE_T)
                MATROSKA_BlockSetTimecode((matroska_block*)Elt, BlockTimecode, ClusterTimecode);
        }
    }
#endif
    return ERR_NONE;
}

static err_t CheckCompression(matroska_block *Block)
{
    ebml_master *Elt, *Header;
    assert(Block->ReadTrack!=NULL);
    Elt = (ebml_master*)EBML_MasterFindChild(Block->ReadTrack, &MATROSKA_ContextContentEncodings);
    if (Elt)
    {
        if (ARRAYCOUNT(Block->Data,uint8_t))
            return ERR_INVALID_PARAM; // we cannot adjust sizes if the data are already read

        Elt = (ebml_master*)EBML_MasterFindChild(Elt, &MATROSKA_ContextContentEncoding);
        if (EBML_MasterChildren(Elt))
        {
            if (EBML_MasterNext(Elt))
                return ERR_INVALID_DATA; // TODO support cascaded compression/encryption

            Elt = (ebml_master*)EBML_MasterFindChild(Elt, &MATROSKA_ContextContentCompression);
            if (!Elt)
                return ERR_INVALID_DATA; // TODO: support encryption

            Header = (ebml_master*)EBML_MasterGetChild(Elt, &MATROSKA_ContextContentCompAlgo);
#if defined(CONFIG_ZLIB) || defined(CONFIG_LZO1X) || defined(CONFIG_BZLIB)
            if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#if defined(CONFIG_ZLIB)
                    if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#endif
#if defined(CONFIG_LZO1X)
                    if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_LZO1X)
#endif
#if defined(CONFIG_BZLIB)
                    if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_BZLIB)
#endif
#else
            if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                return ERR_INVALID_DATA;

            if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_HEADER)
            {
                Header = (ebml_master*)EBML_MasterFindChild(Elt, &MATROSKA_ContextContentCompSettings);
                if (Header)
                {
                    uint32_t *i;
        		    for (i=ARRAYBEGIN(Block->SizeList,uint32_t);i!=ARRAYEND(Block->SizeList,uint32_t);++i)
                        *i += (uint32_t)Header->Base.DataSize;
                }
            }
        }
    }
    return ERR_NONE;
}

err_t MATROSKA_LinkBlockWithReadTracks(matroska_block *Block, ebml_master *Tracks, bool_t UseForWriteToo)
{
    ebml_element *Track;
    ebml_integer *TrackNum;
    bool_t WasLinked = Block->ReadTrack!=NULL;

    assert(EBML_ElementIsType((ebml_element*)Tracks, &MATROSKA_ContextTracks));
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    for (Track=EBML_MasterChildren(Tracks);Track;Track=EBML_MasterNext(Track))
    {
        TrackNum = (ebml_integer*)EBML_MasterFindChild((ebml_master*)Track,&MATROSKA_ContextTrackNumber);
        if (TrackNum && ((ebml_element*)TrackNum)->bValueIsSet && EBML_IntegerValue(TrackNum)==Block->TrackNumber)
        {
            Node_SET(Block,MATROSKA_BLOCK_READ_TRACK,&Track);
#if defined(CONFIG_EBML_WRITING)
            if (UseForWriteToo)
                Node_SET(Block,MATROSKA_BLOCK_WRITE_TRACK,&Track);
#endif
            if (WasLinked)
                return ERR_NONE;
            return CheckCompression(Block);
        }
    }
    return ERR_INVALID_DATA;
}

err_t MATROSKA_LinkBlockReadTrack(matroska_block *Block, ebml_master *Track, bool_t UseForWriteToo)
{
    ebml_integer *TrackNum;
    bool_t WasLinked = Block->ReadTrack!=NULL;

    assert(EBML_ElementIsType((ebml_element*)Track, &MATROSKA_ContextTrackEntry));
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    TrackNum = (ebml_integer*)EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber);
    if (TrackNum && ((ebml_element*)TrackNum)->bValueIsSet)
    {
        Block->TrackNumber = (uint16_t)EBML_IntegerValue(TrackNum);
        Node_SET(Block,MATROSKA_BLOCK_READ_TRACK,&Track);
#if defined(CONFIG_EBML_WRITING)
        if (UseForWriteToo)
            Node_SET(Block,MATROSKA_BLOCK_WRITE_TRACK,&Track);
#endif
        if (WasLinked)
            return ERR_NONE;
        return CheckCompression(Block);
    }
    return ERR_INVALID_DATA;
}

#if defined(CONFIG_EBML_WRITING)
err_t MATROSKA_LinkBlockWithWriteTracks(matroska_block *Block, ebml_master *Tracks)
{
    ebml_master *Track;
    ebml_integer *TrackNum;
    bool_t WasLinked = Block->WriteTrack!=NULL;

    assert(EBML_ElementIsType((ebml_element*)Tracks, &MATROSKA_ContextTracks));
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    for (Track=(ebml_master*)EBML_MasterChildren(Tracks);Track;Track=(ebml_master*)EBML_MasterNext(Track))
    {
        TrackNum = (ebml_integer*)EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber);
        if (TrackNum && ((ebml_element*)TrackNum)->bValueIsSet && EBML_IntegerValue(TrackNum)==Block->TrackNumber)
        {
            Node_SET(Block,MATROSKA_BLOCK_WRITE_TRACK,&Track);
            if (WasLinked)
                return ERR_NONE;
            return CheckCompression(Block);
        }
    }
    return ERR_INVALID_DATA;
}

err_t MATROSKA_LinkBlockWriteTrack(matroska_block *Block, ebml_master *Track)
{
    ebml_integer *TrackNum;
    bool_t WasLinked = Block->WriteTrack!=NULL;

    assert(EBML_ElementIsType((ebml_element*)Track, &MATROSKA_ContextTrackEntry));
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    TrackNum = (ebml_integer*)EBML_MasterFindChild(Track,&MATROSKA_ContextTrackNumber);
    if (TrackNum && ((ebml_element*)TrackNum)->bValueIsSet)
    {
        Block->TrackNumber = (uint16_t)EBML_IntegerValue(TrackNum);
        Node_SET(Block,MATROSKA_BLOCK_WRITE_TRACK,&Track);
        if (WasLinked)
            return ERR_NONE;
        return CheckCompression(Block);
    }
    return ERR_INVALID_DATA;
}
#endif

ebml_element *MATROSKA_BlockReadTrack(const matroska_block *Block)
{
    ebml_element *Track;
    if (Node_GET((node*)Block,MATROSKA_BLOCK_READ_TRACK,&Track)!=ERR_NONE)
        return NULL;
    return Track;
}

#if defined(CONFIG_EBML_WRITING)
ebml_element *MATROSKA_BlockWriteTrack(const matroska_block *Block)
{
    ebml_element *Track;
    if (Node_GET((node*)Block,MATROSKA_BLOCK_WRITE_TRACK,&Track)!=ERR_NONE)
        return NULL;
    return Track;
}
#endif

err_t MATROSKA_LinkMetaSeekElement(matroska_seekpoint *MetaSeek, ebml_element *Link)
{
    assert(EBML_ElementIsType((ebml_element*)MetaSeek, &MATROSKA_ContextSeek));
    Node_SET(MetaSeek,MATROSKA_SEEKPOINT_ELEMENT,&Link);
    return ERR_NONE;
}

fourcc_t MATROSKA_MetaSeekID(const matroska_seekpoint *MetaSeek)
{
	ebml_element *SeekID;
    assert(EBML_ElementIsType((ebml_element*)MetaSeek, &MATROSKA_ContextSeek));
    SeekID = EBML_MasterFindChild((ebml_master*)MetaSeek, &MATROSKA_ContextSeekID);
	if (!SeekID)
		return 0;
	return EBML_BufferToID(EBML_BinaryGetData((ebml_binary*)SeekID));
}

bool_t MATROSKA_MetaSeekIsClass(const matroska_seekpoint *MetaSeek, const ebml_context *Class)
{
    return MATROSKA_MetaSeekID(MetaSeek) == Class->Id;
}

filepos_t MATROSKA_MetaSeekPosInSegment(const matroska_seekpoint *MetaSeek)
{
	ebml_integer *SeekPos;
    assert(EBML_ElementIsType((ebml_element*)MetaSeek, &MATROSKA_ContextSeek));
	SeekPos = (ebml_integer*)EBML_MasterFindChild((ebml_master*)MetaSeek, &MATROSKA_ContextSeekPosition);
	if (!SeekPos)
		return INVALID_FILEPOS_T;
	return EBML_IntegerValue(SeekPos);
}

filepos_t MATROSKA_MetaSeekAbsolutePos(const matroska_seekpoint *MetaSeek)
{
	filepos_t RelPos = MATROSKA_MetaSeekPosInSegment(MetaSeek);
	ebml_element *RSegment;
	if (RelPos==INVALID_FILEPOS_T)
		return INVALID_FILEPOS_T;

    RSegment = EBML_ElementParent(MetaSeek);
    while (RSegment && !EBML_ElementIsType(RSegment, &MATROSKA_ContextSegment))
        RSegment = EBML_ElementParent(RSegment);
    if (!RSegment)
        return INVALID_FILEPOS_T;

	return RelPos + EBML_ElementPositionData(RSegment);
}

err_t MATROSKA_MetaSeekUpdate(matroska_seekpoint *MetaSeek)
{
    ebml_element *WSeekID, *WSeekPosSegmentInfo, *RSegment, *Link = NULL;
    size_t IdSize;
    err_t Err;
    uint8_t IdBuffer[4];

    if (Node_IsPartOf(MetaSeek,EBML_VOID_CLASS))
        return ERR_NONE;

    assert(EBML_ElementIsType((ebml_element*)MetaSeek, &MATROSKA_ContextSeek));
    RSegment = EBML_ElementParent(MetaSeek);
    while (RSegment && !EBML_ElementIsType(RSegment, &MATROSKA_ContextSegment))
        RSegment = EBML_ElementParent(RSegment);
    if (!RSegment)
        return ERR_INVALID_DATA;

    Err = Node_GET(MetaSeek,MATROSKA_SEEKPOINT_ELEMENT,&Link);
    if (Err != ERR_NONE)
        return Err;
    if (Link==NULL)
        return ERR_INVALID_DATA;

    WSeekID = EBML_MasterFindFirstElt((ebml_master*)MetaSeek,&MATROSKA_ContextSeekID,1,0);
    IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),Link->Context->Id);
    EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);

    WSeekPosSegmentInfo = EBML_MasterFindFirstElt((ebml_master*)MetaSeek,&MATROSKA_ContextSeekPosition,1,0);
    EBML_IntegerSetValue((ebml_integer*)WSeekPosSegmentInfo, Link->ElementPosition - EBML_ElementPositionData(RSegment));

    return Err;
}

err_t MATROSKA_LinkClusterReadSegmentInfo(matroska_cluster *Cluster, ebml_master *SegmentInfo, bool_t UseForWriteToo)
{
    assert(EBML_ElementIsType((ebml_element*)Cluster, &MATROSKA_ContextCluster));
    assert(EBML_ElementIsType((ebml_element*)SegmentInfo, &MATROSKA_ContextInfo));
    Node_SET(Cluster,MATROSKA_CLUSTER_READ_SEGMENTINFO,&SegmentInfo);
    if (UseForWriteToo)
        Node_SET(Cluster,MATROSKA_CLUSTER_WRITE_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}

#if defined(CONFIG_EBML_WRITING)
err_t MATROSKA_LinkClusterWriteSegmentInfo(matroska_cluster *Cluster, ebml_master *SegmentInfo)
{
    assert(EBML_ElementIsType((ebml_element*)Cluster, &MATROSKA_ContextCluster));
    assert(EBML_ElementIsType((ebml_element*)SegmentInfo, &MATROSKA_ContextInfo));
    Node_SET(Cluster,MATROSKA_CLUSTER_WRITE_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}
#endif

err_t MATROSKA_LinkBlockReadSegmentInfo(matroska_block *Block, ebml_master *SegmentInfo, bool_t UseForWriteToo)
{
    assert(EBML_ElementIsType((ebml_element*)SegmentInfo, &MATROSKA_ContextInfo));
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    Node_SET(Block,MATROSKA_BLOCK_READ_SEGMENTINFO,&SegmentInfo);
#if defined(CONFIG_EBML_WRITING)
    if (UseForWriteToo)
        Node_SET(Block,MATROSKA_BLOCK_WRITE_SEGMENTINFO,&SegmentInfo);
#endif
    return ERR_NONE;
}

#if defined(CONFIG_EBML_WRITING)
err_t MATROSKA_LinkBlockWriteSegmentInfo(matroska_block *Block, ebml_master *SegmentInfo)
{
    assert(EBML_ElementIsType((ebml_element*)SegmentInfo, &MATROSKA_ContextInfo));
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    Node_SET(Block,MATROSKA_BLOCK_WRITE_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}
#endif

ebml_element *MATROSKA_BlockReadSegmentInfo(const matroska_block *Block)
{
    ebml_element *SegmentInfo;
    if (Node_GET((node*)Block,MATROSKA_BLOCK_READ_SEGMENTINFO,&SegmentInfo)!=ERR_NONE)
        return NULL;
    return SegmentInfo;
}

#if defined(CONFIG_EBML_WRITING)
ebml_element *MATROSKA_BlockWriteSegmentInfo(const matroska_block *Block)
{
    ebml_element *SegmentInfo;
    if (Node_GET((node*)Block,MATROSKA_BLOCK_WRITE_SEGMENTINFO,&SegmentInfo)!=ERR_NONE)
        return NULL;
    return SegmentInfo;
}
#endif

err_t MATROSKA_LinkCueSegmentInfo(matroska_cuepoint *Cue, ebml_master *SegmentInfo)
{
    assert(EBML_ElementIsType((ebml_element*)Cue, &MATROSKA_ContextCuePoint));
    assert(EBML_ElementIsType((ebml_element*)SegmentInfo, &MATROSKA_ContextInfo));
    Node_SET(Cue,MATROSKA_CUE_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}

err_t MATROSKA_LinkCuePointBlock(matroska_cuepoint *CuePoint, matroska_block *Block)
{
    assert(EBML_ElementIsType((ebml_element*)CuePoint, &MATROSKA_ContextCuePoint));
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    Node_SET(CuePoint,MATROSKA_CUE_BLOCK,&Block);
    return ERR_NONE;
}

static int MATROSKA_BlockCmp(const matroska_block *BlockA, const matroska_block *BlockB)
{
    timecode_t TimeA = MATROSKA_BlockTimecode((matroska_block*)BlockA);
    timecode_t TimeB = MATROSKA_BlockTimecode((matroska_block*)BlockB);
    if (TimeA != TimeB)
        return (int)((TimeA - TimeB)/100000);
    return MATROSKA_BlockTrackNum(BlockB) - MATROSKA_BlockTrackNum(BlockA); // usually the first track is video, so put audio/subs first
}

static int ClusterEltCmp(const matroska_cluster* Cluster, const ebml_element** a,const ebml_element** b)
{
    const matroska_block *BlockA = NULL,*BlockB = NULL;
    if (EBML_ElementIsType(*a, &MATROSKA_ContextTimecode))
        return -1;
    if (EBML_ElementIsType(*b, &MATROSKA_ContextTimecode))
        return 1;

    if (EBML_ElementIsType(*a, &MATROSKA_ContextSimpleBlock))
        BlockA = (const matroska_block *)*a;
    else if (EBML_ElementIsType(*a, &MATROSKA_ContextBlockGroup))
        BlockA = (const matroska_block *)EBML_MasterFindChild((ebml_master*)*a,&MATROSKA_ContextBlock);
    if (EBML_ElementIsType(*b, &MATROSKA_ContextSimpleBlock))
        BlockB = (const matroska_block *)*b;
    else if (EBML_ElementIsType(*a, &MATROSKA_ContextBlockGroup))
        BlockB = (const matroska_block *)EBML_MasterFindChild((ebml_master*)*b,&MATROSKA_ContextBlock);
    if (BlockA != NULL && BlockB != NULL)
        return MATROSKA_BlockCmp(BlockA,BlockB);

    assert(0); // unsupported comparison
    return 0;
}

void MATROSKA_ClusterSort(matroska_cluster *Cluster)
{
    EBML_MasterSort((ebml_master*)Cluster,(arraycmp)ClusterEltCmp,Cluster);
}

void MATROSKA_ClusterSetTimecode(matroska_cluster *Cluster, timecode_t Timecode)
{
	ebml_integer *TimecodeElt;
#if defined(CONFIG_EBML_WRITING)
    ebml_element *Elt, *GBlock;
    timecode_t BlockTimeCode;
#endif

    assert(EBML_ElementIsType((ebml_element*)Cluster, &MATROSKA_ContextCluster));
    Cluster->GlobalTimecode = Timecode;
    TimecodeElt = (ebml_integer*)EBML_MasterGetChild((ebml_master*)Cluster,&MATROSKA_ContextTimecode);
#if defined(CONFIG_EBML_WRITING)
	assert(Cluster->WriteSegInfo);
	EBML_IntegerSetValue(TimecodeElt, Scale64(Timecode,1,MATROSKA_SegmentInfoTimecodeScale(Cluster->WriteSegInfo)));
    // update all the blocks LocalTimecode
    for (Elt = EBML_MasterChildren(Cluster); Elt; Elt = EBML_MasterNext(Elt))
    {
        if (EBML_ElementIsType(Elt, &MATROSKA_ContextBlockGroup))
        {
            for (GBlock = EBML_MasterChildren(Elt);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(GBlock, &MATROSKA_ContextBlock))
                {
                    BlockTimeCode = MATROSKA_BlockTimecode((matroska_block*)GBlock);
                    if (BlockTimeCode!=INVALID_TIMECODE_T)
                        MATROSKA_BlockSetTimecode((matroska_block*)GBlock, BlockTimeCode, Timecode);
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(Elt, &MATROSKA_ContextSimpleBlock))
        {
            BlockTimeCode = MATROSKA_BlockTimecode((matroska_block*)Elt);
            if (BlockTimeCode!=INVALID_TIMECODE_T)
                MATROSKA_BlockSetTimecode((matroska_block*)Elt, BlockTimeCode, Timecode);
        }
    }
#else
	assert(Cluster->ReadSegInfo);
	EBML_IntegerSetValue(TimecodeElt, Scale64(Timecode,1,MATROSKA_SegmentInfoTimecodeScale(Cluster->ReadSegInfo)));
#endif
}

timecode_t MATROSKA_ClusterTimecode(matroska_cluster *Cluster)
{
    assert(EBML_ElementIsType((ebml_element*)Cluster, &MATROSKA_ContextCluster));
    if (Cluster->GlobalTimecode == INVALID_TIMECODE_T)
    {
        ebml_integer *Timecode = (ebml_integer*)EBML_MasterFindChild((ebml_master*)Cluster,&MATROSKA_ContextTimecode);
        if (Timecode)
            Cluster->GlobalTimecode = EBML_IntegerValue(Timecode) * MATROSKA_SegmentInfoTimecodeScale(Cluster->ReadSegInfo);
    }
    return Cluster->GlobalTimecode;
}

timecode_t MATROSKA_ClusterTimecodeScale(matroska_cluster *Cluster, bool_t Read)
{
#if defined(CONFIG_EBML_WRITING)
    if (!Read)
        return MATROSKA_SegmentInfoTimecodeScale(Cluster->WriteSegInfo);
#endif
    return MATROSKA_SegmentInfoTimecodeScale(Cluster->ReadSegInfo);
}

err_t MATROSKA_BlockSetTimecode(matroska_block *Block, timecode_t Timecode, timecode_t ClusterTimecode)
{
	int64_t InternalTimecode;
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    assert(Timecode!=INVALID_TIMECODE_T);
#if defined(CONFIG_EBML_WRITING)
	InternalTimecode = Scale64(Timecode - ClusterTimecode,1,(int64_t)(MATROSKA_SegmentInfoTimecodeScale(Block->WriteSegInfo) * MATROSKA_TrackTimecodeScale(Block->WriteTrack)));
#else
	InternalTimecode = Scale64(Timecode - ClusterTimecode,1,(int64_t)(MATROSKA_SegmentInfoTimecodeScale(Block->ReadSegInfo) * MATROSKA_TrackTimecodeScale(Block->ReadTrack)));
#endif
	if (InternalTimecode > 32767 || InternalTimecode < -32768)
		return ERR_INVALID_DATA;
	Block->LocalTimecode = (int16_t)InternalTimecode;
    Block->LocalTimecodeUsed = 1;
	return ERR_NONE;
}

timecode_t MATROSKA_BlockTimecode(matroska_block *Block)
{
    ebml_element *Cluster;
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	if (Block->GlobalTimecode!=INVALID_TIMECODE_T)
		return Block->GlobalTimecode;
    if (Block->ReadTrack==NULL)
        return INVALID_TIMECODE_T;
    assert(Block->LocalTimecodeUsed);
    Cluster = EBML_ElementParent(Block);
    while (Cluster && !EBML_ElementIsType(Cluster, &MATROSKA_ContextCluster))
        Cluster = EBML_ElementParent(Cluster);
    if (!Cluster)
        return INVALID_TIMECODE_T;
    Block->GlobalTimecode = MATROSKA_ClusterTimecode((matroska_cluster*)Cluster) + (timecode_t)(Block->LocalTimecode * MATROSKA_SegmentInfoTimecodeScale(Block->ReadSegInfo) * MATROSKA_TrackTimecodeScale(Block->ReadTrack));
    MATROSKA_BlockSetTimecode(Block, Block->GlobalTimecode, MATROSKA_ClusterTimecode((matroska_cluster*)Cluster));
    return Block->GlobalTimecode;
}

int16_t MATROSKA_BlockTrackNum(const matroska_block *Block)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    assert(Block->LocalTimecodeUsed);
    return Block->TrackNumber;
}

bool_t MATROSKA_BlockKeyframe(const matroska_block *Block)
{
    ebml_master *BlockGroup;
    ebml_integer *Duration;

    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    if (Block->IsKeyframe)
        return 1;

	if (!EBML_ElementIsType((const ebml_element*)Block, &MATROSKA_ContextBlock))
        return 0;
	
	BlockGroup = (ebml_master*)EBML_ElementParent(Block);
    if (!BlockGroup || !Node_IsPartOf(BlockGroup,MATROSKA_BLOCKGROUP_CLASS))
        return 0;

	if (EBML_MasterFindChild(BlockGroup,&MATROSKA_ContextReferenceBlock))
        return 0;

    Duration = (ebml_integer*)EBML_MasterFindChild(BlockGroup,&MATROSKA_ContextBlockDuration);
	if (Duration!=NULL && EBML_IntegerValue(Duration)==0)
        return 0;

    return 1;
}

bool_t MATROSKA_BlockDiscardable(const matroska_block *Block)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	if (EBML_ElementIsType((const ebml_element*)Block, &MATROSKA_ContextBlock))
        return 0;
	return Block->IsDiscardable;
}

void MATROSKA_BlockSetKeyframe(matroska_block *Block, bool_t Set)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	Block->IsKeyframe = Set;
}

void MATROSKA_BlockSetDiscardable(matroska_block *Block, bool_t Set)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	if (EBML_ElementIsType((const ebml_element*)Block, &MATROSKA_ContextSimpleBlock))
    	Block->IsDiscardable = Set;
}

bool_t MATROSKA_BlockLaced(const matroska_block *Block)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    assert(Block->LocalTimecodeUsed);
    return Block->Lacing != LACING_NONE;
}

int16_t MATROSKA_CueTrackNum(const matroska_cuepoint *Cue)
{
    ebml_master *Position;
    ebml_integer *CueTrack;
    assert(EBML_ElementIsType((ebml_element*)Cue, &MATROSKA_ContextCuePoint));
    Position = (ebml_master*)EBML_MasterFindChild((ebml_master*)Cue,&MATROSKA_ContextCueTrackPositions);
    if (!Position)
        return -1;
    CueTrack = (ebml_integer*)EBML_MasterFindChild(Position,&MATROSKA_ContextCueTrack);
    if (!CueTrack)
        return -1;
    return (int16_t)EBML_IntegerValue(CueTrack);
}

void MATROSKA_CuesSort(ebml_master *Cues)
{
    assert(EBML_ElementIsType((ebml_element*)Cues, &MATROSKA_ContextCues));
    EBML_MasterSort(Cues,NULL,NULL);
}

void MATROSKA_AttachmentSort(ebml_master *Attachments)
{
    assert(EBML_ElementIsType((ebml_element*)Attachments, &MATROSKA_ContextAttachments));
    EBML_MasterSort(Attachments,NULL,NULL);
}

timecode_t MATROSKA_SegmentInfoTimecodeScale(const ebml_master *SegmentInfo)
{
    ebml_integer *TimecodeScale = NULL;
    if (SegmentInfo)
    {
        assert(EBML_ElementIsType((ebml_element*)SegmentInfo, &MATROSKA_ContextInfo));
        TimecodeScale = (ebml_integer*)EBML_MasterFindChild((ebml_master*)SegmentInfo,&MATROSKA_ContextTimecodeScale);
    }
    if (!TimecodeScale)
        return MATROSKA_ContextTimecodeScale.DefaultValue;
    return EBML_IntegerValue(TimecodeScale);
}

double MATROSKA_TrackTimecodeScale(const ebml_master *Track)
{
    ebml_element *TimecodeScale;
    assert(EBML_ElementIsType((ebml_element*)Track, &MATROSKA_ContextTrackEntry));
    TimecodeScale = EBML_MasterFindChild((ebml_master*)Track,&MATROSKA_ContextTrackTimecodeScale);
    if (!TimecodeScale)
        return MATROSKA_ContextTrackTimecodeScale.DefaultValue;
    return ((ebml_float*)TimecodeScale)->Value;
}

timecode_t MATROSKA_CueTimecode(const matroska_cuepoint *Cue)
{
    ebml_integer *TimeCode;
    assert(EBML_ElementIsType((ebml_element*)Cue, &MATROSKA_ContextCuePoint));
    TimeCode = (ebml_integer*) EBML_MasterFindChild((ebml_master*)Cue,&MATROSKA_ContextCueTime);
    if (!TimeCode)
        return INVALID_TIMECODE_T;
    return EBML_IntegerValue(TimeCode) * MATROSKA_SegmentInfoTimecodeScale(Cue->SegInfo);
}

filepos_t MATROSKA_CuePosInSegment(const matroska_cuepoint *Cue)
{
    ebml_element *TimeCode;
    assert(EBML_ElementIsType((ebml_element*)Cue, &MATROSKA_ContextCuePoint));
    TimeCode = EBML_MasterFindChild((ebml_master*)Cue,&MATROSKA_ContextCueTrackPositions);
    if (!TimeCode)
        return INVALID_TIMECODE_T;
    TimeCode = EBML_MasterFindChild((ebml_master*)TimeCode,&MATROSKA_ContextCueClusterPosition);
    if (!TimeCode)
        return INVALID_TIMECODE_T;
    return EBML_IntegerValue((ebml_integer*)TimeCode);
}

err_t MATROSKA_CuePointUpdate(matroska_cuepoint *Cue, ebml_element *Segment)
{
    ebml_element *TimecodeElt, *Elt, *PosInCluster;
    ebml_integer *TrackNum;
    assert(EBML_ElementIsType((ebml_element*)Cue, &MATROSKA_ContextCuePoint));
    assert(Cue->Block);
    assert(Cue->SegInfo);
    assert(Segment); // we need the segment location
	EBML_MasterErase((ebml_master*)Cue);
	EBML_MasterAddMandatory((ebml_master*)Cue,1);
    TimecodeElt = EBML_MasterGetChild((ebml_master*)Cue,&MATROSKA_ContextCueTime);
    if (!TimecodeElt)
        return ERR_OUT_OF_MEMORY;
    EBML_IntegerSetValue((ebml_integer*)TimecodeElt, Scale64(MATROSKA_BlockTimecode(Cue->Block),1,MATROSKA_SegmentInfoTimecodeScale(Cue->SegInfo)));

    Elt = EBML_MasterGetChild((ebml_master*)Cue,&MATROSKA_ContextCueTrackPositions);
    if (!Elt)
        return ERR_OUT_OF_MEMORY;
	TrackNum = (ebml_integer*)EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextCueTrack);
    if (!TrackNum)
        return ERR_OUT_OF_MEMORY;
	EBML_IntegerSetValue(TrackNum, MATROSKA_BlockTrackNum(Cue->Block));
	
    PosInCluster = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextCueClusterPosition);
    if (!PosInCluster)
        return ERR_OUT_OF_MEMORY;
    Elt = EBML_ElementParent(Cue->Block);
    while (Elt && !EBML_ElementIsType(Elt, &MATROSKA_ContextCluster))
        Elt = EBML_ElementParent(Elt);
    if (!Elt)
        return ERR_INVALID_DATA;

    assert(Elt->ElementPosition != INVALID_FILEPOS_T);
    
    EBML_IntegerSetValue((ebml_integer*)PosInCluster, Elt->ElementPosition - EBML_ElementPositionData(Segment));

    return ERR_NONE;
}

matroska_block *MATROSKA_GetBlockForTimecode(matroska_cluster *Cluster, timecode_t Timecode, int16_t Track)
{
    ebml_element *Block, *GBlock;
    for (Block = EBML_MasterChildren(Cluster);Block;Block=EBML_MasterNext(Block))
    {
        if (EBML_ElementIsType(Block, &MATROSKA_ContextBlockGroup))
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(GBlock, &MATROSKA_ContextBlock))
                {
                    if (MATROSKA_BlockTrackNum((matroska_block*)GBlock) == Track &&
                        MATROSKA_BlockTimecode((matroska_block*)GBlock) == Timecode)
                    {
                        return (matroska_block*)GBlock;
                    }
                }
            }
        }
        else if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
        {
            if (MATROSKA_BlockTrackNum((matroska_block*)Block) == Track &&
                MATROSKA_BlockTimecode((matroska_block*)Block) == Timecode)
            {
                return (matroska_block*)Block;
            }
        }
    }
    return NULL;
}

void MATROSKA_LinkClusterBlocks(matroska_cluster *Cluster, ebml_master *RSegmentInfo, ebml_master *Tracks, bool_t KeepUnmatched)
{
    ebml_element *Block, *GBlock,*NextBlock;

	assert(Node_IsPartOf(Cluster,MATROSKA_CLUSTER_CLASS));
	assert(EBML_ElementIsType((ebml_element*)RSegmentInfo, &MATROSKA_ContextInfo));
	assert(EBML_ElementIsType((ebml_element*)Tracks, &MATROSKA_ContextTracks));

	// link each Block/SimpleBlock with its Track and SegmentInfo
	MATROSKA_LinkClusterReadSegmentInfo(Cluster,RSegmentInfo,1);
	for (Block = EBML_MasterChildren(Cluster);Block;Block=NextBlock)
	{
        NextBlock = EBML_MasterNext(Block);
		if (EBML_ElementIsType(Block, &MATROSKA_ContextBlockGroup))
		{
			for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
			{
				if (EBML_ElementIsType(GBlock, &MATROSKA_ContextBlock))
				{
					if (MATROSKA_LinkBlockWithReadTracks((matroska_block*)GBlock,Tracks,1)!=ERR_NONE && !KeepUnmatched)
                        NodeDelete((node*)Block);
                    else
					    MATROSKA_LinkBlockReadSegmentInfo((matroska_block*)GBlock,RSegmentInfo,1);
					break;
				}
			}
		}
		else if (EBML_ElementIsType(Block, &MATROSKA_ContextSimpleBlock))
		{
			if (MATROSKA_LinkBlockWithReadTracks((matroska_block*)Block,Tracks,1)!=ERR_NONE && !KeepUnmatched)
                NodeDelete((node*)Block);
            else
    			MATROSKA_LinkBlockReadSegmentInfo((matroska_block*)Block,RSegmentInfo,1);
		}
	}
}


static size_t GetBlockHeadSize(const matroska_block *Element)
{
    assert(Element->TrackNumber < 0x4000);
    if (Element->TrackNumber < 0x80)
        return 4;
    else
        return 5;
}

err_t MATROSKA_BlockReleaseData(matroska_block *Block, bool_t IncludingNotRead)
{
    if (!IncludingNotRead && Block->GlobalTimecode==INVALID_TIMECODE_T)
        return ERR_NONE;
    ArrayClear(&Block->Data);
    Block->Base.Base.bValueIsSet = 0;
    if (ARRAYCOUNT(Block->SizeListIn,int32_t))
    {
        // recover the size of each lace in SizeList for later reading
        int32_t *i,*o;
        assert(ARRAYCOUNT(Block->SizeListIn,int32_t) == ARRAYCOUNT(Block->SizeList,int32_t));
        for (i=ARRAYBEGIN(Block->SizeListIn,int32_t),o=ARRAYBEGIN(Block->SizeList,int32_t);i!=ARRAYEND(Block->SizeListIn,int32_t);++i,++o)
            *o = *i;
        ArrayClear(&Block->SizeListIn);
    }
    return ERR_NONE;
}

err_t MATROSKA_BlockSkipToFrame(const matroska_block *Block, stream *Input, size_t FrameNum)
{
	uint32_t *i;
	filepos_t SeekPos = EBML_ElementPositionData((ebml_element*)Block);
	if (FrameNum >= ARRAYCOUNT(Block->SizeList,uint32_t))
		return ERR_INVALID_PARAM;
	if (Block->Lacing == LACING_NONE)
		SeekPos += GetBlockHeadSize(Block);
	else
	{
		SeekPos = Block->FirstFrameLocation;
		for (i=ARRAYBEGIN(Block->SizeList,uint32_t);FrameNum;--FrameNum,++i)
			SeekPos += *i;
	}
	if (Stream_Seek(Input,SeekPos,SEEK_SET) != SeekPos)
		return ERR_READ;
	return ERR_NONE;
}

// TODO: support zero copy reading (read the frames directly into a buffer with a callback per frame)
//       pass the Input stream and the amount to read per frame, give the timecode of the frame and get the end timecode in return, get an error code if reading failed
err_t MATROSKA_BlockReadData(matroska_block *Element, stream *Input)
{
    size_t Read,BufSize;
    size_t NumFrame;
    err_t Err = ERR_NONE;
    ebml_element *Elt, *Elt2, *Header = NULL;
    uint8_t *InBuf;
    int CompressionScope = MATROSKA_COMPR_SCOPE_BLOCK;

    if (!Element->Base.Base.bValueIsSet)
    {
        // find out if compressed headers are used
        assert(Element->ReadTrack!=NULL);
        Elt = EBML_MasterFindChild(Element->ReadTrack, &MATROSKA_ContextContentEncodings);
        if (Elt)
        {
            Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncoding);
            if (EBML_MasterChildren(Elt))
            {
                if (EBML_MasterNext(Elt))
                    return ERR_NOT_SUPPORTED; // TODO support cascaded compression/encryption

                Elt2 = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncodingScope);
                if (Elt2)
                    CompressionScope = (int)EBML_IntegerValue((ebml_integer*)Elt2);

                Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompression);
                if (!Elt)
                    return ERR_NOT_SUPPORTED; // TODO: support encryption

                Header = EBML_MasterGetChild((ebml_master*)Elt, &MATROSKA_ContextContentCompAlgo);
#if defined(CONFIG_ZLIB) || defined(CONFIG_LZO1X) || defined(CONFIG_BZLIB)
                if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#if defined(CONFIG_ZLIB)
                    if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#endif
#if defined(CONFIG_LZO1X)
                    if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_LZO1X)
#endif
#if defined(CONFIG_BZLIB)
                    if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_BZLIB)
#endif
#else
                if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                    return ERR_INVALID_DATA;

                if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_HEADER)
                    Header = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompSettings);
            }
        }

#if !defined(CONFIG_ZLIB) && !defined(CONFIG_LZO1X) && !defined(CONFIG_BZLIB)
        if (Header && Header->Context==&MATROSKA_ContextContentCompAlgo)
            return ERR_NOT_SUPPORTED;
#endif

        if (Header && Header->Context==&MATROSKA_ContextContentCompAlgo && !(CompressionScope & MATROSKA_COMPR_SCOPE_BLOCK))
            Header = NULL;

        Stream_Seek(Input,Element->FirstFrameLocation,SEEK_SET);
        if (Header)
            ArrayCopy(&Element->SizeListIn, &Element->SizeList);
        switch (Element->Lacing)
        {
        case LACING_NONE:
#if defined(CONFIG_ZLIB) || defined(CONFIG_LZO1X) || defined(CONFIG_BZLIB)
            if (Header && Header->Context==&MATROSKA_ContextContentCompAlgo)
            {
                // zlib handling, read the buffer in temp memory
                array TmpBuf;
                ArrayInit(&TmpBuf);
                if (!ArrayResize(&TmpBuf,(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0],0))
                    Err = ERR_OUT_OF_MEMORY;
                InBuf = ARRAYBEGIN(TmpBuf,uint8_t);
                Err = Stream_Read(Input,InBuf,(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0],&Read);
                if (Err==ERR_NONE)
                {
                    if (Read!=(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0])
                        Err = ERR_READ;
                    else
                    {
#if defined(CONFIG_ZLIB)
                        if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_ZLIB)
                        {
                            // get the ouput size, adjust the Element->SizeList value, write in Element->Data
                            z_stream stream;
                            int Res;
                            memset(&stream,0,sizeof(stream));
                            Res = inflateInit(&stream);
                            if (Res != Z_OK)
                                Err = ERR_INVALID_DATA;
                            else
                            {
                                size_t Count = 0;
                                stream.next_in = InBuf;
                                stream.avail_in = ARRAYBEGIN(Element->SizeList,int32_t)[0];
                                stream.next_out = ARRAYBEGIN(Element->Data,uint8_t);
                                do {
                                    Count = stream.next_out - ARRAYBEGIN(Element->Data,uint8_t);
                                    stream.avail_out = 1024;
                                    if (!ArrayResize(&Element->Data, Count + stream.avail_out, 0))
                                    {
                                        Res = Z_MEM_ERROR;
                                        break;
                                    }
                                    stream.next_out = ARRAYBEGIN(Element->Data,uint8_t) + Count;
                                    Res = inflate(&stream, Z_NO_FLUSH);
                                    if (Res!=Z_STREAM_END && Res!=Z_OK)
                                        break;
                                } while (Res!=Z_STREAM_END && stream.avail_in && !stream.avail_out);
                                ArrayResize(&Element->Data, stream.total_out, 0);
                                ARRAYBEGIN(Element->SizeList,int32_t)[0] = stream.total_out;
                                inflateEnd(&stream);
                                if (Res != Z_STREAM_END)
                                    Err = ERR_INVALID_DATA;
                            }
                        }
#endif
#if defined(CONFIG_LZO1X)
                        if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_LZO1X)
                        {
                            if (lzo_init() != LZO_E_OK)
                                Err = ERR_INVALID_DATA;
                            else
                            {
                                lzo_uint outSize = max(2048, ARRAYBEGIN(Element->SizeList,int32_t)[0] << 2);
                                if (!ArrayResize(&Element->Data, outSize, 0))
                                    Err = ERR_OUT_OF_MEMORY;
                                else
                                {
                                    if (lzo1x_decompress_safe(InBuf, ARRAYBEGIN(Element->SizeList,int32_t)[0], ARRAYBEGIN(Element->Data,uint8_t), &outSize, NULL) != LZO_E_OK)
                                        Err = ERR_INVALID_DATA;
                                    else
                                    {
                                        ARRAYBEGIN(Element->SizeList,int32_t)[0] = outSize;
                                        ArrayResize(&Element->Data,outSize,0);
                                    }
                                }
                            }
                        }
#endif
#if defined(CONFIG_BZLIB)
                        if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_BZLIB)
                        {
                            unsigned int outSize = ARRAYBEGIN(Element->SizeList,int32_t)[0] << 2;
                            bz_stream strm;
                            int Res;

                            if (!ArrayResize(&Element->Data, outSize, 0))
                                Err = ERR_OUT_OF_MEMORY;
                            else
                            {
                               strm.bzalloc = NULL;
                               strm.bzfree = NULL;
                               strm.opaque = NULL;
                               if (BZ2_bzDecompressInit (&strm, 0, 1) != BZ_OK)
                                   Err = ERR_INVALID_DATA;
                               else
                               {
                                    size_t Count = 0;
                                    strm.next_in = (char*)InBuf;
                                    strm.avail_in = ARRAYBEGIN(Element->SizeList,int32_t)[0];
                                    strm.next_out = ARRAYBEGIN(Element->Data,char);
                                    strm.avail_out = 0;

                                    do {
                                        Count = strm.next_out - ARRAYBEGIN(Element->Data,char);
                                        strm.avail_out = 1024;
                                        if (!ArrayResize(&Element->Data, Count + strm.avail_out, 0))
                                        {
                                            Res = BZ_MEM_ERROR;
                                            break;
                                        }
                                        strm.next_out = ARRAYBEGIN(Element->Data,char) + Count;
                                        Res = BZ2_bzDecompress(&strm);
                                        if (Res!=BZ_STREAM_END && Res!=BZ_OK)
                                            break;
                                    } while (Res!=BZ_STREAM_END && strm.avail_in && !strm.avail_out);
                                    ArrayResize(&Element->Data, strm.total_out_lo32, 0);
                                    ARRAYBEGIN(Element->SizeList,int32_t)[0] = strm.total_out_lo32;
                                    BZ2_bzDecompressEnd(&strm);
                                    if (Res != BZ_STREAM_END)
                                        Err = ERR_INVALID_DATA;
                                }
                            }
                        }
#endif
                    }
                }
                ArrayClear(&TmpBuf);
            }
            else
#endif
            {
                if (!ArrayResize(&Element->Data,(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0],0))
                {
                    Err = ERR_OUT_OF_MEMORY;
                    goto failed;
                }
                InBuf = ARRAYBEGIN(Element->Data,uint8_t);
                if (Header)
                {
                    memcpy(InBuf,ARRAYBEGIN(((ebml_binary*)Header)->Data,uint8_t),(size_t)Header->DataSize);
                    InBuf += (size_t)Header->DataSize;
                }
                Err = Stream_Read(Input,InBuf,(size_t)(ARRAYBEGIN(Element->SizeList,int32_t)[0] - (Header?Header->DataSize:0)),&Read);
                if (Err != ERR_NONE)
                    goto failed;
                if (Read + (Header?Header->DataSize:0) != (size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0])
                {
                    Err = ERR_READ;
                    goto failed;
                }
            }
            break;
        case LACING_EBML:
        case LACING_XIPH:
        case LACING_FIXED:
            Read = 0;
            BufSize = 0;
            for (NumFrame=0;NumFrame<ARRAYCOUNT(Element->SizeList,int32_t);++NumFrame)
                BufSize += ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame];
#if defined(CONFIG_ZLIB) || defined(CONFIG_LZO1X) || defined(CONFIG_BZLIB)
            if (Header && Header->Context==&MATROSKA_ContextContentCompAlgo)
            {
                // zlib handling, read the buffer in temp memory
                // get the ouput size, adjust the Element->SizeList value, write in Element->Data
                array TmpBuf;
                int32_t FrameSize;
                size_t OutSize = 0;

                ArrayInit(&TmpBuf);
                if (!ArrayResize(&TmpBuf,BufSize,0))
                {
                    Err = ERR_OUT_OF_MEMORY;
                    goto failed;
                }
                InBuf = ARRAYBEGIN(TmpBuf,uint8_t);
                Err = Stream_Read(Input,InBuf,BufSize,&Read);
                if (Err != ERR_NONE || Read!=BufSize)
                {
                    if (Err==ERR_NONE)
                        Err = ERR_READ;
                    ArrayClear(&TmpBuf);
                    goto failed;
                }
                for (NumFrame=0;Err==ERR_NONE && NumFrame<ARRAYCOUNT(Element->SizeList,int32_t);++NumFrame)
                {
#if defined(CONFIG_ZLIB)
                    if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_ZLIB)
                    {
                        z_stream stream;
                        int Res;
                        memset(&stream,0,sizeof(stream));
                        Res = inflateInit(&stream);
                        if (Res != Z_OK)
                            Err = ERR_INVALID_DATA;
                        else
                        {
                            size_t Count;
                            stream.next_in = InBuf;
                            stream.avail_in = FrameSize = ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame];
                            stream.next_out = ARRAYBEGIN(Element->Data,uint8_t) + OutSize;
                            do {
                                Count = stream.next_out - ARRAYBEGIN(Element->Data,uint8_t);
                                if (!ArrayResize(&Element->Data, Count + 1024, 0))
                                {
                                    Res = Z_MEM_ERROR;
                                    break;
                                }
                                stream.avail_out = ARRAYCOUNT(Element->Data,uint8_t) - Count;
                                stream.next_out = ARRAYBEGIN(Element->Data,uint8_t) + Count;
                                Res = inflate(&stream, Z_NO_FLUSH);
                                if (Res!=Z_STREAM_END && Res!=Z_OK)
                                    break;
                            } while (Res!=Z_STREAM_END && stream.avail_in && !stream.avail_out);
                            ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame] = stream.total_out;
                            OutSize += stream.total_out;
                            inflateEnd(&stream);
                            if (Res != Z_STREAM_END)
                                Err = ERR_INVALID_DATA;
                        }
                    }
#endif
#if defined(CONFIG_LZO1X)
                    if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_LZO1X)
                    {
                        if (lzo_init() != LZO_E_OK)
                            Err = ERR_INVALID_DATA;
                        else
                        {
                            lzo_uint outSize = max(2048, ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame] << 2);
                            FrameSize = ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame];
                            if (!ArrayResize(&Element->Data, OutSize + outSize, 0))
                                Err = ERR_OUT_OF_MEMORY;
                            else
                            {
                                if (lzo1x_decompress_safe(InBuf, ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame], ARRAYBEGIN(Element->Data,uint8_t) + OutSize, &outSize, NULL) != LZO_E_OK)
                                    Err = ERR_INVALID_DATA;
                                else
                                {
                                    OutSize += outSize;
                                    ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame] = outSize;
                                    ArrayResize(&Element->Data,OutSize,0);
                                }
                            }
                        }
                    }
#endif
#if defined(CONFIG_BZLIB)
                    if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_BZLIB)
                    {
                        bz_stream stream;
                        int Res;
                        memset(&stream,0,sizeof(stream));
                        Res = BZ2_bzDecompressInit (&stream, 0, 1);
                        if (Res != BZ_OK)
                            Err = ERR_INVALID_DATA;
                        else
                        {
                            size_t Count;
                            stream.next_in = (char*)InBuf;
                            stream.avail_in = FrameSize = ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame];
                            stream.next_out = ARRAYBEGIN(Element->Data,char) + OutSize;
                            do {
                                Count = stream.next_out - ARRAYBEGIN(Element->Data,char);
                                if (!ArrayResize(&Element->Data, Count + 1024, 0))
                                {
                                    Res = BZ_MEM_ERROR;
                                    break;
                                }
                                stream.avail_out = ARRAYCOUNT(Element->Data,uint8_t) - Count;
                                stream.next_out = ARRAYBEGIN(Element->Data,char) + Count;
                                Res = BZ2_bzDecompress(&stream);
                                if (Res!=BZ_STREAM_END && Res!=BZ_OK)
                                    break;
                            } while (Res!=BZ_STREAM_END && stream.avail_in && !stream.avail_out);
                            ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame] = stream.total_out_lo32;
                            OutSize += stream.total_out_lo32;
                            BZ2_bzDecompressEnd(&stream);
                            if (Res != BZ_STREAM_END)
                                Err = ERR_INVALID_DATA;
                        }
                    }
#endif
                    InBuf += FrameSize;
                }
                ArrayResize(&Element->Data, OutSize, 0); // shrink the buffer
                ArrayClear(&TmpBuf);
            }
            else
#endif
            {
                if (!ArrayResize(&Element->Data,BufSize,0))
                {
                    Err = ERR_OUT_OF_MEMORY;
                    goto failed;
                }
                if (!Header)
                {
                    //assert(BufSize + Element->FirstFrameLocation == Element->Base.Base.DataSize);
                    Err = Stream_Read(Input,ARRAYBEGIN(Element->Data,uint8_t),BufSize,&BufSize);
                }
                else
                {
                    InBuf = ARRAYBEGIN(Element->Data,uint8_t);
                    for (NumFrame=0;NumFrame<ARRAYCOUNT(Element->SizeList,int32_t);++NumFrame)
                    {
                        memcpy(InBuf,ARRAYBEGIN(((ebml_binary*)Header)->Data,uint8_t),(size_t)Header->DataSize);
                        InBuf += (size_t)Header->DataSize;
                        Read = ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame] - (int32_t)Header->DataSize;
                        BufSize = Read;
                        assert(InBuf + Read <= ARRAYEND(Element->Data,uint8_t));
                        Err = Stream_Read(Input,InBuf,BufSize,&Read);
                        if (Err != ERR_NONE || Read!=BufSize)
                            goto failed;
                        InBuf += Read;
                    }
                }
            }
            if (Err != ERR_NONE)
                goto failed;
            break;
        default:
            assert(0); // we should support the other lacing modes
            Err = ERR_NOT_SUPPORTED;
            goto failed;
        }
        Element->Base.Base.bValueIsSet = 1;
    }

#if defined(CONFIG_EBML_WRITING)
	if (Element->ReadTrack != Element->WriteTrack || Element->ReadSegInfo != Element->WriteSegInfo)
		// TODO: only if the track compression/timecode scale is different
		Element->Base.Base.bNeedDataSizeUpdate = 1;
#endif

failed:
    return Err;
}

static err_t SetBlockParent(matroska_block *Block, void* Parent, void* Before)
{
	// update the timecode
	timecode_t AbsTimeCode;
	err_t Result = ERR_NONE;
	if (Block->LocalTimecodeUsed && Parent && NodeTree_Parent(Block))
	{
		assert(Node_IsPartOf(Parent,MATROSKA_CLUSTER_CLASS));
		AbsTimeCode = MATROSKA_BlockTimecode(Block);
        assert(AbsTimeCode != INVALID_TIMECODE_T);
		Result = MATROSKA_BlockSetTimecode(Block,AbsTimeCode,MATROSKA_ClusterTimecode((matroska_cluster*)Parent));
	}
	if (Result==ERR_NONE)
		Result = INHERITED(Block,nodetree_vmt,MATROSKA_BLOCK_CLASS)->SetParent(Block, Parent, Before);
	return Result;
}

static err_t SetBlockGroupParent(ebml_master *Element, void* Parent, void* Before)
{
	// update the timecode
	err_t Result = ERR_NONE;
	matroska_block *Block = (matroska_block*)EBML_MasterFindChild(Element, &MATROSKA_ContextBlock);
	timecode_t AbsTimeCode;
	if (Block && Block->LocalTimecodeUsed && Parent && NodeTree_Parent(Block) && NodeTree_Parent(NodeTree_Parent(Block)))
	{
		assert(Node_IsPartOf(Parent,MATROSKA_CLUSTER_CLASS));
		AbsTimeCode = MATROSKA_BlockTimecode(Block);
        assert(AbsTimeCode != INVALID_TIMECODE_T);
		Result = MATROSKA_BlockSetTimecode(Block,AbsTimeCode,MATROSKA_ClusterTimecode((matroska_cluster*)Parent));
	}
	if (Result==ERR_NONE)
		Result = INHERITED(Element,nodetree_vmt,MATROSKA_BLOCKGROUP_CLASS)->SetParent(Element, Parent, Before);
	return Result;
}

static err_t ReadBigBinaryData(ebml_binary *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    if (Scope == SCOPE_PARTIAL_DATA)
    {
        EBML_ElementSkipData((ebml_element*)Element,Input,ParserContext,NULL,AllowDummyElt);
        return ERR_NONE;
    }
    return INHERITED(Element,ebml_element_vmt,MATROSKA_BIGBINARY_CLASS)->ReadData(Element, Input, ParserContext, AllowDummyElt, Scope, DepthCheckCRC);
}

static err_t ReadBlockData(matroska_block *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    err_t Result;
	uint8_t _TempHead[5];
	uint8_t *cursor = _TempHead;
	uint8_t *_tmpBuf;
	uint8_t BlockHeadSize = 4; // default when the TrackNumber is < 16

    assert(!Element->Base.Base.bValueIsSet);
    Element->Base.Base.bValueIsSet = 0;

    if (Scope == SCOPE_NO_DATA)
        return ERR_NONE;

    if (Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element),SEEK_SET)==INVALID_FILEPOS_T)
    {
        Result = ERR_READ;
        goto failed;
    }

	Result = Stream_Read(Input,_TempHead, 5, NULL);
    if (Result != ERR_NONE)
        goto failed;
	// update internal values
	Element->TrackNumber = *cursor++;
	if (Element->TrackNumber & 0x80)
		Element->TrackNumber &= 0x7F;
    else
    {
		// there is extra data
		if ((Element->TrackNumber & 0x40) == 0)
        {
			// We don't support track numbers that large !
            Result = ERR_INVALID_DATA;
            goto failed;
		}
		Element->TrackNumber = (Element->TrackNumber & 0x3F) << 8;
		Element->TrackNumber += *cursor++;
		BlockHeadSize++;
	}

	Element->LocalTimecode = LOAD16BE(cursor);
	Element->LocalTimecodeUsed = 1;
	cursor += 2;

	if (EBML_ElementIsType((ebml_element*)Element, &MATROSKA_ContextSimpleBlock))
    {
		Element->IsKeyframe = (*cursor & 0x80) != 0;
		Element->IsDiscardable = (*cursor & 0x01) != 0;
	}
	Element->Invisible = (*cursor & 0x08) >> 3;
	Element->Lacing = (*cursor++ & 0x06) >> 1;

    Element->FirstFrameLocation = EBML_ElementPositionData((ebml_element*)Element) + BlockHeadSize;

    if (cursor == &_TempHead[4])
		_TempHead[0] = _TempHead[4];
	else
		Result += Stream_Read(Input,_TempHead, 1, NULL);

	// put all Frames in the list
	if (Element->Lacing == LACING_NONE)
    {
		ArrayResize(&Element->SizeList,sizeof(int32_t),0);
        ARRAYBEGIN(Element->SizeList,int32_t)[0] = (size_t)Element->Base.Base.DataSize - BlockHeadSize;
    }
    else
    {
		// read the number of frames in the lace
		uint32_t LastBufferSize = (size_t)Element->Base.Base.DataSize - BlockHeadSize - 1; // 1 for number of frame
		uint8_t FrameNum = _TempHead[0]; // number of frames in the lace - 1
		// read the list of frame sizes
		uint8_t Index;
		int32_t FrameSize;
		size_t SizeRead;
		filepos_t SizeUnknown;

        Element->FirstFrameLocation++; // for the number of frame
		ArrayResize(&Element->SizeList,sizeof(int32_t)*(FrameNum + 1),0);

		switch (Element->Lacing)
		{
		case LACING_XIPH:
			for (Index=0; Index<FrameNum; Index++)
            {
				// get the size of the frame
				FrameSize = 0;
				do {
					Result += Stream_Read(Input,_TempHead, 1, NULL);
					FrameSize += _TempHead[0];
					LastBufferSize--;

					Element->FirstFrameLocation++;
				} while (_TempHead[0] == 0xFF);

				ARRAYBEGIN(Element->SizeList,int32_t)[Index] = FrameSize;
				LastBufferSize -= FrameSize;
			}
			ARRAYBEGIN(Element->SizeList,int32_t)[Index] = LastBufferSize;
			break;
		case LACING_EBML:
			SizeRead = LastBufferSize;
            _tmpBuf = malloc(FrameNum*4);
			cursor = _tmpBuf; /// \warning assume the mean size will be coded in less than 4 bytes
			Result += Stream_Read(Input,cursor, FrameNum*4,NULL);
			FrameSize = (int32_t)EBML_ReadCodedSizeValue(cursor, &SizeRead, &SizeUnknown);
			ARRAYBEGIN(Element->SizeList,int32_t)[0] = FrameSize;
			cursor += SizeRead;
			LastBufferSize -= FrameSize + SizeRead;

			for (Index=1; Index<FrameNum; Index++)
            {
				// get the size of the frame
				SizeRead = LastBufferSize;
				FrameSize += (int32_t)EBML_ReadCodedSizeSignedValue(cursor, &SizeRead, &SizeUnknown);
				ARRAYBEGIN(Element->SizeList,int32_t)[Index] = FrameSize;
				cursor += SizeRead;
				LastBufferSize -= FrameSize + SizeRead;
			}

			Element->FirstFrameLocation += cursor - _tmpBuf;

			ARRAYBEGIN(Element->SizeList,int32_t)[Index] = LastBufferSize;
			free(_tmpBuf);
			break;
		case LACING_FIXED:
			for (Index=0; Index<=FrameNum; Index++)
				// get the size of the frame
				ARRAYBEGIN(Element->SizeList,int32_t)[Index] = LastBufferSize / (FrameNum + 1);
			break;
		default: // other lacing not supported
			assert(0);
		}
	}

    if (Scope == SCOPE_PARTIAL_DATA)
	{
		if (Stream_Seek(Input,Element->Lacing==LACING_NONE ? (EBML_ElementPositionData((ebml_element*)Element) + BlockHeadSize) : Element->FirstFrameLocation,SEEK_SET)==INVALID_FILEPOS_T)
			Result = ERR_READ;
		else
			Result = ERR_NONE;
	}
    else
        Result = MATROSKA_BlockReadData(Element, Input);

failed:
    return Result;
}

err_t MATROSKA_BlockGetFrame(const matroska_block *Block, size_t FrameNum, matroska_frame *Frame, bool_t WithData)
{
    size_t i;

    assert(!WithData || Block->Base.Base.bValueIsSet);
    if (WithData && !ARRAYCOUNT(Block->Data,uint8_t))
        return ERR_READ;
    if (FrameNum >= ARRAYCOUNT(Block->SizeList,uint32_t))
        return ERR_INVALID_PARAM;

	Frame->Data = WithData ? ARRAYBEGIN(Block->Data,uint8_t) : NULL;
    Frame->Timecode = MATROSKA_BlockTimecode((matroska_block*)Block);
    for (i=0;i<FrameNum;++i)
    {
        if (WithData) Frame->Data += ARRAYBEGIN(Block->SizeList,uint32_t)[i];
        if (Frame->Timecode != INVALID_TIMECODE_T)
        {
            if (i < ARRAYCOUNT(Block->Durations,timecode_t) && ARRAYBEGIN(Block->Durations,timecode_t)[i] != INVALID_TIMECODE_T)
                Frame->Timecode += ARRAYBEGIN(Block->Durations,timecode_t)[i];
            else
                Frame->Timecode = INVALID_TIMECODE_T;
        }
    }

    Frame->Size = ARRAYBEGIN(Block->SizeList,uint32_t)[i];
    if (FrameNum < ARRAYCOUNT(Block->Durations,timecode_t))
        Frame->Duration = ARRAYBEGIN(Block->Durations,timecode_t)[i];
    else
        Frame->Duration = INVALID_TIMECODE_T;
    return ERR_NONE;
}

err_t MATROSKA_BlockAppendFrame(matroska_block *Block, const matroska_frame *Frame, timecode_t ClusterTimecode)
{
    if (!Block->Base.Base.bValueIsSet && Frame->Timecode!=INVALID_TIMECODE_T)
        MATROSKA_BlockSetTimecode(Block,Frame->Timecode,ClusterTimecode);
    ArrayAppend(&Block->Data,Frame->Data,Frame->Size,0);
    ArrayAppend(&Block->Durations,&Frame->Duration,sizeof(Frame->Duration),0);
    ArrayAppend(&Block->SizeList,&Frame->Size,sizeof(Frame->Size),0);
    Block->Base.Base.bValueIsSet = 1;
    Block->Base.Base.bNeedDataSizeUpdate = 1;
    Block->Lacing = LACING_AUTO;
    return ERR_NONE;
}

#if defined(CONFIG_ZLIB)
err_t UnCompressFrameZLib(const uint8_t *Cursor, size_t CursorSize, array *OutBuf, size_t *FrameSize, size_t *ArrayOffset)
{
    z_stream stream;
    int Res;
    err_t Err = ERR_NONE;

    memset(&stream,0,sizeof(stream));
    Res = inflateInit(&stream);
    if (Res != Z_OK)
        Err = ERR_INVALID_DATA;
    else
    {
        size_t Count;
        stream.next_in = Cursor;
        stream.avail_in = CursorSize;
        stream.next_out = ARRAYBEGIN(*OutBuf,uint8_t) + *ArrayOffset;
        do {
            Count = stream.next_out - ARRAYBEGIN(*OutBuf,uint8_t);
            if (!ArrayResize(OutBuf, Count + 1024, 0))
            {
                Res = Z_MEM_ERROR;
                break;
            }
            stream.avail_out = ARRAYCOUNT(*OutBuf,uint8_t) - Count;
            stream.next_out = ARRAYBEGIN(*OutBuf,uint8_t) + Count;
            Res = inflate(&stream, Z_NO_FLUSH);
            if (Res!=Z_STREAM_END && Res!=Z_OK)
                break;
        } while (Res!=Z_STREAM_END && stream.avail_in && !stream.avail_out);
        *FrameSize = stream.total_out;
        *ArrayOffset = *ArrayOffset + stream.total_out;
        inflateEnd(&stream);
        if (Res != Z_STREAM_END)
            Err = ERR_INVALID_DATA;
    }
    return Err;
}

#if defined(CONFIG_EBML_WRITING)
err_t CompressFrameZLib(const uint8_t *Cursor, size_t CursorSize, uint8_t **OutBuf, size_t *OutSize)
{
    err_t Err = ERR_NONE;
    z_stream stream;
    size_t Count;
    array TmpBuf;
    int Res;

    memset(&stream,0,sizeof(stream));
    if (deflateInit(&stream, 9)!=Z_OK)
        return ERR_INVALID_DATA;
    stream.next_in = (Bytef*)Cursor;
    stream.avail_in = CursorSize;
    Count = 0;
    ArrayInit(&TmpBuf);
    stream.next_out = ARRAYBEGIN(TmpBuf,uint8_t);
    do {
        Count = stream.next_out - ARRAYBEGIN(TmpBuf,uint8_t);
        if (!ArrayResize(&TmpBuf,CursorSize + Count,0))
        {
            ArrayClear(&TmpBuf);
            Err = ERR_OUT_OF_MEMORY;
            break;
        }
        stream.avail_out = ARRAYCOUNT(TmpBuf,uint8_t) - Count;
        stream.next_out = ARRAYBEGIN(TmpBuf,uint8_t) + Count;
        Res = deflate(&stream, Z_FINISH);
    } while (stream.avail_out==0 && Res!=Z_STREAM_END);

    if (OutBuf && OutSize)
        // TODO: write directly in the output buffer
        memcpy(*OutBuf, ARRAYBEGIN(TmpBuf,uint8_t), min(*OutSize, stream.total_out));
    ArrayClear(&TmpBuf);

    if (OutSize)
        *OutSize = stream.total_out;

    deflateEnd(&stream);

    return Err;
}
#endif // CONFIG_EBML_WRITING
#endif // CONFIG_ZLIB

static filepos_t GetBlockFrameSize(const matroska_block *Element, size_t Frame, const ebml_element *Header, int CompScope)
{
    if (Frame >= ARRAYCOUNT(Element->SizeList,int32_t))
        return 0;

    if (!Header || (CompScope & MATROSKA_COMPR_SCOPE_BLOCK)==0)
        return ARRAYBEGIN(Element->SizeList,int32_t)[Frame];
    if (Header->Context==&MATROSKA_ContextContentCompAlgo)
    {
        // handle zlib
        size_t OutSize;
        const int32_t *Size = ARRAYBEGIN(Element->SizeList,int32_t);
        const uint8_t *Data = ARRAYBEGIN(Element->Data,uint8_t);
        while (Frame)
        {
            Data += *Size;
            ++Size;
            --Frame;
        }
        OutSize = *Size;
        assert(Element->Base.Base.bValueIsSet);
#if defined(CONFIG_EBML_WRITING) && defined(CONFIG_ZLIB)
        if (!Element->Base.Base.bValueIsSet || CompressFrameZLib(Data,*Size,NULL,&OutSize)!=ERR_NONE)
#else
        if (!Element->Base.Base.bValueIsSet)
#endif
            return *Size; // we can't tell the final size without decoding the data
        return OutSize;
    }
    return ARRAYBEGIN(Element->SizeList,int32_t)[Frame] - Header->DataSize; // header stripping
}

#if defined(CONFIG_EBML_WRITING)
static char GetBestLacingType(const matroska_block *Element)
{
	int XiphLacingSize, EbmlLacingSize;
    size_t i;
    int32_t DataSize;
    ebml_element *Elt, *Elt2, *Header = NULL;
    int CompressionScope = MATROSKA_COMPR_SCOPE_BLOCK;

	if (ARRAYCOUNT(Element->SizeList,int32_t) <= 1)
		return LACING_NONE;

    DataSize = ARRAYBEGIN(Element->SizeList,int32_t)[0];
    for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t);++i)
    {
        if (ARRAYBEGIN(Element->SizeList,int32_t)[i]!=DataSize)
            break;
    }
    if (i==ARRAYCOUNT(Element->SizeList,int32_t))
        return LACING_FIXED;

    // find out if compressed headers are used
    assert(Element->WriteTrack!=NULL);
    Elt = EBML_MasterFindChild(Element->WriteTrack, &MATROSKA_ContextContentEncodings);
    if (Elt)
    {
        Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncoding);
        if (EBML_MasterChildren(Elt))
        {
            if (EBML_MasterNext(Elt))
                return 0; // TODO support cascaded compression/encryption

            Elt2 = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncodingScope);
            if (Elt2)
                CompressionScope = EBML_IntegerValue((ebml_integer*)Elt2);

            Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompression);
            if (!Elt)
                return 0; // TODO: support encryption

            Header = EBML_MasterGetChild((ebml_master*)Elt, &MATROSKA_ContextContentCompAlgo);
#if defined(CONFIG_ZLIB)
            if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
            if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                return 0;

            if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_HEADER)
                Header = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompSettings);
        }
    }

    XiphLacingSize = 0;
    for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
    {
        DataSize = (int32_t)GetBlockFrameSize(Element, i, Header, CompressionScope);
        while (DataSize >= 0xFF)
        {
            XiphLacingSize++;
            DataSize -= 0xFF;
        }
        XiphLacingSize++;
    }

    EbmlLacingSize = EBML_CodedSizeLength(GetBlockFrameSize(Element, 0, Header, CompressionScope),0,1);
    for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
    {
        DataSize = (int32_t)GetBlockFrameSize(Element, i, Header, CompressionScope) - DataSize;
        EbmlLacingSize += EBML_CodedSizeLengthSigned(DataSize,0);
    }

    if (XiphLacingSize < EbmlLacingSize)
		return LACING_XIPH;
	else
		return LACING_EBML;
}

static err_t RenderBlockData(matroska_block *Element, stream *Output, bool_t bForceWithoutMandatory, bool_t bWithDefault, filepos_t *Rendered)
{
    err_t Err = ERR_NONE;
    uint8_t BlockHead[5], *Cursor;
    size_t ToWrite, Written, BlockHeadSize = 4;
    ebml_element *Elt, *Elt2, *Header = NULL;
    int32_t *i;
    int CompressionScope = MATROSKA_COMPR_SCOPE_BLOCK;
    assert(Element->Lacing != LACING_AUTO);

    if (Element->TrackNumber < 0x80)
    {
        BlockHead[0] = 0x80 | (Element->TrackNumber & 0xFF);
        Cursor = &BlockHead[1];
    }
    else if (Element->TrackNumber < 0x4000)
    {
        BlockHead[0] = 0x40 | (Element->TrackNumber >> 8);
        BlockHead[1] = Element->TrackNumber & 0xFF;
        Cursor = &BlockHead[2];
        BlockHeadSize = 5;
    }
    else 
        return ERR_INVALID_DATA;

    STORE16BE(Cursor,Element->LocalTimecode);
    Cursor += 2;

    *Cursor = 0;
    if (Element->Invisible)
        *Cursor |= 0x08;
    *Cursor |= Element->Lacing << 1;
    if (EBML_ElementIsType((ebml_element*)Element, &MATROSKA_ContextSimpleBlock))
    {
        if (Element->IsKeyframe)
            *Cursor |= 0x80;
        if (Element->IsDiscardable)
            *Cursor |= 0x01;
    }

    Err = Stream_Write(Output,BlockHead,BlockHeadSize,&Written);
    if (Err != ERR_NONE)
        goto failed;
    if (Written != BlockHeadSize)
    {
        Err = ERR_WRITE;
        goto failed;
    }
    if (Rendered)
        *Rendered = Written;

    assert(Element->WriteTrack!=NULL);
    Elt = EBML_MasterFindChild(Element->WriteTrack, &MATROSKA_ContextContentEncodings);
    if (Elt)
    {
        Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncoding);
        if (EBML_MasterChildren(Elt))
        {
            if (EBML_MasterNext(Elt))
                return ERR_INVALID_DATA; // TODO support cascaded compression/encryption

            Elt2 = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncodingScope);
            if (Elt2)
                CompressionScope = EBML_IntegerValue((ebml_integer*)Elt2);

            Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompression);
            if (!Elt)
                return ERR_INVALID_DATA; // TODO: support encryption

            Header = EBML_MasterGetChild((ebml_master*)Elt, &MATROSKA_ContextContentCompAlgo);
#if defined(CONFIG_ZLIB)
            if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
            if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
			{
				Err = ERR_NOT_SUPPORTED;
				goto failed;
			}

            if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_HEADER)
                Header = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompSettings);
        }
    }

#if !defined(CONFIG_ZLIB)
    if (Header && Header->Context==&MATROSKA_ContextContentCompAlgo)
    {
        Err = ERR_NOT_SUPPORTED;
        goto failed;
    }
#endif

    if (Element->Lacing == LACING_AUTO)
        Element->Lacing = GetBestLacingType(Element);
    if (Element->Lacing != LACING_NONE)
    {
        uint8_t *LaceHead = malloc(1 + ARRAYCOUNT(Element->SizeList,int32_t)*4);
        size_t i,LaceSize = 1;
        int32_t DataSize, PrevSize;
        if (!LaceHead)
        {
            Err = ERR_OUT_OF_MEMORY;
            goto failed;
        }
        LaceHead[0] = (ARRAYCOUNT(Element->SizeList,int32_t)-1) & 0xFF; // number of elements in the lace
        if (Element->Lacing == LACING_EBML)
        {
            DataSize = (int32_t)GetBlockFrameSize(Element, 0, Header, CompressionScope);
            LaceSize += EBML_CodedValueLength(DataSize,EBML_CodedSizeLength(DataSize,0,1),LaceHead+LaceSize, 1);
            for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                PrevSize = DataSize;
                DataSize = (int32_t)GetBlockFrameSize(Element, i, Header, CompressionScope);
                LaceSize += EBML_CodedValueLengthSigned(DataSize-PrevSize,EBML_CodedSizeLengthSigned(DataSize-PrevSize,0),LaceHead+LaceSize);
            }
        }
        else if (Element->Lacing == LACING_XIPH)
        {
            for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                DataSize = (int32_t)GetBlockFrameSize(Element, i, Header, CompressionScope);
                while (DataSize >= 0xFF)
                {
                    LaceHead[LaceSize++] = 0xFF;
                    DataSize -= 0xFF;
                }
                LaceHead[LaceSize++] = (uint8_t)DataSize;
            }
        }
        else if (Element->Lacing == LACING_FIXED)
        {
            // nothing to write
        }
        assert(LaceSize <= (1 + ARRAYCOUNT(Element->SizeList,int32_t)*4));
        Err = Stream_Write(Output,LaceHead,LaceSize,&Written);
        if (Err != ERR_NONE)
            goto failed;
        if (Rendered)
            *Rendered += Written;
        free(LaceHead);
    }
    Node_SET(Element,MATROSKA_BLOCK_READ_TRACK,&Element->WriteTrack); // now use the write track for consecutive read of the same element

    Cursor = ARRAYBEGIN(Element->Data,uint8_t);
    if (Header && (CompressionScope & MATROSKA_COMPR_SCOPE_BLOCK))
    {
        if (Header && Header->Context==&MATROSKA_ContextContentCompAlgo)
        {
#if defined(CONFIG_ZLIB)
            uint8_t *OutBuf;
            array TmpBuf;
            ArrayInit(&TmpBuf);
            for (i=ARRAYBEGIN(Element->SizeList,int32_t);i!=ARRAYEND(Element->SizeList,int32_t);++i)
            {
                if (!ArrayResize(&TmpBuf,*i + 100,0))
                {
                    ArrayClear(&TmpBuf);
                    Err = ERR_OUT_OF_MEMORY;
                    break;
                }
                OutBuf = ARRAYBEGIN(TmpBuf,uint8_t);
                ToWrite = ARRAYCOUNT(TmpBuf,uint8_t);
                if (CompressFrameZLib(Cursor, *i, &OutBuf, &ToWrite) != ERR_NONE)
                {
                    ArrayClear(&TmpBuf);
                    Err = ERR_OUT_OF_MEMORY;
                    break;
                }

                Err = Stream_Write(Output,OutBuf,ToWrite,&Written);
                ArrayClear(&TmpBuf);
                if (Rendered)
                    *Rendered += Written;
                Cursor += *i;
                if (Err!=ERR_NONE)
                    break;
            }
#endif
        }
        else
        {
            // header compression
            for (i=ARRAYBEGIN(Element->SizeList,int32_t);i!=ARRAYEND(Element->SizeList,int32_t);++i)
            {
                assert(memcmp(Cursor,ARRAYBEGIN(((ebml_binary*)Header)->Data,uint8_t),(size_t)Header->DataSize)==0);
                if (memcmp(Cursor,ARRAYBEGIN(((ebml_binary*)Header)->Data,uint8_t),(size_t)Header->DataSize)!=0)
                {
                    Err = ERR_INVALID_DATA;
                    goto failed;
                }
                Cursor += Header->DataSize;
                ToWrite = *i - (size_t)Header->DataSize;
                Err = Stream_Write(Output,Cursor,ToWrite,&Written);
                if (Rendered)
                    *Rendered += Written;
                Cursor += Written;
            }
        }
    }
    else
    {
        ToWrite = ARRAYCOUNT(Element->Data,uint8_t);
        Err = Stream_Write(Output,Cursor,ToWrite,&Written);
        if (Rendered)
            *Rendered += Written;
    }

failed:
    return Err;
}
#endif

static matroska_block *CopyBlockInfo(const matroska_block *Element, const void *Cookie)
{
    matroska_block *Result = (matroska_block*)INHERITED(Element,ebml_element_vmt,Node_ClassId(Element))->Copy(Element,Cookie);
    if (Result)
    {
        Result->TrackNumber = Element->TrackNumber;
        Result->IsKeyframe = Element->IsKeyframe;
        Result->IsDiscardable = Element->IsDiscardable;
        Result->Invisible = Element->Invisible;
#if 0 // computed once blocks are added
        Result->LocalTimecode = Element->LocalTimecode;
        Result->LocalTimecodeUsed = Element->LocalTimecodeUsed;
	    Result->GlobalTimecode = Element->GlobalTimecode;
        Result->Lacing = Element->Lacing;
        Result->FirstFrameLocation = Element->FirstFrameLocation;
        array SizeList = Element->; // int32_t
        array Data = Element->; // uint8_t
        array Durations = Element->; // timecode_t
#else
        Result->Base.Base.bValueIsSet = 0;
#endif
        Node_SET(Result,MATROSKA_BLOCK_READ_TRACK,&Element->ReadTrack);
        Node_SET(Result,MATROSKA_BLOCK_READ_SEGMENTINFO,&Element->ReadSegInfo);
#if defined(CONFIG_EBML_WRITING)
        Node_SET(Result,MATROSKA_BLOCK_WRITE_TRACK,&Element->WriteTrack);
        Node_SET(Result,MATROSKA_BLOCK_WRITE_SEGMENTINFO,&Element->WriteSegInfo);
#endif
    }
    return Result;
}

static filepos_t UpdateBlockSize(matroska_block *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory)
{
    int CompressionScope = MATROSKA_COMPR_SCOPE_BLOCK;
    if (EBML_ElementNeedsDataSizeUpdate(Element, bWithDefault))
    {
        ebml_element *Header = NULL;
#if defined(CONFIG_EBML_WRITING)
        ebml_element *Elt, *Elt2;
        if (Element->Lacing == LACING_AUTO)
            Element->Lacing = GetBestLacingType(Element);

        assert(Element->WriteTrack!=NULL);
        Elt = EBML_MasterFindChild(Element->WriteTrack, &MATROSKA_ContextContentEncodings);
        if (Elt)
        {
            Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncoding);
            if (EBML_MasterChildren(Elt))
            {
                if (EBML_MasterNext(Elt))
                    return ERR_INVALID_DATA; // TODO support cascaded compression/encryption

                Elt2 = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentEncodingScope);
                if (Elt2)
                    CompressionScope = EBML_IntegerValue((ebml_integer*)Elt2);

                Elt = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompression);
                if (!Elt)
                    return ERR_INVALID_DATA; // TODO: support encryption

                Header = EBML_MasterGetChild((ebml_master*)Elt, &MATROSKA_ContextContentCompAlgo);
#if defined(CONFIG_ZLIB)
                if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
                if (EBML_IntegerValue((ebml_integer*)Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                    return ERR_INVALID_DATA;

                if (EBML_IntegerValue((ebml_integer*)Header)==MATROSKA_BLOCK_COMPR_HEADER)
                    Header = EBML_MasterFindChild((ebml_master*)Elt, &MATROSKA_ContextContentCompSettings);
            }
        }
#else
        assert(Element->Lacing!=LACING_AUTO);
#endif

        if (Element->Lacing == LACING_NONE)
        {
            assert(ARRAYCOUNT(Element->SizeList,int32_t) == 1);
            Element->Base.Base.DataSize = GetBlockHeadSize(Element) + GetBlockFrameSize(Element,0,Header, CompressionScope);
        }
        else if (Element->Lacing == LACING_EBML)
        {
            size_t i;
            filepos_t PrevSize, Size;
            filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
            Size = GetBlockFrameSize(Element,0,Header, CompressionScope);
            Result += EBML_CodedSizeLength(Size,0,1) + Size;
            for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                PrevSize = Size;
                Size = GetBlockFrameSize(Element,i,Header, CompressionScope);
                Result += Size + EBML_CodedSizeLengthSigned(Size - PrevSize,0);
            }
            Result += GetBlockFrameSize(Element,i,Header, CompressionScope);
            Element->Base.Base.DataSize = Result;
        }
        else if (Element->Lacing == LACING_XIPH)
        {
            size_t i;
            filepos_t Size;
            filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
            for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                Size = GetBlockFrameSize(Element,i,Header, CompressionScope);
                Result += (Size / 0xFF + 1) + Size;
            }
            Result += GetBlockFrameSize(Element,i,Header, CompressionScope);
            Element->Base.Base.DataSize = Result;
        }
        else if (Element->Lacing == LACING_FIXED)
        {
            size_t i;
            filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
            for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t);++i)
                Result += GetBlockFrameSize(Element,i,Header, CompressionScope);
            Element->Base.Base.DataSize = Result;
        }
#ifdef TODO
        char LacingHere;
	    // compute the final size of the data
	    switch (ARRAYCOUNT(Element->SizeList,int32_t))
        {
		    case 0:
			    Element->Base.Base.DataSize = 0;
			    break;
		    case 1:
			    Element->Base.Base.DataSize = 4 + *ARRAYBEGIN(Element->SizeList,int32_t);
			    break;
		    default:
			    Element->Base.Base.DataSize = 4 + 1; // 1 for the lacing head
			    if (Element->Lacing == LACING_AUTO)
				    LacingHere = GetBestLacingType(Element);
			    else
				    LacingHere = Element->Lacing;
			    switch (LacingHere)
			    {
			    case LACING_XIPH:
				    for (i=0; i<myBuffers.size()-1; i++) {
					    SetSize_(GetSize() + myBuffers[i]->DataSize() + (myBuffers[i]->DataSize() / 0xFF + 1));
				    }
				    break;
			    case LACING_EBML:
				    SetSize_(GetSize() + myBuffers[0]->DataSize() + CodedSizeLength(myBuffers[0]->DataSize(), 0, IsFiniteSize()));
				    for (i=1; i<myBuffers.size()-1; i++) {
					    SetSize_(GetSize() + myBuffers[i]->DataSize() + CodedSizeLengthSigned(int64(myBuffers[i]->DataSize()) - int64(myBuffers[i-1]->DataSize()), 0));
				    }
				    break;
			    case LACING_FIXED:
				    for (i=0; i<myBuffers.size()-1; i++) {
					    SetSize_(GetSize() + myBuffers[i]->DataSize());
				    }
				    break;
			    default:
				    assert(0);
			    }
			    // DataSize of the last frame (not in lace)
			    SetSize_(GetSize() + myBuffers[i]->DataSize());
			    break;
	    }

	    if (Element->Base.Base.DataSize && Element->TrackNumber >= 0x80)
		    ++Element->Base.Base.DataSize; // the size will be coded with one more octet
#endif
    }

    // skip the EBML_BINARY_CLASS version as we have another internal buffer
	return INHERITED(Element,ebml_element_vmt,EBML_BINARY_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory);
}

static int CmpCuePoint(const matroska_cuepoint* a,const matroska_cuepoint* b)
{
	// returns a > b
    timecode_t TA = MATROSKA_CueTimecode(a);
    timecode_t TB = MATROSKA_CueTimecode(b);
    int NA,NB;
    if (TB < TA)
        return 1;
    if (TB > TA)
        return -1;
    NA = MATROSKA_CueTrackNum(a);
    NB = MATROSKA_CueTrackNum(b);
    if (NB < NA)
        return 1;
    if (NB > NA)
        return -1;
    return 0;
}

static int CmpAttachedFile(const ebml_master* a,const ebml_master* b)
{
	// returns a > b
	// sort cover art names according to http://www.matroska.org/technical/cover_art/index.html
	tchar_t FilenameA[MAXPATH];
	tchar_t FilenameB[MAXPATH];
	bool_t CoverA=0,CoverB=0;
	bool_t LandCoverA=0,LandCoverB=0;
	bool_t SmallCoverA=0,SmallCoverB=0;
	ebml_element *NameA = EBML_MasterFindChild(a,&MATROSKA_ContextFileName);
	ebml_element *NameB = EBML_MasterFindChild(b,&MATROSKA_ContextFileName);

	if (NameB==NULL)
		return -1;
	if (NameA==NULL)
		return 1;
	
	EBML_StringGet((ebml_string*)NameA,FilenameA,TSIZEOF(FilenameA));
	EBML_StringGet((ebml_string*)NameB,FilenameB,TSIZEOF(FilenameB));

	if (tcsisame_ascii(FilenameA, T("cover.jpg")) || tcsisame_ascii(FilenameA, T("cover.png")))
		CoverA = 1;
	else if (tcsisame_ascii(FilenameA, T("cover_land.jpg")) || tcsisame_ascii(FilenameA, T("cover_land.png")))
		LandCoverA = 1;
	else if (tcsisame_ascii(FilenameA, T("small_cover.jpg")) || tcsisame_ascii(FilenameA, T("small_cover.png")))
	{
		CoverA = 1;
		SmallCoverA = 1;
	}
	else if (tcsisame_ascii(FilenameA, T("small_cover_land.jpg")) || tcsisame_ascii(FilenameA, T("small_cover_land.png")))
	{
		LandCoverA = 1;
		SmallCoverA = 1;
	}

	if (tcsisame_ascii(FilenameB, T("cover.jpg")) || tcsisame_ascii(FilenameB, T("cover.png")))
		CoverB = 1;
	else if (tcsisame_ascii(FilenameB, T("cover_land.jpg")) || tcsisame_ascii(FilenameB, T("cover_land.png")))
		LandCoverB = 1;
	else if (tcsisame_ascii(FilenameB, T("small_cover.jpg")) || tcsisame_ascii(FilenameB, T("small_cover.png")))
	{
		CoverB = 1;
		SmallCoverB = 1;
	}
	else if (tcsisame_ascii(FilenameB, T("small_cover_land.jpg")) || tcsisame_ascii(FilenameB, T("small_cover_land.png")))
	{
		LandCoverB = 1;
		SmallCoverB = 1;
	}

	if (!CoverA && !CoverB && !LandCoverA && !LandCoverB)
		return tcscmp(FilenameA,FilenameB);

	// cover.jpg comes first
	if (CoverA && !SmallCoverA)
		return -1;
	if (CoverB && !SmallCoverB)
		return 1;
	if (CoverA == CoverB || LandCoverA == LandCoverB)
		return SmallCoverA - SmallCoverB;
	if (CoverA || LandCoverA)
	{
		if (CoverB)
			return 1;
		return -1;
	}

	if (CoverA)
		return -1;
	return 1;
}

matroska_cuepoint *MATROSKA_CuesGetTimecodeStart(const ebml_element *Cues, timecode_t Timecode)
{
	matroska_cuepoint *Elt,*Prev=NULL;

	assert(Cues!=NULL);
	assert(EBML_ElementIsType(Cues, &MATROSKA_ContextCues));
	if (Timecode==INVALID_TIMECODE_T)
		return NULL;

	for (Elt=(matroska_cuepoint*)EBML_MasterChildren(Cues);Elt;Prev=Elt, Elt=(matroska_cuepoint*)EBML_MasterNext(Elt))
	{
		if (MATROSKA_CueTimecode(Elt) > Timecode)
			break;
	}

	return Prev ? Prev : (matroska_cuepoint*)EBML_MasterChildren(Cues);
}

static bool_t ValidateSizeSegUID(const ebml_binary *p)
{
    uint8_t test[16];
	if (p->Base.DataSize != 16 || !EBML_ElementIsFiniteSize((const ebml_element *)p))
        return 0;
    if (!p->Base.bValueIsSet)
        return 1;
    memset(test,0,sizeof(test));
    return memcmp(ARRAYBEGIN(p->Data,uint8_t),test,16)!=0; // make sure the value is not 0
}

static err_t CreateBlock(matroska_block *p)
{
	p->GlobalTimecode = INVALID_TIMECODE_T;
	return ERR_NONE;
}

static err_t CreateCluster(matroska_cluster *p)
{
	p->GlobalTimecode = INVALID_TIMECODE_T;
    return ERR_NONE;
}

static err_t ReadTrackEntry(matroska_trackentry *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope, size_t DepthCheckCRC)
{
    err_t Result = INHERITED(Element,ebml_element_vmt,MATROSKA_TRACKENTRY_CLASS)->ReadData(Element, Input, ParserContext, AllowDummyElt, Scope, DepthCheckCRC);
    if (Result==ERR_NONE)
    {
        ebml_element *Encodings = EBML_MasterFindChild(Element,&MATROSKA_ContextContentEncodings);
        if (Encodings)
        {
            ebml_element *Elt2 = EBML_MasterFindChild((ebml_master*)Encodings,&MATROSKA_ContextContentEncoding);
            if (Elt2)
            {
                ebml_element *Elt =  EBML_MasterFindChild((ebml_master*)Elt2,&MATROSKA_ContextContentCompression);
                if (Elt)
                {
                    ebml_integer *Scope =  (ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt2,&MATROSKA_ContextContentEncodingScope);
                    Element->CodecPrivateCompressed = Scope && (EBML_IntegerValue(Scope) & MATROSKA_COMPR_SCOPE_PRIVATE)!=0;
                }
            }
        }
    }
    return Result;
}

static filepos_t UpdateDataSizeTrackEntry(matroska_trackentry *Element, bool_t bWithDefault, bool_t bForceWithoutMandatory)
{
#if defined(CONFIG_ZLIB)
    bool_t CodecPrivateCompressed = 0;
    ebml_integer *Scope = NULL;
    ebml_element *Encodings = EBML_MasterFindChild(Element,&MATROSKA_ContextContentEncodings);
    if (Encodings)
    {
        ebml_element *Elt2 = EBML_MasterFindChild((ebml_master*)Encodings,&MATROSKA_ContextContentEncoding);
        if (Elt2)
        {
            ebml_element *Elt =  EBML_MasterFindChild((ebml_master*)Elt2,&MATROSKA_ContextContentCompression);
            if (Elt)
            {
                Scope = (ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt2,&MATROSKA_ContextContentEncodingScope);
                CodecPrivateCompressed = Scope && (EBML_IntegerValue(Scope) & MATROSKA_COMPR_SCOPE_PRIVATE)!=0;
            }
        }
    }

    if (CodecPrivateCompressed != Element->CodecPrivateCompressed)
    {
        ebml_binary *CodecPrivate = (ebml_binary*)EBML_MasterFindChild(Element,&MATROSKA_ContextCodecPrivate);
        if (!Element->CodecPrivateCompressed)
        {
            // compress the codec private
            if (CodecPrivate)
            {
                size_t CompressedSize = ARRAYCOUNT(CodecPrivate->Data,uint8_t);
                uint8_t *Compressed = malloc(CompressedSize);
                if (CompressFrameZLib(ARRAYBEGIN(CodecPrivate->Data,uint8_t), (size_t)CodecPrivate->Base.DataSize, &Compressed, &CompressedSize)==ERR_NONE)
                {
                    if (EBML_BinarySetData(CodecPrivate, Compressed, CompressedSize)==ERR_NONE)
                        Element->CodecPrivateCompressed = 1;
                }
                free(Compressed);
            }
            if (!Element->CodecPrivateCompressed)
                EBML_IntegerSetValue(Scope, EBML_IntegerValue(Scope) ^ MATROSKA_COMPR_SCOPE_PRIVATE);
        }
        else
        {
            if (CodecPrivate)
            {
                size_t CompressedSize = ARRAYCOUNT(CodecPrivate->Data,uint8_t);
                size_t Offset = 0;
                array Compressed;

                ArrayInit(&Compressed);
                if (UnCompressFrameZLib(ARRAYBEGIN(CodecPrivate->Data,uint8_t), (size_t)CodecPrivate->Base.DataSize, &Compressed, &CompressedSize, &Offset)==ERR_NONE)
                {
                    if (EBML_BinarySetData(CodecPrivate, ARRAYBEGIN(Compressed,uint8_t), CompressedSize)==ERR_NONE)
                        Element->CodecPrivateCompressed = 0;
                }
                ArrayClear(&Compressed);
            }
            else
                Element->CodecPrivateCompressed = 0;
            if (Element->CodecPrivateCompressed)
            {
                // TODO: add in the Compression header that the header is still compressed
            }
        }
    }
#endif
    return INHERITED(Element,ebml_element_vmt,MATROSKA_TRACKENTRY_CLASS)->UpdateDataSize(Element, bWithDefault, bForceWithoutMandatory);
}

static matroska_trackentry *CopyTrackEntry(const matroska_trackentry *Element, const void *Cookie)
{
    matroska_trackentry *Result = (matroska_trackentry*)INHERITED(Element,ebml_element_vmt,MATROSKA_TRACKENTRY_CLASS)->Copy(Element, Cookie);
    if (Result)
        Result->CodecPrivateCompressed = Element->CodecPrivateCompressed;
    return Result;
}


int MATROSKA_TrackGetBlockCompression(const matroska_trackentry *TrackEntry)
{
    ebml_element *Encodings, *Elt, *Elt2;
    assert(Node_IsPartOf(TrackEntry, MATROSKA_TRACKENTRY_CLASS));
    Encodings = EBML_MasterFindChild(TrackEntry,&MATROSKA_ContextContentEncodings);
    if (!Encodings)
        return MATROSKA_BLOCK_COMPR_NONE;
    Elt2 = EBML_MasterFindChild((ebml_master*)Encodings,&MATROSKA_ContextContentEncoding);
    if (!Elt2)
        return MATROSKA_BLOCK_COMPR_NONE;
    Elt =  EBML_MasterGetChild((ebml_master*)Elt2,&MATROSKA_ContextContentEncodingScope);
    if (!(EBML_IntegerValue((ebml_integer*)Elt) & MATROSKA_COMPR_SCOPE_BLOCK))
        return MATROSKA_BLOCK_COMPR_NONE;
    Elt =  EBML_MasterFindChild((ebml_master*)Elt2,&MATROSKA_ContextContentCompression);
    if (!Elt)
        return MATROSKA_BLOCK_COMPR_NONE;
    Elt2 = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextContentCompAlgo);
    return (int)EBML_IntegerValue((ebml_integer*)Elt2);
}

bool_t MATROSKA_TrackSetCompressionZlib(matroska_trackentry *TrackEntry, int Scope)
{
    // force zlib compression
    bool_t HadEncoding;
    ebml_element *Encodings, *Elt, *Elt2;
    assert(Node_IsPartOf(TrackEntry, MATROSKA_TRACKENTRY_CLASS));
    // remove the previous compression and the new optimized one
    Encodings = EBML_MasterFindChild(TrackEntry,&MATROSKA_ContextContentEncodings);
    HadEncoding = Encodings!=NULL;
    if (Encodings!=NULL)
        NodeDelete((node*)Encodings);

    if (Scope!=0)
    {
        Encodings = EBML_MasterGetChild((ebml_master*)TrackEntry,&MATROSKA_ContextContentEncodings);
        Elt2 = EBML_MasterGetChild((ebml_master*)Encodings,&MATROSKA_ContextContentEncoding);

        Elt =  EBML_MasterGetChild((ebml_master*)Elt2,&MATROSKA_ContextContentEncodingScope);
        EBML_IntegerSetValue((ebml_integer*)Elt, Scope);

        Elt =  EBML_MasterGetChild((ebml_master*)Elt2,&MATROSKA_ContextContentCompression);
        Elt2 = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextContentCompAlgo);
        EBML_IntegerSetValue((ebml_integer*)Elt2, MATROSKA_BLOCK_COMPR_ZLIB);
    }
    return HadEncoding;
}

bool_t MATROSKA_TrackSetCompressionHeader(matroska_trackentry *TrackEntry, const uint8_t *Header, size_t HeaderSize)
{
    bool_t HadEncoding;
    ebml_element *Encodings, *Elt, *Elt2;
    assert(Node_IsPartOf(TrackEntry, MATROSKA_TRACKENTRY_CLASS));
    // remove the previous compression and the new optimized one
    Encodings = EBML_MasterFindChild(TrackEntry,&MATROSKA_ContextContentEncodings);
    HadEncoding = Encodings!=NULL;
    if (Encodings!=NULL)
        NodeDelete((node*)Encodings);

    if (Header && HeaderSize)
    {
        Encodings = EBML_MasterGetChild((ebml_master*)TrackEntry,&MATROSKA_ContextContentEncodings);
        Elt2 = EBML_MasterGetChild((ebml_master*)Encodings,&MATROSKA_ContextContentEncoding);

        Elt =  EBML_MasterGetChild((ebml_master*)Elt2,&MATROSKA_ContextContentCompression);
        Elt2 = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextContentCompAlgo);
        EBML_IntegerSetValue((ebml_integer*)Elt2, MATROSKA_BLOCK_COMPR_HEADER);
        Elt2 = EBML_MasterGetChild((ebml_master*)Elt,&MATROSKA_ContextContentCompSettings);
        EBML_BinarySetData((ebml_binary*)Elt2, Header, HeaderSize);
    }
    return HadEncoding;
}

bool_t MATROSKA_TrackSetCompressionNone(matroska_trackentry *TrackEntry)
{
    ebml_element *Encodings = EBML_MasterFindChild(TrackEntry,&MATROSKA_ContextContentEncodings);
    assert(Node_IsPartOf(TrackEntry, MATROSKA_TRACKENTRY_CLASS));
    if (!Encodings)
        return 0;
    NodeDelete((node*)Encodings);
    return 1;
}


META_START(Matroska_Class,MATROSKA_BLOCK_CLASS)
META_CLASS(SIZE,sizeof(matroska_block))
META_CLASS(CREATE,CreateBlock)
META_VMT(TYPE_FUNC,nodetree_vmt,SetParent,SetBlockParent)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadBlockData)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateBlockSize)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderBlockData)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,CopyBlockInfo)
META_DATA(TYPE_ARRAY,0,matroska_block,SizeList)
META_DATA(TYPE_ARRAY,0,matroska_block,SizeListIn)
META_DATA(TYPE_ARRAY,0,matroska_block,Data)
META_DATA(TYPE_ARRAY,0,matroska_block,Durations)
META_PARAM(TYPE,MATROSKA_BLOCK_READ_TRACK,TYPE_NODE)
META_DATA_UPDATE_CMP(TYPE_NODE_REF,MATROSKA_BLOCK_READ_TRACK,matroska_block,ReadTrack,BlockTrackChanged)
META_PARAM(TYPE,MATROSKA_BLOCK_READ_SEGMENTINFO,TYPE_NODE)
META_DATA_UPDATE_CMP(TYPE_NODE_REF,MATROSKA_BLOCK_READ_SEGMENTINFO,matroska_block,ReadSegInfo,BlockTrackChanged)
#if defined(CONFIG_EBML_WRITING)
META_PARAM(TYPE,MATROSKA_BLOCK_WRITE_TRACK,TYPE_NODE)
META_DATA_UPDATE_CMP(TYPE_NODE_REF,MATROSKA_BLOCK_WRITE_TRACK,matroska_block,WriteTrack,BlockTrackChanged)
META_PARAM(TYPE,MATROSKA_BLOCK_WRITE_SEGMENTINFO,TYPE_NODE)
META_DATA_UPDATE_CMP(TYPE_NODE_REF,MATROSKA_BLOCK_WRITE_SEGMENTINFO,matroska_block,WriteSegInfo,BlockTrackChanged)
#endif
META_END_CONTINUE(EBML_BINARY_CLASS)

META_START_CONTINUE(MATROSKA_BLOCKGROUP_CLASS)
META_VMT(TYPE_FUNC,nodetree_vmt,SetParent,SetBlockGroupParent)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_BIGBINARY_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadBigBinaryData)
META_END_CONTINUE(EBML_BINARY_CLASS)

META_START_CONTINUE(MATROSKA_CUEPOINT_CLASS)
META_CLASS(SIZE,sizeof(matroska_cuepoint))
META_VMT(TYPE_FUNC,ebml_element_vmt,Cmp,CmpCuePoint)
META_PARAM(TYPE,MATROSKA_CUE_SEGMENTINFO,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_CUE_SEGMENTINFO,matroska_cuepoint,SegInfo)
META_PARAM(TYPE,MATROSKA_CUE_BLOCK,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_CUE_BLOCK,matroska_cuepoint,Block)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_CLUSTER_CLASS)
META_CLASS(SIZE,sizeof(matroska_cluster))
META_CLASS(CREATE,CreateCluster)
META_PARAM(TYPE,MATROSKA_CLUSTER_READ_SEGMENTINFO,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_CLUSTER_READ_SEGMENTINFO,matroska_cluster,ReadSegInfo)
META_PARAM(TYPE,MATROSKA_CLUSTER_WRITE_SEGMENTINFO,TYPE_NODE)
META_DATA_UPDATE_CMP(TYPE_NODE_REF,MATROSKA_CLUSTER_WRITE_SEGMENTINFO,matroska_cluster,WriteSegInfo,ClusterTimeChanged)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_SEEKPOINT_CLASS)
META_CLASS(SIZE,sizeof(matroska_seekpoint))
META_PARAM(TYPE,MATROSKA_SEEKPOINT_ELEMENT,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_SEEKPOINT_ELEMENT,matroska_seekpoint,Link)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_TRACKENTRY_CLASS)
META_CLASS(SIZE,sizeof(matroska_trackentry))
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadTrackEntry)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateDataSize,UpdateDataSizeTrackEntry)
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,CopyTrackEntry)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_SEGMENTUID_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeSegUID)
META_END_CONTINUE(EBML_BINARY_CLASS)

META_START_CONTINUE(MATROSKA_ATTACHMENT_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,Cmp,CmpAttachedFile)
META_END(EBML_MASTER_CLASS)
