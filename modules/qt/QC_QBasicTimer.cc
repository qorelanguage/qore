/*
 QC_QBasicTimer.cc
 
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

#include "QC_QBasicTimer.h"
#include "QC_QObject.h"

#include "qore-qt.h"

int CID_QBASICTIMER;
class QoreClass *QC_QBasicTimer = 0;

//QBasicTimer ()
static void QBASICTIMER_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QBASICTIMER, new QoreQBasicTimer());
   return;
}

static void QBASICTIMER_copy(class QoreObject *self, class QoreObject *old, class QoreQBasicTimer *qbt, ExceptionSink *xsink)
{
   xsink->raiseException("QBASICTIMER-COPY-ERROR", "objects of this class cannot be copied");
}

//bool isActive () const
static QoreNode *QBASICTIMER_isActive(QoreObject *self, QoreQBasicTimer *qbt, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qbt->isActive());
}

//void start ( int msec, QObject * object )
static QoreNode *QBASICTIMER_start(QoreObject *self, QoreQBasicTimer *qbt, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int msec = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreAbstractQObject *object = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!object) {
      if (!xsink->isException())
         xsink->raiseException("QBASICTIMER-START-PARAM-ERROR", "expecting a QObject object as second argument to QBasicTimer::start()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> objectHolder(static_cast<AbstractPrivateData *>(object), xsink);
   qbt->start(msec, object->getQObject());
   return 0;
}

//void stop ()
static QoreNode *QBASICTIMER_stop(QoreObject *self, QoreQBasicTimer *qbt, const QoreNode *params, ExceptionSink *xsink)
{
   qbt->stop();
   return 0;
}

//int timerId () const
static QoreNode *QBASICTIMER_timerId(QoreObject *self, QoreQBasicTimer *qbt, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qbt->timerId());
}

QoreClass *initQBasicTimerClass()
{
   QC_QBasicTimer = new QoreClass("QBasicTimer", QDOM_GUI);
   CID_QBASICTIMER = QC_QBasicTimer->getID();

   QC_QBasicTimer->setConstructor(QBASICTIMER_constructor);
   QC_QBasicTimer->setCopy((q_copy_t)QBASICTIMER_copy);

   QC_QBasicTimer->addMethod("isActive",                    (q_method_t)QBASICTIMER_isActive);
   QC_QBasicTimer->addMethod("start",                       (q_method_t)QBASICTIMER_start);
   QC_QBasicTimer->addMethod("stop",                        (q_method_t)QBASICTIMER_stop);
   QC_QBasicTimer->addMethod("timerId",                     (q_method_t)QBASICTIMER_timerId);

   return QC_QBasicTimer;
}
