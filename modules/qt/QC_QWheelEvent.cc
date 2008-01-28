/*
 QC_QWheelEvent.cc
 
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

#include "QC_QWheelEvent.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

int CID_QWHEELEVENT;
class QoreClass *QC_QWheelEvent = 0;

//QWheelEvent ( const QPoint & pos, int delta, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Qt::Orientation orient = Qt::Vertical )
//QWheelEvent ( const QPoint & pos, const QPoint & globalPos, int delta, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Qt::Orientation orient = Qt::Vertical )
static void QWHEELEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPoint *pos = p ? (QoreQPoint *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
	 xsink->raiseException("QWHEELEVENT-CONSTRUCTOR-PARAM-ERROR", "QWheelEvent::constructor() was expecting a QPoint object as the first argument");
      return;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);

   QoreQPoint *globalPos = 0;

   p = get_param(params, 1);
   int offset = 1;
   if (p && p->type == NT_OBJECT) {
      globalPos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
      if (!globalPos) {
	 if (!xsink->isException())
	    xsink->raiseException("QWHEELEVENT-CONSTRUCTOR-PARAM-ERROR", "this version of QWheelEvent::constructor() expects an object derived from QPoint as the second argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
	 return;
      }
      p = get_param(params, ++offset);
   }
   ReferenceHolder<QoreQPoint> globalPosHolder(globalPos, xsink);

   int delta = p ? p->getAsInt() : 0;
   p = get_param(params, ++offset);
   Qt::MouseButtons buttons = (Qt::MouseButtons)(p ? p->getAsInt() : 0);
   p = get_param(params, ++offset);
   Qt::KeyboardModifiers modifiers = (Qt::KeyboardModifiers)(p ? p->getAsInt() : 0);
   p = get_param(params, ++offset);
   Qt::Orientation orient = !is_nothing(p) ? (Qt::Orientation)p->getAsInt() : Qt::Vertical;

   if (globalPos)
      self->setPrivate(CID_QWHEELEVENT, new QoreQWheelEvent(*(static_cast<QPoint *>(pos)), *(static_cast<QPoint *>(globalPos)), delta, buttons, modifiers, orient));
   else
      self->setPrivate(CID_QWHEELEVENT, new QoreQWheelEvent(*(static_cast<QPoint *>(pos)), delta, buttons, modifiers, orient));
   return;
}
   
static void QWHEELEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQWheelEvent *qwe, ExceptionSink *xsink)
{
   xsink->raiseException("QWHEELEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::MouseButtons buttons () const
static QoreNode *QWHEELEVENT_buttons(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwe->buttons());
}

//int delta () const
static QoreNode *QWHEELEVENT_delta(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwe->delta());
}

//const QPoint & globalPos () const
static QoreNode *QWHEELEVENT_globalPos(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qwe->globalPos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//int globalX () const
static QoreNode *QWHEELEVENT_globalX(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwe->globalX());
}

//int globalY () const
static QoreNode *QWHEELEVENT_globalY(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwe->globalY());
}

//Qt::Orientation orientation () const
static QoreNode *QWHEELEVENT_orientation(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwe->orientation());
}

//const QPoint & pos () const
static QoreNode *QWHEELEVENT_pos(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qwe->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//int x () const
static QoreNode *QWHEELEVENT_x(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwe->x());
}

//int y () const
static QoreNode *QWHEELEVENT_y(QoreObject *self, QoreQWheelEvent *qwe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qwe->y());
}

QoreClass *initQWheelEventClass(QoreClass *qinputevent)
{
   QC_QWheelEvent = new QoreClass("QWheelEvent", QDOM_GUI);
   CID_QWHEELEVENT = QC_QWheelEvent->getID();

   QC_QWheelEvent->addBuiltinVirtualBaseClass(qinputevent);

   QC_QWheelEvent->setConstructor(QWHEELEVENT_constructor);
   QC_QWheelEvent->setCopy((q_copy_t)QWHEELEVENT_copy);

   QC_QWheelEvent->addMethod("buttons",                     (q_method_t)QWHEELEVENT_buttons);
   QC_QWheelEvent->addMethod("delta",                       (q_method_t)QWHEELEVENT_delta);
   QC_QWheelEvent->addMethod("globalPos",                   (q_method_t)QWHEELEVENT_globalPos);
   QC_QWheelEvent->addMethod("globalX",                     (q_method_t)QWHEELEVENT_globalX);
   QC_QWheelEvent->addMethod("globalY",                     (q_method_t)QWHEELEVENT_globalY);
   QC_QWheelEvent->addMethod("orientation",                 (q_method_t)QWHEELEVENT_orientation);
   QC_QWheelEvent->addMethod("pos",                         (q_method_t)QWHEELEVENT_pos);
   QC_QWheelEvent->addMethod("x",                           (q_method_t)QWHEELEVENT_x);
   QC_QWheelEvent->addMethod("y",                           (q_method_t)QWHEELEVENT_y);

   return QC_QWheelEvent;
}
