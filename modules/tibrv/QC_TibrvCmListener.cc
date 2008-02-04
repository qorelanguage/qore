/*
  modules/TIBCO/TibrvListener.cc

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

#include "QC_TibrvCmListener.h"

int CID_TIBRVCMLISTENER;

// syntax: subject, [cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon] 
void TIBRVCMLISTENER_constructor(class QoreObject *self, const QoreListNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVCMLISTENER_constructor");

   const QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMLISTENER-CONSTRUCTOR-ERROR", "missing subject string");
      return;
   }      
   const char *subject = pt->getBuffer();   

   const char *cmName = NULL, *ledgerName = NULL, *relayAgent = NULL;
   bool requestOld, syncLedger;

   pt = test_string_param(params, 1);
   if (pt)
      cmName = pt->getBuffer();

   const AbstractQoreNode *pn = get_param(params, 2);
   requestOld = pn ? pn->getAsBool() : false;

   pt = test_string_param(params, 3);
   if (pt)
      ledgerName = pt->getBuffer();

   pn = get_param(params, 4);
   syncLedger = pn ? pn->getAsBool() : false;

   pt = test_string_param(params, 5);
   if (pt)
      relayAgent = pt->getBuffer();

   const char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_string_param(params, 6);
   if (pt)
      desc = pt->getBuffer();
   pt = test_string_param(params, 7);
   if (pt)
      service = pt->getBuffer();
   pt = test_string_param(params, 8);
   if (pt)
      network = pt->getBuffer();
   pt = test_string_param(params, 9);
   if (pt)
      daemon = pt->getBuffer();

   class QoreTibrvCmListener *qcmlistener = new QoreTibrvCmListener(subject, cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon, xsink);

   if (xsink->isException())
      qcmlistener->deref();
   else
      self->setPrivate(CID_TIBRVCMLISTENER, qcmlistener);

   traceout("TIBRVCMLISTENER_constructor");
}

static void TIBRVCMLISTENER_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvCmListener *cml, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVCMLISTENER-COPY-ERROR", "copying TibrvCmListener objects is curently not supported");
}

static AbstractQoreNode *TIBRVCMLISTENER_getQueueSize(class QoreObject *self, class QoreTibrvCmListener *cml, const QoreListNode *params, ExceptionSink *xsink)
{
   int c = cml->getQueueSize(xsink);
   if (!xsink->isException())
      return new QoreBigIntNode(c);

   return NULL;
}

static AbstractQoreNode *TIBRVCMLISTENER_getMessage(class QoreObject *self, class QoreTibrvCmListener *cml, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 timeout = getMsMinusOneBigInt(get_param(params, 0));

   // do not time out and guarantee to return data (or an error) if timeout is negative
   if (timeout < 0)
      return cml->getMessage(xsink);

   return cml->getMessage(timeout, xsink);
}

static AbstractQoreNode *TIBRVCMLISTENER_createInboxName(class QoreObject *self, class QoreTibrvCmListener *cml, const QoreListNode *params, ExceptionSink *xsink)
{
   return cml->createInboxName(xsink);
}

class AbstractQoreNode *TIBRVCMLISTENER_setStringEncoding(class QoreObject *self, class QoreTibrvCmListener *cml, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMLISTENER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   const QoreEncoding *enc = QEM.findCreate(pt->getBuffer());
   cml->setStringEncoding(enc);

   return NULL;
}

class AbstractQoreNode *TIBRVCMLISTENER_getStringEncoding(class QoreObject *self, class QoreTibrvCmListener *cml, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(cml->getStringEncoding()->getCode());
}

class AbstractQoreNode *TIBRVCMLISTENER_syncLedger(class QoreObject *self, class QoreTibrvCmListener *cml, const QoreListNode *params, ExceptionSink *xsink)
{
   cml->syncLedger(xsink);
   return NULL;
}

static AbstractQoreNode *TIBRVCMLISTENER_getName(class QoreObject *self, class QoreTibrvCmListener *cml, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *name = cml->getName(xsink);
   if (!xsink->isException())
      return new QoreStringNode(name);

   return NULL;
}

class QoreClass *initTibrvCmListenerClass()
{
   tracein("initTibrvCmListenerClass()");

   class QoreClass *QC_TIBRVCMLISTENER = new QoreClass("TibrvCmListener", QDOM_NETWORK);
   CID_TIBRVCMLISTENER = QC_TIBRVCMLISTENER->getID();
   QC_TIBRVCMLISTENER->setConstructor(TIBRVCMLISTENER_constructor);
   QC_TIBRVCMLISTENER->setCopy((q_copy_t)TIBRVCMLISTENER_copy);
   QC_TIBRVCMLISTENER->addMethod("getQueueSize",       (q_method_t)TIBRVCMLISTENER_getQueueSize);
   QC_TIBRVCMLISTENER->addMethod("getMessage",         (q_method_t)TIBRVCMLISTENER_getMessage);
   QC_TIBRVCMLISTENER->addMethod("createInboxName",    (q_method_t)TIBRVCMLISTENER_createInboxName);
   QC_TIBRVCMLISTENER->addMethod("setStringEncoding",  (q_method_t)TIBRVCMLISTENER_setStringEncoding);
   QC_TIBRVCMLISTENER->addMethod("getStringEncoding",  (q_method_t)TIBRVCMLISTENER_getStringEncoding);
   QC_TIBRVCMLISTENER->addMethod("syncLedger",         (q_method_t)TIBRVCMLISTENER_syncLedger);
   QC_TIBRVCMLISTENER->addMethod("getName",            (q_method_t)TIBRVCMLISTENER_getName);

   traceout("initTibrvCmListenerClass()");
   return QC_TIBRVCMLISTENER;
}
