/*
 * DO NOT EDIT, GENERATED WITH schema_2_matroska_sem_h.xsl
 * https://github.com/Matroska-Org/foundation-source/tree/master/spectool
 *
 * $Id$
 * Copyright (c) 2008-2017, Matroska (non-profit organisation)
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

#ifndef MATROSKA_SEMANTIC_H
#define MATROSKA_SEMANTIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "matroska/matroska.h"
#include "matroska/matroska_internal.h"


MATROSKA_DLL extern const ebml_context MATROSKA_ContextSeekID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSeekPosition;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSeek;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextSeekHead;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextSegmentUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSegmentFilename;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrevUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrevFilename;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextNextUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextNextFilename;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSegmentFamily;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTranslateEditionUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTranslateCodec;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTranslateID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTranslate;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTimestampScale;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDuration;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDateUTC;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTitle;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMuxingApp;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextWritingApp;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextInfo;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTimestamp;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSilentTrackNumber;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSilentTracks;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextPosition;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrevSize;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSimpleBlock;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlock;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockVirtual;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAddID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAdditional;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockMore;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAdditions;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockDuration;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextReferencePriority;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextReferenceBlock;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextReferenceVirtual;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecState;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDiscardPadding;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextLaceNumber;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFrameNumber;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAdditionID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDelay;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSliceDuration;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTimeSlice;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextSlices;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextReferenceOffset;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextReferenceTimestamp;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextReferenceFrame;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockGroup;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextEncryptedBlock;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCluster;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackNumber;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFlagEnabled;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFlagDefault;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFlagForced;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFlagLacing;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMinCache;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMaxCache;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDefaultDuration;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDefaultDecodedFieldDuration;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackTimestampScale;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackOffset;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMaxBlockAdditionID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAddIDValue;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAddIDName;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAddIDType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAddIDExtraData;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBlockAdditionMapping;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextName;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextLanguage;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextLanguageIETF;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecPrivate;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecName;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextAttachmentLink;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecSettings;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecInfoURL;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecDownloadURL;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecDecodeAll;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackOverlay;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCodecDelay;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSeekPreRoll;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackTranslateEditionUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackTranslateCodec;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackTranslateTrackID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackTranslate;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextFlagInterlaced;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFieldOrder;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextStereoMode;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextAlphaMode;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextOldStereoMode;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPixelWidth;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPixelHeight;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPixelCropBottom;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPixelCropTop;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPixelCropLeft;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPixelCropRight;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDisplayWidth;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDisplayHeight;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextDisplayUnit;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextAspectRatioType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextColourSpace;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextGammaValue;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFrameRate;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMatrixCoefficients;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBitsPerChannel;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChromaSubsamplingHorz;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChromaSubsamplingVert;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCbSubsamplingHorz;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCbSubsamplingVert;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChromaSitingHorz;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChromaSitingVert;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextRange;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTransferCharacteristics;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrimaries;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMaxCLL;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMaxFALL;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrimaryRChromaticityX;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrimaryRChromaticityY;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrimaryGChromaticityX;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrimaryGChromaticityY;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrimaryBChromaticityX;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextPrimaryBChromaticityY;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextWhitePointChromaticityX;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextWhitePointChromaticityY;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextLuminanceMax;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextLuminanceMin;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextMasteringMetadata;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextColour;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextProjectionType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextProjectionPrivate;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextProjectionPoseYaw;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextProjectionPosePitch;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextProjectionPoseRoll;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextProjection;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextVideo;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextSamplingFrequency;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextOutputSamplingFrequency;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChannels;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChannelPositions;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextBitDepth;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextAudio;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackPlaneUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackPlaneType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackPlane;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackCombinePlanes;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackJoinUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackJoinBlocks;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackOperation;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrickTrackUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrickTrackSegmentUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrickTrackFlag;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrickMasterTrackUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrickMasterTrackSegmentUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncodingOrder;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncodingScope;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncodingType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentCompAlgo;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentCompSettings;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentCompression;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncAlgo;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncKeyID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextAESSettingsCipherMode;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncAESSettings;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentSignature;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentSigKeyID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentSigAlgo;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentSigHashAlgo;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncryption;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncoding;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextContentEncodings;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTrackEntry;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTracks;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueTime;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueTrack;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueClusterPosition;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueRelativePosition;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueDuration;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueBlockNumber;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueCodecState;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueRefTime;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueRefCluster;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueRefNumber;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueRefCodecState;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueReference;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextCueTrackPositions;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextCuePoint;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextCues;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileDescription;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileName;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileMimeType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileData;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileReferral;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileUsedStartTime;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextFileUsedEndTime;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextAttachedFile;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextAttachments;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextEditionUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextEditionFlagHidden;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextEditionFlagDefault;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextEditionFlagOrdered;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterStringUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTimeStart;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTimeEnd;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterFlagHidden;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterFlagEnabled;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterSegmentUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterSegmentEditionUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterPhysicalEquiv;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTrackNumber;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterTrack;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapString;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapLanguage;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapLanguageIETF;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapCountry;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterDisplay;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapProcessCodecID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapProcessPrivate;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapProcessTime;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapProcessData;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapProcessCommand;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapProcess;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapterAtom;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextEditionEntry;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextChapters;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTargetTypeValue;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTargetType;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagTrackUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagEditionUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagChapterUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagAttachmentUID;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTargets;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagName;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagLanguage;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagLanguageIETF;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagDefault;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagString;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextTagBinary;
MATROSKA_DLL extern const ebml_context MATROSKA_ContextSimpleTag;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTag;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextTags;

MATROSKA_DLL extern const ebml_context MATROSKA_ContextSegment;

#ifdef __cplusplus
}
#endif

#endif // MATROSKA_SEMANTIC_H


