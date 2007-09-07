/*
 QC_QPen.cc
 
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

#include "QC_QPen.h"

int CID_QPEN;
class QoreClass *QC_QPen = 0;

//QPen ()
//QPen ( Qt::PenStyle style )
//QPen ( const QColor & color )
//QPen ( const QBrush & brush, qreal width, Qt::PenStyle style = Qt::SolidLine, Qt::PenCapStyle cap = Qt::SquareCap, Qt::PenJoinStyle join = Qt::BevelJoin )
//QPen ( const QPen & pen )
static void QPEN_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QPEN, new QoreQPen());
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQBrush *brush = (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink);
      if (!brush) {
         QoreQColor *color = (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink);
         if (!color) {
            if (!xsink->isException())
               xsink->raiseException("QPEN-QPEN-PARAM-ERROR", "QPen::QPen() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return;
         }
         ReferenceHolder<QoreQColor> colorHolder(color, xsink);
         self->setPrivate(CID_QPEN, new QoreQPen(*(static_cast<QColor *>(color))));
         return;
      }
      ReferenceHolder<QoreQBrush> brushHolder(brush, xsink);
      p = get_param(params, 1);
      qreal width = p ? p->getAsFloat() : 0.0;
      p = get_param(params, 2);
      Qt::PenStyle style = (Qt::PenStyle)(p ? p->getAsInt() : 0);
      p = get_param(params, 3);
      Qt::PenCapStyle cap = (Qt::PenCapStyle)(p ? p->getAsInt() : 0);
      p = get_param(params, 4);
      Qt::PenJoinStyle join = (Qt::PenJoinStyle)(p ? p->getAsInt() : 0);
      self->setPrivate(CID_QPEN, new QoreQPen(*(static_cast<QBrush *>(brush)), width, style, cap, join));
      return;
   }

   Qt::PenStyle style;
   if (p && p->type == NT_PENSTYLE)
      style = (Qt::PenStyle)p->val.intval;
   else
      style = (Qt::PenStyle)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QPEN, new QoreQPen(style));
   return;
}

static void QPEN_copy(class Object *self, class Object *old, class QoreQPen *qp, ExceptionSink *xsink)
{
   xsink->raiseException("QPEN-COPY-ERROR", "objects of this class cannot be copied");
}

//QBrush brush () const
static QoreNode *QPEN_brush(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qb = new Object(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qp->brush());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//Qt::PenCapStyle capStyle () const
static QoreNode *QPEN_capStyle(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->capStyle());
}

//QColor color () const
static QoreNode *QPEN_color(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qc = new Object(QC_QColor, getProgram());
   QoreQColor *q_qc = new QoreQColor(qp->color());
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

//qreal dashOffset () const
static QoreNode *QPEN_dashOffset(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qp->dashOffset());
}

////QVector<qreal> dashPattern () const
//static QoreNode *QPEN_dashPattern(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->dashPattern());
//}

////DataPtr & data_ptr ()
//static QoreNode *QPEN_data_ptr(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->data_ptr());
//}

//bool isCosmetic () const
static QoreNode *QPEN_isCosmetic(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->isCosmetic());
}

//bool isSolid () const
static QoreNode *QPEN_isSolid(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->isSolid());
}

//Qt::PenJoinStyle joinStyle () const
static QoreNode *QPEN_joinStyle(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->joinStyle());
}

//qreal miterLimit () const
static QoreNode *QPEN_miterLimit(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qp->miterLimit());
}

//void setBrush ( const QBrush & brush )
static QoreNode *QPEN_setBrush(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQBrush *brush = (p && p->type == NT_OBJECT) ? (QoreQBrush *)p->val.object->getReferencedPrivateData(CID_QBRUSH, xsink) : 0;
   if (!brush) {
      if (!xsink->isException())
         xsink->raiseException("QPEN-SETBRUSH-PARAM-ERROR", "expecting a QBrush object as first argument to QPen::setBrush()");
      return 0;
   }
   ReferenceHolder<QoreQBrush> brushHolder(brush, xsink);
   qp->setBrush(*(static_cast<QBrush *>(brush)));
   return 0;
}

//void setCapStyle ( Qt::PenCapStyle style )
static QoreNode *QPEN_setCapStyle(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::PenCapStyle style = (Qt::PenCapStyle)(p ? p->getAsInt() : 0);
   qp->setCapStyle(style);
   return 0;
}

//void setColor ( const QColor & color )
static QoreNode *QPEN_setColor(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQColor *color = (p && p->type == NT_OBJECT) ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!color) {
      if (!xsink->isException())
         xsink->raiseException("QPEN-SETCOLOR-PARAM-ERROR", "expecting a QColor object as first argument to QPen::setColor()");
      return 0;
   }
   ReferenceHolder<QoreQColor> colorHolder(color, xsink);
   qp->setColor(*(static_cast<QColor *>(color)));
   return 0;
}

//void setCosmetic ( bool cosmetic )
static QoreNode *QPEN_setCosmetic(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool cosmetic = p ? p->getAsBool() : false;
   qp->setCosmetic(cosmetic);
   return 0;
}

//void setDashOffset ( qreal offset )
static QoreNode *QPEN_setDashOffset(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal offset = p ? p->getAsFloat() : 0.0;
   qp->setDashOffset(offset);
   return 0;
}

////void setDashPattern ( const QVector<qreal> & pattern )
//static QoreNode *QPEN_setDashPattern(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QVector<qreal> pattern = p;
//   qp->setDashPattern(pattern);
//   return 0;
//}

//void setJoinStyle ( Qt::PenJoinStyle style )
static QoreNode *QPEN_setJoinStyle(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::PenJoinStyle style = (Qt::PenJoinStyle)(p ? p->getAsInt() : 0);
   qp->setJoinStyle(style);
   return 0;
}

//void setMiterLimit ( qreal limit )
static QoreNode *QPEN_setMiterLimit(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal limit = p ? p->getAsFloat() : 0.0;
   qp->setMiterLimit(limit);
   return 0;
}

//void setStyle ( Qt::PenStyle style )
static QoreNode *QPEN_setStyle(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::PenStyle style = (Qt::PenStyle)(p ? p->getAsInt() : 0);
   qp->setStyle(style);
   return 0;
}

//void setWidth ( int width )
static QoreNode *QPEN_setWidth(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   qp->setWidth(width);
   return 0;
}

//void setWidthF ( qreal width )
static QoreNode *QPEN_setWidthF(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal width = p ? p->getAsFloat() : 0.0;
   qp->setWidthF(width);
   return 0;
}

//Qt::PenStyle style () const
static QoreNode *QPEN_style(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->style());
}

//int width () const
static QoreNode *QPEN_width(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->width());
}

//qreal widthF () const
static QoreNode *QPEN_widthF(Object *self, QoreQPen *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qp->widthF());
}

QoreClass *initQPenClass()
{
   QC_QPen = new QoreClass("QPen", QDOM_GUI);
   CID_QPEN = QC_QPen->getID();

   QC_QPen->setConstructor(QPEN_constructor);
   QC_QPen->setCopy((q_copy_t)QPEN_copy);

   QC_QPen->addMethod("brush",                       (q_method_t)QPEN_brush);
   QC_QPen->addMethod("capStyle",                    (q_method_t)QPEN_capStyle);
   QC_QPen->addMethod("color",                       (q_method_t)QPEN_color);
   QC_QPen->addMethod("dashOffset",                  (q_method_t)QPEN_dashOffset);
   //QC_QPen->addMethod("dashPattern",                 (q_method_t)QPEN_dashPattern);
   //QC_QPen->addMethod("data_ptr",                    (q_method_t)QPEN_data_ptr);
   QC_QPen->addMethod("isCosmetic",                  (q_method_t)QPEN_isCosmetic);
   QC_QPen->addMethod("isSolid",                     (q_method_t)QPEN_isSolid);
   QC_QPen->addMethod("joinStyle",                   (q_method_t)QPEN_joinStyle);
   QC_QPen->addMethod("miterLimit",                  (q_method_t)QPEN_miterLimit);
   QC_QPen->addMethod("setBrush",                    (q_method_t)QPEN_setBrush);
   QC_QPen->addMethod("setCapStyle",                 (q_method_t)QPEN_setCapStyle);
   QC_QPen->addMethod("setColor",                    (q_method_t)QPEN_setColor);
   QC_QPen->addMethod("setCosmetic",                 (q_method_t)QPEN_setCosmetic);
   QC_QPen->addMethod("setDashOffset",               (q_method_t)QPEN_setDashOffset);
   //QC_QPen->addMethod("setDashPattern",              (q_method_t)QPEN_setDashPattern);
   QC_QPen->addMethod("setJoinStyle",                (q_method_t)QPEN_setJoinStyle);
   QC_QPen->addMethod("setMiterLimit",               (q_method_t)QPEN_setMiterLimit);
   QC_QPen->addMethod("setStyle",                    (q_method_t)QPEN_setStyle);
   QC_QPen->addMethod("setWidth",                    (q_method_t)QPEN_setWidth);
   QC_QPen->addMethod("setWidthF",                   (q_method_t)QPEN_setWidthF);
   QC_QPen->addMethod("style",                       (q_method_t)QPEN_style);
   QC_QPen->addMethod("width",                       (q_method_t)QPEN_width);
   QC_QPen->addMethod("widthF",                      (q_method_t)QPEN_widthF);

   return QC_QPen;
}
