libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

TUXEDO_SOURCES =  tuxedo.cc \
  QC_TuxedoConnection.cc \
  QoreTuxedoConnection.cc \
  connection_parameters_helper.cc \
  tpalloc_helper.cc \
  QC_TuxedoAdapter.cc \
  QoreTuxedoAdapter.cc \
  handle_error.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES = libtuxedo.la
libtuxedo_la_SOURCES = $(TUXEDO_SOURCES)
libtuxedo_la_LDFLAGS = ${TUXEDO_LIBS} -static
else
automoddir = $(pkglibdir)/auto
pkglib_LTLIBRARIES = tuxedo.la
tuxedo_la_SOURCES = $(TUXEDO_SOURCES)
tuxedo_la_LDFLAGS = -module -avoid-version ${TUXEDO_LIBS} 
endif

INCLUDES = -I$(top_srcdir)/include $(TUXEDO_INCLUDES) 

AM_CXXFLAGS = $(TUXEDO_CPPFLAGS) ${PTHREAD_CFLAGS}

if COND_PROFILE
AM_CXXFLAGS += -g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
endif
endif

if !COND_MONOLITHIC
install-exec-hook:
	mv $(pkglibdir)/tuxedo.$(MODULE_SUFFIX) $(pkglibdir)/tuxedo.qmod
	rm -f $(pkglibdir)/tuxedo.la $(pkglibdir)/tuxedo.a
endif

