/*
  List.cc

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
#include <qore/FunctionReference.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <qore/minitest.hpp>
#ifdef DEBUG_TESTS
#  include "tests/List_tests.cc"
#endif

#include <algorithm>

#define LIST_BLOCK 20
#define LIST_PAD   15

typedef ReferenceHolder<QoreNode> safe_qorenode_t;

void QoreList::check_offset(int &offset)
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

void QoreList::check_offset(int &offset, int &len)
{
   check_offset(offset);
   if (len < 0)
   {
      len = length + len - offset;
      if (len < 0)
	 len = 0;
   }
}

QoreList::QoreList()
{
   length = 0;
   allocated = 0;
   entry = NULL;
   needs_eval = 0;
}

QoreList::QoreList(bool i)
{
   length = 0;
   allocated = 0;
   entry = NULL;
   needs_eval = i;
}

QoreList::~QoreList()
{
   assert(!length);

   if (entry)
      free(entry);
}

class QoreNode *QoreList::retrieve_entry(int num) const
{
   if (num >= length || num < 0)
      return NULL;
   return entry[num];
}

int QoreList::getEntryAsInt(int num) const
{
   if (num >= length || num < 0 || !entry[num])
      return 0;
   return entry[num]->getAsInt();
}

QoreNode **QoreList::get_entry_ptr(int num)
{
   if (num >= length)
      resize(num + 1);
   return &entry[num];
}

QoreNode **QoreList::getExistingEntryPtr(int num)
{
   if (num >= length || num < 0)
      return NULL;
   return &entry[num];
}

void QoreList::set_entry(int index, class QoreNode *val, ExceptionSink *xsink)
{
   class QoreNode **v = get_entry_ptr(index);
   if (*v)
      (*v)->deref(xsink);
   *v = val;
}

class QoreNode *QoreList::eval_entry(int num, class ExceptionSink *xsink) const
{
   if (num >= length || num < 0)
      return NULL;
   class QoreNode *rv = entry[num];
   if (rv)
      rv = rv->eval(xsink);
   return rv;
}

void QoreList::push(class QoreNode *val)
{
   class QoreNode **v = get_entry_ptr(length);
   *v = val;
}

void QoreList::merge(class QoreList *list)
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

int QoreList::delete_entry(int ind, ExceptionSink *xsink)
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
void QoreList::pop_entry(int ind, ExceptionSink *xsink)
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

void QoreList::insert(class QoreNode *val)
{
   resize(length + 1);
   memmove(entry + 1, entry, sizeof(QoreNode *) * (length - 1));
   entry[0] = val;
}

QoreNode *QoreList::shift()
{
   if (!length)
      return NULL;
   QoreNode *rv = entry[0];
   memmove(entry, entry + 1, sizeof(QoreNode *) * (length - 1));
   entry[length - 1] = NULL;
   resize(length - 1);
   return rv;
}

QoreNode *QoreList::pop()
{
   if (!length)
      return NULL;
   QoreNode *rv = entry[length - 1];
   entry[length - 1] = NULL;
   resize(length - 1);
   return rv;
}

class QoreList *QoreList::evalList(ExceptionSink *xsink) const
{
   tracein("QoreList::eval()");
   class QoreList *nl = new QoreList();
   for (int i = 0; i < length; i++)
   {
      nl->push(entry[i] ? entry[i]->eval(xsink) : NULL);
      if (xsink->isEvent())
      {
	 nl->dereference(xsink);
	 delete nl;
	 traceout("QoreList::evalList()");
	 return NULL;
      }
   }
   traceout("QoreList::evalList()");
   return nl;
}

class QoreNode *QoreList::eval(ExceptionSink *xsink) const
{
   tracein("QoreList::eval()");
   class QoreList *nl = new QoreList();
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

class QoreNode *QoreList::evalFrom(int offset, ExceptionSink *xsink) const
{
   tracein("List::eval()");
   class QoreList *nl = new QoreList();
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

class QoreNode *QoreList::copy() const
{
   class QoreList *nl = new QoreList();
   for (int i = 0; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return new QoreNode(nl);
}

class QoreList *QoreList::copyListFrom(int offset) const
{
   class QoreList *nl = new QoreList();
   for (int i = offset; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return nl;
}

class QoreList *QoreList::copyList() const
{
   class QoreList *nl = new QoreList();
   for (int i = 0; i < length; i++)
      nl->push(entry[i] ? entry[i]->RefSelf() : NULL);

   return nl;
}

void QoreList::splice(int offset, class ExceptionSink *xsink)
{
   check_offset(offset);
   if (offset == length)
      return;

   splice_intern(offset, length - offset, xsink);
}

void QoreList::splice(int offset, int len, class ExceptionSink *xsink)
{
   check_offset(offset, len);
   if (offset == length)
      return;
   splice_intern(offset, len, xsink);
}

void QoreList::splice(int offset, int len, class QoreNode *l, class ExceptionSink *xsink)
{
   check_offset(offset, len);
   splice_intern(offset, len, l, xsink);
}

class QoreNode *ListIterator::eval(class ExceptionSink *xsink) const
{
   // QoreList::eval_entry() checks if the offset is < 0 already
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
   int rc = (int)OP_LOG_LT->bool_eval(l, r, &xsink);
   //printd(5, "compareListEntries() returning %d\n", rc);
   return rc;
}

static int compareListEntriesDescending(class QoreNode *l, class QoreNode *r)
{
   return compareListEntries(l, r) ? 0 : 1;
}

class QoreNode *QoreList::sort() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntries);
   return rv;
}

class QoreNode *QoreList::sortDescending() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntriesDescending);
   return rv;
}

class QoreNode *QoreList::sortDescending(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{   
   QoreNode *rv = copy();
   if (length)
      if (rv->val.list->qsort(fr, 0, length - 1, false, xsink))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
	 return rv;
}

class QoreNode *StackList::getAndClear(int i)
{
   if (i < 0 || i >= length)
      return NULL;
   class QoreNode *rv = entry[i];
   entry[i] = NULL;
   return rv;
}

static inline class QoreNode *do_args(QoreNode *e1, QoreNode *e2)
{
   class QoreList *l = new QoreList();
   e1->ref();
   l->push(e1);
   e2->ref();
   l->push(e2);
   return new QoreNode(l);
}

// mergesort for controlled and interruptible sorts (stable)
int QoreList::mergesort(const class AbstractFunctionReference *fr, bool ascending, class ExceptionSink *xsink)
{
   //printd(5, "List::mergesort() ENTER this=%08p, pgm=%08p, f=%08p length=%d\n", this, pgm, f, length);
   
   if (length <= 1)
      return 0;

   // separate list into two equal-sized lists
   StackList left(xsink), right(xsink);
   int mid = length / 2;
   {
      int i;
      for (i = 0; i < mid; i++)
	 left.push(entry[i]);
      for (; i < length; i++)
	 right.push(entry[i]);
   }

   // set length to 0 - the temporary lists own the entry references now
   length = 0;

   // mergesort the two lists
   if (left.mergesort(fr, ascending, xsink) || right.mergesort(fr, ascending, xsink))
      return -1;

   // merge the resulting lists
   // use offsets and StackList::getAndClear() to avoid moving a lot of memory around
   int li = 0, ri = 0;
   while ((li < left.length) && (ri < right.length))
   {
      class QoreNode *l = left.entry[li];
      class QoreNode *r = right.entry[ri];
      safe_qorenode_t args(do_args(l, r), xsink);
      safe_qorenode_t rv(fr->exec(*args, xsink), xsink);
      if (xsink->isEvent())
	 return -1;
      int rc = *rv ? rv->getAsInt() : 0;
      if ((ascending && rc <= 0)
	  || (!ascending && rc > 0))
	 push(left.getAndClear(li++));
      else
	 push(right.getAndClear(ri++));
   }

   // only one list will have entries left...
   while (li < left.length)
      push(left.getAndClear(li++));
   while (ri < right.length)
      push(right.getAndClear(ri++));

   //printd(5, "List::mergesort() EXIT this=%08p, length=%d\n", this, length);

   return 0;
}

// quicksort for controlled and interruptible sorts (unstable)
// I am so smart that I did not comment this code
int QoreList::qsort(const class AbstractFunctionReference *fr, int left, int right, bool ascending, class ExceptionSink *xsink)
{
   int l_hold = left;
   int r_hold = right;
   class QoreNode *pivot = entry[left];

   while (left < right)
   {
      while (true)
      {
	 safe_qorenode_t args(do_args(entry[right], pivot), xsink);
	 safe_qorenode_t rv(fr->exec(*args, xsink), xsink);
	 if (xsink->isEvent())
	    return -1;
	 int rc = *rv ? rv->getAsInt() : 0;
	 if ((left < right)
	     && ((rc >= 0 && ascending)
		 || (rc < 0 && !ascending)))
	    right--;
	 else
	    break;
      }

      if (left != right)
      {
	 entry[left] = entry[right];
	 left++;
      }

      while (true)
      {
	 safe_qorenode_t args(do_args(entry[left], pivot), xsink);
	 safe_qorenode_t rv(fr->exec(*args, xsink), xsink);
	 if (xsink->isEvent())
	    return -1;
	 int rc = *rv ? rv->getAsInt() : 0;
	 if ((left < right) 
	     && ((rc <= 0 && ascending)
		 || (rc > 0 && !ascending)))
	    left++;
	 else
	    break;
      }
      
      if (left != right)
      {
	 entry[right] = entry[left];
	 right--;
      }
   }
   entry[left] = pivot;
   int t_left = left;
   left = l_hold;
   right = r_hold;
   int rc = 0;
   if (left < t_left)
      rc = qsort(fr, left, t_left - 1, ascending, xsink);
   if (!rc && right > t_left)
      rc = qsort(fr, t_left + 1, right, ascending, xsink);
   return rc;
}

class QoreNode *QoreList::sort(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{   
   QoreNode *rv = copy();
   if (length)
      if (rv->val.list->qsort(fr, 0, length - 1, true, xsink))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
   return rv;
}

class QoreNode *QoreList::sortStable() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::stable_sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntries);
   return rv;
}

class QoreNode *QoreList::sortDescendingStable() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() entry=%08p length=%d\n", rv->val.list->entry, length);
   std::stable_sort(rv->val.list->entry, rv->val.list->entry + length, compareListEntriesDescending);
   return rv;
}

class QoreNode *QoreList::sortDescendingStable(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{   
   QoreNode *rv = copy();
   if (length)
      if (rv->val.list->mergesort(fr, false, xsink))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
   return rv;
}

class QoreNode *QoreList::sortStable(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{   
   QoreNode *rv = copy();
   if (length)
      if (rv->val.list->mergesort(fr, true, xsink))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
   return rv;
}

// does a deep dereference
void QoreList::deref_intern(ExceptionSink *xsink)
{
//   tracein("List::dereference()");
   for (int i = 0; i < length; i++)
      if (entry[i])
         entry[i]->deref(xsink);
//   traceout("List::dereference()");
}

void QoreList::dereference(ExceptionSink *xsink)
{
   deref_intern(xsink);
   length = 0;
}

void QoreList::derefAndDelete(class ExceptionSink *xsink)
{
   deref_intern(xsink);
#ifdef DEBUG
   length = 0;
#endif
   delete this;
}

void QoreList::resize(int num)
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
      int d = num >> 2;
      allocated = num + (d < LIST_PAD ? LIST_PAD : d);
      //allocated = num + LIST_PAD;
      entry = (class QoreNode **)realloc(entry, sizeof (QoreNode *) * allocated);
      for (int i = length; i < allocated; i++)
	 entry[i] = NULL;
   }
   length = num;
}

void QoreList::splice_intern(int offset, int len, class ExceptionSink *xsink)
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

void QoreList::splice_intern(int offset, int len, class QoreNode *l, class ExceptionSink *xsink)
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

int QoreList::size() const
{
   return length;
}

bool QoreList::needsEval() const
{
   return needs_eval;
}

void QoreList::clearNeedsEval()
{
   needs_eval = false;
}

class QoreNode *QoreList::min() const
{
   class QoreNode *rv = NULL;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   class ExceptionSink xsink;

   for (int i = 0; i < length; i++)
   {
      class QoreNode *v = entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 if (OP_LOG_LT->bool_eval(v, rv, &xsink))
	    rv = v;
	 assert(!xsink);
      }
   }
   return rv ? rv->RefSelf() : NULL;
}

class QoreNode *QoreList::max() const
{
   class QoreNode *rv = NULL;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   class ExceptionSink xsink;

   for (int i = 0; i < length; i++)
   {
      class QoreNode *v = entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 if (OP_LOG_GT->bool_eval(v, rv, &xsink))
	    rv = v;
	 assert(!xsink);
      }
   }
   return rv ? rv->RefSelf() : NULL;
}

class QoreNode *QoreList::min(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{
   class QoreNode *rv = NULL;

   for (int i = 0; i < length; i++)
   {
      class QoreNode *v = entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 safe_qorenode_t args(do_args(v, rv), xsink);
	 safe_qorenode_t result(fr->exec(*args, xsink), xsink);
	 if (xsink->isEvent())
	    return NULL;
	 if (*result ? result->getAsInt() < 0 : false)
	    rv = v;
      }
   }
   return rv ? rv->RefSelf() : NULL;
}

class QoreNode *QoreList::max(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{
   class QoreNode *rv = NULL;

   for (int i = 0; i < length; i++)
   {
      class QoreNode *v = entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 safe_qorenode_t args(do_args(v, rv), xsink);
	 safe_qorenode_t result(fr->exec(*args, xsink), xsink);
	 if (xsink->isEvent())
	    return NULL;
	 if (*result ? result->getAsInt() > 0 : false)
	    rv = v;
      }
   }
   return rv ? rv->RefSelf() : NULL;
}

class QoreList *QoreList::reverse() const
{
   class QoreList *l = new QoreList();
   l->resize(length);
   for (int i = 0; i < length; ++i)
   {
      class QoreNode *n = entry[length - i - 1];
      l->entry[i] = n ? n->RefSelf() : NULL;
   }
   return l;
}

ListIterator::ListIterator(class QoreList *lst) : l(lst), pos(-1)
{ 
}

bool ListIterator::next() 
{
   if (l->size() == 0) return false; // empty
   if (++pos >= l->size()) return false; // finished
   return true;
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

bool ListIterator::first() const
{
   return !pos; 
} 
