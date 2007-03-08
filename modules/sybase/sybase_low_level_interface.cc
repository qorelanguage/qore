/*
  sybase_low_level_interface.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007 Qore Technologies

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
#include <qore/List.h>
#include <qore/Exception.h>

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

//------------------------------------------------------------------------------
std::vector<parameter_info_t> sybase_low_level_get_input_parameters_info(
  const sybase_command_wrapper& wrapper, ExceptionSink* xsink)
{
  typedef std::vector<parameter_info_t> result_t;
  result_t result;

  CS_RETCODE err = ct_dynamic(wrapper(), CS_DESCRIBE_INPUT, wrapper.getStringId(), CS_NULLTERM, 0, CS_UNUSED);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_dynamic(CS_DESCRIBE_INPUT) failed with error %d", (int)err);
    return result;
  }

  err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return result;
  }

  CS_INT result_type;
  while ((err = ct_results(wrapper(), &result_type)) == CS_SUCCEED) {
    if (result_type != CS_DESCRIBE_RESULT) {
      continue;
    }

    CS_INT numparam = 0;
    CS_INT len;
    err = ct_res_info(wrapper(), CS_NUMDATA, &numparam, CS_UNUSED, &len); // get # of input params
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_res_info(CS_DESCRIBE_RESULT) failed with error %d", (int)err);
      return result;
    }
    result.reserve(numparam);

    CS_DATAFMT datafmt;
    for (CS_INT i = 1; i <= numparam; ++i) {
      err = ct_describe(wrapper(), i, &datafmt);
      if (err != CS_SUCCEED) {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_describe(%d) failed with error %d", i, (int)err);
        result.clear();
        return result;
      }
      std::string name;
      if (datafmt.name[0]) {
        name = datafmt.name;
      } else {
        char aux[60];
        sprintf(aux, "unnamed parameter #%d", (int)i);
        name = aux;
      }
      result.push_back(parameter_info_t(name, datafmt.datatype, datafmt.maxlength));
    }
  }

  return result;
}

//------------------------------------------------------------------------------
std::vector<parameter_info_t> sybase_low_level_get_output_data_info(
  const sybase_command_wrapper& wrapper, ExceptionSink* xsink)
{
  typedef std::vector<parameter_info_t> result_t;
  result_t result;

  CS_RETCODE err = ct_dynamic(wrapper(), CS_DESCRIBE_OUTPUT, wrapper.getStringId(), CS_NULLTERM, 0, CS_UNUSED);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_dynamic(CS_DESCRIBE_OUTPUT) failed with error %d", (int)err);
    return result;
  }

  err = ct_send(wrapper());
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_send() failed with error %d", (int)err);
    return result;
  }

  CS_INT result_type;
  while ((err = ct_results(wrapper(), &result_type)) == CS_SUCCEED) {
    if (result_type != CS_DESCRIBE_RESULT) {
      continue;
    }

    CS_INT numparam = 0;
    CS_INT len;
    err = ct_res_info(wrapper(), CS_NUMDATA, &numparam, CS_UNUSED, &len); // get # of out columns
    if (err != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_res_info(CS_DESCRIBE_RESULT) failed with error %d", (int)err);
      return result;
    }
    result.reserve(numparam);

    CS_DATAFMT datafmt;
    for (CS_INT i = 1; i <= numparam; ++i) {
      err = ct_describe(wrapper(), i, &datafmt);
      if (err != CS_SUCCEED) {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call to ct_describe(%d) failed with error %d", i, (int)err);
        result.clear();
        return result;
      }
      std::string name;
      if (datafmt.name[0]) {
        name = datafmt.name;
      } else {
        char aux[60];
        sprintf(aux, "unnamed column #%d", (int)i);
        name = aux;
      }
      result.push_back(parameter_info_t(name, datafmt.datatype, datafmt.maxlength));
    }
  }

  return result;
}

//------------------------------------------------------------------------------
std::string sybase_low_level_get_default_encoding(const sybase_connection& conn, ExceptionSink* xsink)
{
  CS_LOCALE* locale;
  CS_RETCODE err = cs_loc_alloc(conn.getContext(), &locale);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_loc_alloc() returned error %d", (int)err);
    return std::string();
  }
  ON_BLOCK_EXIT(cs_loc_drop, conn.getContext(), locale);

  CS_CHAR encoding_str[100] = "";
  err = cs_locale(conn.getContext(), CS_GET, locale, CS_SYB_CHARSET, encoding_str, sizeof(encoding_str), 0);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_locale() returned error %d", (int)err);
    return std::string();
  }

  if (!encoding_str[0]) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_locale() returned empty string for encoding");
    return std::string();
  }
  return std::string(encoding_str);
}

//------------------------------------------------------------------------------
void sybase_low_level_bind_parameters(
  const sybase_command_wrapper& wrapper,
  const char* command,
  bool command_is_procedure_call,
  const std::vector<parameter_info_t>& inputs,
  const std::vector<parameter_info_t>& outputs,
  List* passed_arguments,
  ExceptionSink* xsink
  )
{
  // TBD
}

#ifdef DEBUG
#  include "tests/sybase_low_level_interface_tests.cc"
#endif

// EOF

