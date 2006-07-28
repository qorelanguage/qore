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

#include "QC_TibrvListener.h"

int CID_TIBRVLISTENER;

static inline void *getTRVL(void *obj)
{
   ((QoreTibrvListener *)obj)->ROreference();
   return obj;
}

// syntax: subject, [desc, service, network, daemon] 
class QoreNode *TIBRVLISTENER_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVLISTENER_constructor");

   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (!pt)
   {
      xsink->raiseException("TIBRVLISTENER-CONSTRUCTOR-ERROR", "missing subject string");
      return NULL;
   }      
   char *subject = pt->val.String->getBuffer();   

   char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   pt = test_param(params, NT_STRING, 1);
   if (pt)
      desc = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 2);
   if (pt)
      service = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 3);
   if (pt)
      network = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 4);
   if (pt)
      daemon = pt->val.String->getBuffer();

   class QoreTibrvListener *qlistener = new QoreTibrvListener(subject, desc, service, network, daemon, xsink);

   if (xsink->isException() || self->setPrivate(CID_TIBRVLISTENER, qlistener, getTRVL))
      qlistener->deref();

   traceout("TIBRVLISTENER_constructor");
   return NULL;
}

class QoreNode *TIBRVLISTENER_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVLISTENER_destructor()");
   // set adapter paramter
   QoreTibrvListener *trvl = (QoreTibrvListener *)self->getAndClearPrivateData(CID_TIBRVLISTENER);
   if (trvl)
      trvl->deref();
   traceout("TIBRVLISTENER_destructor()");
   return NULL;
}

static QoreNode *TIBRVLISTENER_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVLISTENER-COPY-ERROR", "copying TibrvListener objects is curently not supported");
   return NULL;
}

static QoreNode *TIBRVLISTENER_getQueueSize(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvListener *trvl = (QoreTibrvListener *)self->getReferencedPrivateData(CID_TIBRVLISTENER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      int c = trvl->getQueueSize(xsink);
      if (!xsink->isException())
	 rv = new QoreNode((int64)c);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvListener::getQueueSize");

   return rv;
}

static QoreNode *TIBRVLISTENER_getMessage(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvListener *trvl = (QoreTibrvListener *)self->getReferencedPrivateData(CID_TIBRVLISTENER);
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
      alreadyDeleted(xsink, "TibrvListener::getMessage");

   return rv;
}

static QoreNode *TIBRVLISTENER_createInboxName(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvListener *trvl = (QoreTibrvListener *)self->getReferencedPrivateData(CID_TIBRVLISTENER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      class QoreString *str = trvl->createInboxName(xsink);
      if (str)
	 rv = new QoreNode(str);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvListener::createInboxName");

   return rv;
}

class QoreNode *TIBRVLISTENER_setStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvListener *trvl = (QoreTibrvListener *)self->getReferencedPrivateData(CID_TIBRVLISTENER);

   if (trvl)
   {
      class QoreNode *pt = test_param(params, NT_STRING, 0);
      if (!pt)
      {
	 xsink->raiseException("TIBRVLISTENER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
	 return NULL;
      }
      class QoreEncoding *enc = QEM.findCreate(pt->val.String->getBuffer());

      trvl->setStringEncoding(enc);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvListener::setStringEncoding");

   return NULL;
}

class QoreNode *TIBRVLISTENER_getStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvListener *trvl = (QoreTibrvListener *)self->getReferencedPrivateData(CID_TIBRVLISTENER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      class QoreEncoding *enc = trvl->getStringEncoding();
      trvl->deref();
      rv = new QoreNode(enc->code);
   }
   else
      alreadyDeleted(xsink, "TibrvListener::getStringEncoding");

   return rv;
}

class QoreClass *initTibrvListenerClass()
{
   tracein("initTibrvListenerClass()");

   class QoreClass *QC_TIBRVLISTENER = new QoreClass(strdup("TibrvListener"));
   CID_TIBRVLISTENER = QC_TIBRVLISTENER->getID();
   QC_TIBRVLISTENER->addMethod("constructor",        TIBRVLISTENER_constructor);
   QC_TIBRVLISTENER->addMethod("destructor",         TIBRVLISTENER_destructor);
   QC_TIBRVLISTENER->addMethod("copy",               TIBRVLISTENER_copy);
   QC_TIBRVLISTENER->addMethod("getQueueSize",       TIBRVLISTENER_getQueueSize);
   QC_TIBRVLISTENER->addMethod("getMessage",         TIBRVLISTENER_getMessage);
   QC_TIBRVLISTENER->addMethod("createInboxName",    TIBRVLISTENER_createInboxName);
   QC_TIBRVLISTENER->addMethod("setStringEncoding",  TIBRVLISTENER_setStringEncoding);
   QC_TIBRVLISTENER->addMethod("getStringEncoding",  TIBRVLISTENER_getStringEncoding);

   traceout("initTibrvListenerClass()");
   return QC_TIBRVLISTENER;
}
