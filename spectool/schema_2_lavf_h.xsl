<?xml version="1.0"?>
<!--
    File used to generate matroska_ids.h from ebml_matroska.xml or matroska_xsd.xml (the cleaned normative version)
    Usage: xsltproc -o ../libavformat/matroska_ids.h schema_2_lavf_h.xsl ebml_matroska.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:str="http://exslt.org/strings"
    exclude-result-prefixes="str xhtml ebml"
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns="urn:ietf:rfc:8794" xmlns:ebml="urn:ietf:rfc:8794">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes" />

  <xsl:template match="ebml:EBMLSchema">
<xsl:text>/*
 * Matroska Semantic constants
 * Copyright (c) 2003-2022 The FFmpeg Project
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVFORMAT_MATROSKA_IDS_H
#define AVFORMAT_MATROSKA_IDS_H

/*
 * Matroska element IDs, max. 32 bits
 */
</xsl:text>

    <xsl:call-template name="checkFFmpegHack">
        <xsl:with-param name="node" select="ebml:element[@name='TagDefault_Bug']"/>
    </xsl:call-template>

    <xsl:call-template name="parsePath">
        <xsl:with-param name="node" select="ebml:element[@name='Segment']"/>
    </xsl:call-template>
    <!-- TODO even closer ordering with a "sort score"  https://stackoverflow.com/questions/1287651/xslt-custom-sort -->

    <xsl:for-each select="ebml:element">
        <!-- <Parent path>/<id> -->
        <!-- <xsl:sort select="concat(
            substring( @path, 1, string-length(@path)-string-length(@name) ),
            @id
        )" /> -->
        <xsl:sort select="not(@name='TrackType')"/>
        <xsl:sort select="not(@name='ContentCompAlgo')"/>

        <!-- Output the enums after the IDs -->
        <xsl:call-template name="outputAllEnums"/>

    </xsl:for-each>

<xsl:text>
#endif /* AVFORMAT_MATROSKA_IDS_H */
</xsl:text>

  </xsl:template>

    <xsl:template name="checkFFmpegHack">
        <xsl:param name="node"/>
        <xsl:if test="not($node/@name)">
            <xsl:message terminate="yes">Missing TagDefault_Bug!</xsl:message>
        </xsl:if>
    </xsl:template>

    <!-- TODO remove this and list all the known elements in the EBML Schema -->
    <xsl:template name="discardNewElement">
        <xsl:param name="node"/>

        <!-- <xsl:choose>
            <xsl:when test="$node/@name='LanguageIETF'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='GammaValue'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='OldStereoMode'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackJoinBlocks'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChannelPositions'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='DefaultDecodedFieldDuration'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CodecSettings'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackOffset'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackOverlay'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrickTrackUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrickTrackSegmentUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrickMasterTrackSegmentUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrickTrackFlag'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrickMasterTrackUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackTranslate'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagEditionUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagLanguageIETF'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagBinary'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrevFilename'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrevUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='NextFilename'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='NextUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SegmentFamily'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SegmentFilename'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterTranslate'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueCodecState'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueReference'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockVirtual'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ReferencePriority'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ReferenceVirtual'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Slices'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ReferenceFrame'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='EncryptedBlock'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SilentTracks'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapLanguageIETF'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterStringUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterSegmentUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterSegmentEditionUID'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapProcess'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterTrack'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileUsedStartTime'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileUsedEndTime'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileReferral'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='AttachmentLink'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SegmentFamily'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='AESSettingsCipherMode'"><xsl:text>y</xsl:text></xsl:when>
        </xsl:choose> -->
    </xsl:template>

    <xsl:template name="discardStructure">
        <xsl:param name="node"/>
        <xsl:choose>
            <xsl:when test="$node/@name='TransferCharacteristics'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Range'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Primaries'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='AspectRatioType'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MatrixCoefficients'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='OldStereoMode'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapProcessTime'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterTranslateCodec'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TargetTypeValue'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncodingScope'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncodingType'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncAlgo'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentSigAlgo'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentSigHashAlgo'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='AESSettingsCipherMode'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackPlaneType'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackTranslateCodec'"><xsl:text>y</xsl:text></xsl:when>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="parsePath">
        <xsl:param name="node"/>

