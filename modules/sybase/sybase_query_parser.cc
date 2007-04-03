/*
  sybase_query_parser.cc

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
#include <qore/QoreNode.h>
#include <qore/Exception.h>

#include <ctpublic.h>
#include <assert.h>
#include <ctype.h>

#include <qore/ScopeGuard.h>
#include <qore/minitest.hpp>

#include "sybase_query_parser.h"

//------------------------------------------------------------------------------
namespace {
struct array_holder
{
  char* array;
  array_holder(char* a) : array(a) {}
  ~array_holder() { delete[] array; }
};

} // namespace

//------------------------------------------------------------------------------
bool is_query_procedure_call(const char* query)
{
  while (isspace(*query)) ++query;
  if (strncasecmp(query, "exec ", 5) == 0) {
    return true;
  }
  if (strncasecmp(query, "execute ", 8) == 0) {
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
static void parse_rpc_parameters(const char* parameter_list, processed_sybase_query& result, ExceptionSink* xsink)
{
  const char* s = parameter_list;
  for (;;) {
    while (isspace(*s)) ++s;
    if (*s == ')') return;

    // input parameter
    if (*s == '%') {
      ++s;
      if (*s != 'v' && *s != 'd') {
        xsink->raiseException("DBI-EXEC-EXCEPTION", "%%v or %%d expected in procedure parameter list");
        return;
      }
      if (*s == 'v') {
        result.m_parameters.push_back(sybase_query_parameter());
      } else {
        // %d was introduced due to PostgreSQL limitations, it has not a much of use in Sybase 
        // but is handled due to compatibility
        result.m_parameters.push_back(sybase_query_parameter(true));
      }
      ++s;
      while (isspace(*s)) ++s;
      if (*s == ')') return;
      if (*s != ',') {
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Comma separating procedure parameters not found");
        return;
      }
      ++s;
      continue;
    }

    // placeholder
    if (*s == ':') {
      const char* name_start = ++s;
      while (isalnum(*s) || *s == '_') ++s;
      std::string name(name_start, s - name_start);
      result.m_parameters.push_back(sybase_query_parameter(name.c_str()));
    
      while (isspace(*s)) ++s;
      if (*s == ')') return;
      if (*s != ',') {
        xsink->raiseException("DBI-EXEC-EXCEPTION", "Comma separating procedure parameters not found");
        return;
      }
      ++s;
      continue;      
    }

    xsink->raiseException("DBI-EXEC-EXCEPTION", "Unexpected character found. Only %%v and placeholders are allowed in procedure parameter list");
    return;
  } // for
}

//------------------------------------------------------------------------------
// extract name and parameters of procedure call (only %v and placeholders are allowed).
static processed_sybase_query parse_procedure_call(const char* original_query_text, ExceptionSink* xsink)
{
  processed_sybase_query result;
  result.m_is_procedure = true;

  const char* s = original_query_text;
  while (isspace(*s)) ++s;
  if (strncasecmp(s, "exec ", 5) == 0) {
    s += 5;
  } else {
    assert(strncasecmp(s, "execute ", 8) == 0);
    s += 8;
  }
  while (isspace(*s)) ++s;
  const char* rpc_name_start = s;
  while (isalnum(*s) || *s == '_') ++s;
  result.m_result_query_text = std::string(rpc_name_start, s - rpc_name_start);
  
  while (isspace(*s)) ++s;
  if (!*s) return result;
  if (*s != '(') {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Procedure call parameters should be in parenthesis (...)");
    return processed_sybase_query();
  }
  ++s;
  parse_rpc_parameters(s, result, xsink);
  if (xsink->isException()) {
    return processed_sybase_query();
  }
  
  return result;
}

//------------------------------------------------------------------------------
// replace %v with ?. No placeholders allowed here.
static processed_sybase_query parse_sql_query(const char* original_query_text, ExceptionSink* xsink)
{
  unsigned len = strlen(original_query_text);
  array_holder text(new char[len + 1]);
  strcpy(text.array, original_query_text);

  processed_sybase_query result;
  result.m_is_procedure = false;

  char* s = text.array;
  while (*s) {

    // skip double quoted strings
    if (*s == '"') {
      ++s;
      for (;;) {
        while (*s && *s != '"' && *s != '\\') ++s;
        if (!*s) {
          xsink->raiseException("DBI-EXEC-EXCEPTION", "Invalid query: double quoted string is not terminated");
          return processed_sybase_query();
        }
        if (*s == '"') {
          goto next;
        }
        ++s;
      }
    } 

    // skip single qoted strings
    if (*s == '\'') {
      ++s;
      for (;;) {
        while (*s && *s != '\'' && *s != '\\') ++s;
        if (!*s) {
          xsink->raiseException("DBI-EXEC-EXCEPTION", "Invalid query: single quoted string is not terminated");
          return processed_sybase_query();
        }
        if (*s == '\'') {
          goto next;
        }
        ++s;
      }
    }

    if (*s == ':') {
      // Sybase doesn't allow such things
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Placeholders (starting with :) could be used only in procedure calls");
      return processed_sybase_query();
    }

    if (*s == '%' && s[1] == 'v') {
      *s = ' ';
      s[1] = '?'; // Sybase market of value to be bound
      ++s;
      result.m_parameters.push_back(sybase_query_parameter());
    }

next:
    ++s;
  } // while
  
  result.m_result_query_text = text.array;
  return result;
}

//------------------------------------------------------------------------------
processed_sybase_query parse_sybase_query(const char* original_query_text, ExceptionSink* xsink)
{
  if (is_query_procedure_call(original_query_text)) {
    return parse_procedure_call(original_query_text, xsink);
  } else {
    return parse_sql_query(original_query_text, xsink);
  }
}

//------------------------------------------------------------------------------
processed_sybase_query::processed_sybase_query(const char* s, const std::vector<sybase_query_parameter>& params, bool is_procedure)
: m_parameters(params), 
  m_is_procedure(is_procedure)
{
  m_result_query_text = generate_query_parameter_names(s); 
}

//------------------------------------------------------------------------------
std::string processed_sybase_query::generate_query_parameter_names(const char* s) 
{
  std::string result;
  result.reserve(1000);
  unsigned args = 0;
  while (*s) {
    char ch = *s++;

    // skip double qouted strings
    if (ch == '"') {
      result += ch;
      for (;;) {
        ch = *s++;
        result += ch;
        if (ch == '\\') {
          ch = *s++;
          result.push_back(ch);
          continue;
        }
        if (ch == '"') {
          goto next;
        }
      }      
    }

    // skip single qouted strings
    if (ch == '\'') {
      result += ch;
      for (;;) {
        ch = *s++;
        result += ch;
        if (ch == '\\') {
          ch = *s++;
          result.push_back(ch);
          continue;
        }
        if (ch == '\'') {
          goto next;
        }
      }
    }

    if (ch == '?') {
      // ? was inserted originally for the dynamic SQL. ct_dynamic(CS_LANG_CMD) requires something as @xyz
      char aux[20];
      sprintf(aux, "@par%d", args++);
      result += (const char*)aux;
    } else {
      result += ch;
    }

next:;
  } // main while
  return result;
}


#ifdef DEBUG
#  include "tests/sybase_query_parser_tests.cc"
#endif

// EOF

