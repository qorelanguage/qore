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
static void TUXEDOTEST_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  QoreTuxedoTest* tst = new QoreTuxedoTest;
  self->setPrivate(CID_TUXEDOTEST, tst);
}
static void TUXEDOTEST_destructor(Object *self, QoreTuxedoTest* test, ExceptionSink *xsink)
{
  test->deref();
}
#endif

//------------------------------------------------------------------------------
static void TUXEDO_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-CONSTRUCTOR";
  tracein(err_name);
  
  QoreNode* n = test_param(params, NT_HASH, 0);
  if (!n) {
    xsink->raiseException(err_name, "Hash with Tuxedo settings expected.");
    return;
  }
  Hash* h = n->val.hash;

  if (get_param(params, 1)) {
    xsink->raiseException(err_name, "Single parameter is expected.");
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
     QoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->type != NT_INT) {
       assert(false);
     } else
     if (rv->val.intval != 10) {
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
     QoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();     
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->type != NT_INT) {
       assert(false);
     } else
     if (rv->val.intval != 10) {
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
static void TUXEDO_destructor(Object *self, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  tracein("TUXEDO_destructor");
  adapter->deref();
  traceout("TUXEDO_destructor");
}

//------------------------------------------------------------------------------
static void TUXEDO_copy(Object *self, Object *old, QoreTuxedoAdapter* adapter, ExceptionSink *xsink)
{
  xsink->raiseException("TUXEDO-ADAPTER-COPY", "copying Tuxedo::TuxedoAdapter objects is not yet supported.");
}

//-----------------------------------------------------------------------------
static QoreNode* call(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-CALL";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be service string.");
  char* service_name = n->val.String->getBuffer();
  if (!service_name || !service_name[0]) {
    return xsink->raiseException(err_name, "Service name string cannot be empty.");
  }
  QoreNode* data = get_param(params, 1);
  if (!data) {
    return xsink->raiseException(err_name, "Data expected as the second parameter.");    
  }
  if (!(data->type == NT_NOTHING || data->type == NT_STRING || data->type == NT_BINARY || data->type == NT_HASH)) {
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32).");
  }

  // optional settings are either (1) integer flags or (2) hash with flags, suggested out data type, FML/FML32 selector
  Hash* call_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 2)) {
    n = test_param(params, NT_HASH, 2);
    if (n) {
      call_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 2);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The third (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 3)) {
      return xsink->raiseException(err_name, "Up to three parameter are expected.");
    }
  } 
  adapter->setSendBuffer(data, call_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for call().");
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
     QoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->type != NT_INT) {
       assert(false);
     } else
     if (rv->val.intval != 10) {
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
static QoreNode* setStringEncoding(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException("TUXEDO-ADAPTER-SET-STRING-ENCODING", "One parameter expected: string name of the encoding.");
  char* name = n->val.String->getBuffer();
  adapter->setStringEncoding(name);
  return 0;
}

//-----------------------------------------------------------------------------
static QoreNode* asyncCall(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-ASYNC-CALL";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be service string.");
  char* service_name = n->val.String->getBuffer();
  if (!service_name || !service_name[0]) {
    return xsink->raiseException(err_name, "Service name string cannot be empty.");
  }
  QoreNode* data = get_param(params, 1);
  if (!data) {
    return xsink->raiseException(err_name, "Data expected as the second parameter.");
  }
  if (!(data->type == NT_NOTHING || data->type == NT_STRING || data->type == NT_BINARY || data->type == NT_HASH)) {
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32).");
  }

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  Hash* acall_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 2)) {
    n = test_param(params, NT_HASH, 2);
    if (n) {
      acall_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 2);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The third (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 3)) {
      return xsink->raiseException(err_name, "Up to three parameter are expected.");
    }
  }
  adapter->setSendBuffer(data, acall_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for asyncCall().");
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
     QoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->type != NT_INT) {
       assert(false);
     } else
     if (rv->val.intval != 10) {
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
     QoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->type != NT_INT) {
       assert(false);
     } else
     if (rv->val.intval != 10) {
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
static QoreNode* cancelAsyncCall(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-CANCEL-ASYNC_CALL";
  adapter->switchToSavedContext();

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "One parameter, async call handle integer, expected.");
  int handle = (int)n->val.intval;
  adapter->remove_pending_async_call(handle);
  int res = tpcancel(handle);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpcancel"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpcancel() failed with error %d.", tperrno);
}

