libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

NCURSES_SOURCES = ncurses.cc QC_Window.cc QC_Panel.cc

if COND_MONOLITHIC
noinst_LTLIBRARIES = libncurses.la
libncurses_la_SOURCES = $(NCURSES_SOURCES)
libncurses_la_LDFLAGS = -static ${NCURSES_LIBS}
else
pkglib_LTLIBRARIES = ncurses.la
ncurses_la_SOURCES = $(NCURSES_SOURCES)
ncurses_la_LDFLAGS = -module -avoid-version ${NCURSES_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include

AM_CPPFLAGS= ${NCURSES_INCLUDES}
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
	mv $(pkglibdir)/ncurses.$(MODULE_SUFFIX) $(pkglibdir)/ncurses.qmod
	rm -f $(pkglibdir)/ncurses.la $(pkglibdir)/ncurses.a
endif

