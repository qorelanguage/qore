/*
 QC_QIcon.cc
 
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
#include "QC_QIcon.h"

DLLLOCAL int CID_QICON;

static void QICON_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQIcon *qi;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qi = new QoreQIcon();
   else {
      if (p->type != NT_STRING && p->type != NT_OBJECT) {
	 xsink->raiseException("QICON-CONSTRUCTOR-ERROR", "missing icon name or QPixmap as first parameter (got type '%s')", p->type->getName());
	 return;
      }

      if (p->type == NT_OBJECT) {
	 AbstractPrivateData *apd_pixmap = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
	 if (!apd_pixmap) {
	    if (!xsink->isException())
	       xsink->raiseException("QICON-ADDPIXMAP-PARAM-ERROR", "QIcon::constructor() does not know how to handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return;
	 }
	 ReferenceHolder<AbstractPrivateData> holder(apd_pixmap, xsink);
	 QoreAbstractQPixmap *pixmap = dynamic_cast<QoreAbstractQPixmap *>(apd_pixmap);
	 assert(pixmap);
	 
	 qi = new QoreQIcon(*(pixmap->getQPixmap()));
      }
      else {
	 const char *fname = p->val.String->getBuffer();
	 
	 qi = new QoreQIcon(fname);
      }
   }

   self->setPrivate(CID_QICON, qi);
}

static void QICON_copy(class Object *self, class Object *old, class QoreQIcon *qf, ExceptionSink *xsink)
{
   xsink->raiseException("QICON-COPY-ERROR", "objects of this class cannot be copied");
}

//QSize actualSize ( const QSize & size, Mode mode = Normal, State state = Off ) const
//static QoreNode *QICON_actualSize(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QSize size = p;
//   p = get_param(params, 1);
//   QIcon::Mode mode = (QIcon::Mode)(p ? p->getAsInt() : 0);
//   p = get_param(params, 2);
//   QIcon::State state = (QIcon::State)(p ? p->getAsInt() : 0);
//   ??? return new QoreNode((int64)qi->actualSize(size, mode, state));
//}

//void addFile ( const QString & fileName, const QSize & size = QSize(), Mode mode = Normal, State state = Off )
//static QoreNode *QICON_addFile(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QICON-ADDFILE-PARAM-ERROR", "expecting a string as first argument to QIcon::addFile()");
//      return 0;
//   }
//   const char *fileName = p->val.String->getBuffer();
//   p = get_param(params, 1);
//   ??? QSize size = p;
//   p = get_param(params, 2);
//   QIcon::Mode mode = (QIcon::Mode)(p ? p->getAsInt() : 0);
//   p = get_param(params, 3);
//   QIcon::State state = (QIcon::State)(p ? p->getAsInt() : 0);
//   qi->addFile(fileName, size, mode, state);
//   return 0;
//}

//void addPixmap ( const QPixmap & pixmap, Mode mode = Normal, State state = Off )
static QoreNode *QICON_addPixmap(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   AbstractPrivateData *apd_pixmap = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!apd_pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QICON-ADDPIXMAP-PARAM-ERROR", "expecting a QPixmap object as first argument to QIcon::addPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder(apd_pixmap, xsink);
   QoreAbstractQPixmap *pixmap = dynamic_cast<QoreAbstractQPixmap *>(apd_pixmap);
   assert(pixmap);
   p = get_param(params, 1);
   QIcon::Mode mode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   QIcon::State state = (QIcon::State)(p ? p->getAsInt() : 0);
   qi->addPixmap(*(pixmap->getQPixmap()), mode, state);
   return 0;
}

//qint64 cacheKey () const
static QoreNode *QICON_cacheKey(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qi->cacheKey());
}

//DataPtr & data_ptr ()
//static QoreNode *QICON_data_ptr(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qi->data_ptr());
//}

//bool isNull () const
static QoreNode *QICON_isNull(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qi->isNull());
}

//void paint ( QPainter * painter, const QRect & rect, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off ) const
//void paint ( QPainter * painter, int x, int y, int w, int h, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off ) const
static QoreNode *QICON_paint(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPainter *painter = (QoreQPainter *)(p ? p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0);
   if (!painter) {
      if (!xsink->isException())
	 xsink->raiseException("QICON-PAINT-PARAM-ERROR", "QIcon::paint() was expecting an object derived from QPainter as the first argument");
      return 0;
   }

   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   p = get_param(params, 1);
   if (p && p->type == NT_OBJECT) {

      QoreQRect *rect = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rect) {
	 if (!xsink->isException())
	    xsink->raiseException("QICON-PAINT-PARAM-ERROR", "QIcon::paint() does not know how to handle arguments of class '%s' as passed as the second argument", p->val.object->getClass()->getName());
	 return 0;
      }

      ReferenceHolder<QoreQRect> rectHolder(rect, xsink);
      p = get_param(params, 2);
      Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
      p = get_param(params, 3);
      QIcon::Mode mode = (QIcon::Mode)(p ? p->getAsInt() : 0);
      p = get_param(params, 4);
      QIcon::State state = (QIcon::State)(p ? p->getAsInt() : 0);
      qi->paint(static_cast<QPainter *>(painter), *rect, alignment, mode, state);
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int h = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   QIcon::Mode mode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   QIcon::State state = (QIcon::State)(p ? p->getAsInt() : 0);
   qi->paint(static_cast<QPainter *>(painter), x, y, w, h, alignment, mode, state);
   return 0;
}

//QPixmap pixmap ( const QSize & size, Mode mode = Normal, State state = Off ) const
//QPixmap pixmap ( int w, int h, Mode mode = Normal, State state = Off ) const
//QPixmap pixmap ( int extent, Mode mode = Normal, State state = Off ) const
static QoreNode *QICON_pixmap(Object *self, QoreQIcon *qi, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int h = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QIcon::Mode mode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QIcon::State state = (QIcon::State)(p ? p->getAsInt() : 0);
   Object *o_qp = new Object(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qi->pixmap(w, h, mode, state));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

class QoreClass *initQIconClass()
{
   tracein("initQIconClass()");
   
   class QoreClass *QC_QIcon = new QoreClass("QIcon", QDOM_GUI);
   CID_QICON = QC_QIcon->getID();
   QC_QIcon->setConstructor(QICON_constructor);
   QC_QIcon->setCopy((q_copy_t)QICON_copy);

   //QC_QIcon->addMethod("actualSize",                  (q_method_t)QICON_actualSize);
   //QC_QIcon->addMethod("addFile",                     (q_method_t)QICON_addFile);
   QC_QIcon->addMethod("addPixmap",                   (q_method_t)QICON_addPixmap);
   QC_QIcon->addMethod("cacheKey",                    (q_method_t)QICON_cacheKey);
   //QC_QIcon->addMethod("data_ptr",                    (q_method_t)QICON_data_ptr);
   QC_QIcon->addMethod("isNull",                      (q_method_t)QICON_isNull);
   QC_QIcon->addMethod("paint",                       (q_method_t)QICON_paint);
   QC_QIcon->addMethod("pixmap",                      (q_method_t)QICON_pixmap);
   traceout("initQIconClass()");
   return QC_QIcon;
}
