/*
 QC_QStyleOptionViewItemV2.cc
 
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

#include "QC_QStyleOptionViewItemV2.h"
#include "QC_QStyleOptionViewItem.h"

#include "qore-qt.h"

qore_classid_t CID_QSTYLEOPTIONVIEWITEMV2;
QoreClass *QC_QStyleOptionViewItemV2 = 0;

int QStyleOptionViewItemV2_Notification(QoreObject *obj, QStyleOptionViewItemV2 *qsob, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "features")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyleOptionViewItemV2::ViewItemFeatures features = (QStyleOptionViewItemV2::ViewItemFeatures)(p ? p->getAsInt() : 0);
      qsob->features = features;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionViewItemV2_MemberGate(QStyleOptionViewItemV2 *qsob, const char *mem)
{
   if (!strcmp(mem, "features"))
      return new QoreBigIntNode(qsob->features);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONVIEWITEMV2_memberNotification(QoreObject *self, QoreQStyleOptionViewItemV2 *qsob, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionViewItemV2_Notification(self, qsob, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsob, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONVIEWITEMV2_memberGate(QoreObject *self, QoreQStyleOptionViewItemV2 *qsob, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionViewItemV2_MemberGate(qsob, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsob, member);
}

//QStyleOptionViewItemV2 ()
//QStyleOptionViewItemV2 ( const QStyleOptionViewItemV2 & other )
//QStyleOptionViewItemV2 ( const QStyleOptionViewItem & other )
static void QSTYLEOPTIONVIEWITEMV2_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSTYLEOPTIONVIEWITEMV2, new QoreQStyleOptionViewItemV2());
      return;
   }
   QoreQStyleOptionViewItem *other = p ? (QoreQStyleOptionViewItem *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTIONVIEWITEMV2-QSTYLEOPTIONVIEWITEMV2-PARAM-ERROR", "this version of QStyleOptionViewItemV2::QStyleOptionViewItemV2() expects an object derived from QStyleOptionViewItem as the first argument");
      return;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> otherHolder(other, xsink);
   self->setPrivate(CID_QSTYLEOPTIONVIEWITEMV2, new QoreQStyleOptionViewItemV2(*(static_cast<QStyleOptionViewItem *>(other))));
   return;
}

static void QSTYLEOPTIONVIEWITEMV2_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionViewItemV2 *qsoviv2, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONVIEWITEMV2, new QoreQStyleOptionViewItemV2(*qsoviv2));
}

QoreClass *initQStyleOptionViewItemV2Class(QoreClass *qstyleoptionviewitem)
{
   QC_QStyleOptionViewItemV2 = new QoreClass("QStyleOptionViewItemV2", QDOM_GUI);
   CID_QSTYLEOPTIONVIEWITEMV2 = QC_QStyleOptionViewItemV2->getID();

   QC_QStyleOptionViewItemV2->addBuiltinVirtualBaseClass(qstyleoptionviewitem);

   QC_QStyleOptionViewItemV2->setConstructor(QSTYLEOPTIONVIEWITEMV2_constructor);
   QC_QStyleOptionViewItemV2->setCopy((q_copy_t)QSTYLEOPTIONVIEWITEMV2_copy);

   // add special methods
   QC_QStyleOptionViewItemV2->addMethod("memberNotification",          (q_method_t)QSTYLEOPTIONVIEWITEMV2_memberNotification);
   QC_QStyleOptionViewItemV2->addMethod("memberGate",                  (q_method_t)QSTYLEOPTIONVIEWITEMV2_memberGate);

   return QC_QStyleOptionViewItemV2;
}
