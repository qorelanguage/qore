/*
 QC_QRectF.cc
 
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

#include "QC_QRectF.h"
#include "QC_QRect.h"
#include "QC_QColor.h"

int CID_QRECTF;
QoreClass *QC_QRectF = 0;

static void QRECTF_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQRectF *qr;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qr = new QoreQRectF();
   else {
      qreal x = p->getAsFloat();
      p = get_param(params, 1);
      qreal y = p ? p->getAsFloat() : 0;
      p = get_param(params, 2);
      qreal w = p ? p->getAsFloat() : 0;
      p = get_param(params, 3);
      qreal h = p ? p->getAsFloat() : 0;

      qr = new QoreQRectF(x, y, w, h);
   }

   self->setPrivate(CID_QRECTF, qr);
}

static void QRECTF_copy(class QoreObject *self, class QoreObject *old, class QoreQRectF *qr, ExceptionSink *xsink)
{
   self->setPrivate(CID_QRECTF, new QoreQRectF(*qr));
}

//void adjust ( qreal dx1, qreal dy1, qreal dx2, qreal dy2 )
static QoreNode *QRECTF_adjust(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float dx1 = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float dy1 = p ? p->getAsFloat() : 0;
   p = get_param(params, 2);
   float dx2 = p ? p->getAsFloat() : 0;
   p = get_param(params, 3);
   float dy2 = p ? p->getAsFloat() : 0;
   qrf->adjust(dx1, dy1, dx2, dy2);
   return 0;
}

//QRectF adjusted ( qreal dx1, qreal dy1, qreal dx2, qreal dy2 ) const
static QoreNode *QRECTF_adjusted(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float dx1 = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float dy1 = p ? p->getAsFloat() : 0;
   p = get_param(params, 2);
   float dx2 = p ? p->getAsFloat() : 0;
   p = get_param(params, 3);
   float dy2 = p ? p->getAsFloat() : 0;

   QoreQRectF *q_qrf = new QoreQRectF(qrf->adjusted(dx1, dy1, dx2, dy2));
   QoreObject *o_qrf = new QoreObject(self->getClass(CID_QRECTF), getProgram());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return o_qrf;
}

//qreal bottom () const
static QoreNode *QRECTF_bottom(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->bottom());
}

//QPointF bottomLeft () const
//static QoreNode *QRECTF_bottomLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qrf->bottomLeft());
//}

//QPointF bottomRight () const
//static QoreNode *QRECTF_bottomRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qrf->bottomRight());
//}

//QPointF center () const
//static QoreNode *QRECTF_center(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qrf->center());
//}

//bool contains ( const QPointF & point ) const
//static QoreNode *QRECTF_contains(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF point = p;
//   return new QoreBoolNode(qrf->contains(point));
//}

//bool contains ( const QRectF & rectangle ) const
//bool contains ( qreal x, qreal y ) const
static QoreNode *QRECTF_contains(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectangle = (QoreQRectF *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle)
      {
         if (!xsink->isException())
            xsink->raiseException("QRECTF-CONTAINS-PARAM-ERROR", "QRectF::contains() cannot handle arguments of class '%s'", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
         return 0;
      }

      ReferenceHolder<QoreQRectF> holder(rectangle, xsink);
      return new QoreBoolNode(qrf->contains(*((QRectF *)rectangle)));
   }

   float x = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float y = p ? p->getAsFloat() : 0;
   return new QoreBoolNode(qrf->contains(x, y));
}

//void getCoords ( qreal * x1, qreal * y1, qreal * x2, qreal * y2 ) const
//static QoreNode *QRECTF_getCoords(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? qreal* x1 = p;
//   p = get_param(params, 1);
//   ??? qreal* y1 = p;
//   p = get_param(params, 2);
//   ??? qreal* x2 = p;
//   p = get_param(params, 3);
//   ??? qreal* y2 = p;
//   qrf->getCoords(x1, y1, x2, y2);
//   return 0;
//}

//void getRect ( qreal * x, qreal * y, qreal * width, qreal * height ) const
//static QoreNode *QRECTF_getRect(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? qreal* x = p;
//   p = get_param(params, 1);
//   ??? qreal* y = p;
//   p = get_param(params, 2);
//   ??? qreal* width = p;
//   p = get_param(params, 3);
//   ??? qreal* height = p;
//   qrf->getRect(x, y, width, height);
//   return 0;
//}

//qreal height () const
static QoreNode *QRECTF_height(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->height());
}

//QRectF intersected ( const QRectF & rectangle ) const
static QoreNode *QRECTF_intersected(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRectF *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRectF *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!p || !rectangle)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECTF-INTERSECTED-PARAM-ERROR", "expecting a QRectF object as first argument to QRectF::intersected()");
      return 0;
   }
   ReferenceHolder<QoreQRectF> holder(rectangle, xsink);

   QoreQRectF *q_qrf = new QoreQRectF(qrf->intersected(*((QRectF *)rectangle)));
   QoreObject *o_qrf = new QoreObject(self->getClass(CID_QRECTF), getProgram());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return o_qrf;
}

//bool intersects ( const QRectF & rectangle ) const
static QoreNode *QRECTF_intersects(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRectF *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRectF *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!p || !rectangle)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECTF-INTERSECTS-PARAM-ERROR", "expecting a QRectF object as first argument to QRectF::intersects()");
      return 0;
   }
   ReferenceHolder<QoreQRectF> holder(rectangle, xsink);
   return new QoreBoolNode(qrf->intersects(*((QRectF *)rectangle)));
}

//bool isEmpty () const
static QoreNode *QRECTF_isEmpty(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qrf->isEmpty());
}

//bool isNull () const
static QoreNode *QRECTF_isNull(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qrf->isNull());
}

//bool isValid () const
static QoreNode *QRECTF_isValid(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qrf->isValid());
}

//qreal left () const
static QoreNode *QRECTF_left(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->left());
}

//void moveBottom ( qreal y )
static QoreNode *QRECTF_moveBottom(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float y = p ? p->getAsFloat() : 0;
   qrf->moveBottom(y);
   return 0;
}

//void moveBottomLeft ( const QPointF & position )
//static QoreNode *QRECTF_moveBottomLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->moveBottomLeft(position);
//   return 0;
//}

//void moveBottomRight ( const QPointF & position )
//static QoreNode *QRECTF_moveBottomRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->moveBottomRight(position);
//   return 0;
//}

//void moveCenter ( const QPointF & position )
//static QoreNode *QRECTF_moveCenter(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->moveCenter(position);
//   return 0;
//}

//void moveLeft ( qreal x )
static QoreNode *QRECTF_moveLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x = p ? p->getAsFloat() : 0;
   qrf->moveLeft(x);
   return 0;
}

//void moveRight ( qreal x )
static QoreNode *QRECTF_moveRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x = p ? p->getAsFloat() : 0;
   qrf->moveRight(x);
   return 0;
}

//void moveTo ( qreal x, qreal y )
static QoreNode *QRECTF_moveTo(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float y = p ? p->getAsFloat() : 0;
   qrf->moveTo(x, y);
   return 0;
}

//void moveTo ( const QPointF & position )
//static QoreNode *QRECTF_moveTo(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->moveTo(position);
//   return 0;
//}

//void moveTop ( qreal y )
static QoreNode *QRECTF_moveTop(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float y = p ? p->getAsFloat() : 0;
   qrf->moveTop(y);
   return 0;
}

//void moveTopLeft ( const QPointF & position )
//static QoreNode *QRECTF_moveTopLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->moveTopLeft(position);
//   return 0;
//}

//void moveTopRight ( const QPointF & position )
//static QoreNode *QRECTF_moveTopRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->moveTopRight(position);
//   return 0;
//}

//QRectF normalized () const
static QoreNode *QRECTF_normalized(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{

   QoreQRectF *q_qrf = new QoreQRectF(qrf->normalized());
   QoreObject *o_qrf = new QoreObject(self->getClass(CID_QRECTF), getProgram());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return o_qrf;
}

//qreal right () const
static QoreNode *QRECTF_right(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->right());
}

//void setBottom ( qreal y )
static QoreNode *QRECTF_setBottom(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float y = p ? p->getAsFloat() : 0;
   qrf->setBottom(y);
   return 0;
}

//void setBottomLeft ( const QPointF & position )
//static QoreNode *QRECTF_setBottomLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->setBottomLeft(position);
//   return 0;
//}

//void setBottomRight ( const QPointF & position )
//static QoreNode *QRECTF_setBottomRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->setBottomRight(position);
//   return 0;
//}

//void setCoords ( qreal x1, qreal y1, qreal x2, qreal y2 )
static QoreNode *QRECTF_setCoords(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x1 = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float y1 = p ? p->getAsFloat() : 0;
   p = get_param(params, 2);
   float x2 = p ? p->getAsFloat() : 0;
   p = get_param(params, 3);
   float y2 = p ? p->getAsFloat() : 0;
   qrf->setCoords(x1, y1, x2, y2);
   return 0;
}

//void setHeight ( qreal height )
static QoreNode *QRECTF_setHeight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float height = p ? p->getAsFloat() : 0;
   qrf->setHeight(height);
   return 0;
}

//void setLeft ( qreal x )
static QoreNode *QRECTF_setLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x = p ? p->getAsFloat() : 0;
   qrf->setLeft(x);
   return 0;
}

//void setRect ( qreal x, qreal y, qreal width, qreal height )
static QoreNode *QRECTF_setRect(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float y = p ? p->getAsFloat() : 0;
   p = get_param(params, 2);
   float width = p ? p->getAsFloat() : 0;
   p = get_param(params, 3);
   float height = p ? p->getAsFloat() : 0;
   qrf->setRect(x, y, width, height);
   return 0;
}

//void setRight ( qreal x )
static QoreNode *QRECTF_setRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x = p ? p->getAsFloat() : 0;
   qrf->setRight(x);
   return 0;
}

//void setSize ( const QSizeF & size )
//static QoreNode *QRECTF_setSize(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QSizeF size = p;
//   qrf->setSize(size);
//   return 0;
//}

//void setTop ( qreal y )
static QoreNode *QRECTF_setTop(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float y = p ? p->getAsFloat() : 0;
   qrf->setTop(y);
   return 0;
}

//void setTopLeft ( const QPointF & position )
//static QoreNode *QRECTF_setTopLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->setTopLeft(position);
//   return 0;
//}

//void setTopRight ( const QPointF & position )
//static QoreNode *QRECTF_setTopRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF position = p;
//   qrf->setTopRight(position);
//   return 0;
//}

//void setWidth ( qreal width )
static QoreNode *QRECTF_setWidth(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float width = p ? p->getAsFloat() : 0;
   qrf->setWidth(width);
   return 0;
}

//void setX ( qreal x )
static QoreNode *QRECTF_setX(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float x = p ? p->getAsFloat() : 0;
   qrf->setX(x);
   return 0;
}

//void setY ( qreal y )
static QoreNode *QRECTF_setY(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float y = p ? p->getAsFloat() : 0;
   qrf->setY(y);
   return 0;
}

//QSizeF size () const
//static QoreNode *QRECTF_size(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qrf->size());
//}

//QRect toAlignedRect () const
static QoreNode *QRECTF_toAlignedRect(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qrf->toAlignedRect());
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//QRect toRect () const
static QoreNode *QRECTF_toRect(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qrf->toRect());
   QoreObject *o_qr = new QoreObject(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//qreal top () const
static QoreNode *QRECTF_top(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->top());
}

//QPointF topLeft () const
//static QoreNode *QRECTF_topLeft(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qrf->topLeft());
//}

//QPointF topRight () const
//static QoreNode *QRECTF_topRight(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qrf->topRight());
//}

//void translate ( qreal dx, qreal dy )
static QoreNode *QRECTF_translate(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float dx = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float dy = p ? p->getAsFloat() : 0;
   qrf->translate(dx, dy);
   return 0;
}

//void translate ( const QPointF & offset )
//static QoreNode *QRECTF_translate(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF offset = p;
//   qrf->translate(offset);
//   return 0;
//}

//QRectF translated ( qreal dx, qreal dy ) const
static QoreNode *QRECTF_translated(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float dx = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float dy = p ? p->getAsFloat() : 0;

   QoreQRectF *q_qrf = new QoreQRectF(qrf->translated(dx, dy));
   QoreObject *o_qrf = new QoreObject(self->getClass(CID_QRECTF), getProgram());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return o_qrf;
}

//QRectF translated ( const QPointF & offset ) const
//static QoreNode *QRECTF_translated(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPointF offset = p;
//
//   QoreQRectF *q_qrf = new QoreQRectF(qrf->translated(offset));
//   QoreObject *o_qrf = new QoreObject(self->getClass(CID_QRECTF), getProgram());
//   o_qrf->setPrivate(CID_QRECTF, q_qrf);
//   return o_qrf;
//}

//QRectF united ( const QRectF & rectangle ) const
static QoreNode *QRECTF_united(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRectF *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRectF *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!p || !rectangle)
   {
      if (!xsink->isException())
         xsink->raiseException("QRECTF-UNITED-PARAM-ERROR", "expecting a QRectF object as first argument to QRectF::united()");
      return 0;
   }
   ReferenceHolder<QoreQRectF> holder(rectangle, xsink);

   QoreQRectF *q_qrf = new QoreQRectF(qrf->united(*((QRectF *)rectangle)));
   QoreObject *o_qrf = new QoreObject(self->getClass(CID_QRECTF), getProgram());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return o_qrf;
}

//qreal width () const
static QoreNode *QRECTF_width(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->width());
}

//qreal x () const
static QoreNode *QRECTF_x(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->x());
}

//qreal y () const
static QoreNode *QRECTF_y(QoreObject *self, QoreQRectF *qrf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qrf->y());
}

class QoreClass *initQRectFClass()
{
   tracein("initQRectFClass()");
   
   QC_QRectF = new QoreClass("QRectF", QDOM_GUI);
   CID_QRECTF = QC_QRectF->getID();

   QC_QRectF->setConstructor(QRECTF_constructor);
   QC_QRectF->setCopy((q_copy_t)QRECTF_copy);

   QC_QRectF->addMethod("adjust",                      (q_method_t)QRECTF_adjust);
   QC_QRectF->addMethod("adjusted",                    (q_method_t)QRECTF_adjusted);
   QC_QRectF->addMethod("bottom",                      (q_method_t)QRECTF_bottom);
   //QC_QRectF->addMethod("bottomLeft",                  (q_method_t)QRECTF_bottomLeft);
   //QC_QRectF->addMethod("bottomRight",                 (q_method_t)QRECTF_bottomRight);
   //QC_QRectF->addMethod("center",                      (q_method_t)QRECTF_center);
   QC_QRectF->addMethod("contains",                    (q_method_t)QRECTF_contains);
   //QC_QRectF->addMethod("getCoords",                   (q_method_t)QRECTF_getCoords);
   //QC_QRectF->addMethod("getRect",                     (q_method_t)QRECTF_getRect);
   QC_QRectF->addMethod("height",                      (q_method_t)QRECTF_height);
   QC_QRectF->addMethod("intersected",                 (q_method_t)QRECTF_intersected);
   QC_QRectF->addMethod("intersects",                  (q_method_t)QRECTF_intersects);
   QC_QRectF->addMethod("isEmpty",                     (q_method_t)QRECTF_isEmpty);
   QC_QRectF->addMethod("isNull",                      (q_method_t)QRECTF_isNull);
   QC_QRectF->addMethod("isValid",                     (q_method_t)QRECTF_isValid);
   QC_QRectF->addMethod("left",                        (q_method_t)QRECTF_left);
   QC_QRectF->addMethod("moveBottom",                  (q_method_t)QRECTF_moveBottom);
   //QC_QRectF->addMethod("moveBottomLeft",              (q_method_t)QRECTF_moveBottomLeft);
   //QC_QRectF->addMethod("moveBottomRight",             (q_method_t)QRECTF_moveBottomRight);
   //QC_QRectF->addMethod("moveCenter",                  (q_method_t)QRECTF_moveCenter);
   QC_QRectF->addMethod("moveLeft",                    (q_method_t)QRECTF_moveLeft);
   QC_QRectF->addMethod("moveRight",                   (q_method_t)QRECTF_moveRight);
   QC_QRectF->addMethod("moveTo",                      (q_method_t)QRECTF_moveTo);
   //QC_QRectF->addMethod("moveTo",                      (q_method_t)QRECTF_moveTo);
   QC_QRectF->addMethod("moveTop",                     (q_method_t)QRECTF_moveTop);
   //QC_QRectF->addMethod("moveTopLeft",                 (q_method_t)QRECTF_moveTopLeft);
   //QC_QRectF->addMethod("moveTopRight",                (q_method_t)QRECTF_moveTopRight);
   QC_QRectF->addMethod("normalized",                  (q_method_t)QRECTF_normalized);
   QC_QRectF->addMethod("right",                       (q_method_t)QRECTF_right);
   QC_QRectF->addMethod("setBottom",                   (q_method_t)QRECTF_setBottom);
   //QC_QRectF->addMethod("setBottomLeft",               (q_method_t)QRECTF_setBottomLeft);
   //QC_QRectF->addMethod("setBottomRight",              (q_method_t)QRECTF_setBottomRight);
   QC_QRectF->addMethod("setCoords",                   (q_method_t)QRECTF_setCoords);
   QC_QRectF->addMethod("setHeight",                   (q_method_t)QRECTF_setHeight);
   QC_QRectF->addMethod("setLeft",                     (q_method_t)QRECTF_setLeft);
   QC_QRectF->addMethod("setRect",                     (q_method_t)QRECTF_setRect);
   QC_QRectF->addMethod("setRight",                    (q_method_t)QRECTF_setRight);
   //QC_QRectF->addMethod("setSize",                     (q_method_t)QRECTF_setSize);
   QC_QRectF->addMethod("setTop",                      (q_method_t)QRECTF_setTop);
   //QC_QRectF->addMethod("setTopLeft",                  (q_method_t)QRECTF_setTopLeft);
   //QC_QRectF->addMethod("setTopRight",                 (q_method_t)QRECTF_setTopRight);
   QC_QRectF->addMethod("setWidth",                    (q_method_t)QRECTF_setWidth);
   QC_QRectF->addMethod("setX",                        (q_method_t)QRECTF_setX);
   QC_QRectF->addMethod("setY",                        (q_method_t)QRECTF_setY);
   //QC_QRectF->addMethod("size",                        (q_method_t)QRECTF_size);
   QC_QRectF->addMethod("toAlignedRect",               (q_method_t)QRECTF_toAlignedRect);
   QC_QRectF->addMethod("toRect",                      (q_method_t)QRECTF_toRect);
   QC_QRectF->addMethod("top",                         (q_method_t)QRECTF_top);
   //QC_QRectF->addMethod("topLeft",                     (q_method_t)QRECTF_topLeft);
   //QC_QRectF->addMethod("topRight",                    (q_method_t)QRECTF_topRight);
   QC_QRectF->addMethod("translate",                   (q_method_t)QRECTF_translate);
   //QC_QRectF->addMethod("translate",                   (q_method_t)QRECTF_translate);
   QC_QRectF->addMethod("translated",                  (q_method_t)QRECTF_translated);
   //QC_QRectF->addMethod("translated",                  (q_method_t)QRECTF_translated);
   QC_QRectF->addMethod("united",                      (q_method_t)QRECTF_united);
   QC_QRectF->addMethod("width",                       (q_method_t)QRECTF_width);
   QC_QRectF->addMethod("x",                           (q_method_t)QRECTF_x);
   QC_QRectF->addMethod("y",                           (q_method_t)QRECTF_y);

   traceout("initQRectFClass()");
   return QC_QRectF;
}
