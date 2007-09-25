/*
 QC_QDoubleValidator.cc
 
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

#include "QC_QDoubleValidator.h"

int CID_QDOUBLEVALIDATOR;
class QoreClass *QC_QDoubleValidator = 0;

//QDoubleValidator ( QObject * parent )
//QDoubleValidator ( double bottom, double top, int decimals, QObject * parent )
static void QDOUBLEVALIDATOR_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreAbstractQObject *parent = (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink);
      if (!parent) {
         if (!xsink->isException())
            xsink->raiseException("QDOUBLEVALIDATOR-CONSTRUCTOR-PARAM-ERROR", "QDoubleValidator::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QDOUBLEVALIDATOR, new QoreQDoubleValidator(self, parent->getQObject()));
      return;
   }
   double bottom = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   double top = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   int decimals = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QDOUBLEVALIDATOR-CONSTRUCTOR-PARAM-ERROR", "this version of QDoubleValidator::constructor() expects an object derived from QObject as the fourth argument", p->val.object->getClass()->getName());
      return;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QDOUBLEVALIDATOR, new QoreQDoubleValidator(self, bottom, top, decimals, parent->getQObject()));
   return;
}

static void QDOUBLEVALIDATOR_copy(class Object *self, class Object *old, class QoreQDoubleValidator *qdv, ExceptionSink *xsink)
{
   xsink->raiseException("QDOUBLEVALIDATOR-COPY-ERROR", "objects of this class cannot be copied");
}

//double bottom () const
static QoreNode *QDOUBLEVALIDATOR_bottom(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qdv->qobj->bottom());
}

//int decimals () const
static QoreNode *QDOUBLEVALIDATOR_decimals(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qdv->qobj->decimals());
}

//Notation notation () const
static QoreNode *QDOUBLEVALIDATOR_notation(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qdv->qobj->notation());
}

//void setBottom ( double )
static QoreNode *QDOUBLEVALIDATOR_setBottom(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double bottom = p ? p->getAsFloat() : 0.0;
   qdv->qobj->setBottom(bottom);
   return 0;
}

//void setDecimals ( int )
static QoreNode *QDOUBLEVALIDATOR_setDecimals(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qdv->qobj->setDecimals(x);
   return 0;
}

//void setNotation ( Notation )
static QoreNode *QDOUBLEVALIDATOR_setNotation(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDoubleValidator::Notation notation = (QDoubleValidator::Notation)(p ? p->getAsInt() : 0);
   qdv->qobj->setNotation(notation);
   return 0;
}

//virtual void setRange ( double minimum, double maximum, int decimals = 0 )
static QoreNode *QDOUBLEVALIDATOR_setRange(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double minimum = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   double maximum = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   int decimals = !is_nothing(p) ? p->getAsInt() : 0;
   qdv->setRange(minimum, maximum, decimals);
   return 0;
}

//void setTop ( double )
static QoreNode *QDOUBLEVALIDATOR_setTop(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   double top = p ? p->getAsFloat() : 0.0;
   qdv->qobj->setTop(top);
   return 0;
}

//double top () const
static QoreNode *QDOUBLEVALIDATOR_top(Object *self, QoreQDoubleValidator *qdv, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qdv->qobj->top());
}

QoreClass *initQDoubleValidatorClass(QoreClass *qvalidator)
{
   QC_QDoubleValidator = new QoreClass("QDoubleValidator", QDOM_GUI);
   CID_QDOUBLEVALIDATOR = QC_QDoubleValidator->getID();

   QC_QDoubleValidator->addBuiltinVirtualBaseClass(qvalidator);

   QC_QDoubleValidator->setConstructor(QDOUBLEVALIDATOR_constructor);
   QC_QDoubleValidator->setCopy((q_copy_t)QDOUBLEVALIDATOR_copy);

   QC_QDoubleValidator->addMethod("bottom",                      (q_method_t)QDOUBLEVALIDATOR_bottom);
   QC_QDoubleValidator->addMethod("decimals",                    (q_method_t)QDOUBLEVALIDATOR_decimals);
   QC_QDoubleValidator->addMethod("notation",                    (q_method_t)QDOUBLEVALIDATOR_notation);
   QC_QDoubleValidator->addMethod("setBottom",                   (q_method_t)QDOUBLEVALIDATOR_setBottom);
   QC_QDoubleValidator->addMethod("setDecimals",                 (q_method_t)QDOUBLEVALIDATOR_setDecimals);
   QC_QDoubleValidator->addMethod("setNotation",                 (q_method_t)QDOUBLEVALIDATOR_setNotation);
   QC_QDoubleValidator->addMethod("setRange",                    (q_method_t)QDOUBLEVALIDATOR_setRange);
   QC_QDoubleValidator->addMethod("setTop",                      (q_method_t)QDOUBLEVALIDATOR_setTop);
   QC_QDoubleValidator->addMethod("top",                         (q_method_t)QDOUBLEVALIDATOR_top);

   return QC_QDoubleValidator;
}
