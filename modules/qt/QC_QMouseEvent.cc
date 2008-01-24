/*
 QC_QMouseEvent.cc
 
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

#include "QC_QMouseEvent.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

int CID_QMOUSEEVENT;

class QoreClass *QC_QMouseEvent = 0;

static void QMOUSEEVENT_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreQMouseEvent *qr;

   QoreNode *p = get_param(params, 0);
   QEvent::Type type = (QEvent::Type)(p ? p->getAsInt() : 0);

   p = test_param(params, NT_OBJECT, 1);
        
   QoreQPoint *point = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (*xsink)
      return;
   if (!point) {
      xsink->raiseException("QMOUSEEVENT-CONSTRUCTOR-ERROR", "expecting a QPoint object as second argument to QMouseEvent::constructor()");
      return;
   }

   p = get_param(params, 2);
   int offset = 0;
   QoreQPoint *globalPos = 0;
   if (p && p->type == NT_OBJECT) {
      globalPos = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
      if (*xsink)
	 return;
      if (!globalPos) {
	 xsink->raiseException("QMOUSEEVENT-CONSTRUCTOR-ERROR", "object in third argument is not derived from QPoint (class: '%s')", p->val.object->getClass()->getName());
	 return;
      }

      p = get_param(params, 3);
      offset = 1;
   }

   Qt::MouseButton mb = (Qt::MouseButton)(p ? p->getAsInt() : 0);

   p = get_param(params, 3 + offset);
   Qt::MouseButtons mbs = (Qt::MouseButtons)(p ? p->getAsInt() : 0);

   p = get_param(params, 4 + offset);
   Qt::KeyboardModifiers km = (Qt::KeyboardModifiers)(p ? p->getAsInt() : 0);

   if (!offset)
      qr = new QoreQMouseEvent(type, *point, mb, mbs, km);
   else
      qr = new QoreQMouseEvent(type, *point, *globalPos, mb, mbs, km);

   self->setPrivate(CID_QMOUSEEVENT, qr);
}

static void QMOUSEEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQMouseEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QMOUSEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::MouseButton button () const
static QoreNode *QMOUSEEVENT_button(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qme->button());
}

//Qt::MouseButtons buttons () const
static QoreNode *QMOUSEEVENT_buttons(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qme->buttons());
}

//const QPoint & globalPos () const
static QoreNode *QMOUSEEVENT_globalPos(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qme->globalPos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//int globalX () const
static QoreNode *QMOUSEEVENT_globalX(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qme->globalX());
}

//int globalY () const
static QoreNode *QMOUSEEVENT_globalY(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qme->globalY());
}

//const QPoint & pos () const
static QoreNode *QMOUSEEVENT_pos(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qme->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//int x () const
static QoreNode *QMOUSEEVENT_x(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qme->x());
}

//int y () const
static QoreNode *QMOUSEEVENT_y(QoreObject *self, QoreQMouseEvent *qme, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qme->y());
}

class QoreClass *initQMouseEventClass(class QoreClass *qinputevent)
{
   tracein("initQMouseEventClass()");
   
   QC_QMouseEvent = new QoreClass("QMouseEvent", QDOM_GUI);
   CID_QMOUSEEVENT = QC_QMouseEvent->getID();

   QC_QMouseEvent->addBuiltinVirtualBaseClass(qinputevent);

   QC_QMouseEvent->setConstructor(QMOUSEEVENT_constructor);
   QC_QMouseEvent->setCopy((q_copy_t)QMOUSEEVENT_copy);

   QC_QMouseEvent->addMethod("button",                      (q_method_t)QMOUSEEVENT_button);
   QC_QMouseEvent->addMethod("buttons",                     (q_method_t)QMOUSEEVENT_buttons);
   QC_QMouseEvent->addMethod("globalPos",                   (q_method_t)QMOUSEEVENT_globalPos);
   QC_QMouseEvent->addMethod("globalX",                     (q_method_t)QMOUSEEVENT_globalX);
   QC_QMouseEvent->addMethod("globalY",                     (q_method_t)QMOUSEEVENT_globalY);
   QC_QMouseEvent->addMethod("pos",                         (q_method_t)QMOUSEEVENT_pos);
   QC_QMouseEvent->addMethod("x",                           (q_method_t)QMOUSEEVENT_x);
   QC_QMouseEvent->addMethod("y",                           (q_method_t)QMOUSEEVENT_y);

   traceout("initQMouseEventClass()");
   return QC_QMouseEvent;
}
