/*
 QC_QHelpEvent.cc
 
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

#include "QC_QHelpEvent.h"
#include "QC_QPoint.h"

#include "qore-qt-gui.h"

qore_classid_t CID_QHELPEVENT;
class QoreClass *QC_QHelpEvent = 0;

//QHelpEvent ( Type type, const QPoint & pos, const QPoint & globalPos )
static void QHELPEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QHelpEvent::Type type = (QHelpEvent::Type)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQPoint *pos = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QHELPEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as second argument to QHelpEvent::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);

   o = test_object_param(params, 2);
   QoreQPoint *globalPos = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!globalPos) {
      if (!xsink->isException())
         xsink->raiseException("QHELPEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as third argument to QHelpEvent::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> globalPosHolder(static_cast<AbstractPrivateData *>(globalPos), xsink);
   self->setPrivate(CID_QHELPEVENT, new QoreQHelpEvent(type, *(static_cast<QPoint *>(pos)), *(static_cast<QPoint *>(globalPos))));
   return;
}

static void QHELPEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQHelpEvent *qhe, ExceptionSink *xsink)
{
   xsink->raiseException("QHELPEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//const QPoint & globalPos () const
static AbstractQoreNode *QHELPEVENT_globalPos(QoreObject *self, QoreQHelpEvent *qhe, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qhe->globalPos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//int globalX () const
static AbstractQoreNode *QHELPEVENT_globalX(QoreObject *self, QoreQHelpEvent *qhe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhe->globalX());
}

//int globalY () const
static AbstractQoreNode *QHELPEVENT_globalY(QoreObject *self, QoreQHelpEvent *qhe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhe->globalY());
}

//const QPoint & pos () const
static AbstractQoreNode *QHELPEVENT_pos(QoreObject *self, QoreQHelpEvent *qhe, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qhe->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//int x () const
static AbstractQoreNode *QHELPEVENT_x(QoreObject *self, QoreQHelpEvent *qhe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhe->x());
}

//int y () const
static AbstractQoreNode *QHELPEVENT_y(QoreObject *self, QoreQHelpEvent *qhe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhe->y());
}

QoreClass *initQHelpEventClass(QoreClass *qevent)
{
   QC_QHelpEvent = new QoreClass("QHelpEvent", QDOM_GUI);
   CID_QHELPEVENT = QC_QHelpEvent->getID();

   QC_QHelpEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QHelpEvent->setConstructor(QHELPEVENT_constructor);
   QC_QHelpEvent->setCopy((q_copy_t)QHELPEVENT_copy);

   QC_QHelpEvent->addMethod("globalPos",                   (q_method_t)QHELPEVENT_globalPos);
   QC_QHelpEvent->addMethod("globalX",                     (q_method_t)QHELPEVENT_globalX);
   QC_QHelpEvent->addMethod("globalY",                     (q_method_t)QHELPEVENT_globalY);
   QC_QHelpEvent->addMethod("pos",                         (q_method_t)QHELPEVENT_pos);
   QC_QHelpEvent->addMethod("x",                           (q_method_t)QHELPEVENT_x);
   QC_QHelpEvent->addMethod("y",                           (q_method_t)QHELPEVENT_y);

   return QC_QHelpEvent;
}
