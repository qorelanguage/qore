/*
  modules/Tuxedo/QC_TuxedoAdapter.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <assert.h>

#include "QC_TuxedoAdapter.h"
#include "QoreTuxedoAdapter.h"

int CID_TUXEDOADAPTER;

//------------------------------------------------------------------------------
static void getTuxedoAdapter(void* obj)
{
  ((QoreTuxedoAdapter*)obj)->ROreference();
}

static void releaseTuxedoAdapter(void* obj)
{
  ((QoreTuxedoAdapter*)obj)->deref();
}

//------------------------------------------------------------------------------
// []
// [string-name]
// [string-name, parameters-hash]
static void TUXEDOADAPTER_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDADAPTERO_constructor");

  const char* connection_name = 0;
  Hash* params_hash = 0;
  char* err = "QORE-TUXEDO-ADAPTER-CONNECTION";

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_STRING, 0);
    if (!pt) {
      xsink->raiseException(err, 
        "The first parameter (if any) needs to be a string (symbolic name). It could be empty.");
      return;
    }
    connection_name = pt->val.String->getBuffer();
  }

#ifdef DEBUG
  if (connection_name) {
    if (strcmp(connection_name, "test-fail-open") == 0) {
      xsink->raiseException(err, "Dummy adaptor asked to fail in constructor.");
      return;
    }
    if (strstr(connection_name, "test") == connection_name) {
      goto create_object;
    }
  }
#endif

  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_HASH, 1);
    if (!pt) {
      xsink->raiseException(err, "The second parameter (object settings) needs to be a hash.");
      return;
    }
  }

  if (get_param(params, 2)) {
    xsink->raiseException(err, "The Tuxedo::Adapter constructor can have up to two parameters.");
    return;
  }

#ifdef DEBUG
create_object:
#endif
  QoreTuxedoAdapter* adapter = new QoreTuxedoAdapter(connection_name, params_hash, err, xsink);
  if (xsink->isException()) {
    adapter->deref();
  } else {
    self->setPrivate(CID_TUXEDOADAPTER, adapter, getTuxedoAdapter, releaseTuxedoAdapter);
  }

  traceout("TUXEDOCONNECTION_constructor");
}

//------------------------------------------------------------------------------
static void TUXEDOADAPTER_destructor(Object *self, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  tracein("TUXEDOADAPTER_destructor");
  char* err = "QORE-TUXEDO-ADAPTER-DESTRUCTOR";
#ifdef DEBUG
  std::string name = adapter->get_name();
#endif

  adapter->close_adapter(err, xsink);
  adapter->deref();

#ifdef DEBUG
  if (name == "test-fail-close") {
    xsink->raiseException(err, "Dummy object asked to fail in destructor.");
  }
#endif

  traceout("TUXEDOADAPTER_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDOADAPTER_copy(Object *self, Object *old, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  xsink->raiseException("QORE-TUXEDO-ADAPTER-COPY", "copying Tuxedo::Adapter objects is not supported.");
}

//------------------------------------------------------------------------------
static QoreNode* call_impl(Object* self, QoreTuxedoAdapter* adapter, QoreNode *params, ExceptionSink *xsink, bool async)
{
   char* err = (char*)(async ? "QORE-TUXEDO-ADAPTER-ASYNC_CALL" : "QORE-TUXEDO-ADAPTER-CALL");

#ifdef DEBUG
  const char* n = adapter->get_name();
  if (strstr(n, "test") == n) {
    if (strstr(n, "test-fail-call") == n) {
      xsink->raiseException(err, "Test: dummy adapter failed.");
      return 0;
    }
    if (async) {
      return new QoreNode((int64)0);
    }
    if (strcmp(n, "test-success-bool") == 0) {
      return new QoreNode(true);
    }    
    if (strcmp(n, "test-success-int") == 0) {
      return new QoreNode((int64)123);
    }
    if (strcmp(n, "test-success-string") == 0) {
      return new QoreNode("test result");
    }
    assert(false);
    return new QoreNode();
  }
#endif

   char* service_name = 0;
   List* params_list = 0;
   long flags = 0;
   char* all_params = "Three parameters expected: service name (string), parameters (list), flags (integer). File %s[%d].";

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_STRING, 0);
    if (!pt) {
      xsink->raiseException(err, "The first parameter (service name) needs to be a string.");
      return 0;
    }
    service_name = pt->val.String->getBuffer();
    if (!service_name || !service_name[0]) {
      xsink->raiseException(err, "The first parameter (service name) should not be empty.");
      return 0;
    }
  } else {
    xsink->raiseException(err, all_params, __FILE__, __LINE__);
    return 0;
  }
  
  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_LIST, 1);
    if (!pt) {
      xsink->raiseException(err, "The second parameter (call arguments) needs to be a list.");
      return 0;
    }
    params_list = pt->val.list;
  } else {
    xsink->raiseException(err, all_params, __FILE__, __LINE__);
    return 0;
  }

  if (get_param(params, 2)) {
    QoreNode* pt = test_param(params, NT_INT, 2);
    if (!pt) {
      xsink->raiseException(err, "The third parameter (flags) needs to be an integer.");
      return 0;
    }
    flags = (long)pt->val.intval;
  } else {
    xsink->raiseException(err, all_params, __FILE__, __LINE__);
    return 0;
  }

  if (async) {
    int handle = adapter->async_call(service_name, params_list, flags, err, xsink);
    if (xsink->isException()) {
      return 0;
    }
    return new QoreNode((int64)handle);
  } else {
    List* result = adapter->call(service_name, params_list, flags, err, xsink);
    if (xsink->isException()) {
       delete result;
       return 0;
    }
    return new QoreNode(result);
  }
}

//------------------------------------------------------------------------------
// [service-name, parameters-list, flags], returns list with result or exception
static QoreNode* TUXEDOADAPTER_call(Object* self, QoreTuxedoAdapter* adapter, QoreNode *params, ExceptionSink *xsink)
{
  return call_impl(self, adapter, params, xsink, false);
}

//------------------------------------------------------------------------------
// [service-name, parameters-list, flags], returns integer handle or exception
static QoreNode* TUXEDOADAPTER_async_call(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  return call_impl(self, adapter, params, xsink, true);
}

//------------------------------------------------------------------------------
// [handle, flags]
static QoreNode* TUXEDOADAPTER_get_async_result(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-GET_ASYNC_RESULT";

#ifdef DEBUG
  const char* n = adapter->get_name();
  if (strstr(n, "test") == n) {

    if (strstr(n, "test-fail") == n) {
      xsink->raiseException(err, "Test: dummy adapter failed.");
      return 0;
    }

    // does not return list but for testing this should be OK
    if (strcmp(n, "test-success-bool") == 0) {
      return new QoreNode(true);
    }
    if (strcmp(n, "test-success-int") == 0) {
      return new QoreNode((int64)123);
    }
    if (strcmp(n, "test-success-string") == 0) {
      return new QoreNode("test result");
    }
    assert(false);
    return new QoreNode();
  }
#endif

  int handle = 0;
  long flags = 0;
  std::string suggested_output;
  char* all_params = "The function get_async_result() expects two parameters (handle, flags).";

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_INT, 0);
    if (!pt) {
      xsink->raiseException(err, "The first parameter (handle) needs to be an integer.");
      return 0;
    }
    handle = (int)pt->val.intval;
  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }
  
  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_INT, 1);
    if (!pt) {
      xsink->raiseException(err, "The second parameter (flags) needs to be an integer.");
      return 0;
    }
    flags = (long)pt->val.intval;
  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }
  
  if (get_param(params, 2)) {
    xsink->raiseException(err, all_params);
    return 0;
  }

  List* result = adapter->get_async_result(handle, flags, err, xsink);
  if (xsink->isException()) {
    delete result;
    return 0;
  }
  return new QoreNode(result);
}

//------------------------------------------------------------------------------
// [handle]
static QoreNode* TUXEDOADAPTER_cancel_async(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-CANCEL_ASYNC";
#ifdef DEBUG
  const char* n = adapter->get_name();
  if (strstr(n, "test") == n) {
    if (strstr(n, "test-fail-cancel") == n) {
      xsink->raiseException(err, "Test: dummy failure.");
    }
    return 0;
  }
#endif

  int handle = 0;
  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_INT, 0);
    if (!pt) {
      xsink->raiseException(err, "The parameter must be integer (the handle).");
      return 0;
    }
    handle = (int)pt->val.intval;
  } else {
    xsink->raiseException(err, "Integer parameter (the handle) must be supplied.");
    return 0;
  }

  if (get_param(params, 1)) {
    xsink->raiseException(err, "Only one parameter (the handle) is allowed.");
    return 0;
  }
 
  adapter->cancel_async(handle, err, xsink);
  return 0; 
}

//------------------------------------------------------------------------------
// [service-name, data-list, flags]
// [service-name, flags] for no data
static QoreNode* TUXEDOADAPTER_connect(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-CONNECT";
#ifdef DEBUG
  const char* n = adapter->get_name();
  if (strstr(n, "test") == n) {
    if (strcmp(n, "test-fail-connect") == 0) {
      xsink->raiseException(err, "connect() requested to fail.");
      return 0;
    }
    return new QoreNode((int64)0);
  }
#endif

  char* service_name = 0;
  List* params_list;
  long flags = 0;

  char* all_params = "Two or three parameters expected: service-name-string, [data-list], flags.";

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_STRING, 0);
    if (!pt) {
      xsink->raiseException(err, "The first parameter (service name) should be string.");
      return 0;
    }
    service_name = pt->val.String->getBuffer();

  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }
 
  int read_params_count = 1; 
  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_INT, 1);
    if (pt) {
      flags = (long)pt->val.intval;
      read_params_count = 2;
    } else {
      pt = test_param(params, NT_LIST, 1);
      if (!pt) {
        xsink->raiseException(err, "The second parameter should be either integer (flags) or a list (data).");
        return 0;
      } else {
        params_list = pt->val.list;

        if (!get_param(params, 2)) {
          xsink->raiseException(err, "Third parameter (flags) is missing.");
          return 0;
        }
        pt = test_param(params, NT_INT, 2);
        if (!pt) {
          xsink->raiseException(err, "The third parameter (flags) needs to be an integer.");
        }
        flags = (long)pt->val.intval;
        read_params_count = 3;
      }
    }
  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }

  if (get_param(params, read_params_count - 1)) {
    xsink->raiseException(err, all_params);
    return 0;
  }

  int res = adapter->connect(service_name, params_list, flags, err, xsink);
  if (xsink->isException()) {
    return 0;
  }

  return new QoreNode((int64)res);
}

//------------------------------------------------------------------------------
// [handle]
static QoreNode* TUXEDOADAPTER_forced_disconnect(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-FORCED_DISCONNECT";
#ifdef DEBUG
  const char* n = adapter->get_name();
  if (strstr(n, "test") == n) {
    if (strcmp(n, "test-fail-forced_connect") == 0) {
      xsink->raiseException(err, "forced_connect() requested to fail.");
      return 0;
    }
    return new QoreNode();
  }
#endif

  int handle = 0;
  char* all_params = "One integer parameter (the handle) is expected.";

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_INT, 0);
    if (!pt) {
      xsink->raiseException(err, "The first parameter (handle) should be integer.");
      return 0;
    }
    handle = pt->val.intval;

  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }

  if (get_param(params, 1)) {
    xsink->raiseException(err, all_params);
    return 0;
  }

  adapter->forced_disconnect(handle, err, xsink);
  return 0;
}

//------------------------------------------------------------------------------
// [handle, parameters-list, flags], returns True if finished, False if not yet finished
static QoreNode* TUXEDOADAPTER_send(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-SEND";
#ifdef DEBUG
  const char* n = adapter->get_name();
  if (strstr(n, "test") == n) {
    if (strcmp(n, "test-send-fail") == 0) {
      xsink->raiseException(err, "send() requested to fail.");
      return 0;
    }
    return new QoreNode();
  }
#endif

  int handle = 0;
  List* param_list = 0;
  long flags = 0;

  char* all_params = "Three parameters expected: handle (integer), data (list), flags (integer).";

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_INT, 0);
    if (!pt) {
      xsink->raiseException(err, "The first parameted (handle) needs to be an integer.");
      return 0;
    }
    handle = (int)pt->val.intval;
  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }

  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_LIST, 1);
    if (!pt) {
      xsink->raiseException(err, "The second parameter (data) needs to be a list.");
      return 0;
    }
    param_list = pt->val.list;

  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }
  
  if (get_param(params, 2)) {
    QoreNode* pt = test_param(params, NT_INT, 2);
    if (!pt) {
      xsink->raiseException(err, "The third parameter (flags) needs to be an integer.");
      return 0;
    }
    flags = (long)pt->val.intval;

  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }

  if (get_param(params, 3)) {
    xsink->raiseException(err, all_params);
    return 0;
  }

  bool res = adapter->send(handle, param_list, flags, err, xsink);
  return new QoreNode(res);
}

//------------------------------------------------------------------------------
// [handle, flags], returns list with two items: bool (True if finished) + data list
static QoreNode* TUXEDOADAPTER_recv(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO_ADAPTER-RECV";
#ifdef DEBUG
  const char* n = adapter->get_name();
  if (strstr(n, "test") == n) {
    if (strcmp(n, "test-fail-recv") == 0) {
      xsink->raiseException(err, "recv() required to fail.");
      return 0;
    }

    // does not return list but for testing this should be OK
    if (strcmp(n, "test-success-bool") == 0) {
      return new QoreNode(true);
    }
    if (strcmp(n, "test-success-int") == 0) {
      return new QoreNode((int64)123);
    }
    if (strcmp(n, "test-success-string") == 0) {
      return new QoreNode("test result");
    }
    assert(false);
    return new QoreNode();
  }
#endif
  int handle = 0;
  long flags = 0;
  char* all_params = "Two parameters are expected: hande (integer), flags (integer).";

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_INT, 0);
    if (!pt) {
      xsink->raiseException(err, "The first parameter (handle) needs to be an int.");
      return 0;
    }
    handle = (int)pt->val.intval;

  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }

  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_INT, 1);
    if (!pt) {
      xsink->raiseException(err, "The second parameter (flags) needs to be an integer.");
      return 0;
    }
    flags = (long)pt->val.intval;

  } else {
    xsink->raiseException(err, all_params);
    return 0;
  }

  if (get_param(params, 2)) {
    xsink->raiseException(err, all_params);
    return 0;
  }

  std::pair<bool, List*> result = adapter->recv(handle, flags, err, xsink);
  if (xsink->isException()) {
    delete result.second;
    return 0;
  }
  List* full_list = new List();
  full_list->push(new QoreNode(result.first));
  full_list->merge(result.second);
  return new QoreNode(full_list);
}

//------------------------------------------------------------------------------
class QoreClass* initTuxedoAdapterClass()
{
  tracein("initTuxedoAdapterClass");
  QoreClass* adapter = new QoreClass(QDOM_NETWORK, strdup("Adapter"));
  CID_TUXEDOADAPTER = adapter->getID();  

  adapter->setConstructor((q_constructor_t)TUXEDOADAPTER_constructor);
  adapter->setDestructor((q_destructor_t)TUXEDOADAPTER_destructor);
  adapter->setCopy((q_copy_t)TUXEDOADAPTER_copy);
  // synchronous request/response
  adapter->addMethod("call", (q_method_t)TUXEDOADAPTER_call);
  // asynchronous request/response
  adapter->addMethod("async_call", (q_method_t)TUXEDOADAPTER_async_call);
  adapter->addMethod("get_async_result", (q_method_t)TUXEDOADAPTER_get_async_result);
  adapter->addMethod("cancel_async", (q_method_t)TUXEDOADAPTER_cancel_async);
  // conversation service
  adapter->addMethod("connect", (q_method_t)TUXEDOADAPTER_connect);
  adapter->addMethod("forced_disconnect", (q_method_t)TUXEDOADAPTER_forced_disconnect);
  adapter->addMethod("send", (q_method_t)TUXEDOADAPTER_send);
  adapter->addMethod("recv", (q_method_t)TUXEDOADAPTER_recv);
  
  traceout("initTuxedoAdapterClass");
  return adapter;
}

// EOF

