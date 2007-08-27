/*
  qt.cc
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include "QC_QObject.h"
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
#include "QC_QRect.h"
#include "QC_QPalette.h"
#include "QC_QPaintDevice.h"
#include "QC_QPainter.h"
#include "QC_QPoint.h"
#include "QC_QSize.h"
#include "QC_QRegion.h"
#include "QC_QTimer.h"
#include "QC_QLabel.h"
#include "QC_QAbstractSlider.h"
#include "QC_QSlider.h"
#include "QC_QPicture.h"
#include "QC_QPixmap.h"
#include "QC_QBitmap.h"
#include "QC_QMovie.h"
#include "QC_QShortcut.h"
#include "QC_QImage.h"
#include "QC_QDateTime.h"
#include "QC_QDate.h"
#include "QC_QTime.h"
#include "QC_QIcon.h"
#include "QC_QKeySequence.h"
#include "QC_QAction.h"
#include "QC_QActionGroup.h"

#include "qore-qt-events.h"

#include "QT_BrushStyle.h"
#include "QT_PenStyle.h"

#include "qore-qt.h"

#include <QPalette>

#include <assert.h>

QoreType *QT_BRUSHSTYLE = 0;

static class QoreString *qt_module_init();
static void qt_module_ns_init(class Namespace *rns, class Namespace *qns);
static void qt_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "qt";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "QT 4 module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://www.qoretechnologies.com/qore";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = qt_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = qt_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = qt_module_delete;
#endif

static class QoreNode *f_QObject_connect(class QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   class AbstractPrivateData *spd = p ? p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
   QoreAbstractQObject *sender = spd ? dynamic_cast<QoreAbstractQObject *>(spd) : 0;
   assert(!spd || sender);
   if (!sender) {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "first argument is not a QObject");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder1(spd, xsink);

   p = test_param(params, NT_STRING, 1);
   if (!p)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing signal string as second argument");
      return 0;
   }
   const char *signal = p->val.String->getBuffer();

   p = get_param(params, 2);
   if (!p || p->type != NT_OBJECT)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing receiving object as third argument");
      return 0;      
   }
   class AbstractPrivateData *rpd = p ? p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
   QoreAbstractQObject *receiver = rpd ? dynamic_cast<QoreAbstractQObject *>(rpd) : 0;
   assert(!rpd || receiver);
   if (!receiver) {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "third argument is not a QObject");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder2(rpd, xsink);

   // get member/slot name
   p = test_param(params, NT_STRING, 3);
   if (!p)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing slot as fourth argument");
      return 0;
   }
   const char *member = p->val.String->getBuffer();

   /*
   p = get_param(params, 4);
   int conn_type = is_nothing(p) ? Qt::AutoConnection : p->getAsInt();

   bool b = QObject::connect(sender->getQObject(), signal, receiver->getQObject(), member, (enum Qt::ConnectionType)conn_type);
   return new QoreNode(b);
   */
   receiver->connectDynamic(sender, signal, member, xsink);
   return 0;
}

static class QoreNode *f_SLOT(class QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("SLOT-ERROR", "missing slot name");
      return 0;
   }
   QoreString *str = new QoreString("1");
   str->concat(p->val.String->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return new QoreNode(str);
}

static class QoreNode *f_SIGNAL(class QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("SIGNAL-ERROR", "missing signal name");
      return 0;
   }
   QoreString *str = new QoreString("2");
   str->concat(p->val.String->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return new QoreNode(str);
}

static class QoreNode *f_TR(class QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("TR-ERROR", "missing string argument to TR()");
      return 0;
   }
   return new QoreNode(new QoreString(QObject::tr(p->val.String->getBuffer()).toUtf8().data(), QCS_UTF8));
}

static class QoreNode *f_QAPP(class QoreNode *params, class ExceptionSink *xsink)
{
   return get_qore_qapp();
}

static class QoreNode *f_qDebug(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qDebug(str->getBuffer());
   return 0;
}

static class QoreNode *f_qWarning(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qWarning(str->getBuffer());
   return 0;
}

static class QoreNode *f_qCritical(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qCritical(str->getBuffer());
   return 0;
}

static class QoreNode *f_qFatal(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qFatal(str->getBuffer());
   return 0;
}

static class QoreNode *f_qRound(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   return new QoreNode((int64)qRound(p ? p->getAsFloat() : 0.0));
}

static class QoreNode *f_qsrand(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qsrand(p ? p->getAsInt() : 0);
   return 0;
}

static class QoreNode *f_qrand(class QoreNode *params, class ExceptionSink *xsink)
{
   return new QoreNode((int64)qrand());
}

static class QoreString *qt_module_init()
{
   builtinFunctions.add("QObject_connect", f_QObject_connect);
   builtinFunctions.add("SLOT",            f_SLOT);
   builtinFunctions.add("SIGNAL",          f_SIGNAL);
   builtinFunctions.add("TR",              f_TR);
   builtinFunctions.add("QAPP",            f_QAPP);
   builtinFunctions.add("qDebug",          f_qDebug);
   builtinFunctions.add("qWarning",        f_qWarning);
   builtinFunctions.add("qCritical",       f_qCritical);
   builtinFunctions.add("qFatal",          f_qFatal);
   builtinFunctions.add("qRound",          f_qRound);
   builtinFunctions.add("qsrand",          f_qsrand);
   builtinFunctions.add("qrand",           f_qrand);
 
   addBrushStyleType();
   addPenStyleType();
  
   return 0;
}

