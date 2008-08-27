/*
 QC_QRegion.cc
 
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

#include "QC_QRegion.h"
#include "QC_QColor.h"
#include "QC_QBitmap.h"
#include "QC_QRect.h"
#include "QC_QPolygon.h"

qore_classid_t CID_QREGION;
QoreClass *QC_QRegion = 0;

//QRegion ()
//QRegion ( int x, int y, int w, int h, RegionType t = Rectangle )
//QRegion ( const QPolygon & a, Qt::FillRule fillRule = Qt::OddEvenFill )
//QRegion ( const QRegion & r )
//QRegion ( const QBitmap & bm )
//QRegion ( const QRect & r, RegionType t = Rectangle )
static void QREGION_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QREGION, new QoreQRegion());
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQBitmap *bm = (QoreQBitmap *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QBITMAP, xsink);
      if (!bm) {
         QoreQRect *r = (QoreQRect *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink);
         if (!r) {
            QoreQPolygon *a = (QoreQPolygon *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOLYGON, xsink);
            if (!a) {
               if (!xsink->isException())
                  xsink->raiseException("QREGION-CONSTRUCTOR-PARAM-ERROR", "QRegion::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
               return;
            }
            ReferenceHolder<AbstractPrivateData> aHolder(static_cast<AbstractPrivateData *>(a), xsink);
            p = get_param(params, 1);
	    Qt::FillRule fillRule = !is_nothing(p) ? (Qt::FillRule)p->getAsInt() : Qt::OddEvenFill;
            self->setPrivate(CID_QREGION, new QoreQRegion(*(static_cast<QPolygon *>(a)), fillRule));
            return;
         }
         ReferenceHolder<AbstractPrivateData> rHolder(static_cast<AbstractPrivateData *>(r), xsink);
         p = get_param(params, 1);
	 QRegion::RegionType t = !is_nothing(p) ? (QRegion::RegionType)p->getAsInt() : QRegion::Rectangle;
         self->setPrivate(CID_QREGION, new QoreQRegion(*(static_cast<QRect *>(r)), t));
         return;
      }
      ReferenceHolder<AbstractPrivateData> bmHolder(static_cast<AbstractPrivateData *>(bm), xsink);
      self->setPrivate(CID_QREGION, new QoreQRegion(*(static_cast<QBitmap *>(bm))));
      return;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int h = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   QRegion::RegionType t = !is_nothing(p) ? (QRegion::RegionType)p->getAsInt() : QRegion::Rectangle;
   self->setPrivate(CID_QREGION, new QoreQRegion(x, y, w, h, t));
   return;
}

static void QREGION_copy(class QoreObject *self, class QoreObject *old, class QoreQRegion *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QREGION-COPY-ERROR", "objects of this class cannot be copied");
}

//QRect boundingRect () const
static AbstractQoreNode *QREGION_boundingRect(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qr->boundingRect());
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//bool contains ( const QPoint & p ) const
//static AbstractQoreNode *QREGION_contains(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QPoint p = p;
//   return get_bool_node(qr->contains(p));
//}

//bool contains ( const QRect & r ) const
static AbstractQoreNode *QREGION_contains(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQRect *r = p ? (QoreQRect *)p->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-CONTAINS-PARAM-ERROR", "expecting a QRect object as first argument to QRegion::contains()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(r, xsink);
   return get_bool_node(qr->contains(*((QRect *)r)));
}

//Handle handle () const
//static AbstractQoreNode *QREGION_handle(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qr->handle());
//}

//QRegion intersected ( const QRegion & r ) const
static AbstractQoreNode *QREGION_intersected(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQRegion *r = p ? (QoreQRegion *)p->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-INTERSECTED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::intersected()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->intersected(*((QRegion *)r)));
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return o_qr;
}

//bool intersects ( const QRegion & region ) const
static AbstractQoreNode *QREGION_intersects(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQRegion *region = p ? (QoreQRegion *)p->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!region)
   {
      QoreQRect *rect = p ? (QoreQRect *)p->getReferencedPrivateData(CID_QRECT, xsink) : 0;
      if (!rect)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QREGION-INTERSECTS-PARAM-ERROR", "expecting a QRect or QRegion object as first argument to QRegion::intersects()");
	 return 0;
      }
      ReferenceHolder<QoreQRect> holder(rect, xsink);
      return get_bool_node(qr->intersects(*((QRect *)rect)));
   }
   ReferenceHolder<QoreQRegion> holder(region, xsink);
   return get_bool_node(qr->intersects(*((QRegion *)region)));
}

//bool isEmpty () const
static AbstractQoreNode *QREGION_isEmpty(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qr->isEmpty());
}

//QVector<QRect> rects () const
//static AbstractQoreNode *QREGION_rects(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qr->rects());
//}

//void setRects ( const QRect * rects, int number )
static AbstractQoreNode *QREGION_setRects(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQRect *rects = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rects)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-SETRECTS-PARAM-ERROR", "expecting a QRect object as first argument to QRegion::setRects()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(rects, xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   int number = p ? p->getAsInt() : 0;
   qr->setRects((QRect *)rects, number);
   return 0;
}

//QRegion subtracted ( const QRegion & r ) const
static AbstractQoreNode *QREGION_subtracted(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQRegion *r = p ? (QoreQRegion *)p->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-SUBTRACTED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::subtracted()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->subtracted(*((QRegion *)r)));
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return o_qr;
}

//void translate ( int dx, int dy )
static AbstractQoreNode *QREGION_translate(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;
   qr->translate(dx, dy);
   return 0;
}

//void translate ( const QPoint & point )
//static AbstractQoreNode *QREGION_translate(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QPoint point = p;
//   qr->translate(point);
//   return 0;
//}

//QRegion translated ( int dx, int dy ) const
static AbstractQoreNode *QREGION_translated(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;

   QoreQRegion *q_qr = new QoreQRegion(qr->translated(dx, dy));
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return o_qr;
}

//QRegion translated ( const QPoint & p ) const
//static AbstractQoreNode *QREGION_translated(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QPoint p = p;
//
//   QoreQRegion *q_qr = new QoreQRegion(qr->translated(p));
//   QoreObject *o_qr = new QoreObject(self->getClass(CID_QREGION), getProgram());
//   o_qr->setPrivate(CID_QREGION, q_qr);
//   return o_qr;
//}

//QRegion united ( const QRegion & r ) const
static AbstractQoreNode *QREGION_united(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQRegion *r = p ? (QoreQRegion *)p->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-UNITED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::united()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->united(*((QRegion *)r)));
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return o_qr;
}

//QRegion xored ( const QRegion & r ) const
static AbstractQoreNode *QREGION_xored(QoreObject *self, QoreQRegion *qr, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQRegion *r = p ? (QoreQRegion *)p->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-XORED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::xored()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->xored(*((QRegion *)r)));
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return o_qr;
}

class QoreClass *initQRegionClass()
{
   QORE_TRACE("initQRegionClass()");
   
   QC_QRegion = new QoreClass("QRegion", QDOM_GUI);
   CID_QREGION = QC_QRegion->getID();
   QC_QRegion->setConstructor(QREGION_constructor);
   QC_QRegion->setCopy((q_copy_t)QREGION_copy);

   QC_QRegion->addMethod("boundingRect",                (q_method_t)QREGION_boundingRect);
   //QC_QRegion->addMethod("contains",                    (q_method_t)QREGION_contains);
   QC_QRegion->addMethod("contains",                    (q_method_t)QREGION_contains);
   //QC_QRegion->addMethod("handle",                      (q_method_t)QREGION_handle);
   QC_QRegion->addMethod("intersected",                 (q_method_t)QREGION_intersected);
   QC_QRegion->addMethod("intersects",                  (q_method_t)QREGION_intersects);
   QC_QRegion->addMethod("isEmpty",                     (q_method_t)QREGION_isEmpty);
   //QC_QRegion->addMethod("rects",                       (q_method_t)QREGION_rects);
   QC_QRegion->addMethod("setRects",                    (q_method_t)QREGION_setRects);
   QC_QRegion->addMethod("subtracted",                  (q_method_t)QREGION_subtracted);
   QC_QRegion->addMethod("translate",                   (q_method_t)QREGION_translate);
   //QC_QRegion->addMethod("translate",                   (q_method_t)QREGION_translate);
   QC_QRegion->addMethod("translated",                  (q_method_t)QREGION_translated);
   //QC_QRegion->addMethod("translated",                  (q_method_t)QREGION_translated);
   QC_QRegion->addMethod("united",                      (q_method_t)QREGION_united);
   QC_QRegion->addMethod("xored",                       (q_method_t)QREGION_xored);


   return QC_QRegion;
}
