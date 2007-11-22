/*
 QC_QPolygon.cc
 
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

#include "QC_QPolygon.h"
#include "QC_QPoint.h"

int CID_QPOLYGON;
class QoreClass *QC_QPolygon = 0;

// returns 0 for OK, -1 for error
static int qpolygon_add_points(QoreQPolygon *qp, QoreList *l, class ExceptionSink *xsink)
{
   ListIterator li(l);
   while (li.next())
   {
      QoreNode *n = li.getValue();
      QoreQPoint *point = (n && n->type == NT_OBJECT) ?  (QoreQPoint *)n->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
      if (!point) {
	 if (!xsink->isException())
	    xsink->raiseException("QPOINT-LIST-TYPE-ERROR", "expecting only objects derived from QPoint, found other type ('%s')", n ? n->type->getName() : "NOTHING");
	 return -1;
      }
      ReferenceHolder<QoreQPoint> pointHolder(point, xsink);
      qp->push_back(*(static_cast<QPoint *>(point)));
   }
   return 0;
}

////QPolygon ()
////QPolygon ( int size )
////QPolygon ( const QPolygon & polygon )
////QPolygon ( const QVector<QPoint> & points )
////QPolygon ( const QRect & rectangle, bool closed = false )
static void QPOLYGON_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QPOLYGON, new QoreQPolygon());
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQPolygon *polygon = (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink);
      if (!polygon) {
         QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
         if (!rectangle) {
            if (!xsink->isException())
               xsink->raiseException("QPOLYGON-QPOLYGON-PARAM-ERROR", "QPolygon::QPolygon() does not know how to handle argumentsf class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return;
         }
         ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
         p = get_param(params, 1);
         bool closed = p ? p->getAsBool() : false;
         self->setPrivate(CID_QPOLYGON, new QoreQPolygon(*(static_cast<QRect *>(rectangle)), closed));
         return;
      }
      ReferenceHolder<QoreQPolygon> polygonHolder(polygon, xsink);
      self->setPrivate(CID_QPOLYGON, new QoreQPolygon(*(static_cast<QPolygon *>(polygon))));
      return;
   }
   if (p && p->type == NT_LIST) {
      QoreQPolygon *polygon = new QoreQPolygon();
      ReferenceHolder<QoreQPolygon> polygonHolder(polygon, xsink);
      if (qpolygon_add_points(polygon, p->val.list, xsink))
	 return;

      polygonHolder.release();
      self->setPrivate(CID_QPOLYGON, polygon);
      return;
   }
   int size = p ? p->getAsInt() : 0;
   self->setPrivate(CID_QPOLYGON, new QoreQPolygon(size));
   return;
}

static void QPOLYGON_copy(class Object *self, class Object *old, class QoreQPolygon *qp, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPOLYGON, new QoreQPolygon(*qp));
   //xsink->raiseException("QPOLYGON-COPY-ERROR", "objects of this class cannot be copied");
}

//QRect boundingRect () const
static QoreNode *QPOLYGON_boundingRect(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qp->boundingRect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//bool containsPoint ( const QPoint & pt, Qt::FillRule fillRule ) const
static QoreNode *QPOLYGON_containsPoint(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pt = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pt) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGON-CONTAINSPOINT-PARAM-ERROR", "expecting a QPoint object as first argument to QPolygon::containsPoint()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(pt, xsink);
   p = get_param(params, 1);
   Qt::FillRule fillRule = (Qt::FillRule)(p ? p->getAsInt() : 0);
   return new QoreNode(qp->containsPoint(*(static_cast<QPoint *>(pt)), fillRule));
}

//QPolygon intersected ( const QPolygon & r ) const
static QoreNode *QPOLYGON_intersected(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPolygon *r = (p && p->type == NT_OBJECT) ? (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGON-INTERSECTED-PARAM-ERROR", "expecting a QPolygon object as first argument to QPolygon::intersected()");
      return 0;
   }
   ReferenceHolder<QoreQPolygon> holder(r, xsink);
   Object *o_qp = new Object(self->getClass(CID_QPOLYGON), getProgram());
   QoreQPolygon *q_qp = new QoreQPolygon(qp->intersected(*(static_cast<QPolygon *>(r))));
   o_qp->setPrivate(CID_QPOLYGON, q_qp);
   return new QoreNode(o_qp);
}

////void point ( int index, int * x, int * y ) const
//QPoint point ( int index ) const
static QoreNode *QPOLYGON_point(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qp->point(index));
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//void putPoints ( int index, int nPoints, int firstx, int firsty, ... )
//void putPoints ( int index, int nPoints, const QPolygon & fromPolygon, int fromIndex = 0 )
static QoreNode *QPOLYGON_putPoints(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int nPoints = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   if (!p || p->type == NT_OBJECT) {
      xsink->raiseException("QPOLYGON-PUTPOINTS-PARAM-ERROR", "QPolygon::putPoints() expects a QPolygon object as the third argument");
      return 0;
   }
   QoreQPolygon *fromPolygon = (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink);
   if (!fromPolygon) {
      if (!xsink->isException())
	 xsink->raiseException("QPOLYGON-PUTPOINTS-PARAM-ERROR", "QPolygon::putPoints() does not know how to handle argumentsf class '%s' as passed as the third argument", p->val.object->getClass()->getName());
      return 0;
   }
   
   ReferenceHolder<QoreQPolygon> fromPolygonHolder(fromPolygon, xsink);
   p = get_param(params, 3);
   int fromIndex = p ? p->getAsInt() : 0;
   qp->putPoints(index, nPoints, *(static_cast<QPolygon *>(fromPolygon)), fromIndex);
   return 0;
}

//void setPoint ( int index, int x, int y )
//void setPoint ( int index, const QPoint & point )
static QoreNode *QPOLYGON_setPoint(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
	 if (!xsink->isException())
	    xsink->raiseException("QPOLYGON-SETPOINT-PARAM-ERROR", "QPolygon::setPoint() does not know how to handle argumentsf class '%s' as passed as the second argument", p->val.object->getClass()->getName());
	 return 0;
      }

      ReferenceHolder<QoreQPoint> pointHolder(point, xsink);
      qp->setPoint(index, *(static_cast<QPoint *>(point)));
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = p ? p->getAsInt() : 0;
   qp->setPoint(index, x, y);
   return 0;
}

////void setPoints ( int nPoints, const int * points )
////void setPoints ( int nPoints, int firstx, int firsty, ... )
// qore alternative: setPoints(list of QPoints)
static QoreNode *QPOLYGON_setPoints(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   if (p && p->type == NT_LIST)
      qpolygon_add_points(qp, p->val.list, xsink);
   else
      xsink->raiseException("QPOLYGON-SETPOINTS", "expecting list of QPoint objects for QPolygon::setPoints()");

//   int nPoints = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   if (p && p->type == NT_???) {
//      ??? int* points = p;
//      qp->setPoints(nPoints, points);
//      return 0;
//   }
//   int firstx = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   int firsty = p ? p->getAsInt() : 0;
//   p = get_param(params, 3);
//   QPolygon::... ... = (QPolygon::...)(p ? p->getAsInt() : 0);
//   qp->setPoints(nPoints, firstx, firsty, ...);

   return 0;
}

//QPolygon subtracted ( const QPolygon & r ) const
static QoreNode *QPOLYGON_subtracted(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPolygon *r = (p && p->type == NT_OBJECT) ? (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGON-SUBTRACTED-PARAM-ERROR", "expecting a QPolygon object as first argument to QPolygon::subtracted()");
      return 0;
   }
   ReferenceHolder<QoreQPolygon> holder(r, xsink);
   Object *o_qp = new Object(self->getClass(CID_QPOLYGON), getProgram());
   QoreQPolygon *q_qp = new QoreQPolygon(qp->subtracted(*(static_cast<QPolygon *>(r))));
   o_qp->setPrivate(CID_QPOLYGON, q_qp);
   return new QoreNode(o_qp);
}

//void translate ( int dx, int dy )
//void translate ( const QPoint & offset )
static QoreNode *QPOLYGON_translate(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *offset = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!offset) {
         if (!xsink->isException())
            xsink->raiseException("QPOLYGON-TRANSLATE-PARAM-ERROR", "QPolygon::translate() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQPoint> offsetHolder(offset, xsink);
      qp->translate(*(static_cast<QPoint *>(offset)));
      return 0;
   }
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;
   qp->translate(dx, dy);
   return 0;
}

//QPolygon united ( const QPolygon & r ) const
static QoreNode *QPOLYGON_united(Object *self, QoreQPolygon *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPolygon *r = (p && p->type == NT_OBJECT) ? (QoreQPolygon *)p->val.object->getReferencedPrivateData(CID_QPOLYGON, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPOLYGON-UNITED-PARAM-ERROR", "expecting a QPolygon object as first argument to QPolygon::united()");
      return 0;
   }
   ReferenceHolder<QoreQPolygon> holder(r, xsink);
   Object *o_qp = new Object(self->getClass(CID_QPOLYGON), getProgram());
   QoreQPolygon *q_qp = new QoreQPolygon(qp->united(*(static_cast<QPolygon *>(r))));
   o_qp->setPrivate(CID_QPOLYGON, q_qp);
   return new QoreNode(o_qp);
}

QoreClass *initQPolygonClass()
{
   QC_QPolygon = new QoreClass("QPolygon", QDOM_GUI);
   CID_QPOLYGON = QC_QPolygon->getID();

   QC_QPolygon->setConstructor(QPOLYGON_constructor);
   QC_QPolygon->setCopy((q_copy_t)QPOLYGON_copy);

   QC_QPolygon->addMethod("boundingRect",                (q_method_t)QPOLYGON_boundingRect);
   QC_QPolygon->addMethod("containsPoint",               (q_method_t)QPOLYGON_containsPoint);
   QC_QPolygon->addMethod("intersected",                 (q_method_t)QPOLYGON_intersected);
   QC_QPolygon->addMethod("point",                       (q_method_t)QPOLYGON_point);
   QC_QPolygon->addMethod("putPoints",                   (q_method_t)QPOLYGON_putPoints);
   QC_QPolygon->addMethod("setPoint",                    (q_method_t)QPOLYGON_setPoint);
   QC_QPolygon->addMethod("setPoints",                   (q_method_t)QPOLYGON_setPoints);
   QC_QPolygon->addMethod("subtracted",                  (q_method_t)QPOLYGON_subtracted);
   QC_QPolygon->addMethod("translate",                   (q_method_t)QPOLYGON_translate);
   QC_QPolygon->addMethod("united",                      (q_method_t)QPOLYGON_united);

   return QC_QPolygon;
}