//------------------------------------------------------------------------------
static QoreNode* waitForAsyncReply(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  char* err_name = (char*)"TUXEDO-ADAPTER-WAIT-FOR-ASYNC-REPLY";
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "The first parameter, the handle, needs to be an integer.");
  int handle = (int)n->val.intval;

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  Hash* getrply_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 1)) {
    n = test_param(params, NT_HASH, 1);
    if (n) {
      getrply_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 1);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The second (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 2)) {
      return xsink->raiseException(err_name, "Up to two parameter are expected.");
    }
  }

  adapter->remove_pending_async_call(handle); // remove it even if the tpgetrply() fails (I do not know better solution)
  return adapter->get_reply(handle, getrply_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static QoreNode* joinConversation(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-JOIN-CONVERSATION";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be service name string.");
  char* service_name = n->val.String->getBuffer();
  if (!service_name || !service_name[0]) {
    return xsink->raiseException(err_name, "Service name string cannot be empty.");
  }
  QoreNode* data = get_param(params, 1);
  if (!data) {
    return xsink->raiseException(err_name, "Data expected as the second parameter.");
  }
  if (!(data->type == NT_NOTHING || data->type == NT_STRING || data->type == NT_BINARY || data->type == NT_HASH)) {
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32).");
  }

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  Hash* connect_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 2)) {
    n = test_param(params, NT_HASH, 2);
    if (n) {
      connect_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 2);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The third (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 3)) {
      return xsink->raiseException(err_name, "Up to three parameter are expected.");
    }
  }
  adapter->setSendBuffer(data, connect_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for joinConversation().");
  }
  adapter->switchToSavedContext();
  return adapter->connect(service_name, connect_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static QoreNode* breakConversation(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-BREAK-CONVERSATION";
  adapter->switchToSavedContext();

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "One parameter expected: conversation handle integer.");
  int handle = (int)n->val.intval;
  int res = tpdiscon(handle);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpdiscon"), xsink);
  return xsink->raiseExceptionArg(err_name, new QoreNode(h), "tpdiscon() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* sendConversationData(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-SEND-CONVERSATION-DATA";
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be integer handle to the conversation.");
  long handle = (long)n->val.intval;

  QoreNode* data = get_param(params, 1);
  if (!data) {
    return xsink->raiseException(err_name, "Data expected as the second parameter.");
  }
  if (!(data->type == NT_NOTHING || data->type == NT_STRING || data->type == NT_BINARY || data->type == NT_HASH)) {
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32).");
  }

  // optional settings are either (1) integer flags or (2) hash with flags and FML/FML32 selector
  Hash* send_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 2)) {
    n = test_param(params, NT_HASH, 2);
    if (n) {
      send_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 2);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The third (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 3)) {
      return xsink->raiseException(err_name, "Up to three parameter are expected.");
    }
  }
  adapter->setSendBuffer(data, send_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for sendConversationData().");
  }

  adapter->switchToSavedContext();
  return adapter->send(handle, send_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static QoreNode* receiveConversationData(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-RECEIVE-CONVERSATION-DATA";
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be integer handle to the conversation.");
  long handle = (long)n->val.intval;

  // optional settings are either (1) integer flags or (2) hash with flags and out data type selector
  Hash* receive_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 1)) {
    n = test_param(params, NT_HASH, 1);
    if (n) {
      receive_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 1);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The second (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 2)) {
      return xsink->raiseException(err_name, "Up to two parameter are expected.");
    }
  }

  adapter->switchToSavedContext();
  return adapter->receive(handle, receive_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static QoreNode* enqueue(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-ENQUEUE";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be queue space string.");
  char* queue_space = n->val.String->getBuffer();
  if (!queue_space || !queue_space[0]) {
    return xsink->raiseException(err_name, "Queue space parameter cannot be empty.");
  }
  n = test_param(params, NT_STRING, 1);
  if (!n) return xsink->raiseException(err_name, "Second parameter needs to be queue name string.");
  char* queue_name = n->val.String->getBuffer();
  if (!queue_name || !queue_name[0]) return xsink->raiseException(err_name, "Queue name cannot be empty.");

  QoreNode* data = get_param(params, 2);
  if (!data) {
    return xsink->raiseException(err_name, "Data expected as the third parameter.");
  }
  if (!(data->type == NT_NOTHING || data->type == NT_STRING || data->type == NT_BINARY || data->type == NT_HASH)) {
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32).");
  }

  // optional settings are either (1) integer flags or (2) hash with flags,  FML/FML32 selector and queue control parameters
  Hash* enqueue_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 2)) {
    n = test_param(params, NT_HASH, 3);
    if (n) {
      enqueue_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 3);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The fourth (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 4)) {
      return xsink->raiseException(err_name, "Up to four parameter are expected.");
    }
  }
  adapter->setSendBuffer(data, enqueue_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for enqueue().");
  }

  adapter->switchToSavedContext();
  return adapter->enqueue(queue_space, queue_name, enqueue_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static QoreNode* dequeue(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-DEQUEUE";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be queue space string.");
  char* queue_space = n->val.String->getBuffer();
  if (!queue_space || !queue_space[0]) {
    return xsink->raiseException(err_name, "Queue space parameter cannot be empty.");
  }
  n = test_param(params, NT_STRING, 1);
  if (!n) return xsink->raiseException(err_name, "Second parameter needs to be queue name string.");
  char* queue_name = n->val.String->getBuffer();
  if (!queue_name || !queue_name[0]) return xsink->raiseException(err_name, "Queue name cannot be empty.");

  // optional settings are either (1) integer flags or (2) hash with flags and queue control parameters
  Hash* dequeue_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 2)) {
    n = test_param(params, NT_HASH, 2);
    if (n) {
      dequeue_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 2);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The third (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 3)) {
      return xsink->raiseException(err_name, "Up to three parameter are expected.");
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
     QoreNode *rv = pgm->callFunction("test", NULL, &xsink);
     if (xsink.isEvent()) {
       xsink.handleExceptions();
       assert(false);
     }
     if (!rv) {
       assert(false);
     } else
     if (rv->type != NT_INT) {
       assert(false);
     } else
     if (rv->val.intval != 10) {
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
static QoreNode* writeToLog(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException("TUXEDO-ADAPTER-WRITE-TO-LOG", "One parameter expected - string to be written.");
  char* text = n->val.String->getBuffer();
  if (!text) text = (char*)"";
  userlog("%s", text);
  return 0;
}

//-----------------------------------------------------------------------------
static QoreNode* openResourceManager(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpopen();
  if (res == -1) {    
    Hash* h = new Hash;
    h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
    h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpopen"), xsink);
    xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpopen() failed with error %d.", tperrno);
  }
  return 0;
}

//-----------------------------------------------------------------------------
static QoreNode* closeResourceManager(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpclose();
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpclose"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpclose() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* beginTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException("TUXEDO-ADAPTER-BEGIN-TRANSACTION", "One parameter: timeout integer, exception.");
  long timeout = (long)n->val.intval;

  adapter->switchToSavedContext();
  int res = tpbegin(timeout, 0);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpbegin"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpbegin() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* commitTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpcommit(0);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpcommit"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpcommit() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* abortTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpabort(0);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpabort"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpabort() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* lastErrorDetails(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  return new QoreNode((int64)tperrordetail(0));
}

//-----------------------------------------------------------------------------
static QoreNode* setPriority(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-SET-PRIORITY";
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "Integer priority parameter expected.");
  int priority = (int)n->val.intval;

  adapter->switchToSavedContext();
  int res = tpsprio(priority, TPABSOLUTE);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpsprio"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpsprio() failed with error %d.", tperrno);

}

