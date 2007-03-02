/*
  QorePGMapper.cc
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include "QorePGMapper.h"

encoding_map_t QorePGMapper::map;
rev_encoding_map_t QorePGMapper::rmap;

#define DO_MAP(a, b) map[(a)] = (b); rmap[(b)] = (a);

void QorePGMapper::static_init()
{
   DO_MAP("UTF8",       QCS_UTF8);
   DO_MAP("LATIN1",     QCS_ISO_8859_1);
   DO_MAP("LATIN2",     QCS_ISO_8859_2);
   DO_MAP("LATIN3",     QCS_ISO_8859_3);
   DO_MAP("LATIN4",     QCS_ISO_8859_4);
   DO_MAP("ISO_8859_5", QCS_ISO_8859_5);
   DO_MAP("ISO_8859_6", QCS_ISO_8859_6);
   DO_MAP("ISO_8859_7", QCS_ISO_8859_7);
   DO_MAP("ISO_8859_8", QCS_ISO_8859_8);
   DO_MAP("LATIN5",     QCS_ISO_8859_9);
   DO_MAP("LATIN6",     QCS_ISO_8859_10);
   DO_MAP("LATIN7",     QCS_ISO_8859_13);
   DO_MAP("LATIN8",     QCS_ISO_8859_14);
   DO_MAP("LATIN9",     QCS_ISO_8859_15);
   DO_MAP("LATIN10",    QCS_ISO_8859_16);
   DO_MAP("SQL_ASCII",  QCS_USASCII);
   DO_MAP("KOI8",       QCS_KOI8_R);
}

class QoreEncoding *QorePGMapper::getQoreEncoding(const char *cs)
{
   encoding_map_t::const_iterator i = map.find(cs);
   
   if (i != map.end())
      return i->second;

   return QEM.findCreate((char *)cs);
}

const char *QorePGMapper::getPGEncoding(QoreEncoding *enc)
{
   rev_encoding_map_t::const_iterator i = rmap.find(enc);

   if (i != rmap.end())
      return i->second;

   return NULL;
}
