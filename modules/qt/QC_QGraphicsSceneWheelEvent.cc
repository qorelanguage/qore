/*
 QC_QGraphicsSceneWheelEvent.cc
 
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

#include "QC_QGraphicsSceneWheelEvent.h"

qore_classid_t CID_QGRAPHICSSCENEWHEELEVENT;
QoreClass *QC_QGraphicsSceneWheelEvent = 0;

//QGraphicsSceneWheelEvent ( Type type = None )
static void QGRAPHICSSCENEWHEELEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsSceneWheelEvent::Type type = !is_nothing(p) ? (QGraphicsSceneWheelEvent::Type)p->getAsInt() : QGraphicsSceneWheelEvent::None;
   self->setPrivate(CID_QGRAPHICSSCENEWHEELEVENT, new QoreQGraphicsSceneWheelEvent(type));
   return;
}

static void QGRAPHICSSCENEWHEELEVENT_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSceneWheelEvent *qgswe, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSCENEWHEELEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::MouseButtons buttons () const
static AbstractQoreNode *QGRAPHICSSCENEWHEELEVENT_buttons(QoreObject *self, QoreQGraphicsSceneWheelEvent *qgswe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgswe->buttons());
}

//int delta () const
static AbstractQoreNode *QGRAPHICSSCENEWHEELEVENT_delta(QoreObject *self, QoreQGraphicsSceneWheelEvent *qgswe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgswe->delta());
}

//Qt::KeyboardModifiers modifiers () const
static AbstractQoreNode *QGRAPHICSSCENEWHEELEVENT_modifiers(QoreObject *self, QoreQGraphicsSceneWheelEvent *qgswe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgswe->modifiers());
}

//Qt::Orientation orientation () const
static AbstractQoreNode *QGRAPHICSSCENEWHEELEVENT_orientation(QoreObject *self, QoreQGraphicsSceneWheelEvent *qgswe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgswe->orientation());
}

//QPointF pos () const
static AbstractQoreNode *QGRAPHICSSCENEWHEELEVENT_pos(QoreObject *self, QoreQGraphicsSceneWheelEvent *qgswe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgswe->pos()));
}

//QPointF scenePos () const
static AbstractQoreNode *QGRAPHICSSCENEWHEELEVENT_scenePos(QoreObject *self, QoreQGraphicsSceneWheelEvent *qgswe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgswe->scenePos()));
}

//QPoint screenPos () const
static AbstractQoreNode *QGRAPHICSSCENEWHEELEVENT_screenPos(QoreObject *self, QoreQGraphicsSceneWheelEvent *qgswe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qgswe->screenPos()));
}

QoreClass *initQGraphicsSceneWheelEventClass(QoreClass *qgraphicssceneevent)
{
   QC_QGraphicsSceneWheelEvent = new QoreClass("QGraphicsSceneWheelEvent", QDOM_GUI);
   CID_QGRAPHICSSCENEWHEELEVENT = QC_QGraphicsSceneWheelEvent->getID();

   QC_QGraphicsSceneWheelEvent->addBuiltinVirtualBaseClass(qgraphicssceneevent);

   QC_QGraphicsSceneWheelEvent->setConstructor(QGRAPHICSSCENEWHEELEVENT_constructor);
   QC_QGraphicsSceneWheelEvent->setCopy((q_copy_t)QGRAPHICSSCENEWHEELEVENT_copy);

   QC_QGraphicsSceneWheelEvent->addMethod("buttons",                     (q_method_t)QGRAPHICSSCENEWHEELEVENT_buttons);
   QC_QGraphicsSceneWheelEvent->addMethod("delta",                       (q_method_t)QGRAPHICSSCENEWHEELEVENT_delta);
   QC_QGraphicsSceneWheelEvent->addMethod("modifiers",                   (q_method_t)QGRAPHICSSCENEWHEELEVENT_modifiers);
   QC_QGraphicsSceneWheelEvent->addMethod("orientation",                 (q_method_t)QGRAPHICSSCENEWHEELEVENT_orientation);
   QC_QGraphicsSceneWheelEvent->addMethod("pos",                         (q_method_t)QGRAPHICSSCENEWHEELEVENT_pos);
   QC_QGraphicsSceneWheelEvent->addMethod("scenePos",                    (q_method_t)QGRAPHICSSCENEWHEELEVENT_scenePos);
   QC_QGraphicsSceneWheelEvent->addMethod("screenPos",                   (q_method_t)QGRAPHICSSCENEWHEELEVENT_screenPos);

   return QC_QGraphicsSceneWheelEvent;
}
