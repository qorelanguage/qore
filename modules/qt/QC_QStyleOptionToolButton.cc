/*
 QC_QStyleOptionToolButton.cc
 
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

#include "QC_QStyleOptionToolButton.h"

int CID_QSTYLEOPTIONTOOLBUTTON;
class QoreClass *QC_QStyleOptionToolButton = 0;

//QStyleOptionToolButton ()
//QStyleOptionToolButton ( const QStyleOptionToolButton & other )
static void QSTYLEOPTIONTOOLBUTTON_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTOOLBUTTON, new QoreQStyleOptionToolButton());
}

static void QSTYLEOPTIONTOOLBUTTON_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionToolButton *qsotb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONTOOLBUTTON, new QoreQStyleOptionToolButton(*qsotb));
}

QoreClass *initQStyleOptionToolButtonClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionToolButton = new QoreClass("QStyleOptionToolButton", QDOM_GUI);
   CID_QSTYLEOPTIONTOOLBUTTON = QC_QStyleOptionToolButton->getID();

   QC_QStyleOptionToolButton->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionToolButton->setConstructor(QSTYLEOPTIONTOOLBUTTON_constructor);
   QC_QStyleOptionToolButton->setCopy((q_copy_t)QSTYLEOPTIONTOOLBUTTON_copy);


   return QC_QStyleOptionToolButton;
}
