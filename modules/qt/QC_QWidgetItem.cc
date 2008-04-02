/*
 QC_QWidgetItem.cc
 
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

#include "QC_QWidgetItem.h"

qore_classid_t CID_QWIDGETITEM;
QoreClass *QC_QWidgetItem = 0;

//QWidgetItem ( QWidget * widget )
static void QWIDGETITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGETITEM-CONSTRUCTOR-PARAM-ERROR", "expecting a QWidget object as first argument to QWidgetItem::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   self->setPrivate(CID_QWIDGETITEM, new QoreQWidgetItem(self, widget->getQWidget()));
   //printd(0, "widget=%08p parent=%08p\n", widget->getQWidget(), widget->getQWidget()->parent());
   widget->setExternallyOwned();
   return;
}

static void QWIDGETITEM_copy(QoreObject *self, QoreObject *old, QoreQWidgetItem *qwi, ExceptionSink *xsink)
{
   xsink->raiseException("QWIDGETITEM-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQWidgetItemClass(QoreClass *qlayoutitem)
{
   QC_QWidgetItem = new QoreClass("QWidgetItem", QDOM_GUI);
   CID_QWIDGETITEM = QC_QWidgetItem->getID();

   QC_QWidgetItem->addBuiltinVirtualBaseClass(qlayoutitem);

   QC_QWidgetItem->setConstructor(QWIDGETITEM_constructor);
   QC_QWidgetItem->setCopy((q_copy_t)QWIDGETITEM_copy);

   return QC_QWidgetItem;
}
