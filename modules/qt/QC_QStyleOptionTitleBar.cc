/*
 QC_QStyleOptionTitleBar.cc
 
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

#include "QC_QStyleOptionTitleBar.h"

int CID_QSTYLEOPTIONTITLEBAR;
class QoreClass *QC_QStyleOptionTitleBar = 0;

//QStyleOptionTitleBar ()
//QStyleOptionTitleBar ( const QStyleOptionTitleBar & other )
static void QSTYLEOPTIONTITLEBAR_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTITLEBAR, new QoreQStyleOptionTitleBar());
   return;
}

static void QSTYLEOPTIONTITLEBAR_copy(class Object *self, class Object *old, class QoreQStyleOptionTitleBar *qsotb, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLEOPTIONTITLEBAR-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQStyleOptionTitleBarClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionTitleBar = new QoreClass("QStyleOptionTitleBar", QDOM_GUI);
   CID_QSTYLEOPTIONTITLEBAR = QC_QStyleOptionTitleBar->getID();

   QC_QStyleOptionTitleBar->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionTitleBar->setConstructor(QSTYLEOPTIONTITLEBAR_constructor);
   QC_QStyleOptionTitleBar->setCopy((q_copy_t)QSTYLEOPTIONTITLEBAR_copy);


   return QC_QStyleOptionTitleBar;
}
