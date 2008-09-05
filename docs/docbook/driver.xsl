<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
  <!--
  <xsl:import href="/usr/share/sgml/docbook/xsl-stylesheets-1.69.1-5.1/html/docbook.xsl"/>
  <xsl:import href="/usr/share/sgml/docbook/xsl-stylesheets-1.72.0-1.fc6/html/docbook.xsl"/>
  <xsl:import href="/usr/share/sgml/docbook/xsl-stylesheets-1.72.0-2.fc7/html/docbook.xsl"/>
  <xsl:import href="/usr/share/sgml/docbook/xsl-stylesheets-1.73.2/html/docbook.xsl"/>
  <xsl:import href="/opt/local/share/xsl/docbook-xsl/html/docbook.xsl"/>
  -->
  <xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/1.73.2/html/docbook.xsl"/>

  <xsl:param name="chapter.autolabel">1</xsl:param>
  <xsl:param name="section.autolabel">1</xsl:param>
  <xsl:param name="section.label.includes.component.label">1</xsl:param>

  <xsl:param name="generate.toc">
    appendix  toc,title
    article/appendix  nop
    article   toc,title
    book      toc,title,figure,example,equation
    chapter   title
    part      toc,title
    preface   toc,title
    qandadiv  toc
    qandaset  toc
    reference toc,title
    sect1     toc
    sect2     toc
    sect3     toc
    sect4     toc
    sect5     toc
    section   toc
    set       toc,title
  </xsl:param>

  <xsl:param name="toc.max.depth">2</xsl:param>
  <xsl:param name="html.stylesheet">qore-style.css</xsl:param>

</xsl:stylesheet>
