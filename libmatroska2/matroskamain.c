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
#include "matroska/matroska.h"
#include "matroska2_project.h"

#define MATROSKA_BLOCK_CLASS      FOURCC('M','K','B','L')
#define MATROSKA_CUEPOINT_CLASS   FOURCC('M','K','C','P')
#define MATROSKA_CLUSTER_CLASS    FOURCC('M','K','C','U')
#define MATROSKA_SEEKPOINT_CLASS  FOURCC('M','K','S','K')
#define MATROSKA_SEGMENTUID_CLASS FOURCC('M','K','I','D')

// Seek Header
const ebml_context MATROSKA_ContextSeekId = {0x53AB, EBML_BINARY_CLASS, 0, 0, "SeekID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSeekPosition = {0x53AC, EBML_INTEGER_CLASS, 0, 0, "SeekPosition", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSeekPoint[] = {
    {1, 1, &MATROSKA_ContextSeekId       ,0},
    {1, 1, &MATROSKA_ContextSeekPosition ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSeek = {0x4DBB, MATROSKA_SEEKPOINT_CLASS, 0, 0, "SeekPoint", EBML_SemanticSeekPoint, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSeekHead[] = {
    {1, 0, &MATROSKA_ContextSeek ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSeekHead = {0x114D9B74, EBML_MASTER_CLASS, 0, 0, "SeekHeader", EBML_SemanticSeekHead, EBML_SemanticGlobals, NULL};

// Segment Info
const ebml_context MATROSKA_ContextTranslateEditionUID = {0x69FC, EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTranslateCodec = {0x69BF, EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateCodec", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTranslateID = {0x69A5, EBML_BINARY_CLASS, 0, 0, "ChapterTranslateID", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticCodecTranslate[] = {
    {0, 0, &MATROSKA_ContextTranslateEditionUID ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextTranslateCodec      ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextTranslateID         ,PROFILE_TEST},
    {0, 0, NULL, 0} // end of the table
};
const ebml_context MATROSKA_ContextChapterTranslate = {0x6924, EBML_MASTER_CLASS, 0, 0, "ChapterTranslate", EBML_SemanticCodecTranslate, EBML_SemanticGlobals, NULL};

const ebml_context MATROSKA_ContextSegmentUid = {0x73A4, MATROSKA_SEGMENTUID_CLASS, 0, 0, "SegmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSegmentFilename = {0x7384, EBML_UNISTRING_CLASS, 0, 0, "SegmentFilename", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPrevUid = {0x3CB923, MATROSKA_SEGMENTUID_CLASS, 0, 0, "PrevUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPrevFilename = {0x3C83AB, EBML_UNISTRING_CLASS, 0, 0, "PrevFilename", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextNextUid = {0x3EB923, MATROSKA_SEGMENTUID_CLASS, 0, 0, "NextUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextNextFilename = {0x3E83BB, EBML_UNISTRING_CLASS, 0, 0, "NextFilename", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSegmentFamily = {0x4444, EBML_BINARY_CLASS, 0, 0, "SegmentFamily", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTimecodeScale = {0x2AD7B1, EBML_INTEGER_CLASS, 1, 1000000, "TimecodeScale", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDuration = {0x4489, EBML_FLOAT_CLASS, 0, 0, "Duration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSegmentDate = {0x4461, EBML_DATE_CLASS, 0, 0, "DateUTC", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSegmentTitle = {0x7BA9, EBML_UNISTRING_CLASS, 0, 0, "Title", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextMuxingApp = {0x4D80, EBML_UNISTRING_CLASS, 0, 0, "MuxingApp", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextWritingApp = {0x5741, EBML_UNISTRING_CLASS, 0, 0, "WritingApp", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSegmentInfo[] = {
    {0, 1, &MATROSKA_ContextSegmentUid       ,0},
    {0, 1, &MATROSKA_ContextSegmentFilename  ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextPrevUid          ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextPrevFilename     ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextNextUid          ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextNextFilename     ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextSegmentFamily    ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextChapterTranslate ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextTimecodeScale    ,0},
    {0, 1, &MATROSKA_ContextDuration         ,0},
    {0, 1, &MATROSKA_ContextSegmentDate      ,0},
    {0, 1, &MATROSKA_ContextSegmentTitle     ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextMuxingApp        ,0},
    {1, 1, &MATROSKA_ContextWritingApp       ,0},
    {0, 0, NULL, 0} // end of the table
};
const ebml_context MATROSKA_ContextSegmentInfo = {0x1549A966, EBML_MASTER_CLASS, 0, 0, "Info", EBML_SemanticSegmentInfo, EBML_SemanticGlobals, NULL};


// Chapters
const ebml_context MATROSKA_ContextChapterTrackNumber = {0x89, EBML_INTEGER_CLASS, 0, 0, "ChapterFlagEnabled", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterTrack[] = {
    {1, 0, &MATROSKA_ContextChapterTrackNumber ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterProcessTime = {0x6922, EBML_INTEGER_CLASS, 0, 0, "ChapterProcessTime", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcessData = {0x6933, EBML_BINARY_CLASS, 0, 0, "ChapterProcessData", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterProcessCommand[] = {
    {1, 1, &MATROSKA_ContextChapterProcessTime ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextChapterProcessData ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterProcessCodecID = {0x6955, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ChapterProcessCodecID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcessPrivate = {0x450D, EBML_BINARY_CLASS, 0, 0, "ChapterProcessPrivate", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcessCommand = {0x6911, EBML_MASTER_CLASS, 0, 0, "ChapterProcessCommand", EBML_SemanticChapterProcessCommand, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterProcess[] = {
    {1, 1, &MATROSKA_ContextChapterProcessCodecID ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextChapterProcessPrivate ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextChapterProcessCommand ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterString = {0x85, EBML_UNISTRING_CLASS, 0, 0, "ChapterString", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterLanguage = {0x437C, EBML_STRING_CLASS, 1, (intptr_t)"eng", "ChapterLanguage", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterCountry = {0x437E, EBML_STRING_CLASS, 0, 0, "ChapterCountry", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterDisplay[] = {
    {1, 1, &MATROSKA_ContextChapterString   ,PROFILE_TEST},
    {1, 0, &MATROSKA_ContextChapterLanguage ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextChapterCountry  ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterUID = {0x73C4, EBML_INTEGER_CLASS, 0, 0, "ChapterUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTimeStart = {0x91, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeStart", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTimeEnd = {0x92, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeEnd", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterHidden = {0x98, EBML_BOOLEAN_CLASS, 0, 0, "ChapterFlagHidden", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterEnabled = {0x4598, EBML_BOOLEAN_CLASS, 0, 0, "ChapterFlagEnabled", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterSegmentUID = {0x6E67, EBML_BINARY_CLASS, 0, 0, "ChapterSegmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterSegmentEditionUID = {0x6EBC, EBML_BINARY_CLASS, 0, 0, "ChapterSegmentEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterPhysical = {0x63C3, EBML_INTEGER_CLASS, 0, 0, "ChapterPhysicalEquiv", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTrack = {0x8F, EBML_MASTER_CLASS, 0, 0, "ChapterTrack", EBML_SemanticChapterTrack, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterDisplay = {0x80, EBML_MASTER_CLASS, 0, 0, "ChapterDisplay", EBML_SemanticChapterDisplay, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcess = {0x6944, EBML_MASTER_CLASS, 0, 0, "ChapterProcess", EBML_SemanticChapterProcess, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterAtom[] = {
    {0, 0, &MATROSKA_ContextChapterAtom              ,PROFILE_TEST}, // recursive
    {1, 1, &MATROSKA_ContextChapterUID               ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextChapterTimeStart         ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextChapterTimeEnd           ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextChapterHidden            ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextChapterEnabled           ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextChapterSegmentUID        ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextChapterSegmentEditionUID ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextChapterPhysical          ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextChapterTrack             ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextChapterDisplay           ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextChapterProcess           ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterAtom = {0xB6, EBML_MASTER_CLASS, 0, 0, "ChapterAtom", EBML_SemanticChapterAtom, EBML_SemanticGlobals, NULL};

const ebml_context MATROSKA_ContextEditionUID = {0x45BC, EBML_INTEGER_CLASS, 0, 0, "EditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionHidden = {0x45BD, EBML_BOOLEAN_CLASS, 0, 0, "EditionFlagHidden", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionDefault = {0x45DB, EBML_BOOLEAN_CLASS, 0, 0, "EditionFlagDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionOrdered = {0x45DD, EBML_BOOLEAN_CLASS, 0, 0, "EditionFlagOrdered", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticEdition[] = {
    {1, 0, &MATROSKA_ContextEditionUID     ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextEditionHidden  ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextEditionDefault ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextEditionOrdered ,PROFILE_TEST},
    {1, 0, &MATROSKA_ContextChapterAtom    ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterEntry = {0x45B9, EBML_MASTER_CLASS, 0, 0, "EditionEntry", EBML_SemanticEdition, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapters[] = {
    {1, 0, &MATROSKA_ContextChapterEntry ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapters = {0x1043A770, EBML_MASTER_CLASS, 0, 0, "Chapters", EBML_SemanticChapters, EBML_SemanticGlobals, NULL};

// Cluster
const ebml_context MATROSKA_ContextClusterBlockAdditionalID = {0xEE, EBML_INTEGER_CLASS, 1, (intptr_t)1, "ClusterBlockAdditionalID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockAdditional = {0xA5, EBML_BINARY_CLASS, 0, 0, "BlockAdditional", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterBlockMore[] = {
    {1, 1, &MATROSKA_ContextClusterBlockAdditionalID ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextClusterBlockAdditional   ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterBlockMore = {0xA6, EBML_MASTER_CLASS, 0, 0, "ClusterBlockMore", EBML_SemanticClusterBlockMore, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterBlockAdditions[] = {
    {1, 0, &MATROSKA_ContextClusterBlockMore ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterLaceNumber = {0xCC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterLaceNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSliceDuation = {0xCF, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterSliceDuation", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterTimeSlice[] = {
    {0, 1, &MATROSKA_ContextClusterLaceNumber   ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextClusterSliceDuation ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterTimeSlice = {0xE8, EBML_MASTER_CLASS, 0, 0, "ClusterTimeSlice", EBML_SemanticClusterTimeSlice, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterSlices[] = {
    {0, 0, &MATROSKA_ContextClusterTimeSlice ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterBlock = {0xA1, MATROSKA_BLOCK_CLASS, 0, 0, "ClusterBlock", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockAdditions = {0x75A1, EBML_MASTER_CLASS, 0, 0, "ClusterBlockAdditions", EBML_SemanticClusterBlockAdditions, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockDuration = {0x9B, EBML_INTEGER_CLASS, 0, 0, "ClusterBlockDuration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterReferencePriority = {0xFA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterReferencePriority", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterReferenceBlock = {0xFB, EBML_SINTEGER_CLASS, 0, 0, "ClusterReferenceBlock", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterCodecState = {0xA4, EBML_BINARY_CLASS, 0, 0, "CodecState", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSlices = {0x8E, EBML_MASTER_CLASS, 0, 0, "ClusterSlices", EBML_SemanticClusterSlices, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticClusterBlockGroup[] = {
    {1, 1, &MATROSKA_ContextClusterBlock             ,0},
    {0, 1, &MATROSKA_ContextClusterBlockDuration     ,0},
    {1, 1, &MATROSKA_ContextClusterReferencePriority ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextClusterReferenceBlock    ,0},
    {0, 1, &MATROSKA_ContextClusterCodecState        ,PROFILE_MATROSKA_V1|PROFILE_TEST},
    {0, 0, &MATROSKA_ContextClusterSlices            ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextClusterBlockAdditions    ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterSilentTrackNumber = {0x58D7, EBML_INTEGER_CLASS, 0, 0, "ClusterSilentTrackNumber", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticClusterSilentTracks[] = {
    {0, 0, &MATROSKA_ContextClusterSilentTrackNumber ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterTimecode = {0xE7, EBML_INTEGER_CLASS, 0, 0, "ClusterTimecode", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSilentTracks = {0x5854, EBML_MASTER_CLASS, 0, 0, "ClusterSilentTracks", EBML_SemanticClusterSilentTracks, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterPosition = {0xA7, EBML_INTEGER_CLASS, 0, 0, "ClusterPosition", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterPrevSize = {0xAB, EBML_INTEGER_CLASS, 0, 0, "ClusterPrevSize", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockGroup = {0xA0, EBML_MASTER_CLASS, 0, 0, "ClusterBlockGroup", EBML_SemanticClusterBlockGroup, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSimpleBlock = {0xA3, MATROSKA_BLOCK_CLASS, 0, 0, "ClusterSimpleBlock", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCluster[] = {
    {1, 1, &MATROSKA_ContextClusterTimecode     ,0},
    {0, 1, &MATROSKA_ContextClusterPosition     ,0},
    {0, 1, &MATROSKA_ContextClusterPrevSize     ,0},
    {0, 0, &MATROSKA_ContextClusterBlockGroup   ,0},
    {0, 0, &MATROSKA_ContextClusterSimpleBlock  ,PROFILE_MATROSKA_V1},
    {0, 1, &MATROSKA_ContextClusterSilentTracks ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextCluster = {0x1F43B675, MATROSKA_CLUSTER_CLASS, 0, 0, "Cluster", EBML_SemanticCluster, EBML_SemanticGlobals, NULL};

// Tracks
const ebml_context MATROSKA_SemanticTrackAudioSamplingFreq = {0xB5, EBML_FLOAT_CLASS, 1, (intptr_t)8000.0, "TrackAudioSamplingFreq", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackAudioOutputSamplingFreq = {0x78B5, EBML_FLOAT_CLASS, 0, 0, "TrackAudioOutputSamplingFreq", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackAudioChannels = {0x9F, EBML_INTEGER_CLASS, 1, (intptr_t)1, "TrackAudioChannels", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackAudioBitDepth = {0x6264, EBML_INTEGER_CLASS, 0, 0, "TrackAudioBitDepth", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackAudio[] = {
    {1, 1, &MATROSKA_SemanticTrackAudioSamplingFreq       ,0},
    {0, 1, &MATROSKA_SemanticTrackAudioOutputSamplingFreq ,PROFILE_TEST},
    {1, 1, &MATROSKA_SemanticTrackAudioChannels           ,0},
    {0, 1, &MATROSKA_SemanticTrackAudioBitDepth           ,0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_SemanticTrackVideoInterlaced = {0x9A, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "TrackVideoInterlaced", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoStereo = {0x53B8, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoStereo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoPixelWidth = {0xB0, EBML_INTEGER_CLASS, 0, 0, "TrackVideoPixelWidth", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoPixelHeight = {0xBA, EBML_INTEGER_CLASS, 0, 0, "TrackVideoPixelHeight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropBottom = {0x54AA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropBottom", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropTop = {0x54BB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropTop", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropLeft = {0x54CC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropLeft", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropRight = {0x54DD, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropRight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoDisplayWidth = {0x54B0, EBML_INTEGER_CLASS, 0, 0, "TrackVideoDisplayWidth", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoDisplayHeight = {0x54BA, EBML_INTEGER_CLASS, 0, 0, "TrackVideoDisplayHeight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoDisplayUnit = {0x54B2, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoDisplayUnit", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoAspectRatio = {0x54B3, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoAspectRatio", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoColourSpace = {0x2EB524, EBML_BINARY_CLASS, 0, 0, "TrackVideoColourSpace", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackVideoFrameRate = {0x2383E3, EBML_FLOAT_CLASS, 0, 0, "TrackVideoFrameRate", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackVideo[] = {
    {1, 1, &MATROSKA_SemanticTrackVideoInterlaced      ,PROFILE_MATROSKA_V1},
    {1, 1, &MATROSKA_SemanticTrackVideoPixelWidth      ,0},
    {1, 1, &MATROSKA_SemanticTrackVideoPixelHeight     ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoPixelCropBottom ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoPixelCropTop    ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoPixelCropLeft   ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoPixelCropRight  ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoDisplayWidth    ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoDisplayHeight   ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoDisplayUnit     ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoAspectRatio     ,0},
    {0, 1, &MATROSKA_SemanticTrackVideoColourSpace     ,PROFILE_TEST},
    {0, 1, &MATROSKA_SemanticTrackVideoStereo          ,PROFILE_MATROSKA_V1|PROFILE_TEST},
    {0, 1, &MATROSKA_SemanticTrackVideoFrameRate       ,0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncodingCompressionAlgo = {0x4254, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingCompressionAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingCompressionSetting = {0x4255, EBML_BINARY_CLASS, 0, 0, "TrackEncodingCompressionSetting", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncodingCompression[] = {
    {1, 1, &MATROSKA_SemanticTrackEncodingCompressionAlgo    ,0},
    {0, 1, &MATROSKA_SemanticTrackEncodingCompressionSetting ,0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncodingEncryptionAlgo = {0x47E1, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionKeyID = {0x47E2, EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionKeyID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignature = {0x47E3, EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionSignature", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignatureKeyID = {0x47E4, EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionSignatureKeyID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignatureAlgo = {0x47E5, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionSignatureAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignatureHashAlgo = {0x47E6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionSignatureHashAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncodingEncryption[] = {
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionAlgo              ,PROFILE_TEST},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionKeyID             ,PROFILE_TEST},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignature         ,PROFILE_TEST},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignatureKeyID    ,PROFILE_TEST},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignatureAlgo     ,PROFILE_TEST},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignatureHashAlgo ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncodingOrder = {0x5031, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingOrder", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingScope = {0x5032, EBML_INTEGER_CLASS, 1, (intptr_t)1, "TrackEncodingScope", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingType = {0x5033, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingCompression = {0x5034, EBML_MASTER_CLASS, 0, 0, "TrackEncodingCompression", EBML_SemanticTrackEncodingCompression, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackEncodingEncryption = {0x5035, EBML_MASTER_CLASS, 0, 0, "TrackEncodingEncryption", EBML_SemanticTrackEncodingEncryption, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncoding[] = {
    {1, 1, &MATROSKA_SemanticTrackEncodingOrder       ,0},
    {1, 1, &MATROSKA_SemanticTrackEncodingScope       ,0},
    {1, 1, &MATROSKA_SemanticTrackEncodingType        ,0},
    {0, 1, &MATROSKA_SemanticTrackEncodingCompression ,0},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryption  ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncoding = {0x6240, EBML_MASTER_CLASS, 0, 0, "TrackEncoding", EBML_SemanticTrackEncoding, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncodings[] = {
    {1, 0, &MATROSKA_SemanticTrackEncoding ,0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_SemanticTrackTranslateEditionUID = {0x66FC, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackTranslateCodec = {0x66BF, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateCodec", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_SemanticTrackTranslateID = {0x66A5, EBML_BINARY_CLASS, 0, 0, "TrackTranslateID", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackTranslate[] = {
    {0, 0, &MATROSKA_SemanticTrackTranslateEditionUID ,PROFILE_TEST},
    {1, 1, &MATROSKA_SemanticTrackTranslateCodec      ,PROFILE_TEST},
    {1, 1, &MATROSKA_SemanticTrackTranslateID         ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextTrackNumber = {0xD7, EBML_INTEGER_CLASS, 0, 0, "TrackNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackUID = {0x73C5, EBML_INTEGER_CLASS, 0, 0, "TrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackType = {0x83, EBML_INTEGER_CLASS, 0, 0, "TrackType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEnabled = {0xB9, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TrackEnabled", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackDefault = {0x88, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TrackDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackForced = {0x55AA, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "TrackForced", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackLacing = {0x9C, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TrackLacing", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackMinCache = {0x6DE7, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackMinCache", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackMaxCache = {0x6DF8, EBML_INTEGER_CLASS, 0, 0, "TrackMaxCache", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackDefaultDuration = {0x23E383, EBML_INTEGER_CLASS, 0, 0, "TrackDefaultDuration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTimecodeScale = {0x23314F, EBML_FLOAT_CLASS, 1, (intptr_t)1.0, "TrackTimecodeScale", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackMaxBlockAdditionID = {0x55EE, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackMaxBlockAdditionID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackName = {0x536E, EBML_UNISTRING_CLASS, 0, 0, "TrackName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackLanguage = {0x22B59C, EBML_STRING_CLASS, 1, (intptr_t)"eng", "TrackLanguage", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackCodecID = {0x86, EBML_STRING_CLASS, 0, 0, "TrackCodecID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackCodecPrivate = {0x63A2, EBML_BINARY_CLASS, 0, 0, "TrackCodecPrivate", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackCodecName = {0x258688, EBML_UNISTRING_CLASS, 0, 0, "TrackCodecName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackAttachmentLink = {0x7446, EBML_INTEGER_CLASS, 0, 0, "TrackAttachmentLink", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackOverlay = {0x6FAB, EBML_INTEGER_CLASS, 0, 0, "TrackOverlay", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTranslate = {0x6624, EBML_MASTER_CLASS, 0, 0, "TrackTranslate", EBML_SemanticTrackTranslate, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideo = {0xE0, EBML_MASTER_CLASS, 0, 0, "TrackVideo", EBML_SemanticTrackVideo, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackAudio = {0xE1, EBML_MASTER_CLASS, 0, 0, "TrackAudio", EBML_SemanticTrackAudio, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodings = {0x6D80, EBML_MASTER_CLASS, 0, 0, "TrackEncodings", EBML_SemanticTrackEncodings, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackCodecDecodeAll = {0xAA, EBML_INTEGER_CLASS, 1, (intptr_t)1, "TrackCodecDecodeAll", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackEntry[] = {
    {1, 1, &MATROSKA_ContextTrackNumber             ,0},
    {1, 1, &MATROSKA_ContextTrackUID                ,0},
    {1, 1, &MATROSKA_ContextTrackType               ,0},
    {1, 1, &MATROSKA_ContextTrackCodecID            ,0},
    {1, 1, &MATROSKA_ContextTrackEnabled            ,PROFILE_MATROSKA_V1},
    {1, 1, &MATROSKA_ContextTrackDefault            ,0},
    {1, 0, &MATROSKA_ContextTrackForced             ,0},
    {0, 1, &MATROSKA_ContextTrackVideo              ,0},
    {0, 1, &MATROSKA_ContextTrackAudio              ,0},
    {1, 1, &MATROSKA_ContextTrackLacing             ,0},
    {1, 1, &MATROSKA_ContextTrackMinCache           ,0},
    {0, 1, &MATROSKA_ContextTrackMaxCache           ,0},
    {0, 1, &MATROSKA_ContextTrackDefaultDuration    ,0},
    {1, 1, &MATROSKA_ContextTrackTimecodeScale      ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextTrackMaxBlockAdditionID ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextTrackName               ,0},
    {0, 1, &MATROSKA_ContextTrackLanguage           ,0},
    {0, 1, &MATROSKA_ContextTrackCodecPrivate       ,0},
    {0, 1, &MATROSKA_ContextTrackCodecName          ,0},
    {0, 1, &MATROSKA_ContextTrackAttachmentLink     ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextTrackOverlay            ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextTrackEncodings          ,0},
    {0, 0, &MATROSKA_ContextTrackTranslate          ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextTrackCodecDecodeAll     ,PROFILE_MATROSKA_V1},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTrackEntry = {0xAE, EBML_MASTER_CLASS, 0, 0, "TrackEntry", EBML_SemanticTrackEntry, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTracks[] = {
    {1, 0, &MATROSKA_ContextTrackEntry ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTracks = {0x1654AE6B, EBML_MASTER_CLASS, 0, 0, "Tracks", EBML_SemanticTracks, EBML_SemanticGlobals, NULL};

// Cues
const ebml_context MATROSKA_ContextRefTime = {0x96, EBML_INTEGER_CLASS, 0, 0, "CueTrack", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticCueReference[] = {
    {1, 1, &MATROSKA_ContextRefTime ,PROFILE_MATROSKA_V1},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCueTrack = {0xF7, EBML_INTEGER_CLASS, 0, 0, "CueTrack", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueClusterPosition = {0xF1, EBML_INTEGER_CLASS, 0, 0, "CueClusterPosition", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueBlockNumber = {0x5378, EBML_INTEGER_CLASS, 1, (intptr_t)1, "CueBlockNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueCodecState = {0xEA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "CueCodecState", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueReference = {0xDB, EBML_MASTER_CLASS, 0, 0, "CueReference", EBML_SemanticCueReference, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCueTrackPosition[] = {
    {1, 1, &MATROSKA_ContextCueTrack           ,0},
    {1, 1, &MATROSKA_ContextCueClusterPosition ,0},
    {0, 1, &MATROSKA_ContextCueBlockNumber     ,0},
    {0, 1, &MATROSKA_ContextCueCodecState      ,PROFILE_MATROSKA_V1},
    {0, 1, &MATROSKA_ContextCueReference       ,PROFILE_MATROSKA_V1},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCueTrackPositions = {0xB7, EBML_MASTER_CLASS, 0, 0, "CueTrackPosition", EBML_SemanticCueTrackPosition, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueTime = {0xB3, EBML_INTEGER_CLASS, 0, 0, "CueTime", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCuePoint[] = {
    {1, 1, &MATROSKA_ContextCueTime           ,0},
    {1, 0, &MATROSKA_ContextCueTrackPositions ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCuePoint = {0xBB, MATROSKA_CUEPOINT_CLASS, 0, 0, "CuePoint", EBML_SemanticCuePoint, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCues[] = {
    {1, 0, &MATROSKA_ContextCuePoint ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCues = {0x1C53BB6B, EBML_MASTER_CLASS, 0, 0, "Cues", EBML_SemanticCues, EBML_SemanticGlobals, NULL};

// Attachments
const ebml_context MATROSKA_ContextAttachedFileDescription = {0x467E, EBML_UNISTRING_CLASS, 0, 0, "AttachedFileDescription", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAttachedFileName = {0x466E, EBML_UNISTRING_CLASS, 0, 0, "AttachedFileName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAttachedFileMimeType = {0x4660, EBML_STRING_CLASS, 0, 0, "AttachedFileMimeType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAttachedFileData = {0x465C, EBML_BINARY_CLASS, 0, 0, "AttachedFileData", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAttachedFileUID = {0x46AE, EBML_INTEGER_CLASS, 0, 0, "AttachedFileUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticAttachedFile[] = {
    {1, 1, &MATROSKA_ContextAttachedFileName        ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextAttachedFileMimeType    ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextAttachedFileData        ,PROFILE_TEST},
    {1, 1, &MATROSKA_ContextAttachedFileUID         ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextAttachedFileDescription ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextAttachedFile = {0x61A7, EBML_MASTER_CLASS, 0, 0, "AttachedFile", EBML_SemanticAttachedFile, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticAttachments[] = {
    {1, 0, &MATROSKA_ContextAttachedFile ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextAttachments = {0x1941A469, EBML_MASTER_CLASS, 0, 0, "Attachments", EBML_SemanticAttachments, EBML_SemanticGlobals, NULL};

// Tags
const ebml_context MATROSKA_ContextTagTargetTypeValue = {0x68CA, EBML_INTEGER_CLASS, 1, (intptr_t)50, "TagTargetTypeValue", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagTargetType = {0x63CA, EBML_STRING_CLASS, 0, 0, "TagTargetType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagTargetTrackUID = {0x63C5, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetTrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagTargetEditionUID = {0x63C9, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagTargetChapterUID = {0x63C4, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetChapterUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagTargetAttachmentUID = {0x63C6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetAttachmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTagTargets[] = {
    {0, 1, &MATROSKA_ContextTagTargetTypeValue     ,0},
    {0, 1, &MATROSKA_ContextTagTargetType          ,0},
    {0, 0, &MATROSKA_ContextTagTargetTrackUID      ,0},
    {0, 0, &MATROSKA_ContextTagTargetEditionUID    ,0},
    {0, 0, &MATROSKA_ContextTagTargetChapterUID    ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextTagTargetAttachmentUID ,PROFILE_TEST},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTagTargets = {0x63C0, EBML_MASTER_CLASS, 0, 0, "TagTargets", EBML_SemanticTagTargets, EBML_SemanticGlobals, NULL};

const ebml_context MATROSKA_ContextTagName = {0x45A3, EBML_UNISTRING_CLASS, 0, 0, "TagName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagLanguage = {0x447A, EBML_STRING_CLASS, 1, (intptr_t)"und", "TagLanguage", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagDefault = {0x4484, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TagDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagString = {0x4487, EBML_UNISTRING_CLASS, 0, 0, "TagString", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagBinary = {0x4485, EBML_BINARY_CLASS, 0, 0, "TagBinary", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticSimpleTag[] = {
    {1, 1, &MATROSKA_ContextTagName     ,0},
    {1, 1, &MATROSKA_ContextTagLanguage ,0},
    {1, 1, &MATROSKA_ContextTagDefault  ,0},
    {0, 1, &MATROSKA_ContextTagString   ,0},
    {0, 1, &MATROSKA_ContextTagBinary   ,0},
    {0, 0, &MATROSKA_ContextSimpleTag   ,0}, // recursive
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSimpleTag = {0x67C8, EBML_MASTER_CLASS, 0, 0, "SimpleTag", EBML_SemanticSimpleTag, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTag[] = {
    {1, 1, &MATROSKA_ContextTagTargets ,0},
    {1, 0, &MATROSKA_ContextSimpleTag  ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTag = {0x7373, EBML_MASTER_CLASS, 0, 0, "Tag", EBML_SemanticTag, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTags[] = {
    {1, 0, &MATROSKA_ContextTag ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTags = {0x1254C367, EBML_MASTER_CLASS, 0, 0, "Tags", EBML_SemanticTags, EBML_SemanticGlobals, NULL};

// Segment
const ebml_semantic EBML_SemanticSegment[] = {
    {0, 0, &MATROSKA_ContextSeekHead     ,0},
    {1, 0, &MATROSKA_ContextSegmentInfo  ,0},
    {0, 0, &MATROSKA_ContextCluster      ,0},
    {0, 0, &MATROSKA_ContextTracks       ,0},
    {0, 1, &MATROSKA_ContextCues         ,0},
    {0, 1, &MATROSKA_ContextAttachments  ,PROFILE_TEST},
    {0, 1, &MATROSKA_ContextChapters     ,PROFILE_TEST},
    {0, 0, &MATROSKA_ContextTags         ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSegment = {0x18538067, EBML_MASTER_CLASS, 0, 0, "Segment\0wrmf", EBML_SemanticSegment, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticMatroska[] = {
    {1, 0, &EBML_ContextHead        ,0},
    {1, 0, &MATROSKA_ContextSegment ,0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextStream = {FOURCC('M','K','X','_'), EBML_MASTER_CLASS, 0, 0, "Matroska Stream", EBML_SemanticMatroska, EBML_SemanticGlobals, NULL};

err_t MATROSKA_Init(nodecontext *p)
{
    tchar_t LibName[MAXPATH];
    err_t Err = EBML_Init(p);
    if (Err == ERR_NONE)
    {
        tcscpy_s(LibName,TSIZEOF(LibName),PROJECT_NAME T(" v") PROJECT_VERSION);
        Node_SetData(p,CONTEXT_LIBMATROSKA_VERSION,TYPE_STRING,LibName);
    }
    return Err;
}

err_t MATROSKA_Done(nodecontext *p)
{
    return EBML_Done(p);
}


#define MATROSKA_BLOCK_TRACK         0x100
#define MATROSKA_BLOCK_SEGMENTINFO   0x101

#define MATROSKA_CUE_SEGMENTINFO     0x100
#define MATROSKA_CUE_BLOCK           0x101

#define MATROSKA_CLUSTER_SEGMENTINFO 0x100

#define MATROSKA_SEEKPOINT_ELEMENT   0x100

#define LACING_NONE  0
#define LACING_XIPH  1
#define LACING_FIXED 2
#define LACING_EBML  3
#define LACING_AUTO  4

struct matroska_block
{
    ebml_binary Base;
    uint16_t TrackNumber;
    int16_t LocalTimecode;
    bool_t LocalTimecodeUsed;
    bool_t IsKeyframe;
    bool_t IsDiscardable;
    bool_t Invisible;
    char Lacing;
    filepos_t FirstFrameLocation;
    array SizeList; // int32_t
    array Data; // uint8_t
    ebml_element *Track;
    ebml_element *SegInfo;

};

struct matroska_cuepoint
{
    ebml_element Base;
    ebml_element *SegInfo;
    matroska_block *Block;

};

struct matroska_cluster
{
    ebml_element Base;
    ebml_element *SegInfo;

};

struct matroska_seekpoint
{
    ebml_element Base;
    ebml_element *Link;
};

err_t MATROSKA_LinkBlockTrack(matroska_block *Block, ebml_element *Tracks)
{
    ebml_element *Track, *TrackNum;
    assert(Tracks->Context->Id == MATROSKA_ContextTracks.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    for (Track=EBML_MasterChildren(Tracks);Track;Track=EBML_MasterNext(Track))
    {
        TrackNum = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
        if (TrackNum && ((ebml_integer*)TrackNum)->Base.bValueIsSet && ((ebml_integer*)TrackNum)->Value==Block->TrackNumber)
        {
            Node_SET(Block,MATROSKA_BLOCK_TRACK,&Track);
            return ERR_NONE;
        }
    }
    return ERR_INVALID_DATA;
}

err_t MATROSKA_LinkMetaSeekElement(matroska_seekpoint *MetaSeek, ebml_element *Link)
{
    assert(MetaSeek->Base.Context->Id == MATROSKA_ContextSeek.Id);
    Node_SET(MetaSeek,MATROSKA_SEEKPOINT_ELEMENT,&Link);
    return ERR_NONE;
}

fourcc_t MATROSKA_MetaSeekID(const matroska_seekpoint *MetaSeek)
{
	ebml_element *SeekID;
    assert(MetaSeek->Base.Context->Id == MATROSKA_ContextSeek.Id);
	SeekID = EBML_MasterFindFirstElt((ebml_element*)MetaSeek, &MATROSKA_ContextSeekId, 0, 0);
	if (!SeekID)
		return 0;
	return EBML_BufferToID(EBML_BinaryGetData((ebml_binary*)SeekID));
}

filepos_t MATROSKA_MetaSeekPosInSegment(const matroska_seekpoint *MetaSeek)
{
	ebml_element *SeekPos;
    assert(MetaSeek->Base.Context->Id == MATROSKA_ContextSeek.Id);
	SeekPos = EBML_MasterFindFirstElt((ebml_element*)MetaSeek, &MATROSKA_ContextSeekPosition, 0, 0);
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
    while (RSegment && RSegment->Context->Id != MATROSKA_ContextSegment.Id)
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

    assert(MetaSeek->Base.Context->Id == MATROSKA_ContextSeek.Id);
    RSegment = EBML_ElementParent(MetaSeek);
    while (RSegment && RSegment->Context->Id != MATROSKA_ContextSegment.Id)
        RSegment = EBML_ElementParent(RSegment);
    if (!RSegment)
        return ERR_INVALID_DATA;

    Err = Node_GET(MetaSeek,MATROSKA_SEEKPOINT_ELEMENT,&Link);
    if (Err != ERR_NONE)
        return Err;
    if (Link==NULL)
        return ERR_INVALID_DATA;

    WSeekID = EBML_MasterFindFirstElt((ebml_element*)MetaSeek,&MATROSKA_ContextSeekId,1,0);
    IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),Link->Context->Id);
    EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);

    WSeekPosSegmentInfo = EBML_MasterFindFirstElt((ebml_element*)MetaSeek,&MATROSKA_ContextSeekPosition,1,0);
    ((ebml_integer*)WSeekPosSegmentInfo)->Value = Link->ElementPosition - EBML_ElementPositionData(RSegment);
    WSeekPosSegmentInfo->bValueIsSet = 1;

    return Err;
}

err_t MATROSKA_LinkClusterSegmentInfo(matroska_cluster *Cluster, ebml_element *SegmentInfo)
{
    assert(Cluster->Base.Context->Id == MATROSKA_ContextCluster.Id);
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
    Node_SET(Cluster,MATROSKA_CLUSTER_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}

err_t MATROSKA_LinkBlockSegmentInfo(matroska_block *Block, ebml_element *SegmentInfo)
{
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    Node_SET(Block,MATROSKA_BLOCK_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}

err_t MATROSKA_LinkCueSegmentInfo(matroska_cuepoint *Cue, ebml_element *SegmentInfo)
{
    assert(Cue->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
    Node_SET(Cue,MATROSKA_CUE_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}

err_t MATROSKA_LinkCuePointBlock(matroska_cuepoint *CuePoint, matroska_block *Block)
{
    assert(CuePoint->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    Node_SET(CuePoint,MATROSKA_CUE_BLOCK,&Block);
    return ERR_NONE;
}

timecode_t MATROSKA_ClusterTimecode(const matroska_cluster *Cluster)
{
    ebml_element *Timecode;
    assert(Cluster->Base.Context->Id == MATROSKA_ContextCluster.Id);
    Timecode = EBML_MasterFindFirstElt((ebml_element*)Cluster,&MATROSKA_ContextClusterTimecode,0,0);
    if (!Timecode)
        return INVALID_TIMECODE_T;
    assert(Timecode->bValueIsSet);
    return ((ebml_integer*)Timecode)->Value * MATROSKA_SegmentInfoTimecodeScale(Cluster->SegInfo);
}

timecode_t MATROSKA_BlockTimecode(const matroska_block *Block)
{
    ebml_element *Cluster;
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    Cluster = EBML_ElementParent(Block);
    while (Cluster && Cluster->Context->Id != MATROSKA_ContextCluster.Id)
        Cluster = EBML_ElementParent(Cluster);
    if (!Cluster)
        return INVALID_TIMECODE_T;
    return MATROSKA_ClusterTimecode((matroska_cluster*)Cluster) + (timecode_t)(Block->LocalTimecode * MATROSKA_SegmentInfoTimecodeScale(Block->SegInfo) * MATROSKA_TrackTimecodeScale(Block->Track));
}

int16_t MATROSKA_BlockTrackNum(const matroska_block *Block)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    return Block->TrackNumber;
}

bool_t MATROSKA_BlockKeyframe(const matroska_block *Block)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	return Block->IsKeyframe;
}

int16_t MATROSKA_CueTrackNum(const matroska_cuepoint *Cue)
{
    ebml_element *Position, *CueTrack;
    assert(Cue->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    Position = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTrackPositions,0,0);
    if (!Position)
        return -1;
    CueTrack = EBML_MasterFindFirstElt(Position,&MATROSKA_ContextCueTrack,0,0);
    if (!CueTrack)
        return -1;
    assert(CueTrack->bValueIsSet);
    return (int16_t)((ebml_integer*)CueTrack)->Value;
}

void MATROSKA_CuesSort(ebml_element *Cues)
{
    assert(Cues->Context->Id == MATROSKA_ContextCues.Id);
    EBML_MasterSort(Cues,NULL,NULL);
}

timecode_t MATROSKA_SegmentInfoTimecodeScale(const ebml_element *SegmentInfo)
{
    ebml_element *TimecodeScale;
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
    TimecodeScale = EBML_MasterFindFirstElt((ebml_element*)SegmentInfo,&MATROSKA_ContextTimecodeScale,0,0);
    if (!TimecodeScale)
        return MATROSKA_ContextTimecodeScale.DefaultValue;
    assert(TimecodeScale->bValueIsSet);
    return ((ebml_integer*)TimecodeScale)->Value;
}

double MATROSKA_TrackTimecodeScale(const ebml_element *Track)
{
    ebml_element *TimecodeScale;
    assert(Track->Context->Id == MATROSKA_ContextTrackEntry.Id);
    TimecodeScale = EBML_MasterFindFirstElt((ebml_element*)Track,&MATROSKA_ContextTrackTimecodeScale,0,0);
    if (!TimecodeScale)
        return 1.0;
    assert(TimecodeScale->bValueIsSet);
    return ((ebml_float*)TimecodeScale)->Value;
}

timecode_t MATROSKA_CueTimecode(const matroska_cuepoint *Cue)
{
    ebml_element *TimeCode;
    assert(Cue->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    TimeCode = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTime,0,0);
    if (!TimeCode)
        return INVALID_TIMECODE_T;
    assert(TimeCode->bValueIsSet);
    return ((ebml_integer*)TimeCode)->Value * MATROSKA_SegmentInfoTimecodeScale(Cue->SegInfo);
}

err_t MATROSKA_CuePointUpdate(matroska_cuepoint *Cue, ebml_element *Segment)
{
    ebml_element *TimeCode, *Elt, *PosInCluster, *TrackNum;
    assert(Cue->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    assert(Cue->Block);
    assert(Cue->SegInfo);
    assert(Segment); // we need the segment location
	EBML_MasterErase((ebml_element*)Cue);
	EBML_MasterMandatory((ebml_element*)Cue,1);
    TimeCode = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTime,1,1);
    if (!TimeCode)
        return ERR_OUT_OF_MEMORY;
    ((ebml_integer*)TimeCode)->Value = Scale64(MATROSKA_BlockTimecode(Cue->Block),1,MATROSKA_SegmentInfoTimecodeScale(Cue->SegInfo));
	TimeCode->bValueIsSet = 1;

    Elt = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTrackPositions,1,1);
    if (!Elt)
        return ERR_OUT_OF_MEMORY;
	TrackNum = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextCueTrack,1,1);
    if (!TrackNum)
        return ERR_OUT_OF_MEMORY;
	((ebml_integer*)TrackNum)->Value = MATROSKA_BlockTrackNum(Cue->Block);
	TrackNum->bValueIsSet = 1;
	
    PosInCluster = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextCueClusterPosition,1,1);
    if (!PosInCluster)
        return ERR_OUT_OF_MEMORY;
    Elt = EBML_ElementParent(Cue->Block);
    while (Elt && Elt->Context->Id != MATROSKA_ContextCluster.Id)
        Elt = EBML_ElementParent(Elt);
    if (!Elt)
        return ERR_INVALID_DATA;
    
    ((ebml_integer*)PosInCluster)->Value = Elt->ElementPosition - EBML_ElementPositionData(Segment);
	PosInCluster->bValueIsSet = 1;

    return ERR_NONE;
}

static size_t GetBlockHeadSize(const matroska_block *Element)
{
    assert(Element->TrackNumber < 0x4000);
    if (Element->TrackNumber < 0x80)
        return 4;
    else
        return 5;
}

err_t MATROSKA_BlockReleaseData(matroska_block *Block)
{
    ArrayClear(&Block->Data);
    return ERR_NONE;
}

err_t MATROSKA_BlockReadData(matroska_block *Element, stream *Input)
{
    size_t Read;
    size_t NumFrame;
    err_t Err = ERR_NONE;
    Element->Base.Base.bValueIsSet = 1;
    switch (Element->Lacing)
    {
    case LACING_NONE:
        Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element) + GetBlockHeadSize(Element),SEEK_SET);
        ArrayResize(&Element->Data,(size_t)Element->Base.Base.DataSize - GetBlockHeadSize(Element),0);
        Err = Stream_Read(Input,ARRAYBEGIN(Element->Data,uint8_t),(size_t)Element->Base.Base.DataSize - GetBlockHeadSize(Element),&Read);
        if (Err != ERR_NONE)
            goto failed;
        if (Read != Element->Base.Base.DataSize - GetBlockHeadSize(Element))
        {
            Err = ERR_READ;
            goto failed;
        }
        break;
    case LACING_EBML:
    case LACING_XIPH:
    case LACING_FIXED:
        Stream_Seek(Input,EBML_ElementPositionData((ebml_element*)Element) + Element->FirstFrameLocation,SEEK_SET);
        Read = 0;
        for (NumFrame=0;NumFrame<ARRAYCOUNT(Element->SizeList,int32_t);++NumFrame)
            Read += ARRAYBEGIN(Element->SizeList,int32_t)[NumFrame];
        ArrayResize(&Element->Data,Read,0);
        Err = Stream_Read(Input,ARRAYBEGIN(Element->Data,uint8_t),Read,&Read);
        if (Err != ERR_NONE)
            goto failed;
        break;
    default:
        assert(0); // we should support the other lacing modes
        Err = ERR_NOT_SUPPORTED;
    }

failed:
    return Err;
}

static err_t ReadBlockData(matroska_block *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    err_t Result;
	uint8_t _TempHead[5];
	uint8_t *cursor = _TempHead;
	uint8_t *_tmpBuf;
	uint8_t BlockHeadSize = 4; // default when the TrackNumber is < 16

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

	if (Element->Base.Base.Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
    {
		Element->IsKeyframe = (*cursor & 0x80) != 0;
		Element->IsDiscardable = (*cursor & 0x01) != 0;
	}
	Element->Invisible = (*cursor & 0x08) >> 3;
	Element->Lacing = (*cursor++ & 0x06) >> 1;
	if (cursor == &_TempHead[4])
		_TempHead[0] = _TempHead[4];
	else
		Result += Stream_Read(Input,_TempHead, 1, NULL);

	Element->FirstFrameLocation += cursor - _TempHead;

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
	    Result = ERR_NONE;
    else
        Result = MATROSKA_BlockReadData(Element, Input);

failed:
    return Result;
}

static char GetBestLacingType(const matroska_block *Element)
{
	int XiphLacingSize, EbmlLacingSize;
	bool_t SameSize = 1;

	if (ARRAYCOUNT(Element->SizeList,int32_t) <= 1)
		return LACING_NONE;

#ifdef TODO
	XiphLacingSize = 1; // Number of laces is stored in 1 byte.
	EbmlLacingSize = 1;
	for (i = 0; i < (int)myBuffers.size() - 1; i++) {
		if (myBuffers[i]->DataSize() != myBuffers[i + 1]->DataSize())
			SameSize = false;
		XiphLacingSize += myBuffers[i]->DataSize() / 255 + 1;
	}
	EbmlLacingSize += CodedSizeLength(myBuffers[0]->DataSize(), 0, IsFiniteSize());
	for (i = 1; i < (int)myBuffers.size() - 1; i++)
		EbmlLacingSize += CodedSizeLengthSigned(int64(myBuffers[i]->DataSize()) - int64(myBuffers[i - 1]->DataSize()), 0);
#endif
	if (SameSize)
		return LACING_FIXED;
	else if (XiphLacingSize < EbmlLacingSize)
		return LACING_XIPH;
	else
		return LACING_EBML;
}

#if defined(CONFIG_EBML_WRITING)
static err_t RenderBlockData(matroska_block *Element, stream *Output, bool_t bForceRender, bool_t bWithDefault, filepos_t *Rendered)
{
    err_t Err = ERR_NONE;
    uint8_t BlockHead[5], *Cursor;
    size_t Written, BlockHeadSize = 4;
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
    if (Element->Base.Base.Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
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

    assert(Element->Lacing != LACING_AUTO); // TODO: the rest is not supported yet
    if (Element->Lacing != LACING_NONE)
    {
        uint8_t *LaceHead = malloc(1 + ARRAYCOUNT(Element->SizeList,int32_t)*4);
        size_t i,LaceSize = 1;
        int32_t DataSize;
        if (!LaceHead)
        {
            Err = ERR_OUT_OF_MEMORY;;
            goto failed;
        }
        LaceHead[0] = (ARRAYCOUNT(Element->SizeList,int32_t)-1) & 0xFF;
        if (Element->Lacing == LACING_EBML)
        {
            LaceSize += EBML_CodedValueLength(ARRAYBEGIN(Element->SizeList,int32_t)[0],EBML_CodedSizeLength(ARRAYBEGIN(Element->SizeList,int32_t)[0],0,1),LaceHead+LaceSize);
            for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                DataSize = ARRAYBEGIN(Element->SizeList,int32_t)[i] - ARRAYBEGIN(Element->SizeList,int32_t)[i-1];
                LaceSize += EBML_CodedValueLengthSigned(DataSize,EBML_CodedSizeLengthSigned(DataSize,0),LaceHead+LaceSize);
            }
        }
        else if (Element->Lacing == LACING_XIPH)
        {
            for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                DataSize = ARRAYBEGIN(Element->SizeList,int32_t)[i];
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
    Err = Stream_Write(Output,ARRAYBEGIN(Element->Data,uint8_t),ARRAYCOUNT(Element->Data,uint8_t),&Written);

    if (Rendered)
        *Rendered += Written;

failed:
    return Err;
}
#endif

static filepos_t UpdateBlockSize(matroska_block *Element, bool_t bWithDefault, bool_t bForceRender)
{
    assert(Element->Lacing != LACING_AUTO); // TODO: not supported yet
    if (Element->Lacing == LACING_NONE)
    {
        assert(ARRAYCOUNT(Element->SizeList,int32_t) == 1);
        Element->Base.Base.DataSize = GetBlockHeadSize(Element) + ARRAYBEGIN(Element->SizeList,int32_t)[0];
    }
    else if (Element->Lacing == LACING_EBML)
    {
        size_t i;
        filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
        Result += ARRAYBEGIN(Element->SizeList,int32_t)[0] + EBML_CodedSizeLength(ARRAYBEGIN(Element->SizeList,int32_t)[0],0,1);
        for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            Result += ARRAYBEGIN(Element->SizeList,int32_t)[i] + EBML_CodedSizeLengthSigned(ARRAYBEGIN(Element->SizeList,int32_t)[i] - ARRAYBEGIN(Element->SizeList,int32_t)[i-1],0);
        Result += ARRAYBEGIN(Element->SizeList,int32_t)[i];
        Element->Base.Base.DataSize = Result;
    }
    else if (Element->Lacing == LACING_XIPH)
    {
        size_t i;
        filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
        for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            Result += ARRAYBEGIN(Element->SizeList,int32_t)[i] + (ARRAYBEGIN(Element->SizeList,int32_t)[i] / 0xFF + 1);
        Result += ARRAYBEGIN(Element->SizeList,int32_t)[i];
        Element->Base.Base.DataSize = Result;
    }
    else if (Element->Lacing == LACING_FIXED)
    {
        size_t i;
        filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
        for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t);++i)
            Result += ARRAYBEGIN(Element->SizeList,int32_t)[i];
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

	return Element->Base.Base.DataSize;
}

static int CmpCuePoint(const matroska_cuepoint* a,const matroska_cuepoint* b)
{
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

static bool_t ValidateSizeSegUID(const ebml_binary *p)
{
    uint8_t test[16];
    if (p->Base.DataSize != 16)
        return 0;
    if (!p->Base.bValueIsSet)
        return 1;
    memset(test,0,sizeof(test));
    return memcmp(ARRAYBEGIN(p->Data,uint8_t),test,16)!=0; // make sure the value is not 0
}

META_START(Matroska_Class,MATROSKA_BLOCK_CLASS)
META_CLASS(SIZE,sizeof(matroska_block))
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadBlockData)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateBlockSize)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderBlockData)
#endif
META_DATA(TYPE_ARRAY,0,matroska_block,SizeList)
META_DATA(TYPE_ARRAY,0,matroska_block,Data)
META_PARAM(TYPE,MATROSKA_BLOCK_TRACK,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_BLOCK_TRACK,matroska_block,Track)
META_PARAM(TYPE,MATROSKA_BLOCK_SEGMENTINFO,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_BLOCK_SEGMENTINFO,matroska_block,SegInfo)
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
META_PARAM(TYPE,MATROSKA_CLUSTER_SEGMENTINFO,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_CLUSTER_SEGMENTINFO,matroska_cluster,SegInfo)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_SEEKPOINT_CLASS)
META_CLASS(SIZE,sizeof(matroska_seekpoint))
META_PARAM(TYPE,MATROSKA_SEEKPOINT_ELEMENT,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_SEEKPOINT_ELEMENT,matroska_seekpoint,Link)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_SEGMENTUID_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeSegUID)
META_END(EBML_BINARY_CLASS)
