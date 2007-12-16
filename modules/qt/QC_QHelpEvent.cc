/*
 QC_QHelpEvent.cc
 
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

#include "QC_QHelpEvent.h"

int CID_QHELPEVENT;
class QoreClass *QC_QHelpEvent = 0;

//QHelpEvent ( Type type, const QPoint & pos, const QPoint & globalPos )
static void QHELPEVENT_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QHelpEvent::Type type = (QHelpEvent::Type)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QHELPEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as second argument to QHelpEvent::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);
   p = get_param(params, 2);
   QoreQPoint *globalPos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
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
static QoreNode *QHELPEVENT_globalPos(QoreObject *self, QoreQHelpEvent *qhe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qhe->globalPos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//int globalX () const
static QoreNode *QHELPEVENT_globalX(QoreObject *self, QoreQHelpEvent *qhe, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qhe->globalX());
}

//int globalY () const
static QoreNode *QHELPEVENT_globalY(QoreObject *self, QoreQHelpEvent *qhe, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qhe->globalY());
}

//const QPoint & pos () const
static QoreNode *QHELPEVENT_pos(QoreObject *self, QoreQHelpEvent *qhe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qhe->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//int x () const
static QoreNode *QHELPEVENT_x(QoreObject *self, QoreQHelpEvent *qhe, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qhe->x());
}

//int y () const
static QoreNode *QHELPEVENT_y(QoreObject *self, QoreQHelpEvent *qhe, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qhe->y());
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
