/*
  sybase_low_level_interface.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007

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

#include <qore/config.h>
#include <qore/support.h>

#include <ctpublic.h>
#include <assert.h>
#include <pthread.h>
#include <qore/minitest.hpp>
#include <qore/ScopeGuard.h>

#include "sybase_low_level_interface.h"
#include "sybase_connection.h"

//------------------------------------------------------------------------------
int sybase_low_level_commit(sybase_connection* sc, ExceptionSink* xsink)
{
  CS_COMMAND* cmd = 0;
  CS_RETCODE err = ct_cmd_alloc(sc->getConnection(), &cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return 0;
  }
  ON_BLOCK_EXIT(ct_cmd_drop, cmd);
  ScopeGuard canceller = MakeGuard(ct_cancel, (CS_CONNECTION*)0, cmd, CS_CANCEL_ALL);

  char* text = "commit tran";
  err = ct_command(cmd, CS_LANG_CMD, text, strlen(text), CS_END);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_command(\"%s\") failed with error %d", text, (int)err);
    return 0;
  }
  err = ct_send(cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return 0;
  }

  // no results expected
  CS_INT result_type;
  err = ct_results(cmd, &result_type);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_result() failed with error %d", (int)err);
    return 0;
  }
  if (result_type != CS_CMD_SUCCEED) {
    assert(result_type == CS_CMD_FAIL);
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() for \"%s\" failed with error %d", text, (int)err);
    return 0;
  }
  while((err = ct_results(cmd, &result_type)) == CS_SUCCEED);

  canceller.Dismiss();
  return 1;
}

//------------------------------------------------------------------------------
int sybase_low_level_rollback(sybase_connection* sc, ExceptionSink* xsink)
{
  CS_COMMAND* cmd = 0;
  CS_RETCODE err = ct_cmd_alloc(sc->getConnection(), &cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return 0;
  }
  ON_BLOCK_EXIT(ct_cmd_drop, cmd);
  ScopeGuard canceller = MakeGuard(ct_cancel, (CS_CONNECTION*)0, cmd, CS_CANCEL_ALL);

  char* text = "rollback tran";
  err = ct_command(cmd, CS_LANG_CMD, text, strlen(text), CS_END);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_command(\"%s\") failed with error %d", text, (int)err);
    return 0;
  }
  err = ct_send(cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return 0;
  }

  // no results expected
  CS_INT result_type;
  err = ct_results(cmd, &result_type);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_result() failed with error %d", (int)err);
    return 0;
  }
  if (result_type != CS_CMD_SUCCEED) {
    assert(result_type == CS_CMD_FAIL);
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() for \"%s\" failed with error %d", text, (int)err);
    return 0;
  }
  while((err = ct_results(cmd, &result_type)) == CS_SUCCEED);

  canceller.Dismiss();
  return 1;
}

//------------------------------------------------------------------------------
void sybase_low_level_execute_directly_command(CS_CONNECTION* conn, const char* sql_text, ExceptionSink* xsink)
{
  CS_COMMAND* cmd = 0;
  CS_RETCODE err = ct_cmd_alloc(conn, &cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return;
  }
  ON_BLOCK_EXIT(ct_cmd_drop, cmd);
  ScopeGuard canceller = MakeGuard(ct_cancel, (CS_CONNECTION*)0, cmd, CS_CANCEL_ALL);

  err = ct_command(cmd, CS_LANG_CMD, (CS_CHAR*)sql_text, strlen(sql_text), CS_END);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_command(\"%s\") failed with error %d", (int)err, sql_text);
    return;
  }
  err = ct_send(cmd);
  if (err != CS_SUCCEED) {
    // assert(false); - goes this way during tests
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return;
  }

  // no results expected
  CS_INT result_type;
  err = ct_results(cmd, &result_type);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_result() failed with error %d", (int)err);
    return;
  }

  if (result_type != CS_CMD_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() for \"%s\" failed with error %d", sql_text, (int)err);
    return;
  }
  while((err = ct_results(cmd, &result_type)) == CS_SUCCEED);
  canceller.Dismiss();
}

//------------------------------------------------------------------------------
sybase_command_wrapper::sybase_command_wrapper(CS_CONNECTION* conn, ExceptionSink* xsink)
: m_cmd(0)
{
  CS_RETCODE err = ct_cmd_alloc(conn, &m_cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return;
  }

  // a unique (within the connection) string identifier needs to be generated
  static unsigned counter = 0;
  ++counter;
  char aux[30];
  sprintf(aux, "my_cmd_%u_%u", (unsigned)pthread_self(), counter);
  m_string_id = aux;
}

//------------------------------------------------------------------------------
sybase_command_wrapper::~sybase_command_wrapper()
{
  if (!m_cmd) return;
  ct_cancel(0, m_cmd, CS_CANCEL_ALL);
  ct_cmd_drop(m_cmd);
}

//------------------------------------------------------------------------------
void sybase_low_level_prepare_command(const sybase_command_wrapper& wrapper, const char* sql_text, ExceptionSink* xsink)
{
  assert(sql_text && sql_text[0]);
  
  CS_RETCODE err = ct_dynamic(wrapper(), CS_PREPARE, wrapper.getStringId(), CS_NULLTERM, (CS_CHAR*)sql_text, CS_NULLTERM);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_dynamic(CS_PREPARE, \"%s\") failed with error %d", sql_text, (int)err);
    return;
  }

  err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() for \"%s\" failed with error %d", sql_text, (int)err);
    return;
  }

  // no results expected
  CS_INT result_type;
  err = ct_results(wrapper(), &result_type);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_result() failed with error %d", (int)err);
    return;
  }
  if (result_type != CS_CMD_SUCCEED) {
    assert(result_type == CS_CMD_FAIL);
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_results() ct_dynamic(CS_PREPARE) failed with error %d", (int)err);
    return;
  }
  while((err = ct_results(wrapper(), &result_type)) == CS_SUCCEED);
}


#ifdef DEBUG
#  include "tests/sybase_low_level_interface_tests.cc"
#endif

// EOF