static void qt_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   class Namespace *qt = new Namespace("Qt");

    // the order is sensitive here as child classes need the parent IDs
   class QoreClass *qobject, *qwidget, *qlayout, *qframe, *qboxlayout, *qpaintdevice, *qpixmap, *qabstractslider;
   qt->addSystemClass((qobject = initQObjectClass()));
   qt->addSystemClass(initQApplicationClass(qobject));
   qt->addSystemClass(initQMovieClass(qobject));
   qt->addSystemClass(initQActionClass(qobject));
   qt->addSystemClass(initQActionGroupClass(qobject));
   qt->addSystemClass(initQShortcutClass(qobject));

   qt->addSystemClass((qpaintdevice = initQPaintDeviceClass()));
   qt->addSystemClass(initQPictureClass(qpaintdevice));
   qt->addSystemClass(initQImageClass(qpaintdevice));

   qt->addSystemClass((qpixmap = initQPixmapClass(qpaintdevice)));
   qt->addSystemClass(initQBitmapClass(qpixmap));

   qt->addSystemClass((qwidget = initQWidgetClass(qobject, qpaintdevice)));
   qt->addSystemClass(initQPushButtonClass(qwidget));

   qt->addSystemClass((qabstractslider = initQAbstractSliderClass(qwidget)));
   qt->addSystemClass(initQSliderClass(qabstractslider));

   qt->addSystemClass((qframe = initQFrameClass(qwidget)));
   qt->addSystemClass(initQLCDNumberClass(qframe));
   qt->addSystemClass(initQLabelClass(qframe));

   qt->addSystemClass((qlayout = initQLayoutClass(qobject)));
   qt->addSystemClass(initQGridLayoutClass(qlayout));

   qt->addSystemClass((qboxlayout = initQBoxLayoutClass(qlayout)));
   qt->addSystemClass(initQVBoxLayoutClass(qboxlayout));
   qt->addSystemClass(initQHBoxLayoutClass(qboxlayout));

   qt->addSystemClass(initQTimerClass(qobject));

   qt->addSystemClass(initQRectClass());
   qt->addSystemClass(initQBrushClass());
   qt->addSystemClass(initQColorClass());
   qt->addSystemClass(initQPaletteClass());
   qt->addSystemClass(initQPainterClass());
   qt->addSystemClass(initQRegionClass());
   qt->addSystemClass(initQPointClass());
   qt->addSystemClass(initQSizeClass());

   qt->addSystemClass(initQDateTimeClass());
   qt->addSystemClass(initQDateClass());
   qt->addSystemClass(initQTimeClass());

   qt->addSystemClass(initQKeySequenceClass());
   qt->addSystemClass(initQIconClass());
   qt->addSystemClass(initQFontClass());
   qt->addSystemClass(initQMatrixClass());

   class QoreClass *qevent, *qinputevent;
   qt->addSystemClass((qevent = initQEventClass()));
   qt->addSystemClass(initQPaintEventClass(qevent));
   qt->addSystemClass(initQMoveEventClass(qevent));
   qt->addSystemClass(initQResizeEventClass(qevent));

   qt->addSystemClass((qinputevent = initQInputEventClass(qevent)));
   qt->addSystemClass(initQKeyEventClass(qinputevent));
   qt->addSystemClass(initQMouseEventClass(qinputevent));

   // ColorRole enum
   qt->addConstant("WindowText",      new QoreNode((int64)QPalette::WindowText));
   qt->addConstant("Button",          new QoreNode((int64)QPalette::Button));
   qt->addConstant("Light",           new QoreNode((int64)QPalette::Light));
   qt->addConstant("Midlight",        new QoreNode((int64)QPalette::Midlight));
   qt->addConstant("Dark",            new QoreNode((int64)QPalette::Dark));
   qt->addConstant("Mid",             new QoreNode((int64)QPalette::Mid));
   qt->addConstant("Text",            new QoreNode((int64)QPalette::Text));
   qt->addConstant("BrightText",      new QoreNode((int64)QPalette::BrightText));
   qt->addConstant("ButtonText",      new QoreNode((int64)QPalette::ButtonText));
   qt->addConstant("Base",            new QoreNode((int64)QPalette::Base));
   qt->addConstant("Window",          new QoreNode((int64)QPalette::Window));
   qt->addConstant("Shadow",          new QoreNode((int64)QPalette::Shadow));
   qt->addConstant("Highlight",       new QoreNode((int64)QPalette::Highlight));
   qt->addConstant("HighlightedText", new QoreNode((int64)QPalette::HighlightedText));
   qt->addConstant("Link",            new QoreNode((int64)QPalette::Link));
   qt->addConstant("LinkVisited",     new QoreNode((int64)QPalette::LinkVisited));
   qt->addConstant("NoRole",          new QoreNode((int64)QPalette::NoRole));
   qt->addConstant("NColorRoles",     new QoreNode((int64)QPalette::NColorRoles));
   qt->addConstant("Foreground",      new QoreNode((int64)QPalette::Foreground));
   qt->addConstant("Background",      new QoreNode((int64)QPalette::Background));
   
   // ColorGroup enum
   qt->addConstant("Active",          new QoreNode((int64)QPalette::Active));
   qt->addConstant("Disabled",        new QoreNode((int64)QPalette::Disabled));
   qt->addConstant("Inactive",        new QoreNode((int64)QPalette::Inactive));
   qt->addConstant("NColorGroups",    new QoreNode((int64)QPalette::NColorGroups));
   qt->addConstant("Current",         new QoreNode((int64)QPalette::Current));
   qt->addConstant("All",             new QoreNode((int64)QPalette::All));
   qt->addConstant("Normal",          new QoreNode((int64)QPalette::Normal));

   // add QBoxLayout namespace and constants
   class Namespace *qbl = new Namespace("QBoxLayout");
   // Direction enum
   qbl->addConstant("LeftToRight",    new QoreNode((int64)QBoxLayout::LeftToRight));
   qbl->addConstant("RightToLeft",    new QoreNode((int64)QBoxLayout::RightToLeft));
   qbl->addConstant("TopToBottom",    new QoreNode((int64)QBoxLayout::TopToBottom));
   qbl->addConstant("BottomToTop",    new QoreNode((int64)QBoxLayout::BottomToTop));

   qt->addInitialNamespace(qbl);

   // add QFont namespaces and constants
   class Namespace *qframens = new Namespace("QFrame");
   // Shadow enum
   qframens->addConstant("Plain",    new QoreNode((int64)QFrame::Plain));
   qframens->addConstant("Raised",   new QoreNode((int64)QFrame::Raised));
   qframens->addConstant("Sunken",   new QoreNode((int64)QFrame::Sunken));

   // Shape enum
   qframens->addConstant("NoFrame",      new QoreNode((int64)QFrame::NoFrame));
   qframens->addConstant("Box",          new QoreNode((int64)QFrame::Box));
   qframens->addConstant("Panel",        new QoreNode((int64)QFrame::Panel));
   qframens->addConstant("StyledPanel",  new QoreNode((int64)QFrame::StyledPanel));
   qframens->addConstant("HLine",        new QoreNode((int64)QFrame::HLine));
   qframens->addConstant("VLine",        new QoreNode((int64)QFrame::VLine));
   qframens->addConstant("WinPanel",     new QoreNode((int64)QFrame::WinPanel));

   // StyleMask
   qframens->addConstant("Shadow_Mask",  new QoreNode((int64)QFrame::Shadow_Mask));
   qframens->addConstant("Shape_Mask",   new QoreNode((int64)QFrame::Shape_Mask));

   qt->addInitialNamespace(qframens);

   // add QFont namespaces and constants
   class Namespace *qf = new Namespace("QFont");
   // Weight enum
   qf->addConstant("Light",    new QoreNode((int64)QFont::Light));
   qf->addConstant("Normal",   new QoreNode((int64)QFont::Normal));
   qf->addConstant("DemiBold", new QoreNode((int64)QFont::DemiBold));
   qf->addConstant("Bold",     new QoreNode((int64)QFont::Bold));
   qf->addConstant("Black",    new QoreNode((int64)QFont::Black));

   // StyleHint enum
   qf->addConstant("Helvetica",    new QoreNode((int64)QFont::Helvetica));
   qf->addConstant("SansSerif",    new QoreNode((int64)QFont::SansSerif));
   qf->addConstant("Times",        new QoreNode((int64)QFont::Times));
   qf->addConstant("Serif",        new QoreNode((int64)QFont::Serif));
   qf->addConstant("Courier",      new QoreNode((int64)QFont::Courier));
   qf->addConstant("TypeWriter",   new QoreNode((int64)QFont::TypeWriter));
   qf->addConstant("OldEnglish",   new QoreNode((int64)QFont::OldEnglish));
   qf->addConstant("Decorative",   new QoreNode((int64)QFont::Decorative));
   qf->addConstant("System",       new QoreNode((int64)QFont::System));
   qf->addConstant("AnyStyle",     new QoreNode((int64)QFont::AnyStyle));

   // StyleStrategy
   qf->addConstant("PreferDefault",    new QoreNode((int64)QFont::PreferDefault));
   qf->addConstant("PreferBitmap",     new QoreNode((int64)QFont::PreferBitmap));
   qf->addConstant("PreferDevice",     new QoreNode((int64)QFont::PreferDevice));
   qf->addConstant("PreferOutline",    new QoreNode((int64)QFont::PreferOutline));
   qf->addConstant("ForceOutline",     new QoreNode((int64)QFont::ForceOutline));
   qf->addConstant("PreferMatch",      new QoreNode((int64)QFont::PreferMatch));
   qf->addConstant("PreferQuality",    new QoreNode((int64)QFont::PreferQuality));
   qf->addConstant("PreferAntialias",  new QoreNode((int64)QFont::PreferAntialias));
   qf->addConstant("NoAntialias",      new QoreNode((int64)QFont::NoAntialias));
   qf->addConstant("OpenGLCompatible", new QoreNode((int64)QFont::OpenGLCompatible));
   qf->addConstant("NoFontMerging",    new QoreNode((int64)QFont::NoFontMerging));

   // Style enum
   qf->addConstant("StyleNormal",   new QoreNode((int64)QFont::StyleNormal));
   qf->addConstant("StyleItalic",   new QoreNode((int64)QFont::StyleItalic));
   qf->addConstant("StyleOblique",  new QoreNode((int64)QFont::StyleOblique));

   // Stretch enum
   qf->addConstant("UltraCondensed",  new QoreNode((int64)QFont::UltraCondensed));
   qf->addConstant("ExtraCondensed",  new QoreNode((int64)QFont::ExtraCondensed));
   qf->addConstant("Condensed",       new QoreNode((int64)QFont::Condensed));
   qf->addConstant("SemiCondensed",   new QoreNode((int64)QFont::SemiCondensed));
   qf->addConstant("Unstretched",     new QoreNode((int64)QFont::Unstretched));
   qf->addConstant("SemiExpanded",    new QoreNode((int64)QFont::SemiExpanded));
   qf->addConstant("Expanded",        new QoreNode((int64)QFont::Expanded));
   qf->addConstant("ExtraExpanded",   new QoreNode((int64)QFont::ExtraExpanded));
   qf->addConstant("UltraExpanded",   new QoreNode((int64)QFont::UltraExpanded));

   qt->addInitialNamespace(qf);

   // add QLCDNumber namespace and constants
   class Namespace *qlcdn = new Namespace("QLCDNumber");
   qlcdn->addConstant("Outline",   new QoreNode((int64)QLCDNumber::Outline));
   qlcdn->addConstant("Filled",    new QoreNode((int64)QLCDNumber::Filled));
   qlcdn->addConstant("Flat",      new QoreNode((int64)QLCDNumber::Flat));
   qlcdn->addConstant("Hex",       new QoreNode((int64)QLCDNumber::Hex));
   qlcdn->addConstant("Dec",       new QoreNode((int64)QLCDNumber::Dec));
   qlcdn->addConstant("Oct",       new QoreNode((int64)QLCDNumber::Oct));
   qlcdn->addConstant("Bin",       new QoreNode((int64)QLCDNumber::Bin));
   qt->addInitialNamespace(qlcdn);

   // add QAbstractSlider namespace and constants
   class Namespace *qas = new Namespace("QAbstractSlider");
   qas->addConstant("SliderNoAction",        new QoreNode((int64)QAbstractSlider::SliderNoAction));
   qas->addConstant("SliderSingleStepAdd",   new QoreNode((int64)QAbstractSlider::SliderSingleStepAdd));
   qas->addConstant("SliderSingleStepSub",   new QoreNode((int64)QAbstractSlider::SliderSingleStepSub));
   qas->addConstant("SliderPageStepAdd",     new QoreNode((int64)QAbstractSlider::SliderPageStepAdd));
   qas->addConstant("SliderPageStepSub",     new QoreNode((int64)QAbstractSlider::SliderPageStepSub));
   qas->addConstant("SliderToMinimum",       new QoreNode((int64)QAbstractSlider::SliderToMinimum));
   qas->addConstant("SliderToMaximum",       new QoreNode((int64)QAbstractSlider::SliderToMaximum));
   qas->addConstant("SliderMove",            new QoreNode((int64)QAbstractSlider::SliderMove));
   qt->addInitialNamespace(qas);

   // add connection enum values
   qt->addConstant("AutoConnection",           new QoreNode((int64)Qt::AutoConnection));
   qt->addConstant("DirectConnection",         new QoreNode((int64)Qt::DirectConnection));
   qt->addConstant("QueuedConnection",         new QoreNode((int64)Qt::QueuedConnection));
   qt->addConstant("AutoCompatConnection",     new QoreNode((int64)Qt::AutoCompatConnection));
   qt->addConstant("BlockingQueuedConnection", new QoreNode((int64)Qt::BlockingQueuedConnection));

   // orientation enum values
   qt->addConstant("Vertical",        new QoreNode((int64)Qt::Vertical));
   qt->addConstant("Horizontal",      new QoreNode((int64)Qt::Horizontal));

   // GlobalColor enum
   qt->addConstant("color0",            new QoreNode((int64)Qt::color0));
   qt->addConstant("color1",            new QoreNode((int64)Qt::color1));
   qt->addConstant("black",             new QoreNode((int64)Qt::black));
   qt->addConstant("white",             new QoreNode((int64)Qt::white));
   qt->addConstant("darkGray",          new QoreNode((int64)Qt::darkGray));
   qt->addConstant("gray",              new QoreNode((int64)Qt::gray));
   qt->addConstant("lightGray",         new QoreNode((int64)Qt::lightGray));
   qt->addConstant("red",               new QoreNode((int64)Qt::red));
   qt->addConstant("green",             new QoreNode((int64)Qt::green));
   qt->addConstant("blue",              new QoreNode((int64)Qt::blue));
   qt->addConstant("cyan",              new QoreNode((int64)Qt::cyan));
   qt->addConstant("magenta",           new QoreNode((int64)Qt::magenta));
   qt->addConstant("yellow",            new QoreNode((int64)Qt::yellow));
   qt->addConstant("darkRed",           new QoreNode((int64)Qt::darkRed));
   qt->addConstant("darkGreen",         new QoreNode((int64)Qt::darkGreen));
   qt->addConstant("darkBlue",          new QoreNode((int64)Qt::darkBlue));
   qt->addConstant("darkCyan",          new QoreNode((int64)Qt::darkCyan));
   qt->addConstant("darkMagenta",       new QoreNode((int64)Qt::darkMagenta));
   qt->addConstant("darkYellow",        new QoreNode((int64)Qt::darkYellow));
   qt->addConstant("transparent",       new QoreNode((int64)Qt::transparent));

   // BrushStyle enum
   qt->addConstant("NoBrush",                  make_enum(NT_BRUSHSTYLE, (int)Qt::NoBrush));
   qt->addConstant("SolidPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::SolidPattern));
   qt->addConstant("Dense1Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense1Pattern));
   qt->addConstant("Dense2Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense2Pattern));
   qt->addConstant("Dense3Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense3Pattern));
   qt->addConstant("Dense4Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense4Pattern));
   qt->addConstant("Dense5Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense5Pattern));
   qt->addConstant("Dense6Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense6Pattern));
   qt->addConstant("Dense7Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense7Pattern));
   qt->addConstant("HorPattern",               make_enum(NT_BRUSHSTYLE, (int)Qt::HorPattern));
   qt->addConstant("VerPattern",               make_enum(NT_BRUSHSTYLE, (int)Qt::VerPattern));
   qt->addConstant("CrossPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::CrossPattern));
   qt->addConstant("BDiagPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::BDiagPattern));
   qt->addConstant("FDiagPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::FDiagPattern));
   qt->addConstant("DiagCrossPattern",         make_enum(NT_BRUSHSTYLE, (int)Qt::DiagCrossPattern));
   qt->addConstant("LinearGradientPattern",    make_enum(NT_BRUSHSTYLE, (int)Qt::LinearGradientPattern));
   qt->addConstant("RadialGradientPattern",    make_enum(NT_BRUSHSTYLE, (int)Qt::RadialGradientPattern));
   qt->addConstant("ConicalGradientPattern",   make_enum(NT_BRUSHSTYLE, (int)Qt::ConicalGradientPattern));
   qt->addConstant("TexturePattern",           make_enum(NT_BRUSHSTYLE, (int)Qt::TexturePattern));

   // PenStyle enum
   qt->addConstant("NoPen",             make_enum(NT_PENSTYLE, (int)Qt::NoPen));
   qt->addConstant("SolidLine",         make_enum(NT_PENSTYLE, (int)Qt::SolidLine));
   qt->addConstant("DashLine",          make_enum(NT_PENSTYLE, (int)Qt::DashLine));
   qt->addConstant("DotLine",           make_enum(NT_PENSTYLE, (int)Qt::DotLine));
   qt->addConstant("DashDotLine",       make_enum(NT_PENSTYLE, (int)Qt::DashDotLine));
   qt->addConstant("DashDotDotLine",    make_enum(NT_PENSTYLE, (int)Qt::DashDotDotLine));
   qt->addConstant("CustomDashLine",    make_enum(NT_PENSTYLE, (int)Qt::CustomDashLine));

   // AlignmentFlag enum
   qt->addConstant("AlignLeft",                new QoreNode((int64)Qt::AlignLeft));
   qt->addConstant("AlignLeading",             new QoreNode((int64)Qt::AlignLeading));
   qt->addConstant("AlignRight",               new QoreNode((int64)Qt::AlignRight));
   qt->addConstant("AlignTrailing",            new QoreNode((int64)Qt::AlignTrailing));
   qt->addConstant("AlignHCenter",             new QoreNode((int64)Qt::AlignHCenter));
   qt->addConstant("AlignJustify",             new QoreNode((int64)Qt::AlignJustify));
   qt->addConstant("AlignAbsolute",            new QoreNode((int64)Qt::AlignAbsolute));
   qt->addConstant("AlignHorizontal_Mask",     new QoreNode((int64)Qt::AlignHorizontal_Mask));
   qt->addConstant("AlignTop",                 new QoreNode((int64)Qt::AlignTop));
   qt->addConstant("AlignBottom",              new QoreNode((int64)Qt::AlignBottom));
   qt->addConstant("AlignVCenter",             new QoreNode((int64)Qt::AlignVCenter));
   qt->addConstant("AlignVertical_Mask",       new QoreNode((int64)Qt::AlignVertical_Mask));
   qt->addConstant("AlignCenter",              new QoreNode((int64)Qt::AlignCenter));

   // MouseButton enum
   qt->addConstant("NoButton",                 new QoreNode((int64)Qt::NoButton));
   qt->addConstant("LeftButton",               new QoreNode((int64)Qt::LeftButton));
   qt->addConstant("RightButton",              new QoreNode((int64)Qt::RightButton));
   qt->addConstant("MidButton",                new QoreNode((int64)Qt::MidButton));
   qt->addConstant("XButton1",                 new QoreNode((int64)Qt::XButton1));
   qt->addConstant("XButton2",                 new QoreNode((int64)Qt::XButton2));
   qt->addConstant("MouseButtonMask",          new QoreNode((int64)Qt::MouseButtonMask));

   // Modifier enum
   qt->addConstant("META",                     new QoreNode((int64)Qt::META));
   qt->addConstant("SHIFT",                    new QoreNode((int64)Qt::SHIFT));
   qt->addConstant("CTRL",                     new QoreNode((int64)Qt::CTRL));
   qt->addConstant("ALT",                      new QoreNode((int64)Qt::ALT));
   qt->addConstant("MODIFIER_MASK",            new QoreNode((int64)Qt::MODIFIER_MASK));
   qt->addConstant("UNICODE_ACCEL",            new QoreNode((int64)Qt::UNICODE_ACCEL));

   // Key enum
   qt->addConstant("Key_Escape",               new QoreNode((int64)Qt::Key_Escape));
   qt->addConstant("Key_Tab",                  new QoreNode((int64)Qt::Key_Tab));
   qt->addConstant("Key_Backtab",              new QoreNode((int64)Qt::Key_Backtab));
   qt->addConstant("Key_Backspace",            new QoreNode((int64)Qt::Key_Backspace));
   qt->addConstant("Key_Return",               new QoreNode((int64)Qt::Key_Return));
   qt->addConstant("Key_Enter",                new QoreNode((int64)Qt::Key_Enter));
   qt->addConstant("Key_Insert",               new QoreNode((int64)Qt::Key_Insert));
   qt->addConstant("Key_Delete",               new QoreNode((int64)Qt::Key_Delete));
   qt->addConstant("Key_Pause",                new QoreNode((int64)Qt::Key_Pause));
   qt->addConstant("Key_Print",                new QoreNode((int64)Qt::Key_Print));
   qt->addConstant("Key_SysReq",               new QoreNode((int64)Qt::Key_SysReq));
   qt->addConstant("Key_Clear",                new QoreNode((int64)Qt::Key_Clear));
   qt->addConstant("Key_Home",                 new QoreNode((int64)Qt::Key_Home));
   qt->addConstant("Key_End",                  new QoreNode((int64)Qt::Key_End));
   qt->addConstant("Key_Left",                 new QoreNode((int64)Qt::Key_Left));
   qt->addConstant("Key_Up",                   new QoreNode((int64)Qt::Key_Up));
   qt->addConstant("Key_Right",                new QoreNode((int64)Qt::Key_Right));
   qt->addConstant("Key_Down",                 new QoreNode((int64)Qt::Key_Down));
   qt->addConstant("Key_PageUp",               new QoreNode((int64)Qt::Key_PageUp));
   qt->addConstant("Key_PageDown",             new QoreNode((int64)Qt::Key_PageDown));
   qt->addConstant("Key_Shift",                new QoreNode((int64)Qt::Key_Shift));
   qt->addConstant("Key_Control",              new QoreNode((int64)Qt::Key_Control));
   qt->addConstant("Key_Meta",                 new QoreNode((int64)Qt::Key_Meta));
   qt->addConstant("Key_Alt",                  new QoreNode((int64)Qt::Key_Alt));
   qt->addConstant("Key_CapsLock",             new QoreNode((int64)Qt::Key_CapsLock));
   qt->addConstant("Key_NumLock",              new QoreNode((int64)Qt::Key_NumLock));
   qt->addConstant("Key_ScrollLock",           new QoreNode((int64)Qt::Key_ScrollLock));
   qt->addConstant("Key_F1",                   new QoreNode((int64)Qt::Key_F1));
   qt->addConstant("Key_F2",                   new QoreNode((int64)Qt::Key_F2));
   qt->addConstant("Key_F3",                   new QoreNode((int64)Qt::Key_F3));
   qt->addConstant("Key_F4",                   new QoreNode((int64)Qt::Key_F4));
   qt->addConstant("Key_F5",                   new QoreNode((int64)Qt::Key_F5));
   qt->addConstant("Key_F6",                   new QoreNode((int64)Qt::Key_F6));
   qt->addConstant("Key_F7",                   new QoreNode((int64)Qt::Key_F7));
   qt->addConstant("Key_F8",                   new QoreNode((int64)Qt::Key_F8));
   qt->addConstant("Key_F9",                   new QoreNode((int64)Qt::Key_F9));
   qt->addConstant("Key_F10",                  new QoreNode((int64)Qt::Key_F10));
   qt->addConstant("Key_F11",                  new QoreNode((int64)Qt::Key_F11));
   qt->addConstant("Key_F12",                  new QoreNode((int64)Qt::Key_F12));
   qt->addConstant("Key_F13",                  new QoreNode((int64)Qt::Key_F13));
   qt->addConstant("Key_F14",                  new QoreNode((int64)Qt::Key_F14));
   qt->addConstant("Key_F15",                  new QoreNode((int64)Qt::Key_F15));
   qt->addConstant("Key_F16",                  new QoreNode((int64)Qt::Key_F16));
   qt->addConstant("Key_F17",                  new QoreNode((int64)Qt::Key_F17));
   qt->addConstant("Key_F18",                  new QoreNode((int64)Qt::Key_F18));
   qt->addConstant("Key_F19",                  new QoreNode((int64)Qt::Key_F19));
   qt->addConstant("Key_F20",                  new QoreNode((int64)Qt::Key_F20));
   qt->addConstant("Key_F21",                  new QoreNode((int64)Qt::Key_F21));
   qt->addConstant("Key_F22",                  new QoreNode((int64)Qt::Key_F22));
   qt->addConstant("Key_F23",                  new QoreNode((int64)Qt::Key_F23));
   qt->addConstant("Key_F24",                  new QoreNode((int64)Qt::Key_F24));
   qt->addConstant("Key_F25",                  new QoreNode((int64)Qt::Key_F25));
   qt->addConstant("Key_F26",                  new QoreNode((int64)Qt::Key_F26));
   qt->addConstant("Key_F27",                  new QoreNode((int64)Qt::Key_F27));
   qt->addConstant("Key_F28",                  new QoreNode((int64)Qt::Key_F28));
   qt->addConstant("Key_F29",                  new QoreNode((int64)Qt::Key_F29));
   qt->addConstant("Key_F30",                  new QoreNode((int64)Qt::Key_F30));
   qt->addConstant("Key_F31",                  new QoreNode((int64)Qt::Key_F31));
   qt->addConstant("Key_F32",                  new QoreNode((int64)Qt::Key_F32));
   qt->addConstant("Key_F33",                  new QoreNode((int64)Qt::Key_F33));
   qt->addConstant("Key_F34",                  new QoreNode((int64)Qt::Key_F34));
   qt->addConstant("Key_F35",                  new QoreNode((int64)Qt::Key_F35));
   qt->addConstant("Key_Super_L",              new QoreNode((int64)Qt::Key_Super_L));
   qt->addConstant("Key_Super_R",              new QoreNode((int64)Qt::Key_Super_R));
   qt->addConstant("Key_Menu",                 new QoreNode((int64)Qt::Key_Menu));
   qt->addConstant("Key_Hyper_L",              new QoreNode((int64)Qt::Key_Hyper_L));
   qt->addConstant("Key_Hyper_R",              new QoreNode((int64)Qt::Key_Hyper_R));
   qt->addConstant("Key_Help",                 new QoreNode((int64)Qt::Key_Help));
   qt->addConstant("Key_Direction_L",          new QoreNode((int64)Qt::Key_Direction_L));
   qt->addConstant("Key_Direction_R",          new QoreNode((int64)Qt::Key_Direction_R));
   qt->addConstant("Key_Space",                new QoreNode((int64)Qt::Key_Space));
   qt->addConstant("Key_Any",                  new QoreNode((int64)Qt::Key_Any));
   qt->addConstant("Key_Exclam",               new QoreNode((int64)Qt::Key_Exclam));
   qt->addConstant("Key_QuoteDbl",             new QoreNode((int64)Qt::Key_QuoteDbl));
   qt->addConstant("Key_NumberSign",           new QoreNode((int64)Qt::Key_NumberSign));
   qt->addConstant("Key_Dollar",               new QoreNode((int64)Qt::Key_Dollar));
   qt->addConstant("Key_Percent",              new QoreNode((int64)Qt::Key_Percent));
   qt->addConstant("Key_Ampersand",            new QoreNode((int64)Qt::Key_Ampersand));
   qt->addConstant("Key_Apostrophe",           new QoreNode((int64)Qt::Key_Apostrophe));
   qt->addConstant("Key_ParenLeft",            new QoreNode((int64)Qt::Key_ParenLeft));
   qt->addConstant("Key_ParenRight",           new QoreNode((int64)Qt::Key_ParenRight));
   qt->addConstant("Key_Asterisk",             new QoreNode((int64)Qt::Key_Asterisk));
   qt->addConstant("Key_Plus",                 new QoreNode((int64)Qt::Key_Plus));
   qt->addConstant("Key_Comma",                new QoreNode((int64)Qt::Key_Comma));
   qt->addConstant("Key_Minus",                new QoreNode((int64)Qt::Key_Minus));
   qt->addConstant("Key_Period",               new QoreNode((int64)Qt::Key_Period));
   qt->addConstant("Key_Slash",                new QoreNode((int64)Qt::Key_Slash));
   qt->addConstant("Key_0",                    new QoreNode((int64)Qt::Key_0));
   qt->addConstant("Key_1",                    new QoreNode((int64)Qt::Key_1));
   qt->addConstant("Key_2",                    new QoreNode((int64)Qt::Key_2));
   qt->addConstant("Key_3",                    new QoreNode((int64)Qt::Key_3));
   qt->addConstant("Key_4",                    new QoreNode((int64)Qt::Key_4));
   qt->addConstant("Key_5",                    new QoreNode((int64)Qt::Key_5));
   qt->addConstant("Key_6",                    new QoreNode((int64)Qt::Key_6));
   qt->addConstant("Key_7",                    new QoreNode((int64)Qt::Key_7));
   qt->addConstant("Key_8",                    new QoreNode((int64)Qt::Key_8));
   qt->addConstant("Key_9",                    new QoreNode((int64)Qt::Key_9));
   qt->addConstant("Key_Colon",                new QoreNode((int64)Qt::Key_Colon));
   qt->addConstant("Key_Semicolon",            new QoreNode((int64)Qt::Key_Semicolon));
   qt->addConstant("Key_Less",                 new QoreNode((int64)Qt::Key_Less));
   qt->addConstant("Key_Equal",                new QoreNode((int64)Qt::Key_Equal));
   qt->addConstant("Key_Greater",              new QoreNode((int64)Qt::Key_Greater));
   qt->addConstant("Key_Question",             new QoreNode((int64)Qt::Key_Question));
   qt->addConstant("Key_At",                   new QoreNode((int64)Qt::Key_At));
   qt->addConstant("Key_A",                    new QoreNode((int64)Qt::Key_A));
   qt->addConstant("Key_B",                    new QoreNode((int64)Qt::Key_B));
   qt->addConstant("Key_C",                    new QoreNode((int64)Qt::Key_C));
   qt->addConstant("Key_D",                    new QoreNode((int64)Qt::Key_D));
   qt->addConstant("Key_E",                    new QoreNode((int64)Qt::Key_E));
   qt->addConstant("Key_F",                    new QoreNode((int64)Qt::Key_F));
   qt->addConstant("Key_G",                    new QoreNode((int64)Qt::Key_G));
   qt->addConstant("Key_H",                    new QoreNode((int64)Qt::Key_H));
   qt->addConstant("Key_I",                    new QoreNode((int64)Qt::Key_I));
   qt->addConstant("Key_J",                    new QoreNode((int64)Qt::Key_J));
   qt->addConstant("Key_K",                    new QoreNode((int64)Qt::Key_K));
   qt->addConstant("Key_L",                    new QoreNode((int64)Qt::Key_L));
   qt->addConstant("Key_M",                    new QoreNode((int64)Qt::Key_M));
   qt->addConstant("Key_N",                    new QoreNode((int64)Qt::Key_N));
   qt->addConstant("Key_O",                    new QoreNode((int64)Qt::Key_O));
   qt->addConstant("Key_P",                    new QoreNode((int64)Qt::Key_P));
   qt->addConstant("Key_Q",                    new QoreNode((int64)Qt::Key_Q));
   qt->addConstant("Key_R",                    new QoreNode((int64)Qt::Key_R));
   qt->addConstant("Key_S",                    new QoreNode((int64)Qt::Key_S));
   qt->addConstant("Key_T",                    new QoreNode((int64)Qt::Key_T));
   qt->addConstant("Key_U",                    new QoreNode((int64)Qt::Key_U));
   qt->addConstant("Key_V",                    new QoreNode((int64)Qt::Key_V));
   qt->addConstant("Key_W",                    new QoreNode((int64)Qt::Key_W));
   qt->addConstant("Key_X",                    new QoreNode((int64)Qt::Key_X));
   qt->addConstant("Key_Y",                    new QoreNode((int64)Qt::Key_Y));
   qt->addConstant("Key_Z",                    new QoreNode((int64)Qt::Key_Z));
   qt->addConstant("Key_BracketLeft",          new QoreNode((int64)Qt::Key_BracketLeft));
   qt->addConstant("Key_Backslash",            new QoreNode((int64)Qt::Key_Backslash));
   qt->addConstant("Key_BracketRight",         new QoreNode((int64)Qt::Key_BracketRight));
   qt->addConstant("Key_AsciiCircum",          new QoreNode((int64)Qt::Key_AsciiCircum));
   qt->addConstant("Key_Underscore",           new QoreNode((int64)Qt::Key_Underscore));
   qt->addConstant("Key_QuoteLeft",            new QoreNode((int64)Qt::Key_QuoteLeft));
   qt->addConstant("Key_BraceLeft",            new QoreNode((int64)Qt::Key_BraceLeft));
   qt->addConstant("Key_Bar",                  new QoreNode((int64)Qt::Key_Bar));
   qt->addConstant("Key_BraceRight",           new QoreNode((int64)Qt::Key_BraceRight));
   qt->addConstant("Key_AsciiTilde",           new QoreNode((int64)Qt::Key_AsciiTilde));
   qt->addConstant("Key_nobreakspace",         new QoreNode((int64)Qt::Key_nobreakspace));
   qt->addConstant("Key_exclamdown",           new QoreNode((int64)Qt::Key_exclamdown));
   qt->addConstant("Key_cent",                 new QoreNode((int64)Qt::Key_cent));
   qt->addConstant("Key_sterling",             new QoreNode((int64)Qt::Key_sterling));
   qt->addConstant("Key_currency",             new QoreNode((int64)Qt::Key_currency));
   qt->addConstant("Key_yen",                  new QoreNode((int64)Qt::Key_yen));
   qt->addConstant("Key_brokenbar",            new QoreNode((int64)Qt::Key_brokenbar));
   qt->addConstant("Key_section",              new QoreNode((int64)Qt::Key_section));
   qt->addConstant("Key_diaeresis",            new QoreNode((int64)Qt::Key_diaeresis));
   qt->addConstant("Key_copyright",            new QoreNode((int64)Qt::Key_copyright));
   qt->addConstant("Key_ordfeminine",          new QoreNode((int64)Qt::Key_ordfeminine));
   qt->addConstant("Key_guillemotleft",        new QoreNode((int64)Qt::Key_guillemotleft));
   qt->addConstant("Key_notsign",              new QoreNode((int64)Qt::Key_notsign));
   qt->addConstant("Key_hyphen",               new QoreNode((int64)Qt::Key_hyphen));
   qt->addConstant("Key_registered",           new QoreNode((int64)Qt::Key_registered));
   qt->addConstant("Key_macron",               new QoreNode((int64)Qt::Key_macron));
   qt->addConstant("Key_degree",               new QoreNode((int64)Qt::Key_degree));
   qt->addConstant("Key_plusminus",            new QoreNode((int64)Qt::Key_plusminus));
   qt->addConstant("Key_twosuperior",          new QoreNode((int64)Qt::Key_twosuperior));
   qt->addConstant("Key_threesuperior",        new QoreNode((int64)Qt::Key_threesuperior));
   qt->addConstant("Key_acute",                new QoreNode((int64)Qt::Key_acute));
   qt->addConstant("Key_mu",                   new QoreNode((int64)Qt::Key_mu));
   qt->addConstant("Key_paragraph",            new QoreNode((int64)Qt::Key_paragraph));
   qt->addConstant("Key_periodcentered",       new QoreNode((int64)Qt::Key_periodcentered));
   qt->addConstant("Key_cedilla",              new QoreNode((int64)Qt::Key_cedilla));
   qt->addConstant("Key_onesuperior",          new QoreNode((int64)Qt::Key_onesuperior));
   qt->addConstant("Key_masculine",            new QoreNode((int64)Qt::Key_masculine));
   qt->addConstant("Key_guillemotright",       new QoreNode((int64)Qt::Key_guillemotright));
   qt->addConstant("Key_onequarter",           new QoreNode((int64)Qt::Key_onequarter));
   qt->addConstant("Key_onehalf",              new QoreNode((int64)Qt::Key_onehalf));
   qt->addConstant("Key_threequarters",        new QoreNode((int64)Qt::Key_threequarters));
   qt->addConstant("Key_questiondown",         new QoreNode((int64)Qt::Key_questiondown));
   qt->addConstant("Key_Agrave",               new QoreNode((int64)Qt::Key_Agrave));
   qt->addConstant("Key_Aacute",               new QoreNode((int64)Qt::Key_Aacute));
   qt->addConstant("Key_Acircumflex",          new QoreNode((int64)Qt::Key_Acircumflex));
   qt->addConstant("Key_Atilde",               new QoreNode((int64)Qt::Key_Atilde));
   qt->addConstant("Key_Adiaeresis",           new QoreNode((int64)Qt::Key_Adiaeresis));
   qt->addConstant("Key_Aring",                new QoreNode((int64)Qt::Key_Aring));
   qt->addConstant("Key_AE",                   new QoreNode((int64)Qt::Key_AE));
   qt->addConstant("Key_Ccedilla",             new QoreNode((int64)Qt::Key_Ccedilla));
   qt->addConstant("Key_Egrave",               new QoreNode((int64)Qt::Key_Egrave));
   qt->addConstant("Key_Eacute",               new QoreNode((int64)Qt::Key_Eacute));
   qt->addConstant("Key_Ecircumflex",          new QoreNode((int64)Qt::Key_Ecircumflex));
   qt->addConstant("Key_Ediaeresis",           new QoreNode((int64)Qt::Key_Ediaeresis));
   qt->addConstant("Key_Igrave",               new QoreNode((int64)Qt::Key_Igrave));
   qt->addConstant("Key_Iacute",               new QoreNode((int64)Qt::Key_Iacute));
   qt->addConstant("Key_Icircumflex",          new QoreNode((int64)Qt::Key_Icircumflex));
   qt->addConstant("Key_Idiaeresis",           new QoreNode((int64)Qt::Key_Idiaeresis));
   qt->addConstant("Key_ETH",                  new QoreNode((int64)Qt::Key_ETH));
   qt->addConstant("Key_Ntilde",               new QoreNode((int64)Qt::Key_Ntilde));
   qt->addConstant("Key_Ograve",               new QoreNode((int64)Qt::Key_Ograve));
   qt->addConstant("Key_Oacute",               new QoreNode((int64)Qt::Key_Oacute));
   qt->addConstant("Key_Ocircumflex",          new QoreNode((int64)Qt::Key_Ocircumflex));
   qt->addConstant("Key_Otilde",               new QoreNode((int64)Qt::Key_Otilde));
   qt->addConstant("Key_Odiaeresis",           new QoreNode((int64)Qt::Key_Odiaeresis));
   qt->addConstant("Key_multiply",             new QoreNode((int64)Qt::Key_multiply));
   qt->addConstant("Key_Ooblique",             new QoreNode((int64)Qt::Key_Ooblique));
   qt->addConstant("Key_Ugrave",               new QoreNode((int64)Qt::Key_Ugrave));
   qt->addConstant("Key_Uacute",               new QoreNode((int64)Qt::Key_Uacute));
   qt->addConstant("Key_Ucircumflex",          new QoreNode((int64)Qt::Key_Ucircumflex));
   qt->addConstant("Key_Udiaeresis",           new QoreNode((int64)Qt::Key_Udiaeresis));
   qt->addConstant("Key_Yacute",               new QoreNode((int64)Qt::Key_Yacute));
   qt->addConstant("Key_THORN",                new QoreNode((int64)Qt::Key_THORN));
   qt->addConstant("Key_ssharp",               new QoreNode((int64)Qt::Key_ssharp));
   qt->addConstant("Key_division",             new QoreNode((int64)Qt::Key_division));
   qt->addConstant("Key_ydiaeresis",           new QoreNode((int64)Qt::Key_ydiaeresis));
   qt->addConstant("Key_AltGr",                new QoreNode((int64)Qt::Key_AltGr));
   qt->addConstant("Key_Multi_key",            new QoreNode((int64)Qt::Key_Multi_key));
   qt->addConstant("Key_Codeinput",            new QoreNode((int64)Qt::Key_Codeinput));
   qt->addConstant("Key_SingleCandidate",      new QoreNode((int64)Qt::Key_SingleCandidate));
   qt->addConstant("Key_MultipleCandidate",    new QoreNode((int64)Qt::Key_MultipleCandidate));
   qt->addConstant("Key_PreviousCandidate",    new QoreNode((int64)Qt::Key_PreviousCandidate));
   qt->addConstant("Key_Mode_switch",          new QoreNode((int64)Qt::Key_Mode_switch));
   qt->addConstant("Key_Kanji",                new QoreNode((int64)Qt::Key_Kanji));
   qt->addConstant("Key_Muhenkan",             new QoreNode((int64)Qt::Key_Muhenkan));
   qt->addConstant("Key_Henkan",               new QoreNode((int64)Qt::Key_Henkan));
   qt->addConstant("Key_Romaji",               new QoreNode((int64)Qt::Key_Romaji));
   qt->addConstant("Key_Hiragana",             new QoreNode((int64)Qt::Key_Hiragana));
   qt->addConstant("Key_Katakana",             new QoreNode((int64)Qt::Key_Katakana));
   qt->addConstant("Key_Hiragana_Katakana",    new QoreNode((int64)Qt::Key_Hiragana_Katakana));
   qt->addConstant("Key_Zenkaku",              new QoreNode((int64)Qt::Key_Zenkaku));
   qt->addConstant("Key_Hankaku",              new QoreNode((int64)Qt::Key_Hankaku));
   qt->addConstant("Key_Zenkaku_Hankaku",      new QoreNode((int64)Qt::Key_Zenkaku_Hankaku));
   qt->addConstant("Key_Touroku",              new QoreNode((int64)Qt::Key_Touroku));
   qt->addConstant("Key_Massyo",               new QoreNode((int64)Qt::Key_Massyo));
   qt->addConstant("Key_Kana_Lock",            new QoreNode((int64)Qt::Key_Kana_Lock));
   qt->addConstant("Key_Kana_Shift",           new QoreNode((int64)Qt::Key_Kana_Shift));
   qt->addConstant("Key_Eisu_Shift",           new QoreNode((int64)Qt::Key_Eisu_Shift));
   qt->addConstant("Key_Eisu_toggle",          new QoreNode((int64)Qt::Key_Eisu_toggle));
   qt->addConstant("Key_Hangul",               new QoreNode((int64)Qt::Key_Hangul));
   qt->addConstant("Key_Hangul_Start",         new QoreNode((int64)Qt::Key_Hangul_Start));
   qt->addConstant("Key_Hangul_End",           new QoreNode((int64)Qt::Key_Hangul_End));
   qt->addConstant("Key_Hangul_Hanja",         new QoreNode((int64)Qt::Key_Hangul_Hanja));
   qt->addConstant("Key_Hangul_Jamo",          new QoreNode((int64)Qt::Key_Hangul_Jamo));
   qt->addConstant("Key_Hangul_Romaja",        new QoreNode((int64)Qt::Key_Hangul_Romaja));
   qt->addConstant("Key_Hangul_Jeonja",        new QoreNode((int64)Qt::Key_Hangul_Jeonja));
   qt->addConstant("Key_Hangul_Banja",         new QoreNode((int64)Qt::Key_Hangul_Banja));
   qt->addConstant("Key_Hangul_PreHanja",      new QoreNode((int64)Qt::Key_Hangul_PreHanja));
   qt->addConstant("Key_Hangul_PostHanja",     new QoreNode((int64)Qt::Key_Hangul_PostHanja));
   qt->addConstant("Key_Hangul_Special",       new QoreNode((int64)Qt::Key_Hangul_Special));
   qt->addConstant("Key_Dead_Grave",           new QoreNode((int64)Qt::Key_Dead_Grave));
   qt->addConstant("Key_Dead_Acute",           new QoreNode((int64)Qt::Key_Dead_Acute));
   qt->addConstant("Key_Dead_Circumflex",      new QoreNode((int64)Qt::Key_Dead_Circumflex));
   qt->addConstant("Key_Dead_Tilde",           new QoreNode((int64)Qt::Key_Dead_Tilde));
   qt->addConstant("Key_Dead_Macron",          new QoreNode((int64)Qt::Key_Dead_Macron));
   qt->addConstant("Key_Dead_Breve",           new QoreNode((int64)Qt::Key_Dead_Breve));
   qt->addConstant("Key_Dead_Abovedot",        new QoreNode((int64)Qt::Key_Dead_Abovedot));
   qt->addConstant("Key_Dead_Diaeresis",       new QoreNode((int64)Qt::Key_Dead_Diaeresis));
   qt->addConstant("Key_Dead_Abovering",       new QoreNode((int64)Qt::Key_Dead_Abovering));
   qt->addConstant("Key_Dead_Doubleacute",     new QoreNode((int64)Qt::Key_Dead_Doubleacute));
   qt->addConstant("Key_Dead_Caron",           new QoreNode((int64)Qt::Key_Dead_Caron));
   qt->addConstant("Key_Dead_Cedilla",         new QoreNode((int64)Qt::Key_Dead_Cedilla));
   qt->addConstant("Key_Dead_Ogonek",          new QoreNode((int64)Qt::Key_Dead_Ogonek));
   qt->addConstant("Key_Dead_Iota",            new QoreNode((int64)Qt::Key_Dead_Iota));
   qt->addConstant("Key_Dead_Voiced_Sound",    new QoreNode((int64)Qt::Key_Dead_Voiced_Sound));
   qt->addConstant("Key_Dead_Semivoiced_Sound", new QoreNode((int64)Qt::Key_Dead_Semivoiced_Sound));
   qt->addConstant("Key_Dead_Belowdot",        new QoreNode((int64)Qt::Key_Dead_Belowdot));
   qt->addConstant("Key_Dead_Hook",            new QoreNode((int64)Qt::Key_Dead_Hook));
   qt->addConstant("Key_Dead_Horn",            new QoreNode((int64)Qt::Key_Dead_Horn));
   qt->addConstant("Key_Back",                 new QoreNode((int64)Qt::Key_Back));
   qt->addConstant("Key_Forward",              new QoreNode((int64)Qt::Key_Forward));
   qt->addConstant("Key_Stop",                 new QoreNode((int64)Qt::Key_Stop));
   qt->addConstant("Key_Refresh",              new QoreNode((int64)Qt::Key_Refresh));
   qt->addConstant("Key_VolumeDown",           new QoreNode((int64)Qt::Key_VolumeDown));
   qt->addConstant("Key_VolumeMute",           new QoreNode((int64)Qt::Key_VolumeMute));
   qt->addConstant("Key_VolumeUp",             new QoreNode((int64)Qt::Key_VolumeUp));
   qt->addConstant("Key_BassBoost",            new QoreNode((int64)Qt::Key_BassBoost));
   qt->addConstant("Key_BassUp",               new QoreNode((int64)Qt::Key_BassUp));
   qt->addConstant("Key_BassDown",             new QoreNode((int64)Qt::Key_BassDown));
   qt->addConstant("Key_TrebleUp",             new QoreNode((int64)Qt::Key_TrebleUp));
   qt->addConstant("Key_TrebleDown",           new QoreNode((int64)Qt::Key_TrebleDown));
   qt->addConstant("Key_MediaPlay",            new QoreNode((int64)Qt::Key_MediaPlay));
   qt->addConstant("Key_MediaStop",            new QoreNode((int64)Qt::Key_MediaStop));
   qt->addConstant("Key_MediaPrevious",        new QoreNode((int64)Qt::Key_MediaPrevious));
   qt->addConstant("Key_MediaNext",            new QoreNode((int64)Qt::Key_MediaNext));
   qt->addConstant("Key_MediaRecord",          new QoreNode((int64)Qt::Key_MediaRecord));
   qt->addConstant("Key_HomePage",             new QoreNode((int64)Qt::Key_HomePage));
   qt->addConstant("Key_Favorites",            new QoreNode((int64)Qt::Key_Favorites));
   qt->addConstant("Key_Search",               new QoreNode((int64)Qt::Key_Search));
   qt->addConstant("Key_Standby",              new QoreNode((int64)Qt::Key_Standby));
   qt->addConstant("Key_OpenUrl",              new QoreNode((int64)Qt::Key_OpenUrl));
   qt->addConstant("Key_LaunchMail",           new QoreNode((int64)Qt::Key_LaunchMail));
   qt->addConstant("Key_LaunchMedia",          new QoreNode((int64)Qt::Key_LaunchMedia));
   qt->addConstant("Key_Launch0",              new QoreNode((int64)Qt::Key_Launch0));
   qt->addConstant("Key_Launch1",              new QoreNode((int64)Qt::Key_Launch1));
   qt->addConstant("Key_Launch2",              new QoreNode((int64)Qt::Key_Launch2));
   qt->addConstant("Key_Launch3",              new QoreNode((int64)Qt::Key_Launch3));
   qt->addConstant("Key_Launch4",              new QoreNode((int64)Qt::Key_Launch4));
   qt->addConstant("Key_Launch5",              new QoreNode((int64)Qt::Key_Launch5));
   qt->addConstant("Key_Launch6",              new QoreNode((int64)Qt::Key_Launch6));
   qt->addConstant("Key_Launch7",              new QoreNode((int64)Qt::Key_Launch7));
   qt->addConstant("Key_Launch8",              new QoreNode((int64)Qt::Key_Launch8));
   qt->addConstant("Key_Launch9",              new QoreNode((int64)Qt::Key_Launch9));
   qt->addConstant("Key_LaunchA",              new QoreNode((int64)Qt::Key_LaunchA));
   qt->addConstant("Key_LaunchB",              new QoreNode((int64)Qt::Key_LaunchB));
   qt->addConstant("Key_LaunchC",              new QoreNode((int64)Qt::Key_LaunchC));
   qt->addConstant("Key_LaunchD",              new QoreNode((int64)Qt::Key_LaunchD));
   qt->addConstant("Key_LaunchE",              new QoreNode((int64)Qt::Key_LaunchE));
   qt->addConstant("Key_LaunchF",              new QoreNode((int64)Qt::Key_LaunchF));
   qt->addConstant("Key_MediaLast",            new QoreNode((int64)Qt::Key_MediaLast));
   qt->addConstant("Key_Select",               new QoreNode((int64)Qt::Key_Select));
   qt->addConstant("Key_Yes",                  new QoreNode((int64)Qt::Key_Yes));
   qt->addConstant("Key_No",                   new QoreNode((int64)Qt::Key_No));
   qt->addConstant("Key_Cancel",               new QoreNode((int64)Qt::Key_Cancel));
   qt->addConstant("Key_Printer",              new QoreNode((int64)Qt::Key_Printer));
   qt->addConstant("Key_Execute",              new QoreNode((int64)Qt::Key_Execute));
   qt->addConstant("Key_Sleep",                new QoreNode((int64)Qt::Key_Sleep));
   qt->addConstant("Key_Play",                 new QoreNode((int64)Qt::Key_Play));
   qt->addConstant("Key_Zoom",                 new QoreNode((int64)Qt::Key_Zoom));
   qt->addConstant("Key_Context1",             new QoreNode((int64)Qt::Key_Context1));
   qt->addConstant("Key_Context2",             new QoreNode((int64)Qt::Key_Context2));
   qt->addConstant("Key_Context3",             new QoreNode((int64)Qt::Key_Context3));
   qt->addConstant("Key_Context4",             new QoreNode((int64)Qt::Key_Context4));
   qt->addConstant("Key_Call",                 new QoreNode((int64)Qt::Key_Call));
   qt->addConstant("Key_Hangup",               new QoreNode((int64)Qt::Key_Hangup));
   qt->addConstant("Key_Flip",                 new QoreNode((int64)Qt::Key_Flip));
   qt->addConstant("Key_unknown",              new QoreNode((int64)Qt::Key_unknown));

   qns->addInitialNamespace(qt);
}

static void qt_module_delete()
{
   // nothing to do
}
