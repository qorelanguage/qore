/*
 QC_QIcon.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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
#include "QC_QPixmap.h"
#include "QC_QSize.h"
#include "QC_QPainter.h"
#include "QC_QRect.h"

#include "qore-qt-gui.h"

qore_classid_t CID_QICON;
QoreClass *QC_QIcon = 0;

static void QICON_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQIcon *qi;

   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qi = new QoreQIcon();
   else {
      if (p->getType() != NT_STRING && p->getType() != NT_OBJECT) {
	 xsink->raiseException("QICON-CONSTRUCTOR-ERROR", "missing icon name or QPixmap as first parameter (got type '%s')", p->getTypeName());
	 return;
      }

      if (p->getType() == NT_OBJECT) {
	 AbstractPrivateData *apd_pixmap = (p && p->getType() == NT_OBJECT) ? (reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
	 if (!apd_pixmap) {
	    if (!xsink->isException())
	       xsink->raiseException("QICON-ADDPIXMAP-PARAM-ERROR", "QIcon::constructor() does not know how to handle arguments of class '%s'", (reinterpret_cast<const QoreObject *>(p))->getClassName());
	    return;
	 }
	 ReferenceHolder<AbstractPrivateData> holder(apd_pixmap, xsink);
	 QoreAbstractQPixmap *pixmap = dynamic_cast<QoreAbstractQPixmap *>(apd_pixmap);
	 assert(pixmap);
	 
	 qi = new QoreQIcon(*(pixmap->getQPixmap()));
      }
      else {
	 const char *fname = (reinterpret_cast<const QoreStringNode *>(p))->getBuffer();
	 
	 qi = new QoreQIcon(fname);
      }
   }

   self->setPrivate(CID_QICON, qi);
}

static void QICON_copy(class QoreObject *self, class QoreObject *old, class QoreQIcon *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QICON, new QoreQIcon(*qf));
}

//QSize actualSize ( const QSize & size, Mode mode = Normal, State state = Off ) const
static AbstractQoreNode *QICON_actualSize(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQSize *size = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QICON-ACTUALSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QIcon::actualSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   QIcon::Mode mode = !is_nothing(p) ? (QIcon::Mode)p->getAsInt() : QIcon::Normal;
   p = get_param(params, 2);
   QIcon::State state = !is_nothing(p) ? (QIcon::State)p->getAsInt() : QIcon::Off;
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qi->actualSize(*(static_cast<QSize *>(size)), mode, state));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//void addFile ( const QString & fileName, const QSize & size = QSize(), Mode mode = Normal, State state = Off )
static AbstractQoreNode *QICON_addFile(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   const QoreObject *o = test_object_param(params, 1);
   QoreQSize *size = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   p = get_param(params, 2);
   QIcon::Mode mode = !is_nothing(p) ? (QIcon::Mode)p->getAsInt() : QIcon::Normal;
   p = get_param(params, 3);
   QIcon::State state = !is_nothing(p) ? (QIcon::State)p->getAsInt() : QIcon::Off;
   qi->addFile(fileName, size ? *(static_cast<QSize *>(size)) : QSize(), mode, state);
   return 0;
}

//void addPixmap ( const QPixmap & pixmap, Mode mode = Normal, State state = Off )
static AbstractQoreNode *QICON_addPixmap(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   AbstractPrivateData *apd_pixmap = (p && p->getType() == NT_OBJECT) ? (reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
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
static AbstractQoreNode *QICON_cacheKey(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qi->cacheKey());
}

//DataPtr & data_ptr ()
//static AbstractQoreNode *QICON_data_ptr(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qi->data_ptr());
//}

//bool isNull () const
static AbstractQoreNode *QICON_isNull(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qi->isNull());
}

//void paint ( QPainter * painter, const QRect & rect, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off ) const
//void paint ( QPainter * painter, int x, int y, int w, int h, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off ) const
static AbstractQoreNode *QICON_paint(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
	 xsink->raiseException("QICON-PAINT-PARAM-ERROR", "QIcon::paint() was expecting an object derived from QPainter as the first argument");
      return 0;
   }

   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   if (p && p->getType() == NT_OBJECT) {
      o = reinterpret_cast<const QoreObject *>(p);
      QoreQRect *rect = (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rect) {
	 if (!xsink->isException())
	    xsink->raiseException("QICON-PAINT-PARAM-ERROR", "QIcon::paint() does not know how to handle arguments of class '%s' as passed as the second argument", o->getClassName());
	 return 0;
      }

      ReferenceHolder<QoreQRect> rectHolder(rect, xsink);
      p = get_param(params, 2);
      Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
      p = get_param(params, 3);
      QIcon::Mode mode = (QIcon::Mode)(p ? p->getAsInt() : 0);
      p = get_param(params, 4);
      QIcon::State state = (QIcon::State)(p ? p->getAsInt() : 0);
      qi->paint(painter->getQPainter(), *rect, alignment, mode, state);
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
   qi->paint(painter->getQPainter(), x, y, w, h, alignment, mode, state);
   return 0;
}

//QPixmap pixmap ( const QSize & size, Mode mode = Normal, State state = Off ) const
//QPixmap pixmap ( int w, int h, Mode mode = Normal, State state = Off ) const
//QPixmap pixmap ( int extent, Mode mode = Normal, State state = Off ) const
static AbstractQoreNode *QICON_pixmap(QoreObject *self, QoreQIcon *qi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQSize *size = (QoreQSize *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size) {
         if (!xsink->isException())
            xsink->raiseException("QICON-PIXMAP-PARAM-ERROR", "QIcon::pixmap() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
      p = get_param(params, 1);
      QIcon::Mode mode = !is_nothing(p) ? (QIcon::Mode)p->getAsInt() : QIcon::Normal;
      p = get_param(params, 2);
      QIcon::State state = !is_nothing(p) ? (QIcon::State)p->getAsInt() : QIcon::Off;
      QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
      QoreQPixmap *q_qp = new QoreQPixmap(qi->pixmap(*(static_cast<QSize *>(size)), mode, state));
      o_qp->setPrivate(CID_QPIXMAP, q_qp);
      return o_qp;
   }

   QoreObject *o_qp;

   if (num_params(params) == 3) {
      // treat as extent, Mode, and State
      int extent = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      QIcon::Mode mode = !is_nothing(p) ? (QIcon::Mode)p->getAsInt() : QIcon::Normal;
      p = get_param(params, 2);
      QIcon::State state = !is_nothing(p) ? (QIcon::State)p->getAsInt() : QIcon::Off;
      o_qp = new QoreObject(QC_QPixmap, getProgram());
      QoreQPixmap *q_qp = new QoreQPixmap(qi->pixmap(extent, mode, state));
      o_qp->setPrivate(CID_QPIXMAP, q_qp);
   }
   else {
      int w = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int h = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      QIcon::Mode mode = !is_nothing(p) ? (QIcon::Mode)p->getAsInt() : QIcon::Normal;
      p = get_param(params, 3);
      QIcon::State state = !is_nothing(p) ? (QIcon::State)p->getAsInt() : QIcon::Off;
      o_qp = new QoreObject(QC_QPixmap, getProgram());
      QoreQPixmap *q_qp = new QoreQPixmap(qi->pixmap(w, h, mode, state));
      o_qp->setPrivate(CID_QPIXMAP, q_qp);
   }
   return o_qp;
}

class QoreClass *initQIconClass()
{
   QORE_TRACE("initQIconClass()");
   
   QC_QIcon = new QoreClass("QIcon", QDOM_GUI);
   CID_QICON = QC_QIcon->getID();
   QC_QIcon->setConstructor(QICON_constructor);
   QC_QIcon->setCopy((q_copy_t)QICON_copy);

   QC_QIcon->addMethod("actualSize",                  (q_method_t)QICON_actualSize);
   QC_QIcon->addMethod("addFile",                     (q_method_t)QICON_addFile);
   QC_QIcon->addMethod("addPixmap",                   (q_method_t)QICON_addPixmap);
   QC_QIcon->addMethod("cacheKey",                    (q_method_t)QICON_cacheKey);
   //QC_QIcon->addMethod("data_ptr",                    (q_method_t)QICON_data_ptr);
   QC_QIcon->addMethod("isNull",                      (q_method_t)QICON_isNull);
   QC_QIcon->addMethod("paint",                       (q_method_t)QICON_paint);
   QC_QIcon->addMethod("pixmap",                      (q_method_t)QICON_pixmap);

   return QC_QIcon;
}
