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
#include "QC_QVBoxLayout.h"

#include <assert.h>

// abstract class IDs
DLLLOCAL int CID_QLAYOUT;

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

class QoreNode *f_QObject_connect(class QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   class AbstractPrivateData *spd = p ? p->val.object->getReferencedPrivateDataFromMetaClass(CID_QOBJECT, xsink) : NULL;
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
   class AbstractPrivateData *rpd = p ? p->val.object->getReferencedPrivateDataFromMetaClass(CID_QOBJECT, xsink) : NULL;
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

   p = get_param(params, 4);
   int conn_type = is_nothing(p) ? Qt::AutoConnection : p->getAsInt();

   bool b = QObject::connect(sender->getQObject(), signal, receiver->getQObject(), member, (enum Qt::ConnectionType)conn_type);
   return new QoreNode(b);
}

class QoreNode *f_SLOT(class QoreNode *params, class ExceptionSink *xsink)
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

class QoreNode *f_SIGNAL(class QoreNode *params, class ExceptionSink *xsink)
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

class QoreNode *f_TR(class QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("TR-ERROR", "missing string argument to TR()");
      return 0;
   }
   return new QoreNode(new QoreString(QObject::tr(p->val.String->getBuffer()).toUtf8().data()));
}

class QoreNode *f_QAPP(class QoreNode *params, class ExceptionSink *xsink)
{
   return get_qore_qapp();
}

static class QoreString *qt_module_init()
{
   builtinFunctions.add("QObject_connect", f_QObject_connect);
   builtinFunctions.add("SLOT",            f_SLOT);
   builtinFunctions.add("SIGNAL",          f_SIGNAL);
   builtinFunctions.add("TR",              f_TR);
   builtinFunctions.add("QAPP",            f_QAPP);
   
   return 0;
}

static void qt_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   class Namespace *qt = new Namespace("Qt");

   // get abstract class IDs
   CID_QLAYOUT = get_abstract_class_id();

   // the order is sensitive here as child classes need the parent IDs
   qt->addSystemClass(initQObjectClass());
   qt->addSystemClass(initQApplicationClass());
   qt->addSystemClass(initQWidgetClass());
   qt->addSystemClass(initQPushButtonClass());
   qt->addSystemClass(initQFrameClass());
   qt->addSystemClass(initQLCDNumberClass());
   qt->addSystemClass(initQSliderClass());
   qt->addSystemClass(initQVBoxLayoutClass());
   qt->addSystemClass(initQFontClass());
   
   // add QFont namespaces and constants
   class Namespace *qf = new Namespace("QFont");
   qf->addConstant("Light",    new QoreNode((int64)QFont::Light));
   qf->addConstant("Normal",   new QoreNode((int64)QFont::Normal));
   qf->addConstant("DemiBold", new QoreNode((int64)QFont::DemiBold));
   qf->addConstant("Bold",     new QoreNode((int64)QFont::Bold));
   qf->addConstant("Black",    new QoreNode((int64)QFont::Black));
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

   qns->addInitialNamespace(qt);
}

static void qt_module_delete()
{
   // nothing to do
}
