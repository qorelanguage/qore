/*
 QC_QTabletEvent.cc
 
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

#include "QC_QTabletEvent.h"
#include "QC_QPoint.h"
#include "QC_QPointF.h"

#include "qore-qt.h"

int CID_QTABLETEVENT;
class QoreClass *QC_QTabletEvent = 0;

//QTabletEvent ( Type type, const QPoint & pos, const QPoint & globalPos, const QPointF & hiResGlobalPos, int device, int pointerType, qreal pressure, int xTilt, int yTilt, qreal tangentialPressure, qreal rotation, int z, Qt::KeyboardModifiers keyState, qint64 uniqueID )
static void QTABLETEVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTabletEvent::Type type = (QTabletEvent::Type)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQPoint *pos = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QTABLETEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as second argument to QTabletEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);

   o = test_object_param(params, 2);
   QoreQPoint *globalPos = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!globalPos) {
      if (!xsink->isException())
         xsink->raiseException("QTABLETEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as third argument to QTabletEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> globalPosHolder(globalPos, xsink);

   o = test_object_param(params, 3);
   QoreQPointF *hiResGlobalPos = o ? (QoreQPointF *)o->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!hiResGlobalPos) {
      if (!xsink->isException())
         xsink->raiseException("QTABLETEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPointF object as fourth argument to QTabletEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPointF> hiResGlobalPosHolder(hiResGlobalPos, xsink);
   p = get_param(params, 4);
   int device = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int pointerType = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   qreal pressure = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 7);
   int xTilt = p ? p->getAsInt() : 0;
   p = get_param(params, 8);
   int yTilt = p ? p->getAsInt() : 0;
   p = get_param(params, 9);
   qreal tangentialPressure = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 10);
   qreal rotation = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 11);
   int z = p ? p->getAsInt() : 0;
   p = get_param(params, 12);
   Qt::KeyboardModifiers keyState = (Qt::KeyboardModifiers)(p ? p->getAsInt() : 0);
   p = get_param(params, 13);
   int64 uniqueID = p ? p->getAsBigInt() : 0;
   self->setPrivate(CID_QTABLETEVENT, new QoreQTabletEvent(type, *(static_cast<QPoint *>(pos)), *(static_cast<QPoint *>(globalPos)), *(static_cast<QPointF *>(hiResGlobalPos)), device, pointerType, pressure, xTilt, yTilt, tangentialPressure, rotation, z, keyState, uniqueID));
   return;
}

static void QTABLETEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQTabletEvent *qte, ExceptionSink *xsink)
{
   xsink->raiseException("QTABLETEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//TabletDevice device () const
static AbstractQoreNode *QTABLETEVENT_device(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->device());
}

//const QPoint & globalPos () const
static AbstractQoreNode *QTABLETEVENT_globalPos(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qte->globalPos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//int globalX () const
static AbstractQoreNode *QTABLETEVENT_globalX(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->globalX());
}

//int globalY () const
static AbstractQoreNode *QTABLETEVENT_globalY(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->globalY());
}

//const QPointF & hiResGlobalPos () const
static AbstractQoreNode *QTABLETEVENT_hiResGlobalPos(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qpf = new QoreObject(QC_QPointF, getProgram());
   QoreQPointF *q_qpf = new QoreQPointF(qte->hiResGlobalPos());
   o_qpf->setPrivate(CID_QPOINTF, q_qpf);
   return o_qpf;
}

//qreal hiResGlobalX () const
static AbstractQoreNode *QTABLETEVENT_hiResGlobalX(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qte->hiResGlobalX());
}

//qreal hiResGlobalY () const
static AbstractQoreNode *QTABLETEVENT_hiResGlobalY(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qte->hiResGlobalY());
}

//PointerType pointerType () const
static AbstractQoreNode *QTABLETEVENT_pointerType(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->pointerType());
}

//const QPoint & pos () const
static AbstractQoreNode *QTABLETEVENT_pos(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qte->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//qreal pressure () const
static AbstractQoreNode *QTABLETEVENT_pressure(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qte->pressure());
}

//qreal rotation () const
static AbstractQoreNode *QTABLETEVENT_rotation(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qte->rotation());
}

//qreal tangentialPressure () const
static AbstractQoreNode *QTABLETEVENT_tangentialPressure(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qte->tangentialPressure());
}

//qint64 uniqueId () const
static AbstractQoreNode *QTABLETEVENT_uniqueId(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->uniqueId());
}

//int x () const
static AbstractQoreNode *QTABLETEVENT_x(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->x());
}

//int xTilt () const
static AbstractQoreNode *QTABLETEVENT_xTilt(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->xTilt());
}

//int y () const
static AbstractQoreNode *QTABLETEVENT_y(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->y());
}

//int yTilt () const
static AbstractQoreNode *QTABLETEVENT_yTilt(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->yTilt());
}

//int z () const
static AbstractQoreNode *QTABLETEVENT_z(QoreObject *self, QoreQTabletEvent *qte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qte->z());
}

QoreClass *initQTabletEventClass(QoreClass *qinputevent)
{
   QC_QTabletEvent = new QoreClass("QTabletEvent", QDOM_GUI);
   CID_QTABLETEVENT = QC_QTabletEvent->getID();

   QC_QTabletEvent->addBuiltinVirtualBaseClass(qinputevent);

   QC_QTabletEvent->setConstructor(QTABLETEVENT_constructor);
   QC_QTabletEvent->setCopy((q_copy_t)QTABLETEVENT_copy);

   QC_QTabletEvent->addMethod("device",                      (q_method_t)QTABLETEVENT_device);
   QC_QTabletEvent->addMethod("globalPos",                   (q_method_t)QTABLETEVENT_globalPos);
   QC_QTabletEvent->addMethod("globalX",                     (q_method_t)QTABLETEVENT_globalX);
   QC_QTabletEvent->addMethod("globalY",                     (q_method_t)QTABLETEVENT_globalY);
   QC_QTabletEvent->addMethod("hiResGlobalPos",              (q_method_t)QTABLETEVENT_hiResGlobalPos);
   QC_QTabletEvent->addMethod("hiResGlobalX",                (q_method_t)QTABLETEVENT_hiResGlobalX);
   QC_QTabletEvent->addMethod("hiResGlobalY",                (q_method_t)QTABLETEVENT_hiResGlobalY);
   QC_QTabletEvent->addMethod("pointerType",                 (q_method_t)QTABLETEVENT_pointerType);
   QC_QTabletEvent->addMethod("pos",                         (q_method_t)QTABLETEVENT_pos);
   QC_QTabletEvent->addMethod("pressure",                    (q_method_t)QTABLETEVENT_pressure);
   QC_QTabletEvent->addMethod("rotation",                    (q_method_t)QTABLETEVENT_rotation);
   QC_QTabletEvent->addMethod("tangentialPressure",          (q_method_t)QTABLETEVENT_tangentialPressure);
   QC_QTabletEvent->addMethod("uniqueId",                    (q_method_t)QTABLETEVENT_uniqueId);
   QC_QTabletEvent->addMethod("x",                           (q_method_t)QTABLETEVENT_x);
   QC_QTabletEvent->addMethod("xTilt",                       (q_method_t)QTABLETEVENT_xTilt);
   QC_QTabletEvent->addMethod("y",                           (q_method_t)QTABLETEVENT_y);
   QC_QTabletEvent->addMethod("yTilt",                       (q_method_t)QTABLETEVENT_yTilt);
   QC_QTabletEvent->addMethod("z",                           (q_method_t)QTABLETEVENT_z);

   return QC_QTabletEvent;
}
