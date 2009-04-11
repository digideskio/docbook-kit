<?xml version='1.0'?>
<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:l="http://docbook.sourceforge.net/xmlns/l10n/1.0"
    version="1.0">

  <!-- Include other stylesheets -->
  <xsl:import href="../../../docbook/xsl/fo/docbook.xsl"/>
  <xsl:include href="../common/params.xsl" />
  <xsl:include href="../common/inline.xsl" />
  <xsl:include href="inline.xsl" />

  <!-- Images -->
  <xsl:param name="draft.watermark.image" select="''"/>

  <!-- Enable bookmarks -->
  <xsl:param name="fop1.extensions" select="1"/>

  <!-- Do not print out URLs -->
  <xsl:param name="ulink.show" select="0"/>
  <!--<xsl:param name="show.comments" select="0"/>-->

  <!-- Page setup -->
  <xsl:param name="paper.type" select="'A4'"/>
  <!--<xsl:param name="double.sided"/>-->

  <!--<xsl:param name="header.column.widths">0 1 0</xsl:param>-->
  <!--<xsl:attribute-set name="header.content.properties">
    <xsl:attribute name="font-family">Helvetica</xsl:attribute>
    <xsl:attribute name="font-size">12pt</xsl:attribute>
    <xsl:attribute name="font-weight">bold</xsl:attribute>
  </xsl:attribute-set>-->

  <!--<xsl:attribute-set name="footer.content.properties">
    <xsl:attribute name="font-family">Helvetica</xsl:attribute>
    <xsl:attribute name="font-size">8pt</xsl:attribute>
  </xsl:attribute-set>
  <xsl:param name="footer.rule" select="0"/>-->

  <!--<xsl:param name="body.start.indent" select="'10pt'"/>
  <xsl:param name="page.margin.inner" select="'3cm'"/>
  <xsl:param name="page.margin.outer" select="'3cm'"/>-->

  <!-- Fonts & colors -->
  <xsl:param name="variablelist.as.blocks" select="0"/>

  <xsl:param name="body.font.master" select="11"/>

  <xsl:attribute-set name="monospace.verbatim.properties">
    <xsl:attribute name="font-size">.8em</xsl:attribute>
    <xsl:attribute name="padding">5pt</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="nongraphical.admonition.properties">
    <xsl:attribute name="background-color">#F0F0F0</xsl:attribute>
    <xsl:attribute name="padding">5pt</xsl:attribute>
  </xsl:attribute-set>

  <!--<xsl:param name="shade.verbatim" select="1"/>
  <xsl:attribute-set name="shade.verbatim.style">
    <xsl:attribute name="border">0</xsl:attribute>
    <xsl:attribute name="background-color">#F0F0F0</xsl:attribute>
  </xsl:attribute-set> -->

  <xsl:attribute-set name="section.title.chapter.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 1.5"/>
      <xsl:text>pt</xsl:text>
    </xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="section.title.level1.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 1.4"/>
      <xsl:text>pt</xsl:text>
    </xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="section.title.level2.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 1.3"/>
      <xsl:text>pt</xsl:text>
    </xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="toc.line.properties">
    <xsl:attribute name="font-size">10pt</xsl:attribute>
    <xsl:attribute name="font-weight">
      <xsl:choose>
        <xsl:when test="self::chapter | self::preface | self::appendix">bold</xsl:when>
        <xsl:otherwise>normal</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:attribute-set>

  <xsl:param name="generate.toc">
    book      toc,title
  </xsl:param>

</xsl:stylesheet> 

