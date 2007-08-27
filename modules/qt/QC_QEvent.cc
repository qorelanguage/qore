/*
 QC_QEvent.cc
 
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

#include "QC_QEvent.h"
#include "QC_QRegion.h"
#include "QC_QRect.h"

int CID_QEVENT;

class QoreClass *QC_QEvent = 0;

static void QEVENT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   QoreQEvent *qe = new QoreQEvent((QEvent::Type)(p ? p->getAsInt() : 0));

   self->setPrivate(CID_QEVENT, qe);
}

static void QEVENT_copy(class Object *self, class Object *old, class QoreQEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void accept ()
static QoreNode *QEVENT_accept(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   qe->accept();
   return 0;
}

//void ignore ()
static QoreNode *QEVENT_ignore(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   qe->ignore();
   return 0;
}

//bool isAccepted () const
static QoreNode *QEVENT_isAccepted(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qe->isAccepted());
}

//void setAccepted ( bool accepted )
static QoreNode *QEVENT_setAccepted(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool accepted = p ? p->getAsBool() : false;
   qe->setAccepted(accepted);
   return 0;
}

//bool spontaneous () const
static QoreNode *QEVENT_spontaneous(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qe->spontaneous());
}

//Type type () const
static QoreNode *QEVENT_type(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qe->type());
}

class QoreClass *initQEventClass()
{
   tracein("initQEventClass()");
   
   QC_QEvent = new QoreClass("QEvent", QDOM_GUI);
   CID_QEVENT = QC_QEvent->getID();
   QC_QEvent->setConstructor(QEVENT_constructor);
   QC_QEvent->setCopy((q_copy_t)QEVENT_copy);

   QC_QEvent->addMethod("accept",                      (q_method_t)QEVENT_accept);
   QC_QEvent->addMethod("ignore",                      (q_method_t)QEVENT_ignore);
   QC_QEvent->addMethod("isAccepted",                  (q_method_t)QEVENT_isAccepted);
   QC_QEvent->addMethod("setAccepted",                 (q_method_t)QEVENT_setAccepted);
   QC_QEvent->addMethod("spontaneous",                 (q_method_t)QEVENT_spontaneous);
   QC_QEvent->addMethod("type",                        (q_method_t)QEVENT_type);

   traceout("initQEventClass()");
   return QC_QEvent;
}
