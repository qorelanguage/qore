/*
 QC_QGraphicsSceneHoverEvent.cc
 
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

#include "QC_QGraphicsSceneHoverEvent.h"

qore_classid_t CID_QGRAPHICSSCENEHOVEREVENT;
QoreClass *QC_QGraphicsSceneHoverEvent = 0;

//QGraphicsSceneHoverEvent ( Type type = None )
static void QGRAPHICSSCENEHOVEREVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsSceneHoverEvent::Type type = !is_nothing(p) ? (QGraphicsSceneHoverEvent::Type)p->getAsInt() : QGraphicsSceneHoverEvent::None;
   self->setPrivate(CID_QGRAPHICSSCENEHOVEREVENT, new QoreQGraphicsSceneHoverEvent(type));
   return;
}

static void QGRAPHICSSCENEHOVEREVENT_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSceneHoverEvent *qgshe, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSCENEHOVEREVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//QPointF pos () const
static AbstractQoreNode *QGRAPHICSSCENEHOVEREVENT_pos(QoreObject *self, QoreQGraphicsSceneHoverEvent *qgshe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgshe->pos()));
}

//QPointF scenePos () const
static AbstractQoreNode *QGRAPHICSSCENEHOVEREVENT_scenePos(QoreObject *self, QoreQGraphicsSceneHoverEvent *qgshe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgshe->scenePos()));
}

//QPoint screenPos () const
static AbstractQoreNode *QGRAPHICSSCENEHOVEREVENT_screenPos(QoreObject *self, QoreQGraphicsSceneHoverEvent *qgshe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qgshe->screenPos()));
}

QoreClass *initQGraphicsSceneHoverEventClass(QoreClass *qgraphicssceneevent)
{
   QC_QGraphicsSceneHoverEvent = new QoreClass("QGraphicsSceneHoverEvent", QDOM_GUI);
   CID_QGRAPHICSSCENEHOVEREVENT = QC_QGraphicsSceneHoverEvent->getID();

   QC_QGraphicsSceneHoverEvent->addBuiltinVirtualBaseClass(qgraphicssceneevent);

   QC_QGraphicsSceneHoverEvent->setConstructor(QGRAPHICSSCENEHOVEREVENT_constructor);
   QC_QGraphicsSceneHoverEvent->setCopy((q_copy_t)QGRAPHICSSCENEHOVEREVENT_copy);

   QC_QGraphicsSceneHoverEvent->addMethod("pos",                         (q_method_t)QGRAPHICSSCENEHOVEREVENT_pos);
   QC_QGraphicsSceneHoverEvent->addMethod("scenePos",                    (q_method_t)QGRAPHICSSCENEHOVEREVENT_scenePos);
   QC_QGraphicsSceneHoverEvent->addMethod("screenPos",                   (q_method_t)QGRAPHICSSCENEHOVEREVENT_screenPos);

   return QC_QGraphicsSceneHoverEvent;
}
