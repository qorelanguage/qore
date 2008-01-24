/*
 QC_QDragLeaveEvent.cc
 
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

#include "QC_QDragLeaveEvent.h"

int CID_QDRAGLEAVEEVENT;
class QoreClass *QC_QDragLeaveEvent = 0;

//QDragLeaveEvent ()
static void QDRAGLEAVEEVENT_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QDRAGLEAVEEVENT, new QoreQDragLeaveEvent());
   return;
}

static void QDRAGLEAVEEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQDragLeaveEvent *qdle, ExceptionSink *xsink)
{
   xsink->raiseException("QDRAGLEAVEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQDragLeaveEventClass(QoreClass *qevent)
{
   QC_QDragLeaveEvent = new QoreClass("QDragLeaveEvent", QDOM_GUI);
   CID_QDRAGLEAVEEVENT = QC_QDragLeaveEvent->getID();

   QC_QDragLeaveEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QDragLeaveEvent->setConstructor(QDRAGLEAVEEVENT_constructor);
   QC_QDragLeaveEvent->setCopy((q_copy_t)QDRAGLEAVEEVENT_copy);


   return QC_QDragLeaveEvent;
}
