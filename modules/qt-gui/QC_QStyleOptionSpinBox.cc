/*
 QC_QStyleOptionSpinBox.cc
 
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

#include "qore-qt-gui.h"

#include "QC_QStyleOptionSpinBox.h"
#include "QC_QStyleOption.h"

qore_classid_t CID_QSTYLEOPTIONSPINBOX;
class QoreClass *QC_QStyleOptionSpinBox = 0;

int QStyleOptionSpinBox_Notification(QoreObject *obj, QStyleOptionSpinBox *qsosb, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "buttonSymbols")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QAbstractSpinBox::ButtonSymbols buttonSymbols = (QAbstractSpinBox::ButtonSymbols)(p ? p->getAsInt() : 0);
      qsosb->buttonSymbols = buttonSymbols;
      return 0;
   }

   if (!strcmp(mem, "frame")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      bool frame = p ? p->getAsBool() : false;
      qsosb->frame = frame;
      return 0;
   }

   if (!strcmp(mem, "stepEnabled")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QAbstractSpinBox::StepEnabled stepEnabled = (QAbstractSpinBox::StepEnabled)(p ? p->getAsInt() : 0);
      qsosb->stepEnabled = stepEnabled;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionSpinBox_MemberGate(QStyleOptionSpinBox *qsosb, const char *mem)
{
   if (!strcmp(mem, "buttonSymbols"))
      return new QoreBigIntNode(qsosb->buttonSymbols);

   if (!strcmp(mem, "frame"))
      return get_bool_node(qsosb->frame);

   if (!strcmp(mem, "stepEnabled"))
      return new QoreBigIntNode(qsosb->stepEnabled);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONSPINBOX_memberNotification(QoreObject *self, QoreQStyleOptionSpinBox *qsosb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionSpinBox_Notification(self, qsosb, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsosb, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONSPINBOX_memberGate(QoreObject *self, QoreQStyleOptionSpinBox *qsosb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionSpinBox_MemberGate(qsosb, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsosb, member);
}

//QStyleOptionSpinBox ()
//QStyleOptionSpinBox ( const QStyleOptionSpinBox & other )
static void QSTYLEOPTIONSPINBOX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSPINBOX, new QoreQStyleOptionSpinBox());
}

static void QSTYLEOPTIONSPINBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionSpinBox *qsosb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSPINBOX, new QoreQStyleOptionSpinBox(*qsosb));
}

QoreClass *initQStyleOptionSpinBoxClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionSpinBox = new QoreClass("QStyleOptionSpinBox", QDOM_GUI);
   CID_QSTYLEOPTIONSPINBOX = QC_QStyleOptionSpinBox->getID();

   QC_QStyleOptionSpinBox->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionSpinBox->setConstructor(QSTYLEOPTIONSPINBOX_constructor);
   QC_QStyleOptionSpinBox->setCopy((q_copy_t)QSTYLEOPTIONSPINBOX_copy);

   // add special methods
   QC_QStyleOptionSpinBox->addMethod("memberNotification",          (q_method_t)QSTYLEOPTIONSPINBOX_memberNotification);
   QC_QStyleOptionSpinBox->addMethod("memberGate",                  (q_method_t)QSTYLEOPTIONSPINBOX_memberGate);

   return QC_QStyleOptionSpinBox;
}
