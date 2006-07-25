/*
  modules/TIBCO/QoreTibrvSender.cc

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

#include "QoreTibrvSender.h"
#include "qore-rv.h"

QoreTibrvSender::QoreTibrvSender(char *service, char *network, char *daemon, char *desc, class ExceptionSink *xsink)
{
   // create transport (connect to rvd daemon)
   TibrvStatus status = transport.create(service, network, daemon);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVSENDER-CONSTRUCTOR-ERROR", "cannot create Rendezvous transport: %s", status.getText());
      return;
   }
   transport.setDescription(desc);
}

void QoreTibrvSender::sendSubject(char *subject, class Hash *data, class ExceptionSink *xsink)
{
   TibrvMsg msg;

   TibrvStatus status = msg.setSendSubject(subject);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBCO-INVALID-SUBJECT", "'%s': %s", subject, (char *)status.getText());
      return;
   }

   hash_to_tibrvmsg(&msg, data, xsink);
   if (xsink->isException())
      return;

   status = transport.send(msg);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRVSENDER-ERROR", "%s", (char *)status.getText());
      return;
   }
   //printd(0, "subject: %s msg=%08x sent OK\n", subject, data);
}
