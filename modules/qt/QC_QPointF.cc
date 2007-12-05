/*
 QC_QPointF.cc
 
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

#include "QC_QPointF.h"

int CID_QPOINTF;
class QoreClass *QC_QPointF = 0;

//QPointF ()
//QPointF ( const QPoint & point )
//QPointF ( qreal x, qreal y )
static void QPOINTF_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QPOINTF, new QoreQPointF());
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QPOINTF-QPOINTF-PARAM-ERROR", "QPointF::QPointF() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return;
      }
      ReferenceHolder<QoreQPoint> pointHolder(point, xsink);
      self->setPrivate(CID_QPOINTF, new QoreQPointF(*(static_cast<QPoint *>(point))));
      return;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   self->setPrivate(CID_QPOINTF, new QoreQPointF(x, y));
   return;
}

static void QPOINTF_copy(class QoreObject *self, class QoreObject *old, class QoreQPointF *qpf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPOINTF, new QoreQPointF(*qpf));
   //xsink->raiseException("QPOINTF-COPY-ERROR", "objects of this class cannot be copied");
}

//bool isNull () const
static QoreNode *QPOINTF_isNull(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qpf->isNull());
}

////qreal & rx ()
//static QoreNode *QPOINTF_rx(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qpf->rx());
//}

////qreal & ry ()
//static QoreNode *QPOINTF_ry(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qpf->ry());
//}

//void setX ( qreal x )
static QoreNode *QPOINTF_setX(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal x = p ? p->getAsFloat() : 0.0;
   qpf->setX(x);
   return 0;
}

//void setY ( qreal y )
static QoreNode *QPOINTF_setY(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal y = p ? p->getAsFloat() : 0.0;
   qpf->setY(y);
   return 0;
}

//QPoint toPoint () const
static QoreNode *QPOINTF_toPoint(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qpf->toPoint());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//qreal x () const
static QoreNode *QPOINTF_x(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qpf->x());
}

//qreal y () const
static QoreNode *QPOINTF_y(QoreObject *self, QoreQPointF *qpf, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qpf->y());
}

QoreClass *initQPointFClass()
{
   QC_QPointF = new QoreClass("QPointF", QDOM_GUI);
   CID_QPOINTF = QC_QPointF->getID();

   QC_QPointF->setConstructor(QPOINTF_constructor);
   QC_QPointF->setCopy((q_copy_t)QPOINTF_copy);

   QC_QPointF->addMethod("isNull",                      (q_method_t)QPOINTF_isNull);
   //QC_QPointF->addMethod("rx",                          (q_method_t)QPOINTF_rx);
   //QC_QPointF->addMethod("ry",                          (q_method_t)QPOINTF_ry);
   QC_QPointF->addMethod("setX",                        (q_method_t)QPOINTF_setX);
   QC_QPointF->addMethod("setY",                        (q_method_t)QPOINTF_setY);
   QC_QPointF->addMethod("toPoint",                     (q_method_t)QPOINTF_toPoint);
   QC_QPointF->addMethod("x",                           (q_method_t)QPOINTF_x);
   QC_QPointF->addMethod("y",                           (q_method_t)QPOINTF_y);

   return QC_QPointF;
}
