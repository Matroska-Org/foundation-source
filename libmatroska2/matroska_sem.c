/*
 * DO NOT EDIT, GENERATED WITH schema_2_matroska_sem_c.xsl
 * https://github.com/Matroska-Org/foundation-source/tree/master/spectool
 *
 * $Id$
 * Copyright (c) 2008-2020, Matroska (non-profit organisation)
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
#include "matroska2/matroska.h"
#include "matroska2/matroska_sem.h"
#include "matroska2/matroska_classes.h"

ebml_context MATROSKA_ContextSeekID;
const ebml_context *MATROSKA_getContextSeekID() { return &MATROSKA_ContextSeekID; }
ebml_context MATROSKA_ContextSeekPosition;
const ebml_context *MATROSKA_getContextSeekPosition() { return &MATROSKA_ContextSeekPosition; }
ebml_context MATROSKA_ContextSeek;
const ebml_context *MATROSKA_getContextSeek() { return &MATROSKA_ContextSeek; }
ebml_context MATROSKA_ContextSeekHead;
const ebml_context *MATROSKA_getContextSeekHead() { return &MATROSKA_ContextSeekHead; }
ebml_context MATROSKA_ContextSegmentUID;
const ebml_context *MATROSKA_getContextSegmentUID() { return &MATROSKA_ContextSegmentUID; }
ebml_context MATROSKA_ContextSegmentFilename;
const ebml_context *MATROSKA_getContextSegmentFilename() { return &MATROSKA_ContextSegmentFilename; }
ebml_context MATROSKA_ContextPrevUID;
const ebml_context *MATROSKA_getContextPrevUID() { return &MATROSKA_ContextPrevUID; }
ebml_context MATROSKA_ContextPrevFilename;
const ebml_context *MATROSKA_getContextPrevFilename() { return &MATROSKA_ContextPrevFilename; }
ebml_context MATROSKA_ContextNextUID;
const ebml_context *MATROSKA_getContextNextUID() { return &MATROSKA_ContextNextUID; }
ebml_context MATROSKA_ContextNextFilename;
const ebml_context *MATROSKA_getContextNextFilename() { return &MATROSKA_ContextNextFilename; }
ebml_context MATROSKA_ContextSegmentFamily;
const ebml_context *MATROSKA_getContextSegmentFamily() { return &MATROSKA_ContextSegmentFamily; }
ebml_context MATROSKA_ContextChapterTranslateEditionUID;
const ebml_context *MATROSKA_getContextChapterTranslateEditionUID() { return &MATROSKA_ContextChapterTranslateEditionUID; }
ebml_context MATROSKA_ContextChapterTranslateCodec;
const ebml_context *MATROSKA_getContextChapterTranslateCodec() { return &MATROSKA_ContextChapterTranslateCodec; }
ebml_context MATROSKA_ContextChapterTranslateID;
const ebml_context *MATROSKA_getContextChapterTranslateID() { return &MATROSKA_ContextChapterTranslateID; }
ebml_context MATROSKA_ContextChapterTranslate;
const ebml_context *MATROSKA_getContextChapterTranslate() { return &MATROSKA_ContextChapterTranslate; }
ebml_context MATROSKA_ContextTimestampScale;
const ebml_context *MATROSKA_getContextTimestampScale() { return &MATROSKA_ContextTimestampScale; }
ebml_context MATROSKA_ContextDuration;
const ebml_context *MATROSKA_getContextDuration() { return &MATROSKA_ContextDuration; }
ebml_context MATROSKA_ContextDateUTC;
const ebml_context *MATROSKA_getContextDateUTC() { return &MATROSKA_ContextDateUTC; }
ebml_context MATROSKA_ContextTitle;
const ebml_context *MATROSKA_getContextTitle() { return &MATROSKA_ContextTitle; }
ebml_context MATROSKA_ContextMuxingApp;
const ebml_context *MATROSKA_getContextMuxingApp() { return &MATROSKA_ContextMuxingApp; }
ebml_context MATROSKA_ContextWritingApp;
const ebml_context *MATROSKA_getContextWritingApp() { return &MATROSKA_ContextWritingApp; }
ebml_context MATROSKA_ContextInfo;
const ebml_context *MATROSKA_getContextInfo() { return &MATROSKA_ContextInfo; }
ebml_context MATROSKA_ContextTimestamp;
const ebml_context *MATROSKA_getContextTimestamp() { return &MATROSKA_ContextTimestamp; }
ebml_context MATROSKA_ContextSilentTrackNumber;
const ebml_context *MATROSKA_getContextSilentTrackNumber() { return &MATROSKA_ContextSilentTrackNumber; }
ebml_context MATROSKA_ContextSilentTracks;
const ebml_context *MATROSKA_getContextSilentTracks() { return &MATROSKA_ContextSilentTracks; }
ebml_context MATROSKA_ContextPosition;
const ebml_context *MATROSKA_getContextPosition() { return &MATROSKA_ContextPosition; }
ebml_context MATROSKA_ContextPrevSize;
const ebml_context *MATROSKA_getContextPrevSize() { return &MATROSKA_ContextPrevSize; }
ebml_context MATROSKA_ContextSimpleBlock;
const ebml_context *MATROSKA_getContextSimpleBlock() { return &MATROSKA_ContextSimpleBlock; }
ebml_context MATROSKA_ContextBlock;
const ebml_context *MATROSKA_getContextBlock() { return &MATROSKA_ContextBlock; }
ebml_context MATROSKA_ContextBlockVirtual;
const ebml_context *MATROSKA_getContextBlockVirtual() { return &MATROSKA_ContextBlockVirtual; }
ebml_context MATROSKA_ContextBlockAddID;
const ebml_context *MATROSKA_getContextBlockAddID() { return &MATROSKA_ContextBlockAddID; }
ebml_context MATROSKA_ContextBlockAdditional;
const ebml_context *MATROSKA_getContextBlockAdditional() { return &MATROSKA_ContextBlockAdditional; }
ebml_context MATROSKA_ContextBlockMore;
const ebml_context *MATROSKA_getContextBlockMore() { return &MATROSKA_ContextBlockMore; }
ebml_context MATROSKA_ContextBlockAdditions;
const ebml_context *MATROSKA_getContextBlockAdditions() { return &MATROSKA_ContextBlockAdditions; }
ebml_context MATROSKA_ContextBlockDuration;
const ebml_context *MATROSKA_getContextBlockDuration() { return &MATROSKA_ContextBlockDuration; }
ebml_context MATROSKA_ContextReferencePriority;
const ebml_context *MATROSKA_getContextReferencePriority() { return &MATROSKA_ContextReferencePriority; }
ebml_context MATROSKA_ContextReferenceBlock;
const ebml_context *MATROSKA_getContextReferenceBlock() { return &MATROSKA_ContextReferenceBlock; }
ebml_context MATROSKA_ContextReferenceVirtual;
const ebml_context *MATROSKA_getContextReferenceVirtual() { return &MATROSKA_ContextReferenceVirtual; }
ebml_context MATROSKA_ContextCodecState;
const ebml_context *MATROSKA_getContextCodecState() { return &MATROSKA_ContextCodecState; }
ebml_context MATROSKA_ContextDiscardPadding;
const ebml_context *MATROSKA_getContextDiscardPadding() { return &MATROSKA_ContextDiscardPadding; }
ebml_context MATROSKA_ContextLaceNumber;
const ebml_context *MATROSKA_getContextLaceNumber() { return &MATROSKA_ContextLaceNumber; }
ebml_context MATROSKA_ContextFrameNumber;
const ebml_context *MATROSKA_getContextFrameNumber() { return &MATROSKA_ContextFrameNumber; }
ebml_context MATROSKA_ContextBlockAdditionID;
const ebml_context *MATROSKA_getContextBlockAdditionID() { return &MATROSKA_ContextBlockAdditionID; }
ebml_context MATROSKA_ContextDelay;
const ebml_context *MATROSKA_getContextDelay() { return &MATROSKA_ContextDelay; }
ebml_context MATROSKA_ContextSliceDuration;
const ebml_context *MATROSKA_getContextSliceDuration() { return &MATROSKA_ContextSliceDuration; }
ebml_context MATROSKA_ContextTimeSlice;
const ebml_context *MATROSKA_getContextTimeSlice() { return &MATROSKA_ContextTimeSlice; }
ebml_context MATROSKA_ContextSlices;
const ebml_context *MATROSKA_getContextSlices() { return &MATROSKA_ContextSlices; }
ebml_context MATROSKA_ContextReferenceOffset;
const ebml_context *MATROSKA_getContextReferenceOffset() { return &MATROSKA_ContextReferenceOffset; }
ebml_context MATROSKA_ContextReferenceTimestamp;
const ebml_context *MATROSKA_getContextReferenceTimestamp() { return &MATROSKA_ContextReferenceTimestamp; }
ebml_context MATROSKA_ContextReferenceFrame;
const ebml_context *MATROSKA_getContextReferenceFrame() { return &MATROSKA_ContextReferenceFrame; }
ebml_context MATROSKA_ContextBlockGroup;
const ebml_context *MATROSKA_getContextBlockGroup() { return &MATROSKA_ContextBlockGroup; }
ebml_context MATROSKA_ContextEncryptedBlock;
const ebml_context *MATROSKA_getContextEncryptedBlock() { return &MATROSKA_ContextEncryptedBlock; }
ebml_context MATROSKA_ContextCluster;
const ebml_context *MATROSKA_getContextCluster() { return &MATROSKA_ContextCluster; }
ebml_context MATROSKA_ContextTrackNumber;
const ebml_context *MATROSKA_getContextTrackNumber() { return &MATROSKA_ContextTrackNumber; }
ebml_context MATROSKA_ContextTrackUID;
const ebml_context *MATROSKA_getContextTrackUID() { return &MATROSKA_ContextTrackUID; }
ebml_context MATROSKA_ContextTrackType;
const ebml_context *MATROSKA_getContextTrackType() { return &MATROSKA_ContextTrackType; }
ebml_context MATROSKA_ContextFlagEnabled;
const ebml_context *MATROSKA_getContextFlagEnabled() { return &MATROSKA_ContextFlagEnabled; }
ebml_context MATROSKA_ContextFlagDefault;
const ebml_context *MATROSKA_getContextFlagDefault() { return &MATROSKA_ContextFlagDefault; }
ebml_context MATROSKA_ContextFlagForced;
const ebml_context *MATROSKA_getContextFlagForced() { return &MATROSKA_ContextFlagForced; }
ebml_context MATROSKA_ContextFlagHearingImpaired;
const ebml_context *MATROSKA_getContextFlagHearingImpaired() { return &MATROSKA_ContextFlagHearingImpaired; }
ebml_context MATROSKA_ContextFlagVisualImpaired;
const ebml_context *MATROSKA_getContextFlagVisualImpaired() { return &MATROSKA_ContextFlagVisualImpaired; }
ebml_context MATROSKA_ContextFlagTextDescriptions;
const ebml_context *MATROSKA_getContextFlagTextDescriptions() { return &MATROSKA_ContextFlagTextDescriptions; }
ebml_context MATROSKA_ContextFlagOriginal;
const ebml_context *MATROSKA_getContextFlagOriginal() { return &MATROSKA_ContextFlagOriginal; }
ebml_context MATROSKA_ContextFlagCommentary;
const ebml_context *MATROSKA_getContextFlagCommentary() { return &MATROSKA_ContextFlagCommentary; }
ebml_context MATROSKA_ContextFlagLacing;
const ebml_context *MATROSKA_getContextFlagLacing() { return &MATROSKA_ContextFlagLacing; }
ebml_context MATROSKA_ContextMinCache;
const ebml_context *MATROSKA_getContextMinCache() { return &MATROSKA_ContextMinCache; }
ebml_context MATROSKA_ContextMaxCache;
const ebml_context *MATROSKA_getContextMaxCache() { return &MATROSKA_ContextMaxCache; }
ebml_context MATROSKA_ContextDefaultDuration;
const ebml_context *MATROSKA_getContextDefaultDuration() { return &MATROSKA_ContextDefaultDuration; }
ebml_context MATROSKA_ContextDefaultDecodedFieldDuration;
const ebml_context *MATROSKA_getContextDefaultDecodedFieldDuration() { return &MATROSKA_ContextDefaultDecodedFieldDuration; }
ebml_context MATROSKA_ContextTrackTimestampScale;
const ebml_context *MATROSKA_getContextTrackTimestampScale() { return &MATROSKA_ContextTrackTimestampScale; }
ebml_context MATROSKA_ContextTrackOffset;
const ebml_context *MATROSKA_getContextTrackOffset() { return &MATROSKA_ContextTrackOffset; }
ebml_context MATROSKA_ContextMaxBlockAdditionID;
const ebml_context *MATROSKA_getContextMaxBlockAdditionID() { return &MATROSKA_ContextMaxBlockAdditionID; }
ebml_context MATROSKA_ContextBlockAddIDValue;
const ebml_context *MATROSKA_getContextBlockAddIDValue() { return &MATROSKA_ContextBlockAddIDValue; }
ebml_context MATROSKA_ContextBlockAddIDName;
const ebml_context *MATROSKA_getContextBlockAddIDName() { return &MATROSKA_ContextBlockAddIDName; }
ebml_context MATROSKA_ContextBlockAddIDType;
const ebml_context *MATROSKA_getContextBlockAddIDType() { return &MATROSKA_ContextBlockAddIDType; }
ebml_context MATROSKA_ContextBlockAddIDExtraData;
const ebml_context *MATROSKA_getContextBlockAddIDExtraData() { return &MATROSKA_ContextBlockAddIDExtraData; }
ebml_context MATROSKA_ContextBlockAdditionMapping;
const ebml_context *MATROSKA_getContextBlockAdditionMapping() { return &MATROSKA_ContextBlockAdditionMapping; }
ebml_context MATROSKA_ContextName;
const ebml_context *MATROSKA_getContextName() { return &MATROSKA_ContextName; }
ebml_context MATROSKA_ContextLanguage;
const ebml_context *MATROSKA_getContextLanguage() { return &MATROSKA_ContextLanguage; }
ebml_context MATROSKA_ContextLanguageIETF;
const ebml_context *MATROSKA_getContextLanguageIETF() { return &MATROSKA_ContextLanguageIETF; }
ebml_context MATROSKA_ContextCodecID;
const ebml_context *MATROSKA_getContextCodecID() { return &MATROSKA_ContextCodecID; }
ebml_context MATROSKA_ContextCodecPrivate;
const ebml_context *MATROSKA_getContextCodecPrivate() { return &MATROSKA_ContextCodecPrivate; }
ebml_context MATROSKA_ContextCodecName;
const ebml_context *MATROSKA_getContextCodecName() { return &MATROSKA_ContextCodecName; }
ebml_context MATROSKA_ContextAttachmentLink;
const ebml_context *MATROSKA_getContextAttachmentLink() { return &MATROSKA_ContextAttachmentLink; }
ebml_context MATROSKA_ContextCodecSettings;
const ebml_context *MATROSKA_getContextCodecSettings() { return &MATROSKA_ContextCodecSettings; }
ebml_context MATROSKA_ContextCodecInfoURL;
const ebml_context *MATROSKA_getContextCodecInfoURL() { return &MATROSKA_ContextCodecInfoURL; }
ebml_context MATROSKA_ContextCodecDownloadURL;
const ebml_context *MATROSKA_getContextCodecDownloadURL() { return &MATROSKA_ContextCodecDownloadURL; }
ebml_context MATROSKA_ContextCodecDecodeAll;
const ebml_context *MATROSKA_getContextCodecDecodeAll() { return &MATROSKA_ContextCodecDecodeAll; }
ebml_context MATROSKA_ContextTrackOverlay;
const ebml_context *MATROSKA_getContextTrackOverlay() { return &MATROSKA_ContextTrackOverlay; }
ebml_context MATROSKA_ContextCodecDelay;
const ebml_context *MATROSKA_getContextCodecDelay() { return &MATROSKA_ContextCodecDelay; }
ebml_context MATROSKA_ContextSeekPreRoll;
const ebml_context *MATROSKA_getContextSeekPreRoll() { return &MATROSKA_ContextSeekPreRoll; }
ebml_context MATROSKA_ContextTrackTranslateEditionUID;
const ebml_context *MATROSKA_getContextTrackTranslateEditionUID() { return &MATROSKA_ContextTrackTranslateEditionUID; }
ebml_context MATROSKA_ContextTrackTranslateCodec;
const ebml_context *MATROSKA_getContextTrackTranslateCodec() { return &MATROSKA_ContextTrackTranslateCodec; }
ebml_context MATROSKA_ContextTrackTranslateTrackID;
const ebml_context *MATROSKA_getContextTrackTranslateTrackID() { return &MATROSKA_ContextTrackTranslateTrackID; }
ebml_context MATROSKA_ContextTrackTranslate;
const ebml_context *MATROSKA_getContextTrackTranslate() { return &MATROSKA_ContextTrackTranslate; }
ebml_context MATROSKA_ContextFlagInterlaced;
const ebml_context *MATROSKA_getContextFlagInterlaced() { return &MATROSKA_ContextFlagInterlaced; }
ebml_context MATROSKA_ContextFieldOrder;
const ebml_context *MATROSKA_getContextFieldOrder() { return &MATROSKA_ContextFieldOrder; }
ebml_context MATROSKA_ContextStereoMode;
const ebml_context *MATROSKA_getContextStereoMode() { return &MATROSKA_ContextStereoMode; }
ebml_context MATROSKA_ContextAlphaMode;
const ebml_context *MATROSKA_getContextAlphaMode() { return &MATROSKA_ContextAlphaMode; }
ebml_context MATROSKA_ContextOldStereoMode;
const ebml_context *MATROSKA_getContextOldStereoMode() { return &MATROSKA_ContextOldStereoMode; }
ebml_context MATROSKA_ContextPixelWidth;
const ebml_context *MATROSKA_getContextPixelWidth() { return &MATROSKA_ContextPixelWidth; }
ebml_context MATROSKA_ContextPixelHeight;
const ebml_context *MATROSKA_getContextPixelHeight() { return &MATROSKA_ContextPixelHeight; }
ebml_context MATROSKA_ContextPixelCropBottom;
const ebml_context *MATROSKA_getContextPixelCropBottom() { return &MATROSKA_ContextPixelCropBottom; }
ebml_context MATROSKA_ContextPixelCropTop;
const ebml_context *MATROSKA_getContextPixelCropTop() { return &MATROSKA_ContextPixelCropTop; }
ebml_context MATROSKA_ContextPixelCropLeft;
const ebml_context *MATROSKA_getContextPixelCropLeft() { return &MATROSKA_ContextPixelCropLeft; }
ebml_context MATROSKA_ContextPixelCropRight;
const ebml_context *MATROSKA_getContextPixelCropRight() { return &MATROSKA_ContextPixelCropRight; }
ebml_context MATROSKA_ContextDisplayWidth;
const ebml_context *MATROSKA_getContextDisplayWidth() { return &MATROSKA_ContextDisplayWidth; }
ebml_context MATROSKA_ContextDisplayHeight;
const ebml_context *MATROSKA_getContextDisplayHeight() { return &MATROSKA_ContextDisplayHeight; }
ebml_context MATROSKA_ContextDisplayUnit;
const ebml_context *MATROSKA_getContextDisplayUnit() { return &MATROSKA_ContextDisplayUnit; }
ebml_context MATROSKA_ContextAspectRatioType;
const ebml_context *MATROSKA_getContextAspectRatioType() { return &MATROSKA_ContextAspectRatioType; }
ebml_context MATROSKA_ContextColourSpace;
const ebml_context *MATROSKA_getContextColourSpace() { return &MATROSKA_ContextColourSpace; }
ebml_context MATROSKA_ContextGammaValue;
const ebml_context *MATROSKA_getContextGammaValue() { return &MATROSKA_ContextGammaValue; }
ebml_context MATROSKA_ContextFrameRate;
const ebml_context *MATROSKA_getContextFrameRate() { return &MATROSKA_ContextFrameRate; }
ebml_context MATROSKA_ContextMatrixCoefficients;
const ebml_context *MATROSKA_getContextMatrixCoefficients() { return &MATROSKA_ContextMatrixCoefficients; }
ebml_context MATROSKA_ContextBitsPerChannel;
const ebml_context *MATROSKA_getContextBitsPerChannel() { return &MATROSKA_ContextBitsPerChannel; }
ebml_context MATROSKA_ContextChromaSubsamplingHorz;
const ebml_context *MATROSKA_getContextChromaSubsamplingHorz() { return &MATROSKA_ContextChromaSubsamplingHorz; }
ebml_context MATROSKA_ContextChromaSubsamplingVert;
const ebml_context *MATROSKA_getContextChromaSubsamplingVert() { return &MATROSKA_ContextChromaSubsamplingVert; }
ebml_context MATROSKA_ContextCbSubsamplingHorz;
const ebml_context *MATROSKA_getContextCbSubsamplingHorz() { return &MATROSKA_ContextCbSubsamplingHorz; }
ebml_context MATROSKA_ContextCbSubsamplingVert;
const ebml_context *MATROSKA_getContextCbSubsamplingVert() { return &MATROSKA_ContextCbSubsamplingVert; }
ebml_context MATROSKA_ContextChromaSitingHorz;
const ebml_context *MATROSKA_getContextChromaSitingHorz() { return &MATROSKA_ContextChromaSitingHorz; }
ebml_context MATROSKA_ContextChromaSitingVert;
const ebml_context *MATROSKA_getContextChromaSitingVert() { return &MATROSKA_ContextChromaSitingVert; }
ebml_context MATROSKA_ContextRange;
const ebml_context *MATROSKA_getContextRange() { return &MATROSKA_ContextRange; }
ebml_context MATROSKA_ContextTransferCharacteristics;
const ebml_context *MATROSKA_getContextTransferCharacteristics() { return &MATROSKA_ContextTransferCharacteristics; }
ebml_context MATROSKA_ContextPrimaries;
const ebml_context *MATROSKA_getContextPrimaries() { return &MATROSKA_ContextPrimaries; }
ebml_context MATROSKA_ContextMaxCLL;
const ebml_context *MATROSKA_getContextMaxCLL() { return &MATROSKA_ContextMaxCLL; }
ebml_context MATROSKA_ContextMaxFALL;
const ebml_context *MATROSKA_getContextMaxFALL() { return &MATROSKA_ContextMaxFALL; }
ebml_context MATROSKA_ContextPrimaryRChromaticityX;
const ebml_context *MATROSKA_getContextPrimaryRChromaticityX() { return &MATROSKA_ContextPrimaryRChromaticityX; }
ebml_context MATROSKA_ContextPrimaryRChromaticityY;
const ebml_context *MATROSKA_getContextPrimaryRChromaticityY() { return &MATROSKA_ContextPrimaryRChromaticityY; }
ebml_context MATROSKA_ContextPrimaryGChromaticityX;
const ebml_context *MATROSKA_getContextPrimaryGChromaticityX() { return &MATROSKA_ContextPrimaryGChromaticityX; }
ebml_context MATROSKA_ContextPrimaryGChromaticityY;
const ebml_context *MATROSKA_getContextPrimaryGChromaticityY() { return &MATROSKA_ContextPrimaryGChromaticityY; }
ebml_context MATROSKA_ContextPrimaryBChromaticityX;
const ebml_context *MATROSKA_getContextPrimaryBChromaticityX() { return &MATROSKA_ContextPrimaryBChromaticityX; }
ebml_context MATROSKA_ContextPrimaryBChromaticityY;
const ebml_context *MATROSKA_getContextPrimaryBChromaticityY() { return &MATROSKA_ContextPrimaryBChromaticityY; }
ebml_context MATROSKA_ContextWhitePointChromaticityX;
const ebml_context *MATROSKA_getContextWhitePointChromaticityX() { return &MATROSKA_ContextWhitePointChromaticityX; }
ebml_context MATROSKA_ContextWhitePointChromaticityY;
const ebml_context *MATROSKA_getContextWhitePointChromaticityY() { return &MATROSKA_ContextWhitePointChromaticityY; }
ebml_context MATROSKA_ContextLuminanceMax;
const ebml_context *MATROSKA_getContextLuminanceMax() { return &MATROSKA_ContextLuminanceMax; }
ebml_context MATROSKA_ContextLuminanceMin;
const ebml_context *MATROSKA_getContextLuminanceMin() { return &MATROSKA_ContextLuminanceMin; }
ebml_context MATROSKA_ContextMasteringMetadata;
const ebml_context *MATROSKA_getContextMasteringMetadata() { return &MATROSKA_ContextMasteringMetadata; }
ebml_context MATROSKA_ContextColour;
const ebml_context *MATROSKA_getContextColour() { return &MATROSKA_ContextColour; }
ebml_context MATROSKA_ContextProjectionType;
const ebml_context *MATROSKA_getContextProjectionType() { return &MATROSKA_ContextProjectionType; }
ebml_context MATROSKA_ContextProjectionPrivate;
const ebml_context *MATROSKA_getContextProjectionPrivate() { return &MATROSKA_ContextProjectionPrivate; }
ebml_context MATROSKA_ContextProjectionPoseYaw;
const ebml_context *MATROSKA_getContextProjectionPoseYaw() { return &MATROSKA_ContextProjectionPoseYaw; }
ebml_context MATROSKA_ContextProjectionPosePitch;
const ebml_context *MATROSKA_getContextProjectionPosePitch() { return &MATROSKA_ContextProjectionPosePitch; }
ebml_context MATROSKA_ContextProjectionPoseRoll;
const ebml_context *MATROSKA_getContextProjectionPoseRoll() { return &MATROSKA_ContextProjectionPoseRoll; }
ebml_context MATROSKA_ContextProjection;
const ebml_context *MATROSKA_getContextProjection() { return &MATROSKA_ContextProjection; }
ebml_context MATROSKA_ContextVideo;
const ebml_context *MATROSKA_getContextVideo() { return &MATROSKA_ContextVideo; }
ebml_context MATROSKA_ContextSamplingFrequency;
const ebml_context *MATROSKA_getContextSamplingFrequency() { return &MATROSKA_ContextSamplingFrequency; }
ebml_context MATROSKA_ContextOutputSamplingFrequency;
const ebml_context *MATROSKA_getContextOutputSamplingFrequency() { return &MATROSKA_ContextOutputSamplingFrequency; }
ebml_context MATROSKA_ContextChannels;
const ebml_context *MATROSKA_getContextChannels() { return &MATROSKA_ContextChannels; }
ebml_context MATROSKA_ContextChannelPositions;
const ebml_context *MATROSKA_getContextChannelPositions() { return &MATROSKA_ContextChannelPositions; }
ebml_context MATROSKA_ContextBitDepth;
const ebml_context *MATROSKA_getContextBitDepth() { return &MATROSKA_ContextBitDepth; }
ebml_context MATROSKA_ContextAudio;
const ebml_context *MATROSKA_getContextAudio() { return &MATROSKA_ContextAudio; }
ebml_context MATROSKA_ContextTrackPlaneUID;
const ebml_context *MATROSKA_getContextTrackPlaneUID() { return &MATROSKA_ContextTrackPlaneUID; }
ebml_context MATROSKA_ContextTrackPlaneType;
const ebml_context *MATROSKA_getContextTrackPlaneType() { return &MATROSKA_ContextTrackPlaneType; }
ebml_context MATROSKA_ContextTrackPlane;
const ebml_context *MATROSKA_getContextTrackPlane() { return &MATROSKA_ContextTrackPlane; }
ebml_context MATROSKA_ContextTrackCombinePlanes;
const ebml_context *MATROSKA_getContextTrackCombinePlanes() { return &MATROSKA_ContextTrackCombinePlanes; }
ebml_context MATROSKA_ContextTrackJoinUID;
const ebml_context *MATROSKA_getContextTrackJoinUID() { return &MATROSKA_ContextTrackJoinUID; }
ebml_context MATROSKA_ContextTrackJoinBlocks;
const ebml_context *MATROSKA_getContextTrackJoinBlocks() { return &MATROSKA_ContextTrackJoinBlocks; }
ebml_context MATROSKA_ContextTrackOperation;
const ebml_context *MATROSKA_getContextTrackOperation() { return &MATROSKA_ContextTrackOperation; }
ebml_context MATROSKA_ContextTrickTrackUID;
const ebml_context *MATROSKA_getContextTrickTrackUID() { return &MATROSKA_ContextTrickTrackUID; }
ebml_context MATROSKA_ContextTrickTrackSegmentUID;
const ebml_context *MATROSKA_getContextTrickTrackSegmentUID() { return &MATROSKA_ContextTrickTrackSegmentUID; }
ebml_context MATROSKA_ContextTrickTrackFlag;
const ebml_context *MATROSKA_getContextTrickTrackFlag() { return &MATROSKA_ContextTrickTrackFlag; }
ebml_context MATROSKA_ContextTrickMasterTrackUID;
const ebml_context *MATROSKA_getContextTrickMasterTrackUID() { return &MATROSKA_ContextTrickMasterTrackUID; }
ebml_context MATROSKA_ContextTrickMasterTrackSegmentUID;
const ebml_context *MATROSKA_getContextTrickMasterTrackSegmentUID() { return &MATROSKA_ContextTrickMasterTrackSegmentUID; }
ebml_context MATROSKA_ContextContentEncodingOrder;
const ebml_context *MATROSKA_getContextContentEncodingOrder() { return &MATROSKA_ContextContentEncodingOrder; }
ebml_context MATROSKA_ContextContentEncodingScope;
const ebml_context *MATROSKA_getContextContentEncodingScope() { return &MATROSKA_ContextContentEncodingScope; }
ebml_context MATROSKA_ContextContentEncodingType;
const ebml_context *MATROSKA_getContextContentEncodingType() { return &MATROSKA_ContextContentEncodingType; }
ebml_context MATROSKA_ContextContentCompAlgo;
const ebml_context *MATROSKA_getContextContentCompAlgo() { return &MATROSKA_ContextContentCompAlgo; }
ebml_context MATROSKA_ContextContentCompSettings;
const ebml_context *MATROSKA_getContextContentCompSettings() { return &MATROSKA_ContextContentCompSettings; }
ebml_context MATROSKA_ContextContentCompression;
const ebml_context *MATROSKA_getContextContentCompression() { return &MATROSKA_ContextContentCompression; }
ebml_context MATROSKA_ContextContentEncAlgo;
const ebml_context *MATROSKA_getContextContentEncAlgo() { return &MATROSKA_ContextContentEncAlgo; }
ebml_context MATROSKA_ContextContentEncKeyID;
const ebml_context *MATROSKA_getContextContentEncKeyID() { return &MATROSKA_ContextContentEncKeyID; }
ebml_context MATROSKA_ContextAESSettingsCipherMode;
const ebml_context *MATROSKA_getContextAESSettingsCipherMode() { return &MATROSKA_ContextAESSettingsCipherMode; }
ebml_context MATROSKA_ContextContentEncAESSettings;
const ebml_context *MATROSKA_getContextContentEncAESSettings() { return &MATROSKA_ContextContentEncAESSettings; }
ebml_context MATROSKA_ContextContentSignature;
const ebml_context *MATROSKA_getContextContentSignature() { return &MATROSKA_ContextContentSignature; }
ebml_context MATROSKA_ContextContentSigKeyID;
const ebml_context *MATROSKA_getContextContentSigKeyID() { return &MATROSKA_ContextContentSigKeyID; }
ebml_context MATROSKA_ContextContentSigAlgo;
const ebml_context *MATROSKA_getContextContentSigAlgo() { return &MATROSKA_ContextContentSigAlgo; }
ebml_context MATROSKA_ContextContentSigHashAlgo;
const ebml_context *MATROSKA_getContextContentSigHashAlgo() { return &MATROSKA_ContextContentSigHashAlgo; }
ebml_context MATROSKA_ContextContentEncryption;
const ebml_context *MATROSKA_getContextContentEncryption() { return &MATROSKA_ContextContentEncryption; }
ebml_context MATROSKA_ContextContentEncoding;
const ebml_context *MATROSKA_getContextContentEncoding() { return &MATROSKA_ContextContentEncoding; }
ebml_context MATROSKA_ContextContentEncodings;
const ebml_context *MATROSKA_getContextContentEncodings() { return &MATROSKA_ContextContentEncodings; }
ebml_context MATROSKA_ContextTrackEntry;
const ebml_context *MATROSKA_getContextTrackEntry() { return &MATROSKA_ContextTrackEntry; }
ebml_context MATROSKA_ContextTracks;
const ebml_context *MATROSKA_getContextTracks() { return &MATROSKA_ContextTracks; }
ebml_context MATROSKA_ContextCueTime;
const ebml_context *MATROSKA_getContextCueTime() { return &MATROSKA_ContextCueTime; }
ebml_context MATROSKA_ContextCueTrack;
const ebml_context *MATROSKA_getContextCueTrack() { return &MATROSKA_ContextCueTrack; }
ebml_context MATROSKA_ContextCueClusterPosition;
const ebml_context *MATROSKA_getContextCueClusterPosition() { return &MATROSKA_ContextCueClusterPosition; }
ebml_context MATROSKA_ContextCueRelativePosition;
const ebml_context *MATROSKA_getContextCueRelativePosition() { return &MATROSKA_ContextCueRelativePosition; }
ebml_context MATROSKA_ContextCueDuration;
const ebml_context *MATROSKA_getContextCueDuration() { return &MATROSKA_ContextCueDuration; }
ebml_context MATROSKA_ContextCueBlockNumber;
const ebml_context *MATROSKA_getContextCueBlockNumber() { return &MATROSKA_ContextCueBlockNumber; }
ebml_context MATROSKA_ContextCueCodecState;
const ebml_context *MATROSKA_getContextCueCodecState() { return &MATROSKA_ContextCueCodecState; }
ebml_context MATROSKA_ContextCueRefTime;
const ebml_context *MATROSKA_getContextCueRefTime() { return &MATROSKA_ContextCueRefTime; }
ebml_context MATROSKA_ContextCueRefCluster;
const ebml_context *MATROSKA_getContextCueRefCluster() { return &MATROSKA_ContextCueRefCluster; }
ebml_context MATROSKA_ContextCueRefNumber;
const ebml_context *MATROSKA_getContextCueRefNumber() { return &MATROSKA_ContextCueRefNumber; }
ebml_context MATROSKA_ContextCueRefCodecState;
const ebml_context *MATROSKA_getContextCueRefCodecState() { return &MATROSKA_ContextCueRefCodecState; }
ebml_context MATROSKA_ContextCueReference;
const ebml_context *MATROSKA_getContextCueReference() { return &MATROSKA_ContextCueReference; }
ebml_context MATROSKA_ContextCueTrackPositions;
const ebml_context *MATROSKA_getContextCueTrackPositions() { return &MATROSKA_ContextCueTrackPositions; }
ebml_context MATROSKA_ContextCuePoint;
const ebml_context *MATROSKA_getContextCuePoint() { return &MATROSKA_ContextCuePoint; }
ebml_context MATROSKA_ContextCues;
const ebml_context *MATROSKA_getContextCues() { return &MATROSKA_ContextCues; }
ebml_context MATROSKA_ContextFileDescription;
const ebml_context *MATROSKA_getContextFileDescription() { return &MATROSKA_ContextFileDescription; }
ebml_context MATROSKA_ContextFileName;
const ebml_context *MATROSKA_getContextFileName() { return &MATROSKA_ContextFileName; }
ebml_context MATROSKA_ContextFileMimeType;
const ebml_context *MATROSKA_getContextFileMimeType() { return &MATROSKA_ContextFileMimeType; }
ebml_context MATROSKA_ContextFileData;
const ebml_context *MATROSKA_getContextFileData() { return &MATROSKA_ContextFileData; }
ebml_context MATROSKA_ContextFileUID;
const ebml_context *MATROSKA_getContextFileUID() { return &MATROSKA_ContextFileUID; }
ebml_context MATROSKA_ContextFileReferral;
const ebml_context *MATROSKA_getContextFileReferral() { return &MATROSKA_ContextFileReferral; }
ebml_context MATROSKA_ContextFileUsedStartTime;
const ebml_context *MATROSKA_getContextFileUsedStartTime() { return &MATROSKA_ContextFileUsedStartTime; }
ebml_context MATROSKA_ContextFileUsedEndTime;
const ebml_context *MATROSKA_getContextFileUsedEndTime() { return &MATROSKA_ContextFileUsedEndTime; }
ebml_context MATROSKA_ContextAttachedFile;
const ebml_context *MATROSKA_getContextAttachedFile() { return &MATROSKA_ContextAttachedFile; }
ebml_context MATROSKA_ContextAttachments;
const ebml_context *MATROSKA_getContextAttachments() { return &MATROSKA_ContextAttachments; }
ebml_context MATROSKA_ContextEditionUID;
const ebml_context *MATROSKA_getContextEditionUID() { return &MATROSKA_ContextEditionUID; }
ebml_context MATROSKA_ContextEditionFlagHidden;
const ebml_context *MATROSKA_getContextEditionFlagHidden() { return &MATROSKA_ContextEditionFlagHidden; }
ebml_context MATROSKA_ContextEditionFlagDefault;
const ebml_context *MATROSKA_getContextEditionFlagDefault() { return &MATROSKA_ContextEditionFlagDefault; }
ebml_context MATROSKA_ContextEditionFlagOrdered;
const ebml_context *MATROSKA_getContextEditionFlagOrdered() { return &MATROSKA_ContextEditionFlagOrdered; }
ebml_context MATROSKA_ContextChapterUID;
const ebml_context *MATROSKA_getContextChapterUID() { return &MATROSKA_ContextChapterUID; }
ebml_context MATROSKA_ContextChapterStringUID;
const ebml_context *MATROSKA_getContextChapterStringUID() { return &MATROSKA_ContextChapterStringUID; }
ebml_context MATROSKA_ContextChapterTimeStart;
const ebml_context *MATROSKA_getContextChapterTimeStart() { return &MATROSKA_ContextChapterTimeStart; }
ebml_context MATROSKA_ContextChapterTimeEnd;
const ebml_context *MATROSKA_getContextChapterTimeEnd() { return &MATROSKA_ContextChapterTimeEnd; }
ebml_context MATROSKA_ContextChapterFlagHidden;
const ebml_context *MATROSKA_getContextChapterFlagHidden() { return &MATROSKA_ContextChapterFlagHidden; }
ebml_context MATROSKA_ContextChapterFlagEnabled;
const ebml_context *MATROSKA_getContextChapterFlagEnabled() { return &MATROSKA_ContextChapterFlagEnabled; }
ebml_context MATROSKA_ContextChapterSegmentUID;
const ebml_context *MATROSKA_getContextChapterSegmentUID() { return &MATROSKA_ContextChapterSegmentUID; }
ebml_context MATROSKA_ContextChapterSegmentEditionUID;
const ebml_context *MATROSKA_getContextChapterSegmentEditionUID() { return &MATROSKA_ContextChapterSegmentEditionUID; }
ebml_context MATROSKA_ContextChapterPhysicalEquiv;
const ebml_context *MATROSKA_getContextChapterPhysicalEquiv() { return &MATROSKA_ContextChapterPhysicalEquiv; }
ebml_context MATROSKA_ContextChapterTrackNumber;
const ebml_context *MATROSKA_getContextChapterTrackNumber() { return &MATROSKA_ContextChapterTrackNumber; }
ebml_context MATROSKA_ContextChapterTrack;
const ebml_context *MATROSKA_getContextChapterTrack() { return &MATROSKA_ContextChapterTrack; }
ebml_context MATROSKA_ContextChapString;
const ebml_context *MATROSKA_getContextChapString() { return &MATROSKA_ContextChapString; }
ebml_context MATROSKA_ContextChapLanguage;
const ebml_context *MATROSKA_getContextChapLanguage() { return &MATROSKA_ContextChapLanguage; }
ebml_context MATROSKA_ContextChapLanguageIETF;
const ebml_context *MATROSKA_getContextChapLanguageIETF() { return &MATROSKA_ContextChapLanguageIETF; }
ebml_context MATROSKA_ContextChapCountry;
const ebml_context *MATROSKA_getContextChapCountry() { return &MATROSKA_ContextChapCountry; }
ebml_context MATROSKA_ContextChapterDisplay;
const ebml_context *MATROSKA_getContextChapterDisplay() { return &MATROSKA_ContextChapterDisplay; }
ebml_context MATROSKA_ContextChapProcessCodecID;
const ebml_context *MATROSKA_getContextChapProcessCodecID() { return &MATROSKA_ContextChapProcessCodecID; }
ebml_context MATROSKA_ContextChapProcessPrivate;
const ebml_context *MATROSKA_getContextChapProcessPrivate() { return &MATROSKA_ContextChapProcessPrivate; }
ebml_context MATROSKA_ContextChapProcessTime;
const ebml_context *MATROSKA_getContextChapProcessTime() { return &MATROSKA_ContextChapProcessTime; }
ebml_context MATROSKA_ContextChapProcessData;
const ebml_context *MATROSKA_getContextChapProcessData() { return &MATROSKA_ContextChapProcessData; }
ebml_context MATROSKA_ContextChapProcessCommand;
const ebml_context *MATROSKA_getContextChapProcessCommand() { return &MATROSKA_ContextChapProcessCommand; }
ebml_context MATROSKA_ContextChapProcess;
const ebml_context *MATROSKA_getContextChapProcess() { return &MATROSKA_ContextChapProcess; }
ebml_context MATROSKA_ContextChapterAtom;
const ebml_context *MATROSKA_getContextChapterAtom() { return &MATROSKA_ContextChapterAtom; }
ebml_context MATROSKA_ContextEditionEntry;
const ebml_context *MATROSKA_getContextEditionEntry() { return &MATROSKA_ContextEditionEntry; }
ebml_context MATROSKA_ContextChapters;
const ebml_context *MATROSKA_getContextChapters() { return &MATROSKA_ContextChapters; }
ebml_context MATROSKA_ContextTargetTypeValue;
const ebml_context *MATROSKA_getContextTargetTypeValue() { return &MATROSKA_ContextTargetTypeValue; }
ebml_context MATROSKA_ContextTargetType;
const ebml_context *MATROSKA_getContextTargetType() { return &MATROSKA_ContextTargetType; }
ebml_context MATROSKA_ContextTagTrackUID;
const ebml_context *MATROSKA_getContextTagTrackUID() { return &MATROSKA_ContextTagTrackUID; }
ebml_context MATROSKA_ContextTagEditionUID;
const ebml_context *MATROSKA_getContextTagEditionUID() { return &MATROSKA_ContextTagEditionUID; }
ebml_context MATROSKA_ContextTagChapterUID;
const ebml_context *MATROSKA_getContextTagChapterUID() { return &MATROSKA_ContextTagChapterUID; }
ebml_context MATROSKA_ContextTagAttachmentUID;
const ebml_context *MATROSKA_getContextTagAttachmentUID() { return &MATROSKA_ContextTagAttachmentUID; }
ebml_context MATROSKA_ContextTargets;
const ebml_context *MATROSKA_getContextTargets() { return &MATROSKA_ContextTargets; }
ebml_context MATROSKA_ContextTagName;
const ebml_context *MATROSKA_getContextTagName() { return &MATROSKA_ContextTagName; }
ebml_context MATROSKA_ContextTagLanguage;
const ebml_context *MATROSKA_getContextTagLanguage() { return &MATROSKA_ContextTagLanguage; }
ebml_context MATROSKA_ContextTagLanguageIETF;
const ebml_context *MATROSKA_getContextTagLanguageIETF() { return &MATROSKA_ContextTagLanguageIETF; }
ebml_context MATROSKA_ContextTagDefault;
const ebml_context *MATROSKA_getContextTagDefault() { return &MATROSKA_ContextTagDefault; }
ebml_context MATROSKA_ContextTagString;
const ebml_context *MATROSKA_getContextTagString() { return &MATROSKA_ContextTagString; }
ebml_context MATROSKA_ContextTagBinary;
const ebml_context *MATROSKA_getContextTagBinary() { return &MATROSKA_ContextTagBinary; }
ebml_context MATROSKA_ContextSimpleTag;
const ebml_context *MATROSKA_getContextSimpleTag() { return &MATROSKA_ContextSimpleTag; }
ebml_context MATROSKA_ContextTag;
const ebml_context *MATROSKA_getContextTag() { return &MATROSKA_ContextTag; }
ebml_context MATROSKA_ContextTags;
const ebml_context *MATROSKA_getContextTags() { return &MATROSKA_ContextTags; }
ebml_context MATROSKA_ContextSegment;
const ebml_context *MATROSKA_getContextSegment() { return &MATROSKA_ContextSegment; }

const ebml_semantic EBML_SemanticSeek[] = {
    {1, 1, &MATROSKA_ContextSeekID, 0},
    {1, 1, &MATROSKA_ContextSeekPosition, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticSeekHead[] = {
    {1, 0, &MATROSKA_ContextSeek, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticChapterTranslate[] = {
    {0, 0, &MATROSKA_ContextChapterTranslateEditionUID, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapterTranslateCodec, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapterTranslateID, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticInfo[] = {
    {0, 1, &MATROSKA_ContextSegmentUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextSegmentFilename, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrevUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrevFilename, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextNextUID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextNextFilename, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextSegmentFamily, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextChapterTranslate, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTimestampScale, 0},
    {0, 1, &MATROSKA_ContextDuration, 0},
    {0, 1, &MATROSKA_ContextDateUTC, 0},
    {0, 1, &MATROSKA_ContextTitle, 0},
    {1, 1, &MATROSKA_ContextMuxingApp, 0},
    {1, 1, &MATROSKA_ContextWritingApp, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticSilentTracks[] = {
    {0, 0, &MATROSKA_ContextSilentTrackNumber, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticBlockMore[] = {
    {1, 1, &MATROSKA_ContextBlockAddID, 0},
    {1, 1, &MATROSKA_ContextBlockAdditional, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticBlockAdditions[] = {
    {1, 0, &MATROSKA_ContextBlockMore, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTimeSlice[] = {
    {0, 1, &MATROSKA_ContextLaceNumber, PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFrameNumber, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBlockAdditionID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextDelay, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextSliceDuration, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticSlices[] = {
    {0, 0, &MATROSKA_ContextTimeSlice, PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticReferenceFrame[] = {
    {1, 1, &MATROSKA_ContextReferenceOffset, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextReferenceTimestamp, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticBlockGroup[] = {
    {1, 1, &MATROSKA_ContextBlock, 0},
    {0, 1, &MATROSKA_ContextBlockVirtual, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBlockAdditions, 0},
    {0, 1, &MATROSKA_ContextBlockDuration, 0},
    {1, 1, &MATROSKA_ContextReferencePriority, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextReferenceBlock, 0},
    {0, 1, &MATROSKA_ContextReferenceVirtual, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCodecState, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextDiscardPadding, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextSlices, PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextReferenceFrame, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticCluster[] = {
    {1, 1, &MATROSKA_ContextTimestamp, 0},
    {0, 1, &MATROSKA_ContextSilentTracks, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPosition, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrevSize, 0},
    {0, 0, &MATROSKA_ContextSimpleBlock, PROFILE_MATROSKA_V1},
    {0, 0, &MATROSKA_ContextBlockGroup, 0},
    {0, 0, &MATROSKA_ContextEncryptedBlock, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticBlockAdditionMapping[] = {
    {0, 1, &MATROSKA_ContextBlockAddIDValue, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBlockAddIDName, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextBlockAddIDType, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBlockAddIDExtraData, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTrackTranslate[] = {
    {0, 0, &MATROSKA_ContextTrackTranslateEditionUID, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackTranslateCodec, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackTranslateTrackID, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticMasteringMetadata[] = {
    {0, 1, &MATROSKA_ContextPrimaryRChromaticityX, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrimaryRChromaticityY, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrimaryGChromaticityX, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrimaryGChromaticityY, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrimaryBChromaticityX, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrimaryBChromaticityY, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextWhitePointChromaticityX, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextWhitePointChromaticityY, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextLuminanceMax, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextLuminanceMin, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticColour[] = {
    {0, 1, &MATROSKA_ContextMatrixCoefficients, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBitsPerChannel, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChromaSubsamplingHorz, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChromaSubsamplingVert, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCbSubsamplingHorz, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCbSubsamplingVert, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChromaSitingHorz, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChromaSitingVert, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextRange, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTransferCharacteristics, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextPrimaries, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextMaxCLL, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextMaxFALL, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextMasteringMetadata, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticProjection[] = {
    {1, 1, &MATROSKA_ContextProjectionType, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextProjectionPrivate, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextProjectionPoseYaw, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextProjectionPosePitch, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextProjectionPoseRoll, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticVideo[] = {
    {1, 1, &MATROSKA_ContextFlagInterlaced, PROFILE_MATROSKA_V1|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextFieldOrder, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
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
    {0, 1, &MATROSKA_ContextColour, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextProjection, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticAudio[] = {
    {1, 1, &MATROSKA_ContextSamplingFrequency, 0},
    {0, 1, &MATROSKA_ContextOutputSamplingFrequency, 0},
    {1, 1, &MATROSKA_ContextChannels, 0},
    {0, 1, &MATROSKA_ContextChannelPositions, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextBitDepth, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTrackPlane[] = {
    {1, 1, &MATROSKA_ContextTrackPlaneUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackPlaneType, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTrackCombinePlanes[] = {
    {1, 0, &MATROSKA_ContextTrackPlane, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTrackJoinBlocks[] = {
    {1, 0, &MATROSKA_ContextTrackJoinUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTrackOperation[] = {
    {0, 1, &MATROSKA_ContextTrackCombinePlanes, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrackJoinBlocks, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticContentCompression[] = {
    {1, 1, &MATROSKA_ContextContentCompAlgo, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentCompSettings, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticContentEncAESSettings[] = {
    {1, 1, &MATROSKA_ContextAESSettingsCipherMode, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticContentEncryption[] = {
    {1, 1, &MATROSKA_ContextContentEncAlgo, 0},
    {0, 1, &MATROSKA_ContextContentEncKeyID, 0},
    {0, 1, &MATROSKA_ContextContentEncAESSettings, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextContentSignature, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentSigKeyID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentSigAlgo, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentSigHashAlgo, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticContentEncoding[] = {
    {1, 1, &MATROSKA_ContextContentEncodingOrder, 0},
    {1, 1, &MATROSKA_ContextContentEncodingScope, 0},
    {1, 1, &MATROSKA_ContextContentEncodingType, 0},
    {0, 1, &MATROSKA_ContextContentCompression, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextContentEncryption, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticContentEncodings[] = {
    {1, 0, &MATROSKA_ContextContentEncoding, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTrackEntry[] = {
    {1, 1, &MATROSKA_ContextTrackNumber, 0},
    {1, 1, &MATROSKA_ContextTrackUID, 0},
    {1, 1, &MATROSKA_ContextTrackType, 0},
    {1, 1, &MATROSKA_ContextFlagEnabled, PROFILE_MATROSKA_V1|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextFlagDefault, 0},
    {1, 1, &MATROSKA_ContextFlagForced, 0},
    {0, 1, &MATROSKA_ContextFlagHearingImpaired, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFlagVisualImpaired, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFlagTextDescriptions, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFlagOriginal, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextFlagCommentary, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextFlagLacing, 0},
    {1, 1, &MATROSKA_ContextMinCache, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextMaxCache, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextDefaultDuration, 0},
    {0, 1, &MATROSKA_ContextDefaultDecodedFieldDuration, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTrackTimestampScale, PROFILE_MATROSKA_V4|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextTrackOffset, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextMaxBlockAdditionID, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextBlockAdditionMapping, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextName, 0},
    {0, 1, &MATROSKA_ContextLanguage, 0},
    {0, 1, &MATROSKA_ContextLanguageIETF, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextCodecID, 0},
    {0, 1, &MATROSKA_ContextCodecPrivate, 0},
    {0, 1, &MATROSKA_ContextCodecName, 0},
    {0, 1, &MATROSKA_ContextAttachmentLink, PROFILE_MATROSKA_V4|PROFILE_WEBM},
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
    {0, 1, &MATROSKA_ContextContentEncodings, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTracks[] = {
    {1, 0, &MATROSKA_ContextTrackEntry, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticCueReference[] = {
    {1, 1, &MATROSKA_ContextCueRefTime, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextCueRefCluster, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCueRefNumber, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextCueRefCodecState, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_MATROSKA_V4|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticCueTrackPositions[] = {
    {1, 1, &MATROSKA_ContextCueTrack, 0},
    {1, 1, &MATROSKA_ContextCueClusterPosition, 0},
    {0, 1, &MATROSKA_ContextCueRelativePosition, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextCueDuration, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX},
    {0, 1, &MATROSKA_ContextCueBlockNumber, 0},
    {0, 1, &MATROSKA_ContextCueCodecState, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextCueReference, PROFILE_MATROSKA_V1|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticCuePoint[] = {
    {1, 1, &MATROSKA_ContextCueTime, 0},
    {1, 0, &MATROSKA_ContextCueTrackPositions, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticCues[] = {
    {1, 0, &MATROSKA_ContextCuePoint, 0},
    {0, 0, NULL ,0} // end of the table
};

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

const ebml_semantic EBML_SemanticAttachments[] = {
    {1, 0, &MATROSKA_ContextAttachedFile, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticChapterTrack[] = {
    {1, 0, &MATROSKA_ContextChapterTrackNumber, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticChapterDisplay[] = {
    {1, 1, &MATROSKA_ContextChapString, 0},
    {1, 0, &MATROSKA_ContextChapLanguage, 0},
    {0, 0, &MATROSKA_ContextChapLanguageIETF, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextChapCountry, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticChapProcessCommand[] = {
    {1, 1, &MATROSKA_ContextChapProcessTime, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextChapProcessData, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticChapProcess[] = {
    {1, 1, &MATROSKA_ContextChapProcessCodecID, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapProcessPrivate, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextChapProcessCommand, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticChapterAtom[] = {
    {0, 0, &MATROSKA_ContextChapterAtom, 0}, // recursive
    {1, 1, &MATROSKA_ContextChapterUID, 0},
    {0, 1, &MATROSKA_ContextChapterStringUID, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_DIVX},
    {1, 1, &MATROSKA_ContextChapterTimeStart, 0},
    {0, 1, &MATROSKA_ContextChapterTimeEnd, 0},
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

const ebml_semantic EBML_SemanticEditionEntry[] = {
    {0, 1, &MATROSKA_ContextEditionUID, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextEditionFlagHidden, PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextEditionFlagDefault, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextEditionFlagOrdered, PROFILE_WEBM},
    {1, 0, &MATROSKA_ContextChapterAtom, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticChapters[] = {
    {1, 0, &MATROSKA_ContextEditionEntry, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTargets[] = {
    {0, 1, &MATROSKA_ContextTargetTypeValue, 0},
    {0, 1, &MATROSKA_ContextTargetType, 0},
    {0, 0, &MATROSKA_ContextTagTrackUID, 0},
    {0, 0, &MATROSKA_ContextTagEditionUID, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextTagChapterUID, PROFILE_WEBM},
    {0, 0, &MATROSKA_ContextTagAttachmentUID, PROFILE_WEBM},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticSimpleTag[] = {
    {0, 0, &MATROSKA_ContextSimpleTag, 0}, // recursive
    {1, 1, &MATROSKA_ContextTagName, 0},
    {1, 1, &MATROSKA_ContextTagLanguage, 0},
    {0, 1, &MATROSKA_ContextTagLanguageIETF, PROFILE_MATROSKA_V1|PROFILE_MATROSKA_V2|PROFILE_MATROSKA_V3|PROFILE_DIVX|PROFILE_WEBM},
    {1, 1, &MATROSKA_ContextTagDefault, 0},
    {0, 1, &MATROSKA_ContextTagString, 0},
    {0, 1, &MATROSKA_ContextTagBinary, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTag[] = {
    {1, 1, &MATROSKA_ContextTargets, 0},
    {1, 0, &MATROSKA_ContextSimpleTag, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticTags[] = {
    {1, 0, &MATROSKA_ContextTag, 0},
    {0, 0, NULL ,0} // end of the table
};

const ebml_semantic EBML_SemanticSegment[] = {
    {0, 0, &MATROSKA_ContextSeekHead, 0},
    {1, 1, &MATROSKA_ContextInfo, 0},
    {0, 0, &MATROSKA_ContextCluster, 0},
    {0, 1, &MATROSKA_ContextTracks, 0},
    {0, 1, &MATROSKA_ContextCues, 0},
    {0, 1, &MATROSKA_ContextAttachments, PROFILE_WEBM},
    {0, 1, &MATROSKA_ContextChapters, 0},
    {0, 0, &MATROSKA_ContextTags, 0},
    {0, 0, NULL ,0} // end of the table
};

void MATROSKA_InitSemantic()
{
    MATROSKA_ContextSeekID = (ebml_context) {0x53AB, EBML_BINARY_CLASS, 0, 0, "SeekID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSeekPosition = (ebml_context) {0x53AC, EBML_INTEGER_CLASS, 0, 0, "SeekPosition", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSeek = (ebml_context) {0x4DBB, MATROSKA_SEEKPOINT_CLASS, 0, 0, "Seek", EBML_SemanticSeek, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSeekHead = (ebml_context) {0x114D9B74, EBML_MASTER_CLASS, 0, 0, "SeekHead", EBML_SemanticSeekHead, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSegmentUID = (ebml_context) {0x73A4, MATROSKA_SEGMENTUID_CLASS, 0, 0, "SegmentUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSegmentFilename = (ebml_context) {0x7384, EBML_UNISTRING_CLASS, 0, 0, "SegmentFilename", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrevUID = (ebml_context) {0x3CB923, MATROSKA_SEGMENTUID_CLASS, 0, 0, "PrevUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrevFilename = (ebml_context) {0x3C83AB, EBML_UNISTRING_CLASS, 0, 0, "PrevFilename", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextNextUID = (ebml_context) {0x3EB923, MATROSKA_SEGMENTUID_CLASS, 0, 0, "NextUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextNextFilename = (ebml_context) {0x3E83BB, EBML_UNISTRING_CLASS, 0, 0, "NextFilename", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSegmentFamily = (ebml_context) {0x4444, EBML_BINARY_CLASS, 0, 0, "SegmentFamily", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTranslateEditionUID = (ebml_context) {0x69FC, EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateEditionUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTranslateCodec = (ebml_context) {0x69BF, EBML_INTEGER_CLASS, 0, 0, "ChapterTranslateCodec", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTranslateID = (ebml_context) {0x69A5, EBML_BINARY_CLASS, 0, 0, "ChapterTranslateID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTranslate = (ebml_context) {0x6924, EBML_MASTER_CLASS, 0, 0, "ChapterTranslate", EBML_SemanticChapterTranslate, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTimestampScale = (ebml_context) {0x2AD7B1, EBML_INTEGER_CLASS, 1, (intptr_t)1000000, "TimestampScale", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDuration = (ebml_context) {0x4489, EBML_FLOAT_CLASS, 0, 0, "Duration", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDateUTC = (ebml_context) {0x4461, EBML_DATE_CLASS, 0, 0, "DateUTC", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTitle = (ebml_context) {0x7BA9, EBML_UNISTRING_CLASS, 0, 0, "Title", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMuxingApp = (ebml_context) {0x4D80, EBML_UNISTRING_CLASS, 0, 0, "MuxingApp", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextWritingApp = (ebml_context) {0x5741, EBML_UNISTRING_CLASS, 0, 0, "WritingApp", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextInfo = (ebml_context) {0x1549A966, EBML_MASTER_CLASS, 0, 0, "Info", EBML_SemanticInfo, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTimestamp = (ebml_context) {0xE7, EBML_INTEGER_CLASS, 0, 0, "Timestamp", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSilentTrackNumber = (ebml_context) {0x58D7, EBML_INTEGER_CLASS, 0, 0, "SilentTrackNumber", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSilentTracks = (ebml_context) {0x5854, EBML_MASTER_CLASS, 0, 0, "SilentTracks", EBML_SemanticSilentTracks, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPosition = (ebml_context) {0xA7, EBML_INTEGER_CLASS, 0, 0, "Position", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrevSize = (ebml_context) {0xAB, EBML_INTEGER_CLASS, 0, 0, "PrevSize", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSimpleBlock = (ebml_context) {0xA3, MATROSKA_BLOCK_CLASS, 0, 0, "SimpleBlock", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlock = (ebml_context) {0xA1, MATROSKA_BLOCK_CLASS, 0, 0, "Block", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockVirtual = (ebml_context) {0xA2, EBML_BINARY_CLASS, 0, 0, "BlockVirtual", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAddID = (ebml_context) {0xEE, EBML_INTEGER_CLASS, 1, (intptr_t)1, "BlockAddID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAdditional = (ebml_context) {0xA5, EBML_BINARY_CLASS, 0, 0, "BlockAdditional", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockMore = (ebml_context) {0xA6, EBML_MASTER_CLASS, 0, 0, "BlockMore", EBML_SemanticBlockMore, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAdditions = (ebml_context) {0x75A1, EBML_MASTER_CLASS, 0, 0, "BlockAdditions", EBML_SemanticBlockAdditions, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockDuration = (ebml_context) {0x9B, EBML_INTEGER_CLASS, 0, 0, "BlockDuration", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextReferencePriority = (ebml_context) {0xFA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ReferencePriority", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextReferenceBlock = (ebml_context) {0xFB, EBML_SINTEGER_CLASS, 0, 0, "ReferenceBlock", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextReferenceVirtual = (ebml_context) {0xFD, EBML_SINTEGER_CLASS, 0, 0, "ReferenceVirtual", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecState = (ebml_context) {0xA4, EBML_BINARY_CLASS, 0, 0, "CodecState", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDiscardPadding = (ebml_context) {0x75A2, EBML_SINTEGER_CLASS, 0, 0, "DiscardPadding", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextLaceNumber = (ebml_context) {0xCC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "LaceNumber", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFrameNumber = (ebml_context) {0xCD, EBML_INTEGER_CLASS, 1, (intptr_t)0, "FrameNumber", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAdditionID = (ebml_context) {0xCB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "BlockAdditionID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDelay = (ebml_context) {0xCE, EBML_INTEGER_CLASS, 1, (intptr_t)0, "Delay", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSliceDuration = (ebml_context) {0xCF, EBML_INTEGER_CLASS, 1, (intptr_t)0, "SliceDuration", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTimeSlice = (ebml_context) {0xE8, EBML_MASTER_CLASS, 0, 0, "TimeSlice", EBML_SemanticTimeSlice, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSlices = (ebml_context) {0x8E, EBML_MASTER_CLASS, 0, 0, "Slices", EBML_SemanticSlices, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextReferenceOffset = (ebml_context) {0xC9, EBML_INTEGER_CLASS, 0, 0, "ReferenceOffset", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextReferenceTimestamp = (ebml_context) {0xCA, EBML_INTEGER_CLASS, 0, 0, "ReferenceTimestamp", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextReferenceFrame = (ebml_context) {0xC8, EBML_MASTER_CLASS, 0, 0, "ReferenceFrame", EBML_SemanticReferenceFrame, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockGroup = (ebml_context) {0xA0, MATROSKA_BLOCKGROUP_CLASS, 0, 0, "BlockGroup", EBML_SemanticBlockGroup, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextEncryptedBlock = (ebml_context) {0xAF, EBML_BINARY_CLASS, 0, 0, "EncryptedBlock", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCluster = (ebml_context) {0x1F43B675, MATROSKA_CLUSTER_CLASS, 0, 0, "Cluster", EBML_SemanticCluster, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackNumber = (ebml_context) {0xD7, EBML_INTEGER_CLASS, 0, 0, "TrackNumber", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackUID = (ebml_context) {0x73C5, EBML_INTEGER_CLASS, 0, 0, "TrackUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackType = (ebml_context) {0x83, EBML_INTEGER_CLASS, 0, 0, "TrackType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagEnabled = (ebml_context) {0xB9, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "FlagEnabled", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagDefault = (ebml_context) {0x88, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "FlagDefault", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagForced = (ebml_context) {0x55AA, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagForced", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagHearingImpaired = (ebml_context) {0x55AB, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagHearingImpaired", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagVisualImpaired = (ebml_context) {0x55AC, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagVisualImpaired", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagTextDescriptions = (ebml_context) {0x55AD, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagTextDescriptions", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagOriginal = (ebml_context) {0x55AE, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagOriginal", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagCommentary = (ebml_context) {0x55AF, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "FlagCommentary", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagLacing = (ebml_context) {0x9C, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "FlagLacing", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMinCache = (ebml_context) {0x6DE7, EBML_INTEGER_CLASS, 1, (intptr_t)0, "MinCache", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMaxCache = (ebml_context) {0x6DF8, EBML_INTEGER_CLASS, 0, 0, "MaxCache", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDefaultDuration = (ebml_context) {0x23E383, EBML_INTEGER_CLASS, 0, 0, "DefaultDuration", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDefaultDecodedFieldDuration = (ebml_context) {0x234E7A, EBML_INTEGER_CLASS, 0, 0, "DefaultDecodedFieldDuration", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackTimestampScale = (ebml_context) {0x23314F, EBML_FLOAT_CLASS, 1, (intptr_t)1.0, "TrackTimestampScale", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackOffset = (ebml_context) {0x537F, EBML_SINTEGER_CLASS, 1, (intptr_t)0, "TrackOffset", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMaxBlockAdditionID = (ebml_context) {0x55EE, EBML_INTEGER_CLASS, 1, (intptr_t)0, "MaxBlockAdditionID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAddIDValue = (ebml_context) {0x41F0, EBML_INTEGER_CLASS, 0, 0, "BlockAddIDValue", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAddIDName = (ebml_context) {0x41A4, EBML_STRING_CLASS, 0, 0, "BlockAddIDName", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAddIDType = (ebml_context) {0x41E7, EBML_INTEGER_CLASS, 1, (intptr_t)0, "BlockAddIDType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAddIDExtraData = (ebml_context) {0x41ED, EBML_BINARY_CLASS, 0, 0, "BlockAddIDExtraData", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBlockAdditionMapping = (ebml_context) {0x41E4, EBML_MASTER_CLASS, 0, 0, "BlockAdditionMapping", EBML_SemanticBlockAdditionMapping, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextName = (ebml_context) {0x536E, EBML_UNISTRING_CLASS, 0, 0, "Name", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextLanguage = (ebml_context) {0x22B59C, EBML_STRING_CLASS, 1, (intptr_t)"eng", "Language", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextLanguageIETF = (ebml_context) {0x22B59D, EBML_STRING_CLASS, 0, 0, "LanguageIETF", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecID = (ebml_context) {0x86, EBML_STRING_CLASS, 0, 0, "CodecID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecPrivate = (ebml_context) {0x63A2, EBML_BINARY_CLASS, 0, 0, "CodecPrivate", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecName = (ebml_context) {0x258688, EBML_UNISTRING_CLASS, 0, 0, "CodecName", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextAttachmentLink = (ebml_context) {0x7446, EBML_INTEGER_CLASS, 0, 0, "AttachmentLink", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecSettings = (ebml_context) {0x3A9697, EBML_UNISTRING_CLASS, 0, 0, "CodecSettings", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecInfoURL = (ebml_context) {0x3B4040, EBML_STRING_CLASS, 0, 0, "CodecInfoURL", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecDownloadURL = (ebml_context) {0x26B240, EBML_STRING_CLASS, 0, 0, "CodecDownloadURL", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecDecodeAll = (ebml_context) {0xAA, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "CodecDecodeAll", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackOverlay = (ebml_context) {0x6FAB, EBML_INTEGER_CLASS, 0, 0, "TrackOverlay", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCodecDelay = (ebml_context) {0x56AA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "CodecDelay", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSeekPreRoll = (ebml_context) {0x56BB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "SeekPreRoll", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackTranslateEditionUID = (ebml_context) {0x66FC, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateEditionUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackTranslateCodec = (ebml_context) {0x66BF, EBML_INTEGER_CLASS, 0, 0, "TrackTranslateCodec", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackTranslateTrackID = (ebml_context) {0x66A5, EBML_BINARY_CLASS, 0, 0, "TrackTranslateTrackID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackTranslate = (ebml_context) {0x6624, EBML_MASTER_CLASS, 0, 0, "TrackTranslate", EBML_SemanticTrackTranslate, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFlagInterlaced = (ebml_context) {0x9A, EBML_INTEGER_CLASS, 1, (intptr_t)0, "FlagInterlaced", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFieldOrder = (ebml_context) {0x9D, EBML_INTEGER_CLASS, 1, (intptr_t)2, "FieldOrder", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextStereoMode = (ebml_context) {0x53B8, EBML_INTEGER_CLASS, 1, (intptr_t)0, "StereoMode", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextAlphaMode = (ebml_context) {0x53C0, EBML_INTEGER_CLASS, 1, (intptr_t)0, "AlphaMode", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextOldStereoMode = (ebml_context) {0x53B9, EBML_INTEGER_CLASS, 0, 0, "OldStereoMode", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPixelWidth = (ebml_context) {0xB0, EBML_INTEGER_CLASS, 0, 0, "PixelWidth", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPixelHeight = (ebml_context) {0xBA, EBML_INTEGER_CLASS, 0, 0, "PixelHeight", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPixelCropBottom = (ebml_context) {0x54AA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropBottom", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPixelCropTop = (ebml_context) {0x54BB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropTop", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPixelCropLeft = (ebml_context) {0x54CC, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropLeft", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPixelCropRight = (ebml_context) {0x54DD, EBML_INTEGER_CLASS, 1, (intptr_t)0, "PixelCropRight", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDisplayWidth = (ebml_context) {0x54B0, EBML_INTEGER_CLASS, 0, 0, "DisplayWidth", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDisplayHeight = (ebml_context) {0x54BA, EBML_INTEGER_CLASS, 0, 0, "DisplayHeight", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextDisplayUnit = (ebml_context) {0x54B2, EBML_INTEGER_CLASS, 1, (intptr_t)0, "DisplayUnit", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextAspectRatioType = (ebml_context) {0x54B3, EBML_INTEGER_CLASS, 1, (intptr_t)0, "AspectRatioType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextColourSpace = (ebml_context) {0x2EB524, EBML_BINARY_CLASS, 0, 0, "ColourSpace", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextGammaValue = (ebml_context) {0x2FB523, EBML_FLOAT_CLASS, 0, 0, "GammaValue", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFrameRate = (ebml_context) {0x2383E3, EBML_FLOAT_CLASS, 0, 0, "FrameRate", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMatrixCoefficients = (ebml_context) {0x55B1, EBML_INTEGER_CLASS, 1, (intptr_t)2, "MatrixCoefficients", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBitsPerChannel = (ebml_context) {0x55B2, EBML_INTEGER_CLASS, 1, (intptr_t)0, "BitsPerChannel", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChromaSubsamplingHorz = (ebml_context) {0x55B3, EBML_INTEGER_CLASS, 0, 0, "ChromaSubsamplingHorz", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChromaSubsamplingVert = (ebml_context) {0x55B4, EBML_INTEGER_CLASS, 0, 0, "ChromaSubsamplingVert", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCbSubsamplingHorz = (ebml_context) {0x55B5, EBML_INTEGER_CLASS, 0, 0, "CbSubsamplingHorz", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCbSubsamplingVert = (ebml_context) {0x55B6, EBML_INTEGER_CLASS, 0, 0, "CbSubsamplingVert", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChromaSitingHorz = (ebml_context) {0x55B7, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ChromaSitingHorz", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChromaSitingVert = (ebml_context) {0x55B8, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ChromaSitingVert", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextRange = (ebml_context) {0x55B9, EBML_INTEGER_CLASS, 1, (intptr_t)0, "Range", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTransferCharacteristics = (ebml_context) {0x55BA, EBML_INTEGER_CLASS, 1, (intptr_t)2, "TransferCharacteristics", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrimaries = (ebml_context) {0x55BB, EBML_INTEGER_CLASS, 1, (intptr_t)2, "Primaries", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMaxCLL = (ebml_context) {0x55BC, EBML_INTEGER_CLASS, 0, 0, "MaxCLL", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMaxFALL = (ebml_context) {0x55BD, EBML_INTEGER_CLASS, 0, 0, "MaxFALL", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrimaryRChromaticityX = (ebml_context) {0x55D1, EBML_FLOAT_CLASS, 0, 0, "PrimaryRChromaticityX", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrimaryRChromaticityY = (ebml_context) {0x55D2, EBML_FLOAT_CLASS, 0, 0, "PrimaryRChromaticityY", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrimaryGChromaticityX = (ebml_context) {0x55D3, EBML_FLOAT_CLASS, 0, 0, "PrimaryGChromaticityX", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrimaryGChromaticityY = (ebml_context) {0x55D4, EBML_FLOAT_CLASS, 0, 0, "PrimaryGChromaticityY", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrimaryBChromaticityX = (ebml_context) {0x55D5, EBML_FLOAT_CLASS, 0, 0, "PrimaryBChromaticityX", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextPrimaryBChromaticityY = (ebml_context) {0x55D6, EBML_FLOAT_CLASS, 0, 0, "PrimaryBChromaticityY", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextWhitePointChromaticityX = (ebml_context) {0x55D7, EBML_FLOAT_CLASS, 0, 0, "WhitePointChromaticityX", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextWhitePointChromaticityY = (ebml_context) {0x55D8, EBML_FLOAT_CLASS, 0, 0, "WhitePointChromaticityY", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextLuminanceMax = (ebml_context) {0x55D9, EBML_FLOAT_CLASS, 0, 0, "LuminanceMax", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextLuminanceMin = (ebml_context) {0x55DA, EBML_FLOAT_CLASS, 0, 0, "LuminanceMin", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextMasteringMetadata = (ebml_context) {0x55D0, EBML_MASTER_CLASS, 0, 0, "MasteringMetadata", EBML_SemanticMasteringMetadata, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextColour = (ebml_context) {0x55B0, EBML_MASTER_CLASS, 0, 0, "Colour", EBML_SemanticColour, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextProjectionType = (ebml_context) {0x7671, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ProjectionType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextProjectionPrivate = (ebml_context) {0x7672, EBML_BINARY_CLASS, 0, 0, "ProjectionPrivate", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextProjectionPoseYaw = (ebml_context) {0x7673, EBML_FLOAT_CLASS, 1, (intptr_t)0.0, "ProjectionPoseYaw", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextProjectionPosePitch = (ebml_context) {0x7674, EBML_FLOAT_CLASS, 1, (intptr_t)0.0, "ProjectionPosePitch", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextProjectionPoseRoll = (ebml_context) {0x7675, EBML_FLOAT_CLASS, 1, (intptr_t)0.0, "ProjectionPoseRoll", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextProjection = (ebml_context) {0x7670, EBML_MASTER_CLASS, 0, 0, "Projection", EBML_SemanticProjection, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextVideo = (ebml_context) {0xE0, EBML_MASTER_CLASS, 0, 0, "Video", EBML_SemanticVideo, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSamplingFrequency = (ebml_context) {0xB5, EBML_FLOAT_CLASS, 1, (intptr_t)8000.0, "SamplingFrequency", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextOutputSamplingFrequency = (ebml_context) {0x78B5, EBML_FLOAT_CLASS, 0, 0, "OutputSamplingFrequency", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChannels = (ebml_context) {0x9F, EBML_INTEGER_CLASS, 1, (intptr_t)1, "Channels", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChannelPositions = (ebml_context) {0x7D7B, EBML_BINARY_CLASS, 0, 0, "ChannelPositions", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextBitDepth = (ebml_context) {0x6264, EBML_INTEGER_CLASS, 0, 0, "BitDepth", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextAudio = (ebml_context) {0xE1, EBML_MASTER_CLASS, 0, 0, "Audio", EBML_SemanticAudio, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackPlaneUID = (ebml_context) {0xE5, EBML_INTEGER_CLASS, 0, 0, "TrackPlaneUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackPlaneType = (ebml_context) {0xE6, EBML_INTEGER_CLASS, 0, 0, "TrackPlaneType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackPlane = (ebml_context) {0xE4, EBML_MASTER_CLASS, 0, 0, "TrackPlane", EBML_SemanticTrackPlane, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackCombinePlanes = (ebml_context) {0xE3, EBML_MASTER_CLASS, 0, 0, "TrackCombinePlanes", EBML_SemanticTrackCombinePlanes, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackJoinUID = (ebml_context) {0xED, EBML_INTEGER_CLASS, 0, 0, "TrackJoinUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackJoinBlocks = (ebml_context) {0xE9, EBML_MASTER_CLASS, 0, 0, "TrackJoinBlocks", EBML_SemanticTrackJoinBlocks, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackOperation = (ebml_context) {0xE2, EBML_MASTER_CLASS, 0, 0, "TrackOperation", EBML_SemanticTrackOperation, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrickTrackUID = (ebml_context) {0xC0, EBML_INTEGER_CLASS, 0, 0, "TrickTrackUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrickTrackSegmentUID = (ebml_context) {0xC1, EBML_BINARY_CLASS, 0, 0, "TrickTrackSegmentUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrickTrackFlag = (ebml_context) {0xC6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TrickTrackFlag", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrickMasterTrackUID = (ebml_context) {0xC7, EBML_INTEGER_CLASS, 0, 0, "TrickMasterTrackUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrickMasterTrackSegmentUID = (ebml_context) {0xC4, EBML_BINARY_CLASS, 0, 0, "TrickMasterTrackSegmentUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncodingOrder = (ebml_context) {0x5031, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentEncodingOrder", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncodingScope = (ebml_context) {0x5032, EBML_INTEGER_CLASS, 1, (intptr_t)1, "ContentEncodingScope", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncodingType = (ebml_context) {0x5033, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentEncodingType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentCompAlgo = (ebml_context) {0x4254, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentCompAlgo", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentCompSettings = (ebml_context) {0x4255, EBML_BINARY_CLASS, 0, 0, "ContentCompSettings", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentCompression = (ebml_context) {0x5034, EBML_MASTER_CLASS, 0, 0, "ContentCompression", EBML_SemanticContentCompression, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncAlgo = (ebml_context) {0x47E1, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentEncAlgo", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncKeyID = (ebml_context) {0x47E2, EBML_BINARY_CLASS, 0, 0, "ContentEncKeyID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextAESSettingsCipherMode = (ebml_context) {0x47E8, EBML_INTEGER_CLASS, 0, 0, "AESSettingsCipherMode", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncAESSettings = (ebml_context) {0x47E7, EBML_MASTER_CLASS, 0, 0, "ContentEncAESSettings", EBML_SemanticContentEncAESSettings, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentSignature = (ebml_context) {0x47E3, EBML_BINARY_CLASS, 0, 0, "ContentSignature", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentSigKeyID = (ebml_context) {0x47E4, EBML_BINARY_CLASS, 0, 0, "ContentSigKeyID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentSigAlgo = (ebml_context) {0x47E5, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentSigAlgo", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentSigHashAlgo = (ebml_context) {0x47E6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ContentSigHashAlgo", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncryption = (ebml_context) {0x5035, EBML_MASTER_CLASS, 0, 0, "ContentEncryption", EBML_SemanticContentEncryption, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncoding = (ebml_context) {0x6240, EBML_MASTER_CLASS, 0, 0, "ContentEncoding", EBML_SemanticContentEncoding, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextContentEncodings = (ebml_context) {0x6D80, EBML_MASTER_CLASS, 0, 0, "ContentEncodings", EBML_SemanticContentEncodings, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTrackEntry = (ebml_context) {0xAE, MATROSKA_TRACKENTRY_CLASS, 0, 0, "TrackEntry", EBML_SemanticTrackEntry, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTracks = (ebml_context) {0x1654AE6B, EBML_MASTER_CLASS, 0, 0, "Tracks", EBML_SemanticTracks, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueTime = (ebml_context) {0xB3, EBML_INTEGER_CLASS, 0, 0, "CueTime", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueTrack = (ebml_context) {0xF7, EBML_INTEGER_CLASS, 0, 0, "CueTrack", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueClusterPosition = (ebml_context) {0xF1, EBML_INTEGER_CLASS, 0, 0, "CueClusterPosition", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueRelativePosition = (ebml_context) {0xF0, EBML_INTEGER_CLASS, 0, 0, "CueRelativePosition", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueDuration = (ebml_context) {0xB2, EBML_INTEGER_CLASS, 0, 0, "CueDuration", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueBlockNumber = (ebml_context) {0x5378, EBML_INTEGER_CLASS, 1, (intptr_t)1, "CueBlockNumber", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueCodecState = (ebml_context) {0xEA, EBML_INTEGER_CLASS, 1, (intptr_t)0, "CueCodecState", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueRefTime = (ebml_context) {0x96, EBML_INTEGER_CLASS, 0, 0, "CueRefTime", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueRefCluster = (ebml_context) {0x97, EBML_INTEGER_CLASS, 0, 0, "CueRefCluster", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueRefNumber = (ebml_context) {0x535F, EBML_INTEGER_CLASS, 1, (intptr_t)1, "CueRefNumber", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueRefCodecState = (ebml_context) {0xEB, EBML_INTEGER_CLASS, 1, (intptr_t)0, "CueRefCodecState", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueReference = (ebml_context) {0xDB, EBML_MASTER_CLASS, 0, 0, "CueReference", EBML_SemanticCueReference, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCueTrackPositions = (ebml_context) {0xB7, EBML_MASTER_CLASS, 0, 0, "CueTrackPositions", EBML_SemanticCueTrackPositions, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCuePoint = (ebml_context) {0xBB, MATROSKA_CUEPOINT_CLASS, 0, 0, "CuePoint", EBML_SemanticCuePoint, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextCues = (ebml_context) {0x1C53BB6B, EBML_MASTER_CLASS, 0, 0, "Cues", EBML_SemanticCues, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileDescription = (ebml_context) {0x467E, EBML_UNISTRING_CLASS, 0, 0, "FileDescription", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileName = (ebml_context) {0x466E, EBML_UNISTRING_CLASS, 0, 0, "FileName", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileMimeType = (ebml_context) {0x4660, EBML_STRING_CLASS, 0, 0, "FileMimeType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileData = (ebml_context) {0x465C, MATROSKA_BIGBINARY_CLASS, 0, 0, "FileData", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileUID = (ebml_context) {0x46AE, EBML_INTEGER_CLASS, 0, 0, "FileUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileReferral = (ebml_context) {0x4675, EBML_BINARY_CLASS, 0, 0, "FileReferral", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileUsedStartTime = (ebml_context) {0x4661, EBML_INTEGER_CLASS, 0, 0, "FileUsedStartTime", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextFileUsedEndTime = (ebml_context) {0x4662, EBML_INTEGER_CLASS, 0, 0, "FileUsedEndTime", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextAttachedFile = (ebml_context) {0x61A7, MATROSKA_ATTACHMENT_CLASS, 0, 0, "AttachedFile", EBML_SemanticAttachedFile, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextAttachments = (ebml_context) {0x1941A469, EBML_MASTER_CLASS, 0, 0, "Attachments", EBML_SemanticAttachments, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextEditionUID = (ebml_context) {0x45BC, EBML_INTEGER_CLASS, 0, 0, "EditionUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextEditionFlagHidden = (ebml_context) {0x45BD, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagHidden", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextEditionFlagDefault = (ebml_context) {0x45DB, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagDefault", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextEditionFlagOrdered = (ebml_context) {0x45DD, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "EditionFlagOrdered", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterUID = (ebml_context) {0x73C4, EBML_INTEGER_CLASS, 0, 0, "ChapterUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterStringUID = (ebml_context) {0x5654, EBML_UNISTRING_CLASS, 0, 0, "ChapterStringUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTimeStart = (ebml_context) {0x91, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeStart", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTimeEnd = (ebml_context) {0x92, EBML_INTEGER_CLASS, 0, 0, "ChapterTimeEnd", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterFlagHidden = (ebml_context) {0x98, EBML_BOOLEAN_CLASS, 1, (intptr_t)0, "ChapterFlagHidden", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterFlagEnabled = (ebml_context) {0x4598, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "ChapterFlagEnabled", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterSegmentUID = (ebml_context) {0x6E67, EBML_BINARY_CLASS, 0, 0, "ChapterSegmentUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterSegmentEditionUID = (ebml_context) {0x6EBC, EBML_INTEGER_CLASS, 0, 0, "ChapterSegmentEditionUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterPhysicalEquiv = (ebml_context) {0x63C3, EBML_INTEGER_CLASS, 0, 0, "ChapterPhysicalEquiv", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTrackNumber = (ebml_context) {0x89, EBML_INTEGER_CLASS, 0, 0, "ChapterTrackNumber", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterTrack = (ebml_context) {0x8F, EBML_MASTER_CLASS, 0, 0, "ChapterTrack", EBML_SemanticChapterTrack, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapString = (ebml_context) {0x85, EBML_UNISTRING_CLASS, 0, 0, "ChapString", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapLanguage = (ebml_context) {0x437C, EBML_STRING_CLASS, 1, (intptr_t)"eng", "ChapLanguage", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapLanguageIETF = (ebml_context) {0x437D, EBML_STRING_CLASS, 0, 0, "ChapLanguageIETF", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapCountry = (ebml_context) {0x437E, EBML_STRING_CLASS, 0, 0, "ChapCountry", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterDisplay = (ebml_context) {0x80, EBML_MASTER_CLASS, 0, 0, "ChapterDisplay", EBML_SemanticChapterDisplay, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapProcessCodecID = (ebml_context) {0x6955, EBML_INTEGER_CLASS, 1, (intptr_t)0, "ChapProcessCodecID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapProcessPrivate = (ebml_context) {0x450D, EBML_BINARY_CLASS, 0, 0, "ChapProcessPrivate", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapProcessTime = (ebml_context) {0x6922, EBML_INTEGER_CLASS, 0, 0, "ChapProcessTime", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapProcessData = (ebml_context) {0x6933, EBML_BINARY_CLASS, 0, 0, "ChapProcessData", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapProcessCommand = (ebml_context) {0x6911, EBML_MASTER_CLASS, 0, 0, "ChapProcessCommand", EBML_SemanticChapProcessCommand, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapProcess = (ebml_context) {0x6944, EBML_MASTER_CLASS, 0, 0, "ChapProcess", EBML_SemanticChapProcess, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapterAtom = (ebml_context) {0xB6, EBML_MASTER_CLASS, 0, 0, "ChapterAtom", EBML_SemanticChapterAtom, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextEditionEntry = (ebml_context) {0x45B9, EBML_MASTER_CLASS, 0, 0, "EditionEntry", EBML_SemanticEditionEntry, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextChapters = (ebml_context) {0x1043A770, EBML_MASTER_CLASS, 0, 0, "Chapters", EBML_SemanticChapters, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTargetTypeValue = (ebml_context) {0x68CA, EBML_INTEGER_CLASS, 1, (intptr_t)50, "TargetTypeValue", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTargetType = (ebml_context) {0x63CA, EBML_STRING_CLASS, 0, 0, "TargetType", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagTrackUID = (ebml_context) {0x63C5, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagTrackUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagEditionUID = (ebml_context) {0x63C9, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagEditionUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagChapterUID = (ebml_context) {0x63C4, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagChapterUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagAttachmentUID = (ebml_context) {0x63C6, EBML_INTEGER_CLASS, 1, (intptr_t)0, "TagAttachmentUID", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTargets = (ebml_context) {0x63C0, EBML_MASTER_CLASS, 0, 0, "Targets", EBML_SemanticTargets, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagName = (ebml_context) {0x45A3, EBML_UNISTRING_CLASS, 0, 0, "TagName", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagLanguage = (ebml_context) {0x447A, EBML_STRING_CLASS, 1, (intptr_t)"und", "TagLanguage", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagLanguageIETF = (ebml_context) {0x447B, EBML_STRING_CLASS, 0, 0, "TagLanguageIETF", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagDefault = (ebml_context) {0x4484, EBML_BOOLEAN_CLASS, 1, (intptr_t)1, "TagDefault", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagString = (ebml_context) {0x4487, EBML_UNISTRING_CLASS, 0, 0, "TagString", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTagBinary = (ebml_context) {0x4485, EBML_BINARY_CLASS, 0, 0, "TagBinary", NULL, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSimpleTag = (ebml_context) {0x67C8, EBML_MASTER_CLASS, 0, 0, "SimpleTag", EBML_SemanticSimpleTag, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTag = (ebml_context) {0x7373, EBML_MASTER_CLASS, 0, 0, "Tag", EBML_SemanticTag, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextTags = (ebml_context) {0x1254C367, EBML_MASTER_CLASS, 0, 0, "Tags", EBML_SemanticTags, EBML_getSemanticGlobals(), NULL};
    MATROSKA_ContextSegment = (ebml_context) {0x18538067, EBML_MASTER_CLASS, 0, 0, "Segment", EBML_SemanticSegment, EBML_getSemanticGlobals(), NULL};
}