//-----------------------------------------------------------------------------
static QoreNode* getPriority(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpgprio();
  if (res != -1) return new QoreNode((int64)res);
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpgprio"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpgprio() failed with error %d.", tperrno);

}

//-----------------------------------------------------------------------------
static QoreNode* suspendTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int id = ++adapter->m_last_suspended_transaction_id;
  TPTRANID tranid;
  int res = tpsuspend(&tranid, 0);
  if (res != -1) {
    adapter->m_suspended_transactions[id] = tranid;
    return new QoreNode((int64)id);
  }
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpsuspend"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpsuspend() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* resumeTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-RESUME-TRANSACTION";
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "One parameter, suspended transaction ID expected.");
  int id = (int)n->val.intval;

  adapter->switchToSavedContext();
  map<int, TPTRANID>::iterator it = adapter->m_suspended_transactions.find(id);
  if (it == adapter->m_suspended_transactions.end()) {
    return xsink->raiseException(err_name, "Invalid transcation ID.");
  }
  
  int res = tpresume(&it->second, 0);
  if (res != -1) {
    adapter->m_suspended_transactions.erase(it);
    return 0;
  }
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpresume"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpresume() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* finishCommitAfterDataLogged(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpscmt(TP_CMT_LOGGED);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpscmt"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpscmt() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* finishCommitAfterTwoPhaseCompletes(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpscmt(TP_CMT_COMPLETE);
  if (res != -1) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpscmt"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpscmt() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
static QoreNode* isTransactionRunning(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tpgetlev();
  if (res == -1) {
    Hash* h = new Hash;
    h->setKeyValue((char*)"error", new QoreNode((int64)tperrno), xsink);
    h->setKeyValue((char*)"Tuxedo call", new QoreNode("tpgetlev"), xsink);
    return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tpgetlev() failed with error %d.", tperrno);
  }
  return new QoreNode(res != 0);
}

//-----------------------------------------------------------------------------
static QoreNode* postEvent(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-POST-EVENT";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, "First parameter needs to be event name string.");
  char* event_name = n->val.String->getBuffer();
  if (!event_name || !event_name[0]) {
    return xsink->raiseException(err_name, "Event name string cannot be empty.");
  }
  QoreNode* data = get_param(params, 1);
  if (!data) {
    return xsink->raiseException(err_name, "Data expected as the second parameter.");
  }
  if (!(data->type == NT_NOTHING || data->type == NT_STRING || data->type == NT_BINARY || data->type == NT_HASH)) {
    return xsink->raiseException(err_name, "Allowed data: NOTHING, string, binary, hash (FML or FML32).");
  }

  // optional settings are either (1) integer flags or (2) hash with flags, FML or FML32 selector 
  Hash* post_settings = 0;
  long flags = 0;
  long* pflags = 0;

  if (get_param(params, 2)) {
    n = test_param(params, NT_HASH, 2);
    if (n) {
      post_settings = n->val.hash;
    } else {
      n = test_param(params, NT_INT, 2);
      if (n) {
        flags = (long)n->val.intval;
        pflags = &flags;
      } else {
       return xsink->raiseException(err_name, "The third (optional) parameter needs to be hash with settings or integer flags.");
      }
    }
    if (get_param(params, 3)) {
      return xsink->raiseException(err_name, "Up to three parameter are expected.");
    }
  }
  adapter->setSendBuffer(data, post_settings, err_name, xsink);
  if (xsink->isException()) {
    return xsink->raiseException(err_name, "Invalid parameter for postEvent().");
  }
  return adapter->post_event(event_name, post_settings, pflags, xsink);
}

//-----------------------------------------------------------------------------
static QoreNode* beginTxTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_begin();
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_begin"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_begin() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* commitTxTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_commit();
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_commit"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_commit() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* abortTxTransaction(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_rollback();
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_rollback"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_rollback() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* finishTxCommitAfterDataLogged(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_commit_return(TX_COMMIT_DECISION_LOGGED);
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_set_commit_return"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_set_commit_return() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* finishTxCommitAfterTwoPhaseCompletes(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_commit_return(TX_COMMIT_COMPLETED);
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_set_commit_return"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_set_commit_return() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* openTxResourceManager(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_open();
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_open"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_open() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* closeTxResourceManager(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_close();
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_close"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_close() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* setChainedTxTransactions(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_transaction_control(TX_CHAINED);
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_set_transaction_control"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_set_transaction_control() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* setUnchainedTxTransactions(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  int res = tx_set_transaction_control(TX_UNCHAINED);
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_set_transaction_control"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_set_transaction_control() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* setTxTransactionsTimeout(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = (char*)"TUXEDO-ADAPTER-SET-TX-TRANSACTION-TIMEOUT";
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, "Integer timeout in seconds expected.");
  long timeout = (long)n->val.intval;

  adapter->switchToSavedContext();
  int res = tx_set_transaction_timeout(timeout);
  if (res == TX_OK) return 0;
  Hash* h = new Hash;
  h->setKeyValue((char*)"error", new QoreNode((int64)res), xsink);
  h->setKeyValue((char*)"Tuxedo call", new QoreNode("tx_set_transaction"), xsink);
  return xsink->raiseExceptionArg("TUXEDO-ERROR", new QoreNode(h), "tx_set_transaction() failed with error %d.", res);
}

//-----------------------------------------------------------------------------
static QoreNode* error2string(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException("TUXEDO-ADAPTER-ERROR2STRING", "One parameter expected: integer error code.");
  int err = (int)n->val.intval;
  char* str = strerror(err);
  if (!str || !str[0]) str = (char*)"<uknown error>";
  return new QoreNode(str);
}

//-----------------------------------------------------------------------------
#ifdef DEBUG
QoreClass* initDummyTestClass()
{
  QoreClass* tst = new QoreClass(QDOM_NETWORK, strdup("TuxedoTest"));
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
  QoreClass* adapter = new QoreClass(QDOM_NETWORK, strdup("TuxedoAdapter"));
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
  adapter->addMethod("setTxTransactionsTimeout", (q_method_t)setTxTransactionsTimeout);

  traceout("initTuxedoAdapterClass");
  return adapter;
}

// EOF

