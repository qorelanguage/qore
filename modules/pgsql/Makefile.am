libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
PGSQL_SOURCES = single-compilation-unit.cc
else
PGSQL_SOURCES = pgsql.cc QorePGConnection.cc QorePGMapper.cc
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libpgsql.la
libpgsql_la_SOURCES = $(PGSQL_SOURCES)
libpgsql_la_LDFLAGS = ${PGSQL_LIBS} -static
else
pkglib_LTLIBRARIES = pgsql.la
pgsql_la_SOURCES = $(PGSQL_SOURCES)
pgsql_la_LDFLAGS = -module -avoid-version ${PGSQL_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include $(PGSQL_INCLUDES)

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
	mv $(pkglibdir)/pgsql.$(MODULE_SUFFIX) $(pkglibdir)/pgsql.qmod
	rm -f $(pkglibdir)/pgsql.la $(pkglibdir)/pgsql.a
endif

