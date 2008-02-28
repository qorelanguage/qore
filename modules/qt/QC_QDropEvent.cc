/*
 QC_QDropEvent.cc
 
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

#include "QC_QDropEvent.h"
#include "QC_QMimeData.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

qore_classid_t CID_QDROPEVENT;
class QoreClass *QC_QDropEvent = 0;

//QDropEvent ( const QPoint & pos, Qt::DropActions actions, const QMimeData * data, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Type type = Drop )
static void QDROPEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQPoint *pos = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QDROPEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as first argument to QDropEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   Qt::DropActions actions = (Qt::DropActions)(p ? p->getAsInt() : 0);

   o = test_object_param(params, 2);
   QoreQMimeData *data = o ? (QoreQMimeData *)o->getReferencedPrivateData(CID_QMIMEDATA, xsink) : 0;
   if (!data) {
      if (!xsink->isException())
         xsink->raiseException("QDROPEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QMimeData object as third argument to QDropEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQMimeData> dataHolder(data, xsink);
   p = get_param(params, 3);
   Qt::MouseButtons buttons = (Qt::MouseButtons)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   Qt::KeyboardModifiers modifiers = (Qt::KeyboardModifiers)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   QDropEvent::Type type = (QDropEvent::Type)(!is_nothing(p) ? p->getAsInt() : QDropEvent::Drop);
   self->setPrivate(CID_QDROPEVENT, new QoreQDropEvent(*(static_cast<QPoint *>(pos)), actions, static_cast<QMimeData *>(data->qobj), buttons, modifiers, type));
   return;
}

static void QDROPEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQDropEvent *qde, ExceptionSink *xsink)
{
   xsink->raiseException("QDROPEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void acceptProposedAction ()
static AbstractQoreNode *QDROPEVENT_acceptProposedAction(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   qde->acceptProposedAction();
   return 0;
}

//Qt::DropAction dropAction () const
static AbstractQoreNode *QDROPEVENT_dropAction(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qde->dropAction());
}

//Qt::KeyboardModifiers keyboardModifiers () const
static AbstractQoreNode *QDROPEVENT_keyboardModifiers(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qde->keyboardModifiers());
}

//const QMimeData * mimeData () const
static AbstractQoreNode *QDROPEVENT_mimeData(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   const QMimeData *qt_qobj = qde->mimeData();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//Qt::MouseButtons mouseButtons () const
static AbstractQoreNode *QDROPEVENT_mouseButtons(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qde->mouseButtons());
}

//const QPoint & pos () const
static AbstractQoreNode *QDROPEVENT_pos(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qde->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//Qt::DropActions possibleActions () const
static AbstractQoreNode *QDROPEVENT_possibleActions(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qde->possibleActions());
}

//Qt::DropAction proposedAction () const
static AbstractQoreNode *QDROPEVENT_proposedAction(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qde->proposedAction());
}

//void setDropAction ( Qt::DropAction action )
static AbstractQoreNode *QDROPEVENT_setDropAction(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::DropAction action = (Qt::DropAction)(p ? p->getAsInt() : 0);
   qde->setDropAction(action);
   return 0;
}

//QWidget * source () const
static AbstractQoreNode *QDROPEVENT_source(QoreObject *self, QoreQDropEvent *qde, const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qde->source();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

QoreClass *initQDropEventClass(QoreClass *qevent)
{
   QC_QDropEvent = new QoreClass("QDropEvent", QDOM_GUI);
   CID_QDROPEVENT = QC_QDropEvent->getID();

   QC_QDropEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QDropEvent->setConstructor(QDROPEVENT_constructor);
   QC_QDropEvent->setCopy((q_copy_t)QDROPEVENT_copy);

   QC_QDropEvent->addMethod("acceptProposedAction",        (q_method_t)QDROPEVENT_acceptProposedAction);
   QC_QDropEvent->addMethod("dropAction",                  (q_method_t)QDROPEVENT_dropAction);
   QC_QDropEvent->addMethod("keyboardModifiers",           (q_method_t)QDROPEVENT_keyboardModifiers);
   QC_QDropEvent->addMethod("mimeData",                    (q_method_t)QDROPEVENT_mimeData);
   QC_QDropEvent->addMethod("mouseButtons",                (q_method_t)QDROPEVENT_mouseButtons);
   QC_QDropEvent->addMethod("pos",                         (q_method_t)QDROPEVENT_pos);
   QC_QDropEvent->addMethod("possibleActions",             (q_method_t)QDROPEVENT_possibleActions);
   QC_QDropEvent->addMethod("proposedAction",              (q_method_t)QDROPEVENT_proposedAction);
   QC_QDropEvent->addMethod("setDropAction",               (q_method_t)QDROPEVENT_setDropAction);
   QC_QDropEvent->addMethod("source",                      (q_method_t)QDROPEVENT_source);

   return QC_QDropEvent;
}
