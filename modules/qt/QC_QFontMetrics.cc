/*
 QC_QFontMetrics.cc
 
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

#include "QC_QFontMetrics.h"

int CID_QFONTMETRICS;
class QoreClass *QC_QFontMetrics = 0;

//QFontMetrics ( const QFont & font )
//QFontMetrics ( const QFont & font, QPaintDevice * paintdevice )
//QFontMetrics ( const QFontMetrics & fm )
static void QFONTMETRICS_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
	 xsink->raiseException("QFONTMETRICS-CONSTRUCTOR-PARAM-ERROR", "QFontMetrics::constructor() expects an object derived from QFont as the first argument");
      return;
   }
   ReferenceHolder<QoreQFont> fontHolder(font, xsink);

   p = test_param(params, NT_OBJECT, 1);
   AbstractPrivateData *apd = p ? p->val.object->getReferencedPrivateData(CID_QPAINTDEVICE, xsink) : 0;
   if (apd) {
      ReferenceHolder<AbstractPrivateData> holder(apd, xsink);
      QoreAbstractQPaintDevice *qpd = dynamic_cast<QoreAbstractQPaintDevice *>(apd);
      assert(qpd);
      //printd(5, "apd=%08p qpb=%08p\n", apd, qpd);
      self->setPrivate(CID_QFONTMETRICS, new QoreQFontMetrics(*(static_cast<QFont *>(font)), qpd->getQPaintDevice()));
   }
   else
      self->setPrivate(CID_QFONTMETRICS, new QoreQFontMetrics(*(static_cast<QFont *>(font))));
}

static void QFONTMETRICS_copy(class Object *self, class Object *old, class QoreQFontMetrics *qfm, ExceptionSink *xsink)
{
   xsink->raiseException("QFONTMETRICS-COPY-ERROR", "objects of this class cannot be copied");
}

//int ascent () const
static QoreNode *QFONTMETRICS_ascent(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->ascent());
}

//int averageCharWidth () const
static QoreNode *QFONTMETRICS_averageCharWidth(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->averageCharWidth());
}

////QRect boundingRect ( QChar ch ) const
////QRect boundingRect ( const QString & text ) const
////QRect boundingRect ( int x, int y, int width, int height, int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const
////QRect boundingRect ( const QRect & rect, int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const
//static QoreNode *QFONTMETRICS_boundingRect(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (p && p->type == NT_OBJECT) {
//      QoreQRect *rect = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
//      if (!rect) {
//         if (!xsink->isException())
//            xsink->raiseException("QFONTMETRICS-BOUNDINGRECT-PARAM-ERROR", "QFontMetrics::boundingRect() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
//         return 0;
//      }
//      ReferenceHolder<QoreQRect> rectHolder(rect, xsink);
//      p = get_param(params, 1);
//      int flags = p ? p->getAsInt() : 0;
//      p = get_param(params, 2);
//      if (!p || p->type != NT_STRING) {
//         xsink->raiseException("QFONTMETRICS-BOUNDINGRECT-PARAM-ERROR", "expecting a string as third argument to QFontMetrics::boundingRect()");
//         return 0;
//      }
//      const char *text = p->val.String->getBuffer();
//      p = get_param(params, 3);
//      int tabStops = p ? p->getAsInt() : 0;
//      p = get_param(params, 4);
//      ??? int* tabArray = p;
//      Object *o_qr = new Object(QC_QRect, getProgram());
//      QoreQRect *q_qr = new QoreQRect(qfm->boundingRect(*(static_cast<QRect *>(rect)), flags, text, tabStops, tabArray));
//      o_qr->setPrivate(CID_QRECT, q_qr);
//      return new QoreNode(o_qr);
//   }
//   if (p && p->type == NT_STRING) {
//      if (!p || p->type != NT_STRING) {
//         xsink->raiseException("QFONTMETRICS-BOUNDINGRECT-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::boundingRect()");
//         return 0;
//      }
//      const char *text = p->val.String->getBuffer();
//      Object *o_qr = new Object(QC_QRect, getProgram());
//      QoreQRect *q_qr = new QoreQRect(qfm->boundingRect(text));
//      o_qr->setPrivate(CID_QRECT, q_qr);
//      return new QoreNode(o_qr);
//   }
//   int x = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   int y = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   int width = p ? p->getAsInt() : 0;
//   p = get_param(params, 3);
//   int height = p ? p->getAsInt() : 0;
//   p = get_param(params, 4);
//   int flags = p ? p->getAsInt() : 0;
//   p = get_param(params, 5);
//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QFONTMETRICS-BOUNDINGRECT-PARAM-ERROR", "expecting a string as sixth argument to QFontMetrics::boundingRect()");
//      return 0;
//   }
//   const char *text = p->val.String->getBuffer();
//   p = get_param(params, 6);
//   int tabStops = p ? p->getAsInt() : 0;
//   p = get_param(params, 7);
//   ??? int* tabArray = p;
//   Object *o_qr = new Object(QC_QRect, getProgram());
//   QoreQRect *q_qr = new QoreQRect(qfm->boundingRect(x, y, width, height, flags, text, tabStops, tabArray));
//   o_qr->setPrivate(CID_QRECT, q_qr);
//   return new QoreNode(o_qr);
//}

//int charWidth ( const QString & text, int pos ) const
static QoreNode *QFONTMETRICS_charWidth(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTMETRICS-CHARWIDTH-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::charWidth()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   p = get_param(params, 1);
   int pos = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qfm->charWidth(text, pos));
}

//int descent () const
static QoreNode *QFONTMETRICS_descent(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->descent());
}

//QString elidedText ( const QString & text, Qt::TextElideMode mode, int width, int flags = 0 ) const
static QoreNode *QFONTMETRICS_elidedText(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTMETRICS-ELIDEDTEXT-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::elidedText()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   p = get_param(params, 1);
   Qt::TextElideMode mode = (Qt::TextElideMode)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int flags = p ? p->getAsInt() : 0;
   return new QoreNode(new QoreString(qfm->elidedText(text, mode, width, flags).toUtf8().data(), QCS_UTF8));
}

//int height () const
static QoreNode *QFONTMETRICS_height(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->height());
}

//bool inFont ( QChar ch ) const
static QoreNode *QFONTMETRICS_inFont(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QChar ch;
   if (get_qchar(p, ch, xsink))
      return 0;

   return new QoreNode(qfm->inFont(ch));
}

//int leading () const
static QoreNode *QFONTMETRICS_leading(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->leading());
}

//int leftBearing ( QChar ch ) const
static QoreNode *QFONTMETRICS_leftBearing(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTMETRICS-LEFTBEARING-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::leftBearing()");
      return 0;
   }
   const char ch = p->val.String->getBuffer()[0];
   return new QoreNode((int64)qfm->leftBearing(ch));
}

//int lineSpacing () const
static QoreNode *QFONTMETRICS_lineSpacing(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->lineSpacing());
}

//int lineWidth () const
static QoreNode *QFONTMETRICS_lineWidth(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->lineWidth());
}

//int maxWidth () const
static QoreNode *QFONTMETRICS_maxWidth(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->maxWidth());
}

//int minLeftBearing () const
static QoreNode *QFONTMETRICS_minLeftBearing(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->minLeftBearing());
}

//int minRightBearing () const
static QoreNode *QFONTMETRICS_minRightBearing(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->minRightBearing());
}

//int overlinePos () const
static QoreNode *QFONTMETRICS_overlinePos(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->overlinePos());
}

//int rightBearing ( QChar ch ) const
static QoreNode *QFONTMETRICS_rightBearing(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTMETRICS-RIGHTBEARING-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::rightBearing()");
      return 0;
   }
   const char ch = p->val.String->getBuffer()[0];
   return new QoreNode((int64)qfm->rightBearing(ch));
}

////QSize size ( int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const
//static QoreNode *QFONTMETRICS_size(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int flags = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QFONTMETRICS-SIZE-PARAM-ERROR", "expecting a string as second argument to QFontMetrics::size()");
//      return 0;
//   }
//   const char *text = p->val.String->getBuffer();
//   p = get_param(params, 2);
//   int tabStops = p ? p->getAsInt() : 0;
//   p = get_param(params, 3);
//   ??? int* tabArray = p;
//   Object *o_qs = new Object(QC_QSize, getProgram());
//   QoreQSize *q_qs = new QoreQSize(qfm->size(flags, text, tabStops, tabArray));
//   o_qs->setPrivate(CID_QSIZE, q_qs);
//   return new QoreNode(o_qs);
//}

//int strikeOutPos () const
static QoreNode *QFONTMETRICS_strikeOutPos(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->strikeOutPos());
}

//QRect tightBoundingRect ( const QString & text ) const
static QoreNode *QFONTMETRICS_tightBoundingRect(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTMETRICS-TIGHTBOUNDINGRECT-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::tightBoundingRect()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qfm->tightBoundingRect(text));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//int underlinePos () const
static QoreNode *QFONTMETRICS_underlinePos(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->underlinePos());
}

//int width ( const QString & text, int len = -1 ) const
//int width ( QChar ch ) const
static QoreNode *QFONTMETRICS_width(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_STRING) {
      const char *text = p->val.String->getBuffer();
      p = get_param(params, 1);
      return new QoreNode((int64)qfm->width(text, !is_nothing(p) ? p->getAsInt() : -1));
   }

   QChar ch;
   if (get_qchar(p, ch, xsink))
      return 0;

   return new QoreNode((int64)qfm->width(ch));
}

//int xHeight () const
static QoreNode *QFONTMETRICS_xHeight(Object *self, QoreQFontMetrics *qfm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfm->xHeight());
}

QoreClass *initQFontMetricsClass()
{
   QC_QFontMetrics = new QoreClass("QFontMetrics", QDOM_GUI);
   CID_QFONTMETRICS = QC_QFontMetrics->getID();

   QC_QFontMetrics->setConstructor(QFONTMETRICS_constructor);
   QC_QFontMetrics->setCopy((q_copy_t)QFONTMETRICS_copy);

   QC_QFontMetrics->addMethod("ascent",                      (q_method_t)QFONTMETRICS_ascent);
   QC_QFontMetrics->addMethod("averageCharWidth",            (q_method_t)QFONTMETRICS_averageCharWidth);
   //QC_QFontMetrics->addMethod("boundingRect",                (q_method_t)QFONTMETRICS_boundingRect);
   QC_QFontMetrics->addMethod("charWidth",                   (q_method_t)QFONTMETRICS_charWidth);
   QC_QFontMetrics->addMethod("descent",                     (q_method_t)QFONTMETRICS_descent);
   QC_QFontMetrics->addMethod("elidedText",                  (q_method_t)QFONTMETRICS_elidedText);
   QC_QFontMetrics->addMethod("height",                      (q_method_t)QFONTMETRICS_height);
   QC_QFontMetrics->addMethod("inFont",                      (q_method_t)QFONTMETRICS_inFont);
   QC_QFontMetrics->addMethod("leading",                     (q_method_t)QFONTMETRICS_leading);
   QC_QFontMetrics->addMethod("leftBearing",                 (q_method_t)QFONTMETRICS_leftBearing);
   QC_QFontMetrics->addMethod("lineSpacing",                 (q_method_t)QFONTMETRICS_lineSpacing);
   QC_QFontMetrics->addMethod("lineWidth",                   (q_method_t)QFONTMETRICS_lineWidth);
   QC_QFontMetrics->addMethod("maxWidth",                    (q_method_t)QFONTMETRICS_maxWidth);
   QC_QFontMetrics->addMethod("minLeftBearing",              (q_method_t)QFONTMETRICS_minLeftBearing);
   QC_QFontMetrics->addMethod("minRightBearing",             (q_method_t)QFONTMETRICS_minRightBearing);
   QC_QFontMetrics->addMethod("overlinePos",                 (q_method_t)QFONTMETRICS_overlinePos);
   QC_QFontMetrics->addMethod("rightBearing",                (q_method_t)QFONTMETRICS_rightBearing);
   //QC_QFontMetrics->addMethod("size",                        (q_method_t)QFONTMETRICS_size);
   QC_QFontMetrics->addMethod("strikeOutPos",                (q_method_t)QFONTMETRICS_strikeOutPos);
   QC_QFontMetrics->addMethod("tightBoundingRect",           (q_method_t)QFONTMETRICS_tightBoundingRect);
   QC_QFontMetrics->addMethod("underlinePos",                (q_method_t)QFONTMETRICS_underlinePos);
   QC_QFontMetrics->addMethod("width",                       (q_method_t)QFONTMETRICS_width);
   QC_QFontMetrics->addMethod("xHeight",                     (q_method_t)QFONTMETRICS_xHeight);

   return QC_QFontMetrics;
}
