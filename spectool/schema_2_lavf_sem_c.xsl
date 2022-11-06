<?xml version="1.0"?>
<!--
    File used to generate matroska_ids.h from ebml_matroska.xml or matroska_xsd.xml (the cleaned normative version)
    Usage: xsltproc -o ../libavformat/matroskasem.c schema_2_lavf_sem_c.xsl ebml_matroska.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:str="http://exslt.org/strings"
    exclude-result-prefixes="str xhtml ebml"
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns="urn:ietf:rfc:8794" xmlns:ebml="urn:ietf:rfc:8794">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes" />

  <xsl:template match="ebml:EBMLSchema">/*
 * Matroska file semantic definition
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

/**
 * @file
 * Matroska file semantic element definitions
 * @author Ronald Bultje &lt;rbultje@ronald.bitfreak.net&gt;
 * @author with a little help from Moritz Bunkus &lt;moritz@bunkus.org&gt;
 * @author totally reworked by Aurelien Jacobs &lt;aurel@gnuage.org&gt;
 * @author Split from decoder by Steve Lhomme &lt;slhomme@matroska.org&gt;
 * @see specs available on the Matroska project page: http://www.matroska.org/
 */

#include "config.h"

#include &lt;inttypes.h&gt;

#include "matroskasem.h"

#define CHILD_OF(parent) { .def = { .n = parent } }

// The following forward declarations need their size because
// a tentative definition with internal linkage must not be an
// incomplete type (6.7.2 in C90, 6.9.2 in C99).
// Removing the sizes breaks MSVC.
<xsl:call-template name="output-static-syntax">
    <xsl:with-param name="node" select="ebml:element[@path='\Segment']"/>
</xsl:call-template>

static EbmlSyntax ebml_header[] = {
    { EBML_ID_EBMLREADVERSION,    EBML_UINT, 0, 0, offsetof(Ebml, version),         { .u = EBML_VERSION } },
    { EBML_ID_EBMLMAXSIZELENGTH,  EBML_UINT, 0, 0, offsetof(Ebml, max_size),        { .u = 8 } },
    { EBML_ID_EBMLMAXIDLENGTH,    EBML_UINT, 0, 0, offsetof(Ebml, id_length),       { .u = 4 } },
    { EBML_ID_DOCTYPE,            EBML_STR,  0, 0, offsetof(Ebml, doctype),         { .s = "(none)" } },
    { EBML_ID_DOCTYPEREADVERSION, EBML_UINT, 0, 0, offsetof(Ebml, doctype_version), { .u = 1 } },
    { EBML_ID_EBMLVERSION,        EBML_NONE },
    { EBML_ID_DOCTYPEVERSION,     EBML_NONE },
    CHILD_OF(ebml_syntax)
};

EbmlSyntax ebml_syntax[] = {
    { EBML_ID_HEADER,      EBML_NEST, 0, 0, 0, { .n = ebml_header } },
    { MATROSKA_ID_SEGMENT, EBML_STOP },
    { 0 }
};
<xsl:call-template name="checkFFmpegHack">
    <xsl:with-param name="node" select="ebml:element[@name='TagDefault_Bug']"/>
</xsl:call-template>

<xsl:for-each select="ebml:element[not(starts-with(@path,'\EBML\'))]">
    <xsl:sort select="@path" order="descending"/>
    <xsl:apply-templates select="."/>
</xsl:for-each>
EbmlSyntax matroska_segments[] = {
    { MATROSKA_ID_SEGMENT, EBML_NEST, 0, 0, 0, { .n = matroska_segment } },
    { 0 }
};

