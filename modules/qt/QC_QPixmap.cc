/*
 QC_QPixmap.cc
 
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

#include "QC_QPixmap.h"
#include "QC_QBitmap.h"
#include "QC_QSize.h"
#include "QC_QRect.h"
#include "QC_QColor.h"
#include "QC_QWidget.h"
#include "QC_QPoint.h"
#include "QC_QImage.h"

int CID_QPIXMAP;
QoreClass *QC_QPixmap = 0;

//QPixmap () 
//QPixmap ( int width, int height ) 
//QPixmap ( const QString & fileName, const char * format = 0, Qt::ImageConversionFlags flags = Qt::AutoColor ) 
//QPixmap ( const char * const[] xpm ) 
//QPixmap ( const QPixmap & pixmap ) 
//QPixmap ( const QSize & size )
static void QPIXMAP_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreQPixmap *qp;
   QoreNode *p = get_param(params, 0);

   QString fileName;
   if (!get_qstring(p, fileName, xsink, true)) {
      QoreStringNode *str = test_string_param(params, 1);
      const char *format = str ? str->getBuffer() : 0;
      p = get_param(params, 2);
      Qt::ImageConversionFlags flags = !is_nothing(p) ? (Qt::ImageConversionFlags)p->getAsInt() : Qt::AutoColor;

      qp = new QoreQPixmap(fileName, format, flags);
      //printd(5, "QPixmap::constructor('%s', %08p, %d) valid=%s\n", fileName.toUtf8().data(), format, (int)flags, !qp->isNull() ? "true" : "false");
   }

/*
   if (p && p->type == NT_BINARY)
      qp = new QoreQPixmap(p->val.bin);
*/
   else if (p && p->type == NT_OBJECT) {
      QoreQSize *size = (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size) {
         if (!xsink->isException())
            xsink->raiseException("QPIXMAP-CONSTRUCTOR-PARAM-ERROR", "QPixmap::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return;
      }
      ReferenceHolder<QoreQSize> sizeHolder(size, xsink);
      qp = new QoreQPixmap(*(static_cast<QSize *>(size)));
   }
   else {
      int w = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int h = p ? p->getAsInt() : 0;
      qp = new QoreQPixmap(w, h);
   }

   self->setPrivate(CID_QPIXMAP, qp);
}

static void QPIXMAP_copy(class QoreObject *self, class QoreObject *old, class QoreQPixmap *qlcdn, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPIXMAP, new QoreQPixmap(*qlcdn));
   //xsink->raiseException("QPIXMAP-COPY-ERROR", "objects of this class cannot be copied");
}

//QPixmap alphaChannel () const
static QoreNode *QPIXMAP_alphaChannel(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(self->getClass(CID_QPIXMAP), getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qp->getQPixmap()->alphaChannel());
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//qint64 cacheKey () const
static QoreNode *QPIXMAP_cacheKey(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPixmap()->cacheKey());
}

