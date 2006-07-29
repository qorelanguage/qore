/*
  modules/TIBCO/QoreTibrvFtMember.h

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

#ifndef _QORE_TIBCO_QORETIBRVFTMEMBER_H

#define _QORE_TIBCO_QORETIBRVFTMEMBER_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/ReferenceObject.h>
#include <qore/Exception.h>
#include <qore/charset.h>
#include <qore/QoreQueue.h>
#include <qore/Hash.h>

#include "QoreTibrvTransport.h"

#include <tibrv/ftcpp.h>

class QoreTibrvFtMember : public ReferenceObject, public QoreTibrvTransport
{
   private:
      TibrvFtMember ftMember;
      TibrvQueue queue;
      class QoreTibrvFtMemberCallback *callback;
      class QoreQueue qoreq;
      
   protected:
      inline ~QoreTibrvFtMember();

   public:
      QoreTibrvFtMember(char *groupname, int weight, int activeGoal, int64 heartbeat, int64 prep, int64 activation,
			char *service, char *network, char *daemon, char *desc, 
			class ExceptionSink *xsink);

      inline QoreNode *getEvent(int timeout_ms)
      {
	 bool to;
	 return qoreq.shift(timeout_ms, &to);
      }

      inline QoreNode *getEvent()
      {
	 return qoreq.shift();
      }

      inline void stop()
      {
	 const char *groupName;
	 TibrvStatus status = ftMember.getGroupName(groupName);
	 class QoreNode *gn = new QoreNode((char *)groupName);

	 ftMember.destroy();

	 class Hash *h = new Hash();
	 h->setKeyValue("action", new QoreNode((int64)-1), NULL);

	 if (status == TIBRV_OK)
	    h->setKeyValue("group", gn, NULL);

	 qoreq.push(new QoreNode(h));
      }

      inline void setWeight(int weight, class ExceptionSink *xsink)
      {
	 TibrvStatus status = ftMember.setWeight((tibrv_u16)weight);
	 if (status != TIBRV_OK)
	    xsink->raiseException("TIBRVFTMEMEBER-SETWEIGHT-ERROR", "%s", (char *)status.getText());
      }

      inline const char *getGroupName()
      {
	 const char *groupName;
	 TibrvStatus status = ftMember.getGroupName(groupName);
	 if (status == TIBRV_OK)
	    return groupName;
	 return NULL;
      }
      
      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

class QoreTibrvFtMemberCallback : public TibrvFtMemberCallback
{
   private:
      class QoreQueue *qoreq;

      virtual void onFtAction(TibrvFtMember *ftMember, const char *groupName, tibrvftAction action)
      {
	 printd(0, "onFtAction %s: %d\n", groupName, action);
	 class Hash *h = new Hash();
	 h->setKeyValue("action", new QoreNode((int64)action), NULL);
	 h->setKeyValue("group", new QoreNode(groupName), NULL);
	 qoreq->push(new QoreNode(h));
      }

   public:
      inline QoreTibrvFtMemberCallback(class QoreQueue *q)
      {
	 qoreq = q;
      }

      virtual ~QoreTibrvFtMemberCallback() {}
};

inline QoreTibrvFtMember::~QoreTibrvFtMember()
{
   if (callback)
      delete callback;
}

#endif
