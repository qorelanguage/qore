/*
  modules/TIBCO/QoreTibrvCmTransport.cc

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>

#include "QoreTibrvCmTransport.h"

QoreTibrvCmTransport::QoreTibrvCmTransport(char *cmName, bool requestOld, char *ledgerName, bool syncLedger, char *relayAgent, 
					   char *desc, char *service, char *network, char *daemon, class ExceptionSink *xsink) 
   : QoreTibrvTransport(desc, service, network, daemon, xsink)
{
   if (xsink->isException())
      return;

   TibrvStatus status = cmTransport.create(&transport, (const char*)cmName, (tibrv_bool)requestOld, (const char *)ledgerName, (tibrv_bool)syncLedger, (const char *)relayAgent);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRV-CMTRANSPORT-ERROR", "%s", (char *)status.getText());
      return;
   }
}
