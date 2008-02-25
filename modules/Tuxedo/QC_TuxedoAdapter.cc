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

#include <qore/Qore.h>
#include <qore/minitest.hpp>

#include "userlog.h"
#include <string>
#include <vector>
#include <tx.h>

#include "QC_TuxedoAdapter.h"
#include "QoreTuxedoAdapter.h"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::map;

int CID_TUXEDOADAPTER;
#ifdef DEBUG
int CID_TUXEDOTEST;
#endif

//------------------------------------------------------------------------------
#ifdef DEBUG
// helper to ease testing on different machines
static const char* tuxfile(const char* file_name_part)
{  
  static string result;

  result = getenv("HOME");
  result += "/";
  result += file_name_part;
  
  return result.c_str();
}
#endif

//------------------------------------------------------------------------------

#ifdef DEBUG
static void TUXEDOTEST_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
  QoreTuxedoTest* tst = new QoreTuxedoTest;
  self->setPrivate(CID_TUXEDOTEST, tst);
}
static void TUXEDOTEST_destructor(QoreObject *self, QoreTuxedoTest* test, ExceptionSink *xsink)
{
  test->deref();
}
#endif

//------------------------------------------------------------------------------
static void TUXEDO_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-CONSTRUCTOR";
  tracein(err_name);
  
  const QoreHashNode *h = test_hash_param(params, 0);
  if (!h) {
    xsink->raiseException(err_name, "Hash with Tuxedo settings expected");
    return;
  }

  QoreTuxedoAdapter* adapter = new QoreTuxedoAdapter(h, xsink);
  if (*xsink) {    
    delete adapter;
    return;
  }

  self->setPrivate(CID_TUXEDOADAPTER, adapter);
  traceout(err_name);
}

#ifdef DEBUG
/* this kind of tests is not deprecated in favor of using QoreProgram
TEST()
{
  char buffer[1024];
  sprintf(buffer, "qore -e '%%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoTest();\n"
    "delete $a;\n"
    "exit(10);'\n",
    tuxfile(TUXCONFIG_SIMPLE_TEST)
  );

  int res = system(buffer);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
*/

TEST()
{
  QoreString str;
  str.sprintf("%%requires tuxedo\n"
    "sub test() {\n"
    "$a = new Tuxedo::TuxedoTest();\n"
    "delete $a;\n"
    "return 10; }\n",
    tuxfile(TUXCONFIG_SIMPLE_TEST)
  );

  ExceptionSink xsink;
  QoreProgram *pgm = new QoreProgram();
  pgm->parse(str.getBuffer(), "test", &xsink);
  if (xsink.isEvent()) {
    assert(false);
  } else {
     AbstractQoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->getType() != NT_INT) {
       assert(false);
     } else
	if (rv->getAsInt() != 10) {
       assert(false);
     }
     discard(rv, &xsink);
  }
  pgm->deref(&xsink);
  xsink.handleExceptions();
}


#ifdef TUXCONFIG_SIMPLE_TEST
TEST()
{
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;
  printf("testing construction - simple test server should run\n");

  QoreString str;
  str.sprintf("%%requires tuxedo\n"
    "sub test() { $settings = (\"TUXCONFIG\" : \"%s\", \"TUXDIR\" : \"" TUXDIR "\");\n"
    "$a = new Tuxedo::TuxedoAdapter($settings);\n"
    "delete $a;\n"
    "return 10; }\n",
    tuxfile(TUXCONFIG_SIMPLE_TEST)
  );

  ExceptionSink xsink;
  QoreProgram *pgm = new QoreProgram();
  pgm->parse(str.getBuffer(), "test", &xsink);
  if (xsink.isEvent()) {
    assert(false);
  } else {
     AbstractQoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();     
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->getType() != NT_INT) {
       assert(false);
     } else
	if (rv->getAsInt() != 10) {
       assert(false);
     }
     discard(rv, &xsink);
  }
  pgm->deref(&xsink);
  xsink.handleExceptions();
}
#endif
#endif

