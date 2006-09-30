/*
  StringList.h

  is a singly-linked list where inserts can be made at either end

  Qore Programming Language

  Copyright (C) 2003,2004,2005 David Nichols

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

#include <qore/config.h>

#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <functional>
//#include <list>
#include <deque>

// STL is currently missing "slist"

#ifdef HAVE_QORE_SLIST
#include <qore/slist_include.h>

typedef slist<char *> strslist_t;
#else
typedef std::list<char *> strslist_t;
#endif

// there doesn't seem to be any singly-linked list with constant-time inserts at the beginning and end
// ("head" and "tail" pointers) meaning that if I want to use STL (or almost-STL) containers, then to get
// this I have to use "list", which is wasteful of space (because it's doubly-linked) :-(
// anyway a deque should require fewer memory allocations compared to a linked list, so we'll go with the
// deque for now..

typedef std::deque<char *> strlist_t;

template <typename T> struct free_ptr : std::unary_function <T*, void>
{
   void operator()(T *ptr)
   {
      free(ptr);
   }
};

class StringList : public strlist_t
{
   public:
      inline ~StringList()
      {
	 std::for_each(begin(), end(), free_ptr<char>());
      }
      void addDirList(char *str);
};

class charPtrList : public strlist_t
{
   public:   
      // returns 0 for found, -1 for not found
      // FIXME: use STL find algorithm
      inline int find(char *str)
      {
	 charPtrList::iterator i = begin();
	 while (i != end())
	 {
	    if (!strcmp(*i, str))
	       return 0;
	    i++;
	 }

	 return -1;
      }
   
      inline void populate(class charPtrList *l)
      {
	 charPtrList::iterator i = begin();
	 while (i != end())
	 {
	    l->push_back(*i);
	    i++;
	 }
      }
};

#endif
