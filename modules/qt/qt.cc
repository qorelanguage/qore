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
#include "QC_QPaintEvent.h"
#include "QC_QPainter.h"
#include "QC_QPoint.h"
#include "QC_QRegion.h"
#include "QC_QTimer.h"

#include <QPalette>

#include <assert.h>

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
   
   return 0;
}

static void qt_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   class Namespace *qt = new Namespace("Qt");

    // the order is sensitive here as child classes need the parent IDs
   class QoreClass *qobject, *qwidget, *qlayout, *qframe, *qboxlayout, *qpaintdevice;
   qt->addSystemClass((qobject = initQObjectClass()));
   qt->addSystemClass(initQApplicationClass(qobject));

   qt->addSystemClass((qpaintdevice = initQPaintDeviceClass()));

   qt->addSystemClass((qwidget = initQWidgetClass(qobject, qpaintdevice)));
   qt->addSystemClass(initQPushButtonClass(qwidget));

   qt->addSystemClass((qframe = initQFrameClass(qwidget)));
   qt->addSystemClass(initQLCDNumberClass(qframe));
   qt->addSystemClass(initQSliderClass(qframe));

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
   qt->addSystemClass(initQPaintEventClass());
   qt->addSystemClass(initQRegionClass());
   qt->addSystemClass(initQPointClass());

   qt->addSystemClass(initQFontClass());

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

   // PenStyle enum
   qt->addConstant("NoPen",             new QoreNode((int64)Qt::NoPen));
   qt->addConstant("SolidLine",         new QoreNode((int64)Qt::SolidLine));
   qt->addConstant("DashLine",          new QoreNode((int64)Qt::DashLine));
   qt->addConstant("DotLine",           new QoreNode((int64)Qt::DotLine));
   qt->addConstant("DashDotLine",       new QoreNode((int64)Qt::DashDotLine));
   qt->addConstant("DashDotDotLine",    new QoreNode((int64)Qt::DashDotDotLine));
   qt->addConstant("CustomDashLine",    new QoreNode((int64)Qt::CustomDashLine));

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
   qt->addConstant("NoBrush",                  new QoreNode((int64)Qt::NoBrush));
   qt->addConstant("SolidPattern",             new QoreNode((int64)Qt::SolidPattern));
   qt->addConstant("Dense1Pattern",            new QoreNode((int64)Qt::Dense1Pattern));
   qt->addConstant("Dense2Pattern",            new QoreNode((int64)Qt::Dense2Pattern));
   qt->addConstant("Dense3Pattern",            new QoreNode((int64)Qt::Dense3Pattern));
   qt->addConstant("Dense4Pattern",            new QoreNode((int64)Qt::Dense4Pattern));
   qt->addConstant("Dense5Pattern",            new QoreNode((int64)Qt::Dense5Pattern));
   qt->addConstant("Dense6Pattern",            new QoreNode((int64)Qt::Dense6Pattern));
   qt->addConstant("Dense7Pattern",            new QoreNode((int64)Qt::Dense7Pattern));
   qt->addConstant("HorPattern",               new QoreNode((int64)Qt::HorPattern));
   qt->addConstant("VerPattern",               new QoreNode((int64)Qt::VerPattern));
   qt->addConstant("CrossPattern",             new QoreNode((int64)Qt::CrossPattern));
   qt->addConstant("BDiagPattern",             new QoreNode((int64)Qt::BDiagPattern));
   qt->addConstant("FDiagPattern",             new QoreNode((int64)Qt::FDiagPattern));
   qt->addConstant("DiagCrossPattern",         new QoreNode((int64)Qt::DiagCrossPattern));
   qt->addConstant("LinearGradientPattern",    new QoreNode((int64)Qt::LinearGradientPattern));
   qt->addConstant("RadialGradientPattern",    new QoreNode((int64)Qt::RadialGradientPattern));
   qt->addConstant("ConicalGradientPattern",   new QoreNode((int64)Qt::ConicalGradientPattern));
   qt->addConstant("TexturePattern",           new QoreNode((int64)Qt::TexturePattern));

   qns->addInitialNamespace(qt);
}

static void qt_module_delete()
{
   // nothing to do
}
