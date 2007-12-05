/*
 QC_QTimerEvent.cc
 
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

#include "QC_QTimerEvent.h"

int CID_QTIMEREVENT;
class QoreClass *QC_QTimerEvent = 0;

//QTimerEvent ( int timerId )
static void QTIMEREVENT_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int timerId = p ? p->getAsInt() : 0;
   self->setPrivate(CID_QTIMEREVENT, new QoreQTimerEvent(timerId));
   return;
}

static void QTIMEREVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQTimerEvent *qte, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTIMEREVENT, new QoreQTimerEvent(*qte));
}

//int timerId () const
static QoreNode *QTIMEREVENT_timerId(QoreObject *self, QoreQTimerEvent *qte, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qte->timerId());
}

QoreClass *initQTimerEventClass(QoreClass *qevent)
{
   QC_QTimerEvent = new QoreClass("QTimerEvent", QDOM_GUI);
   CID_QTIMEREVENT = QC_QTimerEvent->getID();

   QC_QTimerEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QTimerEvent->setConstructor(QTIMEREVENT_constructor);
   QC_QTimerEvent->setCopy((q_copy_t)QTIMEREVENT_copy);

   QC_QTimerEvent->addMethod("timerId",                     (q_method_t)QTIMEREVENT_timerId);

   return QC_QTimerEvent;
}
