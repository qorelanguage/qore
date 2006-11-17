/*
  modules/TIBCO/QoreTibrvFtMonitor.h

  TIBCO Rendezvous integration to QORE

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

#ifndef _QORE_TIBCO_QORETIBRVFTMONITOR_H

#define _QORE_TIBCO_QORETIBRVFTMONITOR_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/ReferenceObject.h>
#include <qore/Exception.h>
#include <qore/charset.h>
#include <qore/Hash.h>

#include "QoreTibrvTransport.h"

#include <tibrv/ftcpp.h>

class QoreTibrvFtMonitor : public ReferenceObject, public QoreTibrvTransport
{
   private:
      TibrvFtMonitor ftMonitor;
      TibrvQueue queue;
      class QoreTibrvFtMonitorCallback *callback;

   protected:
      inline ~QoreTibrvFtMonitor();

   public:
      QoreTibrvFtMonitor(char *groupname, int64 lostInterval,
			 char *desc, char *service, char *network, char *daemon, 
			 class ExceptionSink *xsink);

      inline QoreNode *getEvent(class ExceptionSink *xsink);

      inline void stop()
      {
	 ftMonitor.destroy();
	 queue.destroy();
      }

      inline const char *getGroupName()
      {
	 const char *groupName;
	 TibrvStatus status = ftMonitor.getGroupName(groupName);
	 if (status == TIBRV_OK)
	    return groupName;
	 return NULL;
      }
      
      inline int getQueueSize(class ExceptionSink *xsink)
      {
         tibrv_u32 count;
         TibrvStatus status = queue.getCount(count);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRVLISTENER-GETQUEUESIZE-ERROR", (char *)status.getText());
            return 0;
         }
         
         return count;
      }

      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

// each dispatched event calling onFtAction() must be followed by a getEvent() call
class QoreTibrvFtMonitorCallback : public TibrvFtMonitorCallback
{
   private:
      class Hash *h;

      virtual void onFtMonitor(TibrvFtMonitor *ftMonitor, const char *groupName, tibrv_u32 numActiveMembers)
      {
	 //printd(5, "onFtAction %s: %d\n", groupName, action);
	 h = new Hash();
	 h->setKeyValue("group", new QoreNode(groupName), NULL);
	 h->setKeyValue("numActiveMembers", new QoreNode((int64)numActiveMembers), NULL);
      }

   public:
      inline QoreTibrvFtMonitorCallback()
      {
	 h = NULL;
      }

      virtual ~QoreTibrvFtMonitorCallback() 
      {
	 if (h)
	 {
	    h->dereference(NULL);
	    delete h;
	 }
      }

      class Hash *getEvent()
      {
	 class Hash *rv = h;
	 h = NULL;
	 //printd(0, "callback getEvent() returning %08p\n", rv);
	 return rv;
      }
};

inline QoreTibrvFtMonitor::~QoreTibrvFtMonitor()
{
   stop();
   if (callback)
      delete callback;
}

inline QoreNode *QoreTibrvFtMonitor::getEvent(class ExceptionSink *xsink)
{
   while (true)
   {
      TibrvStatus status = queue.dispatch();
   
      if (status == TIBRV_TIMEOUT)
	 return NULL;

      if (status == TIBRV_INVALID_QUEUE)
      {
	 class Hash *h = new Hash();
	 h->setKeyValue("action", new QoreNode(-1LL), NULL);
	 return new QoreNode(h);
      }
   
      if (status != TIBRV_OK)
      {
	 xsink->raiseException("TIBRVFTMONITOR-GETEVENT-ERROR", (char *)status.getText());
	 return NULL;
      }
      
      class Hash *h = callback->getEvent();
      if (!h)
	 continue;

      return new QoreNode(h);
   }
}

#endif
