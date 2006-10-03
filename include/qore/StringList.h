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

#include <qore/config.h>

#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <functional>
#include <list>
#include <deque>

// STL is currently missing "slist"

#ifdef HAVE_QORE_SLIST
#include <qore/slist_include.h>

typedef slist<char *> strslist_t;
#else
typedef std::list<char *> strslist_t;
#endif

typedef std::list<char *> strlist_t;

typedef std::deque<char *> strdeque_t;

template <typename T> struct free_ptr : std::unary_function <T*, void>
{
   void operator()(T *ptr)
   {
      free(ptr);
   }
};

// non-thread-safe list
// a deque should require fewer memory allocations compared to a linked list, so we'll go with the
// deque for now for this list
class StringList : public strdeque_t
{
   public:
      inline ~StringList()
      {
	 std::for_each(begin(), end(), free_ptr<char>());
      }
      void addDirList(char *str);
};

// there doesn't seem to be any singly-linked list with constant-time inserts at the beginning and end
// ("head" and "tail" pointers) meaning that if I want to use STL (or almost-STL) containers, then to get
// this I have to use "list", which is wasteful of space (because it's doubly-linked) :-(
// the safe_dslist defined below is a singly-linked list with constant time inserts at the beginning and
// at the end.  Also it can be read multiple times during inserts - only insers need to be atomic
// Only a subset of the list methods are defined.  

template<typename T> struct _qore_list_node
{
      typedef _qore_list_node self_t;
      self_t *next;
      T data;
   
      inline _qore_list_node(T d, self_t *n) { next = n; data = d; }
      inline _qore_list_node(T d) { next = NULL; data = d; }
};

template<typename T> struct _qore_list_iterator
{
      typedef _qore_list_node<T> node_t;
      typedef _qore_list_iterator<T> self_t;
   
      // data for the node
      node_t *node;

      inline _qore_list_iterator(node_t *n) { node = n; }
      inline void operator++(int) { node = node->next; }
      inline T operator*() { return node->data; }
      inline bool operator!=(const self_t &n) { return n.node != node; }
      inline bool operator==(const self_t &n) { return n.node == node; }
};

template<typename T> class safe_dslist
{  
   public:
      typedef _qore_list_iterator<T> iterator;
      typedef _qore_list_node<T> node_t;
      typedef safe_dslist<T> self_t;
      
   private:
      node_t *head, *tail;

   public: 
      inline safe_dslist() : head(NULL), tail(NULL) {}
      inline ~safe_dslist()
      {
	 node_t *w = head;
	 while (w)
	 {
	    node_t *n = w->next;
	    delete w;
	    w = n;
	 }
      }

      inline iterator begin()
      {
	 return head;
      }
      inline iterator end()
      {
	 return NULL;
      }
      void push_front(T data)
      {
	 node_t *n = new node_t(data, head);
	 if (!tail)
	    tail = n;
	 head = n;
      }
      
      void push_back(T data)
      {
	 node_t *n = new node_t(data);
	 if (tail)
	    tail->next = n;
	 else
	    head = n;
	 tail = n;
      }
   
      inline void populate(self_t &other)
      {
	 iterator i = begin();
	 while (i != end())
	 {
	    other.push_back(*i);
	    i++;
	 }
      }

      inline void populate(self_t *other)
      {
	 iterator i = begin();
	 while (i != end())
	 {
	    other->push_back(*i);
	    i++;
	 }
      }
};

class charPtrList : public safe_dslist<char *>
{
   public:
      // returns 0 for found, -1 for not found
      // FIXME: use STL find algorithm
      inline int find(char *str)
      {
	 iterator i = begin();
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
