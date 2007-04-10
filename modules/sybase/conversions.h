/*
  conversions.h

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

#ifndef SYBASE_CONVERSIONS_H_
#define SYBASE_CONVERSIONS_H_

// Conversions between some Sybase and Qore types.

#include <ctpublic.h>

class QoreNode;
class ExceptionSink;
class DateTime;
class connection;


//------------------------------------------------------------------------------
// Sybase DATETIME datatype manipulation 
extern void DateTime_to_DATETIME(connection& conn, DateTime* dt, CS_DATETIME& out, ExceptionSink* xsink);
extern void DateTime_to_DATETIME4(connection& conn, DateTime* dt, CS_DATETIME4& out, ExceptionSink* xsink);

extern DateTime* DATETIME_to_DateTime(connection& conn, CS_DATETIME& dt, ExceptionSink* xsink);
extern DateTime* DATETIME4_to_DateTime(connection& conn, CS_DATETIME4& dt, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Sybase MONEY datatype manipulation (it is converted from/to float)
extern void double_to_MONEY(connection& conn, double val, CS_MONEY& out, ExceptionSink* xsink);
extern void double_to_MONEY4(connection& conn, double val, CS_MONEY4& out, ExceptionSink* xsink);

extern double MONEY_to_double(connection& conn, CS_MONEY& m, ExceptionSink* xsink);
extern double MONEY4_to_double(connection& conn, CS_MONEY4& m, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Sybase DECIMAL datatype manipulation (it is converted from/to float)
extern void double_to_DECIMAL(connection& conn, double val, CS_DECIMAL& out, ExceptionSink* xsink);
extern double DECIMAL_to_double(connection& conn, CS_DECIMAL& m, ExceptionSink* xsink);

//------------------------------------------------------------------------------
// Sybase NUMERIC datatype manipulation (it is converted from/to float)
extern void double_to_NUMERIC(connection& conn, double val, CS_NUMERIC& out, ExceptionSink* xsink);
extern double NUMERIC_to_double(connection& conn, CS_NUMERIC& m, ExceptionSink* xsink);

#endif

// EOF

