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
#ifndef MATROSKA_MATROSKA_H
#define MATROSKA_MATROSKA_H

#include "ebml2/ebml.h"
#include "matroska2/matroska_config.h"

#if defined(matroska2_EXPORTS)
#define MATROSKA_DLL DLLEXPORT
#elif defined(matroska2_IMPORTS)
#define MATROSKA_DLL DLLIMPORT
#else
#define MATROSKA_DLL extern
#endif

#define CONTEXT_LIBMATROSKA_VERSION  0x00402

// if a profile is set in ebml_semantic.DisabledProfile that means the element is not available for that profile
#define PROFILE_MATROSKA_V1          1
#define PROFILE_MATROSKA_V2          2
#define PROFILE_MATROSKA_V3          4
#define PROFILE_MATROSKA_V4          32
#define PROFILE_WEBM                 8
#define PROFILE_DIVX                16
#define PROFILE_MATROSKA_ALL        (PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4) 
#define PROFILE_MATROSKA_ANY        (PROFILE_MATROSKA_ALL|PROFILE_WEBM|PROFILE_DIVX) 

#define MATROSKA_VERSION  2

#define MATROSKA_BLOCK_COMPR_NONE   -1
#define MATROSKA_BLOCK_COMPR_ZLIB    0
#define MATROSKA_BLOCK_COMPR_BZLIB   1
#define MATROSKA_BLOCK_COMPR_LZO1X   2
#define MATROSKA_BLOCK_COMPR_HEADER  3

#define MATROSKA_COMPR_SCOPE_BLOCK   1
#define MATROSKA_COMPR_SCOPE_PRIVATE 2
#define MATROSKA_COMPR_SCOPE_NEXT    4

#define MATROSKA_DISPLAY_UNIT_PIXEL  0
#define MATROSKA_DISPLAY_UNIT_CM     1
#define MATROSKA_DISPLAY_UNIT_INCHES 2
#define MATROSKA_DISPLAY_UNIT_DAR    3


MATROSKA_DLL err_t MATROSKA_Init(parsercontext *p);
MATROSKA_DLL void MATROSKA_Done(parsercontext *p);

#define INVALID_TIMESTAMP_T      MAX_INT64
typedef int64_t    mkv_timestamp_t; // in nanoseconds

#define TRACK_TYPE_VIDEO    1
#define TRACK_TYPE_AUDIO    2
#define TRACK_TYPE_COMPLEX  3
#define TRACK_TYPE_SUBTITLE 17
#define TRACK_TYPE_LOGO     16
#define TRACK_TYPE_BUTTONS  18
#define TRACK_TYPE_CONTROL  32

#define TRACK_STEREO_MODE_MONO                 0
#define TRACK_STEREO_MODE_SIDEBYSIDE_L         1
#define TRACK_STEREO_MODE_TOPBOTTOM_R          2
#define TRACK_STEREO_MODE_TOPBOTTOM_L          3
#define TRACK_STEREO_MODE_CHECKBOARD_R         4
#define TRACK_STEREO_MODE_CHECKBOARD_L         5
#define TRACK_STEREO_MODE_INTERLEAVED_R        6
#define TRACK_STEREO_MODE_INTERLEAVED_L        7
#define TRACK_STEREO_MODE_COL_INTERLEAVED_L    8
#define TRACK_STEREO_MODE_COL_INTERLEAVED_R    9
#define TRACK_STEREO_MODE_ANAGLYPH_CYAN_RED   10
#define TRACK_STEREO_MODE_SIDEBYSIDE_R        11
#define TRACK_STEREO_MODE_ANAGLYPH_GREEN_MAG  12
#define TRACK_STEREO_MODE_ALTERNATE_PACKED_L  13
#define TRACK_STEREO_MODE_ALTERNATE_PACKED_R  14

#define TRACK_PLANE_LEFT        0
#define TRACK_PLANE_RIGHT       1
#define TRACK_PLANE_BACKGROUND  2

#define TRACK_OLD_STEREOMODE_MONO   0
#define TRACK_OLD_STEREOMODE_RIGHT  1
#define TRACK_OLD_STEREOMODE_LEFT   2
#define TRACK_OLD_STEREOMODE_BOTH   3

typedef struct matroska_block matroska_block;
typedef struct matroska_cuepoint matroska_cuepoint;
typedef struct matroska_cluster matroska_cluster;
typedef struct matroska_seekpoint matroska_seekpoint;
typedef struct matroska_trackentry matroska_trackentry;

typedef struct matroska_frame
{
    uint8_t *Data;
    uint32_t Size;
    mkv_timestamp_t Timestamp;
    mkv_timestamp_t Duration;

} matroska_frame;

