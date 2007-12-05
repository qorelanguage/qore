/*
  modules/TIBCO/QoreTibrvTransport.h

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

#ifndef _QORE_TIBCO_QORETIBRVTRANSPORT_H

#define _QORE_TIBCO_QORETIBRVTRANSPORT_H

#include <qore/Qore.h>

#include <tibrv/tibrvcpp.h>

class QoreTibrvTransport
{
   public:
      class TibrvNetTransport transport;
      class QoreEncoding *enc;

      QoreTibrvTransport(const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink);

      inline void setStringEncoding(class QoreEncoding *e)
      {
	 enc = e;
      }
      inline class QoreEncoding *getStringEncoding()
      {
	 return enc;
      }

      // return -1 for error, 0 for success
      inline int send(TibrvMsg *msg, class ExceptionSink *xsink)
      {
	 TibrvStatus status = transport.send(*msg);
	 if (status != TIBRV_OK)
	 {
	    xsink->raiseException("TIBRV-SEND-ERROR", "%s", status.getText());
	    return -1;
	 }
	 return 0;
      }

      // returns 1 for timeout, -1 for error, 0 for success
      inline int sendRequest(TibrvMsg *msg, TibrvMsg *reply, int64 to, class ExceptionSink *xsink)
      {
	 // convert integer milliseconds value to float seconds value with remainder
	 tibrv_f64 timeout = (tibrv_f64)to / 1000;

	 TibrvStatus status = transport.sendRequest(*msg, *reply, timeout);
	 if (status == TIBRV_TIMEOUT)
	    return 1;

	 if (status != TIBRV_OK)
	 {
	    xsink->raiseException("TIBRV-SENDREQUEST-ERROR", "%s", status.getText());
	    return -1;
	 }
	 return 0;
      }

      int doEncodedType(TibrvMsg *msg, const char *key, const char *type, class QoreNode *val, class ExceptionSink *xsink);
      class QoreHash *parseMsg(TibrvMsg *msg, class ExceptionSink *xsink);
      class QoreHash *msgToHash(TibrvMsg *msg, class ExceptionSink *xsink);
      int hashToMsg(TibrvMsg *msg, class QoreHash *hash, class ExceptionSink *xsink);
      int valueToField(const char *key, class QoreNode *v, TibrvMsg *msg, class ExceptionSink *xsink);
      class QoreNode *fieldToNode(TibrvMsgField *field, class ExceptionSink *xsink);
      class QoreNode *listToNode(TibrvMsgField *field, class ExceptionSink *xsink);
};

#endif
