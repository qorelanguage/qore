/*
  qt-core-module.cc
  
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

#include "QC_QObject.h"
#include "QC_QRect.h"
#include "QC_QRectF.h"
#include "QC_QPoint.h"
#include "QC_QPointF.h"
#include "QC_QSize.h"
#include "QC_QDateTime.h"
#include "QC_QDate.h"
#include "QC_QTime.h"
#include "QC_QLine.h"
#include "QC_QLineF.h"
#include "QC_QLocale.h"
#include "QC_QByteArray.h"
#include "QC_QUrl.h"
#include "QC_QVariant.h"
#include "QC_QDir.h"
#include "QC_QMetaObject.h"
#include "QC_QRegExp.h"
#include "QC_QFileInfo.h"
#include "QC_QIODevice.h"
#include "QC_QChar.h"
#include "QC_QMimeData.h"
#include "QC_QEvent.h"
#include "QC_QCoreApplication.h"
#include "QC_QTimeLine.h"
#include "QC_QAbstractItemModel.h"
#include "QC_QBasicTimer.h"
#include "QC_QLibraryInfo.h"
#include "QC_QTimer.h"
#include "QC_QModelIndex.h"
#include "QC_QTranslator.h"

static QoreStringNode *qt_core_module_init();
static void qt_core_module_ns_init(QoreNamespace *rns, QoreNamespace *qns);
static void qt_core_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "qt-core";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "QT 4 Core module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://www.qoretechnologies.com/qore";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = qt_core_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = qt_core_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = qt_core_module_delete;
DLLEXPORT qore_license_t qore_module_license = QL_GPL;
#endif

static class AbstractQoreNode *f_QObject_connect(const QoreListNode *params, class ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   class AbstractPrivateData *spd = p ? p->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
   QoreAbstractQObject *sender = spd ? dynamic_cast<QoreAbstractQObject *>(spd) : 0;
   assert(!spd || sender);
   if (!sender) {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "first argument is not a QObject");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder1(spd, xsink);

   const QoreStringNode *str = test_string_param(params, 1);
   if (!str)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing signal string as second argument");
      return 0;
   }
   const char *signal = str->getBuffer();

   const QoreObject *o = test_object_param(params, 2);
   if (!o)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing receiving object as third argument");
      return 0;      
   }
   class AbstractPrivateData *rpd = o ? o->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
   QoreAbstractQObject *receiver = rpd ? dynamic_cast<QoreAbstractQObject *>(rpd) : 0;
   assert(!rpd || receiver);
   if (!receiver) {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "third argument is not a QObject");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder2(rpd, xsink);

   // get member/slot name
   str = test_string_param(params, 3);
   if (!str)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing slot as fourth argument");
      return 0;
   }
   const char *member = str->getBuffer();

   /*
   p = get_param(params, 4);
   int conn_type = is_nothing(p) ? Qt::AutoConnection : p->getAsInt();

   bool b = QObject::connect(sender->getQObject(), signal, receiver->getQObject(), member, (enum Qt::ConnectionType)conn_type);
   return get_bool_node(b);
   */
   receiver->connectDynamic(sender, signal, member, xsink);
   return 0;
}

static class AbstractQoreNode *f_SLOT(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get slot name
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("SLOT-ERROR", "missing slot name");
      return 0;
   }
   QoreStringNode *str = new QoreStringNode("1");
   str->concat(p->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return str;
}

static class AbstractQoreNode *f_SIGNAL(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get slot name
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("SIGNAL-ERROR", "missing signal name");
      return 0;
   }
   QoreStringNode *str = new QoreStringNode("2");
   str->concat(p->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return str;
}

static class AbstractQoreNode *f_TR(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get slot name
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("TR-ERROR", "missing string argument to TR()");
      return 0;
   }
   return new QoreStringNode(QObject::tr(p->getBuffer()).toUtf8().data(), QCS_UTF8);
}

