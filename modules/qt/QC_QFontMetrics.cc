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
#include "QC_QFont.h"
#include "QC_QPaintDevice.h"
#include "QC_QRect.h"

#include "qore-qt.h"


int CID_QFONTMETRICS;
class QoreClass *QC_QFontMetrics = 0;

//QFontMetrics ( const QFont & font )
//QFontMetrics ( const QFont & font, QPaintDevice * paintdevice )
//QFontMetrics ( const QFontMetrics & fm )
static void QFONTMETRICS_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQFont *font = o ? (QoreQFont *)o->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
	 xsink->raiseException("QFONTMETRICS-CONSTRUCTOR-PARAM-ERROR", "QFontMetrics::constructor() expects an object derived from QFont as the first argument");
      return;
   }
   ReferenceHolder<QoreQFont> fontHolder(font, xsink);

   o = test_object_param(params, 1);
   AbstractPrivateData *apd = o ? o->getReferencedPrivateData(CID_QPAINTDEVICE, xsink) : 0;
   if (*xsink)
      return;
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

static void QFONTMETRICS_copy(class QoreObject *self, class QoreObject *old, class QoreQFontMetrics *qfm, ExceptionSink *xsink)
{
   self->setPrivate(CID_QFONTMETRICS, new QoreQFontMetrics(*qfm));
}

//int ascent () const
static AbstractQoreNode *QFONTMETRICS_ascent(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->ascent());
}

//int averageCharWidth () const
static AbstractQoreNode *QFONTMETRICS_averageCharWidth(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->averageCharWidth());
}

////QRect boundingRect ( QChar ch ) const
////QRect boundingRect ( const QString & text ) const
////QRect boundingRect ( int x, int y, int width, int height, int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const
////QRect boundingRect ( const QRect & rect, int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const
//static AbstractQoreNode *QFONTMETRICS_boundingRect(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->type == NT_OBJECT) {
//      QoreQRect *rect = (QoreQRect *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink);
//      if (!rect) {
//         if (!xsink->isException())
//            xsink->raiseException("QFONTMETRICS-BOUNDINGRECT-PARAM-ERROR", "QFontMetrics::boundingRect() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
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
//      const char *text = p->getBuffer();
//      p = get_param(params, 3);
//      int tabStops = p ? p->getAsInt() : 0;
//      p = get_param(params, 4);
//      ??? int* tabArray = p;
//      QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
//      QoreQRect *q_qr = new QoreQRect(qfm->boundingRect(*(static_cast<QRect *>(rect)), flags, text, tabStops, tabArray));
//      o_qr->setPrivate(CID_QRECT, q_qr);
//      return o_qr;
//   }
//   if (p && p->type == NT_STRING) {
//      if (!p || p->type != NT_STRING) {
//         xsink->raiseException("QFONTMETRICS-BOUNDINGRECT-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::boundingRect()");
//         return 0;
//      }
//      const char *text = p->getBuffer();
//      QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
//      QoreQRect *q_qr = new QoreQRect(qfm->boundingRect(text));
//      o_qr->setPrivate(CID_QRECT, q_qr);
//      return o_qr;
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
//   const char *text = p->getBuffer();
//   p = get_param(params, 6);
//   int tabStops = p ? p->getAsInt() : 0;
//   p = get_param(params, 7);
//   ??? int* tabArray = p;
//   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
//   QoreQRect *q_qr = new QoreQRect(qfm->boundingRect(x, y, width, height, flags, text, tabStops, tabArray));
//   o_qr->setPrivate(CID_QRECT, q_qr);
//   return o_qr;
//}

//int charWidth ( const QString & text, int pos ) const
static AbstractQoreNode *QFONTMETRICS_charWidth(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr) {
      xsink->raiseException("QFONTMETRICS-CHARWIDTH-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::charWidth()");
      return 0;
   }
   const char *text = pstr->getBuffer();

   const AbstractQoreNode *p = get_param(params, 1);
   int pos = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qfm->charWidth(text, pos));
}

//int descent () const
static AbstractQoreNode *QFONTMETRICS_descent(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->descent());
}

