/*
 QC_QStyleOptionViewItemV2.cc
 
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

#include "QC_QStyleOptionViewItemV2.h"

int CID_QSTYLEOPTIONVIEWITEMV2;
class QoreClass *QC_QStyleOptionViewItemV2 = 0;

//QStyleOptionViewItemV2 ()
//QStyleOptionViewItemV2 ( const QStyleOptionViewItemV2 & other )
//QStyleOptionViewItemV2 ( const QStyleOptionViewItem & other )
static void QSTYLEOPTIONVIEWITEMV2_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSTYLEOPTIONVIEWITEMV2, new QoreQStyleOptionViewItemV2());
      return;
   }
   QoreQStyleOptionViewItem *other = p ? (QoreQStyleOptionViewItem *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONVIEWITEM, xsink) : 0;
   if (!other) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTIONVIEWITEMV2-QSTYLEOPTIONVIEWITEMV2-PARAM-ERROR", "this version of QStyleOptionViewItemV2::QStyleOptionViewItemV2() expects an object derived from QStyleOptionViewItem as the first argument", p->val.object->getClass()->getName());
      return;
   }
   ReferenceHolder<QoreQStyleOptionViewItem> otherHolder(other, xsink);
   self->setPrivate(CID_QSTYLEOPTIONVIEWITEMV2, new QoreQStyleOptionViewItemV2(*(static_cast<QStyleOptionViewItem *>(other))));
   return;
}

static void QSTYLEOPTIONVIEWITEMV2_copy(class Object *self, class Object *old, class QoreQStyleOptionViewItemV2 *qsoviv2, ExceptionSink *xsink)
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


   return QC_QStyleOptionViewItemV2;
}
