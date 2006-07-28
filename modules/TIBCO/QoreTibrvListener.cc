/*
  modules/TIBCO/QoreTibrvListener.cc

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include "QoreTibrvListener.h"

QoreTibrvListener::QoreTibrvListener(char *subject, char *service, char *network, char *daemon, char *desc, class ExceptionSink *xsink) : QoreTibrvTransport(service, network, daemon, desc, xsink)
{
   if (xsink->isException())
      return;

   callback = new QoreTibrvMsgCallback(this);

   // create queue
   TibrvStatus status = queue.create();
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create queue: %s", status.getText());
      return;
   }
   
   // create transport (connect to rvd daemon)
   status = transport.create(service, network, daemon);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create Rendezvous transport: %s", status.getText());
      return;
   }
   transport.setDescription(desc);
   
   // create listener
   status = listener.create(&queue, callback, &transport, subject);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "cannot create listener on %s: %s", subject, status.getText());
      return;
   }  
}

class Hash *QoreTibrvListener::getMessage(int64 timeout, class ExceptionSink *xsink)
{
   TibrvStatus status = queue.timedDispatch(timeout);

   if (status == TIBRV_TIMEOUT)
      return NULL;

   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVLISTENER-GETMESSAGE-ERROR", (char *)status.getText());
      return NULL;
   }

   return callback->getMessage(xsink);
}
