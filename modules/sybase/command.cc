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

#ifdef DEBUG
#  include "tests/command_tests.cc"
#endif

// EOF


