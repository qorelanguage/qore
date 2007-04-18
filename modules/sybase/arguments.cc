/*
  arguments.cc

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

#include <assert.h>
#include <cstypes.h>

#include "arguments.h"
#include "connection.h"
#include <cstypes.h>
#include <qore/QoreNode.h>
#include <qore/List.h>

//------------------------------------------------------------------------------
static bool is_valid_Sybase_type(int64 type)
{
  switch (type) {
  case CS_CHAR_TYPE: 
  case CS_BINARY_TYPE: 
  case CS_LONGCHAR_TYPE:
  case CS_LONGBINARY_TYPE:
  case CS_TEXT_TYPE:
  case CS_IMAGE_TYPE:
  case CS_TINYINT_TYPE:
  case CS_SMALLINT_TYPE:
  case CS_INT_TYPE:
  case CS_REAL_TYPE:
  case CS_FLOAT_TYPE:
  case CS_BIT_TYPE:
  case CS_DATETIME_TYPE:
  case CS_DATETIME4_TYPE:
  case CS_MONEY_TYPE:
  case CS_MONEY4_TYPE:
  case CS_NUMERIC_TYPE:
  case CS_DECIMAL_TYPE:
  case CS_VARCHAR_TYPE:
  case CS_VARBINARY_TYPE:
    return true;
  default:
    return false; 
  }
}

//------------------------------------------------------------------------------
static bool is_integer_Sybase_type(int64 type)
{
  switch (type) {
  case CS_INT_TYPE:
  case CS_SMALLINT_TYPE:
  case CS_TINYINT_TYPE:
    return true;
  default:
    return false;
  }
}

//------------------------------------------------------------------------------
std::vector<argument_t> extract_language_command_arguments(List* args, const std::vector<char>& arg_types, ExceptionSink* xsink)
{
  typedef std::vector<argument_t> result_t;

  unsigned query_params = arg_types.size();
  unsigned args_num = args ? args->size() : 0;

  if (query_params == 0 && args_num == 0) {
    return result_t();
  }
  if (query_params == 0 && args_num != 0) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "No parameters were expected");
    return result_t();
  }
  if (args_num != query_params && args_num != query_params * 2) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Expected either %d or %d (with types) parameters", query_params, query_params * 2);
    return result_t();
  }

  result_t result;
  if (args_num == query_params) {
    // no explicit types 
    for (unsigned i = 0; i < args_num; ++i) {
      argument_t added_argument;    
      QoreNode* par = args->retrieve_entry(i);
      added_argument.m_node = par;

      if (arg_types[i] == 'd') {
        if (par->type != NT_INT && par->type != NT_NULL && par->type != NT_NOTHING) {
          xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d needs to be an integer (because of %%d)", i);
          return result_t();
        }
        added_argument.m_type = CS_INT_TYPE;
      } else {
        added_argument.m_type = CS_CHAR_TYPE;
      }
      result.push_back(added_argument);
    } // for 
    return result;
  }
  
  // types are set explicitly
  for (unsigned i = query_params; i < args_num; ++i) {
    QoreNode* type = args->retrieve_entry(i);
    if (type->type != NT_INT) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d needs to be an integer with Sybase type (e.g. Sybase::CS_INT_TYPE)", i);
      return result_t();
    }
    if (!is_valid_Sybase_type(type->val.intval)) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d is not recognized as Sybase type (e.g. Sybase::CS_INT_TYPE)", i);
      return result_t();
    }
  }

  for (unsigned i = 0; i < query_params; ++i) {
    argument_t new_arg;
    new_arg.m_node = args->retrieve_entry(i);

    if (arg_types[i] == 'd') { // more checking
      if (new_arg.m_node->type != NT_INT && new_arg.m_node->type != NT_NOTHING && new_arg.m_node->type != NT_NULL) {
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d is expected to be integer (because of %%d)", i);
        return result_t();
      }
      QoreNode* type_node = args->retrieve_entry(i + query_params);
      if (!is_integer_Sybase_type(type_node->val.intval)) {
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d is expected to be integer type like Sybase::CS_INT_TYPE", i + query_params);
        return result_t();
      }
    }

    result.push_back(new_arg);
  }
  return result;
}

//------------------------------------------------------------------------------
std::vector<argument_t> extract_procedure_call_arguments(List* args,
  const std::vector<processed_procedure_call_t::parameter_t>& arg_infos, ExceptionSink* xsink)
{
  typedef std::vector<argument_t> result_t;

  unsigned input_params_count = 0;
  for (unsigned i = 0; i < arg_infos.size(); ++i) {
    if (arg_infos[i].first == false) { // %v or %d, not a placeholder
      ++input_params_count;
    }
  }
  unsigned expected_params_count = arg_infos.size() + input_params_count;
  unsigned args_count = args ? args->size() : 0;
  
  if (expected_params_count != args_count) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Expected %d parameters (including types), %d parameters found", expected_params_count, args_count);
    return result_t();
  }

  // check type params
  for (unsigned i = input_params_count; i < args_count; ++i) {
    QoreNode* type_node = args->retrieve_entry(i);
    if (type_node->type != NT_INT) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d should be an integer with SYbase type, e.g. Sybase::CS-INT_TYPE)", i);
      return result_t();
    }
    if (!is_valid_Sybase_type(type_node->val.intval)) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d is not recognized as a Sybase type (e.g. Sybase::CS_INT_TYPE)", i);
      return result_t();
    }
  }

  result_t result;

  unsigned read_input_params = 0;
  for (unsigned i = 0; i < arg_infos.size(); ++i) {
    argument_t new_arg;    
    QoreNode* type_node = args->retrieve_entry(i + input_params_count);

    if (arg_infos[i].first == true) { // placeholder
      new_arg.m_node = 0;
      new_arg.m_type = (int)type_node->val.intval;
      new_arg.m_name = arg_infos[i].second;
    } else { // %v or %d
      assert(read_input_params < input_params_count);
      new_arg.m_node = args->retrieve_entry(read_input_params++);
      new_arg.m_type = (int)type_node->val.intval;

      if (arg_infos[i].second == "d") { // more checking for the %d
        if (new_arg.m_node->type != NT_INT && new_arg.m_node->type != NT_NULL && new_arg.m_node->type != NT_NOTHING) {
          xsink->raiseException("DBI-EXEC-EXCEPTION", "Input parameter #%d needs to be an integer (because of %%d)", read_input_params - 1);
          return result_t();
        }
        if (!is_integer_Sybase_type(new_arg.m_type)) {
          xsink->raiseException("DBI-EXEC-EXCEPTION", "Parameter #%d (type) needs to be a Sybase integer type (e.g. Sybase::CS_INT_TYPE) because of %%d", i + input_params_count);
          return result_t();
        }
      }
    }

    result.push_back(new_arg);
  }
  return result;
}

#ifdef DEBUG
#  include "tests/arguments_tests.cc"
#endif

// EOF


