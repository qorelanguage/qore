/*
  modules/TIBCO/TibrvListener.cc

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

#include "QC_TibrvCmListener.h"

int CID_TIBRVCMLISTENER;

static inline void *getTRVL(void *obj)
{
   ((QoreTibrvCmListener *)obj)->ROreference();
   return obj;
}

// syntax: subject, [cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon] 
class QoreNode *TIBRVCMLISTENER_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVCMLISTENER_constructor");

   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMLISTENER-CONSTRUCTOR-ERROR", "missing subject string");
      return NULL;
   }      
   char *subject = pt->val.String->getBuffer();   

   char *cmName = NULL, *ledgerName = NULL, *relayAgent = NULL;
   bool requestOld, syncLedger;

   pt = test_param(params, NT_STRING, 1);
   if (pt)
      cmName = pt->val.String->getBuffer();

   pt = get_param(params, 2);
   requestOld = pt ? pt->getAsBool() : false;

   pt = test_param(params, NT_STRING, 3);
   if (pt)
      ledgerName = pt->val.String->getBuffer();

   pt = get_param(params, 4);
   syncLedger = pt ? pt->getAsBool() : false;

   pt = test_param(params, NT_STRING, 5);
   if (pt)
      relayAgent = pt->val.String->getBuffer();

   char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_param(params, NT_STRING, 6);
   if (pt)
      desc = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 7);
   if (pt)
      service = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 8);
   if (pt)
      network = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 9);
   if (pt)
      daemon = pt->val.String->getBuffer();

   class QoreTibrvCmListener *qcmlistener = new QoreTibrvCmListener(subject, cmName, requestOld, ledgerName, syncLedger, relayAgent, desc, service, network, daemon, xsink);

   if (xsink->isException() || self->setPrivate(CID_TIBRVCMLISTENER, qcmlistener, getTRVL))
      qcmlistener->deref();

   traceout("TIBRVCMLISTENER_constructor");
   return NULL;
}

class QoreNode *TIBRVCMLISTENER_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVCMLISTENER_destructor()");
   // set adapter paramter
   QoreTibrvCmListener *trvl = (QoreTibrvCmListener *)self->getAndClearPrivateData(CID_TIBRVCMLISTENER);
   if (trvl)
      trvl->deref();
   traceout("TIBRVCMLISTENER_destructor()");
   return NULL;
}

static QoreNode *TIBRVCMLISTENER_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVCMLISTENER-COPY-ERROR", "copying TibrvCmListener objects is curently not supported");
   return NULL;
}

static QoreNode *TIBRVCMLISTENER_getQueueSize(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmListener *trvl = (QoreTibrvCmListener *)self->getReferencedPrivateData(CID_TIBRVCMLISTENER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      int c = trvl->getQueueSize(xsink);
      if (!xsink->isException())
	 rv = new QoreNode((int64)c);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmListener::getQueueSize");

   return rv;
}

static QoreNode *TIBRVCMLISTENER_getMessage(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmListener *trvl = (QoreTibrvCmListener *)self->getReferencedPrivateData(CID_TIBRVCMLISTENER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      class QoreNode *p0 = get_param(params, 0);
      int64 timeout = is_nothing(p0) ? -1LL : p0->getAsBigInt();

      class Hash *h = trvl->getMessage(timeout, xsink);
      if (h)
	 rv = new QoreNode(h);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmListener::getMessage");

   return rv;
}

static QoreNode *TIBRVCMLISTENER_createInboxName(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmListener *trvl = (QoreTibrvCmListener *)self->getReferencedPrivateData(CID_TIBRVCMLISTENER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      class QoreString *str = trvl->createInboxName(xsink);
      if (str)
	 rv = new QoreNode(str);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmListener::createInboxName");

   return rv;
}

class QoreNode *TIBRVCMLISTENER_setStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVCMLISTENER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
      return NULL;
   }

   QoreTibrvCmListener *trvl = (QoreTibrvCmListener *)self->getReferencedPrivateData(CID_TIBRVCMLISTENER);

   if (trvl)
   {
      class QoreEncoding *enc = QEM.findCreate(pt->val.String->getBuffer());

      trvl->setStringEncoding(enc);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmListener::setStringEncoding");

   return NULL;
}

class QoreNode *TIBRVCMLISTENER_getStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmListener *trvl = (QoreTibrvCmListener *)self->getReferencedPrivateData(CID_TIBRVCMLISTENER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      class QoreEncoding *enc = trvl->getStringEncoding();
      trvl->deref();
      rv = new QoreNode(enc->code);
   }
   else
      alreadyDeleted(xsink, "TibrvCmListener::getStringEncoding");

   return rv;
}

class QoreNode *TIBRVCMLISTENER_syncLedger(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvCmListener *trvl = (QoreTibrvCmListener *)self->getReferencedPrivateData(CID_TIBRVCMLISTENER);

   if (trvl)
   {
      trvl->syncLedger(xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvCmListener::syncLedger");

   return NULL;
}

class QoreClass *initTibrvCmListenerClass()
{
   tracein("initTibrvCmListenerClass()");

   class QoreClass *QC_TIBRVCMLISTENER = new QoreClass(strdup("TibrvCmListener"));
   CID_TIBRVCMLISTENER = QC_TIBRVCMLISTENER->getID();
   QC_TIBRVCMLISTENER->addMethod("constructor",        TIBRVCMLISTENER_constructor);
   QC_TIBRVCMLISTENER->addMethod("destructor",         TIBRVCMLISTENER_destructor);
   QC_TIBRVCMLISTENER->addMethod("copy",               TIBRVCMLISTENER_copy);
   QC_TIBRVCMLISTENER->addMethod("getQueueSize",       TIBRVCMLISTENER_getQueueSize);
   QC_TIBRVCMLISTENER->addMethod("getMessage",         TIBRVCMLISTENER_getMessage);
   QC_TIBRVCMLISTENER->addMethod("createInboxName",    TIBRVCMLISTENER_createInboxName);
   QC_TIBRVCMLISTENER->addMethod("setStringEncoding",  TIBRVCMLISTENER_setStringEncoding);
   QC_TIBRVCMLISTENER->addMethod("getStringEncoding",  TIBRVCMLISTENER_getStringEncoding);
   QC_TIBRVCMLISTENER->addMethod("syncLedger",         TIBRVCMLISTENER_syncLedger);

   traceout("initTibrvCmListenerClass()");
   return QC_TIBRVCMLISTENER;
}