<!-- <xsl:value-of select="$node/@name"/><xsl:text>/</xsl:text><xsl:value-of select="$node/@type"/> -->

        <xsl:variable name="plainPath">
            <xsl:value-of select="translate($node/@path, '\+', '\')" />
        </xsl:variable>

        <!-- Master element comment header -->
        <xsl:if test="$node/@type='master'">
            <xsl:choose>
                <xsl:when test="$node/@name='Segment'">
                    <xsl:text>
/* toplevel segment */
#define MATROSKA_ID_SEGMENT    0x18538067

/* Matroska top-level master IDs */
</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text>/* IDs in the </xsl:text>
                    <xsl:variable name="masterName">
                        <xsl:choose>
                            <xsl:when test="$node/@name='CuePoint'"><xsl:text>PointEntry</xsl:text></xsl:when>
                            <xsl:when test="$node/@name='Audio'"><xsl:text>TrackAudio</xsl:text></xsl:when>
                            <xsl:when test="$node/@name='Video'"><xsl:text>TrackVideo</xsl:text></xsl:when>
                            <xsl:when test="$node/@name='Seek'"><xsl:text>SeekPoint</xsl:text></xsl:when>
                            <xsl:when test="$node/@name='ContentEncoding'"><xsl:text>Content Encoding</xsl:text></xsl:when>
                            <xsl:when test="$node/@name='BlockAdditionMapping'"><xsl:text>block addition mapping</xsl:text></xsl:when>
                            <xsl:when test="$node/@name='CueTrackPositions'"><xsl:text>cuetrackposition</xsl:text></xsl:when>
                            <xsl:otherwise><xsl:value-of select="$node/@name"/></xsl:otherwise>
                        </xsl:choose>
                    </xsl:variable>

                    <xsl:value-of select="translate($masterName, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz')"/>
                    <xsl:text> master */&#10;</xsl:text>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:if>

        <!-- Output each child ID -->

        <!-- select the sorting based on the parent due to sorting limitations -->
<!-- <xsl:value-of select="$plainPath"/><xsl:text>&#10;</xsl:text> -->
        <xsl:choose>
            <xsl:when test="$node/@path='\Segment'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='Info')" />
                    <xsl:sort select="not(@name='Tracks')" />
                    <xsl:sort select="not(@name='Cues')" />
                    <xsl:sort select="not(@name='Tags')" />
                    <xsl:sort select="@name='Chapters'" />
                    <xsl:sort select="@name='Cluster'" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Info'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='TimestampScale')" />
                    <xsl:sort select="@name='SegmentUID'" />
                    <xsl:sort select="@name='DateUTC'" />
                    <xsl:sort select="@name='MuxingApp'" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='TrackNumber')" />
                    <xsl:sort select="not(@name='TrackUID')" />
                    <xsl:sort select="not(@name='TrackType')" />
                    <xsl:sort select="not(@name='Video')" />
                    <xsl:sort select="not(@name='Audio')" />
                    <xsl:sort select="not(@name='TrackOperation')" />
                    <xsl:sort select="@name='ContentEncodings'" />
                    <xsl:sort select="@name='DefaultDuration'" />
                    <xsl:sort select="@name='MaxCache'" />
                    <xsl:sort select="@name='MinCache'" />
                    <xsl:sort select="@name='FlagLacing'" />
                    <xsl:sort select="@name='FlagForced'" />
                    <xsl:sort select="@name='FlagDefault'" />
                    <xsl:sort select="@name='FlagEnabled'" />
                    <!-- <xsl:sort select="@name='Language'" /> limit reached -->
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\Video'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='FrameRate')" />
                    <xsl:sort select="not(@name='DisplayWidth')" />
                    <xsl:sort select="not(@name='DisplayHeight')" />
                    <xsl:sort select="not(@name='PixelWidth')" />
                    <xsl:sort select="not(@name='PixelHeight')" />
                    <xsl:sort select="not(@name='PixelCropBottom')" />
                    <xsl:sort select="not(@name='PixelCropTop')" />
                    <xsl:sort select="not(@name='PixelCropLeft')" />
                    <xsl:sort select="not(@name='PixelCropRight')" />
                    <xsl:sort select="not(@name='DisplayUnit')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\Audio'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='SamplingFrequency')" />
                    <xsl:sort select="@name='Channels'" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\ContentEncodings\ContentEncoding\ContentEncryption'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='ContentEncAESSettings')" />
                    <xsl:sort select="@name='ContentSignature'" />
                    <xsl:sort select="@name='ContentSigKeyID'" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Cues\CuePoint\CueTrackPositions'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='CueTrack')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Tags\Tag'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='SimpleTag')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Tags\Tag\Targets'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='TargetType')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Cluster'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='Timestamp')" />
                    <xsl:sort select="@name='SimpleBlock'" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Cluster\BlockGroup'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='Block')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Cluster\BlockGroup\BlockAdditions\BlockMore'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='BlockAddID')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Chapters\EditionEntry\+ChapterAtom'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='ChapterUID')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$node/@path='\Segment\Attachments\AttachedFile'">
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="not(@name='FileDescription')" />
                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:when>
            <xsl:otherwise>
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:sort select="string-length(@id)" />
                    <xsl:sort select="@id" />

                    <xsl:call-template name="outputNodeID">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:for-each>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:text>&#10;</xsl:text>

        <!-- Output elements from child masters -->
        <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
            <xsl:sort select="not(@name='Info')" />
            <xsl:sort select="not(@name='Tracks')" />
            <xsl:sort select="not(@name='Cues')" />
            <xsl:sort select="not(@name='Tags')" />
            <xsl:sort select="@name='Chapters'" />
            <xsl:sort select="@name='Audio'" />
            <xsl:sort select="@name" />
            <xsl:if test="@type='master'">
                <xsl:variable name="isDiscarded">
                    <xsl:call-template name="discardNewElement">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:variable>

