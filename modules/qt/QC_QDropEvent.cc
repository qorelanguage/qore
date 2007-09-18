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

int CID_QDROPEVENT;
class QoreClass *QC_QDropEvent = 0;

//QDropEvent ( const QPoint & pos, Qt::DropActions actions, const QMimeData * data, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Type type = Drop )
static void QDROPEVENT_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QDROPEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as first argument to QDropEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   p = get_param(params, 1);
   Qt::DropActions actions = (Qt::DropActions)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   QoreQMimeData *data = (p && p->type == NT_OBJECT) ? (QoreQMimeData *)p->val.object->getReferencedPrivateData(CID_QMIMEDATA, xsink) : 0;
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

static void QDROPEVENT_copy(class Object *self, class Object *old, class QoreQDropEvent *qde, ExceptionSink *xsink)
{
   xsink->raiseException("QDROPEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void acceptProposedAction ()
static QoreNode *QDROPEVENT_acceptProposedAction(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   qde->acceptProposedAction();
   return 0;
}

//Qt::DropAction dropAction () const
static QoreNode *QDROPEVENT_dropAction(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qde->dropAction());
}

//Qt::KeyboardModifiers keyboardModifiers () const
static QoreNode *QDROPEVENT_keyboardModifiers(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qde->keyboardModifiers());
}

//const QMimeData * mimeData () const
static QoreNode *QDROPEVENT_mimeData(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   const QMimeData *qt_qobj = qde->mimeData();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//Qt::MouseButtons mouseButtons () const
static QoreNode *QDROPEVENT_mouseButtons(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qde->mouseButtons());
}

//const QPoint & pos () const
static QoreNode *QDROPEVENT_pos(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qde->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//Qt::DropActions possibleActions () const
static QoreNode *QDROPEVENT_possibleActions(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qde->possibleActions());
}

//Qt::DropAction proposedAction () const
static QoreNode *QDROPEVENT_proposedAction(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qde->proposedAction());
}

//void setDropAction ( Qt::DropAction action )
static QoreNode *QDROPEVENT_setDropAction(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::DropAction action = (Qt::DropAction)(p ? p->getAsInt() : 0);
   qde->setDropAction(action);
   return 0;
}

//QWidget * source () const
static QoreNode *QDROPEVENT_source(Object *self, QoreQDropEvent *qde, QoreNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qde->source();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
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
