/*
  command.h

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

#ifndef SYBASE_COMMAND_H_
#define SYBASE_COMMAND_H_

#include <ctpublic.h>

class connection;
class ExceptionSink;

// Wrapper for Sybase CS_COMMAND. When the wrapper object
// is destroyed it automatically cleans up held resources.

//------------------------------------------------------------------------------
class command
{ 
  connection& m_conn;
  CS_COMMAND* m_cmd;
  std::string m_string_id; // should be unique across connection

public:
  command(connection& conn, ExceptionSink* xsink);
  ~command();

  CS_COMMAND* operator()() const { return m_cmd; }
  char* getStringId() const { return (char*)m_string_id.c_str(); }
  connection& getConnection() const { return m_conn; }
};

#endif

// EOF

