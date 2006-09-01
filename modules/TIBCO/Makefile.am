libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

TIBRV_SOURCES = tibrv.cc \
	QoreTibrvTransport.cc \
	QoreTibrvSender.cc \
	QoreTibrvListener.cc \
	QoreTibrvFtMember.cc \
	QoreTibrvFtMonitor.cc \
	QoreTibrvCmTransport.cc \
	QoreTibrvCmSender.cc \
	QoreTibrvCmListener.cc \
	QoreTibrvDistributedQueue.cc \
	QC_TibrvSender.cc \
	QC_TibrvListener.cc \
	QC_TibrvFtMember.cc \
	QC_TibrvFtMonitor.cc \
	QC_TibrvCmSender.cc \
	QC_TibrvCmListener.cc \
	QC_TibrvDistributedQueue.cc \
	tibrv-module.cc
TIBAE_SOURCES = QoreApp.cc tibae.cc tibae-module.cc 

if COND_MONOLITHIC
noinst_LTLIBRARIES = libtibrv.la
libtibrv_la_SOURCES = $(TIBRV_SOURCES)
libtibrv_la_LDFLAGS = ${TIBRV_LIBS} -static
libtibrv_la_CPPFLAGS = $(TIBRV_CPPFLAGS)

if COND_TIBAE
noinst_LTLIBRARIES += libtibae.la 
libtibae_la_SOURCES = $(TIBAE_SOURCES)
libtibae_la_LDFLAGS = $(TIBAE_LIBS) ${TIBRV_LIBS} -static
libtibae_la_CPPFLAGS = $(TIBAE_CPPFLAGS) $(TIBRV_CPPFLAGS)
endif

else
pkglib_LTLIBRARIES = tibrv.la
tibrv_la_SOURCES = $(TIBRV_SOURCES)
tibrv_la_LDFLAGS = -module -avoid-version $(TIBRV_LIBS)
tibrv_la_CPPFLAGS = $(TIBRV_CPPFLAGS)

if COND_TIBAE
pkglib_LTLIBRARIES += tibae.la
tibae_la_SOURCES = $(TIBAE_SOURCES)
tibae_la_LDFLAGS = -module -avoid-version $(TIBAE_LIBS) ${TIBRV_LIBS}
tibae_la_CPPFLAGS = $(TIBAE_CPPFLAGS) $(TIBRV_CPPFLAGS)
endif

endif

AM_CPPFLAGS = ${PTHREAD_CFLAGS}
INCLUDES = -I$(top_srcdir)/include
AM_CXXFLAGS =

if COND_DEBUG
AM_CXXFLAGS += -g
else
if COND_PROFILE
AM_CXXFLAGS += -g -pg
endif
endif

if !COND_MONOLITHIC
install-exec-hook:
	mv $(pkglibdir)/tibrv.$(MODULE_SUFFIX) $(pkglibdir)/tibrv.qmod
	rm -f $(pkglibdir)/tibrv.la $(pkglibdir)/tibrv.a
if COND_TIBAE
	mv $(pkglibdir)/tibae.$(MODULE_SUFFIX) $(pkglibdir)/tibae.qmod
	rm -f $(pkglibdir)/tibae.la $(pkglibdir)/tibae.a
endif

endif

