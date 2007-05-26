/*
  command.cc

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

#include <qore/Qore.h>
#include <qore/minitest.hpp>

#include <assert.h>

#include "command.h"
#include "connection.h"

//------------------------------------------------------------------------------
command::command(connection& conn, ExceptionSink* xsink)
: m_conn(conn),
  m_cmd(0)
{
  CS_RETCODE err = ct_cmd_alloc(m_conn.getConnection(), &m_cmd);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_cmd_alloc() failed with error %d", (int)err);
    return;
  }

  // a unique (within the connection) string identifier needs to be generated
  static unsigned counter = 0;
  ++counter;
  char aux[30];
  sprintf(aux, "my_cmd_%u_%u", (unsigned)pthread_self(), counter);
  m_string_id = aux;
}

//------------------------------------------------------------------------------
command::~command()
{
  if (!m_cmd) return;
  ct_cancel(0, m_cmd, CS_CANCEL_ALL);
  ct_cmd_drop(m_cmd);
}

int command::send(class ExceptionSink *xsink)
{
   CS_RETCODE err = ct_send(m_cmd);
   if (err != CS_SUCCEED) {
      m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_send() failed");
      return -1;
   } 
   return 0;
}

int command::initiate_language_command(const char *cmd_text, class ExceptionSink *xsink)
{
   assert(cmd_text && cmd_text[0]);
   CS_RETCODE err = ct_command(m_cmd, CS_LANG_CMD, (CS_CHAR*)cmd_text, CS_NULLTERM, CS_UNUSED);
   if (err != CS_SUCCEED) {
      m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_command(CS_LANG_CMD, '%s') failed with error %d", cmd_text, (int)err);
      return -1;
   }
   return 0;
}

/*
int command::initiate_rpc_command(const char *rpc, class ExceptionSink *xsink)
{
   assert(rpc && rpc[0]);
   CS_RETCODE err = ct_command(m_cmd, CS_RPC_CMD, (CS_CHAR*)rpc, CS_NULLTERM, CS_UNUSED);
   if (err != CS_SUCCEED) {
      m_comm.do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_command(CS_RPC_CMD, \"%s\") failed with error %d", rpc, (int)err);
      return -1;
   }
   return 0;
} 

bool command::fetch_row_into_buffers(class ExceptionSink *xsink)
{
   CS_INT rows_read;
   CS_RETCODE err = ct_fetch(m_cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED, &rows_read);
   //printd(5, "ct_fetch() returned %d rows_read=%d\n", err, rows_read);
   if (err == CS_SUCCEED) {
      if (rows_read != 1) {
	 m_conn.do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_fetch() returned %d rows (expected 1)", (int)rows_read);
	 return false;
      }
      return true;
   }
   if (err == CS_END_DATA) {
      return false;
   }
   m_conn.do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_fetch() returned errno %d", (int)err);
   return false;
}

unsigned command::get_column_count(ExceptionSink* xsink)
{
   CS_INT num_cols;
   CS_RETCODE err = ct_res_info(m_cmd, CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
   if (err != CS_SUCCEED) {
      m_conn.do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_res_info() failed with error %d", (int)err);
      return 0;
   }
   if (num_cols <= 0) {
      m_conn.do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_res_info() failed");
      return 0;
   }
   return num_cols;
}

#ifdef DEBUG
#  include "tests/initiate_rpc_command_tests.cc"
#endif
*/

#ifdef DEBUG
#  include "tests/send_command_tests.cc"
#  include "tests/command_tests.cc"
#endif

// EOF


