/*
 QC_QFontInfo.cc
 
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

#include "QC_QFontInfo.h"

int CID_QFONTINFO;
class QoreClass *QC_QFontInfo = 0;

//QFontInfo ( const QFont & font )
//QFontInfo ( const QFontInfo & fi )
static void QFONTINFO_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QFONTINFO-QFONTINFO-PARAM-ERROR", "expecting a QFont object as first argument to QFontInfo::QFontInfo()");
      return;
   }
   ReferenceHolder<QoreQFont> fontHolder(font, xsink);
   self->setPrivate(CID_QFONTINFO, new QoreQFontInfo(*(static_cast<QFont *>(font))));
   return;
}

static void QFONTINFO_copy(class Object *self, class Object *old, class QoreQFontInfo *qfi, ExceptionSink *xsink)
{
   xsink->raiseException("QFONTINFO-COPY-ERROR", "objects of this class cannot be copied");
}

//bool bold () const
static QoreNode *QFONTINFO_bold(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qfi->bold());
}

//bool exactMatch () const
static QoreNode *QFONTINFO_exactMatch(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qfi->exactMatch());
}

//QString family () const
static QoreNode *QFONTINFO_family(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qfi->family().toUtf8().data(), QCS_UTF8));
}

//bool fixedPitch () const
static QoreNode *QFONTINFO_fixedPitch(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qfi->fixedPitch());
}

//bool italic () const
static QoreNode *QFONTINFO_italic(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qfi->italic());
}

//int pixelSize () const
static QoreNode *QFONTINFO_pixelSize(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfi->pixelSize());
}

//int pointSize () const
static QoreNode *QFONTINFO_pointSize(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfi->pointSize());
}

//qreal pointSizeF () const
static QoreNode *QFONTINFO_pointSizeF(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qfi->pointSizeF());
}

//bool rawMode () const
static QoreNode *QFONTINFO_rawMode(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qfi->rawMode());
}

//QFont::Style style () const
static QoreNode *QFONTINFO_style(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfi->style());
}

//QFont::StyleHint styleHint () const
static QoreNode *QFONTINFO_styleHint(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfi->styleHint());
}

//int weight () const
static QoreNode *QFONTINFO_weight(Object *self, QoreQFontInfo *qfi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfi->weight());
}

QoreClass *initQFontInfoClass()
{
   QC_QFontInfo = new QoreClass("QFontInfo", QDOM_GUI);
   CID_QFONTINFO = QC_QFontInfo->getID();

   QC_QFontInfo->setConstructor(QFONTINFO_constructor);
   QC_QFontInfo->setCopy((q_copy_t)QFONTINFO_copy);

   QC_QFontInfo->addMethod("bold",                        (q_method_t)QFONTINFO_bold);
   QC_QFontInfo->addMethod("exactMatch",                  (q_method_t)QFONTINFO_exactMatch);
   QC_QFontInfo->addMethod("family",                      (q_method_t)QFONTINFO_family);
   QC_QFontInfo->addMethod("fixedPitch",                  (q_method_t)QFONTINFO_fixedPitch);
   QC_QFontInfo->addMethod("italic",                      (q_method_t)QFONTINFO_italic);
   QC_QFontInfo->addMethod("pixelSize",                   (q_method_t)QFONTINFO_pixelSize);
   QC_QFontInfo->addMethod("pointSize",                   (q_method_t)QFONTINFO_pointSize);
   QC_QFontInfo->addMethod("pointSizeF",                  (q_method_t)QFONTINFO_pointSizeF);
   QC_QFontInfo->addMethod("rawMode",                     (q_method_t)QFONTINFO_rawMode);
   QC_QFontInfo->addMethod("style",                       (q_method_t)QFONTINFO_style);
   QC_QFontInfo->addMethod("styleHint",                   (q_method_t)QFONTINFO_styleHint);
   QC_QFontInfo->addMethod("weight",                      (q_method_t)QFONTINFO_weight);

   return QC_QFontInfo;
}
