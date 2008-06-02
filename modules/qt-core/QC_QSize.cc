/*
 QC_QSize.cc
 
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

#include "qt-core.h"

#include "QC_QSize.h"

qore_classid_t CID_QSIZE;
QoreClass *QC_QSize = 0;

static void QSIZE_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQSize *qr;

   const AbstractQoreNode *p = get_param(params, 0);
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
static AbstractQoreNode *QSIZE_boundedTo(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQSize *otherSize = p ? (QoreQSize *)p->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
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
static AbstractQoreNode *QSIZE_expandedTo(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQSize *otherSize = p ? (QoreQSize *)p->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
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
static AbstractQoreNode *QSIZE_height(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qs->height());
}

//bool isEmpty () const
static AbstractQoreNode *QSIZE_isEmpty(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qs->isEmpty());
}

//bool isNull () const
static AbstractQoreNode *QSIZE_isNull(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qs->isNull());
}

//bool isValid () const
static AbstractQoreNode *QSIZE_isValid(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qs->isValid());
}

//int & rheight ()
//static AbstractQoreNode *QSIZE_rheight(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qs->rheight());
//}

//int & rwidth ()
//static AbstractQoreNode *QSIZE_rwidth(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qs->rwidth());
//}

//void scale ( int width, int height, Qt::AspectRatioMode mode )
//void scale ( const QSize & size, Qt::AspectRatioMode mode )
static AbstractQoreNode *QSIZE_scale(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQSize *size = (QoreQSize *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size) {
         if (!xsink->isException())
            xsink->raiseException("QSIZE-SCALE-PARAM-ERROR", "QSize::scale() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
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
static AbstractQoreNode *QSIZE_setHeight(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int height = p ? p->getAsInt() : 0;
   qs->setHeight(height);
   return 0;
}

//void setWidth ( int width )
static AbstractQoreNode *QSIZE_setWidth(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   qs->setWidth(width);
   return 0;
}

//void transpose ()
static AbstractQoreNode *QSIZE_transpose(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   qs->transpose();
   return 0;
}

//int width () const
static AbstractQoreNode *QSIZE_width(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qs->width());
}

//subtraction operator
static AbstractQoreNode *QSIZE_subtract(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);

   QoreQSize *qsize = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (*xsink)
      return 0;

   if (!qsize) {
      xsink->raiseException("QSIZE-SUBTRACT-OPERATOR-ERROR", "expecting an object derived from QSize as sole argument to QSize::subtract()");
      return 0;
   }
  
   ReferenceHolder<AbstractPrivateData> holder(qsize, xsink);

   return return_object(QC_QSize, new QoreQSize(*qs - *qsize));
}

//subtraction operator
static AbstractQoreNode *QSIZE_subtractEquals(QoreObject *self, QoreQSize *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);

   QoreQSize *qsize = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (*xsink)
      return 0;

   if (!qsize) {
      xsink->raiseException("QSIZE-SUBTRACTEQUALS-OPERATOR-ERROR", "expecting an object derived from QSize as sole argument to QSize::subtractEquals()");
      return 0;
   }
  
   ReferenceHolder<AbstractPrivateData> holder(qsize, xsink);

   *qs -= *qsize;

   return 0;
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

   // operators
   QC_QSize->addMethod("subtract",                    (q_method_t)QSIZE_subtract);
   QC_QSize->addMethod("subtractEquals",              (q_method_t)QSIZE_subtractEquals);

   traceout("initQSizeClass()");
   return QC_QSize;
}
