/*
  StringList.h

  Qore Programming Language

  Copyright (C) 2003,2004,2005,2006 David Nichols

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

#ifndef _QORE_STRINGLIST_H

#define _QORE_STRINGLIST_H

#include <qore/safe_dslist>

#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <functional>
#include <list>
#include <deque>

typedef std::list<char *> strlist_t;

typedef std::deque<char *> strdeque_t;

// non-thread-safe list
// a deque should require fewer memory allocations compared to a linked list, so we'll go with the
// deque for now for this list
class StringList : public strdeque_t
{
   public:
      DLLLOCAL ~StringList()
      {
	 std::for_each(begin(), end(), free_ptr<char>());
      }
      DLLLOCAL void addDirList(const char *str);
};

class charPtrList : public safe_dslist<const char *>
{
   public:
      // returns 0 for found, -1 for not found
      // FIXME: use STL find algorithm
      DLLLOCAL int find(const char *str) const
      {
	 const_iterator i = begin();
	 while (i != end())
	 {
	    if (!strcmp(*i, str))
	       return 0;
	    i++;
	 }
   
	 return -1;
      }
};

#endif
