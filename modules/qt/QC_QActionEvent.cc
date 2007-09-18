/*
 QC_QActionEvent.cc
 
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

#include "QC_QActionEvent.h"

int CID_QACTIONEVENT;
class QoreClass *QC_QActionEvent = 0;

//QActionEvent ( int type, QAction * action, QAction * before = 0 )
static void QACTIONEVENT_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int type = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQAction *action = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QACTIONEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QAction object as second argument to QActionEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQAction> actionHolder(action, xsink);
   p = get_param(params, 2);
   QoreQAction *before = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<QoreQAction> beforeHolder(before, xsink);
   self->setPrivate(CID_QACTIONEVENT, new QoreQActionEvent(type, static_cast<QAction *>(action->qobj), before ? static_cast<QAction *>(before->qobj) : 0));
   return;
}

static void QACTIONEVENT_copy(class Object *self, class Object *old, class QoreQActionEvent *qae, ExceptionSink *xsink)
{
   xsink->raiseException("QACTIONEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//QAction * action () const
static QoreNode *QACTIONEVENT_action(Object *self, QoreQActionEvent *qae, QoreNode *params, ExceptionSink *xsink)
{
   QAction *qt_qobj = qae->action();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//QAction * before () const
static QoreNode *QACTIONEVENT_before(Object *self, QoreQActionEvent *qae, QoreNode *params, ExceptionSink *xsink)
{
   QAction *qt_qobj = qae->before();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

QoreClass *initQActionEventClass(QoreClass *qevent)
{
   QC_QActionEvent = new QoreClass("QActionEvent", QDOM_GUI);
   CID_QACTIONEVENT = QC_QActionEvent->getID();

   QC_QActionEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QActionEvent->setConstructor(QACTIONEVENT_constructor);
   QC_QActionEvent->setCopy((q_copy_t)QACTIONEVENT_copy);

   QC_QActionEvent->addMethod("action",                      (q_method_t)QACTIONEVENT_action);
   QC_QActionEvent->addMethod("before",                      (q_method_t)QACTIONEVENT_before);

   return QC_QActionEvent;
}
