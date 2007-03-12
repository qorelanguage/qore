libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

SYBASE_SOURCES = sybase.cc sybase_connection.cc sybase_low_level_interface.cc sybase_executor.cc sybase_query_parser.cc sybase_read_output.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES = libsybase.la
libsybase_la_SOURCES = $(SYBASE_SOURCES)
libsybase_la_LDFLAGS = ${SYBASE_LIBS} -static
else
automoddir = $(pkglibdir)/auto
pkglib_LTLIBRARIES = sybase.la
sybase_la_SOURCES = $(SYBASE_SOURCES)
sybase_la_LDFLAGS = -module -avoid-version ${SYBASE_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include $(SYBASE_INCLUDES)

AM_CXXFLAGS = ${PTHREAD_CFLAGS}

if COND_PROFILE
AM_CXXFLAGS += -g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
endif
endif

if !COND_MONOLITHIC
install-exec-hook:
	if ! test -d $(automoddir); then mkdir $(automoddir); fi
	mv $(pkglibdir)/sybase.$(MODULE_SUFFIX) $(automoddir)/sybase.qmod
	rm -f $(pkglibdir)/sybase.la $(pkglibdir)/sybase.a
endif
