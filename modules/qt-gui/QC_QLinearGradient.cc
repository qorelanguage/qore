/*
 QC_QLinearGradient.cc
 
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

#include "qore-qt.h"

#include "QC_QLinearGradient.h"
#include "QC_QPointF.h"

int CID_QLINEARGRADIENT;
QoreClass *QC_QLinearGradient = 0;

//QLinearGradient ()
//QLinearGradient ( const QPointF & start, const QPointF & finalStop )
//QLinearGradient ( qreal x1, qreal y1, qreal x2, qreal y2 )
static void QLINEARGRADIENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QLINEARGRADIENT, new QoreQLinearGradient());
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQPointF *start = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!start) {
         if (!xsink->isException())
            xsink->raiseException("QLINEARGRADIENT-CONSTRUCTOR-PARAM-ERROR", "QLinearGradient::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> startHolder(static_cast<AbstractPrivateData *>(start), xsink);
      p = get_param(params, 1);
      QoreQPointF *finalStop = (p && p->getType() == NT_OBJECT) ? (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
      if (!finalStop) {
         if (!xsink->isException())
            xsink->raiseException("QLINEARGRADIENT-CONSTRUCTOR-PARAM-ERROR", "this version of QLinearGradient::constructor() expects an object derived from QPointF as the second argument");
         return;
      }
      ReferenceHolder<AbstractPrivateData> finalStopHolder(static_cast<AbstractPrivateData *>(finalStop), xsink);
      self->setPrivate(CID_QLINEARGRADIENT, new QoreQLinearGradient(*(static_cast<QPointF *>(start)), *(static_cast<QPointF *>(finalStop))));
      return;
   }
   qreal x1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y1 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal x2 = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal y2 = p ? p->getAsFloat() : 0.0;
   self->setPrivate(CID_QLINEARGRADIENT, new QoreQLinearGradient(x1, y1, x2, y2));
   return;
}

static void QLINEARGRADIENT_copy(QoreObject *self, QoreObject *old, QoreQLinearGradient *qlg, ExceptionSink *xsink)
{
   xsink->raiseException("QLINEARGRADIENT-COPY-ERROR", "objects of this class cannot be copied");
}

//QPointF finalStop () const
static AbstractQoreNode *QLINEARGRADIENT_finalStop(QoreObject *self, QoreQLinearGradient *qlg, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qlg->finalStop()));
}

//void setFinalStop ( const QPointF & stop )
//void setFinalStop ( qreal x, qreal y )
static AbstractQoreNode *QLINEARGRADIENT_setFinalStop(QoreObject *self, QoreQLinearGradient *qlg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPointF *stop = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!stop) {
         if (!xsink->isException())
            xsink->raiseException("QLINEARGRADIENT-SETFINALSTOP-PARAM-ERROR", "QLinearGradient::setFinalStop() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> stopHolder(static_cast<AbstractPrivateData *>(stop), xsink);
      qlg->setFinalStop(*(static_cast<QPointF *>(stop)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   qlg->setFinalStop(x, y);
   return 0;
}

//void setStart ( const QPointF & start )
//void setStart ( qreal x, qreal y )
static AbstractQoreNode *QLINEARGRADIENT_setStart(QoreObject *self, QoreQLinearGradient *qlg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPointF *start = (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!start) {
         if (!xsink->isException())
            xsink->raiseException("QLINEARGRADIENT-SETSTART-PARAM-ERROR", "QLinearGradient::setStart() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> startHolder(static_cast<AbstractPrivateData *>(start), xsink);
      qlg->setStart(*(static_cast<QPointF *>(start)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   qlg->setStart(x, y);
   return 0;
}

//QPointF start () const
static AbstractQoreNode *QLINEARGRADIENT_start(QoreObject *self, QoreQLinearGradient *qlg, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qlg->start()));
}

QoreClass *initQLinearGradientClass(QoreClass *qgradient)
{
   QC_QLinearGradient = new QoreClass("QLinearGradient", QDOM_GUI);
   CID_QLINEARGRADIENT = QC_QLinearGradient->getID();

   QC_QLinearGradient->addBuiltinVirtualBaseClass(qgradient);

   QC_QLinearGradient->setConstructor(QLINEARGRADIENT_constructor);
   QC_QLinearGradient->setCopy((q_copy_t)QLINEARGRADIENT_copy);

   QC_QLinearGradient->addMethod("finalStop",                   (q_method_t)QLINEARGRADIENT_finalStop);
   QC_QLinearGradient->addMethod("setFinalStop",                (q_method_t)QLINEARGRADIENT_setFinalStop);
   QC_QLinearGradient->addMethod("setStart",                    (q_method_t)QLINEARGRADIENT_setStart);
   QC_QLinearGradient->addMethod("start",                       (q_method_t)QLINEARGRADIENT_start);

   return QC_QLinearGradient;
}
