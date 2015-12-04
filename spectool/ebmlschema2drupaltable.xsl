<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0" extension-element-prefixes="xsi">
  <xsl:output encoding="utf-8" method="xml" version="1.0" indent="yes" />
  <xsl:template match="table">
    <table>
      <tbody>
        <xsl:apply-templates select="element"/>
        <xsl:call-template name="AddColumnHeader"/>
      </tbody>
    </table>
  </xsl:template>
  <xsl:template match="element">
    <xsl:if test="@id='0x1A45DFA3' or @id='0xEC'   or @id='0x18538067' or @id='0x114D9B74' or @id='0x1549A966' or @id='0x1F43B675' or @id='0x1654AE6B' or @id='0x1C53BB6B' or @id='0x1941A469' or @id='0x1043A770' or @id='0x1254C367'">
      <xsl:call-template name="AddColumnHeader"/>
    </xsl:if>
    <!-- add sectional headers -->
    <xsl:choose>
      <xsl:when test="@id='0x1A45DFA3'">
        <tr>
          <th colspan="14" id="LevelEBML">EBML Header</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0xEC'">
        <tr>
          <th colspan="14">Global elements (used everywhere in the format)</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x18538067'">
        <tr>
          <th colspan="14" id="LevelSegment">Segment</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x114D9B74'">
        <tr>
          <th colspan="14" id="MetaSeekInformation">Meta Seek Information</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x1549A966'">
        <tr>
          <th colspan="14" id="SegmentInformation">Segment Information</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x1F43B675'">
        <tr>
          <th colspan="14" id="LevelCluster">Cluster</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x1654AE6B'">
        <tr>
          <th colspan="14" id="LevelTrack">Track</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x1C53BB6B'">
        <tr>
          <th colspan="14" id="CueingData">Cueing Data</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x1941A469'">
        <tr>
          <th colspan="14" id="Attachment">Attachment</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x1043A770'">
        <tr>
          <th colspan="14" id="Chapters">Chapters</th>
        </tr>
      </xsl:when>
      <xsl:when test="@id='0x1254C367'">
        <tr>
          <th colspan="14" id="Tagging">Tagging</th>
        </tr>
      </xsl:when>
    </xsl:choose>
    <tr>
      <xsl:attribute name="id">
        <xsl:value-of select="@name"/>
      </xsl:attribute>
      <xsl:attribute name="class">
        <xsl:choose>
          <xsl:when test="not(@minver) or @minver='0' or @maxver">
            <xsl:text>version2</xsl:text>
          </xsl:when>
          <xsl:when test="@level='-1'">
            <xsl:text>level1</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="concat('level', @level)"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
      <td><xsl:value-of select="@name"/></td>
      <td>
        <xsl:choose>
          <xsl:when test="@level='-1'">
            <xsl:text>g</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="@level"/>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:if test="@recursive='1'">
          <xsl:text>+</xsl:text>
        </xsl:if>
      </td>
      <td>
        <xsl:if test="@level=0">
          <xsl:attribute name="style">
            <xsl:text>white-space: nowrap</xsl:text>
          </xsl:attribute>
        </xsl:if>
        <xsl:if test="string-length(@id)>=4">
          <xsl:text>[</xsl:text><xsl:value-of select="substring(@id, 3, 2)"/><xsl:text>]</xsl:text>
        </xsl:if>
        <xsl:if test="string-length(@id)>=6">
          <xsl:text>[</xsl:text><xsl:value-of select="substring(@id, 5, 2)"/><xsl:text>]</xsl:text>
        </xsl:if>
        <xsl:if test="string-length(@id)>=8">
          <xsl:text>[</xsl:text><xsl:value-of select="substring(@id, 7, 2)"/><xsl:text>]</xsl:text>
        </xsl:if>
        <xsl:if test="string-length(@id)>=10">
          <xsl:text>[</xsl:text><xsl:value-of select="substring(@id, 9, 2)"/><xsl:text>]</xsl:text>
        </xsl:if>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@mandatory='1'">
            <xsl:text>mand.</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>unset</xsl:text>
            </xsl:attribute>
            <xsl:text>-</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@multiple='1'">
            <xsl:text>mult.</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>unset</xsl:text>
            </xsl:attribute>
            <xsl:text>-</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@range">
            <xsl:value-of select="@range"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>unset</xsl:text>
            </xsl:attribute>
            <xsl:text>-</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="@default">
            <xsl:value-of select="@default"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>unset</xsl:text>
            </xsl:attribute>
            <xsl:text>-</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <abbr>
          <xsl:choose>
            <xsl:when test="@type='master'">
              <xsl:attribute name="title"><xsl:text>Master Elements</xsl:text></xsl:attribute>
              <xsl:text>m</xsl:text>
            </xsl:when>
            <xsl:when test="@type='integer'">
              <xsl:attribute name="title"><xsl:text>Signed Integer</xsl:text></xsl:attribute>
              <xsl:text>i</xsl:text>
            </xsl:when>
            <xsl:when test="@type='uinteger'">
              <xsl:attribute name="title"><xsl:text>Unsigned Integer</xsl:text></xsl:attribute>
              <xsl:text>u</xsl:text>
            </xsl:when>
            <xsl:when test="@type='date'">
              <xsl:attribute name="title"><xsl:text>Date</xsl:text></xsl:attribute>
              <xsl:text>d</xsl:text>
            </xsl:when>
            <xsl:when test="@type='float'">
              <xsl:attribute name="title"><xsl:text>Float</xsl:text></xsl:attribute>
              <xsl:text>f</xsl:text>
            </xsl:when>
            <xsl:when test="@type='string'">
              <xsl:attribute name="title"><xsl:text>String</xsl:text></xsl:attribute>
              <xsl:text>s</xsl:text>
            </xsl:when>
            <xsl:when test="@type='utf-8'">
              <xsl:attribute name="title"><xsl:text>UTF-8</xsl:text></xsl:attribute>
              <xsl:text>8</xsl:text>
            </xsl:when>
            <xsl:when test="@type='binary'">
              <xsl:attribute name="title"><xsl:text>Binary</xsl:text></xsl:attribute>
              <xsl:text>b</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:attribute name="class">
                <xsl:text>unset</xsl:text>
              </xsl:attribute>
              <xsl:text>ERROR</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </abbr>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="not(@minver) or @minver='0'">
            <xsl:text></xsl:text>
          </xsl:when>
          <xsl:when test="1>=@minver">
            <xsl:text>*</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>flagnot</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="not(@minver) or @minver='0'">
            <xsl:text></xsl:text>
          </xsl:when>
          <xsl:when test="2>=@minver">
            <xsl:text>*</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>flagnot</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="not(@minver) or @minver='0'">
            <xsl:text></xsl:text>
          </xsl:when>
          <xsl:when test="3>=@minver">
            <xsl:text>*</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>flagnot</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="not(@minver) or @minver='0'">
            <xsl:text></xsl:text>
          </xsl:when>
          <xsl:when test="@maxver">
            <xsl:attribute name="class">
              <xsl:text>flagnot</xsl:text>
            </xsl:attribute>
          </xsl:when>
          <xsl:when test="4>=@minver">
            <xsl:text>*</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="class">
              <xsl:text>flagnot</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:choose>
          <xsl:when test="not(@minver) and not(@maxver)">
            <xsl:text></xsl:text>
          </xsl:when>
          <xsl:when test="@minver='0' or @maxver='0'">
            <xsl:text></xsl:text>
          </xsl:when>
          <xsl:when test="@webm='0' or @maxver">
            <xsl:attribute name="class">
              <xsl:text>flagnot</xsl:text>
            </xsl:attribute>
          </xsl:when>
          <xsl:when test="@webm='1'">
            <xsl:text>*</xsl:text>
          </xsl:when>
          <xsl:when test="@minver='4'">
            <xsl:attribute name="class">
              <xsl:text>flagnot</xsl:text>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>*</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </td>
      <td>
        <xsl:copy-of select="./node()"/>
      </td>
    </tr>
  </xsl:template>
  <xsl:template name="AddColumnHeader">
    <tr class="toptitle">
      <th style="white-space: nowrap">Element Name</th>
      <th title="Level"><abbr title="Level">L</abbr> </th>
      <th style="white-space: nowrap">EBML ID</th>
      <th title="Mandatory"><abbr title="Mandatory">Ma</abbr> </th>
      <th title="Multiple"><abbr title="Multiple">Mu</abbr> </th>
      <th title="Range"><abbr title="Range">Rng</abbr> </th>
      <th>Default</th>
      <th title="Element Type"><abbr title="Element Type">T</abbr> </th>
      <th title="Version 1"><abbr title="Version 1">1</abbr> </th>
      <th title="Version 2"><abbr title="Version 2">2</abbr> </th>
      <th title="Version 3"><abbr title="Version 3">3</abbr> </th>
      <th title="Version 4"><abbr title="Version 4">4</abbr> </th>
      <th title="WebM"><abbr title="WebM">W</abbr> </th>
      <th>Description</th>
    </tr>
  </xsl:template>
</xsl:stylesheet>
