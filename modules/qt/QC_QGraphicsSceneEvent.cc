/*
 QC_QGraphicsSceneEvent.cc
 
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

#include "qore-qt.h"

#include "QC_QGraphicsSceneEvent.h"

qore_classid_t CID_QGRAPHICSSCENEEVENT;
QoreClass *QC_QGraphicsSceneEvent = 0;

//QGraphicsSceneEvent ( Type type )
static void QGRAPHICSSCENEEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsSceneEvent::Type type = (QGraphicsSceneEvent::Type)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QGRAPHICSSCENEEVENT, new QoreQGraphicsSceneEvent(type));
   return;
}

static void QGRAPHICSSCENEEVENT_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSceneEvent *qgse, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSCENEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//QWidget * widget () const
static AbstractQoreNode *QGRAPHICSSCENEEVENT_widget(QoreObject *self, QoreQGraphicsSceneEvent *qgse, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qwidget(qgse->widget());
}

QoreClass *initQGraphicsSceneEventClass(QoreClass *qevent)
{
   QC_QGraphicsSceneEvent = new QoreClass("QGraphicsSceneEvent", QDOM_GUI);
   CID_QGRAPHICSSCENEEVENT = QC_QGraphicsSceneEvent->getID();

   QC_QGraphicsSceneEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QGraphicsSceneEvent->setConstructor(QGRAPHICSSCENEEVENT_constructor);
   QC_QGraphicsSceneEvent->setCopy((q_copy_t)QGRAPHICSSCENEEVENT_copy);

   QC_QGraphicsSceneEvent->addMethod("widget",      (q_method_t)QGRAPHICSSCENEEVENT_widget);

   return QC_QGraphicsSceneEvent;
}
