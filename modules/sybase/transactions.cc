/*
  transactions.cc

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

#include "transactions.h"
#include "direct_execute.h"
#include "connection.h"

/*
// 0 = OK, -1 = error
// currently unused - we use "chained transaction mode" instead
int begin_transaction(connection& conn, ExceptionSink* xsink)
{
  direct_execute(conn, "begin tran", xsink);
  return xsink->isException() ? -1 : 0;
}
*/

//------------------------------------------------------------------------------
// 0 = OK, -1 = error
int commit(connection& conn, ExceptionSink* xsink)
{
  direct_execute(conn, "commit", xsink);
  return xsink->isException() ? -1 : 0;
}

//------------------------------------------------------------------------------
// 0 = OK, -1 = error
int rollback(connection& conn, ExceptionSink* xsink)
{
  direct_execute(conn, "rollback", xsink);
  return xsink->isException() ? -1 : 0;
}

// already included in direct_execute.cc
//#ifdef DEBUG
//#  include "tests/direct_execute_tests.cc"
//#endif

// EOF