MATROSKA_DLL err_t MATROSKA_LinkMetaSeekElement(matroska_seekpoint *MetaSeek, ebml_element *Link);
MATROSKA_DLL err_t MATROSKA_MetaSeekUpdate(matroska_seekpoint *MetaSeek);
MATROSKA_DLL err_t MATROSKA_LinkClusterReadSegmentInfo(matroska_cluster *Cluster, ebml_master *SegmentInfo, bool_t UseForWriteToo);
MATROSKA_DLL err_t MATROSKA_LinkBlockWithReadTracks(matroska_block *Block, ebml_master *Tracks, bool_t UseForWriteToo, int ForProfile);
MATROSKA_DLL err_t MATROSKA_LinkBlockReadTrack(matroska_block *Block, ebml_master *Track, bool_t UseForWriteToo, int ForProfile);
MATROSKA_DLL err_t MATROSKA_LinkBlockReadSegmentInfo(matroska_block *Block, ebml_master *SegmentInfo, bool_t UseForWriteToo);
#if defined(CONFIG_EBML_WRITING)
MATROSKA_DLL err_t MATROSKA_LinkClusterWriteSegmentInfo(matroska_cluster *Cluster, ebml_master *SegmentInfo);
MATROSKA_DLL err_t MATROSKA_LinkBlockWithWriteTracks(matroska_block *Block, ebml_master *Tracks, int ForProfile);
MATROSKA_DLL err_t MATROSKA_LinkBlockWriteTrack(matroska_block *Block, ebml_master *Track, int ForProfile);
MATROSKA_DLL err_t MATROSKA_LinkBlockWriteSegmentInfo(matroska_block *Block, ebml_master *SegmentInfo);
#endif
//MATROSKA_DLL err_t MATROSKA_LinkCueTrack(const ebml_element *Cue, ebml_element *Tracks);
MATROSKA_DLL err_t MATROSKA_LinkCueSegmentInfo(matroska_cuepoint *Cue, ebml_master *SegmentInfo);
MATROSKA_DLL err_t MATROSKA_LinkCuePointBlock(matroska_cuepoint *Cue, matroska_block *Block);
MATROSKA_DLL err_t MATROSKA_CuePointUpdate(matroska_cuepoint *Cue, ebml_element *Segment, int ForProfile);
MATROSKA_DLL double MATROSKA_TrackTimestampScale(const ebml_master *Track);
MATROSKA_DLL mkv_timestamp_t MATROSKA_SegmentInfoTimestampScale(const ebml_master *SegmentInfo);
MATROSKA_DLL void MATROSKA_ClusterSetTimestamp(matroska_cluster *Cluster, mkv_timestamp_t Timestamp);
MATROSKA_DLL err_t MATROSKA_BlockSetTimestamp(matroska_block *Block, mkv_timestamp_t Timestamp, mkv_timestamp_t ClusterTimestamp);
MATROSKA_DLL mkv_timestamp_t MATROSKA_ClusterTimestamp(matroska_cluster *Cluster);
MATROSKA_DLL mkv_timestamp_t MATROSKA_ClusterTimestampScale(matroska_cluster *Cluster, bool_t Read);
MATROSKA_DLL mkv_timestamp_t MATROSKA_BlockTimestamp(matroska_block *Block);
MATROSKA_DLL mkv_timestamp_t MATROSKA_CueTimestamp(const matroska_cuepoint *Cue);
MATROSKA_DLL filepos_t MATROSKA_CuePosInSegment(const matroska_cuepoint *Cue);
MATROSKA_DLL int16_t MATROSKA_BlockTrackNum(const matroska_block *Block);
MATROSKA_DLL bool_t MATROSKA_BlockKeyframe(const matroska_block *Block);
MATROSKA_DLL bool_t MATROSKA_BlockDiscardable(const matroska_block *Block);
MATROSKA_DLL bool_t MATROSKA_BlockLaced(const matroska_block *Block);
MATROSKA_DLL err_t MATROSKA_BlockReadData(matroska_block *Block, stream *Input, int ForProfile);
MATROSKA_DLL err_t MATROSKA_BlockReleaseData(matroska_block *Block, bool_t IncludingNotRead);
MATROSKA_DLL int16_t MATROSKA_CueTrackNum(const matroska_cuepoint *Cue);
MATROSKA_DLL void MATROSKA_CuesSort(ebml_master *Cues);
MATROSKA_DLL void MATROSKA_AttachmentSort(ebml_master *Attachments);
MATROSKA_DLL fourcc_t MATROSKA_MetaSeekID(const matroska_seekpoint *MetaSeek);
MATROSKA_DLL bool_t MATROSKA_MetaSeekIsClass(const matroska_seekpoint *MetaSeek, const ebml_context *Class);
MATROSKA_DLL filepos_t MATROSKA_MetaSeekPosInSegment(const matroska_seekpoint *MetaSeek);
MATROSKA_DLL filepos_t MATROSKA_MetaSeekAbsolutePos(const matroska_seekpoint *MetaSeek);

MATROSKA_DLL matroska_cuepoint *MATROSKA_CuesGetTimestampStart(const ebml_element *Cues, mkv_timestamp_t Timestamp);

