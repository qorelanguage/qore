libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

QT_SOURCES = qt-core-module.cc

if COND_SINGLE_COMPILATION_UNIT
QORE_QT_SOURCES = single-compilation-unit.cc
else
QORE_QT_SOURCES = qt-core.cc \
	QC_QObject.cc \
	QC_QRect.cc \
	QC_QRectF.cc \
	QoreQtDynamicMethod.cc \
	QC_QEvent.cc \
	QC_QSize.cc \
	QC_QPoint.cc \
	QC_QPointF.cc \
	QC_QLine.cc \
	QC_QLineF.cc \
	QC_QDateTime.cc \
	QC_QDate.cc \
	QC_QTime.cc 
	QoreAbstractQAbstractItemModel.cc \
	QoreAbstractQObject.cc \
	QC_QAbstractItemModel.cc \
	QC_QLocale.cc \
	QC_QByteArray.cc \
	QC_QUrl.cc \
	QC_QVariant.cc \
	QC_QChar.cc \
	QC_QDir.cc \
	QC_QRegExp.cc \
	QC_QFileInfo.cc \
	QC_QChildEvent.cc \
	QC_QTimerEvent.cc \
	QC_QLibraryInfo.cc \
	QC_QCoreApplication.cc \
	QC_QTimeLine.cc
endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libqt-core.la
libqt_core_la_SOURCES = $(QT_SOURCES)
libqt_core_la_LDFLAGS = ${QT_CORE_LIBS} -static
else
lib_LTLIBRARIES = libqore-qt-core.la
libqore_qt_core_la_SOURCES = $(QORE_QT_SOURCES)
libqore_qt_core_la_LDFLAGS = ${QT_CORE_LIBS}
pkglib_LTLIBRARIES = qt-core.la
qt_core_la_SOURCES = $(QT_SOURCES)
qt_core_la_LDFLAGS = -module -avoid-version -lqore-qt-core
endif

INCLUDES = -I$(top_srcdir)/include $(QT_CORE_INCLUDES)

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
	mv $(pkglibdir)/qt-core.$(MODULE_SUFFIX) $(pkglibdir)/qt-core.qmod
	rm -f $(pkglibdir)/qt-core.la $(pkglibdir)/qt-core.a
endif
