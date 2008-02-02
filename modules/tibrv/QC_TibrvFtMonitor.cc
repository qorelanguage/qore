/*
  modules/TIBCO/TibrvFtMonitor.cc

  TIBCO integration to QORE

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

#include "QC_TibrvFtMonitor.h"

int CID_TIBRVFTMONITOR;

// syntax: group name, lostInterval, [service, network, daemon, desc] 
static void TIBRVFTMONITOR_constructor(class QoreObject *self, const QoreListNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMONITOR_constructor");

   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVFTMONITOR-CONSTRUCTOR-ERROR", "missing fault-tolerant group name as first parameter to TibrvFtMonitor::constructor()");
      return;
   }
   const char *groupName = pt->getBuffer();

   int64 lostInterval;
   AbstractQoreNode *pn = get_param(params, 1);
   lostInterval = pn ? pn->getAsInt() : 0;
   if (lostInterval <= 0)
   {
      xsink->raiseException("TIBRVFTMONITOR-CONSTRUCTOR-ERROR", "lostInterval must be greater than zero (value passed: %lld)", lostInterval);
      return;
   }

   const char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_string_param(params, 2);
   if (pt)
      service = pt->getBuffer();
   pt = test_string_param(params, 3);
   if (pt)
      network = pt->getBuffer();
   pt = test_string_param(params, 4);
   if (pt)
      daemon = pt->getBuffer();
   pt = test_string_param(params, 5);
   if (pt)
      desc = pt->getBuffer();

   class QoreTibrvFtMonitor *qftmonitor = new QoreTibrvFtMonitor(groupName, lostInterval, service, network, daemon, desc, xsink);

   if (xsink->isException())
      qftmonitor->deref();
   else
      self->setPrivate(CID_TIBRVFTMONITOR, qftmonitor);

   traceout("TIBRVFTMONITOR_constructor");
}

static void TIBRVFTMONITOR_destructor(class QoreObject *self, class QoreTibrvFtMonitor *ftm, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMONITOR_destructor()");
   ftm->stop();
   ftm->deref();
   traceout("TIBRVFTMONITOR_destructor()");
}

static void TIBRVFTMONITOR_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvFtMonitor *ftm, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVFTMONITOR-COPY-ERROR", "copying TibrvFtMonitor objects is curently not supported");
}

static AbstractQoreNode *TIBRVFTMONITOR_getEvent(class QoreObject *self, class QoreTibrvFtMonitor *ftm, const QoreListNode *params, ExceptionSink *xsink)
{
   return ftm->getEvent(xsink);
}

static AbstractQoreNode *TIBRVFTMONITOR_stop(class QoreObject *self, class QoreTibrvFtMonitor *ftm, const QoreListNode *params, ExceptionSink *xsink)
{
   ftm->stop();
   return NULL;
}

class AbstractQoreNode *TIBRVFTMONITOR_getGroupName(class QoreObject *self, class QoreTibrvFtMonitor *ftm, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *name = ftm->getGroupName();
   if (name)
      return new QoreStringNode((char *)name);

   return NULL;
}

class QoreClass *initTibrvFtMonitorClass()
{
   tracein("initTibrvFtMonitorClass()");

   class QoreClass *QC_TIBRVFTMONITOR = new QoreClass("TibrvFtMonitor", QDOM_NETWORK);
   CID_TIBRVFTMONITOR = QC_TIBRVFTMONITOR->getID();
   QC_TIBRVFTMONITOR->setConstructor(TIBRVFTMONITOR_constructor);
   QC_TIBRVFTMONITOR->setDestructor((q_destructor_t)TIBRVFTMONITOR_destructor);
   QC_TIBRVFTMONITOR->setCopy((q_copy_t)TIBRVFTMONITOR_copy);
   QC_TIBRVFTMONITOR->addMethod("getEvent",      (q_method_t)TIBRVFTMONITOR_getEvent);
   QC_TIBRVFTMONITOR->addMethod("stop",          (q_method_t)TIBRVFTMONITOR_stop);
   QC_TIBRVFTMONITOR->addMethod("getGroupName",  (q_method_t)TIBRVFTMONITOR_getGroupName);

   traceout("initTibrvFtMonitorClass()");
   return QC_TIBRVFTMONITOR;
}
