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
#include "matroska/matroska.h"
#include "matroska/matroska_internal.h"

#ifndef MATROSKA_SEMANTIC_H
#define MATROSKA_SEMANTIC_H

EBML_DLL extern const ebml_context MATROSKA_ContextSeekID;
EBML_DLL extern const ebml_context MATROSKA_ContextSeekPosition;
EBML_DLL extern const ebml_context MATROSKA_ContextSeek;

EBML_DLL extern const ebml_context MATROSKA_ContextSeekHead;

EBML_DLL extern const ebml_context MATROSKA_ContextSegmentUID;
EBML_DLL extern const ebml_context MATROSKA_ContextSegmentFilename;
EBML_DLL extern const ebml_context MATROSKA_ContextPrevUID;
EBML_DLL extern const ebml_context MATROSKA_ContextPrevFilename;
EBML_DLL extern const ebml_context MATROSKA_ContextNextUID;
EBML_DLL extern const ebml_context MATROSKA_ContextNextFilename;
EBML_DLL extern const ebml_context MATROSKA_ContextSegmentFamily;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTranslateEditionUID;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTranslateCodec;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTranslateID;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTranslate;

EBML_DLL extern const ebml_context MATROSKA_ContextTimestampScale;
EBML_DLL extern const ebml_context MATROSKA_ContextDuration;
EBML_DLL extern const ebml_context MATROSKA_ContextDateUTC;
EBML_DLL extern const ebml_context MATROSKA_ContextTitle;
EBML_DLL extern const ebml_context MATROSKA_ContextMuxingApp;
EBML_DLL extern const ebml_context MATROSKA_ContextWritingApp;
EBML_DLL extern const ebml_context MATROSKA_ContextInfo;

EBML_DLL extern const ebml_context MATROSKA_ContextTimestamp;
EBML_DLL extern const ebml_context MATROSKA_ContextSilentTrackNumber;
EBML_DLL extern const ebml_context MATROSKA_ContextSilentTracks;

EBML_DLL extern const ebml_context MATROSKA_ContextPosition;
EBML_DLL extern const ebml_context MATROSKA_ContextPrevSize;
EBML_DLL extern const ebml_context MATROSKA_ContextSimpleBlock;
EBML_DLL extern const ebml_context MATROSKA_ContextBlock;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockVirtual;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAddID;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAdditional;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockMore;

EBML_DLL extern const ebml_context MATROSKA_ContextBlockAdditions;

EBML_DLL extern const ebml_context MATROSKA_ContextBlockDuration;
EBML_DLL extern const ebml_context MATROSKA_ContextReferencePriority;
EBML_DLL extern const ebml_context MATROSKA_ContextReferenceBlock;
EBML_DLL extern const ebml_context MATROSKA_ContextReferenceVirtual;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecState;
EBML_DLL extern const ebml_context MATROSKA_ContextDiscardPadding;
EBML_DLL extern const ebml_context MATROSKA_ContextLaceNumber;
EBML_DLL extern const ebml_context MATROSKA_ContextFrameNumber;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAdditionID;
EBML_DLL extern const ebml_context MATROSKA_ContextDelay;
EBML_DLL extern const ebml_context MATROSKA_ContextSliceDuration;
EBML_DLL extern const ebml_context MATROSKA_ContextTimeSlice;

EBML_DLL extern const ebml_context MATROSKA_ContextSlices;

EBML_DLL extern const ebml_context MATROSKA_ContextReferenceOffset;
EBML_DLL extern const ebml_context MATROSKA_ContextReferenceTimestamp;
EBML_DLL extern const ebml_context MATROSKA_ContextReferenceFrame;

EBML_DLL extern const ebml_context MATROSKA_ContextBlockGroup;

EBML_DLL extern const ebml_context MATROSKA_ContextEncryptedBlock;
EBML_DLL extern const ebml_context MATROSKA_ContextCluster;

EBML_DLL extern const ebml_context MATROSKA_ContextTrackNumber;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackType;
EBML_DLL extern const ebml_context MATROSKA_ContextFlagEnabled;
EBML_DLL extern const ebml_context MATROSKA_ContextFlagDefault;
EBML_DLL extern const ebml_context MATROSKA_ContextFlagForced;
EBML_DLL extern const ebml_context MATROSKA_ContextFlagLacing;
EBML_DLL extern const ebml_context MATROSKA_ContextMinCache;
EBML_DLL extern const ebml_context MATROSKA_ContextMaxCache;
EBML_DLL extern const ebml_context MATROSKA_ContextDefaultDuration;
EBML_DLL extern const ebml_context MATROSKA_ContextDefaultDecodedFieldDuration;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackTimestampScale;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackOffset;
EBML_DLL extern const ebml_context MATROSKA_ContextMaxBlockAdditionID;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAddIDValue;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAddIDName;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAddIDType;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAddIDExtraData;
EBML_DLL extern const ebml_context MATROSKA_ContextBlockAdditionMapping;

