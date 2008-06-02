/*
 QC_QGraphicsSceneContextMenuEvent.cc
 
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

#include "qore-qt-gui.h"

#include "QC_QGraphicsSceneContextMenuEvent.h"
#include "QC_QPoint.h"
#include "QC_QPointF.h"

qore_classid_t CID_QGRAPHICSSCENECONTEXTMENUEVENT;
QoreClass *QC_QGraphicsSceneContextMenuEvent = 0;

//QGraphicsSceneContextMenuEvent ( Type type = None )
static void QGRAPHICSSCENECONTEXTMENUEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsSceneContextMenuEvent::Type type = !is_nothing(p) ? (QGraphicsSceneContextMenuEvent::Type)p->getAsInt() : QGraphicsSceneContextMenuEvent::None;
   self->setPrivate(CID_QGRAPHICSSCENECONTEXTMENUEVENT, new QoreQGraphicsSceneContextMenuEvent(type));
   return;
}

static void QGRAPHICSSCENECONTEXTMENUEVENT_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSceneContextMenuEvent *qgscme, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSCENECONTEXTMENUEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::KeyboardModifiers modifiers () const
static AbstractQoreNode *QGRAPHICSSCENECONTEXTMENUEVENT_modifiers(QoreObject *self, QoreQGraphicsSceneContextMenuEvent *qgscme, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgscme->modifiers());
}

//QPointF pos () const
static AbstractQoreNode *QGRAPHICSSCENECONTEXTMENUEVENT_pos(QoreObject *self, QoreQGraphicsSceneContextMenuEvent *qgscme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgscme->pos()));
}

//Reason reason () const
static AbstractQoreNode *QGRAPHICSSCENECONTEXTMENUEVENT_reason(QoreObject *self, QoreQGraphicsSceneContextMenuEvent *qgscme, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgscme->reason());
}

//QPointF scenePos () const
static AbstractQoreNode *QGRAPHICSSCENECONTEXTMENUEVENT_scenePos(QoreObject *self, QoreQGraphicsSceneContextMenuEvent *qgscme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgscme->scenePos()));
}

//QPoint screenPos () const
static AbstractQoreNode *QGRAPHICSSCENECONTEXTMENUEVENT_screenPos(QoreObject *self, QoreQGraphicsSceneContextMenuEvent *qgscme, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qgscme->screenPos()));
}

static QoreClass *initQGraphicsSceneContextMenuEventClass(QoreClass *qgraphicssceneevent)
{
   QC_QGraphicsSceneContextMenuEvent = new QoreClass("QGraphicsSceneContextMenuEvent", QDOM_GUI);
   CID_QGRAPHICSSCENECONTEXTMENUEVENT = QC_QGraphicsSceneContextMenuEvent->getID();

   QC_QGraphicsSceneContextMenuEvent->addBuiltinVirtualBaseClass(qgraphicssceneevent);

   QC_QGraphicsSceneContextMenuEvent->setConstructor(QGRAPHICSSCENECONTEXTMENUEVENT_constructor);
   QC_QGraphicsSceneContextMenuEvent->setCopy((q_copy_t)QGRAPHICSSCENECONTEXTMENUEVENT_copy);

   QC_QGraphicsSceneContextMenuEvent->addMethod("modifiers",                   (q_method_t)QGRAPHICSSCENECONTEXTMENUEVENT_modifiers);
   QC_QGraphicsSceneContextMenuEvent->addMethod("pos",                         (q_method_t)QGRAPHICSSCENECONTEXTMENUEVENT_pos);
   QC_QGraphicsSceneContextMenuEvent->addMethod("reason",                      (q_method_t)QGRAPHICSSCENECONTEXTMENUEVENT_reason);
   QC_QGraphicsSceneContextMenuEvent->addMethod("scenePos",                    (q_method_t)QGRAPHICSSCENECONTEXTMENUEVENT_scenePos);
   QC_QGraphicsSceneContextMenuEvent->addMethod("screenPos",                   (q_method_t)QGRAPHICSSCENECONTEXTMENUEVENT_screenPos);

   return QC_QGraphicsSceneContextMenuEvent;
}

QoreNamespace *initQGraphicsSceneContextMenuEventNS(QoreClass *qgraphicssceneevent)
{
   QoreNamespace *ns = new QoreNamespace("QGraphicsSceneContextMenuEvent");
   ns->addSystemClass(initQGraphicsSceneContextMenuEventClass(qgraphicssceneevent));

   // Reason enum
   ns->addConstant("Mouse",                    new QoreBigIntNode(QGraphicsSceneContextMenuEvent::Mouse));
   ns->addConstant("Keyboard",                 new QoreBigIntNode(QGraphicsSceneContextMenuEvent::Keyboard));
   ns->addConstant("Other",                    new QoreBigIntNode(QGraphicsSceneContextMenuEvent::Other));

   return ns;
}
