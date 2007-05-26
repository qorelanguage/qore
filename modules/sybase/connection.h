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

#include <cstypes.h>
#include <ctpublic.h>

#include <stdarg.h>

// Instantiated class is kept as private data of the Datasource
// for the time the Datasource exists. All other Sybase
// resources are shortlived (including CS_COMMAND* and its wrapper).
class connection
{
private:
  CS_CONTEXT* m_context;
  CS_CONNECTION* m_connection;
  CS_LOCALE* m_charset_locale; // lifetime is equal to lifetime of the connection
  bool connected;

  // Sybase callbacks
/*
  static CS_RETCODE clientmsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_CLIENTMSG* errmsg);
  static CS_RETCODE servermsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_SERVERMSG* svrmsg);
*/

#ifdef FREETDS
  DLLLOCAL int set_chained_transaction_mode(ExceptionSink* xsink);
#endif

public:
  DLLLOCAL connection();
  DLLLOCAL ~connection();

  // to be called after the object is constructed
  // returns 0=OK, -1=error (exception raised)
  DLLLOCAL int init(const char* username, const char* password, const char* dbname, const char *db_encoding, ExceptionSink* xsink);
  // returns 0=OK, -1=error (exception raised)
  DLLLOCAL int purge_messages(class ExceptionSink *xsink);
  // returns -1
  DLLLOCAL int do_exception(class ExceptionSink *xsink, const char *err, const char *fmt, ...);
  DLLLOCAL int direct_execute(const char* sql_text, ExceptionSink* xsink);

  DLLLOCAL CS_CONNECTION* getConnection() const { return m_connection; }
  DLLLOCAL CS_CONTEXT* getContext() const { return m_context; }
};

#endif

// EOF

