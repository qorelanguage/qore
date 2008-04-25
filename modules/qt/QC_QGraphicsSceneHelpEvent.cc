/*
 QC_QGraphicsSceneHelpEvent.cc
 
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

#include "QC_QGraphicsSceneHelpEvent.h"

qore_classid_t CID_QGRAPHICSSCENEHELPEVENT;
QoreClass *QC_QGraphicsSceneHelpEvent = 0;

//QGraphicsSceneHelpEvent ( Type type = None )
static void QGRAPHICSSCENEHELPEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsSceneHelpEvent::Type type = !is_nothing(p) ? (QGraphicsSceneHelpEvent::Type)p->getAsInt() : QGraphicsSceneHelpEvent::None;
   self->setPrivate(CID_QGRAPHICSSCENEHELPEVENT, new QoreQGraphicsSceneHelpEvent(type));
   return;
}

static void QGRAPHICSSCENEHELPEVENT_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSceneHelpEvent *qgshe, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSCENEHELPEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//QPointF scenePos () const
static AbstractQoreNode *QGRAPHICSSCENEHELPEVENT_scenePos(QoreObject *self, QoreQGraphicsSceneHelpEvent *qgshe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgshe->scenePos()));
}

//QPoint screenPos () const
static AbstractQoreNode *QGRAPHICSSCENEHELPEVENT_screenPos(QoreObject *self, QoreQGraphicsSceneHelpEvent *qgshe, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qgshe->screenPos()));
}

QoreClass *initQGraphicsSceneHelpEventClass(QoreClass *qgraphicssceneevent)
{
   QC_QGraphicsSceneHelpEvent = new QoreClass("QGraphicsSceneHelpEvent", QDOM_GUI);
   CID_QGRAPHICSSCENEHELPEVENT = QC_QGraphicsSceneHelpEvent->getID();

   QC_QGraphicsSceneHelpEvent->addBuiltinVirtualBaseClass(qgraphicssceneevent);

   QC_QGraphicsSceneHelpEvent->setConstructor(QGRAPHICSSCENEHELPEVENT_constructor);
   QC_QGraphicsSceneHelpEvent->setCopy((q_copy_t)QGRAPHICSSCENEHELPEVENT_copy);

   QC_QGraphicsSceneHelpEvent->addMethod("scenePos",                    (q_method_t)QGRAPHICSSCENEHELPEVENT_scenePos);
   QC_QGraphicsSceneHelpEvent->addMethod("screenPos",                   (q_method_t)QGRAPHICSSCENEHELPEVENT_screenPos);

   return QC_QGraphicsSceneHelpEvent;
}