EBML_DLL extern const ebml_context MATROSKA_ContextName;
EBML_DLL extern const ebml_context MATROSKA_ContextLanguage;
EBML_DLL extern const ebml_context MATROSKA_ContextLanguageIETF;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecID;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecPrivate;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecName;
EBML_DLL extern const ebml_context MATROSKA_ContextAttachmentLink;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecSettings;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecInfoURL;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecDownloadURL;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecDecodeAll;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackOverlay;
EBML_DLL extern const ebml_context MATROSKA_ContextCodecDelay;
EBML_DLL extern const ebml_context MATROSKA_ContextSeekPreRoll;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackTranslateEditionUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackTranslateCodec;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackTranslateTrackID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackTranslate;

EBML_DLL extern const ebml_context MATROSKA_ContextFlagInterlaced;
EBML_DLL extern const ebml_context MATROSKA_ContextFieldOrder;
EBML_DLL extern const ebml_context MATROSKA_ContextStereoMode;
EBML_DLL extern const ebml_context MATROSKA_ContextAlphaMode;
EBML_DLL extern const ebml_context MATROSKA_ContextOldStereoMode;
EBML_DLL extern const ebml_context MATROSKA_ContextPixelWidth;
EBML_DLL extern const ebml_context MATROSKA_ContextPixelHeight;
EBML_DLL extern const ebml_context MATROSKA_ContextPixelCropBottom;
EBML_DLL extern const ebml_context MATROSKA_ContextPixelCropTop;
EBML_DLL extern const ebml_context MATROSKA_ContextPixelCropLeft;
EBML_DLL extern const ebml_context MATROSKA_ContextPixelCropRight;
EBML_DLL extern const ebml_context MATROSKA_ContextDisplayWidth;
EBML_DLL extern const ebml_context MATROSKA_ContextDisplayHeight;
EBML_DLL extern const ebml_context MATROSKA_ContextDisplayUnit;
EBML_DLL extern const ebml_context MATROSKA_ContextAspectRatioType;
EBML_DLL extern const ebml_context MATROSKA_ContextColourSpace;
EBML_DLL extern const ebml_context MATROSKA_ContextGammaValue;
EBML_DLL extern const ebml_context MATROSKA_ContextFrameRate;
EBML_DLL extern const ebml_context MATROSKA_ContextMatrixCoefficients;
EBML_DLL extern const ebml_context MATROSKA_ContextBitsPerChannel;
EBML_DLL extern const ebml_context MATROSKA_ContextChromaSubsamplingHorz;
EBML_DLL extern const ebml_context MATROSKA_ContextChromaSubsamplingVert;
EBML_DLL extern const ebml_context MATROSKA_ContextCbSubsamplingHorz;
EBML_DLL extern const ebml_context MATROSKA_ContextCbSubsamplingVert;
EBML_DLL extern const ebml_context MATROSKA_ContextChromaSitingHorz;
EBML_DLL extern const ebml_context MATROSKA_ContextChromaSitingVert;
EBML_DLL extern const ebml_context MATROSKA_ContextRange;
EBML_DLL extern const ebml_context MATROSKA_ContextTransferCharacteristics;
EBML_DLL extern const ebml_context MATROSKA_ContextPrimaries;
EBML_DLL extern const ebml_context MATROSKA_ContextMaxCLL;
EBML_DLL extern const ebml_context MATROSKA_ContextMaxFALL;
EBML_DLL extern const ebml_context MATROSKA_ContextPrimaryRChromaticityX;
EBML_DLL extern const ebml_context MATROSKA_ContextPrimaryRChromaticityY;
EBML_DLL extern const ebml_context MATROSKA_ContextPrimaryGChromaticityX;
EBML_DLL extern const ebml_context MATROSKA_ContextPrimaryGChromaticityY;
EBML_DLL extern const ebml_context MATROSKA_ContextPrimaryBChromaticityX;
EBML_DLL extern const ebml_context MATROSKA_ContextPrimaryBChromaticityY;
EBML_DLL extern const ebml_context MATROSKA_ContextWhitePointChromaticityX;
EBML_DLL extern const ebml_context MATROSKA_ContextWhitePointChromaticityY;
EBML_DLL extern const ebml_context MATROSKA_ContextLuminanceMax;
EBML_DLL extern const ebml_context MATROSKA_ContextLuminanceMin;
EBML_DLL extern const ebml_context MATROSKA_ContextMasteringMetadata;

