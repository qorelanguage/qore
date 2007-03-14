/*
  sybase_executor.cc

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
#include <qore/Exception.h>
#include <qore/QoreString.h>
#include <qore/QoreNode.h>
#include <qore/Datasource.h>
#include <qore/List.h>

#include <assert.h>
#include <memory>
#include <qore/ScopeGuard.h>
#include <qore/minitest.hpp>

#include "sybase_executor.h"
#include "sybase_low_level_interface.h"
#include "sybase_query_parser.h"
#include "sybase_connection.h"
#include "sybase_read_output.h"

//------------------------------------------------------------------------------
sybase_executor::sybase_executor(Datasource* ds, QoreString* ostr, List *args, ExceptionSink *xsink)
: m_ds(ds),
  m_args(args)
{
  std::auto_ptr<QoreString> cmd(ostr->convertEncoding(get_encoding(), xsink));
  if (xsink->isException()) {
    return;
  }

  m_parsed_query = parse_sybase_query(cmd->getBuffer(), xsink);
  if (xsink->isException()) {
    return;
  }
  // verify that the query is correct  
  for (unsigned i = 0, n = m_parsed_query.m_parameters.size(); i != n; ++i) {
    if (!m_parsed_query.m_parameters[i].is_input_parameter()) {
      if (m_parsed_query.m_is_procedure == false) {
        xsink->raiseException("DBI-EXEC-PARSE-EXCEPTION", "Only procedure calls can have placeholder output parameters");
        return;
      }
    }
  }
}

//------------------------------------------------------------------------------
sybase_executor::~sybase_executor()
{
}

//------------------------------------------------------------------------------
QoreNode* sybase_executor::exec_procedure_call(const sybase_command_wrapper& w, ExceptionSink* xsink)
{

/*### TBD
  execute_RPC_call(w, m_parsed_query.m_result_query_text.c_str(), ..., xsink);
  const sybase_command_wrapper& wrapper,
  const char* RPC_command, // just name, w/o "exec[ute]" or parameters list
  const std::vector<RPC_parameter_info_t>& parameters,
  ExceptionSink* xsink
  );
*/
  // TBD
  return 0;
}

//------------------------------------------------------------------------------
QoreNode* sybase_executor::exec_language_command(const sybase_command_wrapper& w, ExceptionSink* xsink)
{
printf("### position %d\n", __LINE__);
  sybase_low_level_prepare_command(w, m_parsed_query.m_result_query_text.c_str(), xsink);
  if (xsink->isException()) {
    return 0;
  }

printf("### position %d\n", __LINE__);
  std::vector<parameter_info_t> inputs = sybase_low_level_get_input_parameters_info(w, xsink);
  if (xsink->isException()) {
    return 0;
  }
printf("### position %d\n", __LINE__);
  std::vector<parameter_info_t> outputs = sybase_low_level_get_output_data_info(w, xsink);
  if (xsink->isException()) {
    assert(false);
    return 0;
  }

printf("### position %d\n", __LINE__);
  if (inputs.empty()) {
    if (m_args && m_args->size() != 0) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "No parameters excepted for the query %s", m_parsed_query.m_result_query_text.c_str());
      return 0;
    }
  } else {
    unsigned provided_args = 0;
    if (m_args) {
      provided_args = m_args->size();
    }
    if (provided_args != inputs.size()) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "%d parameters expected, %d are provided, query %s", inputs.size(), provided_args, m_parsed_query.m_result_query_text.c_str());
      return 0;
    }
  }
printf("### position %d\n", __LINE__);
  std::vector<bind_parameter_t> bindings;
  bindings.reserve(inputs.size());
  for (unsigned i = 0, n = inputs.size(); i != n; ++i) {
    bindings.push_back(bind_parameter_t(inputs[i].m_type, inputs[i].m_max_size, m_args->retrieve_entry(i)));
  }

printf("### position %d\n", __LINE__);
  sybase_low_level_bind_parameters(w, get_encoding(), m_parsed_query.m_result_query_text.c_str(), bindings, xsink);
printf("#### after binding\n");
  if (xsink->isException()) {
    assert(false);
    return 0;
  }

printf("### position %d\n", __LINE__);
  return convert_sybase_output_to_Qore(w, get_encoding(), m_parsed_query, xsink);
}

//------------------------------------------------------------------------------
QoreNode* sybase_executor::exec_impl(ExceptionSink* xsink)
{
printf("### in exec_impl\n");
  CS_CONNECTION* conn = get_connection()->getConnection();
  assert(conn);
printf("#### in exec_impl2\n");
  sybase_command_wrapper cmd_wrapper(conn, xsink);
  if (xsink->isException()) {
    return 0;
  }

  // procedures are handled differently (e.g no prepare step)
  if (m_parsed_query.m_is_procedure) {
    return exec_procedure_call(cmd_wrapper, xsink);
  } else {
printf("### calling exec_language_command\n");
    return exec_language_command(cmd_wrapper, xsink);
  }
}

//------------------------------------------------------------------------------
QoreNode* sybase_executor::exec(ExceptionSink *xsink)
{
printf("### in exec\n");
  QoreNode* n = exec_impl(xsink);
printf("#### after exec\n");
  if (n) n->deref(xsink); // not needed
  if (xsink->isException()) {
    return 0;
  }
  if (is_autocommit_enabled()) {
    sybase_low_level_commit(get_connection(), xsink);
  }
  return 0;
}

//------------------------------------------------------------------------------
QoreNode* sybase_executor::select(ExceptionSink *xsink)
{
  if (m_parsed_query.m_is_procedure) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "A procedure call cannot select row");
    return 0;
  }

  QoreNode* n = exec_impl(xsink);
  if (xsink->isException()) {
    if (n) n->deref(xsink);
    return 0;
  }
  if (n) {
    if (n->type == NT_LIST) {
      n->deref(xsink);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "SQL command returned more than one row");
      return 0;
    }
    if (n->type != NT_HASH) {
      n->deref(xsink);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error - unexpected type returned");
      return 0;
    }
  }
  return n;
}

//------------------------------------------------------------------------------
QoreNode* sybase_executor::selectRows(ExceptionSink *xsink)
{
  if (m_parsed_query.m_is_procedure) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "A procedure call cannot select row");
    return 0;
  }

  QoreNode* n = exec_impl(xsink);
  if (xsink->isException()) {
    if (n) n->deref(xsink);
    return 0;
  }
  if (n) {
    if (n->type != NT_LIST) {
      if (n->type != NT_HASH) {
        n->deref(xsink);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error - unexpected type returned");
        return 0;
      }
    }
  }
  return n;
}

#ifdef DEBUG
#  include "tests/sybase_executor_tests.cc"
#endif

// EOF

