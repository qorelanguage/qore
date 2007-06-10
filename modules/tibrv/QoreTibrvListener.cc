/*
  modules/TIBCO/QoreTibrvListener.cc

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

#include "QoreTibrvListener.h"

QoreTibrvListener::QoreTibrvListener(const char *subject, const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink) : QoreTibrvTransport(desc, service, network, daemon, xsink)
{
   if (xsink->isException())
   {
      callback = NULL;
      return;
   }

   callback = new QoreTibrvMsgCallback(this);

   // create queue
   TibrvStatus status = queue.create();
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create queue: %s", status.getText());
      return;
   }
   
   // create listener
   status = listener.create(&queue, callback, &transport, subject);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create listener on %s: %s", subject, status.getText());
      return;
   }
   //printd(5, "created listener subj=%s, desc=%s, service=%s, network=%s, daemon=%s\n", subject, desc ? desc : "n/a", service ? service : "n/a", network ? network : "n/a", daemon ? daemon : "n/a");
}

class Hash *QoreTibrvListener::getMessage(class ExceptionSink *xsink)
{
   class Hash *h;

   while (true)
   {
      TibrvStatus status = queue.dispatch();

      if (status != TIBRV_OK)
      {
	 xsink->raiseException("TIBRVLISTENER-GETMESSAGE-ERROR", status.getText());
	 return NULL;
      }
      if ((h = callback->getMessage(xsink)))
	 return h;
   }

   return NULL;
}

class Hash *QoreTibrvListener::getMessage(int64 timeout_ms, class ExceptionSink *xsink)
{
   tibrv_f64 timeout = (tibrv_f64)timeout_ms / 1000.0;
   TibrvStatus status = queue.timedDispatch(timeout);

   if (status == TIBRV_TIMEOUT)
      return NULL;

   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-GETMESSAGE-ERROR", status.getText());
      return NULL;
   }

   return callback->getMessage(xsink);
}
