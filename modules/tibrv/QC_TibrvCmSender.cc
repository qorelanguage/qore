/*
  modules/TIBCO/TibrvCmSender.cc

  TIBCO integration to QORE

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

#include "QC_TibrvCmSender.h"

int CID_TIBRVCMSENDER;

// syntax: [cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon] 
void TIBRVCMSENDER_constructor(class QoreObject *self, const QoreListNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVCMSENDER_constructor");

   const char *cmName = NULL, *ledgerName = NULL, *relayAgent = NULL;
   bool requestOld, syncLedger;

   QoreStringNode *pt = test_string_param(params, 0);
   if (pt)
      cmName = pt->getBuffer();

   AbstractQoreNode *pn = get_param(params, 1);
   requestOld = pn ? pn->getAsBool() : false;

   pt = test_string_param(params, 2);
   if (pt)
      ledgerName = pt->getBuffer();

   pn = get_param(params, 3);
   syncLedger = pn ? pn->getAsBool() : false;

   pt = test_string_param(params, 4);
   if (pt)
      relayAgent = pt->getBuffer();

   const char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_string_param(params, 5);
   if (pt)
      desc = pt->getBuffer();
   pt = test_string_param(params, 6);
   if (pt)
      service = pt->getBuffer();
   pt = test_string_param(params, 7);
   if (pt)
      network = pt->getBuffer();
   pt = test_string_param(params, 8);
   if (pt)
      daemon = pt->getBuffer();

   class QoreTibrvCmSender *qcmsender = new QoreTibrvCmSender(cmName, requestOld, ledgerName, syncLedger,relayAgent, desc, service, network, daemon, xsink);

   if (xsink->isException())
      qcmsender->deref();
   else
      self->setPrivate(CID_TIBRVCMSENDER, qcmsender);

   traceout("TIBRVCMSENDER_constructor");
}

void TIBRVCMSENDER_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvCmSender *cms, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVCMSENDER-COPY-ERROR", "copying TibrvCmSender objects is curently not supported");
}

static AbstractQoreNode *TIBRVCMSENDER_sendSubject(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECT-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   const char *subject = pt->getBuffer();
   QoreHashNode *h = test_hash_param(params, 1);
   if (!h)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECT-ERROR", "missing data hash as second parameter to method");
      return 0;
   }
   pt = test_string_param(params, 2);
   const char *replySubject = pt ? pt->getBuffer() : NULL;

   // get certified delivery time limit
   int64 time_limit = getMsZeroBigInt(get_param(params, 3));

   cms->sendSubject(subject, h, replySubject, time_limit, xsink);

   return NULL;
}

static AbstractQoreNode *TIBRVCMSENDER_sendSubjectWithSyncReply(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *str = test_string_param(params, 0);
   if (!str)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   const char *subject = str->getBuffer();

   QoreHashNode *h = test_hash_param(params, 1);
   if (!h)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing data hash as second parameter to method");
      return NULL;
   }

   AbstractQoreNode *pt = get_param(params, 2);
   int64 timeout = getMsMinusOneBigInt(pt);

   // get certified delivery time limit
   pt = get_param(params, 3);
   int64 time_limit = pt ? pt->getAsBigInt() : 0;

   return cms->sendSubjectWithSyncReply(subject, h, timeout, time_limit, xsink);
}

class AbstractQoreNode *TIBRVCMSENDER_setStringEncoding(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   const QoreEncoding *enc = QEM.findCreate(pt->getBuffer());
   cms->setStringEncoding(enc);

   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_getStringEncoding(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(cms->getStringEncoding()->getCode());
}

class AbstractQoreNode *TIBRVCMSENDER_connectToRelayAgent(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   cms->connectToRelayAgent(xsink);
   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_disconnectFromRelayAgent(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   cms->disconnectFromRelayAgent(xsink);
   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_expireMessages(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *str = test_string_param(params, 0);
   if (!str)
   {
      xsink->raiseException("TIBRV-CMSENDER-EXPIRE-MESSAGES-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   const char *subj = str->getBuffer();

   AbstractQoreNode *pt = get_param(params, 1);
   int64 seqNum = pt ? pt->getAsBigInt() : 0;

   cms->expireMessages(subj, seqNum, xsink);
   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_getName(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *name = cms->getName(xsink);
   if (!xsink->isException())
      return new QoreStringNode(name);

   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_getDefaultTimeLimit(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 tl = cms->getDefaultTimeLimit(xsink);
   if (!xsink->isException())
      return new QoreBigIntNode(tl);

   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_setDefaultTimeLimit(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   class AbstractQoreNode *pt = get_param(params, 0);
   int64 tl = pt ? pt->getAsBigInt() : 0;

   cms->setDefaultTimeLimit(tl, xsink);
   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_reviewLedger(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-CMSENDER-REVIEW-LEDGER-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   const char *subj = pt->getBuffer();

   return cms->reviewLedger(subj, xsink);
}

class AbstractQoreNode *TIBRVCMSENDER_removeSendState(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-CMSENDER-REMOVE-SEND-STATE-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   const char *subj = pt->getBuffer();

   cms->removeSendState(subj, xsink);
   return NULL;
}

class AbstractQoreNode *TIBRVCMSENDER_syncLedger(class QoreObject *self, class QoreTibrvCmSender *cms, const QoreListNode *params, ExceptionSink *xsink)
{
   cms->syncLedger(xsink);
   return NULL;
}

class QoreClass *initTibrvCmSenderClass()
{
   tracein("initTibrvCmSenderClass()");

   class QoreClass *QC_TIBRVCMSENDER = new QoreClass("TibrvCmSender", QDOM_NETWORK);
   CID_TIBRVCMSENDER = QC_TIBRVCMSENDER->getID();
   QC_TIBRVCMSENDER->setConstructor(TIBRVCMSENDER_constructor);
   QC_TIBRVCMSENDER->setCopy((q_copy_t)TIBRVCMSENDER_copy);
   QC_TIBRVCMSENDER->addMethod("sendSubject",               (q_method_t)TIBRVCMSENDER_sendSubject);
   QC_TIBRVCMSENDER->addMethod("sendSubjectWithSyncReply",  (q_method_t)TIBRVCMSENDER_sendSubjectWithSyncReply);
   QC_TIBRVCMSENDER->addMethod("setStringEncoding",         (q_method_t)TIBRVCMSENDER_setStringEncoding);
   QC_TIBRVCMSENDER->addMethod("getStringEncoding",         (q_method_t)TIBRVCMSENDER_getStringEncoding);

   QC_TIBRVCMSENDER->addMethod("connectToRelayAgent",       (q_method_t)TIBRVCMSENDER_connectToRelayAgent);
   QC_TIBRVCMSENDER->addMethod("disconnectFromRelayAgent",  (q_method_t)TIBRVCMSENDER_disconnectFromRelayAgent);
   QC_TIBRVCMSENDER->addMethod("expireMssages",             (q_method_t)TIBRVCMSENDER_expireMessages);
   QC_TIBRVCMSENDER->addMethod("getName",                   (q_method_t)TIBRVCMSENDER_getName);
   QC_TIBRVCMSENDER->addMethod("getDefaultTimeLimit",       (q_method_t)TIBRVCMSENDER_getDefaultTimeLimit);
   QC_TIBRVCMSENDER->addMethod("setDefaultTimeLimit",       (q_method_t)TIBRVCMSENDER_setDefaultTimeLimit);
   QC_TIBRVCMSENDER->addMethod("reviewLedger",              (q_method_t)TIBRVCMSENDER_reviewLedger);
   QC_TIBRVCMSENDER->addMethod("removeSendState",           (q_method_t)TIBRVCMSENDER_removeSendState);
   QC_TIBRVCMSENDER->addMethod("syncLedger",                (q_method_t)TIBRVCMSENDER_syncLedger);

   traceout("initTibrvCmSenderClass()");
   return QC_TIBRVCMSENDER;
}