//QPixmap copy ( const QRect & rectangle = QRect() ) const
//QPixmap copy ( int x, int y, int width, int height ) const
static QoreNode *QPIXMAP_QT_copy(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rectangle) {
         if (!xsink->isException())
            xsink->raiseException("QPIXMAP-COPY-PARAM-ERROR", "QPixmap::copy() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
      QoreObject *o_qp = new QoreObject(self->getClass(CID_QPIXMAP), getProgram());
      QoreQPixmap *q_qp = new QoreQPixmap(qp->getQPixmap()->copy(*(static_cast<QRect *>(rectangle))));
      o_qp->setPrivate(CID_QPIXMAP, q_qp);
      return new QoreNode(o_qp);
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   QoreObject *o_qp = new QoreObject(self->getClass(CID_QPIXMAP), getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qp->getQPixmap()->copy(x, y, width, height));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//QBitmap createHeuristicMask ( bool clipTight = true ) const
static QoreNode *QPIXMAP_createHeuristicMask(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool clipTight = !is_nothing(p) ? p->getAsBool() : true;
   QoreObject *o_qb = new QoreObject(QC_QBitmap, getProgram());
   QoreQBitmap *q_qb = new QoreQBitmap(qp->getQPixmap()->createHeuristicMask(clipTight));
   o_qb->setPrivate(CID_QBITMAP, q_qb);
   return new QoreNode(o_qb);
}

//QBitmap createMaskFromColor ( const QColor & maskColor, Qt::MaskMode mode ) const
//QBitmap createMaskFromColor ( const QColor & maskColor ) const
static QoreNode *QPIXMAP_createMaskFromColor(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQColor *maskColor = p ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!maskColor) {
      xsink->raiseException("QPIXMAP-CREATEMASKFROMCOLOR-PARAM-ERROR", "QPixmap::createMaskFromColor() expected an object derived from QColor as the first argument");
      return 0;
   }
   
   ReferenceHolder<QoreQColor> maskColorHolder(maskColor, xsink);

   QoreQBitmap *q_qb;

   p = get_param(params, 1);
   if (is_nothing(p))
      q_qb = new QoreQBitmap(qp->getQPixmap()->createMaskFromColor(*(static_cast<QColor *>(maskColor))));
   else {
      Qt::MaskMode mode = (Qt::MaskMode)p->getAsInt();
      q_qb = new QoreQBitmap(qp->getQPixmap()->createMaskFromColor(*(static_cast<QColor *>(maskColor)), mode));
   }
 
   QoreObject *o_qb = new QoreObject(QC_QBitmap, getProgram());
   o_qb->setPrivate(CID_QBITMAP, q_qb);
   return new QoreNode(o_qb);
}

//DataPtr & data_ptr ()
//static QoreNode *QPIXMAP_data_ptr(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPixmap()->data_ptr());
//}

//int depth () const
static QoreNode *QPIXMAP_depth(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPixmap()->depth());
}

//void detach ()
static QoreNode *QPIXMAP_detach(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   qp->getQPixmap()->detach();
   return 0;
}

//void fill ( const QColor & fillColor = Qt::white )
//void fill ( const QWidget * widget, const QPoint & offset )
//void fill ( const QWidget * widget, int x, int y )
static QoreNode *QPIXMAP_fill(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQColor *fillColor = (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink);
      if (fillColor) {
	 ReferenceHolder<QoreQColor> fillColorHolder(fillColor, xsink);
	 qp->getQPixmap()->fill(*(static_cast<QColor *>(fillColor)));
	 return 0;
      }

      QoreAbstractQWidget *widget = (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!widget) {
	 if (!xsink->isException())
	    xsink->raiseException("QPIXMAP-FILL-PARAM-ERROR", "QPixmap::fill() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
	 return 0;
      }

      ReferenceHolder<QoreAbstractQWidget> widgetHolder(widget, xsink);

      p = get_param(params, 1);
      if (p && p->type == NT_OBJECT) {

	 QoreQPoint *offset = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
	 if (!offset) {
	    if (!xsink->isException())
	       xsink->raiseException("QPIXMAP-FILL-PARAM-ERROR", "QPixmap::fill() does not know how to handle arguments of class '%s' as passed as the second argument (expecting an object derived from QPoint)", p->val.object->getClass()->getName());
	    return 0;
	 }

	 ReferenceHolder<QoreQPoint> offsetHolder(offset, xsink);
	 qp->getQPixmap()->fill(widget->getQWidget(), *(static_cast<QPoint *>(offset)));
	 return 0;

      }

      int x = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int y = p ? p->getAsInt() : 0;
      qp->getQPixmap()->fill(widget->getQWidget(), x, y);
      return 0;
   }

   qp->getQPixmap()->fill(Qt::white);
   return 0;
}

//Qt::HANDLE handle () const
//static QoreNode *QPIXMAP_handle(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   return new QoreNode((int64)qp->getQPixmap()->handle());
//}

//bool hasAlpha () const
static QoreNode *QPIXMAP_hasAlpha(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPixmap()->hasAlpha());
}

//bool hasAlphaChannel () const
static QoreNode *QPIXMAP_hasAlphaChannel(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPixmap()->hasAlphaChannel());
}

//int height () const
static QoreNode *QPIXMAP_height(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPixmap()->height());
}

//bool isNull () const
static QoreNode *QPIXMAP_isNull(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPixmap()->isNull());
}

//bool isQBitmap () const
static QoreNode *QPIXMAP_isQBitmap(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->getQPixmap()->isQBitmap());
}

