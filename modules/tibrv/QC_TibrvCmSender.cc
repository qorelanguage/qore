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
void TIBRVCMSENDER_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVCMSENDER_constructor");

   char *cmName = NULL, *ledgerName = NULL, *relayAgent = NULL;
   bool requestOld, syncLedger;

   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (pt)
      cmName = pt->val.String->getBuffer();

   pt = get_param(params, 1);
   requestOld = pt ? pt->getAsBool() : false;

   pt = test_param(params, NT_STRING, 2);
   if (pt)
      ledgerName = pt->val.String->getBuffer();

   pt = get_param(params, 3);
   syncLedger = pt ? pt->getAsBool() : false;

   pt = test_param(params, NT_STRING, 4);
   if (pt)
      relayAgent = pt->val.String->getBuffer();

   char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_param(params, NT_STRING, 5);
   if (pt)
      desc = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 6);
   if (pt)
      service = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 7);
   if (pt)
      network = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 8);
   if (pt)
      daemon = pt->val.String->getBuffer();

   class QoreTibrvCmSender *qcmsender = new QoreTibrvCmSender(cmName, requestOld, ledgerName, syncLedger,relayAgent, desc, service, network, daemon, xsink);

   if (xsink->isException())
      qcmsender->deref();
   else
      self->setPrivate(CID_TIBRVCMSENDER, qcmsender);

   traceout("TIBRVCMSENDER_constructor");
}

void TIBRVCMSENDER_copy(class Object *self, class Object *old, class QoreTibrvCmSender *cms, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVCMSENDER-COPY-ERROR", "copying TibrvCmSender objects is curently not supported");
}

static QoreNode *TIBRVCMSENDER_sendSubject(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECT-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   char *subject = pt->val.String->getBuffer();
   pt = test_param(params, NT_HASH, 1);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECT-ERROR", "missing data hash as second parameter to method");
      return NULL;
   }
   class Hash *h = pt->val.hash;
   pt = test_param(params, NT_STRING, 2);
   char *replySubject = pt ? pt->val.String->getBuffer() : NULL;

   // get certified delivery time limit
   int64 time_limit = getMsZeroBigInt(get_param(params, 3));

   cms->sendSubject(subject, h, replySubject, time_limit, xsink);

   return NULL;
}

static QoreNode *TIBRVCMSENDER_sendSubjectWithSyncReply(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   char *subject = pt->val.String->getBuffer();
   pt = test_param(params, NT_HASH, 1);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing data hash as second parameter to method");
      return NULL;
   }
   class Hash *h = pt->val.hash;
   pt = get_param(params, 2);
   int64 timeout = getMsMinusOneBigInt(pt);

   // get certified delivery time limit
   pt = get_param(params, 3);
   int64 time_limit = pt ? pt->getAsBigInt() : 0;

   h = cms->sendSubjectWithSyncReply(subject, h, timeout, time_limit, xsink);
   if (h)
      return new QoreNode(h);

   return NULL;
}

class QoreNode *TIBRVCMSENDER_setStringEncoding(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   class QoreEncoding *enc = QEM.findCreate(pt->val.String->getBuffer());
   cms->setStringEncoding(enc);

   return NULL;
}

class QoreNode *TIBRVCMSENDER_getStringEncoding(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(cms->getStringEncoding()->getCode());
}

class QoreNode *TIBRVCMSENDER_connectToRelayAgent(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   cms->connectToRelayAgent(xsink);
   return NULL;
}

class QoreNode *TIBRVCMSENDER_disconnectFromRelayAgent(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   cms->disconnectFromRelayAgent(xsink);
   return NULL;
}

class QoreNode *TIBRVCMSENDER_expireMessages(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-CMSENDER-EXPIRE-MESSAGES-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   char *subj = pt->val.String->getBuffer();

   pt = get_param(params, 1);
   int64 seqNum = pt ? pt->getAsBigInt() : 0;

   cms->expireMessages(subj, seqNum, xsink);
   return NULL;
}

class QoreNode *TIBRVCMSENDER_getName(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   char *name = cms->getName(xsink);
   if (!xsink->isException())
      return new QoreNode(name);

   return NULL;
}

class QoreNode *TIBRVCMSENDER_getDefaultTimeLimit(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   int64 tl = cms->getDefaultTimeLimit(xsink);
   if (!xsink->isException())
      return new QoreNode(tl);

   return NULL;
}

class QoreNode *TIBRVCMSENDER_setDefaultTimeLimit(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = get_param(params, 0);
   int64 tl = pt ? pt->getAsBigInt() : 0;

   cms->setDefaultTimeLimit(tl, xsink);
   return NULL;
}

class QoreNode *TIBRVCMSENDER_reviewLedger(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-CMSENDER-REVIEW-LEDGER-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   char *subj = pt->val.String->getBuffer();

   return cms->reviewLedger(subj, xsink);
}

class QoreNode *TIBRVCMSENDER_removeSendState(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-CMSENDER-REMOVE-SEND-STATE-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   char *subj = pt->val.String->getBuffer();

   cms->removeSendState(subj, xsink);
   return NULL;
}

class QoreNode *TIBRVCMSENDER_syncLedger(class Object *self, class QoreTibrvCmSender *cms, QoreNode *params, ExceptionSink *xsink)
{
   cms->syncLedger(xsink);
   return NULL;
}

class QoreClass *initTibrvCmSenderClass()
{
   tracein("initTibrvCmSenderClass()");

   class QoreClass *QC_TIBRVCMSENDER = new QoreClass(QDOM_NETWORK, strdup("TibrvCmSender"));
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
