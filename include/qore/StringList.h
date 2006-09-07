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

#include <stdlib.h>
#include <string.h>

// frees all strings on deletion
class StringNode {
   public:
      char *str;
      class StringNode *next;

      inline StringNode(char *s) { str = s; }
      inline ~StringNode() { free(str); }
};

// does not free anything on deletion
class charPtrNode {
   public:
      char *str;
      class charPtrNode *next;

      inline charPtrNode(char *s) { str = s; }
};

class StringListIterator {
   private:
      class StringNode *head, *ptr;

   public:
      inline StringListIterator(class StringList *l);

      inline class StringNode *next()
      {
	 if (ptr)
	    ptr = ptr->next;
	 else
	    ptr = head;
	 return ptr;
      }
      inline char *getString()
      {
	 if (!ptr)
	    return NULL;

	 return ptr->str;
      }
};

// double-ended string list - append at the start or end
// frees all strings on deletion
class StringList
{
   friend class StringListIterator;

   private:
      class StringNode *head, *tail;

   public:
      inline StringList()
      {
         head = tail = NULL;
      }

      inline ~StringList()
      {
         class StringNode *w = head;
         
         while (w)
         {
            class StringNode *n = w->next;
            delete w;
            w = n;
         }
      }

      inline void append(char *s)
      {
         class StringNode *n = new StringNode(s);
         n->next = NULL;
	 if (tail)
	    tail->next = n;
	 else
	    head = n;
         tail = n;
      }

      inline void insert(char *s)
      {
         class StringNode *n = new StringNode(s);
         n->next = head;
         head = n;
	 if (!tail)
	    tail = n;
      }

      inline class StringNode *getHead()
      {
	 return head;
      }

      // adds directories in the format <dir1>:<dir2>:... to the list
      void addDirList(char *str);
};

// "head first" string list
// append at the head, only one pointer necessary
class HFStringList
{
   private:
      class StringNode *head;

   public:
      inline HFStringList()
      {
         head = NULL;
      }

      inline ~HFStringList()
      {
         class StringNode *w = head;
         
         while (w)
         {
            class StringNode *n = w->next;
            delete w;
            w = n;
         }
      }

      inline void insert(char *s)
      {
         class StringNode *n = new StringNode(s);
         n->next = head;
         head = n;
      }

      inline class StringNode *getHead()
      {
	 return head;
      }
};

// charPtr list (no deletion)
class charPtrList
{
   private:
   class charPtrNode *head, *tail;

   public:
      inline charPtrList()
      {
         head = tail = NULL;
      }

      inline ~charPtrList()
      {
         class charPtrNode *w = head;
         
         while (w)
         {
            class charPtrNode *n = w->next;
            delete w;
            w = n;
         }
      }

      inline void insert(char *s)
      {
         class charPtrNode *n = new charPtrNode(s);
         n->next = head;
         head = n;
	 if (!tail)
	    tail = n;
      }

      inline void append(char *s)
      {
         class charPtrNode *n = new charPtrNode(s);
         n->next = NULL;
	 if (tail)
	    tail->next = n;
	 else
	    head = n;
         tail = n;
      }

      inline class charPtrNode *getHead()
      {
	 return head;
      }

      // returns 0 for found, -1 for not found
      inline int find(char *str)
      {
	 class charPtrNode *w = head;
	 while (w)
	 {
	    if (!strcmp(w->str, str))
	       return 0;
	    w = w->next;
	 }
	 return -1;
      }

      inline void populate(class charPtrList *l)
      {
	 class charPtrNode *w = head;
	 while (w)
	 {
	    l->append(w->str);
	    w = w->next;
	 }
      }
};

inline StringListIterator::StringListIterator(class StringList *l)
{
   ptr = NULL;
   head = l->head;
}

#endif
