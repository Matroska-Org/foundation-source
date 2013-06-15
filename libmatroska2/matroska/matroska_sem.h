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
#include "matroska/matroska_internal.h"

#ifndef MATROSKA_SEMANTIC_H
#define MATROSKA_SEMANTIC_H

extern const ebml_context MATROSKA_ContextSeekID;
extern const ebml_context MATROSKA_ContextSeekPosition;
extern const ebml_context MATROSKA_ContextSeek;

extern const ebml_context MATROSKA_ContextSeekHead;

extern const ebml_context MATROSKA_ContextSegmentUID;
extern const ebml_context MATROSKA_ContextSegmentFilename;
extern const ebml_context MATROSKA_ContextPrevUID;
extern const ebml_context MATROSKA_ContextPrevFilename;
extern const ebml_context MATROSKA_ContextNextUID;
extern const ebml_context MATROSKA_ContextNextFilename;
extern const ebml_context MATROSKA_ContextSegmentFamily;
extern const ebml_context MATROSKA_ContextChapterTranslateEditionUID;
extern const ebml_context MATROSKA_ContextChapterTranslateCodec;
extern const ebml_context MATROSKA_ContextChapterTranslateID;
extern const ebml_context MATROSKA_ContextChapterTranslate;

extern const ebml_context MATROSKA_ContextTimecodeScale;
extern const ebml_context MATROSKA_ContextDuration;
extern const ebml_context MATROSKA_ContextDateUTC;
extern const ebml_context MATROSKA_ContextTitle;
extern const ebml_context MATROSKA_ContextMuxingApp;
extern const ebml_context MATROSKA_ContextWritingApp;
extern const ebml_context MATROSKA_ContextInfo;

extern const ebml_context MATROSKA_ContextTimecode;
extern const ebml_context MATROSKA_ContextSilentTrackNumber;
extern const ebml_context MATROSKA_ContextSilentTracks;

extern const ebml_context MATROSKA_ContextPosition;
extern const ebml_context MATROSKA_ContextPrevSize;
extern const ebml_context MATROSKA_ContextSimpleBlock;
extern const ebml_context MATROSKA_ContextBlock;
extern const ebml_context MATROSKA_ContextBlockVirtual;
extern const ebml_context MATROSKA_ContextBlockAddID;
extern const ebml_context MATROSKA_ContextBlockAdditional;
extern const ebml_context MATROSKA_ContextBlockMore;

extern const ebml_context MATROSKA_ContextBlockAdditions;

extern const ebml_context MATROSKA_ContextBlockDuration;
extern const ebml_context MATROSKA_ContextReferencePriority;
extern const ebml_context MATROSKA_ContextReferenceBlock;
extern const ebml_context MATROSKA_ContextReferenceVirtual;
extern const ebml_context MATROSKA_ContextCodecState;
extern const ebml_context MATROSKA_ContextDiscardPadding;
extern const ebml_context MATROSKA_ContextLaceNumber;
extern const ebml_context MATROSKA_ContextFrameNumber;
extern const ebml_context MATROSKA_ContextBlockAdditionID;
extern const ebml_context MATROSKA_ContextDelay;
extern const ebml_context MATROSKA_ContextSliceDuration;
extern const ebml_context MATROSKA_ContextTimeSlice;

extern const ebml_context MATROSKA_ContextSlices;

extern const ebml_context MATROSKA_ContextReferenceOffset;
extern const ebml_context MATROSKA_ContextReferenceTimeCode;
extern const ebml_context MATROSKA_ContextReferenceFrame;

extern const ebml_context MATROSKA_ContextBlockGroup;

extern const ebml_context MATROSKA_ContextEncryptedBlock;
extern const ebml_context MATROSKA_ContextCluster;

