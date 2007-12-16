/*
 QC_QHideEvent.cc
 
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

#include "QC_QHideEvent.h"

int CID_QHIDEEVENT;
class QoreClass *QC_QHideEvent = 0;

//QHideEvent ()
static void QHIDEEVENT_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QHIDEEVENT, new QoreQHideEvent());
   return;
}

static void QHIDEEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQHideEvent *qhe, ExceptionSink *xsink)
{
   xsink->raiseException("QHIDEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQHideEventClass(QoreClass *qevent)
{
   QC_QHideEvent = new QoreClass("QHideEvent", QDOM_GUI);
   CID_QHIDEEVENT = QC_QHideEvent->getID();

   QC_QHideEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QHideEvent->setConstructor(QHIDEEVENT_constructor);
   QC_QHideEvent->setCopy((q_copy_t)QHIDEEVENT_copy);


   return QC_QHideEvent;
}