EbmlSyntax matroska_cluster_enter[] = {
    { MATROSKA_ID_CLUSTER,     EBML_NEST, 0, 0, 0, { .n = &amp;matroska_cluster_parsing[2] } },
    { 0 }
};
</xsl:template>
    <!-- Apply the ebml:element template on the Segment node -->
    <!-- <xsl:apply-templates select="ebml:element[@name='Segment']"/> -->
    <!-- <xsl:for-each select="ebml:element[@type = 'master']"> -->
        <!-- TODO even closer ordering with a "sort score"  https://stackoverflow.com/questions/1287651/xslt-custom-sort -->
        <!-- <xsl:sort select="@path" order="descending"/> -->
  <xsl:template match="ebml:element">

        <xsl:variable name="hasStoredElts">
            <xsl:call-template name="masterHasStoredElts">
                <xsl:with-param name="node" select="."/>
            </xsl:call-template>
        </xsl:variable>

        <xsl:if test="not($hasStoredElts='')">
            <xsl:call-template name="parsePath">
                <xsl:with-param name="node" select="."/>
            </xsl:call-template>
        </xsl:if>
    <!-- </xsl:for-each> -->
    </xsl:template>

  <xsl:template name="output-static-syntax">
    <xsl:param name="node"/>

    <xsl:variable name="plainPath">
        <xsl:value-of select="translate($node/@path, '\+', '\')" />
    </xsl:variable>

    <xsl:if test="$node/@type='master'">
        <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
            <xsl:call-template name="output-static-syntax">
                <xsl:with-param name="node" select="."/>
            </xsl:call-template>
        </xsl:for-each>

        <xsl:variable name="hasStoredElts">
            <xsl:call-template name="masterHasStoredElts">
                <xsl:with-param name="node" select="$node"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:if test="not($hasStoredElts='')">
            <xsl:variable name="hasStoredMaster">
                <xsl:call-template name="masterHasStoredMaster">
                    <xsl:with-param name="node" select="$node"/>
                </xsl:call-template>
            </xsl:variable>
            <xsl:if test="not($hasStoredMaster='')">
                <xsl:variable name="childCount">
                    <xsl:call-template name="masterCountChildren">
                        <xsl:with-param name="node" select="$node"/>
                    </xsl:call-template>
                </xsl:variable>
                <xsl:variable name="isShared">
                    <xsl:call-template name="syntaxIsShared">
                        <xsl:with-param name="node" select="$node"/>
                    </xsl:call-template>
                </xsl:variable>
                <xsl:if test="not($isShared='')">
                    <xsl:text>// </xsl:text>
                </xsl:if>
                <xsl:text>static EbmlSyntax matroska_</xsl:text>
                <xsl:call-template name="masterListName">
                    <xsl:with-param name="node" select="$node"/>
                </xsl:call-template>
                <xsl:text>[</xsl:text>
                <xsl:choose>
                    <xsl:when test="@recursive='1'">
                        <xsl:value-of select="string-length(concat($childCount,'yy'))" />
                    </xsl:when>
                    <xsl:when test="@name='BlockGroup'">
                        <!-- Extra elements to do the Block parsing -->
                        <xsl:value-of select="string-length(concat($childCount,'yy'))" />
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:value-of select="string-length(concat($childCount,'y'))" />
                    </xsl:otherwise>
                </xsl:choose>
                <xsl:text>];&#10;</xsl:text>
            </xsl:if>
        </xsl:if>
    </xsl:if>
  </xsl:template>

    <xsl:template name="checkFFmpegHack">
        <xsl:param name="node"/>
        <xsl:if test="not($node/@name)">
            <xsl:message terminate="yes">Missing TagDefault_Bug!</xsl:message>
        </xsl:if>
    </xsl:template>

    <!-- Elements also declared in matroskasem.h -->
    <xsl:template name="syntaxIsShared">
        <xsl:param name="node"/>
        <xsl:choose>
            <xsl:when test="$node/@name='Segment'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockGroup'"><xsl:text>y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Cluster'"><xsl:text>y</xsl:text></xsl:when>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="parsePath">
        <xsl:param name="node"/>

        <!-- Master element comment header -->
        <xsl:if test="@type='master'">
            <xsl:if test="@name='Cluster'">
                <!-- Extra elements to do the (Simple)Block parsing -->
                <xsl:text>&#10;// The following array contains SimpleBlock and BlockGroup twice</xsl:text>
                <xsl:text>&#10;// in order to reuse the other values for matroska_cluster_enter.</xsl:text>
            </xsl:if>

<!-- <xsl:value-of select="concat(
            substring( $node/@path, 1, string-length($node/@path)-string-length($node/@name)-1 ),
            $node/@id
        )"/> -->
