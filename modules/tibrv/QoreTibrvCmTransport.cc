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

#include <qore/Qore.h>

#include "QoreTibrvCmTransport.h"

QoreTibrvCmTransport::QoreTibrvCmTransport(const char *cmName, bool requestOld, const char *ledgerName, bool syncLedger, const char *relayAgent, 
					   const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink) 
   : QoreTibrvTransport(desc, service, network, daemon, xsink)
{
   if (xsink->isException())
      return;

   TibrvStatus status = cmTransport.create(&transport, cmName, (tibrv_bool)requestOld, ledgerName, (tibrv_bool)syncLedger, relayAgent);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRV-CMTRANSPORT-ERROR", "%s", status.getText());
      return;
   }
}
