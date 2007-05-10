/*
  executor.cc

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
#include <qore/Exception.h>
#include <qore/minitest.hpp>
#include <qore/List.h>
#include <qore/Hash.h>
#include <qore/QoreNode.h>
#include <qore/QoreString.h>
#include <qore/QoreType.h>

#include <assert.h>

#include "executor.h"
#include "connection.h"
#include "encoding_helpers.h"
#include "initiate_language_command.h"
#include "initiate_rpc_command.h"
#include "send_command.h"
#include "read_output.h"
#include "query_processing.h"
#include "command.h"
#include "arguments.h"
#include "read_output.h"
#include "set_parameter.h"

//------------------------------------------------------------------------------
static QoreNode* execute_command_impl(connection& conn, QoreString* cmd_text, List* qore_args, QoreEncoding* encoding, ExceptionSink* xsink)
{
  processed_language_command_t query = process_language_command(cmd_text->getBuffer(), xsink);
  if (xsink->isException()) {
    return 0;
  }

  command cmd(conn, xsink);
  if (xsink->isException()) {
    return 0;
  }

  initiate_language_command(cmd, query.m_cmd.c_str(), xsink);
  if (xsink->isException()) {
    return 0;
  }

  if (!query.m_parameter_types.empty()) {
    // has some input parameters, set them now
    std::vector<argument_t> input_params_extracted = extract_language_command_arguments(qore_args, query.m_parameter_types, xsink);
    if (xsink->isException()) {
      return 0;
    }
    assert(input_params_extracted.size() == query.m_parameter_types.size());
    assert(input_params_extracted.size() == (unsigned)qore_args->size());
    // set input parameters
    for (unsigned i = 0, n = input_params_extracted.size(); i != n; ++i) {
      set_input_parameter(cmd, i, input_params_extracted[i].m_type, input_params_extracted[i].m_node, encoding, xsink);
      if (xsink->isException()) {
        return 0;
      }
    }
  }

  send_command(cmd, xsink);
  if (xsink->isException()) {
    return 0;
  }

  QoreNode* result = read_output(cmd, encoding, xsink);
  if (xsink->isException()) {
    if (result) result->deref(xsink);
    return 0;
  }
  return result;
}

//------------------------------------------------------------------------------
static QoreNode* execute_rpc_impl(connection& conn, QoreString* rpc_text, List* qore_args, QoreEncoding* encoding, ExceptionSink* xsink)
{
  processed_procedure_call_t query = process_procedure_call(rpc_text->getBuffer(), xsink);
  if (xsink->isException()) {
    return 0;
  }

  command cmd(conn, xsink);
  if (xsink->isException()) {
    return 0;
  }

  initiate_rpc_command(cmd, query.m_cmd.c_str(), xsink);
  if (xsink->isException()) {
    return 0;
  }

  std::vector<std::string> out_names;
  if (!query.m_parameters.empty()) {
    std::vector<argument_t> extracted_params = extract_procedure_call_arguments(qore_args, query.m_parameters, xsink);
    if (xsink->isException()) {
      return 0;
    }
    for (unsigned i = 0, n = extracted_params.size(); i != n; ++i) {
      if (extracted_params[i].m_node) {
        // it is input parameter
        set_input_parameter(cmd, i, extracted_params[i].m_type, extracted_params[i].m_node, encoding, xsink);
        if (xsink->isException()) {
          return 0;
        }
      } else {
        // it is output parameter
        set_output_parameter(cmd, i, extracted_params[i].m_name.c_str(), extracted_params[i].m_type, xsink);
        if (xsink->isException()) {
          return 0;
        }
        out_names.push_back(extracted_params[i].m_name);
      }
    }
  }

  send_command(cmd, xsink);
  if (xsink->isException()) {
    return 0;
  }

  QoreNode* result = read_output(cmd, encoding, xsink);
  if (!result) {
    if (!out_names.empty()) {
      assert(false); // internal error
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: %d output parameters expected, no one returned", out_names.size());
    }
    return 0;  
  }
  if (result->type != NT_HASH) {
    assert(false); // internal error
    result->deref(xsink);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: hash output was expected");
    return 0;
  }
  if (result->val.hash->size() != (int)out_names.size()) {
    assert(false);
    result->deref(xsink);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: %d output parameters expected, %d returned", out_names.size(), result->val.hash->size());
    return 0;
  }

  // convert into a hash with expected output names
  Hash* h = new Hash;
  HashIterator it(result->val.hash);
  unsigned i = 0;
  while (it.next()) {
    QoreNode* val = it.getValue();
    assert(val);
    val->ref();
    assert(i < out_names.size());
    h->setKeyValue(out_names[i].c_str(), val, xsink);
    ++i;
  }
  QoreNode* res2 = new QoreNode(h);
  result->deref(xsink);
  return res2;
}

//------------------------------------------------------------------------------
QoreNode* execute(connection& conn, QoreString* cmd, List* parameters, ExceptionSink* xsink)
{
  std::string enc_s = get_default_Sybase_encoding(conn, xsink);
  QoreEncoding* enc = name_to_QoreEncoding(enc_s.c_str());
  if (xsink->isException()) {
    return 0;
  }
  TempEncodingHelper query(cmd, enc, xsink);
  if (xsink->isException()) {
    return 0;
  }

  QoreNode* res = 0;
  if (is_query_procedure_call(query->getBuffer())) {
    res = execute_rpc_impl(conn, *query, parameters, enc, xsink);    
  } else {
    res = execute_command_impl(conn, *query, parameters, enc, xsink);
  }
  if (res) res->deref(xsink);
  return 0;
}

//------------------------------------------------------------------------------
QoreNode* execute_select(connection& conn, QoreString* cmd, List* parameters, ExceptionSink* xsink)
{
  std::string enc_s = get_default_Sybase_encoding(conn, xsink);
  QoreEncoding* enc = name_to_QoreEncoding(enc_s.c_str());
  if (xsink->isException()) {
    return 0;
  }
  TempEncodingHelper query(cmd, enc, xsink);
  if (xsink->isException()) {
    return 0;
  }
  if (is_query_procedure_call(query->getBuffer())) {
    assert(false); // procedure returns status code, not rows
    xsink->raiseException("DBI-EXEC-EXCEPTION", "'select' cannot be used for procedure calls");
    return 0;
  }

  QoreNode* res = execute_command_impl(conn, *query, parameters, enc, xsink);
  if (xsink->isException()) {
    if (res) res->deref(xsink);
    return 0;
  }
  if (!res) res = 0;

  assert(res->type == NT_LIST || res->type == NT_HASH);
  if (res->type == NT_LIST) {
    assert(false);
    res->deref(xsink);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "'select' returned more than single row");
    return 0;
  }

  return res;
}

//------------------------------------------------------------------------------
QoreNode* execute_select_rows(connection& conn, QoreString* cmd, List* parameters, ExceptionSink* xsink)
{
  std::string enc_s = get_default_Sybase_encoding(conn, xsink);
  QoreEncoding* enc = name_to_QoreEncoding(enc_s.c_str());
  if (xsink->isException()) {
    return 0;
  }
  TempEncodingHelper query(cmd, enc, xsink);
  if (xsink->isException()) {
    return 0;
  }
  if (is_query_procedure_call(query->getBuffer())) {
    assert(false); // procedure returns status code, not rows
    xsink->raiseException("DBI-EXEC-EXCEPTION", "'select rows' cannot be used for procedure calls");
    return 0;
  }

  QoreNode* res = execute_command_impl(conn, *query, parameters, enc, xsink);
  if (xsink->isException()) {
    if (res) res->deref(xsink);
    return 0;
  }
  if (!res) return 0;

  assert(res->type == NT_LIST || res->type == NT_HASH);
  if (res->type == NT_HASH) {
    List* l = new List;
    l->push(res);
    res = new QoreNode(l); 
  }
  return res;
}

#ifdef DEBUG
#  include "tests/executor_tests.cc" 
#  include "tests/executor_rpc_tests.cc"
#endif

// EOF

