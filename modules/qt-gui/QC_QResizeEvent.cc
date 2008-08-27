/*
 QC_QResizeEvent.cc
 
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

#include "QC_QResizeEvent.h"
#include "QC_QSize.h"

qore_classid_t CID_QRESIZEEVENT;

class QoreClass *QC_QResizeEvent = 0;

static void QRESIZEEVENT_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QRESIZEEVENT-CONSTRUCTOR-ERROR", "QResizeEvent is an abstract base class");
}

static void QRESIZEEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQResizeEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QRESIZEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//const QSize & oldSize () const
static AbstractQoreNode *QRESIZEEVENT_oldSize(QoreObject *self, QoreQResizeEvent *qre, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qre->oldSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//const QSize & size () const
static AbstractQoreNode *QRESIZEEVENT_size(QoreObject *self, QoreQResizeEvent *qre, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qre->size());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

class QoreClass *initQResizeEventClass(class QoreClass *qevent)
{
   QORE_TRACE("initQResizeEventClass()");
   
   QC_QResizeEvent = new QoreClass("QResizeEvent", QDOM_GUI);
   CID_QRESIZEEVENT = QC_QResizeEvent->getID();

   QC_QResizeEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QResizeEvent->setConstructor(QRESIZEEVENT_constructor);
   QC_QResizeEvent->setCopy((q_copy_t)QRESIZEEVENT_copy);

   QC_QResizeEvent->addMethod("oldSize",                     (q_method_t)QRESIZEEVENT_oldSize);
   QC_QResizeEvent->addMethod("size",                        (q_method_t)QRESIZEEVENT_size);


   return QC_QResizeEvent;
}
