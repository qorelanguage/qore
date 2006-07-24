libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

MYSQL_SOURCES = mysql.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES = libmysql.la
libmysql_la_SOURCES = $(MYSQL_SOURCES)
libmysql_la_LDFLAGS = ${MYSQL_LIBS} -static
else
automoddir = $(pkglibdir)/auto
pkglib_LTLIBRARIES = mysql.la
mysql_la_SOURCES = $(MYSQL_SOURCES)
mysql_la_LDFLAGS = -module -avoid-version ${MYSQL_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include $(MYSQL_INCLUDES)

AM_CXXFLAGS = ${PTHREAD_CFLAGS}

if COND_DEBUG
AM_CXXFLAGS += -g
else
if COND_PROFILE
AM_CXXFLAGS += -g -pg
endif
endif

if !COND_MONOLITHIC
install-exec-hook:
	if ! test -d $(automoddir); then mkdir $(automoddir); fi
	mv $(pkglibdir)/mysql.$(MODULE_SUFFIX) $(automoddir)/mysql.qmod
	rm -f $(pkglibdir)/mysql.la $(pkglibdir)/mysql.a
endif

