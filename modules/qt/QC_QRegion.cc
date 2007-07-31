/*
 QC_QRegion.cc
 
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

#include "QC_QRegion.h"
#include "QC_QColor.h"

DLLLOCAL int CID_QREGION;

static void QREGION_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQRegion *qr;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qr = new QoreQRegion();
   else if (p->type == NT_OBJECT) {
      QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rectangle)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QREGION-CONSTRUCTOR-ERROR", "QRegion::constructor() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	 return;
      }
      ReferenceHolder<QoreQRect> holder(rectangle, xsink);
      p = get_param(params, 1);
      QRegion::RegionType t = !is_nothing(p) ? (QRegion::RegionType)p->getAsInt() : QRegion::Rectangle;

      qr = new QoreQRegion(*rectangle, t);
   }
   else {
      int x = p->getAsInt();
      p = get_param(params, 1);
      int y = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int w = p ? p->getAsInt() : 0;
      p = get_param(params, 3);
      int h = p ? p->getAsInt() : 0;
      p = get_param(params, 4);
      QRegion::RegionType t = !is_nothing(p) ? (QRegion::RegionType)p->getAsInt() : QRegion::Rectangle;

      qr = new QoreQRegion(x, y, w, h, t);
   }

   self->setPrivate(CID_QREGION, qr);
}

static void QREGION_copy(class Object *self, class Object *old, class QoreQRegion *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QREGION-COPY-ERROR", "objects of this class cannot be copied");
}

//QRect boundingRect () const
static QoreNode *QREGION_boundingRect(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qr->boundingRect());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//bool contains ( const QPoint & p ) const
//static QoreNode *QREGION_contains(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPoint p = p;
//   return new QoreNode(qr->contains(p));
//}

//bool contains ( const QRect & r ) const
static QoreNode *QREGION_contains(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *r = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-CONTAINS-PARAM-ERROR", "expecting a QRect object as first argument to QRegion::contains()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(r, xsink);
   return new QoreNode(qr->contains(*((QRect *)r)));
}

//Handle handle () const
//static QoreNode *QREGION_handle(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qr->handle());
//}

//QRegion intersected ( const QRegion & r ) const
static QoreNode *QREGION_intersected(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegion *r = (p && p->type == NT_OBJECT) ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-INTERSECTED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::intersected()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->intersected(*((QRegion *)r)));
   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//bool intersects ( const QRegion & region ) const
static QoreNode *QREGION_intersects(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegion *region = (p && p->type == NT_OBJECT) ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!region)
   {
      QoreQRect *rect = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
      if (!rect)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QREGION-INTERSECTS-PARAM-ERROR", "expecting a QRect or QRegion object as first argument to QRegion::intersects()");
	 return 0;
      }
      ReferenceHolder<QoreQRect> holder(rect, xsink);
      return new QoreNode(qr->intersects(*((QRect *)rect)));
   }
   ReferenceHolder<QoreQRegion> holder(region, xsink);
   return new QoreNode(qr->intersects(*((QRegion *)region)));
}

//bool isEmpty () const
static QoreNode *QREGION_isEmpty(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qr->isEmpty());
}

//QVector<QRect> rects () const
//static QoreNode *QREGION_rects(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qr->rects());
//}

//void setRects ( const QRect * rects, int number )
static QoreNode *QREGION_setRects(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *rects = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!p || !rects)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-SETRECTS-PARAM-ERROR", "expecting a QRect object as first argument to QRegion::setRects()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(rects, xsink);
   p = get_param(params, 1);
   int number = p ? p->getAsInt() : 0;
   qr->setRects((QRect *)rects, number);
   return 0;
}

//QRegion subtracted ( const QRegion & r ) const
static QoreNode *QREGION_subtracted(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegion *r = (p && p->type == NT_OBJECT) ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-SUBTRACTED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::subtracted()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->subtracted(*((QRegion *)r)));
   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//void translate ( int dx, int dy )
static QoreNode *QREGION_translate(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;
   qr->translate(dx, dy);
   return 0;
}

//void translate ( const QPoint & point )
//static QoreNode *QREGION_translate(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPoint point = p;
//   qr->translate(point);
//   return 0;
//}

//QRegion translated ( int dx, int dy ) const
static QoreNode *QREGION_translated(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;

   QoreQRegion *q_qr = new QoreQRegion(qr->translated(dx, dy));
   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//QRegion translated ( const QPoint & p ) const
//static QoreNode *QREGION_translated(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPoint p = p;
//
//   QoreQRegion *q_qr = new QoreQRegion(qr->translated(p));
//   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
//   o_qr->setPrivate(CID_QREGION, q_qr);
//   return new QoreNode(o_qr);
//}

//QRegion united ( const QRegion & r ) const
static QoreNode *QREGION_united(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegion *r = (p && p->type == NT_OBJECT) ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-UNITED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::united()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->united(*((QRegion *)r)));
   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//QRegion xored ( const QRegion & r ) const
static QoreNode *QREGION_xored(Object *self, QoreQRegion *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegion *r = (p && p->type == NT_OBJECT) ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!p || !r)
   {
      if (!xsink->isException())
         xsink->raiseException("QREGION-XORED-PARAM-ERROR", "expecting a QRegion object as first argument to QRegion::xored()");
      return 0;
   }
   ReferenceHolder<QoreQRegion> holder(r, xsink);

   QoreQRegion *q_qr = new QoreQRegion(qr->xored(*((QRegion *)r)));
   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

class QoreClass *initQRegionClass()
{
   tracein("initQRegionClass()");
   
   class QoreClass *QC_QRegion = new QoreClass("QRegion", QDOM_GUI);
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

   traceout("initQRegionClass()");
   return QC_QRegion;
}
