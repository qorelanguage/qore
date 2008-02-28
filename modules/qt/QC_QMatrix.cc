/*
 QC_QMatrix.cc
 
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
#include "QC_QMatrix.h"
#include "QC_QRect.h"
#include "QC_QRectF.h"
#include "QC_QPoint.h"
//#include "QC_QPointF.h"
#include "QC_QRegion.h"

#include "qore-qt.h"

qore_classid_t CID_QMATRIX;
QoreClass *QC_QMatrix = 0;

static void QMATRIX_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal m11 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal m12 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal m21 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal m22 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 5);
   qreal dy = p ? p->getAsFloat() : 0.0;

   QoreQMatrix *qf = new QoreQMatrix(m11, m12, m21, m22, dx, dy);
   self->setPrivate(CID_QMATRIX, qf);
}

static void QMATRIX_copy(class QoreObject *self, class QoreObject *old, class QoreQMatrix *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QMATRIX, new QoreQMatrix(*qf));
}

//qreal m11 () const
static AbstractQoreNode *QMATRIX_m11(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qm->m11());
}

//qreal m12 () const
static AbstractQoreNode *QMATRIX_m12(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qm->m12());
}

//qreal m21 () const
static AbstractQoreNode *QMATRIX_m21(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qm->m21());
}

//qreal m22 () const
static AbstractQoreNode *QMATRIX_m22(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qm->m22());
}

//qreal det () const
static AbstractQoreNode *QMATRIX_det(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qm->det());
}

//qreal dx () const
static AbstractQoreNode *QMATRIX_dx(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qm->dx());
}

//qreal dy () const
static AbstractQoreNode *QMATRIX_dy(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qm->dy());
}

//QMatrix inverted ( bool * invertible = 0 ) const
static AbstractQoreNode *QMATRIX_inverted(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* invertible = p;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->inverted());
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return o_qm;
}

//bool isIdentity () const
static AbstractQoreNode *QMATRIX_isIdentity(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qm->isIdentity());
}

//bool isInvertible () const
static AbstractQoreNode *QMATRIX_isInvertible(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qm->isInvertible());
}

//void map ( qreal x, qreal y, qreal * tx, qreal * ty ) const
//void map ( int x, int y, int * tx, int * ty ) const
//QPointF map ( const QPointF & point ) const
//QPoint map ( const QPoint & point ) const
//QLineF map ( const QLineF & line ) const
//QLine map ( const QLine & line ) const
//QPolygonF map ( const QPolygonF & polygon ) const
//QPolygon map ( const QPolygon & polygon ) const
//QRegion map ( const QRegion & region ) const
//QPainterPath map ( const QPainterPath & path ) const
static AbstractQoreNode *QMATRIX_map(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         QoreQRegion *region = (QoreQRegion *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QREGION, xsink);
         if (!region) {
            if (!xsink->isException())
               xsink->raiseException("QMATRIX-MAP-PARAM-ERROR", "QMatrix::map() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
            return 0;
         }
         ReferenceHolder<QoreQRegion> regionHolder(region, xsink);
         qm->map(*(static_cast<QRegion *>(region)));
         return 0;
      }
      ReferenceHolder<QoreQPoint> pointHolder(point, xsink);
      qm->map(*(static_cast<QPoint *>(point)));
      return 0;
   }
   xsink->raiseException("QMATRIX-MAP-ERROR", "QMatrix::map() not implemented for this argument type");
   return 0;
}

//QRectF mapRect ( const QRectF & rectangle ) const
//QRect mapRect ( const QRect & rectangle ) const
static AbstractQoreNode *QMATRIX_mapRect(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQRectF *rectangle = (QoreQRectF *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
         QoreQRect *rectangle = (QoreQRect *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink);
         if (!rectangle) {
            if (!xsink->isException())
               xsink->raiseException("QMATRIX-MAPRECT-PARAM-ERROR", "QMatrix::mapRect() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
            return 0;
         }
         ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
         QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
         QoreQRectF *q_qrf = new QoreQRectF(qm->mapRect(*(static_cast<QRect *>(rectangle))));
         o_qrf->setPrivate(CID_QRECTF, q_qrf);
         return o_qrf;
      }
      ReferenceHolder<QoreQRectF> rectangleHolder(rectangle, xsink);
      QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
      QoreQRectF *q_qrf = new QoreQRectF(qm->mapRect(*(static_cast<QRectF *>(rectangle))));
      o_qrf->setPrivate(CID_QRECTF, q_qrf);
      return o_qrf;
   }
   xsink->raiseException("QMATRIX-MAPRECT-ERROR", "expecting a QRect or QRectF object as sole argument to QMatrix::mapRect()");
   return 0;
}

//QPolygon mapToPolygon ( const QRect & rectangle ) const
//static AbstractQoreNode *QMATRIX_mapToPolygon(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreObject *p = test_object_param(params, 0);
//   QoreQRect *rectangle = p ? (QoreQRect *)p->getReferencedPrivateData(CID_QRECT, xsink) : 0;
//   if (!rectangle) {
//      if (!xsink->isException())
//         xsink->raiseException("QMATRIX-MAPTOPOLYGON-PARAM-ERROR", "expecting a QRect object as first argument to QMatrix::mapToPolygon()");
//      return 0;
//   }
//   ReferenceHolder<QoreQRect> holder(rectangle, xsink);
//   ??? return new QoreBigIntNode(qm->mapToPolygon(static_cast<QRect *>(rectangle)));
//}

//void reset ()
static AbstractQoreNode *QMATRIX_reset(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   qm->reset();
   return 0;
}

//QMatrix & rotate ( qreal degrees )
static AbstractQoreNode *QMATRIX_rotate(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal degrees = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->rotate(degrees));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return o_qm;
}

//QMatrix & scale ( qreal sx, qreal sy )
static AbstractQoreNode *QMATRIX_scale(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal sx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sy = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->scale(sx, sy));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return o_qm;
}

//void setMatrix ( qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy )
static AbstractQoreNode *QMATRIX_setMatrix(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal m11 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal m12 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal m21 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal m22 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 5);
   qreal dy = p ? p->getAsFloat() : 0.0;
   qm->setMatrix(m11, m12, m21, m22, dx, dy);
   return 0;
}

//QMatrix & shear ( qreal sh, qreal sv )
static AbstractQoreNode *QMATRIX_shear(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal sh = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sv = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->shear(sh, sv));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return o_qm;
}

//QMatrix & translate ( qreal dx, qreal dy )
static AbstractQoreNode *QMATRIX_translate(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->translate(dx, dy));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return o_qm;
}

class QoreClass *initQMatrixClass()
{
   tracein("initQMatrixClass()");
   
   QC_QMatrix = new QoreClass("QMatrix", QDOM_GUI);
   CID_QMATRIX = QC_QMatrix->getID();
   QC_QMatrix->setConstructor(QMATRIX_constructor);
   QC_QMatrix->setCopy((q_copy_t)QMATRIX_copy);

   QC_QMatrix->addMethod("m11",                         (q_method_t)QMATRIX_m11);
   QC_QMatrix->addMethod("m12",                         (q_method_t)QMATRIX_m12);
   QC_QMatrix->addMethod("m21",                         (q_method_t)QMATRIX_m21);
   QC_QMatrix->addMethod("m22",                         (q_method_t)QMATRIX_m22);
   QC_QMatrix->addMethod("det",                         (q_method_t)QMATRIX_det);
   QC_QMatrix->addMethod("dx",                          (q_method_t)QMATRIX_dx);
   QC_QMatrix->addMethod("dy",                          (q_method_t)QMATRIX_dy);
   QC_QMatrix->addMethod("inverted",                    (q_method_t)QMATRIX_inverted);
   QC_QMatrix->addMethod("isIdentity",                  (q_method_t)QMATRIX_isIdentity);
   QC_QMatrix->addMethod("isInvertible",                (q_method_t)QMATRIX_isInvertible);
   QC_QMatrix->addMethod("map",                         (q_method_t)QMATRIX_map);
   QC_QMatrix->addMethod("mapRect",                     (q_method_t)QMATRIX_mapRect);
   //QC_QMatrix->addMethod("mapToPolygon",                (q_method_t)QMATRIX_mapToPolygon);
   QC_QMatrix->addMethod("reset",                       (q_method_t)QMATRIX_reset);
   QC_QMatrix->addMethod("rotate",                      (q_method_t)QMATRIX_rotate);
   QC_QMatrix->addMethod("scale",                       (q_method_t)QMATRIX_scale);
   QC_QMatrix->addMethod("setMatrix",                   (q_method_t)QMATRIX_setMatrix);
   QC_QMatrix->addMethod("shear",                       (q_method_t)QMATRIX_shear);
   QC_QMatrix->addMethod("translate",                   (q_method_t)QMATRIX_translate);

   traceout("initQMatrixClass()");
   return QC_QMatrix;
}
