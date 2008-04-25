/*
  QorePGConnection.h
  
  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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


#ifndef _QORE_QOREPGMAPPER_H
#define _QORE_QOREPGMAPPER_H

#include <qore/Qore.h>

typedef std::map<const QoreEncoding *, const char *> rev_encoding_map_t;

class QorePGMapper {
   private:
      DLLLOCAL static const_encoding_map_t map;
      DLLLOCAL static rev_encoding_map_t rmap;
      
      DLLLOCAL QorePGMapper() {}
      DLLLOCAL ~QorePGMapper() {}

   public:
      // static functions
      DLLLOCAL static void static_init();
      DLLLOCAL static const QoreEncoding *getQoreEncoding(const char *enc);
      DLLLOCAL static const char *getPGEncoding(const QoreEncoding *enc);
};

#endif
