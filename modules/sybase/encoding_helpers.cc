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
#include <qore/config.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/QoreString.h>
#include <qore/QoreNode.h>
#include <qore/charset.h>
#include <qore/minitest.hpp>
#include <qore/ScopeGuard.h>

#include <assert.h>
#include <map>

#include <ctpublic.h>

#include "encoding_helpers.h"
#include "connection.h"

//------------------------------------------------------------------------------
typedef std::map<QoreEncoding*, const char*> rev_encoding_map_t;

static encoding_map_t encoding_map;
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
  DO_MAP("iso_1",    QCS_ISO_8859_1); // Western Europe Latin
  DO_MAP("iso88592", QCS_ISO_8859_2); // Central Europe Latin
  DO_MAP("iso88595", QCS_ISO_8859_5); // Cyrilic
  DO_MAP("iso88596", QCS_ISO_8859_6); // Arabic
  DO_MAP("iso88597", QCS_ISO_8859_7); // Greek
  DO_MAP("iso88598", QCS_ISO_8859_8); // Hebrew
  DO_MAP("iso88599", QCS_ISO_8859_9); // Turkish
  DO_MAP("ascii8",   QCS_USASCII);
  DO_MAP("koi8",     QCS_KOI8_R); // Cyrilic
  return 0;   
}
static bool encoding_dummy = init_encoding_maps();

#undef DO_MAP

//------------------------------------------------------------------------------
QoreEncoding* name_to_QoreEncoding(const char* name)
{
  encoding_map_t::const_iterator it = encoding_map.find(name);
  if (it != encoding_map.end()) return it->second;
  return QEM.findCreate((char*)name);
}

//------------------------------------------------------------------------------
const char* QoreEncoding_to_SybaseName(QoreEncoding *enc)
{
  rev_encoding_map_t::const_iterator it = encoding_rmap.find(enc);
  if (it != encoding_rmap.end()) return it->second;
  return 0;
}

//------------------------------------------------------------------------------
std::string get_default_Sybase_encoding(connection& conn, ExceptionSink* xsink)
{
  CS_LOCALE* locale;
  CS_RETCODE err = cs_loc_alloc(conn.getContext(), &locale);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_loc_alloc() returned error %d", (int)err);
    return std::string();
  }
  ON_BLOCK_EXIT(cs_loc_drop, conn.getContext(), locale);

  CS_CHAR encoding_str[100] = "";
  err = cs_locale(conn.getContext(), CS_GET, locale, CS_SYB_CHARSET, encoding_str, sizeof(encoding_str), 0);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_locale() returned error %d", (int)err);
    return std::string();
  }
#ifdef SYBASE
#else
  // FreeTDS 0.64 has a bug and always returns empty string
  assert(encoding_str[0] == 0); // if fails the FreeTDS got fixed
  strcpy(encoding_str, "utf8");
#endif
  return std::string(encoding_str);
}

#ifdef DEBUG
#  include "tests/encoding_helpers_tests.cc"
#endif

// EOF

