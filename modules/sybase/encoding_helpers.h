/*
  encoding_helpers.h

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

#ifndef SYBASE_ENCODING_HELPERS_H_
#define SYBASE_ENCODING_HELPERS_H_

#include <string>

class connection;
class ExceptionSink;

//------------------------------------------------------------------------------
// Code based on QoreXYZMapper sources for other databases.

class QoreNode;
class QoreEncoding;

extern QoreEncoding* name_to_QoreEncoding(const char* name);
extern const char* QoreEncoding_to_SybaseName(QoreEncoding *enc);

//------------------------------------------------------------------------------
// Return lowercased encoding name in Sybase format (e.g. utf8, iso_1).
// See http://infocenter.sybase.com/help/index.jsp?topic=/com.sybase.dc35823_1500/html/uconfig/X29127.htm
// (customizing locale information for Adaptive Server)
//
extern std::string get_default_Sybase_encoding(connection& conn, ExceptionSink* xsink);

#endif

// EOF