#if defined(CONFIG_EBML_WRITING)
MATROSKA_DLL int MATROSKA_TrackGetBlockCompression(const matroska_trackentry *TrackEntry, int ForProfile);
MATROSKA_DLL bool_t MATROSKA_TrackSetCompressionZlib(matroska_trackentry *TrackEntry, int Scope, int ForProfile);
MATROSKA_DLL bool_t MATROSKA_TrackSetCompressionHeader(matroska_trackentry *TrackEntry, const uint8_t *Header, size_t HeaderSize, int ForProfile);
MATROSKA_DLL bool_t MATROSKA_TrackSetCompressionNone(matroska_trackentry *TrackEntry);
#if defined(CONFIG_ZLIB)
MATROSKA_DLL err_t CompressFrameZLib(const uint8_t *Cursor, size_t CursorSize, uint8_t **OutBuf, size_t *OutSize);
#else // !CONFIG_ZLIB
static INLINE err_t CompressFrameZLib(const uint8_t *Cursor, size_t CursorSize, uint8_t **OutBuf, size_t *OutSize)
{
    return ERR_NOT_SUPPORTED;
}
#endif // !CONFIG_ZLIB
#endif

MATROSKA_DLL void MATROSKA_ClusterSort(matroska_cluster *Cluster); // not good with P frames!!!

MATROSKA_DLL ebml_element *MATROSKA_BlockReadTrack(const matroska_block *Block);
MATROSKA_DLL ebml_element *MATROSKA_BlockReadSegmentInfo(const matroska_block *Block);
#if defined(CONFIG_EBML_WRITING)
MATROSKA_DLL ebml_element *MATROSKA_BlockWriteTrack(const matroska_block *Block);
MATROSKA_DLL ebml_element *MATROSKA_BlockWriteSegmentInfo(const matroska_block *Block);
#endif

MATROSKA_DLL err_t MATROSKA_BlockSkipToFrame(const matroska_block *Block, stream *Input, size_t FrameNum);
MATROSKA_DLL void MATROSKA_BlockSetKeyframe(matroska_block *Block, bool_t Set);
MATROSKA_DLL void MATROSKA_BlockSetDiscardable(matroska_block *Block, bool_t Set);
MATROSKA_DLL err_t MATROSKA_BlockProcessFrameDurations(matroska_block *Block, stream *Input, int ForProfile);
MATROSKA_DLL size_t MATROSKA_BlockGetFrameCount(const matroska_block *Block);
MATROSKA_DLL mkv_timestamp_t MATROSKA_BlockGetFrameDuration(const matroska_block *Block, size_t FrameNum);
MATROSKA_DLL mkv_timestamp_t MATROSKA_BlockGetFrameStart(const matroska_block *Block, size_t FrameNum);
MATROSKA_DLL mkv_timestamp_t MATROSKA_BlockGetFrameEnd(const matroska_block *Block, size_t FrameNum);
MATROSKA_DLL size_t MATROSKA_BlockGetLength(const matroska_block *Block, size_t FrameNum);

MATROSKA_DLL err_t MATROSKA_BlockGetFrame(const matroska_block *Block, size_t FrameNum, matroska_frame *Frame, bool_t WithData);
MATROSKA_DLL err_t MATROSKA_BlockAppendFrame(matroska_block *Block, const matroska_frame *Frame, mkv_timestamp_t ClusterTimestamp);
MATROSKA_DLL bool_t MATROSKA_BlockIsKeyframe(const matroska_block *Block);


MATROSKA_DLL matroska_block *MATROSKA_GetBlockForTimestamp(matroska_cluster *Cluster, mkv_timestamp_t Timestamp, int16_t Track);
MATROSKA_DLL void MATROSKA_LinkClusterBlocks(matroska_cluster *Cluster, ebml_master *RSegmentInfo, ebml_master *Tracks, bool_t KeepUnmatched, int ForProfile);

MATROSKA_DLL const ebml_context *MATROSKA_getContextStream();

#define MATROSKA_BLOCK_CLASS      FOURCC('M','K','B','L')
#define MATROSKA_BLOCKGROUP_CLASS FOURCC('M','K','B','G')
#define MATROSKA_CUEPOINT_CLASS   FOURCC('M','K','C','P')
#define MATROSKA_CLUSTER_CLASS    FOURCC('M','K','C','U')
#define MATROSKA_SEEKPOINT_CLASS  FOURCC('M','K','S','K')
#define MATROSKA_SEGMENTUID_CLASS FOURCC('M','K','I','D')
#define MATROSKA_BIGBINARY_CLASS  FOURCC('M','K','B','B')
#define MATROSKA_ATTACHMENT_CLASS FOURCC('M','K','A','T')
#define MATROSKA_TRACKENTRY_CLASS FOURCC('M','K','T','E')

#endif // MATROSKA_MATROSKA_H