//QString elidedText ( const QString & text, Qt::TextElideMode mode, int width, int flags = 0 ) const
static AbstractQoreNode *QFONTMETRICS_elidedText(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr) {
      xsink->raiseException("QFONTMETRICS-ELIDEDTEXT-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::elidedText()");
      return 0;
   }
   const char *text = pstr->getBuffer();

   const AbstractQoreNode *p = get_param(params, 1);
   Qt::TextElideMode mode = (Qt::TextElideMode)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int flags = p ? p->getAsInt() : 0;
   return new QoreStringNode(qfm->elidedText(text, mode, width, flags).toUtf8().data(), QCS_UTF8);
}

//int height () const
static AbstractQoreNode *QFONTMETRICS_height(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->height());
}

//bool inFont ( QChar ch ) const
static AbstractQoreNode *QFONTMETRICS_inFont(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QChar ch;
   if (get_qchar(p, ch, xsink))
      return 0;

   return new QoreBoolNode(qfm->inFont(ch));
}

//int leading () const
static AbstractQoreNode *QFONTMETRICS_leading(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->leading());
}

//int leftBearing ( QChar ch ) const
static AbstractQoreNode *QFONTMETRICS_leftBearing(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QFONTMETRICS-LEFTBEARING-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::leftBearing()");
      return 0;
   }
   const char ch = p->getBuffer()[0];
   return new QoreBigIntNode(qfm->leftBearing(ch));
}

//int lineSpacing () const
static AbstractQoreNode *QFONTMETRICS_lineSpacing(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->lineSpacing());
}

//int lineWidth () const
static AbstractQoreNode *QFONTMETRICS_lineWidth(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->lineWidth());
}

//int maxWidth () const
static AbstractQoreNode *QFONTMETRICS_maxWidth(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->maxWidth());
}

//int minLeftBearing () const
static AbstractQoreNode *QFONTMETRICS_minLeftBearing(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->minLeftBearing());
}

//int minRightBearing () const
static AbstractQoreNode *QFONTMETRICS_minRightBearing(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->minRightBearing());
}

//int overlinePos () const
static AbstractQoreNode *QFONTMETRICS_overlinePos(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->overlinePos());
}

//int rightBearing ( QChar ch ) const
static AbstractQoreNode *QFONTMETRICS_rightBearing(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QFONTMETRICS-RIGHTBEARING-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::rightBearing()");
      return 0;
   }
   const char ch = p->getBuffer()[0];
   return new QoreBigIntNode(qfm->rightBearing(ch));
}

////QSize size ( int flags, const QString & text, int tabStops = 0, int * tabArray = 0 ) const
//static AbstractQoreNode *QFONTMETRICS_size(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   int flags = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QFONTMETRICS-SIZE-PARAM-ERROR", "expecting a string as second argument to QFontMetrics::size()");
//      return 0;
//   }
//   const char *text = p->getBuffer();
//   p = get_param(params, 2);
//   int tabStops = p ? p->getAsInt() : 0;
//   p = get_param(params, 3);
//   ??? int* tabArray = p;
//   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
//   QoreQSize *q_qs = new QoreQSize(qfm->size(flags, text, tabStops, tabArray));
//   o_qs->setPrivate(CID_QSIZE, q_qs);
//   return o_qs;
//}

//int strikeOutPos () const
static AbstractQoreNode *QFONTMETRICS_strikeOutPos(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->strikeOutPos());
}

//QRect tightBoundingRect ( const QString & text ) const
static AbstractQoreNode *QFONTMETRICS_tightBoundingRect(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QFONTMETRICS-TIGHTBOUNDINGRECT-PARAM-ERROR", "expecting a string as first argument to QFontMetrics::tightBoundingRect()");
      return 0;
   }
   const char *text = p->getBuffer();
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qfm->tightBoundingRect(text));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//int underlinePos () const
static AbstractQoreNode *QFONTMETRICS_underlinePos(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->underlinePos());
}

//int width ( const QString & text, int len = -1 ) const
//int width ( QChar ch ) const
static AbstractQoreNode *QFONTMETRICS_width(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   {
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(p);
      if (str) {
	 const char *text = str->getBuffer();
	 p = get_param(params, 1);
	 return new QoreBigIntNode(qfm->width(text, !is_nothing(p) ? p->getAsInt() : -1));
      }
   }

   QChar ch;
   if (get_qchar(p, ch, xsink))
      return 0;

   return new QoreBigIntNode(qfm->width(ch));
}

//int xHeight () const
static AbstractQoreNode *QFONTMETRICS_xHeight(QoreObject *self, QoreQFontMetrics *qfm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qfm->xHeight());
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
