/*
 * DO NOT EDIT, GENERATED WITH DATA2LIB2
 *
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

const ebml_context MATROSKA_ContextSeekID = {0x53AB, EBML_BINARY_CLASS, 0, 0, "SeekID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSeekPosition = {0x53AC, EBML_INTEGER_CLASS, 0, 0, "SeekPosition", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSeek[] = {
    {1, 1, &MATROSKA_ContextSeekID, 0},
    {1, 1, &MATROSKA_ContextSeekPosition, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSeek = {0x4DBB, MATROSKA_SEEKPOINT_CLASS, 0, 0, "Seek", EBML_SemanticSeek, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSeekHead[] = {
    {1, 0, &MATROSKA_ContextSeek, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSeekHead = {0x114D9B74, EBML_MASTER_CLASS, 0, 0, "SeekHead", EBML_SemanticSeekHead, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSegmentUID = {0x73A4, MATROSKA_SEGMENTUID_CLASS, 0, 0, "SegmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSegmentFilename = {0x7384, EBML_UNISTRING_CLASS, 0, 0, "SegmentFilename", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPrevUID = {0x3CB923, MATROSKA_SEGMENTUID_CLASS, 0, 0, "PrevUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPrevFilename = {0x3C83AB, EBML_UNISTRING_CLASS, 0, 0, "PrevFilename", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextNextUID = {0x3EB923, MATROSKA_SEGMENTUID_CLASS, 0, 0, "NextUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextNextFilename = {0x3E83BB, EBML_UNISTRING_CLASS, 0, 0, "NextFilename", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSegmentFamily = {0x4444, EBML_BINARY_CLASS, 0, 0, "SegmentFamily", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTranslateEditionUID = {0x69FC, EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTranslateCodec = {0x69BF, EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateCodec", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTranslateID = {0x69A5, EBML_BINARY_CLASS, 0, 0, "ChapterTranslateID", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterTranslate[] = {
    {0, 0, &MATROSKA_ContextChapterTranslateEditionUID, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapterTranslateCodec, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapterTranslateID, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterTranslate = {0x6924, EBML_MASTER_CLASS, 0, 0, "ChapterTranslate", EBML_SemanticChapterTranslate, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTimecodeScale = {0x2AD7B1, EBML_INTEGER_CLASS, 1, (intptr_t)1000000, "TimecodeScale", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDuration = {0x4489, EBML_FLOAT_CLASS, 0, 0, "Duration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDateUTC = {0x4461, EBML_DATE_CLASS, 0, 0, "DateUTC", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTitle = {0x7BA9, EBML_UNISTRING_CLASS, 0, 0, "Title", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextMuxingApp = {0x4D80, EBML_UNISTRING_CLASS, 0, 0, "MuxingApp", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextWritingApp = {0x5741, EBML_UNISTRING_CLASS, 0, 0, "WritingApp", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticInfo[] = {
    {0, 1, &MATROSKA_ContextSegmentUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextSegmentFilename, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrevUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrevFilename, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextNextUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextNextFilename, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextSegmentFamily, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextChapterTranslate, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTimecodeScale, 0},
    {0, 1, &MATROSKA_ContextDuration, 0},
    {0, 1, &MATROSKA_ContextDateUTC, 0},
    {0, 1, &MATROSKA_ContextTitle, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextMuxingApp, 0},
    {1, 1, &MATROSKA_ContextWritingApp, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextInfo = {0x1549A966, EBML_MASTER_CLASS, 0, 0, "Info", EBML_SemanticInfo, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTimecode = {0xE7, EBML_INTEGER_CLASS, 0, 0, "Timecode", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSilentTrackNumber = {0x58D7, EBML_INTEGER_CLASS, 0, 0, "SilentTrackNumber", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSilentTracks[] = {
    {0, 0, &MATROSKA_ContextSilentTrackNumber, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSilentTracks = {0x5854, EBML_MASTER_CLASS, 0, 0, "SilentTracks", EBML_SemanticSilentTracks, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPosition = {0xA7, EBML_INTEGER_CLASS, 0, 0, "Position", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPrevSize = {0xAB, EBML_INTEGER_CLASS, 0, 0, "PrevSize", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSimpleBlock = {0xA3, MATROSKA_BLOCK_CLASS, 0, 0, "SimpleBlock", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextBlock = {0xA1, MATROSKA_BLOCK_CLASS, 0, 0, "Block", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextBlockVirtual = {0xA2, EBML_BINARY_CLASS, 0, 0, "BlockVirtual", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextBlockAddID = {0xEE, EBML_INTEGER_CLASS, 1, (intptr_t)1, "BlockAddID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextBlockAdditional = {0xA5, EBML_BINARY_CLASS, 0, 0, "BlockAdditional", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticBlockMore[] = {
    {1, 1, &MATROSKA_ContextBlockAddID, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextBlockAdditional, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextBlockMore = {0xA6, EBML_MASTER_CLASS, 0, 0, "BlockMore", EBML_SemanticBlockMore, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticBlockAdditions[] = {
    {1, 0, &MATROSKA_ContextBlockMore, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextBlockAdditions = {0x75A1, EBML_MASTER_CLASS, 0, 0, "BlockAdditions", EBML_SemanticBlockAdditions, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextBlockDuration = {0x9B, EBML_INTEGER_CLASS, 0, 0, "BlockDuration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextReferencePriority = {0xFA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ReferencePriority", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextReferenceBlock = {0xFB, EBML_SINTEGER_CLASS, 0, 0, "ReferenceBlock", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextReferenceVirtual = {0xFD, EBML_SINTEGER_CLASS, 0, 0, "ReferenceVirtual", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecState = {0xA4, EBML_BINARY_CLASS, 0, 0, "CodecState", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDiscardPadding = {0x75A2, EBML_SINTEGER_CLASS, 0, 0, "DiscardPadding", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextLaceNumber = {0xCC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "LaceNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFrameNumber = {0xCD, EBML_INTEGER_CLASS, 1, (intptr_t)0, "FrameNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextBlockAdditionID = {0xCB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "BlockAdditionID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDelay = {0xCE, EBML_INTEGER_CLASS, 1, (intptr_t)0, "Delay", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSliceDuration = {0xCF, EBML_INTEGER_CLASS, 1, (intptr_t)0, "SliceDuration", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTimeSlice[] = {
    {0, 1, &MATROSKA_ContextLaceNumber, PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextFrameNumber, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBlockAdditionID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextDelay, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextSliceDuration, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTimeSlice = {0xE8, EBML_MASTER_CLASS, 0, 0, "TimeSlice", EBML_SemanticTimeSlice, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSlices[] = {
    {0, 0, &MATROSKA_ContextTimeSlice, PROFILE_DIVX},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSlices = {0x8E, EBML_MASTER_CLASS, 0, 0, "Slices", EBML_SemanticSlices, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextReferenceOffset = {0xC9, EBML_INTEGER_CLASS, 0, 0, "ReferenceOffset", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextReferenceTimeCode = {0xCA, EBML_INTEGER_CLASS, 0, 0, "ReferenceTimeCode", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticReferenceFrame[] = {
    {1, 1, &MATROSKA_ContextReferenceOffset, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextReferenceTimeCode, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextReferenceFrame = {0xC8, EBML_MASTER_CLASS, 0, 0, "ReferenceFrame", EBML_SemanticReferenceFrame, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticBlockGroup[] = {
    {1, 1, &MATROSKA_ContextBlock, 0},
    {0, 1, &MATROSKA_ContextBlockVirtual, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBlockAdditions, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBlockDuration, 0},
    {1, 1, &MATROSKA_ContextReferencePriority, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextReferenceBlock, 0},
    {0, 1, &MATROSKA_ContextReferenceVirtual, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCodecState, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextDiscardPadding, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextSlices, PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextReferenceFrame, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextBlockGroup = {0xA0, MATROSKA_BLOCKGROUP_CLASS, 0, 0, "BlockGroup", EBML_SemanticBlockGroup, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEncryptedBlock = {0xAF, EBML_BINARY_CLASS, 0, 0, "EncryptedBlock", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCluster[] = {
    {1, 1, &MATROSKA_ContextTimecode, 0},
    {0, 1, &MATROSKA_ContextSilentTracks, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPosition, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrevSize, 0},
    {0, 0, &MATROSKA_ContextSimpleBlock, PROFILE_MATROSKA_V1},
    {0, 0, &MATROSKA_ContextBlockGroup, 0},
    {0, 0, &MATROSKA_ContextEncryptedBlock, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCluster = {0x1F43B675, MATROSKA_CLUSTER_CLASS, 0, 0, "Cluster", EBML_SemanticCluster, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackNumber = {0xD7, EBML_INTEGER_CLASS, 0, 0, "TrackNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackUID = {0x73C5, EBML_INTEGER_CLASS, 0, 0, "TrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackType = {0x83, EBML_INTEGER_CLASS, 0, 0, "TrackType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFlagEnabled = {0xB9, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "FlagEnabled", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFlagDefault = {0x88, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "FlagDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFlagForced = {0x55AA, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagForced", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFlagLacing = {0x9C, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "FlagLacing", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextMinCache = {0x6DE7, EBML_INTEGER_CLASS, 1, (intptr_t)0, "MinCache", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextMaxCache = {0x6DF8, EBML_INTEGER_CLASS, 0, 0, "MaxCache", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDefaultDuration = {0x23E383, EBML_INTEGER_CLASS, 0, 0, "DefaultDuration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDefaultDecodedFieldDuration = {0x234E7A, EBML_INTEGER_CLASS, 0, 0, "DefaultDecodedFieldDuration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTimecodeScale = {0x23314F, EBML_FLOAT_CLASS, 1, (intptr_t)1.0, "TrackTimecodeScale", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackOffset = {0x537F, EBML_SINTEGER_CLASS, 1, (intptr_t)0, "TrackOffset", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextMaxBlockAdditionID = {0x55EE, EBML_INTEGER_CLASS, 1, (intptr_t)0, "MaxBlockAdditionID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextName = {0x536E, EBML_UNISTRING_CLASS, 0, 0, "Name", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextLanguage = {0x22B59C, EBML_STRING_CLASS, 1, (intptr_t)"eng", "Language", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecID = {0x86, EBML_STRING_CLASS, 0, 0, "CodecID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecPrivate = {0x63A2, EBML_BINARY_CLASS, 0, 0, "CodecPrivate", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecName = {0x258688, EBML_UNISTRING_CLASS, 0, 0, "CodecName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAttachmentLink = {0x7446, EBML_INTEGER_CLASS, 0, 0, "AttachmentLink", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecSettings = {0x3A9697, EBML_UNISTRING_CLASS, 0, 0, "CodecSettings", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecInfoURL = {0x3B4040, EBML_STRING_CLASS, 0, 0, "CodecInfoURL", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecDownloadURL = {0x26B240, EBML_STRING_CLASS, 0, 0, "CodecDownloadURL", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecDecodeAll = {0xAA, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "CodecDecodeAll", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackOverlay = {0x6FAB, EBML_INTEGER_CLASS, 0, 0, "TrackOverlay", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCodecDelay = {0x56AA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "CodecDelay", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSeekPreRoll = {0x56BB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "SeekPreRoll", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTranslateEditionUID = {0x66FC, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTranslateCodec = {0x66BF, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateCodec", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackTranslateTrackID = {0x66A5, EBML_BINARY_CLASS, 0, 0, "TrackTranslateTrackID", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackTranslate[] = {
    {0, 0, &MATROSKA_ContextTrackTranslateEditionUID, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackTranslateCodec, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackTranslateTrackID, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTrackTranslate = {0x6624, EBML_MASTER_CLASS, 0, 0, "TrackTranslate", EBML_SemanticTrackTranslate, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFlagInterlaced = {0x9A, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagInterlaced", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextStereoMode = {0x53B8, EBML_INTEGER_CLASS, 1, (intptr_t)0, "StereoMode", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAlphaMode = {0x53C0, EBML_INTEGER_CLASS, 1, (intptr_t)0, "AlphaMode", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextOldStereoMode = {0x53B9, EBML_INTEGER_CLASS, 0, 0, "OldStereoMode", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPixelWidth = {0xB0, EBML_INTEGER_CLASS, 0, 0, "PixelWidth", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPixelHeight = {0xBA, EBML_INTEGER_CLASS, 0, 0, "PixelHeight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPixelCropBottom = {0x54AA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropBottom", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPixelCropTop = {0x54BB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropTop", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPixelCropLeft = {0x54CC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropLeft", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextPixelCropRight = {0x54DD, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropRight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDisplayWidth = {0x54B0, EBML_INTEGER_CLASS, 0, 0, "DisplayWidth", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDisplayHeight = {0x54BA, EBML_INTEGER_CLASS, 0, 0, "DisplayHeight", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextDisplayUnit = {0x54B2, EBML_INTEGER_CLASS, 1, (intptr_t)0, "DisplayUnit", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextAspectRatioType = {0x54B3, EBML_INTEGER_CLASS, 1, (intptr_t)0, "AspectRatioType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextColourSpace = {0x2EB524, EBML_BINARY_CLASS, 0, 0, "ColourSpace", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextGammaValue = {0x2FB523, EBML_FLOAT_CLASS, 0, 0, "GammaValue", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFrameRate = {0x2383E3, EBML_FLOAT_CLASS, 0, 0, "FrameRate", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticVideo[] = {
    {1, 1, &MATROSKA_ContextFlagInterlaced, PROFILE_MATROSKA_V1|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextStereoMode, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextAlphaMode, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextOldStereoMode, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextPixelWidth, 0},
    {1, 1, &MATROSKA_ContextPixelHeight, 0},
    {0, 1, &MATROSKA_ContextPixelCropBottom, 0},
    {0, 1, &MATROSKA_ContextPixelCropTop, 0},
    {0, 1, &MATROSKA_ContextPixelCropLeft, 0},
    {0, 1, &MATROSKA_ContextPixelCropRight, 0},
    {0, 1, &MATROSKA_ContextDisplayWidth, 0},
    {0, 1, &MATROSKA_ContextDisplayHeight, 0},
    {0, 1, &MATROSKA_ContextDisplayUnit, 0},
    {0, 1, &MATROSKA_ContextAspectRatioType, 0},
    {0, 1, &MATROSKA_ContextColourSpace, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextGammaValue, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFrameRate, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextVideo = {0xE0, EBML_MASTER_CLASS, 0, 0, "Video", EBML_SemanticVideo, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextSamplingFrequency = {0xB5, EBML_FLOAT_CLASS, 1, (intptr_t)8000.0, "SamplingFrequency", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextOutputSamplingFrequency = {0x78B5, EBML_FLOAT_CLASS, 0, 0, "OutputSamplingFrequency", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChannels = {0x9F, EBML_INTEGER_CLASS, 1, (intptr_t)1, "Channels", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChannelPositions = {0x7D7B, EBML_BINARY_CLASS, 0, 0, "ChannelPositions", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextBitDepth = {0x6264, EBML_INTEGER_CLASS, 0, 0, "BitDepth", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticAudio[] = {
    {1, 1, &MATROSKA_ContextSamplingFrequency, 0},
    {0, 1, &MATROSKA_ContextOutputSamplingFrequency, 0},
    {1, 1, &MATROSKA_ContextChannels, 0},
    {0, 1, &MATROSKA_ContextChannelPositions, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBitDepth, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextAudio = {0xE1, EBML_MASTER_CLASS, 0, 0, "Audio", EBML_SemanticAudio, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackPlaneUID = {0xE5, EBML_INTEGER_CLASS, 0, 0, "TrackPlaneUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackPlaneType = {0xE6, EBML_INTEGER_CLASS, 0, 0, "TrackPlaneType", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackPlane[] = {
    {1, 1, &MATROSKA_ContextTrackPlaneUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackPlaneType, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTrackPlane = {0xE4, EBML_MASTER_CLASS, 0, 0, "TrackPlane", EBML_SemanticTrackPlane, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackCombinePlanes[] = {
    {1, 0, &MATROSKA_ContextTrackPlane, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTrackCombinePlanes = {0xE3, EBML_MASTER_CLASS, 0, 0, "TrackCombinePlanes", EBML_SemanticTrackCombinePlanes, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrackJoinUID = {0xED, EBML_INTEGER_CLASS, 0, 0, "TrackJoinUID", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackJoinBlocks[] = {
    {1, 0, &MATROSKA_ContextTrackJoinUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTrackJoinBlocks = {0xE9, EBML_MASTER_CLASS, 0, 0, "TrackJoinBlocks", EBML_SemanticTrackJoinBlocks, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackOperation[] = {
    {0, 1, &MATROSKA_ContextTrackCombinePlanes, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrackJoinBlocks, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTrackOperation = {0xE2, EBML_MASTER_CLASS, 0, 0, "TrackOperation", EBML_SemanticTrackOperation, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickTrackUID = {0xC0, EBML_INTEGER_CLASS, 0, 0, "TrickTrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickTrackSegmentUID = {0xC1, EBML_BINARY_CLASS, 0, 0, "TrickTrackSegmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickTrackFlag = {0xC6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrickTrackFlag", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickMasterTrackUID = {0xC7, EBML_INTEGER_CLASS, 0, 0, "TrickMasterTrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTrickMasterTrackSegmentUID = {0xC4, EBML_BINARY_CLASS, 0, 0, "TrickMasterTrackSegmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentEncodingOrder = {0x5031, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentEncodingOrder", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentEncodingScope = {0x5032, EBML_INTEGER_CLASS, 1, (intptr_t)1, "ContentEncodingScope", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentEncodingType = {0x5033, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentEncodingType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentCompAlgo = {0x4254, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentCompAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentCompSettings = {0x4255, EBML_BINARY_CLASS, 0, 0, "ContentCompSettings", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticContentCompression[] = {
    {1, 1, &MATROSKA_ContextContentCompAlgo, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentCompSettings, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextContentCompression = {0x5034, EBML_MASTER_CLASS, 0, 0, "ContentCompression", EBML_SemanticContentCompression, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentEncAlgo = {0x47E1, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentEncAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentEncKeyID = {0x47E2, EBML_BINARY_CLASS, 0, 0, "ContentEncKeyID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentSignature = {0x47E3, EBML_BINARY_CLASS, 0, 0, "ContentSignature", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentSigKeyID = {0x47E4, EBML_BINARY_CLASS, 0, 0, "ContentSigKeyID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentSigAlgo = {0x47E5, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentSigAlgo", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextContentSigHashAlgo = {0x47E6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentSigHashAlgo", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticContentEncryption[] = {
    {0, 1, &MATROSKA_ContextContentEncAlgo, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentEncKeyID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentSignature, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentSigKeyID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentSigAlgo, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentSigHashAlgo, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextContentEncryption = {0x5035, EBML_MASTER_CLASS, 0, 0, "ContentEncryption", EBML_SemanticContentEncryption, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticContentEncoding[] = {
    {1, 1, &MATROSKA_ContextContentEncodingOrder, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextContentEncodingScope, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextContentEncodingType, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentCompression, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentEncryption, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextContentEncoding = {0x6240, EBML_MASTER_CLASS, 0, 0, "ContentEncoding", EBML_SemanticContentEncoding, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticContentEncodings[] = {
    {1, 0, &MATROSKA_ContextContentEncoding, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextContentEncodings = {0x6D80, EBML_MASTER_CLASS, 0, 0, "ContentEncodings", EBML_SemanticContentEncodings, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTrackEntry[] = {
    {1, 1, &MATROSKA_ContextTrackNumber, 0},
    {1, 1, &MATROSKA_ContextTrackUID, 0},
    {1, 1, &MATROSKA_ContextTrackType, 0},
    {1, 1, &MATROSKA_ContextFlagEnabled, PROFILE_MATROSKA_V1|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextFlagDefault, 0},
    {1, 1, &MATROSKA_ContextFlagForced, 0},
    {1, 1, &MATROSKA_ContextFlagLacing, 0},
    {1, 1, &MATROSKA_ContextMinCache, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextMaxCache, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextDefaultDuration, 0},
    {0, 1, &MATROSKA_ContextDefaultDecodedFieldDuration, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackTimecodeScale, PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrackOffset, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextMaxBlockAdditionID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextName, 0},
    {0, 1, &MATROSKA_ContextLanguage, 0},
    {1, 1, &MATROSKA_ContextCodecID, 0},
    {0, 1, &MATROSKA_ContextCodecPrivate, 0},
    {0, 1, &MATROSKA_ContextCodecName, 0},
    {0, 1, &MATROSKA_ContextAttachmentLink, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCodecSettings, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextCodecInfoURL, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextCodecDownloadURL, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextCodecDecodeAll, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextTrackOverlay, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCodecDelay, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextSeekPreRoll, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 0, &MATROSKA_ContextTrackTranslate, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextVideo, 0},
    {0, 1, &MATROSKA_ContextAudio, 0},
    {0, 1, &MATROSKA_ContextTrackOperation, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrickTrackUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrickTrackSegmentUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrickTrackFlag, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrickMasterTrackUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrickMasterTrackSegmentUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentEncodings, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTrackEntry = {0xAE, MATROSKA_TRACKENTRY_CLASS, 0, 0, "TrackEntry", EBML_SemanticTrackEntry, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTracks[] = {
    {1, 0, &MATROSKA_ContextTrackEntry, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTracks = {0x1654AE6B, EBML_MASTER_CLASS, 0, 0, "Tracks", EBML_SemanticTracks, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueTime = {0xB3, EBML_INTEGER_CLASS, 0, 0, "CueTime", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueTrack = {0xF7, EBML_INTEGER_CLASS, 0, 0, "CueTrack", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueClusterPosition = {0xF1, EBML_INTEGER_CLASS, 0, 0, "CueClusterPosition", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueRelativePosition = {0xF0, EBML_INTEGER_CLASS, 0, 0, "CueRelativePosition", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueDuration = {0xB2, EBML_INTEGER_CLASS, 0, 0, "CueDuration", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueBlockNumber = {0x5378, EBML_INTEGER_CLASS, 1, (intptr_t)1, "CueBlockNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueCodecState = {0xEA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "CueCodecState", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueRefTime = {0x96, EBML_INTEGER_CLASS, 0, 0, "CueRefTime", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueRefCluster = {0x97, EBML_INTEGER_CLASS, 0, 0, "CueRefCluster", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueRefNumber = {0x535F, EBML_INTEGER_CLASS, 1, (intptr_t)1, "CueRefNumber", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextCueRefCodecState = {0xEB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "CueRefCodecState", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCueReference[] = {
    {1, 1, &MATROSKA_ContextCueRefTime, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextCueRefCluster, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCueRefNumber, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCueRefCodecState, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCueReference = {0xDB, EBML_MASTER_CLASS, 0, 0, "CueReference", EBML_SemanticCueReference, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCueTrackPositions[] = {
    {1, 1, &MATROSKA_ContextCueTrack, 0},
    {1, 1, &MATROSKA_ContextCueClusterPosition, 0},
    {0, 1, &MATROSKA_ContextCueRelativePosition, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCueDuration, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCueBlockNumber, 0},
    {0, 1, &MATROSKA_ContextCueCodecState, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextCueReference, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCueTrackPositions = {0xB7, EBML_MASTER_CLASS, 0, 0, "CueTrackPositions", EBML_SemanticCueTrackPositions, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCuePoint[] = {
    {1, 1, &MATROSKA_ContextCueTime, 0},
    {1, 0, &MATROSKA_ContextCueTrackPositions, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCuePoint = {0xBB, MATROSKA_CUEPOINT_CLASS, 0, 0, "CuePoint", EBML_SemanticCuePoint, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticCues[] = {
    {1, 0, &MATROSKA_ContextCuePoint, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextCues = {0x1C53BB6B, EBML_MASTER_CLASS, 0, 0, "Cues", EBML_SemanticCues, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileDescription = {0x467E, EBML_UNISTRING_CLASS, 0, 0, "FileDescription", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileName = {0x466E, EBML_UNISTRING_CLASS, 0, 0, "FileName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileMimeType = {0x4660, EBML_STRING_CLASS, 0, 0, "FileMimeType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileData = {0x465C, MATROSKA_BIGBINARY_CLASS, 0, 0, "FileData", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileUID = {0x46AE, EBML_INTEGER_CLASS, 0, 0, "FileUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileReferral = {0x4675, EBML_BINARY_CLASS, 0, 0, "FileReferral", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileUsedStartTime = {0x4661, EBML_INTEGER_CLASS, 0, 0, "FileUsedStartTime", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextFileUsedEndTime = {0x4662, EBML_INTEGER_CLASS, 0, 0, "FileUsedEndTime", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticAttachedFile[] = {
    {0, 1, &MATROSKA_ContextFileDescription, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextFileName, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextFileMimeType, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextFileData, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextFileUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFileReferral, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFileUsedStartTime, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFileUsedEndTime, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextAttachedFile = {0x61A7, MATROSKA_ATTACHMENT_CLASS, 0, 0, "AttachedFile", EBML_SemanticAttachedFile, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticAttachments[] = {
    {1, 0, &MATROSKA_ContextAttachedFile, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextAttachments = {0x1941A469, EBML_MASTER_CLASS, 0, 0, "Attachments", EBML_SemanticAttachments, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionUID = {0x45BC, EBML_INTEGER_CLASS, 0, 0, "EditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionFlagHidden = {0x45BD, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagHidden", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionFlagDefault = {0x45DB, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextEditionFlagOrdered = {0x45DD, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagOrdered", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterUID = {0x73C4, EBML_INTEGER_CLASS, 0, 0, "ChapterUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterStringUID = {0x5654, EBML_UNISTRING_CLASS, 0, 0, "ChapterStringUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTimeStart = {0x91, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeStart", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTimeEnd = {0x92, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeEnd", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterFlagHidden = {0x98, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "ChapterFlagHidden", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterFlagEnabled = {0x4598, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "ChapterFlagEnabled", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterSegmentUID = {0x6E67, EBML_BINARY_CLASS, 0, 0, "ChapterSegmentUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterSegmentEditionUID = {0x6EBC, EBML_INTEGER_CLASS, 0, 0, "ChapterSegmentEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterPhysicalEquiv = {0x63C3, EBML_INTEGER_CLASS, 0, 0, "ChapterPhysicalEquiv", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapterTrackNumber = {0x89, EBML_INTEGER_CLASS, 0, 0, "ChapterTrackNumber", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterTrack[] = {
    {1, 0, &MATROSKA_ContextChapterTrackNumber, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterTrack = {0x8F, EBML_MASTER_CLASS, 0, 0, "ChapterTrack", EBML_SemanticChapterTrack, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapString = {0x85, EBML_UNISTRING_CLASS, 0, 0, "ChapString", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapLanguage = {0x437C, EBML_STRING_CLASS, 1, (intptr_t)"eng", "ChapLanguage", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapCountry = {0x437E, EBML_STRING_CLASS, 0, 0, "ChapCountry", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterDisplay[] = {
    {1, 1, &MATROSKA_ContextChapString, 0},
    {1, 0, &MATROSKA_ContextChapLanguage, 0},
    {0, 0, &MATROSKA_ContextChapCountry, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterDisplay = {0x80, EBML_MASTER_CLASS, 0, 0, "ChapterDisplay", EBML_SemanticChapterDisplay, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapProcessCodecID = {0x6955, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ChapProcessCodecID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapProcessPrivate = {0x450D, EBML_BINARY_CLASS, 0, 0, "ChapProcessPrivate", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapProcessTime = {0x6922, EBML_INTEGER_CLASS, 0, 0, "ChapProcessTime", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextChapProcessData = {0x6933, EBML_BINARY_CLASS, 0, 0, "ChapProcessData", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapProcessCommand[] = {
    {1, 1, &MATROSKA_ContextChapProcessTime, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapProcessData, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapProcessCommand = {0x6911, EBML_MASTER_CLASS, 0, 0, "ChapProcessCommand", EBML_SemanticChapProcessCommand, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapProcess[] = {
    {1, 1, &MATROSKA_ContextChapProcessCodecID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapProcessPrivate, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextChapProcessCommand, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapProcess = {0x6944, EBML_MASTER_CLASS, 0, 0, "ChapProcess", EBML_SemanticChapProcess, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapterAtom[] = {
    {0, 0, &MATROSKA_ContextChapterAtom, 0}, // recursive
    {1, 1, &MATROSKA_ContextChapterUID, 0},
    {0, 1, &MATROSKA_ContextChapterStringUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextChapterTimeStart, 0},
    {0, 1, &MATROSKA_ContextChapterTimeEnd, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapterFlagHidden, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapterFlagEnabled, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapterSegmentUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapterSegmentEditionUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapterPhysicalEquiv, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapterTrack, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextChapterDisplay, 0},
    {0, 0, &MATROSKA_ContextChapProcess, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapterAtom = {0xB6, EBML_MASTER_CLASS, 0, 0, "ChapterAtom", EBML_SemanticChapterAtom, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticEditionEntry[] = {
    {0, 1, &MATROSKA_ContextEditionUID, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextEditionFlagHidden, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextEditionFlagDefault, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextEditionFlagOrdered, PROFILE_WEBM},
    {1, 0, &MATROSKA_ContextChapterAtom, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextEditionEntry = {0x45B9, EBML_MASTER_CLASS, 0, 0, "EditionEntry", EBML_SemanticEditionEntry, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticChapters[] = {
    {1, 0, &MATROSKA_ContextEditionEntry, 0},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextChapters = {0x1043A770, EBML_MASTER_CLASS, 0, 0, "Chapters", EBML_SemanticChapters, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTargetTypeValue = {0x68CA, EBML_INTEGER_CLASS, 1, (intptr_t)50, "TargetTypeValue", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTargetType = {0x63CA, EBML_STRING_CLASS, 0, 0, "TargetType", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagTrackUID = {0x63C5, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTrackUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagEditionUID = {0x63C9, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagEditionUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagChapterUID = {0x63C4, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagChapterUID", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagAttachmentUID = {0x63C6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagAttachmentUID", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTargets[] = {
    {0, 1, &MATROSKA_ContextTargetTypeValue, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTargetType, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextTagTrackUID, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextTagEditionUID, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextTagChapterUID, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextTagAttachmentUID, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTargets = {0x63C0, EBML_MASTER_CLASS, 0, 0, "Targets", EBML_SemanticTargets, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagName = {0x45A3, EBML_UNISTRING_CLASS, 0, 0, "TagName", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagLanguage = {0x447A, EBML_STRING_CLASS, 1, (intptr_t)"und", "TagLanguage", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagDefault = {0x4484, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TagDefault", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagString = {0x4487, EBML_UNISTRING_CLASS, 0, 0, "TagString", NULL, EBML_SemanticGlobals, NULL};
const ebml_context MATROSKA_ContextTagBinary = {0x4485, EBML_BINARY_CLASS, 0, 0, "TagBinary", NULL, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSimpleTag[] = {
    {0, 0, &MATROSKA_ContextSimpleTag, PROFILE_WEBM}, // recursive
    {1, 1, &MATROSKA_ContextTagName, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTagLanguage, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTagDefault, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTagString, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTagBinary, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSimpleTag = {0x67C8, EBML_MASTER_CLASS, 0, 0, "SimpleTag", EBML_SemanticSimpleTag, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTag[] = {
    {1, 1, &MATROSKA_ContextTargets, PROFILE_WEBM},
    {1, 0, &MATROSKA_ContextSimpleTag, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTag = {0x7373, EBML_MASTER_CLASS, 0, 0, "Tag", EBML_SemanticTag, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticTags[] = {
    {1, 0, &MATROSKA_ContextTag, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextTags = {0x1254C367, EBML_MASTER_CLASS, 0, 0, "Tags", EBML_SemanticTags, EBML_SemanticGlobals, NULL};

const ebml_semantic EBML_SemanticSegment[] = {
    {0, 0, &MATROSKA_ContextSeekHead, 0},
    {1, 0, &MATROSKA_ContextInfo, 0},
    {0, 0, &MATROSKA_ContextCluster, 0},
    {0, 0, &MATROSKA_ContextTracks, 0},
    {0, 1, &MATROSKA_ContextCues, 0},
    {0, 1, &MATROSKA_ContextAttachments, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapters, 0},
    {0, 0, &MATROSKA_ContextTags, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};
const ebml_context MATROSKA_ContextSegment = {0x18538067, EBML_MASTER_CLASS, 0, 0, "Segment", EBML_SemanticSegment, EBML_SemanticGlobals, NULL};
