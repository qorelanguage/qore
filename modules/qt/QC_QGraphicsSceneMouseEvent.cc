/*
 QC_QGraphicsSceneMouseEvent.cc
 
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

#include "QC_QGraphicsSceneMouseEvent.h"

qore_classid_t CID_QGRAPHICSSCENEMOUSEEVENT;
QoreClass *QC_QGraphicsSceneMouseEvent = 0;

//QGraphicsSceneMouseEvent ( Type type = None )
static void QGRAPHICSSCENEMOUSEEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsSceneMouseEvent::Type type = !is_nothing(p) ? (QGraphicsSceneMouseEvent::Type)p->getAsInt() : QGraphicsSceneMouseEvent::None;
   self->setPrivate(CID_QGRAPHICSSCENEMOUSEEVENT, new QoreQGraphicsSceneMouseEvent(type));
   return;
}

static void QGRAPHICSSCENEMOUSEEVENT_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSceneMouseEvent *qgsme, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSCENEMOUSEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::MouseButton button () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_button(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsme->button());
}

//QPointF buttonDownPos ( Qt::MouseButton button ) const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_buttonDownPos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::MouseButton button = (Qt::MouseButton)(p ? p->getAsInt() : 0);
   return return_object(QC_QPointF, new QoreQPointF(qgsme->buttonDownPos(button)));
}

//QPointF buttonDownScenePos ( Qt::MouseButton button ) const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_buttonDownScenePos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::MouseButton button = (Qt::MouseButton)(p ? p->getAsInt() : 0);
   return return_object(QC_QPointF, new QoreQPointF(qgsme->buttonDownScenePos(button)));
}

//QPoint buttonDownScreenPos ( Qt::MouseButton button ) const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_buttonDownScreenPos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::MouseButton button = (Qt::MouseButton)(p ? p->getAsInt() : 0);
   return return_object(QC_QPoint, new QoreQPoint(qgsme->buttonDownScreenPos(button)));
}

//Qt::MouseButtons buttons () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_buttons(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsme->buttons());
}

//QPointF lastPos () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_lastPos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgsme->lastPos()));
}

//QPointF lastScenePos () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_lastScenePos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgsme->lastScenePos()));
}

//QPoint lastScreenPos () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_lastScreenPos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qgsme->lastScreenPos()));
}

//Qt::KeyboardModifiers modifiers () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_modifiers(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsme->modifiers());
}

//QPointF pos () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_pos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgsme->pos()));
}

//QPointF scenePos () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_scenePos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgsme->scenePos()));
}

//QPoint screenPos () const
static AbstractQoreNode *QGRAPHICSSCENEMOUSEEVENT_screenPos(QoreObject *self, QoreQGraphicsSceneMouseEvent *qgsme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qgsme->screenPos()));
}

QoreClass *initQGraphicsSceneMouseEventClass(QoreClass *qgraphicssceneevent)
{
   QC_QGraphicsSceneMouseEvent = new QoreClass("QGraphicsSceneMouseEvent", QDOM_GUI);
   CID_QGRAPHICSSCENEMOUSEEVENT = QC_QGraphicsSceneMouseEvent->getID();

   QC_QGraphicsSceneMouseEvent->addBuiltinVirtualBaseClass(qgraphicssceneevent);

   QC_QGraphicsSceneMouseEvent->setConstructor(QGRAPHICSSCENEMOUSEEVENT_constructor);
   QC_QGraphicsSceneMouseEvent->setCopy((q_copy_t)QGRAPHICSSCENEMOUSEEVENT_copy);

   QC_QGraphicsSceneMouseEvent->addMethod("button",                      (q_method_t)QGRAPHICSSCENEMOUSEEVENT_button);
   QC_QGraphicsSceneMouseEvent->addMethod("buttonDownPos",               (q_method_t)QGRAPHICSSCENEMOUSEEVENT_buttonDownPos);
   QC_QGraphicsSceneMouseEvent->addMethod("buttonDownScenePos",          (q_method_t)QGRAPHICSSCENEMOUSEEVENT_buttonDownScenePos);
   QC_QGraphicsSceneMouseEvent->addMethod("buttonDownScreenPos",         (q_method_t)QGRAPHICSSCENEMOUSEEVENT_buttonDownScreenPos);
   QC_QGraphicsSceneMouseEvent->addMethod("buttons",                     (q_method_t)QGRAPHICSSCENEMOUSEEVENT_buttons);
   QC_QGraphicsSceneMouseEvent->addMethod("lastPos",                     (q_method_t)QGRAPHICSSCENEMOUSEEVENT_lastPos);
   QC_QGraphicsSceneMouseEvent->addMethod("lastScenePos",                (q_method_t)QGRAPHICSSCENEMOUSEEVENT_lastScenePos);
   QC_QGraphicsSceneMouseEvent->addMethod("lastScreenPos",               (q_method_t)QGRAPHICSSCENEMOUSEEVENT_lastScreenPos);
   QC_QGraphicsSceneMouseEvent->addMethod("modifiers",                   (q_method_t)QGRAPHICSSCENEMOUSEEVENT_modifiers);
   QC_QGraphicsSceneMouseEvent->addMethod("pos",                         (q_method_t)QGRAPHICSSCENEMOUSEEVENT_pos);
   QC_QGraphicsSceneMouseEvent->addMethod("scenePos",                    (q_method_t)QGRAPHICSSCENEMOUSEEVENT_scenePos);
   QC_QGraphicsSceneMouseEvent->addMethod("screenPos",                   (q_method_t)QGRAPHICSSCENEMOUSEEVENT_screenPos);

   return QC_QGraphicsSceneMouseEvent;
}
