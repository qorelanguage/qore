/*
  encoding_helpers.cc

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
#include <map>

#include <ctpublic.h>

#include "encoding_helpers.h"
#include "connection.h"

//------------------------------------------------------------------------------
typedef std::map<const QoreEncoding*, const char*> rev_encoding_map_t;

static const_encoding_map_t encoding_map;
static rev_encoding_map_t encoding_rmap;

#ifdef DO_MAP
#  error
#endif

#define DO_MAP(a, b) encoding_map[(a)] = (b); encoding_rmap[(b)] = (a);

// See http://infocenter.sybase.com/help/index.jsp?topic=/com.sybase.dc35823_1500/html/uconfig/X29127.htm
// for supported languages
static bool init_encoding_maps()
{
  DO_MAP("utf8",     QCS_UTF8);
  DO_MAP("iso_1",    QCS_ISO_8859_1);   // Western Europe Latin
  DO_MAP("iso88592", QCS_ISO_8859_2);   // Central Europe Latin
  DO_MAP("iso88595", QCS_ISO_8859_5);   // Cyrillic
  DO_MAP("iso88596", QCS_ISO_8859_6);   // Arabic
  DO_MAP("iso88597", QCS_ISO_8859_7);   // Greek
  DO_MAP("iso88598", QCS_ISO_8859_8);   // Hebrew
  DO_MAP("iso88599", QCS_ISO_8859_9);   // Turkish
  DO_MAP("iso15",    QCS_ISO_8859_15);  // latin 1 with euro
  DO_MAP("ascii8",   QCS_USASCII);
  DO_MAP("koi8",     QCS_KOI8_R); // Cyrilic
  return 0;   
}
static bool encoding_dummy = init_encoding_maps();

#undef DO_MAP

//------------------------------------------------------------------------------
const QoreEncoding* name_to_QoreEncoding(const char* name)
{
   const_encoding_map_t::const_iterator it = encoding_map.find(name);
   if (it != encoding_map.end()) return it->second;
   return QEM.findCreate((char*)name);
}

//------------------------------------------------------------------------------
const char* QoreEncoding_to_SybaseName(const QoreEncoding *enc)
{
   rev_encoding_map_t::const_iterator it = encoding_rmap.find(enc);
   if (it != encoding_rmap.end()) return it->second;
   return 0;
}

#ifdef DEBUG
//#  include "tests/encoding_helpers_tests.cc"
#endif

// EOF

