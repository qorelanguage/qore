libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

ORACLE_SOURCES = oracle.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES = liboracle.la
liboracle_la_SOURCES = $(ORACLE_SOURCES)
liboracle_la_LDFLAGS = ${ORACLE_LIBS} -static
else
automoddir = $(pkglibdir)/auto
pkglib_LTLIBRARIES = oracle.la
oracle_la_SOURCES = $(ORACLE_SOURCES)
oracle_la_LDFLAGS = -module -avoid-version ${ORACLE_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include $(ORACLE_INCLUDES)

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
	mv $(pkglibdir)/oracle.$(MODULE_SUFFIX) $(automoddir)/oracle.qmod
	rm -f $(pkglibdir)/oracle.la $(pkglibdir)/oracle.a
endif
