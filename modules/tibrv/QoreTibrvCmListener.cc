/*
  modules/TIBCO/QoreTibrvCmListener.cc

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

#include <qore/Qore.h>

#include "QoreTibrvCmListener.h"

QoreTibrvCmListener::QoreTibrvCmListener(const char *subject, const char *cmName, bool requestOld, const char *ledgerName, bool syncLedger, const char *relayAgent, 
					 const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink)
   : QoreTibrvCmTransport(cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon, xsink)
{
   if (xsink->isException())
   {
      callback = NULL;
      return;
   }

   callback = new QoreTibrvCmMsgCallback(this);

   // create queue
   TibrvStatus status = queue.create();
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVCMLISTENER-CONSTRUCTOR-ERROR", "cannot create queue: %s", status.getText());
      return;
   }
   
   // create cmListener
   status = cmListener.create(&queue, callback, &cmTransport, subject, NULL);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVCMLISTENER-CONSTRUCTOR-ERROR", "cannot create cmlistener on %s: %s", subject, status.getText());
      return;
   }  
}

class Hash *QoreTibrvCmListener::getMessage(class ExceptionSink *xsink)
{
   class Hash *h;

   while (true)
   {
      TibrvStatus status = queue.dispatch();

      if (status != TIBRV_OK)
      {
	 xsink->raiseException("TIBRVCMLISTENER-GETMESSAGE-ERROR", status.getText());
	 return NULL;
      }
      if ((h = callback->getMessage(xsink)))
	 return h;
   }

   return NULL;
}

class Hash *QoreTibrvCmListener::getMessage(int64 timeout_ms, class ExceptionSink *xsink)
{
   tibrv_f64 timeout = (tibrv_f64)timeout_ms / 1000.0;
   TibrvStatus status = queue.timedDispatch(timeout);

   if (status == TIBRV_TIMEOUT)
      return NULL;

   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVCMLISTENER-GETMESSAGE-ERROR", status.getText());
      return NULL;
   }

   return callback->getMessage(xsink);
}
