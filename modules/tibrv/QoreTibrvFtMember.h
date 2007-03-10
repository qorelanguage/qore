/*
  modules/TIBCO/QoreTibrvFtMember.h

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

#ifndef _QORE_TIBCO_QORETIBRVFTMEMBER_H

#define _QORE_TIBCO_QORETIBRVFTMEMBER_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/AbstractPrivateData.h>
#include <qore/Exception.h>
#include <qore/charset.h>
#include <qore/Hash.h>

#include "QoreTibrvTransport.h"

#include <tibrv/ftcpp.h>

class QoreTibrvFtMember : public AbstractPrivateData, public QoreTibrvTransport
{
   private:
      TibrvFtMember ftMember;
      TibrvQueue queue;
      class QoreTibrvFtMemberCallback *callback;

   protected:
      virtual ~QoreTibrvFtMember();

   public:
      QoreTibrvFtMember(const char *groupname, int weight, int activeGoal, int64 heartbeat, int64 prep, int64 activation,
			const char *desc, const char *service, const char *network, const char *daemon, 
			class ExceptionSink *xsink);

      inline QoreNode *getEvent(class ExceptionSink *xsink);

      inline void stop()
      {
	 ftMember.destroy();
	 queue.destroy();
      }

      inline void setWeight(int weight, class ExceptionSink *xsink)
      {
	 TibrvStatus status = ftMember.setWeight((tibrv_u16)weight);
	 if (status != TIBRV_OK)
	    xsink->raiseException("TIBRVFTMEMEBER-SETWEIGHT-ERROR", "%s", status.getText());
      }

      inline const char *getGroupName()
      {
	 const char *groupName;
	 TibrvStatus status = ftMember.getGroupName(groupName);
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
            xsink->raiseException("TIBRVLISTENER-GETQUEUESIZE-ERROR", status.getText());
            return 0;
         }
         
         return count;
      }
};

// each dispatched event calling onFtAction() must be followed by a getEvent() call
class QoreTibrvFtMemberCallback : public TibrvFtMemberCallback
{
   private:
      class Hash *h;

      virtual void onFtAction(TibrvFtMember *ftMember, const char *groupName, tibrvftAction action)
      {
	 //printd(0, "onFtAction %s: %d\n", groupName, action);
	 h = new Hash();
	 h->setKeyValue("action", new QoreNode((int64)action), NULL);
	 h->setKeyValue("group", new QoreNode(groupName), NULL);
      }

   public:
      inline QoreTibrvFtMemberCallback()
      {
	 h = NULL;
      }

      virtual ~QoreTibrvFtMemberCallback() 
      {
	 if (h)
	    h->derefAndDelete(NULL);
      }

      class Hash *getEvent()
      {
	 class Hash *rv = h;
	 h = NULL;
	 //printd(0, "callback getEvent() returning %08p\n", rv);
	 return rv;
      }
};

inline QoreTibrvFtMember::~QoreTibrvFtMember()
{
   if (callback)
      delete callback;
}

inline QoreNode *QoreTibrvFtMember::getEvent(class ExceptionSink *xsink)
{
   while (true)
   {
      TibrvStatus status = queue.dispatch();
   
      if (status == TIBRV_TIMEOUT)
	 return NULL;

      if (status == TIBRV_INVALID_QUEUE)
      {
	 class Hash *h = new Hash();
	 h->setKeyValue("action", new QoreNode((int64)-1), NULL);
	 return new QoreNode(h);
      }
   
      if (status != TIBRV_OK)
      {
	 xsink->raiseException("TIBRVFTMEMBER-GETEVENT-ERROR", status.getText());
	 return NULL;
      }
      
      class Hash *h = callback->getEvent();
      if (!h)
	 continue;

      return new QoreNode(h);
   }
}

#endif
