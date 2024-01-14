<?xml version="1.0"?>
<!--
    File used to generate libmatroska KaxSemantic.cpp from ebml_matroska.xml
    Usage: xsltproc -o KaxSemantic.cpp schema_2_kaxsemantic_cpp.xsl ebml_matroska.xml
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
**  DO NOT EDIT, GENERATED WITH schema_2_kaxsemantic_cpp.xsl
**  https://github.com/Matroska-Org/foundation-source/tree/master/spectool
**********************************************************************/

#include "matroska/KaxContexts.h"
#include "matroska/KaxSemantic.h"
#include "matroska/KaxSegment.h"
#include "matroska/KaxSeekHead.h"
#include "matroska/KaxCluster.h"
#include "matroska/KaxTracks.h"
#include "matroska/KaxCues.h"
#include "matroska/KaxBlockData.h"
#include "matroska/KaxCuesData.h"

namespace libmatroska {

static constexpr const MatroskaProfile VERSION_ALL_MATROSKA = {true, true, 0, MatroskaProfile::ANY_VERSION};
static constexpr const MatroskaProfile VERSION_DEPRECATED   = {false, false, MatroskaProfile::ANY_VERSION, 0};
static constexpr const MatroskaProfile VERSION_DIVX_ONLY    = {false, true, MatroskaProfile::ANY_VERSION, 0};
<xsl:for-each select="ebml:element[not(starts-with(@path,'\EBML\'))]">
    <!-- sorting messes the detection of the previous element MATROSKA_VERSION state -->
    <!-- Maybe for each output we create we also create a counterpart call to check if the new MATROSKA_VERSION state that should be used -->
    <!-- <xsl:sort select="translate(@path, '\+', '\')" /> -->
    <xsl:apply-templates select="."/>
</xsl:for-each>

<xsl:for-each select="ebml:element[not(starts-with(@path,'\EBML\'))]">
    <!-- sorting messes the detection of the previous element MATROSKA_VERSION state -->
    <!-- Maybe for each output we create we also create a counterpart call to check if the new MATROSKA_VERSION state that should be used -->
    <!-- <xsl:sort select="translate(@path, '\+', '\')" /> -->
    <xsl:call-template name="output-blocked-render">
        <xsl:with-param name="node" select="."/>
    </xsl:call-template>
</xsl:for-each>

<!-- <xsl:apply-templates select="ebml:element">
    <xsl:sort select="translate(@path, '\+', '\')" />
</xsl:apply-templates> -->
} // namespace libmatroska
</xsl:template>
  <xsl:template match="ebml:element">
    <xsl:variable name="plainPath">
        <xsl:value-of select="translate(@path, '\+', '\')" />
    </xsl:variable>
    <xsl:variable name="minVer">
        <xsl:call-template name="get-min-ver">
            <xsl:with-param name="node" select="."/>
        </xsl:call-template>
    </xsl:variable>
        <!-- <xsl:if test="$minVer &gt; 1 or ebml:extension[@divx='1']">
            <xsl:text>#if MATROSKA_VERSION >= 2&#10;</xsl:text>
        </xsl:if> -->
        <xsl:choose>
            <xsl:when test="@type='master'">
                <xsl:text>&#10;DEFINE_START_SEMANTIC(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>)&#10;</xsl:text>
                <xsl:variable name="masterMinVer">
                    <xsl:value-of select="$minVer" />
                </xsl:variable>
