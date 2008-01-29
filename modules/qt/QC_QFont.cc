/*
 QC_QFont.cc
 
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
#include "QC_QFont.h"
#include "qore-qt.h"

int CID_QFONT;
QoreClass *QC_QFont = 0;

static void QFONT_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *str = test_string_param(params, 0);
   if (!str) {
      self->setPrivate(CID_QFONT, new QoreQFont());
      return;
   }
   const char *fname = str->getBuffer();

   // get point size
   QoreNode *p = get_param(params, 1);
   int point_size = p ? p->getAsInt() : -1;

   // get weight
   p = get_param(params, 2);
   int weight = p ? p->getAsInt() : -1;
   
   // get italic flag
   p = get_param(params, 3);
   bool italic = p ? p->getAsBool() : false;

   QoreQFont *qf = new QoreQFont(fname, point_size, weight, italic);
   self->setPrivate(CID_QFONT, qf);
}

static void QFONT_copy(class QoreObject *self, class QoreObject *old, class QoreQFont *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QFONT, new QoreQFont(*qf));
}

//bool bold () const
static QoreNode *QFONT_bold(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->bold());
}

//QString defaultFamily () const
static QoreNode *QFONT_defaultFamily(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qf->defaultFamily().toUtf8().data(), QCS_UTF8);
}

//bool exactMatch () const
static QoreNode *QFONT_exactMatch(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->exactMatch());
}

//QString family () const
static QoreNode *QFONT_family(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qf->family().toUtf8().data(), QCS_UTF8);
}

//bool fixedPitch () const
static QoreNode *QFONT_fixedPitch(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->fixedPitch());
}

//FT_Face freetypeFace () const
//static QoreNode *QFONT_freetypeFace(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qf->freetypeFace());
//}

//bool fromString ( const QString & descrip )
static QoreNode *QFONT_fromString(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString descrip;
   if (get_qstring(p, descrip, xsink))
      return 0;

   return new QoreBoolNode(qf->fromString(descrip));
}

//HFONT handle () const
//static QoreNode *QFONT_handle(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qf->handle());
//}

//bool isCopyOf ( const QFont & f ) const
//static QoreNode *QFONT_isCopyOf(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QFont& f = p;
//   return new QoreBoolNode(qf->isCopyOf(f));
//}

//bool italic () const
static QoreNode *QFONT_italic(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->italic());
}

//bool kerning () const
static QoreNode *QFONT_kerning(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->kerning());
}

//QString key () const
static QoreNode *QFONT_key(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qf->key().toUtf8().data(), QCS_UTF8);
}

//QString lastResortFamily () const
static QoreNode *QFONT_lastResortFamily(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qf->lastResortFont().toUtf8().data(), QCS_UTF8);
}

//QString lastResortFont () const
static QoreNode *QFONT_lastResortFont(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qf->lastResortFont().toUtf8().data(), QCS_UTF8);
}

#ifdef Q_WS_MAC
//quint32 macFontID () const
static QoreNode *QFONT_macFontID(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->macFontID());
}
#endif

//bool overline () const
static QoreNode *QFONT_overline(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->overline());
}

//int pixelSize () const
static QoreNode *QFONT_pixelSize(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->pixelSize());
}

//int pointSize () const
static QoreNode *QFONT_pointSize(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->pointSize());
}

//qreal pointSizeF () const
static QoreNode *QFONT_pointSizeF(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qf->pointSizeF());
}

//bool rawMode () const
static QoreNode *QFONT_rawMode(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->rawMode());
}

//QString rawName () const
static QoreNode *QFONT_rawName(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qf->rawName().toUtf8().data(), QCS_UTF8);
}

//QFont resolve ( const QFont & other ) const
//static QoreNode *QFONT_resolve(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QFont& other = p;
//   ??? return new QoreBigIntNode(qf->resolve(other));
//}

//void setBold ( bool enable )
static QoreNode *QFONT_setBold(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setBold(enable);
   return 0;
}

//void setFamily ( const QString & family )
static QoreNode *QFONT_setFamily(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString family;
   if (get_qstring(p, family, xsink))
      return 0;

   qf->setFamily(family);
   return 0;
}

//void setFixedPitch ( bool enable )
static QoreNode *QFONT_setFixedPitch(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setFixedPitch(enable);
   return 0;
}

//void setItalic ( bool enable )
static QoreNode *QFONT_setItalic(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setItalic(enable);
   return 0;
}

//void setKerning ( bool enable )
static QoreNode *QFONT_setKerning(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setKerning(enable);
   return 0;
}

//void setOverline ( bool enable )
static QoreNode *QFONT_setOverline(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setOverline(enable);
   return 0;
}

//void setPixelSize ( int pixelSize )
static QoreNode *QFONT_setPixelSize(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int pixelSize = p ? p->getAsInt() : 0;
   qf->setPixelSize(pixelSize);
   return 0;
}

//void setPointSize ( int pointSize )
static QoreNode *QFONT_setPointSize(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int pointSize = p ? p->getAsInt() : 0;
   qf->setPointSize(pointSize);
   return 0;
}

//void setPointSizeF ( qreal pointSize )
static QoreNode *QFONT_setPointSizeF(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float pointSize = p ? p->getAsFloat() : 0;
   qf->setPointSizeF(pointSize);
   return 0;
}

//void setRawMode ( bool enable )
static QoreNode *QFONT_setRawMode(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setRawMode(enable);
   return 0;
}

//void setRawName ( const QString & name )
static QoreNode *QFONT_setRawName(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;

   qf->setRawName(name);
   return 0;
}

//void setStretch ( int factor )
static QoreNode *QFONT_setStretch(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int factor = p ? p->getAsInt() : 0;
   qf->setStretch(factor);
   return 0;
}

//void setStrikeOut ( bool enable )
static QoreNode *QFONT_setStrikeOut(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setStrikeOut(enable);
   return 0;
}

//void setStyle ( Style style )
static QoreNode *QFONT_setStyle(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFont::Style style = (QFont::Style)(p ? p->getAsInt() : 0);
   qf->setStyle(style);
   return 0;
}

//void setStyleHint ( StyleHint hint, StyleStrategy strategy = PreferDefault )
static QoreNode *QFONT_setStyleHint(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFont::StyleHint hint = (QFont::StyleHint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QFont::StyleStrategy strategy = (QFont::StyleStrategy)(p ? p->getAsInt() : 0);
   qf->setStyleHint(hint, strategy);
   return 0;
}

//void setStyleStrategy ( StyleStrategy s )
static QoreNode *QFONT_setStyleStrategy(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFont::StyleStrategy s = (QFont::StyleStrategy)(p ? p->getAsInt() : 0);
   qf->setStyleStrategy(s);
   return 0;
}

//void setUnderline ( bool enable )
static QoreNode *QFONT_setUnderline(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qf->setUnderline(enable);
   return 0;
}

//void setWeight ( int weight )
static QoreNode *QFONT_setWeight(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int weight = p ? p->getAsInt() : 0;
   qf->setWeight(weight);
   return 0;
}

//int stretch () const
static QoreNode *QFONT_stretch(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->stretch());
}

//bool strikeOut () const
static QoreNode *QFONT_strikeOut(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->strikeOut());
}

//Style style () const
static QoreNode *QFONT_style(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->style());
}

//StyleHint styleHint () const
static QoreNode *QFONT_styleHint(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->styleHint());
}

//StyleStrategy styleStrategy () const
static QoreNode *QFONT_styleStrategy(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->styleStrategy());
}

//QString toString () const
static QoreNode *QFONT_toString(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qf->toString().toUtf8().data(), QCS_UTF8);
}

//bool underline () const
static QoreNode *QFONT_underline(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qf->underline());
}

//int weight () const
static QoreNode *QFONT_weight(QoreObject *self, QoreQFont *qf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qf->weight());
}

class QoreClass *initQFontClass()
{
   tracein("initQFontClass()");
   
   QC_QFont = new QoreClass("QFont", QDOM_GUI);
   CID_QFONT = QC_QFont->getID();
   QC_QFont->setConstructor(QFONT_constructor);
   QC_QFont->setCopy((q_copy_t)QFONT_copy);

   QC_QFont->addMethod("bold",                        (q_method_t)QFONT_bold);
   QC_QFont->addMethod("defaultFamily",               (q_method_t)QFONT_defaultFamily);
   QC_QFont->addMethod("exactMatch",                  (q_method_t)QFONT_exactMatch);
   QC_QFont->addMethod("family",                      (q_method_t)QFONT_family);
   QC_QFont->addMethod("fixedPitch",                  (q_method_t)QFONT_fixedPitch);
   //QC_QFont->addMethod("freetypeFace",                (q_method_t)QFONT_freetypeFace);
   QC_QFont->addMethod("fromString",                  (q_method_t)QFONT_fromString);
   //QC_QFont->addMethod("handle",                      (q_method_t)QFONT_handle);
   //QC_QFont->addMethod("isCopyOf",                    (q_method_t)QFONT_isCopyOf);
   QC_QFont->addMethod("italic",                      (q_method_t)QFONT_italic);
   QC_QFont->addMethod("kerning",                     (q_method_t)QFONT_kerning);
   QC_QFont->addMethod("key",                         (q_method_t)QFONT_key);
   QC_QFont->addMethod("lastResortFamily",            (q_method_t)QFONT_lastResortFamily);
   QC_QFont->addMethod("lastResortFont",              (q_method_t)QFONT_lastResortFont);
#ifdef Q_WS_MAC
   QC_QFont->addMethod("macFontID",                   (q_method_t)QFONT_macFontID);
#endif
   QC_QFont->addMethod("overline",                    (q_method_t)QFONT_overline);
   QC_QFont->addMethod("pixelSize",                   (q_method_t)QFONT_pixelSize);
   QC_QFont->addMethod("pointSize",                   (q_method_t)QFONT_pointSize);
   QC_QFont->addMethod("pointSizeF",                  (q_method_t)QFONT_pointSizeF);
   QC_QFont->addMethod("rawMode",                     (q_method_t)QFONT_rawMode);
   QC_QFont->addMethod("rawName",                     (q_method_t)QFONT_rawName);
   //QC_QFont->addMethod("resolve",                     (q_method_t)QFONT_resolve);
   QC_QFont->addMethod("setBold",                     (q_method_t)QFONT_setBold);
   QC_QFont->addMethod("setFamily",                   (q_method_t)QFONT_setFamily);
   QC_QFont->addMethod("setFixedPitch",               (q_method_t)QFONT_setFixedPitch);
   QC_QFont->addMethod("setItalic",                   (q_method_t)QFONT_setItalic);
   QC_QFont->addMethod("setKerning",                  (q_method_t)QFONT_setKerning);
   QC_QFont->addMethod("setOverline",                 (q_method_t)QFONT_setOverline);
   QC_QFont->addMethod("setPixelSize",                (q_method_t)QFONT_setPixelSize);
   QC_QFont->addMethod("setPointSize",                (q_method_t)QFONT_setPointSize);
   QC_QFont->addMethod("setPointSizeF",               (q_method_t)QFONT_setPointSizeF);
   QC_QFont->addMethod("setRawMode",                  (q_method_t)QFONT_setRawMode);
   QC_QFont->addMethod("setRawName",                  (q_method_t)QFONT_setRawName);
   QC_QFont->addMethod("setStretch",                  (q_method_t)QFONT_setStretch);
   QC_QFont->addMethod("setStrikeOut",                (q_method_t)QFONT_setStrikeOut);
   QC_QFont->addMethod("setStyle",                    (q_method_t)QFONT_setStyle);
   QC_QFont->addMethod("setStyleHint",                (q_method_t)QFONT_setStyleHint);
   QC_QFont->addMethod("setStyleStrategy",            (q_method_t)QFONT_setStyleStrategy);
   QC_QFont->addMethod("setUnderline",                (q_method_t)QFONT_setUnderline);
   QC_QFont->addMethod("setWeight",                   (q_method_t)QFONT_setWeight);
   QC_QFont->addMethod("stretch",                     (q_method_t)QFONT_stretch);
   QC_QFont->addMethod("strikeOut",                   (q_method_t)QFONT_strikeOut);
   QC_QFont->addMethod("style",                       (q_method_t)QFONT_style);
   QC_QFont->addMethod("styleHint",                   (q_method_t)QFONT_styleHint);
   QC_QFont->addMethod("styleStrategy",               (q_method_t)QFONT_styleStrategy);
   QC_QFont->addMethod("toString",                    (q_method_t)QFONT_toString);
   QC_QFont->addMethod("underline",                   (q_method_t)QFONT_underline);
   QC_QFont->addMethod("weight",                      (q_method_t)QFONT_weight);

   traceout("initQFontClass()");
   return QC_QFont;
}
