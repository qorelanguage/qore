/*
 QC_QSize.cc
 
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

#include "QC_QSize.h"
#include "QC_QColor.h"

int CID_QSIZE;
class QoreClass *QC_QSize = 0;


static void QSIZE_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQSize *qr;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qr = new QoreQSize();
   else {
      int x = p->getAsInt();
      p = get_param(params, 1);
      int y = p ? p->getAsInt() : 0;

      qr = new QoreQSize(x, y);
   }

   self->setPrivate(CID_QSIZE, qr);
}

static void QSIZE_copy(class QoreObject *self, class QoreObject *old, class QoreQSize *qr, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSIZE, new QoreQSize(*qr));
}

//QSize boundedTo ( const QSize & otherSize ) const
static QoreNode *QSIZE_boundedTo(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *otherSize = (p && p->type == NT_OBJECT) ? (QoreQSize *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!otherSize) {
      if (!xsink->isException())
         xsink->raiseException("QSIZE-BOUNDEDTO-PARAM-ERROR", "expecting a QSize object as first argument to QSize::boundedTo()");
      return 0;
   }
   ReferenceHolder<QoreQSize> holder(otherSize, xsink);
   QoreObject *o_qs = new QoreObject(self->getClass(CID_QSIZE), getProgram());
   QoreQSize *q_qs = new QoreQSize(qs->boundedTo(*(static_cast<QSize *>(otherSize))));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//QSize expandedTo ( const QSize & otherSize ) const
static QoreNode *QSIZE_expandedTo(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *otherSize = (p && p->type == NT_OBJECT) ? (QoreQSize *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!otherSize) {
      if (!xsink->isException())
         xsink->raiseException("QSIZE-EXPANDEDTO-PARAM-ERROR", "expecting a QSize object as first argument to QSize::expandedTo()");
      return 0;
   }
   ReferenceHolder<QoreQSize> holder(otherSize, xsink);
   QoreObject *o_qs = new QoreObject(self->getClass(CID_QSIZE), getProgram());
   QoreQSize *q_qs = new QoreQSize(qs->expandedTo(*(static_cast<QSize *>(otherSize))));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//int height () const
static QoreNode *QSIZE_height(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qs->height());
}

//bool isEmpty () const
static QoreNode *QSIZE_isEmpty(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qs->isEmpty());
}

//bool isNull () const
static QoreNode *QSIZE_isNull(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qs->isNull());
}

//bool isValid () const
static QoreNode *QSIZE_isValid(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qs->isValid());
}

//int & rheight ()
//static QoreNode *QSIZE_rheight(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qs->rheight());
//}

//int & rwidth ()
//static QoreNode *QSIZE_rwidth(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qs->rwidth());
//}

//void scale ( int width, int height, Qt::AspectRatioMode mode )
//void scale ( const QSize & size, Qt::AspectRatioMode mode )
static QoreNode *QSIZE_scale(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQSize *size = (QoreQSize *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size) {
         if (!xsink->isException())
            xsink->raiseException("QSIZE-SCALE-PARAM-ERROR", "QSize::scale() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQSize> sizeHolder(size, xsink);
      p = get_param(params, 1);
      Qt::AspectRatioMode mode = (Qt::AspectRatioMode)(p ? p->getAsInt() : 0);
      qs->scale(*(static_cast<QSize *>(size)), mode);
      return 0;
   }
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   Qt::AspectRatioMode mode = (Qt::AspectRatioMode)(p ? p->getAsInt() : 0);
   qs->scale(width, height, mode);
   return 0;
}

//void setHeight ( int height )
static QoreNode *QSIZE_setHeight(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int height = p ? p->getAsInt() : 0;
   qs->setHeight(height);
   return 0;
}

//void setWidth ( int width )
static QoreNode *QSIZE_setWidth(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   qs->setWidth(width);
   return 0;
}

//void transpose ()
static QoreNode *QSIZE_transpose(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   qs->transpose();
   return 0;
}

//int width () const
static QoreNode *QSIZE_width(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qs->width());
}

class QoreClass *initQSizeClass()
{
   tracein("initQSizeClass()");
   
   QC_QSize = new QoreClass("QSize", QDOM_GUI);
   CID_QSIZE = QC_QSize->getID();
   QC_QSize->setConstructor(QSIZE_constructor);
   QC_QSize->setCopy((q_copy_t)QSIZE_copy);

   QC_QSize->addMethod("boundedTo",                   (q_method_t)QSIZE_boundedTo);
   QC_QSize->addMethod("expandedTo",                  (q_method_t)QSIZE_expandedTo);
   QC_QSize->addMethod("height",                      (q_method_t)QSIZE_height);
   QC_QSize->addMethod("isEmpty",                     (q_method_t)QSIZE_isEmpty);
   QC_QSize->addMethod("isNull",                      (q_method_t)QSIZE_isNull);
   QC_QSize->addMethod("isValid",                     (q_method_t)QSIZE_isValid);
   //QC_QSize->addMethod("rheight",                     (q_method_t)QSIZE_rheight);
   //QC_QSize->addMethod("rwidth",                      (q_method_t)QSIZE_rwidth);
   QC_QSize->addMethod("scale",                       (q_method_t)QSIZE_scale);
   QC_QSize->addMethod("setHeight",                   (q_method_t)QSIZE_setHeight);
   QC_QSize->addMethod("setWidth",                    (q_method_t)QSIZE_setWidth);
   QC_QSize->addMethod("transpose",                   (q_method_t)QSIZE_transpose);
   QC_QSize->addMethod("width",                       (q_method_t)QSIZE_width);

   traceout("initQSizeClass()");
   return QC_QSize;
}
