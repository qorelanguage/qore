/*
 QC_QFontDatabase.cc
 
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

#include "QC_QFontDatabase.h"

int CID_QFONTDATABASE;
class QoreClass *QC_QFontDatabase = 0;

//QFontDatabase ()
static void QFONTDATABASE_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QFONTDATABASE, new QoreQFontDatabase());
   return;
}

static void QFONTDATABASE_copy(class Object *self, class Object *old, class QoreQFontDatabase *qfd, ExceptionSink *xsink)
{
   xsink->raiseException("QFONTDATABASE-COPY-ERROR", "objects of this class cannot be copied");
}

//bool bold ( const QString & family, const QString & style ) const
static QoreNode *QFONTDATABASE_bold(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-BOLD-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::bold()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-BOLD-PARAM-ERROR", "expecting a string as second argument to QFontDatabase::bold()");
      return 0;
   }
   const char *style = p->val.String->getBuffer();
   return new QoreNode(qfd->bold(family, style));
}

//QStringList families ( WritingSystem writingSystem = Any ) const
static QoreNode *QFONTDATABASE_families(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFontDatabase::WritingSystem writingSystem = (QFontDatabase::WritingSystem)(p ? p->getAsInt() : 0);
   QStringList strlist_rv = qfd->families(writingSystem);
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreNode(new QoreString((*i).toUtf8().data(), QCS_UTF8)));
   return new QoreNode(l);
}

//QFont font ( const QString & family, const QString & style, int pointSize ) const
static QoreNode *QFONTDATABASE_font(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-FONT-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::font()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-FONT-PARAM-ERROR", "expecting a string as second argument to QFontDatabase::font()");
      return 0;
   }
   const char *style = p->val.String->getBuffer();
   p = get_param(params, 2);
   int pointSize = p ? p->getAsInt() : 0;
   Object *o_qf = new Object(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qfd->font(family, style, pointSize));
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//bool isBitmapScalable ( const QString & family, const QString & style = QString() ) const
static QoreNode *QFONTDATABASE_isBitmapScalable(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-ISBITMAPSCALABLE-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::isBitmapScalable()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *style = p ? p->val.String->getBuffer() : 0;
   if (style)
      return new QoreNode(qfd->isBitmapScalable(family, style));
   return new QoreNode(qfd->isBitmapScalable(family));
}

//bool isFixedPitch ( const QString & family, const QString & style = QString() ) const
static QoreNode *QFONTDATABASE_isFixedPitch(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-ISFIXEDPITCH-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::isFixedPitch()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *style = p ? p->val.String->getBuffer() : 0;
   if (style)
      return new QoreNode(qfd->isFixedPitch(family, style));
   return new QoreNode(qfd->isFixedPitch(family));
}

//bool isScalable ( const QString & family, const QString & style = QString() ) const
static QoreNode *QFONTDATABASE_isScalable(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-ISSCALABLE-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::isScalable()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *style = p ? p->val.String->getBuffer() : 0;
   if (style)
      return new QoreNode(qfd->isScalable(family, style));
   return new QoreNode(qfd->isScalable(family));
}

//bool isSmoothlyScalable ( const QString & family, const QString & style = QString() ) const
static QoreNode *QFONTDATABASE_isSmoothlyScalable(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-ISSMOOTHLYSCALABLE-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::isSmoothlyScalable()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *style = p ? p->val.String->getBuffer() : 0;
   if (style)
      return new QoreNode(qfd->isSmoothlyScalable(family, style));
   return new QoreNode(qfd->isSmoothlyScalable(family));
}

//bool italic ( const QString & family, const QString & style ) const
static QoreNode *QFONTDATABASE_italic(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-ITALIC-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::italic()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-ITALIC-PARAM-ERROR", "expecting a string as second argument to QFontDatabase::italic()");
      return 0;
   }
   const char *style = p->val.String->getBuffer();
   return new QoreNode(qfd->italic(family, style));
}

////QList<int> pointSizes ( const QString & family, const QString & style = QString() )
static QoreNode *QFONTDATABASE_pointSizes(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-POINTSIZES-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::pointSizes()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   const char *style = p ? p->val.String->getBuffer() : "";
   QList<int> ilist_rv = qfd->pointSizes(family, style);
   QoreList *l = new QoreList();
   for (QList<int>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreNode((int64)(*i)));
   return new QoreNode(l);
}

//QList<int> smoothSizes ( const QString & family, const QString & style )
static QoreNode *QFONTDATABASE_smoothSizes(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-SMOOTHSIZES-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::smoothSizes()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-SMOOTHSIZES-PARAM-ERROR", "expecting a string as second argument to QFontDatabase::smoothSizes()");
      return 0;
   }
   const char *style = p->val.String->getBuffer();
   QList<int> ilist_rv = qfd->smoothSizes(family, style);
   QoreList *l = new QoreList();
   for (QList<int>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreNode((int64)(*i)));
   return new QoreNode(l);
}

//QString styleString ( const QFont & font )
//QString styleString ( const QFontInfo & fontInfo )
static QoreNode *QFONTDATABASE_styleString(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
  
   QoreQFontInfo *fontInfo = p ? (QoreQFontInfo *)p->val.object->getReferencedPrivateData(CID_QFONTINFO, xsink) : 0;
   if (!fontInfo) {
      QoreQFont *font = p ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
      if (!font) {
	 if (!xsink->isException())
	    xsink->raiseException("QFONTDATABASE-STYLESTRING-PARAM-ERROR", "QFontDatabase::styleString() was expecting a QFont or QFontInfo object as the first argument"); 
	 return 0;
      }
      ReferenceHolder<QoreQFont> fontHolder(font, xsink);
      return new QoreNode(new QoreString(qfd->styleString(*(static_cast<QFont *>(font))).toUtf8().data(), QCS_UTF8));
   }
   ReferenceHolder<QoreQFontInfo> fontInfoHolder(fontInfo, xsink);
   return new QoreNode(new QoreString(qfd->styleString(*(static_cast<QFontInfo *>(fontInfo))).toUtf8().data(), QCS_UTF8));
}

//QStringList styles ( const QString & family ) const
static QoreNode *QFONTDATABASE_styles(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-STYLES-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::styles()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   QStringList styles = qfd->styles(family);
   QoreList *l = new QoreList();
   for (QStringList::iterator i = styles.begin(), e = styles.end(); i != e; ++i)
      l->push(new QoreNode(new QoreString((*i).toUtf8().data(), QCS_UTF8)));
   return new QoreNode(l);
}

//int weight ( const QString & family, const QString & style ) const
static QoreNode *QFONTDATABASE_weight(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-WEIGHT-PARAM-ERROR", "expecting a string as first argument to QFontDatabase::weight()");
      return 0;
   }
   const char *family = p->val.String->getBuffer();
   p = get_param(params, 1);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-WEIGHT-PARAM-ERROR", "expecting a string as second argument to QFontDatabase::weight()");
      return 0;
   }
   const char *style = p->val.String->getBuffer();
   return new QoreNode((int64)qfd->weight(family, style));
}

//QList<WritingSystem> writingSystems () const
//QList<WritingSystem> writingSystems ( const QString & family ) const
static QoreNode *QFONTDATABASE_writingSystems(Object *self, QoreQFontDatabase *qfd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!is_nothing(p) && p->type != NT_STRING) {
      xsink->raiseException("QFONTDATABASE-WRITINGSYSTEMS-PARAM-ERROR", "expecting either NOTHING or a string as first argument to QFontDatabase::writingSystems()");
      return 0;
   }

   QList<QFontDatabase::WritingSystem> ilist_rv;
   
   if (is_nothing(p))
      ilist_rv = qfd->writingSystems();
   else
      ilist_rv = qfd->writingSystems(p->val.String->getBuffer());

   QoreList *l = new QoreList();
   for (QList<QFontDatabase::WritingSystem>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreNode((int64)(*i)));
   return new QoreNode(l);
}

static QoreNode *f_QFontDatabase_standardSizes(QoreNode *params, ExceptionSink *xsink)
{
   QList<int> list = QFontDatabase::standardSizes();
   QoreList *l = new QoreList();
   for (QList<int>::iterator i = list.begin(), e = list.end(); i != e; ++i)
      l->push(new QoreNode((int64)(*i)));
   return new QoreNode(l);
}

QoreClass *initQFontDatabaseClass()
{
   QC_QFontDatabase = new QoreClass("QFontDatabase", QDOM_GUI);
   CID_QFONTDATABASE = QC_QFontDatabase->getID();

   QC_QFontDatabase->setConstructor(QFONTDATABASE_constructor);
   QC_QFontDatabase->setCopy((q_copy_t)QFONTDATABASE_copy);

   QC_QFontDatabase->addMethod("bold",                        (q_method_t)QFONTDATABASE_bold);
   QC_QFontDatabase->addMethod("families",                    (q_method_t)QFONTDATABASE_families);
   QC_QFontDatabase->addMethod("font",                        (q_method_t)QFONTDATABASE_font);
   QC_QFontDatabase->addMethod("isBitmapScalable",            (q_method_t)QFONTDATABASE_isBitmapScalable);
   QC_QFontDatabase->addMethod("isFixedPitch",                (q_method_t)QFONTDATABASE_isFixedPitch);
   QC_QFontDatabase->addMethod("isScalable",                  (q_method_t)QFONTDATABASE_isScalable);
   QC_QFontDatabase->addMethod("isSmoothlyScalable",          (q_method_t)QFONTDATABASE_isSmoothlyScalable);
   QC_QFontDatabase->addMethod("italic",                      (q_method_t)QFONTDATABASE_italic);
   QC_QFontDatabase->addMethod("pointSizes",                  (q_method_t)QFONTDATABASE_pointSizes);
   QC_QFontDatabase->addMethod("smoothSizes",                 (q_method_t)QFONTDATABASE_smoothSizes);
   QC_QFontDatabase->addMethod("styleString",                 (q_method_t)QFONTDATABASE_styleString);
   QC_QFontDatabase->addMethod("styles",                      (q_method_t)QFONTDATABASE_styles);
   QC_QFontDatabase->addMethod("weight",                      (q_method_t)QFONTDATABASE_weight);
   QC_QFontDatabase->addMethod("writingSystems",              (q_method_t)QFONTDATABASE_writingSystems);

   return QC_QFontDatabase;
}

void initQFontDatabaseStaticFunctions()
{
   // add static functions as builtin methods
   builtinFunctions.add("QFontDatabase_standardSizes",     f_QFontDatabase_standardSizes);
}
