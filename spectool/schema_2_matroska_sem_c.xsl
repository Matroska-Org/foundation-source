<?xml version="1.0"?>
<!--
    File used to generate libmatroska2 matroska_sem.c from ebml_matroska.xml 
    Usage: xsltproc -o matroska_sem.c schema_2_matroska_sem_c.xsl ebml_matroska.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" 
    xmlns:str="http://exslt.org/strings"
    exclude-result-prefixes="str xhtml ebml"
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns="urn:ietf:rfc:8794" xmlns:ebml="urn:ietf:rfc:8794">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes" />
  <xsl:template match="ebml:EBMLSchema">/*
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

<xsl:call-template name="output-element-declaration">
    <xsl:with-param name="node" select="ebml:element[@path='\Segment']"/>
</xsl:call-template>
<xsl:call-template name="output-element-semantic">
    <xsl:with-param name="node" select="ebml:element[@path='\Segment']"/>
</xsl:call-template>
<xsl:text>&#10;void MATROSKA_InitSemantic()&#10;</xsl:text>
<xsl:text>{&#10;</xsl:text>
<xsl:call-template name="output-element-definition">
    <xsl:with-param name="node" select="ebml:element[@path='\Segment']"/>
</xsl:call-template>
<xsl:text>}&#10;</xsl:text>
</xsl:template>

  <xsl:template name="output-element-declaration">
    <xsl:param name="node"/>

    <xsl:variable name="plainPath">
        <xsl:value-of select="translate($node/@path, '\+', '\')" />
    </xsl:variable>

    <xsl:if test="$node/@type='master'">
        <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
            <xsl:call-template name="output-element-declaration">
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

    <xsl:text>ebml_context MATROSKA_Context</xsl:text>
    <xsl:value-of select="$lib2Name" />
    <xsl:text>;&#10;</xsl:text>
    <xsl:text>const ebml_context *MATROSKA_getContext</xsl:text>
    <xsl:value-of select="$lib2Name" />
    <xsl:text>() { return &amp;MATROSKA_Context</xsl:text>
    <xsl:value-of select="$lib2Name" />
    <xsl:text>; }&#10;</xsl:text>
  </xsl:template>

  <xsl:template name="output-element-semantic">
    <xsl:param name="node"/>

    <xsl:variable name="plainPath">
        <xsl:value-of select="translate($node/@path, '\+', '\')" />
    </xsl:variable>

    <xsl:if test="$node/@type='master'">
        <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
            <xsl:call-template name="output-element-semantic">
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

    <xsl:choose>
        <xsl:when test="$node/@type='master'">
            <xsl:text>&#10;const ebml_semantic EBML_Semantic</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text>[] = {&#10;</xsl:text>
            <xsl:if test="$node/@recursive=1">
                <xsl:call-template name="outputContextTableItem">
                    <xsl:with-param name="node" select="$node"/>
                    <xsl:with-param name="asRecursive" select="1"/>
                </xsl:call-template>
            </xsl:if>
            <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                <xsl:call-template name="outputContextTableItem">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
            </xsl:for-each>
            <xsl:text>    {0, 0, NULL ,0} // end of the table&#10;</xsl:text>
            <xsl:text>};&#10;</xsl:text>

        </xsl:when>
    </xsl:choose>
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

    <xsl:choose>
        <xsl:when test="$node/@type='master'">
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$node/@name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:choose>
                <xsl:when test="$node/@name='AttachedFile'"><xsl:text>MATROSKA_ATTACHMENT</xsl:text></xsl:when>
                <xsl:when test="$node/@name='Cluster'"><xsl:text>MATROSKA_CLUSTER</xsl:text></xsl:when>
                <xsl:when test="$node/@name='BlockGroup'"><xsl:text>MATROSKA_BLOCKGROUP</xsl:text></xsl:when>
                <xsl:when test="$node/@name='CuePoint'"><xsl:text>MATROSKA_CUEPOINT</xsl:text></xsl:when>
                <xsl:when test="$node/@name='Seek'"><xsl:text>MATROSKA_SEEKPOINT</xsl:text></xsl:when>
                <xsl:when test="$node/@name='TrackEntry'"><xsl:text>MATROSKA_TRACKENTRY</xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>EBML_MASTER</xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>_CLASS</xsl:text>
            <xsl:text>, 0, </xsl:text>
            <xsl:text>0, </xsl:text>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>EBML_Semantic</xsl:text>
            <xsl:value-of select="$node/@name" />
            <xsl:text>, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
            
        </xsl:when>
        <xsl:when test="$node/@type='binary'">
            
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:choose>
                <xsl:when test="$node/@name='FileData'"><xsl:text>MATROSKA_BIGBINARY</xsl:text></xsl:when>
                <xsl:when test="$node/@name='Block' or $node/@name='SimpleBlock'"><xsl:text>MATROSKA_BLOCK</xsl:text></xsl:when>
                <xsl:when test="$node/@name='SegmentUID' or $node/@name='NextUID' or $node/@name='PrevUID'"><xsl:text>MATROSKA_SEGMENTUID</xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>EBML_BINARY</xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>_CLASS</xsl:text>
            <xsl:text>, 0, </xsl:text>
            <xsl:text>0, </xsl:text>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>NULL, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
        </xsl:when>
        <xsl:when test="$node/@type='uinteger'">
            
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:choose>
                <xsl:when test="$node/@range='0-1'"><xsl:text>EBML_BOOLEAN_CLASS, </xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>EBML_INTEGER_CLASS, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
                <xsl:when test="$node/@default and (number($node/@default)=number($node/@default))"><xsl:text>1, (intptr_t)</xsl:text><xsl:value-of select="$node/@default" /><xsl:text>, </xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>NULL, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
            
        </xsl:when>
        <xsl:when test="$node/@type='integer'">
            
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:text>EBML_SINTEGER_CLASS, </xsl:text>
            <xsl:choose>
                <xsl:when test="$node/@default and (number($node/@default)=number($node/@default))"><xsl:text>1, (intptr_t)</xsl:text><xsl:value-of select="$node/@default" /><xsl:text>, </xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>NULL, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
            
        </xsl:when>
        <xsl:when test="$node/@type='utf-8'">
            
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:text>EBML_UNISTRING_CLASS, </xsl:text>
            <xsl:choose>
                <xsl:when test="$node/@default"><xsl:text>1, (intptr_t)"</xsl:text><xsl:value-of select="$node/@default" /><xsl:text>", </xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>NULL, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
            
        </xsl:when>
        <xsl:when test="$node/@type='string'">
            
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:text>EBML_STRING_CLASS, </xsl:text>
            <xsl:choose>
                <xsl:when test="$node/@default"><xsl:text>1, (intptr_t)"</xsl:text><xsl:value-of select="$node/@default" /><xsl:text>", </xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>NULL, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
            
        </xsl:when>
        <xsl:when test="$node/@type='float'">
            
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:text>EBML_FLOAT_CLASS, </xsl:text>
            <xsl:choose>
                <xsl:when test="$node/@default and starts-with($node/@default,'0x')">
                    <xsl:text>1, (intptr_t)</xsl:text>
                    <xsl:choose>
                        <xsl:when test="$node/@default='0x1p+0'">1.0, </xsl:when>
                        <xsl:when test="$node/@default='0x0p+0'">0.0, </xsl:when>
                        <xsl:when test="$node/@default='0x1.f4p+12'">8000.0, </xsl:when>
                        <xsl:otherwise><xsl:text>, </xsl:text><xsl:value-of select="$node/@default" /></xsl:otherwise>>
                    </xsl:choose>
                </xsl:when>
                <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>NULL, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
            
        </xsl:when>
        <xsl:when test="$node/@type='date'">
            <xsl:text>    MATROSKA_Context</xsl:text>
            <xsl:value-of select="$lib2Name" />
            <xsl:text> = (ebml_context) {</xsl:text>
            <xsl:value-of select="$node/@id" /><xsl:text>, </xsl:text>
            <xsl:text>EBML_DATE_CLASS, </xsl:text>
            <xsl:text>0, </xsl:text>
            <xsl:text>0, </xsl:text>
            <xsl:text>"</xsl:text><xsl:value-of select="$lib2Name" /><xsl:text>", </xsl:text>
            <xsl:text>NULL, </xsl:text>
            <xsl:text>EBML_getSemanticGlobals(), </xsl:text>
            <xsl:text>NULL</xsl:text>
            <xsl:text>};&#10;</xsl:text>
        </xsl:when>
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

  <xsl:template name="outputContextTableItem">
    <xsl:param name="node"/>
    <xsl:param name="asRecursive"/>

    <xsl:variable name="lib2Name">
        <xsl:choose>
            <xsl:when test="$node/@name='ChapterTrackUID'"><xsl:text>ChapterTrackNumber</xsl:text></xsl:when>
            <xsl:otherwise><xsl:value-of select="$node/@name"/></xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:text>    {</xsl:text>
    <xsl:choose>
        <xsl:when test="$asRecursive=1"><xsl:text>0</xsl:text></xsl:when>
        <xsl:when test="$node/@minOccurs!='' and $node/@minOccurs!='0'"><xsl:text>1</xsl:text></xsl:when>
        <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:text>, </xsl:text>
    <xsl:choose>
        <xsl:when test="$node/@maxOccurs='1'"><xsl:text>1</xsl:text></xsl:when>
        <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:text>, &amp;MATROSKA_Context</xsl:text>
    <xsl:value-of select="$lib2Name" />
    <xsl:text>, </xsl:text>
    <xsl:if test="(not($node/@minver) or $node/@minver &lt; 2) and not(ebml:extension[@webm='0']) and not(ebml:extension[@divx='0'])"><xsl:text>0</xsl:text></xsl:if>
    <xsl:if test="($node/@maxver and $node/@maxver &lt; 1) or ($node/@minver &gt; 1)">PROFILE_MATROSKA_V1</xsl:if>
    <xsl:if test="($node/@maxver and $node/@maxver &lt; 2) or ($node/@minver &gt; 2)">
        <xsl:if test="($node/@maxver and $node/@maxver &lt; 1) or ($node/@minver &gt; 1)"><xsl:text>|</xsl:text></xsl:if>
        <xsl:text>PROFILE_MATROSKA_V2</xsl:text>
    </xsl:if>
    <xsl:if test="($node/@maxver and $node/@maxver &lt; 3) or ($node/@minver &gt; 3)">
        <xsl:if test="($node/@maxver and $node/@maxver &lt; 2) or ($node/@minver &gt; 2)"><xsl:text>|</xsl:text></xsl:if>
        <xsl:text>PROFILE_MATROSKA_V3</xsl:text>
    </xsl:if>
    <xsl:if test="($node/@maxver and $node/@maxver &lt; 4) or ($node/@minver &gt; 4)">
        <xsl:if test="($node/@maxver and $node/@maxver &lt; 3) or ($node/@minver &gt; 3)"><xsl:text>|</xsl:text></xsl:if>
        <xsl:text>PROFILE_MATROSKA_V4</xsl:text>
    </xsl:if>
    <xsl:if test="ebml:extension[@divx='0'] or ((not(ebml:extension[@divx]) or not(ebml:extension[@divx='1'])) and (($node/@maxver and $node/@maxver &lt; 2) or ($node/@minver &gt; 1)))">
        <xsl:if test="($node/@maxver) or ($node/@minver &gt; 1)"><xsl:text>|</xsl:text></xsl:if>
        <xsl:text>PROFILE_DIVX</xsl:text>
    </xsl:if>
    <xsl:if test="ebml:extension[@webm='0']">
        <xsl:if test="($node/@maxver) or ($node/@minver &gt; 1) or ebml:extension[@divx='0']">|</xsl:if>
        <xsl:text>PROFILE_WEBM</xsl:text>
    </xsl:if>
    <xsl:text>},</xsl:text>
    <xsl:if test="$asRecursive=1"><xsl:text> // recursive</xsl:text></xsl:if>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
