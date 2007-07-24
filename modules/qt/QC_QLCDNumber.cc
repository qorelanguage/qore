/*
 QC_QLCDNumber.cc
 
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

#include "QC_QLCDNumber.h"

int CID_QLCDNUMBER;

static void QLCDNUMBER_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQLCDNumber *qlcdn;
   QoreNode *p = get_param(params, 0);
   if (p && p->type != NT_OBJECT)
   {
      int num_digits = p->getAsInt();
      p = test_param(params, NT_OBJECT, 1);
      QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (p && !parent)
      {
	 xsink->raiseException("QLCDNUMBER-CONSTRUCTOR-ERROR", "object passed to QLCDNumber::constructor() is second argument is not derived from QWidget (class: '%s')", p->val.object->getClass()->getName());
	 return;
      }
      if (!parent)
	 qlcdn = new QoreQLCDNumber(num_digits, 0);
      else
      {
	 ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
	 qlcdn = new QoreQLCDNumber(num_digits, parent->getQWidget());
      }
   }
   else {
      p = test_param(params, NT_OBJECT, 0);
      QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (p && !parent)
      {
	 xsink->raiseException("QLCDNUMBER-CONSTRUCTOR-ERROR", "object passed to QLCDNumber::constructor() is not derived from QWidget (class: '%s')", p->val.object->getClass()->getName());
	 return;
      }

      if (!parent)
	 qlcdn = new QoreQLCDNumber();
      else 
      {
	 ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
	 qlcdn = new QoreQLCDNumber(parent->getQWidget());
      }
   }

   self->setPrivate(CID_QLCDNUMBER, qlcdn);
}

static void QLCDNUMBER_destructor(class Object *self, class QoreQLCDNumber *qlcdn, ExceptionSink *xsink)
{
   qlcdn->destructor(xsink);
   qlcdn->deref(xsink);
}

static void QLCDNUMBER_copy(class Object *self, class Object *old, class QoreQLCDNumber *qlcdn, ExceptionSink *xsink)
{
   xsink->raiseException("QLCDNUMBER-COPY-ERROR", "objects of this class cannot be copied");
}

static class QoreNode *QLCDNUMBER_setSegmentStyle(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qlcdn->qobj->setSegmentStyle((QLCDNumber::SegmentStyle)p->getAsInt());
   return 0;
}

static class QoreNode *QLCDNUMBER_setNumDigits(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qlcdn->qobj->setNumDigits(p->getAsInt());
   return 0;
}

static class QoreNode *QLCDNUMBER_numDigits(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlcdn->qobj->numDigits());
}

static class QoreNode *QLCDNUMBER_setSmallDecimalPoint(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qlcdn->qobj->setSmallDecimalPoint(p ? p->getAsBool() : false);
   return 0;
}

static class QoreNode *QLCDNUMBER_smallDecimalPoint(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlcdn->qobj->smallDecimalPoint());
}

static class QoreNode *QLCDNUMBER_display(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_STRING)
      qlcdn->qobj->display(p->val.String->getBuffer());
   else if (p && p->type == NT_FLOAT)
      qlcdn->qobj->display(p->val.floatval);
   else
      qlcdn->qobj->display(p ? p->getAsInt() : 0);
   return 0;
}

static class QoreNode *QLCDNUMBER_value(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlcdn->qobj->value());
}

static class QoreNode *QLCDNUMBER_intValue(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlcdn->qobj->intValue());
}

static class QoreNode *QLCDNUMBER_setHexMode(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setHexMode();
   return 0;
}

static class QoreNode *QLCDNUMBER_setDecMode(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setDecMode();
   return 0;
}

static class QoreNode *QLCDNUMBER_setOctMode(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setOctMode();
   return 0;
}

static class QoreNode *QLCDNUMBER_setBinMode(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setBinMode();
   return 0;
}

static class QoreNode *QLCDNUMBER_setMode(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qlcdn->qobj->setMode((QLCDNumber::Mode)p->getAsInt());
   return 0;
}

static class QoreNode *QLCDNUMBER_checkOverflow(class Object *self, class QoreQLCDNumber *qlcdn, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   bool rc;
   if (p && p->type == NT_FLOAT)
      rc = qlcdn->qobj->checkOverflow(p->val.floatval);
   else
      rc = qlcdn->qobj->checkOverflow(p ? p->getAsInt() : 0);
   return new QoreNode(rc);
}

class QoreClass *initQLCDNumberClass(class QoreClass *qframe)
{
   tracein("initQLCDNumberClass()");
   
   class QoreClass *QC_QLCDNumber = new QoreClass("QLCDNumber", QDOM_GUI);
   CID_QLCDNUMBER = QC_QLCDNumber->getID();

   QC_QLCDNumber->addBuiltinVirtualBaseClass(qframe);

   QC_QLCDNumber->setConstructor(QLCDNUMBER_constructor);
   QC_QLCDNumber->setDestructor((q_destructor_t)QLCDNUMBER_destructor);
   QC_QLCDNumber->setCopy((q_copy_t)QLCDNUMBER_copy);

   QC_QLCDNumber->addMethod("setSegmentStyle",        (q_method_t)QLCDNUMBER_setSegmentStyle);
   QC_QLCDNumber->addMethod("setNumDigits",           (q_method_t)QLCDNUMBER_setNumDigits);
   QC_QLCDNumber->addMethod("numDigits",              (q_method_t)QLCDNUMBER_numDigits);
   QC_QLCDNumber->addMethod("setSmallDecimalPoint",   (q_method_t)QLCDNUMBER_setSmallDecimalPoint);
   QC_QLCDNumber->addMethod("smallDecimalPoint",      (q_method_t)QLCDNUMBER_smallDecimalPoint);
   QC_QLCDNumber->addMethod("display",                (q_method_t)QLCDNUMBER_display);
   QC_QLCDNumber->addMethod("value",                  (q_method_t)QLCDNUMBER_value);
   QC_QLCDNumber->addMethod("intValue",               (q_method_t)QLCDNUMBER_intValue);
   QC_QLCDNumber->addMethod("setHexMode",             (q_method_t)QLCDNUMBER_setHexMode);
   QC_QLCDNumber->addMethod("setDecMode",             (q_method_t)QLCDNUMBER_setDecMode);
   QC_QLCDNumber->addMethod("setOctMode",             (q_method_t)QLCDNUMBER_setOctMode);
   QC_QLCDNumber->addMethod("setBinMode",             (q_method_t)QLCDNUMBER_setBinMode);
   QC_QLCDNumber->addMethod("setMode",                (q_method_t)QLCDNUMBER_setMode);
   QC_QLCDNumber->addMethod("checkOverflow",          (q_method_t)QLCDNUMBER_checkOverflow);

   traceout("initQLCDNumberClass()");
   return QC_QLCDNumber;
}
