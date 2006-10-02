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

  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_STRING, 0);
    if (!pt) {
      xsink->raiseException("QORE-TUXEDO-ADAPTER-CONNECTION",
        "The first parameter (if any) needs to be a string (symbolic name). It could be empty.");
      return;
    }
    connection_name = pt->val.String->getBuffer();
  }

#ifdef DEBUG
  if (connection_name && strstr(connection_name, "test") == connection_name) {
    goto create_object;
  }
#endif

  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_HASH, 1);
    if (!pt) {
      xsink->raiseException("QORE-TUXEDO-ADAPTER-CONSTRUCTOR",
        "The second parameter (object settings) needs to be a hash.");
      return;
    }
  }

  if (get_param(params, 2)) {
    xsink->raiseException("QORE-TUXEDO-ADAPTER-CONSTRUCTOR",
      "The Tuxedo::Adapter constructor can have up to two parameters.");
    return;
  }

#ifdef DEBUG
create_object:
#endif
  QoreTuxedoAdapter* adapter = new QoreTuxedoAdapter(connection_name, params_hash, xsink);
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
  adapter->close_adapter(xsink);
  adapter->deref();
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
  const char* n = adapter->name();
  if (strstr(n, "test") == n) {
    if (strstr(n, "test-fail") == n) {
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
      return 123;
    }
    if (strcmp(n, "test-success-string") == 0) {
      return new QoreNode("test result");
    }
    assert(false);
    return new QoreNode();
  }
#endif

   char* service_name = 0;
   Hash* params_hash = 0;
   long flags = 0;
   char* all_params = "Three parameters expected: service name (string), parameters (hash), flags (integer).";

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
    xsink->raiseException(err, all_params);
    return 0;
  }
  
  if (get_param(params, 1)) {
    QoreNode* pt = test_param(params, NT_HASH, 1);
    if (!pt) {
      xsink->raiseException(err, "The second parameter (call arguments) needs to be a hash.");
      return 0;
    }
    params_hash = pt->val.hash;
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

  if (async) {
    int handle = adapter->async_call(service_name, params_hash, flags, xsink);
    if (xsink->isException()) {
      return 0;
    }
    return new QoreNode((int64)handle);
  } else {
    Hash* result = adapter->call(service_name, params_hash, flags, xsink);
    if (xsink->isException()) {
       delete result;
       return 0;
    }
    return new QoreNode(result);
  }
}

//------------------------------------------------------------------------------
// [service-name, parameters-hash, flags], returns hash with result or exception
static QoreNode* TUXEDOADAPTER_call(Object* self, QoreTuxedoAdapter* adapter, QoreNode *params, ExceptionSink *xsink)
{
  return call_impl(self, adapter, params, xsink, false);
}

//------------------------------------------------------------------------------
// [service-name, parameters-hash, flags], returns integer handle or exception
static QoreNode* TUXEDOADAPTER_async_call(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  return call_impl(self, adapter, params, xsink, true);
}

//------------------------------------------------------------------------------
// [handle, flags]
// [handle, flags, out-hash]
static QoreNode* TUXEDOADAPTER_get_async_result(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-GET_ASYNC_RESULT";

#ifdef DEBUG
  const char* n = adapter->name();
  if (strstr(n, "test") == n) {
    if (strstr(n, "test-fail") == n) {
      xsink->raiseException(err, "Test: dummy adapter failed.");
      return 0;
    }
    if (strcmp(n, "test-success-bool") == 0) {
      return new QoreNode(true);
    }
    if (strcmp(n, "test-success-int") == 0) {
      return 123;
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
  Hash* params_hash = 0;
  char* all_params = "The function get_async_result() expects 2 or 3 parameters (handle, flags, suggested output).";

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
    QoreNode* pt = test_param(params, NT_INT, 0);
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
    QoreNode* pt = test_param(params, NT_HASH, 2);
    if (!pt) {
      xsink->raiseException(err, "The third parameter (suggested output) is expected to be hash.");
      return 0;
    }
    params_hash = pt->val.hash;
    
    if (get_param(params, 3)) {
      xsink->raiseException(err, all_params);
      return 0;
    }
  }

  Hash* result = adapter->get_async_result(handle, params_hash, flags, xsink);
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
#ifdef DEBUG
  const char* n = adapter->name();
  if (strstr(n, "test") == n) {
    if (strstr(n, "test-fail") == n) {
      xsink->raiseException("QORE-TUXEDO-ADAPTER-CANCEL_ASYNC", "Test: dummy failure.");
    }
    return 0;
  }
#endif

  int handle = 0;
  if (get_param(params, 0)) {
    QoreNode* pt = test_param(params, NT_INT, 0);
    if (!pt) {
      xsink->raiseException("QORE-TUXEDO-ADAPTER-CANCEL_ASYNC",
        "The parameter must be integer (the handle).");
      return 0;
    }
    handle = (int)pt->val.intval;
  } else {
    xsink->raiseException("QORE-TUXEDO-ADAPTER-CANCEL_ASYNC",
      "Integer parameter (the handle) must be supplied.");
    return 0;
  }

  if (get_param(params, 1)) {
    xsink->raiseException("QORE-TUXEDO-ADAPTER-CANCEL_ASYNC",
      "Only one parameter (the handle) is allowed.");
    return 0;
  }
 
  adapter->cancel_async(handle, xsink);
  return 0; 
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
  adapter->addMethod("call", (q_method_t)TUXEDOADAPTER_call);
  adapter->addMethod("async_call", (q_method_t)TUXEDOADAPTER_async_call);
  adapter->addMethod("get_async_result", (q_method_t)TUXEDOADAPTER_get_async_result);
  adapter->addMethod("cancel_async", (q_method_t)TUXEDOADAPTER_cancel_async);

  traceout("initTuxedoAdapterClass");
  return adapter;
}

// EOF