<!-- <xsl:value-of select="$node/@path"/> -->

            <xsl:text>&#10;</xsl:text>
            <xsl:variable name="isShared">
                <xsl:call-template name="syntaxIsShared">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
            </xsl:variable>
            <xsl:if test="$isShared=''">
                <xsl:text>static </xsl:text>
            </xsl:if>
            <xsl:text>EbmlSyntax matroska_</xsl:text>
            <xsl:call-template name="masterListName">
                <xsl:with-param name="node" select="$node"/>
            </xsl:call-template>
            <xsl:text>[] = {&#10;</xsl:text>

<!-- <xsl:value-of select="$node/@path"/><xsl:text>&#10;</xsl:text> -->
            <xsl:choose>
                <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\Video\Colour\MasteringMetadata'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                            <!-- Common elements that should be found early when parsing -->
                            <xsl:sort select="@name='LuminanceMax'"/>
                            <xsl:sort select="@name='LuminanceMin'"/>
                            <!-- <xsl:sort select="@type='master'"/> -->
                        <!-- <xsl:sort select="@id" /> -->
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\Video'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                            <!-- Common elements that should be found early when parsing -->
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
                            <xsl:sort select="@name='AspectRatioType'"/>
                            <xsl:sort select="@name='StereoMode'"/>
                        <!-- <xsl:sort select="@id" /> -->
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\Audio'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                    <xsl:sort select="not(@name='SamplingFrequency')" />
                    <xsl:sort select="@name='Channels'" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='TrackNumber')" />
                        <xsl:sort select="not(@name='Name')" />
                        <xsl:sort select="not(@name='TrackUID')" />
                        <xsl:sort select="not(@name='TrackType')" />
                        <xsl:sort select="not(@name='CodecID')" />
                        <xsl:sort select="not(@name='CodecPrivate')" />
                        <xsl:sort select="not(@name='CodecDelay')" />
                        <xsl:sort select="not(@name='Language')" />
                        <xsl:sort select="not(@name='DefaultDuration')" />
                        <xsl:sort select="not(@name='TrackTimestampScale')" />
                        <xsl:sort select="not(@name='FlagDefault')" />
                        <xsl:sort select="not(@name='FlagForced')" />
                        <xsl:sort select="not(@name='Video')" />
                        <xsl:sort select="not(@name='Audio')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\ContentEncodings\ContentEncoding'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="@name='ContentEncodingOrder'" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tracks\TrackEntry\ContentEncodings\ContentEncoding\ContentEncryption'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="@name='ContentSignature'" />
                        <xsl:sort select="@name='ContentSigKeyID'" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tags\Tag\Targets'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='TargetType')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tags\Tag\+SimpleTag'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='TagName')" />
                        <xsl:sort select="not(@name='TagString')" />
                        <xsl:sort select="@name='TagDefault_Bug'" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Tags\Tag'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='SimpleTag')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Info'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='TimestampScale')" />
                        <xsl:sort select="@name='SegmentUID'" />
                        <xsl:sort select="@name='DateUTC'" />
                        <xsl:sort select="@name='MuxingApp'" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Cues\CuePoint\CueTrackPositions'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='CueTrack')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Cluster\BlockGroup\BlockAdditions\BlockMore'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='BlockAddID')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Cluster\BlockGroup'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='Block')" />
                        <xsl:sort select="not(@name='BlockAdditions')" />
                        <xsl:sort select="not(@name='BlockDuration')" />
                        <xsl:sort select="not(@name='DiscardPadding')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Chapters\EditionEntry\+ChapterAtom\ChapterDisplay'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='ChapString')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Chapters\EditionEntry\+ChapterAtom'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='ChapterTimeStart')" />
                        <xsl:sort select="not(@name='ChapterTimeEnd')" />
                        <xsl:sort select="not(@name='ChapterUID')" />
                        <xsl:sort select="not(@name='ChapterDisplay')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Chapters\EditionEntry'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='ChapterAtom')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Attachments\AttachedFile'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='FileUID')" />
                        <xsl:sort select="@name='FileDescription'" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment\Cluster'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='SimpleBlock')" />
                        <xsl:sort select="not(@name='BlockGroup')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                    <!-- Extra elements to do the (Simple)Block parsing -->
                    <xsl:text>    { MATROSKA_ID_SIMPLEBLOCK,                EBML_STOP },&#10;</xsl:text>
                    <xsl:text>    { MATROSKA_ID_BLOCKGROUP,                 EBML_STOP },&#10;</xsl:text>
                </xsl:when>
                <xsl:when test="$node/@path='\Segment'">
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                        <xsl:sort select="not(@name='Cluster')" />
                        <xsl:sort select="not(@name='Info')" />
                        <xsl:sort select="not(@name='Tracks')" />
                        <xsl:sort select="not(@name='Attachments')" />
                        <xsl:sort select="not(@name='Chapters')" />
                        <xsl:sort select="not(@name='Cues')" />
                        <xsl:sort select="not(@name='Tags')" />
                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:for-each>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                                        ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
                            <xsl:sort select="not(@name='Cluster')"/>
                            <xsl:sort select="not(@name='TrackNumber')"/>
                            <xsl:sort select="not(@name='SimpleBlock')"/>
                            <xsl:sort select="not(@name='BlockGroup')"/>
                            <xsl:sort select="not(@name='Block')"/>
                            <xsl:sort select="not(@name='Video')"/>
                            <xsl:sort select="not(@name='Audio')"/>
                            <xsl:sort select="not(@name='Info')"/>
                            <xsl:sort select="not(@name='TimestampScale')"/>
                            <xsl:sort select="not(@name='CueTrack')"/>
                            <xsl:sort select="not(@name='CueClusterPosition')"/>
                            <xsl:sort select="@type='master'"/>
                        <xsl:sort select="@id" />

                        <xsl:call-template name="outputChildEbmlSyntax">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>

                    </xsl:for-each>
                </xsl:otherwise>
            </xsl:choose>

            <xsl:if test="@recursive='1'">
                <xsl:call-template name="outputChildEbmlSyntax">
                    <xsl:with-param name="node" select="."/>
                    <xsl:with-param name="recursive" select="@recursive"/>
                </xsl:call-template>
            </xsl:if>

            <xsl:if test="@name='BlockGroup'">
                <!-- Extra elements to do the Block parsing -->
                <xsl:text>    {                                      1, EBML_UINT,  0, 0, offsetof(MatroskaBlock, non_simple), { .u = 1 } },&#10;</xsl:text>
            </xsl:if>

            <xsl:choose>
                <xsl:when test="@name='Segment'">
                    <xsl:text>    { 0 }   /* We don't want to go back to level 0, so don't add the parent. */&#10;</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text>    CHILD_OF(matroska_</xsl:text>
                    <xsl:variable name="parentFullPath">
                        <xsl:call-template name="getParentPath">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:variable>
                    <xsl:call-template name="masterListName">
                        <xsl:with-param name="node" select="../ebml:element[@path = $parentFullPath]"/>
                    </xsl:call-template>
                    <xsl:text>)&#10;</xsl:text>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:text>};&#10;</xsl:text>

        </xsl:if>

    </xsl:template>

    <xsl:template name="getParentPath">
        <xsl:param name="node"/>
        <xsl:call-template name="parentName">
            <xsl:with-param name="pText" select="substring( $node/@path, 2 )"/>
        </xsl:call-template>
    </xsl:template>

    <xsl:template name="parentName">
        <xsl:param name="pText"/>
        <xsl:choose>
            <xsl:when test="contains($pText, '\')">
                <xsl:text>\</xsl:text>
                <xsl:value-of select="substring-before($pText, '\')"/>
                <xsl:call-template name="parentName">
                    <xsl:with-param name="pText" select="substring-after($pText, '\')"/>
                </xsl:call-template>
            </xsl:when>
            <!-- <xsl:otherwise>
                <xsl:value-of select="$pText"/>
            </xsl:otherwise> -->
        </xsl:choose>
    </xsl:template>

    <xsl:template name="masterHasStoredElts">
        <xsl:param name="node"/>

        <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                              ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
            <xsl:variable name="lavfStorage">
                <xsl:choose>
                    <xsl:when test="@type='master'">
                        <xsl:call-template name="masterHasStoredElts">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:call-template name="getLAVFStorage">
                            <xsl:with-param name="node" select="."/>
                            <xsl:with-param name="recursive" select="@recursive"/>
                        </xsl:call-template>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <xsl:value-of select="$lavfStorage"/>
        </xsl:for-each>

    </xsl:template>

    <xsl:template name="masterHasStoredMaster">
        <xsl:param name="node"/>

        <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                              ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
            <xsl:if test="@type='master'">
                <xsl:text>y</xsl:text>
            </xsl:if>
        </xsl:for-each>

    </xsl:template>

    <xsl:template name="masterCountChildren">
        <xsl:param name="node"/>

        <xsl:for-each select="../ebml:element[@path = concat(concat($node/@path, '\'), @name)] |
                              ../ebml:element[@path = concat(concat($node/@path, '\+'), @name)] ">
            <xsl:text>y</xsl:text>
        </xsl:for-each>

    </xsl:template>

    <xsl:template name="getLAVFStorage">
        <xsl:param name="node"/>
        <xsl:param name="recursive"/>

        <xsl:choose>
            <xsl:when test="$node/@name='DateUTC'"><xsl:text>date_utc</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Duration'"><xsl:text>duration</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MuxingApp'"><xsl:text>muxingapp</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Title'"><xsl:text>title</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Cluster'"><xsl:text>STOP</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TimestampScale'"><xsl:text>time_scale</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackEntry'"><xsl:text>tracks</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackNumber'"><xsl:text>num</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackName'"><xsl:text>name</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackUID'"><xsl:text>uid</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackType'"><xsl:text>type</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CodecID'"><xsl:text>codec_id</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagDefault'"><xsl:text>flag_default</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagForced'"><xsl:text>flag_forced</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagHearingImpaired'"><xsl:text>flag_hearingimpaired</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagVisualImpaired'"><xsl:text>flag_visualimpaired</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagTextDescriptions'"><xsl:text>flag_textdescriptions</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagOriginal'"><xsl:text>flag_original</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagCommentary'"><xsl:text>flag_comment</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CodecDelay'"><xsl:text>codec_delay</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CodecPrivate'"><xsl:text>codec_priv</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SeekPreRoll'"><xsl:text>seek_preroll</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Video'"><xsl:text>video</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Audio'"><xsl:text>audio</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackOperation'"><xsl:text>operation</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Name'"><xsl:text>name</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MaxBlockAdditionID'"><xsl:text>max_block_additional_id</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Language'"><xsl:text>language</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackTimestampScale'"><xsl:text>time_scale</xsl:text></xsl:when>
            <xsl:when test="$node/@name='DefaultDuration'"><xsl:text>default_duration</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncoding'"><xsl:text>encodings</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncodingScope'"><xsl:text>scope</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncodingType'"><xsl:text>type</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentCompression'"><xsl:text>compression</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncryption'"><xsl:text>encryption</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentCompAlgo'"><xsl:text>algo</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentCompSettings'"><xsl:text>settings</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncAlgo'"><xsl:text>algo</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncKeyID'"><xsl:text>key_id</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackPlane'"><xsl:text>combine_planes</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackPlaneUID'"><xsl:text>uid</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackPlaneType'"><xsl:text>type</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FlagInterlaced'"><xsl:text>interlaced</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FieldOrder'"><xsl:text>field_order</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PixelWidth'"><xsl:text>pixel_width</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PixelHeight'"><xsl:text>pixel_height</xsl:text></xsl:when>
            <xsl:when test="$node/@name='StereoMode'"><xsl:text>stereo_mode</xsl:text></xsl:when>
            <xsl:when test="$node/@name='AlphaMode'"><xsl:text>alpha_mode</xsl:text></xsl:when>
            <xsl:when test="$node/@name='DisplayWidth'"><xsl:text>display_width</xsl:text></xsl:when>
            <xsl:when test="$node/@name='DisplayHeight'"><xsl:text>display_height</xsl:text></xsl:when>
            <xsl:when test="$node/@name='DisplayUnit'"><xsl:text>display_unit</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Colour'"><xsl:text>color</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Projection'"><xsl:text>projection</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FrameRate'"><xsl:text>frame_rate</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ColourSpace'"><xsl:text>color_space</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MatrixCoefficients'"><xsl:text>matrix_coefficients</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BitsPerChannel'"><xsl:text>bits_per_channel</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChromaSubsamplingHorz'"><xsl:text>chroma_sub_horz</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChromaSubsamplingVert'"><xsl:text>chroma_sub_vert</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CbSubsamplingHorz'"><xsl:text>cb_sub_horz</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CbSubsamplingVert'"><xsl:text>cb_sub_vert</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChromaSitingHorz'"><xsl:text>chroma_siting_horz</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChromaSitingVert'"><xsl:text>chroma_siting_vert</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Range'"><xsl:text>range</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TransferCharacteristics'"><xsl:text>transfer_characteristics</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Primaries'"><xsl:text>primaries</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MaxCLL'"><xsl:text>max_cll</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MaxFALL'"><xsl:text>max_fall</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MasteringMetadata'"><xsl:text>mastering_meta</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrimaryRChromaticityX'"><xsl:text>r_x</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrimaryRChromaticityY'"><xsl:text>r_y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrimaryGChromaticityX'"><xsl:text>g_x</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrimaryGChromaticityY'"><xsl:text>g_y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrimaryBChromaticityX'"><xsl:text>b_x</xsl:text></xsl:when>
            <xsl:when test="$node/@name='PrimaryBChromaticityY'"><xsl:text>b_y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='WhitePointChromaticityX'"><xsl:text>white_x</xsl:text></xsl:when>
            <xsl:when test="$node/@name='WhitePointChromaticityY'"><xsl:text>white_y</xsl:text></xsl:when>
            <xsl:when test="$node/@name='LuminanceMax'"><xsl:text>max_luminance</xsl:text></xsl:when>
            <xsl:when test="$node/@name='LuminanceMin'"><xsl:text>min_luminance</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ProjectionType'"><xsl:text>type</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ProjectionPrivate'"><xsl:text>private</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ProjectionPoseYaw'"><xsl:text>yaw</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ProjectionPosePitch'"><xsl:text>pitch</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ProjectionPoseRoll'"><xsl:text>roll</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Channels'"><xsl:text>channels</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SamplingFrequency'"><xsl:text>samplerate</xsl:text></xsl:when>
            <xsl:when test="$node/@name='OutputSamplingFrequency'"><xsl:text>out_samplerate</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BitDepth'"><xsl:text>bitdepth</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CuePoint'"><xsl:text>index</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueTime'"><xsl:text>time</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueTrackPositions'"><xsl:text>pos</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueTime'"><xsl:text>time</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueClusterPosition'"><xsl:text>pos</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueTrack'"><xsl:text>track</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Targets'"><xsl:text>target</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockAdditionMapping'"><xsl:text>block_addition_mappings</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockAddIDValue'"><xsl:text>value</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockAddIDName'"><xsl:text>name</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockAddIDType'"><xsl:text>type</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockAddIDExtraData'"><xsl:text>extradata</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SimpleTag'">
                <xsl:choose>
                    <xsl:when test="$recursive='1'"><xsl:text>sub</xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>tag</xsl:text></xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:when test="$node/@name='AttachedFile'"><xsl:text>attachments</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileData'"><xsl:text>bin</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileMimeType'"><xsl:text>mime</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileDescription'"><xsl:text>description</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileName'"><xsl:text>filename</xsl:text></xsl:when>
            <xsl:when test="$node/@name='FileUID'"><xsl:text>uid</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SimpleBlock'"><xsl:text>bin</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Timestamp'"><xsl:text>timecode</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockDuration'"><xsl:text>duration</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Block'"><xsl:text>bin</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ReferenceBlock'"><xsl:text>reference</xsl:text></xsl:when>
            <xsl:when test="$node/@name='DiscardPadding'"><xsl:text>discard_padding</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Tag'"><xsl:text>tags</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagChapterUID'"><xsl:text>chapteruid</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagTrackUID'"><xsl:text>trackuid</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagAttachmentUID'"><xsl:text>attachuid</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TargetType'"><xsl:text>type</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TargetTypeValue'"><xsl:text>typevalue</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Seek'"><xsl:text>seekhead</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SeekID'"><xsl:text>id</xsl:text></xsl:when>
            <xsl:when test="$node/@name='SeekPosition'"><xsl:text>pos</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterAtom'">
                <xsl:choose>
                    <xsl:when test="$recursive='1'"></xsl:when>
                    <xsl:otherwise><xsl:text>chapters</xsl:text></xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:when test="$node/@name='BlockAdditional'"><xsl:text>additional</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockAddID'"><xsl:text>additional_id</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterTimeStart'"><xsl:text>start</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterTimeEnd'"><xsl:text>end</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterUID'"><xsl:text>uid</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapString'"><xsl:text>title</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagLanguage'"><xsl:text>lang</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagDefault'"><xsl:text>def</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagDefault_Bug'"><xsl:text>def</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagString'"><xsl:text>string</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TagName'"><xsl:text>name</xsl:text></xsl:when>

        </xsl:choose>

    </xsl:template>


    <!-- Type of elements stored in an EbmlList -->
    <xsl:template name="ebmlListStructure">
        <xsl:param name="node"/>
        <xsl:param name="recursive"/>

        <xsl:choose>
            <xsl:when test="$node/@type='master'">
                <xsl:choose>
                    <xsl:when test="$node/@name='TrackEntry'"><xsl:text>MatroskaTrack</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncoding'"><xsl:text>MatroskaTrackEncoding</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TrackPlane'"><xsl:text>MatroskaTrackPlane</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Colour'"><xsl:text>MatroskaTrackVideoColor</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='CuePoint'"><xsl:text>MatroskaIndex</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='CueTrackPositions'"><xsl:text>MatroskaIndexPos</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='AttachedFile'"><xsl:text>MatroskaAttachment</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Seek'"><xsl:text>MatroskaSeekhead</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Tag'"><xsl:text>MatroskaTags</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='SimpleTag'"><xsl:text>MatroskaTag</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockAdditionMapping'"><xsl:text>MatroskaBlockAdditionMapping</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChapterAtom' and not($recursive='1')"><xsl:text>MatroskaChapter</xsl:text></xsl:when>

                </xsl:choose>
            </xsl:when>
        </xsl:choose>
    </xsl:template>

    <!-- Type of elements stored in an Structure -->
    <xsl:template name="hardcodedStructure">
        <xsl:param name="node"/>
        <xsl:choose>
            <xsl:when test="$node/@type='master'">
                <xsl:choose>
                    <xsl:when test="$node/@name='Info'"><xsl:text>MatroskaDemuxContext</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Tracks'"><xsl:text>MatroskaDemuxContext</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Cues'"><xsl:text>MatroskaDemuxContext</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Tags'"><xsl:text>MatroskaDemuxContext</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Attachments'"><xsl:text>MatroskaDemuxContext</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='SeekHead'"><xsl:text>MatroskaDemuxContext</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TrackCombinePlanes'"><xsl:text>MatroskaTrackOperation</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncryption'"><xsl:text>MatroskaTrackEncryption</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentCompression'"><xsl:text>MatroskaTrackCompression</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentEncodings'"><xsl:text>MatroskaTrack</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Video'"><xsl:text>MatroskaTrackVideo</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Audio'"><xsl:text>MatroskaTrackAudio</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='MasteringMetadata'"><xsl:text>MatroskaMasteringMeta</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Projection'"><xsl:text>MatroskaTrackVideoProjection</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Cluster'"><xsl:text>MatroskaCluster</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockGroup'"><xsl:text>MatroskaBlock</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Targets'"><xsl:text>MatroskaTagTarget</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockMore'"><xsl:text>MatroskaBlock</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='EditionEntry'"><xsl:text>MatroskaDemuxContext</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChapterDisplay'"><xsl:text>MatroskaChapter</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='BlockAdditionMapping'"><xsl:text>MatroskaBlockAdditionMapping</xsl:text></xsl:when>

                </xsl:choose>
            </xsl:when>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="masterListName">
        <xsl:param name="node"/>
        <xsl:choose>
            <xsl:when test="$node/@name='TrackEntry'"><xsl:text>track</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Cues'"><xsl:text>index</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CuePoint'"><xsl:text>index_entry</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Audio'"><xsl:text>track_audio</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Video'"><xsl:text>track_video</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackOperation'"><xsl:text>track_operation</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Seek'"><xsl:text>seekhead_entry</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncodings'"><xsl:text>track_encodings</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncoding'"><xsl:text>track_encoding</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentCompression'"><xsl:text>track_encoding_compression</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ContentEncryption'"><xsl:text>track_encoding_encryption</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackCombinePlanes'"><xsl:text>track_combine_planes</xsl:text></xsl:when>
            <xsl:when test="$node/@name='TrackPlane'"><xsl:text>track_plane</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Colour'"><xsl:text>track_video_color</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Projection'"><xsl:text>track_video_projection</xsl:text></xsl:when>
            <xsl:when test="$node/@name='MasteringMetadata'"><xsl:text>mastering_meta</xsl:text></xsl:when>
            <xsl:when test="$node/@name='CueTrackPositions'"><xsl:text>index_pos</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Targets'"><xsl:text>tagtargets</xsl:text></xsl:when>
            <xsl:when test="$node/@name='AttachedFile'"><xsl:text>attachment</xsl:text></xsl:when>
            <xsl:when test="$node/@name='EditionEntry'"><xsl:text>chapter</xsl:text></xsl:when>
            <xsl:when test="$node/@name='Cluster'"><xsl:text>cluster_parsing</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterAtom'"><xsl:text>chapter_entry</xsl:text></xsl:when>
            <xsl:when test="$node/@name='ChapterDisplay'"><xsl:text>chapter_display</xsl:text></xsl:when>
            <xsl:when test="$node/@name='BlockAdditionMapping'"><xsl:text>block_addition_mapping</xsl:text></xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="translate($node/@name, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ ', 'abcdefghijklmnopqrstuvwxyz_')"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template name="outputChildEbmlSyntax">
        <xsl:param name="node"/>
        <xsl:param name="recursive"/>

        <!-- <xsl:if test="$isDiscarded=''"> -->
            <!-- Transform the ebml_matroska.xml name into the libavformat name -->
            <xsl:variable name="lavfName">
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

    <!-- <xsl:value-of select="$lavfName"/> -->
    <!-- <xsl:value-of select="$node/@name"/> -->

            <!-- Storage name in a structure if any -->
            <!-- Master elements are assumed to be always stored -->
            <xsl:variable name="lavfStorage">
                <xsl:call-template name="getLAVFStorage">
                    <xsl:with-param name="node" select="$node"/>
                    <xsl:with-param name="recursive" select="$recursive"/>
                </xsl:call-template>
            </xsl:variable>

            <!-- Structure name for master elements stored in an EbmlList -->
            <xsl:variable name="lavfListElementSize">
                <xsl:call-template name="ebmlListStructure">
                    <xsl:with-param name="node" select="$node"/>
                    <xsl:with-param name="recursive" select="$recursive"/>
                </xsl:call-template>
            </xsl:variable>

            <xsl:variable name="parentFullPath">
                <xsl:call-template name="getParentPath">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
            </xsl:variable>

            <!-- <xsl:variable name="parentNode" select="../ebml:element[@path = $parentFullPath]"/> -->
    <!-- <xsl:value-of select="$parentFullPath"/><xsl:text>&#10;</xsl:text> -->
    <!-- <xsl:value-of select="$parentNode/@name"/> -->


            <!-- Default value to use -->
            <!-- Master elements use their own sub-EbmlSyntax structure -->
            <xsl:variable name="lavfDefault">
                <xsl:choose>
                    <xsl:when test="$node/@type='master'">
                        <xsl:choose>
                            <xsl:when test="$recursive='1' and $node/@name='ChapterAtom'"></xsl:when>
                            <xsl:otherwise>
                                <xsl:text>matroska_</xsl:text>
                                <xsl:call-template name="masterListName">
                                    <xsl:with-param name="node" select="$node"/>
                                </xsl:call-template>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:when>
                    <!-- <xsl:when test="$parentFullPath='\Segment\Tracks\TrackEntry\Video\Colour\MasteringMetadata' and $node/@type='float'"><xsl:text>-1</xsl:text></xsl:when> -->
                    <xsl:when test="$node/@name='FlagInterlaced'"><xsl:text>MATROSKA_VIDEO_INTERLACE_FLAG_UNDETERMINED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='FieldOrder'"><xsl:text>MATROSKA_VIDEO_FIELDORDER_UNDETERMINED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='StereoMode'"><xsl:text>MATROSKA_VIDEO_STEREOMODE_TYPE_NB</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ProjectionType'"><xsl:text>MATROSKA_VIDEO_PROJECTION_TYPE_RECTANGULAR</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='MatrixCoefficients'"><xsl:text>AVCOL_SPC_UNSPECIFIED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Primaries'"><xsl:text>AVCOL_PRI_UNSPECIFIED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='TransferCharacteristics'"><xsl:text>AVCOL_TRC_UNSPECIFIED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='Range'"><xsl:text>AVCOL_RANGE_UNSPECIFIED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChromaSitingHorz'"><xsl:text>MATROSKA_COLOUR_CHROMASITINGHORZ_UNDETERMINED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChromaSitingVert'"><xsl:text>MATROSKA_COLOUR_CHROMASITINGVERT_UNDETERMINED</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='DisplayWidth'"><xsl:text>-1</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='DisplayHeight'"><xsl:text>-1</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='DisplayUnit'"><xsl:text>MATROSKA_VIDEO_DISPLAYUNIT_PIXELS</xsl:text></xsl:when>
                    <!-- <xsl:when test="$node/@name='ReferenceBlock'"><xsl:text>INT64_MIN</xsl:text></xsl:when> -->
                    <xsl:when test="$node/@name='ChapterTimeStart'"><xsl:text>AV_NOPTS_VALUE</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ChapterTimeEnd'"><xsl:text>AV_NOPTS_VALUE</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='SeekPosition'"><xsl:text>-1</xsl:text></xsl:when>
                    <xsl:when test="$node/@name='ContentCompAlgo'"><xsl:text>MATROSKA_TRACK_ENCODING_COMP_ZLIB</xsl:text></xsl:when>
                    <xsl:when test="$node/@default='0x1p+0'"><xsl:text>1.0</xsl:text></xsl:when>
                    <xsl:when test="$node/@default='0x0p+0'"><xsl:text>0.0</xsl:text></xsl:when>
                    <xsl:when test="$node/@default='0x1.f4p+12'"><xsl:text>8000.0</xsl:text></xsl:when>
                    <xsl:when test="$node/@type='master'">
                        <xsl:value-of select="$node/@default"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:if test="not($lavfStorage='')">
                            <xsl:value-of select="$node/@default"/>
                        </xsl:if>
                        <!-- <xsl:if test="$node/@type='float'">
                            <xsl:text>-1</xsl:text>
                        </xsl:if> -->
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:variable>

            <!-- generate EbmlSyntax.id -->
            <xsl:text>    { MATROSKA_ID_</xsl:text>
            <xsl:choose>
                <xsl:when test="string-length($lavfNameUpper) &lt; 26">
                    <xsl:value-of select="substring(concat($lavfNameUpper, ',                                     '),0,28)"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$lavfNameUpper"/><xsl:text>,</xsl:text>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:text> </xsl:text>

            <!-- generate EbmlSyntax.type -->
            <xsl:variable name="ebmlType">
                <xsl:choose>
                    <xsl:when test="$node/@name='Cluster'">
                        <xsl:text>EBML_STOP</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@name='SeekID'">
                        <!-- can be stored in an integer -->
                        <xsl:text>EBML_UINT</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='master'">
                        <xsl:choose>
                            <xsl:when test="$parentFullPath='\Segment'">
                                <xsl:text>EBML_LEVEL1</xsl:text>
                            </xsl:when>
                            <xsl:when test="$recursive='1' and $node/@name='ChapterAtom'">
                                <xsl:text>EBML_NONE</xsl:text>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:variable name="hasStoredElts">
                                    <xsl:call-template name="masterHasStoredElts">
                                        <xsl:with-param name="node" select="$node"/>
                                    </xsl:call-template>
                                </xsl:variable>

                                <xsl:choose>
                                    <xsl:when test="$hasStoredElts=''"><xsl:text>EBML_NONE</xsl:text></xsl:when>
                                    <xsl:otherwise><xsl:text>EBML_NEST</xsl:text></xsl:otherwise>
                                </xsl:choose>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:when>
                    <xsl:when test="$lavfStorage=''">
                        <xsl:text>EBML_NONE</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='uinteger'">
                        <xsl:text>EBML_UINT</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='integer'">
                        <xsl:text>EBML_SINT</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='utf-8'">
                        <xsl:text>EBML_UTF8</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='string'">
                        <xsl:text>EBML_STR</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='binary'">
                        <xsl:text>EBML_BIN</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='date'">
                        <xsl:text>EBML_BIN</xsl:text>
                    </xsl:when>
                    <xsl:when test="$node/@type='float'">
                        <xsl:text>EBML_FLOAT</xsl:text>
                    </xsl:when>
                </xsl:choose>
            </xsl:variable>
            <xsl:choose>
                <xsl:when test="$ebmlType='EBML_NONE'">
                    <xsl:value-of select="$ebmlType"/>
                </xsl:when>
                <xsl:when test="$ebmlType='EBML_STOP'">
                    <xsl:value-of select="$ebmlType"/>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="substring(concat($ebmlType, ',                   '),0,13)"/>
                </xsl:otherwise>
            </xsl:choose>

            <!-- generate EbmlSyntax.is_counted -->
            <xsl:choose>
                <xsl:when test="$lavfStorage='STOP'">
                </xsl:when>
                <xsl:when test="$ebmlType='EBML_NONE' and @type='master'">
                </xsl:when>
                <xsl:when test="$node/@name='LuminanceMin'"><xsl:text>1, </xsl:text></xsl:when>
                <xsl:when test="$node/@name='FlagOriginal'"><xsl:text>1, </xsl:text></xsl:when>
                <xsl:when test="$node/@name='ReferenceBlock'"><xsl:text>1, </xsl:text></xsl:when>
                <xsl:when test="$lavfListElementSize='' and not($lavfDefault='')">
                    <xsl:text>0, </xsl:text>
                </xsl:when>
                <xsl:when test="$lavfListElementSize='' and $lavfStorage=''">
                </xsl:when>
                <xsl:when test="$lavfListElementSize='' and not($lavfStorage='')">
                    <xsl:text>0, </xsl:text>
                </xsl:when>
                <xsl:when test="$lavfListElementSize=''">
                </xsl:when>
                <xsl:when test="$lavfListElementSize">
                    <xsl:text>0, </xsl:text>
                </xsl:when>
            </xsl:choose>

            <!-- generate EbmlSyntax.list_elem_size -->
            <xsl:choose>
                <xsl:when test="$lavfStorage='STOP'">
                </xsl:when>
                <xsl:when test="$ebmlType='EBML_NONE' and @type='master'">
                </xsl:when>
                <xsl:when test="$lavfListElementSize='' and not($lavfDefault='')">
                    <xsl:text>0, </xsl:text>
                </xsl:when>
                <xsl:when test="$lavfListElementSize='' and $lavfStorage=''">
                </xsl:when>
                <xsl:when test="$lavfListElementSize='' and not($lavfStorage='')">
                    <xsl:text>0, </xsl:text>
                </xsl:when>
                <xsl:when test="$lavfListElementSize=''">
                </xsl:when>
                <xsl:when test="$lavfListElementSize">
                    <xsl:text>sizeof(</xsl:text>
                    <xsl:value-of select="$lavfListElementSize"/>
                    <xsl:text>), </xsl:text>
                </xsl:when>
            </xsl:choose>

            <!-- generate EbmlSyntax.data_offset -->
            <xsl:choose>
                <xsl:when test="$lavfStorage='STOP'">
                </xsl:when>
                <xsl:when test="$ebmlType='EBML_NONE' and @type='master'">
                </xsl:when>
                <xsl:when test="$lavfStorage='' and not($lavfDefault='')">
                    <xsl:text>0</xsl:text>
                </xsl:when>
                <xsl:when test="$lavfStorage=''">
                </xsl:when>
                <xsl:otherwise>
                    <xsl:variable name="parentStructureFromList">
                        <xsl:call-template name="ebmlListStructure">
                            <xsl:with-param name="node" select="../ebml:element[@path = $parentFullPath]"/>
                        </xsl:call-template>
                    </xsl:variable>

                    <xsl:variable name="parentStructure">
                        <xsl:choose>
                            <xsl:when test="$recursive=1">
                                <xsl:call-template name="ebmlListStructure">
                                    <xsl:with-param name="node" select="$node"/>
                                </xsl:call-template>
                            </xsl:when>
                            <xsl:when test="not($parentStructureFromList='')">
                                <xsl:value-of select="$parentStructureFromList"/>
                            </xsl:when>
                            <xsl:when test="@name='SimpleBlock'">
                                <xsl:text>MatroskaBlock</xsl:text>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:call-template name="hardcodedStructure">
                                    <xsl:with-param name="node" select="../ebml:element[@path = $parentFullPath]"/>
                                </xsl:call-template>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:variable>
    <!-- <xsl:value-of select="$parentStructure"/> -->

                    <xsl:text>offsetof(</xsl:text>
                    <xsl:value-of select="$parentStructure"/>
                    <xsl:text>, </xsl:text>
                    <xsl:value-of select="$lavfStorage"/>
                    <xsl:text>)</xsl:text>
                </xsl:otherwise>
            </xsl:choose>

            <!-- generate EbmlSyntax.def -->
            <xsl:choose>
                <xsl:when test="$lavfStorage='STOP'">
                </xsl:when>
                <xsl:when test="$ebmlType='EBML_NONE' and @type='master'">
                </xsl:when>
                <xsl:when test="$lavfDefault=''">
                </xsl:when>
                <xsl:when test="$node/@type='uinteger' and $lavfDefault='0'">
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text>, { </xsl:text>
                    <xsl:choose>
                        <xsl:when test="$node/@type='uinteger'">
                            <xsl:text>.u = </xsl:text>
                        </xsl:when>
                        <xsl:when test="$node/@type='integer'">
                            <xsl:text>.i = </xsl:text>
                        </xsl:when>
                        <xsl:when test="$node/@type='float'">
                            <xsl:text>.f = </xsl:text>
                        </xsl:when>
                        <xsl:when test="$node/@type='string'">
                            <xsl:text>.s = "</xsl:text>
                        </xsl:when>
                        <xsl:when test="$node/@type='utf-8'">
                            <xsl:text>.s = "</xsl:text>
                        </xsl:when>
                        <xsl:when test="$node/@type='master'">
                            <xsl:text>.n = </xsl:text>
                        </xsl:when>
                    </xsl:choose>
                    <xsl:value-of select="$lavfDefault"/>
                    <xsl:choose>
                        <xsl:when test="$node/@type='string'">
                            <xsl:text>"</xsl:text>
                        </xsl:when>
                        <xsl:when test="$node/@type='utf-8'">
                            <xsl:text>"</xsl:text>
                        </xsl:when>
                    </xsl:choose>
                    <xsl:text> }</xsl:text>
                </xsl:otherwise>
            </xsl:choose>

            <xsl:text> },&#10;</xsl:text>
        <!-- </xsl:if> -->
    </xsl:template>

</xsl:stylesheet>
