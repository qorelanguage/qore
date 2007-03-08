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

class sybase_connection;
class List;

//------------------------------------------------------------------------------
extern int sybase_low_level_commit(sybase_connection* sc, ExceptionSink* xsink);
extern int sybase_low_level_rollback(sybase_connection* sc, ExceptionSink* xsink);

// The command has no bindings and returns no results.
extern void sybase_low_level_execute_directly_command(CS_CONNECTION* conn, const char* sql_text, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Class used to automatically free all resources associated with a Sybase command
class sybase_command_wrapper
{
  CS_COMMAND* m_cmd;
  std::string m_string_id; // should be unique across connection

public:
  sybase_command_wrapper(CS_CONNECTION* conn, ExceptionSink* xsink);
  ~sybase_command_wrapper();
  
  CS_COMMAND* operator()() const { return m_cmd; }
  char* getStringId() const { return (char*)m_string_id.c_str(); }
};

//------------------------------------------------------------------------------
extern void sybase_low_level_prepare_command(const sybase_command_wrapper& wrapper, const char* sql_text, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Return lowercased encoding name in Sybase format (e.g. utf8, iso_1).
//
// See http://infocenter.sybase.com/help/index.jsp?topic=/com.sybase.dc35823_1500/html/uconfig/X29127.htm
// (customizing locale information for Adaptive Server)
//
extern std::string sybase_low_level_get_default_encoding(const sybase_connection& conn, ExceptionSink* xsink);

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
// Bind input (and ouput by reference) parameters to the command.
extern void sybase_low_level_bind_parameters(const sybase_command_wrapper& wrapper,
 const std::vector<parameter_info_t>& inputs, const std::vector<parameter_info_t>& outputs,
 List* passed_arguments, ExceptionSink* xsink);

#endif

// EOF

