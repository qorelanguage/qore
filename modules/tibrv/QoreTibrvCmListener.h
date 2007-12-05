/*
  modules/TIBCO/QoreTibrvCmListener.h

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

#ifndef _QORE_TIBCO_QORETIBRVCMLISTENER_H

#define _QORE_TIBCO_QORETIBRVCMLISTENER_H

#include <qore/Qore.h>

#include "QoreTibrvCmTransport.h"

class QoreTibrvCmListener : public AbstractPrivateData, public QoreTibrvCmTransport
{
   private:
      class TibrvCmListener cmListener;
      class TibrvQueue queue;
      class QoreTibrvCmMsgCallback *callback;

   protected:
      virtual ~QoreTibrvCmListener();

   public:
      QoreTibrvCmListener(const char *subject, const char *cmName, bool requestOld, const char *ledgerName, bool syncLedger, const char *relayAgent, 
			  const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink);

      inline int getQueueSize(class ExceptionSink *xsink)
      {
	 tibrv_u32 count;
	 TibrvStatus status = queue.getCount(count);
	 if (status != TIBRV_OK)
	 {
	    xsink->raiseException("TIBRVCMLISTENER-GETQUEUESIZE-ERROR", status.getText());
	    return 0;
	 }
	 
	 return count;
      }

      class QoreHash *getMessage(class ExceptionSink *xsink);
      class QoreHash *getMessage(int64 timeout, class ExceptionSink *xsink);

      class QoreString *createInboxName(class ExceptionSink *xsink)
      {
	 char name[120];
	 
	 TibrvStatus status = transport.createInbox(name, 119);
	 if (status != TIBRV_OK)
	 {
	    xsink->raiseException("TIBRVCMLISTENER-CREATEINBOXNAME-ERROR", "cannot create inbox name: %s", status.getText());
	    return NULL;
	 }
	 return new QoreString(name);
      }
};

// each dispatched event calling onMsg() must be followed by a getMessage() call
class QoreTibrvCmMsgCallback : public TibrvCmMsgCallback
{
   private:
      class QoreTibrvCmListener *ql;
      class ExceptionSink xsink;
      class QoreHash *h;

      virtual void onCmMsg(TibrvCmListener *cmListener, TibrvMsg &msg)
      {
	 h = ql->parseMsg(&msg, &xsink);
	 // add certified information if available
	 if (h)
	 {
	    const char *name;
	    if (TibrvCmMsg::getSender(msg, name) == TIBRV_OK)
	       h->setKeyValue("cmSender", new QoreNode(name), NULL);
	    
	    tibrv_u64 seq;
	    if (TibrvCmMsg::getSequence(msg, seq) == TIBRV_OK)
	       h->setKeyValue("cmSequence", new QoreNode((int64)seq), NULL);
	 }
      }

   public:
      inline QoreTibrvCmMsgCallback(class QoreTibrvCmListener *l)
      {
	 ql = l;
	 h = NULL;
      }

      virtual ~QoreTibrvCmMsgCallback()
      {
	 if (h)
	    h->derefAndDelete(&xsink);
      }

      inline class QoreHash *getMessage(class ExceptionSink *xs)
      {
	 if (xsink.isException())
	 {
	    xs->assimilate(&xsink);
	    return NULL;
	 }
	 class QoreHash *rv = h;
	 h = NULL;
	 return rv;
      }
};

inline QoreTibrvCmListener::~QoreTibrvCmListener() 
{ 
   if (callback)
      delete callback; 
}

#endif
