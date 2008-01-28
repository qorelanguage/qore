/*
 QC_QStyleOptionSlider.cc
 
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

#include "QC_QStyleOptionSlider.h"

int CID_QSTYLEOPTIONSLIDER;
class QoreClass *QC_QStyleOptionSlider = 0;

//QStyleOptionSlider ()
//QStyleOptionSlider ( const QStyleOptionSlider & other )
static void QSTYLEOPTIONSLIDER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSLIDER, new QoreQStyleOptionSlider());
}

static void QSTYLEOPTIONSLIDER_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionSlider *qsos, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSLIDER, new QoreQStyleOptionSlider(*qsos));
}

QoreClass *initQStyleOptionSliderClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionSlider = new QoreClass("QStyleOptionSlider", QDOM_GUI);
   CID_QSTYLEOPTIONSLIDER = QC_QStyleOptionSlider->getID();

   QC_QStyleOptionSlider->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionSlider->setConstructor(QSTYLEOPTIONSLIDER_constructor);
   QC_QStyleOptionSlider->setCopy((q_copy_t)QSTYLEOPTIONSLIDER_copy);


   return QC_QStyleOptionSlider;
}
