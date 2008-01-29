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
#include "QC_QWidget.h"

int CID_QLCDNUMBER;

static void QLCDNUMBER_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQLCDNumber *qlcdn;
   QoreNode *p = get_param(params, 0);
   if (p && p->type != NT_OBJECT)
   {
      int num_digits = p->getAsInt();
      p = test_param(params, NT_OBJECT, 1);
      QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (p && !parent)
      {
	 xsink->raiseException("QLCDNUMBER-CONSTRUCTOR-ERROR", "object passed to QLCDNumber::constructor() is second argument is not derived from QWidget (class: '%s')", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
	 return;
      }
      if (!parent)
	 qlcdn = new QoreQLCDNumber(self, num_digits, 0);
      else
      {
	 ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
	 qlcdn = new QoreQLCDNumber(self, num_digits, parent->getQWidget());
      }
   }
   else {
      p = test_param(params, NT_OBJECT, 0);
      QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (p && !parent)
      {
	 xsink->raiseException("QLCDNUMBER-CONSTRUCTOR-ERROR", "object passed to QLCDNumber::constructor() is not derived from QWidget (class: '%s')", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
	 return;
      }

      if (!parent)
	 qlcdn = new QoreQLCDNumber(self);
      else 
      {
	 ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
	 qlcdn = new QoreQLCDNumber(self, parent->getQWidget());
      }
   }

   self->setPrivate(CID_QLCDNUMBER, qlcdn);
}

static void QLCDNUMBER_copy(class QoreObject *self, class QoreObject *old, class QoreQLCDNumber *qlcdn, ExceptionSink *xsink)
{
   xsink->raiseException("QLCDNUMBER-COPY-ERROR", "objects of this class cannot be copied");
}

static class QoreNode *QLCDNUMBER_setSegmentStyle(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qlcdn->qobj->setSegmentStyle((QLCDNumber::SegmentStyle)p->getAsInt());
   return 0;
}

static class QoreNode *QLCDNUMBER_setNumDigits(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qlcdn->qobj->setNumDigits(p->getAsInt());
   return 0;
}

static class QoreNode *QLCDNUMBER_numDigits(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qlcdn->qobj->numDigits());
}

static class QoreNode *QLCDNUMBER_smallDecimalPoint(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qlcdn->qobj->smallDecimalPoint());
}

static class QoreNode *QLCDNUMBER_value(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qlcdn->qobj->value());
}

static class QoreNode *QLCDNUMBER_intValue(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qlcdn->qobj->intValue());
}

static class QoreNode *QLCDNUMBER_setMode(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (!is_nothing(p))
      qlcdn->qobj->setMode((QLCDNumber::Mode)p->getAsInt());
   return 0;
}

static class QoreNode *QLCDNUMBER_checkOverflow(class QoreObject *self, class QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   bool rc;
   if (p && p->type == NT_FLOAT)
      rc = qlcdn->qobj->checkOverflow(reinterpret_cast<const QoreFloatNode *>(p)->f);
   else
      rc = qlcdn->qobj->checkOverflow(p ? p->getAsInt() : 0);
   return new QoreBoolNode(rc);
}

// slots

//void display ( int num )
static QoreNode *QLCDNUMBER_display(QoreObject *self, QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_FLOAT)
      qlcdn->qobj->display(reinterpret_cast<const QoreFloatNode *>(p)->f);
   else if (p && p->type == NT_STRING)
      qlcdn->qobj->display((reinterpret_cast<QoreStringNode *>(p))->getBuffer());
   else {
      int num = p ? p->getAsInt() : 0;
      qlcdn->qobj->display(num);
   }
   return 0;
}

//void setBinMode ()
static QoreNode *QLCDNUMBER_setBinMode(QoreObject *self, QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setBinMode();
   return 0;
}

//void setDecMode ()
static QoreNode *QLCDNUMBER_setDecMode(QoreObject *self, QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setDecMode();
   return 0;
}

//void setHexMode ()
static QoreNode *QLCDNUMBER_setHexMode(QoreObject *self, QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setHexMode();
   return 0;
}

//void setOctMode ()
static QoreNode *QLCDNUMBER_setOctMode(QoreObject *self, QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   qlcdn->qobj->setOctMode();
   return 0;
}

//void setSmallDecimalPoint ( bool )
static QoreNode *QLCDNUMBER_setSmallDecimalPoint(QoreObject *self, QoreQLCDNumber *qlcdn, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : 0;
   qlcdn->qobj->setSmallDecimalPoint(b);
   return 0;
}

class QoreClass *initQLCDNumberClass(class QoreClass *qframe)
{
   tracein("initQLCDNumberClass()");
   
   class QoreClass *QC_QLCDNumber = new QoreClass("QLCDNumber", QDOM_GUI);
   CID_QLCDNUMBER = QC_QLCDNumber->getID();

   QC_QLCDNumber->addBuiltinVirtualBaseClass(qframe);

   QC_QLCDNumber->setConstructor(QLCDNUMBER_constructor);
   QC_QLCDNumber->setCopy((q_copy_t)QLCDNUMBER_copy);

   QC_QLCDNumber->addMethod("setSegmentStyle",        (q_method_t)QLCDNUMBER_setSegmentStyle);
   QC_QLCDNumber->addMethod("setNumDigits",           (q_method_t)QLCDNUMBER_setNumDigits);
   QC_QLCDNumber->addMethod("numDigits",              (q_method_t)QLCDNUMBER_numDigits);
   QC_QLCDNumber->addMethod("smallDecimalPoint",      (q_method_t)QLCDNUMBER_smallDecimalPoint);
   QC_QLCDNumber->addMethod("value",                  (q_method_t)QLCDNUMBER_value);
   QC_QLCDNumber->addMethod("intValue",               (q_method_t)QLCDNUMBER_intValue);
   QC_QLCDNumber->addMethod("setMode",                (q_method_t)QLCDNUMBER_setMode);
   QC_QLCDNumber->addMethod("checkOverflow",          (q_method_t)QLCDNUMBER_checkOverflow);

   // slots
   QC_QLCDNumber->addMethod("display",                (q_method_t)QLCDNUMBER_display);
   QC_QLCDNumber->addMethod("setBinMode",             (q_method_t)QLCDNUMBER_setBinMode);
   QC_QLCDNumber->addMethod("setDecMode",             (q_method_t)QLCDNUMBER_setDecMode);
   QC_QLCDNumber->addMethod("setHexMode",             (q_method_t)QLCDNUMBER_setHexMode);
   QC_QLCDNumber->addMethod("setOctMode",             (q_method_t)QLCDNUMBER_setOctMode);
   QC_QLCDNumber->addMethod("setSmallDecimalPoint",   (q_method_t)QLCDNUMBER_setSmallDecimalPoint);

   traceout("initQLCDNumberClass()");
   return QC_QLCDNumber;
}
