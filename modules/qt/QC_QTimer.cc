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

int CID_QTIMER;

static void QTIMER_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQTimer *qw;
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQTimer(self);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQTimer(self, parent->getQWidget());
   }

   self->setPrivate(CID_QTIMER, qw);
}

static void QTIMER_copy(class Object *self, class Object *old, class QoreQTimer *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QTIMER-COPY-ERROR", "objects of this class cannot be copied");
}

//int interval () const
static QoreNode *QTIMER_interval(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qt->qobj->interval());
}

//bool isActive () const
static QoreNode *QTIMER_isActive(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qt->qobj->isActive());
}

//bool isSingleShot () const
static QoreNode *QTIMER_isSingleShot(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qt->qobj->isSingleShot());
}

//void setInterval ( int msec )
static QoreNode *QTIMER_setInterval(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int msec = p ? p->getAsInt() : 0;
   qt->qobj->setInterval(msec);
   return 0;
}

//void setSingleShot ( bool singleShot )
static QoreNode *QTIMER_setSingleShot(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool singleShot = p ? p->getAsBool() : false;
   qt->qobj->setSingleShot(singleShot);
   return 0;
}

//int timerId () const
static QoreNode *QTIMER_timerId(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qt->qobj->timerId());
}

//slots

//void start ( int msec )
//void start ()
static QoreNode *QTIMER_start(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qt->qobj->start();
   else {
      int msec = p ? p->getAsInt() : 0;
      qt->qobj->start(msec);
   }
   return 0;
}

//void stop ()
static QoreNode *QTIMER_stop(Object *self, QoreQTimer *qt, QoreNode *params, ExceptionSink *xsink)
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
