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
#include "qore-qt.h"

#include "QC_QBrush.h"
#include "QC_QColor.h"
#include "QC_QPixmap.h"
#include "QC_QImage.h"

int CID_QBRUSH;
QoreClass *QC_QBrush = 0;

//QBrush ()
//QBrush ( Qt::BrushStyle style )
//QBrush ( const QColor & color, Qt::BrushStyle style = Qt::SolidPattern )
//QBrush ( const QColor & color, const QPixmap & pixmap )
//QBrush ( Qt::GlobalColor color, Qt::BrushStyle style = Qt::SolidPattern )
//QBrush ( Qt::GlobalColor color, const QPixmap & pixmap )
//QBrush ( const QPixmap & pixmap )
//QBrush ( const QImage & image )
//QBrush ( const QBrush & other )
//QBrush ( const QGradient & gradient )
static void QBRUSH_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQBrush *qb;

   AbstractQoreNode *p = get_param(params, 0);
   //printd(5, "QBrush::constructor() p0=%08p '%s'\n", p, p && p->type == NT_OBJECT ? (reinterpret_cast<QoreObject *>(p))->getClass()->getName() : "n/a");
   if (is_nothing(p))
      qb = new QoreQBrush();
   else if (p->type == NT_OBJECT)
   {
      QoreQColor *color = p ? (QoreQColor *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
      if (color) {
	 ReferenceHolder<QoreQColor> holder(color, xsink);
	 
	 p = get_param(params, 1);
	 //printd(5, "QBrush::constructor() p1=%08p '%s'\n", p, p && p->type == NT_OBJECT ? (reinterpret_cast<QoreObject *>(p))->getClass()->getName() : "n/a");
	 if (p && p->type == NT_OBJECT) {

	    AbstractPrivateData *apd_qpixmap = (p && p->type == NT_OBJECT) ? (reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
	    if (!apd_qpixmap) {
	       if (!xsink->isException())
		  xsink->raiseException("QBRUSH-CONSTRUCTOR-ERROR", "QLabel::setPixmap() does not know how to handle arguments of class '%s'", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
	       return;
	    }
	    ReferenceHolder<AbstractPrivateData> holder(apd_qpixmap, xsink);
	    QoreAbstractQPixmap *qpixmap = dynamic_cast<QoreAbstractQPixmap *>(apd_qpixmap);
	    assert(qpixmap);
	    //printd(5, "QBrush::constructor(color=%08p, pixmap=%08p)\n", color, qpixmap->getQPixmap());
	    qb = new QoreQBrush(*color, *(qpixmap->getQPixmap()));
	 }

	 Qt::BrushStyle style = p && p->type == NT_BRUSHSTYLE ? (reinterpret_cast<BrushStyleNode *>(p))->getStyle() : Qt::SolidPattern;	 
	 qb = new QoreQBrush(*color, style);
      }
      else {
	 xsink->raiseException("QBRUSH-CONSTRUCTOR-ERROR", "QBrush::constructor() does not take objects of class '%s' as an argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
	 return;
      }
   }
   else if (p->type == NT_BRUSHSTYLE) {
      Qt::BrushStyle style = (reinterpret_cast<BrushStyleNode *>(p))->getStyle();
      qb = new QoreQBrush(style);
   }
   else {
      Qt::GlobalColor color = (Qt::GlobalColor)p->getAsInt();

      p = get_param(params, 1);
      if (p && p->type == NT_OBJECT) {

	 AbstractPrivateData *apd_qpixmap = (p && p->type == NT_OBJECT) ? (reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
	 if (!apd_qpixmap) {
	    if (!xsink->isException())
	       xsink->raiseException("QBRUSH-CONSTRUCTOR-ERROR", "QLabel::setPixmap() does not know how to handle arguments of class '%s'", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
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

static void QBRUSH_copy(class QoreObject *self, class QoreObject *old, class QoreQBrush *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QBRUSH, new QoreQBrush(*qf));
}

//const QColor & color () const
static AbstractQoreNode *QBRUSH_color(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQColor *n_qc = new QoreQColor(qb->getQBrush()->color());
   QoreObject *nqc = new QoreObject(QC_QColor, getProgram());
   nqc->setPrivate(CID_QCOLOR, n_qc);
   return nqc;
}

//DataPtr & data_ptr ()
//static AbstractQoreNode *QBRUSH_data_ptr(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qb->getQBrush()->data_ptr());
//}

//const QGradient * gradient () const
//static AbstractQoreNode *QBRUSH_gradient(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return qb->getQBrush()->gradient();
//}

//bool isDetached () const
static AbstractQoreNode *QBRUSH_isDetached(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qb->getQBrush()->isDetached());
}

//bool isOpaque () const
static AbstractQoreNode *QBRUSH_isOpaque(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qb->getQBrush()->isOpaque());
}

//const QMatrix & matrix () const
//static AbstractQoreNode *QBRUSH_matrix(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qb->getQBrush()->matrix());
//}

//void setColor ( const QColor & color )
static AbstractQoreNode *QBRUSH_setColor(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQColor *color = (QoreQColor *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (!color)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QBRUSH-SETCOLOR-PARAM-ERROR", "QBrush::setColor() cannot handle argument of class '%s'", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<QoreQColor> holder(color, xsink);
      qb->getQBrush()->setColor(*((QColor *)color));
   }
   else {
      qb->getQBrush()->setColor((Qt::GlobalColor)(p ? p->getAsInt() : 0));
   }
      
   return 0;
}

//void setMatrix ( const QMatrix & matrix )
//static AbstractQoreNode *QBRUSH_setMatrix(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   ??? QMatrix matrix = p;
//   qb->getQBrush()->setMatrix(matrix);
//   return 0;
//}

//void setStyle ( Qt::BrushStyle style )
static AbstractQoreNode *QBRUSH_setStyle(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = test_param(params, NT_BRUSHSTYLE, 0);
   if (!p) {
      xsink->raiseException("QBRUSH-SETSTYLE-ERROR", "expecting a BrushStyle constant as the sole argument to QBrush::setStyle()");
      return 0;
   }
   Qt::BrushStyle style = (reinterpret_cast<BrushStyleNode *>(p))->getStyle();
   qb->getQBrush()->setStyle(style);
   return 0;
}

//void setTexture ( const QPixmap & pixmap )
static AbstractQoreNode *QBRUSH_setTexture(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QBRUSH-SETTEXTURE-PARAM-ERROR", "expecting a QPixmap object as first argument to QBrush::setTexture()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   qb->getQBrush()->setTexture(*(static_cast<QPixmap *>(pixmap)));
   return 0;
}

//void setTextureImage ( const QImage & image )
static AbstractQoreNode *QBRUSH_setTextureImage(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQImage *image = (p && p->type == NT_OBJECT) ? (QoreQImage *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
   if (!image) {
      if (!xsink->isException())
         xsink->raiseException("QBRUSH-SETTEXTUREIMAGE-PARAM-ERROR", "expecting a QImage object as first argument to QBrush::setTextureImage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
   qb->getQBrush()->setTextureImage(*(static_cast<QImage *>(image)));
   return 0;
}

//void setTransform ( const QTransform & )
//static AbstractQoreNode *QBRUSH_setTransform(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   ??? QTransform qtransform = p;
//   qb->getQBrush()->setTransform(qtransform);
//   return 0;
//}

//Qt::BrushStyle style () const
static AbstractQoreNode *QBRUSH_style(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new BrushStyleNode(qb->getQBrush()->style());
}

//QPixmap texture () const
static AbstractQoreNode *QBRUSH_texture(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qb->getQBrush()->texture());
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//QImage textureImage () const
static AbstractQoreNode *QBRUSH_textureImage(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QImage, getProgram());
   QoreQImage *q_qi = new QoreQImage(qb->getQBrush()->textureImage());
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return o_qi;
}

//QTransform transform () const
//static AbstractQoreNode *QBRUSH_transform(QoreObject *self, QoreQBrush *qb, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qb->getQBrush()->transform());
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
   //QC_QBrush->addMethod("setMatrix",                   (q_method_t)QBRUSH_setMatrix);
   QC_QBrush->addMethod("setStyle",                    (q_method_t)QBRUSH_setStyle);
   QC_QBrush->addMethod("setTexture",                  (q_method_t)QBRUSH_setTexture);
   QC_QBrush->addMethod("setTextureImage",             (q_method_t)QBRUSH_setTextureImage);
   //QC_QBrush->addMethod("setTransform",                (q_method_t)QBRUSH_setTransform);
   QC_QBrush->addMethod("style",                       (q_method_t)QBRUSH_style);
   QC_QBrush->addMethod("texture",                     (q_method_t)QBRUSH_texture);
   QC_QBrush->addMethod("textureImage",                (q_method_t)QBRUSH_textureImage);
   //QC_QBrush->addMethod("transform",                   (q_method_t)QBRUSH_transform);

   traceout("initQBrushClass()");
   return QC_QBrush;
}
