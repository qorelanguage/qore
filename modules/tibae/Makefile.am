libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

TIBAE_SOURCES = QoreApp.cc tibae.cc tibae-module.cc 

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
AM_CXXFLAGS = ${PTHREAD_CFLAGS} ${OPENSSL_CPPFLAGS}

if COND_PROFILE
AM_CXXFLAGS += -g -pg
else
if COND_DEBUG
AM_CXXFLAGS += -g
endif
endif

if !COND_MONOLITHIC
install-exec-hook:
	mv $(pkglibdir)/tibae.$(MODULE_SUFFIX) $(pkglibdir)/tibae.qmod
	rm -f $(pkglibdir)/tibae.la $(pkglibdir)/tibae.a
endif

