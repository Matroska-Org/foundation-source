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
#include "matroska_internal.h"
#if defined(CONFIG_ZLIB)
#include "zlib.h"
#endif
#if defined(MATROSKA_LIBRARY)
#include "matroska2_project.h"
#endif

#define MATROSKA_BLOCK_CLASS      FOURCC('M','K','B','L')
#define MATROSKA_BLOCKGROUP_CLASS FOURCC('M','K','B','G')
#define MATROSKA_CUEPOINT_CLASS   FOURCC('M','K','C','P')
#define MATROSKA_CLUSTER_CLASS    FOURCC('M','K','C','U')
#define MATROSKA_SEEKPOINT_CLASS  FOURCC('M','K','S','K')
#define MATROSKA_SEGMENTUID_CLASS FOURCC('M','K','I','D')
#define MATROSKA_BIGBINARY_CLASS  FOURCC('M','K','B','B')

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
    {0, 0, &MATROSKA_ContextTranslateEditionUID ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTranslateCodec      ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTranslateID         ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
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
    {0, 1, &MATROSKA_ContextSegmentFilename  ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextPrevUid          ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextPrevFilename     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextNextUid          ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextNextFilename     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextSegmentFamily    ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextChapterTranslate ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTimecodeScale    ,0},
    {0, 1, &MATROSKA_ContextDuration         ,0},
    {0, 1, &MATROSKA_ContextSegmentDate      ,0},
    {0, 1, &MATROSKA_ContextSegmentTitle     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextMuxingApp        ,0},
    {1, 1, &MATROSKA_ContextWritingApp       ,0},
    {0, 0, NULL, 0} // end of the table
};
const ebml_context MATROSKA_ContextSegmentInfo = {0x1549A966, EBML_MASTER_CLASS, 0, 0, "Info", EBML_SemanticSegmentInfo, EBML_SemanticGlobals, NULL};


