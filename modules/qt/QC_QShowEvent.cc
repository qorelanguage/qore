/*
 QC_QShowEvent.cc
 
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

#include "QC_QShowEvent.h"

int CID_QSHOWEVENT;
class QoreClass *QC_QShowEvent = 0;

//QShowEvent ()
static void QSHOWEVENT_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSHOWEVENT, new QoreQShowEvent());
   return;
}

static void QSHOWEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQShowEvent *qse, ExceptionSink *xsink)
{
   xsink->raiseException("QSHOWEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQShowEventClass(QoreClass *qevent)
{
   QC_QShowEvent = new QoreClass("QShowEvent", QDOM_GUI);
   CID_QSHOWEVENT = QC_QShowEvent->getID();

   QC_QShowEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QShowEvent->setConstructor(QSHOWEVENT_constructor);
   QC_QShowEvent->setCopy((q_copy_t)QSHOWEVENT_copy);


   return QC_QShowEvent;
}
