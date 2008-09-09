
SOURCES=bookinfo.docbook \
	class_lib.docbook \
	comparison.docbook \
	future.docbook \
	keywords.docbook \
	qore.docbook \
	bugs.docbook \
	command_line.docbook \
	function_lib.docbook \
	introduction.docbook \
	language.docbook \
	parse_directives.docbook \
	driver.xsl

all: qore.html

qore.html: $(SOURCES)
	#docbook2html -u qore.docbook
	xsltproc -o $@ driver.xsl qore.docbook

