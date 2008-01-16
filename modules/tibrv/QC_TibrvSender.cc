/*
  modules/TIBCO/TibrvSender.cc

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

#include "QC_TibrvSender.h"

int CID_TIBRVSENDER;

// syntax: [desc, service, network, daemon] 
void TIBRVSENDER_constructor(class QoreObject *self, const QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVSENDER_constructor");

   const char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   QoreStringNode *pt = test_string_param(params, 0);
   if (pt)
      desc = pt->getBuffer();
   pt = test_string_param(params, 1);
   if (pt)
      service = pt->getBuffer();
   pt = test_string_param(params, 2);
   if (pt)
      network = pt->getBuffer();
   pt = test_string_param(params, 3);
   if (pt)
      daemon = pt->getBuffer();

   class QoreTibrvSender *qsender = new QoreTibrvSender(desc, service, network, daemon, xsink);

   if (xsink->isException())
      qsender->deref();
   else
      self->setPrivate(CID_TIBRVSENDER, qsender);

   traceout("TIBRVSENDER_constructor");
}

void TIBRVSENDER_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvSender *trvs, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVSENDER-COPY-ERROR", "copying TibrvSender objects is curently not supported");
}

static QoreNode *TIBRVSENDER_sendSubject(class QoreObject *self, class QoreTibrvSender *trvs, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECT-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   const char *subject = pt->getBuffer();
   QoreNode *ph = test_param(params, NT_HASH, 1);
   if (!ph)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECT-ERROR", "missing data hash as second parameter to method");
      return NULL;
   }
   class QoreHash *h = ph->val.hash;
   pt = test_string_param(params, 2);
   const char *replySubject = pt ? pt->getBuffer() : NULL;

   trvs->sendSubject(subject, h, replySubject, xsink);
   return NULL;
}

static QoreNode *TIBRVSENDER_sendSubjectWithSyncReply(class QoreObject *self, class QoreTibrvSender *trvs, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   const char *subject = pt->getBuffer();
   QoreNode *ph = test_param(params, NT_HASH, 1);
   if (!ph)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing data hash as second parameter to method");
      return NULL;
   }
   class QoreHash *h = ph->val.hash;
   int64 timeout = getMsMinusOneBigInt(get_param(params, 2));

   h = trvs->sendSubjectWithSyncReply(subject, h, timeout, xsink);
   if (h)
      return new QoreNode(h);
   return NULL;
}

class QoreNode *TIBRVSENDER_setStringEncoding(class QoreObject *self, class QoreTibrvSender *trvs, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVSENDER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   const QoreEncoding *enc = QEM.findCreate(pt->getBuffer());
   trvs->setStringEncoding(enc);
   return NULL;
}

class QoreNode *TIBRVSENDER_getStringEncoding(class QoreObject *self, class QoreTibrvSender *trvs, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(trvs->getStringEncoding()->getCode());
}

class QoreClass *initTibrvSenderClass()
{
   tracein("initTibrvSenderClass()");

   class QoreClass *QC_TIBRVSENDER = new QoreClass("TibrvSender", QDOM_NETWORK);
   CID_TIBRVSENDER = QC_TIBRVSENDER->getID();
   QC_TIBRVSENDER->setConstructor(TIBRVSENDER_constructor);
   QC_TIBRVSENDER->setCopy((q_copy_t)TIBRVSENDER_copy);
   QC_TIBRVSENDER->addMethod("sendSubject",               (q_method_t)TIBRVSENDER_sendSubject);
   QC_TIBRVSENDER->addMethod("sendSubjectWithSyncReply",  (q_method_t)TIBRVSENDER_sendSubjectWithSyncReply);
   QC_TIBRVSENDER->addMethod("setStringEncoding",         (q_method_t)TIBRVSENDER_setStringEncoding);
   QC_TIBRVSENDER->addMethod("getStringEncoding",         (q_method_t)TIBRVSENDER_getStringEncoding);

   traceout("initTibrvSenderClass()");
   return QC_TIBRVSENDER;
}
