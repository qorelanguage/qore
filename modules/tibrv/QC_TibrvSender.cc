/*
  modules/TIBCO/TibrvSender.cc

  TIBCO integration to QORE

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

qore_classid_t CID_TIBRVSENDER;

// syntax: [desc, service, network, daemon] 
void TIBRVSENDER_constructor(class QoreObject *self, const QoreListNode *params, class ExceptionSink *xsink)
{
   QORE_TRACE("TIBRVSENDER_constructor");

   const char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   const QoreStringNode *pt = test_string_param(params, 0);
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


}

void TIBRVSENDER_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvSender *trvs, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVSENDER-COPY-ERROR", "copying TibrvSender objects is curently not supported");
}

static AbstractQoreNode *TIBRVSENDER_sendSubject(class QoreObject *self, class QoreTibrvSender *trvs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECT-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   const char *subject = pt->getBuffer();
   const QoreHashNode *h = test_hash_param(params, 1);
   if (!h)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECT-ERROR", "missing data hash as second parameter to method");
      return NULL;
   }
   pt = test_string_param(params, 2);
   const char *replySubject = pt ? pt->getBuffer() : NULL;

   trvs->sendSubject(subject, h, replySubject, xsink);
   return NULL;
}

static AbstractQoreNode *TIBRVSENDER_sendSubjectWithSyncReply(class QoreObject *self, class QoreTibrvSender *trvs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing subject string as first parameter to method");
      return NULL;
   }
   const char *subject = pt->getBuffer();
   const QoreHashNode *h = test_hash_param(params, 1);
   if (!h)
   {
      xsink->raiseException("TIBRVSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR", "missing data hash as second parameter to method");
      return NULL;
   }
   int64 timeout = getMsMinusOneBigInt(get_param(params, 2));

   return trvs->sendSubjectWithSyncReply(subject, h, timeout, xsink);
}

class AbstractQoreNode *TIBRVSENDER_setStringEncoding(class QoreObject *self, class QoreTibrvSender *trvs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVSENDER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   const QoreEncoding *enc = QEM.findCreate(pt->getBuffer());
   trvs->setStringEncoding(enc);
   return NULL;
}

class AbstractQoreNode *TIBRVSENDER_getStringEncoding(class QoreObject *self, class QoreTibrvSender *trvs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(trvs->getStringEncoding()->getCode());
}

class QoreClass *initTibrvSenderClass()
{
   QORE_TRACE("initTibrvSenderClass()");

   class QoreClass *QC_TIBRVSENDER = new QoreClass("TibrvSender", QDOM_NETWORK);
   CID_TIBRVSENDER = QC_TIBRVSENDER->getID();
   QC_TIBRVSENDER->setConstructor(TIBRVSENDER_constructor);
   QC_TIBRVSENDER->setCopy((q_copy_t)TIBRVSENDER_copy);
   QC_TIBRVSENDER->addMethod("sendSubject",               (q_method_t)TIBRVSENDER_sendSubject);
   QC_TIBRVSENDER->addMethod("sendSubjectWithSyncReply",  (q_method_t)TIBRVSENDER_sendSubjectWithSyncReply);
   QC_TIBRVSENDER->addMethod("setStringEncoding",         (q_method_t)TIBRVSENDER_setStringEncoding);
   QC_TIBRVSENDER->addMethod("getStringEncoding",         (q_method_t)TIBRVSENDER_getStringEncoding);


   return QC_TIBRVSENDER;
}
