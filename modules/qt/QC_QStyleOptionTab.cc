/*
 QC_QStyleOptionTab.cc
 
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

#include "QC_QStyleOptionTab.h"

qore_classid_t CID_QSTYLEOPTIONTAB;
class QoreClass *QC_QStyleOptionTab = 0;

//QStyleOptionTab ()
//QStyleOptionTab ( const QStyleOptionTab & other )
static void QSTYLEOPTIONTAB_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTAB, new QoreQStyleOptionTab());
}

static void QSTYLEOPTIONTAB_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionTab *qsot, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTAB, new QoreQStyleOptionTab(*qsot));
}

QoreClass *initQStyleOptionTabClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionTab = new QoreClass("QStyleOptionTab", QDOM_GUI);
   CID_QSTYLEOPTIONTAB = QC_QStyleOptionTab->getID();

   QC_QStyleOptionTab->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionTab->setConstructor(QSTYLEOPTIONTAB_constructor);
   QC_QStyleOptionTab->setCopy((q_copy_t)QSTYLEOPTIONTAB_copy);


   return QC_QStyleOptionTab;
}
