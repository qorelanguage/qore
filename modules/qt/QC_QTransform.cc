/*
 QC_QTransform.cc
 
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

#include "QC_QTransform.h"

qore_classid_t CID_QTRANSFORM;
QoreClass *QC_QTransform = 0;

//QTransform ()
//QTransform ( qreal h11, qreal h12, qreal h13, qreal h21, qreal h22, qreal h23, qreal h31, qreal h32, qreal h33 = 1.0 )
//QTransform ( qreal h11, qreal h12, qreal h21, qreal h22, qreal dx, qreal dy )
//QTransform ( const QMatrix & matrix )
static void QTRANSFORM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTRANSFORM, new QoreQTransform());
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQMatrix *matrix = (QoreQMatrix *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QMATRIX, xsink);
      if (!matrix) {
         if (!xsink->isException())
            xsink->raiseException("QTRANSFORM-CONSTRUCTOR-PARAM-ERROR", "QTransform::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
      self->setPrivate(CID_QTRANSFORM, new QoreQTransform(*(static_cast<QMatrix *>(matrix))));
      return;
   }
   qreal h11 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal h12 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal h13 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h21 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal h22 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 5);
   qreal h23 = p ? p->getAsFloat() : 0.0;

   if (num_params(params) == 6) {
      self->setPrivate(CID_QTRANSFORM, new QoreQTransform(h11, h12, h13, h21, h22, h23));
      return;
   }

   p = get_param(params, 6);
   qreal h31 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 7);
   qreal h32 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 8);
   qreal h33 = p ? p->getAsFloat() : 1.0;
   self->setPrivate(CID_QTRANSFORM, new QoreQTransform(h11, h12, h13, h21, h22, h23, h31, h32, h33));
   return;
}

static void QTRANSFORM_copy(QoreObject *self, QoreObject *old, QoreQTransform *qt, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTRANSFORM, new QoreQTransform(*qt));
}

//qreal m11 () const
static AbstractQoreNode *QTRANSFORM_m11(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m11());
}

//qreal m12 () const
static AbstractQoreNode *QTRANSFORM_m12(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m12());
}

//qreal m13 () const
static AbstractQoreNode *QTRANSFORM_m13(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m13());
}

//qreal m21 () const
static AbstractQoreNode *QTRANSFORM_m21(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m21());
}

//qreal m22 () const
static AbstractQoreNode *QTRANSFORM_m22(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m22());
}

//qreal m23 () const
static AbstractQoreNode *QTRANSFORM_m23(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m23());
}

//qreal m31 () const
static AbstractQoreNode *QTRANSFORM_m31(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m31());
}

//qreal m32 () const
static AbstractQoreNode *QTRANSFORM_m32(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m32());
}

//qreal m33 () const
static AbstractQoreNode *QTRANSFORM_m33(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->m33());
}

//QTransform adjoint () const
static AbstractQoreNode *QTRANSFORM_adjoint(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QTransform, new QoreQTransform(qt->adjoint()));
}

//qreal det () const
static AbstractQoreNode *QTRANSFORM_det(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->det());
}

//qreal determinant () const
static AbstractQoreNode *QTRANSFORM_determinant(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->determinant());
}

//qreal dx () const
static AbstractQoreNode *QTRANSFORM_dx(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->dx());
}

//qreal dy () const
static AbstractQoreNode *QTRANSFORM_dy(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qt->dy());
}

//QTransform inverted ( bool * invertible = 0 ) const
static AbstractQoreNode *QTRANSFORM_inverted(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const ReferenceNode *ri = test_reference_param(params, 0);
   bool invertible;
   QTransform transform;

   if (ri) {
      AutoVLock vl(xsink);
      ReferenceHelper refi(ri, vl, xsink);
      if (!refi)
	 return 0;

      transform = qt->inverted(&invertible);

      refi.assign(get_bool_node(invertible), xsink);
      if (*xsink)
	 return 0;
   }
   else
      transform = qt->inverted(&invertible);
      
   return return_object(QC_QTransform, new QoreQTransform(transform));
}

//bool isAffine () const
static AbstractQoreNode *QTRANSFORM_isAffine(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qt->isAffine());
}

//bool isIdentity () const
static AbstractQoreNode *QTRANSFORM_isIdentity(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qt->isIdentity());
}

//bool isInvertible () const
static AbstractQoreNode *QTRANSFORM_isInvertible(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qt->isInvertible());
}

//bool isRotating () const
static AbstractQoreNode *QTRANSFORM_isRotating(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qt->isRotating());
}

//bool isScaling () const
static AbstractQoreNode *QTRANSFORM_isScaling(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qt->isScaling());
}

//bool isTranslating () const
static AbstractQoreNode *QTRANSFORM_isTranslating(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qt->isTranslating());
}

//void map ( int x, int y, int * tx, int * ty ) const
//void map ( qreal x, qreal y, qreal * tx, qreal * ty ) const
//QPointF map ( const QPointF & p ) const
//QPoint map ( const QPoint & point ) const
//QLine map ( const QLine & l ) const
//QLineF map ( const QLineF & line ) const
//QPolygonF map ( const QPolygonF & polygon ) const
//QPolygon map ( const QPolygon & polygon ) const
//QRegion map ( const QRegion & region ) const
//QPainterPath map ( const QPainterPath & path ) const
static AbstractQoreNode *QTRANSFORM_map(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         QoreQLine *l = (QoreQLine *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLINE, xsink);
         if (!l) {
            QoreQLineF *line = (QoreQLineF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLINEF, xsink);
            if (!line) {
               QoreQPolygonF *polygon = (QoreQPolygonF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGONF, xsink);
               if (!polygon) {
                  QoreQPolygon *polygon = (QoreQPolygon *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOLYGON, xsink);
                  if (!polygon) {
                     QoreQRegion *region = (QoreQRegion *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QREGION, xsink);
                     if (!region) {
                        QoreQPainterPath *path = (QoreQPainterPath *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTERPATH, xsink);
                        if (!path) {
                           QoreQPointF *p = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
                           if (!p) {
                              if (!xsink->isException())
                                 xsink->raiseException("QTRANSFORM-MAP-PARAM-ERROR", "QTransform::map() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
                              return 0;
                           }
                           ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(p), xsink);
                           qt->map(*(static_cast<QPointF *>(p)));
                           return 0;
                        }
                        ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
                        qt->map(*(static_cast<QPainterPath *>(path)));
                        return 0;
                     }
                     ReferenceHolder<AbstractPrivateData> regionHolder(static_cast<AbstractPrivateData *>(region), xsink);
                     qt->map(*(static_cast<QRegion *>(region)));
                     return 0;
                  }
                  ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
                  qt->map(*(static_cast<QPolygon *>(polygon)));
                  return 0;
               }
               ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
               qt->map(*(static_cast<QPolygonF *>(polygon)));
               return 0;
            }
            ReferenceHolder<AbstractPrivateData> lineHolder(static_cast<AbstractPrivateData *>(line), xsink);
            qt->map(*(static_cast<QLineF *>(line)));
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> lHolder(static_cast<AbstractPrivateData *>(l), xsink);
         qt->map(*(static_cast<QLine *>(l)));
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      qt->map(*(static_cast<QPoint *>(point)));
      return 0;
   }
   if (p && p->getType() == NT_INT) {
      int x = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int y = p ? p->getAsInt() : 0;
      
      const ReferenceNode *rx = test_reference_param(params, 2);
      if (!rx) {
	 xsink->raiseException("QTRANSFORM-MAP-ERROR", "expecting an lvalue reference as the third argument for this version of QTransform::map()");
	 return 0;
      }

      const ReferenceNode *ry = test_reference_param(params, 3);
      if (!ry) {
	 xsink->raiseException("QTRANSFORM-MAP-ERROR", "expecting an lvalue reference as the fourth argument for this version of QTransform::map()");
	 return 0;
      }

      int tx, ty;
      qt->map(x, y, &tx, &ty);

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
   qt->map(x, y, &tx, &ty);

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
static AbstractQoreNode *QTRANSFORM_mapRect(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   if (!p || p->getType() != NT_OBJECT) {
      xsink->raiseException("QTRANSFORM-MAPRECT-PARAM-ERROR", "QTransform::mapRect() requires an object derived from QRect or QRectF as the sole argument (type passed: '%s')", p ? p->getTypeName() : "NOTHING");
      return 0;
   }

   const QoreObject *o = reinterpret_cast<const QoreObject *>(p);

   QoreQRect *rectangle = (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink);
   if (!rectangle) {
      QoreQRectF *rectangle = (QoreQRectF *)o->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
	 if (!xsink->isException())
	    xsink->raiseException("QTRANSFORM-MAPRECT-PARAM-ERROR", "QTransform::mapRect() does not know how to handle arguments of class '%s' as passed as the first argument", o->getClassName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      return return_object(QC_QRectF, new QoreQRectF(qt->mapRect(*(static_cast<QRectF *>(rectangle)))));
   }
   ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
   return return_object(QC_QRectF, new QoreQRectF(qt->mapRect(*(static_cast<QRect *>(rectangle)))));
}

//QPolygon mapToPolygon ( const QRect & rectangle ) const
static AbstractQoreNode *QTRANSFORM_mapToPolygon(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQRect *rectangle = (p && p->getType() == NT_OBJECT) ? (QoreQRect *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QTRANSFORM-MAPTOPOLYGON-PARAM-ERROR", "expecting a QRect object as first argument to QTransform::mapToPolygon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
   return return_object(QC_QPolygon, new QoreQPolygon(qt->mapToPolygon(*(static_cast<QRect *>(rectangle)))));
}

//void reset ()
static AbstractQoreNode *QTRANSFORM_reset(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   qt->reset();
   return 0;
}

//QTransform & rotate ( qreal angle, Qt::Axis axis = Qt::ZAxis )
static AbstractQoreNode *QTRANSFORM_rotate(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal angle = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   Qt::Axis axis = !is_nothing(p) ? (Qt::Axis)p->getAsInt() : Qt::ZAxis;
   return return_object(QC_QTransform, new QoreQTransform(qt->rotate(angle, axis)));
}

//QTransform & rotateRadians ( qreal angle, Qt::Axis axis = Qt::ZAxis )
static AbstractQoreNode *QTRANSFORM_rotateRadians(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal angle = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   Qt::Axis axis = !is_nothing(p) ? (Qt::Axis)p->getAsInt() : Qt::ZAxis;
   return return_object(QC_QTransform, new QoreQTransform(qt->rotateRadians(angle, axis)));
}

//QTransform & scale ( qreal sx, qreal sy )
static AbstractQoreNode *QTRANSFORM_scale(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal sx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sy = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QTransform, new QoreQTransform(qt->scale(sx, sy)));
}

//void setMatrix ( qreal m11, qreal m12, qreal m13, qreal m21, qreal m22, qreal m23, qreal m31, qreal m32, qreal m33 )
static AbstractQoreNode *QTRANSFORM_setMatrix(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal m11 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal m12 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal m13 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal m21 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal m22 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 5);
   qreal m23 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 6);
   qreal m31 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 7);
   qreal m32 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 8);
   qreal m33 = p ? p->getAsFloat() : 0.0;
   qt->setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
   return 0;
}

//QTransform & shear ( qreal sh, qreal sv )
static AbstractQoreNode *QTRANSFORM_shear(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal sh = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal sv = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QTransform, new QoreQTransform(qt->shear(sh, sv)));
}

//const QMatrix & toAffine () const
static AbstractQoreNode *QTRANSFORM_toAffine(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QMatrix, new QoreQMatrix(qt->toAffine()));
}

//QTransform & translate ( qreal dx, qreal dy )
static AbstractQoreNode *QTRANSFORM_translate(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   return return_object(QC_QTransform, new QoreQTransform(qt->translate(dx, dy)));
}

//QTransform transposed () const
static AbstractQoreNode *QTRANSFORM_transposed(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QTransform, new QoreQTransform(qt->transposed()));
}

//TransformationType type () const
static AbstractQoreNode *QTRANSFORM_type(QoreObject *self, QoreQTransform *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->type());
}

static QoreClass *initQTransformClass()
{
   QC_QTransform = new QoreClass("QTransform", QDOM_GUI);
   CID_QTRANSFORM = QC_QTransform->getID();

   QC_QTransform->setConstructor(QTRANSFORM_constructor);
   QC_QTransform->setCopy((q_copy_t)QTRANSFORM_copy);

   QC_QTransform->addMethod("m11",                         (q_method_t)QTRANSFORM_m11);
   QC_QTransform->addMethod("m12",                         (q_method_t)QTRANSFORM_m12);
   QC_QTransform->addMethod("m13",                         (q_method_t)QTRANSFORM_m13);
   QC_QTransform->addMethod("m21",                         (q_method_t)QTRANSFORM_m21);
   QC_QTransform->addMethod("m22",                         (q_method_t)QTRANSFORM_m22);
   QC_QTransform->addMethod("m23",                         (q_method_t)QTRANSFORM_m23);
   QC_QTransform->addMethod("m31",                         (q_method_t)QTRANSFORM_m31);
   QC_QTransform->addMethod("m32",                         (q_method_t)QTRANSFORM_m32);
   QC_QTransform->addMethod("m33",                         (q_method_t)QTRANSFORM_m33);
   QC_QTransform->addMethod("adjoint",                     (q_method_t)QTRANSFORM_adjoint);
   QC_QTransform->addMethod("det",                         (q_method_t)QTRANSFORM_det);
   QC_QTransform->addMethod("determinant",                 (q_method_t)QTRANSFORM_determinant);
   QC_QTransform->addMethod("dx",                          (q_method_t)QTRANSFORM_dx);
   QC_QTransform->addMethod("dy",                          (q_method_t)QTRANSFORM_dy);
   QC_QTransform->addMethod("inverted",                    (q_method_t)QTRANSFORM_inverted);
   QC_QTransform->addMethod("isAffine",                    (q_method_t)QTRANSFORM_isAffine);
   QC_QTransform->addMethod("isIdentity",                  (q_method_t)QTRANSFORM_isIdentity);
   QC_QTransform->addMethod("isInvertible",                (q_method_t)QTRANSFORM_isInvertible);
   QC_QTransform->addMethod("isRotating",                  (q_method_t)QTRANSFORM_isRotating);
   QC_QTransform->addMethod("isScaling",                   (q_method_t)QTRANSFORM_isScaling);
   QC_QTransform->addMethod("isTranslating",               (q_method_t)QTRANSFORM_isTranslating);
   QC_QTransform->addMethod("map",                         (q_method_t)QTRANSFORM_map);
   QC_QTransform->addMethod("mapRect",                     (q_method_t)QTRANSFORM_mapRect);
   QC_QTransform->addMethod("mapToPolygon",                (q_method_t)QTRANSFORM_mapToPolygon);
   QC_QTransform->addMethod("reset",                       (q_method_t)QTRANSFORM_reset);
   QC_QTransform->addMethod("rotate",                      (q_method_t)QTRANSFORM_rotate);
   QC_QTransform->addMethod("rotateRadians",               (q_method_t)QTRANSFORM_rotateRadians);
   QC_QTransform->addMethod("scale",                       (q_method_t)QTRANSFORM_scale);
   QC_QTransform->addMethod("setMatrix",                   (q_method_t)QTRANSFORM_setMatrix);
   QC_QTransform->addMethod("shear",                       (q_method_t)QTRANSFORM_shear);
   QC_QTransform->addMethod("toAffine",                    (q_method_t)QTRANSFORM_toAffine);
   QC_QTransform->addMethod("translate",                   (q_method_t)QTRANSFORM_translate);
   QC_QTransform->addMethod("transposed",                  (q_method_t)QTRANSFORM_transposed);
   QC_QTransform->addMethod("type",                        (q_method_t)QTRANSFORM_type);

   return QC_QTransform;
}

QoreNamespace *initQTransformNS()
{
   QoreNamespace *ns = new QoreNamespace("QTransform");
   ns->addSystemClass(initQTransformClass());

   // TransformationType enum
   ns->addConstant("TxNone",                   new QoreBigIntNode(QTransform::TxNone));
   ns->addConstant("TxTranslate",              new QoreBigIntNode(QTransform::TxTranslate));
   ns->addConstant("TxScale",                  new QoreBigIntNode(QTransform::TxScale));
   ns->addConstant("TxRotate",                 new QoreBigIntNode(QTransform::TxRotate));
   ns->addConstant("TxShear",                  new QoreBigIntNode(QTransform::TxShear));
   ns->addConstant("TxProject",                new QoreBigIntNode(QTransform::TxProject));

   return ns;
}
