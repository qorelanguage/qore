/*
 QC_QCloseEvent.cc
 
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

#include "QC_QCloseEvent.h"

int CID_QCLOSEEVENT;
class QoreClass *QC_QCloseEvent = 0;

//QCloseEvent ()
static void QCLOSEEVENT_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QCLOSEEVENT, new QoreQCloseEvent());
   return;
}

static void QCLOSEEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQCloseEvent *qce, ExceptionSink *xsink)
{
   xsink->raiseException("QCLOSEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQCloseEventClass(QoreClass *qevent)
{
   QC_QCloseEvent = new QoreClass("QCloseEvent", QDOM_GUI);
   CID_QCLOSEEVENT = QC_QCloseEvent->getID();

   QC_QCloseEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QCloseEvent->setConstructor(QCLOSEEVENT_constructor);
   QC_QCloseEvent->setCopy((q_copy_t)QCLOSEEVENT_copy);


   return QC_QCloseEvent;
}
