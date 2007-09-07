/*
 QC_QStyleOptionViewItem.cc
 
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

#include "QC_QStyleOptionViewItem.h"

int CID_QSTYLEOPTIONVIEWITEM;
class QoreClass *QC_QStyleOptionViewItem = 0;

//QStyleOptionViewItem ()
//QStyleOptionViewItem ( const QStyleOptionViewItem & other )
static void QSTYLEOPTIONVIEWITEM_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSTYLEOPTIONVIEWITEM, new QoreQStyleOptionViewItem());
      return;
   }
}

static void QSTYLEOPTIONVIEWITEM_copy(class Object *self, class Object *old, class QoreQStyleOptionViewItem *qsovi, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLEOPTIONVIEWITEM-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQStyleOptionViewItemClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionViewItem = new QoreClass("QStyleOptionViewItem", QDOM_GUI);
   CID_QSTYLEOPTIONVIEWITEM = QC_QStyleOptionViewItem->getID();

   QC_QStyleOptionViewItem->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionViewItem->setConstructor(QSTYLEOPTIONVIEWITEM_constructor);
   QC_QStyleOptionViewItem->setCopy((q_copy_t)QSTYLEOPTIONVIEWITEM_copy);


   return QC_QStyleOptionViewItem;
}
