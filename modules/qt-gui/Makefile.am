libdir = ${exec_prefix}/lib${LIBSUFFIX}
pkglibdir = $(libdir)/@PACKAGE@-@VERSION@

QT_SOURCES = qt-gui-module.cc

if COND_SINGLE_COMPILATION_UNIT_QT
QORE_QT_GUI_SOURCES = single-compilation-unit.cc
else
QORE_QT_GUI_SOURCES = qt-gui-util.cc QC_QApplication.cc QC_QPushButton.cc \
	QC_QFrame.cc QC_QLCDNumber.cc QC_QSlider.cc QC_QLayout.cc \
	QC_QBoxLayout.cc QC_QVBoxLayout.cc QC_QHBoxLayout.cc \
	QC_QGridLayout.cc \
	QC_QBrush.cc \
	QC_QPainter.cc QC_QPalette.cc QC_QRegion.cc \
	QC_QLabel.cc \
	QC_QMovie.cc BrushStyleNode.cc \
	QC_QKeySequence.cc \
	QC_QAction.cc QC_QActionGroup.cc PenStyleNode.cc \
	QC_QMatrix.cc QC_QShortcut.cc \
	QC_QAbstractButton.cc \
	QC_QWidget.cc \
	QC_QColor.cc \
	QC_QPaintDevice.cc \
	QC_QMouseEvent.cc \
	QC_QInputEvent.cc \
	QC_QKeyEvent.cc \
	QC_QMoveEvent.cc \
	QC_QResizeEvent.cc \
	QC_QPaintEvent.cc \
	QC_QPolygon.cc \
	QC_QPolygonF.cc \
	QC_QIcon.cc \
	QC_QFont.cc \
	QC_QPixmap.cc \
	QC_QImage.cc \
	QC_QPicture.cc \
	QC_QBitmap.cc \
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
	QC_QStyleOptionViewItem.cc \
	QC_QStyleOptionViewItemV2.cc \
	QC_QAbstractItemDelegate.cc \
	QC_QItemDelegate.cc \
	QC_QComboBox.cc \
	QC_QCheckBox.cc \
	QC_QAbstractSpinBox.cc \
	QC_QDateTimeEdit.cc \
	QC_QGroupBox.cc \
	QC_QDateEdit.cc \
	QC_QFontMetrics.cc \
	QC_QFontDatabase.cc \
	QC_QFontInfo.cc \
	QC_QScrollBar.cc \
	QC_QAbstractScrollArea.cc \
	QC_QScrollArea.cc \
	QC_QActionEvent.cc \
	QC_QCloseEvent.cc \
	QC_QContextMenuEvent.cc \
	QC_QDropEvent.cc \
	QC_QDragMoveEvent.cc \
	QC_QDragEnterEvent.cc \
	QC_QDragLeaveEvent.cc \
	QC_QFocusEvent.cc \
	QC_QHideEvent.cc \
	QC_QInputMethodEvent.cc \
	QC_QShowEvent.cc \
	QC_QTabletEvent.cc \
	QC_QWheelEvent.cc \
	QC_QClipboard.cc \
	QC_QFontComboBox.cc \
	QC_QMainWindow.cc \
	QC_QRadioButton.cc \
	QC_QStyle.cc \
	QC_QStyleOptionComplex.cc \
	QC_QStyleOptionComboBox.cc \
	QC_QStyleOptionGroupBox.cc \
	QC_QStyleOptionSizeGrip.cc \
	QC_QStyleOptionSlider.cc \
	QC_QStyleOptionSpinBox.cc \
	QC_QStyleOptionTitleBar.cc \
	QC_QStyleOptionToolButton.cc \
	QC_QMotifStyle.cc \
	QC_QCDEStyle.cc \
	QC_QWindowsStyle.cc \
	QC_QCleanlooksStyle.cc \
	QC_QPlastiqueStyle.cc \
	QC_QSpinBox.cc \
	QC_QAbstractItemView.cc \
	QC_QTableView.cc \
	QC_QTableWidget.cc \
	QC_QTableWidgetItem.cc \
	QC_QStyleOptionMenuItem.cc \
	QC_QMessageBox.cc \
	QC_QStyleOptionButton.cc \
	QC_QFileDialog.cc \
	QC_QHeaderView.cc \
	QC_QMenuBar.cc \
	QC_QPrinter.cc \
	QC_QPrintDialog.cc \
	QC_QValidator.cc \
	QC_QDoubleValidator.cc \
	QC_QIntValidator.cc \
	QC_QRegExpValidator.cc \
	QC_QColorDialog.cc \
	QC_QInputDialog.cc \
	QC_QImageWriter.cc \
	QC_QDial.cc \
	QC_QStackedWidget.cc \
	QC_QDoubleSpinBox.cc \
	QC_QTimeEdit.cc \
	QC_QProgressBar.cc \
	QC_QPainterPath.cc \
	QC_QPaintEngine.cc \
	QC_QTextEdit.cc \
	QC_QTabBar.cc \
	QC_QStyleOptionTab.cc \
	QC_QStyleOptionTabWidgetFrame.cc \
	QC_QTabWidget.cc \
	QC_QAbstractSlider.cc \
	QC_QHelpEvent.cc \
	QC_QDesktopWidget.cc \
	QC_QSystemTrayIcon.cc \
	QC_QWizard.cc \
	QC_QWizardPage.cc \
	QC_QListView.cc \
	QC_QListWidget.cc \
	QC_QListWidgetItem.cc \
	QC_QDialogButtonBox.cc \
	QC_QToolBar.cc \
	QC_QProgressDialog.cc \
	QC_QFontDialog.cc \
	QC_QErrorMessage.cc \
	QC_QStackedLayout.cc \
	QC_QGradient.cc \
	QC_QLinearGradient.cc \
	QC_QTextBrowser.cc \
	QC_QLayoutItem.cc \
	QC_QWidgetItem.cc \
	QC_QCursor.cc \
	QC_QStyleOptionGraphicsItem.cc \
	QC_QTransform.cc \
	QC_QGraphicsSceneEvent.cc \
	QC_QGraphicsSceneContextMenuEvent.cc \
	QC_QGraphicsSceneDragDropEvent.cc \
	QC_QGraphicsSceneHelpEvent.cc \
	QC_QGraphicsSceneHoverEvent.cc \
	QC_QGraphicsSceneMouseEvent.cc \
	QC_QGraphicsSceneWheelEvent.cc \
	QC_QSplashScreen.cc \
	QC_QSplitter.cc \
	QC_QSplitterHandle.cc \
	QC_QGraphicsItem.cc \
	QC_QAbstractGraphicsShapeItem.cc \
	QC_QGraphicsEllipseItem.cc \
	QC_QGraphicsPathItem.cc \
	QC_QGraphicsPolygonItem.cc \
	QC_QGraphicsRectItem.cc \
	QC_QGraphicsSimpleTextItem.cc \
	QC_QGraphicsItemGroup.cc \
	QC_QGraphicsLineItem.cc \
	QC_QGraphicsPixmapItem.cc \
	QC_QTextLine.cc \
	QC_QTextOption.cc \
	QC_QTextLayout.cc
