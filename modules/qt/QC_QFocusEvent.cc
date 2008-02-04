/*
 QC_QFocusEvent.cc
 
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

#include "QC_QFocusEvent.h"

int CID_QFOCUSEVENT;
class QoreClass *QC_QFocusEvent = 0;

//QFocusEvent ( Type type, Qt::FocusReason reason = Qt::OtherFocusReason )
static void QFOCUSEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QFocusEvent::Type type = (QFocusEvent::Type)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   Qt::FocusReason reason = !is_nothing(p) ? (Qt::FocusReason)p->getAsInt() : Qt::OtherFocusReason;
   self->setPrivate(CID_QFOCUSEVENT, new QoreQFocusEvent(type, reason));
   return;
}

static void QFOCUSEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQFocusEvent *qfe, ExceptionSink *xsink)
{
   xsink->raiseException("QFOCUSEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool gotFocus () const
static AbstractQoreNode *QFOCUSEVENT_gotFocus(QoreObject *self, QoreQFocusEvent *qfe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfe->gotFocus());
}

//bool lostFocus () const
static AbstractQoreNode *QFOCUSEVENT_lostFocus(QoreObject *self, QoreQFocusEvent *qfe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qfe->lostFocus());
}

//Qt::FocusReason reason () const
static AbstractQoreNode *QFOCUSEVENT_reason(QoreObject *self, QoreQFocusEvent *qfe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfe->reason());
}

QoreClass *initQFocusEventClass(QoreClass *qevent)
{
   QC_QFocusEvent = new QoreClass("QFocusEvent", QDOM_GUI);
   CID_QFOCUSEVENT = QC_QFocusEvent->getID();

   QC_QFocusEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QFocusEvent->setConstructor(QFOCUSEVENT_constructor);
   QC_QFocusEvent->setCopy((q_copy_t)QFOCUSEVENT_copy);

   QC_QFocusEvent->addMethod("gotFocus",                    (q_method_t)QFOCUSEVENT_gotFocus);
   QC_QFocusEvent->addMethod("lostFocus",                   (q_method_t)QFOCUSEVENT_lostFocus);
   QC_QFocusEvent->addMethod("reason",                      (q_method_t)QFOCUSEVENT_reason);

   return QC_QFocusEvent;
}
