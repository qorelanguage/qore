/*
 QC_QRect.cc
 
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

#include "QC_QRect.h"
#include "QC_QColor.h"
#include "QC_QPoint.h"

int CID_QRECT;
class QoreClass *QC_QRect = 0;

static void QRECT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQRect *qr;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qr = new QoreQRect();
   else {
      int x = p->getAsInt();
      p = get_param(params, 1);
      int y = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int w = p ? p->getAsInt() : 0;
      p = get_param(params, 3);
      int h = p ? p->getAsInt() : 0;

      qr = new QoreQRect(x, y, w, h);
   }

   self->setPrivate(CID_QRECT, qr);
}

static void QRECT_copy(class Object *self, class Object *old, class QoreQRect *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QRECT-COPY-ERROR", "objects of this class cannot be copied");
}

//void adjust ( int dx1, int dy1, int dx2, int dy2 )
static QoreNode *QRECT_adjust(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int dx1 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int dx2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int dy2 = p ? p->getAsInt() : 0;
   qr->adjust(dx1, dy1, dx2, dy2);
   return 0;
}

//QRect adjusted ( int dx1, int dy1, int dx2, int dy2 ) const
static QoreNode *QRECT_adjusted(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int dx1 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int dx2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int dy2 = p ? p->getAsInt() : 0;

   QoreQRect *q_qr = new QoreQRect(qr->adjusted(dx1, dy1, dx2, dy2));
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//int bottom () const
static QoreNode *QRECT_bottom(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->bottom());
}

//QPoint bottomLeft () const
static QoreNode *QRECT_bottomLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{

   QoreQPoint *q_qp = new QoreQPoint(qr->bottomLeft());
   Object *o_qp = new Object(QC_QPoint, getProgram());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint bottomRight () const
static QoreNode *QRECT_bottomRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{

   QoreQPoint *q_qp = new QoreQPoint(qr->bottomRight());
   Object *o_qp = new Object(QC_QPoint, getProgram());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint center () const
static QoreNode *QRECT_center(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{

   QoreQPoint *q_qp = new QoreQPoint(qr->center());
   Object *o_qp = new Object(QC_QPoint, getProgram());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//bool contains ( const QPoint & point, bool proper = false ) const
//bool contains ( int x, int y, bool proper ) const
static QoreNode *QRECT_contains(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point)
      {
	 QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
	 if (!rectangle)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QRECT-CONTAINS-PARAM-ERROR", "QRect::contains() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }

	 ReferenceHolder<QoreQRect> holder(rectangle, xsink);
	 p = get_param(params, 1);
	 bool proper = p ? p->getAsBool() : 0;
	 return new QoreNode(qr->contains(*((QRect *)rectangle), proper));	 
      }
      ReferenceHolder<QoreQPoint> holder(point, xsink);
      p = get_param(params, 1);
      bool proper = p ? p->getAsBool() : 0;
      return new QoreNode(qr->contains(*((QPoint *)point), proper));
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   bool proper = p ? p->getAsBool() : 0;
   return new QoreNode(qr->contains(x, y, proper));
}

//void getCoords ( int * x1, int * y1, int * x2, int * y2 ) const
//static QoreNode *QRECT_getCoords(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? int* x1 = p;
//   p = get_param(params, 1);
//   ??? int* y1 = p;
//   p = get_param(params, 2);
//   ??? int* x2 = p;
//   p = get_param(params, 3);
//   ??? int* y2 = p;
//   qr->getCoords(x1, y1, x2, y2);
//   return 0;
//}

//void getRect ( int * x, int * y, int * width, int * height ) const
//static QoreNode *QRECT_getRect(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? int* x = p;
//   p = get_param(params, 1);
//   ??? int* y = p;
//   p = get_param(params, 2);
//   ??? int* width = p;
//   p = get_param(params, 3);
//   ??? int* height = p;
//   qr->getRect(x, y, width, height);
//   return 0;
//}

//int height () const
static QoreNode *QRECT_height(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->height());
}

//QRect intersected ( const QRect & rectangle ) const
static QoreNode *QRECT_intersected(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!p || !rectangle)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-INTERSECTED-PARAM-ERROR", "expecting a QRect object as first argument to QRect::intersected()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(rectangle, xsink);

   QoreQRect *q_qr = new QoreQRect(qr->intersected(*((QRect *)rectangle)));
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//bool intersects ( const QRect & rectangle ) const
static QoreNode *QRECT_intersects(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!p || !rectangle)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-INTERSECTS-PARAM-ERROR", "expecting a QRect object as first argument to QRect::intersects()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(rectangle, xsink);
   return new QoreNode(qr->intersects(*((QRect *)rectangle)));
}

//bool isEmpty () const
static QoreNode *QRECT_isEmpty(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qr->isEmpty());
}

//bool isNull () const
static QoreNode *QRECT_isNull(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qr->isNull());
}

//bool isValid () const
static QoreNode *QRECT_isValid(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qr->isValid());
}

//int left () const
static QoreNode *QRECT_left(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->left());
}

//void moveBottom ( int y )
static QoreNode *QRECT_moveBottom(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qr->moveBottom(y);
   return 0;
}

//void moveBottomLeft ( const QPoint & position )
static QoreNode *QRECT_moveBottomLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-MOVEBOTTOMLEFT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::moveBottomLeft()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->moveBottomLeft(*((QPoint *)position));
   return 0;
}

//void moveBottomRight ( const QPoint & position )
static QoreNode *QRECT_moveBottomRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-MOVEBOTTOMRIGHT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::moveBottomRight()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->moveBottomRight(*((QPoint *)position));
   return 0;
}

//void moveCenter ( const QPoint & position )
static QoreNode *QRECT_moveCenter(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-MOVECENTER-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::moveCenter()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->moveCenter(*((QPoint *)position));
   return 0;
}

//void moveLeft ( int x )
static QoreNode *QRECT_moveLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qr->moveLeft(x);
   return 0;
}

//void moveRight ( int x )
static QoreNode *QRECT_moveRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qr->moveRight(x);
   return 0;
}

//void moveTo ( const QPoint & position )
//void moveTo ( int x, int y )
static QoreNode *QRECT_moveTo(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *position = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!position)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QRECT-MOVETO-PARAM-ERROR", "QRect::moveTo() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<QoreQPoint> holder(position, xsink);
      qr->moveTo(*((QPoint *)position));
      return 0;
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   qr->moveTo(x, y);
   return 0;
}

//void moveTop ( int y )
static QoreNode *QRECT_moveTop(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qr->moveTop(y);
   return 0;
}

//void moveTopLeft ( const QPoint & position )
static QoreNode *QRECT_moveTopLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-MOVETOPLEFT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::moveTopLeft()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->moveTopLeft(*((QPoint *)position));
   return 0;
}

//void moveTopRight ( const QPoint & position )
static QoreNode *QRECT_moveTopRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-MOVETOPRIGHT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::moveTopRight()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->moveTopRight(*((QPoint *)position));
   return 0;
}

//QRect normalized () const
static QoreNode *QRECT_normalized(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qr->normalized());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//int right () const
static QoreNode *QRECT_right(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->right());
}

//void setBottom ( int y )
static QoreNode *QRECT_setBottom(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qr->setBottom(y);
   return 0;
}

//void setBottomLeft ( const QPoint & position )
static QoreNode *QRECT_setBottomLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-SETBOTTOMLEFT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::setBottomLeft()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->setBottomLeft(*((QPoint *)position));
   return 0;
}

//void setBottomRight ( const QPoint & position )
static QoreNode *QRECT_setBottomRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-SETBOTTOMRIGHT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::setBottomRight()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->setBottomRight(*((QPoint *)position));
   return 0;
}

//void setCoords ( int x1, int y1, int x2, int y2 )
static QoreNode *QRECT_setCoords(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x1 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int x2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int y2 = p ? p->getAsInt() : 0;
   qr->setCoords(x1, y1, x2, y2);
   return 0;
}

//void setHeight ( int height )
static QoreNode *QRECT_setHeight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int height = p ? p->getAsInt() : 0;
   qr->setHeight(height);
   return 0;
}

//void setLeft ( int x )
static QoreNode *QRECT_setLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qr->setLeft(x);
   return 0;
}

//void setRect ( int x, int y, int width, int height )
static QoreNode *QRECT_setRect(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   qr->setRect(x, y, width, height);
   return 0;
}

//void setRight ( int x )
static QoreNode *QRECT_setRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qr->setRight(x);
   return 0;
}

//void setSize ( const QSize & size )
//static QoreNode *QRECT_setSize(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QSize size = p;
//   qr->setSize(size);
//   return 0;
//}

//void setTop ( int y )
static QoreNode *QRECT_setTop(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qr->setTop(y);
   return 0;
}

//void setTopLeft ( const QPoint & position )
static QoreNode *QRECT_setTopLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-SETTOPLEFT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::setTopLeft()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->setTopLeft(*((QPoint *)position));
   return 0;
}

//void setTopRight ( const QPoint & position )
static QoreNode *QRECT_setTopRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!p || !position)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-SETTOPRIGHT-PARAM-ERROR", "expecting a QPoint object as first argument to QRect::setTopRight()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(position, xsink);
   qr->setTopRight(*((QPoint *)position));
   return 0;
}

//void setWidth ( int width )
static QoreNode *QRECT_setWidth(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   qr->setWidth(width);
   return 0;
}

//void setX ( int x )
static QoreNode *QRECT_setX(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qr->setX(x);
   return 0;
}

//void setY ( int y )
static QoreNode *QRECT_setY(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qr->setY(y);
   return 0;
}

//QSize size () const
//static QoreNode *QRECT_size(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qr->size());
//}

//int top () const
static QoreNode *QRECT_top(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->top());
}

//QPoint topLeft () const
static QoreNode *QRECT_topLeft(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{

   QoreQPoint *q_qp = new QoreQPoint(qr->topLeft());
   Object *o_qp = new Object(QC_QPoint, getProgram());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint topRight () const
static QoreNode *QRECT_topRight(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{

   QoreQPoint *q_qp = new QoreQPoint(qr->topRight());
   Object *o_qp = new Object(QC_QPoint, getProgram());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//void translate ( const QPoint & offset )
//void translate ( int dx, int dy )
static QoreNode *QRECT_translate(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *offset = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!offset)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QRECT-TRANSLATE-PARAM-ERROR", "QRect::translate() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<QoreQPoint> holder(offset, xsink);
      qr->translate(*((QPoint *)offset));
      return 0;
   }

   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;
   qr->translate(dx, dy);
   return 0;
}

//QRect translated ( int dx, int dy ) const
//QRect translated ( const QPoint & offset ) const
static QoreNode *QRECT_translated(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *offset = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!offset) {
         if (!xsink->isException())
            xsink->raiseException("QRECT-TRANSLATED-PARAM-ERROR", "QRect::translated() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQPoint> offsetHolder(offset, xsink);
      QoreQRect *q_qr = new QoreQRect(qr->translated(*((QPoint *)offset)));
      Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
      o_qr->setPrivate(CID_QRECT, q_qr);
      return new QoreNode(o_qr);
   }
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;
   QoreQRect *q_qr = new QoreQRect(qr->translated(dx, dy));
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//QRect united ( const QRect & rectangle ) const
static QoreNode *QRECT_united(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!p || !rectangle)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECT-UNITED-PARAM-ERROR", "expecting a QRect object as first argument to QRect::united()");
      return 0;
   }
   ReferenceHolder<QoreQRect> holder(rectangle, xsink);

   QoreQRect *q_qr = new QoreQRect(qr->united(*((QRect *)rectangle)));
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//int width () const
static QoreNode *QRECT_width(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->width());
}

//int x () const
static QoreNode *QRECT_x(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->x());
}

//int y () const
static QoreNode *QRECT_y(Object *self, QoreQRect *qr, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qr->y());
}

class QoreClass *initQRectClass()
{
   tracein("initQRectClass()");
   
   QC_QRect = new QoreClass("QRect", QDOM_GUI);
   CID_QRECT = QC_QRect->getID();
   QC_QRect->setConstructor(QRECT_constructor);
   QC_QRect->setCopy((q_copy_t)QRECT_copy);

   QC_QRect->addMethod("adjust",                      (q_method_t)QRECT_adjust);
   QC_QRect->addMethod("adjusted",                    (q_method_t)QRECT_adjusted);
   QC_QRect->addMethod("bottom",                      (q_method_t)QRECT_bottom);
   QC_QRect->addMethod("bottomLeft",                  (q_method_t)QRECT_bottomLeft);
   QC_QRect->addMethod("bottomRight",                 (q_method_t)QRECT_bottomRight);
   QC_QRect->addMethod("center",                      (q_method_t)QRECT_center);
   QC_QRect->addMethod("contains",                    (q_method_t)QRECT_contains);
   //QC_QRect->addMethod("getCoords",                   (q_method_t)QRECT_getCoords);
   //QC_QRect->addMethod("getRect",                     (q_method_t)QRECT_getRect);
   QC_QRect->addMethod("height",                      (q_method_t)QRECT_height);
   QC_QRect->addMethod("intersected",                 (q_method_t)QRECT_intersected);
   QC_QRect->addMethod("intersects",                  (q_method_t)QRECT_intersects);
   QC_QRect->addMethod("isEmpty",                     (q_method_t)QRECT_isEmpty);
   QC_QRect->addMethod("isNull",                      (q_method_t)QRECT_isNull);
   QC_QRect->addMethod("isValid",                     (q_method_t)QRECT_isValid);
   QC_QRect->addMethod("left",                        (q_method_t)QRECT_left);
   QC_QRect->addMethod("moveBottom",                  (q_method_t)QRECT_moveBottom);
   QC_QRect->addMethod("moveBottomLeft",              (q_method_t)QRECT_moveBottomLeft);
   QC_QRect->addMethod("moveBottomRight",             (q_method_t)QRECT_moveBottomRight);
   QC_QRect->addMethod("moveCenter",                  (q_method_t)QRECT_moveCenter);
   QC_QRect->addMethod("moveLeft",                    (q_method_t)QRECT_moveLeft);
   QC_QRect->addMethod("moveRight",                   (q_method_t)QRECT_moveRight);
   QC_QRect->addMethod("moveTo",                      (q_method_t)QRECT_moveTo);
   QC_QRect->addMethod("moveTop",                     (q_method_t)QRECT_moveTop);
   QC_QRect->addMethod("moveTopLeft",                 (q_method_t)QRECT_moveTopLeft);
   QC_QRect->addMethod("moveTopRight",                (q_method_t)QRECT_moveTopRight);
   QC_QRect->addMethod("normalized",                  (q_method_t)QRECT_normalized);
   QC_QRect->addMethod("right",                       (q_method_t)QRECT_right);
   QC_QRect->addMethod("setBottom",                   (q_method_t)QRECT_setBottom);
   QC_QRect->addMethod("setBottomLeft",               (q_method_t)QRECT_setBottomLeft);
   QC_QRect->addMethod("setBottomRight",              (q_method_t)QRECT_setBottomRight);
   QC_QRect->addMethod("setCoords",                   (q_method_t)QRECT_setCoords);
   QC_QRect->addMethod("setHeight",                   (q_method_t)QRECT_setHeight);
   QC_QRect->addMethod("setLeft",                     (q_method_t)QRECT_setLeft);
   QC_QRect->addMethod("setRect",                     (q_method_t)QRECT_setRect);
   QC_QRect->addMethod("setRight",                    (q_method_t)QRECT_setRight);
   //QC_QRect->addMethod("setSize",                     (q_method_t)QRECT_setSize);
   QC_QRect->addMethod("setTop",                      (q_method_t)QRECT_setTop);
   QC_QRect->addMethod("setTopLeft",                  (q_method_t)QRECT_setTopLeft);
   QC_QRect->addMethod("setTopRight",                 (q_method_t)QRECT_setTopRight);
   QC_QRect->addMethod("setWidth",                    (q_method_t)QRECT_setWidth);
   QC_QRect->addMethod("setX",                        (q_method_t)QRECT_setX);
   QC_QRect->addMethod("setY",                        (q_method_t)QRECT_setY);
   //QC_QRect->addMethod("size",                        (q_method_t)QRECT_size);
   QC_QRect->addMethod("top",                         (q_method_t)QRECT_top);
   QC_QRect->addMethod("topLeft",                     (q_method_t)QRECT_topLeft);
   QC_QRect->addMethod("topRight",                    (q_method_t)QRECT_topRight);
   QC_QRect->addMethod("translate",                   (q_method_t)QRECT_translate);
   QC_QRect->addMethod("translate",                   (q_method_t)QRECT_translate);
   QC_QRect->addMethod("translated",                  (q_method_t)QRECT_translated);
   QC_QRect->addMethod("translated",                  (q_method_t)QRECT_translated);
   QC_QRect->addMethod("united",                      (q_method_t)QRECT_united);
   QC_QRect->addMethod("width",                       (q_method_t)QRECT_width);
   QC_QRect->addMethod("x",                           (q_method_t)QRECT_x);
   QC_QRect->addMethod("y",                           (q_method_t)QRECT_y);

   traceout("initQRectClass()");
   return QC_QRect;
}
