<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:str="http://exslt.org/strings" exclude-result-prefixes="str">
  <!-- File used to generate KaxSemantic.cpp -->
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes" />
  <xsl:template match="EBMLSchema">/**********************************************************************
 *  DO NOT EDIT, GENERATED WITH schema_2_matroska_sem_c.xsl
 * https://github.com/Matroska-Org/foundation-source/tree/master/spectool
 *
 * $Id$
 * Copyright (c) 2008-2019, Matroska (non-profit organisation)
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
<xsl:apply-templates select="element">
    <xsl:sort select="translate(substring-before(substring-after(@path,'('),')'), '(1*(', '')" />
</xsl:apply-templates>
</xsl:template>
  <xsl:template match="element">
    <xsl:variable name="plainPath"><xsl:value-of select="translate(substring-before(substring-after(@path,'('),')'), '(1*(', '')" /></xsl:variable>
<!-- <xsl:text>plainPath = </xsl:text><xsl:value-of select="$plainPath" /><xsl:text>&#10;</xsl:text> -->
    <!-- Ignore EBML extra constraints -->
    <xsl:if test="not(starts-with($plainPath,'\EBML\'))">
    <xsl:copy>
        <xsl:choose>
            <xsl:when test="@type='master'">
                <xsl:text>&#10;const ebml_semantic EBML_Semantic</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text>[] = {&#10;</xsl:text>
                <xsl:call-template name="list-master-children">
                    <xsl:with-param name="findPath"><xsl:value-of select="$plainPath" /></xsl:with-param>
                </xsl:call-template>
                <xsl:text>    {0, 0, NULL ,0} // end of the table&#10;</xsl:text>
                <xsl:text>};&#10;</xsl:text>

                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:choose>
                    <xsl:when test="@name='AttachedFile'"><xsl:text>MATROSKA_ATTACHMENT</xsl:text></xsl:when>
                    <xsl:when test="@name='Cluster'"><xsl:text>MATROSKA_CLUSTER</xsl:text></xsl:when>
                    <xsl:when test="@name='BlockGroup'"><xsl:text>MATROSKA_BLOCKGROUP</xsl:text></xsl:when>
                    <xsl:when test="@name='CuePoint'"><xsl:text>MATROSKA_CUEPOINT</xsl:text></xsl:when>
                    <xsl:when test="@name='Seek'"><xsl:text>MATROSKA_SEEKPOINT</xsl:text></xsl:when>
                    <xsl:when test="@name='TrackEntry'"><xsl:text>MATROSKA_TRACKENTRY</xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>EBML_MASTER</xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>_CLASS</xsl:text>
                <xsl:text>, 0, </xsl:text>
                <xsl:text>0, </xsl:text>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>EBML_Semantic</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text>, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
                
            </xsl:when>
            <xsl:when test="@type='binary'">
                
                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:choose>
                    <xsl:when test="@name='FileData'"><xsl:text>MATROSKA_BIGBINARY</xsl:text></xsl:when>
                    <xsl:when test="@name='Block' or @name='SimpleBlock'"><xsl:text>MATROSKA_BLOCK</xsl:text></xsl:when>
                    <xsl:when test="@name='SegmentUID' or @name='NextUID' or @name='PrevUID'"><xsl:text>MATROSKA_SEGMENTUID</xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>EBML_BINARY</xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>_CLASS</xsl:text>
                <xsl:text>, 0, </xsl:text>
                <xsl:text>0, </xsl:text>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>NULL, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
            </xsl:when>
            <xsl:when test="@type='uinteger'">
                
                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:choose>
                    <xsl:when test="@range='0-1'"><xsl:text>EBML_BOOLEAN_CLASS, </xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>EBML_INTEGER_CLASS, </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:choose>
                    <xsl:when test="@default and (number(@default)=number(@default))"><xsl:text>1, (intptr_t)</xsl:text><xsl:value-of select="@default" /><xsl:text>, </xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>NULL, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
                
            </xsl:when>
            <xsl:when test="@type='integer'">
                
                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:text>EBML_SINTEGER_CLASS, </xsl:text>
                <xsl:choose>
                    <xsl:when test="@default and (number(@default)=number(@default))"><xsl:text>1, (intptr_t)</xsl:text><xsl:value-of select="@default" /><xsl:text>, </xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>NULL, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
                
            </xsl:when>
            <xsl:when test="@type='utf-8'">
                
                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:text>EBML_UNISTRING_CLASS, </xsl:text>
                <xsl:choose>
                    <xsl:when test="@default"><xsl:text>1, (intptr_t)"</xsl:text><xsl:value-of select="@default" /><xsl:text>", </xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>NULL, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
                
            </xsl:when>
            <xsl:when test="@type='string'">
                
                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:text>EBML_STRING_CLASS, </xsl:text>
                <xsl:choose>
                    <xsl:when test="@default"><xsl:text>1, (intptr_t)"</xsl:text><xsl:value-of select="@default" /><xsl:text>", </xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>NULL, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
                
            </xsl:when>
            <xsl:when test="@type='float'">
                
                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:text>EBML_FLOAT_CLASS, </xsl:text>
                <xsl:choose>
                    <xsl:when test="@default and starts-with(@default,'0x')">
                        <xsl:text>1, (intptr_t)</xsl:text>
                        <xsl:choose>
                            <xsl:when test="@default='0x1p+0'">1.0, </xsl:when>
                            <xsl:when test="@default='0x0p+0'">0.0, </xsl:when>
                            <xsl:when test="@default='0x1.f4p+12'">8000.0, </xsl:when>
                            <xsl:otherwise><xsl:text>, </xsl:text><xsl:value-of select="@default" /></xsl:otherwise>>
                        </xsl:choose>
                    </xsl:when>
                    <xsl:otherwise><xsl:text>0, 0, </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>NULL, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
                
            </xsl:when>
            <xsl:when test="@type='date'">
                <xsl:text>const ebml_context MATROSKA_Context</xsl:text>
                <xsl:value-of select="@name" />
                <xsl:text> = {</xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:text>EBML_DATE_CLASS, </xsl:text>
                <xsl:text>0, </xsl:text>
                <xsl:text>0, </xsl:text>
                <xsl:text>"</xsl:text><xsl:value-of select="@name" /><xsl:text>", </xsl:text>
                <xsl:text>NULL, </xsl:text>
                <xsl:text>EBML_SemanticGlobals, </xsl:text>
                <xsl:text>NULL</xsl:text>
                <xsl:text>};&#10;</xsl:text>
            </xsl:when>
        </xsl:choose>
    </xsl:copy>
    </xsl:if>
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

  <xsl:template name="list-master-children">
    <xsl:param name="findPath"/>
    <xsl:for-each select="../element">
        <xsl:variable name="plainPath"><xsl:value-of select="translate(substring-before(substring-after(@path,'('),')'), '(1*(', '')" /></xsl:variable>
        <xsl:if test="$plainPath=concat(concat($findPath,'\'),@name) or ($plainPath=$findPath and contains(@path,'(1*(\'))">
            <xsl:variable name="EBMLElementOccurrence" select="substring-before(@path,'(')"/>
            <xsl:variable name="EBMLMinOccurrence"     select="substring-before($EBMLElementOccurrence,'*')"/>
            <xsl:variable name="EBMLMaxOccurrence"     select="substring-after($EBMLElementOccurrence,'*')"/>
            <xsl:text>    {</xsl:text>
            <xsl:choose>
                <xsl:when test="$plainPath=$findPath and contains(@path,'(1*(\')"><xsl:text>0</xsl:text></xsl:when>
                <xsl:when test="$EBMLMinOccurrence!='' and $EBMLMinOccurrence!='0'"><xsl:text>1</xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>, </xsl:text>
            <xsl:choose>
                <xsl:when test="$EBMLMaxOccurrence='1'"><xsl:text>1</xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>0</xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:text>, &amp;MATROSKA_Context</xsl:text>
            <xsl:value-of select="@name" />
            <xsl:text>, </xsl:text>
            <xsl:if test="(not(@minver) or @minver &lt; 2) and (not(@webm) or @webm='1')"><xsl:text>0</xsl:text></xsl:if>
            <xsl:if test="(@maxver and @maxver &lt; 1) or (@minver &gt; 1)">PROFILE_MATROSKA_V1</xsl:if>
            <xsl:if test="(@maxver and @maxver &lt; 2) or (@minver &gt; 2)">
                <xsl:if test="(@maxver and @maxver &lt; 1) or (@minver &gt; 1)"><xsl:text>|</xsl:text></xsl:if>
                <xsl:text>PROFILE_MATROSKA_V2</xsl:text>
            </xsl:if>
            <xsl:if test="(@maxver and @maxver &lt; 3) or (@minver &gt; 3)">
                <xsl:if test="(@maxver and @maxver &lt; 2) or (@minver &gt; 2)"><xsl:text>|</xsl:text></xsl:if>
                <xsl:text>PROFILE_MATROSKA_V3</xsl:text>
            </xsl:if>
            <xsl:if test="(@maxver and @maxver &lt; 4) or (@minver &gt; 4)">
                <xsl:if test="(@maxver and @maxver &lt; 3) or (@minver &gt; 3)"><xsl:text>|</xsl:text></xsl:if>
                <xsl:text>PROFILE_MATROSKA_V4</xsl:text>
            </xsl:if>
            <xsl:if test="@divx='0' or ((not(@divx) or @divx!='1') and ((@maxver and @maxver &lt; 2) or (@minver &gt; 1)))">
                <xsl:if test="(@maxver) or (@minver &gt; 1)"><xsl:text>|</xsl:text></xsl:if>
                <xsl:text>PROFILE_DIVX</xsl:text>
            </xsl:if>
            <xsl:if test="@webm='0'">
                <xsl:if test="not(@minver) or @minver &gt; 1 or @maxver or @divx='0'">|</xsl:if>
                <xsl:text>PROFILE_WEBM</xsl:text>
            </xsl:if>
            <xsl:text>},</xsl:text>
            <xsl:if test="$plainPath=$findPath and contains(@path,'(1*(\')"><xsl:text> // recursive</xsl:text></xsl:if>
            <xsl:text>&#10;</xsl:text>
        </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
