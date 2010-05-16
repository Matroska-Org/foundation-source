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

#if defined(MATROSKA2_EXPORTS)
#define MATROSKA_DLL DLLEXPORT
#elif defined(MATROSKA2_IMPORTS)
#define MATROSKA_DLL DLLIMPORT
#else
#define MATROSKA_DLL
#endif

#define CONTEXT_LIBMATROSKA_VERSION  0x401

// if a profile is set in ebml_semantic.DisabledProfile that means the element is not available for that profile
#define PROFILE_MATROSKA_V1          1
#define PROFILE_MATROSKA_V2          2
#define PROFILE_TEST_V1              4
#define PROFILE_TEST_V2              8


EBML_DLL err_t MATROSKA_Init(nodecontext *p);
EBML_DLL err_t MATROSKA_Done(nodecontext *p);

#define INVALID_TIMECODE_T      MAX_INT64
typedef int64_t    timecode_t; // in nanoseconds

#define TRACK_TYPE_VIDEO   1
#define TRACK_TYPE_AUDIO   2
#define TRACK_TYPE_COMPLEX 3

typedef struct matroska_block matroska_block;
typedef struct matroska_cuepoint matroska_cuepoint;
typedef struct matroska_cluster matroska_cluster;
typedef struct matroska_seekpoint matroska_seekpoint;

EBML_DLL err_t MATROSKA_LinkMetaSeekElement(matroska_seekpoint *MetaSeek, ebml_element *Link);
EBML_DLL err_t MATROSKA_MetaSeekUpdate(matroska_seekpoint *MetaSeek);
EBML_DLL err_t MATROSKA_LinkClusterSegmentInfo(matroska_cluster *Cluster, ebml_element *SegmentInfo);
EBML_DLL err_t MATROSKA_LinkBlockTrack(matroska_block *Block, ebml_element *Tracks);
EBML_DLL err_t MATROSKA_LinkBlockSegmentInfo(matroska_block *Block, ebml_element *SegmentInfo);
//EBML_DLL err_t MATROSKA_LinkCueTrack(const ebml_element *Cue, ebml_element *Tracks);
EBML_DLL err_t MATROSKA_LinkCueSegmentInfo(matroska_cuepoint *Cue, ebml_element *SegmentInfo);
EBML_DLL err_t MATROSKA_LinkCuePointBlock(matroska_cuepoint *Cue, matroska_block *Block);
EBML_DLL err_t MATROSKA_CuePointUpdate(matroska_cuepoint *Cue, ebml_element *Segment);
EBML_DLL double MATROSKA_TrackTimecodeScale(const ebml_element *Track);
EBML_DLL timecode_t MATROSKA_SegmentInfoTimecodeScale(const ebml_element *SegmentInfo);
EBML_DLL timecode_t MATROSKA_ClusterTimecode(const matroska_cluster *Cluster);
EBML_DLL timecode_t MATROSKA_BlockTimecode(const matroska_block *Block);
EBML_DLL timecode_t MATROSKA_CueTimecode(const matroska_cuepoint *Cue);
EBML_DLL int16_t MATROSKA_BlockTrackNum(const matroska_block *Block);
EBML_DLL bool_t MATROSKA_BlockKeyframe(const matroska_block *Block);
EBML_DLL bool_t MATROSKA_BlockLaced(const matroska_block *Block);
EBML_DLL err_t MATROSKA_BlockReadData(matroska_block *Block, stream *Input);
EBML_DLL err_t MATROSKA_BlockReleaseData(matroska_block *Block);
EBML_DLL int16_t MATROSKA_CueTrackNum(const matroska_cuepoint *Cue);
EBML_DLL void MATROSKA_CuesSort(ebml_element *Cues);
EBML_DLL fourcc_t MATROSKA_MetaSeekID(const matroska_seekpoint *MetaSeek);
EBML_DLL filepos_t MATROSKA_MetaSeekPosInSegment(const matroska_seekpoint *MetaSeek);
EBML_DLL filepos_t MATROSKA_MetaSeekAbsolutePos(const matroska_seekpoint *MetaSeek);

EBML_DLL matroska_block *MATROSKA_GetBlockForTimecode(matroska_cluster *Cluster, timecode_t Timecode, int16_t Track);
EBML_DLL void MATROSKA_LinkClusterBlocks(matroska_cluster *Cluster, ebml_element *RSegmentInfo, ebml_element *Tracks);

#define MATROSKA_VERSION  2

// EBML contexts
extern const ebml_context MATROSKA_ContextStream;

extern const ebml_context MATROSKA_ContextSegment;

extern const ebml_context MATROSKA_ContextSeekHead;
extern const ebml_context MATROSKA_ContextSeek;
extern const ebml_context MATROSKA_ContextSeekId;
extern const ebml_context MATROSKA_ContextSeekPosition;

extern const ebml_context MATROSKA_ContextSegmentInfo;
extern const ebml_context MATROSKA_ContextWritingApp;
extern const ebml_context MATROSKA_ContextMuxingApp;

extern const ebml_context MATROSKA_ContextCluster;
extern const ebml_context MATROSKA_ContextTracks;
extern const ebml_context MATROSKA_ContextCues;
extern const ebml_context MATROSKA_ContextAttachments;
extern const ebml_context MATROSKA_ContextChapters;
extern const ebml_context MATROSKA_ContextTags;

extern const ebml_context MATROSKA_ContextTrackEntry;
extern const ebml_context MATROSKA_ContextTrackType;
extern const ebml_context MATROSKA_ContextTrackNumber;
extern const ebml_context MATROSKA_ContextTrackCodecID;
extern const ebml_context MATROSKA_ContextTrackLacing;

extern const ebml_context MATROSKA_ContextCuePoint;

extern const ebml_context MATROSKA_ContextTag;
extern const ebml_context MATROSKA_ContextSimpleTag;
extern const ebml_context MATROSKA_ContextTagTargets;

extern const ebml_context MATROSKA_ContextChapterAtom;

extern const ebml_context MATROSKA_ContextClusterBlockGroup;
extern const ebml_context MATROSKA_ContextClusterBlock;
extern const ebml_context MATROSKA_ContextClusterReferenceBlock;
extern const ebml_context MATROSKA_ContextClusterSimpleBlock;
