/*
 QC_QLine.cc
 
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

#include "QC_QLine.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

int CID_QLINE;
class QoreClass *QC_QLine = 0;

//QLine ()
//QLine ( const QPoint & p1, const QPoint & p2 )
//QLine ( int x1, int y1, int x2, int y2 )
static void QLINE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QLINE, new QoreQLine());
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *p1 = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!p1) {
         if (!xsink->isException())
            xsink->raiseException("QLINE-QLINE-PARAM-ERROR", "QLine::QLine() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return;
      }
      ReferenceHolder<QoreQPoint> p1Holder(p1, xsink);
      p = get_param(params, 1);
      QoreQPoint *p2 = p ? (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
      if (!p2) {
         if (!xsink->isException())
            xsink->raiseException("QLINE-QLINE-PARAM-ERROR", "this version of QLine::QLine() expects an object derived from QPoint as the second argument");
         return;
      }
      ReferenceHolder<QoreQPoint> p2Holder(p2, xsink);
      self->setPrivate(CID_QLINE, new QoreQLine(*(static_cast<QPoint *>(p1)), *(static_cast<QPoint *>(p2))));
      return;
   }
   int x1 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int x2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int y2 = p ? p->getAsInt() : 0;
   self->setPrivate(CID_QLINE, new QoreQLine(x1, y1, x2, y2));
   return;
}

static void QLINE_copy(class QoreObject *self, class QoreObject *old, class QoreQLine *ql, ExceptionSink *xsink)
{
   self->setPrivate(CID_QLINE, new QoreQLine(*ql));
   //xsink->raiseException("QLINE-COPY-ERROR", "objects of this class cannot be copied");
}

//QPoint p1 () const
static AbstractQoreNode *QLINE_p1(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(ql->p1());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//QPoint p2 () const
static AbstractQoreNode *QLINE_p2(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(ql->p2());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//int x1 () const
static AbstractQoreNode *QLINE_x1(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->x1());
}

//int x2 () const
static AbstractQoreNode *QLINE_x2(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->x2());
}

//int y1 () const
static AbstractQoreNode *QLINE_y1(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->y1());
}

//int y2 () const
static AbstractQoreNode *QLINE_y2(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->y2());
}

//int dx () const
static AbstractQoreNode *QLINE_dx(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->dx());
}

//int dy () const
static AbstractQoreNode *QLINE_dy(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->dy());
}

//bool isNull () const
static AbstractQoreNode *QLINE_isNull(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(ql->isNull());
}

//void translate ( const QPoint & offset )
//void translate ( int dx, int dy )
static AbstractQoreNode *QLINE_translate(QoreObject *self, QoreQLine *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *offset = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!offset) {
         if (!xsink->isException())
            xsink->raiseException("QLINE-TRANSLATE-PARAM-ERROR", "QLine::translate() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<QoreQPoint> offsetHolder(offset, xsink);
      ql->translate(*(static_cast<QPoint *>(offset)));
      return 0;
   }
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;
   ql->translate(dx, dy);
   return 0;
}

QoreClass *initQLineClass()
{
   QC_QLine = new QoreClass("QLine", QDOM_GUI);
   CID_QLINE = QC_QLine->getID();

   QC_QLine->setConstructor(QLINE_constructor);
   QC_QLine->setCopy((q_copy_t)QLINE_copy);

   QC_QLine->addMethod("p1",                          (q_method_t)QLINE_p1);
   QC_QLine->addMethod("p2",                          (q_method_t)QLINE_p2);
   QC_QLine->addMethod("x1",                          (q_method_t)QLINE_x1);
   QC_QLine->addMethod("x2",                          (q_method_t)QLINE_x2);
   QC_QLine->addMethod("y1",                          (q_method_t)QLINE_y1);
   QC_QLine->addMethod("y2",                          (q_method_t)QLINE_y2);
   QC_QLine->addMethod("dx",                          (q_method_t)QLINE_dx);
   QC_QLine->addMethod("dy",                          (q_method_t)QLINE_dy);
   QC_QLine->addMethod("isNull",                      (q_method_t)QLINE_isNull);
   QC_QLine->addMethod("translate",                   (q_method_t)QLINE_translate);

   return QC_QLine;
}
