/*
 QC_QTimer.cc
 
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

#include "QC_QTimer.h"
#include "QC_QFont.h"
#include "QC_QWidget.h"
#include "QC_QObject.h"

int CID_QTIMER;

// QTimer ( QObject * parent = 0 )
static void QTIMER_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQTimer *qw;
   const QoreObject *p = test_object_param(params, 0);
   QoreAbstractQObject *parent = p ? (QoreAbstractQObject *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;

   if (!parent)
      qw = new QoreQTimer(self);
   else 
   {
      ReferenceHolder<QoreAbstractQObject> holder(parent, xsink);
      qw = new QoreQTimer(self, parent->getQObject());
   }

   self->setPrivate(CID_QTIMER, qw);
}

static void QTIMER_copy(class QoreObject *self, class QoreObject *old, class QoreQTimer *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QTIMER-COPY-ERROR", "objects of this class cannot be copied");
}

//int interval () const
static AbstractQoreNode *QTIMER_interval(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->qobj->interval());
}

//bool isActive () const
static AbstractQoreNode *QTIMER_isActive(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qt->qobj->isActive());
}

//bool isSingleShot () const
static AbstractQoreNode *QTIMER_isSingleShot(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qt->qobj->isSingleShot());
}

//void setInterval ( int msec )
static AbstractQoreNode *QTIMER_setInterval(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int msec = p ? p->getAsInt() : 0;
   qt->qobj->setInterval(msec);
   return 0;
}

//void setSingleShot ( bool singleShot )
static AbstractQoreNode *QTIMER_setSingleShot(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool singleShot = p ? p->getAsBool() : false;
   qt->qobj->setSingleShot(singleShot);
   return 0;
}

//int timerId () const
static AbstractQoreNode *QTIMER_timerId(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->qobj->timerId());
}

//slots

//void start ( int msec )
//void start ()
static AbstractQoreNode *QTIMER_start(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qt->qobj->start();
   else {
      int msec = p ? p->getAsInt() : 0;
      qt->qobj->start(msec);
   }
   return 0;
}

//void stop ()
static AbstractQoreNode *QTIMER_stop(QoreObject *self, QoreQTimer *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   qt->qobj->stop();
   return 0;
}

class QoreClass *initQTimerClass(class QoreClass *qobject)
{
   tracein("initQTimerClass()");
   
   class QoreClass *QC_QTimer = new QoreClass("QTimer", QDOM_GUI);
   CID_QTIMER = QC_QTimer->getID();

   QC_QTimer->addBuiltinVirtualBaseClass(qobject);

   QC_QTimer->setConstructor(QTIMER_constructor);
   QC_QTimer->setCopy((q_copy_t)QTIMER_copy);

   QC_QTimer->addMethod("interval",                    (q_method_t)QTIMER_interval);
   QC_QTimer->addMethod("isActive",                    (q_method_t)QTIMER_isActive);
   QC_QTimer->addMethod("isSingleShot",                (q_method_t)QTIMER_isSingleShot);
   QC_QTimer->addMethod("setInterval",                 (q_method_t)QTIMER_setInterval);
   QC_QTimer->addMethod("setSingleShot",               (q_method_t)QTIMER_setSingleShot);
   QC_QTimer->addMethod("timerId",                     (q_method_t)QTIMER_timerId);
   QC_QTimer->addMethod("start",                       (q_method_t)QTIMER_start);
   QC_QTimer->addMethod("stop",                        (q_method_t)QTIMER_stop);

   traceout("initQTimerClass()");
   return QC_QTimer;
}

//void singleShot ( int msec, QObject * receiver, const char * member )
static AbstractQoreNode *f_QTimer_singleShot(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int msec = p ? p->getAsInt() : 0;
   const QoreObject *o = test_object_param(params, 1);
   QoreAbstractQObject *receiver = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
         xsink->raiseException("QTIMER-SINGLESHOT-PARAM-ERROR", "expecting a QObject object as second argument to QTimer::singleShot()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

   const QoreStringNode *pstr = test_string_param(params, 2);
   if (!pstr) {
      xsink->raiseException("QTIMER-SINGLESHOT-PARAM-ERROR", "expecting a string as third argument to QTimer::singleShot()");
      return 0;
   }
   const char *member = pstr->getBuffer();

   QoreObject *obj = new QoreObject(QC_QObject, getProgram());
   class QoreQtSingleShotTimer *qsst = new QoreQtSingleShotTimer(obj, msec, receiver, member, xsink);
   if (!*xsink)
      obj->setPrivate(CID_QOBJECT, qsst);

   return 0;
}

void initQTimerStaticFunctions()
{
   builtinFunctions.add("QTimer_singleShot",                   f_QTimer_singleShot);
}
