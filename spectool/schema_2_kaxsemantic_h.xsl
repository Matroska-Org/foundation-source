<?xml version="1.0"?>
<!--
    File used to generate libmatroska KaxSemantic.h from ebml_matroska.xml
    Usage: xsltproc -o KaxSemantic.h schema_2_kaxsemantic_h.xsl ebml_matroska.xml
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:str="http://exslt.org/strings"
    exclude-result-prefixes="str xhtml ebml"
    xmlns:xhtml="http://www.w3.org/1999/xhtml"
    xmlns="urn:ietf:rfc:8794" xmlns:ebml="urn:ietf:rfc:8794">
  <xsl:output encoding="UTF-8" method="text" version="1.0" indent="yes" />
  <xsl:template match="ebml:EBMLSchema">// Copyright © 2002-2020 Matroska (non-profit organisation).
// SPDX-License-Identifier: LGPL-2.1-or-later

/**********************************************************************
**  DO NOT EDIT, GENERATED WITH schema_2_kaxsemantic_h.xsl
**  https://github.com/Matroska-Org/foundation-source/tree/master/spectool
**********************************************************************/


#ifndef LIBMATROSKA_SEMANTIC_H
#define LIBMATROSKA_SEMANTIC_H

#include "matroska/KaxTypes.h"
#include &lt;ebml/EbmlUInteger.h&gt;
#include &lt;ebml/EbmlSInteger.h&gt;
#include &lt;ebml/EbmlDate.h&gt;
#include &lt;ebml/EbmlFloat.h&gt;
#include &lt;ebml/EbmlString.h&gt;
#include &lt;ebml/EbmlUnicodeString.h&gt;
#include &lt;ebml/EbmlBinary.h&gt;
#include &lt;ebml/EbmlMaster.h&gt;
#include "matroska/KaxDefines.h"

using namespace libebml;

namespace libmatroska {
<xsl:for-each select="ebml:element[not(starts-with(@path,'\EBML\'))]">
    <!-- <xsl:sort select="translate(@path, '\+', '\')" /> -->
    <xsl:apply-templates select="."/>
</xsl:for-each>

<xsl:for-each select="ebml:element">
    <xsl:sort select="not(@name='TrackType')"/>
    <xsl:sort select="not(@name='ContentCompAlgo')"/>

    <!-- Output the enums after the IDs -->
    <xsl:call-template name="outputAllEnums"/>
</xsl:for-each>
} // namespace libmatroska

