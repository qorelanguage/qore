/*
 QC_QStyleOptionSizeGrip.cc
 
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

#include "QC_QStyleOptionSizeGrip.h"

int CID_QSTYLEOPTIONSIZEGRIP;
class QoreClass *QC_QStyleOptionSizeGrip = 0;

//QStyleOptionSizeGrip ()
//QStyleOptionSizeGrip ( const QStyleOptionSizeGrip & other )
static void QSTYLEOPTIONSIZEGRIP_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONSIZEGRIP, new QoreQStyleOptionSizeGrip());
   return;
}

static void QSTYLEOPTIONSIZEGRIP_copy(class Object *self, class Object *old, class QoreQStyleOptionSizeGrip *qsosg, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLEOPTIONSIZEGRIP-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQStyleOptionSizeGripClass(QoreClass *qstyleoptioncomplex)
{
   QC_QStyleOptionSizeGrip = new QoreClass("QStyleOptionSizeGrip", QDOM_GUI);
   CID_QSTYLEOPTIONSIZEGRIP = QC_QStyleOptionSizeGrip->getID();

   QC_QStyleOptionSizeGrip->addBuiltinVirtualBaseClass(qstyleoptioncomplex);

   QC_QStyleOptionSizeGrip->setConstructor(QSTYLEOPTIONSIZEGRIP_constructor);
   QC_QStyleOptionSizeGrip->setCopy((q_copy_t)QSTYLEOPTIONSIZEGRIP_copy);


   return QC_QStyleOptionSizeGrip;
}
