/*
 QC_QIntValidator.cc
 
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

#include "QC_QIntValidator.h"
#include "QC_QObject.h"

#include "qore-qt.h"

int CID_QINTVALIDATOR;
class QoreClass *QC_QIntValidator = 0;

//QIntValidator ( QObject * parent )
//QIntValidator ( int minimum, int maximum, QObject * parent )
static void QINTVALIDATOR_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreAbstractQObject *parent = (QoreAbstractQObject *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QOBJECT, xsink);
      if (!parent) {
         if (!xsink->isException())
            xsink->raiseException("QINTVALIDATOR-CONSTRUCTOR-PARAM-ERROR", "QIntValidator::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QINTVALIDATOR, new QoreQIntValidator(self, parent->getQObject()));
      return;
   }
   int minimum = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int maximum = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QINTVALIDATOR-CONSTRUCTOR-PARAM-ERROR", "this version of QIntValidator::constructor() expects an object derived from QObject as the third argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
      return;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QINTVALIDATOR, new QoreQIntValidator(self, minimum, maximum, parent->getQObject()));
   return;
}

static void QINTVALIDATOR_copy(class QoreObject *self, class QoreObject *old, class QoreQIntValidator *qiv, ExceptionSink *xsink)
{
   xsink->raiseException("QINTVALIDATOR-COPY-ERROR", "objects of this class cannot be copied");
}

//int bottom () const
static QoreNode *QINTVALIDATOR_bottom(QoreObject *self, QoreQIntValidator *qiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiv->qobj->bottom());
}

//void setBottom ( int )
static QoreNode *QINTVALIDATOR_setBottom(QoreObject *self, QoreQIntValidator *qiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qiv->qobj->setBottom(x);
   return 0;
}

//virtual void setRange ( int bottom, int top )
static QoreNode *QINTVALIDATOR_setRange(QoreObject *self, QoreQIntValidator *qiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int bottom = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int top = p ? p->getAsInt() : 0;
   qiv->setRange(bottom, top);
   return 0;
}

//void setTop ( int )
static QoreNode *QINTVALIDATOR_setTop(QoreObject *self, QoreQIntValidator *qiv, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qiv->qobj->setTop(x);
   return 0;
}

//int top () const
static QoreNode *QINTVALIDATOR_top(QoreObject *self, QoreQIntValidator *qiv, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiv->qobj->top());
}

QoreClass *initQIntValidatorClass(QoreClass *qvalidator)
{
   QC_QIntValidator = new QoreClass("QIntValidator", QDOM_GUI);
   CID_QINTVALIDATOR = QC_QIntValidator->getID();

   QC_QIntValidator->addBuiltinVirtualBaseClass(qvalidator);

   QC_QIntValidator->setConstructor(QINTVALIDATOR_constructor);
   QC_QIntValidator->setCopy((q_copy_t)QINTVALIDATOR_copy);

   QC_QIntValidator->addMethod("bottom",                      (q_method_t)QINTVALIDATOR_bottom);
   QC_QIntValidator->addMethod("setBottom",                   (q_method_t)QINTVALIDATOR_setBottom);
   QC_QIntValidator->addMethod("setRange",                    (q_method_t)QINTVALIDATOR_setRange);
   QC_QIntValidator->addMethod("setTop",                      (q_method_t)QINTVALIDATOR_setTop);
   QC_QIntValidator->addMethod("top",                         (q_method_t)QINTVALIDATOR_top);

   return QC_QIntValidator;
}