extern const ebml_context MATROSKA_ContextTrackNumber;
extern const ebml_context MATROSKA_ContextTrackUID;
extern const ebml_context MATROSKA_ContextTrackType;
extern const ebml_context MATROSKA_ContextFlagEnabled;
extern const ebml_context MATROSKA_ContextFlagDefault;
extern const ebml_context MATROSKA_ContextFlagForced;
extern const ebml_context MATROSKA_ContextFlagLacing;
extern const ebml_context MATROSKA_ContextMinCache;
extern const ebml_context MATROSKA_ContextMaxCache;
extern const ebml_context MATROSKA_ContextDefaultDuration;
extern const ebml_context MATROSKA_ContextDefaultDecodedFieldDuration;
extern const ebml_context MATROSKA_ContextTrackTimecodeScale;
extern const ebml_context MATROSKA_ContextTrackOffset;
extern const ebml_context MATROSKA_ContextMaxBlockAdditionID;
extern const ebml_context MATROSKA_ContextName;
extern const ebml_context MATROSKA_ContextLanguage;
extern const ebml_context MATROSKA_ContextCodecID;
extern const ebml_context MATROSKA_ContextCodecPrivate;
extern const ebml_context MATROSKA_ContextCodecName;
extern const ebml_context MATROSKA_ContextAttachmentLink;
extern const ebml_context MATROSKA_ContextCodecSettings;
extern const ebml_context MATROSKA_ContextCodecInfoURL;
extern const ebml_context MATROSKA_ContextCodecDownloadURL;
extern const ebml_context MATROSKA_ContextCodecDecodeAll;
extern const ebml_context MATROSKA_ContextTrackOverlay;
extern const ebml_context MATROSKA_ContextCodecDelay;
extern const ebml_context MATROSKA_ContextSeekPreRoll;
extern const ebml_context MATROSKA_ContextTrackTranslateEditionUID;
extern const ebml_context MATROSKA_ContextTrackTranslateCodec;
extern const ebml_context MATROSKA_ContextTrackTranslateTrackID;
extern const ebml_context MATROSKA_ContextTrackTranslate;

extern const ebml_context MATROSKA_ContextFlagInterlaced;
extern const ebml_context MATROSKA_ContextStereoMode;
extern const ebml_context MATROSKA_ContextAlphaMode;
extern const ebml_context MATROSKA_ContextOldStereoMode;
extern const ebml_context MATROSKA_ContextPixelWidth;
extern const ebml_context MATROSKA_ContextPixelHeight;
extern const ebml_context MATROSKA_ContextPixelCropBottom;
extern const ebml_context MATROSKA_ContextPixelCropTop;
extern const ebml_context MATROSKA_ContextPixelCropLeft;
extern const ebml_context MATROSKA_ContextPixelCropRight;
extern const ebml_context MATROSKA_ContextDisplayWidth;
extern const ebml_context MATROSKA_ContextDisplayHeight;
extern const ebml_context MATROSKA_ContextDisplayUnit;
extern const ebml_context MATROSKA_ContextAspectRatioType;
extern const ebml_context MATROSKA_ContextColourSpace;
extern const ebml_context MATROSKA_ContextGammaValue;
extern const ebml_context MATROSKA_ContextFrameRate;
extern const ebml_context MATROSKA_ContextVideo;

extern const ebml_context MATROSKA_ContextSamplingFrequency;
extern const ebml_context MATROSKA_ContextOutputSamplingFrequency;
extern const ebml_context MATROSKA_ContextChannels;
extern const ebml_context MATROSKA_ContextChannelPositions;
extern const ebml_context MATROSKA_ContextBitDepth;
extern const ebml_context MATROSKA_ContextAudio;

extern const ebml_context MATROSKA_ContextTrackPlaneUID;
extern const ebml_context MATROSKA_ContextTrackPlaneType;
extern const ebml_context MATROSKA_ContextTrackPlane;

extern const ebml_context MATROSKA_ContextTrackCombinePlanes;

extern const ebml_context MATROSKA_ContextTrackJoinUID;
extern const ebml_context MATROSKA_ContextTrackJoinBlocks;

extern const ebml_context MATROSKA_ContextTrackOperation;

extern const ebml_context MATROSKA_ContextTrickTrackUID;
extern const ebml_context MATROSKA_ContextTrickTrackSegmentUID;
extern const ebml_context MATROSKA_ContextTrickTrackFlag;
extern const ebml_context MATROSKA_ContextTrickMasterTrackUID;
extern const ebml_context MATROSKA_ContextTrickMasterTrackSegmentUID;
extern const ebml_context MATROSKA_ContextContentEncodingOrder;
extern const ebml_context MATROSKA_ContextContentEncodingScope;
extern const ebml_context MATROSKA_ContextContentEncodingType;
extern const ebml_context MATROSKA_ContextContentCompAlgo;
extern const ebml_context MATROSKA_ContextContentCompSettings;
extern const ebml_context MATROSKA_ContextContentCompression;

extern const ebml_context MATROSKA_ContextContentEncAlgo;
extern const ebml_context MATROSKA_ContextContentEncKeyID;
extern const ebml_context MATROSKA_ContextContentSignature;
extern const ebml_context MATROSKA_ContextContentSigKeyID;
extern const ebml_context MATROSKA_ContextContentSigAlgo;
extern const ebml_context MATROSKA_ContextContentSigHashAlgo;
extern const ebml_context MATROSKA_ContextContentEncryption;

extern const ebml_context MATROSKA_ContextContentEncoding;

extern const ebml_context MATROSKA_ContextContentEncodings;

extern const ebml_context MATROSKA_ContextTrackEntry;

