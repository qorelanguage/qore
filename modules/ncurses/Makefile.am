libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/$(mypkglibdir)

if COND_SINGLE_COMPILATION_UNIT
NCURSES_SOURCES = single-compilation-unit.cc
else
NCURSES_SOURCES = ncurses.cc QC_Window.cc QC_Panel.cc
endif

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

AM_CPPFLAGS= ${NCURSES_CPPFLAGS}
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
	mv $(pkglibdir)/ncurses.$(MODULE_SUFFIX) $(pkglibdir)/ncurses.qmod
	rm -f $(pkglibdir)/ncurses.la $(pkglibdir)/ncurses.a
endif

