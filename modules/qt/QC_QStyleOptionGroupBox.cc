/*
 QC_QStyleOptionGroupBox.cc
 
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

#include "QC_QStyleOptionGroupBox.h"

int CID_QSTYLEOPTIONGROUPBOX;
class QoreClass *QC_QStyleOptionGroupBox = 0;

//QStyleOptionGroupBox ()
//QStyleOptionGroupBox ( const QStyleOptionGroupBox & other )
static void QSTYLEOPTIONGROUPBOX_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONGROUPBOX, new QoreQStyleOptionGroupBox());
}

static void QSTYLEOPTIONGROUPBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionGroupBox *qsogb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONGROUPBOX, new QoreQStyleOptionGroupBox(*qsogb));
}

QoreClass *initQStyleOptionGroupBoxClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionGroupBox = new QoreClass("QStyleOptionGroupBox", QDOM_GUI);
   CID_QSTYLEOPTIONGROUPBOX = QC_QStyleOptionGroupBox->getID();

   QC_QStyleOptionGroupBox->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionGroupBox->setConstructor(QSTYLEOPTIONGROUPBOX_constructor);
   QC_QStyleOptionGroupBox->setCopy((q_copy_t)QSTYLEOPTIONGROUPBOX_copy);


   return QC_QStyleOptionGroupBox;
}
