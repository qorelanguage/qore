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

#include "QC_TibrvListener.h"

int CID_TIBRVLISTENER;

// syntax: subject, [desc, service, network, daemon] 
void TIBRVLISTENER_constructor(class QoreObject *self, const QoreListNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVLISTENER_constructor");

   class QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "missing subject string");
      return;
   }      
   const char *subject = pt->getBuffer();   

   const char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_string_param(params, 1);
   if (pt)
      desc = pt->getBuffer();
   pt = test_string_param(params, 2);
   if (pt)
      service = pt->getBuffer();
   pt = test_string_param(params, 3);
   if (pt)
      network = pt->getBuffer();
   pt = test_string_param(params, 4);
   if (pt)
      daemon = pt->getBuffer();

   class QoreTibrvListener *qlistener = new QoreTibrvListener(subject, desc, service, network, daemon, xsink);

   if (xsink->isException())
      qlistener->deref();
   else
      self->setPrivate(CID_TIBRVLISTENER, qlistener);

   traceout("TIBRVLISTENER_constructor");
}

void TIBRVLISTENER_copy(class QoreObject *self, class QoreObject *old, class QoreTibrvListener *trvl, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVLISTENER-COPY-ERROR", "copying TibrvListener objects is curently not supported");
}

static AbstractQoreNode *TIBRVLISTENER_getQueueSize(class QoreObject *self, class QoreTibrvListener *trvl, const QoreListNode *params, ExceptionSink *xsink)
{
   int c = trvl->getQueueSize(xsink);
   if (!xsink->isException())
      return new QoreBigIntNode(c);

   return NULL;
}

static AbstractQoreNode *TIBRVLISTENER_getMessage(class QoreObject *self, class QoreTibrvListener *trvl, const QoreListNode *params, ExceptionSink *xsink)
{
   int64 timeout = getMsMinusOneBigInt(get_param(params, 0));

   // if timeout is < 0, then do not time out
   if (timeout < 0)
      return trvl->getMessage(xsink);

   return trvl->getMessage(timeout, xsink);
}

static AbstractQoreNode *TIBRVLISTENER_createInboxName(class QoreObject *self, class QoreTibrvListener *trvl, const QoreListNode *params, ExceptionSink *xsink)
{
   return trvl->createInboxName(xsink);
}

class AbstractQoreNode *TIBRVLISTENER_setStringEncoding(class QoreObject *self, class QoreTibrvListener *trvl, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pt = test_string_param(params, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVLISTENER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   const QoreEncoding *enc = QEM.findCreate(pt->getBuffer());
   trvl->setStringEncoding(enc);
   return NULL;
}

class AbstractQoreNode *TIBRVLISTENER_getStringEncoding(class QoreObject *self, class QoreTibrvListener *trvl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(trvl->getStringEncoding()->getCode());
}

class QoreClass *initTibrvListenerClass()
{
   tracein("initTibrvListenerClass()");

   class QoreClass *QC_TIBRVLISTENER = new QoreClass("TibrvListener", QDOM_NETWORK);
   CID_TIBRVLISTENER = QC_TIBRVLISTENER->getID();
   QC_TIBRVLISTENER->setConstructor(TIBRVLISTENER_constructor);
   QC_TIBRVLISTENER->setCopy((q_copy_t)TIBRVLISTENER_copy);
   QC_TIBRVLISTENER->addMethod("getQueueSize",       (q_method_t)TIBRVLISTENER_getQueueSize);
   QC_TIBRVLISTENER->addMethod("getMessage",         (q_method_t)TIBRVLISTENER_getMessage);
   QC_TIBRVLISTENER->addMethod("createInboxName",    (q_method_t)TIBRVLISTENER_createInboxName);
   QC_TIBRVLISTENER->addMethod("setStringEncoding",  (q_method_t)TIBRVLISTENER_setStringEncoding);
   QC_TIBRVLISTENER->addMethod("getStringEncoding",  (q_method_t)TIBRVLISTENER_getStringEncoding);

   traceout("initTibrvListenerClass()");
   return QC_TIBRVLISTENER;
}
