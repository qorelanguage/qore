libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
TIBRV_SOURCES = single-compilation-unit.cc
else
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
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libtibrv.la
libtibrv_la_SOURCES = $(TIBRV_SOURCES)
libtibrv_la_LDFLAGS = ${TIBRV_LDFLAGS} $(TIBRVCPP_LDFLAGS) -static
libtibrv_la_CPPFLAGS = $(TIBRV_CPPFLAGS)
else
pkglib_LTLIBRARIES = tibrv.la
tibrv_la_SOURCES = $(TIBRV_SOURCES)
tibrv_la_LDFLAGS = -module -avoid-version $(TIBRV_LDFLAGS) $(TIBRVCPP_LDFLAGS)
tibrv_la_CPPFLAGS = $(TIBRV_CPPFLAGS)
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
	mv $(pkglibdir)/tibrv.$(MODULE_SUFFIX) $(pkglibdir)/tibrv.qmod
	rm -f $(pkglibdir)/tibrv.la $(pkglibdir)/tibrv.a
endif

