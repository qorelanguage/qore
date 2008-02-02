/*
 QC_QTextCharFormat.cc
 
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

#include "QC_QTextCharFormat.h"
#include "QC_QPen.h"
#include "QC_QFont.h"
#include "QC_QColor.h"

#include "qore-qt.h"

int CID_QTEXTCHARFORMAT;
class QoreClass *QC_QTextCharFormat = 0;

//QTextCharFormat ()
static void QTEXTCHARFORMAT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTCHARFORMAT, new QoreQTextCharFormat());
   return;
}

static void QTEXTCHARFORMAT_copy(class QoreObject *self, class QoreObject *old, class QoreQTextCharFormat *qtcf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTCHARFORMAT, new QoreQTextCharFormat(*qtcf));
}

//QString anchorHref () const
static AbstractQoreNode *QTEXTCHARFORMAT_anchorHref(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtcf->anchorHref().toUtf8().data(), QCS_UTF8);
}

////QStringList anchorNames () const
//static AbstractQoreNode *QTEXTCHARFORMAT_anchorNames(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qtcf->anchorNames());
//}

//QFont font () const
static AbstractQoreNode *QTEXTCHARFORMAT_font(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qtcf->font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return o_qf;
}

//QString fontFamily () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontFamily(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtcf->fontFamily().toUtf8().data(), QCS_UTF8);
}

//bool fontFixedPitch () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontFixedPitch(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtcf->fontFixedPitch());
}

//bool fontItalic () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontItalic(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtcf->fontItalic());
}

//bool fontOverline () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontOverline(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtcf->fontOverline());
}

//qreal fontPointSize () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontPointSize(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtcf->fontPointSize());
}

//bool fontStrikeOut () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontStrikeOut(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtcf->fontStrikeOut());
}

//bool fontUnderline () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontUnderline(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtcf->fontUnderline());
}

//int fontWeight () const
static AbstractQoreNode *QTEXTCHARFORMAT_fontWeight(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtcf->fontWeight());
}

//bool isAnchor () const
static AbstractQoreNode *QTEXTCHARFORMAT_isAnchor(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtcf->isAnchor());
}

//bool isValid () const
static AbstractQoreNode *QTEXTCHARFORMAT_isValid(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtcf->isValid());
}

//void setAnchor ( bool anchor )
static AbstractQoreNode *QTEXTCHARFORMAT_setAnchor(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool anchor = p ? p->getAsBool() : false;
   qtcf->setAnchor(anchor);
   return 0;
}

//void setAnchorHref ( const QString & value )
static AbstractQoreNode *QTEXTCHARFORMAT_setAnchorHref(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QString value;

   if (get_qstring(p, value, xsink))
      return 0;

   qtcf->setAnchorHref(value);
   return 0;
}

////void setAnchorNames ( const QStringList & names )
//static AbstractQoreNode *QTEXTCHARFORMAT_setAnchorNames(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   ??? QStringList names = p;
//   qtcf->setAnchorNames(names);
//   return 0;
//}

//void setFont ( const QFont & font )
static AbstractQoreNode *QTEXTCHARFORMAT_setFont(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTCHARFORMAT-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QTextCharFormat::setFont()");
      return 0;
   }
   ReferenceHolder<QoreQFont> fontHolder(font, xsink);
   qtcf->setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setFontFamily ( const QString & family )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontFamily(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QString family;

   if (get_qstring(p, family, xsink))
      return 0;

   qtcf->setFontFamily(family);
   return 0;
}

//void setFontFixedPitch ( bool fixedPitch )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontFixedPitch(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool fixedPitch = p ? p->getAsBool() : false;
   qtcf->setFontFixedPitch(fixedPitch);
   return 0;
}

//void setFontItalic ( bool italic )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontItalic(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool italic = p ? p->getAsBool() : false;
   qtcf->setFontItalic(italic);
   return 0;
}

//void setFontOverline ( bool overline )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontOverline(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool overline = p ? p->getAsBool() : false;
   qtcf->setFontOverline(overline);
   return 0;
}

//void setFontPointSize ( qreal size )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontPointSize(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   qreal size = p ? p->getAsFloat() : 0.0;
   qtcf->setFontPointSize(size);
   return 0;
}

//void setFontStrikeOut ( bool strikeOut )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontStrikeOut(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool strikeOut = p ? p->getAsBool() : false;
   qtcf->setFontStrikeOut(strikeOut);
   return 0;
}

//void setFontUnderline ( bool underline )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontUnderline(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool underline = p ? p->getAsBool() : false;
   qtcf->setFontUnderline(underline);
   return 0;
}

//void setFontWeight ( int weight )
static AbstractQoreNode *QTEXTCHARFORMAT_setFontWeight(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int weight = p ? p->getAsInt() : 0;
   qtcf->setFontWeight(weight);
   return 0;
}

//void setTextOutline ( const QPen & pen )
static AbstractQoreNode *QTEXTCHARFORMAT_setTextOutline(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQPen *pen = (p && p->type == NT_OBJECT) ? (QoreQPen *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPEN, xsink) : 0;
   if (!pen) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTCHARFORMAT-SETTEXTOUTLINE-PARAM-ERROR", "expecting a QPen object as first argument to QTextCharFormat::setTextOutline()");
      return 0;
   }
   ReferenceHolder<QoreQPen> penHolder(pen, xsink);
   qtcf->setTextOutline(*(static_cast<QPen *>(pen)));
   return 0;
}

//void setToolTip ( const QString & text )
static AbstractQoreNode *QTEXTCHARFORMAT_setToolTip(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   qtcf->setToolTip(text);
   return 0;
}

//void setUnderlineColor ( const QColor & color )
static AbstractQoreNode *QTEXTCHARFORMAT_setUnderlineColor(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQColor *color = (p && p->type == NT_OBJECT) ? (QoreQColor *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!color) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTCHARFORMAT-SETUNDERLINECOLOR-PARAM-ERROR", "expecting a QColor object as first argument to QTextCharFormat::setUnderlineColor()");
      return 0;
   }
   ReferenceHolder<QoreQColor> colorHolder(color, xsink);
   qtcf->setUnderlineColor(*(static_cast<QColor *>(color)));
   return 0;
}

//void setUnderlineStyle ( UnderlineStyle style )
static AbstractQoreNode *QTEXTCHARFORMAT_setUnderlineStyle(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QTextCharFormat::UnderlineStyle style = (QTextCharFormat::UnderlineStyle)(p ? p->getAsInt() : 0);
   qtcf->setUnderlineStyle(style);
   return 0;
}

//void setVerticalAlignment ( VerticalAlignment alignment )
static AbstractQoreNode *QTEXTCHARFORMAT_setVerticalAlignment(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QTextCharFormat::VerticalAlignment alignment = (QTextCharFormat::VerticalAlignment)(p ? p->getAsInt() : 0);
   qtcf->setVerticalAlignment(alignment);
   return 0;
}

//QPen textOutline () const
static AbstractQoreNode *QTEXTCHARFORMAT_textOutline(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPen, getProgram());
   QoreQPen *q_qp = new QoreQPen(qtcf->textOutline());
   o_qp->setPrivate(CID_QPEN, q_qp);
   return o_qp;
}

//QString toolTip () const
static AbstractQoreNode *QTEXTCHARFORMAT_toolTip(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtcf->toolTip().toUtf8().data(), QCS_UTF8);
}

//QColor underlineColor () const
static AbstractQoreNode *QTEXTCHARFORMAT_underlineColor(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qc = new QoreObject(QC_QColor, getProgram());
   QoreQColor *q_qc = new QoreQColor(qtcf->underlineColor());
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return o_qc;
}

//UnderlineStyle underlineStyle () const
static AbstractQoreNode *QTEXTCHARFORMAT_underlineStyle(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtcf->underlineStyle());
}

//VerticalAlignment verticalAlignment () const
static AbstractQoreNode *QTEXTCHARFORMAT_verticalAlignment(QoreObject *self, QoreQTextCharFormat *qtcf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtcf->verticalAlignment());
}

QoreClass *initQTextCharFormatClass(QoreClass *qtextformat)
{
   QC_QTextCharFormat = new QoreClass("QTextCharFormat", QDOM_GUI);
   CID_QTEXTCHARFORMAT = QC_QTextCharFormat->getID();

   QC_QTextCharFormat->addBuiltinVirtualBaseClass(qtextformat);

   QC_QTextCharFormat->setConstructor(QTEXTCHARFORMAT_constructor);
   QC_QTextCharFormat->setCopy((q_copy_t)QTEXTCHARFORMAT_copy);

   QC_QTextCharFormat->addMethod("anchorHref",                  (q_method_t)QTEXTCHARFORMAT_anchorHref);
   //QC_QTextCharFormat->addMethod("anchorNames",                 (q_method_t)QTEXTCHARFORMAT_anchorNames);
   QC_QTextCharFormat->addMethod("font",                        (q_method_t)QTEXTCHARFORMAT_font);
   QC_QTextCharFormat->addMethod("fontFamily",                  (q_method_t)QTEXTCHARFORMAT_fontFamily);
   QC_QTextCharFormat->addMethod("fontFixedPitch",              (q_method_t)QTEXTCHARFORMAT_fontFixedPitch);
   QC_QTextCharFormat->addMethod("fontItalic",                  (q_method_t)QTEXTCHARFORMAT_fontItalic);
   QC_QTextCharFormat->addMethod("fontOverline",                (q_method_t)QTEXTCHARFORMAT_fontOverline);
   QC_QTextCharFormat->addMethod("fontPointSize",               (q_method_t)QTEXTCHARFORMAT_fontPointSize);
   QC_QTextCharFormat->addMethod("fontStrikeOut",               (q_method_t)QTEXTCHARFORMAT_fontStrikeOut);
   QC_QTextCharFormat->addMethod("fontUnderline",               (q_method_t)QTEXTCHARFORMAT_fontUnderline);
   QC_QTextCharFormat->addMethod("fontWeight",                  (q_method_t)QTEXTCHARFORMAT_fontWeight);
   QC_QTextCharFormat->addMethod("isAnchor",                    (q_method_t)QTEXTCHARFORMAT_isAnchor);
   QC_QTextCharFormat->addMethod("isValid",                     (q_method_t)QTEXTCHARFORMAT_isValid);
   QC_QTextCharFormat->addMethod("setAnchor",                   (q_method_t)QTEXTCHARFORMAT_setAnchor);
   QC_QTextCharFormat->addMethod("setAnchorHref",               (q_method_t)QTEXTCHARFORMAT_setAnchorHref);
   //QC_QTextCharFormat->addMethod("setAnchorNames",              (q_method_t)QTEXTCHARFORMAT_setAnchorNames);
   QC_QTextCharFormat->addMethod("setFont",                     (q_method_t)QTEXTCHARFORMAT_setFont);
   QC_QTextCharFormat->addMethod("setFontFamily",               (q_method_t)QTEXTCHARFORMAT_setFontFamily);
   QC_QTextCharFormat->addMethod("setFontFixedPitch",           (q_method_t)QTEXTCHARFORMAT_setFontFixedPitch);
   QC_QTextCharFormat->addMethod("setFontItalic",               (q_method_t)QTEXTCHARFORMAT_setFontItalic);
   QC_QTextCharFormat->addMethod("setFontOverline",             (q_method_t)QTEXTCHARFORMAT_setFontOverline);
   QC_QTextCharFormat->addMethod("setFontPointSize",            (q_method_t)QTEXTCHARFORMAT_setFontPointSize);
   QC_QTextCharFormat->addMethod("setFontStrikeOut",            (q_method_t)QTEXTCHARFORMAT_setFontStrikeOut);
   QC_QTextCharFormat->addMethod("setFontUnderline",            (q_method_t)QTEXTCHARFORMAT_setFontUnderline);
   QC_QTextCharFormat->addMethod("setFontWeight",               (q_method_t)QTEXTCHARFORMAT_setFontWeight);
   QC_QTextCharFormat->addMethod("setTextOutline",              (q_method_t)QTEXTCHARFORMAT_setTextOutline);
   QC_QTextCharFormat->addMethod("setToolTip",                  (q_method_t)QTEXTCHARFORMAT_setToolTip);
   QC_QTextCharFormat->addMethod("setUnderlineColor",           (q_method_t)QTEXTCHARFORMAT_setUnderlineColor);
   QC_QTextCharFormat->addMethod("setUnderlineStyle",           (q_method_t)QTEXTCHARFORMAT_setUnderlineStyle);
   QC_QTextCharFormat->addMethod("setVerticalAlignment",        (q_method_t)QTEXTCHARFORMAT_setVerticalAlignment);
   QC_QTextCharFormat->addMethod("textOutline",                 (q_method_t)QTEXTCHARFORMAT_textOutline);
   QC_QTextCharFormat->addMethod("toolTip",                     (q_method_t)QTEXTCHARFORMAT_toolTip);
   QC_QTextCharFormat->addMethod("underlineColor",              (q_method_t)QTEXTCHARFORMAT_underlineColor);
   QC_QTextCharFormat->addMethod("underlineStyle",              (q_method_t)QTEXTCHARFORMAT_underlineStyle);
   QC_QTextCharFormat->addMethod("verticalAlignment",           (q_method_t)QTEXTCHARFORMAT_verticalAlignment);

   return QC_QTextCharFormat;
}