static class AbstractQoreNode *f_qDebug(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qDebug(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qWarning(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qWarning(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qCritical(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qCritical(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qFatal(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qFatal(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qRound(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreBigIntNode(qRound(p ? p->getAsFloat() : 0.0));
}

static class AbstractQoreNode *f_qsrand(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qsrand(p ? p->getAsInt() : 0);
   return 0;
}

static class AbstractQoreNode *f_qrand(const QoreListNode *params, class ExceptionSink *xsink)
{
   return new QoreBigIntNode(qrand());
}

static class AbstractQoreNode *f_qSwap(const QoreListNode *params, class ExceptionSink *xsink)
{
   const ReferenceNode *r0 = test_reference_param(params, 0);
   if (!r0) {
      xsink->raiseException("QSWAP-ERROR", "first argument must be a reference");
      return 0;
   }

   const ReferenceNode *r1 = test_reference_param(params, 1);
   if (!r1) {
      xsink->raiseException("QSWAP-ERROR", "second argument must be a reference");
      return 0;
   }

   AutoVLock vl(xsink);
   ReferenceHelper ref0(r0, vl, xsink);
   if (!ref0)
      return 0;

   ReferenceHelper ref1(r1, vl, xsink);
   if (!ref1)
      return 0;

   ref0.swap(ref1);
   return 0;
}


static QoreNamespace qt_ns("Qt");

static void init_namespace()
{
    // the order is sensitive here as child classes need the parent IDs
   QoreClass *qobject, *qcoreapplication;

   qt_ns.addSystemClass((qobject = initQObjectClass()));
   qt_ns.addSystemClass((qcoreapplication = initQCoreApplicationClass(qobject)));
   qt_ns.addSystemClass(initQRectClass());
   qt_ns.addSystemClass(initQRectFClass());

   qt_ns.addSystemClass(initQPointClass());
   qt_ns.addSystemClass(initQPointFClass());

   qt_ns.addSystemClass(initQLineClass());
   qt_ns.addSystemClass(initQLineFClass());

   qt_ns.addSystemClass(initQSizeClass());

   qt_ns.addSystemClass(initQDateTimeClass());
   qt_ns.addSystemClass(initQDateClass());
   qt_ns.addSystemClass(initQTimeClass());

   qt_ns.addSystemClass(initQFileInfoClass());
   qt_ns.addSystemClass(initQIODeviceClass(QC_QObject));

   qt_ns.addInitialNamespace(initQDirNS());
   qt_ns.addSystemClass(initQMetaObjectClass());
   qt_ns.addSystemClass(initQRegExpClass());

   qt_ns.addSystemClass(initQAbstractItemModelClass(QC_QObject));

   qt_ns.addSystemClass(initQBasicTimerClass());

   qt_ns.addSystemClass(initQByteArrayClass());
   qt_ns.addSystemClass(initQUrlClass());
   qt_ns.addSystemClass(initQVariantClass());

   qt_ns.addInitialNamespace(initQCharNS());

   qt_ns.addInitialNamespace(initQLibraryInfoNS());

   qt_ns.addInitialNamespace(initQEventNS());

   qt_ns.addInitialNamespace(initQLocaleNS());

   qt_ns.addSystemClass(initQMimeDataClass(QC_QObject));

   qt_ns.addInitialNamespace(initQTimeLineNS(QC_QObject));

   qt_ns.addSystemClass(initQTimerClass(QC_QObject));

   qt_ns.addSystemClass(initQModelIndexClass());

   qt_ns.addSystemClass(initQTranslatorClass(QC_QObject));

   QtNS = &qt_ns;
}

static QoreStringNode *qt_core_module_init()
{
   init_namespace();

   builtinFunctions.add("QObject_connect",            f_QObject_connect, QDOM_GUI);
   builtinFunctions.add("SLOT",                       f_SLOT, QDOM_GUI);
   builtinFunctions.add("SIGNAL",                     f_SIGNAL, QDOM_GUI);
   builtinFunctions.add("TR",                         f_TR, QDOM_GUI);
   builtinFunctions.add("qDebug",                     f_qDebug, QDOM_GUI);
   builtinFunctions.add("qWarning",                   f_qWarning, QDOM_GUI);
   builtinFunctions.add("qCritical",                  f_qCritical, QDOM_GUI);
   builtinFunctions.add("qFatal",                     f_qFatal, QDOM_GUI);
   builtinFunctions.add("qRound",                     f_qRound, QDOM_GUI);
   builtinFunctions.add("qsrand",                     f_qsrand, QDOM_GUI);
   builtinFunctions.add("qrand",                      f_qrand, QDOM_GUI);
   builtinFunctions.add("qSwap",                      f_qSwap, QDOM_GUI);

   // add static class functions as builtin functions
   initQCoreApplicationStaticFunctions();
   initQLocaleStaticFunctions();
   initQDirStaticFunctions();
   initQLibraryInfoStaticFunctions();
   initQTimerStaticFunctions();

   return 0;
}

static void qt_core_module_ns_init(QoreNamespace *rns, QoreNamespace *qns)
{
   qns->addInitialNamespace(qt_ns.copy());
}

static void qt_core_module_delete()
{
}
