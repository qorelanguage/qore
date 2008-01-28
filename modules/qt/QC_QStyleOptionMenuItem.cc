/*
 QC_QStyleOptionMenuItem.cc
 
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

#include "QC_QStyleOptionMenuItem.h"

int CID_QSTYLEOPTIONMENUITEM;
class QoreClass *QC_QStyleOptionMenuItem = 0;

//QStyleOptionMenuItem ()
//QStyleOptionMenuItem ( const QStyleOptionMenuItem & other )
static void QSTYLEOPTIONMENUITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONMENUITEM, new QoreQStyleOptionMenuItem());
}

static void QSTYLEOPTIONMENUITEM_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionMenuItem *qsomi, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONMENUITEM, new QoreQStyleOptionMenuItem(*qsomi));
}

QoreClass *initQStyleOptionMenuItemClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionMenuItem = new QoreClass("QStyleOptionMenuItem", QDOM_GUI);
   CID_QSTYLEOPTIONMENUITEM = QC_QStyleOptionMenuItem->getID();

   QC_QStyleOptionMenuItem->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionMenuItem->setConstructor(QSTYLEOPTIONMENUITEM_constructor);
   QC_QStyleOptionMenuItem->setCopy((q_copy_t)QSTYLEOPTIONMENUITEM_copy);


   return QC_QStyleOptionMenuItem;
}
