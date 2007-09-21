/*
 QC_QStyleOptionButton.cc
 
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

#include "QC_QStyleOptionButton.h"

int CID_QSTYLEOPTIONBUTTON;
class QoreClass *QC_QStyleOptionButton = 0;

//QStyleOptionButton ()
//QStyleOptionButton ( const QStyleOptionButton & other )
static void QSTYLEOPTIONBUTTON_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONBUTTON, new QoreQStyleOptionButton());
   return;
}

static void QSTYLEOPTIONBUTTON_copy(class Object *self, class Object *old, class QoreQStyleOptionButton *qsob, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLEOPTIONBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQStyleOptionButtonClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionButton = new QoreClass("QStyleOptionButton", QDOM_GUI);
   CID_QSTYLEOPTIONBUTTON = QC_QStyleOptionButton->getID();

   QC_QStyleOptionButton->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionButton->setConstructor(QSTYLEOPTIONBUTTON_constructor);
   QC_QStyleOptionButton->setCopy((q_copy_t)QSTYLEOPTIONBUTTON_copy);


   return QC_QStyleOptionButton;
}