//bool load ( const QString & fileName, const char * format = 0, Qt::ImageConversionFlags flags = Qt::AutoColor )
static QoreNode *QPIXMAP_load(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;

   QoreStringNode *pstr = test_string_param(params, 1);
   const char *format = pstr ? pstr->getBuffer() : 0;

   p = get_param(params, 2);
   Qt::ImageConversionFlags flags = !is_nothing(p) ? (Qt::ImageConversionFlags)p->getAsInt() :  Qt::AutoColor;
   return new QoreNode(qp->getQPixmap()->load(fileName, format, flags));
}

//bool loadFromData ( const uchar * data, uint len, const char * format = 0, Qt::ImageConversionFlags flags = Qt::AutoColor )
//bool loadFromData ( const QByteArray & data, const char * format = 0, Qt::ImageConversionFlags flags = Qt::AutoColor )
//static QoreNode *QPIXMAP_loadFromData(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? uchar* data = p;
//   p = get_param(params, 1);
//   if (p && p->type == NT_STRING) {
//      const char *format = p ? p->getBuffer() : 0;
//   p = get_param(params, 2);
//   const char *format = p ? p->getBuffer() : 0;
//   p = get_param(params, 3);
//   Qt::ImageConversionFlags flags = (Qt::ImageConversionFlags)(p ? p->getAsInt() : 0);
//   return new QoreNode(qp->getQPixmap()->loadFromData(data, format, format, flags));
//   }
//   unsigned len = p ? p->getAsBigInt() : 0;
//   p = get_param(params, 2);
//   const char *format = p ? p->getBuffer() : 0;
//   p = get_param(params, 3);
//   Qt::ImageConversionFlags flags = (Qt::ImageConversionFlags)(p ? p->getAsInt() : 0);
//   return new QoreNode(qp->getQPixmap()->loadFromData(data, len, format, flags));
//}

//QBitmap mask () const
static QoreNode *QPIXMAP_mask(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBitmap, getProgram());
   QoreQBitmap *q_qb = new QoreQBitmap(qp->getQPixmap()->mask());
   o_qb->setPrivate(CID_QBITMAP, q_qb);
   return new QoreNode(o_qb);
}

//QRect rect () const
static QoreNode *QPIXMAP_rect(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qp->getQPixmap()->rect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//bool save ( const QString & fileName, const char * format = 0, int quality = -1 ) const
//bool save ( QIODevice * device, const char * format = 0, int quality = -1 ) const
static QoreNode *QPIXMAP_save(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QPIXMAP-SAVE-PARAM-ERROR", "expecting a string as first argument to QPixmap::save()");
      return 0;
   }
   const char *fileName = p->getBuffer();

   p = test_string_param(params, 1);
   const char *format = p ? p->getBuffer() : 0;

   QoreNode *pn = get_param(params, 2);
   int quality = !is_nothing(pn) ? pn->getAsInt() : -1;
   return new QoreNode(qp->getQPixmap()->save(fileName, format, quality));
}

