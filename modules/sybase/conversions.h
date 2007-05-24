/*
  conversions.h

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007 Qore Technologies sro

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

#ifndef SYBASE_CONVERSIONS_H_
#define SYBASE_CONVERSIONS_H_

// Conversions between some Sybase and Qore types.

#include <ctpublic.h>

class QoreNode;
class ExceptionSink;
class DateTime;
class connection;


// Sybase DATETIME datatype manipulation 
// returns 0=OK, -1=error (exception raised)
DLLLOCAL extern int DateTime_to_DATETIME(DateTime* dt, CS_DATETIME& out, ExceptionSink* xsink);
DLLLOCAL extern void DateTime_to_DATETIME4(connection& conn, DateTime* dt, CS_DATETIME4& out, ExceptionSink* xsink);

DLLLOCAL extern DateTime *TIME_to_DateTime(CS_DATETIME &dt);

DLLLOCAL extern DateTime* DATETIME_to_DateTime(CS_DATETIME& dt);
DLLLOCAL extern DateTime* DATETIME4_to_DateTime(connection& conn, CS_DATETIME4& dt, ExceptionSink* xsink);

// Sybase MONEY datatype manipulation (it is converted from/to float)
DLLLOCAL extern void double_to_MONEY(connection& conn, double val, CS_MONEY& out, ExceptionSink* xsink);
DLLLOCAL extern void double_to_MONEY4(connection& conn, double val, CS_MONEY4& out, ExceptionSink* xsink);

DLLLOCAL extern double MONEY_to_double(connection& conn, CS_MONEY& m, ExceptionSink* xsink);
DLLLOCAL extern double MONEY4_to_double(connection& conn, CS_MONEY4& m, ExceptionSink* xsink);

// Sybase DECIMAL datatype manipulation
DLLLOCAL extern void double_to_DECIMAL(connection& conn, double val, CS_DECIMAL& out, ExceptionSink* xsink);
DLLLOCAL extern class QoreString *DECIMAL_to_string(connection& conn, CS_DECIMAL& m, ExceptionSink* xsink);


#endif

// EOF

