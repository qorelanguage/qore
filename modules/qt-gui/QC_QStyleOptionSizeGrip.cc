/*
 QC_QStyleOptionSizeGrip.cc
 
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

#include "QC_QStyleOptionSizeGrip.h"
#include "QC_QStyleOption.h"

qore_classid_t CID_QSTYLEOPTIONSIZEGRIP;
QoreClass *QC_QStyleOptionSizeGrip = 0;

int QStyleOptionSizeGrip_Notification(QoreObject *obj, QStyleOptionSizeGrip *qsosg, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "corner")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::Corner corner = (Qt::Corner)(p ? p->getAsInt() : 0);
      qsosg->corner = corner;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionSizeGrip_MemberGate(QStyleOptionSizeGrip *qsosg, const char *mem)
{
   if (!strcmp(mem, "corner"))
      return new QoreBigIntNode(qsosg->corner);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONSIZEGRIP_memberNotification(QoreObject *self, QoreQStyleOptionSizeGrip *qsosg, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionSizeGrip_Notification(self, qsosg, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsosg, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONSIZEGRIP_memberGate(QoreObject *self, QoreQStyleOptionSizeGrip *qsosg, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionSizeGrip_MemberGate(qsosg, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsosg, member);
}

//QStyleOptionSizeGrip ()
//QStyleOptionSizeGrip ( const QStyleOptionSizeGrip & other )
static void QSTYLEOPTIONSIZEGRIP_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSIZEGRIP, new QoreQStyleOptionSizeGrip());
}

static void QSTYLEOPTIONSIZEGRIP_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionSizeGrip *qsosg, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSIZEGRIP, new QoreQStyleOptionSizeGrip(*qsosg));
}

QoreClass *initQStyleOptionSizeGripClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionSizeGrip = new QoreClass("QStyleOptionSizeGrip", QDOM_GUI);
   CID_QSTYLEOPTIONSIZEGRIP = QC_QStyleOptionSizeGrip->getID();

   QC_QStyleOptionSizeGrip->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionSizeGrip->setConstructor(QSTYLEOPTIONSIZEGRIP_constructor);
   QC_QStyleOptionSizeGrip->setCopy((q_copy_t)QSTYLEOPTIONSIZEGRIP_copy);

   // add special methods
   QC_QStyleOptionSizeGrip->addMethod("memberNotification",   (q_method_t)QSTYLEOPTIONSIZEGRIP_memberNotification);
   QC_QStyleOptionSizeGrip->addMethod("memberGate",           (q_method_t)QSTYLEOPTIONSIZEGRIP_memberGate);

   return QC_QStyleOptionSizeGrip;
}