//QPixmap scaled ( const QSize & size, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, Qt::TransformationMode transformMode = Qt::FastTransformation ) const
//QPixmap scaled ( int width, int height, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, Qt::TransformationMode transformMode = Qt::FastTransformation ) const
static QoreNode *QPIXMAP_scaled(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = 0;
   if (p && p->type == NT_OBJECT) {
      size = (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size) {
         if (!xsink->isException())
            xsink->raiseException("QPIXMAP-SCALED-PARAM-ERROR", "QPixmap::scaled() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
   }
   ReferenceHolder<QoreQSize> sizeHolder(size, xsink);
   int offset = 0, width = 0, height = 0;
   if (!size) {
      width = p ? p->getAsInt() : 0;
      p = get_param(params, ++offset);
      height = p ? p->getAsInt() : 0;
   }
   p = get_param(params, ++offset);
   Qt::AspectRatioMode aspectRatioMode = !is_nothing(p) ? (Qt::AspectRatioMode)p->getAsInt() :  Qt::IgnoreAspectRatio;
   p = get_param(params, ++offset);
   Qt::TransformationMode transformMode = !is_nothing(p) ? (Qt::TransformationMode)p->getAsInt() :  Qt::FastTransformation;
   QoreObject *o_qp = new QoreObject(self->getClass(CID_QPIXMAP), getProgram());
   QoreQPixmap *q_qp;
   if (size)
      q_qp = new QoreQPixmap(qp->getQPixmap()->scaled(*size, aspectRatioMode, transformMode));
   else
      q_qp = new QoreQPixmap(qp->getQPixmap()->scaled(width, height, aspectRatioMode, transformMode));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//QPixmap scaledToHeight ( int height, Qt::TransformationMode mode = Qt::FastTransformation ) const
static QoreNode *QPIXMAP_scaledToHeight(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::TransformationMode mode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
   QoreObject *o_qp = new QoreObject(self->getClass(CID_QPIXMAP), getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qp->getQPixmap()->scaledToHeight(height, mode));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//QPixmap scaledToWidth ( int width, Qt::TransformationMode mode = Qt::FastTransformation ) const
static QoreNode *QPIXMAP_scaledToWidth(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::TransformationMode mode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
   QoreObject *o_qp = new QoreObject(self->getClass(CID_QPIXMAP), getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qp->getQPixmap()->scaledToWidth(width, mode));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//void setAlphaChannel ( const QPixmap & alphaChannel )
static QoreNode *QPIXMAP_setAlphaChannel(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   AbstractPrivateData *apd_alphaChannel = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!apd_alphaChannel) {
      if (!xsink->isException())
         xsink->raiseException("QPIXMAP-SETALPHACHANNEL-PARAM-ERROR", "expecting a QPixmap object as first argument to QPixmap::setAlphaChannel()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder(apd_alphaChannel, xsink);
   QoreAbstractQPixmap *alphaChannel = dynamic_cast<QoreAbstractQPixmap *>(apd_alphaChannel);
   assert(alphaChannel);
   qp->getQPixmap()->setAlphaChannel(*(alphaChannel->getQPixmap()));
   return 0;
}

//void setMask ( const QBitmap & newmask )
static QoreNode *QPIXMAP_setMask(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQBitmap *newmask = (p && p->type == NT_OBJECT) ? (QoreQBitmap *)p->val.object->getReferencedPrivateData(CID_QBITMAP, xsink) : 0;
   if (!newmask) {
      if (!xsink->isException())
         xsink->raiseException("QPIXMAP-SETMASK-PARAM-ERROR", "expecting a QBitmap object as first argument to QPixmap::setMask()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(newmask), xsink);
   qp->getQPixmap()->setMask(*(static_cast<QBitmap *>(newmask)));
   return 0;
}

//QSize size () const
static QoreNode *QPIXMAP_size(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qp->getQPixmap()->size());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QImage toImage () const
static QoreNode *QPIXMAP_toImage(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QImage, getProgram());
   QoreQImage *q_qi = new QoreQImage(qp->getQPixmap()->toImage());
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//CGImageRef toMacCGImageRef () const
//static QoreNode *QPIXMAP_toMacCGImageRef(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPixmap()->toMacCGImageRef());
//}

//HBITMAP toWinHBITMAP ( HBitmapFormat format = NoAlpha ) const
//static QoreNode *QPIXMAP_toWinHBITMAP(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QPixmap::HBitmapFormat format = (QPixmap::HBitmapFormat)(p ? p->getAsInt() : 0);
//   ??? return new QoreNode((int64)qp->getQPixmap()->toWinHBITMAP(format));
//}

//QPixmap transformed ( const QMatrix & matrix, Qt::TransformationMode mode = Qt::FastTransformation ) const
//QPixmap transformed ( const QTransform &, Qt::TransformationMode mode = Qt::FastTransformation ) const
//static QoreNode *QPIXMAP_transformed(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QMatrix matrix = p;
//   p = get_param(params, 1);
//   Qt::TransformationMode mode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
//   QoreObject *o_qp = new QoreObject(self->getClass(CID_QPIXMAP), getProgram());
//   QoreAbstractQPixmap *q_qp = new QoreQPixmap(*(qp->getQPixmap()->transformed(matrix, mode)));
//   o_qp->setPrivate(CID_QPIXMAP, q_qp);
//   return new QoreNode(o_qp);
//}

//int width () const
static QoreNode *QPIXMAP_width(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->getQPixmap()->width());
}

//const QX11Info & x11Info () const
//static QoreNode *QPIXMAP_x11Info(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->getQPixmap()->x11Info());
//}

//Qt::HANDLE x11PictureHandle () const
//static QoreNode *QPIXMAP_x11PictureHandle(QoreObject *self, QoreAbstractQPixmap *qp, const QoreList *params, ExceptionSink *xsink)
//{
//   return new QoreNode((int64)qp->getQPixmap()->x11PictureHandle());
//}

class QoreClass *initQPixmapClass(class QoreClass *qpaintdevice)
{
   tracein("initQPixmapClass()");
   
   QC_QPixmap = new QoreClass("QPixmap", QDOM_GUI);
   CID_QPIXMAP = QC_QPixmap->getID();

   QC_QPixmap->addBuiltinVirtualBaseClass(qpaintdevice);

   QC_QPixmap->setConstructor(QPIXMAP_constructor);
   QC_QPixmap->setCopy((q_copy_t)QPIXMAP_copy);

   QC_QPixmap->addMethod("alphaChannel",                (q_method_t)QPIXMAP_alphaChannel);
   QC_QPixmap->addMethod("cacheKey",                    (q_method_t)QPIXMAP_cacheKey);
   QC_QPixmap->addMethod("qt_copy",                     (q_method_t)QPIXMAP_QT_copy);
   QC_QPixmap->addMethod("createHeuristicMask",         (q_method_t)QPIXMAP_createHeuristicMask);
   QC_QPixmap->addMethod("createMaskFromColor",         (q_method_t)QPIXMAP_createMaskFromColor);
   //QC_QPixmap->addMethod("data_ptr",                    (q_method_t)QPIXMAP_data_ptr);
   QC_QPixmap->addMethod("depth",                       (q_method_t)QPIXMAP_depth);
   QC_QPixmap->addMethod("detach",                      (q_method_t)QPIXMAP_detach);
   QC_QPixmap->addMethod("fill",                        (q_method_t)QPIXMAP_fill);
   //QC_QPixmap->addMethod("handle",                      (q_method_t)QPIXMAP_handle);
   QC_QPixmap->addMethod("hasAlpha",                    (q_method_t)QPIXMAP_hasAlpha);
   QC_QPixmap->addMethod("hasAlphaChannel",             (q_method_t)QPIXMAP_hasAlphaChannel);
   QC_QPixmap->addMethod("height",                      (q_method_t)QPIXMAP_height);
   QC_QPixmap->addMethod("isNull",                      (q_method_t)QPIXMAP_isNull);
   QC_QPixmap->addMethod("isQBitmap",                   (q_method_t)QPIXMAP_isQBitmap);
   QC_QPixmap->addMethod("load",                        (q_method_t)QPIXMAP_load);
   //QC_QPixmap->addMethod("loadFromData",                (q_method_t)QPIXMAP_loadFromData);
   QC_QPixmap->addMethod("mask",                        (q_method_t)QPIXMAP_mask);
   QC_QPixmap->addMethod("rect",                        (q_method_t)QPIXMAP_rect);
   QC_QPixmap->addMethod("save",                        (q_method_t)QPIXMAP_save);
   QC_QPixmap->addMethod("scaled",                      (q_method_t)QPIXMAP_scaled);
   QC_QPixmap->addMethod("scaledToHeight",              (q_method_t)QPIXMAP_scaledToHeight);
   QC_QPixmap->addMethod("scaledToWidth",               (q_method_t)QPIXMAP_scaledToWidth);
   QC_QPixmap->addMethod("setAlphaChannel",             (q_method_t)QPIXMAP_setAlphaChannel);
   QC_QPixmap->addMethod("setMask",                     (q_method_t)QPIXMAP_setMask);
   QC_QPixmap->addMethod("size",                        (q_method_t)QPIXMAP_size);
   QC_QPixmap->addMethod("toImage",                     (q_method_t)QPIXMAP_toImage);
   //QC_QPixmap->addMethod("toMacCGImageRef",             (q_method_t)QPIXMAP_toMacCGImageRef);
   //QC_QPixmap->addMethod("toWinHBITMAP",                (q_method_t)QPIXMAP_toWinHBITMAP);
   //QC_QPixmap->addMethod("transformed",                 (q_method_t)QPIXMAP_transformed);
   QC_QPixmap->addMethod("width",                       (q_method_t)QPIXMAP_width);
   //QC_QPixmap->addMethod("x11Info",                     (q_method_t)QPIXMAP_x11Info);
   //QC_QPixmap->addMethod("x11PictureHandle",            (q_method_t)QPIXMAP_x11PictureHandle);


   traceout("initQPixmapClass()");
   return QC_QPixmap;
}

//int defaultDepth ()
static QoreNode *f_QPixmap_defaultDepth(const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)QPixmap::defaultDepth());
}

//QPixmap fromImage ( const QImage & image, Qt::ImageConversionFlags flags = Qt::AutoColor )
static QoreNode *f_QPixmap_fromImage(const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQImage *image = (p && p->type == NT_OBJECT) ? (QoreQImage *)p->val.object->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
   if (!image) {
      if (!xsink->isException())
         xsink->raiseException("QPIXMAP-FROMIMAGE-PARAM-ERROR", "expecting a QImage object as first argument to QPixmap::fromImage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
   p = get_param(params, 1);
   Qt::ImageConversionFlags flags = !is_nothing(p) ? (Qt::ImageConversionFlags)p->getAsInt() : Qt::AutoColor;
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(QPixmap::fromImage(*(static_cast<QImage *>(image)), flags));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//QPixmap grabWidget ( QWidget * widget, const QRect & rectangle )
//QPixmap grabWidget ( QWidget * widget, int x = 0, int y = 0, int width = -1, int height = -1 )
static QoreNode *f_QPixmap_grabWidget(const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
	 xsink->raiseException("QPIXMAP-GRABWIDGET-PARAM-ERROR", "QPixmap::grabWidget() requires a QWidget as the first argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (*xsink)
      return 0;
   if (!rectangle) {

      if (p && p->type == NT_OBJECT) {
	 xsink->raiseException("QPIXMAP-GRABWIDGET-PARAM-ERROR", "this version of QPixmap::grabWidget() expects an object derived from QRect as the second argument", p->val.object->getClass()->getName());
	 return 0;
      }

      int x = !is_nothing(p) ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int y = !is_nothing(p) ? p->getAsInt() : 0;
      p = get_param(params, 3);
      int width = !is_nothing(p) ? p->getAsInt() : -1;
      p = get_param(params, 4);
      int height = !is_nothing(p) ? p->getAsInt() : -1;

      QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
      QoreQPixmap *q_qp = new QoreQPixmap(QPixmap::grabWidget(static_cast<QWidget *>(widget->getQWidget()), x, y, width, height));
      o_qp->setPrivate(CID_QPIXMAP, q_qp);
      return new QoreNode(o_qp);
   }

   ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(QPixmap::grabWidget(static_cast<QWidget *>(widget->getQWidget()), *(static_cast<QRect *>(rectangle))));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//QPixmap grabWindow ( WId window, int x = 0, int y = 0, int width = -1, int height = -1 )
static QoreNode *f_QPixmap_grabWindow(const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   WId window = (WId)(p ? p->getAsBigInt() : 0);
   p = get_param(params, 1);
   int x = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int width = !is_nothing(p) ? p->getAsInt() : -1;
   p = get_param(params, 4);
   int height = !is_nothing(p) ? p->getAsInt() : -1;
   QoreQPixmap *q_qp = new QoreQPixmap(QPixmap::grabWindow(window, x, y, width, height));
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

void initQPixmapStaticFunctions()
{
   builtinFunctions.add("QPixmap_defaultDepth",                 f_QPixmap_defaultDepth);
   builtinFunctions.add("QPixmap_fromImage",                    f_QPixmap_fromImage);
   //builtinFunctions.add("QPixmap_fromMacCGImageRef",            f_QPixmap_fromMacCGImageRef);
   //builtinFunctions.add("QPixmap_fromWinHBITMAP",               f_QPixmap_fromWinHBITMAP);
   builtinFunctions.add("QPixmap_grabWidget",                   f_QPixmap_grabWidget);
   builtinFunctions.add("QPixmap_grabWindow",                   f_QPixmap_grabWindow);
   //builtinFunctions.add("QPixmap_trueMatrix",                   f_QPixmap_trueMatrix);
}