EBML_DLL extern const ebml_context MATROSKA_ContextColour;

EBML_DLL extern const ebml_context MATROSKA_ContextProjectionType;
EBML_DLL extern const ebml_context MATROSKA_ContextProjectionPrivate;
EBML_DLL extern const ebml_context MATROSKA_ContextProjectionPoseYaw;
EBML_DLL extern const ebml_context MATROSKA_ContextProjectionPosePitch;
EBML_DLL extern const ebml_context MATROSKA_ContextProjectionPoseRoll;
EBML_DLL extern const ebml_context MATROSKA_ContextProjection;

EBML_DLL extern const ebml_context MATROSKA_ContextVideo;

EBML_DLL extern const ebml_context MATROSKA_ContextSamplingFrequency;
EBML_DLL extern const ebml_context MATROSKA_ContextOutputSamplingFrequency;
EBML_DLL extern const ebml_context MATROSKA_ContextChannels;
EBML_DLL extern const ebml_context MATROSKA_ContextChannelPositions;
EBML_DLL extern const ebml_context MATROSKA_ContextBitDepth;
EBML_DLL extern const ebml_context MATROSKA_ContextAudio;

EBML_DLL extern const ebml_context MATROSKA_ContextTrackPlaneUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackPlaneType;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackPlane;

EBML_DLL extern const ebml_context MATROSKA_ContextTrackCombinePlanes;

EBML_DLL extern const ebml_context MATROSKA_ContextTrackJoinUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrackJoinBlocks;

EBML_DLL extern const ebml_context MATROSKA_ContextTrackOperation;

EBML_DLL extern const ebml_context MATROSKA_ContextTrickTrackUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrickTrackSegmentUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrickTrackFlag;
EBML_DLL extern const ebml_context MATROSKA_ContextTrickMasterTrackUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTrickMasterTrackSegmentUID;
EBML_DLL extern const ebml_context MATROSKA_ContextContentEncodingOrder;
EBML_DLL extern const ebml_context MATROSKA_ContextContentEncodingScope;
EBML_DLL extern const ebml_context MATROSKA_ContextContentEncodingType;
EBML_DLL extern const ebml_context MATROSKA_ContextContentCompAlgo;
EBML_DLL extern const ebml_context MATROSKA_ContextContentCompSettings;
EBML_DLL extern const ebml_context MATROSKA_ContextContentCompression;

EBML_DLL extern const ebml_context MATROSKA_ContextContentEncAlgo;
EBML_DLL extern const ebml_context MATROSKA_ContextContentEncKeyID;
EBML_DLL extern const ebml_context MATROSKA_ContextAESSettingsCipherMode;
EBML_DLL extern const ebml_context MATROSKA_ContextContentEncAESSettings;

EBML_DLL extern const ebml_context MATROSKA_ContextContentSignature;
EBML_DLL extern const ebml_context MATROSKA_ContextContentSigKeyID;
EBML_DLL extern const ebml_context MATROSKA_ContextContentSigAlgo;
EBML_DLL extern const ebml_context MATROSKA_ContextContentSigHashAlgo;
EBML_DLL extern const ebml_context MATROSKA_ContextContentEncryption;

EBML_DLL extern const ebml_context MATROSKA_ContextContentEncoding;

EBML_DLL extern const ebml_context MATROSKA_ContextContentEncodings;

EBML_DLL extern const ebml_context MATROSKA_ContextTrackEntry;

EBML_DLL extern const ebml_context MATROSKA_ContextTracks;

EBML_DLL extern const ebml_context MATROSKA_ContextCueTime;
EBML_DLL extern const ebml_context MATROSKA_ContextCueTrack;
EBML_DLL extern const ebml_context MATROSKA_ContextCueClusterPosition;
EBML_DLL extern const ebml_context MATROSKA_ContextCueRelativePosition;
EBML_DLL extern const ebml_context MATROSKA_ContextCueDuration;
EBML_DLL extern const ebml_context MATROSKA_ContextCueBlockNumber;
EBML_DLL extern const ebml_context MATROSKA_ContextCueCodecState;
EBML_DLL extern const ebml_context MATROSKA_ContextCueRefTime;
EBML_DLL extern const ebml_context MATROSKA_ContextCueRefCluster;
EBML_DLL extern const ebml_context MATROSKA_ContextCueRefNumber;
EBML_DLL extern const ebml_context MATROSKA_ContextCueRefCodecState;
EBML_DLL extern const ebml_context MATROSKA_ContextCueReference;

