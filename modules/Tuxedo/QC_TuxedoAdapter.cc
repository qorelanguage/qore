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
#include <qore/minitest.hpp>

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
// No parameters.
static void TUXEDO_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
  tracein("TUXEDO_constructor");

  QoreTuxedoAdapter* adapter = new QoreTuxedoAdapter();
  self->setPrivate(CID_TUXEDOADAPTER, adapter, getTuxedoAdapter, releaseTuxedoAdapter);

  traceout("TUXEDO_constructor");
}

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
static QoreNode* setUserName(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::setUserName", "String parameter expected.");
  char* s = n->val.String->getBuffer(); 
  adapter->m_username = s ? s : "";
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setUserName(\"aaa\");\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

  cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setUserName(\"\");\n" // empty 
    "exit(10);\n'";

  res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setClientName(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::setClientName", "String parameter expected.");
  char* s = n->val.String->getBuffer();
  adapter->m_clientname = s ? s : "";
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setClientName(\"aaa\");\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

  cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setClientName(\"\");\n" // empty
    "exit(10);\n'";

  res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setGroupName(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::setGroupName", "String parameter expected.");
  char* s = n->val.String->getBuffer();
  adapter->m_groupname = s ? s : "";
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setGroupName(\"aaa\");\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

  cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setGroupName(\"\");\n" // empty
    "exit(10);\n'";

  res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setPassword(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::setPassword", "String parameter expected.");
  char* s = n->val.String->getBuffer();
  adapter->m_password = s ? s : "";
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setPassword(\"aaa\");\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);

  cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setPassword(\"\");\n" // empty
    "exit(10);\n'";

  res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setBinaryConnectionData(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_BINARY, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::setBinaryConnectionData", "Binary parameter expected.");
  char* data = (char*)n->val.bin->getPtr();
  int size = n->val.bin->size();
  adapter->m_binary_data.clear();
  if (size) adapter->m_binary_data.insert(adapter->m_binary_data.begin(), data, data + size);
  return 0;
}

//-----------------------------------------------------------------------------
static QoreNode* setConnectionFlags(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::setConnectionFlags", "Integer parameter expected.");
  adapter->m_connection_flags = (long)n->val.intval;
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setConnectionFlags(123);\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setEnvironmentVariable(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = "TuxedoAdapter::setEnvironmentVariable";
  char* err_text = "Two string parameters needed: environment variable name and value.";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* name = n->val.String->getBuffer();
  if (!name || !name[0]) return xsink->raiseException(err_name, err_text);
  n = test_param(params, NT_STRING, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* value = n->val.String->getBuffer();
  if (value && !value[0]) value = 0;

  typedef QoreTuxedoAdapter::env_var_t env_t;
  for (env_t::iterator it = adapter->m_env_variables.begin(), end = adapter->m_env_variables.end(); it != end; ++it) {
    if (it->first == name) {
      if (value) {
        it->second = value; // replace previous env value
      } else {
        adapter->m_env_variables.erase(it);
      }
      return 0;
    }
  }
  if (value) adapter->m_env_variables.push_back(std::make_pair(name, value));
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"aaa\", \"a val\");\n"
    "$a.setEnvironmentVariable(\"bbb\", \"b val\");\n"
    "$a.setEnvironmentVariable(\"aaa\", \"x\");\n"
    "$a.setEnvironmentVariable(\"bbb\", \"\");\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* getNeededAuthentication(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  int auth;
  int err = adapter->getNeededAuthentication(auth);
  if (err) return xsink->raiseException("TuxedoAdapter::getNeededAuthentication", "tpchkauth() failed:  %s.", strerror(err));
  return new QoreNode((int64)auth);
}

#ifdef DEBUG
TEST()
{
#ifdef TUXCONFIG_SIMPLE
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;

  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \""  TUXCONFIG_SIMPLE "\");\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR "\");\n"
    "$res = $a.getNeededAuthentication();\n"
    "if ($res != Tuxedo::TPNOAUTH) { printf(\"getNeededAuthentication() failed\n\"); exit(1); }\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
#endif
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* init(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  return new QoreNode((int64)adapter->init());
}

//-----------------------------------------------------------------------------
static QoreNode* closeAdapter(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();
  return new QoreNode((int64)adapter->close());
}

#ifdef DEBUG
TEST()
{
#ifdef TUXCONFIG_SIMPLE
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;

  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \""  TUXCONFIG_SIMPLE "\");\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR "\");\n"
    "$res = $a.init();\n"
    "if ($res != 0) { printf(\"init failed with %d\n\", $res); exit(11); }\n"
    "$res = $a.close();\n"
    "if ($res != 0) { printf(\"close failed with %d\n\", $res); exit(11); }\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
#endif
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setStringDataToSend(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = "TuxedoAdapter::setStringDataToSend";
  char* err_text = "One to three string parameters expected: data, optional type, optional subtype.";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);

  QoreString encoded_string(n->val.String->getBuffer(), adapter->m_string_encoding);
  char* s = encoded_string.getBuffer();
  if (!s) s = "";

  char* type = "STRING";
  char* subtype = 0;
  n = test_param(params, NT_STRING, 1);
  if (n) {
    type = n->val.String->getBuffer();
    n = test_param(params, NT_STRING, 2);
    if (n) {
      subtype = n->val.String->getBuffer();
      if (subtype && !subtype[0]) subtype = 0;
    }
  }

  return new QoreNode((int64)adapter->setDataToSend(s, strlen(s) + 1, type, subtype));
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setStringEncoding(\"ISO-8859-5\");\n"  // Cyrilic
    "$res = $a.setStringDataToSend(\"aaaaa\");\n"
    "if ($res != 0) exit(11);\n"
    "$res = $a.setStringDataToSend(\"\");\n"
    "if ($res != 0) exit(11);\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setBinaryDataToSend(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = "TuxedoAdapter::setBinaryDataToSend";
  char* err_text = "One to three parameters expected: binary data, optional string type, optional string subtype.";
  QoreNode* n = test_param(params, NT_BINARY, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  void* data = n->val.bin->getPtr();
  int size = n->val.bin->size();
  if (size == 0) {
    adapter->resetDataToSend();
    return new QoreNode((int64)0);
  }
  char* type = "CARRAY";
  char* subtype = 0;
  n = test_param(params, NT_STRING, 1);
  if (n) {
    type = n->val.String->getBuffer();
    n = test_param(params, NT_STRING, 2);
    if (n) {
      subtype = n->val.String->getBuffer();
      if (subtype && !subtype[0]) subtype = 0;
    }
  }

  return new QoreNode((int64)adapter->setDataToSend(data, size, type, subtype));
}

#ifdef DEBUG
TEST()
{
  QoreTuxedoAdapter adapter;
  ExceptionSink xsink;
 
  // some binary data 
  List* l = new List;
  void* data = malloc(20);
  l->push(new QoreNode(new BinaryObject(data, 20)));
  QoreNode* params = new QoreNode(l);

  QoreNode* res = setBinaryDataToSend(0, &adapter, params, &xsink);
  assert(!xsink);
  assert(res);
  assert(res->type == NT_INT);
  assert(res->val.intval == 0);

  res->deref(&xsink);
  params->deref(&xsink);
  assert(!xsink);
}

TEST()
{
  QoreTuxedoAdapter adapter;
  ExceptionSink xsink;

  // empty binary data
  List* l = new List;
  l->push(new QoreNode(new BinaryObject));
  l->push(new QoreNode("STRING"));
  l->push(new QoreNode(""));
  QoreNode* params = new QoreNode(l);

  QoreNode* res = setBinaryDataToSend(0, &adapter, params, &xsink);
  assert(!xsink);
  assert(res);
  assert(res->type == NT_INT);
  assert(res->val.intval == 0);

  res->deref(&xsink);
  params->deref(&xsink);
  assert(!xsink);

}
#endif

//-----------------------------------------------------------------------------
static QoreNode* error2string(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::error2string", "One parameter expected: integer error code.");
  int err = (int)n->val.intval;
  char* str = strerror(err);
  if (!str || !str[0]) str = "<uknown error>";
  return new QoreNode(str);
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$text = $a.error2string(4);\n" 
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* allocateReceiveBuffer(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  char* err_name = "TuxedoAdapter::allocateReceiveBuffer";
  char* err_text = "Three parameters expected: string type, string subtype (could be empty), integer size.";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* type = n->val.String->getBuffer();
  if (!type) type = "";
  n = test_param(params, NT_STRING, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* subtype = n->val.String->getBuffer();
  if (subtype && !subtype[0]) subtype = 0;
  n = test_param(params, NT_INT, 2);
  if (!n) return xsink->raiseException(err_name, err_text);
  long size = (long)n->val.intval;

  return new QoreNode((int64)adapter->allocateReceiveBuffer(type, subtype, size));
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$res = $a.allocateReceiveBuffer(\"STRING\", \"\", 100);\n"
    "if ($res != 0) { printf(\"allocateReceiveBuffer failed %d\n\", $res); exit(11); }\n"
    "$res = $a.allocateReceiveBuffer(\"CARRAY\", \"\", 1000);\n"
    "if ($res != 0) { printf(\"allocateReceiveBuffer failed %d\n\", $res); exit(11); }\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* saveContext(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  return new QoreNode((int64)adapter->saveContext());
}

//-----------------------------------------------------------------------------
static QoreNode* switchToSavedContext(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  return new QoreNode((int64)adapter->switchToSavedContext());
}

#ifdef DEBUG
TEST()
{
#ifdef TUXCONFIG_SIMPLE
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;

char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \""  TUXCONFIG_SIMPLE "\");\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR "\");\n"
    "$res = $a.init();\n"
    "if ($res != 0) { printf(\"init failed with %d\n\", $res); exit(11); }\n"
    "$res = $a.saveContext();\n"
    "if ($res != 0) { printf(\"saveContext failed with %d\n\", $res); exit(11); }\n"
    "$res = $a.switchToSavedContext();\n"
    "if ($res != 0) { printf(\"switchToSavedContext failed with %d\n\", $res); exit(11); }\n"
    "$res = $a.close();\n"
    "if ($res != 0) { printf(\"close failed with %d\n\", $res); exit(11); }\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
#endif
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* resetDataToSend(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->resetDataToSend();
  return 0;
}

//-----------------------------------------------------------------------------
static QoreNode* resetReceiveBuffer(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->resetReceiveBuffer();
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setStringDataToSend(\"aaaaa\");\n"
    "$a.allocateReceiveBuffer(\"CARRAY\",\"\", 1000);\n"
    "$a.resetDataToSend();\n"
    "$a.resetReceiveBuffer();\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* setStringEncoding(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::setStringEncoding", "One parameter expected: string name of the encoding.");
  char* name = n->val.String->getBuffer();
  adapter->setStringEncoding(name);
  return 0;
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setStringEncoding(\"ISO-8859-5\");\n"  // Cyrilic
    "$res = $a.setStringDataToSend(\"aaaaa\");\n"
    "if ($res != 0) exit(11);\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* getReceivedString(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  if (!adapter->m_receive_buffer) {
    return xsink->raiseException("TuxedoAdapter::getReceivedString", "No data in received buffer.");
  }
  char* s = adapter->m_receive_buffer;
  if (!s[0]) return new QoreNode("");

  QoreString encoded_string(s, adapter->m_string_encoding);
  s = encoded_string.getBuffer();
  return new QoreNode(s); 
}

//-----------------------------------------------------------------------------
static QoreNode* getReceivedBinary(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  if (!adapter->m_receive_buffer || !adapter->m_receive_buffer_size) {
    return new QoreNode(new BinaryObject);
  }
  void* copy = malloc(adapter->m_receive_buffer_size);
  if (!copy) {
    xsink->outOfMemory();
    return 0;
  }
  return new QoreNode(new BinaryObject(copy, adapter->m_receive_buffer_size));
}

//-----------------------------------------------------------------------------
static QoreNode* getReceivedDataType(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  List* l = new List;
  if (!adapter->m_receive_buffer || !adapter->m_receive_buffer_size) {
    l->push(new QoreNode((int64)TPEINVAL));
  } else {
    char type[20];
    char subtype[20];
    if (tptypes(adapter->m_receive_buffer, type, subtype) == -1) {
      l->push(new QoreNode((int64)tperrno));
    } else {
      l->push(new QoreNode((int64)0));
      l->push(new QoreNode(type));
      l->push(new QoreNode(subtype));
    }
  }
  return new QoreNode(l);
}

//-----------------------------------------------------------------------------
static QoreNode* call(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  char* err_name = "TuxedoAdapter::call";
  char* err_text = "Two parameters expected: string service name, integer flags.";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* service = n->val.String->getBuffer();
  n = test_param(params, NT_INT, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  long flags = (long)n->val.intval;

  int res = tpcall(service, adapter->m_send_buffer, adapter->m_send_buffer_size,
    &adapter->m_receive_buffer, &adapter->m_receive_buffer_size, flags);
  return new QoreNode((int64)(res == -1 ? tperrno : 0));
}

#ifdef DEBUG
TEST()
{
#ifdef TUXCONFIG_SIMPLE
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;

  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \""  TUXCONFIG_SIMPLE "\");\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR "\");\n"
    "$res = $a.init();\n"
    "if ($res != 0) { printf(\"init failed with %d\n\", $res); exit(11); }\n"

    "$res = $a.setStringDataToSend(\"abcd\");\n"
    "if ($res != 0) { printf(\"setStringDataToSend failed %d\n\", $res); exit(11); }\n"
    "$res = $a.allocateReceiveBuffer(\"STRING\", \"\", 100);\n"
    "if ($res != 0) { printf(\"allocateReceiveBuffer failed %d\n\", $res); exit(11); }\n"

    "$res = $a.call(\"TOUPPER\", 0);\n"
    "if ($res != 0) { printf(\"call failed %d\n\", $res); exit(11); }\n"
    "$out = $a.getReceivedString();\n"
    "if ($out != \"ABCD\") { printf(\"service failed\n\"); exit(11); }\n"

    // second call
    "$res = $a.setStringDataToSend(\"abcdefgh\");\n"
    "if ($res != 0) { printf(\"setStringDataToSend failed %d\n\", $res); exit(11); }\n"

    "$res = $a.call(\"TOUPPER\", 0);\n"
    "if ($res != 0) { printf(\"call failed %d\n\", $res); exit(11); }\n"
    "$out = $a.getReceivedString();\n"
    "if ($out != \"ABCDEFGH\") { printf(\"service failed\n\"); exit(11); }\n"

    "$res = $a.close();\n"
    "if ($res != 0) { printf(\"close failed with %d\n\", $res); exit(11); }\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
#endif
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* asyncCall(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  char* err_name = "TuxedoAdapter::asyncCall";
  char* err_text = "Two parameters expected: string service name, integer flags.";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* service = n->val.String->getBuffer();
  n = test_param(params, NT_INT, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  long flags = (long)n->val.intval;

  int res = tpacall(service, adapter->m_send_buffer, adapter->m_send_buffer_size, flags);

  List* l = new List;
  if (res == -1) {
    l->push(new QoreNode((int64)tperrno));
  } else {
    l->push(new QoreNode((int64)0));
    l->push(new QoreNode((int64)res));
    if (res != 0) { // 0 == no reply to be expected
      adapter->m_pending_async_calls.push_back(res);
    }
  }
  return new QoreNode(l);
}

//-----------------------------------------------------------------------------
static QoreNode* cancelAsyncCall(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException("TuxedoAdapter::cancelAsyncCall", "One parameter, async call handle integer, expected.");
  int handle = (int)n->val.intval;
  adapter->remove_pending_async_call(handle);
  return new QoreNode((int64)(tpcancel(handle) == -1 ? tperrno : 0));
}

//------------------------------------------------------------------------------
static QoreNode* waitForAsyncReply(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  char* err_name = "TuxedoAdapter::waitForAsyncReply";
  char* err_text = "Two integer parameters expected: async call handle and flags.";
  QoreNode* n = test_param(params, NT_INT, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  int handle = (int)n->val.intval;
  n = test_param(params, NT_INT, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  long flags = (long)n->val.intval;

  adapter->remove_pending_async_call(handle);
  int res = tpgetrply(&handle, &adapter->m_receive_buffer, &adapter->m_receive_buffer_size, flags);
  return new QoreNode((int64)(res == -1 ? tperrno : 0));  
}

#ifdef DEBUG
TEST()
{
#ifdef TUXCONFIG_SIMPLE
  char* stop = getenv("TUXEDO_NO_SIMPLE_TEST");
  if (stop && stop[0]) return;

  // send 3 requests asynchronously, cancel one
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \""  TUXCONFIG_SIMPLE "\");\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR "\");\n"
    "$res = $a.init();\n"
    "if ($res != 0) { printf(\"init failed with %d\n\", $res); exit(11); }\n"

     // req 1
    "$res = $a.setStringDataToSend(\"abcd\");\n"
    "if ($res != 0) { printf(\"setStringDataToSend failed %d\n\", $res); exit(11); }\n"
    "$res = $a.asyncCall(\"TOUPPER\", 0);\n"
    "if ($res[0] != 0) { printf(\"call failed %d\n\", $res); exit(11); }\n"
    "$handle1 = $res[1];\n"

    // req 2
    "$res = $a.setStringDataToSend(\"xyz\");\n"
    "if ($res != 0) { printf(\"setStringDataToSend failed %d\n\", $res); exit(11); }\n"
    "$res = $a.asyncCall(\"TOUPPER\", 0);\n"
    "if ($res[0] != 0) { printf(\"call failed %d\n\", $res); exit(11); }\n"
    "$handle2 = $res[1];\n"

    // req 3
    "$res = $a.setStringDataToSend(\"mnop\");\n"
    "if ($res != 0) { printf(\"setStringDataToSend failed %d\n\", $res); exit(11); }\n"
    "$res = $a.asyncCall(\"TOUPPER\", 0);\n"
    "if ($res[0] != 0) { printf(\"call failed %d\n\", $res); exit(11); }\n"
    "$handle3 = $res[1];\n"

    // cancel req 2
    "$res = $a.cancelAsyncCall($handle2);\n"
    "if ($res != 0) { printf(\"cancelAsyncCall failed %d\n\", $res); exit(11); }\n"

    // read req 3 and 1
    "$a.allocateReceiveBuffer(\"STRING\", \"\", 100);\n"

    "$res = $a.waitForAsyncReply($handle3, Tuxedo::TPNOTIME);\n"
    "if ($res != 0) { printf(\"waitForAsyncReply(%d) 3 failed %d\n\", $handle3, $res); exit(11); }\n"
    "$out = $a.getReceivedString();\n"
    "if ($out != \"MNOP\") { printf(\"service failed\n\"); exit(11); }\n"

    "$res = $a.waitForAsyncReply($handle1, Tuxedo::TPNOTIME);\n"
    "if ($res != 0) { printf(\"waitForAsyncReply 1 failed %d\n\", $res); exit(11); }\n"
    "$out = $a.getReceivedString();\n"
    "if ($out != \"ABCD\") { printf(\"service failed\n\"); exit(11); }\n"

    "$res = $a.close();\n"
    "if ($res != 0) { printf(\"close failed with %d\n\", $res); exit(11); }\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
#endif
}
#endif

//-----------------------------------------------------------------------------
static QoreNode* connectConversation(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  char* err_name = "TuxedoAdapter::connectConversation";
  char* err_text = "Two parameters expected: string service name, integer flags.";
  QoreNode* n = test_param(params , NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* service_name = n->val.String->getBuffer();
  n = test_param(params, NT_INT, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  long flags = (long)n->val.intval;

  int res = tpconnect(service_name, adapter->m_send_buffer, adapter->m_send_buffer_size, flags);

  List* l = new List;
  if (res == -1) {
    l->push(new QoreNode((int64)tperrno));
  } else {
    l->push(new QoreNode((int64)0));
    l->push(new QoreNode((int64)res));
  } 
  return new QoreNode(l);
}

#ifdef DEBUG
// TBD
#endif

//-----------------------------------------------------------------------------
static QoreNode* enqueue(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  char* err_name = "TuxedoAdapter::enqueue";
  char* err_text = "Three to four parameter expected: string queue space, string queue name, integer flags, optional hash with control parameters.";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* queue_space = n->val.String->getBuffer();
  n = test_param(params, NT_STRING, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* queue_name = n->val.String->getBuffer();
  n = test_param(params, NT_INT, 2);
  if (!n) return xsink->raiseException(err_name, err_text);
  long flags = (long)n->val.intval;
  n = test_param(params, NT_HASH, 3);
  Hash* settings = 0;
  if (n) {
    settings = n->val.hash;
  }
  Hash* out_settings = 0;
  int res = adapter->enqueue(queue_space, queue_name, flags, settings, out_settings);

  List* l = new List;
  if (res == 0) {
    l->push(new QoreNode((int64)0));
    l->push(new QoreNode(out_settings));
  } else {
    l->push(new QoreNode((int64)res));
  }
  return new QoreNode(l);
}

//-----------------------------------------------------------------------------
static QoreNode* dequeue(Object* self, QoreTuxedoAdapter* adapter, QoreNode* params, ExceptionSink* xsink)
{
  adapter->switchToSavedContext();

  char* err_name = "TuxedoAdapter::dequeue";
  char* err_text = "Three to four parameter expected: string queue space, string queue name, integer flags, optional hash control parameters.";
  QoreNode* n = test_param(params, NT_STRING, 0);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* queue_space = n->val.String->getBuffer();
  n = test_param(params, NT_STRING, 1);
  if (!n) return xsink->raiseException(err_name, err_text);
  char* queue_name = n->val.String->getBuffer();
  n = test_param(params, NT_INT, 2);
  if (!n) return xsink->raiseException(err_name, err_text);
  long flags = (long)n->val.intval;
  n = test_param(params, NT_HASH, 3);
  Hash* settings = 0;
  if (n) {
    settings = n->val.hash;
  }
  Hash* out_settings = 0;
  int res = adapter->dequeue(queue_space, queue_name, flags, settings, out_settings);

  List* l = new List;
  if (res == 0) {
    l->push(new QoreNode((int64)0));
    l->push(new QoreNode(out_settings));
  } else {
    l->push(new QoreNode((int64)res));
  }
  return new QoreNode(l);
}

#ifdef DEBUG
TEST()
{
#ifdef TUXDIR_QUEUE
  char* stop = getenv("TUXEDO_NO_QUEUE_TEST");
  if (stop && stop[0]) return;

  // test modeled by qsample from ATMI samples, uses its server  
  char* cmd = "qore -e '%requires tuxedo\n"
    "$a = new TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR "\");\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \"" TUXCONFIG_QUEUE "\");\n"
    "$res = $a.init();\n"
    "if ($res != 0) { printf(\"init failed!!!!\n\"); exit(11); }\n"

    "$a.setStringDataToSend(\"test queue\");\n"
    "$h = (\"flags\" : Tuxedo::TPQREPLYQ, \"replyqueue\" : \"RPLYQ\");\n"

    "$res = $a.enqueue(\"QSPACE\", \"STRING\", 0, $h);\n"
    "if ($res[0] != 0) { printf(\"enqueue failed\n\"); exit(11); }\n"

    "$a.allocateReceiveBuffer(\"STRING\", \"\", 100);\n"
    "$h = (\"flags\" : Tuxedo::TPQWAIT);\n"

    "$res = $a.dequeue(\"QSPACE\", \"RPLYQ\", Tuxedo::TPNOTIME, $h);\n"
    "if ($res[0] != 0) { printf(\"dequeue failed\n\"); exit(11); }\n"

    "$res = $a.getReceivedString();\n"
    "if ($res != \"TEST QUEUE\") { printf(\"service failed\n\"); exit(11); }\n"

    "$a.close();\n"
    "exit(10);'\n";


  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
#endif
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

  // init/close
  adapter->addMethod("setUserName", (q_method_t)setUserName);
  adapter->addMethod("setClientName", (q_method_t)setClientName);
  adapter->addMethod("setGroupName", (q_method_t)setGroupName);
  adapter->addMethod("setPassword", (q_method_t)setPassword);
  adapter->addMethod("setBinaryConnectionData", (q_method_t)setBinaryConnectionData);
  adapter->addMethod("setConnectionFlags", (q_method_t)setConnectionFlags);
  adapter->addMethod("setEnvironmentVariable", (q_method_t)setEnvironmentVariable);
  adapter->addMethod("getNeededAuthentication", (q_method_t)getNeededAuthentication);
  adapter->addMethod("init", (q_method_t)init);
  adapter->addMethod("close", (q_method_t)closeAdapter);

  // set/get data
  adapter->addMethod("setStringDataToSend", (q_method_t)setStringDataToSend);
  adapter->addMethod("setBinaryDataToSend", (q_method_t)setBinaryDataToSend);  
  adapter->addMethod("resetDataToSend", (q_method_t)resetDataToSend);
  adapter->addMethod("allocateReceiveBuffer", (q_method_t)allocateReceiveBuffer);
  adapter->addMethod("resetReceiveBuffer", (q_method_t)resetReceiveBuffer);
  adapter->addMethod("setStringEncoding", (q_method_t)setStringEncoding);
  adapter->addMethod("getReceivedString", (q_method_t)getReceivedString);
  adapter->addMethod("getReceivedBinary", (q_method_t)getReceivedBinary);
  adapter->addMethod("getReceivedDataType", (q_method_t)getReceivedDataType);

  // misc
  adapter->addMethod("error2string", (q_method_t)error2string);
  adapter->addMethod("saveContext", (q_method_t)saveContext);
  adapter->addMethod("switchToSavedContext", (q_method_t)switchToSavedContext);

  // request/response
  adapter->addMethod("call", (q_method_t)call);
  adapter->addMethod("asyncCall", (q_method_t)asyncCall);
  adapter->addMethod("cancelAsyncCall", (q_method_t)cancelAsyncCall);
  adapter->addMethod("waitForAsyncReply", (q_method_t)waitForAsyncReply);

  // conversational mode
  adapter->addMethod("connectConversation", (q_method_t)connectConversation);

  // queueing
  adapter->addMethod("enqueue", (q_method_t)enqueue);
  adapter->addMethod("dequeue", (q_method_t)dequeue);

  traceout("initTuxedoAdapterClass");
  return adapter;
}

//----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // just test that it could be instantiated
  QoreTuxedoAdapter ad1;
  QoreTuxedoAdapter ad2;
}

TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "exit(10);\n'";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
}
#endif

// EOF


