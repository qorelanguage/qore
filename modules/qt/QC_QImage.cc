/*
 QC_QImage.cc
 
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

#include "QC_QImage.h"

int CID_QIMAGE;
QoreClass *QC_QImage = 0;

static void QIMAGE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQImage *qp;
   QoreNode *p = get_param(params, 0);

/*
   if (p && p->type == NT_BINARY)
      qp = new QoreQImage(p->val.bin);
*/
   if (p && p->type == NT_STRING) {
      const char *filename = p->val.String->getBuffer();

      p = get_param(params, 1);
      const char *format = p ? p->val.String->getBuffer() : 0;

      qp = new QoreQImage(filename, format);
   }
   else {
      int w = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int h = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      QImage::Format format = (QImage::Format)(p ? p->getAsInt() : 0);

      qp = new QoreQImage(w, h, format);
   }

   self->setPrivate(CID_QIMAGE, qp);
}

static void QIMAGE_copy(class Object *self, class Object *old, class QoreQImage *qlcdn, ExceptionSink *xsink)
{
   xsink->raiseException("QIMAGE-COPY-ERROR", "objects of this class cannot be copied");
}

//bool allGray () const
static QoreNode *QIMAGE_allGray(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qi->allGray());
}

//QImage alphaChannel () const
static QoreNode *QIMAGE_alphaChannel(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->alphaChannel());
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//uchar * bits ()
//const uchar * bits () const
//static QoreNode *QIMAGE_bits(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//}

//int bytesPerLine () const
static QoreNode *QIMAGE_bytesPerLine(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->bytesPerLine());
}

//qint64 cacheKey () const
static QoreNode *QIMAGE_cacheKey(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->cacheKey());
}

//QRgb color ( int i ) const
static QoreNode *QIMAGE_color(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int i = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qi->color(i));
}

//QVector<QRgb> colorTable () const
//static QoreNode *QIMAGE_colorTable(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qi->colorTable());
//}

//QImage convertToFormat ( Format format, Qt::ImageConversionFlags flags = Qt::AutoColor ) const
//QImage convertToFormat ( Format format, const QVector<QRgb> & colorTable, Qt::ImageConversionFlags flags = Qt::AutoColor ) const
//static QoreNode *QIMAGE_convertToFormat(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QImage::Format format = (QImage::Format)(p ? p->getAsInt() : 0);
//   p = get_param(params, 1);
//   if (p && p->type == NT_???) {
//      ??? QVector<QRgb> colorTable = p;
//   p = get_param(params, 2);
//   }
//   Qt::ImageConversionFlags flags = (Qt::ImageConversionFlags)(p ? p->getAsInt() : 0);
//   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
//   QoreQImage *q_qi = new QoreQImage(qi->convertToFormat(format, flags));
//   o_qi->setPrivate(CID_QIMAGE, q_qi);
//   return new QoreNode(o_qi);
//}

//QImage copy ( const QRect & rectangle = QRect() ) const
//QImage copy ( int x, int y, int width, int height ) const
static QoreNode *QIMAGE_QT_copy(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRect *rectangle = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rectangle) {
         if (!xsink->isException())
            xsink->raiseException("QIMAGE-COPY-PARAM-ERROR", "QImage::copy() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
      Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
      QoreQImage *q_qi = new QoreQImage(qi->copy(*(static_cast<QRect *>(rectangle))));
      o_qi->setPrivate(CID_QIMAGE, q_qi);
      return new QoreNode(o_qi);
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->copy(x, y, width, height));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//QImage createAlphaMask ( Qt::ImageConversionFlags flags = Qt::AutoColor ) const
static QoreNode *QIMAGE_createAlphaMask(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ImageConversionFlags flags = (Qt::ImageConversionFlags)(p ? p->getAsInt() : 0);
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->createAlphaMask(flags));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//QImage createHeuristicMask ( bool clipTight = true ) const
static QoreNode *QIMAGE_createHeuristicMask(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool clipTight = !is_nothing(p) ? p->getAsBool() : true;
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->createHeuristicMask(clipTight));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//QImage createMaskFromColor ( QRgb color, Qt::MaskMode mode = Qt::MaskInColor ) const
static QoreNode *QIMAGE_createMaskFromColor(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int64 color = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   Qt::MaskMode mode = (Qt::MaskMode)(p ? p->getAsInt() : 0);
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->createMaskFromColor(color, mode));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//DataPtr & data_ptr ()
//static QoreNode *QIMAGE_data_ptr(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qi->data_ptr());
//}

//int depth () const
static QoreNode *QIMAGE_depth(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->depth());
}