EBML_DLL extern const ebml_context MATROSKA_ContextCueTrackPositions;

EBML_DLL extern const ebml_context MATROSKA_ContextCuePoint;

EBML_DLL extern const ebml_context MATROSKA_ContextCues;

EBML_DLL extern const ebml_context MATROSKA_ContextFileDescription;
EBML_DLL extern const ebml_context MATROSKA_ContextFileName;
EBML_DLL extern const ebml_context MATROSKA_ContextFileMimeType;
EBML_DLL extern const ebml_context MATROSKA_ContextFileData;
EBML_DLL extern const ebml_context MATROSKA_ContextFileUID;
EBML_DLL extern const ebml_context MATROSKA_ContextFileReferral;
EBML_DLL extern const ebml_context MATROSKA_ContextFileUsedStartTime;
EBML_DLL extern const ebml_context MATROSKA_ContextFileUsedEndTime;
EBML_DLL extern const ebml_context MATROSKA_ContextAttachedFile;

EBML_DLL extern const ebml_context MATROSKA_ContextAttachments;

EBML_DLL extern const ebml_context MATROSKA_ContextEditionUID;
EBML_DLL extern const ebml_context MATROSKA_ContextEditionFlagHidden;
EBML_DLL extern const ebml_context MATROSKA_ContextEditionFlagDefault;
EBML_DLL extern const ebml_context MATROSKA_ContextEditionFlagOrdered;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterUID;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterStringUID;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTimeStart;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTimeEnd;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterFlagHidden;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterFlagEnabled;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterSegmentUID;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterSegmentEditionUID;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterPhysicalEquiv;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTrackNumber;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterTrack;

EBML_DLL extern const ebml_context MATROSKA_ContextChapString;
EBML_DLL extern const ebml_context MATROSKA_ContextChapLanguage;
EBML_DLL extern const ebml_context MATROSKA_ContextChapLanguageIETF;
EBML_DLL extern const ebml_context MATROSKA_ContextChapCountry;
EBML_DLL extern const ebml_context MATROSKA_ContextChapterDisplay;

EBML_DLL extern const ebml_context MATROSKA_ContextChapProcessCodecID;
EBML_DLL extern const ebml_context MATROSKA_ContextChapProcessPrivate;
EBML_DLL extern const ebml_context MATROSKA_ContextChapProcessTime;
EBML_DLL extern const ebml_context MATROSKA_ContextChapProcessData;
EBML_DLL extern const ebml_context MATROSKA_ContextChapProcessCommand;

EBML_DLL extern const ebml_context MATROSKA_ContextChapProcess;

EBML_DLL extern const ebml_context MATROSKA_ContextChapterAtom;

EBML_DLL extern const ebml_context MATROSKA_ContextEditionEntry;

EBML_DLL extern const ebml_context MATROSKA_ContextChapters;

EBML_DLL extern const ebml_context MATROSKA_ContextTargetTypeValue;
EBML_DLL extern const ebml_context MATROSKA_ContextTargetType;
EBML_DLL extern const ebml_context MATROSKA_ContextTagTrackUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTagEditionUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTagChapterUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTagAttachmentUID;
EBML_DLL extern const ebml_context MATROSKA_ContextTargets;

EBML_DLL extern const ebml_context MATROSKA_ContextTagName;
EBML_DLL extern const ebml_context MATROSKA_ContextTagLanguage;
EBML_DLL extern const ebml_context MATROSKA_ContextTagLanguageIETF;
EBML_DLL extern const ebml_context MATROSKA_ContextTagDefault;
EBML_DLL extern const ebml_context MATROSKA_ContextTagString;
EBML_DLL extern const ebml_context MATROSKA_ContextTagBinary;
EBML_DLL extern const ebml_context MATROSKA_ContextSimpleTag;

EBML_DLL extern const ebml_context MATROSKA_ContextTag;

EBML_DLL extern const ebml_context MATROSKA_ContextTags;

EBML_DLL extern const ebml_context MATROSKA_ContextSegment;

#endif // MATROSKA_SEMANTIC_H

