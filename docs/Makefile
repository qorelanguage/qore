all:	qore.html

qore.html:	docbook/*docbook
	docbook2html -u docbook/qore.docbook

clean:
	rm -fv qore.html

distclean:	clean
	

# "modern" way how to handle docbook files
# opensuse
#/usr/share/xml/docbook/stylesheet/nwalsh5/current/xhtml/docbook.xsl \
# macports
experimental:
	xsltproc --stringparam html.stylesheet qore-style-exp.css \
	         --stringparam use.id.as.filename 1 \
	         --stringparam generate.toc "book toc,title" \
	         --output qore.html \
	         /opt/local/share/xsl/docbook-xsl/xhtml/chunk.xsl \
	         docbook/qore.docbook

