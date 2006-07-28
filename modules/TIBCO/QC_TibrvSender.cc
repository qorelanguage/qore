/*
  modules/TIBCO/TibrvSender.cc

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

#include "QC_TibrvSender.h"

int CID_TIBRVSENDER;

static inline void *getTRVS(void *obj)
{
   ((QoreTibrvSender *)obj)->ROreference();
   return obj;
}

// syntax: [desc, service, network, daemon] 
class QoreNode *TIBRVSENDER_constructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVSENDER_constructor");

   char *service = NULL, *network = NULL, *daemon = NULL, *desc = NULL;
   class QoreNode *pt = test_param(params, NT_STRING, 0);
   if (pt)
      desc = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 1);
   if (pt)
      service = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 2);
   if (pt)
      network = pt->val.String->getBuffer();
   pt = test_param(params, NT_STRING, 3);
   if (pt)
      daemon = pt->val.String->getBuffer();

   class QoreTibrvSender *qsender = new QoreTibrvSender(desc, service, network, daemon, xsink);

   if (xsink->isException() || self->setPrivate(CID_TIBRVSENDER, qsender, getTRVS))
      qsender->deref();

   traceout("TIBRVSENDER_constructor");
   return NULL;
}

class QoreNode *TIBRVSENDER_destructor(class Object *self, class QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBRVSENDER_destructor()");
   // set adapter paramter
   QoreTibrvSender *trvl = (QoreTibrvSender *)self->getAndClearPrivateData(CID_TIBRVSENDER);
   if (trvl)
      trvl->deref();
   traceout("TIBRVSENDER_destructor()");
   return NULL;
}

static QoreNode *TIBRVSENDER_copy(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("TIBRVSENDER-COPY-ERROR", "copying TibrvSender objects is curently not supported");
   return NULL;
}

static QoreNode *TIBRVSENDER_sendSubject(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvSender *trvl = (QoreTibrvSender *)self->getReferencedPrivateData(CID_TIBRVSENDER);

   if (trvl)
   {
      class QoreNode *pt = test_param(params, NT_STRING, 0);
      if (!pt)
      {
	 xsink->raiseException("TIBRVSENDER-SENDSUBJECT-ERROR", "missing subject string as first parameter to method");
	 return NULL;
      }
      char *subject = pt->val.String->getBuffer();
      pt = test_param(params, NT_HASH, 1);
      if (!pt)
      {
	 xsink->raiseException("TIBRVSENDER-SENDSUBJECT-ERROR", "missing data hash as second parameter to method");
	 return NULL;
      }

      trvl->sendSubject(subject, pt->val.hash, xsink);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvSender::sendSubject");

   return NULL;
}

class QoreNode *TIBRVSENDER_setStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvSender *trvl = (QoreTibrvSender *)self->getReferencedPrivateData(CID_TIBRVSENDER);

   if (trvl)
   {
      class QoreNode *pt = test_param(params, NT_STRING, 0);
      if (!pt)
      {
	 xsink->raiseException("TIBRVSENDER-SETSTRINGENCODING-ERROR", "missing string encoding as first parameter to method");
	 return NULL;
      }
      class QoreEncoding *enc = QEM.findCreate(pt->val.String->getBuffer());

      trvl->setStringEncoding(enc);
      trvl->deref();
   }
   else
      alreadyDeleted(xsink, "TibrvSender::setStringEncoding");

   return NULL;
}

class QoreNode *TIBRVSENDER_getStringEncoding(class Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreTibrvSender *trvl = (QoreTibrvSender *)self->getReferencedPrivateData(CID_TIBRVSENDER);
   class QoreNode *rv = NULL;

   if (trvl)
   {
      class QoreEncoding *enc = trvl->getStringEncoding();
      trvl->deref();
      rv = new QoreNode(enc->code);
   }
   else
      alreadyDeleted(xsink, "TibrvSender::getStringEncoding");

   return rv;
}

class QoreClass *initTibrvSenderClass()
{
   tracein("initTibrvSenderClass()");

   class QoreClass *QC_TIBRVSENDER = new QoreClass(strdup("TibrvSender"));
   CID_TIBRVSENDER = QC_TIBRVSENDER->getID();
   QC_TIBRVSENDER->addMethod("constructor",        TIBRVSENDER_constructor);
   QC_TIBRVSENDER->addMethod("destructor",         TIBRVSENDER_destructor);
   QC_TIBRVSENDER->addMethod("copy",               TIBRVSENDER_copy);
   QC_TIBRVSENDER->addMethod("sendSubject",        TIBRVSENDER_sendSubject);
   QC_TIBRVSENDER->addMethod("setStringEncoding",  TIBRVSENDER_setStringEncoding);
   QC_TIBRVSENDER->addMethod("getStringEncoding",  TIBRVSENDER_getStringEncoding);

   traceout("initTibrvSenderClass()");
   return QC_TIBRVSENDER;
}