//int dotsPerMeterX () const
static QoreNode *QIMAGE_dotsPerMeterX(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->dotsPerMeterX());
}

//int dotsPerMeterY () const
static QoreNode *QIMAGE_dotsPerMeterY(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->dotsPerMeterY());
}

//void fill ( uint pixelValue )
static QoreNode *QIMAGE_fill(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   unsigned pixelValue = p ? p->getAsBigInt() : 0;
   qi->fill(pixelValue);
   return 0;
}

//Format format () const
static QoreNode *QIMAGE_format(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->format());
}

//bool hasAlphaChannel () const
static QoreNode *QIMAGE_hasAlphaChannel(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qi->hasAlphaChannel());
}

//int height () const
static QoreNode *QIMAGE_height(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->height());
}

//void invertPixels ( InvertMode mode = InvertRgb )
static QoreNode *QIMAGE_invertPixels(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QImage::InvertMode mode = (QImage::InvertMode)(p ? p->getAsInt() : 0);
   qi->invertPixels(mode);
   return 0;
}

//bool isGrayscale () const
static QoreNode *QIMAGE_isGrayscale(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qi->isGrayscale());
}

//bool isNull () const
static QoreNode *QIMAGE_isNull(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qi->isNull());
}

//bool load ( const QString & fileName, const char * format = 0 )
//bool load ( QIODevice * device, const char * format )
static QoreNode *QIMAGE_load(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QIMAGE-LOAD-PARAM-ERROR", "expecting a string as first argument to QImage::load()");
      return 0;
   }
   const char *fileName = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *format = p ? p->val.String->getBuffer() : 0;
   return new QoreNode(qi->load(fileName, format));
}

//bool loadFromData ( const uchar * data, int len, const char * format = 0 )
//bool loadFromData ( const QByteArray & data, const char * format = 0 )
static QoreNode *QIMAGE_loadFromData(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_BINARY, 0);
   if (!p) {
      xsink->raiseException("QIMAGE-LOAD-FROM-DATA", "expected a binary object as sole argument to QImage::loadFromData()");
      return 0;
   }
   BinaryObject *data = p->val.bin;

   const char *format;

   p = test_param(params, NT_STRING, 1);
   format = p ? p->val.String->getBuffer() : 0;

   return new QoreNode(qi->loadFromData((const uchar *)data->getPtr(), data->size(), format));
}

//QImage mirrored ( bool horizontal = false, bool vertical = true ) const
static QoreNode *QIMAGE_mirrored(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool horizontal = p ? p->getAsBool() : false;
   p = get_param(params, 1);
   bool vertical = !is_nothing(p) ? p->getAsBool() : true;
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->mirrored(horizontal, vertical));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//int numBytes () const
static QoreNode *QIMAGE_numBytes(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->numBytes());
}

//int numColors () const
static QoreNode *QIMAGE_numColors(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->numColors());
}

//QPoint offset () const
static QoreNode *QIMAGE_offset(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qi->offset());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QRgb pixel ( const QPoint & position ) const
//QRgb pixel ( int x, int y ) const
static QoreNode *QIMAGE_pixel(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *position = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!position) {
         if (!xsink->isException())
            xsink->raiseException("QIMAGE-PIXEL-PARAM-ERROR", "QImage::pixel() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQPoint> positionHolder(position, xsink);
      return new QoreNode((int64)qi->pixel(*(static_cast<QPoint *>(position))));
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qi->pixel(x, y));
}

//int pixelIndex ( const QPoint & position ) const
//int pixelIndex ( int x, int y ) const
static QoreNode *QIMAGE_pixelIndex(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *position = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!position) {
         if (!xsink->isException())
            xsink->raiseException("QIMAGE-PIXELINDEX-PARAM-ERROR", "QImage::pixelIndex() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQPoint> positionHolder(position, xsink);
      return new QoreNode((int64)qi->pixelIndex(*(static_cast<QPoint *>(position))));
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qi->pixelIndex(x, y));
}

