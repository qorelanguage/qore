all:	qore.html

qore.html:	docbook/*docbook
	docbook2html -u docbook/qore.docbook

clean:
	rm -fv qore.html

distclean:	clean
	

# TODO/FIXME: use this "modern way" in the future...
#xsltproc --stringparam html.stylesheet qore-style.css \
#         --stringparam use.id.as.filename 1 \
#         --stringparam generate.toc "book toc,title" \
#         --output qore.html \
#         /usr/share/xml/docbook/stylesheet/nwalsh5/current/xhtml/docbook.xsl \
#         docbook/qore.docbook

