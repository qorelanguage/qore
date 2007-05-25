/*
  initiate_rpc_command.cc

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
#include <ctpublic.h>
#include <cstypes.h>

#include "initiate_rpc_command.h"
#include "command.h"

//------------------------------------------------------------------------------
void initiate_rpc_command(command& cmd, const char* rpc, ExceptionSink* xsink)
{
  assert(rpc && rpc[0]);
  CS_RETCODE err = ct_command(cmd(), CS_RPC_CMD, (CS_CHAR*)rpc, CS_NULLTERM, CS_UNUSED);
  if (err != CS_SUCCEED) {
    xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_command(CS_RPC_CMD, \"%s\") failed with error %d", rpc, (int)err);
  } 
}

#ifdef DEBUG
#  include "tests/initiate_rpc_command_tests.cc"
#endif

// EOF


