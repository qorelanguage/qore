/*
 QC_QGraphicsSceneDragDropEvent.cc
 
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

#include "QC_QGraphicsSceneDragDropEvent.h"
#include "QC_QVariant.h"
#include "QC_QMimeData.h"
#include "QC_QPoint.h"
#include "QC_QPointF.h"

qore_classid_t CID_QGRAPHICSSCENEDRAGDROPEVENT;
QoreClass *QC_QGraphicsSceneDragDropEvent = 0;

//QGraphicsSceneDragDropEvent ( Type type = None );
static void QGRAPHICSSCENEDRAGDROPEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGraphicsSceneDragDropEvent::Type type = !is_nothing(p) ? (QGraphicsSceneDragDropEvent::Type)p->getAsInt() : QGraphicsSceneDragDropEvent::None;
   self->setPrivate(CID_QGRAPHICSSCENEDRAGDROPEVENT, new QoreQGraphicsSceneDragDropEvent(type));
   return;
}

static void QGRAPHICSSCENEDRAGDROPEVENT_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSceneDragDropEvent *qgsdde, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSCENEDRAGDROPEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void acceptProposedAction ()
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_acceptProposedAction(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   qgsdde->acceptProposedAction();
   return 0;
}

//Qt::MouseButtons buttons () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_buttons(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsdde->buttons());
}

//Qt::DropAction dropAction () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_dropAction(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsdde->dropAction());
}

//const QMimeData * mimeData () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_mimeData(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   const QMimeData *qt_qobj = qgsdde->mimeData();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      return rv_obj->refSelf();
   rv_obj = new QoreObject(QC_QMimeData, getProgram());
   QoreQtQMimeData *t_qobj = new QoreQtQMimeData(rv_obj, const_cast<QMimeData *>(qt_qobj), false);
   rv_obj->setPrivate(CID_QMIMEDATA, t_qobj);
   return rv_obj;
}

//Qt::KeyboardModifiers modifiers () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_modifiers(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsdde->modifiers());
}

//QPointF pos () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_pos(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgsdde->pos()));
}

//Qt::DropActions possibleActions () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_possibleActions(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsdde->possibleActions());
}

//Qt::DropAction proposedAction () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_proposedAction(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qgsdde->proposedAction());
}

//QPointF scenePos () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_scenePos(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qgsdde->scenePos()));
}

//QPoint screenPos () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_screenPos(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qgsdde->screenPos()));
}

//void setDropAction ( Qt::DropAction action )
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_setDropAction(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::DropAction action = (Qt::DropAction)(p ? p->getAsInt() : 0);
   qgsdde->setDropAction(action);
   return 0;
}

//QWidget * source () const
static AbstractQoreNode *QGRAPHICSSCENEDRAGDROPEVENT_source(QoreObject *self, QoreQGraphicsSceneDragDropEvent *qgsdde, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qwidget(qgsdde->source());
}

QoreClass *initQGraphicsSceneDragDropEventClass(QoreClass *qgraphicssceneevent)
{
   QC_QGraphicsSceneDragDropEvent = new QoreClass("QGraphicsSceneDragDropEvent", QDOM_GUI);
   CID_QGRAPHICSSCENEDRAGDROPEVENT = QC_QGraphicsSceneDragDropEvent->getID();

   QC_QGraphicsSceneDragDropEvent->addBuiltinVirtualBaseClass(qgraphicssceneevent);

   QC_QGraphicsSceneDragDropEvent->setConstructor(QGRAPHICSSCENEDRAGDROPEVENT_constructor);
   QC_QGraphicsSceneDragDropEvent->setCopy((q_copy_t)QGRAPHICSSCENEDRAGDROPEVENT_copy);

   QC_QGraphicsSceneDragDropEvent->addMethod("acceptProposedAction",        (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_acceptProposedAction);
   QC_QGraphicsSceneDragDropEvent->addMethod("buttons",                     (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_buttons);
   QC_QGraphicsSceneDragDropEvent->addMethod("dropAction",                  (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_dropAction);
   QC_QGraphicsSceneDragDropEvent->addMethod("mimeData",                    (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_mimeData);
   QC_QGraphicsSceneDragDropEvent->addMethod("modifiers",                   (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_modifiers);
   QC_QGraphicsSceneDragDropEvent->addMethod("pos",                         (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_pos);
   QC_QGraphicsSceneDragDropEvent->addMethod("possibleActions",             (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_possibleActions);
   QC_QGraphicsSceneDragDropEvent->addMethod("proposedAction",              (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_proposedAction);
   QC_QGraphicsSceneDragDropEvent->addMethod("scenePos",                    (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_scenePos);
   QC_QGraphicsSceneDragDropEvent->addMethod("screenPos",                   (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_screenPos);
   QC_QGraphicsSceneDragDropEvent->addMethod("setDropAction",               (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_setDropAction);
   QC_QGraphicsSceneDragDropEvent->addMethod("source",                      (q_method_t)QGRAPHICSSCENEDRAGDROPEVENT_source);

   return QC_QGraphicsSceneDragDropEvent;
}
