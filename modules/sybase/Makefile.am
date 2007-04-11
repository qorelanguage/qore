libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

SYBASE_SOURCES = sybase.cc connection.cc direct_execute.cc transactions.cc conversions.cc command.cc initiate_language_command.cc send_command.cc encoding_helpers.cc query_processing.cc initiate_rpc_command.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES=
if COND_SYBASE
noinst_LTLIBRARIES += libsybase.la
libsybase_la_SOURCES = $(SYBASE_SOURCES)
libsybase_la_LDFLAGS = ${SYBASE_LIBS} -static
libsybase_la_CPPFLAGS = $(SYBASE_INCLUDES) -DSYBASE=1
endif
if COND_FREETDS
noinst_LTLIBRARIES += libmssql.la
libmssql_la_SOURCES = $(SYBASE_SOURCES)
libmssql_la_LDFLAGS = ${FREETDS_LIBS} -static
libmssql_la_CPPFLAGS = $(FREETDS_INCLUDES) -DFREETDS=1
endif
else
pkglib_LTLIBRARIES=
if COND_SYBASE
pkglib_LTLIBRARIES += sybase.la
sybase_la_SOURCES = $(SYBASE_SOURCES)
sybase_la_LDFLAGS = -module -avoid-version ${SYBASE_LIBS}
sybase_la_CPPFLAGS = $(SYBASE_INCLUDES) -DSYBASE=1
endif
if COND_FREETDS
pkglib_LTLIBRARIES += mssql.la
mssql_la_SOURCES = $(SYBASE_SOURCES)
mssql_la_LDFLAGS = -module -avoid-version ${FREETDS_LIBS}
mssql_la_CPPFLAGS = $(FREETDS_INCLUDES) -DFREETDS=1
endif
endif

INCLUDES = -I$(top_srcdir)/include 

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
if COND_SYBASE
	mv $(pkglibdir)/sybase.$(MODULE_SUFFIX) $(pkglibdir)/sybase.qmod
	rm -f $(pkglibdir)/sybase.la $(pkglibdir)/sybase.a
endif
if COND_FREETDS
	mv $(pkglibdir)/mssql.$(MODULE_SUFFIX) $(pkglibdir)/mssql.qmod
	rm -f $(pkglibdir)/mssql.la $(pkglibdir)/mssql.a
endif
endif