//------------------------------------------------------------------------------
static void TUXEDO_destructor(QoreObject *self, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  tracein("TUXEDO_destructor");
  adapter->deref();
  traceout("TUXEDO_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDO_copy(QoreObject *self, QoreObject *old, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-ADAPTER-COPY", "copying Tuxedo::TuxedoAdapter objects is not yet supported");
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* call(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-CALL";
  const QoreStringNode* nstr = test_string_param(params, 0);
  if (!nstr) return xsink->raiseException(err_name, "First parameter needs to be service string");
  const char* service_name = nstr->getBuffer();
  if (!service_name || !service_name[0]) {
    return xsink->raiseException(err_name, "Service name string cannot be empty");
  }
  const AbstractQoreNode* data = get_param(params, 1);
  qore_type_t dtype = data ? data->getType() : 0;
  if (dtype && !(dtype == NT_NOTHING || dtype == NT_STRING || dtype == NT_BINARY || dtype == NT_HASH))
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32)");

  // optional settings are either (1) integer flags or (2) hash with flags, suggested out data type, FML/FML32 selector
  const QoreHashNode *call_settings = 0;
  long flags = 0;
  long* pflags = 0;

  const AbstractQoreNode *n = get_param(params, 2);
  if (!is_nothing(n)) {
     qore_type_t ntype = n->getType();
     if (ntype == NT_HASH)
	call_settings = reinterpret_cast<const QoreHashNode *>(n);
     else {
        flags = (long)n->getAsBigInt();
        pflags = &flags;
     }
  }
  adapter->setSendBuffer(data, call_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for call()");
  }
  adapter->switchToSavedContext();
  return adapter->call(service_name, call_settings, pflags, xsink);
}

#ifdef DEBUG
#ifdef TUXCONFIG_SIMPLE_TEST
TEST()
{
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;
  printf("testing synchronous call - simple test server should run\n");

  QoreString str;
  str.sprintf("%%requires tuxedo\n"
   "sub test() {\n"
    "$settings = (\"TUXCONFIG\" : \"%s\", \"TUXDIR\" : \"" TUXDIR "\");\n"
    "$a = new Tuxedo::TuxedoAdapter($settings);\n"
    "$res = $a.call(\"TOUPPER\", \"abcd\", 0);\n"
    "if ($res != \"ABCD\") { printf(\"call() failed\n\"); return 12; }\n"
    "$res = $a.call(\"TOUPPER\", \"xyz\", 0);\n" // second call
    "if ($res != \"XYZ\") { printf(\"call() failed\n\"); return 11; }\n"
    "delete $a;\n"
    "return 10; }\n",
    tuxfile(TUXCONFIG_SIMPLE_TEST)
  );

  ExceptionSink xsink;
  QoreProgram *pgm = new QoreProgram();
  pgm->parse(str.getBuffer(), "test", &xsink);
  if (xsink.isEvent()) {
    assert(false);
  } else {
     AbstractQoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->getType() != NT_INT) {
       assert(false);
     } else
	if (rv->getAsInt() != 10) {
       assert(false);
     }
     discard(rv, &xsink);
  }
  pgm->deref(&xsink);
  xsink.handleExceptions();
}
#endif
#endif

//-----------------------------------------------------------------------------
static AbstractQoreNode* setStringEncoding(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  const QoreStringNode* n = test_string_param(params, 0);
  if (!n) return xsink->raiseException("TUXEDO-ADAPTER-SET-STRING-ENCODING", "One parameter expected: string name of the encoding");
  const char* name = n->getBuffer();
  adapter->setStringEncoding(name);
  return 0;
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* asyncCall(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-ASYNC-CALL";
  const QoreStringNode* nstr = test_string_param(params, 0);
  if (!nstr) return xsink->raiseException(err_name, "First parameter needs to be service string");
  const char* service_name = nstr->getBuffer();
  if (!service_name || !service_name[0]) {
    return xsink->raiseException(err_name, "Service name string cannot be empty");
  }
  const AbstractQoreNode* data = get_param(params, 1);
  qore_type_t dtype = data ? data->getType() : 0;
  if (dtype && !(dtype == NT_NOTHING || dtype == NT_STRING || dtype == NT_BINARY || dtype == NT_HASH))
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32)");

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  const QoreHashNode *acall_settings = 0;
  long flags = 0;
  long* pflags = 0;

  const AbstractQoreNode *n = get_param(params, 2);
  if (!is_nothing(n)) {
     const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
     if (h)
	acall_settings = h;
     else {
	flags = (long)n->getAsBigInt();
	pflags = &flags;
     }
  }
  adapter->setSendBuffer(data, acall_settings, err_name, xsink);
  if (xsink->isException()) {
     return xsink->raiseException(err_name, "Invalid parameter for asyncCall()");
  }
  adapter->switchToSavedContext();
  return adapter->acall(service_name, acall_settings, pflags, xsink);
}

