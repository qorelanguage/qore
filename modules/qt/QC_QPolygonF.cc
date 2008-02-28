/*
 QC_QPolygonF.cc
 
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

#include "QC_QPolygonF.h"
#include "QC_QPointF.h"
#include "QC_QRectF.h"
#include "QC_QPolygon.h"

#include "qore-qt.h"

qore_classid_t CID_QPOLYGONF;
class QoreClass *QC_QPolygonF = 0;

// returns 0 for OK, -1 for error
static int qpolygonf_add_points(QoreQPolygonF *qp, const QoreListNode *l, class ExceptionSink *xsink)
{
   ConstListIterator li(l);
   while (li.next())
   {
      const AbstractQoreNode *n = li.getValue();
      QoreQPointF *point = (n && n->getType() == NT_OBJECT) ?  (QoreQPointF *)(reinterpret_cast<const QoreObject *>(n))->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
      if (!point) {
	 if (!xsink->isException())
	    xsink->raiseException("QPOINT-LIST-TYPE-ERROR", "expecting only objects derived from QPointF, found other type ('%s')", n ? n->getTypeName() : "NOTHING");
	 return -1;
      }
      ReferenceHolder<QoreQPointF> pointHolder(point, xsink);
      qp->push_back(*(static_cast<QPointF *>(point)));
   }
   return 0;
}

//QPolygonF ()
//QPolygonF ( int size )
//QPolygonF ( const QPolygonF & polygon )
////QPolygonF ( const QVector<QPointF> & points )
//QPolygonF ( const QRectF & rectangle )
//QPolygonF ( const QPolygon & polygon )
static void QPOLYGONF_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QPOLYGONF, new QoreQPolygonF());
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQPolygonF *polygonf = (QoreQPolygonF *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOLYGONF, xsink);
      if (!polygonf) {
         QoreQRectF *rectangle = (QoreQRectF *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECTF, xsink);
         if (!rectangle) {
            QoreQPolygon *polygon = (QoreQPolygon *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOLYGON, xsink);
            if (!polygon) {
               if (!xsink->isException())
                  xsink->raiseException("QPOLYGONF-QPOLYGONF-PARAM-ERROR", "QPolygonF::QPolygonF() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
               return;
            }
            ReferenceHolder<QoreQPolygon> polygonHolder(polygon, xsink);
            self->setPrivate(CID_QPOLYGONF, new QoreQPolygonF(*(static_cast<QPolygon *>(polygon))));
            return;
         }
         ReferenceHolder<QoreQRectF> rectangleHolder(rectangle, xsink);
         self->setPrivate(CID_QPOLYGONF, new QoreQPolygonF(*(static_cast<QRectF *>(rectangle))));
         return;
      }
      ReferenceHolder<QoreQPolygonF> polygonHolder(polygonf, xsink);
      self->setPrivate(CID_QPOLYGONF, new QoreQPolygonF(*(static_cast<QPolygonF *>(polygonf))));
      return;
   }
   {
      const QoreListNode *l = dynamic_cast<const QoreListNode *>(p);
      if (l) {
	 QoreQPolygonF *polygon = new QoreQPolygonF();
	 ReferenceHolder<QoreQPolygonF> polygonHolder(polygon, xsink);
	 if (qpolygonf_add_points(polygon, l, xsink))
	    return;
	 
	 polygonHolder.release();
	 self->setPrivate(CID_QPOLYGONF, polygon);
	 return;
      }
   }
   int size = p ? p->getAsInt() : 0;
   self->setPrivate(CID_QPOLYGONF, new QoreQPolygonF(size));
   return;
}

static void QPOLYGONF_copy(class QoreObject *self, class QoreObject *old, class QoreQPolygonF *qpf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPOLYGONF, new QoreQPolygonF(*qpf));
   //xsink->raiseException("QPOLYGONF-COPY-ERROR", "objects of this class cannot be copied");
}

//QRectF boundingRect () const
static AbstractQoreNode *QPOLYGONF_boundingRect(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
   QoreQRectF *q_qrf = new QoreQRectF(qpf->boundingRect());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return o_qrf;
}

//bool containsPoint ( const QPointF & point, Qt::FillRule fillRule ) const
static AbstractQoreNode *QPOLYGONF_containsPoint(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQPointF *point = o ? (QoreQPointF *)o->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGONF-CONTAINSPOINT-PARAM-ERROR", "expecting a QPointF object as first argument to QPolygonF::containsPoint()");
      return 0;
   }
   ReferenceHolder<QoreQPointF> holder(point, xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   Qt::FillRule fillRule = (Qt::FillRule)(p ? p->getAsInt() : 0);
   return get_bool_node(qpf->containsPoint(*(static_cast<QPointF *>(point)), fillRule));
}

//QPolygonF intersected ( const QPolygonF & r ) const
static AbstractQoreNode *QPOLYGONF_intersected(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPolygonF *r = p ? (QoreQPolygonF *)p->getReferencedPrivateData(CID_QPOLYGONF, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGONF-INTERSECTED-PARAM-ERROR", "expecting a QPolygonF object as first argument to QPolygonF::intersected()");
      return 0;
   }
   ReferenceHolder<QoreQPolygonF> holder(r, xsink);
   QoreObject *o_qpf = new QoreObject(self->getClass(CID_QPOLYGONF), getProgram());
   QoreQPolygonF *q_qpf = new QoreQPolygonF(qpf->intersected(*(static_cast<QPolygonF *>(r))));
   o_qpf->setPrivate(CID_QPOLYGONF, q_qpf);
   return o_qpf;
}

//bool isClosed () const
static AbstractQoreNode *QPOLYGONF_isClosed(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qpf->isClosed());
}

//QPolygonF subtracted ( const QPolygonF & r ) const
static AbstractQoreNode *QPOLYGONF_subtracted(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPolygonF *r = p ? (QoreQPolygonF *)p->getReferencedPrivateData(CID_QPOLYGONF, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGONF-SUBTRACTED-PARAM-ERROR", "expecting a QPolygonF object as first argument to QPolygonF::subtracted()");
      return 0;
   }
   ReferenceHolder<QoreQPolygonF> holder(r, xsink);
   QoreObject *o_qpf = new QoreObject(self->getClass(CID_QPOLYGONF), getProgram());
   QoreQPolygonF *q_qpf = new QoreQPolygonF(qpf->subtracted(*(static_cast<QPolygonF *>(r))));
   o_qpf->setPrivate(CID_QPOLYGONF, q_qpf);
   return o_qpf;
}

//QPolygon toPolygon () const
static AbstractQoreNode *QPOLYGONF_toPolygon(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPolygon, getProgram());
   QoreQPolygon *q_qp = new QoreQPolygon(qpf->toPolygon());
   o_qp->setPrivate(CID_QPOLYGON, q_qp);
   return o_qp;
}

//void translate ( const QPointF & offset )
//void translate ( qreal dx, qreal dy )
static AbstractQoreNode *QPOLYGONF_translate(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPointF *offset = (QoreQPointF *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!offset) {
         if (!xsink->isException())
            xsink->raiseException("QPOLYGONF-TRANSLATE-PARAM-ERROR", "QPolygonF::translate() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<QoreQPointF> offsetHolder(offset, xsink);
      qpf->translate(*(static_cast<QPointF *>(offset)));
      return 0;
   }
   qreal dx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal dy = p ? p->getAsFloat() : 0.0;
   qpf->translate(dx, dy);
   return 0;
}

//QPolygonF united ( const QPolygonF & r ) const
static AbstractQoreNode *QPOLYGONF_united(QoreObject *self, QoreQPolygonF *qpf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPolygonF *r = p ? (QoreQPolygonF *)p->getReferencedPrivateData(CID_QPOLYGONF, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGONF-UNITED-PARAM-ERROR", "expecting a QPolygonF object as first argument to QPolygonF::united()");
      return 0;
   }
   ReferenceHolder<QoreQPolygonF> holder(r, xsink);
   QoreObject *o_qpf = new QoreObject(self->getClass(CID_QPOLYGONF), getProgram());
   QoreQPolygonF *q_qpf = new QoreQPolygonF(qpf->united(*(static_cast<QPolygonF *>(r))));
   o_qpf->setPrivate(CID_QPOLYGONF, q_qpf);
   return o_qpf;
}

QoreClass *initQPolygonFClass()
{
   QC_QPolygonF = new QoreClass("QPolygonF", QDOM_GUI);
   CID_QPOLYGONF = QC_QPolygonF->getID();

   QC_QPolygonF->setConstructor(QPOLYGONF_constructor);
   QC_QPolygonF->setCopy((q_copy_t)QPOLYGONF_copy);

   QC_QPolygonF->addMethod("boundingRect",                (q_method_t)QPOLYGONF_boundingRect);
   QC_QPolygonF->addMethod("containsPoint",               (q_method_t)QPOLYGONF_containsPoint);
   QC_QPolygonF->addMethod("intersected",                 (q_method_t)QPOLYGONF_intersected);
   QC_QPolygonF->addMethod("isClosed",                    (q_method_t)QPOLYGONF_isClosed);
   QC_QPolygonF->addMethod("subtracted",                  (q_method_t)QPOLYGONF_subtracted);
   QC_QPolygonF->addMethod("toPolygon",                   (q_method_t)QPOLYGONF_toPolygon);
   QC_QPolygonF->addMethod("translate",                   (q_method_t)QPOLYGONF_translate);
   QC_QPolygonF->addMethod("united",                      (q_method_t)QPOLYGONF_united);

   return QC_QPolygonF;
}
