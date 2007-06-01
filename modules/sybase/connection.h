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

#if defined(SYBASE) || defined(FREETDS_LOCALE)
#define SYB_HAVE_LOCALE 1
#else
#undef SYB_HAVE_LOCALE
#endif

// Instantiated class is kept as private data of the Datasource
// for the time the Datasource exists. All other Sybase
// resources are shortlived (including CS_COMMAND* and its wrapper).
class connection
{
   private:
      CS_CONTEXT* m_context;
      CS_CONNECTION* m_connection;
      bool connected;
      class QoreEncoding *enc;

      class QoreNode *exec_intern(class QoreString *cmd_text, class List *qore_args, bool need_list, class ExceptionSink* xsink);

public:
      DLLLOCAL connection();
      DLLLOCAL ~connection();

      // to be called after the object is constructed
      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int init(const char *username, const char *password, const char *dbname, const char *db_encoding, class QoreEncoding *n_enc, ExceptionSink* xsink);
      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int purge_messages(class ExceptionSink *xsink);
      // returns -1
      DLLLOCAL int do_exception(class ExceptionSink *xsink, const char *err, const char *fmt, ...);
      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int direct_execute(const char *sql_text, class ExceptionSink *xsink);
      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int commit(class ExceptionSink *xsink);
      // returns 0=OK, -1=error (exception raised)
      DLLLOCAL int rollback(class ExceptionSink *xsink);
      
      DLLLOCAL class QoreNode *exec(QoreString *cmd, List *parameters, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *exec_rows(QoreString *cmd, List *parameters, class ExceptionSink *xsink);

      DLLLOCAL CS_CONNECTION* getConnection() const { return m_connection; }
      DLLLOCAL CS_CONTEXT* getContext() const { return m_context; }
      DLLLOCAL class QoreEncoding *getEncoding() const { return enc; }

      DLLLOCAL class QoreString *get_client_version(class ExceptionSink *xsink);
      DLLLOCAL QoreNode *get_server_version(class ExceptionSink *xsink);
};

#endif

// EOF

