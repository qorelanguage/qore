/*
  sybase_query_parser.h

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

#ifndef SYBASE_QUERY_PARSER_H_
#define SYBASE_QUERY_PARSER_H_

#include <string>
#include <vector>

class ExceptionSink;

//------------------------------------------------------------------------------
// description of extracted query parameter (value to bind or a placeholder name)
struct sybase_query_parameter
{
  sybase_query_parameter() : m_input_parameter(true) {}
  sybase_query_parameter(const char* s) : m_input_parameter(false), m_placeholder(s) {}

  bool m_input_parameter; // if true then the %v was found
  std::string m_placeholder; // if non-empty then the :name was found and 'name' is placed here

  bool is_input_parameter() const { return m_input_parameter;  } // as opposite to a placeholder
};

//------------------------------------------------------------------------------
// the final result of query processing
struct processed_sybase_query
{
  processed_sybase_query(const char* s, const std::vector<sybase_query_parameter>& params, bool is_procedure)
  : m_result_query_text(s), m_parameters(params), m_is_procedure(is_procedure) {}

  processed_sybase_query() : m_is_procedure(false) {}

  std::string m_result_query_text;
  std::vector<sybase_query_parameter> m_parameters;
  bool m_is_procedure;
};

//------------------------------------------------------------------------------
// Procedure call is: exec[ute] rpc_name(%v, %v, ... :placeholder1, :placeholder2, ...)
//
extern bool is_query_procedure_call(const char* query);

// Process the query text to fit Sybase standards
extern processed_sybase_query parse_sybase_query(const char* original_query_text, ExceptionSink* xsink);

#endif

// EOF

