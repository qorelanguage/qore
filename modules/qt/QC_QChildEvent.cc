/*
 QC_QChildEvent.cc
 
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

#include "QC_QChildEvent.h"

int CID_QCHILDEVENT;
class QoreClass *QC_QChildEvent = 0;

//QChildEvent ( Type type, QObject * child )
static void QCHILDEVENT_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QChildEvent::Type type = (QChildEvent::Type)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreAbstractQObject *child = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!child) {
      if (!xsink->isException())
         xsink->raiseException("QCHILDEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QObject object as second argument to QChildEvent::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> childHolder(static_cast<AbstractPrivateData *>(child), xsink);
   self->setPrivate(CID_QCHILDEVENT, new QoreQChildEvent(type, child->getQObject()));
   return;
}

static void QCHILDEVENT_copy(class Object *self, class Object *old, class QoreQChildEvent *qce, ExceptionSink *xsink)
{
   self->setPrivate(CID_QCHILDEVENT, new QoreQChildEvent(*qce));
}

//bool added () const
static QoreNode *QCHILDEVENT_added(Object *self, QoreQChildEvent *qce, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qce->added());
}

//QObject * child () const
static QoreNode *QCHILDEVENT_child(Object *self, QoreQChildEvent *qce, QoreNode *params, ExceptionSink *xsink)
{
   return return_qobject(qce->child());
}

//bool polished () const
static QoreNode *QCHILDEVENT_polished(Object *self, QoreQChildEvent *qce, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qce->polished());
}

//bool removed () const
static QoreNode *QCHILDEVENT_removed(Object *self, QoreQChildEvent *qce, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qce->removed());
}

QoreClass *initQChildEventClass(QoreClass *qevent)
{
   QC_QChildEvent = new QoreClass("QChildEvent", QDOM_GUI);
   CID_QCHILDEVENT = QC_QChildEvent->getID();

   QC_QChildEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QChildEvent->setConstructor(QCHILDEVENT_constructor);
   QC_QChildEvent->setCopy((q_copy_t)QCHILDEVENT_copy);

   QC_QChildEvent->addMethod("added",                       (q_method_t)QCHILDEVENT_added);
   QC_QChildEvent->addMethod("child",                       (q_method_t)QCHILDEVENT_child);
   QC_QChildEvent->addMethod("polished",                    (q_method_t)QCHILDEVENT_polished);
   QC_QChildEvent->addMethod("removed",                     (q_method_t)QCHILDEVENT_removed);

   return QC_QChildEvent;
}