#endif // LIBMATROSKA_SEMANTIC_H
</xsl:template>
  <xsl:template match="ebml:element">
    <!-- Ignore EBML extra constraints -->
    <xsl:if test="@name!='Segment' and @name!='Cluster' and @name!='BlockGroup' and @name!='Block' and @name!='BlockVirtual' and @name!='ReferenceBlock' and @name!='SimpleBlock' and @name!='Cues' and @name!='CuePoint' and @name!='CueTrackPositions' and @name!='CueReference' and @name!='SeekHead' and @name!='Seek' and @name!='TrackEntry'">
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
        <xsl:if test="@maxver='0' or @maxver='1' or @maxver='2' or @maxver='3' or @length">
            <xsl:text>public:&#10;</xsl:text>
        </xsl:if>
        <xsl:if test="@maxver='0' or @maxver='1' or @maxver='2' or @maxver='3'">
            <xsl:text>  filepos_t RenderData(IOCallback &amp; output, bool bForceRender, ShouldWrite writeFilter) override;&#10;</xsl:text>
        </xsl:if>
        <xsl:if test="@length">
            <xsl:text>  bool ValidateSize() const override {return IsFiniteSize() &amp;&amp; GetSize() </xsl:text>
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


    <!-- Enum output -->
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
        <xsl:if test="ebml:restriction/ebml:enum and @type!='string'">
            <xsl:variable name="prefix">
                <xsl:choose>
                    <xsl:when test="@name='ContentCompAlgo'">TRACK_ENCODING_COMP</xsl:when>
                    <xsl:when test="@name='TrackType'">TRACK_TYPE</xsl:when>
                    <!-- <xsl:when test="@name='ProjectionType'">VIDEO_PROJECTION_TYPE</xsl:when>
                    <xsl:when test="@name='FlagInterlaced'">VIDEO_INTERLACE_FLAG</xsl:when> -->
                    <xsl:when test="@name='StereoMode'">VIDEO_STEREO</xsl:when>
                    <xsl:when test="@name='DisplayUnit'">DISPLAY_UNIT</xsl:when>
                    <xsl:when test="@name='TransferCharacteristics'">TRANSFER</xsl:when>
                    <xsl:when test="@name='TargetTypeValue'">TARGET_TYPE</xsl:when>
                    <!-- <xsl:when test="contains(@path,'\TrackEntry\Video\Colour\')"><xsl:text>COLOUR_</xsl:text><xsl:value-of select="@name"/></xsl:when> -->
                    <xsl:when test="contains(@path,'\TrackEntry\Video\')"><xsl:text>VIDEO_</xsl:text><xsl:value-of select="@name"/></xsl:when>
                    <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise>
                </xsl:choose>
            </xsl:variable>
            <xsl:variable name="align" as="xs:integer">
                <!-- padding added in structures to have fancy alignment -->
                17
                <!-- <xsl:choose>
                    <xsl:when test="@name='DUMMY'">12</xsl:when> -->
                    <!-- <xsl:when test="@name='ContentCompAlgo'">12</xsl:when> -->
                    <!-- <xsl:when test="@name='TrackType'">9</xsl:when> -->
                    <!-- <xsl:when test="@name='ProjectionType'">19</xsl:when> -->
                    <!-- <xsl:when test="@name='FlagInterlaced'">13</xsl:when> -->
                    <!-- <xsl:when test="@name='StereoMode'">19</xsl:when> -->
                    <!-- <xsl:when test="@name='DisplayUnit'">12</xsl:when> -->
                    <!-- <xsl:when test="@name='FieldOrder'">13</xsl:when> -->
                    <!-- <xsl:otherwise>17</xsl:otherwise>
                </xsl:choose> -->
            </xsl:variable>

            <xsl:text>/**&#10; *</xsl:text>
            <xsl:value-of select="ebml:documentation[@purpose='definition']"/>
            <xsl:text>&#10; */&#10;</xsl:text>
            <xsl:text>typedef enum {&#10;</xsl:text>

            <!-- Internal value not found in the specs -->
            <!-- <xsl:if test="@name='TrackType'"><xsl:text>  MATROSKA_TRACK_TYPE_NONE     = 0x0,&#10;</xsl:text></xsl:if> -->
            <xsl:if test="@name='ContentCompAlgo'">
              <xsl:text>  MATROSKA_</xsl:text><xsl:value-of select="translate($prefix, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/><xsl:text>_NONE</xsl:text>
              <xsl:value-of select="substring('            ',0,$align)"/>
              <xsl:text> = -1,&#10;</xsl:text>
            </xsl:if>

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
                <xsl:choose>
                    <xsl:when test="ebml:documentation[@purpose='definition']">
                        <xsl:text>, // </xsl:text>
                        <xsl:call-template name="cleanEnumDoc">
                            <xsl:with-param name="label" select="ebml:documentation[@purpose='definition']"/>
                        </xsl:call-template>
                        <xsl:text>&#10;</xsl:text>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:text>,&#10;</xsl:text>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:for-each>

            <!-- Extra enum count -->
            <!-- <xsl:choose> -->
                <!-- <xsl:when test="@name='ContentCompAlgo'"><xsl:text>  MATROSKA_</xsl:text><xsl:value-of select="translate($prefix, 'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/><xsl:text>_NONE&#10;</xsl:text></xsl:when> -->
                <!-- <xsl:when test="@name='ChromaSitingHorz'"><xsl:text>  MATROSKA_COLOUR_CHROMASITINGHORZ_NB&#10;</xsl:text></xsl:when> -->
                <!-- <xsl:when test="@name='ChromaSitingVert'"><xsl:text>  MATROSKA_COLOUR_CHROMASITINGVERT_NB&#10;</xsl:text></xsl:when> -->
                <!-- <xsl:when test="@name='StereoMode'"><xsl:text>  MATROSKA_VIDEO_STEREOMODE_TYPE_NB,&#10;</xsl:text></xsl:when> -->
            <!-- </xsl:choose> -->

            <xsl:text>} Matroska</xsl:text>
            <xsl:choose>
                <xsl:when test="@name='ContentCompAlgo'"><xsl:text>TrackEncodingCompAlgo</xsl:text></xsl:when>
                <xsl:when test="@name='ChromaSitingHorz'"><xsl:text>ColourChromaSitingHorz</xsl:text></xsl:when>
                <xsl:when test="@name='ChromaSitingVert'"><xsl:text>ColourChromaSitingVert</xsl:text></xsl:when>
                <xsl:when test="@name='ChapProcessTime'"><xsl:text>ChapterProcessTime</xsl:text></xsl:when>
                <xsl:when test="@name='ContentSigAlgo'"><xsl:text>ContentSignatureAlgo</xsl:text></xsl:when>
                <xsl:when test="@name='ContentEncAlgo'"><xsl:text>ContentEncodingAlgo</xsl:text></xsl:when>
                <!-- <xsl:when test="@name='FlagInterlaced'"><xsl:text>VideoInterlaceFlag</xsl:text></xsl:when> -->
                <!-- <xsl:when test="@name='StereoMode'"><xsl:text>VideoStereoModeType</xsl:text></xsl:when> -->
                <xsl:when test="contains(@path,'\TrackEntry\Video\')"><xsl:text>Video</xsl:text><xsl:value-of select="@name"/></xsl:when>
                <xsl:when test="contains(@path,'\TrackEntry\Audio\')"><xsl:text>Audio</xsl:text><xsl:value-of select="@name"/></xsl:when>
                <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise>
            </xsl:choose>
            <xsl:text>;&#10;&#10;</xsl:text>
        </xsl:if>
    </xsl:template>

  <xsl:template name="outputEnumLabel">
    <xsl:param name="label"/>
    <xsl:param name="align" as="xs:integer"/>
    <!-- Turn the ebml_matroska.xml enum labels into the names used in libmatroska2 -->
    <!-- Recursive calls until we end up with a matching name with no space, parenthesis, comas, etc -->
    <xsl:choose>
        <xsl:when test="$label='Header Stripping'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'HeaderStrip'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='display aspect ratio'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'DisplayAspectRatio'"/></xsl:call-template>
        </xsl:when>
        <!-- Field Order -->
        <xsl:when test="$label='tff'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'TopFieldFirst'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='bff'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BottomFieldFirst'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='tff(swapped)'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'TopFieldSwapped'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="$label='bff(swapped)'">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BottomFieldSwapped'"/></xsl:call-template>
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
        <!-- <xsl:when test="contains($label,'checkboard ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/>
                <xsl:with-param name="label" select="concat(concat(substring-before($label, 'checkboard '), 'checkerboard '), substring-after($label, 'checkboard '))"/>
            </xsl:call-template>
        </xsl:when> -->
        <xsl:when test="contains($label,'laced in one ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label, 'laced in one '), substring-after($label, 'laced in one '))"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'The ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(substring-before($label, 'The '), substring-after($label, 'The '))"/></xsl:call-template>
        </xsl:when>

        <!-- chapter process time -->
        <xsl:when test="contains($label,'during the ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'during'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'before starting ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'before'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'after playback ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'after'"/></xsl:call-template>
        </xsl:when>

        <xsl:when test="contains($label,'SMPTE ST ')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat('SMPTE ' ,concat(substring-before($label, 'SMPTE ST '), substring-after($label, 'SMPTE ST ')))"/></xsl:call-template>
        </xsl:when>

        <!-- Transfer -->
        <xsl:when test="contains($label,'Gamma 2.2 curve')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'Gamma22'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'Gamma 2.8 curve')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'Gamma28'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.709')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT709'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.2020 10 bit')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT2020 10-bit'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.2020 12 bit')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT2020 12-bit'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.2020')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT2020'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.1361')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT1361 ECG'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'Perceptual Quantization')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT2100 PQ'"/></xsl:call-template>
        </xsl:when>

        <!-- Primaries -->
        <xsl:when test="contains($label,'EBU Tech. 3213-E')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'JEDEC_P22'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.470BG')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT470BG'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.470M')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT470M'"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'ITU-R BT.601 525')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="'BT601 525'"/></xsl:call-template>
        </xsl:when>

        <!-- MatrixCoefficients -->
        <xsl:when test="contains($label,' Non-constant Luminance')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(concat(substring-before($label, ' Non-constant Luminance'), substring-after($label, ' Non-constant Luminance')),' NCL')"/></xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' Constant Luminance')">
            <xsl:call-template name="outputEnumLabel"><xsl:with-param name="align" select="$align"/><xsl:with-param name="label" select="concat(concat(substring-before($label, ' Constant Luminance'), substring-after($label, ' Constant Luminance')),' CL')"/></xsl:call-template>
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

  <xsl:template name="cleanEnumDoc">
    <xsl:param name="label"/>
    <xsl:choose>
        <xsl:when test="contains($label,' [@!')">
            <xsl:call-template name="cleanEnumDoc">
                <xsl:with-param name="label" select="concat(substring-before($label, ' [@!'), ' (', substring-before(substring-after($label, ' [@!'), ']'), ')', substring-after(substring-after($label, ' [@!'), ']'))"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,' [@?')">
            <xsl:call-template name="cleanEnumDoc">
                <xsl:with-param name="label" select="concat(substring-before($label, ' [@?'), ' (', substring-before(substring-after($label, ' [@?'), ']'), ')', substring-after(substring-after($label, ' [@?'), ']'))"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:when test="contains($label,'; see usage notes')">
            <xsl:call-template name="cleanEnumDoc">
                <xsl:with-param name="label" select="concat(substring-before($label, '; see usage notes'), substring-after($label, '; see usage notes'))"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="translate($label, '&#10;', ' ')"/>
        </xsl:otherwise>
    </xsl:choose>
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