#ifdef DEBUG
#ifdef TUXCONFIG_SIMPLE_TEST
TEST()
{
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;
  printf("testing asyncCall() - simple test server should run\n");

  // send 3 requests asynchronously, cancel one
  QoreString str;
  str.sprintf("%%requires tuxedo\n"
    "sub test() {\n"
    "$settings = (\"TUXCONFIG\" : \"%s\", \"TUXDIR\" : \"" TUXDIR "\");\n"
    "$a = new Tuxedo::TuxedoAdapter($settings);\n"

    "$res1 = $a.asyncCall(\"TOUPPER\", \"data1\", 0);\n"
    "$res2 = $a.asyncCall(\"TOUPPER\", \"data2\", 0);\n"
    "$res3 = $a.asyncCall(\"TOUPPER\", \"data3\", 0);\n"
    // cancel req 2
    "$a.cancelAsyncCall($res2);\n"

    // read req 3 and 1
    "$res = $a.waitForAsyncReply($res3, Tuxedo::TPNOTIME);\n"
    "if ($res.data != \"DATA3\") { printf(\"service 3 failed\n\"); return 11; }\n"
    "$res = $a.waitForAsyncReply($res1, Tuxedo::TPNOTIME);\n"
    "if ($res.data != \"DATA1\") { printf(\"service 1 failed\n\"); return 12; }\n"
    "return 10; }\n",
    tuxfile(TUXCONFIG_SIMPLE_TEST)
  );

  ExceptionSink xsink;
  QoreProgram *pgm = new QoreProgram();
  pgm->parse(str.getBuffer(), "test", &xsink);
  if (xsink.isEvent()) {
    assert(false);
  } else {
     AbstractQoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->getType() != NT_INT) {
       assert(false);
     } else
	if (rv->getAsInt() != 10) {
       assert(false);
     }
     discard(rv, &xsink);
  }
  pgm->deref(&xsink);
  xsink.handleExceptions();
}

TEST()
{
  // send asynchronous, read in an other thread
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;
  printf("testing asyncCall() multithreaded - simple server should run\n");

  QoreString str;
  str.sprintf("%%requires tuxedo\n"
    "our $handle;\n"
    "our $result = 13;\n"

    "sub bgthread($my_a, $my_handle) {\n"
    "  my $res = $my_a.waitForAsyncReply($my_handle, Tuxedo::TPNOTIME);\n"
    "  if ($res.data != \"DATA1\") { printf(\"service failed\n\"); $result = 15; return; }\n"
    "  $result = 10;\n"
    "}\n"

    "sub test() {\n"
    "$settings = (\"TUXCONFIG\" : \"%s\", \"TUXDIR\" : \"" TUXDIR "\");\n"
    "our $a = new Tuxedo::TuxedoAdapter($settings);\n"

    "our $handle = $a.asyncCall(\"TOUPPER\", \"data1\", 0);\n"
    "background bgthread($a, $handle);\n"
    "sleep(1);\n"
    "return $result; }\n",
    tuxfile(TUXCONFIG_SIMPLE_TEST)
    );

  ExceptionSink xsink;
  QoreProgram *pgm = new QoreProgram();
  pgm->parse(str.getBuffer(), "test", &xsink);
  if (xsink.isEvent()) {
    assert(false);
  } else {
     AbstractQoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->getType() != NT_INT) {
       assert(false);
     } else
	if (rv->getAsInt() != 10) {
       assert(false);
     }
     discard(rv, &xsink);
  }
  pgm->deref(&xsink);
  xsink.handleExceptions();
}

