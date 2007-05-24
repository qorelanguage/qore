/*
  direct_execute.cc

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
#include <qore/ScopeGuard.h>
#include <qore/minitest.hpp>

#include <cstypes.h>
#include <ctpublic.h>
#include <assert.h>

#include "direct_execute.h"
#include "connection.h"

//------------------------------------------------------------------------------
void direct_execute(const connection& conn, const char* sql_text, ExceptionSink* xsink)
{
  assert(sql_text && sql_text[0]);

  CS_COMMAND* cmd = 0;
  CS_RETCODE err = ct_cmd_alloc(conn.getConnection(), &cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_cmd_alloc() failed with error %d", (int)err);
    return;
  }
  ON_BLOCK_EXIT(ct_cmd_drop, cmd);
  ScopeGuard canceller = MakeGuard(ct_cancel, (CS_CONNECTION*)0, cmd, CS_CANCEL_ALL);

  err = ct_command(cmd, CS_LANG_CMD, (CS_CHAR*)sql_text, strlen(sql_text), CS_END);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_command(\"%s\") failed with error %d", (int)err, sql_text);
    return;
  }
  err = ct_send(cmd);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_send() failed with error %d", (int)err);
    return;
  }

  // no results expected
  CS_INT result_type;
  err = ct_results(cmd, &result_type);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_result() failed with error %d", (int)err);
    return;
  }

  if (result_type != CS_CMD_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_results() for \"%s\" failed with error %d", sql_text, (int)err);
    return;
  }
  while((err = ct_results(cmd, &result_type)) == CS_SUCCEED);
  canceller.Dismiss();
}

#ifdef DEBUG
#  include "tests/direct_execute_tests.cc"
#endif

// EOF


