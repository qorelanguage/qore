/*
 QC_QBrush.cc
 
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

#include "QC_QBrush.h"
#include "QC_QColor.h"

int CID_QBRUSH;
QoreClass *QC_QBrush = 0;

static void QBRUSH_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQBrush *qb;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qb = new QoreQBrush();
   else if (p->type == NT_OBJECT)
   {
      QoreQColor *color = p ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
      if (color) {
	 ReferenceHolder<QoreQColor> holder(color, xsink);
	 
	 p = get_param(params, 1);
	 if (p && p->type == NT_OBJECT) {

	    AbstractPrivateData *apd_qpixmap = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
	    if (!apd_qpixmap) {
	       if (!xsink->isException())
		  xsink->raiseException("QBRUSH-CONSTRUCTOR-ERROR", "QLabel::setPixmap() does not know how to handle arguments of class '%s'", p->val.object->getClass()->getName());
	       return;
	    }
	    ReferenceHolder<AbstractPrivateData> holder(apd_qpixmap, xsink);
	    QoreAbstractQPixmap *qpixmap = dynamic_cast<QoreAbstractQPixmap *>(apd_qpixmap);
	    assert(qpixmap);
	    qb = new QoreQBrush(*color, *(qpixmap->getQPixmap()));
	 }

	 Qt::BrushStyle style = p && p->type == NT_BRUSHSTYLE ? (Qt::BrushStyle)p->val.intval : Qt::SolidPattern;	 
	 qb = new QoreQBrush(*color, style);
      }
      else {
	 xsink->raiseException("QBRUSH-CONSTRUCTOR-ERROR", "QBrush::constructor() does not take objects of class '%s' as an argument", p->val.object->getClass()->getName());
	 return;
      }
   }
   else if (p->type == NT_BRUSHSTYLE) {
      Qt::BrushStyle style = (Qt::BrushStyle)p->val.intval;
      qb = new QoreQBrush(style);
   }
   else {
      Qt::GlobalColor color = (Qt::GlobalColor)p->getAsInt();

      p = get_param(params, 1);
      if (p && p->type == NT_OBJECT) {

	 AbstractPrivateData *apd_qpixmap = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
	 if (!apd_qpixmap) {
	    if (!xsink->isException())
	       xsink->raiseException("QBRUSH-CONSTRUCTOR-ERROR", "QLabel::setPixmap() does not know how to handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return;
	 }
	 ReferenceHolder<AbstractPrivateData> holder(apd_qpixmap, xsink);
	 QoreAbstractQPixmap *qpixmap = dynamic_cast<QoreAbstractQPixmap *>(apd_qpixmap);
	 assert(qpixmap);
	 qb = new QoreQBrush(color, *(qpixmap->getQPixmap()));
      }
      else
	 qb = new QoreQBrush(color);
   }

   self->setPrivate(CID_QBRUSH, qb);
}

static void QBRUSH_copy(class Object *self, class Object *old, class QoreQBrush *qf, ExceptionSink *xsink)
{
   xsink->raiseException("QBRUSH-COPY-ERROR", "objects of this class cannot be copied");
}

//const QColor & color () const
static QoreNode *QBRUSH_color(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
{
   QoreQColor *n_qc = new QoreQColor(qb->color());
   Object *nqc = new Object(self->getClass(CID_QCOLOR), getProgram());
   nqc->setPrivate(CID_QCOLOR, n_qc);
   return new QoreNode(nqc);
}

//DataPtr & data_ptr ()
//static QoreNode *QBRUSH_data_ptr(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qb->data_ptr());
//}

//const QGradient * gradient () const
//static QoreNode *QBRUSH_gradient(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qb->gradient();
//}

//bool isDetached () const
static QoreNode *QBRUSH_isDetached(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qb->isDetached());
}

//bool isOpaque () const
static QoreNode *QBRUSH_isOpaque(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qb->isOpaque());
}

//const QMatrix & matrix () const
//static QoreNode *QBRUSH_matrix(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qb->matrix());
//}

//void setColor ( const QColor & color )
static QoreNode *QBRUSH_setColor(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQColor *color = (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (!color)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QBRUSH-SETCOLOR-PARAM-ERROR", "QBrush::setColor() cannot handle argument of class '%s'", p->val.object->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<QoreQColor> holder(color, xsink);
      qb->setColor(*((QColor *)color));
   }
   else {
      qb->setColor((Qt::GlobalColor)(p ? p->getAsInt() : 0));
   }
      
   return 0;
}

//void setMatrix ( const QMatrix & matrix )
//static QoreNode *QBRUSH_setMatrix(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QMatrix matrix = p;
//   qb->setMatrix(matrix);
//   return 0;
//}

//void setStyle ( Qt::BrushStyle style )
static QoreNode *QBRUSH_setStyle(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_BRUSHSTYLE, 0);
   if (!p) {
      xsink->raiseException("QBRUSH-SETSTYLE-ERROR", "expecting a BrushStyle constant as the sole argument to QBrush::setStyle()");
      return 0;
   }
   Qt::BrushStyle style = (Qt::BrushStyle)p->val.intval;
   qb->setStyle(style);
   return 0;
}

//void setTexture ( const QPixmap & pixmap )
//static QoreNode *QBRUSH_setTexture(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QPixmap pixmap = p;
//   qb->setTexture(pixmap);
//   return 0;
//}

//void setTextureImage ( const QImage & image )
//static QoreNode *QBRUSH_setTextureImage(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QImage image = p;
//   qb->setTextureImage(image);
//   return 0;
//}

//void setTransform ( const QTransform & )
//static QoreNode *QBRUSH_setTransform(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QTransform qtransform = p;
//   qb->setTransform(qtransform);
//   return 0;
//}

//Qt::BrushStyle style () const
static QoreNode *QBRUSH_style(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
{
   return make_enum(NT_BRUSHSTYLE, (int)qb->style());
}

//QPixmap texture () const
//static QoreNode *QBRUSH_texture(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qb->texture());
//}

//QImage textureImage () const
//static QoreNode *QBRUSH_textureImage(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qb->textureImage());
//}

//QTransform transform () const
//static QoreNode *QBRUSH_transform(Object *self, QoreQBrush *qb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qb->transform());
//}


class QoreClass *initQBrushClass()
{
   tracein("initQBrushClass()");
   
   QC_QBrush = new QoreClass("QBrush", QDOM_GUI);
   CID_QBRUSH = QC_QBrush->getID();
   QC_QBrush->setConstructor(QBRUSH_constructor);
   QC_QBrush->setCopy((q_copy_t)QBRUSH_copy);

   QC_QBrush->addMethod("color",                       (q_method_t)QBRUSH_color);
   //QC_QBrush->addMethod("data_ptr",                    (q_method_t)QBRUSH_data_ptr);
   //QC_QBrush->addMethod("gradient",                    (q_method_t)QBRUSH_gradient);
   QC_QBrush->addMethod("isDetached",                  (q_method_t)QBRUSH_isDetached);
   QC_QBrush->addMethod("isOpaque",                    (q_method_t)QBRUSH_isOpaque);
   //QC_QBrush->addMethod("matrix",                      (q_method_t)QBRUSH_matrix);
   QC_QBrush->addMethod("setColor",                    (q_method_t)QBRUSH_setColor);
   QC_QBrush->addMethod("setColor",                    (q_method_t)QBRUSH_setColor);
   //QC_QBrush->addMethod("setMatrix",                   (q_method_t)QBRUSH_setMatrix);
   QC_QBrush->addMethod("setStyle",                    (q_method_t)QBRUSH_setStyle);
   //QC_QBrush->addMethod("setTexture",                  (q_method_t)QBRUSH_setTexture);
   //QC_QBrush->addMethod("setTextureImage",             (q_method_t)QBRUSH_setTextureImage);
   //QC_QBrush->addMethod("setTransform",                (q_method_t)QBRUSH_setTransform);
   QC_QBrush->addMethod("style",                       (q_method_t)QBRUSH_style);
   //QC_QBrush->addMethod("texture",                     (q_method_t)QBRUSH_texture);
   //QC_QBrush->addMethod("textureImage",                (q_method_t)QBRUSH_textureImage);
   //QC_QBrush->addMethod("transform",                   (q_method_t)QBRUSH_transform);

   traceout("initQBrushClass()");
   return QC_QBrush;
}