//QRect rect () const
static QoreNode *QIMAGE_rect(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qi->rect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//QImage rgbSwapped () const
static QoreNode *QIMAGE_rgbSwapped(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->rgbSwapped());
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//bool save ( const QString & fileName, const char * format = 0, int quality = -1 ) const
//bool save ( QIODevice * device, const char * format = 0, int quality = -1 ) const
static QoreNode *QIMAGE_save(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QIMAGE-SAVE-PARAM-ERROR", "expecting a string as first argument to QImage::save()");
      return 0;
   }
   const char *fileName = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *format = p ? p->val.String->getBuffer() : 0;
   p = get_param(params, 2);
   int quality = !is_nothing(p) ? p->getAsInt() : -1;
   return new QoreNode(qi->save(fileName, format, quality));
}

//QImage scaled ( const QSize & size, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, Qt::TransformationMode transformMode = Qt::FastTransformation ) const
//QImage scaled ( int width, int height, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, Qt::TransformationMode transformMode = Qt::FastTransformation ) const
static QoreNode *QIMAGE_scaled(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   Qt::AspectRatioMode aspectRatioMode = (Qt::AspectRatioMode)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   Qt::TransformationMode transformMode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->scaled(width, height, aspectRatioMode, transformMode));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//QImage scaledToHeight ( int height, Qt::TransformationMode mode = Qt::FastTransformation ) const
static QoreNode *QIMAGE_scaledToHeight(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::TransformationMode mode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->scaledToHeight(height, mode));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//QImage scaledToWidth ( int width, Qt::TransformationMode mode = Qt::FastTransformation ) const
static QoreNode *QIMAGE_scaledToWidth(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::TransformationMode mode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
   QoreQImage *q_qi = new QoreQImage(qi->scaledToWidth(width, mode));
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//uchar * scanLine ( int i )
//const uchar * scanLine ( int i ) const
//static QoreNode *QIMAGE_scanLine(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int i = p ? p->getAsInt() : 0;
//   ??? return qi->scanLine(i);
//}

//void setAlphaChannel ( const QImage & alphaChannel )
static QoreNode *QIMAGE_setAlphaChannel(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQImage *alphaChannel = (p && p->type == NT_OBJECT) ? (QoreQImage *)p->val.object->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
   if (!alphaChannel) {
      if (!xsink->isException())
         xsink->raiseException("QIMAGE-SETALPHACHANNEL-PARAM-ERROR", "expecting a QImage object as first argument to QImage::setAlphaChannel()");
      return 0;
   }
   ReferenceHolder<QoreQImage> holder(alphaChannel, xsink);
   qi->setAlphaChannel(*(static_cast<QImage *>(alphaChannel)));
   return 0;
}

//void setColor ( int index, QRgb colorValue )
static QoreNode *QIMAGE_setColor(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int64 colorValue = p ? p->getAsBigInt() : 0;
   qi->setColor(index, colorValue);
   return 0;
}

//void setColorTable ( const QVector<QRgb> colors )
//static QoreNode *QIMAGE_setColorTable(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QImage::QVector<QRgb> colors = (QImage::QVector<QRgb>)(p ? p->getAsInt() : 0);
//   qi->setColorTable(colors);
//   return 0;
//}

//void setDotsPerMeterX ( int x )
static QoreNode *QIMAGE_setDotsPerMeterX(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qi->setDotsPerMeterX(x);
   return 0;
}

//void setDotsPerMeterY ( int y )
static QoreNode *QIMAGE_setDotsPerMeterY(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qi->setDotsPerMeterY(y);
   return 0;
}

//void setNumColors ( int numColors )
static QoreNode *QIMAGE_setNumColors(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int numColors = p ? p->getAsInt() : 0;
   qi->setNumColors(numColors);
   return 0;
}

//void setOffset ( const QPoint & offset )
static QoreNode *QIMAGE_setOffset(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *offset = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!offset) {
      if (!xsink->isException())
         xsink->raiseException("QIMAGE-SETOFFSET-PARAM-ERROR", "expecting a QPoint object as first argument to QImage::setOffset()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> holder(offset, xsink);
   qi->setOffset(*(static_cast<QPoint *>(offset)));
   return 0;
}

//void setPixel ( const QPoint & position, uint index_or_rgb )
//void setPixel ( int x, int y, uint index_or_rgb )
static QoreNode *QIMAGE_setPixel(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *position = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!position) {
         if (!xsink->isException())
            xsink->raiseException("QIMAGE-SETPIXEL-PARAM-ERROR", "QImage::setPixel() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQPoint> positionHolder(position, xsink);
      p = get_param(params, 1);
      unsigned index_or_rgb = p ? p->getAsBigInt() : 0;
      qi->setPixel(*(static_cast<QPoint *>(position)), index_or_rgb);
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   unsigned index_or_rgb = p ? p->getAsBigInt() : 0;
   qi->setPixel(x, y, index_or_rgb);
   return 0;
}

//void setText ( const QString & key, const QString & text )
static QoreNode *QIMAGE_setText(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QIMAGE-SETTEXT-PARAM-ERROR", "expecting a string as first argument to QImage::setText()");
      return 0;
   }
   const char *key = p->val.String->getBuffer();
   p = get_param(params, 1);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QIMAGE-SETTEXT-PARAM-ERROR", "expecting a string as second argument to QImage::setText()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   qi->setText(key, text);
   return 0;
}

//QSize size () const
//static QoreNode *QIMAGE_size(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qi->size());
//}

//QString text ( const QString & key = QString() ) const
static QoreNode *QIMAGE_text(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   const char *key = p ? p->val.String->getBuffer() : "";
   return new QoreNode(new QoreString(qi->text(key).toUtf8().data(), QCS_UTF8));
}

//QStringList textKeys () const
//static QoreNode *QIMAGE_textKeys(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qi->textKeys());
//}

//QImage transformed ( const QMatrix & matrix, Qt::TransformationMode mode = Qt::FastTransformation ) const
//QImage transformed ( const QTransform & matrix, Qt::TransformationMode mode = Qt::FastTransformation ) const
//static QoreNode *QIMAGE_transformed(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QMatrix matrix = p;
//   p = get_param(params, 1);
//   Qt::TransformationMode mode = (Qt::TransformationMode)(p ? p->getAsInt() : 0);
//   Object *o_qi = new Object(self->getClass(CID_QIMAGE), getProgram());
//   QoreQImage *q_qi = new QoreQImage(qi->transformed(matrix, mode));
//   o_qi->setPrivate(CID_QIMAGE, q_qi);
//   return new QoreNode(o_qi);
//}

//bool valid ( const QPoint & pos ) const
//bool valid ( int x, int y ) const
static QoreNode *QIMAGE_valid(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *pos = (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!pos) {
         if (!xsink->isException())
            xsink->raiseException("QIMAGE-VALID-PARAM-ERROR", "QImage::valid() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
      return new QoreNode(qi->valid(*(static_cast<QPoint *>(pos))));
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   return new QoreNode(qi->valid(x, y));
}

//int width () const
static QoreNode *QIMAGE_width(Object *self, QoreQImage *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->width());
}

class QoreClass *initQImageClass(class QoreClass *qpaintdevice)
{
   tracein("initQImageClass()");
   
   QC_QImage = new QoreClass("QImage", QDOM_GUI);
   CID_QIMAGE = QC_QImage->getID();

   QC_QImage->addBuiltinVirtualBaseClass(qpaintdevice);

   QC_QImage->setConstructor(QIMAGE_constructor);
   QC_QImage->setCopy((q_copy_t)QIMAGE_copy);

   QC_QImage->addMethod("allGray",                     (q_method_t)QIMAGE_allGray);
   QC_QImage->addMethod("alphaChannel",                (q_method_t)QIMAGE_alphaChannel);
   //QC_QImage->addMethod("bits",                        (q_method_t)QIMAGE_bits);
   QC_QImage->addMethod("bytesPerLine",                (q_method_t)QIMAGE_bytesPerLine);
   QC_QImage->addMethod("cacheKey",                    (q_method_t)QIMAGE_cacheKey);
   QC_QImage->addMethod("color",                       (q_method_t)QIMAGE_color);
   //QC_QImage->addMethod("colorTable",                  (q_method_t)QIMAGE_colorTable);
   //QC_QImage->addMethod("convertToFormat",             (q_method_t)QIMAGE_convertToFormat);
   QC_QImage->addMethod("qt_copy",                     (q_method_t)QIMAGE_QT_copy);
   QC_QImage->addMethod("createAlphaMask",             (q_method_t)QIMAGE_createAlphaMask);
   QC_QImage->addMethod("createHeuristicMask",         (q_method_t)QIMAGE_createHeuristicMask);
   QC_QImage->addMethod("createMaskFromColor",         (q_method_t)QIMAGE_createMaskFromColor);
   //QC_QImage->addMethod("data_ptr",                    (q_method_t)QIMAGE_data_ptr);
   QC_QImage->addMethod("depth",                       (q_method_t)QIMAGE_depth);
   QC_QImage->addMethod("dotsPerMeterX",               (q_method_t)QIMAGE_dotsPerMeterX);
   QC_QImage->addMethod("dotsPerMeterY",               (q_method_t)QIMAGE_dotsPerMeterY);
   QC_QImage->addMethod("fill",                        (q_method_t)QIMAGE_fill);
   QC_QImage->addMethod("format",                      (q_method_t)QIMAGE_format);
   QC_QImage->addMethod("hasAlphaChannel",             (q_method_t)QIMAGE_hasAlphaChannel);
   QC_QImage->addMethod("height",                      (q_method_t)QIMAGE_height);
   QC_QImage->addMethod("invertPixels",                (q_method_t)QIMAGE_invertPixels);
   QC_QImage->addMethod("isGrayscale",                 (q_method_t)QIMAGE_isGrayscale);
   QC_QImage->addMethod("isNull",                      (q_method_t)QIMAGE_isNull);
   QC_QImage->addMethod("load",                        (q_method_t)QIMAGE_load);
   QC_QImage->addMethod("loadFromData",                (q_method_t)QIMAGE_loadFromData);
   QC_QImage->addMethod("mirrored",                    (q_method_t)QIMAGE_mirrored);
   QC_QImage->addMethod("numBytes",                    (q_method_t)QIMAGE_numBytes);
   QC_QImage->addMethod("numColors",                   (q_method_t)QIMAGE_numColors);
   QC_QImage->addMethod("offset",                      (q_method_t)QIMAGE_offset);
   QC_QImage->addMethod("pixel",                       (q_method_t)QIMAGE_pixel);
   QC_QImage->addMethod("pixelIndex",                  (q_method_t)QIMAGE_pixelIndex);
   QC_QImage->addMethod("rect",                        (q_method_t)QIMAGE_rect);
   QC_QImage->addMethod("rgbSwapped",                  (q_method_t)QIMAGE_rgbSwapped);
   QC_QImage->addMethod("save",                        (q_method_t)QIMAGE_save);
   QC_QImage->addMethod("scaled",                      (q_method_t)QIMAGE_scaled);
   QC_QImage->addMethod("scaledToHeight",              (q_method_t)QIMAGE_scaledToHeight);
   QC_QImage->addMethod("scaledToWidth",               (q_method_t)QIMAGE_scaledToWidth);
   //QC_QImage->addMethod("scanLine",                    (q_method_t)QIMAGE_scanLine);
   QC_QImage->addMethod("setAlphaChannel",             (q_method_t)QIMAGE_setAlphaChannel);
   QC_QImage->addMethod("setColor",                    (q_method_t)QIMAGE_setColor);
   //QC_QImage->addMethod("setColorTable",               (q_method_t)QIMAGE_setColorTable);
   QC_QImage->addMethod("setDotsPerMeterX",            (q_method_t)QIMAGE_setDotsPerMeterX);
   QC_QImage->addMethod("setDotsPerMeterY",            (q_method_t)QIMAGE_setDotsPerMeterY);
   QC_QImage->addMethod("setNumColors",                (q_method_t)QIMAGE_setNumColors);
   QC_QImage->addMethod("setOffset",                   (q_method_t)QIMAGE_setOffset);
   QC_QImage->addMethod("setPixel",                    (q_method_t)QIMAGE_setPixel);
   QC_QImage->addMethod("setText",                     (q_method_t)QIMAGE_setText);
   //QC_QImage->addMethod("size",                        (q_method_t)QIMAGE_size);
   QC_QImage->addMethod("text",                        (q_method_t)QIMAGE_text);
   //QC_QImage->addMethod("textKeys",                    (q_method_t)QIMAGE_textKeys);
   //QC_QImage->addMethod("transformed",                 (q_method_t)QIMAGE_transformed);
   QC_QImage->addMethod("valid",                       (q_method_t)QIMAGE_valid);
   QC_QImage->addMethod("width",                       (q_method_t)QIMAGE_width);

   traceout("initQImageClass()");
   return QC_QImage;
}
