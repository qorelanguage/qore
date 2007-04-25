/*
  initiate_language_command.cc

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

#include "initiate_language_command.h"
#include "command.h"

//------------------------------------------------------------------------------
void initiate_language_command(command& cmd, const char* cmd_text, ExceptionSink* xsink)
{
  assert(cmd_text && cmd_text[0]);
printf("############################################\n");
printf("#### executing cmd [%s]\n", cmd_text);
printf("############################################\n");
  CS_RETCODE err = ct_command(cmd(), CS_LANG_CMD, (CS_CHAR*)cmd_text, CS_NULLTERM, CS_UNUSED);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_command(CS_LANG_CMD, \"%s\") failed with error %d", cmd_text, (int)err);
  } 
}

#ifdef DEBUG
#  include "tests/initiate_language_command_tests.cc"
#endif

// EOF