<!-- <xsl:value-of select="@name"/><xsl:text>/</xsl:text><xsl:value-of select="@type"/> -->

                <xsl:if test="$isDiscarded=''">
                    <xsl:call-template name="parsePath">
                        <xsl:with-param name="node" select="."/>
                    </xsl:call-template>
                </xsl:if>
            </xsl:if>
        </xsl:for-each>

    </xsl:template>

    <xsl:template name="outputNodeID">
        <xsl:param name="node"/>

        <xsl:variable name="isDiscarded">
            <xsl:call-template name="discardNewElement">
                <xsl:with-param name="node" select="$node"/>
            </xsl:call-template>
        </xsl:variable>

        <xsl:if test="$isDiscarded=''">
            <xsl:variable name="lavfName">
                <!-- Transform the ebml_matroska.xml name into the libavformat name -->
                <xsl:choose>
                    <xsl:when test="$node/@name='FileDescription'"><xsl:text>FileDesc</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChapLanguage'"><xsl:text>ChapLang</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ReferenceBlock'"><xsl:text>BlockReference</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Position'"><xsl:text>ClusterPosition</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='PrevSize'"><xsl:text>ClusterPrevSize</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Timestamp'"><xsl:text>ClusterTimecode</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='CuePoint'"><xsl:text>PointEntry</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='CueTrackPositions'"><xsl:text>CueTrackPosition</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TimestampScale'"><xsl:text>TimecodeScale</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Seek'"><xsl:text>SeekEntry</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TagLanguage'"><xsl:text>TagLang</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Targets'"><xsl:text>TagTargets</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TagAttachmentUID'"><xsl:text>TagTargets_AttachUID</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TagChapterUID'"><xsl:text>TagTargets_ChapterUID</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TagTrackUID'"><xsl:text>TagTargets_TrackUID</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TargetType'"><xsl:text>TagTargets_Type</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TargetTypeValue'"><xsl:text>TagTargets_TypeValue</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChapterPhysicalEquiv'"><xsl:text>ChapterPhysEquiv</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='OutputSamplingFrequency'"><xsl:text>AudioOutSamplingFreq</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='SamplingFrequency'"><xsl:text>AudioSamplingFreq</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncodings'"><xsl:text>TrackContentEncodings</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncoding'"><xsl:text>TrackContentEncoding</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentCompression'"><xsl:text>EncodingCompression</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentCompAlgo'"><xsl:text>EncodingCompAlgo</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentCompSettings'"><xsl:text>EncodingCompSettings</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncodingOrder'"><xsl:text>EncodingOrder</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncodingScope'"><xsl:text>EncodingScope</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncodingType'"><xsl:text>EncodingType</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncryption'"><xsl:text>EncodingEncryption</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncAESSettings'"><xsl:text>EncodingEncAESSettings</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncAlgo'"><xsl:text>EncodingEncAlgo</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncKeyID'"><xsl:text>EncodingEncKeyId</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentSigAlgo'"><xsl:text>EncodingSigAlgo</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentSignature'"><xsl:text>EncodingSignature</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentSigKeyID'"><xsl:text>EncodingSigKeyId</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentSigHashAlgo'"><xsl:text>EncodingSigHashAlgo</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='MaxBlockAdditionID'"><xsl:text>TrackMaxBlkAddID</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockAdditionMapping'"><xsl:text>TrackBlkAddMapping</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockAddIDValue'"><xsl:text>BlkAddIDValue</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockAddIDName'"><xsl:text>BlkAddIDName</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockAddIDType'"><xsl:text>BlkAddIDType</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockAddIDExtraData'"><xsl:text>BlkAddIDExtraData</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='SeekPreRoll'"><xsl:text>SeekPreRoll</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TrackTimestampScale'"><xsl:text>TrackTimecodeScale</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='AspectRatioType'"><xsl:text>VideoAspectRatio</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Colour'"><xsl:text>VideoColor</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ColourSpace'"><xsl:text>VideoColorSpace</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChromaSubsamplingHorz'"><xsl:text>VideoColorChromaSubHorz</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChromaSubsamplingVert'"><xsl:text>VideoColorChromaSubVert</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='CbSubsamplingHorz'"><xsl:text>VideoColorCbSubHorz</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='CbSubsamplingVert'"><xsl:text>VideoColorCbSubVert</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='MasteringMetadata'"><xsl:text>VideoColorMasteringMeta</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='MatrixCoefficients'"><xsl:text>VideoColorMatrixCoeff</xsl:text></xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\Audio\')"><xsl:text>Audio</xsl:text><xsl:value-of select="$node/@name"/></xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\Video\PixelCrop')"><xsl:text>VideoPixelCrop</xsl:text><xsl:value-of select="substring($node/@name, 10, 1)"/></xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\Video\Colour\MasteringMetadata\Primary')">
                        <xsl:text>VideoColor_</xsl:text><xsl:value-of select="substring($node/@name, 8, 1)"/><xsl:value-of select="substring($node/@name, string-length($node/@name))"/>
                    </xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\Video\Colour\MasteringMetadata\WhitePointChromaticity')">
                        <xsl:text>VideoColor_WHITE</xsl:text><xsl:value-of select="substring($node/@name, string-length($node/@name))"/>
                    </xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\Video\Colour\MasteringMetadata\Luminance')"><xsl:text>VideoColor_</xsl:text><xsl:value-of select="$node/@name"/></xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\Video\Colour\')"><xsl:text>VideoColor</xsl:text><xsl:value-of select="$node/@name"/></xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\Video\')"><xsl:text>Video</xsl:text><xsl:value-of select="$node/@name"/></xsl:when>
                    <xsl:when test="contains($node/@path,'\TrackEntry\') and not(contains($node/@name,'Track')) and not(contains($node/@name,'Codec'))"><xsl:text>Track</xsl:text><xsl:value-of select="$node/@name"/></xsl:when>
                    <xsl:otherwise><xsl:value-of select="$node/@name"/></xsl:otherwise>
                </xsl:choose>
            </xsl:variable>
            <xsl:variable name="lavfNameUpper">
                <xsl:value-of select="translate($lavfName, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
            </xsl:variable>

            <xsl:text>#define MATROSKA_ID_</xsl:text>
            <xsl:choose>
                <xsl:when test="string-length($lavfNameUpper) &lt; 19">
                    <xsl:value-of select="substring(concat($lavfNameUpper, '                          '),0,19)"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$lavfNameUpper"/>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:text>  </xsl:text>
            <xsl:value-of select="$node/@id"/>
            <xsl:text>&#10;</xsl:text>
        </xsl:if>
    </xsl:template>

    <xsl:template name="ConvertDecToHex">
        <xsl:param name="index" />
        <xsl:if test="$index > 0">
        <xsl:call-template name="ConvertDecToHex">
            <xsl:with-param name="index" select="floor($index div 16)" />
        </xsl:call-template>
        <xsl:choose>
            <xsl:when test="$index mod 16 &lt; 10">
                <xsl:value-of select="$index mod 16" />
            </xsl:when>
            <xsl:otherwise>
            <xsl:choose>
                <xsl:when test="$index mod 16 = 10">A</xsl:when>
                <xsl:when test="$index mod 16 = 11">B</xsl:when>
                <xsl:when test="$index mod 16 = 12">C</xsl:when>
                <xsl:when test="$index mod 16 = 13">D</xsl:when>
                <xsl:when test="$index mod 16 = 14">E</xsl:when>
                <xsl:when test="$index mod 16 = 15">F</xsl:when>
                <xsl:otherwise>A</xsl:otherwise>
            </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
        </xsl:if>
    </xsl:template>

    <xsl:template name="ConvertToHex">
        <xsl:param name="index" />
        <xsl:text>0x</xsl:text>
        <xsl:call-template name="ConvertDecToHex">
            <xsl:with-param name="index" select="$index" />
        </xsl:call-template>
    </xsl:template>


    <xsl:template name="outputAllEnums">
        <xsl:variable name="isDiscarded">
            <xsl:call-template name="discardStructure">
                <xsl:with-param name="node" select="."/>
            </xsl:call-template>
        </xsl:variable>

        <xsl:if test="$isDiscarded=''">

            <xsl:if test="ebml:restriction/ebml:enum and @type!='string'">
                <xsl:variable name="prefix">
                    <xsl:choose>
                        <xsl:when test="@name='ContentCompAlgo'">TRACK_ENCODING_COMP</xsl:when>
                        <xsl:when test="@name='TrackType'">TRACK_TYPE</xsl:when>
                        <xsl:when test="@name='ProjectionType'">VIDEO_PROJECTION_TYPE</xsl:when>
                        <xsl:when test="@name='FlagInterlaced'">VIDEO_INTERLACE_FLAG</xsl:when>
                        <xsl:when test="@name='StereoMode'">VIDEO_STEREOMODE_TYPE</xsl:when>
                        <xsl:when test="contains(@path,'\TrackEntry\Video\Colour\')"><xsl:text>COLOUR_</xsl:text><xsl:value-of select="@name"/></xsl:when>
                        <xsl:when test="contains(@path,'\TrackEntry\Video\')"><xsl:text>VIDEO_</xsl:text><xsl:value-of select="@name"/></xsl:when>
                        <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>
                <xsl:variable name="align" as="xs:integer">
                    <!-- padding added in structures to have fancy alignment -->
                    <xsl:choose>
                        <xsl:when test="@name='ContentCompAlgo'">12</xsl:when>
                        <xsl:when test="@name='TrackType'">9</xsl:when>
                        <xsl:when test="@name='ProjectionType'">19</xsl:when>
                        <xsl:when test="@name='FlagInterlaced'">13</xsl:when>
                        <xsl:when test="@name='StereoMode'">19</xsl:when>
                        <xsl:when test="@name='DisplayUnit'">12</xsl:when>
                        <xsl:when test="@name='FieldOrder'">13</xsl:when>
                        <xsl:otherwise>17</xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>
                <xsl:text>typedef enum {&#10;</xsl:text>

                <!-- Internal value not found in the specs -->
                <xsl:if test="@name='TrackType'"><xsl:text>  MATROSKA_TRACK_TYPE_NONE     = 0x0,&#10;</xsl:text></xsl:if>

                <xsl:for-each select="ebml:restriction/ebml:enum">
                    <xsl:sort select="value"/>
                    <xsl:text>  MATROSKA_</xsl:text>
                    <xsl:value-of select="translate($prefix, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
                    <xsl:text>_</xsl:text>
                    <xsl:call-template name="outputEnumLabel">
                        <xsl:with-param name="label" select="@label"/>
                        <xsl:with-param name="align" select="$align"/>
                    </xsl:call-template>

                    <xsl:text> = </xsl:text>
                    <xsl:choose>
                        <xsl:when test="$prefix='TRACK_TYPE'">
                            <xsl:call-template name="ConvertToHex">
                                <xsl:with-param name="index" select="@value"/>
                            </xsl:call-template>
                        </xsl:when>
                        <xsl:otherwise><xsl:value-of select="@value"/></xsl:otherwise>
                    </xsl:choose>
                    <xsl:text>,&#10;</xsl:text>
                </xsl:for-each>

                <!-- Extra enum count -->
                <xsl:choose>
                    <xsl:when test="@name='ChromaSitingHorz'"><xsl:text>  MATROSKA_COLOUR_CHROMASITINGHORZ_NB&#10;</xsl:text></xsl:when>
                    <xsl:when test="@name='ChromaSitingVert'"><xsl:text>  MATROSKA_COLOUR_CHROMASITINGVERT_NB&#10;</xsl:text></xsl:when>
                    <xsl:when test="@name='StereoMode'"><xsl:text>  MATROSKA_VIDEO_STEREOMODE_TYPE_NB,&#10;</xsl:text></xsl:when>
                </xsl:choose>

                <xsl:text>} Matroska</xsl:text>
                <xsl:choose>
                    <xsl:when test="@name='ContentCompAlgo'"><xsl:text>TrackEncodingCompAlgo</xsl:text></xsl:when>
                    <xsl:when test="@name='ChromaSitingHorz'"><xsl:text>ColourChromaSitingHorz</xsl:text></xsl:when>
                    <xsl:when test="@name='ChromaSitingVert'"><xsl:text>ColourChromaSitingVert</xsl:text></xsl:when>
                    <xsl:when test="@name='FlagInterlaced'"><xsl:text>VideoInterlaceFlag</xsl:text></xsl:when>
                    <xsl:when test="@name='StereoMode'"><xsl:text>VideoStereoModeType</xsl:text></xsl:when>
                    <xsl:when test="contains(@path,'\TrackEntry\Video\')"><xsl:text>Video</xsl:text><xsl:value-of select="@name"/></xsl:when>
                    <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise>
                </xsl:choose>
                <xsl:text>;&#10;&#10;</xsl:text>
            </xsl:if>
        </xsl:if>
    </xsl:template>

  <xsl:template name="outputEnumLabel">
    <xsl:param name="label"/>
    <xsl:param name="align" as="xs:integer"/>
    <!-- Turn the ebml_matroska.xml enum labels into the names used in libavformat -->
    <!-- Recursive calls until we end up with a matching name with no space, parenthesis, comas, etc -->
    <xsl:choose>
        <xsl:when test="$label='lzo1x'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'LZO'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='Header Stripping'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'HeaderStrip'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='unspecified'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'undetermined'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='display aspect ratio'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'dar'"/></xsl:call-template>
        </xsl:when>
        <!-- Field Order -->
        <xsl:when test="$label='tff'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'tt'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='bff'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'bb'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='tff(swapped)'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'bt'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='bff(swapped)'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'tb'"/></xsl:call-template>
        </xsl:when>

        <!-- Stereo Mode -->
        <xsl:when test="contains($label,'top - bottom (left eye is first)')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'top bottom'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'top - bottom (right eye is first)')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'bottom top'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'anaglyph (cyan/red)')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'anaglyph cyan red'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'anaglyph (green/magenta)')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'anaglyph green mag'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'side by side (')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="substring-after($label, 'side by side (')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'(right eye is first)')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label,'(right eye is first)'), 'rl')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'(left eye is first)')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label,'(left eye is first)'), 'lr')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'left eye first')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'left right'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'right eye first')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'right left'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'column ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(concat(substring-before($label, 'column '), 'col '), substring-after($label, 'column '))"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'checkboard ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/>
                <xsl:with-param name="label" select="concat(concat(substring-before($label, 'checkboard '), 'checkerboard '), substring-after($label, 'checkboard '))"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'laced in one ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label, 'laced in one '), substring-after($label, 'laced in one '))"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'The ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label, 'The '), substring-after($label, 'The '))"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' private data')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'private data'"/></xsl:call-template>
        </xsl:when>

        <xsl:when test="contains($label,',')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="substring-before($label, ',')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,')')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="substring-before($label, ')')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' collocated')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="substring-before($label, ' collocated')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' (')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="substring-before($label, ' (')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' /')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="substring-before($label, ' /')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'__')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label, '__'), substring-after($label, '__'))"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' - ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(concat(substring-before($label, ' - '), '_'), substring-after($label, ' - '))"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' -')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(concat(substring-before($label, ' -'), '_'), substring-after($label, ' -'))"/></xsl:call-template>
        </xsl:when>
        <!-- <xsl:when test="contains($label, &apos;)">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label, &apos;), substring-after($label, '&apos;'))"/></xsl:call-template>
        </xsl:when> -->
        <xsl:when test="contains($label,'-')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="translate($label, '-', '_')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="translate($label, ' ', '_')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'.')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="translate($label, '.', '_')"/></xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <!-- Now we have a suitable name, output it in upper case with padding -->
            <xsl:call-template name="finalOutputEnumLabel">
                <xsl:with-param name="label" select="translate($label, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
                <xsl:with-param name="align" select="$align"/>
            </xsl:call-template>
        </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="finalOutputEnumLabel">
    <xsl:param name="label"/>
    <xsl:param name="align" as="xs:integer"/>
    <xsl:choose>
        <xsl:when test="string-length($label) &lt; $align">
            <xsl:value-of select="substring(concat($label, '                          '),0,$align)"/>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$label"/>
        </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
