/*
  modules/TIBCO/QoreTibrvTransport.h

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

#ifndef _QORE_TIBCO_QORETIBRVTRANSPORT_H

#define _QORE_TIBCO_QORETIBRVTRANSPORT_H

#include <qore/common.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/charset.h>

#include <tibrv/tibrvcpp.h>

class QoreTibrvTransport
{
   public:
      class TibrvNetTransport transport;
      class QoreEncoding *enc;

      QoreTibrvTransport(char *service, char *network, char *daemon, char *desc, class ExceptionSink *xsink);

      void sendSubject(char *subject, class Hash *data, class ExceptionSink *xsink);

      inline void setStringEncoding(class QoreEncoding *e)
      {
	 enc = e;
      }
      inline class QoreEncoding *getStringEncoding()
      {
	 return enc;
      }

      inline int send(TibrvMsg msg, class ExceptionSink *xsink)
      {
	 TibrvStatus status = transport.send(msg);
	 if (status != TIBRV_OK)
	 {
	    xsink->raiseException("TIBRVSENDER-ERROR", "%s", (char *)status.getText());
	    return -1;
	 }
	 return 0;
      }

      class Hash *msgToHash(TibrvMsg *msg, class ExceptionSink *xsink);
      int hashToMsg(TibrvMsg *msg, class Hash *hash, class ExceptionSink *xsink);
      int valueToField(char *key, class QoreNode *v, TibrvMsg *msg, class ExceptionSink *xsink);
      class QoreNode *fieldToNode(TibrvMsgField *field, class ExceptionSink *xsink);
      class QoreNode *listToNode(TibrvMsgField *field, class ExceptionSink *xsink);

};

#endif
