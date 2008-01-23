/*
  modules/TIBCO/QoreTibrvCmTransport.h

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

#ifndef _QORE_TIBCO_QORETIBRVCMTRANSPORT_H

#define _QORE_TIBCO_QORETIBRVCMTRANSPORT_H

#include <qore/Qore.h>

#include "QoreTibrvTransport.h"

#include <tibrv/cmcpp.h>

class QoreTibrvCmTransport : public QoreTibrvTransport
{
   private:
      inline int setTimeLimit(TibrvMsg *msg, int64 time_limit, class ExceptionSink *xsink)
      {
	 tibrv_f64 tl = (tibrv_f64)time_limit / 1000.0;
	 TibrvStatus status = TibrvCmMsg::setTimeLimit(*msg, tl);
	 if (status != TIBRV_OK)
	 {
	    xsink->raiseException("TIBRV-SENDCM-ERROR", "error setting delivery time limit to %lldms: %s", time_limit, status.getText());
	    return -1;
	 }
	 return 0;
      }

   public:
      TibrvCmTransport cmTransport;

      QoreTibrvCmTransport(const char *cmName, bool requestOld, const char *ledgerName, bool syncLedger, const char *relayAgent, 
			   const char *desc, const char *service, const char *network, const char *daemon, class ExceptionSink *xsink); 
      ~QoreTibrvCmTransport() {}

      // return -1 for error, 0 for success
      inline int send(TibrvMsg *msg, int64 time_limit, class ExceptionSink *xsink)
      {
	 if (time_limit && setTimeLimit(msg, time_limit, xsink))
	    return -1;

         TibrvStatus status = cmTransport.send(*msg);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-SENDCM-ERROR", "%s", status.getText());
            return -1;
         }
         return 0;
      }

      // returns 1 for timeout, -1 for error, 0 for success
      inline int sendRequest(TibrvMsg *msg, TibrvMsg *reply, int64 to, int64 time_limit, class ExceptionSink *xsink)
      {
	 if (time_limit && setTimeLimit(msg, time_limit, xsink))
	    return -1;

         // convert integer milliseconds value to float seconds value with remainder
         tibrv_f64 timeout = (tibrv_f64)to / 1000;

         TibrvStatus status = cmTransport.sendRequest(*msg, *reply, timeout);
         if (status == TIBRV_TIMEOUT)
            return 1;

         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-SENDCMREQUEST-ERROR", "%s", status.getText());
            return -1;
         }
         return 0;
      }

      inline int connectToRelayAgent(class ExceptionSink *xsink)
      {
         TibrvStatus status = cmTransport.connectToRelayAgent();
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-CONNECT-TO-RELAY-AGENT-ERROR", "%s", status.getText());
            return -1;
         }
	 return 0;
      }

      inline int disconnectFromRelayAgent(class ExceptionSink *xsink)
      {
         TibrvStatus status = cmTransport.disconnectFromRelayAgent();
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-DISCONNECT-FROM-RELAY-AGENT-ERROR", "%s", status.getText());
            return -1;
         }
	 return 0;
      }

      inline int expireMessages(const char *subject, int64 seqNum, class ExceptionSink *xsink)
      {
         TibrvStatus status = cmTransport.expireMessages(subject, (tibrv_u64)seqNum);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-EXPIRE-MESSAGES-ERROR", "%s", status.getText());
            return -1;
         }
	 return 0;	 
      }

      // returns the default time limit in milliseconds
      inline const char *getName(class ExceptionSink *xsink)
      {
	 const char *name;
	 
	 TibrvStatus status = cmTransport.getName(name);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-GET-NAME-ERROR", "%s", (char *)status.getText());
            return NULL;
         }
	 return name;
      }

      // returns the default time limit in milliseconds
      inline int64 getDefaultTimeLimit(class ExceptionSink *xsink)
      {
	 tibrv_f64 tl;
	 
	 TibrvStatus status = cmTransport.getDefaultTimeLimit(tl);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-GET-DEFAULT-TIME-LIMIT-ERROR", "%s", status.getText());
            return -1;
         }
	 return (int64)tl * 1000;
      }

      // sets the default time limit from an integer value in ms
      inline int setDefaultTimeLimit(int64 time_limit, class ExceptionSink *xsink)
      {
	 tibrv_f64 tl = (tibrv_f64)time_limit / 1000.0;

	 TibrvStatus status = cmTransport.setDefaultTimeLimit(tl);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-SET-DEFAULT-TIME-LIMIT-ERROR", "%s", status.getText());
            return -1;
         }
	 return 0;
      }

      inline class QoreNode *reviewLedger(const char *subject, class ExceptionSink *xsink);

      inline int removeSendState(const char *subject, class ExceptionSink *xsink)
      {
         TibrvStatus status = cmTransport.removeSendState(subject);
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-REMOVE-SEND-STATE-ERROR", "%s", status.getText());
            return -1;
         }
	 return 0;	 
      }

      inline int syncLedger(class ExceptionSink *xsink)
      {
         TibrvStatus status = cmTransport.syncLedger();
         if (status != TIBRV_OK)
         {
            xsink->raiseException("TIBRV-SYNC-LEDGER-ERROR", "%s", status.getText());
            return -1;
         }
	 return 0;	 
      }
};

class QoreTibrvCmReviewCallback : public TibrvCmReviewCallback
{
   private:
      class QoreList *l;
      class ExceptionSink xsink;

      virtual void *onLedgerMsg(TibrvCmTransport *cmTransport, const char *subject, TibrvMsg &msg, void *closure)
      {
	 class QoreTibrvCmTransport *cmt = (class QoreTibrvCmTransport *)closure;

	 if (!l)
	    l = new QoreList();
	 
	 ReferenceHolder<QoreHashNode> h(cmt->msgToHash(&msg, &xsink), &xsink);
	 if (xsink.isException())
	 {
	    l->derefAndDelete(&xsink);
	    l = NULL;
	    return this;
	 }

	 l->push(h.release());
	 return NULL;
      }

   public:
      inline QoreTibrvCmReviewCallback()
      {
	 l = NULL;
      }

      virtual inline ~QoreTibrvCmReviewCallback()
      {
	 if (l)
	    l->derefAndDelete(&xsink);
      }

      inline void cleanup()
      {
	 if (l)
	 {
	    l->derefAndDelete(&xsink);
	    l = NULL;
	 }
      }
      
      inline class QoreNode *getLedger(class ExceptionSink *xs)
      {
         if (xsink.isException())
         {
            xs->assimilate(&xsink);
            return NULL;
         }

	 class QoreNode *rv = l ? new QoreNode(l) : NULL;
	 l = NULL;
	 return rv;
      }
};

inline class QoreNode *QoreTibrvCmTransport::reviewLedger(const char *subject, class ExceptionSink *xsink)
{
   class QoreTibrvCmReviewCallback cb;
   TibrvStatus status = cmTransport.reviewLedger(&cb, subject, this);
   if (status != TIBRV_OK)
   {
      xsink->raiseException("TIBRV-REVIEW-LEDGER-ERROR", "%s", status.getText());
      return NULL;
   }

   return cb.getLedger(xsink);
}

#endif
