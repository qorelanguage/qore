/*
  QC_TibcoAdapter.cc

  TIBCO Active Enterprise integration to QORE

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

#include "QC_TibcoAdapter.h"
#include "TibCommandLine.h"
#include "tibae.h"
#include "QoreApp.h"

#include <memory>

qore_classid_t CID_TIBAE;

// usage: new TibcoAdapter(session-name, properties, classlist, [, service, network, daemon])
void TIBAE_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QORE_TRACE("TIBAE_constructor");

   const char *session_name, *service = NULL, *network = NULL, *daemon = NULL;
   const QoreStringNode *p0;
   const QoreHashNode *p1, *p2;
   const AbstractQoreNode *p;

   if (!(p0 = test_string_param(params, 0)) || !(p1 = test_hash_param(params, 1)))
   {
      xsink->raiseException("TIBCO-PARAMETER-ERROR", "invalid parameters passed to Tibco() constructor, expecting session name (string), properties (object), [classlist (object), service (string), network (string), daemon (string)]");

      return;
   }

   session_name = p0->getBuffer();
   QoreHashNodeHolder classlist(xsink);
   if ((p2 = test_hash_param(params, 2)))
   {
      // FIXME: check that classlist hash has only String values!
      classlist = p2->QoreHashNode::copy();
   }

   QoreString tmp;
   if ((p = get_param(params, 3)))
   {
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(p);
      if (str)
	 service = str->getBuffer();
      else if (p->getType() == NT_INT)
      {
	 tmp.sprintf("%lld", reinterpret_cast<const QoreBigIntNode *>(p)->val);
	 service = tmp.getBuffer();
      }
   }

   const QoreStringNode *pstr;
   if ((pstr = test_string_param(params, 3)))
      service = pstr->getBuffer();

   if ((pstr = test_string_param(params, 4)))
      network = pstr->getBuffer();

   if ((pstr = test_string_param(params, 5)))
      daemon = pstr->getBuffer();

   // create adapter instance
   printd(1, "TIBAE_constructor() session=%s service=%s network=%s daemon=%s\n",
	  session_name, 
	  service ? service : "(null)", 
	  network ? network : "(null)", 
	  daemon  ? daemon  : "(null)");
   class QoreApp *myQoreApp = NULL;

   MAppProperties *appProps = new MAppProperties();

   TibCommandLine tcl;
   set_properties(appProps, p1, tcl, xsink); 

   if (*xsink)
      return;

   try 
   {
      //printd(5, "before QoreApp constructor\n");
      myQoreApp =
         new QoreApp(appProps, session_name, classlist.release(), service, network, daemon);

      //printd(5, "after QoreApp constructor (%08p)\n", myQoreApp);

      //printd(5, "before start()\n");
      myQoreApp->start(Mfalse);
      //printd(5, "after start()\n");
   }
   catch (MException &te)
   {
      xsink->raiseException("TIBCO-EXCEPTION", "Exception thrown in Tibco() constructor %s: %s",
			 te.getType().c_str(), te.getDescription().c_str());
      if (myQoreApp)
	 myQoreApp->deref(xsink);

      return;
   }
   self->setPrivate(CID_TIBAE, myQoreApp);
   printd(5, "TIBAE_constructor() this=%08p myQoreApp=%08p\n", self, myQoreApp);

}

void TIBAE_copy(class QoreObject *self, class QoreObject *old, class QoreApp *myQoreApp, class ExceptionSink *xsink)
{
   xsink->raiseException("TIBCO-ADAPTER-COPY-ERROR", "copying TibcoAdapter objects is curently not supported");
}

// usage: TIBAE_sendSubject(subject, function_name, message)
class AbstractQoreNode *TIBAE_sendSubject(class QoreObject *self, class QoreApp *myQoreApp, const QoreListNode *params, class ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   const AbstractQoreNode *p2;

   // check input parameters
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)) ||
       is_nothing((p2 = get_param(params, 2))))
   {
      xsink->raiseException("TIBCO-SENDSUBJECT-PARAMETER-ERROR", "invalid parameters passed to TibcoAdapter::sendSubject(), expecting subject (string), function name (string), message (hash)");
      return NULL;
   }

   const char *subject = p0->getBuffer();
   const char *fname = p1->getBuffer();

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
   return 0;
}

// usage: Tibco::sendSubjectWithSyncReply(subject, function_name, message[, timeout])
class AbstractQoreNode *TIBAE_sendSubjectWithSyncReply(class QoreObject *self, class QoreApp *myQoreApp, const QoreListNode *params, class ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   const AbstractQoreNode *p2, *p3;

   // check input parameters
   if (!(p0 = test_string_param(params, 0)) ||
       !(p1 = test_string_param(params, 1)) ||
       is_nothing((p2 = get_param(params, 2))))
   {
      xsink->raiseException("TIBCO-SEND-WITH-SYNC-REPLY-PARAMETER-ERROR", "invalid parameters passed to tibco_send_with_sync_reply(), expecting subject (string), function name (string), message data, [timeout (date/time or integer milliseconds)]");
      return NULL;
   }

   const char *fname = p1->getBuffer();

   // set timeout parameter if present
   int timeout = 0;
   if ((p3 = get_param(params, 3)))
      timeout = getMsZeroInt(p3);

   // try to send message
   try
   {
      printd(2, "TIBAE_sendSubjectWithSyncReply() calling sendWithSyncReply()\n");
      return myQoreApp->sendWithSyncReply(p0->getBuffer(), fname, p2, timeout, xsink);
   }
   catch(MException &te)
   {
      xsink->raiseException("TIBCO-EXCEPTION", 
			    "QoreException caught while sending \"%s\" with subject \"%s\": %s: %s", 
			    fname, p0->getBuffer(), te.getType().c_str(), te.getDescription().c_str());      
   }

   return 0;
}

// Tibco::receive(subject, [timeout])
class AbstractQoreNode *TIBAE_receive(class QoreObject *self, class QoreApp *myQoreApp, const QoreListNode *params, class ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("TIBCO-RECEIVE-PARAMETER-ERROR", "invalid parameters passed to tibco_receive(), expecting subject (string), [timeout (date/time or integer milliseconds)]");
      return NULL;
   }

   const char *subject = p0->getBuffer();
   unsigned long timeout = 0;

   const AbstractQoreNode *p1; 
   if ((p1 = get_param(params, 1)))
      timeout = getMsZeroInt(p1);
   
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
static AbstractQoreNode* TIBAE_operationsCallWithSyncResult(QoreObject* self, QoreApp* myQoreApp, const QoreListNode *params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string), data (hash), "
      "[timeout (integer in millis or date/time), ] [client name (string)]";
   char* func = "TIBCO-OPERATIONS-CALL-WITH-SYNC_RESULT";
   const QoreStringNode* class_name = test_string_param(params, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->getBuffer(); 
   const QoreStringNode* method_name = test_string_param(params, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   } 
   const char* method_name_extracted = method_name->getBuffer();
   const QoreHashNode *data = test_hash_param(params, 2);
   if (!data) {
      return xsink->raiseException(func, err);
   }
   unsigned timeout = 60 * 1000;
   const char* client_name = "";
  
   int next_item = 3;
   const AbstractQoreNode* n = get_param(params, 3);
   qore_type_t ntype = n ? n->getType() : 0;
   if (ntype == NT_INT) {
      timeout = (unsigned)(reinterpret_cast<const QoreBigIntNode *>(n)->val);
      ++next_item;
   } else if (ntype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(n);
      timeout = (unsigned)date->getRelativeMilliseconds();
      ++next_item;
   }
   const QoreStringNode *nstr = test_string_param(params, next_item);
   if (nstr) {
      client_name = nstr->getBuffer();
   }

   return myQoreApp->operationsCallWithSyncResult(class_name_extracted, method_name_extracted, data, timeout, client_name, xsink);
}

//------------------------------------------------------------------------------
// The same parameters as for TIBAE_operationsCallWithSyncResult, 
// except for the timeout (not needed), always returns 0.
//
static AbstractQoreNode* TIBAE_operationsOneWayCall(QoreObject* self, QoreApp* myQoreApp, const QoreListNode *params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string), data (hash), "
      "[client name (string)]";
   char* func = "TIBCO-OPERATIONS-ONE-WAY-CALL";
   const QoreStringNode* class_name = test_string_param(params, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->getBuffer();
   const QoreStringNode* method_name = test_string_param(params, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   }
   const char* method_name_extracted = method_name->getBuffer();
   const QoreHashNode *data = test_hash_param(params, 2);
   if (!data) {
      return xsink->raiseException(func, err);
   }
   const char* client_name = "";

   const QoreStringNode* n = test_string_param(params, 3);
   if (n) {
      client_name = n->getBuffer();
   }

   myQoreApp->operationsOneWayCall(class_name_extracted, method_name_extracted, data, client_name, xsink);
   return 0;
}

// The same parameters as for TIBAE_operationsCallWithSyncResult (including timeout).
// Always return 0. To get the reply use combination of class name + method name passed to this call.
static AbstractQoreNode* TIBAE_operationsAsyncCall(QoreObject* self, QoreApp* myQoreApp, const QoreListNode *params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string), data (hash), "
      "[timeout (integer in millis or date/time), ] [client name (string)]";
   char* func = "TIBCO-OPERATIONS-ASYNC-CALL";
   const QoreStringNode* class_name = test_string_param(params, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->getBuffer();
   const QoreStringNode* method_name = test_string_param(params, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   }
   const char* method_name_extracted = method_name->getBuffer();
   const QoreHashNode *data = test_hash_param(params, 2);
   if (!data) {
      return xsink->raiseException(func, err);
   }
   unsigned timeout = 60 * 1000;
   const char* client_name = "";

   int next_item = 3;
   const AbstractQoreNode *n = get_param(params, 3);
   qore_type_t ntype = n ? n->getType() : 0;
   if (ntype == NT_INT) {
      timeout = (unsigned)(reinterpret_cast<const QoreBigIntNode *>(n)->val);
      ++next_item;
   } else if (ntype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(n);
      timeout = (unsigned)date->getRelativeMilliseconds();
      ++next_item;
   }
   const QoreStringNode *nstr = test_string_param(params, next_item);
   if (nstr) {
      client_name = nstr->getBuffer();
   }

   myQoreApp->operationsAsyncCall(class_name_extracted, method_name_extracted, data, timeout, client_name, xsink);
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
static AbstractQoreNode* TIBAE_operationsGetAsyncCallResult(QoreObject* self, QoreApp* myQoreApp, const QoreListNode *params, ExceptionSink *xsink)
{
   char* err = "Invalid parameters. Expected: class name (string), method name (string)";
   char* func = "TIBCO-OPERATIONS-GET-ASYNC-RESULT";
   const QoreStringNode* class_name = test_string_param(params, 0);
   if (!class_name) {
      return xsink->raiseException(func, err);
   }
   const char* class_name_extracted = class_name->getBuffer();
   const QoreStringNode* method_name = test_string_param(params, 1);
   if (!method_name) {
      return xsink->raiseException(func, err);
   }
   const char* method_name_extracted = method_name->getBuffer();

   return myQoreApp->operationsGetAsyncCallResult(class_name_extracted, method_name_extracted, xsink);
}

class QoreClass *initTibcoAdapterClass()
{
   QORE_TRACE("initTibcoAdapterClass()");

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


   return QC_TIBAE;
}

