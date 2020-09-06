<?xml version="1.0"?>
<!--
    File used to generate libmatroska KaxSemantic.h from ebml_matroska.xml 
    Usage: xsltproc -o KaxSemantic.h schema_2_kaxsematic_h.xsl ebml_matroska.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" 
    xmlns:str="http://exslt.org/strings"
    exclude-result-prefixes="str xhtml ebml"
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns="urn:ietf:rfc:8794" xmlns:ebml="urn:ietf:rfc:8794">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes" />
  <xsl:template match="ebml:EBMLSchema">/**********************************************************************
**  DO NOT EDIT, GENERATED WITH DATA2LIB
**  https://github.com/Matroska-Org/foundation-source/tree/master/spectool
**
**  libmatroska : parse Matroska files, see https://www.matroska.org/
**
**  Copyright (c) 2002-2020, Matroska (non-profit organisation)
**  All rights reserved.
**
** This file is part of libmatroska.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** See http://www.gnu.org/licenses/lgpl-2.1.html for LGPL licensing information.**
** Contact license@matroska.org if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/


#ifndef LIBMATROSKA_SEMANTIC_H
#define LIBMATROSKA_SEMANTIC_H

#include "matroska/KaxTypes.h"
#include "ebml/EbmlUInteger.h"
#include "ebml/EbmlSInteger.h"
#include "ebml/EbmlDate.h"
#include "ebml/EbmlFloat.h"
#include "ebml/EbmlString.h"
#include "ebml/EbmlUnicodeString.h"
#include "ebml/EbmlBinary.h"
#include "ebml/EbmlMaster.h"
#include "matroska/KaxDefines.h"

using namespace LIBEBML_NAMESPACE;

START_LIBMATROSKA_NAMESPACE
<xsl:for-each select="ebml:element[not(starts-with(@path,'\EBML\'))]">
    <!-- <xsl:sort select="translate(@path, '\+', '\')" /> -->
    <xsl:apply-templates select="."/>
</xsl:for-each>
END_LIBMATROSKA_NAMESPACE

#endif // LIBMATROSKA_SEMANTIC_H
</xsl:template>
  <xsl:template match="ebml:element">
    <!-- Ignore EBML extra constraints -->
    <xsl:if test="@name!='Segment' and @name!='Cluster' and @name!='BlockGroup' and @name!='Block' and @name!='BlockVirtual' and @name!='ReferenceBlock' and @name!='SimpleBlock' and @name!='Cues' and @name!='CuePoint' and @name!='CueTrackPositions' and @name!='CueReference' and @name!='NextUID' and @name!='PrevUID' and @name!='SeekHead' and @name!='Seek' and @name!='TrackEntry'">
    <!-- <xsl:copy> -->

        <xsl:variable name="minVer">
            <xsl:call-template name="get-min-ver">
                <xsl:with-param name="node" select="."/>
            </xsl:call-template>
        </xsl:variable>

        <!-- <xsl:if test="$minVer &gt; 1 or ebml:extension[@divx='1']">#if MATROSKA_VERSION >= 2&#10;</xsl:if> -->
        <xsl:choose>
            <xsl:when test="@type='master'">
                <xsl:text>DECLARE_MKX_MASTER(Kax</xsl:text>
            </xsl:when>
            <xsl:when test="@type='binary'">
                <xsl:text>DECLARE_MKX_BINARY (Kax</xsl:text>
            </xsl:when>
            <xsl:when test="@type='uinteger'">
                <xsl:text>DECLARE_MKX_UINTEGER(Kax</xsl:text>
            </xsl:when>
            <xsl:when test="@type='integer'">
                <xsl:text>DECLARE_MKX_SINTEGER(Kax</xsl:text>
            </xsl:when>
            <xsl:when test="@type='utf-8'">
                <xsl:text>DECLARE_MKX_UNISTRING(Kax</xsl:text>
            </xsl:when>
            <xsl:when test="@type='string'">
                <xsl:text>DECLARE_MKX_STRING(Kax</xsl:text>
            </xsl:when>
            <xsl:when test="@type='float'">
                <xsl:text>DECLARE_MKX_FLOAT(Kax</xsl:text>
            </xsl:when>
            <xsl:when test="@type='date'">
                <xsl:text>DECLARE_MKX_DATE    (Kax</xsl:text>
            </xsl:when>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="ebml:extension[@cppname]"><xsl:value-of select="ebml:extension[@cppname][1]/@cppname" /></xsl:when>
            <xsl:otherwise><xsl:value-of select="@name" /></xsl:otherwise>
        </xsl:choose>
        <xsl:text>)&#10;</xsl:text>
        <xsl:if test="@name='SegmentUID'">
            <xsl:text>#if defined(HAVE_EBML2) || defined(HAS_EBML2)&#10;</xsl:text>
            <xsl:text>public:&#10;</xsl:text>
            <xsl:text>  KaxSegmentUID(EBML_DEF_CONS EBML_DEF_SEP EBML_EXTRA_PARAM);&#10;</xsl:text>
            <xsl:text>#endif&#10;</xsl:text>
        </xsl:if>
        <xsl:if test="@maxver='0' or @maxver='1' or @maxver='2' or @maxver='3' or @length">
            <xsl:text>public:&#10;</xsl:text>
        </xsl:if>
        <xsl:if test="@maxver='0' or @maxver='1' or @maxver='2' or @maxver='3'">
            <xsl:text>  filepos_t RenderData(IOCallback &amp; output, bool bForceRender, bool bSaveDefault);&#10;</xsl:text>
        </xsl:if>
        <xsl:if test="@length">
            <xsl:text>  virtual bool ValidateSize() const {return IsFiniteSize() &amp;&amp; GetSize() </xsl:text>
            <xsl:choose>
                <xsl:when test="contains(@length, '=') or contains(@length, '&lt;') or contains(@length, '&gt;')"><xsl:value-of select="@length"/></xsl:when>
                <xsl:otherwise><xsl:text>== </xsl:text><xsl:value-of select="@length"/></xsl:otherwise>
            </xsl:choose>
            <xsl:text>;}&#10;</xsl:text>
        </xsl:if>
        <xsl:text>};&#10;</xsl:text>
        <!-- <xsl:if test="$minVer &gt; 1 or ebml:extension[@divx='1']">#endif&#10;</xsl:if> -->
        <xsl:text>&#10;</xsl:text>
    <!-- </xsl:copy> -->
    </xsl:if>
  </xsl:template>

  <xsl:template name="get-min-ver">
    <xsl:param name="node"/>
    <xsl:choose>
        <xsl:when test="$node/@name='EncryptedBlock'">2</xsl:when>
        <xsl:when test="$node/@name='BlockVirtual'">2</xsl:when>
        <xsl:when test="$node/@name='ReferenceVirtual'">2</xsl:when>
        <xsl:when test="$node/@name='FrameNumber'">2</xsl:when>
        <xsl:when test="$node/@name='BlockAdditionID'">2</xsl:when>
        <xsl:when test="$node/@name='Delay'">2</xsl:when>
        <xsl:when test="$node/@name='SliceDuration'">2</xsl:when>
        <xsl:when test="$node/@name='TrackOffset'">2</xsl:when>
        <xsl:when test="$node/@name='CodecSettings'">2</xsl:when>
        <xsl:when test="$node/@name='CodecInfoURL'">2</xsl:when>
        <xsl:when test="$node/@name='CodecDownloadURL'">2</xsl:when>
        <xsl:when test="$node/@name='OldStereoMode'">2</xsl:when>
        <xsl:when test="$node/@name='GammaValue'">2</xsl:when>
        <xsl:when test="$node/@name='FrameRate'">2</xsl:when>
        <xsl:when test="$node/@name='ChannelPositions'">2</xsl:when>
        <xsl:when test="$node/@name='CueRefCluster'">2</xsl:when>
        <xsl:when test="$node/@name='CueRefNumber'">2</xsl:when>
        <xsl:when test="$node/@name='CueRefCodecState'">2</xsl:when>
        <xsl:otherwise><xsl:value-of select="$node/@minver" /></xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="documentation">
    <documentation>
        <xsl:attribute name="lang"><xsl:value-of select="@lang" /></xsl:attribute>
        <xsl:if test="@type">
            <xsl:attribute name="type"><xsl:value-of select="@type" /></xsl:attribute>
        </xsl:if>
        <!-- <xsl:attribute name="type">
            <xsl:choose>
                <xsl:when test="@type">
                    <xsl:value-of select="@type"/>
                </xsl:when>
                <xsl:otherwise>documentation</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute> -->
        <!-- make sure the links are kept -->
        <xsl:apply-templates/>
    </documentation>
  </xsl:template>

  <!-- HTML tags found in documentation -->
  <xsl:template match="a">
    <a href="{@href}"><xsl:apply-templates/></a>
  </xsl:template>
  <xsl:template match="strong">
    <strong><xsl:apply-templates/></strong>
  </xsl:template>
  <xsl:template match="br">
    <br/><xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="comment()">
    <xsl:comment>
      <xsl:value-of select="."/>
    </xsl:comment>
  </xsl:template>

  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
