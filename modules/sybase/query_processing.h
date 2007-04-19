/*
  query_processing.h

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

#ifndef SYBASE_QUERY_PROCESSING_H_
#define SYBASE_QUERY_PROCESSING_H_

#include <string>
#include <vector>
#include <utility>

class ExceptionSink;

extern bool is_query_procedure_call(const char* query);

//------------------------------------------------------------------------------
// Language command (i.e. not a RPC)
// 
typedef struct processed_language_command_s {
  // with %v and %d replaced with @parX
  std::string m_cmd; 
  // 'v' means %v, 'd' means %d
  std::vector<char> m_parameter_types;
} processed_language_command_t;

extern processed_language_command_t process_language_command(const char* cmd_text, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// A RPC command (with placeholders)
//
typedef struct processed_procedure_call_s {
  // extracted RPC call name
  std::string m_cmd;
  typedef std::pair<
    bool, // true == placeholder name w/o colon, false == "v" or "d"
    std::string // placeholder name or "v" (%v) or "d" (%d)
  > parameter_t;
  std::vector<parameter_t> m_parameters;
} processed_procedure_call_t;

extern processed_procedure_call_t process_procedure_call(const char* rpc_text, ExceptionSink* xsink);

#endif

// EOF

