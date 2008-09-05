
SOURCES=bookinfo.docbook class_lib.docbook comparison.docbook future.docbook keywords.docbook modules.docbook qore.docbook bugs.docbook command_line.docbook function_lib.docbook introduction.docbook language.docbook parse_directives.docbook driver.xsl tibrv_module.docbook ncurses_module.docbook oracle_module.docbook mysql_module.docbook pgsql_module.docbook sybase_module.docbook mssql_module.docbook opengl_module.docbook glut_module.docbook qt-core_module.docbook qt-gui_module.docbook qt-opengl_module.docbook qt-svg_module.docbook

all: qore.html

qore.html: $(SOURCES)
	#docbook2html -u qore.docbook
	xsltproc -o $@ driver.xsl qore.docbook

