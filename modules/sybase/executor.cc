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

#include <qore/Qore.h>
#include <qore/minitest.hpp>

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
static QoreNode* execute_command_impl(connection& conn, QoreString* cmd_text, List* qore_args, QoreEncoding* encoding, bool list, ExceptionSink* xsink)
{
   processed_language_command_t query;
   if (query.init(cmd_text->getBuffer(), qore_args, xsink))
      return 0;

  command cmd(conn, xsink);
  if (xsink->isException()) {
    return 0;
  }

  initiate_language_command(cmd, query.m_cmd.getBuffer(), xsink);
  if (xsink->isException()) {
    return 0;
  }

  if (!query.m_parameter_types.empty() && set_input_params(cmd, query, qore_args, encoding, xsink))
     return 0;

  send_command(cmd, xsink);
  if (xsink->isException()) {
    return 0;
  }

  QoreNode* result = read_output(cmd, encoding, list, xsink);
  if (xsink->isException()) {
    if (result) result->deref(xsink);
    return 0;
  }
  //printd(5, "execute_command_impl() result=%08p (%lld)\n", result, result && result->type == NT_INT ? result->val.intval : 0LL);
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

  QoreNode* result = read_output(cmd, encoding, true, xsink);
  if (!result) {
    if (!out_names.empty()) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: %d output parameters expected, no one returned", out_names.size());
    }
    return 0;  
  }
  if (result->type != NT_HASH) {
    result->deref(xsink);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: hash output was expected");
    return 0;
  }
  if (result->val.hash->size() != (int)out_names.size()) {
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
QoreNode* execute(Datasource *ds, QoreString* cmd, List* parameters, ExceptionSink* xsink)
{
   connection *conn = (connection*)ds->getPrivateData();
   class QoreEncoding *enc = ds->getQoreEncoding();

   TempEncodingHelper query(cmd, enc, xsink);
   if (!query)
      return 0;

   QoreNode* res = 0;
   if (is_query_procedure_call(query->getBuffer())) {
      res = execute_rpc_impl(*conn, *query, parameters, enc, xsink);    
   } else {
      res = execute_command_impl(*conn, *query, parameters, enc, false, xsink);
   }
   return res;
}

//------------------------------------------------------------------------------
QoreNode* execute_select(Datasource *ds, QoreString* cmd, List* parameters, ExceptionSink* xsink)
{
   connection *conn = (connection*)ds->getPrivateData();
   class QoreEncoding *enc = ds->getQoreEncoding();
  
   TempEncodingHelper query(cmd, enc, xsink);
   if (!query)
      return 0;

   if (is_query_procedure_call(query->getBuffer())) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "'select' cannot be used for procedure calls");
      return 0;
   }

   return execute_command_impl(*conn, *query, parameters, enc, false, xsink);
}

//------------------------------------------------------------------------------
QoreNode* execute_select_rows(Datasource *ds, QoreString* cmd, List* parameters, ExceptionSink* xsink)
{
   printd(5, "execute_select_rows(ds=%08p, cmd='%s', params=%08p)\n", ds, cmd->getBuffer(), parameters); 

   connection *conn = (connection*)ds->getPrivateData();
   // ensure query is in correct encoding for database
   class QoreEncoding *enc = ds->getQoreEncoding();
   TempEncodingHelper query(cmd, enc, xsink);
   if (!query)
      return 0;

   if (is_query_procedure_call(query->getBuffer())) {
      // procedure returns status code, not rows
      xsink->raiseException("DBI-EXEC-EXCEPTION", "'select rows' cannot be used for procedure calls");
      return 0;
   }
   
   QoreNode* res = execute_command_impl(*conn, *query, parameters, enc, true, xsink);
   if (!res) return 0;

   //assert(res->type == NT_LIST || res->type == NT_HASH);

/*
   if (res->type != NT_LIST && res->type != NT_HASH)
      printd(5, "select_rows returning %08p=%s\n", res, res ? res->type->getName() : "x");
*/

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

