/*
 * $Id: matroskamain.c 1319 2008-09-19 16:08:57Z robux4 $
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

// Seek Header
const ebml_context MATROSKA_ContextSeekId = {FOURCC(0x53,0xAB,0,0), EBML_BINARY_CLASS, 0, 0, "SeekID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextSeekPosition = {FOURCC(0x53,0xAC,0,0), EBML_INTEGER_CLASS, 0, 0, "SeekPosition", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticSeekPoint[] = {
    {1, 1, &MATROSKA_ContextSeekId},
    {1, 1, &MATROSKA_ContextSeekPosition},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextSeek = {FOURCC(0x4D,0xBB,0,0), EBML_MASTER_CLASS, 0, 0, "SeekPoint", EBML_SemanticSeekPoint, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticSeekHead[] = {
    {1, 0, &MATROSKA_ContextSeek},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextSeekHead = {FOURCC(0x11,0x4D,0x9B,0x74), EBML_MASTER_CLASS, 0, 0, "SeekHeader", EBML_SemanticSeekHead, EBML_GlobalsSemantic};

// Segment Info
const ebml_context MATROSKA_ContextTranslateEditionUID = {FOURCC(0x69,0xFC,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateEditionUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTranslateCodec = {FOURCC(0x69,0xBF,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateCodec", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTranslateID = {FOURCC(0x69,0xA5,0,0), EBML_BINARY_CLASS, 0, 0, "ChapterTranslateID", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticCodecTranslate[] = {
    {0, 0, &MATROSKA_ContextTranslateEditionUID},
    {1, 1, &MATROSKA_ContextTranslateCodec},
    {1, 1, &MATROSKA_ContextTranslateID},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextChapterTranslate = {FOURCC(0x69,0x24,0,0), EBML_MASTER_CLASS, 0, 0, "ChapterTranslate", EBML_SemanticCodecTranslate, EBML_GlobalsSemantic};

const ebml_context MATROSKA_ContextSegmentUid = {FOURCC(0x73,0xA4,0,0), EBML_BINARY_CLASS, 0, 0, "SegmentUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextSegmentFilename = {FOURCC(0x73,0x84,0,0), EBML_UNISTRING_CLASS, 0, 0, "SegmentFilename", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextPrevUid = {FOURCC(0x3C,0xB9,0x23,0), EBML_BINARY_CLASS, 0, 0, "PrevUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextPrevFilename = {FOURCC(0x3C,0x83,0xAB,0), EBML_UNISTRING_CLASS, 0, 0, "PrevFilename", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextNextUid = {FOURCC(0x3E,0xB9,0x23,0), EBML_BINARY_CLASS, 0, 0, "NextUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextNextFilename = {FOURCC(0x3E,0x83,0xBB,0), EBML_UNISTRING_CLASS, 0, 0, "NextFilename", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextSegmentFamily = {FOURCC(0x44,0x44,0,0), EBML_BINARY_CLASS, 0, 0, "SegmentFamily", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTimecodeScale = {FOURCC(0x2A,0xD7,0xB1,0), EBML_INTEGER_CLASS, 1, 1000000, "TimecodeScale", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextDuration = {FOURCC(0x44,0x89,0,0), EBML_FLOAT_CLASS, 0, 0, "Duration", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextSegmentDate = {FOURCC(0x44,0x61,0,0), EBML_DATE_CLASS, 0, 0, "DateUTC", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextSegmentTitle = {FOURCC(0x7B,0xA9,0,0), EBML_UNISTRING_CLASS, 0, 0, "Title", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextMuxingApp = {FOURCC(0x4D,0x80,0,0), EBML_UNISTRING_CLASS, 0, 0, "MuxingApp", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextWritingApp = {FOURCC(0x57,0x41,0,0), EBML_UNISTRING_CLASS, 0, 0, "WritingApp", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticSegmentInfo[] = {
    {0, 0, &MATROSKA_ContextSegmentUid},
    {0, 0, &MATROSKA_ContextSegmentFilename},
    {0, 0, &MATROSKA_ContextPrevUid},
    {0, 0, &MATROSKA_ContextPrevFilename},
    {0, 0, &MATROSKA_ContextNextUid},
    {0, 0, &MATROSKA_ContextNextFilename},
    {0, 0, &MATROSKA_ContextSegmentFamily},
    {0, 0, &MATROSKA_ContextChapterTranslate},
    {0, 0, &MATROSKA_ContextTimecodeScale},
    {0, 0, &MATROSKA_ContextDuration},
    {0, 0, &MATROSKA_ContextSegmentDate},
    {0, 0, &MATROSKA_ContextSegmentTitle},
    {0, 0, &MATROSKA_ContextMuxingApp},
    {0, 0, &MATROSKA_ContextWritingApp},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextSegmentInfo = {FOURCC(0x15,0x49,0xA9,0x66), EBML_MASTER_CLASS, 0, 0, "Info", EBML_SemanticSegmentInfo, EBML_GlobalsSemantic};


// Chapters
const ebml_context MATROSKA_ContextChapterTrackNumber = {FOURCC(0x89,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterFlagEnabled", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticChapterTrack[] = {
    {1, 0, &MATROSKA_ContextChapterTrackNumber},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextChapterProcessTime = {FOURCC(0x69,0x22,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterProcessTime", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterProcessData = {FOURCC(0x69,0x33,0,0), EBML_BINARY_CLASS, 0, 0, "ChapterProcessData", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticChapterProcessCommand[] = {
    {1, 1, &MATROSKA_ContextChapterProcessTime},
    {1, 1, &MATROSKA_ContextChapterProcessData},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextChapterProcessCodecID = {FOURCC(0x69,0x55,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterProcessCodecID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterProcessPrivate = {FOURCC(0x45,0x0D,0,0), EBML_BINARY_CLASS, 0, 0, "ChapterProcessPrivate", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterProcessCommand = {FOURCC(0x69,0x11,0,0), EBML_MASTER_CLASS, 0, 0, "ChapterProcessCommand", EBML_SemanticChapterProcessCommand, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticChapterProcess[] = {
    {1, 1, &MATROSKA_ContextChapterProcessCodecID},
    {0, 1, &MATROSKA_ContextChapterProcessPrivate},
    {0, 0, &MATROSKA_ContextChapterProcessCommand},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextChapterString = {FOURCC(0x85,0,0,0), EBML_UNISTRING_CLASS, 0, 0, "ChapterString", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterLanguage = {FOURCC(0x43,0x7C,0,0), EBML_STRING_CLASS, 1, (intptr_t)"eng", "ChapterLanguage", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterCountry = {FOURCC(0x43,0x7E,0,0), EBML_STRING_CLASS, 0, 0, "ChapterCountry", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticChapterDisplay[] = {
    {1, 1, &MATROSKA_ContextChapterString},
    {1, 0, &MATROSKA_ContextChapterLanguage},
    {0, 0, &MATROSKA_ContextChapterCountry},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextChapterUID = {FOURCC(0x73,0xC4,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterTimeStart = {FOURCC(0x91,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterTimeStart", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterTimeEnd = {FOURCC(0x92,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterTimeEnd", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterHidden = {FOURCC(0x98,0,0,0), EBML_BOOLEAN_CLASS, 0, 0, "ChapterFlagHidden", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterEnabled = {FOURCC(0x45,0x98,0,0), EBML_BOOLEAN_CLASS, 0, 0, "ChapterFlagEnabled", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterSegmentUID = {FOURCC(0x6E,0x67,0,0), EBML_BINARY_CLASS, 0, 0, "ChapterSegmentUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterSegmentEditionUID = {FOURCC(0x6E,0xBC,0,0), EBML_BINARY_CLASS, 0, 0, "ChapterSegmentEditionUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterPhysical = {FOURCC(0x63,0xC3,0,0), EBML_INTEGER_CLASS, 0, 0, "ChapterPhysicalEquiv", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterTrack = {FOURCC(0x8F,0,0,0), EBML_MASTER_CLASS, 0, 0, "ChapterTrack", EBML_SemanticChapterTrack, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterDisplay = {FOURCC(0x80,0,0,0), EBML_MASTER_CLASS, 0, 0, "ChapterDisplay", EBML_SemanticChapterDisplay, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextChapterProcess = {FOURCC(0x69,0x44,0,0), EBML_MASTER_CLASS, 0, 0, "ChapterProcess", EBML_SemanticChapterProcess, EBML_GlobalsSemantic};

const ebml_context MATROSKA_ContextEditionUID = {FOURCC(0x45,0xBC,0,0), EBML_INTEGER_CLASS, 0, 0, "EditionUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextEditionHidden = {FOURCC(0x45,0xBD,0,0), EBML_BOOLEAN_CLASS, 0, 0, "EditionFlagHidden", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextEditionDefault = {FOURCC(0x45,0xDB,0,0), EBML_BOOLEAN_CLASS, 0, 0, "EditionFlagDefault", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextEditionOrdered = {FOURCC(0x45,0xDD,0,0), EBML_BOOLEAN_CLASS, 0, 0, "EditionFlagOrdered", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticChapterAtom[] = {
    {0, 0, &MATROSKA_ContextChapterAtom},
    {0, 1, &MATROSKA_ContextChapterUID},
    {1, 1, &MATROSKA_ContextChapterTimeStart},
    {0, 1, &MATROSKA_ContextChapterTimeEnd},
    {1, 1, &MATROSKA_ContextChapterHidden},
    {1, 1, &MATROSKA_ContextChapterEnabled},
    {0, 1, &MATROSKA_ContextChapterSegmentUID},
    {0, 1, &MATROSKA_ContextChapterSegmentEditionUID},
    {0, 1, &MATROSKA_ContextChapterPhysical},
    {0, 0, &MATROSKA_ContextChapterTrack},
    {0, 0, &MATROSKA_ContextChapterDisplay},
    {0, 0, &MATROSKA_ContextChapterProcess},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextChapterAtom = {FOURCC(0xB6,0,0,0), EBML_MASTER_CLASS, 0, 0, "ChapterAtom", EBML_SemanticChapterAtom, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticEdition[] = {
    {1, 0, &MATROSKA_ContextEditionUID},
    {0, 1, &MATROSKA_ContextEditionHidden},
    {1, 1, &MATROSKA_ContextEditionDefault},
    {0, 1, &MATROSKA_ContextEditionOrdered},
    {1, 0, &MATROSKA_ContextChapterAtom},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextChapterEntry = {FOURCC(0x45,0xB9,0,0), EBML_MASTER_CLASS, 0, 0, "EditionEntry", EBML_SemanticEdition, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticChapters[] = {
    {1, 0, &MATROSKA_ContextChapterEntry},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextChapters = {FOURCC(0x10,0x43,0xA7,0x70), EBML_MASTER_CLASS, 0, 0, "Chapters", EBML_SemanticChapters, EBML_GlobalsSemantic};

// Cluster
const ebml_context MATROSKA_ContextClusterBlockAdditionalID = {FOURCC(0xEE,0,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)1, "ClusterBlockAdditionalID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterBlockAdditional = {FOURCC(0xA5,0,0,0), EBML_BINARY_CLASS, 0, 0, "BlockAdditional", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticClusterBlockMore[] = {
    {1, 1, &MATROSKA_ContextClusterBlockAdditionalID},
    {1, 1, &MATROSKA_ContextClusterBlockAdditional},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextClusterBlockMore = {FOURCC(0xA6,0,0,0), EBML_MASTER_CLASS, 0, 0, "ClusterBlockMore", EBML_SemanticClusterBlockMore, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticClusterBlockAdditions[] = {
    {1, 0, &MATROSKA_ContextClusterBlockMore},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextClusterLaceNumber = {FOURCC(0xCC,0,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterLaceNumber", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterSliceDuation = {FOURCC(0xCF,0,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "ClusterSliceDuation", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticClusterTimeSlice[] = {
    {0, 1, &MATROSKA_ContextClusterLaceNumber},
    {0, 1, &MATROSKA_ContextClusterSliceDuation},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextClusterTimeSlice = {FOURCC(0xE8,0,0,0), EBML_MASTER_CLASS, 0, 0, "ClusterTimeSlice", EBML_SemanticClusterTimeSlice, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticClusterSlices[] = {
    {0, 0, &MATROSKA_ContextClusterTimeSlice},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextClusterBlock = {FOURCC(0xA1,0,0,0), EBML_BINARY_CLASS, 0, 0, "ClusterBlock", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterBlockAdditions = {FOURCC(0x75,0xA1,0,0), EBML_MASTER_CLASS, 0, 0, "ClusterBlockAdditions", EBML_SemanticClusterBlockAdditions, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterBlockDuration = {FOURCC(0x9B,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ClusterBlockDuration", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterReferencePriority = {FOURCC(0xFA,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ClusterReferencePriority", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterReferenceBlock = {FOURCC(0xFB,0,0,0), EBML_SINTEGER_CLASS, 0, 0, "ClusterReferenceBlock", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterSlices = {FOURCC(0x8E,0,0,0), EBML_MASTER_CLASS, 0, 0, "ClusterSlices", EBML_SemanticClusterSlices, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticClusterBlockGroup[] = {
    {1, 1, &MATROSKA_ContextClusterBlock},
    {0, 1, &MATROSKA_ContextClusterBlockDuration},
    {1, 1, &MATROSKA_ContextClusterReferencePriority},
    {0, 0, &MATROSKA_ContextClusterReferenceBlock},
    {0, 0, &MATROSKA_ContextClusterSlices},
    {0, 1, &MATROSKA_ContextClusterBlockAdditions},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextClusterSilentTrackNumber = {FOURCC(0x58,0xD7,0,0), EBML_INTEGER_CLASS, 0, 0, "ClusterSilentTrackNumber", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticClusterSilentTracks[] = {
    {0, 0, &MATROSKA_ContextClusterSilentTrackNumber},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextClusterTimecode = {FOURCC(0xE7,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ClusterTimecode", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterSilentTracks = {FOURCC(0x58,0x54,0,0), EBML_MASTER_CLASS, 0, 0, "ClusterSilentTracks", EBML_SemanticClusterSilentTracks, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterPosition = {FOURCC(0xA7,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ClusterPosition", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterPrevSize = {FOURCC(0xAB,0,0,0), EBML_INTEGER_CLASS, 0, 0, "ClusterPrevSize", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterBlockGroup = {FOURCC(0xA0,0,0,0), EBML_MASTER_CLASS, 0, 0, "ClusterBlockGroup", EBML_SemanticClusterBlockGroup, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextClusterSimpleBlock = {FOURCC(0xA3,0,0,0), EBML_BINARY_CLASS, 0, 0, "ClusterSimpleBlock", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticCluster[] = {
    {1, 1, &MATROSKA_ContextClusterTimecode},
    {0, 1, &MATROSKA_ContextClusterPosition},
    {0, 1, &MATROSKA_ContextClusterPrevSize},
    {0, 0, &MATROSKA_ContextClusterBlockGroup},
    {0, 0, &MATROSKA_ContextClusterSimpleBlock},
    {0, 1, &MATROSKA_ContextClusterSilentTracks},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextCluster = {FOURCC(0x1F,0x43,0xB6,0x75), EBML_MASTER_CLASS, 0, 0, "Cluster", EBML_SemanticCluster, EBML_GlobalsSemantic};

// Tracks
const ebml_context MATROSKA_SemanticTrackAudioSamplingFreq = {FOURCC(0xB5,0,0,0), EBML_FLOAT_CLASS, 1, (intptr_t)8000.0, "TrackAudioSamplingFreq", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackAudioOutputSamplingFreq = {FOURCC(0x78,0xB5,0,0), EBML_FLOAT_CLASS, 0, 0, "TrackAudioOutputSamplingFreq", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackAudioChannels = {FOURCC(0x9F,0,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)1, "TrackAudioChannels", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackAudioBitDepth = {FOURCC(0x62,0x64,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackAudioBitDepth", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTrackAudio[] = {
    {1, 1, &MATROSKA_SemanticTrackAudioSamplingFreq},
    {0, 1, &MATROSKA_SemanticTrackAudioOutputSamplingFreq},
    {1, 1, &MATROSKA_SemanticTrackAudioChannels},
    {0, 1, &MATROSKA_SemanticTrackAudioBitDepth},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_SemanticTrackVideoInterlaced = {FOURCC(0x9A,0,0,0), EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "TrackVideoInterlaced", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoPixelWidth = {FOURCC(0xB0,0,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackVideoPixelWidth", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoPixelHeight = {FOURCC(0xBA,0,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackVideoPixelHeight", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropBottom = {FOURCC(0x54,0xAA,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropBottom", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropTop = {FOURCC(0x54,0xBB,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropTop", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropLeft = {FOURCC(0x54,0xCC,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropLeft", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoPixelCropRight = {FOURCC(0x54,0xDD,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoPixelCropRight", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoDisplayWidth = {FOURCC(0x54,0xB0,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackVideoDisplayWidth", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoDisplayHeight = {FOURCC(0x54,0xBA,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackVideoDisplayHeight", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoDisplayUnit = {FOURCC(0x54,0xB2,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoDisplayUnit", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoAspectRatio = {FOURCC(0x54,0xB3,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackVideoAspectRatio", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackVideoColourSpace = {FOURCC(0x2E,0xB5,0x24,0), EBML_BINARY_CLASS, 0, 0, "TrackVideoColourSpace", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTrackVideo[] = {
    {1, 1, &MATROSKA_SemanticTrackVideoInterlaced},
    {1, 1, &MATROSKA_SemanticTrackVideoPixelWidth},
    {1, 1, &MATROSKA_SemanticTrackVideoPixelHeight},
    {1, 0, &MATROSKA_SemanticTrackVideoPixelCropBottom},
    {1, 0, &MATROSKA_SemanticTrackVideoPixelCropTop},
    {1, 0, &MATROSKA_SemanticTrackVideoPixelCropLeft},
    {1, 0, &MATROSKA_SemanticTrackVideoPixelCropRight},
    {1, 0, &MATROSKA_SemanticTrackVideoDisplayWidth},
    {1, 0, &MATROSKA_SemanticTrackVideoDisplayHeight},
    {1, 0, &MATROSKA_SemanticTrackVideoDisplayUnit},
    {1, 0, &MATROSKA_SemanticTrackVideoAspectRatio},
    {1, 0, &MATROSKA_SemanticTrackVideoColourSpace},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncodingCompressionAlgo = {FOURCC(0x42,0x54,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingCompressionAlgo", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingCompressionSetting = {FOURCC(0x42,0x55,0,0), EBML_BINARY_CLASS, 0, 0, "TrackEncodingCompressionSetting", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTrackEncodingCompression[] = {
    {1, 1, &MATROSKA_SemanticTrackEncodingCompressionAlgo},
    {0, 1, &MATROSKA_SemanticTrackEncodingCompressionSetting},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncodingEncryptionAlgo = {FOURCC(0x47,0xE1,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionAlgo", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionKeyID = {FOURCC(0x47,0xE2,0,0), EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionKeyID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignature = {FOURCC(0x47,0xE3,0,0), EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionSignature", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignatureKeyID = {FOURCC(0x47,0xE4,0,0), EBML_BINARY_CLASS, 0, 0, "TrackEncodingEncryptionSignatureKeyID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignatureAlgo = {FOURCC(0x47,0xE5,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionSignatureAlgo", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingEncryptionSignatureHashAlgo = {FOURCC(0x47,0xE6,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingEncryptionSignatureHashAlgo", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTrackEncodingEncryption[] = {
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionAlgo},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionKeyID},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignature},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignatureKeyID},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignatureAlgo},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryptionSignatureHashAlgo},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncodingOrder = {FOURCC(0x50,0x31,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingOrder", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingScope = {FOURCC(0x50,0x32,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)1, "TrackEncodingScope", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingType = {FOURCC(0x50,0x33,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackEncodingType", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingCompression = {FOURCC(0x50,0x34,0,0), EBML_MASTER_CLASS, 0, 0, "TrackEncodingCompression", EBML_SemanticTrackEncodingCompression, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackEncodingEncryption = {FOURCC(0x50,0x35,0,0), EBML_MASTER_CLASS, 0, 0, "TrackEncodingEncryption", EBML_SemanticTrackEncodingEncryption, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTrackEncoding[] = {
    {1, 1, &MATROSKA_SemanticTrackEncodingOrder},
    {1, 1, &MATROSKA_SemanticTrackEncodingScope},
    {1, 1, &MATROSKA_SemanticTrackEncodingType},
    {0, 1, &MATROSKA_SemanticTrackEncodingCompression},
    {0, 1, &MATROSKA_SemanticTrackEncodingEncryption},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_SemanticTrackEncoding = {FOURCC(0x62,0x40,0,0), EBML_MASTER_CLASS, 0, 0, "TrackEncoding", EBML_SemanticTrackEncoding, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTrackEncodings[] = {
    {1, 0, &MATROSKA_SemanticTrackEncoding},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_SemanticTrackTranslateEditionUID = {FOURCC(0x66,0xFC,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackTranslateEditionUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackTranslateCodec = {FOURCC(0x66,0xBF,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackTranslateCodec", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_SemanticTrackTranslateID = {FOURCC(0x66,0xA5,0,0), EBML_BINARY_CLASS, 0, 0, "TrackTranslateID", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTrackTranslate[] = {
    {0, 0, &MATROSKA_SemanticTrackTranslateEditionUID},
    {1, 1, &MATROSKA_SemanticTrackTranslateCodec},
    {1, 1, &MATROSKA_SemanticTrackTranslateID},
    {0, 0, NULL} // end of the table
};

const ebml_context MATROSKA_ContextTrackNumber = {FOURCC(0xD7,0,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackNumber", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackUID = {FOURCC(0x73,0xC5,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackType = {FOURCC(0x83,0,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackType", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackEnabled = {FOURCC(0xB9,0,0,0), EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TrackEnabled", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackDefault = {FOURCC(0x88,0,0,0), EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TrackDefault", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackForced = {FOURCC(0x55,0xAA,0,0), EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "TrackForced", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackLacing = {FOURCC(0x9C,0,0,0), EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TrackLacing", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackMinCache = {FOURCC(0x6D,0xE7,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackMinCache", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackMaxCache = {FOURCC(0x6D,0xF8,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackMaxCache", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackDefaultDuration = {FOURCC(0x23,0xE3,0x83,0), EBML_INTEGER_CLASS, 0, 0, "TrackDefaultDuration", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackTimecodeScale = {FOURCC(0x23,0x31,0x4F,0), EBML_FLOAT_CLASS, 1, (intptr_t)1.0, "TrackTimecodeScale", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackMaxBlockAdditionID = {FOURCC(0x55,0xEE,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrackMaxBlockAdditionID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackName = {FOURCC(0x53,0x6E,0,0), EBML_UNISTRING_CLASS, 0, 0, "TrackName", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackLanguage = {FOURCC(0x22,0xB5,0x9C,0), EBML_STRING_CLASS, 1, (intptr_t)"eng", "TrackLanguage", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackCodecID = {FOURCC(0x86,0,0,0), EBML_STRING_CLASS, 0, 0, "TrackCodecID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackCodecPrivate = {FOURCC(0x63,0xA2,0,0), EBML_BINARY_CLASS, 0, 0, "TrackCodecPrivate", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackCodecName = {FOURCC(0x25,0x86,0x88,0), EBML_UNISTRING_CLASS, 0, 0, "TrackCodecName", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackAttachmentLink = {FOURCC(0x74,0x46,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackAttachmentLink", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackOverlay = {FOURCC(0x6F,0xAB,0,0), EBML_INTEGER_CLASS, 0, 0, "TrackOverlay", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackTranslate = {FOURCC(0x66,0x24,0,0), EBML_MASTER_CLASS, 0, 0, "TrackTranslate", EBML_SemanticTrackTranslate, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackVideo = {FOURCC(0xE0,0,0,0), EBML_MASTER_CLASS, 0, 0, "TrackVideo", EBML_SemanticTrackVideo, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackAudio = {FOURCC(0xE1,0,0,0), EBML_MASTER_CLASS, 0, 0, "TrackAudio", EBML_SemanticTrackAudio, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTrackEncodings = {FOURCC(0x6D,0x80,0,0), EBML_MASTER_CLASS, 0, 0, "TrackEncodings", EBML_SemanticTrackEncodings, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticTrackEntry[] = {
    {1, 1, &MATROSKA_ContextTrackNumber},
    {1, 1, &MATROSKA_ContextTrackUID},
    {1, 1, &MATROSKA_ContextTrackType},
    {1, 1, &MATROSKA_ContextTrackEnabled},
    {1, 1, &MATROSKA_ContextTrackDefault},
    {1, 0, &MATROSKA_ContextTrackForced},
    {0, 1, &MATROSKA_ContextTrackVideo},
    {0, 1, &MATROSKA_ContextTrackAudio},
    {1, 1, &MATROSKA_ContextTrackLacing},
    {1, 1, &MATROSKA_ContextTrackMinCache},
    {0, 1, &MATROSKA_ContextTrackMaxCache},
    {0, 1, &MATROSKA_ContextTrackDefaultDuration},
    {1, 1, &MATROSKA_ContextTrackTimecodeScale},
    {1, 1, &MATROSKA_ContextTrackMaxBlockAdditionID},
    {0, 1, &MATROSKA_ContextTrackName},
    {0, 1, &MATROSKA_ContextTrackLanguage},
    {1, 1, &MATROSKA_ContextTrackCodecID},
    {0, 1, &MATROSKA_ContextTrackCodecPrivate},
    {0, 1, &MATROSKA_ContextTrackCodecName},
    {0, 1, &MATROSKA_ContextTrackAttachmentLink},
    {0, 0, &MATROSKA_ContextTrackOverlay},
    {0, 1, &MATROSKA_ContextTrackEncodings},
    {0, 0, &MATROSKA_ContextTrackTranslate},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextTrackEntry = {FOURCC(0xAE,0,0,0), EBML_MASTER_CLASS, 0, 0, "TrackEntry", EBML_SemanticTrackEntry, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticTracks[] = {
    {1, 0, &MATROSKA_ContextTrackEntry},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextTracks = {FOURCC(0x16,0x54,0xAE,0x6B), EBML_MASTER_CLASS, 0, 0, "Tracks", EBML_SemanticTracks, EBML_GlobalsSemantic};

// Cues
const ebml_context MATROSKA_ContextCueTrack = {FOURCC(0xF7,0,0,0), EBML_INTEGER_CLASS, 0, 0, "CueTrack", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextCueClusterPosition = {FOURCC(0xF1,0,0,0), EBML_INTEGER_CLASS, 0, 0, "CueClusterPosition", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextCueBlockNumber = {FOURCC(0x53,0x78,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)1, "CueBlockNumber", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticCueTrackPosition[] = {
    {1, 1, &MATROSKA_ContextCueTrack},
    {1, 1, &MATROSKA_ContextCueClusterPosition},
    {0, 1, &MATROSKA_ContextCueBlockNumber},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextCueTrackPositions = {FOURCC(0xB7,0,0,0), EBML_MASTER_CLASS, 0, 0, "CueTrackPosition", EBML_SemanticCueTrackPosition, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextCueTime = {FOURCC(0xB3,0,0,0), EBML_INTEGER_CLASS, 0, 0, "CueTime", NULL, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticCuePoint[] = {
    {1, 1, &MATROSKA_ContextCueTime},
    {1, 0, &MATROSKA_ContextCueTrackPositions},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextCuePoint = {FOURCC(0xBB,0,0,0), EBML_MASTER_CLASS, 0, 0, "CuePoint", EBML_SemanticCuePoint, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticCues[] = {
    {1, 0, &MATROSKA_ContextCuePoint},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextCues = {FOURCC(0x1C,0x53,0xBB,0x6B), EBML_MASTER_CLASS, 0, 0, "Cues", EBML_SemanticCues, EBML_GlobalsSemantic};

// Attachments
const ebml_context MATROSKA_ContextAttachedFileDescription = {FOURCC(0x46,0x7E,0,0), EBML_UNISTRING_CLASS, 0, 0, "AttachedFileDescription", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextAttachedFileName = {FOURCC(0x46,0x6E,0,0), EBML_UNISTRING_CLASS, 0, 0, "AttachedFileName", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextAttachedFileMimeType = {FOURCC(0x46,0x60,0,0), EBML_STRING_CLASS, 0, 0, "AttachedFileMimeType", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextAttachedFileData = {FOURCC(0x46,0x5C,0,0), EBML_BINARY_CLASS, 0, 0, "AttachedFileData", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextAttachedFileUID = {FOURCC(0x46,0xAE,0,0), EBML_INTEGER_CLASS, 0, 0, "AttachedFileUID", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticAttachedFile[] = {
    {1, 1, &MATROSKA_ContextAttachedFileName},
    {1, 1, &MATROSKA_ContextAttachedFileMimeType},
    {1, 1, &MATROSKA_ContextAttachedFileData},
    {1, 1, &MATROSKA_ContextAttachedFileUID},
    {0, 1, &MATROSKA_ContextAttachedFileDescription},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextAttachedFile = {FOURCC(0x61,0xA7,0,0), EBML_MASTER_CLASS, 0, 0, "AttachedFile", EBML_SemanticAttachedFile, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticAttachments[] = {
    {1, 0, &MATROSKA_ContextAttachedFile},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextAttachments = {FOURCC(0x19,0x41,0xA4,0x69), EBML_MASTER_CLASS, 0, 0, "Attachments", EBML_SemanticAttachments, EBML_GlobalsSemantic};

// Tags
const ebml_context MATROSKA_ContextTagTargetTypeValue = {FOURCC(0x68,0xCA,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)50, "TagTargetTypeValue", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagTargetType = {FOURCC(0x63,0xCA,0,0), EBML_STRING_CLASS, 0, 0, "TagTargetType", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagTargetTrackUID = {FOURCC(0x63,0xC5,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetTrackUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagTargetEditionUID = {FOURCC(0x63,0xC9,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetEditionUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagTargetChapterUID = {FOURCC(0x63,0xC4,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetChapterUID", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagTargetAttachmentUID = {FOURCC(0x63,0xC6,0,0), EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTargetAttachmentUID", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTagTargets[] = {
    {0, 1, &MATROSKA_ContextTagTargetTypeValue},
    {0, 1, &MATROSKA_ContextTagTargetType},
    {0, 0, &MATROSKA_ContextTagTargetTrackUID},
    {0, 0, &MATROSKA_ContextTagTargetEditionUID},
    {0, 0, &MATROSKA_ContextTagTargetChapterUID},
    {0, 0, &MATROSKA_ContextTagTargetAttachmentUID},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextTagTargets = {FOURCC(0x63,0xC0,0,0), EBML_MASTER_CLASS, 0, 0, "TagTargets", EBML_SemanticTagTargets, EBML_GlobalsSemantic};

const ebml_context MATROSKA_ContextTagName = {FOURCC(0x45,0xA3,0,0), EBML_UNISTRING_CLASS, 0, 0, "TagName", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagLanguage = {FOURCC(0x44,0x7A,0,0), EBML_STRING_CLASS, 1, (intptr_t)"und", "TagLanguage", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagDefault = {FOURCC(0x44,0x84,0,0), EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TagDefault", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagString = {FOURCC(0x44,0x87,0,0), EBML_UNISTRING_CLASS, 0, 0, "TagString", NULL, EBML_GlobalsSemantic};
const ebml_context MATROSKA_ContextTagBinary = {FOURCC(0x44,0x85,0,0), EBML_BINARY_CLASS, 0, 0, "TagBinary", NULL, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticSimpleTag[] = {
    {1, 1, &MATROSKA_ContextTagName},
    {1, 1, &MATROSKA_ContextTagLanguage},
    {1, 1, &MATROSKA_ContextTagDefault},
    {0, 1, &MATROSKA_ContextTagString},
    {0, 1, &MATROSKA_ContextTagBinary},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextSimpleTag = {FOURCC(0x67,0xC8,0,0), EBML_MASTER_CLASS, 0, 0, "SimpleTag", EBML_SemanticSimpleTag, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticTag[] = {
    {1, 1, &MATROSKA_ContextTagTargets},
    {1, 0, &MATROSKA_ContextSimpleTag},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextTag = {FOURCC(0x73,0x73,0,0), EBML_MASTER_CLASS, 0, 0, "Tag", EBML_SemanticTag, EBML_GlobalsSemantic};
const ebml_semantic EBML_SemanticTags[] = {
    {1, 0, &MATROSKA_ContextTag},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextTags = {FOURCC(0x12,0x54,0xC3,0x67), EBML_MASTER_CLASS, 0, 0, "Tags", EBML_SemanticTags, EBML_GlobalsSemantic};

// Segment
const ebml_semantic EBML_SemanticSegment[] = {
    {0, 0, &MATROSKA_ContextSeekHead},
    {1, 0, &MATROSKA_ContextSegmentInfo},
    {0, 0, &MATROSKA_ContextCluster},
    {0, 0, &MATROSKA_ContextTracks},
    {0, 1, &MATROSKA_ContextCues},
    {0, 1, &MATROSKA_ContextAttachments},
    {0, 1, &MATROSKA_ContextChapters},
    {0, 0, &MATROSKA_ContextTags},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextSegment = {FOURCC(0x18,0x53,0x80,0x67), EBML_MASTER_CLASS, 0, 0, "Segment\0wrmf", EBML_SemanticSegment, EBML_GlobalsSemantic};

const ebml_semantic EBML_SemanticMatroska[] = {
    {1, 0, &EBML_ContextHead},
    {1, 0, &MATROSKA_ContextSegment},
    {0, 0, NULL} // end of the table
};
const ebml_context MATROSKA_ContextStream = {FOURCC('M','K','X','_'), EBML_MASTER_CLASS, 0, 0, "Matroska Stream", EBML_SemanticMatroska, EBML_GlobalsSemantic};

err_t MATROSKA_Init(nodecontext *p)
{
    return EBML_Init(p);
}

err_t MATROSKA_Done(nodecontext *p)
{
    return EBML_Done(p);
}
