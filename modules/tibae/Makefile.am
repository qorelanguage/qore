libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
TIBAE_SOURCES = single-compilation-unit.cc
else
TIBAE_SOURCES = QoreApp.cc QC_TibcoAdapter.cc TibCommandLine.cc tibae.cc tibae-module.cc 
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libtibae.la 
libtibae_la_SOURCES = $(TIBAE_SOURCES)
libtibae_la_LDFLAGS = $(TIBAE_LDFLAGS) ${TIBRV_LDFLAGS} -static
libtibae_la_CPPFLAGS = $(TIBAE_CPPFLAGS) $(TIBRV_CPPFLAGS)
else
pkglib_LTLIBRARIES = tibae.la
tibae_la_SOURCES = $(TIBAE_SOURCES)
tibae_la_LDFLAGS = -module -avoid-version $(TIBAE_LDFLAGS) ${TIBRV_LDFLAGS}
tibae_la_CPPFLAGS = $(TIBAE_CPPFLAGS) $(TIBRV_CPPFLAGS)
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

BUILT_SOURCES = dummy

dummy:
	$(top_srcdir)/next_build.sh

if !COND_MONOLITHIC
install-exec-hook:
	mv $(pkglibdir)/tibae.$(MODULE_SUFFIX) $(pkglibdir)/tibae.qmod
	rm -f $(pkglibdir)/tibae.la $(pkglibdir)/tibae.a
endif

