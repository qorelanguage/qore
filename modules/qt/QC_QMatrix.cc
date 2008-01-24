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

int CID_QMATRIX;
QoreClass *QC_QMatrix = 0;

static void QMATRIX_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
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
static QoreNode *QMATRIX_m11(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qm->m11());
}

//qreal m12 () const
static QoreNode *QMATRIX_m12(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qm->m12());
}

//qreal m21 () const
static QoreNode *QMATRIX_m21(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qm->m21());
}

//qreal m22 () const
static QoreNode *QMATRIX_m22(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qm->m22());
}

//qreal det () const
static QoreNode *QMATRIX_det(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qm->det());
}

//qreal dx () const
static QoreNode *QMATRIX_dx(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qm->dx());
}

//qreal dy () const
static QoreNode *QMATRIX_dy(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qm->dy());
}

//QMatrix inverted ( bool * invertible = 0 ) const
static QoreNode *QMATRIX_inverted(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
//   QoreNode *p = get_param(params, 0);
//   ??? bool* invertible = p;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->inverted());
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
}

//bool isIdentity () const
static QoreNode *QMATRIX_isIdentity(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->isIdentity());
}

//bool isInvertible () const
static QoreNode *QMATRIX_isInvertible(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->isInvertible());
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
static QoreNode *QMATRIX_map(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         QoreQRegion *region = (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink);
         if (!region) {
            if (!xsink->isException())
               xsink->raiseException("QMATRIX-MAP-PARAM-ERROR", "QMatrix::map() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
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
static QoreNode *QMATRIX_mapRect(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectangle = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
         QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
         if (!rectangle) {
            if (!xsink->isException())
               xsink->raiseException("QMATRIX-MAPRECT-PARAM-ERROR", "QMatrix::mapRect() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
         QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
         QoreQRectF *q_qrf = new QoreQRectF(qm->mapRect(*(static_cast<QRect *>(rectangle))));
         o_qrf->setPrivate(CID_QRECTF, q_qrf);
         return new QoreNode(o_qrf);
      }
      ReferenceHolder<QoreQRectF> rectangleHolder(rectangle, xsink);
      QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
      QoreQRectF *q_qrf = new QoreQRectF(qm->mapRect(*(static_cast<QRectF *>(rectangle))));
      o_qrf->setPrivate(CID_QRECTF, q_qrf);
      return new QoreNode(o_qrf);
   }
   xsink->raiseException("QMATRIX-MAPRECT-ERROR", "expecting a QRect or QRectF object as sole argument to QMatrix::mapRect()");
   return 0;
}

//QPolygon mapToPolygon ( const QRect & rectangle ) const
//static QoreNode *QMATRIX_mapToPolygon(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
//   if (!rectangle) {
//      if (!xsink->isException())
//         xsink->raiseException("QMATRIX-MAPTOPOLYGON-PARAM-ERROR", "expecting a QRect object as first argument to QMatrix::mapToPolygon()");
//      return 0;
//   }
//   ReferenceHolder<QoreQRect> holder(rectangle, xsink);
//   ??? return new QoreNode((int64)qm->mapToPolygon(static_cast<QRect *>(rectangle)));
//}

//void reset ()
static QoreNode *QMATRIX_reset(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   qm->reset();
   return 0;
}

//QMatrix & rotate ( qreal degrees )
static QoreNode *QMATRIX_rotate(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal degrees = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->rotate(degrees));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
}

//QMatrix & scale ( qreal sx, qreal sy )
static QoreNode *QMATRIX_scale(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal sx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sy = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->scale(sx, sy));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
}

//void setMatrix ( qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy )
static QoreNode *QMATRIX_setMatrix(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
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
static QoreNode *QMATRIX_shear(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal sh = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sv = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->shear(sh, sv));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
}

//QMatrix & translate ( qreal dx, qreal dy )
static QoreNode *QMATRIX_translate(QoreObject *self, QoreQMatrix *qm, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qm = new QoreObject(self->getClass(CID_QMATRIX), getProgram());
   QoreQMatrix *q_qm = new QoreQMatrix(qm->translate(dx, dy));
   o_qm->setPrivate(CID_QMATRIX, q_qm);
   return new QoreNode(o_qm);
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
