/*
 QC_QContextMenuEvent.cc
 
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

#include "QC_QContextMenuEvent.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

int CID_QCONTEXTMENUEVENT;
class QoreClass *QC_QContextMenuEvent = 0;

//QContextMenuEvent ( Reason reason, const QPoint & pos, const QPoint & globalPos )
//QContextMenuEvent ( Reason reason, const QPoint & pos )
static void QCONTEXTMENUEVENT_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QContextMenuEvent::Reason reason = (QContextMenuEvent::Reason)(p ? p->getAsInt() : 0);
   p = test_param(params, NT_OBJECT, 1);
   QoreQPoint *pos = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
	 xsink->raiseException("QCONTEXTMENUEVENT-CONSTRUCTOR-PARAM-ERROR", "QContextMenuEvent::constructor() expects a QPoint object as the second parameter");
      return;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   p = test_param(params, NT_OBJECT, 2);
   QoreQPoint *globalPos = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!globalPos && p) {
      if (!xsink->isException())
	 xsink->raiseException("QCONTEXTMENUEVENT-CONSTRUCTOR-PARAM-ERROR", "QContextMenuEvent::constructor() does not know how to handle arguments of class '%s' as the third parameter", p->val.object->getClass()->getName());
      return;
   }
   ReferenceHolder<QoreQPoint> globalPosHolder(globalPos, xsink);
   if (globalPos)
      self->setPrivate(CID_QCONTEXTMENUEVENT, new QoreQContextMenuEvent(reason, *(static_cast<QPoint *>(pos)), *(static_cast<QPoint *>(globalPos))));
   else
      self->setPrivate(CID_QCONTEXTMENUEVENT, new QoreQContextMenuEvent(reason, *(static_cast<QPoint *>(pos))));
   return;
}

static void QCONTEXTMENUEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQContextMenuEvent *qcme, ExceptionSink *xsink)
{
   xsink->raiseException("QCONTEXTMENUEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//const QPoint & globalPos () const
static QoreNode *QCONTEXTMENUEVENT_globalPos(QoreObject *self, QoreQContextMenuEvent *qcme, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qcme->globalPos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//int globalX () const
static QoreNode *QCONTEXTMENUEVENT_globalX(QoreObject *self, QoreQContextMenuEvent *qcme, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcme->globalX());
}

//int globalY () const
static QoreNode *QCONTEXTMENUEVENT_globalY(QoreObject *self, QoreQContextMenuEvent *qcme, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcme->globalY());
}

//const QPoint & pos () const
static QoreNode *QCONTEXTMENUEVENT_pos(QoreObject *self, QoreQContextMenuEvent *qcme, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qcme->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//Reason reason () const
static QoreNode *QCONTEXTMENUEVENT_reason(QoreObject *self, QoreQContextMenuEvent *qcme, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcme->reason());
}

//int x () const
static QoreNode *QCONTEXTMENUEVENT_x(QoreObject *self, QoreQContextMenuEvent *qcme, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcme->x());
}

//int y () const
static QoreNode *QCONTEXTMENUEVENT_y(QoreObject *self, QoreQContextMenuEvent *qcme, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcme->y());
}

QoreClass *initQContextMenuEventClass(QoreClass *qinputevent)
{
   QC_QContextMenuEvent = new QoreClass("QContextMenuEvent", QDOM_GUI);
   CID_QCONTEXTMENUEVENT = QC_QContextMenuEvent->getID();

   QC_QContextMenuEvent->addBuiltinVirtualBaseClass(qinputevent);

   QC_QContextMenuEvent->setConstructor(QCONTEXTMENUEVENT_constructor);
   QC_QContextMenuEvent->setCopy((q_copy_t)QCONTEXTMENUEVENT_copy);

   QC_QContextMenuEvent->addMethod("globalPos",                   (q_method_t)QCONTEXTMENUEVENT_globalPos);
   QC_QContextMenuEvent->addMethod("globalX",                     (q_method_t)QCONTEXTMENUEVENT_globalX);
   QC_QContextMenuEvent->addMethod("globalY",                     (q_method_t)QCONTEXTMENUEVENT_globalY);
   QC_QContextMenuEvent->addMethod("pos",                         (q_method_t)QCONTEXTMENUEVENT_pos);
   QC_QContextMenuEvent->addMethod("reason",                      (q_method_t)QCONTEXTMENUEVENT_reason);
   QC_QContextMenuEvent->addMethod("x",                           (q_method_t)QCONTEXTMENUEVENT_x);
   QC_QContextMenuEvent->addMethod("y",                           (q_method_t)QCONTEXTMENUEVENT_y);

   return QC_QContextMenuEvent;
}
