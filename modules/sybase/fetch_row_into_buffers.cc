/*
  fetch_rows_into_buffers.cc

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
#include <qore/minitest.hpp>

#include <assert.h>

#include "fetch_row_into_buffers.h"
#include "command.h"

//------------------------------------------------------------------------------
bool fetch_row_into_buffers(command& cmd, ExceptionSink* xsink)
{
  CS_INT rows_read;
  CS_RETCODE err = ct_fetch(cmd(), CS_UNUSED, CS_UNUSED, CS_UNUSED, &rows_read);
  //printd(5, "ct_fetch() returned %d rows_read=%d\n", err, rows_read);
  if (err == CS_SUCCEED) {
    if (rows_read != 1) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: ct_fetch() returned %d rows (expected 1)", (int)rows_read);
      return false;
    }
    return true;
  }
  if (err == CS_END_DATA) {
    return false;
  }
  xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_fetch() returned erro %d", (int)err);
  return false;
}

// EOF

