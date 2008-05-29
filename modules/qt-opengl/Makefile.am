libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
QT_OPENGL_SOURCES = single-compilation-unit.cc
else
QT_OPENGL_SOURCES = qt-opengl.cc \
	QC_QGLWidget.cc \
	QC_QGLFormat.cc \
	QC_QGLContext.cc \
	QC_QGLColormap.cc \
	QC_QGLFramebufferObject.cc \
	QC_QGLPixelBuffer.cc
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libqt-opengl.la
libqt_opengl_la_SOURCES = $(QT_OPENGL_SOURCES)
libqt_opengl_la_LDFLAGS = ${QT_CORE_LIBS} ${QT_GUI_LIBS} ${QT_OPENGL_LIBS} -static
else
pkglib_LTLIBRARIES = qt-opengl.la
qt_opengl_la_SOURCES = $(QT_OPENGL_SOURCES)
qt_opengl_la_LDFLAGS = -module -avoid-version ${QT_OPENGL_LIBS} ${QT_CORE_LIBS} ${QT_GUI_LIBS}
qt_opengl_la_LIBADD = ../qt-core/libqore-qt-core.la ../qt-gui/libqore-qt-gui.la
endif

INCLUDES = -I$(top_srcdir)/include $(QT_CORE_INCLUDES) $(QT_GUI_INCLUDES) $(QT_OPENGL_INCLUDES) -I ../qt-core -I ../qt-gui

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
	mv $(pkglibdir)/qt-opengl.$(MODULE_SUFFIX) $(pkglibdir)/qt-opengl.qmod
	rm -f $(pkglibdir)/qt-opengl.la $(pkglibdir)/qt-opengl.a
endif
