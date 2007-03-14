/*
  QoreSybaseMapper.cc
  
  Qore Programming Language

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

#include "QoreSybaseMapper.h"

encoding_map_t QoreSybaseMapper::map;
rev_encoding_map_t QoreSybaseMapper::rmap;

#ifdef DO_MAP
#  error
#endif

#define DO_MAP(a, b) map[(a)] = (b); rmap[(b)] = (a);

// See http://infocenter.sybase.com/help/index.jsp?topic=/com.sybase.dc35823_1500/html/uconfig/X29127.htm
// for supported languages
void QoreSybaseMapper::static_init()
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
}

class QoreEncoding *QoreSybaseMapper::getQoreEncoding(const char *cs)
{
   encoding_map_t::const_iterator i = map.find(cs);
   
   if (i != map.end())
      return i->second;

   return QEM.findCreate((char *)cs);
}

const char *QoreSybaseMapper::getSybaseEncoding(QoreEncoding *enc)
{
   rev_encoding_map_t::const_iterator i = rmap.find(enc);

   if (i != rmap.end())
      return i->second;

   return NULL;
}

#undef DO_MAP

// EOF

