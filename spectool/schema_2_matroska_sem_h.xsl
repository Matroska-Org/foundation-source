<?xml version="1.0"?>
<!--
    File used to generate libmatroska2 matroska_sem.h from ebml_matroska.xml 
    Usage: xsltproc -o matroska_sem.h schema_2_matroska_sem_h.xsl ebml_matroska.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" 
    xmlns:str="http://exslt.org/strings"
    exclude-result-prefixes="str xhtml ebml"
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns="urn:ietf:rfc:8794" xmlns:ebml="urn:ietf:rfc:8794">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes" />
  <xsl:template match="ebml:EBMLSchema">/*
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
#include "matroska2/matroska.h"

#ifndef MATROSKA_SEMANTIC_H
#define MATROSKA_SEMANTIC_H

void MATROSKA_InitSemantic();

<xsl:call-template name="output-element-definition">
    <xsl:with-param name="node" select="ebml:element[@path='\Segment']"/>
</xsl:call-template>#endif // MATROSKA_SEMANTIC_H
</xsl:template>
  <xsl:template name="output-element-definition">
    <xsl:param name="node"/>

    <xsl:variable name="plainPath">
        <xsl:value-of select="translate($node/@path, '\+', '\')" />
    </xsl:variable>

    <xsl:if test="$node/@type='master'">
        <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
            <xsl:call-template name="output-element-definition">
                <xsl:with-param name="node" select="."/>
            </xsl:call-template>
        </xsl:for-each>
    </xsl:if>

<!-- <xsl:text>plainPath = </xsl:text><xsl:value-of select="$plainPath" /><xsl:text>&#10;</xsl:text> -->
        <xsl:variable name="lib2Name">
            <xsl:choose>
                <xsl:when test="$node/@name='ChapterTrackUID'"><xsl:text>ChapterTrackNumber</xsl:text></xsl:when>
                <xsl:otherwise><xsl:value-of select="$node/@name"/></xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:text>MATROSKA_DLL const ebml_context *MATROSKA_getContext</xsl:text>
        <xsl:value-of select="$lib2Name" />
        <xsl:text>();&#10;</xsl:text>
        <xsl:if test="$node/@type='master'"><xsl:text>&#10;</xsl:text></xsl:if>
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
