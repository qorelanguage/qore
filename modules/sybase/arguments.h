/*
  arguments.h

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

#ifndef SYBASE_ARGUMENTS_H_
#define SYBASE_ARGUMENTS_H_

// Process passed parameters and return them in a form used 
// for binding the parameters before execution.
// Type checking (except %d) is postponed until
// binding.

#include <vector>
#include <utility>
#include <string>
#include "query_processing.h"

class QoreNode;
class List;

typedef struct argument_t {
  std::string m_name; // could be empty
  int m_type; // like CS_INT_TYPE
  QoreNode* m_node; // 0 for output parameters
};

extern std::vector<argument_t> extract_language_command_arguments(List* args, const std::vector<char>& arg_types, ExceptionSink* xsink);
extern std::vector<argument_t> extract_procedure_call_arguments(List* args, 
  const std::vector<processed_procedure_call_t::parameter_t>& arg_infos, ExceptionSink* xsink);

#endif

// EOF