// Chapters
const ebml_context MATROSKA_ContextChapterTrackNumber = {0x89, EBML_INTEGER_CLASS, 0, 0, "ChapterFlagEnabled", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterTrack[] = {
    {1, 0, &MATROSKA_ContextChapterTrackNumber ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterProcessTime = {0x6922, EBML_INTEGER_CLASS, 0, 0, "ChapterProcessTime", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcessData = {0x6933, EBML_BINARY_CLASS, 0, 0, "ChapterProcessData", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterProcessCommand[] = {
    {1, 1, &MATROSKA_ContextChapterProcessTime ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextChapterProcessData ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterProcessCodecID = {0x6955, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ChapterProcessCodecID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcessPrivate = {0x450D, EBML_BINARY_CLASS, 0, 0, "ChapterProcessPrivate", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcessCommand = {0x6911, EBML_MASTER_CLASS, 0, 0, "ChapterProcessCommand", EBML_SemanticChapterProcessCommand, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterProcess[] = {
    {1, 1, &MATROSKA_ContextChapterProcessCodecID ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextChapterProcessPrivate ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextChapterProcessCommand ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterString = {0x85, EBML_UNISTRING_CLASS, 0, 0, "ChapterString", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterLanguage = {0x437C, EBML_STRING_CLASS, 1, (intptr_t)"eng", "ChapterLanguage", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterCountry = {0x437E, EBML_STRING_CLASS, 0, 0, "ChapterCountry", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterDisplay[] = {
    {1, 1, &MATROSKA_ContextChapterString   ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 0, &MATROSKA_ContextChapterLanguage ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextChapterCountry  ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextChapterUID = {0x73C4, EBML_INTEGER_CLASS, 0, 0, "ChapterUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTimeStart = {0x91, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeStart", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTimeEnd = {0x92, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeEnd", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterHidden = {0x98, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "ChapterFlagHidden", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterEnabled = {0x4598, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "ChapterFlagEnabled", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterSegmentUID = {0x6E67, EBML_BINARY_CLASS, 0, 0, "ChapterSegmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterSegmentEditionUID = {0x6EBC, EBML_BINARY_CLASS, 0, 0, "ChapterSegmentEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterPhysical = {0x63C3, EBML_INTEGER_CLASS, 0, 0, "ChapterPhysicalEquiv", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTrack = {0x8F, EBML_MASTER_CLASS, 0, 0, "ChapterTrack", EBML_SemanticChapterTrack, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterDisplay = {0x80, EBML_MASTER_CLASS, 0, 0, "ChapterDisplay", EBML_SemanticChapterDisplay, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterProcess = {0x6944, EBML_MASTER_CLASS, 0, 0, "ChapterProcess", EBML_SemanticChapterProcess, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterAtom[] = {
    {0, 0, &MATROSKA_ContextChapterAtom              ,PROFILE_WEBM_V1|PROFILE_WEBM_V2}, // recursive
    {1, 1, &MATROSKA_ContextChapterUID               ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextChapterTimeStart         ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextChapterTimeEnd           ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextChapterHidden            ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextChapterEnabled           ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextChapterSegmentUID        ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextChapterSegmentEditionUID ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextChapterPhysical          ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextChapterTrack             ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextChapterDisplay           ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextChapterProcess           ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterAtom = {0xB6, EBML_MASTER_CLASS, 0, 0, "ChapterAtom", EBML_SemanticChapterAtom, EBML_SemanticGlobals, NULL};

const ebml_context MATROSKA_ContextEditionUID = {0x45BC, EBML_INTEGER_CLASS, 0, 0, "EditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionHidden = {0x45BD, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagHidden", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionDefault = {0x45DB, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionOrdered = {0x45DD, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagOrdered", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticEdition[] = {
    {1, 0, &MATROSKA_ContextEditionUID     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextEditionHidden  ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextEditionDefault ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextEditionOrdered ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 0, &MATROSKA_ContextChapterAtom    ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterEntry = {0x45B9, EBML_MASTER_CLASS, 0, 0, "EditionEntry", EBML_SemanticEdition, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapters[] = {
    {1, 0, &MATROSKA_ContextChapterEntry ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapters = {0x1043A770, EBML_MASTER_CLASS, 0, 0, "Chapters", EBML_SemanticChapters, EBML_SemanticGlobals, NULL};

// Cluster
const ebml_context MATROSKA_ContextClusterBlockAdditionalID = {0xEE, EBML_INTEGER_CLASS, 1, (intptr_t)1, "ClusterBlockAdditionalID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockAdditional = {0xA5, EBML_BINARY_CLASS, 0, 0, "BlockAdditional", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterBlockMore[] = {
    {1, 1, &MATROSKA_ContextClusterBlockAdditionalID ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextClusterBlockAdditional   ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterBlockMore = {0xA6, EBML_MASTER_CLASS, 0, 0, "ClusterBlockMore", EBML_SemanticClusterBlockMore, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterBlockAdditions[] = {
    {1, 0, &MATROSKA_ContextClusterBlockMore ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterLaceNumber = {0xCC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterLaceNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSliceDuation = {0xCF, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterSliceDuation", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterTimeSlice[] = {
    {0, 1, &MATROSKA_ContextClusterLaceNumber   ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextClusterSliceDuation ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterTimeSlice = {0xE8, EBML_MASTER_CLASS, 0, 0, "ClusterTimeSlice", EBML_SemanticClusterTimeSlice, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticClusterSlices[] = {
    {0, 0, &MATROSKA_ContextClusterTimeSlice ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

// DivX trick track extenstions http://developer.divx.com/docs/divx_plus_hd/format_features/Smooth_FF_RW
const ebml_context MATROSKA_ContextClusterReferenceOffset = {0xC9, EBML_INTEGER_CLASS, 0, 0, "ClusterReferenceOffset", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterReferenceTimeCode = {0xCA, EBML_INTEGER_CLASS, 0, 0, "ClusterReferenceTimeCode", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticClusterReferenceFrame[] = {
    {1, 1, &MATROSKA_ContextClusterReferenceOffset    ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextClusterReferenceTimeCode  ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterBlock = {0xA1, MATROSKA_BLOCK_CLASS, 0, 0, "ClusterBlock", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockAdditions = {0x75A1, EBML_MASTER_CLASS, 0, 0, "ClusterBlockAdditions", EBML_SemanticClusterBlockAdditions, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockDuration = {0x9B, EBML_INTEGER_CLASS, 0, 0, "ClusterBlockDuration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterReferencePriority = {0xFA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterReferencePriority", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterReferenceBlock = {0xFB, EBML_SINTEGER_CLASS, 0, 0, "ClusterReferenceBlock", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterCodecState = {0xA4, EBML_BINARY_CLASS, 0, 0, "CodecState", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSlices = {0x8E, EBML_MASTER_CLASS, 0, 0, "ClusterSlices", EBML_SemanticClusterSlices, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterReferenceFrame = {0xC8, EBML_MASTER_CLASS, 0, 0, "ReferenceFrame", EBML_SemanticClusterReferenceFrame, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticClusterBlockGroup[] = {
    {1, 1, &MATROSKA_ContextClusterBlock             ,0},
    {0, 1, &MATROSKA_ContextClusterBlockDuration     ,0},
    {1, 1, &MATROSKA_ContextClusterReferencePriority ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextClusterReferenceBlock    ,0},
    {0, 1, &MATROSKA_ContextClusterCodecState        ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextClusterSlices            ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextClusterBlockAdditions    ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    // DivX trick track extenstions http://developer.divx.com/docs/divx_plus_hd/format_features/Smooth_FF_RW
    {0, 1, &MATROSKA_ContextClusterReferenceFrame    ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterSilentTrackNumber = {0x58D7, EBML_INTEGER_CLASS, 0, 0, "ClusterSilentTrackNumber", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticClusterSilentTracks[] = {
    {0, 0, &MATROSKA_ContextClusterSilentTrackNumber ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextClusterTimecode = {0xE7, EBML_INTEGER_CLASS, 0, 0, "ClusterTimecode", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSilentTracks = {0x5854, EBML_MASTER_CLASS, 0, 0, "ClusterSilentTracks", EBML_SemanticClusterSilentTracks, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterPosition = {0xA7, EBML_INTEGER_CLASS, 0, 0, "ClusterPosition", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterPrevSize = {0xAB, EBML_INTEGER_CLASS, 0, 0, "ClusterPrevSize", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterBlockGroup = {0xA0, MATROSKA_BLOCKGROUP_CLASS, 0, 0, "ClusterBlockGroup", EBML_SemanticClusterBlockGroup, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextClusterSimpleBlock = {0xA3, MATROSKA_BLOCK_CLASS, 0, 0, "ClusterSimpleBlock", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCluster[] = {
    {1, 1, &MATROSKA_ContextClusterTimecode     ,0},
    {0, 1, &MATROSKA_ContextClusterPosition     ,0},
    {0, 1, &MATROSKA_ContextClusterPrevSize     ,0},
    {0, 0, &MATROSKA_ContextClusterBlockGroup   ,0},
    {0, 0, &MATROSKA_ContextClusterSimpleBlock  ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1},
    {0, 1, &MATROSKA_ContextClusterSilentTracks ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextCluster = {0x1F43B675, MATROSKA_CLUSTER_CLASS, 0, 0, "Cluster", EBML_SemanticCluster, EBML_SemanticGlobals, NULL};

// Tracks
const ebml_context MATROSKA_ContextTrackAudioSamplingFreq = {0xB5, EBML_FLOAT_CLASS, 1, (intptr_t)8000.0, "TrackAudioSamplingFreq", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackAudioOutputSamplingFreq = {0x78B5, EBML_FLOAT_CLASS, 0, 0, "TrackAudioOutputSamplingFreq", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackAudioChannels = {0x9F, EBML_INTEGER_CLASS, 1, (intptr_t)1, "TrackAudioChannels", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackAudioBitDepth = {0x6264, EBML_INTEGER_CLASS, 0, 0, "TrackAudioBitDepth", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackAudio[] = {
    {1, 1, &MATROSKA_ContextTrackAudioSamplingFreq       ,0},
    {0, 1, &MATROSKA_ContextTrackAudioOutputSamplingFreq ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTrackAudioChannels           ,0},
    {0, 1, &MATROSKA_ContextTrackAudioBitDepth           ,0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextTrackVideoInterlaced = {0x9A, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "TrackVideoInterlaced", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoStereo = {0x53B8, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoStereo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoPixelWidth = {0xB0, EBML_INTEGER_CLASS, 0, 0, "TrackVideoPixelWidth", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoPixelHeight = {0xBA, EBML_INTEGER_CLASS, 0, 0, "TrackVideoPixelHeight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoPixelCropBottom = {0x54AA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropBottom", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoPixelCropTop = {0x54BB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropTop", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoPixelCropLeft = {0x54CC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropLeft", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoPixelCropRight = {0x54DD, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropRight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoDisplayWidth = {0x54B0, EBML_INTEGER_CLASS, 0, 0, "TrackVideoDisplayWidth", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoDisplayHeight = {0x54BA, EBML_INTEGER_CLASS, 0, 0, "TrackVideoDisplayHeight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoDisplayUnit = {0x54B2, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoDisplayUnit", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoAspectRatio = {0x54B3, EBML_INTEGER_CLASS, 1, (intptr_t)MATROSKA_DISPLAY_UNIT_PIXEL, "TrackVideoAspectRatio", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoColourSpace = {0x2EB524, EBML_BINARY_CLASS, 0, 0, "TrackVideoColourSpace", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoFrameRate = {0x2383E3, EBML_FLOAT_CLASS, 0, 0, "TrackVideoFrameRate", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackVideoGammaValue = {0x2FB523, EBML_FLOAT_CLASS, 0, 0, "TrackVideoGammaValue", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackVideo[] = {
    {1, 1, &MATROSKA_ContextTrackVideoInterlaced      ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1},
    {1, 1, &MATROSKA_ContextTrackVideoPixelWidth      ,0},
    {1, 1, &MATROSKA_ContextTrackVideoPixelHeight     ,0},
    {0, 1, &MATROSKA_ContextTrackVideoPixelCropBottom ,0},
    {0, 1, &MATROSKA_ContextTrackVideoPixelCropTop    ,0},
    {0, 1, &MATROSKA_ContextTrackVideoPixelCropLeft   ,0},
    {0, 1, &MATROSKA_ContextTrackVideoPixelCropRight  ,0},
    {0, 1, &MATROSKA_ContextTrackVideoDisplayWidth    ,0},
    {0, 1, &MATROSKA_ContextTrackVideoDisplayHeight   ,0},
    {0, 1, &MATROSKA_ContextTrackVideoDisplayUnit     ,0},
    {0, 1, &MATROSKA_ContextTrackVideoAspectRatio     ,0},
    {0, 1, &MATROSKA_ContextTrackVideoColourSpace     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackVideoStereo          ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackVideoFrameRate       ,0},
    {0, 1, &MATROSKA_ContextTrackVideoGammaValue      ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2|PROFILE_DIVX_V1|PROFILE_DIVX_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextTrackEncodingCompressionAlgo = {0x4254, EBML_INTEGER_CLASS, 1, (intptr_t)MATROSKA_BLOCK_COMPR_ZLIB, "TrackEncodingCompressionAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingCompressionSetting = {0x4255, EBML_BINARY_CLASS, 0, 0, "TrackEncodingCompressionSetting", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncodingCompression[] = {
    {1, 1, &MATROSKA_ContextTrackEncodingCompressionAlgo    ,0},
    {0, 1, &MATROSKA_ContextTrackEncodingCompressionSetting ,0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextTrackEncodingEncryptionAlgo = {0x47E1, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingEncryptionKeyID = {0x47E2, EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionKeyID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingEncryptionSignature = {0x47E3, EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionSignature", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingEncryptionSignatureKeyID = {0x47E4, EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionSignatureKeyID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingEncryptionSignatureAlgo = {0x47E5, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionSignatureAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingEncryptionSignatureHashAlgo = {0x47E6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionSignatureHashAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncodingEncryption[] = {
    {0, 1, &MATROSKA_ContextTrackEncodingEncryptionAlgo              ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackEncodingEncryptionKeyID             ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackEncodingEncryptionSignature         ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackEncodingEncryptionSignatureKeyID    ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackEncodingEncryptionSignatureAlgo     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackEncodingEncryptionSignatureHashAlgo ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextTrackEncodingOrder = {0x5031, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingOrder", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingScope = {0x5032, EBML_INTEGER_CLASS, 1, (intptr_t)1, "TrackEncodingScope", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingType = {0x5033, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingCompression = {0x5034, EBML_MASTER_CLASS, 0, 0, "TrackEncodingCompression", EBML_SemanticTrackEncodingCompression, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackEncodingEncryption = {0x5035, EBML_MASTER_CLASS, 0, 0, "TrackEncodingEncryption", EBML_SemanticTrackEncodingEncryption, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncoding[] = {
    {1, 1, &MATROSKA_ContextTrackEncodingOrder       ,0},
    {1, 1, &MATROSKA_ContextTrackEncodingScope       ,0},
    {1, 1, &MATROSKA_ContextTrackEncodingType        ,0},
    {0, 1, &MATROSKA_ContextTrackEncodingCompression ,0},
    {0, 1, &MATROSKA_ContextTrackEncodingEncryption  ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextTrackEncoding = {0x6240, EBML_MASTER_CLASS, 0, 0, "TrackEncoding", EBML_SemanticTrackEncoding, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackEncodings[] = {
    {1, 0, &MATROSKA_ContextTrackEncoding ,0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_context MATROSKA_ContextTrackTranslateEditionUID = {0x66FC, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTranslateCodec = {0x66BF, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateCodec", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTranslateID = {0x66A5, EBML_BINARY_CLASS, 0, 0, "TrackTranslateID", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTrackTranslate[] = {
    {0, 0, &MATROSKA_ContextTrackTranslateEditionUID ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTrackTranslateCodec      ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTrackTranslateID         ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
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

// DivX trick track extenstions http://developer.divx.com/docs/divx_plus_hd/format_features/Smooth_FF_RW
const ebml_context MATROSKA_ContextTrickTrackUID = {0xC0, EBML_INTEGER_CLASS, 0, 0, "TrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickTrackSegUID = {0xC1, EBML_BINARY_CLASS, 0, 0, "TrackSegUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickTrackFlag = {0xC6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackFlag", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickMasterTrackUID = {0xC7, EBML_INTEGER_CLASS, 0, 0, "MasterTrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickMasterTrackSegUID = {0xC4, EBML_BINARY_CLASS, 0, 0, "MasterTrackSegUID", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackEntry[] = {
    {1, 1, &MATROSKA_ContextTrackNumber             ,0},
    {1, 1, &MATROSKA_ContextTrackUID                ,0},
    {1, 1, &MATROSKA_ContextTrackType               ,0},
    {1, 1, &MATROSKA_ContextTrackCodecID            ,0},
    {1, 1, &MATROSKA_ContextTrackEnabled            ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1},
    {1, 1, &MATROSKA_ContextTrackDefault            ,0},
    {1, 0, &MATROSKA_ContextTrackForced             ,0},
    {0, 1, &MATROSKA_ContextTrackVideo              ,0},
    {0, 1, &MATROSKA_ContextTrackAudio              ,0},
    {1, 1, &MATROSKA_ContextTrackLacing             ,0},
    {1, 1, &MATROSKA_ContextTrackMinCache           ,0},
    {0, 1, &MATROSKA_ContextTrackMaxCache           ,0},
    {0, 1, &MATROSKA_ContextTrackDefaultDuration    ,0},
    {1, 1, &MATROSKA_ContextTrackTimecodeScale      ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTrackMaxBlockAdditionID ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackName               ,0},
    {0, 1, &MATROSKA_ContextTrackLanguage           ,0},
    {0, 1, &MATROSKA_ContextTrackCodecPrivate       ,0},
    {0, 1, &MATROSKA_ContextTrackCodecName          ,0},
    {0, 1, &MATROSKA_ContextTrackAttachmentLink     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextTrackOverlay            ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrackEncodings          ,0},
    {0, 0, &MATROSKA_ContextTrackTranslate          ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTrackCodecDecodeAll     ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1},
    // DivX trick track extenstions http://developer.divx.com/docs/divx_plus_hd/format_features/Smooth_FF_RW
    {0, 1, &MATROSKA_ContextTrickTrackUID           ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrickTrackSegUID        ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrickTrackFlag          ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrickMasterTrackUID     ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTrickMasterTrackSegUID  ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
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
    {1, 1, &MATROSKA_ContextRefTime ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1},
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
    {0, 1, &MATROSKA_ContextCueCodecState      ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1},
    {0, 1, &MATROSKA_ContextCueReference       ,PROFILE_MATROSKA_V1|PROFILE_DIVX_V1|PROFILE_WEBM_V1},
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
const ebml_context MATROSKA_ContextAttachedFileData = {0x465C, MATROSKA_BIGBINARY_CLASS, 0, 0, "AttachedFileData", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAttachedFileUID = {0x46AE, EBML_INTEGER_CLASS, 0, 0, "AttachedFileUID", NULL, EBML_SemanticGlobals, NULL};
// DivX extensions http://developer.divx.com/docs/divx_plus_hd/format_features/World_Fonts
const ebml_context MATROSKA_ContextAttachedFileUsedStartTime = {0x4661, EBML_INTEGER_CLASS, 0, 0, "AttachedFileUsedStartTime", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAttachedFileUsedEndTime = {0x4662, EBML_INTEGER_CLASS, 0, 0, "AttachedFileUsedEndTime", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticAttachedFile[] = {
    {1, 1, &MATROSKA_ContextAttachedFileName          ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextAttachedFileMimeType      ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextAttachedFileData          ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextAttachedFileUID           ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextAttachedFileDescription   ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    // DivX extensions http://developer.divx.com/docs/divx_plus_hd/format_features/World_Fonts
    {0, 1, &MATROSKA_ContextAttachedFileUsedStartTime ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextAttachedFileUsedEndTime   ,PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextAttachedFile = {0x61A7, EBML_MASTER_CLASS, 0, 0, "AttachedFile", EBML_SemanticAttachedFile, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticAttachments[] = {
    {1, 0, &MATROSKA_ContextAttachedFile ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
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
    {0, 1, &MATROSKA_ContextTagTargetTypeValue     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTagTargetType          ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextTagTargetTrackUID      ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextTagTargetEditionUID    ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextTagTargetChapterUID    ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextTagTargetAttachmentUID ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTagTargets = {0x63C0, EBML_MASTER_CLASS, 0, 0, "TagTargets", EBML_SemanticTagTargets, EBML_SemanticGlobals, NULL};

const ebml_context MATROSKA_ContextTagName = {0x45A3, EBML_UNISTRING_CLASS, 0, 0, "TagName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagLanguage = {0x447A, EBML_STRING_CLASS, 1, (intptr_t)"und", "TagLanguage", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagDefault = {0x4484, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TagDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagString = {0x4487, EBML_UNISTRING_CLASS, 0, 0, "TagString", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagBinary = {0x4485, EBML_BINARY_CLASS, 0, 0, "TagBinary", NULL, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticSimpleTag[] = {
    {1, 1, &MATROSKA_ContextTagName     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTagLanguage ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 1, &MATROSKA_ContextTagDefault  ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTagString   ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextTagBinary   ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextSimpleTag   ,PROFILE_WEBM_V1|PROFILE_WEBM_V2}, // recursive
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSimpleTag = {0x67C8, EBML_MASTER_CLASS, 0, 0, "SimpleTag", EBML_SemanticSimpleTag, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTag[] = {
    {1, 1, &MATROSKA_ContextTagTargets ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {1, 0, &MATROSKA_ContextSimpleTag  ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTag = {0x7373, EBML_MASTER_CLASS, 0, 0, "Tag", EBML_SemanticTag, EBML_SemanticGlobals, NULL};
const ebml_semantic EBML_SemanticTags[] = {
    {1, 0, &MATROSKA_ContextTag ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
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
    {0, 1, &MATROSKA_ContextAttachments  ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 1, &MATROSKA_ContextChapters     ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
    {0, 0, &MATROSKA_ContextTags         ,PROFILE_WEBM_V1|PROFILE_WEBM_V2},
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
    ebml_element Base;
    ebml_element *SegInfo;
    matroska_block *Block;

};

struct matroska_cluster
{
    ebml_element Base;
    ebml_element *ReadSegInfo;
    ebml_element *WriteSegInfo;
    timecode_t GlobalTimecode;
};

struct matroska_seekpoint
{
    ebml_element Base;
    ebml_element *Link;
};

static err_t CheckCompression(matroska_block *Block)
{
    ebml_element *Elt, *Header;
    assert(Block->ReadTrack!=NULL);
    Elt = EBML_MasterFindFirstElt(Block->ReadTrack, &MATROSKA_ContextTrackEncodings, 0, 0);
    if (Elt)
    {
        if (ARRAYCOUNT(Block->Data,uint8_t))
            return ERR_INVALID_PARAM; // we cannot adjust sizes if the data are already read

        Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncoding, 0, 0);
        if (EBML_MasterChildren(Elt))
        {
            if (EBML_MasterNext(Elt))
                return ERR_INVALID_DATA; // TODO support cascaded compression/encryption

            Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompression, 0, 0);
            if (!Elt)
                return ERR_INVALID_DATA; // TODO: support encryption

            Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionAlgo, 1, 1);
#if defined(CONFIG_ZLIB)
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                return ERR_INVALID_DATA;

            if (EBML_IntegerValue(Header)==MATROSKA_BLOCK_COMPR_HEADER)
            {
                Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionSetting, 0, 0);
                if (Header)
                {
                    uint32_t *i;
        		    for (i=ARRAYBEGIN(Block->SizeList,uint32_t);i!=ARRAYEND(Block->SizeList,uint32_t);++i)
                        *i += (uint32_t)Header->DataSize;
                }
            }
        }
    }
    return ERR_NONE;
}

err_t MATROSKA_LinkBlockWithReadTracks(matroska_block *Block, ebml_element *Tracks, bool_t UseForWriteToo)
{
    ebml_element *Track, *TrackNum;
    bool_t WasLinked = Block->ReadTrack!=NULL;

    assert(Tracks->Context->Id == MATROSKA_ContextTracks.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    for (Track=EBML_MasterChildren(Tracks);Track;Track=EBML_MasterNext(Track))
    {
        TrackNum = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
        if (TrackNum && ((ebml_integer*)TrackNum)->Base.bValueIsSet && EBML_IntegerValue(TrackNum)==Block->TrackNumber)
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

err_t MATROSKA_LinkBlockReadTrack(matroska_block *Block, ebml_element *Track, bool_t UseForWriteToo)
{
    ebml_element *TrackNum;
    bool_t WasLinked = Block->ReadTrack!=NULL;

    assert(Track->Context->Id == MATROSKA_ContextTrackEntry.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    TrackNum = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
    if (TrackNum && TrackNum->bValueIsSet)
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
err_t MATROSKA_LinkBlockWithWriteTracks(matroska_block *Block, ebml_element *Tracks)
{
    ebml_element *Track, *TrackNum;
    bool_t WasLinked = Block->WriteTrack!=NULL;

    assert(Tracks->Context->Id == MATROSKA_ContextTracks.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    for (Track=EBML_MasterChildren(Tracks);Track;Track=EBML_MasterNext(Track))
    {
        TrackNum = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
        if (TrackNum && ((ebml_integer*)TrackNum)->Base.bValueIsSet && EBML_IntegerValue(TrackNum)==Block->TrackNumber)
        {
            Node_SET(Block,MATROSKA_BLOCK_WRITE_TRACK,&Track);
            if (WasLinked)
                return ERR_NONE;
            return CheckCompression(Block);
        }
    }
    return ERR_INVALID_DATA;
}

err_t MATROSKA_LinkBlockWriteTrack(matroska_block *Block, ebml_element *Track)
{
    ebml_element *TrackNum;
    bool_t WasLinked = Block->WriteTrack!=NULL;

    assert(Track->Context->Id == MATROSKA_ContextTrackEntry.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    TrackNum = EBML_MasterFindFirstElt(Track,&MATROSKA_ContextTrackNumber,0,0);
    if (TrackNum && TrackNum->bValueIsSet)
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

    if (Node_IsPartOf(MetaSeek,EBML_VOID_CLASS))
        return ERR_NONE;

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
    EBML_IntegerSetValue((ebml_integer*)WSeekPosSegmentInfo, Link->ElementPosition - EBML_ElementPositionData(RSegment));

    return Err;
}

err_t MATROSKA_LinkClusterReadSegmentInfo(matroska_cluster *Cluster, ebml_element *SegmentInfo, bool_t UseForWriteToo)
{
    assert(Cluster->Base.Context->Id == MATROSKA_ContextCluster.Id);
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
    Node_SET(Cluster,MATROSKA_CLUSTER_READ_SEGMENTINFO,&SegmentInfo);
    if (UseForWriteToo)
        Node_SET(Cluster,MATROSKA_CLUSTER_WRITE_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}

#if defined(CONFIG_EBML_WRITING)
err_t MATROSKA_LinkClusterWriteSegmentInfo(matroska_cluster *Cluster, ebml_element *SegmentInfo)
{
    assert(Cluster->Base.Context->Id == MATROSKA_ContextCluster.Id);
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
    Node_SET(Cluster,MATROSKA_CLUSTER_WRITE_SEGMENTINFO,&SegmentInfo);
    return ERR_NONE;
}
#endif

err_t MATROSKA_LinkBlockReadSegmentInfo(matroska_block *Block, ebml_element *SegmentInfo, bool_t UseForWriteToo)
{
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    Node_SET(Block,MATROSKA_BLOCK_READ_SEGMENTINFO,&SegmentInfo);
#if defined(CONFIG_EBML_WRITING)
    if (UseForWriteToo)
        Node_SET(Block,MATROSKA_BLOCK_WRITE_SEGMENTINFO,&SegmentInfo);
#endif
    return ERR_NONE;
}

#if defined(CONFIG_EBML_WRITING)
err_t MATROSKA_LinkBlockWriteSegmentInfo(matroska_block *Block, ebml_element *SegmentInfo)
{
    assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
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
    if ((*a)->Context->Id == MATROSKA_ContextClusterTimecode.Id)
        return -1;
    if ((*b)->Context->Id == MATROSKA_ContextClusterTimecode.Id)
        return 1;

    if ((*a)->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
        BlockA = (const matroska_block *)*a;
    else if ((*a)->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
        BlockA = (const matroska_block *)EBML_MasterFindFirstElt((ebml_element*)*a,&MATROSKA_ContextClusterBlock,0,0);
    if ((*b)->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
        BlockB = (const matroska_block *)*b;
    else if ((*a)->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
        BlockB = (const matroska_block *)EBML_MasterFindFirstElt((ebml_element*)*b,&MATROSKA_ContextClusterBlock,0,0);
    if (BlockA != NULL && BlockB != NULL)
        return MATROSKA_BlockCmp(BlockA,BlockB);

    assert(0); // unsupported comparison
    return 0;
}

void MATROSKA_ClusterSort(matroska_cluster *Cluster)
{
    EBML_MasterSort((ebml_element*)Cluster,(arraycmp)ClusterEltCmp,Cluster);
}

void MATROSKA_ClusterSetTimecode(matroska_cluster *Cluster, timecode_t Timecode)
{
	ebml_element *TimecodeElt;
    assert(Cluster->Base.Context->Id == MATROSKA_ContextCluster.Id);
    Cluster->GlobalTimecode = Timecode;
    TimecodeElt = EBML_MasterFindFirstElt((ebml_element*)Cluster,&MATROSKA_ContextClusterTimecode,1,1);
#if defined(CONFIG_EBML_WRITING)
	assert(Cluster->WriteSegInfo);
	EBML_IntegerSetValue((ebml_integer*)TimecodeElt, Scale64(Timecode,1,MATROSKA_SegmentInfoTimecodeScale(Cluster->WriteSegInfo)));
#else
	assert(Cluster->ReadSegInfo);
	EBML_IntegerSetValue((ebml_integer*)TimecodeElt, Scale64(Timecode,1,MATROSKA_SegmentInfoTimecodeScale(Cluster->ReadSegInfo)));
#endif
}

timecode_t MATROSKA_ClusterTimecode(matroska_cluster *Cluster)
{
    assert(Cluster->Base.Context->Id == MATROSKA_ContextCluster.Id);
    if (Cluster->GlobalTimecode == INVALID_TIMECODE_T)
    {
        ebml_element *Timecode = EBML_MasterFindFirstElt((ebml_element*)Cluster,&MATROSKA_ContextClusterTimecode,0,0);
        if (Timecode)
            Cluster->GlobalTimecode = EBML_IntegerValue(Timecode) * MATROSKA_SegmentInfoTimecodeScale(Cluster->ReadSegInfo);
    }
    return Cluster->GlobalTimecode;
}

err_t MATROSKA_BlockSetTimecode(matroska_block *Block, timecode_t Timecode, timecode_t Relative)
{
	int64_t InternalTimecode;
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
    assert(Timecode!=INVALID_TIMECODE_T);
#if defined(CONFIG_EBML_WRITING)
	InternalTimecode = Scale64(Timecode - Relative,1,(int64_t)(MATROSKA_SegmentInfoTimecodeScale(Block->WriteSegInfo) * MATROSKA_TrackTimecodeScale(Block->WriteTrack)));
#else
	InternalTimecode = Scale64(Timecode - Relative,1,(int64_t)(MATROSKA_SegmentInfoTimecodeScale(Block->ReadSegInfo) * MATROSKA_TrackTimecodeScale(Block->ReadTrack)));
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
    assert(Block->LocalTimecodeUsed);
    Cluster = EBML_ElementParent(Block);
    while (Cluster && Cluster->Context->Id != MATROSKA_ContextCluster.Id)
        Cluster = EBML_ElementParent(Cluster);
    if (!Cluster)
        return INVALID_TIMECODE_T;
    Block->GlobalTimecode = MATROSKA_ClusterTimecode((matroska_cluster*)Cluster) + (timecode_t)(Block->LocalTimecode * MATROSKA_SegmentInfoTimecodeScale(Block->ReadSegInfo) * MATROSKA_TrackTimecodeScale(Block->ReadTrack));
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
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	if (Block->Base.Base.Context->Id == MATROSKA_ContextClusterBlock.Id)
	{
		ebml_element *BlockGroup = EBML_ElementParent(Block);
		if (BlockGroup && Node_IsPartOf(BlockGroup,MATROSKA_BLOCKGROUP_CLASS))
			return (EBML_MasterFindFirstElt(BlockGroup,&MATROSKA_ContextClusterReferenceBlock,0,0)==NULL);
	}
	return Block->IsKeyframe;
}

bool_t MATROSKA_BlockDiscardable(const matroska_block *Block)
{
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	if (Block->Base.Base.Context->Id == MATROSKA_ContextClusterBlock.Id)
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
	if (Block->Base.Base.Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
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
    ebml_element *Position, *CueTrack;
    assert(Cue->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    Position = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTrackPositions,0,0);
    if (!Position)
        return -1;
    CueTrack = EBML_MasterFindFirstElt(Position,&MATROSKA_ContextCueTrack,0,0);
    if (!CueTrack)
        return -1;
    return (int16_t)EBML_IntegerValue(CueTrack);
}

void MATROSKA_CuesSort(ebml_element *Cues)
{
    assert(Cues->Context->Id == MATROSKA_ContextCues.Id);
    EBML_MasterSort(Cues,NULL,NULL);
}

timecode_t MATROSKA_SegmentInfoTimecodeScale(const ebml_element *SegmentInfo)
{
    ebml_element *TimecodeScale = NULL;
    if (SegmentInfo)
    {
        assert(SegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
        TimecodeScale = EBML_MasterFindFirstElt((ebml_element*)SegmentInfo,&MATROSKA_ContextTimecodeScale,0,0);
    }
    if (!TimecodeScale)
        return MATROSKA_ContextTimecodeScale.DefaultValue;
    return EBML_IntegerValue(TimecodeScale);
}

double MATROSKA_TrackTimecodeScale(const ebml_element *Track)
{
    ebml_element *TimecodeScale;
    assert(Track->Context->Id == MATROSKA_ContextTrackEntry.Id);
    TimecodeScale = EBML_MasterFindFirstElt((ebml_element*)Track,&MATROSKA_ContextTrackTimecodeScale,0,0);
    if (!TimecodeScale)
        return MATROSKA_ContextTrackTimecodeScale.DefaultValue;
    return ((ebml_float*)TimecodeScale)->Value;
}

timecode_t MATROSKA_CueTimecode(const matroska_cuepoint *Cue)
{
    ebml_element *TimeCode;
    assert(Cue->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    TimeCode = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTime,0,0);
    if (!TimeCode)
        return INVALID_TIMECODE_T;
    return EBML_IntegerValue(TimeCode) * MATROSKA_SegmentInfoTimecodeScale(Cue->SegInfo);
}

filepos_t MATROSKA_CuePosInSegment(const matroska_cuepoint *Cue)
{
    ebml_element *TimeCode;
    assert(Cue->Base.Context->Id == MATROSKA_ContextCuePoint.Id);
    TimeCode = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTrackPositions,0,0);
    if (!TimeCode)
        return INVALID_TIMECODE_T;
    TimeCode = EBML_MasterFindFirstElt((ebml_element*)TimeCode,&MATROSKA_ContextCueClusterPosition,0,0);
    if (!TimeCode)
        return INVALID_TIMECODE_T;
    return EBML_IntegerValue(TimeCode);
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
    EBML_IntegerSetValue((ebml_integer*)TimeCode, Scale64(MATROSKA_BlockTimecode(Cue->Block),1,MATROSKA_SegmentInfoTimecodeScale(Cue->SegInfo)));

    Elt = EBML_MasterFindFirstElt((ebml_element*)Cue,&MATROSKA_ContextCueTrackPositions,1,1);
    if (!Elt)
        return ERR_OUT_OF_MEMORY;
	TrackNum = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextCueTrack,1,1);
    if (!TrackNum)
        return ERR_OUT_OF_MEMORY;
	EBML_IntegerSetValue((ebml_integer*)TrackNum, MATROSKA_BlockTrackNum(Cue->Block));
	
    PosInCluster = EBML_MasterFindFirstElt(Elt,&MATROSKA_ContextCueClusterPosition,1,1);
    if (!PosInCluster)
        return ERR_OUT_OF_MEMORY;
    Elt = EBML_ElementParent(Cue->Block);
    while (Elt && Elt->Context->Id != MATROSKA_ContextCluster.Id)
        Elt = EBML_ElementParent(Elt);
    if (!Elt)
        return ERR_INVALID_DATA;
    
    EBML_IntegerSetValue((ebml_integer*)PosInCluster, Elt->ElementPosition - EBML_ElementPositionData(Segment));

    return ERR_NONE;
}

matroska_block *MATROSKA_GetBlockForTimecode(matroska_cluster *Cluster, timecode_t Timecode, int16_t Track)
{
    ebml_element *Block, *GBlock;
    for (Block = EBML_MasterChildren(Cluster);Block;Block=EBML_MasterNext(Block))
    {
        if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
        {
            for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
            {
                if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
                {
                    if (MATROSKA_BlockTrackNum((matroska_block*)GBlock) == Track &&
                        MATROSKA_BlockTimecode((matroska_block*)GBlock) == Timecode)
                    {
                        return (matroska_block*)GBlock;
                    }
                }
            }
        }
        else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
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

void MATROSKA_LinkClusterBlocks(matroska_cluster *Cluster, ebml_element *RSegmentInfo, ebml_element *Tracks, bool_t KeepUnmatched)
{
    ebml_element *Block, *GBlock,*NextBlock;

	assert(Node_IsPartOf(Cluster,MATROSKA_CLUSTER_CLASS));
	assert(RSegmentInfo->Context->Id == MATROSKA_ContextSegmentInfo.Id);
	assert(Tracks->Context->Id == MATROSKA_ContextTracks.Id);

	// link each Block/SimpleBlock with its Track and SegmentInfo
	MATROSKA_LinkClusterReadSegmentInfo(Cluster,RSegmentInfo,1);
	for (Block = EBML_MasterChildren(Cluster);Block;Block=NextBlock)
	{
        NextBlock = EBML_MasterNext(Block);
		if (Block->Context->Id == MATROSKA_ContextClusterBlockGroup.Id)
		{
			for (GBlock = EBML_MasterChildren(Block);GBlock;GBlock=EBML_MasterNext(GBlock))
			{
				if (GBlock->Context->Id == MATROSKA_ContextClusterBlock.Id)
				{
					if (MATROSKA_LinkBlockWithReadTracks((matroska_block*)GBlock,Tracks,1)!=ERR_NONE && !KeepUnmatched)
                        NodeDelete((node*)Block);
                    else
					    MATROSKA_LinkBlockReadSegmentInfo((matroska_block*)GBlock,RSegmentInfo,1);
					break;
				}
			}
		}
		else if (Block->Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
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

err_t MATROSKA_BlockReleaseData(matroska_block *Block)
{
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
    ebml_element *Elt, *Header = NULL;
    uint8_t *InBuf;

    if (!Element->Base.Base.bValueIsSet)
    {
        // find out if compressed headers are used
        assert(Element->ReadTrack!=NULL);
        Elt = EBML_MasterFindFirstElt(Element->ReadTrack, &MATROSKA_ContextTrackEncodings, 0, 0);
        if (Elt)
        {
            Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncoding, 0, 0);
            if (EBML_MasterChildren(Elt))
            {
                if (EBML_MasterNext(Elt))
                    return ERR_NOT_SUPPORTED; // TODO support cascaded compression/encryption

                Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompression, 0, 0);
                if (!Elt)
                    return ERR_NOT_SUPPORTED; // TODO: support encryption

                Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionAlgo, 1, 1);
#if defined(CONFIG_ZLIB)
                if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
                if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                    return ERR_INVALID_DATA;

                if (EBML_IntegerValue(Header)==MATROSKA_BLOCK_COMPR_HEADER)
                    Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionSetting, 0, 0);
            }
        }

#if !defined(CONFIG_ZLIB)
        if (Header && Header->Context==&MATROSKA_ContextTrackEncodingCompressionAlgo)
            return ERR_NOT_SUPPORTED;
#endif

        Stream_Seek(Input,Element->FirstFrameLocation,SEEK_SET);
        switch (Element->Lacing)
        {
        case LACING_NONE:
#if defined(CONFIG_ZLIB)
            if (Header && Header->Context==&MATROSKA_ContextTrackEncodingCompressionAlgo)
            {
                // zlib handling, read the buffer in temp memory
                array TmpBuf;
                ArrayInit(&TmpBuf);
                if (!ArrayResize(&TmpBuf,(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0],0))
                    Err = ERR_OUT_OF_MEMORY;
                InBuf = ARRAYBEGIN(TmpBuf,uint8_t);
                ArrayCopy(&Element->SizeListIn, &Element->SizeList);
                Err = Stream_Read(Input,InBuf,(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0],&Read);
                if (Err==ERR_NONE)
                {
                    if (Read!=(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0])
                        Err = ERR_READ;
                    else
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
                }
                ArrayClear(&TmpBuf);
            }
            else
#endif
            {
                ArrayResize(&Element->Data,(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0] + (Header?(size_t)Header->DataSize:0),0);
                InBuf = ARRAYBEGIN(Element->Data,uint8_t);
                if (Header)
                {
                    memcpy(InBuf,ARRAYBEGIN(((ebml_binary*)Header)->Data,uint8_t),(size_t)Header->DataSize);
                    InBuf += (size_t)Header->DataSize;
                }
                Err = Stream_Read(Input,InBuf,(size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0],&Read);
                if (Err != ERR_NONE)
                    goto failed;
                if (Read != (size_t)ARRAYBEGIN(Element->SizeList,int32_t)[0])
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
#if defined(CONFIG_ZLIB)
            if (Header && Header->Context==&MATROSKA_ContextTrackEncodingCompressionAlgo)
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
                ArrayCopy(&Element->SizeListIn, &Element->SizeList);
                for (NumFrame=0;Err==ERR_NONE && NumFrame<ARRAYCOUNT(Element->SizeList,int32_t);++NumFrame)
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
                    InBuf += FrameSize;
                }
                ArrayResize(&Element->Data, OutSize, 0); // shrink the buffer
                ArrayClear(&TmpBuf);
            }
            else
#endif
            {
                ArrayResize(&Element->Data,BufSize,0);
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

static err_t SetBlockGroupParent(ebml_element *Element, void* Parent, void* Before)
{
	// update the timecode
	err_t Result = ERR_NONE;
	matroska_block *Block = (matroska_block*)EBML_MasterFindFirstElt(Element, &MATROSKA_ContextClusterBlock, 0, 0);
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

static err_t ReadBigBinaryData(ebml_binary *Element, stream *Input, const ebml_parser_context *ParserContext, bool_t AllowDummyElt, int Scope)
{
    if (Scope == SCOPE_PARTIAL_DATA)
    {
        EBML_ElementSkipData((ebml_element*)Element,Input,ParserContext,NULL,AllowDummyElt);
        return ERR_NONE;
    }
    return INHERITED(Element,ebml_element_vmt,MATROSKA_BIGBINARY_CLASS)->ReadData(Element, Input, ParserContext, AllowDummyElt, Scope);
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

	if (Element->Base.Base.Context->Id == MATROSKA_ContextClusterSimpleBlock.Id)
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

err_t MATROSKA_BlockAppendFrame(matroska_block *Block, const matroska_frame *Frame, timecode_t Relative)
{
    if (!Block->Base.Base.bValueIsSet && Frame->Timecode!=INVALID_TIMECODE_T)
        MATROSKA_BlockSetTimecode(Block,Frame->Timecode,Relative);
    ArrayAppend(&Block->Data,Frame->Data,Frame->Size,0);
    ArrayAppend(&Block->Durations,&Frame->Duration,sizeof(Frame->Duration),0);
    ArrayAppend(&Block->SizeList,&Frame->Size,sizeof(Frame->Size),0);
    Block->Base.Base.bValueIsSet = 1;
    Block->Lacing = LACING_AUTO;
    return ERR_NONE;
}

#if defined(CONFIG_EBML_WRITING) && defined(CONFIG_ZLIB)
static err_t CompressFrameZLib(const uint8_t *Cursor, size_t CursorSize, uint8_t **OutBuf, size_t *OutSize)
{
    err_t Err = ERR_NONE;
    z_stream stream;
    size_t Count;
    array TmpBuf;
    int Res;

    ArrayInit(&TmpBuf);
    memset(&stream,0,sizeof(stream));
    stream.next_in = (Bytef*)Cursor;
    stream.avail_in = CursorSize;
    Count = 0;
    stream.next_out = ARRAYBEGIN(TmpBuf,uint8_t);
    deflateInit(&stream, 9);
    do {
        Count = stream.next_out - ARRAYBEGIN(TmpBuf,uint8_t);
        stream.avail_out = CursorSize; // add some bytes to the output
        if (!ArrayResize(&TmpBuf,stream.avail_out + Count,0))
        {
            ArrayClear(&TmpBuf);
            Err = ERR_OUT_OF_MEMORY;
            break;
        }
        stream.next_out = ARRAYBEGIN(TmpBuf,uint8_t) + Count;
        Res = deflate(&stream, Z_FINISH);
    } while (stream.avail_out==0 && Res!=Z_STREAM_END);

    if (OutBuf && OutSize)
    {
        // TODO: write directly in the output buffer
        memcpy(*OutBuf, ARRAYBEGIN(TmpBuf,uint8_t), min(*OutSize, stream.total_out));
        *OutSize = stream.total_out;
    }
    else if (OutSize)
        *OutSize = stream.total_out;
    ArrayClear(&TmpBuf);

    deflateEnd(&stream);

    return Err;
}
#endif

static filepos_t GetBlockFrameSize(const matroska_block *Element, size_t Frame, const ebml_element *Header)
{
    if (Frame >= ARRAYCOUNT(Element->SizeList,int32_t))
        return 0;

    if (!Header)
        return ARRAYBEGIN(Element->SizeList,int32_t)[Frame];
    if (Header->Context==&MATROSKA_ContextTrackEncodingCompressionAlgo)
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
    ebml_element *Elt, *Header = NULL;

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
    Elt = EBML_MasterFindFirstElt(Element->WriteTrack, &MATROSKA_ContextTrackEncodings, 0, 0);
    if (Elt)
    {
        Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncoding, 0, 0);
        if (EBML_MasterChildren(Elt))
        {
            if (EBML_MasterNext(Elt))
                return 0; // TODO support cascaded compression/encryption

            Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompression, 0, 0);
            if (!Elt)
                return 0; // TODO: support encryption

            Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionAlgo, 1, 1);
#if defined(CONFIG_ZLIB)
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                return 0; // TODO: support more than header stripping

            if (EBML_IntegerValue(Header)==MATROSKA_BLOCK_COMPR_HEADER)
                Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionSetting, 0, 0);
        }
    }

    XiphLacingSize = 0;
    for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
    {
        DataSize = (int32_t)GetBlockFrameSize(Element, i, Header);
        while (DataSize >= 0xFF)
        {
            XiphLacingSize++;
            DataSize -= 0xFF;
        }
        XiphLacingSize++;
    }

    EbmlLacingSize = EBML_CodedSizeLength(GetBlockFrameSize(Element, 0, Header),0,1);
    for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
    {
        DataSize = (int32_t)GetBlockFrameSize(Element, i, Header) - DataSize;
        EbmlLacingSize += EBML_CodedSizeLengthSigned(DataSize,0);
    }

    if (XiphLacingSize < EbmlLacingSize)
		return LACING_XIPH;
	else
		return LACING_EBML;
}

static err_t RenderBlockData(matroska_block *Element, stream *Output, bool_t bForceRender, bool_t bWithDefault, filepos_t *Rendered)
{
    err_t Err = ERR_NONE;
    uint8_t BlockHead[5], *Cursor;
    size_t ToWrite, Written, BlockHeadSize = 4;
    ebml_element *Elt, *Header = NULL;
    int32_t *i;
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

    assert(Element->WriteTrack!=NULL);
    Elt = EBML_MasterFindFirstElt(Element->WriteTrack, &MATROSKA_ContextTrackEncodings, 0, 0);
    if (Elt)
    {
        Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncoding, 0, 0);
        if (EBML_MasterChildren(Elt))
        {
            if (EBML_MasterNext(Elt))
                return ERR_INVALID_DATA; // TODO support cascaded compression/encryption

            Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompression, 0, 0);
            if (!Elt)
                return ERR_INVALID_DATA; // TODO: support encryption

            Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionAlgo, 1, 1);
#if defined(CONFIG_ZLIB)
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                return ERR_INVALID_DATA; // TODO: support more than header stripping

            if (EBML_IntegerValue(Header)==MATROSKA_BLOCK_COMPR_HEADER)
                Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionSetting, 0, 0);
        }
    }

#if !defined(CONFIG_ZLIB)
    if (Header && Header->Context==&MATROSKA_ContextTrackEncodingCompressionAlgo)
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
            DataSize = (int32_t)GetBlockFrameSize(Element, 0, Header);
            LaceSize += EBML_CodedValueLength(DataSize,EBML_CodedSizeLength(DataSize,0,1),LaceHead+LaceSize, 1);
            for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                PrevSize = DataSize;
                DataSize = (int32_t)GetBlockFrameSize(Element, i, Header);
                LaceSize += EBML_CodedValueLengthSigned(DataSize-PrevSize,EBML_CodedSizeLengthSigned(DataSize-PrevSize,0),LaceHead+LaceSize);
            }
        }
        else if (Element->Lacing == LACING_XIPH)
        {
            for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
            {
                DataSize = (int32_t)GetBlockFrameSize(Element, i, Header);
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
    ToWrite = ARRAYCOUNT(Element->Data,uint8_t);
    if (Header)
    {
        if (Header && Header->Context==&MATROSKA_ContextTrackEncodingCompressionAlgo)
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

static filepos_t UpdateBlockSize(matroska_block *Element, bool_t bWithDefault, bool_t bForceRender)
{
    ebml_element *Header = NULL;
#if defined(CONFIG_EBML_WRITING)
    ebml_element *Elt;
    if (Element->Lacing == LACING_AUTO)
        Element->Lacing = GetBestLacingType(Element);

    assert(Element->WriteTrack!=NULL);
    Elt = EBML_MasterFindFirstElt(Element->WriteTrack, &MATROSKA_ContextTrackEncodings, 0, 0);
    if (Elt)
    {
        Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncoding, 0, 0);
        if (EBML_MasterChildren(Elt))
        {
            if (EBML_MasterNext(Elt))
                return ERR_INVALID_DATA; // TODO support cascaded compression/encryption

            Elt = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompression, 0, 0);
            if (!Elt)
                return ERR_INVALID_DATA; // TODO: support encryption

            Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionAlgo, 1, 1);
#if defined(CONFIG_ZLIB)
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER && EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_ZLIB)
#else
            if (EBML_IntegerValue(Header)!=MATROSKA_BLOCK_COMPR_HEADER)
#endif
                return ERR_INVALID_DATA; // TODO: support more than header stripping

            if (EBML_IntegerValue(Header)==MATROSKA_BLOCK_COMPR_HEADER)
                Header = EBML_MasterFindFirstElt(Elt, &MATROSKA_ContextTrackEncodingCompressionSetting, 0, 0);
        }
    }
#else
    assert(Element->Lacing!=LACING_AUTO);
#endif

    if (Element->Lacing == LACING_NONE)
    {
        assert(ARRAYCOUNT(Element->SizeList,int32_t) == 1);
        Element->Base.Base.DataSize = GetBlockHeadSize(Element) + GetBlockFrameSize(Element,0,Header);
    }
    else if (Element->Lacing == LACING_EBML)
    {
        size_t i;
        filepos_t PrevSize, Size;
        filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
        Size = GetBlockFrameSize(Element,0,Header);
        Result += EBML_CodedSizeLength(Size,0,1) + Size;
        for (i=1;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
        {
            PrevSize = Size;
            Size = GetBlockFrameSize(Element,i,Header);
            Result += Size + EBML_CodedSizeLengthSigned(Size - PrevSize,0);
        }
        Result += GetBlockFrameSize(Element,i,Header);
        Element->Base.Base.DataSize = Result;
    }
    else if (Element->Lacing == LACING_XIPH)
    {
        size_t i;
        filepos_t Size;
        filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
        for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t)-1;++i)
        {
            Size = GetBlockFrameSize(Element,i,Header);
            Result += (Size / 0xFF + 1) + Size;
        }
        Result += GetBlockFrameSize(Element,i,Header);
        Element->Base.Base.DataSize = Result;
    }
    else if (Element->Lacing == LACING_FIXED)
    {
        size_t i;
        filepos_t Result = GetBlockHeadSize(Element) + 1; // 1 for the number of frames
        for (i=0;i<ARRAYCOUNT(Element->SizeList,int32_t);++i)
            Result += GetBlockFrameSize(Element,i,Header);
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

matroska_cuepoint *MATROSKA_CuesGetTimecodeStart(const ebml_element *Cues, timecode_t Timecode)
{
	matroska_cuepoint *Elt,*Prev=NULL;

	assert(Cues!=NULL);
	assert(Cues->Context->Id == MATROSKA_ContextCues.Id);
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

META_START(Matroska_Class,MATROSKA_BLOCK_CLASS)
META_CLASS(SIZE,sizeof(matroska_block))
META_CLASS(CREATE,CreateBlock)
META_VMT(TYPE_FUNC,nodetree_vmt,SetParent,SetBlockParent)
META_VMT(TYPE_FUNC,ebml_element_vmt,ReadData,ReadBlockData)
META_VMT(TYPE_FUNC,ebml_element_vmt,UpdateSize,UpdateBlockSize)
#if defined(CONFIG_EBML_WRITING)
META_VMT(TYPE_FUNC,ebml_element_vmt,RenderData,RenderBlockData)
#endif
META_VMT(TYPE_FUNC,ebml_element_vmt,Copy,CopyBlockInfo)
META_DATA(TYPE_ARRAY,0,matroska_block,SizeList)
META_DATA(TYPE_ARRAY,0,matroska_block,SizeListIn)
META_DATA(TYPE_ARRAY,0,matroska_block,Data)
META_DATA(TYPE_ARRAY,0,matroska_block,Durations)
META_PARAM(TYPE,MATROSKA_BLOCK_READ_TRACK,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_BLOCK_READ_TRACK,matroska_block,ReadTrack)
META_PARAM(TYPE,MATROSKA_BLOCK_READ_SEGMENTINFO,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_BLOCK_READ_SEGMENTINFO,matroska_block,ReadSegInfo)
#if defined(CONFIG_EBML_WRITING)
META_PARAM(TYPE,MATROSKA_BLOCK_WRITE_TRACK,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_BLOCK_WRITE_TRACK,matroska_block,WriteTrack)
META_PARAM(TYPE,MATROSKA_BLOCK_WRITE_SEGMENTINFO,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_BLOCK_WRITE_SEGMENTINFO,matroska_block,WriteSegInfo)
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
META_DATA(TYPE_NODE_REF,MATROSKA_CLUSTER_WRITE_SEGMENTINFO,matroska_cluster,WriteSegInfo)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_SEEKPOINT_CLASS)
META_CLASS(SIZE,sizeof(matroska_seekpoint))
META_PARAM(TYPE,MATROSKA_SEEKPOINT_ELEMENT,TYPE_NODE)
META_DATA(TYPE_NODE_REF,MATROSKA_SEEKPOINT_ELEMENT,matroska_seekpoint,Link)
META_END_CONTINUE(EBML_MASTER_CLASS)

META_START_CONTINUE(MATROSKA_SEGMENTUID_CLASS)
META_VMT(TYPE_FUNC,ebml_element_vmt,ValidateSize,ValidateSizeSegUID)
META_END(EBML_BINARY_CLASS)
