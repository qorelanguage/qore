/*
  modules/TIBCO/QoreTibrvListener.h

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

#ifndef _QORE_TIBCO_QORETIBRVLISTENER_H

#define _QORE_TIBCO_QORETIBRVLISTENER_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/ReferenceObject.h>
#include <qore/Exception.h>
#include <qore/charset.h>

#include "QoreTibrvTransport.h"

class QoreTibrvListener : public ReferenceObject, public QoreTibrvTransport
{
   private:
      class TibrvListener listener;
      class TibrvQueue queue;
      class QoreTibrvMsgCallback *callback;

   protected:
      inline ~QoreTibrvListener();

   public:
      QoreTibrvListener(char *subject, char *service, char *network, char *daemon, char *desc, class ExceptionSink *xsink);

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

      class Hash *getMessage(int64 timeout, class ExceptionSink *xsink);

      class QoreString *createInboxName(class ExceptionSink *xsink)
      {
	 char name[120];
	 
	 TibrvStatus status = transport.createInbox(name, 119);
	 if (status != TIBRV_OK)
	 {
	    xsink->raiseException("TIBRVLISTENER-CREATEINBOXNAME-ERROR", "cannot create inbox name: %s", status.getText());
	    return NULL;
	 }
	 return new QoreString(name);
      }

      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

// each dispatched event calling onMsg() must be followed by a getMessage() call
class QoreTibrvMsgCallback : public TibrvMsgCallback
{
   private:
      class QoreTibrvListener *ql;
      class ExceptionSink xsink;
      class Hash *h;

      virtual void onMsg(TibrvListener *listener, TibrvMsg &msg)
      {
	 class Hash *data = ql->msgToHash(&msg, &xsink);
	 if (xsink.isException())
	 {
	    if (data)
	    {
	       data->dereference(&xsink);
	       delete data;
	    }
	    return;
	 }
	 
	 h = new Hash();
	 h->setKeyValue("msg", new QoreNode(data), NULL);
   
	 const char *str;
	 TibrvStatus status = msg.getReplySubject(str);
	 if (status == TIBRV_OK)
	    h->setKeyValue("replySubject", new QoreNode(str), &xsink);
   
	 status = msg.getSendSubject(str);
	 if (status == TIBRV_OK)
	    h->setKeyValue("subject", new QoreNode(str), &xsink);
      }

   public:
      inline QoreTibrvMsgCallback(class QoreTibrvListener *l)
      {
	 ql = l;
	 h = NULL;
      }

      virtual ~QoreTibrvMsgCallback()
      {
	 if (h)
	 {
	    h->dereference(&xsink);
	    delete h;
	 }
      }

      inline class Hash *getMessage(class ExceptionSink *xs)
      {
	 if (xsink.isException())
	 {
	    xs->assimilate(&xsink);
	    return NULL;
	 }
	 class Hash *rv = h;
	 h = NULL;
	 return rv;
      }
};

inline QoreTibrvListener::~QoreTibrvListener() 
{ 
   delete callback; 
}

#endif
