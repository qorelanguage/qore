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

int CID_QPOINT;
class QoreClass *QC_QPoint = 0;


static void QPOINT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQPoint *qr;

   QoreNode *p = get_param(params, 0);
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

static void QPOINT_copy(class Object *self, class Object *old, class QoreQPoint *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QPOINT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool isNull () const
static QoreNode *QPOINT_isNull(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qp->isNull());
}

//int manhattanLength () const
static QoreNode *QPOINT_manhattanLength(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->manhattanLength());
}

//int & rx ()
//static QoreNode *QPOINT_rx(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->rx());
//}

//int & ry ()
//static QoreNode *QPOINT_ry(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qp->ry());
//}

//void setX ( int x )
static QoreNode *QPOINT_setX(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qp->setX(x);
   return 0;
}

//void setY ( int y )
static QoreNode *QPOINT_setY(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int y = p ? p->getAsInt() : 0;
   qp->setY(y);
   return 0;
}

//int x () const
static QoreNode *QPOINT_x(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->x());
}

//int y () const
static QoreNode *QPOINT_y(Object *self, QoreQPoint *qp, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qp->y());
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

   traceout("initQPointClass()");
   return QC_QPoint;
}
