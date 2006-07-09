/*
  modules/TIBCO/QoreTibrvMsgCallback.h

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

#ifndef _QORE_TIBCO_QORETIBRVMSGCALLBACK_H

#define _QORE_TIBCO_QORETIBRVMSGCALLBACK_H

#include <qore/common.h>
#include <qore/support.h>

#include "qore-rv.h"

#include <tibrv/tibrvcpp.h>

// each dispatched event calling onMsg() must be followed by a getMessage() call
class QoreTibrvMsgCallback : public TibrvMsgCallback
{
   private:
      class ExceptionSink xsink;
      class Hash *h;

      virtual void onMsg(TibrvListener *listener, TibrvMsg &msg)
      {
	 class Hash *data = tibrvmsg_to_hash(&msg, &xsink);
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
      inline QoreTibrvMsgCallback()
      {
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

#endif
