/*
  qt-module.cc
  
  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <qore/Qore.h>

#include "QC_QApplication.h"
#include "QC_QPushButton.h"
#include "QC_QFont.h"
#include "QC_QMatrix.h"
#include "QC_QWidget.h"
#include "QC_QFrame.h"
#include "QC_QLCDNumber.h"
#include "QC_QLayout.h"
#include "QC_QBoxLayout.h"
#include "QC_QVBoxLayout.h"
#include "QC_QHBoxLayout.h"
#include "QC_QGridLayout.h"
#include "QC_QBrush.h"
#include "QC_QColor.h"
#include "QC_QPalette.h"
#include "QC_QPaintDevice.h"
#include "QC_QPainter.h"
#include "QC_QRegion.h"
#include "QC_QLabel.h"
#include "QC_QAbstractSlider.h"
#include "QC_QSlider.h"
#include "QC_QPicture.h"
#include "QC_QPixmap.h"
#include "QC_QBitmap.h"
#include "QC_QMovie.h"
#include "QC_QShortcut.h"
#include "QC_QImage.h"
#include "QC_QIcon.h"
#include "QC_QKeySequence.h"
#include "QC_QAction.h"
#include "QC_QActionGroup.h"
#include "QC_QPolygon.h"
#include "QC_QPolygonF.h"
#include "QC_QAbstractButton.h"
#include "QC_QMenu.h"
#include "QC_QToolButton.h"
#include "QC_QDialog.h"
#include "QC_QLineEdit.h"
#include "QC_QTextLength.h"
#include "QC_QTextFormat.h"
#include "QC_QTextBlockFormat.h"
#include "QC_QTextCharFormat.h"
#include "QC_QPen.h"
#include "QC_QTextFrameFormat.h"
#include "QC_QTextTableFormat.h"
#include "QC_QTextListFormat.h"
#include "QC_QTextImageFormat.h"
#include "QC_QCalendarWidget.h"
#include "QC_QStyleOption.h"
#include "QC_QStyleOptionViewItem.h"
#include "QC_QStyleOptionViewItemV2.h"
#include "QC_QAbstractItemDelegate.h"
#include "QC_QItemDelegate.h"
#include "QC_QComboBox.h"
#include "QC_QCheckBox.h"
#include "QC_QAbstractSpinBox.h"
#include "QC_QDateTimeEdit.h"
#include "QC_QGroupBox.h"
#include "QC_QDateEdit.h"
#include "QC_QFontMetrics.h"
#include "QC_QFontDatabase.h"
#include "QC_QFontInfo.h"
#include "QC_QScrollBar.h"
#include "QC_QAbstractScrollArea.h"
#include "QC_QScrollArea.h"
#include "QC_QClipboard.h"
#include "QC_QFontComboBox.h"
#include "QC_QMainWindow.h"
#include "QC_QRadioButton.h"
#include "QC_QStyle.h"
#include "QC_QStyleOptionComplex.h"
#include "QC_QStyleOptionComboBox.h"
#include "QC_QStyleOptionGroupBox.h"
#include "QC_QStyleOptionSizeGrip.h"
#include "QC_QStyleOptionSlider.h"
#include "QC_QStyleOptionSpinBox.h"
#include "QC_QStyleOptionTitleBar.h"
#include "QC_QStyleOptionToolButton.h"
#include "QC_QSpinBox.h"
#include "QC_QAbstractItemView.h"
#include "QC_QTableView.h"
#include "QC_QTableWidget.h"
#include "QC_QTableWidgetItem.h"
#include "QC_QStyleOptionMenuItem.h"
#include "QC_QMessageBox.h"
#include "QC_QStyleOptionButton.h"
#include "QC_QFileDialog.h"
#include "QC_QHeaderView.h"
#include "QC_QMenuBar.h"
#include "QC_QPrinter.h"
#include "QC_QPrintDialog.h"
#include "QC_QValidator.h"
#include "QC_QDoubleValidator.h"
#include "QC_QIntValidator.h"
#include "QC_QRegExpValidator.h"
#include "QC_QColorDialog.h"
#include "QC_QInputDialog.h"
#include "QC_QImageWriter.h"
#include "QC_QDial.h"
#include "QC_QStackedWidget.h"
#include "QC_QDoubleSpinBox.h"
#include "QC_QTimeEdit.h"
#include "QC_QProgressBar.h"
#include "QC_QPainterPath.h"
#include "QC_QPaintEngine.h"
#include "QC_QTextEdit.h"
#include "QC_QTabBar.h"
#include "QC_QStyleOptionTab.h"
#include "QC_QStyleOptionTabWidgetFrame.h"
#include "QC_QTabWidget.h"
#include "QC_QDesktopWidget.h"
#include "QC_QSystemTrayIcon.h"
#include "QC_QWizard.h"
#include "QC_QWizardPage.h"
#include "QC_QListView.h"
#include "QC_QListWidgetItem.h"
#include "QC_QDialogButtonBox.h"
#include "QC_QToolBar.h"
#include "QC_QProgressDialog.h"
#include "QC_QFontDialog.h"
#include "QC_QErrorMessage.h"
#include "QC_QStackedLayout.h"
#include "QC_QGradient.h"
#include "QC_QLayoutItem.h"
#include "QC_QWidgetItem.h"
#include "QC_QCursor.h"
#include "QC_QStyleOptionGraphicsItem.h"
#include "QC_QTransform.h"
#include "QC_QSplashScreen.h"
#include "QC_QSplitter.h"
#include "QC_QSplitterHandle.h"
#include "QC_QGraphicsItem.h"
#include "QC_QTextLine.h"
#include "QC_QTextOption.h"
#include "QC_QTextLayout.h"
#include "QC_QGraphicsSceneEvent.h"
#include "QC_QGraphicsSceneContextMenuEvent.h"
#include "QC_QGraphicsSceneDragDropEvent.h"
#include "QC_QGraphicsSceneHelpEvent.h"
#include "QC_QGraphicsSceneHoverEvent.h"
#include "QC_QGraphicsSceneMouseEvent.h"
#include "QC_QGraphicsSceneWheelEvent.h"

#include "qore-qt.h"

#include <QPalette>
#include <QToolTip>
#include <QStyleFactory>

#include <assert.h>

static QoreStringNode *qt_module_init();
static void qt_module_ns_init(QoreNamespace *rns, QoreNamespace *qns);
static void qt_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "qt-gui";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "QT 4 module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://www.qoretechnologies.com/qore";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = qt_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = qt_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = qt_module_delete;
DLLEXPORT qore_license_t qore_module_license = QL_GPL;
DLLEXPORT char *qore_module_dependencies[] = { "qt-core", 0 };
#endif

static class AbstractQoreNode *f_QAPP(const QoreListNode *params, class ExceptionSink *xsink)
{
   return get_qore_qapp();
}


static AbstractQoreNode *f_QToolTip_font(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(QToolTip::font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return o_qf;
}

//void hideText ()
static AbstractQoreNode *f_QToolTip_hideText(const QoreListNode *params, ExceptionSink *xsink)
{
   QToolTip::hideText();
   return 0;
}

//QPalette palette ()
static AbstractQoreNode *f_QToolTip_palette(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(QToolTip::palette());
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return o_qp;
}

//void setFont ( const QFont & font )
static AbstractQoreNode *f_QToolTip_setFont(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQFont *font = p ? (QoreQFont *)p->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLTIP-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QToolTip_setFont()");
      return 0;
   }
   ReferenceHolder<QoreQFont> fontHolder(font, xsink);
   QToolTip::setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setPalette ( const QPalette & palette )
static AbstractQoreNode *f_QToolTip_setPalette(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQPalette *palette = p ? (QoreQPalette *)p->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLTIP-SETPALETTE-PARAM-ERROR", "expecting a QPalette object as first argument to QToolTip_setPalette()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> paletteHolder(palette, xsink);
   QToolTip::setPalette(*(palette->getQPalette()));
   return 0;
}


//void showText ( const QPoint & pos, const QString & text, QWidget * w, const QRect & rect ) 
//void showText ( const QPoint & pos, const QString & text, QWidget * w = 0 )
static class AbstractQoreNode *f_QToolTip_showText(const QoreListNode *params, class ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQPoint *pos = p ? (QoreQPoint *)p->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "QToolTip_showText() was expecting a QPoint as the first argument");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   const QoreStringNode *str = test_string_param(params, 1);
   if (!str) {
      xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "expecting a string as second argument to QToolTip_showText()");
      return 0;
   }
   const char *text = str->getBuffer();

   p = test_object_param(params, 2);
   QoreQWidget *w = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (p && !w) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "QToolTip_showText() does not know how to handle arguments of class '%s' as passed as the third argument", p->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQWidget> wHolder(w, xsink);

   QoreQRect *rect = 0;
   if (w) {
      p = test_object_param(params, 3);
      rect = p ? (QoreQRect *)p->getReferencedPrivateData(CID_QRECT, xsink) : 0;
      if (!rect && p) {
	 if (!xsink->isException())
	    xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "this version of QToolTip_showText() does not know how to handle arguments of class '%s' as passed as the fourth argument", p->getClass()->getName());
	 return 0;
      }
   }
   ReferenceHolder<QoreQRect> rectHolder(rect, xsink);

   if (rect)
      QToolTip::showText(*(static_cast<QPoint *>(pos)), text, w ? w->getQWidget() : 0, *(static_cast<QRect *>(rect)));
   else
      QToolTip::showText(*(static_cast<QPoint *>(pos)), text, w ? w->getQWidget() : 0);

   return 0;
}

//QStyle * create ( const QString & key )
AbstractQoreNode *f_QStyleFactory_create(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;

   return return_qstyle(key, QStyleFactory::create(key), xsink);
}

//QStringList keys ()
static AbstractQoreNode *f_QStyleFactory_keys(const QoreListNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QStyleFactory::keys();
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

//int qAlpha ( QRgb rgba )
static AbstractQoreNode *f_qAlpha(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreBigIntNode(qAlpha(p ? p->getAsBigInt() : 0));
}

//int qBlue ( QRgb rgb )
static AbstractQoreNode *f_qBlue(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreBigIntNode(qBlue(p ? p->getAsBigInt() : 0));
}

//int qGray ( int r, int g, int b )
//int qGray ( QRgb rgb )
static AbstractQoreNode *f_qGray(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (num_params(params) == 1)
      return new QoreBigIntNode(qGray(p ? p->getAsBigInt() : 0));
   int r = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int g = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int b = p ? p->getAsInt() : 0;

   return new QoreBigIntNode(qGray(r, g, b));
}

//int qGreen ( QRgb rgb )
static AbstractQoreNode *f_qGreen(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreBigIntNode(qGreen(p ? p->getAsBigInt() : 0));
}

//int qRed ( QRgb rgb )
static AbstractQoreNode *f_qRed(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreBigIntNode(qRed(p ? p->getAsBigInt() : 0));
}

//QRgb qRgb ( int r, int g, int b )
static AbstractQoreNode *f_qRgb(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int r = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int g = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int b = p ? p->getAsInt() : 0;

   return new QoreBigIntNode(qRgb(r, g, b));
}

//QRgb qRgba ( int r, int g, int b, int a )
static AbstractQoreNode *f_qRgba(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int r = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int g = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int b = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int a = p ? p->getAsInt() : 0;

   return new QoreBigIntNode(qRgba(r, g, b, a));
}

static QoreNamespace *qt_ns;

static void init_namespace()
{
   qt_ns = new QoreNamespace("Qt");

    // the order is sensitive here as child classes need the parent IDs
   QoreClass *qwidget, *qlayout, *qframe, 
      *qboxlayout, *qpaintdevice, *qpixmap, *qabstractslider;

   qt_ns->addSystemClass(initQApplicationClass(QC_QCoreApplication));
   qt_ns->addSystemClass(initQActionClass(QC_QObject));
   qt_ns->addSystemClass(initQActionGroupClass(QC_QObject));
   qt_ns->addSystemClass(initQShortcutClass(QC_QObject));

   qt_ns->addSystemClass((qpaintdevice = initQPaintDeviceClass()));
   qt_ns->addSystemClass(initQPictureClass(qpaintdevice));

   qt_ns->addSystemClass((qpixmap = initQPixmapClass(qpaintdevice)));
   qt_ns->addSystemClass(initQBitmapClass(qpixmap));

   qt_ns->addSystemClass((qwidget = initQWidgetClass(QC_QObject, qpaintdevice)));

   qt_ns->addSystemClass((qabstractslider = initQAbstractSliderClass(qwidget)));

   qt_ns->addSystemClass((qframe = initQFrameClass(qwidget)));
   qt_ns->addSystemClass(initQLCDNumberClass(qframe));
   qt_ns->addSystemClass(initQLabelClass(qframe));

   qt_ns->addSystemClass(initQBrushClass());
   qt_ns->addSystemClass(initQColorClass());

   qt_ns->addSystemClass(initQKeySequenceClass());
   qt_ns->addSystemClass(initQFontClass());
   qt_ns->addSystemClass(initQMatrixClass());

   QoreNamespace *qimage = new QoreNamespace("QImage");

   // InvertMode enum
   qimage->addConstant("InvertRgb",                new QoreBigIntNode(QImage::InvertRgb));
   qimage->addConstant("InvertRgba",               new QoreBigIntNode(QImage::InvertRgba));

   // Format enum
   qimage->addConstant("Format_Invalid",           new QoreBigIntNode(QImage::Format_Invalid));
   qimage->addConstant("Format_Mono",              new QoreBigIntNode(QImage::Format_Mono));
   qimage->addConstant("Format_MonoLSB",           new QoreBigIntNode(QImage::Format_MonoLSB));
   qimage->addConstant("Format_Indexed8",          new QoreBigIntNode(QImage::Format_Indexed8));
   qimage->addConstant("Format_RGB32",             new QoreBigIntNode(QImage::Format_RGB32));
   qimage->addConstant("Format_ARGB32",            new QoreBigIntNode(QImage::Format_ARGB32));
   qimage->addConstant("Format_ARGB32_Premultiplied", new QoreBigIntNode(QImage::Format_ARGB32_Premultiplied));
   qimage->addConstant("Format_RGB16",             new QoreBigIntNode(QImage::Format_RGB16));

   qimage->addSystemClass(initQImageClass(qpaintdevice));

   qt_ns->addInitialNamespace(qimage);

   QoreNamespace *qregion = new QoreNamespace("QRegion");
   
   // RegionType enum
   qregion->addConstant("Rectangle",                new QoreBigIntNode(QRegion::Rectangle));
   qregion->addConstant("Ellipse",                  new QoreBigIntNode(QRegion::Ellipse));

   qregion->addSystemClass(initQRegionClass());

   qt_ns->addInitialNamespace(qregion);

   QoreClass *qlayoutitem;
   qt_ns->addSystemClass((qlayoutitem = initQLayoutItemClass()));

   QoreNamespace *qlayout_ns = new QoreNamespace("QLayout");

   qlayout_ns->addSystemClass((qlayout = initQLayoutClass(QC_QObject, qlayoutitem)));
   qlayout_ns->addSystemClass(initQGridLayoutClass(qlayout));

   qlayout_ns->addSystemClass((qboxlayout = initQBoxLayoutClass(qlayout)));
   qlayout_ns->addSystemClass(initQVBoxLayoutClass(qboxlayout));
   qlayout_ns->addSystemClass(initQHBoxLayoutClass(qboxlayout));

   qlayout_ns->addConstant("SetNoConstraint",          new QoreBigIntNode(QLayout::SetNoConstraint));
   qlayout_ns->addConstant("SetMinimumSize",           new QoreBigIntNode(QLayout::SetMinimumSize));
   qlayout_ns->addConstant("SetFixedSize",             new QoreBigIntNode(QLayout::SetFixedSize));
   qlayout_ns->addConstant("SetMaximumSize",           new QoreBigIntNode(QLayout::SetMaximumSize));
   qlayout_ns->addConstant("SetMinAndMaxSize",         new QoreBigIntNode(QLayout::SetMinAndMaxSize));

   qt_ns->addInitialNamespace(qlayout_ns);

   QoreNamespace *qmovie = new QoreNamespace("QMovie");

   // MovieState enum
   qmovie->addConstant("NotRunning",               new QoreBigIntNode(QMovie::NotRunning));
   qmovie->addConstant("Paused",                   new QoreBigIntNode(QMovie::Paused));
   qmovie->addConstant("Running",                  new QoreBigIntNode(QMovie::Running));

   // CacheMode enum
   qmovie->addConstant("CacheNone",                new QoreBigIntNode(QMovie::CacheNone));
   qmovie->addConstant("CacheAll",                 new QoreBigIntNode(QMovie::CacheAll));

   qmovie->addSystemClass(initQMovieClass(QC_QObject));

   qt_ns->addInitialNamespace(qmovie);

   QoreNamespace *qslider = new QoreNamespace("QSlider");

   // TickPosition enum
   qslider->addConstant("NoTicks",                  new QoreBigIntNode(QSlider::NoTicks));
   qslider->addConstant("TicksAbove",               new QoreBigIntNode(QSlider::TicksAbove));
   qslider->addConstant("TicksLeft",                new QoreBigIntNode(QSlider::TicksLeft));
   qslider->addConstant("TicksBelow",               new QoreBigIntNode(QSlider::TicksBelow));
   qslider->addConstant("TicksRight",               new QoreBigIntNode(QSlider::TicksRight));
   qslider->addConstant("TicksBothSides",           new QoreBigIntNode(QSlider::TicksBothSides));

   qslider->addSystemClass(initQSliderClass(qabstractslider));

   qt_ns->addInitialNamespace(qslider);

   QoreNamespace *qsizepolicy = new QoreNamespace("QSizePolicy");

   // PolicyFlag enum
   qsizepolicy->addConstant("GrowFlag",                 new QoreBigIntNode(QSizePolicy::GrowFlag));
   qsizepolicy->addConstant("ExpandFlag",               new QoreBigIntNode(QSizePolicy::ExpandFlag));
   qsizepolicy->addConstant("ShrinkFlag",               new QoreBigIntNode(QSizePolicy::ShrinkFlag));
   qsizepolicy->addConstant("IgnoreFlag",               new QoreBigIntNode(QSizePolicy::IgnoreFlag));

   // Policy enum
   qsizepolicy->addConstant("Fixed",                    new QoreBigIntNode(QSizePolicy::Fixed));
   qsizepolicy->addConstant("Minimum",                  new QoreBigIntNode(QSizePolicy::Minimum));
   qsizepolicy->addConstant("Maximum",                  new QoreBigIntNode(QSizePolicy::Maximum));
   qsizepolicy->addConstant("Preferred",                new QoreBigIntNode(QSizePolicy::Preferred));
   qsizepolicy->addConstant("MinimumExpanding",         new QoreBigIntNode(QSizePolicy::MinimumExpanding));
   qsizepolicy->addConstant("Expanding",                new QoreBigIntNode(QSizePolicy::Expanding));
   qsizepolicy->addConstant("Ignored",                  new QoreBigIntNode(QSizePolicy::Ignored));

   // ControlType enum
   qsizepolicy->addConstant("DefaultType",              new QoreBigIntNode(QSizePolicy::DefaultType));
   qsizepolicy->addConstant("ButtonBox",                new QoreBigIntNode(QSizePolicy::ButtonBox));
   qsizepolicy->addConstant("CheckBox",                 new QoreBigIntNode(QSizePolicy::CheckBox));
   qsizepolicy->addConstant("ComboBox",                 new QoreBigIntNode(QSizePolicy::ComboBox));
   qsizepolicy->addConstant("Frame",                    new QoreBigIntNode(QSizePolicy::Frame));
   qsizepolicy->addConstant("GroupBox",                 new QoreBigIntNode(QSizePolicy::GroupBox));
   qsizepolicy->addConstant("Label",                    new QoreBigIntNode(QSizePolicy::Label));
   qsizepolicy->addConstant("Line",                     new QoreBigIntNode(QSizePolicy::Line));
   qsizepolicy->addConstant("LineEdit",                 new QoreBigIntNode(QSizePolicy::LineEdit));
   qsizepolicy->addConstant("PushButton",               new QoreBigIntNode(QSizePolicy::PushButton));
   qsizepolicy->addConstant("RadioButton",              new QoreBigIntNode(QSizePolicy::RadioButton));
   qsizepolicy->addConstant("Slider",                   new QoreBigIntNode(QSizePolicy::Slider));
   qsizepolicy->addConstant("SpinBox",                  new QoreBigIntNode(QSizePolicy::SpinBox));
   qsizepolicy->addConstant("TabWidget",                new QoreBigIntNode(QSizePolicy::TabWidget));
   qsizepolicy->addConstant("ToolButton",               new QoreBigIntNode(QSizePolicy::ToolButton));

   qt_ns->addInitialNamespace(qsizepolicy);

   QoreNamespace *qicon = new QoreNamespace("QIcon");

   // Mode enum
   qicon->addConstant("Normal",                   new QoreBigIntNode(QIcon::Normal));
   qicon->addConstant("Disabled",                 new QoreBigIntNode(QIcon::Disabled));
   qicon->addConstant("Active",                   new QoreBigIntNode(QIcon::Active));
   qicon->addConstant("Selected",                 new QoreBigIntNode(QIcon::Selected));

   // State enum
   qicon->addConstant("On",                       new QoreBigIntNode(QIcon::On));
   qicon->addConstant("Off",                      new QoreBigIntNode(QIcon::Off));

   qicon->addSystemClass(initQIconClass());
   qt_ns->addInitialNamespace(qicon);

   qt_ns->addInitialNamespace(initQPaletteNS());

   QoreNamespace *qpainter_ns = new QoreNamespace("QPainter");
   
   // RenderHint enum
   qpainter_ns->addConstant("Antialiasing",             new QoreBigIntNode(QPainter::Antialiasing));
   qpainter_ns->addConstant("TextAntialiasing",         new QoreBigIntNode(QPainter::TextAntialiasing));
   qpainter_ns->addConstant("SmoothPixmapTransform",    new QoreBigIntNode(QPainter::SmoothPixmapTransform));
   qpainter_ns->addConstant("HighQualityAntialiasing",  new QoreBigIntNode(QPainter::HighQualityAntialiasing));
   
   // CompositionMode enum
   qpainter_ns->addConstant("CompositionMode_SourceOver",      new QoreBigIntNode(QPainter::CompositionMode_SourceOver));
   qpainter_ns->addConstant("CompositionMode_DestinationOver", new QoreBigIntNode(QPainter::CompositionMode_DestinationOver));
   qpainter_ns->addConstant("CompositionMode_Clear",           new QoreBigIntNode(QPainter::CompositionMode_Clear));
   qpainter_ns->addConstant("CompositionMode_Source",          new QoreBigIntNode(QPainter::CompositionMode_Source));
   qpainter_ns->addConstant("CompositionMode_Destination",     new QoreBigIntNode(QPainter::CompositionMode_Destination));
   qpainter_ns->addConstant("CompositionMode_SourceIn",        new QoreBigIntNode(QPainter::CompositionMode_SourceIn));
   qpainter_ns->addConstant("CompositionMode_DestinationIn",   new QoreBigIntNode(QPainter::CompositionMode_DestinationIn));
   qpainter_ns->addConstant("CompositionMode_SourceOut",       new QoreBigIntNode(QPainter::CompositionMode_SourceOut));
   qpainter_ns->addConstant("CompositionMode_DestinationOut",  new QoreBigIntNode(QPainter::CompositionMode_DestinationOut));
   qpainter_ns->addConstant("CompositionMode_SourceAtop",      new QoreBigIntNode(QPainter::CompositionMode_SourceAtop));
   qpainter_ns->addConstant("CompositionMode_DestinationAtop", new QoreBigIntNode(QPainter::CompositionMode_DestinationAtop));
   qpainter_ns->addConstant("CompositionMode_Xor",             new QoreBigIntNode(QPainter::CompositionMode_Xor));
   qpainter_ns->addConstant("CompositionMode_Plus",            new QoreBigIntNode(QPainter::CompositionMode_Plus));
   qpainter_ns->addConstant("CompositionMode_Multiply",        new QoreBigIntNode(QPainter::CompositionMode_Multiply));
   qpainter_ns->addConstant("CompositionMode_Screen",          new QoreBigIntNode(QPainter::CompositionMode_Screen));
   qpainter_ns->addConstant("CompositionMode_Overlay",         new QoreBigIntNode(QPainter::CompositionMode_Overlay));
   qpainter_ns->addConstant("CompositionMode_Darken",          new QoreBigIntNode(QPainter::CompositionMode_Darken));
   qpainter_ns->addConstant("CompositionMode_Lighten",         new QoreBigIntNode(QPainter::CompositionMode_Lighten));
   qpainter_ns->addConstant("CompositionMode_ColorDodge",      new QoreBigIntNode(QPainter::CompositionMode_ColorDodge));
   qpainter_ns->addConstant("CompositionMode_ColorBurn",       new QoreBigIntNode(QPainter::CompositionMode_ColorBurn));
   qpainter_ns->addConstant("CompositionMode_HardLight",       new QoreBigIntNode(QPainter::CompositionMode_HardLight));
   qpainter_ns->addConstant("CompositionMode_SoftLight",       new QoreBigIntNode(QPainter::CompositionMode_SoftLight));
   qpainter_ns->addConstant("CompositionMode_Difference",      new QoreBigIntNode(QPainter::CompositionMode_Difference));
   qpainter_ns->addConstant("CompositionMode_Exclusion",       new QoreBigIntNode(QPainter::CompositionMode_Exclusion));

   qpainter_ns->addSystemClass(initQPainterClass());

   qt_ns->addInitialNamespace(qpainter_ns);

   QoreClass *qabstractbutton, *qtextformat, *qtextframeformat, *qtextcharformat,
      *qstyleoption, *qstyleoptionviewitem, *qabstractitemdelegate,
      *qabstractspinbox, *qdatetimeedit, *qabstractscrollarea, 
      *qcombobox, *qstyleoptioncomplex, *qabstractitemview, 
      *qtableview, *qdialog, *qvalidator;

   QoreNamespace *qdialog_ns = new QoreNamespace("QDialog");

   qdialog_ns->addSystemClass((qdialog = initQDialogClass(qwidget)));
   qdialog_ns->addInitialNamespace(initQFileDialogNS(qdialog));
   qdialog_ns->addSystemClass(initQPrintDialogClass(qdialog));

   qdialog_ns->addConstant("Rejected",   new QoreBigIntNode(QDialog::Rejected));
   qdialog_ns->addConstant("Accepted",   new QoreBigIntNode(QDialog::Accepted));
 
   qt_ns->addInitialNamespace(initQStyleNS(QC_QObject));

   // automatically added classes
   qt_ns->addSystemClass(initQPolygonClass());
   qt_ns->addSystemClass(initQPolygonFClass());
   qt_ns->addSystemClass((qabstractbutton = initQAbstractButtonClass(qwidget)));
   qt_ns->addSystemClass(initQPushButtonClass(qabstractbutton));
   qt_ns->addSystemClass(initQMenuClass(qwidget));
   qt_ns->addSystemClass(initQToolButtonClass(qabstractbutton));
   qt_ns->addSystemClass(initQTextLengthClass());
   qt_ns->addSystemClass((qtextformat = initQTextFormatClass()));
   qt_ns->addSystemClass(initQTextBlockFormatClass(qtextformat));
   qt_ns->addSystemClass((qtextcharformat = initQTextCharFormatClass(qtextformat)));
   qt_ns->addSystemClass(initQPenClass());
   qt_ns->addSystemClass((qtextframeformat = initQTextFrameFormatClass(qtextformat)));
   qt_ns->addSystemClass(initQTextTableFormatClass(qtextframeformat));
   qt_ns->addSystemClass(initQTextListFormatClass(qtextformat));
   qt_ns->addSystemClass(initQTextImageFormatClass(qtextcharformat));
   qt_ns->addSystemClass((qstyleoption = initQStyleOptionClass()));
   qt_ns->addSystemClass((qstyleoptioncomplex = initQStyleOptionComplexClass(qstyleoption)));
   qt_ns->addSystemClass(initQStyleOptionComboBoxClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionGroupBoxClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionSizeGripClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionSliderClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionSpinBoxClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionTitleBarClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionToolButtonClass(qstyleoptioncomplex));
   qt_ns->addInitialNamespace(initQStyleOptionButtonNS(qstyleoption));
   qt_ns->addSystemClass((qstyleoptionviewitem = initQStyleOptionViewItemClass(qstyleoption)));
   qt_ns->addSystemClass(initQStyleOptionViewItemV2Class(qstyleoptionviewitem));
   qt_ns->addSystemClass((qabstractitemdelegate = initQAbstractItemDelegateClass(QC_QObject)));
   qt_ns->addSystemClass(initQItemDelegateClass(qabstractitemdelegate));
   qt_ns->addSystemClass((qcombobox = initQComboBoxClass(qwidget)));
   qt_ns->addSystemClass(initQCheckBoxClass(qabstractbutton));
   qt_ns->addSystemClass((qabstractspinbox = initQAbstractSpinBoxClass(qwidget)));
   qt_ns->addSystemClass(initQGroupBoxClass(qwidget));
   qt_ns->addSystemClass(initQFontMetricsClass());
   qt_ns->addSystemClass(initQFontDatabaseClass());
   qt_ns->addSystemClass(initQFontInfoClass());
   qt_ns->addSystemClass(initQScrollBarClass(qabstractslider));
   qt_ns->addSystemClass((qabstractscrollarea = initQAbstractScrollAreaClass(qframe)));
   qt_ns->addSystemClass(initQScrollAreaClass(qabstractscrollarea));
   qt_ns->addSystemClass(initQFontComboBoxClass(qcombobox));
   qt_ns->addSystemClass(initQMainWindowClass(qwidget));
   qt_ns->addSystemClass(initQRadioButtonClass(qabstractbutton));
   qt_ns->addSystemClass(initQSpinBoxClass(qabstractspinbox));
   qt_ns->addSystemClass(initQTableWidgetItemClass());
   qt_ns->addSystemClass(initQStyleOptionMenuItemClass(qstyleoption));
   qt_ns->addSystemClass(initQMenuBarClass(qwidget));
   qt_ns->addSystemClass((qvalidator = initQValidatorClass(QC_QObject)));
   qt_ns->addSystemClass(initQDoubleValidatorClass(qvalidator));
   qt_ns->addSystemClass(initQIntValidatorClass(qvalidator));
   qt_ns->addSystemClass(initQRegExpValidatorClass(qvalidator));
   qt_ns->addSystemClass(initQImageWriterClass());
   qt_ns->addSystemClass(initQDialClass(qabstractslider));
   qt_ns->addSystemClass(initQStackedWidgetClass(qframe));
   qt_ns->addSystemClass(initQDoubleSpinBoxClass(qabstractspinbox));
   qt_ns->addSystemClass(initQProgressBarClass(qwidget));
   qt_ns->addSystemClass(initQPainterPathClass());
   qt_ns->addSystemClass(initQPaintEngineClass());
   qt_ns->addSystemClass(initQTabBarClass(qwidget));
   qt_ns->addSystemClass(initQStyleOptionTabClass(qstyleoption));
   qt_ns->addSystemClass(initQStyleOptionTabWidgetFrameClass(qstyleoption));
   qt_ns->addSystemClass(initQTabWidgetClass(qwidget));

   qt_ns->addInitialNamespace(initQTextEditNS(qabstractscrollarea));
   qt_ns->addSystemClass(initQDesktopWidgetClass(qwidget));
   qt_ns->addSystemClass(initQWizardPageClass(qwidget));
   qt_ns->addInitialNamespace(initQListWidgetItemNS());
   qt_ns->addInitialNamespace(initQDialogButtonBoxNS(qwidget));
   qt_ns->addInitialNamespace(initQToolBarNS(qwidget));
   qdialog_ns->addSystemClass(initQProgressDialogClass(qdialog));
   qdialog_ns->addSystemClass(initQErrorMessageClass(qdialog));
   qt_ns->addSystemClass(initQStackedLayoutClass(qlayout));
   qt_ns->addInitialNamespace(initQGradientNS());
   qt_ns->addSystemClass(initQWidgetItemClass(qlayoutitem));
   qt_ns->addSystemClass(initQCursorClass());
   qt_ns->addInitialNamespace(initQStyleOptionGraphicsItemNS(qstyleoption));
   qt_ns->addInitialNamespace(initQTransformNS());
   qt_ns->addSystemClass(initQSplashScreenClass(qwidget));
   qt_ns->addSystemClass(initQSplitterClass(qwidget));
   qt_ns->addSystemClass(initQSplitterHandleClass(qwidget));
   qt_ns->addInitialNamespace(initQGraphicsItemNS());
   qt_ns->addInitialNamespace(initQTextLineNS());
   qt_ns->addInitialNamespace(initQTextOptionNS());
   qt_ns->addInitialNamespace(initQTextLayoutNS());

   // add QBoxLayout namespace and constants
   class QoreNamespace *qbl = new QoreNamespace("QBoxLayout");

   // Direction enum
   qbl->addConstant("LeftToRight",    new QoreBigIntNode(QBoxLayout::LeftToRight));
   qbl->addConstant("RightToLeft",    new QoreBigIntNode(QBoxLayout::RightToLeft));
   qbl->addConstant("TopToBottom",    new QoreBigIntNode(QBoxLayout::TopToBottom));
   qbl->addConstant("BottomToTop",    new QoreBigIntNode(QBoxLayout::BottomToTop));

   qt_ns->addInitialNamespace(qbl);

   qt_ns->addInitialNamespace(initQSystemTrayIconNS(QC_QObject));

   QoreNamespace *qdatetimeedit_ns = new QoreNamespace("QDateTimeEdit");
   
   // Section enum
   qdatetimeedit_ns->addConstant("NoSection",                new QoreBigIntNode(QDateTimeEdit::NoSection));
   qdatetimeedit_ns->addConstant("AmPmSection",              new QoreBigIntNode(QDateTimeEdit::AmPmSection));
   qdatetimeedit_ns->addConstant("MSecSection",              new QoreBigIntNode(QDateTimeEdit::MSecSection));
   qdatetimeedit_ns->addConstant("SecondSection",            new QoreBigIntNode(QDateTimeEdit::SecondSection));
   qdatetimeedit_ns->addConstant("MinuteSection",            new QoreBigIntNode(QDateTimeEdit::MinuteSection));
   qdatetimeedit_ns->addConstant("HourSection",              new QoreBigIntNode(QDateTimeEdit::HourSection));
   qdatetimeedit_ns->addConstant("DaySection",               new QoreBigIntNode(QDateTimeEdit::DaySection));
   qdatetimeedit_ns->addConstant("MonthSection",             new QoreBigIntNode(QDateTimeEdit::MonthSection));
   qdatetimeedit_ns->addConstant("YearSection",              new QoreBigIntNode(QDateTimeEdit::YearSection));
   qdatetimeedit_ns->addConstant("TimeSections_Mask",        new QoreBigIntNode(QDateTimeEdit::TimeSections_Mask));
   qdatetimeedit_ns->addConstant("DateSections_Mask",        new QoreBigIntNode(QDateTimeEdit::DateSections_Mask));

   qdatetimeedit_ns->addSystemClass((qdatetimeedit = initQDateTimeEditClass(qabstractspinbox)));
   qdatetimeedit_ns->addSystemClass(initQDateEditClass(qdatetimeedit));
   qdatetimeedit_ns->addSystemClass(initQTimeEditClass(qdatetimeedit));

   qt_ns->addInitialNamespace(qdatetimeedit_ns);

   qdialog_ns->addInitialNamespace(initQWizardNS(qdialog));

   QoreNamespace *qmessagebox = new QoreNamespace("QMessageBox");
   qmessagebox->addSystemClass(initQMessageBoxClass(qdialog));

   // Icon enum
   qmessagebox->addConstant("NoIcon",                   new QoreBigIntNode(QMessageBox::NoIcon));
   qmessagebox->addConstant("Information",              new QoreBigIntNode(QMessageBox::Information));
   qmessagebox->addConstant("Warning",                  new QoreBigIntNode(QMessageBox::Warning));
   qmessagebox->addConstant("Critical",                 new QoreBigIntNode(QMessageBox::Critical));
   qmessagebox->addConstant("Question",                 new QoreBigIntNode(QMessageBox::Question));

   // ButtonRole enum
   qmessagebox->addConstant("InvalidRole",              new QoreBigIntNode(QMessageBox::InvalidRole));
   qmessagebox->addConstant("AcceptRole",               new QoreBigIntNode(QMessageBox::AcceptRole));
   qmessagebox->addConstant("RejectRole",               new QoreBigIntNode(QMessageBox::RejectRole));
   qmessagebox->addConstant("DestructiveRole",          new QoreBigIntNode(QMessageBox::DestructiveRole));
   qmessagebox->addConstant("ActionRole",               new QoreBigIntNode(QMessageBox::ActionRole));
   qmessagebox->addConstant("HelpRole",                 new QoreBigIntNode(QMessageBox::HelpRole));
   qmessagebox->addConstant("YesRole",                  new QoreBigIntNode(QMessageBox::YesRole));
   qmessagebox->addConstant("NoRole",                   new QoreBigIntNode(QMessageBox::NoRole));
   qmessagebox->addConstant("ResetRole",                new QoreBigIntNode(QMessageBox::ResetRole));
   qmessagebox->addConstant("ApplyRole",                new QoreBigIntNode(QMessageBox::ApplyRole));

   // StandardButton enum
   qmessagebox->addConstant("NoButton",                 new QoreBigIntNode(QMessageBox::NoButton));
   qmessagebox->addConstant("Ok",                       new QoreBigIntNode(QMessageBox::Ok));
   qmessagebox->addConstant("Save",                     new QoreBigIntNode(QMessageBox::Save));
   qmessagebox->addConstant("SaveAll",                  new QoreBigIntNode(QMessageBox::SaveAll));
   qmessagebox->addConstant("Open",                     new QoreBigIntNode(QMessageBox::Open));
   qmessagebox->addConstant("Yes",                      new QoreBigIntNode(QMessageBox::Yes));
   qmessagebox->addConstant("YesToAll",                 new QoreBigIntNode(QMessageBox::YesToAll));
   qmessagebox->addConstant("No",                       new QoreBigIntNode(QMessageBox::No));
   qmessagebox->addConstant("NoToAll",                  new QoreBigIntNode(QMessageBox::NoToAll));
   qmessagebox->addConstant("Abort",                    new QoreBigIntNode(QMessageBox::Abort));
   qmessagebox->addConstant("Retry",                    new QoreBigIntNode(QMessageBox::Retry));
   qmessagebox->addConstant("Ignore",                   new QoreBigIntNode(QMessageBox::Ignore));
   qmessagebox->addConstant("Close",                    new QoreBigIntNode(QMessageBox::Close));
   qmessagebox->addConstant("Cancel",                   new QoreBigIntNode(QMessageBox::Cancel));
   qmessagebox->addConstant("Discard",                  new QoreBigIntNode(QMessageBox::Discard));
   qmessagebox->addConstant("Help",                     new QoreBigIntNode(QMessageBox::Help));
   qmessagebox->addConstant("Apply",                    new QoreBigIntNode(QMessageBox::Apply));
   qmessagebox->addConstant("Reset",                    new QoreBigIntNode(QMessageBox::Reset));
   qmessagebox->addConstant("RestoreDefaults",          new QoreBigIntNode(QMessageBox::RestoreDefaults));
   qmessagebox->addConstant("FirstButton",              new QoreBigIntNode(QMessageBox::FirstButton));
   qmessagebox->addConstant("LastButton",               new QoreBigIntNode(QMessageBox::LastButton));
   qmessagebox->addConstant("YesAll",                   new QoreBigIntNode(QMessageBox::YesAll));
   qmessagebox->addConstant("NoAll",                    new QoreBigIntNode(QMessageBox::NoAll));
   qmessagebox->addConstant("Default",                  new QoreBigIntNode(QMessageBox::Default));
   qmessagebox->addConstant("Escape",                   new QoreBigIntNode(QMessageBox::Escape));
   qmessagebox->addConstant("FlagMask",                 new QoreBigIntNode(QMessageBox::FlagMask));
   qmessagebox->addConstant("ButtonMask",               new QoreBigIntNode(QMessageBox::ButtonMask));

   qdialog_ns->addInitialNamespace(qmessagebox);

   qt_ns->addInitialNamespace(qdialog_ns);

   QoreNamespace *qprinter = new QoreNamespace("QPrinter");

   qprinter->addSystemClass(initQPrinterClass(qpaintdevice));

   // PrinterMode enum
   qprinter->addConstant("ScreenResolution",         new QoreBigIntNode(QPrinter::ScreenResolution));
   qprinter->addConstant("PrinterResolution",        new QoreBigIntNode(QPrinter::PrinterResolution));
   qprinter->addConstant("HighResolution",           new QoreBigIntNode(QPrinter::HighResolution));

   // Orientation enum
   qprinter->addConstant("Portrait",                 new QoreBigIntNode(QPrinter::Portrait));
   qprinter->addConstant("Landscape",                new QoreBigIntNode(QPrinter::Landscape));

   // PageSize enum
   qprinter->addConstant("A4",                       new QoreBigIntNode(QPrinter::A4));
   qprinter->addConstant("B5",                       new QoreBigIntNode(QPrinter::B5));
   qprinter->addConstant("Letter",                   new QoreBigIntNode(QPrinter::Letter));
   qprinter->addConstant("Legal",                    new QoreBigIntNode(QPrinter::Legal));
   qprinter->addConstant("Executive",                new QoreBigIntNode(QPrinter::Executive));
   qprinter->addConstant("A0",                       new QoreBigIntNode(QPrinter::A0));
   qprinter->addConstant("A1",                       new QoreBigIntNode(QPrinter::A1));
   qprinter->addConstant("A2",                       new QoreBigIntNode(QPrinter::A2));
   qprinter->addConstant("A3",                       new QoreBigIntNode(QPrinter::A3));
   qprinter->addConstant("A5",                       new QoreBigIntNode(QPrinter::A5));
   qprinter->addConstant("A6",                       new QoreBigIntNode(QPrinter::A6));
   qprinter->addConstant("A7",                       new QoreBigIntNode(QPrinter::A7));
   qprinter->addConstant("A8",                       new QoreBigIntNode(QPrinter::A8));
   qprinter->addConstant("A9",                       new QoreBigIntNode(QPrinter::A9));
   qprinter->addConstant("B0",                       new QoreBigIntNode(QPrinter::B0));
   qprinter->addConstant("B1",                       new QoreBigIntNode(QPrinter::B1));
   qprinter->addConstant("B10",                      new QoreBigIntNode(QPrinter::B10));
   qprinter->addConstant("B2",                       new QoreBigIntNode(QPrinter::B2));
   qprinter->addConstant("B3",                       new QoreBigIntNode(QPrinter::B3));
   qprinter->addConstant("B4",                       new QoreBigIntNode(QPrinter::B4));
   qprinter->addConstant("B6",                       new QoreBigIntNode(QPrinter::B6));
   qprinter->addConstant("B7",                       new QoreBigIntNode(QPrinter::B7));
   qprinter->addConstant("B8",                       new QoreBigIntNode(QPrinter::B8));
   qprinter->addConstant("B9",                       new QoreBigIntNode(QPrinter::B9));
   qprinter->addConstant("C5E",                      new QoreBigIntNode(QPrinter::C5E));
   qprinter->addConstant("Comm10E",                  new QoreBigIntNode(QPrinter::Comm10E));
   qprinter->addConstant("DLE",                      new QoreBigIntNode(QPrinter::DLE));
   qprinter->addConstant("Folio",                    new QoreBigIntNode(QPrinter::Folio));
   qprinter->addConstant("Ledger",                   new QoreBigIntNode(QPrinter::Ledger));
   qprinter->addConstant("Tabloid",                  new QoreBigIntNode(QPrinter::Tabloid));
   qprinter->addConstant("Custom",                   new QoreBigIntNode(QPrinter::Custom));

   // PageOrder enum
   qprinter->addConstant("FirstPageFirst",           new QoreBigIntNode(QPrinter::FirstPageFirst));
   qprinter->addConstant("LastPageFirst",            new QoreBigIntNode(QPrinter::LastPageFirst));
   
   // ColorMode enum
   qprinter->addConstant("GrayScale",                new QoreBigIntNode(QPrinter::GrayScale));
   qprinter->addConstant("Color",                    new QoreBigIntNode(QPrinter::Color));

   // PaperSource enum
   qprinter->addConstant("OnlyOne",                  new QoreBigIntNode(QPrinter::OnlyOne));
   qprinter->addConstant("Lower",                    new QoreBigIntNode(QPrinter::Lower));
   qprinter->addConstant("Middle",                   new QoreBigIntNode(QPrinter::Middle));
   qprinter->addConstant("Manual",                   new QoreBigIntNode(QPrinter::Manual));
   qprinter->addConstant("Envelope",                 new QoreBigIntNode(QPrinter::Envelope));
   qprinter->addConstant("EnvelopeManual",           new QoreBigIntNode(QPrinter::EnvelopeManual));
   qprinter->addConstant("Auto",                     new QoreBigIntNode(QPrinter::Auto));
   qprinter->addConstant("Tractor",                  new QoreBigIntNode(QPrinter::Tractor));
   qprinter->addConstant("SmallFormat",              new QoreBigIntNode(QPrinter::SmallFormat));
   qprinter->addConstant("LargeFormat",              new QoreBigIntNode(QPrinter::LargeFormat));
   qprinter->addConstant("LargeCapacity",            new QoreBigIntNode(QPrinter::LargeCapacity));
   qprinter->addConstant("Cassette",                 new QoreBigIntNode(QPrinter::Cassette));
   qprinter->addConstant("FormSource",               new QoreBigIntNode(QPrinter::FormSource));
   qprinter->addConstant("MaxPageSource",            new QoreBigIntNode(QPrinter::MaxPageSource));

   // PrinterState enum
   qprinter->addConstant("Idle",                     new QoreBigIntNode(QPrinter::Idle));
   qprinter->addConstant("Active",                   new QoreBigIntNode(QPrinter::Active));
   qprinter->addConstant("Aborted",                  new QoreBigIntNode(QPrinter::Aborted));
   qprinter->addConstant("Error",                    new QoreBigIntNode(QPrinter::Error));

   // OutputFormat enum
   qprinter->addConstant("NativeFormat",             new QoreBigIntNode(QPrinter::NativeFormat));
   qprinter->addConstant("PdfFormat",                new QoreBigIntNode(QPrinter::PdfFormat));
   qprinter->addConstant("PostScriptFormat",         new QoreBigIntNode(QPrinter::PostScriptFormat));

   // PrintRange enum
   qprinter->addConstant("AllPages",                 new QoreBigIntNode(QPrinter::AllPages));
   qprinter->addConstant("Selection",                new QoreBigIntNode(QPrinter::Selection));
   qprinter->addConstant("PageRange",                new QoreBigIntNode(QPrinter::PageRange));

   qt_ns->addInitialNamespace(qprinter);

   QoreNamespace *qlineedit = new QoreNamespace("QLineEdit");

   // EchoMode enum
   qlineedit->addConstant("Normal",                   new QoreBigIntNode(QLineEdit::Normal));
   qlineedit->addConstant("NoEcho",                   new QoreBigIntNode(QLineEdit::NoEcho));
   qlineedit->addConstant("Password",                 new QoreBigIntNode(QLineEdit::Password));
   qlineedit->addConstant("PasswordEchoOnEdit",       new QoreBigIntNode(QLineEdit::PasswordEchoOnEdit));

   qlineedit->addSystemClass(initQLineEditClass(qwidget));

   qt_ns->addInitialNamespace(qlineedit);

   QoreNamespace *qabstractitemview_ns = new QoreNamespace("QAbstractItemView");
   
   // SelectionMode enum
   qabstractitemview_ns->addConstant("NoSelection",              new QoreBigIntNode(QAbstractItemView::NoSelection));
   qabstractitemview_ns->addConstant("SingleSelection",          new QoreBigIntNode(QAbstractItemView::SingleSelection));
   qabstractitemview_ns->addConstant("MultiSelection",           new QoreBigIntNode(QAbstractItemView::MultiSelection));
   qabstractitemview_ns->addConstant("ExtendedSelection",        new QoreBigIntNode(QAbstractItemView::ExtendedSelection));
   qabstractitemview_ns->addConstant("ContiguousSelection",      new QoreBigIntNode(QAbstractItemView::ContiguousSelection));

   // SelectionBehavior enum
   qabstractitemview_ns->addConstant("SelectItems",              new QoreBigIntNode(QAbstractItemView::SelectItems));
   qabstractitemview_ns->addConstant("SelectRows",               new QoreBigIntNode(QAbstractItemView::SelectRows));
   qabstractitemview_ns->addConstant("SelectColumns",            new QoreBigIntNode(QAbstractItemView::SelectColumns));

   // ScrollHint enum
   qabstractitemview_ns->addConstant("EnsureVisible",            new QoreBigIntNode(QAbstractItemView::EnsureVisible));
   qabstractitemview_ns->addConstant("PositionAtTop",            new QoreBigIntNode(QAbstractItemView::PositionAtTop));
   qabstractitemview_ns->addConstant("PositionAtBottom",         new QoreBigIntNode(QAbstractItemView::PositionAtBottom));
   qabstractitemview_ns->addConstant("PositionAtCenter",         new QoreBigIntNode(QAbstractItemView::PositionAtCenter));

   // EditTrigger enum
   qabstractitemview_ns->addConstant("NoEditTriggers",           new QoreBigIntNode(QAbstractItemView::NoEditTriggers));
   qabstractitemview_ns->addConstant("CurrentChanged",           new QoreBigIntNode(QAbstractItemView::CurrentChanged));
   qabstractitemview_ns->addConstant("DoubleClicked",            new QoreBigIntNode(QAbstractItemView::DoubleClicked));
   qabstractitemview_ns->addConstant("SelectedClicked",          new QoreBigIntNode(QAbstractItemView::SelectedClicked));
   qabstractitemview_ns->addConstant("EditKeyPressed",           new QoreBigIntNode(QAbstractItemView::EditKeyPressed));
   qabstractitemview_ns->addConstant("AnyKeyPressed",            new QoreBigIntNode(QAbstractItemView::AnyKeyPressed));
   qabstractitemview_ns->addConstant("AllEditTriggers",          new QoreBigIntNode(QAbstractItemView::AllEditTriggers));

   // ScrollMode enum
   qabstractitemview_ns->addConstant("ScrollPerItem",            new QoreBigIntNode(QAbstractItemView::ScrollPerItem));
   qabstractitemview_ns->addConstant("ScrollPerPixel",           new QoreBigIntNode(QAbstractItemView::ScrollPerPixel));

   qabstractitemview_ns->addSystemClass((qabstractitemview = initQAbstractItemViewClass(qabstractscrollarea)));
   qabstractitemview_ns->addSystemClass((qtableview = initQTableViewClass(qabstractitemview)));
   qabstractitemview_ns->addSystemClass(initQTableWidgetClass(qtableview));

   qabstractitemview_ns->addInitialNamespace(initQListViewNS(qabstractitemview));
   
   qt_ns->addInitialNamespace(qabstractitemview_ns);

   QoreNamespace *qheaderview = new QoreNamespace("QHeaderView");

   // ResizeMode enum
   qheaderview->addConstant("Interactive",              new QoreBigIntNode(QHeaderView::Interactive));
   qheaderview->addConstant("Stretch",                  new QoreBigIntNode(QHeaderView::Stretch));
   qheaderview->addConstant("Fixed",                    new QoreBigIntNode(QHeaderView::Fixed));
   qheaderview->addConstant("ResizeToContents",         new QoreBigIntNode(QHeaderView::ResizeToContents));
   qheaderview->addConstant("Custom",                   new QoreBigIntNode(QHeaderView::Custom));

   qheaderview->addSystemClass(initQHeaderViewClass(qabstractitemview));

   qt_ns->addInitialNamespace(qheaderview);

   // add event class hierarchy in QtGui
   QoreClass *qinputevent, *qdropevent, *qdragmoveevent;
   ns->addSystemClass(initQPaintEventClass(QC_QEvent));
   ns->addSystemClass(initQMoveEventClass(QC_QEvent));
   ns->addSystemClass(initQResizeEventClass(QC_QEvent));

   ns->addSystemClass((qinputevent = initQInputEventClass(QC_QEvent)));
   ns->addSystemClass(initQKeyEventClass(qinputevent));
   ns->addSystemClass(initQMouseEventClass(qinputevent));
   ns->addSystemClass(initQContextMenuEventClass(qinputevent));
   ns->addSystemClass(initQTabletEventClass(qinputevent));
   ns->addSystemClass(initQWheelEventClass(qinputevent));

   ns->addSystemClass(initQActionEventClass(QC_QEvent));
   ns->addSystemClass(initQCloseEventClass(QC_QEvent));

   ns->addSystemClass((qdropevent = initQDropEventClass(QC_QEvent)));
   ns->addSystemClass((qdragmoveevent = initQDragMoveEventClass(qdropevent)));
   ns->addSystemClass(initQDragEnterEventClass(qdragmoveevent));

   ns->addSystemClass(initQDragLeaveEventClass(QC_QEvent));
   ns->addSystemClass(initQFocusEventClass(QC_QEvent));
   ns->addSystemClass(initQHideEventClass(QC_QEvent));
   ns->addSystemClass(initQInputMethodEventClass(QC_QEvent));
   ns->addSystemClass(initQShowEventClass(QC_QEvent));
   ns->addSystemClass(initQHelpEventClass(QC_QEvent));

   QoreClass *qgraphicssceneevent;
   ns->addSystemClass((qgraphicssceneevent = initQGraphicsSceneEventClass(QC_QEvent)));
   ns->addInitialNamespace(initQGraphicsSceneContextMenuEventNS(qgraphicssceneevent));
   ns->addSystemClass(initQGraphicsSceneDragDropEventClass(qgraphicssceneevent));
   ns->addSystemClass(initQGraphicsSceneHelpEventClass(qgraphicssceneevent));
   ns->addSystemClass(initQGraphicsSceneHoverEventClass(qgraphicssceneevent));
   ns->addSystemClass(initQGraphicsSceneMouseEventClass(qgraphicssceneevent));
   ns->addSystemClass(initQGraphicsSceneWheelEventClass(qgraphicssceneevent));

   QoreNamespace *qclipboard = new QoreNamespace("QClipboard");
   
   // Mode enum
   qclipboard->addConstant("Clipboard",                new QoreBigIntNode(QClipboard::Clipboard));
   qclipboard->addConstant("Selection",                new QoreBigIntNode(QClipboard::Selection));
   qclipboard->addConstant("FindBuffer",               new QoreBigIntNode(QClipboard::FindBuffer));
   qclipboard->addConstant("LastMode",                 new QoreBigIntNode(QClipboard::LastMode));

   qclipboard->addSystemClass(initQClipboardClass(QC_QObject));

   qt_ns->addInitialNamespace(qclipboard);

   QoreNamespace *qcalendarwidget = new QoreNamespace("QCalendarWidget");

   qcalendarwidget->addSystemClass(initQCalendarWidgetClass(qwidget));

   // SelectionMode enum
   qcalendarwidget->addConstant("NoSelection",              new QoreBigIntNode(QCalendarWidget::NoSelection));
   qcalendarwidget->addConstant("SingleSelection",          new QoreBigIntNode(QCalendarWidget::SingleSelection));

   // HorizontalHeaderFormat enum
   qcalendarwidget->addConstant("NoHorizontalHeader",       new QoreBigIntNode(QCalendarWidget::NoHorizontalHeader));
   qcalendarwidget->addConstant("SingleLetterDayNames",     new QoreBigIntNode(QCalendarWidget::SingleLetterDayNames));
   qcalendarwidget->addConstant("ShortDayNames",            new QoreBigIntNode(QCalendarWidget::ShortDayNames));
   qcalendarwidget->addConstant("LongDayNames",             new QoreBigIntNode(QCalendarWidget::LongDayNames));

   // VeritcalHeaderFormat enum
   qcalendarwidget->addConstant("NoVerticalHeader",         new QoreBigIntNode(QCalendarWidget::NoVerticalHeader));
   qcalendarwidget->addConstant("ISOWeekNumbers",           new QoreBigIntNode(QCalendarWidget::ISOWeekNumbers));

   qt_ns->addInitialNamespace(qcalendarwidget);

   // add QFont namespaces and constants
   QoreNamespace *qframens = new QoreNamespace("QFrame");
   // Shadow enum
   qframens->addConstant("Plain",    new QoreBigIntNode(QFrame::Plain));
   qframens->addConstant("Raised",   new QoreBigIntNode(QFrame::Raised));
   qframens->addConstant("Sunken",   new QoreBigIntNode(QFrame::Sunken));

   // Shape enum
   qframens->addConstant("NoFrame",      new QoreBigIntNode(QFrame::NoFrame));
   qframens->addConstant("Box",          new QoreBigIntNode(QFrame::Box));
   qframens->addConstant("Panel",        new QoreBigIntNode(QFrame::Panel));
   qframens->addConstant("StyledPanel",  new QoreBigIntNode(QFrame::StyledPanel));
   qframens->addConstant("HLine",        new QoreBigIntNode(QFrame::HLine));
   qframens->addConstant("VLine",        new QoreBigIntNode(QFrame::VLine));
   qframens->addConstant("WinPanel",     new QoreBigIntNode(QFrame::WinPanel));

   // StyleMask
   qframens->addConstant("Shadow_Mask",  new QoreBigIntNode(QFrame::Shadow_Mask));
   qframens->addConstant("Shape_Mask",   new QoreBigIntNode(QFrame::Shape_Mask));

   qt_ns->addInitialNamespace(qframens);

   // add QFont namespaces and constants
   class QoreNamespace *qf = new QoreNamespace("QFont");
   // Weight enum
   qf->addConstant("Light",    new QoreBigIntNode(QFont::Light));
   qf->addConstant("Normal",   new QoreBigIntNode(QFont::Normal));
   qf->addConstant("DemiBold", new QoreBigIntNode(QFont::DemiBold));
   qf->addConstant("Bold",     new QoreBigIntNode(QFont::Bold));
   qf->addConstant("Black",    new QoreBigIntNode(QFont::Black));

   // StyleHint enum
   qf->addConstant("Helvetica",    new QoreBigIntNode(QFont::Helvetica));
   qf->addConstant("SansSerif",    new QoreBigIntNode(QFont::SansSerif));
   qf->addConstant("Times",        new QoreBigIntNode(QFont::Times));
   qf->addConstant("Serif",        new QoreBigIntNode(QFont::Serif));
   qf->addConstant("Courier",      new QoreBigIntNode(QFont::Courier));
   qf->addConstant("TypeWriter",   new QoreBigIntNode(QFont::TypeWriter));
   qf->addConstant("OldEnglish",   new QoreBigIntNode(QFont::OldEnglish));
   qf->addConstant("Decorative",   new QoreBigIntNode(QFont::Decorative));
   qf->addConstant("System",       new QoreBigIntNode(QFont::System));
   qf->addConstant("AnyStyle",     new QoreBigIntNode(QFont::AnyStyle));

   // StyleStrategy
   qf->addConstant("PreferDefault",    new QoreBigIntNode(QFont::PreferDefault));
   qf->addConstant("PreferBitmap",     new QoreBigIntNode(QFont::PreferBitmap));
   qf->addConstant("PreferDevice",     new QoreBigIntNode(QFont::PreferDevice));
   qf->addConstant("PreferOutline",    new QoreBigIntNode(QFont::PreferOutline));
   qf->addConstant("ForceOutline",     new QoreBigIntNode(QFont::ForceOutline));
   qf->addConstant("PreferMatch",      new QoreBigIntNode(QFont::PreferMatch));
   qf->addConstant("PreferQuality",    new QoreBigIntNode(QFont::PreferQuality));
   qf->addConstant("PreferAntialias",  new QoreBigIntNode(QFont::PreferAntialias));
   qf->addConstant("NoAntialias",      new QoreBigIntNode(QFont::NoAntialias));
   qf->addConstant("OpenGLCompatible", new QoreBigIntNode(QFont::OpenGLCompatible));
   qf->addConstant("NoFontMerging",    new QoreBigIntNode(QFont::NoFontMerging));

   // Style enum
   qf->addConstant("StyleNormal",   new QoreBigIntNode(QFont::StyleNormal));
   qf->addConstant("StyleItalic",   new QoreBigIntNode(QFont::StyleItalic));
   qf->addConstant("StyleOblique",  new QoreBigIntNode(QFont::StyleOblique));

   // Stretch enum
   qf->addConstant("UltraCondensed",  new QoreBigIntNode(QFont::UltraCondensed));
   qf->addConstant("ExtraCondensed",  new QoreBigIntNode(QFont::ExtraCondensed));
   qf->addConstant("Condensed",       new QoreBigIntNode(QFont::Condensed));
   qf->addConstant("SemiCondensed",   new QoreBigIntNode(QFont::SemiCondensed));
   qf->addConstant("Unstretched",     new QoreBigIntNode(QFont::Unstretched));
   qf->addConstant("SemiExpanded",    new QoreBigIntNode(QFont::SemiExpanded));
   qf->addConstant("Expanded",        new QoreBigIntNode(QFont::Expanded));
   qf->addConstant("ExtraExpanded",   new QoreBigIntNode(QFont::ExtraExpanded));
   qf->addConstant("UltraExpanded",   new QoreBigIntNode(QFont::UltraExpanded));

   qt_ns->addInitialNamespace(qf);

   // add QLCDNumber namespace and constants
   class QoreNamespace *qlcdn = new QoreNamespace("QLCDNumber");
   qlcdn->addConstant("Outline",   new QoreBigIntNode(QLCDNumber::Outline));
   qlcdn->addConstant("Filled",    new QoreBigIntNode(QLCDNumber::Filled));
   qlcdn->addConstant("Flat",      new QoreBigIntNode(QLCDNumber::Flat));
   qlcdn->addConstant("Hex",       new QoreBigIntNode(QLCDNumber::Hex));
   qlcdn->addConstant("Dec",       new QoreBigIntNode(QLCDNumber::Dec));
   qlcdn->addConstant("Oct",       new QoreBigIntNode(QLCDNumber::Oct));
   qlcdn->addConstant("Bin",       new QoreBigIntNode(QLCDNumber::Bin));
   qt_ns->addInitialNamespace(qlcdn);

   // add QAbstractSlider namespace and constants
   class QoreNamespace *qas = new QoreNamespace("QAbstractSlider");
   qas->addConstant("SliderNoAction",        new QoreBigIntNode(QAbstractSlider::SliderNoAction));
   qas->addConstant("SliderSingleStepAdd",   new QoreBigIntNode(QAbstractSlider::SliderSingleStepAdd));
   qas->addConstant("SliderSingleStepSub",   new QoreBigIntNode(QAbstractSlider::SliderSingleStepSub));
   qas->addConstant("SliderPageStepAdd",     new QoreBigIntNode(QAbstractSlider::SliderPageStepAdd));
   qas->addConstant("SliderPageStepSub",     new QoreBigIntNode(QAbstractSlider::SliderPageStepSub));
   qas->addConstant("SliderToMinimum",       new QoreBigIntNode(QAbstractSlider::SliderToMinimum));
   qas->addConstant("SliderToMaximum",       new QoreBigIntNode(QAbstractSlider::SliderToMaximum));
   qas->addConstant("SliderMove",            new QoreBigIntNode(QAbstractSlider::SliderMove));
   qt_ns->addInitialNamespace(qas);

   // CheckState enum
   qt_ns->addConstant("Unchecked",                new QoreBigIntNode(Qt::Unchecked));
   qt_ns->addConstant("PartiallyChecked",         new QoreBigIntNode(Qt::PartiallyChecked));
   qt_ns->addConstant("Checked",                  new QoreBigIntNode(Qt::Checked));

   // orientation enum values
   qt_ns->addConstant("Vertical",        new QoreBigIntNode(Qt::Vertical));
   qt_ns->addConstant("Horizontal",      new QoreBigIntNode(Qt::Horizontal));

   // GlobalColor enum
   qt_ns->addConstant("color0",            new QoreBigIntNode(Qt::color0));
   qt_ns->addConstant("color1",            new QoreBigIntNode(Qt::color1));
   qt_ns->addConstant("black",             new QoreBigIntNode(Qt::black));
   qt_ns->addConstant("white",             new QoreBigIntNode(Qt::white));
   qt_ns->addConstant("darkGray",          new QoreBigIntNode(Qt::darkGray));
   qt_ns->addConstant("gray",              new QoreBigIntNode(Qt::gray));
   qt_ns->addConstant("lightGray",         new QoreBigIntNode(Qt::lightGray));
   qt_ns->addConstant("red",               new QoreBigIntNode(Qt::red));
   qt_ns->addConstant("green",             new QoreBigIntNode(Qt::green));
   qt_ns->addConstant("blue",              new QoreBigIntNode(Qt::blue));
   qt_ns->addConstant("cyan",              new QoreBigIntNode(Qt::cyan));
   qt_ns->addConstant("magenta",           new QoreBigIntNode(Qt::magenta));
   qt_ns->addConstant("yellow",            new QoreBigIntNode(Qt::yellow));
   qt_ns->addConstant("darkRed",           new QoreBigIntNode(Qt::darkRed));
   qt_ns->addConstant("darkGreen",         new QoreBigIntNode(Qt::darkGreen));
   qt_ns->addConstant("darkBlue",          new QoreBigIntNode(Qt::darkBlue));
   qt_ns->addConstant("darkCyan",          new QoreBigIntNode(Qt::darkCyan));
   qt_ns->addConstant("darkMagenta",       new QoreBigIntNode(Qt::darkMagenta));
   qt_ns->addConstant("darkYellow",        new QoreBigIntNode(Qt::darkYellow));
   qt_ns->addConstant("transparent",       new QoreBigIntNode(Qt::transparent));

   // BrushStyle enum
   qt_ns->addConstant("NoBrush",                  new BrushStyleNode(Qt::NoBrush));
   qt_ns->addConstant("SolidPattern",             new BrushStyleNode(Qt::SolidPattern));
   qt_ns->addConstant("Dense1Pattern",            new BrushStyleNode(Qt::Dense1Pattern));
   qt_ns->addConstant("Dense2Pattern",            new BrushStyleNode(Qt::Dense2Pattern));
   qt_ns->addConstant("Dense3Pattern",            new BrushStyleNode(Qt::Dense3Pattern));
   qt_ns->addConstant("Dense4Pattern",            new BrushStyleNode(Qt::Dense4Pattern));
   qt_ns->addConstant("Dense5Pattern",            new BrushStyleNode(Qt::Dense5Pattern));
   qt_ns->addConstant("Dense6Pattern",            new BrushStyleNode(Qt::Dense6Pattern));
   qt_ns->addConstant("Dense7Pattern",            new BrushStyleNode(Qt::Dense7Pattern));
   qt_ns->addConstant("HorPattern",               new BrushStyleNode(Qt::HorPattern));
   qt_ns->addConstant("VerPattern",               new BrushStyleNode(Qt::VerPattern));
   qt_ns->addConstant("CrossPattern",             new BrushStyleNode(Qt::CrossPattern));
   qt_ns->addConstant("BDiagPattern",             new BrushStyleNode(Qt::BDiagPattern));
   qt_ns->addConstant("FDiagPattern",             new BrushStyleNode(Qt::FDiagPattern));
   qt_ns->addConstant("DiagCrossPattern",         new BrushStyleNode(Qt::DiagCrossPattern));
   qt_ns->addConstant("LinearGradientPattern",    new BrushStyleNode(Qt::LinearGradientPattern));
   qt_ns->addConstant("RadialGradientPattern",    new BrushStyleNode(Qt::RadialGradientPattern));
   qt_ns->addConstant("ConicalGradientPattern",   new BrushStyleNode(Qt::ConicalGradientPattern));
   qt_ns->addConstant("TexturePattern",           new BrushStyleNode(Qt::TexturePattern));

   // PenStyle enum
   qt_ns->addConstant("NoPen",             new PenStyleNode(Qt::NoPen));
   qt_ns->addConstant("SolidLine",         new PenStyleNode(Qt::SolidLine));
   qt_ns->addConstant("DashLine",          new PenStyleNode(Qt::DashLine));
   qt_ns->addConstant("DotLine",           new PenStyleNode(Qt::DotLine));
   qt_ns->addConstant("DashDotLine",       new PenStyleNode(Qt::DashDotLine));
   qt_ns->addConstant("DashDotDotLine",    new PenStyleNode(Qt::DashDotDotLine));
   qt_ns->addConstant("CustomDashLine",    new PenStyleNode(Qt::CustomDashLine));

   // AlignmentFlag enum
   qt_ns->addConstant("AlignLeft",                new QoreBigIntNode(Qt::AlignLeft));
   qt_ns->addConstant("AlignLeading",             new QoreBigIntNode(Qt::AlignLeading));
   qt_ns->addConstant("AlignRight",               new QoreBigIntNode(Qt::AlignRight));
   qt_ns->addConstant("AlignTrailing",            new QoreBigIntNode(Qt::AlignTrailing));
   qt_ns->addConstant("AlignHCenter",             new QoreBigIntNode(Qt::AlignHCenter));
   qt_ns->addConstant("AlignJustify",             new QoreBigIntNode(Qt::AlignJustify));
   qt_ns->addConstant("AlignAbsolute",            new QoreBigIntNode(Qt::AlignAbsolute));
   qt_ns->addConstant("AlignHorizontal_Mask",     new QoreBigIntNode(Qt::AlignHorizontal_Mask));
   qt_ns->addConstant("AlignTop",                 new QoreBigIntNode(Qt::AlignTop));
   qt_ns->addConstant("AlignBottom",              new QoreBigIntNode(Qt::AlignBottom));
   qt_ns->addConstant("AlignVCenter",             new QoreBigIntNode(Qt::AlignVCenter));
   qt_ns->addConstant("AlignVertical_Mask",       new QoreBigIntNode(Qt::AlignVertical_Mask));
   qt_ns->addConstant("AlignCenter",              new QoreBigIntNode(Qt::AlignCenter));

   // MouseButton enum
   qt_ns->addConstant("NoButton",                 new QoreBigIntNode(Qt::NoButton));
   qt_ns->addConstant("LeftButton",               new QoreBigIntNode(Qt::LeftButton));
   qt_ns->addConstant("RightButton",              new QoreBigIntNode(Qt::RightButton));
   qt_ns->addConstant("MidButton",                new QoreBigIntNode(Qt::MidButton));
   qt_ns->addConstant("XButton1",                 new QoreBigIntNode(Qt::XButton1));
   qt_ns->addConstant("XButton2",                 new QoreBigIntNode(Qt::XButton2));
   qt_ns->addConstant("MouseButtonMask",          new QoreBigIntNode(Qt::MouseButtonMask));

   // Modifier enum
   qt_ns->addConstant("META",                     new QoreBigIntNode(Qt::META));
   qt_ns->addConstant("SHIFT",                    new QoreBigIntNode(Qt::SHIFT));
   qt_ns->addConstant("CTRL",                     new QoreBigIntNode(Qt::CTRL));
   qt_ns->addConstant("ALT",                      new QoreBigIntNode(Qt::ALT));
   qt_ns->addConstant("MODIFIER_MASK",            new QoreBigIntNode(Qt::MODIFIER_MASK));
   qt_ns->addConstant("UNICODE_ACCEL",            new QoreBigIntNode(Qt::UNICODE_ACCEL));

   // DayOfWeek
   qt_ns->addConstant("Monday",                   new QoreBigIntNode(Qt::Monday));
   qt_ns->addConstant("Tuesday",                  new QoreBigIntNode(Qt::Tuesday));
   qt_ns->addConstant("Wednesday",                new QoreBigIntNode(Qt::Wednesday));
   qt_ns->addConstant("Thursday",                 new QoreBigIntNode(Qt::Thursday));
   qt_ns->addConstant("Friday",                   new QoreBigIntNode(Qt::Friday));
   qt_ns->addConstant("Saturday",                 new QoreBigIntNode(Qt::Saturday));
   qt_ns->addConstant("Sunday",                   new QoreBigIntNode(Qt::Sunday));

   // ContextMenuPolicy enum
   qt_ns->addConstant("NoContextMenu",            new QoreBigIntNode(Qt::NoContextMenu));
   qt_ns->addConstant("DefaultContextMenu",       new QoreBigIntNode(Qt::DefaultContextMenu));
   qt_ns->addConstant("ActionsContextMenu",       new QoreBigIntNode(Qt::ActionsContextMenu));
   qt_ns->addConstant("CustomContextMenu",        new QoreBigIntNode(Qt::CustomContextMenu));
   qt_ns->addConstant("PreventContextMenu",       new QoreBigIntNode(Qt::PreventContextMenu));

   // Key enum
   qt_ns->addConstant("Key_Escape",               new QoreBigIntNode(Qt::Key_Escape));
   qt_ns->addConstant("Key_Tab",                  new QoreBigIntNode(Qt::Key_Tab));
   qt_ns->addConstant("Key_Backtab",              new QoreBigIntNode(Qt::Key_Backtab));
   qt_ns->addConstant("Key_Backspace",            new QoreBigIntNode(Qt::Key_Backspace));
   qt_ns->addConstant("Key_Return",               new QoreBigIntNode(Qt::Key_Return));
   qt_ns->addConstant("Key_Enter",                new QoreBigIntNode(Qt::Key_Enter));
   qt_ns->addConstant("Key_Insert",               new QoreBigIntNode(Qt::Key_Insert));
   qt_ns->addConstant("Key_Delete",               new QoreBigIntNode(Qt::Key_Delete));
   qt_ns->addConstant("Key_Pause",                new QoreBigIntNode(Qt::Key_Pause));
   qt_ns->addConstant("Key_Print",                new QoreBigIntNode(Qt::Key_Print));
   qt_ns->addConstant("Key_SysReq",               new QoreBigIntNode(Qt::Key_SysReq));
   qt_ns->addConstant("Key_Clear",                new QoreBigIntNode(Qt::Key_Clear));
   qt_ns->addConstant("Key_Home",                 new QoreBigIntNode(Qt::Key_Home));
   qt_ns->addConstant("Key_End",                  new QoreBigIntNode(Qt::Key_End));
   qt_ns->addConstant("Key_Left",                 new QoreBigIntNode(Qt::Key_Left));
   qt_ns->addConstant("Key_Up",                   new QoreBigIntNode(Qt::Key_Up));
   qt_ns->addConstant("Key_Right",                new QoreBigIntNode(Qt::Key_Right));
   qt_ns->addConstant("Key_Down",                 new QoreBigIntNode(Qt::Key_Down));
   qt_ns->addConstant("Key_PageUp",               new QoreBigIntNode(Qt::Key_PageUp));
   qt_ns->addConstant("Key_PageDown",             new QoreBigIntNode(Qt::Key_PageDown));
   qt_ns->addConstant("Key_Shift",                new QoreBigIntNode(Qt::Key_Shift));
   qt_ns->addConstant("Key_Control",              new QoreBigIntNode(Qt::Key_Control));
   qt_ns->addConstant("Key_Meta",                 new QoreBigIntNode(Qt::Key_Meta));
   qt_ns->addConstant("Key_Alt",                  new QoreBigIntNode(Qt::Key_Alt));
   qt_ns->addConstant("Key_CapsLock",             new QoreBigIntNode(Qt::Key_CapsLock));
   qt_ns->addConstant("Key_NumLock",              new QoreBigIntNode(Qt::Key_NumLock));
   qt_ns->addConstant("Key_ScrollLock",           new QoreBigIntNode(Qt::Key_ScrollLock));
   qt_ns->addConstant("Key_F1",                   new QoreBigIntNode(Qt::Key_F1));
   qt_ns->addConstant("Key_F2",                   new QoreBigIntNode(Qt::Key_F2));
   qt_ns->addConstant("Key_F3",                   new QoreBigIntNode(Qt::Key_F3));
   qt_ns->addConstant("Key_F4",                   new QoreBigIntNode(Qt::Key_F4));
   qt_ns->addConstant("Key_F5",                   new QoreBigIntNode(Qt::Key_F5));
   qt_ns->addConstant("Key_F6",                   new QoreBigIntNode(Qt::Key_F6));
   qt_ns->addConstant("Key_F7",                   new QoreBigIntNode(Qt::Key_F7));
   qt_ns->addConstant("Key_F8",                   new QoreBigIntNode(Qt::Key_F8));
   qt_ns->addConstant("Key_F9",                   new QoreBigIntNode(Qt::Key_F9));
   qt_ns->addConstant("Key_F10",                  new QoreBigIntNode(Qt::Key_F10));
   qt_ns->addConstant("Key_F11",                  new QoreBigIntNode(Qt::Key_F11));
   qt_ns->addConstant("Key_F12",                  new QoreBigIntNode(Qt::Key_F12));
   qt_ns->addConstant("Key_F13",                  new QoreBigIntNode(Qt::Key_F13));
   qt_ns->addConstant("Key_F14",                  new QoreBigIntNode(Qt::Key_F14));
   qt_ns->addConstant("Key_F15",                  new QoreBigIntNode(Qt::Key_F15));
   qt_ns->addConstant("Key_F16",                  new QoreBigIntNode(Qt::Key_F16));
   qt_ns->addConstant("Key_F17",                  new QoreBigIntNode(Qt::Key_F17));
   qt_ns->addConstant("Key_F18",                  new QoreBigIntNode(Qt::Key_F18));
   qt_ns->addConstant("Key_F19",                  new QoreBigIntNode(Qt::Key_F19));
   qt_ns->addConstant("Key_F20",                  new QoreBigIntNode(Qt::Key_F20));
   qt_ns->addConstant("Key_F21",                  new QoreBigIntNode(Qt::Key_F21));
   qt_ns->addConstant("Key_F22",                  new QoreBigIntNode(Qt::Key_F22));
   qt_ns->addConstant("Key_F23",                  new QoreBigIntNode(Qt::Key_F23));
   qt_ns->addConstant("Key_F24",                  new QoreBigIntNode(Qt::Key_F24));
   qt_ns->addConstant("Key_F25",                  new QoreBigIntNode(Qt::Key_F25));
   qt_ns->addConstant("Key_F26",                  new QoreBigIntNode(Qt::Key_F26));
   qt_ns->addConstant("Key_F27",                  new QoreBigIntNode(Qt::Key_F27));
   qt_ns->addConstant("Key_F28",                  new QoreBigIntNode(Qt::Key_F28));
   qt_ns->addConstant("Key_F29",                  new QoreBigIntNode(Qt::Key_F29));
   qt_ns->addConstant("Key_F30",                  new QoreBigIntNode(Qt::Key_F30));
   qt_ns->addConstant("Key_F31",                  new QoreBigIntNode(Qt::Key_F31));
   qt_ns->addConstant("Key_F32",                  new QoreBigIntNode(Qt::Key_F32));
   qt_ns->addConstant("Key_F33",                  new QoreBigIntNode(Qt::Key_F33));
   qt_ns->addConstant("Key_F34",                  new QoreBigIntNode(Qt::Key_F34));
   qt_ns->addConstant("Key_F35",                  new QoreBigIntNode(Qt::Key_F35));
   qt_ns->addConstant("Key_Super_L",              new QoreBigIntNode(Qt::Key_Super_L));
   qt_ns->addConstant("Key_Super_R",              new QoreBigIntNode(Qt::Key_Super_R));
   qt_ns->addConstant("Key_Menu",                 new QoreBigIntNode(Qt::Key_Menu));
   qt_ns->addConstant("Key_Hyper_L",              new QoreBigIntNode(Qt::Key_Hyper_L));
   qt_ns->addConstant("Key_Hyper_R",              new QoreBigIntNode(Qt::Key_Hyper_R));
   qt_ns->addConstant("Key_Help",                 new QoreBigIntNode(Qt::Key_Help));
   qt_ns->addConstant("Key_Direction_L",          new QoreBigIntNode(Qt::Key_Direction_L));
   qt_ns->addConstant("Key_Direction_R",          new QoreBigIntNode(Qt::Key_Direction_R));
   qt_ns->addConstant("Key_Space",                new QoreBigIntNode(Qt::Key_Space));
   qt_ns->addConstant("Key_Any",                  new QoreBigIntNode(Qt::Key_Any));
   qt_ns->addConstant("Key_Exclam",               new QoreBigIntNode(Qt::Key_Exclam));
   qt_ns->addConstant("Key_QuoteDbl",             new QoreBigIntNode(Qt::Key_QuoteDbl));
   qt_ns->addConstant("Key_NumberSign",           new QoreBigIntNode(Qt::Key_NumberSign));
   qt_ns->addConstant("Key_Dollar",               new QoreBigIntNode(Qt::Key_Dollar));
   qt_ns->addConstant("Key_Percent",              new QoreBigIntNode(Qt::Key_Percent));
   qt_ns->addConstant("Key_Ampersand",            new QoreBigIntNode(Qt::Key_Ampersand));
   qt_ns->addConstant("Key_Apostrophe",           new QoreBigIntNode(Qt::Key_Apostrophe));
   qt_ns->addConstant("Key_ParenLeft",            new QoreBigIntNode(Qt::Key_ParenLeft));
   qt_ns->addConstant("Key_ParenRight",           new QoreBigIntNode(Qt::Key_ParenRight));
   qt_ns->addConstant("Key_Asterisk",             new QoreBigIntNode(Qt::Key_Asterisk));
   qt_ns->addConstant("Key_Plus",                 new QoreBigIntNode(Qt::Key_Plus));
   qt_ns->addConstant("Key_Comma",                new QoreBigIntNode(Qt::Key_Comma));
   qt_ns->addConstant("Key_Minus",                new QoreBigIntNode(Qt::Key_Minus));
   qt_ns->addConstant("Key_Period",               new QoreBigIntNode(Qt::Key_Period));
   qt_ns->addConstant("Key_Slash",                new QoreBigIntNode(Qt::Key_Slash));
   qt_ns->addConstant("Key_0",                    new QoreBigIntNode(Qt::Key_0));
   qt_ns->addConstant("Key_1",                    new QoreBigIntNode(Qt::Key_1));
   qt_ns->addConstant("Key_2",                    new QoreBigIntNode(Qt::Key_2));
   qt_ns->addConstant("Key_3",                    new QoreBigIntNode(Qt::Key_3));
   qt_ns->addConstant("Key_4",                    new QoreBigIntNode(Qt::Key_4));
   qt_ns->addConstant("Key_5",                    new QoreBigIntNode(Qt::Key_5));
   qt_ns->addConstant("Key_6",                    new QoreBigIntNode(Qt::Key_6));
   qt_ns->addConstant("Key_7",                    new QoreBigIntNode(Qt::Key_7));
   qt_ns->addConstant("Key_8",                    new QoreBigIntNode(Qt::Key_8));
   qt_ns->addConstant("Key_9",                    new QoreBigIntNode(Qt::Key_9));
   qt_ns->addConstant("Key_Colon",                new QoreBigIntNode(Qt::Key_Colon));
   qt_ns->addConstant("Key_Semicolon",            new QoreBigIntNode(Qt::Key_Semicolon));
   qt_ns->addConstant("Key_Less",                 new QoreBigIntNode(Qt::Key_Less));
   qt_ns->addConstant("Key_Equal",                new QoreBigIntNode(Qt::Key_Equal));
   qt_ns->addConstant("Key_Greater",              new QoreBigIntNode(Qt::Key_Greater));
   qt_ns->addConstant("Key_Question",             new QoreBigIntNode(Qt::Key_Question));
   qt_ns->addConstant("Key_At",                   new QoreBigIntNode(Qt::Key_At));
   qt_ns->addConstant("Key_A",                    new QoreBigIntNode(Qt::Key_A));
   qt_ns->addConstant("Key_B",                    new QoreBigIntNode(Qt::Key_B));
   qt_ns->addConstant("Key_C",                    new QoreBigIntNode(Qt::Key_C));
   qt_ns->addConstant("Key_D",                    new QoreBigIntNode(Qt::Key_D));
   qt_ns->addConstant("Key_E",                    new QoreBigIntNode(Qt::Key_E));
   qt_ns->addConstant("Key_F",                    new QoreBigIntNode(Qt::Key_F));
   qt_ns->addConstant("Key_G",                    new QoreBigIntNode(Qt::Key_G));
   qt_ns->addConstant("Key_H",                    new QoreBigIntNode(Qt::Key_H));
   qt_ns->addConstant("Key_I",                    new QoreBigIntNode(Qt::Key_I));
   qt_ns->addConstant("Key_J",                    new QoreBigIntNode(Qt::Key_J));
   qt_ns->addConstant("Key_K",                    new QoreBigIntNode(Qt::Key_K));
   qt_ns->addConstant("Key_L",                    new QoreBigIntNode(Qt::Key_L));
   qt_ns->addConstant("Key_M",                    new QoreBigIntNode(Qt::Key_M));
   qt_ns->addConstant("Key_N",                    new QoreBigIntNode(Qt::Key_N));
   qt_ns->addConstant("Key_O",                    new QoreBigIntNode(Qt::Key_O));
   qt_ns->addConstant("Key_P",                    new QoreBigIntNode(Qt::Key_P));
   qt_ns->addConstant("Key_Q",                    new QoreBigIntNode(Qt::Key_Q));
   qt_ns->addConstant("Key_R",                    new QoreBigIntNode(Qt::Key_R));
   qt_ns->addConstant("Key_S",                    new QoreBigIntNode(Qt::Key_S));
   qt_ns->addConstant("Key_T",                    new QoreBigIntNode(Qt::Key_T));
   qt_ns->addConstant("Key_U",                    new QoreBigIntNode(Qt::Key_U));
   qt_ns->addConstant("Key_V",                    new QoreBigIntNode(Qt::Key_V));
   qt_ns->addConstant("Key_W",                    new QoreBigIntNode(Qt::Key_W));
   qt_ns->addConstant("Key_X",                    new QoreBigIntNode(Qt::Key_X));
   qt_ns->addConstant("Key_Y",                    new QoreBigIntNode(Qt::Key_Y));
   qt_ns->addConstant("Key_Z",                    new QoreBigIntNode(Qt::Key_Z));
   qt_ns->addConstant("Key_BracketLeft",          new QoreBigIntNode(Qt::Key_BracketLeft));
   qt_ns->addConstant("Key_Backslash",            new QoreBigIntNode(Qt::Key_Backslash));
   qt_ns->addConstant("Key_BracketRight",         new QoreBigIntNode(Qt::Key_BracketRight));
   qt_ns->addConstant("Key_AsciiCircum",          new QoreBigIntNode(Qt::Key_AsciiCircum));
   qt_ns->addConstant("Key_Underscore",           new QoreBigIntNode(Qt::Key_Underscore));
   qt_ns->addConstant("Key_QuoteLeft",            new QoreBigIntNode(Qt::Key_QuoteLeft));
   qt_ns->addConstant("Key_BraceLeft",            new QoreBigIntNode(Qt::Key_BraceLeft));
   qt_ns->addConstant("Key_Bar",                  new QoreBigIntNode(Qt::Key_Bar));
   qt_ns->addConstant("Key_BraceRight",           new QoreBigIntNode(Qt::Key_BraceRight));
   qt_ns->addConstant("Key_AsciiTilde",           new QoreBigIntNode(Qt::Key_AsciiTilde));
   qt_ns->addConstant("Key_nobreakspace",         new QoreBigIntNode(Qt::Key_nobreakspace));
   qt_ns->addConstant("Key_exclamdown",           new QoreBigIntNode(Qt::Key_exclamdown));
   qt_ns->addConstant("Key_cent",                 new QoreBigIntNode(Qt::Key_cent));
   qt_ns->addConstant("Key_sterling",             new QoreBigIntNode(Qt::Key_sterling));
   qt_ns->addConstant("Key_currency",             new QoreBigIntNode(Qt::Key_currency));
   qt_ns->addConstant("Key_yen",                  new QoreBigIntNode(Qt::Key_yen));
   qt_ns->addConstant("Key_brokenbar",            new QoreBigIntNode(Qt::Key_brokenbar));
   qt_ns->addConstant("Key_section",              new QoreBigIntNode(Qt::Key_section));
   qt_ns->addConstant("Key_diaeresis",            new QoreBigIntNode(Qt::Key_diaeresis));
   qt_ns->addConstant("Key_copyright",            new QoreBigIntNode(Qt::Key_copyright));
   qt_ns->addConstant("Key_ordfeminine",          new QoreBigIntNode(Qt::Key_ordfeminine));
   qt_ns->addConstant("Key_guillemotleft",        new QoreBigIntNode(Qt::Key_guillemotleft));
   qt_ns->addConstant("Key_notsign",              new QoreBigIntNode(Qt::Key_notsign));
   qt_ns->addConstant("Key_hyphen",               new QoreBigIntNode(Qt::Key_hyphen));
   qt_ns->addConstant("Key_registered",           new QoreBigIntNode(Qt::Key_registered));
   qt_ns->addConstant("Key_macron",               new QoreBigIntNode(Qt::Key_macron));
   qt_ns->addConstant("Key_degree",               new QoreBigIntNode(Qt::Key_degree));
   qt_ns->addConstant("Key_plusminus",            new QoreBigIntNode(Qt::Key_plusminus));
   qt_ns->addConstant("Key_twosuperior",          new QoreBigIntNode(Qt::Key_twosuperior));
   qt_ns->addConstant("Key_threesuperior",        new QoreBigIntNode(Qt::Key_threesuperior));
   qt_ns->addConstant("Key_acute",                new QoreBigIntNode(Qt::Key_acute));
   qt_ns->addConstant("Key_mu",                   new QoreBigIntNode(Qt::Key_mu));
   qt_ns->addConstant("Key_paragraph",            new QoreBigIntNode(Qt::Key_paragraph));
   qt_ns->addConstant("Key_periodcentered",       new QoreBigIntNode(Qt::Key_periodcentered));
   qt_ns->addConstant("Key_cedilla",              new QoreBigIntNode(Qt::Key_cedilla));
   qt_ns->addConstant("Key_onesuperior",          new QoreBigIntNode(Qt::Key_onesuperior));
   qt_ns->addConstant("Key_masculine",            new QoreBigIntNode(Qt::Key_masculine));
   qt_ns->addConstant("Key_guillemotright",       new QoreBigIntNode(Qt::Key_guillemotright));
   qt_ns->addConstant("Key_onequarter",           new QoreBigIntNode(Qt::Key_onequarter));
   qt_ns->addConstant("Key_onehalf",              new QoreBigIntNode(Qt::Key_onehalf));
   qt_ns->addConstant("Key_threequarters",        new QoreBigIntNode(Qt::Key_threequarters));
   qt_ns->addConstant("Key_questiondown",         new QoreBigIntNode(Qt::Key_questiondown));
   qt_ns->addConstant("Key_Agrave",               new QoreBigIntNode(Qt::Key_Agrave));
   qt_ns->addConstant("Key_Aacute",               new QoreBigIntNode(Qt::Key_Aacute));
   qt_ns->addConstant("Key_Acircumflex",          new QoreBigIntNode(Qt::Key_Acircumflex));
   qt_ns->addConstant("Key_Atilde",               new QoreBigIntNode(Qt::Key_Atilde));
   qt_ns->addConstant("Key_Adiaeresis",           new QoreBigIntNode(Qt::Key_Adiaeresis));
   qt_ns->addConstant("Key_Aring",                new QoreBigIntNode(Qt::Key_Aring));
   qt_ns->addConstant("Key_AE",                   new QoreBigIntNode(Qt::Key_AE));
   qt_ns->addConstant("Key_Ccedilla",             new QoreBigIntNode(Qt::Key_Ccedilla));
   qt_ns->addConstant("Key_Egrave",               new QoreBigIntNode(Qt::Key_Egrave));
   qt_ns->addConstant("Key_Eacute",               new QoreBigIntNode(Qt::Key_Eacute));
   qt_ns->addConstant("Key_Ecircumflex",          new QoreBigIntNode(Qt::Key_Ecircumflex));
   qt_ns->addConstant("Key_Ediaeresis",           new QoreBigIntNode(Qt::Key_Ediaeresis));
   qt_ns->addConstant("Key_Igrave",               new QoreBigIntNode(Qt::Key_Igrave));
   qt_ns->addConstant("Key_Iacute",               new QoreBigIntNode(Qt::Key_Iacute));
   qt_ns->addConstant("Key_Icircumflex",          new QoreBigIntNode(Qt::Key_Icircumflex));
   qt_ns->addConstant("Key_Idiaeresis",           new QoreBigIntNode(Qt::Key_Idiaeresis));
   qt_ns->addConstant("Key_ETH",                  new QoreBigIntNode(Qt::Key_ETH));
   qt_ns->addConstant("Key_Ntilde",               new QoreBigIntNode(Qt::Key_Ntilde));
   qt_ns->addConstant("Key_Ograve",               new QoreBigIntNode(Qt::Key_Ograve));
   qt_ns->addConstant("Key_Oacute",               new QoreBigIntNode(Qt::Key_Oacute));
   qt_ns->addConstant("Key_Ocircumflex",          new QoreBigIntNode(Qt::Key_Ocircumflex));
   qt_ns->addConstant("Key_Otilde",               new QoreBigIntNode(Qt::Key_Otilde));
   qt_ns->addConstant("Key_Odiaeresis",           new QoreBigIntNode(Qt::Key_Odiaeresis));
   qt_ns->addConstant("Key_multiply",             new QoreBigIntNode(Qt::Key_multiply));
   qt_ns->addConstant("Key_Ooblique",             new QoreBigIntNode(Qt::Key_Ooblique));
   qt_ns->addConstant("Key_Ugrave",               new QoreBigIntNode(Qt::Key_Ugrave));
   qt_ns->addConstant("Key_Uacute",               new QoreBigIntNode(Qt::Key_Uacute));
   qt_ns->addConstant("Key_Ucircumflex",          new QoreBigIntNode(Qt::Key_Ucircumflex));
   qt_ns->addConstant("Key_Udiaeresis",           new QoreBigIntNode(Qt::Key_Udiaeresis));
   qt_ns->addConstant("Key_Yacute",               new QoreBigIntNode(Qt::Key_Yacute));
   qt_ns->addConstant("Key_THORN",                new QoreBigIntNode(Qt::Key_THORN));
   qt_ns->addConstant("Key_ssharp",               new QoreBigIntNode(Qt::Key_ssharp));
   qt_ns->addConstant("Key_division",             new QoreBigIntNode(Qt::Key_division));
   qt_ns->addConstant("Key_ydiaeresis",           new QoreBigIntNode(Qt::Key_ydiaeresis));
   qt_ns->addConstant("Key_AltGr",                new QoreBigIntNode(Qt::Key_AltGr));
   qt_ns->addConstant("Key_Multi_key",            new QoreBigIntNode(Qt::Key_Multi_key));
   qt_ns->addConstant("Key_Codeinput",            new QoreBigIntNode(Qt::Key_Codeinput));
   qt_ns->addConstant("Key_SingleCandidate",      new QoreBigIntNode(Qt::Key_SingleCandidate));
   qt_ns->addConstant("Key_MultipleCandidate",    new QoreBigIntNode(Qt::Key_MultipleCandidate));
   qt_ns->addConstant("Key_PreviousCandidate",    new QoreBigIntNode(Qt::Key_PreviousCandidate));
   qt_ns->addConstant("Key_Mode_switch",          new QoreBigIntNode(Qt::Key_Mode_switch));
   qt_ns->addConstant("Key_Kanji",                new QoreBigIntNode(Qt::Key_Kanji));
   qt_ns->addConstant("Key_Muhenkan",             new QoreBigIntNode(Qt::Key_Muhenkan));
   qt_ns->addConstant("Key_Henkan",               new QoreBigIntNode(Qt::Key_Henkan));
   qt_ns->addConstant("Key_Romaji",               new QoreBigIntNode(Qt::Key_Romaji));
   qt_ns->addConstant("Key_Hiragana",             new QoreBigIntNode(Qt::Key_Hiragana));
   qt_ns->addConstant("Key_Katakana",             new QoreBigIntNode(Qt::Key_Katakana));
   qt_ns->addConstant("Key_Hiragana_Katakana",    new QoreBigIntNode(Qt::Key_Hiragana_Katakana));
   qt_ns->addConstant("Key_Zenkaku",              new QoreBigIntNode(Qt::Key_Zenkaku));
   qt_ns->addConstant("Key_Hankaku",              new QoreBigIntNode(Qt::Key_Hankaku));
   qt_ns->addConstant("Key_Zenkaku_Hankaku",      new QoreBigIntNode(Qt::Key_Zenkaku_Hankaku));
   qt_ns->addConstant("Key_Touroku",              new QoreBigIntNode(Qt::Key_Touroku));
   qt_ns->addConstant("Key_Massyo",               new QoreBigIntNode(Qt::Key_Massyo));
   qt_ns->addConstant("Key_Kana_Lock",            new QoreBigIntNode(Qt::Key_Kana_Lock));
   qt_ns->addConstant("Key_Kana_Shift",           new QoreBigIntNode(Qt::Key_Kana_Shift));
   qt_ns->addConstant("Key_Eisu_Shift",           new QoreBigIntNode(Qt::Key_Eisu_Shift));
   qt_ns->addConstant("Key_Eisu_toggle",          new QoreBigIntNode(Qt::Key_Eisu_toggle));
   qt_ns->addConstant("Key_Hangul",               new QoreBigIntNode(Qt::Key_Hangul));
   qt_ns->addConstant("Key_Hangul_Start",         new QoreBigIntNode(Qt::Key_Hangul_Start));
   qt_ns->addConstant("Key_Hangul_End",           new QoreBigIntNode(Qt::Key_Hangul_End));
   qt_ns->addConstant("Key_Hangul_Hanja",         new QoreBigIntNode(Qt::Key_Hangul_Hanja));
   qt_ns->addConstant("Key_Hangul_Jamo",          new QoreBigIntNode(Qt::Key_Hangul_Jamo));
   qt_ns->addConstant("Key_Hangul_Romaja",        new QoreBigIntNode(Qt::Key_Hangul_Romaja));
   qt_ns->addConstant("Key_Hangul_Jeonja",        new QoreBigIntNode(Qt::Key_Hangul_Jeonja));
   qt_ns->addConstant("Key_Hangul_Banja",         new QoreBigIntNode(Qt::Key_Hangul_Banja));
   qt_ns->addConstant("Key_Hangul_PreHanja",      new QoreBigIntNode(Qt::Key_Hangul_PreHanja));
   qt_ns->addConstant("Key_Hangul_PostHanja",     new QoreBigIntNode(Qt::Key_Hangul_PostHanja));
   qt_ns->addConstant("Key_Hangul_Special",       new QoreBigIntNode(Qt::Key_Hangul_Special));
   qt_ns->addConstant("Key_Dead_Grave",           new QoreBigIntNode(Qt::Key_Dead_Grave));
   qt_ns->addConstant("Key_Dead_Acute",           new QoreBigIntNode(Qt::Key_Dead_Acute));
   qt_ns->addConstant("Key_Dead_Circumflex",      new QoreBigIntNode(Qt::Key_Dead_Circumflex));
   qt_ns->addConstant("Key_Dead_Tilde",           new QoreBigIntNode(Qt::Key_Dead_Tilde));
   qt_ns->addConstant("Key_Dead_Macron",          new QoreBigIntNode(Qt::Key_Dead_Macron));
   qt_ns->addConstant("Key_Dead_Breve",           new QoreBigIntNode(Qt::Key_Dead_Breve));
   qt_ns->addConstant("Key_Dead_Abovedot",        new QoreBigIntNode(Qt::Key_Dead_Abovedot));
   qt_ns->addConstant("Key_Dead_Diaeresis",       new QoreBigIntNode(Qt::Key_Dead_Diaeresis));
   qt_ns->addConstant("Key_Dead_Abovering",       new QoreBigIntNode(Qt::Key_Dead_Abovering));
   qt_ns->addConstant("Key_Dead_Doubleacute",     new QoreBigIntNode(Qt::Key_Dead_Doubleacute));
   qt_ns->addConstant("Key_Dead_Caron",           new QoreBigIntNode(Qt::Key_Dead_Caron));
   qt_ns->addConstant("Key_Dead_Cedilla",         new QoreBigIntNode(Qt::Key_Dead_Cedilla));
   qt_ns->addConstant("Key_Dead_Ogonek",          new QoreBigIntNode(Qt::Key_Dead_Ogonek));
   qt_ns->addConstant("Key_Dead_Iota",            new QoreBigIntNode(Qt::Key_Dead_Iota));
   qt_ns->addConstant("Key_Dead_Voiced_Sound",    new QoreBigIntNode(Qt::Key_Dead_Voiced_Sound));
   qt_ns->addConstant("Key_Dead_Semivoiced_Sound", new QoreBigIntNode(Qt::Key_Dead_Semivoiced_Sound));
   qt_ns->addConstant("Key_Dead_Belowdot",        new QoreBigIntNode(Qt::Key_Dead_Belowdot));
   qt_ns->addConstant("Key_Dead_Hook",            new QoreBigIntNode(Qt::Key_Dead_Hook));
   qt_ns->addConstant("Key_Dead_Horn",            new QoreBigIntNode(Qt::Key_Dead_Horn));
   qt_ns->addConstant("Key_Back",                 new QoreBigIntNode(Qt::Key_Back));
   qt_ns->addConstant("Key_Forward",              new QoreBigIntNode(Qt::Key_Forward));
   qt_ns->addConstant("Key_Stop",                 new QoreBigIntNode(Qt::Key_Stop));
   qt_ns->addConstant("Key_Refresh",              new QoreBigIntNode(Qt::Key_Refresh));
   qt_ns->addConstant("Key_VolumeDown",           new QoreBigIntNode(Qt::Key_VolumeDown));
   qt_ns->addConstant("Key_VolumeMute",           new QoreBigIntNode(Qt::Key_VolumeMute));
   qt_ns->addConstant("Key_VolumeUp",             new QoreBigIntNode(Qt::Key_VolumeUp));
   qt_ns->addConstant("Key_BassBoost",            new QoreBigIntNode(Qt::Key_BassBoost));
   qt_ns->addConstant("Key_BassUp",               new QoreBigIntNode(Qt::Key_BassUp));
   qt_ns->addConstant("Key_BassDown",             new QoreBigIntNode(Qt::Key_BassDown));
   qt_ns->addConstant("Key_TrebleUp",             new QoreBigIntNode(Qt::Key_TrebleUp));
   qt_ns->addConstant("Key_TrebleDown",           new QoreBigIntNode(Qt::Key_TrebleDown));
   qt_ns->addConstant("Key_MediaPlay",            new QoreBigIntNode(Qt::Key_MediaPlay));
   qt_ns->addConstant("Key_MediaStop",            new QoreBigIntNode(Qt::Key_MediaStop));
   qt_ns->addConstant("Key_MediaPrevious",        new QoreBigIntNode(Qt::Key_MediaPrevious));
   qt_ns->addConstant("Key_MediaNext",            new QoreBigIntNode(Qt::Key_MediaNext));
   qt_ns->addConstant("Key_MediaRecord",          new QoreBigIntNode(Qt::Key_MediaRecord));
   qt_ns->addConstant("Key_HomePage",             new QoreBigIntNode(Qt::Key_HomePage));
   qt_ns->addConstant("Key_Favorites",            new QoreBigIntNode(Qt::Key_Favorites));
   qt_ns->addConstant("Key_Search",               new QoreBigIntNode(Qt::Key_Search));
   qt_ns->addConstant("Key_Standby",              new QoreBigIntNode(Qt::Key_Standby));
   qt_ns->addConstant("Key_OpenUrl",              new QoreBigIntNode(Qt::Key_OpenUrl));
   qt_ns->addConstant("Key_LaunchMail",           new QoreBigIntNode(Qt::Key_LaunchMail));
   qt_ns->addConstant("Key_LaunchMedia",          new QoreBigIntNode(Qt::Key_LaunchMedia));
   qt_ns->addConstant("Key_Launch0",              new QoreBigIntNode(Qt::Key_Launch0));
   qt_ns->addConstant("Key_Launch1",              new QoreBigIntNode(Qt::Key_Launch1));
   qt_ns->addConstant("Key_Launch2",              new QoreBigIntNode(Qt::Key_Launch2));
   qt_ns->addConstant("Key_Launch3",              new QoreBigIntNode(Qt::Key_Launch3));
   qt_ns->addConstant("Key_Launch4",              new QoreBigIntNode(Qt::Key_Launch4));
   qt_ns->addConstant("Key_Launch5",              new QoreBigIntNode(Qt::Key_Launch5));
   qt_ns->addConstant("Key_Launch6",              new QoreBigIntNode(Qt::Key_Launch6));
   qt_ns->addConstant("Key_Launch7",              new QoreBigIntNode(Qt::Key_Launch7));
   qt_ns->addConstant("Key_Launch8",              new QoreBigIntNode(Qt::Key_Launch8));
   qt_ns->addConstant("Key_Launch9",              new QoreBigIntNode(Qt::Key_Launch9));
   qt_ns->addConstant("Key_LaunchA",              new QoreBigIntNode(Qt::Key_LaunchA));
   qt_ns->addConstant("Key_LaunchB",              new QoreBigIntNode(Qt::Key_LaunchB));
   qt_ns->addConstant("Key_LaunchC",              new QoreBigIntNode(Qt::Key_LaunchC));
   qt_ns->addConstant("Key_LaunchD",              new QoreBigIntNode(Qt::Key_LaunchD));
   qt_ns->addConstant("Key_LaunchE",              new QoreBigIntNode(Qt::Key_LaunchE));
   qt_ns->addConstant("Key_LaunchF",              new QoreBigIntNode(Qt::Key_LaunchF));
   qt_ns->addConstant("Key_MediaLast",            new QoreBigIntNode(Qt::Key_MediaLast));
   qt_ns->addConstant("Key_Select",               new QoreBigIntNode(Qt::Key_Select));
   qt_ns->addConstant("Key_Yes",                  new QoreBigIntNode(Qt::Key_Yes));
   qt_ns->addConstant("Key_No",                   new QoreBigIntNode(Qt::Key_No));
   qt_ns->addConstant("Key_Cancel",               new QoreBigIntNode(Qt::Key_Cancel));
   qt_ns->addConstant("Key_Printer",              new QoreBigIntNode(Qt::Key_Printer));
   qt_ns->addConstant("Key_Execute",              new QoreBigIntNode(Qt::Key_Execute));
   qt_ns->addConstant("Key_Sleep",                new QoreBigIntNode(Qt::Key_Sleep));
   qt_ns->addConstant("Key_Play",                 new QoreBigIntNode(Qt::Key_Play));
   qt_ns->addConstant("Key_Zoom",                 new QoreBigIntNode(Qt::Key_Zoom));
   qt_ns->addConstant("Key_Context1",             new QoreBigIntNode(Qt::Key_Context1));
   qt_ns->addConstant("Key_Context2",             new QoreBigIntNode(Qt::Key_Context2));
   qt_ns->addConstant("Key_Context3",             new QoreBigIntNode(Qt::Key_Context3));
   qt_ns->addConstant("Key_Context4",             new QoreBigIntNode(Qt::Key_Context4));
   qt_ns->addConstant("Key_Call",                 new QoreBigIntNode(Qt::Key_Call));
   qt_ns->addConstant("Key_Hangup",               new QoreBigIntNode(Qt::Key_Hangup));
   qt_ns->addConstant("Key_Flip",                 new QoreBigIntNode(Qt::Key_Flip));
   qt_ns->addConstant("Key_unknown",              new QoreBigIntNode(Qt::Key_unknown));

   // MatchFlag enum
   qt_ns->addConstant("MatchExactly",             new QoreBigIntNode(Qt::MatchExactly));
   qt_ns->addConstant("MatchContains",            new QoreBigIntNode(Qt::MatchContains));
   qt_ns->addConstant("MatchStartsWith",          new QoreBigIntNode(Qt::MatchStartsWith));
   qt_ns->addConstant("MatchEndsWith",            new QoreBigIntNode(Qt::MatchEndsWith));
   qt_ns->addConstant("MatchRegExp",              new QoreBigIntNode(Qt::MatchRegExp));
   qt_ns->addConstant("MatchWildcard",            new QoreBigIntNode(Qt::MatchWildcard));
   qt_ns->addConstant("MatchFixedString",         new QoreBigIntNode(Qt::MatchFixedString));
   qt_ns->addConstant("MatchCaseSensitive",       new QoreBigIntNode(Qt::MatchCaseSensitive));
   qt_ns->addConstant("MatchWrap",                new QoreBigIntNode(Qt::MatchWrap));
   qt_ns->addConstant("MatchRecursive",           new QoreBigIntNode(Qt::MatchRecursive));

   // ItemDataRole enum
   qt_ns->addConstant("DisplayRole",              new QoreBigIntNode(Qt::DisplayRole));
   qt_ns->addConstant("DecorationRole",           new QoreBigIntNode(Qt::DecorationRole));
   qt_ns->addConstant("EditRole",                 new QoreBigIntNode(Qt::EditRole));
   qt_ns->addConstant("ToolTipRole",              new QoreBigIntNode(Qt::ToolTipRole));
   qt_ns->addConstant("StatusTipRole",            new QoreBigIntNode(Qt::StatusTipRole));
   qt_ns->addConstant("WhatsThisRole",            new QoreBigIntNode(Qt::WhatsThisRole));
   qt_ns->addConstant("FontRole",                 new QoreBigIntNode(Qt::FontRole));
   qt_ns->addConstant("TextAlignmentRole",        new QoreBigIntNode(Qt::TextAlignmentRole));
   qt_ns->addConstant("BackgroundColorRole",      new QoreBigIntNode(Qt::BackgroundColorRole));
   qt_ns->addConstant("BackgroundRole",           new QoreBigIntNode(Qt::BackgroundRole));
   qt_ns->addConstant("TextColorRole",            new QoreBigIntNode(Qt::TextColorRole));
   qt_ns->addConstant("ForegroundRole",           new QoreBigIntNode(Qt::ForegroundRole));
   qt_ns->addConstant("CheckStateRole",           new QoreBigIntNode(Qt::CheckStateRole));
   qt_ns->addConstant("AccessibleTextRole",       new QoreBigIntNode(Qt::AccessibleTextRole));
   qt_ns->addConstant("AccessibleDescriptionRole", new QoreBigIntNode(Qt::AccessibleDescriptionRole));
   qt_ns->addConstant("SizeHintRole",             new QoreBigIntNode(Qt::SizeHintRole));
   qt_ns->addConstant("UserRole",                 new QoreBigIntNode(Qt::UserRole));

   // ItemFlag enum
   qt_ns->addConstant("ItemIsSelectable",         new QoreBigIntNode(Qt::ItemIsSelectable));
   qt_ns->addConstant("ItemIsEditable",           new QoreBigIntNode(Qt::ItemIsEditable));
   qt_ns->addConstant("ItemIsDragEnabled",        new QoreBigIntNode(Qt::ItemIsDragEnabled));
   qt_ns->addConstant("ItemIsDropEnabled",        new QoreBigIntNode(Qt::ItemIsDropEnabled));
   qt_ns->addConstant("ItemIsUserCheckable",      new QoreBigIntNode(Qt::ItemIsUserCheckable));
   qt_ns->addConstant("ItemIsEnabled",            new QoreBigIntNode(Qt::ItemIsEnabled));
   qt_ns->addConstant("ItemIsTristate",           new QoreBigIntNode(Qt::ItemIsTristate));
	
   // AspectRatioMode enum
   qt_ns->addConstant("IgnoreAspectRatio",        new QoreBigIntNode(Qt::IgnoreAspectRatio));
   qt_ns->addConstant("KeepAspectRatio",          new QoreBigIntNode(Qt::KeepAspectRatio));
   qt_ns->addConstant("KeepAspectRatioByExpanding", new QoreBigIntNode(Qt::KeepAspectRatioByExpanding));

   // TextFormat enum
   qt_ns->addConstant("PlainText",                new QoreBigIntNode(Qt::PlainText));
   qt_ns->addConstant("RichText",                 new QoreBigIntNode(Qt::RichText));
   qt_ns->addConstant("AutoText",                 new QoreBigIntNode(Qt::AutoText));
   qt_ns->addConstant("LogText",                  new QoreBigIntNode(Qt::LogText));

   // CursorShape enum
   qt_ns->addConstant("ArrowCursor",              new QoreBigIntNode(Qt::ArrowCursor));
   qt_ns->addConstant("UpArrowCursor",            new QoreBigIntNode(Qt::UpArrowCursor));
   qt_ns->addConstant("CrossCursor",              new QoreBigIntNode(Qt::CrossCursor));
   qt_ns->addConstant("WaitCursor",               new QoreBigIntNode(Qt::WaitCursor));
   qt_ns->addConstant("IBeamCursor",              new QoreBigIntNode(Qt::IBeamCursor));
   qt_ns->addConstant("SizeVerCursor",            new QoreBigIntNode(Qt::SizeVerCursor));
   qt_ns->addConstant("SizeHorCursor",            new QoreBigIntNode(Qt::SizeHorCursor));
   qt_ns->addConstant("SizeBDiagCursor",          new QoreBigIntNode(Qt::SizeBDiagCursor));
   qt_ns->addConstant("SizeFDiagCursor",          new QoreBigIntNode(Qt::SizeFDiagCursor));
   qt_ns->addConstant("SizeAllCursor",            new QoreBigIntNode(Qt::SizeAllCursor));
   qt_ns->addConstant("BlankCursor",              new QoreBigIntNode(Qt::BlankCursor));
   qt_ns->addConstant("SplitVCursor",             new QoreBigIntNode(Qt::SplitVCursor));
   qt_ns->addConstant("SplitHCursor",             new QoreBigIntNode(Qt::SplitHCursor));
   qt_ns->addConstant("PointingHandCursor",       new QoreBigIntNode(Qt::PointingHandCursor));
   qt_ns->addConstant("ForbiddenCursor",          new QoreBigIntNode(Qt::ForbiddenCursor));
   qt_ns->addConstant("WhatsThisCursor",          new QoreBigIntNode(Qt::WhatsThisCursor));
   qt_ns->addConstant("BusyCursor",               new QoreBigIntNode(Qt::BusyCursor));
   qt_ns->addConstant("OpenHandCursor",           new QoreBigIntNode(Qt::OpenHandCursor));
   qt_ns->addConstant("ClosedHandCursor",         new QoreBigIntNode(Qt::ClosedHandCursor));
   qt_ns->addConstant("LastCursor",               new QoreBigIntNode(Qt::LastCursor));
   qt_ns->addConstant("BitmapCursor",             new QoreBigIntNode(Qt::BitmapCursor));
   qt_ns->addConstant("CustomCursor",             new QoreBigIntNode(Qt::CustomCursor));

   // AnchorAttribute enum
   qt_ns->addConstant("AnchorName",               new QoreBigIntNode(Qt::AnchorName));
   qt_ns->addConstant("AnchorHref",               new QoreBigIntNode(Qt::AnchorHref));

   // DockWidgetArea enum
   qt_ns->addConstant("LeftDockWidgetArea",       new QoreBigIntNode(Qt::LeftDockWidgetArea));
   qt_ns->addConstant("RightDockWidgetArea",      new QoreBigIntNode(Qt::RightDockWidgetArea));
   qt_ns->addConstant("TopDockWidgetArea",        new QoreBigIntNode(Qt::TopDockWidgetArea));
   qt_ns->addConstant("BottomDockWidgetArea",     new QoreBigIntNode(Qt::BottomDockWidgetArea));
   qt_ns->addConstant("DockWidgetArea_Mask",      new QoreBigIntNode(Qt::DockWidgetArea_Mask));
   qt_ns->addConstant("AllDockWidgetAreas",       new QoreBigIntNode(Qt::AllDockWidgetAreas));
   qt_ns->addConstant("NoDockWidgetArea",         new QoreBigIntNode(Qt::NoDockWidgetArea));

   // DockWidgetAreaSizes enum
   qt_ns->addConstant("NDockWidgetAreas",         new QoreBigIntNode(Qt::NDockWidgetAreas));

   // ToolBarArea enum
   qt_ns->addConstant("LeftToolBarArea",          new QoreBigIntNode(Qt::LeftToolBarArea));
   qt_ns->addConstant("RightToolBarArea",         new QoreBigIntNode(Qt::RightToolBarArea));
   qt_ns->addConstant("TopToolBarArea",           new QoreBigIntNode(Qt::TopToolBarArea));
   qt_ns->addConstant("BottomToolBarArea",        new QoreBigIntNode(Qt::BottomToolBarArea));
   qt_ns->addConstant("ToolBarArea_Mask",         new QoreBigIntNode(Qt::ToolBarArea_Mask));
   qt_ns->addConstant("AllToolBarAreas",          new QoreBigIntNode(Qt::AllToolBarAreas));
   qt_ns->addConstant("NoToolBarArea",            new QoreBigIntNode(Qt::NoToolBarArea));

   // ToolBarSizes enum
   qt_ns->addConstant("NToolBarAreas",            new QoreBigIntNode(Qt::NToolBarAreas));

   // PenCapStyle enum
   qt_ns->addConstant("FlatCap",                  new QoreBigIntNode(Qt::FlatCap));
   qt_ns->addConstant("SquareCap",                new QoreBigIntNode(Qt::SquareCap));
   qt_ns->addConstant("RoundCap",                 new QoreBigIntNode(Qt::RoundCap));
   qt_ns->addConstant("MPenCapStyle",             new QoreBigIntNode(Qt::MPenCapStyle));

   // PenJoinStyle enum
   qt_ns->addConstant("MiterJoin",                new QoreBigIntNode(Qt::MiterJoin));
   qt_ns->addConstant("BevelJoin",                new QoreBigIntNode(Qt::BevelJoin));
   qt_ns->addConstant("RoundJoin",                new QoreBigIntNode(Qt::RoundJoin));
   qt_ns->addConstant("SvgMiterJoin",             new QoreBigIntNode(Qt::SvgMiterJoin));
   qt_ns->addConstant("MPenJoinStyle",            new QoreBigIntNode(Qt::MPenJoinStyle));

   // WidgetAttribute enum
   qt_ns->addConstant("WA_Disabled",              new QoreBigIntNode(Qt::WA_Disabled));
   qt_ns->addConstant("WA_UnderMouse",            new QoreBigIntNode(Qt::WA_UnderMouse));
   qt_ns->addConstant("WA_MouseTracking",         new QoreBigIntNode(Qt::WA_MouseTracking));
   qt_ns->addConstant("WA_ContentsPropagated",    new QoreBigIntNode(Qt::WA_ContentsPropagated));
   qt_ns->addConstant("WA_OpaquePaintEvent",      new QoreBigIntNode(Qt::WA_OpaquePaintEvent));
   qt_ns->addConstant("WA_NoBackground",          new QoreBigIntNode(Qt::WA_NoBackground));
   qt_ns->addConstant("WA_StaticContents",        new QoreBigIntNode(Qt::WA_StaticContents));
   qt_ns->addConstant("WA_LaidOut",               new QoreBigIntNode(Qt::WA_LaidOut));
   qt_ns->addConstant("WA_PaintOnScreen",         new QoreBigIntNode(Qt::WA_PaintOnScreen));
   qt_ns->addConstant("WA_NoSystemBackground",    new QoreBigIntNode(Qt::WA_NoSystemBackground));
   qt_ns->addConstant("WA_UpdatesDisabled",       new QoreBigIntNode(Qt::WA_UpdatesDisabled));
   qt_ns->addConstant("WA_Mapped",                new QoreBigIntNode(Qt::WA_Mapped));
   qt_ns->addConstant("WA_MacNoClickThrough",     new QoreBigIntNode(Qt::WA_MacNoClickThrough));
   qt_ns->addConstant("WA_PaintOutsidePaintEvent", new QoreBigIntNode(Qt::WA_PaintOutsidePaintEvent));
   qt_ns->addConstant("WA_InputMethodEnabled",    new QoreBigIntNode(Qt::WA_InputMethodEnabled));
   qt_ns->addConstant("WA_WState_Visible",        new QoreBigIntNode(Qt::WA_WState_Visible));
   qt_ns->addConstant("WA_WState_Hidden",         new QoreBigIntNode(Qt::WA_WState_Hidden));
   qt_ns->addConstant("WA_ForceDisabled",         new QoreBigIntNode(Qt::WA_ForceDisabled));
   qt_ns->addConstant("WA_KeyCompression",        new QoreBigIntNode(Qt::WA_KeyCompression));
   qt_ns->addConstant("WA_PendingMoveEvent",      new QoreBigIntNode(Qt::WA_PendingMoveEvent));
   qt_ns->addConstant("WA_PendingResizeEvent",    new QoreBigIntNode(Qt::WA_PendingResizeEvent));
   qt_ns->addConstant("WA_SetPalette",            new QoreBigIntNode(Qt::WA_SetPalette));
   qt_ns->addConstant("WA_SetFont",               new QoreBigIntNode(Qt::WA_SetFont));
   qt_ns->addConstant("WA_SetCursor",             new QoreBigIntNode(Qt::WA_SetCursor));
   qt_ns->addConstant("WA_NoChildEventsFromChildren", new QoreBigIntNode(Qt::WA_NoChildEventsFromChildren));
   qt_ns->addConstant("WA_WindowModified",        new QoreBigIntNode(Qt::WA_WindowModified));
   qt_ns->addConstant("WA_Resized",               new QoreBigIntNode(Qt::WA_Resized));
   qt_ns->addConstant("WA_Moved",                 new QoreBigIntNode(Qt::WA_Moved));
   qt_ns->addConstant("WA_PendingUpdate",         new QoreBigIntNode(Qt::WA_PendingUpdate));
   qt_ns->addConstant("WA_InvalidSize",           new QoreBigIntNode(Qt::WA_InvalidSize));
   qt_ns->addConstant("WA_MacBrushedMetal",       new QoreBigIntNode(Qt::WA_MacBrushedMetal));
   qt_ns->addConstant("WA_MacMetalStyle",         new QoreBigIntNode(Qt::WA_MacMetalStyle));
   qt_ns->addConstant("WA_CustomWhatsThis",       new QoreBigIntNode(Qt::WA_CustomWhatsThis));
   qt_ns->addConstant("WA_LayoutOnEntireRect",    new QoreBigIntNode(Qt::WA_LayoutOnEntireRect));
   qt_ns->addConstant("WA_OutsideWSRange",        new QoreBigIntNode(Qt::WA_OutsideWSRange));
   qt_ns->addConstant("WA_GrabbedShortcut",       new QoreBigIntNode(Qt::WA_GrabbedShortcut));
   qt_ns->addConstant("WA_TransparentForMouseEvents", new QoreBigIntNode(Qt::WA_TransparentForMouseEvents));
   qt_ns->addConstant("WA_PaintUnclipped",        new QoreBigIntNode(Qt::WA_PaintUnclipped));
   qt_ns->addConstant("WA_SetWindowIcon",         new QoreBigIntNode(Qt::WA_SetWindowIcon));
   qt_ns->addConstant("WA_NoMouseReplay",         new QoreBigIntNode(Qt::WA_NoMouseReplay));
   qt_ns->addConstant("WA_DeleteOnClose",         new QoreBigIntNode(Qt::WA_DeleteOnClose));
   qt_ns->addConstant("WA_RightToLeft",           new QoreBigIntNode(Qt::WA_RightToLeft));
   qt_ns->addConstant("WA_SetLayoutDirection",    new QoreBigIntNode(Qt::WA_SetLayoutDirection));
   qt_ns->addConstant("WA_NoChildEventsForParent", new QoreBigIntNode(Qt::WA_NoChildEventsForParent));
   qt_ns->addConstant("WA_ForceUpdatesDisabled",  new QoreBigIntNode(Qt::WA_ForceUpdatesDisabled));
   qt_ns->addConstant("WA_WState_Created",        new QoreBigIntNode(Qt::WA_WState_Created));
   qt_ns->addConstant("WA_WState_CompressKeys",   new QoreBigIntNode(Qt::WA_WState_CompressKeys));
   qt_ns->addConstant("WA_WState_InPaintEvent",   new QoreBigIntNode(Qt::WA_WState_InPaintEvent));
   qt_ns->addConstant("WA_WState_Reparented",     new QoreBigIntNode(Qt::WA_WState_Reparented));
   qt_ns->addConstant("WA_WState_ConfigPending",  new QoreBigIntNode(Qt::WA_WState_ConfigPending));
   qt_ns->addConstant("WA_WState_Polished",       new QoreBigIntNode(Qt::WA_WState_Polished));
   qt_ns->addConstant("WA_WState_DND",            new QoreBigIntNode(Qt::WA_WState_DND));
   qt_ns->addConstant("WA_WState_OwnSizePolicy",  new QoreBigIntNode(Qt::WA_WState_OwnSizePolicy));
   qt_ns->addConstant("WA_WState_ExplicitShowHide", new QoreBigIntNode(Qt::WA_WState_ExplicitShowHide));
   qt_ns->addConstant("WA_ShowModal",             new QoreBigIntNode(Qt::WA_ShowModal));
   qt_ns->addConstant("WA_MouseNoMask",           new QoreBigIntNode(Qt::WA_MouseNoMask));
   qt_ns->addConstant("WA_GroupLeader",           new QoreBigIntNode(Qt::WA_GroupLeader));
   qt_ns->addConstant("WA_NoMousePropagation",    new QoreBigIntNode(Qt::WA_NoMousePropagation));
   qt_ns->addConstant("WA_Hover",                 new QoreBigIntNode(Qt::WA_Hover));
   qt_ns->addConstant("WA_InputMethodTransparent", new QoreBigIntNode(Qt::WA_InputMethodTransparent));
   qt_ns->addConstant("WA_QuitOnClose",           new QoreBigIntNode(Qt::WA_QuitOnClose));
   qt_ns->addConstant("WA_KeyboardFocusChange",   new QoreBigIntNode(Qt::WA_KeyboardFocusChange));
   qt_ns->addConstant("WA_AcceptDrops",           new QoreBigIntNode(Qt::WA_AcceptDrops));
   qt_ns->addConstant("WA_DropSiteRegistered",    new QoreBigIntNode(Qt::WA_DropSiteRegistered));
   qt_ns->addConstant("WA_ForceAcceptDrops",      new QoreBigIntNode(Qt::WA_ForceAcceptDrops));
   qt_ns->addConstant("WA_WindowPropagation",     new QoreBigIntNode(Qt::WA_WindowPropagation));
   qt_ns->addConstant("WA_NoX11EventCompression", new QoreBigIntNode(Qt::WA_NoX11EventCompression));
   qt_ns->addConstant("WA_TintedBackground",      new QoreBigIntNode(Qt::WA_TintedBackground));
   qt_ns->addConstant("WA_X11OpenGLOverlay",      new QoreBigIntNode(Qt::WA_X11OpenGLOverlay));
   qt_ns->addConstant("WA_AlwaysShowToolTips",    new QoreBigIntNode(Qt::WA_AlwaysShowToolTips));
   qt_ns->addConstant("WA_MacOpaqueSizeGrip",     new QoreBigIntNode(Qt::WA_MacOpaqueSizeGrip));
   qt_ns->addConstant("WA_SetStyle",              new QoreBigIntNode(Qt::WA_SetStyle));
   qt_ns->addConstant("WA_SetLocale",             new QoreBigIntNode(Qt::WA_SetLocale));
   qt_ns->addConstant("WA_MacShowFocusRect",      new QoreBigIntNode(Qt::WA_MacShowFocusRect));
   qt_ns->addConstant("WA_MacNormalSize",         new QoreBigIntNode(Qt::WA_MacNormalSize));
   qt_ns->addConstant("WA_MacSmallSize",          new QoreBigIntNode(Qt::WA_MacSmallSize));
   qt_ns->addConstant("WA_MacMiniSize",           new QoreBigIntNode(Qt::WA_MacMiniSize));
   qt_ns->addConstant("WA_LayoutUsesWidgetRect",  new QoreBigIntNode(Qt::WA_LayoutUsesWidgetRect));
   qt_ns->addConstant("WA_StyledBackground",      new QoreBigIntNode(Qt::WA_StyledBackground));
   qt_ns->addConstant("WA_MSWindowsUseDirect3D",  new QoreBigIntNode(Qt::WA_MSWindowsUseDirect3D));
   qt_ns->addConstant("WA_CanHostQMdiSubWindowTitleBar", new QoreBigIntNode(Qt::WA_CanHostQMdiSubWindowTitleBar));
   qt_ns->addConstant("WA_MacAlwaysShowToolWindow", new QoreBigIntNode(Qt::WA_MacAlwaysShowToolWindow));
   qt_ns->addConstant("WA_StyleSheet",            new QoreBigIntNode(Qt::WA_StyleSheet));
   qt_ns->addConstant("WA_AttributeCount",        new QoreBigIntNode(Qt::WA_AttributeCount));
   
   // WindowType enum
   qt_ns->addConstant("Widget",                   new QoreBigIntNode(Qt::Widget));
   qt_ns->addConstant("Window",                   new QoreBigIntNode(Qt::Window));
   qt_ns->addConstant("Dialog",                   new QoreBigIntNode(Qt::Dialog));
   qt_ns->addConstant("Sheet",                    new QoreBigIntNode(Qt::Sheet));
   qt_ns->addConstant("Drawer",                   new QoreBigIntNode(Qt::Drawer));
   qt_ns->addConstant("Popup",                    new QoreBigIntNode(Qt::Popup));
   qt_ns->addConstant("Tool",                     new QoreBigIntNode(Qt::Tool));
   qt_ns->addConstant("ToolTip",                  new QoreBigIntNode(Qt::ToolTip));
   qt_ns->addConstant("SplashScreen",             new QoreBigIntNode(Qt::SplashScreen));
   qt_ns->addConstant("Desktop",                  new QoreBigIntNode(Qt::Desktop));
   qt_ns->addConstant("SubWindow",                new QoreBigIntNode(Qt::SubWindow));
   qt_ns->addConstant("WindowType_Mask",          new QoreBigIntNode(Qt::WindowType_Mask));
   qt_ns->addConstant("MSWindowsFixedSizeDialogHint", new QoreBigIntNode(Qt::MSWindowsFixedSizeDialogHint));
   qt_ns->addConstant("MSWindowsOwnDC",           new QoreBigIntNode(Qt::MSWindowsOwnDC));
   qt_ns->addConstant("X11BypassWindowManagerHint", new QoreBigIntNode(Qt::X11BypassWindowManagerHint));
   qt_ns->addConstant("FramelessWindowHint",      new QoreBigIntNode(Qt::FramelessWindowHint));
   qt_ns->addConstant("WindowTitleHint",          new QoreBigIntNode(Qt::WindowTitleHint));
   qt_ns->addConstant("WindowSystemMenuHint",     new QoreBigIntNode(Qt::WindowSystemMenuHint));
   qt_ns->addConstant("WindowMinimizeButtonHint", new QoreBigIntNode(Qt::WindowMinimizeButtonHint));
   qt_ns->addConstant("WindowMaximizeButtonHint", new QoreBigIntNode(Qt::WindowMaximizeButtonHint));
   qt_ns->addConstant("WindowMinMaxButtonsHint",  new QoreBigIntNode(Qt::WindowMinMaxButtonsHint));
   qt_ns->addConstant("WindowContextHelpButtonHint", new QoreBigIntNode(Qt::WindowContextHelpButtonHint));
   qt_ns->addConstant("WindowShadeButtonHint",    new QoreBigIntNode(Qt::WindowShadeButtonHint));
   qt_ns->addConstant("WindowStaysOnTopHint",     new QoreBigIntNode(Qt::WindowStaysOnTopHint));
   qt_ns->addConstant("CustomizeWindowHint",      new QoreBigIntNode(Qt::CustomizeWindowHint));

   // FocusPolicy enum
   qt_ns->addConstant("NoFocus",                  new QoreBigIntNode(Qt::NoFocus));
   qt_ns->addConstant("TabFocus",                 new QoreBigIntNode(Qt::TabFocus));
   qt_ns->addConstant("ClickFocus",               new QoreBigIntNode(Qt::ClickFocus));
   qt_ns->addConstant("StrongFocus",              new QoreBigIntNode(Qt::StrongFocus));
   qt_ns->addConstant("WheelFocus",               new QoreBigIntNode(Qt::WheelFocus));

   // ConnectionType enum
   qt_ns->addConstant("AutoConnection",           new QoreBigIntNode(Qt::AutoConnection));
   qt_ns->addConstant("DirectConnection",         new QoreBigIntNode(Qt::DirectConnection));
   qt_ns->addConstant("QueuedConnection",         new QoreBigIntNode(Qt::QueuedConnection));
   qt_ns->addConstant("AutoCompatConnection",     new QoreBigIntNode(Qt::AutoCompatConnection));
   qt_ns->addConstant("BlockingQueuedConnection", new QoreBigIntNode(Qt::BlockingQueuedConnection));

   // DateFormat enum
   qt_ns->addConstant("TextDate",                 new QoreBigIntNode(Qt::TextDate));
   qt_ns->addConstant("ISODate",                  new QoreBigIntNode(Qt::ISODate));
   qt_ns->addConstant("SystemLocaleDate",         new QoreBigIntNode(Qt::SystemLocaleDate));
   qt_ns->addConstant("LocalDate",                new QoreBigIntNode(Qt::LocalDate));
   qt_ns->addConstant("LocaleDate",               new QoreBigIntNode(Qt::LocaleDate));

   // TimeSpec enum
   qt_ns->addConstant("LocalTime",                new QoreBigIntNode(Qt::LocalTime));
   qt_ns->addConstant("UTC",                      new QoreBigIntNode(Qt::UTC));

   // ScrollBarPolicy enum
   qt_ns->addConstant("ScrollBarAsNeeded",        new QoreBigIntNode(Qt::ScrollBarAsNeeded));
   qt_ns->addConstant("ScrollBarAlwaysOff",       new QoreBigIntNode(Qt::ScrollBarAlwaysOff));
   qt_ns->addConstant("ScrollBarAlwaysOn",        new QoreBigIntNode(Qt::ScrollBarAlwaysOn));

   // CaseSensitivity enum
   qt_ns->addConstant("CaseInsensitive",          new QoreBigIntNode(Qt::CaseInsensitive));
   qt_ns->addConstant("CaseSensitive",            new QoreBigIntNode(Qt::CaseSensitive));

   // Corner enum
   qt_ns->addConstant("TopLeftCorner",            new QoreBigIntNode(Qt::TopLeftCorner));
   qt_ns->addConstant("TopRightCorner",           new QoreBigIntNode(Qt::TopRightCorner));
   qt_ns->addConstant("BottomLeftCorner",         new QoreBigIntNode(Qt::BottomLeftCorner));
   qt_ns->addConstant("BottomRightCorner",        new QoreBigIntNode(Qt::BottomRightCorner));

   // ShortcutContext enum
   qt_ns->addConstant("WidgetShortcut",           new QoreBigIntNode(Qt::WidgetShortcut));
   qt_ns->addConstant("WindowShortcut",           new QoreBigIntNode(Qt::WindowShortcut));
   qt_ns->addConstant("ApplicationShortcut",      new QoreBigIntNode(Qt::ApplicationShortcut));

   // FillRule enum
   qt_ns->addConstant("OddEvenFill",              new QoreBigIntNode(Qt::OddEvenFill));
   qt_ns->addConstant("WindingFill",              new QoreBigIntNode(Qt::WindingFill));

   // MaskMode enum
   qt_ns->addConstant("MaskInColor",              new QoreBigIntNode(Qt::MaskInColor));
   qt_ns->addConstant("MaskOutColor",             new QoreBigIntNode(Qt::MaskOutColor));

   // ClipOperation enum
   qt_ns->addConstant("NoClip",                   new QoreBigIntNode(Qt::NoClip));
   qt_ns->addConstant("ReplaceClip",              new QoreBigIntNode(Qt::ReplaceClip));
   qt_ns->addConstant("IntersectClip",            new QoreBigIntNode(Qt::IntersectClip));
   qt_ns->addConstant("UniteClip",                new QoreBigIntNode(Qt::UniteClip));

   // LayoutDirection enum
   qt_ns->addConstant("LeftToRight",              new QoreBigIntNode(Qt::LeftToRight));
   qt_ns->addConstant("RightToLeft",              new QoreBigIntNode(Qt::RightToLeft));

   // ItemSelectionMode
   qt_ns->addConstant("ContainsItemShape",        new QoreBigIntNode(Qt::ContainsItemShape));
   qt_ns->addConstant("IntersectsItemShape",      new QoreBigIntNode(Qt::IntersectsItemShape));
   qt_ns->addConstant("ContainsItemBoundingRect", new QoreBigIntNode(Qt::ContainsItemBoundingRect));
   qt_ns->addConstant("IntersectsItemBoundingRect", new QoreBigIntNode(Qt::IntersectsItemBoundingRect));

   // TransformationMode enum
   qt_ns->addConstant("FastTransformation",       new QoreBigIntNode(Qt::FastTransformation));
   qt_ns->addConstant("SmoothTransformation",     new QoreBigIntNode(Qt::SmoothTransformation));

   // Axis enum
   qt_ns->addConstant("XAxis",                    new QoreBigIntNode(Qt::XAxis));
   qt_ns->addConstant("YAxis",                    new QoreBigIntNode(Qt::YAxis));
   qt_ns->addConstant("ZAxis",                    new QoreBigIntNode(Qt::ZAxis));

   // WindowModality enum
   qt_ns->addConstant("NonModal",                 new QoreBigIntNode(Qt::NonModal));
   qt_ns->addConstant("WindowModal",              new QoreBigIntNode(Qt::WindowModal));
   qt_ns->addConstant("ApplicationModal",         new QoreBigIntNode(Qt::ApplicationModal));

}

static class QoreStringNode *qt_module_init()
{
   // add hooks to qt-core functionality
   register_return_qvariant_hook(return_gui_qvariant);
   register_return_qobject_hook(return_gui_qobject);
   register_return_qevent_hook(return_gui_qevent);

   // add new types
   addBrushStyleType();
   addPenStyleType();
   
   // initialize namespace (must come after type initialization)
   init_namespace();

   builtinFunctions.add("QAPP",                       f_QAPP, QDOM_GUI);

   // QToolTip static functions
   builtinFunctions.add("QToolTip_font",              f_QToolTip_font, QDOM_GUI);
   builtinFunctions.add("QToolTip_hideText",          f_QToolTip_hideText, QDOM_GUI);
   builtinFunctions.add("QToolTip_palette",           f_QToolTip_palette, QDOM_GUI);
   builtinFunctions.add("QToolTip_setFont",           f_QToolTip_setFont, QDOM_GUI);
   builtinFunctions.add("QToolTip_setPalette",        f_QToolTip_setPalette, QDOM_GUI);
   builtinFunctions.add("QToolTip_showText",          f_QToolTip_showText, QDOM_GUI);

   // QStyleFactory static functions
   builtinFunctions.add("QStyleFactory_create",       f_QStyleFactory_create, QDOM_GUI);
   builtinFunctions.add("QStyleFactory_keys",         f_QStyleFactory_keys, QDOM_GUI);

   builtinFunctions.add("qAlpha",                     f_qAlpha, QDOM_GUI);
   builtinFunctions.add("qBlue",                      f_qBlue, QDOM_GUI);
   builtinFunctions.add("qGray",                      f_qGray, QDOM_GUI);
   builtinFunctions.add("qGray",                      f_qGray, QDOM_GUI);
   builtinFunctions.add("qGreen",                     f_qGreen, QDOM_GUI);
   builtinFunctions.add("qRed",                       f_qRed, QDOM_GUI);
   builtinFunctions.add("qRgb",                       f_qRgb, QDOM_GUI);
   builtinFunctions.add("qRgba",                      f_qRgba, QDOM_GUI);

   // add static class functions as builtin functions
   initQApplicationStaticFunctions();
   initQFontDatabaseStaticFunctions();
   initQMessageBoxStaticFunctions();
   initQPixmapStaticFunctions();
   initQFileDialogStaticFunctions();
   initQMovieStaticFunctions();
   initQColorDialogStaticFunctions();
   initQInputDialogStaticFunctions();
   initQImageWriterStaticFunctions();
   initQColorStaticFunctions();
   initQSystemTrayIconStaticFunctions();
   initQFontDialogStaticFunctions();
   initQCursorStaticFunctions();

   return 0;
}

static void qt_module_ns_init(QoreNamespace *rns, QoreNamespace *qns)
{
   qns->addInitialNamespace(qt_ns->copy());
}

static void qt_module_delete()
{
   if (C_Clipboard) {
      ExceptionSink xsink;
      C_Clipboard->deref(&xsink);
   }
   delete qt_ns;
}
