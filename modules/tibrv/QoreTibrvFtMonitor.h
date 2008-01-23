/*
  modules/TIBCO/QoreTibrvFtMonitor.h

  TIBCO Rendezvous integration to QORE

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

#ifndef _QORE_TIBCO_QORETIBRVFTMONITOR_H

#define _QORE_TIBCO_QORETIBRVFTMONITOR_H

#include <qore/Qore.h>

#include "QoreTibrvTransport.h"

#include <tibrv/ftcpp.h>

// each dispatched event calling onFtAction() must be followed by a getEvent() call
class QoreTibrvFtMonitorCallback : public TibrvFtMonitorCallback
{
   private:
      QoreHashNode *h;

      DLLLOCAL virtual void onFtMonitor(TibrvFtMonitor *ftMonitor, const char *groupName, tibrv_u32 numActiveMembers)
      {
	 //printd(5, "onFtAction %s: %d\n", groupName, action);
	 h = new QoreHashNode();
	 h->setKeyValue("group", new QoreStringNode(groupName), NULL);
	 h->setKeyValue("numActiveMembers", new QoreNode((int64)numActiveMembers), NULL);
      }

   public:
      DLLLOCAL QoreTibrvFtMonitorCallback()
      {
	 h = NULL;
      }

      DLLLOCAL virtual ~QoreTibrvFtMonitorCallback() 
      {
	 if (h)
	    h->deref(NULL);
      }

      DLLLOCAL QoreHashNode *getEvent()
      {
	 QoreHashNode *rv = h;
	 h = NULL;
	 //printd(0, "callback getEvent() returning %08p\n", rv);
	 return rv;
      }
};

class QoreTibrvFtMonitor : public AbstractPrivateData, public QoreTibrvTransport
{
   private:
      TibrvFtMonitor ftMonitor;
      TibrvQueue queue;
      class QoreTibrvFtMonitorCallback *callback;

   protected:
      DLLLOCAL virtual ~QoreTibrvFtMonitor()
      {
	 stop();
	 if (callback)
	    delete callback;
      }
      
   public:
      DLLLOCAL QoreTibrvFtMonitor(const char *groupname, int64 lostInterval,
				  const char *desc, const char *service, const char *network, const char *daemon, 
				  class ExceptionSink *xsink);

      DLLLOCAL QoreHashNode *getEvent(class ExceptionSink *xsink)
      {
	 while (true)
	 {
	    TibrvStatus status = queue.dispatch();
	    
	    if (status == TIBRV_TIMEOUT)
	       return NULL;
	    
	    if (status == TIBRV_INVALID_QUEUE)
	    {
	       QoreHashNode *h = new QoreHashNode();
	       h->setKeyValue("action", new QoreNode((int64)-1), NULL);
	       return h;
	    }
	    
	    if (status != TIBRV_OK)
	    {
	       xsink->raiseException("TIBRVFTMONITOR-GETEVENT-ERROR", status.getText());
	       return NULL;
	    }
	    
	    QoreHashNode *h = callback->getEvent();
	    if (!h)
	       continue;
	    
	    return h;
	 }
      }

      DLLLOCAL void stop()
      {
	 ftMonitor.destroy();
	 queue.destroy();
      }

      DLLLOCAL const char *getGroupName()
      {
	 const char *groupName;
	 TibrvStatus status = ftMonitor.getGroupName(groupName);
	 if (status == TIBRV_OK)
	    return groupName;
	 return NULL;
      }
      
      DLLLOCAL int getQueueSize(class ExceptionSink *xsink)
      {
         tibrv_u32 count;
         TibrvStatus status = queue.getCount(count);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRVLISTENER-GETQUEUESIZE-ERROR", status.getText());
            return 0;
         }
         
         return count;
      }
};

#endif
