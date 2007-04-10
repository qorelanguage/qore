/*
  sybase_connection.h

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

#ifndef SYBASE_CONNECTION_H_
#define SYBASE_CONNECTION_H_

#include <qore/Exception.h>
#include <cstypes.h>

//------------------------------------------------------------------------------
// Instantiated class is kept as private data of the Datasource
// for the time thge Datasource exists. All other Sybase
// resources are shortlived (including CS_COMMAND* and its wrapper).
//
class connection
{
private:
  CS_CONTEXT* m_context;
  CS_CONNECTION* m_connection;

  // Sybase callbacks
  static CS_RETCODE clientmsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_CLIENTMSG* errmsg);
  static CS_RETCODE servermsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_SERVERMSG* svrmsg);

public:
  connection();
  ~connection();

  // to be called after the object is constructed
  void init(const char* username, const char* password, const char* dbname, ExceptionSink* xsink);

  CS_CONNECTION* getConnection() const { return m_connection; }
  CS_CONTEXT* getContext() const { return m_context; }
};

#endif

// EOF

