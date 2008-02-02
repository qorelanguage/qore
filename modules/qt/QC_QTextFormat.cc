/*
 QC_QTextFormat.cc
 
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

#include "QC_QTextFormat.h"
#include "QC_QBrush.h"
#include "QC_QColor.h"
#include "QC_QTextLength.h"
#include "QC_QPen.h"
#include "QC_QTextBlockFormat.h"
#include "QC_QTextCharFormat.h"
#include "QC_QTextFrameFormat.h"
#include "QC_QTextListFormat.h"
#include "QC_QTextImageFormat.h"
#include "QC_QTextTableFormat.h"

#include "qore-qt.h"

int CID_QTEXTFORMAT;
class QoreClass *QC_QTextFormat = 0;

//QTextFormat ()
//QTextFormat ( int type )
//QTextFormat ( const QTextFormat & other )
static void QTEXTFORMAT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTEXTFORMAT, new QoreQTextFormat());
      return;
   }
   int type = p ? p->getAsInt() : 0;
   self->setPrivate(CID_QTEXTFORMAT, new QoreQTextFormat(type));
   return;
}

static void QTEXTFORMAT_copy(class QoreObject *self, class QoreObject *old, class QoreQTextFormat *qtf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTFORMAT, new QoreQTextFormat(*qtf));
}

//QBrush background () const
static AbstractQoreNode *QTEXTFORMAT_background(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qtf->background());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//bool boolProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_boolProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   return new QoreBoolNode(qtf->boolProperty(propertyId));
}

//QBrush brushProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_brushProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qtf->brushProperty(propertyId));
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//void clearBackground ()
static AbstractQoreNode *QTEXTFORMAT_clearBackground(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   qtf->clearBackground();
   return 0;
}

//void clearForeground ()
static AbstractQoreNode *QTEXTFORMAT_clearForeground(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   qtf->clearForeground();
   return 0;
}

//void clearProperty ( int propertyId )
static AbstractQoreNode *QTEXTFORMAT_clearProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   qtf->clearProperty(propertyId);
   return 0;
}

//QColor colorProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_colorProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   QoreObject *o_qc = new QoreObject(QC_QColor, getProgram());
   QoreQColor *q_qc = new QoreQColor(qtf->colorProperty(propertyId));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return o_qc;
}

//qreal doubleProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_doubleProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   return new QoreFloatNode((double)qtf->doubleProperty(propertyId));
}

//QBrush foreground () const
static AbstractQoreNode *QTEXTFORMAT_foreground(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qtf->foreground());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//bool hasProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_hasProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   return new QoreBoolNode(qtf->hasProperty(propertyId));
}

//int intProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_intProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qtf->intProperty(propertyId));
}

//bool isBlockFormat () const
static AbstractQoreNode *QTEXTFORMAT_isBlockFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtf->isBlockFormat());
}

//bool isCharFormat () const
static AbstractQoreNode *QTEXTFORMAT_isCharFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtf->isCharFormat());
}

//bool isFrameFormat () const
static AbstractQoreNode *QTEXTFORMAT_isFrameFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtf->isFrameFormat());
}

//bool isImageFormat () const
static AbstractQoreNode *QTEXTFORMAT_isImageFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtf->isImageFormat());
}

//bool isListFormat () const
static AbstractQoreNode *QTEXTFORMAT_isListFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtf->isListFormat());
}

//bool isTableFormat () const
static AbstractQoreNode *QTEXTFORMAT_isTableFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtf->isTableFormat());
}

//bool isValid () const
static AbstractQoreNode *QTEXTFORMAT_isValid(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtf->isValid());
}

//Qt::LayoutDirection layoutDirection () const
static AbstractQoreNode *QTEXTFORMAT_layoutDirection(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtf->layoutDirection());
}

//QTextLength lengthProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_lengthProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   QoreObject *o_qtl = new QoreObject(QC_QTextLength, getProgram());
   QoreQTextLength *q_qtl = new QoreQTextLength(qtf->lengthProperty(propertyId));
   o_qtl->setPrivate(CID_QTEXTLENGTH, q_qtl);
   return o_qtl;
}

////QVector<QTextLength> lengthVectorProperty ( int propertyId ) const
//static AbstractQoreNode *QTEXTFORMAT_lengthVectorProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   int propertyId = p ? p->getAsInt() : 0;
//   ??? return new QoreBigIntNode(qtf->lengthVectorProperty(propertyId));
//}

//void merge ( const QTextFormat & other )
static AbstractQoreNode *QTEXTFORMAT_merge(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQTextFormat *other = (p && p->type == NT_OBJECT) ? (QoreQTextFormat *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QTEXTFORMAT, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTFORMAT-MERGE-PARAM-ERROR", "expecting a QTextFormat object as first argument to QTextFormat::merge()");
      return 0;
   }
   ReferenceHolder<QoreQTextFormat> otherHolder(other, xsink);
   qtf->merge(*(static_cast<QTextFormat *>(other)));
   return 0;
}

//int objectIndex () const
static AbstractQoreNode *QTEXTFORMAT_objectIndex(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtf->objectIndex());
}

//int objectType () const
static AbstractQoreNode *QTEXTFORMAT_objectType(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtf->objectType());
}

//QPen penProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_penProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   QoreObject *o_qp = new QoreObject(QC_QPen, getProgram());
   QoreQPen *q_qp = new QoreQPen(qtf->penProperty(propertyId));
   o_qp->setPrivate(CID_QPEN, q_qp);
   return o_qp;
}

////QMap<int, QVariant> properties () const
//static AbstractQoreNode *QTEXTFORMAT_properties(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qtf->properties());
//}

////QVariant property ( int propertyId ) const
//static AbstractQoreNode *QTEXTFORMAT_property(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   int propertyId = p ? p->getAsInt() : 0;
//   ??? return new QoreBigIntNode(qtf->property(propertyId));
//}

//int propertyCount () const
static AbstractQoreNode *QTEXTFORMAT_propertyCount(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtf->propertyCount());
}

//void setBackground ( const QBrush & brush )
static AbstractQoreNode *QTEXTFORMAT_setBackground(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qtf->setBackground(brush);
   return 0;
}

//void setForeground ( const QBrush & brush )
static AbstractQoreNode *QTEXTFORMAT_setForeground(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qtf->setForeground(brush);
   return 0;
}

//void setLayoutDirection ( Qt::LayoutDirection direction )
static AbstractQoreNode *QTEXTFORMAT_setLayoutDirection(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::LayoutDirection direction = (Qt::LayoutDirection)(p ? p->getAsInt() : 0);
   qtf->setLayoutDirection(direction);
   return 0;
}

//void setObjectIndex ( int index )
static AbstractQoreNode *QTEXTFORMAT_setObjectIndex(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtf->setObjectIndex(index);
   return 0;
}

//void setObjectType ( int type )
static AbstractQoreNode *QTEXTFORMAT_setObjectType(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int type = p ? p->getAsInt() : 0;
   qtf->setObjectType(type);
   return 0;
}

////void setProperty ( int propertyId, const QVariant & value )
////void setProperty ( int propertyId, const QVector<QTextLength> & value )
//static AbstractQoreNode *QTEXTFORMAT_setProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   AbstractQoreNode *p = get_param(params, 0);
//   int propertyId = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   ??? QVariant value = p;
//   qtf->setProperty(propertyId, value);
//   return 0;
//}

//QString stringProperty ( int propertyId ) const
static AbstractQoreNode *QTEXTFORMAT_stringProperty(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int propertyId = p ? p->getAsInt() : 0;
   return new QoreStringNode(qtf->stringProperty(propertyId).toUtf8().data(), QCS_UTF8);
}

//QTextBlockFormat toBlockFormat () const
static AbstractQoreNode *QTEXTFORMAT_toBlockFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtbf = new QoreObject(QC_QTextBlockFormat, getProgram());
   QoreQTextBlockFormat *q_qtbf = new QoreQTextBlockFormat(qtf->toBlockFormat());
   o_qtbf->setPrivate(CID_QTEXTBLOCKFORMAT, q_qtbf);
   return o_qtbf;
}

//QTextCharFormat toCharFormat () const
static AbstractQoreNode *QTEXTFORMAT_toCharFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtcf = new QoreObject(QC_QTextCharFormat, getProgram());
   QoreQTextCharFormat *q_qtcf = new QoreQTextCharFormat(qtf->toCharFormat());
   o_qtcf->setPrivate(CID_QTEXTCHARFORMAT, q_qtcf);
   return o_qtcf;
}

//QTextFrameFormat toFrameFormat () const
static AbstractQoreNode *QTEXTFORMAT_toFrameFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtff = new QoreObject(QC_QTextFrameFormat, getProgram());
   QoreQTextFrameFormat *q_qtff = new QoreQTextFrameFormat(qtf->toFrameFormat());
   o_qtff->setPrivate(CID_QTEXTFRAMEFORMAT, q_qtff);
   return o_qtff;
}

//QTextImageFormat toImageFormat () const
static AbstractQoreNode *QTEXTFORMAT_toImageFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtif = new QoreObject(QC_QTextImageFormat, getProgram());
   QoreQTextImageFormat *q_qtif = new QoreQTextImageFormat(qtf->toImageFormat());
   o_qtif->setPrivate(CID_QTEXTIMAGEFORMAT, q_qtif);
   return o_qtif;
}

//QTextListFormat toListFormat () const
static AbstractQoreNode *QTEXTFORMAT_toListFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtlf = new QoreObject(QC_QTextListFormat, getProgram());
   QoreQTextListFormat *q_qtlf = new QoreQTextListFormat(qtf->toListFormat());
   o_qtlf->setPrivate(CID_QTEXTLISTFORMAT, q_qtlf);
   return o_qtlf;
}

//QTextTableFormat toTableFormat () const
static AbstractQoreNode *QTEXTFORMAT_toTableFormat(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qttf = new QoreObject(QC_QTextTableFormat, getProgram());
   QoreQTextTableFormat *q_qttf = new QoreQTextTableFormat(qtf->toTableFormat());
   o_qttf->setPrivate(CID_QTEXTTABLEFORMAT, q_qttf);
   return o_qttf;
}

//int type () const
static AbstractQoreNode *QTEXTFORMAT_type(QoreObject *self, QoreQTextFormat *qtf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtf->type());
}

QoreClass *initQTextFormatClass()
{
   QC_QTextFormat = new QoreClass("QTextFormat", QDOM_GUI);
   CID_QTEXTFORMAT = QC_QTextFormat->getID();

   QC_QTextFormat->setConstructor(QTEXTFORMAT_constructor);
   QC_QTextFormat->setCopy((q_copy_t)QTEXTFORMAT_copy);

   QC_QTextFormat->addMethod("background",                  (q_method_t)QTEXTFORMAT_background);
   QC_QTextFormat->addMethod("boolProperty",                (q_method_t)QTEXTFORMAT_boolProperty);
   QC_QTextFormat->addMethod("brushProperty",               (q_method_t)QTEXTFORMAT_brushProperty);
   QC_QTextFormat->addMethod("clearBackground",             (q_method_t)QTEXTFORMAT_clearBackground);
   QC_QTextFormat->addMethod("clearForeground",             (q_method_t)QTEXTFORMAT_clearForeground);
   QC_QTextFormat->addMethod("clearProperty",               (q_method_t)QTEXTFORMAT_clearProperty);
   QC_QTextFormat->addMethod("colorProperty",               (q_method_t)QTEXTFORMAT_colorProperty);
   QC_QTextFormat->addMethod("doubleProperty",              (q_method_t)QTEXTFORMAT_doubleProperty);
   QC_QTextFormat->addMethod("foreground",                  (q_method_t)QTEXTFORMAT_foreground);
   QC_QTextFormat->addMethod("hasProperty",                 (q_method_t)QTEXTFORMAT_hasProperty);
   QC_QTextFormat->addMethod("intProperty",                 (q_method_t)QTEXTFORMAT_intProperty);
   QC_QTextFormat->addMethod("isBlockFormat",               (q_method_t)QTEXTFORMAT_isBlockFormat);
   QC_QTextFormat->addMethod("isCharFormat",                (q_method_t)QTEXTFORMAT_isCharFormat);
   QC_QTextFormat->addMethod("isFrameFormat",               (q_method_t)QTEXTFORMAT_isFrameFormat);
   QC_QTextFormat->addMethod("isImageFormat",               (q_method_t)QTEXTFORMAT_isImageFormat);
   QC_QTextFormat->addMethod("isListFormat",                (q_method_t)QTEXTFORMAT_isListFormat);
   QC_QTextFormat->addMethod("isTableFormat",               (q_method_t)QTEXTFORMAT_isTableFormat);
   QC_QTextFormat->addMethod("isValid",                     (q_method_t)QTEXTFORMAT_isValid);
   QC_QTextFormat->addMethod("layoutDirection",             (q_method_t)QTEXTFORMAT_layoutDirection);
   QC_QTextFormat->addMethod("lengthProperty",              (q_method_t)QTEXTFORMAT_lengthProperty);
   //QC_QTextFormat->addMethod("lengthVectorProperty",        (q_method_t)QTEXTFORMAT_lengthVectorProperty);
   QC_QTextFormat->addMethod("merge",                       (q_method_t)QTEXTFORMAT_merge);
   QC_QTextFormat->addMethod("objectIndex",                 (q_method_t)QTEXTFORMAT_objectIndex);
   QC_QTextFormat->addMethod("objectType",                  (q_method_t)QTEXTFORMAT_objectType);
   QC_QTextFormat->addMethod("penProperty",                 (q_method_t)QTEXTFORMAT_penProperty);
   //QC_QTextFormat->addMethod("properties",                  (q_method_t)QTEXTFORMAT_properties);
   //QC_QTextFormat->addMethod("property",                    (q_method_t)QTEXTFORMAT_property);
   QC_QTextFormat->addMethod("propertyCount",               (q_method_t)QTEXTFORMAT_propertyCount);
   QC_QTextFormat->addMethod("setBackground",               (q_method_t)QTEXTFORMAT_setBackground);
   QC_QTextFormat->addMethod("setForeground",               (q_method_t)QTEXTFORMAT_setForeground);
   QC_QTextFormat->addMethod("setLayoutDirection",          (q_method_t)QTEXTFORMAT_setLayoutDirection);
   QC_QTextFormat->addMethod("setObjectIndex",              (q_method_t)QTEXTFORMAT_setObjectIndex);
   QC_QTextFormat->addMethod("setObjectType",               (q_method_t)QTEXTFORMAT_setObjectType);
   //QC_QTextFormat->addMethod("setProperty",                 (q_method_t)QTEXTFORMAT_setProperty);
   QC_QTextFormat->addMethod("stringProperty",              (q_method_t)QTEXTFORMAT_stringProperty);
   QC_QTextFormat->addMethod("toBlockFormat",               (q_method_t)QTEXTFORMAT_toBlockFormat);
   QC_QTextFormat->addMethod("toCharFormat",                (q_method_t)QTEXTFORMAT_toCharFormat);
   QC_QTextFormat->addMethod("toFrameFormat",               (q_method_t)QTEXTFORMAT_toFrameFormat);
   QC_QTextFormat->addMethod("toImageFormat",               (q_method_t)QTEXTFORMAT_toImageFormat);
   QC_QTextFormat->addMethod("toListFormat",                (q_method_t)QTEXTFORMAT_toListFormat);
   QC_QTextFormat->addMethod("toTableFormat",               (q_method_t)QTEXTFORMAT_toTableFormat);
   QC_QTextFormat->addMethod("type",                        (q_method_t)QTEXTFORMAT_type);

   return QC_QTextFormat;
}