extern const ebml_context MATROSKA_ContextTracks;

extern const ebml_context MATROSKA_ContextCueTime;
extern const ebml_context MATROSKA_ContextCueTrack;
extern const ebml_context MATROSKA_ContextCueClusterPosition;
extern const ebml_context MATROSKA_ContextCueRelativePosition;
extern const ebml_context MATROSKA_ContextCueDuration;
extern const ebml_context MATROSKA_ContextCueBlockNumber;
extern const ebml_context MATROSKA_ContextCueCodecState;
extern const ebml_context MATROSKA_ContextCueRefTime;
extern const ebml_context MATROSKA_ContextCueRefCluster;
extern const ebml_context MATROSKA_ContextCueRefNumber;
extern const ebml_context MATROSKA_ContextCueRefCodecState;
extern const ebml_context MATROSKA_ContextCueReference;

extern const ebml_context MATROSKA_ContextCueTrackPositions;

extern const ebml_context MATROSKA_ContextCuePoint;

extern const ebml_context MATROSKA_ContextCues;

extern const ebml_context MATROSKA_ContextFileDescription;
extern const ebml_context MATROSKA_ContextFileName;
extern const ebml_context MATROSKA_ContextFileMimeType;
extern const ebml_context MATROSKA_ContextFileData;
extern const ebml_context MATROSKA_ContextFileUID;
extern const ebml_context MATROSKA_ContextFileReferral;
extern const ebml_context MATROSKA_ContextFileUsedStartTime;
extern const ebml_context MATROSKA_ContextFileUsedEndTime;
extern const ebml_context MATROSKA_ContextAttachedFile;

extern const ebml_context MATROSKA_ContextAttachments;

extern const ebml_context MATROSKA_ContextEditionUID;
extern const ebml_context MATROSKA_ContextEditionFlagHidden;
extern const ebml_context MATROSKA_ContextEditionFlagDefault;
extern const ebml_context MATROSKA_ContextEditionFlagOrdered;
extern const ebml_context MATROSKA_ContextChapterUID;
extern const ebml_context MATROSKA_ContextChapterStringUID;
extern const ebml_context MATROSKA_ContextChapterTimeStart;
extern const ebml_context MATROSKA_ContextChapterTimeEnd;
extern const ebml_context MATROSKA_ContextChapterFlagHidden;
extern const ebml_context MATROSKA_ContextChapterFlagEnabled;
extern const ebml_context MATROSKA_ContextChapterSegmentUID;
extern const ebml_context MATROSKA_ContextChapterSegmentEditionUID;
extern const ebml_context MATROSKA_ContextChapterPhysicalEquiv;
extern const ebml_context MATROSKA_ContextChapterTrackNumber;
extern const ebml_context MATROSKA_ContextChapterTrack;

extern const ebml_context MATROSKA_ContextChapString;
extern const ebml_context MATROSKA_ContextChapLanguage;
extern const ebml_context MATROSKA_ContextChapCountry;
extern const ebml_context MATROSKA_ContextChapterDisplay;

extern const ebml_context MATROSKA_ContextChapProcessCodecID;
extern const ebml_context MATROSKA_ContextChapProcessPrivate;
extern const ebml_context MATROSKA_ContextChapProcessTime;
extern const ebml_context MATROSKA_ContextChapProcessData;
extern const ebml_context MATROSKA_ContextChapProcessCommand;

extern const ebml_context MATROSKA_ContextChapProcess;

extern const ebml_context MATROSKA_ContextChapterAtom;

extern const ebml_context MATROSKA_ContextEditionEntry;

extern const ebml_context MATROSKA_ContextChapters;

extern const ebml_context MATROSKA_ContextTargetTypeValue;
extern const ebml_context MATROSKA_ContextTargetType;
extern const ebml_context MATROSKA_ContextTagTrackUID;
extern const ebml_context MATROSKA_ContextTagEditionUID;
extern const ebml_context MATROSKA_ContextTagChapterUID;
extern const ebml_context MATROSKA_ContextTagAttachmentUID;
extern const ebml_context MATROSKA_ContextTargets;

extern const ebml_context MATROSKA_ContextTagName;
extern const ebml_context MATROSKA_ContextTagLanguage;
extern const ebml_context MATROSKA_ContextTagDefault;
extern const ebml_context MATROSKA_ContextTagString;
extern const ebml_context MATROSKA_ContextTagBinary;
extern const ebml_context MATROSKA_ContextSimpleTag;

extern const ebml_context MATROSKA_ContextTag;

extern const ebml_context MATROSKA_ContextTags;

extern const ebml_context MATROSKA_ContextSegment;

#endif // MATROSKA_SEMANTIC_H
