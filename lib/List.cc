/*
  List.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols
  
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
#include <qore/Qore.h>
#include <qore/List.h>
#include <qore/support.h>
#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/QoreType.h>
#include <qore/Object.h>
#include <qore/Exception.h>
#ifdef DEBUG
#include <qore/support.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <qore/minitest.hpp>
#ifdef DEBUG
#  include "tests/List_tests.cc"
#endif

#include <algorithm>
using namespace std;

#define LIST_BLOCK 20
#define LIST_PAD   15

void List::check_offset(int &offset)
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

void List::check_offset(int &offset, int &len)
{
   check_offset(offset);
   if (len < 0)
   {
      len = length + len - offset;
      if (len < 0)
	 len = 0;
   }
}

List::List()
{
   length = 0;
   allocated = 0;
   entry = NULL;
   needs_eval = 0;
}

List::List(bool i)
{
   length = 0;
   allocated = 0;
   entry = NULL;
   needs_eval = i;
}

List::~List()
{
#ifdef DEBUG
   if (length)
      run_time_error("List::~List() %08p not empty! elements=%d entry=%08p\n", this, length, entry);
#endif
   if (entry)
      free(entry);
}

class QoreNode *List::retrieve_entry(int num) const
{
   if (num >= length || num < 0)
      return NULL;
   return entry[num];
}

int List::getEntryAsInt(int num) const
{
   if (num >= length || num < 0 || !entry[num])
      return 0;
   return entry[num]->getAsInt();
}

QoreNode **List::get_entry_ptr(int num)
{
   if (num >= length)
      resize(num + 1);
   return &entry[num];
}

QoreNode **List::getExistingEntryPtr(int num)
{
   if (num >= length || num < 0)
      return NULL;
   return &entry[num];
}

void List::set_entry(int index, class QoreNode *val, ExceptionSink *xsink)
{
   class QoreNode **v = get_entry_ptr(index);
   if (*v)
      (*v)->deref(xsink);
   *v = val;
}

class QoreNode *List::eval_entry(int num, class ExceptionSink *xsink) const
{
   if (num >= length || num < 0)
      return NULL;
   class QoreNode *rv = entry[num];
   if (rv)
      rv = rv->eval(xsink);
   return rv;
}

void List::push(class QoreNode *val)
{
   class QoreNode **v = get_entry_ptr(length);
   *v = val;
}

void List::merge(class List *list)
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

int List::delete_entry(int ind, ExceptionSink *xsink)
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
void List::pop_entry(int ind, ExceptionSink *xsink)
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

void List::insert(class QoreNode *val)
{
   resize(length + 1);
   memmove(entry + 1, entry, sizeof(QoreNode *) * (length - 1));
   entry[0] = val;
}

QoreNode *List::shift()
{
   if (!length)
      return NULL;
   QoreNode *rv = entry[0];
   memmove(entry, entry + 1, sizeof(QoreNode *) * (length - 1));
   entry[length - 1] = NULL;
   resize(length - 1);
   return rv;
}

QoreNode *List::pop()
{
   if (!length)
      return NULL;
   QoreNode *rv = entry[length - 1];
   entry[length - 1] = NULL;
   resize(length - 1);
   return rv;
}

class List *List::evalList(ExceptionSink *xsink) const
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

class QoreNode *List::eval(ExceptionSink *xsink) const
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

class QoreNode *List::evalFrom(int offset, ExceptionSink *xsink) const
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

class QoreNode *List::copy() const
{
   class List *nl = new List();
   for (int i = 0; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return new QoreNode(nl);
}

class List *List::copyListFrom(int offset) const
{
   class List *nl = new List();
   for (int i = offset; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return nl;
}

class List *List::copyList() const
{
   class List *nl = new List();
   for (int i = 0; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return nl;
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

class QoreNode *ListIterator::eval(class ExceptionSink *xsink) const
{
   // List::eval_entry() checks if the offset is < 0 already
   return l->eval_entry(pos, xsink);
}

static int compareListEntries(class QoreNode *l, class QoreNode *r)
{
   //printd(5, "compareListEntries(%08p, %08p) (%s %s)\n", l, r, l->type == NT_STRING ? l->val.String->getBuffer() : "?", r->type == NT_STRING ? r->val.String->getBuffer() : "?");

   // sort non-existant values last
   if (is_nothing(l))
      return 0;
   if (is_nothing(r))
      return 1;

   class ExceptionSink xsink;
   class QoreNode *rv = OP_LOG_LT->eval(l, r, &xsink);
   int rc = (int)rv->val.boolval;
   rv->deref(NULL);
   //printd(5, "compareListEntries() returning %d\n", rc);
   return rc;
}

static int compareListEntriesDescending(class QoreNode *l, class QoreNode *r)
{
   return compareListEntries(l, r) ? 0 : 1;
}

class QoreNode *List::sort() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntries);
   return rv;
}

class QoreNode *List::sortDescending() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntriesDescending);
   return rv;
}

// does a deep dereference
void List::deref_intern(ExceptionSink *xsink)
{
//   tracein("List::dereference()");
   for (int i = 0; i < length; i++)
      if (entry[i])
         entry[i]->deref(xsink);
//   traceout("List::dereference()");
}

void List::dereference(ExceptionSink *xsink)
{
   deref_intern(xsink);
   length = 0;
}

void List::derefAndDelete(class ExceptionSink *xsink)
{
   deref_intern(xsink);
#ifdef DEBUG
   length = 0;
#endif
   delete this;
}

void List::resize(int num)
{
   if (num < length) // make smaller
   {
      ExceptionSink xsink;
      for (int i = length - 1; i >= num; i--)
	 if (entry[i])
	 {
	    // resize should only be called when the missing
	    // entries are already NULL
	    entry[i]->deref(&xsink);
	    entry[i] = NULL;
	 }
      //entry = (class QoreNode **)realloc(entry, sizeof (QoreNode **) * num);
      length = num;
      return;
   }
   // make larger
   if (num >= allocated)
   {
      allocated = num + LIST_PAD;
      entry = (class QoreNode **)realloc(entry, sizeof (QoreNode *) * allocated);
      for (int i = length; i < allocated; i++)
	 entry[i] = NULL;
   }
   length = num;
}

void List::splice_intern(int offset, int len, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, len=%d, length=%d)\n", offset, len, length);
   int end;
   if (len > (length - offset))
   {
      end = length;
      len = length - offset;
   }
   else
      end = offset + len;

   // dereference all entries that will be removed
   for (int i = offset; i < end; i++)
      if (entry[i])
	 entry[i]->deref(xsink);

   // move down entries if necessary
   if (end != length)
   {
      memmove(entry + offset, entry + end, sizeof(entry) * (length - end));
      // zero out trailing entries
      for (int i = length - len; i < length; i++)
	 entry[i] = NULL;
   }
   else // set last entry to NULL
      entry[end - 1] = NULL;
      
   resize(length - len);
}

void List::splice_intern(int offset, int len, class QoreNode *l, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, len=%d, length=%d)\n", offset, len, length);
   int end;
   if (len > (length - offset))
   {
      end = length;
      len = length - offset;
   }
   else
      end = offset + len;

   // dereference all entries that will be removed
   for (int i = offset; i < end; i++)
      if (entry[i])
	 entry[i]->deref(xsink);

   // get number of entries to insert
   int n;
   if (!l)
      n = 1;
   else if (l->type == NT_LIST)
      n = l->val.list->size();
   else
      n = 1;
   // difference
   if (n > len) // make bigger
   {
      int ol = length;
      resize(length - len + n);
      // move trailing entries forward if necessary
      if (end != ol)
	 memmove(entry + (end - len + n), entry + end, sizeof(entry) * (ol - end));
   }
   else if (len > n) // make list smaller
   {
      memmove(entry + offset + n, entry + offset + len, sizeof(entry) * (length - offset - n));
      // zero out trailing entries
      for (int i = length - (len - n); i < length; i++)
	 entry[i] = NULL;
      // resize list
      resize(length - (len - n));
   }

   // add in new entries
   if (!l || l->type != NT_LIST)
   {
      if (l)
	 l->ref();
      entry[offset] = l;
   }
   else
      for (int i = 0; i < n; i++)
	 entry[offset + i] = l->val.list->retrieve_entry(i) ? l->val.list->retrieve_entry(i)->RefSelf() : NULL;
}

int List::size() const
{
   return length;
}

bool List::needsEval() const
{
   return needs_eval;
}

void List::clearNeedsEval()
{
   needs_eval = false;
}

ListIterator::ListIterator(class List *lst) 
{ 
   l = lst; 
   pos = -1; 
}

bool ListIterator::next() 
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

class QoreNode *ListIterator::getValue() const
{
   if (pos < 0)
      return NULL;
   return l->retrieve_entry(pos);
}

class QoreNode **ListIterator::getValuePtr() const
{
   if (pos < 0)
      return NULL;
   return l->get_entry_ptr(pos);
}

bool ListIterator::last() const
{ 
   return (bool)(pos == (l->size() - 1)); 
} 
