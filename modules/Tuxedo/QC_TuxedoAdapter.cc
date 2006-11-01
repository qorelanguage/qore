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
  if (!size) return 0;
  adapter->m_binary_data.insert(adapter->m_binary_data.begin(), data, data + size);
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
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \""  TUXCONFIG_SIMPLE "\");\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR_SIMPLE "\");\n"
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
  return new QoreNode((int64)adapter->close());
}

#ifdef DEBUG
TEST()
{
  char* cmd =
    "qore -e '%requires tuxedo\n"
    "$a = new Tuxedo::TuxedoAdapter();\n"
    "$a.setEnvironmentVariable(\"TUXCONFIG\", \""  TUXCONFIG_SIMPLE "\");\n"
    "$a.setEnvironmentVariable(\"TUXDIR\", \"" TUXDIR_SIMPLE "\");\n"
    "$res = $a.init();\n"
    "if ($res != 0) { printf(\"init failed with %d\n\", $res); exit(11); }\n"
    "$res = $a.close();\n"
    "if ($res != 0) { printf(\"close failed with %d\n\", $res); exit(11); }\n"
    "exit(10);'\n";

  int res = system(cmd);
  res = WEXITSTATUS(res);
  assert(res == 10);
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


