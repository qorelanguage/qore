#all:	qore.html
#
#qore.html:	docbook/*docbook
#	docbook2html -u docbook/qore.docbook
#
#clean:
#	rm -fv qore.html
#
#distclean:	clean
	

DOCBOOK_ROOT := $(shell ./get-docbook-root.sh)

# "modern" way how to handle docbook files
# PATHS /usr/share/xml/docbook/stylesheet/nwalsh5/current/xhtml # opensuse                               
#       /opt/local/share/xsl/docbook-xsl/xhtml                  # macpors                                
#       /usr/share/sgml/docbook/xsl-stylesheets/xhtml           # fedora  
all: monolithic chunked

mono: monolithic

monolithic: docbook/*docbook
	xsltproc --stringparam html.stylesheet qore-style.css \
	         --stringparam use.id.as.filename 1 \
	         --stringparam generate.toc "book toc,title" \
	         --stringparam toc.max.depth 2 \
	         --output qore.html \
		 ${DOCBOOK_ROOT}/docbook.xsl \
	         docbook/qore.docbook 

chunked: docbook/*docbook
	xsltproc --stringparam html.stylesheet qore-style.css \
	         --stringparam use.id.as.filename 1 \
	         --stringparam generate.toc "book toc,title" \
	         --stringparam toc.max.depth 2 \
	         --stringparam base.dir "qore-html/" \
		 ${DOCBOOK_ROOT}/chunk.xsl \
	         docbook/qore.docbook

qthelp: qore-html/*html
	./docbook2qthelp > ./qore-html/qore_language_reference.qhp
	cp qore-style.css qore-html/qore-style.css
	qhelpgenerator qore-html/qore_language_reference.qhp -o library/qore_language_reference.qch

qtregister:
	assistant -register library/qore_language_reference.qch
	assistant -register library/qore_c_library_reference.qch

qtunregister:
	assistant -unregister library/qore_c_library_reference.qch
	assistant -unregister library/qore_language_reference.qch

clean:
	rm -fv qore.html
	rm -rfv qore-html

distclean: clean
