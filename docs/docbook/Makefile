
SOURCES=bookinfo.docbook class_lib.docbook comparison.docbook future.docbook keywords.docbook modules.docbook qore.docbook bugs.docbook command_line.docbook function_lib.docbook introduction.docbook language.docbook parse_directives.docbook driver.xsl tuxedo_module.docbook tibrv_module.docbook tibae_module.docbook ncurses_module.docbook oracle_module.docbook mysql_module.docbook pgsql_module.docbook sybase_module.docbook

all: qore.html

qore.html: $(SOURCES)
	#docbook2html -u qore.docbook
	xsltproc -o $@ driver.xsl qore.docbook

