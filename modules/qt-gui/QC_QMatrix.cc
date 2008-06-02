/*
 QC_QMatrix.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#include "qore-qt-gui.h"

#include "QC_QMatrix.h"
#include "QC_QRect.h"
#include "QC_QRectF.h"
#include "QC_QRegion.h"
#include "QC_QPolygon.h"
#include "QC_QPolygonF.h"
#include "QC_QPoint.h"
#include "QC_QPointF.h"
#include "QC_QPainterPath.h"
#include "QC_QLine.h"
#include "QC_QLineF.h"

qore_classid_t CID_QMATRIX;
QoreClass *QC_QMatrix = 0;

//QMatrix ()
//QMatrix ( qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy )
//QMatrix ( const QMatrix & matrix )
static void QMATRIX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QMATRIX, new QoreQMatrix());
      return;
   }
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
   self->setPrivate(CID_QMATRIX, new QoreQMatrix(m11, m12, m21, m22, dx, dy));
   return;
}

static void QMATRIX_copy(QoreObject *self, QoreObject *old, QoreQMatrix *qm, ExceptionSink *xsink)
{
   self->setPrivate(CID_QMATRIX, new QoreQMatrix(*qm));
}

//qreal m11 () const
static AbstractQoreNode *QMATRIX_m11(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qm->m11());
}

//qreal m12 () const
static AbstractQoreNode *QMATRIX_m12(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qm->m12());
}

//qreal m21 () const
static AbstractQoreNode *QMATRIX_m21(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qm->m21());
}

//qreal m22 () const
static AbstractQoreNode *QMATRIX_m22(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qm->m22());
}

//qreal det () const
static AbstractQoreNode *QMATRIX_det(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qm->det());
}

//qreal dx () const
static AbstractQoreNode *QMATRIX_dx(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qm->dx());
}

//qreal dy () const
static AbstractQoreNode *QMATRIX_dy(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qm->dy());
}

//QMatrix inverted ( bool * invertible = 0 ) const
static AbstractQoreNode *QMATRIX_inverted(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const ReferenceNode *ri = test_reference_param(params, 0);
   bool invertible;
   QMatrix matrix;

   if (ri) {
      AutoVLock vl(xsink);
      ReferenceHelper refi(ri, vl, xsink);
      if (!refi)
	 return 0;

      matrix = qm->inverted(&invertible);

      refi.assign(get_bool_node(invertible), xsink);
      if (*xsink)
	 return 0;
   }
   else
      matrix = qm->inverted(&invertible);
      
   return return_object(QC_QMatrix, new QoreQMatrix(matrix));
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
      QoreQPoint *point = (QoreQPoint *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         QoreQLineF *line = (QoreQLineF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLINEF, xsink);
         if (!line) {
            QoreQLine *line = (QoreQLine *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLINE, xsink);
            if (!line) {
               QoreQPolygonF *polygon = (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink);
               if (!polygon) {
                  QoreQPolygon *polygon = (QoreQPolygon *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGON, xsink);
                  if (!polygon) {
                     QoreQRegion *region = (QoreQRegion *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QREGION, xsink);
                     if (!region) {
                        QoreQPainterPath *path = (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
                        if (!path) {
                           QoreQPointF *point = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
                           if (!point) {
                              if (!xsink->isException())
                                 xsink->raiseException("QMATRIX-MAP-PARAM-ERROR", "QMatrix::map() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
                              return 0;
                           }
                           ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
                           QPointF rv = qm->map(*(static_cast<QPointF *>(point)));
                           return return_object(QC_QPointF, new QoreQPointF(rv));
                        }
                        ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
                        QPainterPath rv = qm->map(*(static_cast<QPainterPath *>(path)));
                        return return_object(QC_QPainterPath, new QoreQPainterPath(rv));
                     }
                     ReferenceHolder<AbstractPrivateData> regionHolder(static_cast<AbstractPrivateData *>(region), xsink);
                     QRegion rv = qm->map(*(static_cast<QRegion *>(region)));
                     return return_object(QC_QRegion, new QoreQRegion(rv));
                  }
                  ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
                  QPolygon rv = qm->map(*(static_cast<QPolygon *>(polygon)));
		  return return_object(QC_QPolygon, new QoreQPolygon(rv));
               }
               ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
               QPolygonF rv = qm->map(*(static_cast<QPolygonF *>(polygon)));
               return return_object(QC_QPolygonF, new QoreQPolygonF(rv));
            }
            ReferenceHolder<AbstractPrivateData> lineHolder(static_cast<AbstractPrivateData *>(line), xsink);
            QLine rv = qm->map(*(static_cast<QLine *>(line)));
	    return return_object(QC_QLine, new QoreQLine(rv));
         }
         ReferenceHolder<AbstractPrivateData> lineHolder(static_cast<AbstractPrivateData *>(line), xsink);
         QLineF rv = qm->map(*(static_cast<QLineF *>(line)));
	 return return_object(QC_QLineF, new QoreQLineF(rv));
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      QPoint rv = qm->map(*(static_cast<QPoint *>(point)));
      return return_object(QC_QPoint, new QoreQPoint(rv));
   }
   if (p && p->getType() == NT_INT) {
      int x = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int y = p ? p->getAsInt() : 0;
      
      const ReferenceNode *rx = test_reference_param(params, 2);
      if (!rx) {
	 xsink->raiseException("QMATRIX-MAP-ERROR", "expecting an lvalue reference as the third argument for this version of QMatrix::map()");
	 return 0;
      }

      const ReferenceNode *ry = test_reference_param(params, 3);
      if (!ry) {
	 xsink->raiseException("QMATRIX-MAP-ERROR", "expecting an lvalue reference as the fourth argument for this version of QMatrix::map()");
	 return 0;
      }

      int tx, ty;
      qm->map(x, y, &tx, &ty);

      AutoVLock vl(xsink);
      ReferenceHelper refx(rx, vl, xsink);
      if (!refx)
	 return 0;

      ReferenceHelper refy(ry, vl, xsink);
      if (!refy)
	 return 0;

      if (refx.assign(new QoreBigIntNode(tx), xsink))
	 return 0;

      refy.assign(new QoreBigIntNode(ty), xsink);
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
     
   const ReferenceNode *rx = test_reference_param(params, 2);
   if (!rx) {
      xsink->raiseException("QMATRIX-MAP-ERROR", "expecting an lvalue reference as the third argument for this version of QMatrix::map()");
      return 0;
   }

   const ReferenceNode *ry = test_reference_param(params, 3);
   if (!ry) {
      xsink->raiseException("QMATRIX-MAP-ERROR", "expecting an lvalue reference as the fourth argument for this version of QMatrix::map()");
      return 0;
   }
   
   qreal tx, ty;
   qm->map(x, y, &tx, &ty);

   AutoVLock vl(xsink);
   ReferenceHelper refx(rx, vl, xsink);
   if (!refx)
      return 0;
   
   ReferenceHelper refy(ry, vl, xsink);
   if (!refy)
      return 0;
   
   if (refx.assign(new QoreFloatNode(tx), xsink))
      return 0;
   
   refy.assign(new QoreFloatNode(ty), xsink);
   return 0;
}

//QRectF mapRect ( const QRectF & rectangle ) const
//QRect mapRect ( const QRect & rectangle ) const
static AbstractQoreNode *QMATRIX_mapRect(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_OBJECT) {
      xsink->raiseException("QMATRIX-MAPRECT-PARAM-ERROR", "QMatrix::mapRect() requires an object derived from QRect or QRectF as the sole argument (type passed: '%s')", p ? p->getTypeName() : "NOTHING");
      return 0;
   }

   const QoreObject *o = reinterpret_cast<const QoreObject *>(p);

   QoreQRect *rectangle = (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink);
   if (!rectangle) {
      QoreQRectF *rectangle = (QoreQRectF *)o->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
	 if (!xsink->isException())
	    xsink->raiseException("QMATRIX-MAPRECT-PARAM-ERROR", "QMatrix::mapRect() does not know how to handle arguments of class '%s' as passed as the first argument", o->getClassName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      return return_object(QC_QRectF, new QoreQRectF(qm->mapRect(*(static_cast<QRectF *>(rectangle)))));
   }
   ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
   return return_object(QC_QRectF, new QoreQRectF(qm->mapRect(*(static_cast<QRect *>(rectangle)))));
}

//QPolygon mapToPolygon ( const QRect & rectangle ) const
static AbstractQoreNode *QMATRIX_mapToPolygon(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQRect *rectangle = (p && p->getType() == NT_OBJECT) ? (QoreQRect *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QMATRIX-MAPTOPOLYGON-PARAM-ERROR", "expecting a QRect object as first argument to QMatrix::mapToPolygon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
   return return_object(QC_QPolygon, new QoreQPolygon(qm->mapToPolygon(*(static_cast<QRect *>(rectangle)))));
}

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
   return return_object(QC_QMatrix, new QoreQMatrix(qm->rotate(degrees)));
}

//QMatrix & scale ( qreal sx, qreal sy )
static AbstractQoreNode *QMATRIX_scale(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal sx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sy = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QMatrix, new QoreQMatrix(qm->scale(sx, sy)));
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
   return return_object(QC_QMatrix, new QoreQMatrix(qm->shear(sh, sv)));
}

//QMatrix & translate ( qreal dx, qreal dy )
static AbstractQoreNode *QMATRIX_translate(QoreObject *self, QoreQMatrix *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QMatrix, new QoreQMatrix(qm->translate(dx, dy)));
}

QoreClass *initQMatrixClass()
{
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
   QC_QMatrix->addMethod("mapToPolygon",                (q_method_t)QMATRIX_mapToPolygon);
   QC_QMatrix->addMethod("reset",                       (q_method_t)QMATRIX_reset);
   QC_QMatrix->addMethod("rotate",                      (q_method_t)QMATRIX_rotate);
   QC_QMatrix->addMethod("scale",                       (q_method_t)QMATRIX_scale);
   QC_QMatrix->addMethod("setMatrix",                   (q_method_t)QMATRIX_setMatrix);
   QC_QMatrix->addMethod("shear",                       (q_method_t)QMATRIX_shear);
   QC_QMatrix->addMethod("translate",                   (q_method_t)QMATRIX_translate);

   return QC_QMatrix;
}
