/*
  sybase_low_level_interface.h

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

#ifndef SYBASE_LOW_LEVEL_INTERFACE_H_
#define SYBASE_LOW_LEVEL_INTERFACE_H_

// Low level interface to Sybase CT library.
// Most of CT calls should be wrapped here.

#include <string>
#include <vector>
#include <qore/Exception.h>
#include <ctpublic.h>
#include "sybase_query_parser.h"

class sybase_connection;
class QoreNode;
class QoreEncoding;
class DateTime;

// The command has no bindings and returns no results.
extern void sybase_low_level_execute_directly_command(CS_CONNECTION* conn, const char* sql_text, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// for ct_dynamic(CS_EXECUTE)
extern void sybase_low_level_prepare_command(const sybase_command_wrapper& wrapper, const char* sql_text, ExceptionSink* xsink);
// for ct_command(CS_LANG_CMD)
extern void sybase_low_level_initiate_lang_command(const sybase_command_wrapper& wrapper, const char* sql_text, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// description of required input and output parameters for a SQL command, is extracted from Sybase 
struct parameter_info_t {
  parameter_info_t(const std::string& n, unsigned t, unsigned s) 
  : m_name(n), m_type(t), m_max_size(s) {}

  std::string m_name; // either provided by Sybase or assembled by the module
  unsigned m_type; // CS_..._TYPE constants
  unsigned m_max_size;
};

// get input & outputs of a SQL command
extern std::vector<parameter_info_t> sybase_low_level_get_input_parameters_info(const sybase_command_wrapper& wrapper, ExceptionSink* xsink);
extern std::vector<parameter_info_t> sybase_low_level_get_output_data_info(const sybase_command_wrapper& wrapper, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// used by the function bellow
struct bind_parameter_t 
{
  bind_parameter_t(int t, QoreNode* n) 
  : m_type(t), m_node(n)  {}

  int m_type; // e.g. CS_INT_TYPE
  QoreNode* m_node; // not owned
};

// bind SQL command (RPCs do not use binding)
extern void sybase_low_level_bind_parameters(
  const sybase_command_wrapper& wrapper,
  const QoreEncoding* encoding,
  const char* command,
  const std::vector<bind_parameter_t>& parameters,
  ExceptionSink* xsink
  );

//------------------------------------------------------------------------------
// used by the function bellow
struct RPC_parameter_info_t
{
  RPC_parameter_info_t(int t)
  : m_type(t), m_is_input(false), m_node(0) {}
  RPC_parameter_info_t(int t, QoreNode* n) 
  : m_type(t), m_is_input(true), m_node(n) {}

  int m_type; // e.g. CS_TYPE_INT
  bool m_is_input; // either input (then m_node may contain value) or a placeholder
  QoreNode* m_node; // not owned
};

// send RPC command to the server
extern void execute_RPC_call(
  const sybase_command_wrapper& wrapper,
  const QoreEncoding* encoding,
  const char* RPC_command, // just name, w/o "exec[ute]" or parameters list
  const std::vector<RPC_parameter_info_t>& parameters,
  ExceptionSink* xsink
  );

//------------------------------------------------------------------------------
// wraps ct_dynamic()
extern void sybase_ct_dynamic(
  const sybase_command_wrapper& wrapper,
  int command_type_code, // e.g. CS_EXECUTE
  ExceptionSink* xsink
  );
  
//------------------------------------------------------------------------------
// wraps ct_param()
extern void sybase_ct_param(
  const sybase_command_wrapper& wrapper,
  unsigned parameter_index, // starting with 0
  const QoreEncoding* encoding,
  int type, // like CS_INT_TYPE
  QoreNode* data,
  ExceptionSink* xsink
  );

//------------------------------------------------------------------------------
// Sybase DATETIME datatype manipulation 
extern void convert_QoreDatetime2SybaseDatetime(CS_CONTEXT* context, DateTime* dt, CS_DATETIME& out, ExceptionSink* xsink);
extern void convert_QoreDatetime2SybaseDatetime4(CS_CONTEXT* context, DateTime* dt, CS_DATETIME4& out, ExceptionSink* xsink);

extern DateTime* convert_SybaseDatetime2QoreDatetime(CS_CONTEXT* context, CS_DATETIME& dt, ExceptionSink* xsink);
extern DateTime* convert_SybaseDatetime4_2QoreDatetime(CS_CONTEXT* context, CS_DATETIME4& dt, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Sybase MONEY datatype manipulation (it is converted from float)
extern void convert_float2SybaseMoney(CS_CONTEXT* context, double val, CS_MONEY& out, ExceptionSink* xsink);
extern void convert_float2SybaseMoney4(CS_CONTEXT* context, double val, CS_MONEY4& out, ExceptionSink* xsink);

extern double convert_SybaseMoney2float(CS_CONTEXT* context, CS_MONEY& m, ExceptionSink* xsink);
extern double convert_SybaseMoney4_2float(CS_CONTEXT* context, CS_MONEY4& m, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Sybase DECIMAL datatype manipulation
extern void convert_float2SybaseDecimal(CS_CONTEXT* context, double val, CS_DECIMAL& out, ExceptionSink* xsink);
extern double convert_SybaseDecimal2float(CS_CONTEXT* context, CS_DECIMAL& m, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Sybase NUMERIC datatype manipulation
extern void convert_float2SybaseNumeric(CS_CONTEXT* context, double val, CS_NUMERIC& out, ExceptionSink* xsink);
extern double convert_SybaseNumeric2float(CS_CONTEXT* context, CS_NUMERIC& m, ExceptionSink* xsink);

#endif

// EOF