<!-- <xsl:value-of select="$masterMinVer" /><xsl:text>&#10;</xsl:text> -->

                <xsl:if test="@recursive=1">
                    <xsl:call-template name="outputContextTableItem">
                        <xsl:with-param name="node" select="."/>
                        <xsl:with-param name="inMinver" select="$masterMinVer"/>
                        <xsl:with-param name="asRecursive" select="1"/>
                    </xsl:call-template>
                </xsl:if>
                <xsl:for-each select="/ebml:EBMLSchema/ebml:element[translate(@path, '\+', '\') = concat(concat($plainPath, '\'), @name)]">
                    <xsl:call-template name="outputContextTableItem">
                        <xsl:with-param name="node" select="."/>
                        <xsl:with-param name="inMinver" select="$masterMinVer"/>
                    </xsl:call-template>
                </xsl:for-each>
                <xsl:text>DEFINE_END_SEMANTIC(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>)&#10;&#10;</xsl:text>

                <xsl:text>DEFINE_MKX_MASTER</xsl:text>
                <xsl:if test="not(contains(substring($plainPath,2),'\'))"><xsl:text>_ORPHAN</xsl:text></xsl:if>
                <!-- Needs a special constructor -->
                <xsl:if test="@name='Attachments' or @name='AttachedFile' or @name='Cluster' or @name='BlockGroup' or @name='TrackEntry'"><xsl:text>_CONS</xsl:text></xsl:if>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:choose>
                    <xsl:when test="not(contains(substring($plainPath,2),'\'))" />
                    <xsl:otherwise>
                        <xsl:text>, Kax</xsl:text>
                        <xsl:call-template name="output-master-parent">
                            <xsl:with-param name="node" select="."/>
                        </xsl:call-template>
                    </xsl:otherwise>
                </xsl:choose>
                <xsl:choose>
                    <xsl:when test="@unknownsizeallowed=1"><xsl:text>, true</xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>, false</xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>)&#10;</xsl:text>

            </xsl:when>
            <xsl:when test="@type='binary'">

                <xsl:text>DEFINE_MKX_BINARY</xsl:text>
                <xsl:choose>
                    <!-- Needs a special constructor -->
                    <xsl:when test="@name='Block' or @name='SimpleBlock' or @name='BlockVirtual'"><xsl:text>_CONS</xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text> </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:text>, Kax</xsl:text>
                <xsl:call-template name="output-master-parent">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>)&#10;</xsl:text>

            </xsl:when>
            <xsl:when test="@type='uinteger'">

                <xsl:text>DEFINE_MKX_UINTEGER</xsl:text>
                <xsl:if test="@default and (number(@default)=number(@default))"><xsl:text>_DEF</xsl:text></xsl:if>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:text>, Kax</xsl:text>
                <xsl:call-template name="output-master-parent">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:if test="@default and (number(@default)=number(@default))"><xsl:text>, </xsl:text><xsl:value-of select="@default" /></xsl:if>
                <xsl:text>)&#10;</xsl:text>

            </xsl:when>
            <xsl:when test="@type='integer'">

                <xsl:text>DEFINE_MKX_SINTEGER</xsl:text>
                <xsl:if test="@default and (number(@default)=number(@default))"><xsl:text>_DEF</xsl:text></xsl:if>
                <!-- Needs a special constructor -->
                <xsl:if test="@name='ReferenceBlock'"><xsl:text>_CONS</xsl:text></xsl:if>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:text>, Kax</xsl:text>
                <xsl:call-template name="output-master-parent">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:if test="@default and (number(@default)=number(@default))"><xsl:text>, </xsl:text><xsl:value-of select="@default" /></xsl:if>
                <xsl:text>)&#10;</xsl:text>

            </xsl:when>
            <xsl:when test="@type='utf-8'">

                <xsl:text>DEFINE_MKX_UNISTRING</xsl:text>
                <xsl:if test="@default"><xsl:text>_DEF</xsl:text></xsl:if>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:text>, Kax</xsl:text>
                <xsl:call-template name="output-master-parent">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:if test="@default"><xsl:text>, "</xsl:text><xsl:value-of select="@default" />"</xsl:if>
                <xsl:text>)&#10;</xsl:text>

            </xsl:when>
            <xsl:when test="@type='string'">

                <xsl:text>DEFINE_MKX_STRING</xsl:text>
                <xsl:if test="@default"><xsl:text>_DEF</xsl:text></xsl:if>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:text>, Kax</xsl:text>
                <xsl:call-template name="output-master-parent">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:if test="@default"><xsl:text>, "</xsl:text><xsl:value-of select="@default" /><xsl:text>"</xsl:text></xsl:if>
                <xsl:text>)&#10;</xsl:text>

            </xsl:when>
            <xsl:when test="@type='float'">

                <xsl:text>DEFINE_MKX_FLOAT</xsl:text>
                <xsl:if test="@default and starts-with(@default,'0x')"><xsl:text>_DEF</xsl:text></xsl:if>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:text>, Kax</xsl:text>
                <xsl:call-template name="output-master-parent">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:if test="@default and starts-with(@default,'0x')">
                    <xsl:choose>
                        <xsl:when test="@default='0x1p+0'">, 1</xsl:when>
                        <xsl:when test="@default='0x0p+0'">, 0</xsl:when>
                        <xsl:when test="@default='0x1.f4p+12'">, 8000</xsl:when>
                        <xsl:otherwise><xsl:text>, </xsl:text><xsl:value-of select="@default" /></xsl:otherwise>>
                    </xsl:choose>
                </xsl:if>
                <xsl:text>)&#10;</xsl:text>

            </xsl:when>
            <xsl:when test="@type='date'">
                <xsl:text>DEFINE_MKX_DATE</xsl:text>
                <xsl:choose>
                    <xsl:when test="@default and (number(@default)=number(@default))"><xsl:text>_DEF</xsl:text></xsl:when>
                    <xsl:otherwise><xsl:text>    </xsl:text></xsl:otherwise>
                </xsl:choose>
                <xsl:text>(Kax</xsl:text>
                <xsl:call-template name="get-class-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:text>, </xsl:text>
                <xsl:value-of select="@id" /><xsl:text>, </xsl:text>
                <xsl:value-of select="((string-length(@id) - 2) * 0.5)" />
                <xsl:text>, Kax</xsl:text>
                <xsl:call-template name="output-master-parent">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-display-name">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:call-template name="output-minmax">
                    <xsl:with-param name="node" select="."/>
                </xsl:call-template>
                <xsl:if test="@default and (number(@default)=number(@default))"><xsl:text>, </xsl:text><xsl:value-of select="@default" /></xsl:if>
                <xsl:text>)&#10;</xsl:text>
            </xsl:when>
        </xsl:choose>


        <!-- <xsl:if test="$minVer &gt; 1 or ebml:extension[@divx='1']">
            <xsl:text>#endif&#10;</xsl:text>
        </xsl:if> -->
    <!-- </xsl:copy> -->
    <!-- </xsl:if> -->
  </xsl:template>

  <xsl:template name="outputContextTableItem">
    <xsl:param name="node"/>
    <xsl:param name="asRecursive"/>
    <xsl:param name="inMinver"/>

    <xsl:variable name="minVer">
        <xsl:call-template name="get-min-ver">
            <xsl:with-param name="node" select="$node"/>
        </xsl:call-template>
    </xsl:variable>

    <!-- <xsl:if test="$minVer &gt; 1 or ebml:extension[@divx='1']">
        <xsl:text>#if MATROSKA_VERSION >= 2&#10;</xsl:text>
    </xsl:if> -->
    <xsl:text>DEFINE_SEMANTIC_ITEM(</xsl:text>
    <xsl:choose>
        <xsl:when test="$asRecursive=1"><xsl:text>false</xsl:text></xsl:when>
        <xsl:when test="$node/@minOccurs!='' and $node/@minOccurs!='0'"><xsl:text>true</xsl:text></xsl:when>
        <xsl:otherwise><xsl:text>false</xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:text>, </xsl:text>
    <xsl:choose>
        <xsl:when test="$node/@recurring='1'"><xsl:text>false</xsl:text></xsl:when>
        <xsl:when test="$node/@maxOccurs='1'"><xsl:text>true</xsl:text></xsl:when>
        <xsl:otherwise><xsl:text>false</xsl:text></xsl:otherwise>
    </xsl:choose>
    <xsl:text>, Kax</xsl:text>
    <xsl:call-template name="get-class-name">
        <xsl:with-param name="node" select="."/>
    </xsl:call-template>
    <xsl:text>)</xsl:text>
    <xsl:if test="$asRecursive=1"><xsl:text> // recursive</xsl:text></xsl:if>
    <xsl:text>&#10;</xsl:text>
    <!-- <xsl:if test="$minVer &gt; 1 or ebml:extension[@divx='1']">
        <xsl:text>#endif // MATROSKA_VERSION&#10;</xsl:text>
    </xsl:if> -->
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

  <xsl:template name="output-blocked-render">
    <xsl:param name="node"/>

    <xsl:if test="$node/@maxver='0' or $node/@maxver='1' or $node/@maxver='2' or $node/@maxver='3'">
        <xsl:text>&#10;libebml::filepos_t Kax</xsl:text>
        <xsl:call-template name="get-class-name">
            <xsl:with-param name="node" select="."/>
        </xsl:call-template>
        <xsl:text>::RenderData(libebml::IOCallback &amp; /* output */, bool /* bForceRender */, ShouldWrite /* writeFilter */) {&#10;</xsl:text>
        <xsl:text>  assert(false); // no you are not allowed to use this element !&#10;</xsl:text>
        <xsl:text>  return 0;&#10;</xsl:text>
        <xsl:text>}&#10;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="output-master-parent">
    <xsl:param name="node"/>

    <xsl:variable name="findName">
        <xsl:choose>
            <xsl:when test="$node/@recursive=1"><xsl:text>+</xsl:text><xsl:value-of select="$node/@name"/></xsl:when>
            <xsl:otherwise><xsl:value-of select="$node/@name"/></xsl:otherwise>
        </xsl:choose>
    </xsl:variable>

    <xsl:for-each select="/ebml:EBMLSchema/ebml:element[$node/@path = concat(concat(@path, '\'), $findName)]">
        <xsl:call-template name="get-class-name">
            <xsl:with-param name="node" select="."/>
        </xsl:call-template>
    </xsl:for-each>
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

  <xsl:template name="get-class-name">
    <xsl:param name="node"/>

    <xsl:choose>
        <xsl:when test="$node/ebml:extension[@cppname]">
            <xsl:choose>
                <xsl:when test="$node/ebml:extension[@cppname][1]/@cppname='TimecodeScale'"><xsl:value-of select="$node/@name" /></xsl:when>
                <xsl:when test="$node/ebml:extension[@cppname][1]/@cppname='ReferenceTimeCode'"><xsl:value-of select="$node/@name" /></xsl:when>
                <xsl:when test="$node/ebml:extension[@cppname][1]/@cppname='TrackTimecodeScale'"><xsl:value-of select="$node/@name" /></xsl:when>
                <xsl:when test="$node/ebml:extension[@cppname][1]/@cppname='ClusterTimecode'">ClusterTimestamp</xsl:when>
                <xsl:otherwise><xsl:value-of select="$node/ebml:extension[@cppname][1]/@cppname" /></xsl:otherwise>
            </xsl:choose>
        </xsl:when>
        <xsl:otherwise><xsl:value-of select="$node/@name" /></xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="output-minmax">
    <xsl:param name="node"/>

    <xsl:text>, </xsl:text>
    <xsl:choose>
        <xsl:when test="(not($node/@minver) or $node/@minver='0') and not($node/@maxver) and ebml:extension[@webm='1']"><xsl:text>VERSION_ALL_MATROSKA</xsl:text></xsl:when>
        <xsl:when test="$node/@maxver='0' and not(ebml:extension[@divx='1'])"><xsl:text>VERSION_DEPRECATED</xsl:text></xsl:when>
        <xsl:when test="$node/@maxver='0' and ebml:extension[@divx='1']"><xsl:text>VERSION_DIVX_ONLY</xsl:text></xsl:when>
        <xsl:otherwise>
            <xsl:text>MatroskaProfile(</xsl:text>
            <xsl:choose>
                <xsl:when test="not(ebml:extension[@webm='1'])"><xsl:text>false, </xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>true, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
                <xsl:when test="not(ebml:extension[@divx='1'])"><xsl:text>false, </xsl:text></xsl:when>
                <xsl:otherwise><xsl:text>true, </xsl:text></xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
                <xsl:when test="$node/@maxver='0'">MatroskaProfile::ANY_VERSION</xsl:when>
                <xsl:when test="not($node/@minver)">0</xsl:when>
                <xsl:otherwise><xsl:value-of select="$node/@minver" /></xsl:otherwise>
            </xsl:choose>
            <xsl:choose>
                <xsl:when test="not($node/@maxver)"></xsl:when>
                <xsl:otherwise><xsl:text>, </xsl:text><xsl:value-of select="$node/@maxver" /></xsl:otherwise>
            </xsl:choose>
            <xsl:text>)</xsl:text>
        </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="output-display-name">
    <xsl:param name="node"/>

    <xsl:text>, "</xsl:text>
    <xsl:choose>
        <!-- Don't use the cppname for these -->
        <xsl:when test="$node/@name='AttachedFile'"><xsl:value-of select="$node/@name" /></xsl:when>
        <xsl:when test="$node/@name='TagLanguage'"><xsl:value-of select="$node/@name" /></xsl:when>
        <xsl:when test="$node/@name='TimestampScale'"><xsl:value-of select="$node/@name" /></xsl:when>
        <xsl:when test="$node/@name='ReferenceTimestamp'"><xsl:value-of select="$node/@name" /></xsl:when>
        <xsl:when test="$node/@name='TrackTimestampScale'"><xsl:value-of select="$node/@name" /></xsl:when>
        <!-- Custom legacy debug names -->
        <xsl:when test="$node/@name='FileMediaType'"><xsl:text>FileMimeType</xsl:text></xsl:when>
        <xsl:when test="$node/@name='SeekHead'"><xsl:text>SeekHeader</xsl:text></xsl:when>
        <xsl:when test="$node/@name='Seek'"><xsl:text>SeekPoint</xsl:text></xsl:when>
        <xsl:when test="$node/@name='ReferencePriority'"><xsl:text>FlagReferenced</xsl:text></xsl:when>
        <!-- Use the cppname as debug name -->
        <xsl:when test="$node/ebml:extension[@cppname]">
            <xsl:choose>
                <xsl:when test="$node/ebml:extension[@cppname][1]/@cppname='ClusterTimecode'">ClusterTimestamp</xsl:when>
                <xsl:otherwise><xsl:value-of select="$node/ebml:extension[@cppname][1]/@cppname" /></xsl:otherwise>
            </xsl:choose>
        </xsl:when>
        <xsl:otherwise><xsl:value-of select="$node/@name" /></xsl:otherwise>
    </xsl:choose>
    <xsl:text>"</xsl:text>
  </xsl:template>

  <xsl:template match="@* | node()">
    <xsl:copy>
      <xsl:apply-templates select="@* | node()"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
