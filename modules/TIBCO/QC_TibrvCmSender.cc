/*
  modules/TIBCO/TibrvCmSender.cc

  TIBCO integration to QORE

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
#include <qore/params.h>
#include <qore/charset.h>

#include "QC_TibrvCmSender.h"

int CID_TIBRVCMSENDER;

static inline void *getTRVS(void *obj)
{
   ((QoreTibrvCmSender *)obj)->ROreference();
   return obj;
}

// syntax: [cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon] 
class QoreNode *TIBRVCMSENDER_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
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

   if (xsink->isException() || self->setPrivate(CID_TIBRVCMSENDER, qcmsender, getTRVS))
      qcmsender->deref();

   traceout("TIBRVCMSENDER_constructor");
   return NULL;
}

class QoreNode *TIBRVCMSENDER_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVCMSENDER_destructor()");
   // set adapter paramter
   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getAndClearPrivateData(CID_TIBRVCMSENDER);
   if (trvl)
      trvl->deref();
   traceout("TIBRVCMSENDER_destructor()");
   return NULL;
}

static QoreNode *TIBRVCMSENDER_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVCMSENDER-COPY-ERROR", "copying TibrvCmSender objects is curently not supported");
   return NULL;
}

static QoreNode *TIBRVCMSENDER_sendSubject(class Object *self, QoreNode *params, ExceptionSink *xsink)
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
   pt = get_param(params, 3);
   int64 time_limit = pt ? pt->getAsBigInt() : 0;

   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      trvl->sendSubject(subject, h, replySubject, time_limit, xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::sendSubject");

   return NULL;
}

static QoreNode *TIBRVCMSENDER_sendSubjectWithSyncReply(class Object *self, QoreNode *params, ExceptionSink *xsink)
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
   int64 timeout = pt ? pt->getAsBigInt() : -1;

   // get certified delivery time limit
   pt = get_param(params, 3);
   int64 time_limit = pt ? pt->getAsBigInt() : 0;

   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      h = trvl->sendSubjectWithSyncReply(subject, h, timeout, time_limit, xsink);
      if (h)
	 rv = new QoreNode(h);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::sendSubjectWithSyncReply");

   return rv;
}

class QoreNode *TIBRVCMSENDER_setStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMSENDER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      class QoreEncoding *enc = QEM.findCreate(pt->val.String->getBuffer());

      trvl->setStringEncoding(enc);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::setStringEncoding");

   return NULL;
}

class QoreNode *TIBRVCMSENDER_getStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      class QoreEncoding *enc = trvl->getStringEncoding();
      trvl->deref();
      rv = new QoreNode(enc->code);
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::getStringEncoding");

   return rv;
}

class QoreNode *TIBRVCMSENDER_connectToRelayAgent(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      trvl->connectToRelayAgent(xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::connectToRelayAgent");

   return NULL;
}

class QoreNode *TIBRVCMSENDER_disconnectFromRelayAgent(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      trvl->disconnectFromRelayAgent(xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::disconnectFromRelayAgent");

   return NULL;
}

class QoreNode *TIBRVCMSENDER_expireMessages(class Object *self, QoreNode *params, ExceptionSink *xsink)
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

   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      trvl->expireMessages(subj, seqNum, xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::expireMessages");

   return NULL;
}

class QoreNode *TIBRVCMSENDER_getName(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      char *name = trvl->getName(xsink);
      if (!xsink->isException())
	 rv = new QoreNode(name);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::getName");

   return rv;
}

class QoreNode *TIBRVCMSENDER_getDefaultTimeLimit(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      int64 tl = trvl->getDefaultTimeLimit(xsink);
      if (!xsink->isException())
	 rv = new QoreNode(tl);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::getDefaultTimeLimit");

   return rv;
}

class QoreNode *TIBRVCMSENDER_setDefaultTimeLimit(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = get_param(params, 0);
   int64 tl = pt ? pt->getAsBigInt() : 0;

   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      trvl->setDefaultTimeLimit(tl, xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::setDefaultTimeLimit");

   return NULL;
}

class QoreNode *TIBRVCMSENDER_reviewLedger(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-CMSENDER-REVIEW-LEDGER-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   char *subj = pt->val.String->getBuffer();

   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      rv = trvl->reviewLedger(subj, xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::reviewLedger");

   return rv;
}

class QoreNode *TIBRVCMSENDER_removeSendState(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRV-CMSENDER-REMOVE-SEND-STATE-ERROR", "missing subject name as first parameter to method");
      return NULL;
   }
   char *subj = pt->val.String->getBuffer();

   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      trvl->removeSendState(subj, xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::removeSendState");

   return NULL;
}

class QoreNode *TIBRVCMSENDER_syncLedger(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmSender *trvl = (QoreTibrvCmSender *)self->getReferencedPrivateData(CID_TIBRVCMSENDER);

   if (trvl)
   {
      trvl->syncLedger(xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmSender::syncLedger");

   return NULL;
}

class QoreClass *initTibrvCmSenderClass()
{
   tracein("initTibrvCmSenderClass()");

   class QoreClass *QC_TIBRVCMSENDER = new QoreClass(strdup("TibrvCmSender"));
   CID_TIBRVCMSENDER = QC_TIBRVCMSENDER->getID();
   QC_TIBRVCMSENDER->addMethod("constructor",               TIBRVCMSENDER_constructor);
   QC_TIBRVCMSENDER->addMethod("destructor",                TIBRVCMSENDER_destructor);
   QC_TIBRVCMSENDER->addMethod("copy",                      TIBRVCMSENDER_copy);
   QC_TIBRVCMSENDER->addMethod("sendSubject",               TIBRVCMSENDER_sendSubject);
   QC_TIBRVCMSENDER->addMethod("sendSubjectWithSyncReply",  TIBRVCMSENDER_sendSubjectWithSyncReply);
   QC_TIBRVCMSENDER->addMethod("setStringEncoding",         TIBRVCMSENDER_setStringEncoding);
   QC_TIBRVCMSENDER->addMethod("getStringEncoding",         TIBRVCMSENDER_getStringEncoding);

   QC_TIBRVCMSENDER->addMethod("connectToRelayAgent",       TIBRVCMSENDER_connectToRelayAgent);
   QC_TIBRVCMSENDER->addMethod("disconnectFromRelayAgent",  TIBRVCMSENDER_disconnectFromRelayAgent);
   QC_TIBRVCMSENDER->addMethod("expireMssages",             TIBRVCMSENDER_expireMessages);
   QC_TIBRVCMSENDER->addMethod("getName",                   TIBRVCMSENDER_getName);
   QC_TIBRVCMSENDER->addMethod("getDefaultTimeLimit",       TIBRVCMSENDER_getDefaultTimeLimit);
   QC_TIBRVCMSENDER->addMethod("setDefaultTimeLimit",       TIBRVCMSENDER_setDefaultTimeLimit);
   QC_TIBRVCMSENDER->addMethod("reviewLedger",              TIBRVCMSENDER_reviewLedger);
   QC_TIBRVCMSENDER->addMethod("removeSendState",           TIBRVCMSENDER_removeSendState);
   QC_TIBRVCMSENDER->addMethod("syncLedger",                TIBRVCMSENDER_syncLedger);

   traceout("initTibrvCmSenderClass()");
   return QC_TIBRVCMSENDER;
}
