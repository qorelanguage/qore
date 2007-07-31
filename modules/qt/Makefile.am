libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
QT_SOURCES = single-compilation-unit.cc
else
QT_SOURCES = qt.cc QC_Qobject.cc QC_QApplication.cc QC_QPushButton.cc QC_QFont.cc QC_QWidget.cc QC_QFrame.cc QC_QLCDNumber.cc QC_QSlider.cc QC_QLayout.cc QC_QBoxLayout.cc QC_QVBoxLayout.cc QC_QHBoxLayout.cc QC_QGridLayout.cc QC_QBrush.cc QC_QColor.cc QC_QPaintDevice.cc QC_QRect.cc QC_QRectF.cc QC_QPainter.cc QC_QPalette.cc QoreQtDynamicMethod.cc QC_QRegion.cc QC_QPaintEvent.cc
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libqt.la
libqt_la_SOURCES = $(QT_SOURCES)
libqt_la_LDFLAGS = ${QT_LIBS} -static
else
pkglib_LTLIBRARIES = qt.la
qt_la_SOURCES = $(QT_SOURCES)
qt_la_LDFLAGS = -module -avoid-version ${QT_LIBS}
endif

INCLUDES = -I$(top_srcdir)/include $(QT_INCLUDES)

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
	mv $(pkglibdir)/qt.$(MODULE_SUFFIX) $(pkglibdir)/qt.qmod
	rm -f $(pkglibdir)/qt.la $(pkglibdir)/qt.a
endif
