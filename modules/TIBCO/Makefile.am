pkglibdir = $(libdir)/@PACKAGE@-@VERSION@
#
TIBCO_SOURCES = QoreApp.cc tibco.cc tibco-module.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES = libtibco.la
libtibco_la_SOURCES = $(TIBCO_SOURCES)
libtibco_la_LDFLAGS = ${TIBCO_LIBS} -static
else
pkglib_LTLIBRARIES = tibco.la
tibco_la_SOURCES = $(TIBCO_SOURCES)
tibco_la_LDFLAGS = -module -avoid-version ${TIBCO_LIBS}
endif

AM_CXXFLAGS = ${TIBCO_CFLAGS} ${PTHREAD_CFLAGS}
INCLUDES = -I$(top_srcdir)/include ${TIBCO_INCLUDES}

if COND_DEBUG
AM_CXXFLAGS += -g
else
if COND_PROFILE
AM_CXXFLAGS += -g -pg
endif
endif

if !COND_MONOLITHIC
install-exec-hook:
	mv $(pkglibdir)/tibco.$(MODULE_SUFFIX) $(pkglibdir)/tibco.qmod
	rm -f $(pkglibdir)/tibco.la $(pkglibdir)/tibco.a
endif

