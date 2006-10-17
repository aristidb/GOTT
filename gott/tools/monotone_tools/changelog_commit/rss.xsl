<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

<xsl:output method="html"/>

<xsl:variable name="host" select="'http://snalp.codefreakz.de'"/>

<xsl:template match="rss">
  <html>
    <head>
      <link type="text/css" rel="stylesheet" href="synopsis.css" />
      <xsl:apply-templates select="channel" mode="head"/>
    </head>
    <body>
      <div class="content">
        <xsl:apply-templates select="channel" mode="body"/>
      </div>
    </body>
  </html>
</xsl:template>

<xsl:template match="channel" mode="head">
  <title>
    <xsl:value-of select="title"/>
  </title>
</xsl:template>

<xsl:template match="channel" mode="body">
  <h1>
    <xsl:apply-templates select="title"/>
  </h1>
  <p class="description">
    <xsl:apply-templates select="description"/>
  </p>
  <xsl:apply-templates select="item"/>
</xsl:template>

<xsl:template match="item">
  <xsl:variable name="href">
    <xsl:choose>
      <xsl:when test="starts-with(link, $host)">
	<xsl:value-of select="substring-after(link, $host)"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="link"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <p class="logentry">
    <a class="title" href="{$href}"><xsl:apply-templates select="title"/> </a>
    <span class="author"> by <xsl:value-of select="author"/></span>
    <span class="date"> on <xsl:value-of select="pubDate"/></span>
  </p>
  <pre class="message"><xsl:apply-templates select="description"/></pre>
</xsl:template>

</xsl:stylesheet>
