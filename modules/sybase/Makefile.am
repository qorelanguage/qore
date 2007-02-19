libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

SYBASE_SOURCES = sybase.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES = libsybase.la
libsybase_la_SOURCES = $(SYBASE_SOURCES)
libsybase_la_LDFLAGS = ${SYBASE_LIBS} -static
else
automoddir = $(pkglibdir)/auto
pkglib_LTLIBRARIES = sybase.la
sybase_la_SOURCES = $(SYBASE_SOURCES)
sybase_la_LDFLAGS = -module -avoid-version ${SYBASE_LIBS}  -L/opt/sybase/OCS-15_0/lib -lsybcs_r -lsybintl_r -lsybblk_r -lsybcomn_r -lsybct_r
endif

INCLUDES = -I$(top_srcdir)/include $(SYBASE_INCLUDES) -I/opt/sybase/OCS-15_0/include

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
