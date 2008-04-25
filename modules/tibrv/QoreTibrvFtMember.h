/*
  modules/TIBCO/QoreTibrvFtMember.h

  TIBCO Rendezvous integration to QORE

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

#ifndef _QORE_TIBCO_QORETIBRVFTMEMBER_H

#define _QORE_TIBCO_QORETIBRVFTMEMBER_H

#include <qore/Qore.h>

#include "QoreTibrvTransport.h"

#include <tibrv/ftcpp.h>

// each dispatched event calling onFtAction() must be followed by a getEvent() call
class QoreTibrvFtMemberCallback : public TibrvFtMemberCallback
{
   private:
      QoreHashNode *h;

      DLLLOCAL virtual void onFtAction(TibrvFtMember *ftMember, const char *groupName, tibrvftAction action)
      {
	 //printd(0, "onFtAction %s: %d\n", groupName, action);
	 h = new QoreHashNode();
	 h->setKeyValue("action", new QoreBigIntNode(action), NULL);
	 h->setKeyValue("group", new QoreStringNode(groupName), NULL);
      }

   public:
      DLLLOCAL QoreTibrvFtMemberCallback()
      {
	 h = NULL;
      }

      DLLLOCAL virtual ~QoreTibrvFtMemberCallback() 
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

class QoreTibrvFtMember : public AbstractPrivateData, public QoreTibrvTransport
{
   private:
      TibrvFtMember ftMember;
      TibrvQueue queue;
      class QoreTibrvFtMemberCallback *callback;

   protected:
      DLLLOCAL virtual ~QoreTibrvFtMember()
      {
	 if (callback)
	    delete callback;
      }

   public:
      DLLLOCAL QoreTibrvFtMember(const char *groupname, int weight, int activeGoal, int64 heartbeat, int64 prep, int64 activation,
				 const char *desc, const char *service, const char *network, const char *daemon, 
				 ExceptionSink *xsink);

      DLLLOCAL QoreHashNode *getEvent(ExceptionSink *xsink)
      {
	 while (true)
	 {
	    TibrvStatus status = queue.dispatch();
	    
	    if (status == TIBRV_TIMEOUT)
	       return NULL;
	    
	    if (status == TIBRV_INVALID_QUEUE)
	    {
	       QoreHashNode *h = new QoreHashNode();
	       h->setKeyValue("action", new QoreBigIntNode(-1), NULL);
	       return h;
	    }
	    
	    if (status != TIBRV_OK)
	    {
	       xsink->raiseException("TIBRVFTMEMBER-GETEVENT-ERROR", status.getText());
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
	 ftMember.destroy();
	 queue.destroy();
      }

      DLLLOCAL void setWeight(int weight, class ExceptionSink *xsink)
      {
	 TibrvStatus status = ftMember.setWeight((tibrv_u16)weight);
	 if (status != TIBRV_OK)
	    xsink->raiseException("TIBRVFTMEMEBER-SETWEIGHT-ERROR", "%s", status.getText());
      }

      DLLLOCAL const char *getGroupName()
      {
	 const char *groupName;
	 TibrvStatus status = ftMember.getGroupName(groupName);
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
