/*
  modules/TIBCO/QoreTibrvCmSender.h

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

#ifndef _QORE_TIBCO_QORETIBRVCMSENDER_H

#define _QORE_TIBCO_QORETIBRVCMSENDER_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/AbstractPrivateData.h>
#include <qore/Exception.h>
#include <qore/charset.h>

#include "QoreTibrvCmTransport.h"

class QoreTibrvCmSender : public AbstractPrivateData, public QoreTibrvCmTransport
{
   private:

   protected:
      virtual ~QoreTibrvCmSender() {}

   public:
      inline QoreTibrvCmSender(char *cmName, bool requestOld, char *ledgerName, bool syncLedger, char *relayAgent, 
			       char *desc, char *service, char *network, char *daemon, class ExceptionSink *xsink) 
	 : QoreTibrvCmTransport(cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon, xsink)  { }

      // time_limit for certified delivery in ms
      void sendSubject(char *subject, class Hash *data, char *replySubject, int64 time_limit, class ExceptionSink *xsink);

      // timeout and time_limit for certified delivery in ms
      class Hash *sendSubjectWithSyncReply(char *subject, class Hash *data, int64 timeout, int64 time_limit, class ExceptionSink *xsink);
};

#endif
