/*
  QC_TibcoAdapter.cc

  TIBCO Active Enterprise integration to QORE

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

#include "QC_TibcoAdapter.h"
#include "TibCommandLine.h"
#include "tibae.h"
#include "QoreApp.h"

#include <memory>

int CID_TIBAE;

// usage: new TibcoAdapter(session-name, properties, classlist, [, service, network, daemon])
void TIBAE_constructor(class QoreObject *self, const QoreNode *params, class ExceptionSink *xsink)
{
   tracein("TIBAE_constructor");

   const char *session_name, *service = NULL, *network = NULL, *daemon = NULL;
   QoreNode *p0, *p1, *p2, *p;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_HASH, 1)))
   {
      xsink->raiseException("TIBCO-PARAMETER-ERROR", "invalid parameters passed to Tibco() constructor, expecting session name (string), properties (object), [classlist (object), service (string), network (string), daemon (string)]");
      traceout("TIBAE_constructor");
      return;
   }

   session_name = p0->val.String->getBuffer();
   QoreHash *classlist;
   if ((p2 = test_param(params, NT_HASH, 2)))
   {
      // FIXME: check that classlist hash has only String values!
      classlist = p2->val.hash->copy();
   }
   else
      classlist = NULL;

   QoreString tmp;
   if ((p = get_param(params, 3)))
   {
      if (p->type == NT_STRING)
	 service = p->val.String->getBuffer();
      else if (p->type == NT_INT)
      {
	 tmp.sprintf("%lld", p->val.intval);
	 service = tmp.getBuffer();
      }
   }
   
   if ((p = test_param(params, NT_STRING, 3)))
      service = p->val.String->getBuffer();

   if ((p = test_param(params, NT_STRING, 4)))
      network = p->val.String->getBuffer();

   if ((p = test_param(params, NT_STRING, 5)))
      daemon = p->val.String->getBuffer();

   // create adapter instance
   printd(1, "TIBAE_constructor() session=%s service=%s network=%s daemon=%s\n",
	  session_name, 
	  service ? service : "(null)", 
	  network ? network : "(null)", 
	  daemon  ? daemon  : "(null)");
   class QoreApp *myQoreApp = NULL;

   MAppProperties *appProps = new MAppProperties();

   TibCommandLine tcl;
   set_properties(appProps, p1->val.hash, tcl, xsink); 

   if (xsink->isEvent())
   {
      if (classlist)
	 classlist->derefAndDelete(xsink);
      return;
   }
   try 
   {
      //printd(5, "before QoreApp constructor\n");
      myQoreApp =
         new QoreApp(appProps, session_name, classlist, service, network, daemon);

      //printd(5, "after QoreApp constructor (%08p)\n", myQoreApp);

      //printd(5, "before start()\n");
      myQoreApp->start(Mfalse);
      //printd(5, "after start()\n");
   }
   catch (MException &te)
   {
      xsink->raiseException("TIBCO-EXCEPTION", "QoreException thrown in Tibco() constructor %s: %s",
			 te.getType().c_str(), te.getDescription().c_str());
      if (myQoreApp)
	 myQoreApp->deref(xsink);
      traceout("TIBAE_constructor");
      return;
   }
   self->setPrivate(CID_TIBAE, myQoreApp);
   printd(5, "TIBAE_constructor() this=%08p myQoreApp=%08p\n", self, myQoreApp);
   traceout("TIBAE_constructor");
}

void TIBAE_copy(class QoreObject *self, class QoreObject *old, class QoreApp *myQoreApp, class ExceptionSink *xsink)
{
   xsink->raiseException("TIBCO-ADAPTER-COPY-ERROR", "copying TibcoAdapter objects is curently not supported");
}

// usage: TIBAE_sendSubject(subject, function_name, message)
class QoreNode *TIBAE_sendSubject(class QoreObject *self, class QoreApp *myQoreApp, const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2;

   // check input parameters
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)) ||
       !(p2 = test_param(params, NT_HASH, 2)))
   {
      xsink->raiseException("TIBCO-SENDSUBJECT-PARAMETER-ERROR", "invalid parameters passed to TibcoAdapter::sendSubject(), expecting subject (string), function name (string), message (hash)");
      return NULL;
   }

   const char *subject = p0->val.String->getBuffer();
   const char *fname = p1->val.String->getBuffer();

   // try to send message
   try
   {
      myQoreApp->send(subject, fname, p2, xsink);
   }
   catch(MException &te)
   {
      xsink->raiseException("TIBCO-EXCEPTION", 
			    "QoreException caught while sending \"%s\" with subject \"%s\": %s: %s", 
			    fname, subject, te.getType().c_str(), te.getDescription().c_str());      
   }
   return NULL;
}

// usage: Tibco::sendSubjectWithSyncReply(subject, function_name, message[, timeout])
class QoreNode *TIBAE_sendSubjectWithSyncReply(class QoreObject *self, class QoreApp *myQoreApp, const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2, *p3;

   // check input parameters
   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)) ||
       !((p2 = test_param(params, NT_OBJECT, 2)) || (p2 = test_param(params, NT_HASH, 2))))
   {
      xsink->raiseException("TIBCO-SEND-WITH-SYNC-REPLY-PARAMETER-ERROR", "invalid parameters passed to tibco_send_with_sync_reply(), expecting subject (string), function name (string), message (object), [timeout (date/time or integer milliseconds)]");
      return NULL;
   }

   const char *fname = p1->val.String->getBuffer();

   // set timeout parameter if present
   int timeout = 0;
   if ((p3 = get_param(params, 3))) {
      QoreNode* n = test_param(params, NT_INT, 3);
      if (n) {
        timeout = p3->getAsInt();
      } else {
        n = test_param(params, NT_DATE, 3);
        if (!n) {
          xsink->raiseException("TIBCO-SEND-WITH-SYNC-REPLY-PARAMETER-ERROR", "The timeout parameter needs to be integer or string.");
          return 0;
        }
        timeout = n->val.date_time->getRelativeMilliseconds();
      }
   }

   // try to send message
   try
   {
      printd(2, "TIBAE_sendSubjectWithSyncReply() calling sendWithSyncReply()\n");
      return myQoreApp->sendWithSyncReply(p0->val.String->getBuffer(), fname, p2, timeout, xsink);
   }
   catch(MException &te)
   {
      xsink->raiseException("TIBCO-EXCEPTION", 
			    "QoreException caught while sending \"%s\" with subject \"%s\": %s: %s", 
			    fname, p0->val.String->getBuffer(), te.getType().c_str(), te.getDescription().c_str());      
   }

   return NULL;
}

// Tibco::receive(subject, [timeout])
class QoreNode *TIBAE_receive(class QoreObject *self, class QoreApp *myQoreApp, const QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("TIBCO-RECEIVE-PARAMETER-ERROR", "invalid parameters passed to tibco_receive(), expecting subject (string), [timeout (date/time or integer milliseconds)]");
      return NULL;
   }

   const char *subject = p0->val.String->getBuffer();
   unsigned long timeout = 0;
   if ((p1 = get_param(params, 1))) {
     QoreNode* n = test_param(params, NT_INT, 1);
     if (n) {
       timeout = (unsigned long)n->val.intval;
     } else {
       n = test_param(params, NT_DATE, 3);
       if (n) {
        timeout = (unsigned long)n->val.date_time->getRelativeMilliseconds();
       } else {
        xsink->raiseException("TIBCO-RECEIVE-PARAMETER-ERROR", "timeout parameter needs to be either integer or date/time.");
       }
     }
   }

   return myQoreApp->receive(subject, timeout, xsink);
}

//------------------------------------------------------------------------------
// Parameters:
// * class name (string), in Tibco docs called "name of the class repository object in which this operation is defined"
//   Example: "greetings".
// * method name (string), in Tibco docs "name of the operation repository that defined this operation"
//   Example: "setGreetings"
// * data (hash), keys are string parameter names + appropriate type (according to repository) of values
// * optional: timeout (date/time or integer, millseconds), default is 60 seconds, 0 means infinite
// * optional: client name (string), in Tibco docs "this name must refer to a client repository object defined 
//   in endpoint.clients directory
//   Default value is ""
//
// Returns hash with data send as a reply to this call.
//
static QoreNode* TIBAE_operationsCallWithSyncResult(QoreObject* self, QoreApp* myQoreApp, QoreNode* params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string), data (hash), "
      "[timeout (integer in millis or date/time), ] [client name (string)]";
   char* func = "TIBCO-OPERATIONS-CALL-WITH-SYNC_RESULT";
   QoreNode* class_name = test_param(params, NT_STRING, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->val.String->getBuffer(); 
   QoreNode* method_name = test_param(params, NT_STRING, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   } 
   const char* method_name_extracted = method_name->val.String->getBuffer();
   QoreNode* data = test_param(params, NT_HASH, 2);
   if (!data) {
      return xsink->raiseException(func, err);
   }
   QoreHash* data_extracted = data->val.hash;
   unsigned timeout = 60 * 1000;
   const char* client_name = "";
  
   int next_item = 3;
   QoreNode* n = test_param(params, NT_INT, 3);
   if (n) {
      timeout = (unsigned)n->val.intval;
      ++next_item;
   } else {
      n = test_param(params, NT_DATE, 3);
      if (n) {
	 timeout = (unsigned)n->val.date_time->getRelativeMilliseconds();
	 ++next_item;
      }
   }
   n = test_param(params, NT_STRING, next_item);
   if (n) {
      client_name = n->val.String->getBuffer();
   }

   return myQoreApp->operationsCallWithSyncResult(class_name_extracted, method_name_extracted, data_extracted,
						  timeout, client_name, xsink);
}

//------------------------------------------------------------------------------
// The same parameters as for TIBAE_operationsCallWithSyncResult, 
// except for the timeout (not needed), always returns 0.
//
static QoreNode* TIBAE_operationsOneWayCall(QoreObject* self, QoreApp* myQoreApp, QoreNode* params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string), data (hash), "
      "[client name (string)]";
   char* func = "TIBCO-OPERATIONS-ONE-WAY-CALL";
   QoreNode* class_name = test_param(params, NT_STRING, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->val.String->getBuffer();
   QoreNode* method_name = test_param(params, NT_STRING, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   }
   const char* method_name_extracted = method_name->val.String->getBuffer();
   QoreNode* data = test_param(params, NT_HASH, 2);
   if (!data) {
      return xsink->raiseException(func, err);
   }
   QoreHash* data_extracted = data->val.hash;
   const char* client_name = "";

   QoreNode* n = test_param(params, NT_STRING, 3);
   if (n) {
      client_name = n->val.String->getBuffer();
   }

   myQoreApp->operationsOneWayCall(class_name_extracted, method_name_extracted, data_extracted, client_name, xsink);
   return 0;
}

// The same parameters as for TIBAE_operationsCallWithSyncResult (including timeout).
// Always return 0. To get the reply use combination of class name + method name passed to this call.
static QoreNode* TIBAE_operationsAsyncCall(QoreObject* self, QoreApp* myQoreApp, QoreNode* params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string), data (hash), "
      "[timeout (integer in millis or date/time), ] [client name (string)]";
   char* func = "TIBCO-OPERATIONS-ASYNC-CALL";
   QoreNode* class_name = test_param(params, NT_STRING, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->val.String->getBuffer();
   QoreNode* method_name = test_param(params, NT_STRING, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   }
   const char* method_name_extracted = method_name->val.String->getBuffer();
   QoreNode* data = test_param(params, NT_HASH, 2);
   if (!data) {
      return xsink->raiseException(func, err);
   }
   QoreHash* data_extracted = data->val.hash;
   unsigned timeout = 60 * 1000;
   const char* client_name = "";

   int next_item = 3;
   QoreNode* n = test_param(params, NT_INT, 3);
   if (n) {
      timeout = (unsigned)n->val.intval;
      ++next_item;
   } else {
      n = test_param(params, NT_DATE, 3);
      if (n) {
	 timeout = (unsigned)n->val.date_time->getRelativeMilliseconds();
	 ++next_item;
      }
   }
   n = test_param(params, NT_STRING, next_item);
   if (n) {
      client_name = n->val.String->getBuffer();
   }

   myQoreApp->operationsAsyncCall(class_name_extracted, method_name_extracted, data_extracted, timeout, client_name, xsink);
   return 0;
}

//------------------------------------------------------------------------------
// Parameters:
// * class name (string)
// * method name (string)
// The class/method need to be invoked prior with operationsAsyncCall().
//
// Returns hash with retrieved values.
//
static QoreNode* TIBAE_operationsGetAsyncCallResult(QoreObject* self, QoreApp* myQoreApp, QoreNode* params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string)";
   char* func = "TIBCO-OPERATIONS-GET-ASYNC-RESULT";
   QoreNode* class_name = test_param(params, NT_STRING, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->val.String->getBuffer();
   QoreNode* method_name = test_param(params, NT_STRING, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   }
   const char* method_name_extracted = method_name->val.String->getBuffer();

   return myQoreApp->operationsGetAsyncCallResult(class_name_extracted, method_name_extracted, xsink);
}

class QoreClass *initTibcoAdapterClass()
{
   tracein("initTibcoAdapterClass()");

   class QoreClass *QC_TIBAE = new QoreClass("TibcoAdapter", QDOM_NETWORK);
   CID_TIBAE = QC_TIBAE->getID();
   //printd(5, "initTibcoAdapterClass() CID_TIBAE=%d\n", CID_TIBAE);
   QC_TIBAE->setConstructor(TIBAE_constructor);
   QC_TIBAE->setCopy((q_copy_t)TIBAE_copy);
   QC_TIBAE->addMethod("receive",                  (q_method_t)TIBAE_receive);
   QC_TIBAE->addMethod("sendSubject",              (q_method_t)TIBAE_sendSubject);
   QC_TIBAE->addMethod("sendSubjectWithSyncReply", (q_method_t)TIBAE_sendSubjectWithSyncReply);

   // operations
   QC_TIBAE->addMethod("callOperationWithSyncReply",  (q_method_t)TIBAE_operationsCallWithSyncResult);
   QC_TIBAE->addMethod("callOperationOneWay",         (q_method_t)TIBAE_operationsOneWayCall);

   /*
   // commented out for now as the retrieving async call method still fails
   QC_TIBAE->addMethod("callOperationAsync",          (q_method_t)TIBAE_operationsAsyncCall);
   QC_TIBAE->addMethod("getAsyncOperationCallResult", (q_method_t)TIBAE_operationsGetAsyncCallResult);
   */

   traceout("initTibcoAdapterClass()");
   return QC_TIBAE;
}

