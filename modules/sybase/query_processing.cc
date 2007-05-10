/*
  query_processing.cc

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
#include <ctype.h>

#include "query_processing.h"

//------------------------------------------------------------------------------
bool is_query_procedure_call(const char* query)
{
  while (isspace(*query)) ++query;
  if (strncasecmp(query, "execute ", 8) == 0) {
    return true;
  }
  if (strncasecmp(query, "exec ", 5) == 0) {
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
processed_language_command_t process_language_command(const char* cmd_text, ExceptionSink* xsink)
{
  processed_language_command_t result;
  result.m_cmd.reserve(1000); // guess
 
  const char* s = cmd_text;   
  while (*s) {
    char ch = *s++;

    // skip double qouted strings
    if (ch == '"') {
      result.m_cmd += ch;
      for (;;) {
        ch = *s++;
        result.m_cmd += ch;
        if (ch == '\\') {
          ch = *s++;
          result.m_cmd += ch;
          continue;
        }
        if (ch == '"') {
          goto next;
        }
      }
    }
    // skip single qouted strings
    if (ch == '\'') {
      result.m_cmd += ch;
      for (;;) {
        ch = *s++;
        result.m_cmd += ch;
        if (ch == '\\') {
          ch = *s++;
          result.m_cmd += ch;
          continue;
        }
        if (ch == '\'') {
          goto next;
        }
      }
    }

    if (ch == '%') {
      ch = *s++;
      if (ch == 'v') {
        result.m_parameter_types.push_back('v');
      } else
      if (ch == 'd') {
        result.m_parameter_types.push_back('d');
      } else {
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Only %%v or %%d expected in parameter list");
        return processed_language_command_t();
      }
    
      char aux[20];
      sprintf(aux, "@par%u", result.m_parameter_types.size());
      result.m_cmd += (const char*)aux;
    } else {
      result.m_cmd += ch;
    }

next:;
  } // while

  return result;
}

//------------------------------------------------------------------------------
processed_procedure_call_t process_procedure_call(const char* rpc_text, ExceptionSink* xsink)
{
  if (!is_query_procedure_call(rpc_text)) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Procedure call should start with 'exec' or 'execute'");
    return processed_procedure_call_t();
  }

  processed_procedure_call_t result;
  const char* s = rpc_text;

  // get the RPC name first
  while (isspace(*s)) ++s;
  if (strncasecmp(s, "execute ", 8) == 0) {
    s += 8;
  } else
  if (strncasecmp(s, "exec ", 5) == 0) {
    s += 5;
  } else {
    assert(false);
  }
  while (isspace(*s)) ++s;

  const char* name_start = s;
  while (isalnum(*s) || *s == '_') ++s;
  if (s == name_start) { 
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Procedure name is missing");
    return processed_procedure_call_t();
  }
  result.m_cmd = std::string(name_start, s - name_start);
  while (isspace(*s)) ++s;
  if (*s == 0) { // just RPC name
    return result;
  }
  if (*s != '(') {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Open parenthesis '(' expected after procedure name");
    return processed_procedure_call_t();
  }
  ++s;
  while (isspace(*s)) ++s;
  if (*s == ')') { // no parameters
    return result;
  }
  if (!*s) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Closing parenthesis ')' is missing");
    return processed_procedure_call_t();
  }
  
  // read parameters
  while (*s) {
    while (isspace(*s)) ++s;

    if (*s == ')') {
      return result;
    }
    // read input parameter marker
    if (*s == '%') { // %v or %d
      ++s;
      if (*s == 'v') {
        result.m_parameters.push_back(std::make_pair(false, std::string("v")));
      } else 
      if (*s == 'd') {
        result.m_parameters.push_back(std::make_pair(false, std::string("d")));
      } else {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "%%v or %%d expected for an input parameter");
        return processed_procedure_call_t();
      }
      ++s;
      while (isspace(*s)) ++s;
      if (*s == ')') {
        return result;
      } else
      if (*s == ',') {
        ++s;
      } else {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "%%v or %%d should be followed by comma or closing parenthesis");
        return processed_procedure_call_t();
      }
      continue;
    }
    // read placeholder name
    if (*s == ':') {
      ++s;
      const char* placeholder_start = s;
      while (isalnum(*s) || *s == '_') ++s;
      if (s == placeholder_start) {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Placeholder name missing after ':'");
        return processed_procedure_call_t();
      }
      std::string name(placeholder_start, s - placeholder_start);
      result.m_parameters.push_back(std::make_pair(true, name));

      while (isspace(*s)) ++s;
      if (*s == ')') {
        return result;
      } else
      if (*s == ',') {
        ++s;
      } else {
        assert(false);
        xsink->raiseException("DBI-EXEC-EXCEPTION", "placeholder should be followed by comma or closing parenthesis");
        return processed_procedure_call_t();
      }
      continue;
    }

    if (*s == 0) {
      assert(false);
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Unexpected end of query - closing parenthesis is missing");
      return processed_procedure_call_t();
    }
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Unexpected text in query starting with [%s]", s);
    return processed_procedure_call_t();

  } // while

  return result;
}


#ifdef DEBUG
#  include "tests/query_processing_tests.cc"
#endif

// EOF

