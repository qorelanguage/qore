/*
  modules/TIBCO/TibrvFtMonitor.cc

  TIBCO integration to QORE

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/params.h>

#include "QC_TibrvFtMonitor.h"

int CID_TIBRVFTMONITOR;

static inline void *getFTM(void *obj)
{
   ((QoreTibrvFtMonitor *)obj)->ROreference();
   return obj;
}

// syntax: group name, lostInterval, [service, network, daemon, desc] 
class QoreNode *TIBRVFTMONITOR_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMONITOR_constructor");

   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVFTMONITOR-CONSTRUCTOR-ERROR", "missing fault-tolerant group name as first parameter to TibrvFtMonitor::constructor()");
      return NULL;
   }
   char *groupName = pt->val.String->getBuffer();

   int64 lostInterval;
   pt = get_param(params, 1);
   lostInterval = pt ? pt->getAsInt() : 0;
   if (lostInterval <= 0)
   {
      xsink->raiseException("TIBRVFTMONITOR-CONSTRUCTOR-ERROR", "lostInterval must be greater than zero (value passed: %lld)", lostInterval);
      return NULL;
   }

   char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_param(params, NT_STRING, 2);
   if (pt)
      service = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 3);
   if (pt)
      network = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 4);
   if (pt)
      daemon = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 5);
   if (pt)
      desc = pt->val.String->getBuffer();

   class QoreTibrvFtMonitor *qftmonitor = new QoreTibrvFtMonitor(groupName, lostInterval, service, network, daemon, desc, xsink);

   if (xsink->isException() || self->setPrivate(CID_TIBRVFTMONITOR, qftmonitor, getFTM))
      qftmonitor->deref();

   traceout("TIBRVFTMONITOR_constructor");
   return NULL;
}

class QoreNode *TIBRVFTMONITOR_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVFTMONITOR_destructor()");

   QoreTibrvFtMonitor *ftm = (QoreTibrvFtMonitor *)self->getAndClearPrivateData(CID_TIBRVFTMONITOR);
   if (ftm)
   {
      ftm->stop();
      ftm->deref();
   }
   traceout("TIBRVFTMONITOR_destructor()");
   return NULL;
}

static QoreNode *TIBRVFTMONITOR_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVFTMONITOR-COPY-ERROR", "copying TibrvFtMonitor objects is curently not supported");
   return NULL;
}

static QoreNode *TIBRVFTMONITOR_getEvent(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvFtMonitor *ftm = (QoreTibrvFtMonitor *)self->getReferencedPrivateData(CID_TIBRVFTMONITOR);
   class QoreNode *rv = NULL;

   if (ftm)
   {
      rv = ftm->getEvent(xsink);

      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvFtMonitor::getEvent");

   return rv;
}

static QoreNode *TIBRVFTMONITOR_stop(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvFtMonitor *ftm = (QoreTibrvFtMonitor *)self->getReferencedPrivateData(CID_TIBRVFTMONITOR);

   if (ftm)
   {
      ftm->stop();
      ftm->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvFtMonitor::stop");

   return NULL;
}

class QoreNode *TIBRVFTMONITOR_getGroupName(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvFtMonitor *ftm = (QoreTibrvFtMonitor *)self->getReferencedPrivateData(CID_TIBRVFTMONITOR);
   class QoreNode *rv = NULL;

   if (ftm)
   {
      const char *name = ftm->getGroupName();
      ftm->deref();
      if (name)
	 rv = new QoreNode((char *)name);
   }
   else
      alreadyDeleted(xsink, "TibrvFtMonitor::getGroupName");

   return rv;
}

class QoreClass *initTibrvFtMonitorClass()
{
   tracein("initTibrvFtMonitorClass()");

   class QoreClass *QC_TIBRVFTMONITOR = new QoreClass(strdup("TibrvFtMonitor"));
   CID_TIBRVFTMONITOR = QC_TIBRVFTMONITOR->getID();
   QC_TIBRVFTMONITOR->addMethod("constructor",   TIBRVFTMONITOR_constructor);
   QC_TIBRVFTMONITOR->addMethod("destructor",    TIBRVFTMONITOR_destructor);
   QC_TIBRVFTMONITOR->addMethod("copy",          TIBRVFTMONITOR_copy);
   QC_TIBRVFTMONITOR->addMethod("getEvent",      TIBRVFTMONITOR_getEvent);
   QC_TIBRVFTMONITOR->addMethod("stop",          TIBRVFTMONITOR_stop);
   QC_TIBRVFTMONITOR->addMethod("getGroupName",  TIBRVFTMONITOR_getGroupName);

   traceout("initTibrvFtMonitorClass()");
   return QC_TIBRVFTMONITOR;
}
