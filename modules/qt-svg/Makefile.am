libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
QT_SVG_SOURCES = single-compilation-unit.cc
else
QT_SVG_SOURCES = qt-svg.cc \
        QC_QSvgRenderer.cc \
        QC_QSvgGenerator.cc \
        QC_QSvgWidget.cc
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libqt-svg.la
libqt_svg_la_SOURCES = $(QT_SVG_SOURCES)
libqt_svg_la_LDFLAGS = ${QT_CORE_LIBS} ${QT_GUI_LIBS} ${QT_SVG_LIBS} -static
else
pkglib_LTLIBRARIES = qt-svg.la
qt_svg_la_SOURCES = $(QT_SVG_SOURCES)
qt_svg_la_LDFLAGS = -module -avoid-version ${QT_SVG_LIBS} ${QT_CORE_LIBS} ${QT_GUI_LIBS}
qt_svg_la_LIBADD = ../qt-core/libqore-qt-core.la ../qt-gui/libqore-qt-gui.la
endif

INCLUDES = -I$(top_srcdir)/include $(QT_CORE_INCLUDES) $(QT_GUI_INCLUDES) $(QT_SVG_INCLUDES) -I ../qt-core -I ../qt-gui

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
	mv $(pkglibdir)/qt-svg.$(MODULE_SUFFIX) $(pkglibdir)/qt-svg.qmod
	rm -f $(pkglibdir)/qt-svg.la $(pkglibdir)/qt-svg.a
endif
