/*
  get_columns_count.cc

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

#include <assert.h>

#include "get_columns_count.h"
#include "command.h"

//------------------------------------------------------------------------------
unsigned get_columns_count(command& cmd, ExceptionSink* xsink)
{
  CS_INT num_cols;
  CS_RETCODE err = ct_res_info(cmd(), CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call ct_res_info() failed to get number of columns, error %d", (int)err);
    return 0;
  }
  if (num_cols <= 0) {
    assert(false); // cannot happen
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Internal error: no columns returned");
    return 0;
  }
  return num_cols;
}

// EOF

