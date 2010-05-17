#!/bin/sh

LOC="/usr/share/xml/docbook/stylesheet/nwalsh5/current/xhtml /opt/local/share/xsl/docbook-xsl/xhtml /usr/share/sgml/docbook/xsl-stylesheets/xhtml"

for a in $LOC; do
    if [ -d $a ]; then
	f=$a;
	break;
    fi
done

#if not found in the default locations, try to find it using locate
if [ -z "$f" ]; then
    f=`locate docbook.xsl|grep xhtml|grep /docbook.xsl|sort|tail -1|sed "s/\/docbook.xsl$//`
fi

echo $f