if COND_MACOSX
QORE_QT_GUI_SOURCES += QC_QMacStyle.cc
endif

endif

if COND_MONOLITHIC
noinst_LTLIBRARIES = libqt-gui.la
libqt_gui_la_SOURCES = $(QT_SOURCES) $(QORE_QT_GUI_SOURCES)
libqt_gui_la_LDFLAGS = ${QT_CORE_LIBS} ${QT_GUI_LIBS} -static
else
lib_LTLIBRARIES = libqore-qt-gui.la
libqore_qt_gui_la_SOURCES = $(QORE_QT_GUI_SOURCES)
libqore_qt_gui_la_LDFLAGS = ${QT_CORE_LIBS} ${QT_GUI_LIBS}
pkglib_LTLIBRARIES = qt-gui.la
qt_gui_la_SOURCES = $(QT_SOURCES)
qt_gui_la_LDFLAGS = -module -avoid-version -lqore-qt-gui
qt_gui_la_LIBADD = ../qt-core/libqore-qt-core.la
endif

INCLUDES = -I$(top_srcdir)/include $(QT_CORE_INCLUDES) $(QT_GUI_INCLUDES) -I ../qt-core

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
	mv $(pkglibdir)/qt-gui.$(MODULE_SUFFIX) $(pkglibdir)/qt-gui.qmod
	rm -f $(pkglibdir)/qt-gui.la $(pkglibdir)/qt-gui.a
endif