#endif
#endif

//-----------------------------------------------------------------------------
static AbstractQoreNode* cancelAsyncCall(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  const AbstractQoreNode* n = get_param(params, 0);
  int handle = n ? n->getAsInt() : 0;
  adapter->remove_pending_async_call(handle);
  int res = tpcancel(handle);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpcancel"), "tpcancel() failed with error %d", tperrno);
}

//------------------------------------------------------------------------------
static AbstractQoreNode* waitForAsyncReply(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  const AbstractQoreNode* n = get_param(params, 0);
  int handle = n ? n->getAsInt() : 0;

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  const QoreHashNode *getrply_settings = 0;
  long flags = 0;
  long* pflags = 0;

  n = get_param(params, 1);
  if (!is_nothing(n)) {
     const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
     if (h)
	getrply_settings = h;
     else {
	flags = (long)n->getAsBigInt();
	pflags = &flags;
     }
  }

  adapter->remove_pending_async_call(handle); // remove it even if the tpgetrply() fails (I do not know better solution)
  return adapter->get_reply(handle, getrply_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* joinConversation(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-JOIN-CONVERSATION";
  const QoreStringNode* nstr = test_string_param(params, 0);
  if (!nstr) return xsink->raiseException(err_name, "First parameter needs to be service name string");
  const char* service_name = nstr->getBuffer();
  if (!service_name || !service_name[0]) {
    return xsink->raiseException(err_name, "Service name string cannot be empty");
  }
  const AbstractQoreNode* data = get_param(params, 1);
  qore_type_t dtype = data ? data->getType() : 0;
  if (dtype && !(dtype == NT_NOTHING || dtype == NT_STRING || dtype == NT_BINARY || dtype == NT_HASH))
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32)");

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  const QoreHashNode *connect_settings = 0;
  long flags = 0;
  long* pflags = 0;

  const AbstractQoreNode *n = get_param(params, 2);
  if (!is_nothing(n)) {
     const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
     if (h)
	connect_settings = h;
     else {
	flags = (long)n->getAsBigInt();
	pflags = &flags;
     }
  }
  adapter->setSendBuffer(data, connect_settings, err_name, xsink);
  if (xsink->isException()) {
     return xsink->raiseException(err_name, "Invalid parameter for joinConversation()");
  }
  adapter->switchToSavedContext();
  return adapter->connect(service_name, connect_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* breakConversation(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-BREAK-CONVERSATION";
  adapter->switchToSavedContext();

  const AbstractQoreNode* n = get_param(params, 0);
  int handle = n ? n->getAsInt() : 0;
  int res = tpdiscon(handle);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpdiscon"), "tpdiscon() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* sendConversationData(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-SEND-CONVERSATION-DATA";
  const AbstractQoreNode* n = get_param(params, 0);
  long handle = (long)(n ? n->getAsBigInt() : 0);

  const AbstractQoreNode* data = get_param(params, 1);
  qore_type_t dtype = data ? data->getType() : 0;
  if (dtype && !(dtype == NT_NOTHING || dtype == NT_STRING || dtype == NT_BINARY || dtype == NT_HASH))
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32)");

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  const QoreHashNode *send_settings = 0;
  long flags = 0;
  long* pflags = 0;

  n = get_param(params, 2);
  if (n) {
     const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
     if (h)
	send_settings = h;
     else {
	flags = (long)n->getAsBigInt();
	pflags = &flags;
     }
  }
  adapter->setSendBuffer(data, send_settings, err_name, xsink);
  if (xsink->isException()) {
     return xsink->raiseException(err_name, "Invalid parameter for sendConversationData()");
  }
  
  adapter->switchToSavedContext();
  return adapter->send(handle, send_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* receiveConversationData(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
   const AbstractQoreNode* n = get_param(params, 0);
   long handle = (long)(n ? n->getAsBigInt() : 0);

   // optional settings are either (1) integer flags or (2) hash with flags and out data type selector
   const QoreHashNode *receive_settings = 0;
   long flags = 0;
   long* pflags = 0;

   n = get_param(params, 1);
   if (!is_nothing(n)) {
      const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
      if (h)
	 receive_settings = h;
      else {
	 flags = (long)n->getAsBigInt();
	 pflags = &flags;
      }
   }
   
   adapter->switchToSavedContext();
   return adapter->receive(handle, receive_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* enqueue(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-ENQUEUE";
  const QoreStringNode* nstr = test_string_param(params, 0);
  if (!nstr) return xsink->raiseException(err_name, "First parameter needs to be queue space string");
  const char* queue_space = nstr->getBuffer();
  if (!queue_space || !queue_space[0]) {
    return xsink->raiseException(err_name, "Queue space parameter cannot be empty");
  }
  nstr = test_string_param(params, 1);
  if (!nstr) return xsink->raiseException(err_name, "Second parameter needs to be queue name string");
  const char* queue_name = nstr->getBuffer();
  if (!queue_name || !queue_name[0]) return xsink->raiseException(err_name, "Queue name cannot be empty");

  const AbstractQoreNode* data = get_param(params, 2);
  qore_type_t dtype = data ? data->getType() : 0;
  if (dtype && !(dtype == NT_NOTHING || dtype == NT_STRING || dtype == NT_BINARY || dtype == NT_HASH))
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32)");

  // optional settings are either (1) integer flags or (2) hash with flags,  FML/FML32 selector and queue control parameters
  const QoreHashNode *enqueue_settings = 0;
  long flags = 0;
  long* pflags = 0;

  const AbstractQoreNode *n = get_param(params, 2);
  if (!is_nothing(n)) {
     const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
     if (h)
	enqueue_settings = h;
     else {
	flags = (long)n->getAsBigInt();
	pflags = &flags;
     }
  }
  adapter->setSendBuffer(data, enqueue_settings, err_name, xsink);
  if (xsink->isException()) {
     return xsink->raiseException(err_name, "Invalid parameter for enqueue()");
  }

  adapter->switchToSavedContext();
  return adapter->enqueue(queue_space, queue_name, enqueue_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* dequeue(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-DEQUEUE";
  const QoreStringNode* nstr = test_string_param(params, 0);
  if (!nstr) return xsink->raiseException(err_name, "First parameter needs to be queue space string");
  const char* queue_space = nstr->getBuffer();
  if (!queue_space || !queue_space[0]) {
    return xsink->raiseException(err_name, "Queue space parameter cannot be empty");
  }
  nstr = test_string_param(params, 1);
  if (!nstr) return xsink->raiseException(err_name, "Second parameter needs to be queue name string");
  const char* queue_name = nstr->getBuffer();
  if (!queue_name || !queue_name[0]) return xsink->raiseException(err_name, "Queue name cannot be empty");

  // optional settings are either (1) integer flags or (2) hash with flags and queue control parameters
  const QoreHashNode *dequeue_settings = 0;
  long flags = 0;
  long* pflags = 0;

  const AbstractQoreNode *n = get_param(params, 2);
  if (!is_nothing(n)) {
     const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
     if (h)
	dequeue_settings = h;
     else {
        flags = (long)n->getAsBigInt();
        pflags = &flags;
     }
  }

  adapter->switchToSavedContext();
  return adapter->dequeue(queue_space, queue_name, dequeue_settings, pflags, xsink);
}

#ifdef DEBUG
#ifdef TUXCONFIG_QUEUE_TEST
TEST()
{
  char* stop = getenv("TUXEDO_NO_QUEUE_TEST");
  if (stop && stop[0]) return;
  printf("testing enqueue() and dequeue() - queue test should run\n");

  // test modeled by qsample from ATMI samples, uses its server
  QoreString str;
  str.sprintf("%%requires tuxedo\n"
    "sub test() {\n"
    "$settings = (\"TUXCONFIG\" : \"%s\", \"TUXDIR\" : \"" TUXDIR "\");\n"
    "our $a = new Tuxedo::TuxedoAdapter($settings);\n"

    "$h = (\"queue_control_flags\" : Tuxedo::TPQREPLYQ, \"queue_control_replyqueue\" : \"RPLYQ\", \"flags\" : 0);\n"
    "$a.enqueue(\"QSPACE\", \"STRING\", \"sample data\", $h);\n"

    "$h = (\"queue_control_flags\" : Tuxedo::TPQWAIT, \"flags\" : Tuxedo::TPNOTIME);\n"
    "$res = $a.dequeue(\"QSPACE\", \"RPLYQ\", $h);\n"
    "if ($res != \"SAMPLE DATA\") { printf(\"dequeue failed\"); return 12; }\n"

    "return 10; }\n",
    tuxfile(TUXCONFIG_QUEUE_TEST)
  );

  ExceptionSink xsink;
  QoreProgram *pgm = new QoreProgram();
  pgm->parse(str.getBuffer(), "test", &xsink);
  if (xsink.isEvent()) {
    assert(false);
  } else {
     AbstractQoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->getType() != NT_INT) {
       assert(false);
     } else
	if (rv->getAsInt() != 10) {
       assert(false);
     }
     discard(rv, &xsink);
  }
  pgm->deref(&xsink);
  xsink.handleExceptions();
}
#endif
#endif

//-----------------------------------------------------------------------------
static AbstractQoreNode* writeToLog(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  
  const QoreStringNode* n = test_string_param(params, 0);
  if (!n) return xsink->raiseException("TUXEDO-ADAPTER-WRITE-TO-LOG", "One parameter expected - string to be written");
  const char* text = n->getBuffer();
  if (!text) text = (char*)"";
  userlog("%s", text);
  return 0;
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* openResourceManager(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpopen();
  if (res == -1) {    
    xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpopen"), "tpopen() failed with error %d", tperrno);
  }
  return 0;
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* closeResourceManager(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpclose();
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpclose"), "tpclose() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* beginTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
   //char* err = (char*)"One optional parameter: integer or date/time timeout in seconds expected";
  const AbstractQoreNode* n = get_param(params, 0);
  long timeout = 0; // no timeout by default

  if (n) {
     const DateTimeNode *date = dynamic_cast<const DateTimeNode *>(n);
     if (date) 
	timeout = (long)date->getRelativeSeconds();
     else
	timeout = (long)n->getAsBigInt();
  }

  adapter->switchToSavedContext();
  int res = tpbegin(timeout, 0);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpbegin"), "tpbegin() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* commitTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpcommit(0);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpcommit"), "tpcommit() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* abortTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpabort(0);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpabort"), "tpabort() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* lastErrorDetails(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  return new QoreBigIntNode(tperrordetail(0));
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* setPriority(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-SET-PRIORITY";
  const AbstractQoreNode* n = get_param(params, 0);
  if (is_nothing(n)) return xsink->raiseException(err_name, "Integer priority parameter expected");
  int priority = n->getAsInt();

  adapter->switchToSavedContext();
  int res = tpsprio(priority, TPABSOLUTE);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpsprio"), "tpsprio() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* getPriority(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpgprio();
  if (res != -1) return new QoreBigIntNode(res);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpgrio"), "tpgprio() failed with error %d", tperrno);

}

//-----------------------------------------------------------------------------
static AbstractQoreNode* suspendTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int id = ++adapter->m_last_suspended_transaction_id;
  TPTRANID tranid;
  int res = tpsuspend(&tranid, 0);
  if (res != -1) {
    adapter->m_suspended_transactions[id] = tranid;
    return new QoreBigIntNode(id);
  }
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpsuspend"), "tpsuspend() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* resumeTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-RESUME-TRANSACTION";
  const AbstractQoreNode* n = get_param(params, 0);
  if (is_nothing(n)) return xsink->raiseException(err_name, "One parameter, suspended transaction ID expected");
  int id = n->getAsInt();

  adapter->switchToSavedContext();
  map<int, TPTRANID>::iterator it = adapter->m_suspended_transactions.find(id);
  if (it == adapter->m_suspended_transactions.end()) {
    return xsink->raiseException(err_name, "Invalid transaction ID");
  }
  
  int res = tpresume(&it->second, 0);
  if (res != -1) {
    adapter->m_suspended_transactions.erase(it);
    return 0;
  }
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpresume"), "tpresume() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* finishCommitAfterDataLogged(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpscmt(TP_CMT_LOGGED);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpscmt"), "tpscmt() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* finishCommitAfterTwoPhaseCompletes(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpscmt(TP_CMT_COMPLETE);
  if (res != -1) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpscmt"), "tpscmt() failed with error %d", tperrno);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* isTransactionRunning(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpgetlev();
  if (res == -1) {
    return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpgetlev"), "tpgetlev() failed with error %d", tperrno);
  }
  return get_bool_node(res != 0);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* postEvent(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-POST-EVENT";
  const QoreStringNode* nstr = test_string_param(params, 0);
  if (!nstr) return xsink->raiseException(err_name, "First parameter needs to be event name string");
  const char* event_name = nstr->getBuffer();
  if (!event_name || !event_name[0]) {
    return xsink->raiseException(err_name, "Event name string cannot be empty");
  }
  const AbstractQoreNode* data = get_param(params, 1);
  qore_type_t dtype = data ? data->getType() : 0;
  if (dtype && !(dtype == NT_NOTHING || dtype == NT_STRING || dtype == NT_BINARY || dtype == NT_HASH))
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32)");

  // optional settings are either (1) integer flags or (2) hash with flags, FML or FML32 selector 
  const QoreHashNode *post_settings = 0;
  long flags = 0;
  long* pflags = 0;

  const AbstractQoreNode *n = get_param(params, 2);
  if (!is_nothing(n)) {
     const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(n);
     if (h)
	post_settings = h;
     else {
	flags = (long)n->getAsBigInt();
	pflags = &flags;
     }
  }
  adapter->setSendBuffer(data, post_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for postEvent()");
  }
  return adapter->post_event(event_name, post_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* beginTxTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_begin();
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_begin"), "tx_begin() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* commitTxTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_commit();
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_commit"), "tx_commit() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* abortTxTransaction(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_rollback();
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_rollback"), "tx_rollback() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* finishTxCommitAfterDataLogged(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_commit_return(TX_COMMIT_DECISION_LOGGED);
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_set_commit_return"), "tx_set_commit_return() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* finishTxCommitAfterTwoPhaseCompletes(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_commit_return(TX_COMMIT_COMPLETED);
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_set_commit_return"), "tx_set_commit_return() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* openTxResourceManager(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_open();
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_open"), "tx_open() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* closeTxResourceManager(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_close();
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_close"), "tx_close() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* setChainedTxTransactions(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_transaction_control(TX_CHAINED);
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_set_transaction_control"), "tx_set_transaction_control() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* setUnchainedTxTransactions(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_transaction_control(TX_UNCHAINED);
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_set_transaction_control"), "tx_set_transaction_control() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* setTxTransactionsTimeout(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
  const AbstractQoreNode* n = get_param(params, 0);
  long timeout = 0;
  if (n) {
     const DateTimeNode *date = dynamic_cast<const DateTimeNode *>(n);
     if (date) 
	timeout = (long)date->getRelativeSeconds();
     else
	timeout = (long)n->getAsBigInt();
  }

  adapter->switchToSavedContext();
  int res = tx_set_transaction_timeout(timeout);
  if (res == TX_OK) return 0;
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tx_set_transaction"), "tx_set_transaction() failed with error %d", res);
}

//-----------------------------------------------------------------------------
static AbstractQoreNode* error2string(QoreObject* self, QoreTuxedoAdapter* adapter, const QoreListNode *params, ExceptionSink* xsink)
{
   const AbstractQoreNode *n = get_param(params, 0);
   int err = n ? n->getAsInt() : 0;
   char* str = strerror(err);
   if (!str || !str[0]) str = (char*)"<uknown error>";
   return new QoreStringNode(str);
}

//-----------------------------------------------------------------------------
#ifdef DEBUG
QoreClass* initDummyTestClass()
{
  QoreClass* tst = new QoreClass("TuxedoTest", QDOM_NETWORK);
  CID_TUXEDOTEST = tst->getID();

  tst->setConstructor((q_constructor_t)TUXEDOTEST_constructor);
  tst->setDestructor((q_destructor_t)TUXEDOTEST_destructor);
  return tst;
}
#endif

//-----------------------------------------------------------------------------
class QoreClass* initTuxedoAdapterClass()
{
  tracein("initTuxedoAdapterClass");
  QoreClass* adapter = new QoreClass("TuxedoAdapter", QDOM_NETWORK);
  CID_TUXEDOADAPTER = adapter->getID();  

  adapter->setConstructor((q_constructor_t)TUXEDO_constructor);
  adapter->setDestructor((q_destructor_t)TUXEDO_destructor);
  adapter->setCopy((q_copy_t)TUXEDO_copy);

  // misc
  adapter->addMethod("setStringEncoding", (q_method_t)setStringEncoding);
  adapter->addMethod("error2string", (q_method_t)error2string);
  adapter->addMethod("writeToLog", (q_method_t)writeToLog);
  adapter->addMethod("lastErrorDetails", (q_method_t)lastErrorDetails);
  adapter->addMethod("setPriority",(q_method_t)setPriority);
  adapter->addMethod("getPriority", (q_method_t)getPriority);
  adapter->addMethod("postEvent", (q_method_t)postEvent);

  // request/response
  adapter->addMethod("call", (q_method_t)call);
  adapter->addMethod("asyncCall", (q_method_t)asyncCall);
  adapter->addMethod("cancelAsyncCall", (q_method_t)cancelAsyncCall);
  adapter->addMethod("waitForAsyncReply", (q_method_t)waitForAsyncReply);

  // conversational mode
  adapter->addMethod("joinConversation", (q_method_t)joinConversation);
  adapter->addMethod("breakConversation", (q_method_t)breakConversation);
  adapter->addMethod("sendConversationData", (q_method_t)sendConversationData);
  adapter->addMethod("receiveConversationData", (q_method_t)receiveConversationData);

  // queueing
  adapter->addMethod("enqueue", (q_method_t)enqueue);
  adapter->addMethod("dequeue", (q_method_t)dequeue);

  // resource manager
  adapter->addMethod("openResourceManager", (q_method_t)openResourceManager);
  adapter->addMethod("closeResourceManager", (q_method_t)closeResourceManager);
  adapter->addMethod("openTxResourceManager", (q_method_t)openTxResourceManager);
  adapter->addMethod("closeTxResourceManager", (q_method_t)closeTxResourceManager);

  // transactions
  adapter->addMethod("beginTransaction", (q_method_t)beginTransaction);
  adapter->addMethod("commitTransaction", (q_method_t)commitTransaction);
  adapter->addMethod("abortTransaction", (q_method_t)abortTransaction);
  adapter->addMethod("suspendTransaction", (q_method_t)suspendTransaction);
  adapter->addMethod("resumeTransaction", (q_method_t)resumeTransaction);
  adapter->addMethod("isTransactionRunning", (q_method_t)isTransactionRunning);
  adapter->addMethod("finishCommitAfterDataLogged", (q_method_t)finishCommitAfterDataLogged);
  adapter->addMethod("finishCommitAfterTwoPhaseCompletes", (q_method_t)finishCommitAfterTwoPhaseCompletes);

  // TX transactions
  adapter->addMethod("beginTxTransaction", (q_method_t)beginTxTransaction);
  adapter->addMethod("commitTxTransaction", (q_method_t)commitTxTransaction);
  adapter->addMethod("abortTxTransaction", (q_method_t)abortTxTransaction);
  adapter->addMethod("finishTxCommitAfterDataLogged", (q_method_t)finishTxCommitAfterDataLogged);
  adapter->addMethod("finishTxCommitAfterTwoPhaseCompletes", (q_method_t)finishTxCommitAfterTwoPhaseCompletes);
  adapter->addMethod("setChainedTxTransactions", (q_method_t)setChainedTxTransactions);
  adapter->addMethod("setUnchainedTxTransactions", (q_method_t)setUnchainedTxTransactions);
  adapter->addMethod("setTxTransactionTimeout", (q_method_t)setTxTransactionsTimeout);

  traceout("initTuxedoAdapterClass");
  return adapter;
}

// EOF

