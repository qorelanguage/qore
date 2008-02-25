/*
 QC_QPoint.cc
 
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

#include "QC_QPoint.h"
#include "QC_QColor.h"

#include "qore-qt.h"

int CID_QPOINT;
class QoreClass *QC_QPoint = 0;


static void QPOINT_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQPoint *qr;

   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qr = new QoreQPoint();
   else {
      int x = p->getAsInt();
      p = get_param(params, 1);
      int y = p ? p->getAsInt() : 0;

      qr = new QoreQPoint(x, y);
   }

   self->setPrivate(CID_QPOINT, qr);
}

static void QPOINT_copy(class QoreObject *self, class QoreObject *old, class QoreQPoint *qr, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPOINT, new QoreQPoint(*qr));
   //xsink->raiseException("QPOINT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool isNull () const
static AbstractQoreNode *QPOINT_isNull(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qp->isNull());
}

//int manhattanLength () const
static AbstractQoreNode *QPOINT_manhattanLength(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->manhattanLength());
}

//int & rx ()
//static AbstractQoreNode *QPOINT_rx(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qp->rx());
//}

//int & ry ()
//static AbstractQoreNode *QPOINT_ry(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qp->ry());
//}

//void setX ( int x )
static AbstractQoreNode *QPOINT_setX(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qp->setX(x);
   return 0;
}

//void setY ( int y )
static AbstractQoreNode *QPOINT_setY(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qp->setY(y);
   return 0;
}

//int x () const
static AbstractQoreNode *QPOINT_x(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->x());
}

//int y () const
static AbstractQoreNode *QPOINT_y(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->y());
}

//QPoint unaryMinus () const
static AbstractQoreNode *QPOINT_unaryMinus(QoreObject *self, QoreQPoint *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(-(*qp)));
}

class QoreClass *initQPointClass()
{
   tracein("initQPointClass()");
   
   QC_QPoint = new QoreClass("QPoint", QDOM_GUI);
   CID_QPOINT = QC_QPoint->getID();
   QC_QPoint->setConstructor(QPOINT_constructor);
   QC_QPoint->setCopy((q_copy_t)QPOINT_copy);

   QC_QPoint->addMethod("isNull",                      (q_method_t)QPOINT_isNull);
   QC_QPoint->addMethod("manhattanLength",             (q_method_t)QPOINT_manhattanLength);
   //QC_QPoint->addMethod("rx",                          (q_method_t)QPOINT_rx);
   //QC_QPoint->addMethod("ry",                          (q_method_t)QPOINT_ry);
   QC_QPoint->addMethod("setX",                        (q_method_t)QPOINT_setX);
   QC_QPoint->addMethod("setY",                        (q_method_t)QPOINT_setY);
   QC_QPoint->addMethod("x",                           (q_method_t)QPOINT_x);
   QC_QPoint->addMethod("y",                           (q_method_t)QPOINT_y);

   // in place of operators
   QC_QPoint->addMethod("unaryMinus",                  (q_method_t)QPOINT_unaryMinus);

   traceout("initQPointClass()");
   return QC_QPoint;
}
