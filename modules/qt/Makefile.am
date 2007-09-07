libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

if COND_SINGLE_COMPILATION_UNIT
QT_SOURCES = single-compilation-unit.cc
else
QT_SOURCES = qt.cc QC_Qobject.cc QC_QApplication.cc QC_QPushButton.cc QC_QFont.cc \
	QC_QWidget.cc QC_QFrame.cc QC_QLCDNumber.cc QC_QSlider.cc QC_QLayout.cc \
	QC_QBoxLayout.cc QC_QVBoxLayout.cc QC_QHBoxLayout.cc QC_QGridLayout.cc \
	QC_QBrush.cc QC_QColor.cc QC_QPaintDevice.cc QC_QRect.cc QC_QRectF.cc \
	QC_QPainter.cc QC_QPalette.cc QoreQtDynamicMethod.cc QC_QRegion.cc \
	QC_QPaintEvent.cc QC_QPoint.cc QC_QTimer.cc QC_QLabel.cc QC_QPicture.cc \
	QC_QPixmap.cc QC_QMovie.cc QC_QBitmap.cc QC_QImage.cc QC_QDateTime.cc \
	QC_QDate.cc QC_QTime.cc QT_BrushStyle.cc QC_QIcon.cc QC_QKeySequence.cc \
	QC_QAction.cc QC_QActionGroup.cc QT_PenStyle.cc QC_QMouseEvent.cc \
	QC_QEvent.cc QC_QInputEvent.cc QC_QKeyEvent.cc QC_QMoveEvent.cc \
	QC_QResizeEvent.cc QC_QSize.cc QC_QMatrix.cc QC_QShortcut.cc \
	QC_QPointF.cc \
	QC_QPolygon.cc \
	QC_QPolygonF.cc \
	QC_QLine.cc \
	QC_QLineF.cc \
	QC_QAbstractButton.cc \
	QC_QMenu.cc \
	QC_QToolButton.cc \
	QC_QDialog.cc \
	QC_QLineEdit.cc \
	QC_QTextLength.cc \
	QC_QTextFormat.cc \
	QC_QTextBlockFormat.cc \
	QC_QTextCharFormat.cc \
	QC_QPen.cc \
	QC_QTextFrameFormat.cc \
	QC_QTextTableFormat.cc \
	QC_QTextListFormat.cc \
	QC_QTextImageFormat.cc \
	QC_QCalendarWidget.cc \
	QC_QStyleOption.cc \
	QC_QModelIndex.cc \
	QC_QStyleOptionViewItem.cc \
	QC_QStyleOptionViewItemV2.cc \
	QC_QAbstractItemModel.cc \
	QC_QAbstractItemDelegate.cc \
	QC_QItemDelegate.cc \
	QC_QComboBox.cc \
	QC_QCheckBox.cc \
	QC_QAbstractSpinBox.cc \
	QC_QDateTimeEdit.cc \
	QC_QLocale.cc \
	QC_QByteArray.cc \
	QC_QUrl.cc \
	QC_QVariant.cc \
	QC_QGroupBox.cc \
	QC_QDateEdit.cc
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
