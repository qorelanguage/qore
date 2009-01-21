/*
  ConstantList.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  constants can only be defined when parsing
  constants values will be substituted during the 2nd parse phase

  this structure can be safely read at all times, and writes are
  wrapped under the program-level parse lock

  NOTE: constants can only hold immediate values (no objects)

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

#ifndef _QORE_CONSTANTLIST_H

#define _QORE_CONSTANTLIST_H

#ifdef _QORE_LIB_INTERN

#include <qore/common.h>
#include <qore/hash_map.h>

class ConstantList
{
   private:
      hm_qn_t hm;

      DLLLOCAL void remove(hm_qn_t::iterator i);
      DLLLOCAL void deleteAll();

   public:
      DLLLOCAL ~ConstantList();
      DLLLOCAL void add(const char *name, class AbstractQoreNode *value);
      DLLLOCAL class AbstractQoreNode *find(const char *name);
      DLLLOCAL class ConstantList *copy();
      DLLLOCAL void reset();
      DLLLOCAL void assimilate(class ConstantList *n, class ConstantList *otherlist, const char *nsname);
      DLLLOCAL void assimilate(class ConstantList *n);
      DLLLOCAL void parseInit();
      DLLLOCAL QoreHashNode *getInfo();
};

#endif

#endif // _QORE_CONSTANTLIST_H
