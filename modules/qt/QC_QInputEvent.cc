/*
 QC_QInputEvent.cc
 
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

#include "QC_QInputEvent.h"
#include "QC_QRegion.h"
#include "QC_QRect.h"

int CID_QINPUTEVENT;

class QoreClass *QC_QInputEvent = 0;

static void QINPUTEVENT_constructor(class QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QINPUTEVENT-CONSTRUCTOR-ERROR", "QInputEvent is an abstract base class");
}

static void QINPUTEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQInputEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QINPUTEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::KeyboardModifiers modifiers () const
static QoreNode *QINPUTEVENT_modifiers(QoreObject *self, QoreQInputEvent *qie, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qie->modifiers());
}

class QoreClass *initQInputEventClass(class QoreClass *qevent)
{
   tracein("initQInputEventClass()");
   
   QC_QInputEvent = new QoreClass("QInputEvent", QDOM_GUI);
   CID_QINPUTEVENT = QC_QInputEvent->getID();

   QC_QInputEvent->addBuiltinVirtualBaseClass(qevent);

   QC_QInputEvent->setConstructor(QINPUTEVENT_constructor);
   QC_QInputEvent->setCopy((q_copy_t)QINPUTEVENT_copy);

   QC_QInputEvent->addMethod("modifiers",                   (q_method_t)QINPUTEVENT_modifiers);

   traceout("initQInputEventClass()");
   return QC_QInputEvent;
}
