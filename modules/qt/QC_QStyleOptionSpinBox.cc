/*
 QC_QStyleOptionSpinBox.cc
 
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

#include "QC_QStyleOptionSpinBox.h"

int CID_QSTYLEOPTIONSPINBOX;
class QoreClass *QC_QStyleOptionSpinBox = 0;

//QStyleOptionSpinBox ()
//QStyleOptionSpinBox ( const QStyleOptionSpinBox & other )
static void QSTYLEOPTIONSPINBOX_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSPINBOX, new QoreQStyleOptionSpinBox());
}

static void QSTYLEOPTIONSPINBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionSpinBox *qsosb, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSPINBOX, new QoreQStyleOptionSpinBox(*qsosb));
}

QoreClass *initQStyleOptionSpinBoxClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionSpinBox = new QoreClass("QStyleOptionSpinBox", QDOM_GUI);
   CID_QSTYLEOPTIONSPINBOX = QC_QStyleOptionSpinBox->getID();

   QC_QStyleOptionSpinBox->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionSpinBox->setConstructor(QSTYLEOPTIONSPINBOX_constructor);
   QC_QStyleOptionSpinBox->setCopy((q_copy_t)QSTYLEOPTIONSPINBOX_copy);


   return QC_QStyleOptionSpinBox;
}
