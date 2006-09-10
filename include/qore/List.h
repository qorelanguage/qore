/*
  List.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  FIXME: require derefence before delete

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

#ifndef _QORE_LIST_H

#define _QORE_LIST_H

class List {   
      class QoreNode **entry;
      int length;
      int allocated;

      void resize(int num);
      inline void check_offset(int &offset);
      inline void check_offset(int &offset, int &len);
      void splice_intern(int offset, int length, class ExceptionSink *xsink);
      void splice_intern(int offset, int length, class QoreNode *l, class ExceptionSink *xsink);

   public:
      int needs_eval;

      inline List();
      inline List(int i);
      inline ~List();
      inline class QoreNode *retrieve_entry(int index);
      inline int getEntryAsInt(int index);
      inline class QoreNode **get_entry_ptr(int index);
      inline class QoreNode **getExistingEntryPtr(int index);
      inline void set_entry(int index, class QoreNode *val, class ExceptionSink *xsink);
      inline class QoreNode *eval_entry(int num, class ExceptionSink *xsink);
      inline void push(class QoreNode *val);
      inline void insert(class QoreNode *val);
      inline class QoreNode *pop();
      inline class QoreNode *shift();
      inline void merge(class List *list);
      inline int delete_entry(int index, class ExceptionSink *xsink);
      inline void pop_entry(int index, class ExceptionSink *xsink);
      inline int size();
      void dereference(class ExceptionSink *xsink);
      inline class List *evalList(class ExceptionSink *xsink);
      inline class QoreNode *eval(class ExceptionSink *xsink);
      inline class QoreNode *evalFrom(int offset, class ExceptionSink *xsink);
      inline class QoreNode *copy();
      inline class List *copyList();
      inline class List *copyListFrom(int offset);
      class QoreNode *sort();
      class QoreNode *sortDescending();
      inline void splice(int offset, class ExceptionSink *xsink);
      inline void splice(int offset, int length, class ExceptionSink *xsink);
      inline void splice(int offset, int length, class QoreNode *l, class ExceptionSink *xsink);
};

#include <stdio.h>

class ListIterator
{
private:
   int pos;
   class List *l;
   
public:
   inline ListIterator(class List *lst) { l = lst; pos = -1; }
   
   inline bool next() 
   { 
      if (pos < 0)
      {
	 if (l->size())
	    pos = 0;
      }
      else
      {
	 if (++pos == l->size())
	    pos = -1;
      }
      return pos >= 0;
   }
   inline class QoreNode *getValue()
   {
      if (pos < 0)
	 return NULL;
      return l->retrieve_entry(pos);
   }
   inline class QoreNode **getValuePtr()
   {
      if (pos < 0)
	 return NULL;
      return l->get_entry_ptr(pos);
   }
   inline class QoreNode *eval(class ExceptionSink *xsink);
   //inline bool last() { return (bool)(ptr ? !ptr->next : false); } 
   //inline void setValue(class QoreNode *val, class ExceptionSink *xsink);
};

#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/Object.h>
#ifdef DEBUG
#include <qore/support.h>
#endif

#include <stdlib.h>
#include <string.h>

#define LIST_BLOCK 20
#define LIST_PAD   15

inline List::List()
{
   length = 0;
   allocated = 0;
   entry = NULL;
   needs_eval = 0;
}

inline List::List(int i)
{
   length = 0;
   allocated = 0;
   entry = NULL;
   needs_eval = i;
}

inline List::~List()
{
   int i;
   ExceptionSink xsink;

   for (i = 0; i < length; i++)
      if (entry[i])
	 entry[i]->deref(&xsink);
   if (entry)
      free(entry);
}

inline class QoreNode *List::retrieve_entry(int num)
{
   if (num >= length || num < 0)
      return NULL;
   return entry[num];
}

inline int List::getEntryAsInt(int num)
{
   if (num >= length || num < 0 || !entry[num])
      return 0;
   return entry[num]->getAsInt();
}

inline QoreNode **List::get_entry_ptr(int num)
{
   if (num >= length)
      resize(num + 1);
   return &entry[num];
}

inline QoreNode **List::getExistingEntryPtr(int num)
{
   if (num >= length || num < 0)
      return NULL;
   return &entry[num];
}

inline int List::size()
{
   return length;
}

inline void List::set_entry(int index, class QoreNode *val, ExceptionSink *xsink)
{
   class QoreNode **v = get_entry_ptr(index);
   if (*v)
      (*v)->deref(xsink);
   *v = val;
}

inline class QoreNode *List::eval_entry(int num, class ExceptionSink *xsink)
{
   if (num >= length || num < 0)
      return NULL;
   class QoreNode *rv = entry[num];
   if (rv)
      rv = rv->eval(xsink);
   return rv;
}

inline void List::push(class QoreNode *val)
{
   class QoreNode **v = get_entry_ptr(length);
   *v = val;
}

inline void List::merge(class List *list)
{
   int start = length;
   resize(length + list->length);
   for (int i = 0; i < list->length; i++)
   {
      if (list->entry[i])
	 entry[start + i] = list->entry[i]->RefSelf();
      else
	 entry[start + i] = NULL;
   }
}

inline int List::delete_entry(int ind, ExceptionSink *xsink)
{
   if (ind >= length || ind < 0)
      return 1;

   if (entry[ind] && entry[ind]->type == NT_OBJECT)
      entry[ind]->val.object->doDelete(xsink);

   if (entry[ind])
   {
      entry[ind]->deref(xsink);
      entry[ind] = NULL;
   }

   // resize list if deleting last element
   if (ind == (length - 1))
      resize(ind);

   return 0;
}

// delete an entry and move down the rest of the entries
inline void List::pop_entry(int ind, ExceptionSink *xsink)
{
   if (ind >= length || ind < 0)
      return;

   if (entry[ind] && entry[ind]->type == NT_OBJECT)
      entry[ind]->val.object->doDelete(xsink);

   if (entry[ind])
   {
      entry[ind]->deref(xsink);
      entry[ind] = NULL;
   }

   // resize list
   length--;
   if (ind < length)
      memmove(entry + ind, entry + ind + 1, sizeof(entry) * (length - ind));
   resize(length);
}

inline void List::insert(class QoreNode *val)
{
   resize(length + 1);
   memmove(entry + 1, entry, sizeof(QoreNode *) * (length - 1));
   entry[0] = val;
}

inline QoreNode *List::shift()
{
   if (!length)
      return NULL;
   QoreNode *rv = entry[0];
   memmove(entry, entry + 1, sizeof(QoreNode *) * (length - 1));
   entry[length - 1] = NULL;
   resize(length - 1);
   return rv;
}

inline QoreNode *List::pop()
{
   if (!length)
      return NULL;
   QoreNode *rv = entry[length - 1];
   entry[length - 1] = NULL;
   resize(length - 1);
   return rv;
}

inline class List *List::evalList(ExceptionSink *xsink)
{
   tracein("List::eval()");
   class List *nl = new List();
   for (int i = 0; i < length; i++)
   {
      nl->push(entry[i] ? entry[i]->eval(xsink) : NULL);
      if (xsink->isEvent())
      {
	 nl->dereference(xsink);
	 delete nl;
	 traceout("List::evalList()");
	 return NULL;
      }
   }
   traceout("List::evalList()");
   return nl;
}

inline class QoreNode *List::eval(ExceptionSink *xsink)
{
   tracein("List::eval()");
   class List *nl = new List();
   for (int i = 0; i < length; i++)
   {
      nl->push(entry[i] ? entry[i]->eval(xsink) : NULL);
      if (xsink->isEvent())
      {
	 nl->dereference(xsink);
	 delete nl;
	 traceout("List::eval()");
	 return NULL;
      }
   }
   traceout("List::eval()");
   return new QoreNode(nl);
}

inline class QoreNode *List::evalFrom(int offset, ExceptionSink *xsink)
{
   tracein("List::eval()");
   class List *nl = new List();
   for (int i = offset; i < length; i++)
   {
      nl->push(entry[i] ? entry[i]->eval(xsink) : NULL);
      if (xsink->isEvent())
      {
	 nl->dereference(xsink);
	 delete nl;
	 traceout("List::eval()");
	 return NULL;
      }
   }
   traceout("List::eval()");
   return new QoreNode(nl);
}

inline class QoreNode *List::copy()
{
   class List *nl = new List();
   for (int i = 0; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return new QoreNode(nl);
}

inline class List *List::copyListFrom(int offset)
{
   class List *nl = new List();
   for (int i = offset; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return nl;
}

inline class List *List::copyList()
{
   class List *nl = new List();
   for (int i = 0; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return nl;
}

inline void List::check_offset(int &offset)
{
   if (offset < 0)
   {
      offset = length + offset;
      if (offset < 0)
	 offset = 0;
   }
   else if (offset > length)
      offset = length;
}

inline void List::check_offset(int &offset, int &len)
{
   check_offset(offset);
   if (len < 0)
   {
      len = length + len - offset;
      if (len < 0)
	 len = 0;
   }
}

void List::splice(int offset, class ExceptionSink *xsink)
{
   check_offset(offset);
   if (offset == length)
      return;

   splice_intern(offset, length - offset, xsink);
}

void List::splice(int offset, int len, class ExceptionSink *xsink)
{
   check_offset(offset, len);
   if (offset == length)
      return;
   splice_intern(offset, len, xsink);
}

void List::splice(int offset, int len, class QoreNode *l, class ExceptionSink *xsink)
{
   check_offset(offset, len);
   splice_intern(offset, len, l, xsink);
}

inline class QoreNode *ListIterator::eval(class ExceptionSink *xsink)
{
   // List::eval_entry() checks if the offset is < 0 already
   return l->eval_entry(pos, xsink);
}

#endif
