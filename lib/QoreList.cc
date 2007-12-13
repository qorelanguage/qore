/*
  QoreList.cc

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
#include <qore/intern/FunctionReference.h>

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

struct qore_list_private {
      class QoreNode **entry;
      int length;
      int allocated;
      bool needs_eval;

      DLLLOCAL qore_list_private()
      {
	 length = 0;
	 allocated = 0;
	 entry = 0;
      }
      DLLLOCAL ~qore_list_private()
      {
	 assert(!length);

	 if (entry)
	    free(entry);
      }
};

void QoreList::check_offset(int &offset)
{
   if (offset < 0)
   {
      offset = priv->length + offset;
      if (offset < 0)
	 offset = 0;
   }
   else if (offset > priv->length)
      offset = priv->length;
}

void QoreList::check_offset(int &offset, int &len)
{
   check_offset(offset);
   if (len < 0)
   {
      len = priv->length + len - offset;
      if (len < 0)
	 len = 0;
   }
}

QoreList::QoreList() : priv(new qore_list_private)
{
   priv->needs_eval = 0;
}

QoreList::QoreList(bool i) : priv(new qore_list_private)
{
   priv->needs_eval = i;
}

QoreList::~QoreList()
{
   delete priv;
}

class QoreNode *QoreList::retrieve_entry(int num) const
{
   if (num >= priv->length || num < 0)
      return NULL;
   return priv->entry[num];
}

int QoreList::getEntryAsInt(int num) const
{
   if (num >= priv->length || num < 0 || !priv->entry[num])
      return 0;
   return priv->entry[num]->getAsInt();
}

QoreNode **QoreList::get_entry_ptr(int num)
{
   if (num >= priv->length)
      resize(num + 1);
   return &priv->entry[num];
}

QoreNode **QoreList::getExistingEntryPtr(int num)
{
   if (num >= priv->length || num < 0)
      return NULL;
   return &priv->entry[num];
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
   if (num >= priv->length || num < 0)
      return NULL;
   class QoreNode *rv = priv->entry[num];
   if (rv)
      rv = rv->eval(xsink);
   return rv;
}

void QoreList::push(class QoreNode *val)
{
   class QoreNode **v = get_entry_ptr(priv->length);
   *v = val;
}

void QoreList::merge(class QoreList *list)
{
   int start = priv->length;
   resize(priv->length + list->priv->length);
   for (int i = 0; i < list->priv->length; i++)
   {
      if (list->priv->entry[i])
	 priv->entry[start + i] = list->priv->entry[i]->RefSelf();
      else
	 priv->entry[start + i] = NULL;
   }
}

int QoreList::delete_entry(int ind, ExceptionSink *xsink)
{
   if (ind >= priv->length || ind < 0)
      return 1;

   if (priv->entry[ind] && priv->entry[ind]->type == NT_OBJECT)
      priv->entry[ind]->val.object->doDelete(xsink);

   if (priv->entry[ind])
   {
      priv->entry[ind]->deref(xsink);
      priv->entry[ind] = NULL;
   }

   // resize list if deleting last element
   if (ind == (priv->length - 1))
      resize(ind);

   return 0;
}

// delete an priv->entry and move down the rest of the entries
void QoreList::pop_entry(int ind, ExceptionSink *xsink)
{
   if (ind >= priv->length || ind < 0)
      return;

   if (priv->entry[ind] && priv->entry[ind]->type == NT_OBJECT)
      priv->entry[ind]->val.object->doDelete(xsink);

   if (priv->entry[ind])
   {
      priv->entry[ind]->deref(xsink);
      priv->entry[ind] = NULL;
   }

   // resize list
   priv->length--;
   if (ind < priv->length)
      memmove(priv->entry + ind, priv->entry + ind + 1, sizeof(priv->entry) * (priv->length - ind));
   resize(priv->length);
}

void QoreList::insert(class QoreNode *val)
{
   resize(priv->length + 1);
   memmove(priv->entry + 1, priv->entry, sizeof(QoreNode *) * (priv->length - 1));
   priv->entry[0] = val;
}

QoreNode *QoreList::shift()
{
   if (!priv->length)
      return NULL;
   QoreNode *rv = priv->entry[0];
   memmove(priv->entry, priv->entry + 1, sizeof(QoreNode *) * (priv->length - 1));
   priv->entry[priv->length - 1] = NULL;
   resize(priv->length - 1);
   return rv;
}

QoreNode *QoreList::pop()
{
   if (!priv->length)
      return NULL;
   QoreNode *rv = priv->entry[priv->length - 1];
   priv->entry[priv->length - 1] = NULL;
   resize(priv->length - 1);
   return rv;
}

class QoreList *QoreList::evalList(ExceptionSink *xsink) const
{
   tracein("QoreList::eval()");
   class QoreList *nl = new QoreList();
   for (int i = 0; i < priv->length; i++)
   {
      nl->push(priv->entry[i] ? priv->entry[i]->eval(xsink) : NULL);
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
   for (int i = 0; i < priv->length; i++)
   {
      nl->push(priv->entry[i] ? priv->entry[i]->eval(xsink) : NULL);
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
   for (int i = offset; i < priv->length; i++)
   {
      nl->push(priv->entry[i] ? priv->entry[i]->eval(xsink) : NULL);
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
   for (int i = 0; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->RefSelf() : NULL);

   return new QoreNode(nl);
}

class QoreList *QoreList::copyListFrom(int offset) const
{
   class QoreList *nl = new QoreList();
   for (int i = offset; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->RefSelf() : NULL);

   return nl;
}

class QoreList *QoreList::copyList() const
{
   class QoreList *nl = new QoreList();
   for (int i = 0; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->RefSelf() : NULL);

   return nl;
}

void QoreList::splice(int offset, class ExceptionSink *xsink)
{
   check_offset(offset);
   if (offset == priv->length)
      return;

   splice_intern(offset, priv->length - offset, xsink);
}

void QoreList::splice(int offset, int len, class ExceptionSink *xsink)
{
   check_offset(offset, len);
   if (offset == priv->length)
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
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->val.list->priv->entry, priv->length);
   std::sort(rv->val.list->priv->entry, rv->val.list->priv->entry + priv->length, compareListEntries);
   return rv;
}

class QoreNode *QoreList::sortDescending() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->val.list->priv->entry, priv->length);
   std::sort(rv->val.list->priv->entry, rv->val.list->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

class QoreNode *QoreList::sortDescending(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{   
   QoreNode *rv = copy();
   if (priv->length)
      if (rv->val.list->qsort(fr, 0, priv->length - 1, false, xsink))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
	 return rv;
}

class QoreNode *StackList::getAndClear(int i)
{
   if (i < 0 || i >= priv->length)
      return NULL;
   class QoreNode *rv = priv->entry[i];
   priv->entry[i] = NULL;
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
   //printd(5, "List::mergesort() ENTER this=%08p, pgm=%08p, f=%08p priv->length=%d\n", this, pgm, f, priv->length);
   
   if (priv->length <= 1)
      return 0;

   // separate list into two equal-sized lists
   StackList left(xsink), right(xsink);
   int mid = priv->length / 2;
   {
      int i;
      for (i = 0; i < mid; i++)
	 left.push(priv->entry[i]);
      for (; i < priv->length; i++)
	 right.push(priv->entry[i]);
   }

   // set priv->length to 0 - the temporary lists own the priv->entry references now
   priv->length = 0;

   // mergesort the two lists
   if (left.mergesort(fr, ascending, xsink) || right.mergesort(fr, ascending, xsink))
      return -1;

   // merge the resulting lists
   // use offsets and StackList::getAndClear() to avoid moving a lot of memory around
   int li = 0, ri = 0;
   while ((li < left.priv->length) && (ri < right.priv->length))
   {
      class QoreNode *l = left.priv->entry[li];
      class QoreNode *r = right.priv->entry[ri];
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
   while (li < left.priv->length)
      push(left.getAndClear(li++));
   while (ri < right.priv->length)
      push(right.getAndClear(ri++));

   //printd(5, "List::mergesort() EXIT this=%08p, priv->length=%d\n", this, priv->length);

   return 0;
}

// quicksort for controlled and interruptible sorts (unstable)
// I am so smart that I did not comment this code
int QoreList::qsort(const class AbstractFunctionReference *fr, int left, int right, bool ascending, class ExceptionSink *xsink)
{
   int l_hold = left;
   int r_hold = right;
   class QoreNode *pivot = priv->entry[left];

   while (left < right)
   {
      while (true)
      {
	 safe_qorenode_t args(do_args(priv->entry[right], pivot), xsink);
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
	 priv->entry[left] = priv->entry[right];
	 left++;
      }

      while (true)
      {
	 safe_qorenode_t args(do_args(priv->entry[left], pivot), xsink);
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
	 priv->entry[right] = priv->entry[left];
	 right--;
      }
   }
   priv->entry[left] = pivot;
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
   if (priv->length)
      if (rv->val.list->qsort(fr, 0, priv->length - 1, true, xsink))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
   return rv;
}

class QoreNode *QoreList::sortStable() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->val.list->priv->entry, priv->length);
   std::stable_sort(rv->val.list->priv->entry, rv->val.list->priv->entry + priv->length, compareListEntries);
   return rv;
}

class QoreNode *QoreList::sortDescendingStable() const
{
   class QoreNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->val.list->priv->entry, priv->length);
   std::stable_sort(rv->val.list->priv->entry, rv->val.list->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

class QoreNode *QoreList::sortDescendingStable(const class AbstractFunctionReference *fr, class ExceptionSink *xsink) const
{   
   QoreNode *rv = copy();
   if (priv->length)
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
   if (priv->length)
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
   for (int i = 0; i < priv->length; i++)
      if (priv->entry[i])
         priv->entry[i]->deref(xsink);
//   traceout("List::dereference()");
}

void QoreList::dereference(ExceptionSink *xsink)
{
   deref_intern(xsink);
   priv->length = 0;
}

void QoreList::derefAndDelete(class ExceptionSink *xsink)
{
   deref_intern(xsink);
#ifdef DEBUG
   priv->length = 0;
#endif
   delete this;
}

void QoreList::resize(int num)
{
   if (num < priv->length) // make smaller
   {
      ExceptionSink xsink;
      for (int i = priv->length - 1; i >= num; i--)
	 if (priv->entry[i])
	 {
	    // resize should only be called when the missing
	    // entries are already NULL
	    priv->entry[i]->deref(&xsink);
	    priv->entry[i] = NULL;
	 }
      //priv->entry = (class QoreNode **)realloc(priv->entry, sizeof (QoreNode **) * num);
      priv->length = num;
      return;
   }
   // make larger
   if (num >= priv->allocated)
   {
      int d = num >> 2;
      priv->allocated = num + (d < LIST_PAD ? LIST_PAD : d);
      //priv->allocated = num + LIST_PAD;
      priv->entry = (class QoreNode **)realloc(priv->entry, sizeof (QoreNode *) * priv->allocated);
      for (int i = priv->length; i < priv->allocated; i++)
	 priv->entry[i] = NULL;
   }
   priv->length = num;
}

void QoreList::splice_intern(int offset, int len, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, len=%d, priv->length=%d)\n", offset, len, priv->length);
   int end;
   if (len > (priv->length - offset))
   {
      end = priv->length;
      len = priv->length - offset;
   }
   else
      end = offset + len;

   // dereference all entries that will be removed
   for (int i = offset; i < end; i++)
      if (priv->entry[i])
	 priv->entry[i]->deref(xsink);

   // move down entries if necessary
   if (end != priv->length)
   {
      memmove(priv->entry + offset, priv->entry + end, sizeof(priv->entry) * (priv->length - end));
      // zero out trailing entries
      for (int i = priv->length - len; i < priv->length; i++)
	 priv->entry[i] = NULL;
   }
   else // set last priv->entry to NULL
      priv->entry[end - 1] = NULL;
      
   resize(priv->length - len);
}

void QoreList::splice_intern(int offset, int len, class QoreNode *l, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, len=%d, priv->length=%d)\n", offset, len, priv->length);
   int end;
   if (len > (priv->length - offset))
   {
      end = priv->length;
      len = priv->length - offset;
   }
   else
      end = offset + len;

   // dereference all entries that will be removed
   for (int i = offset; i < end; i++)
      if (priv->entry[i])
	 priv->entry[i]->deref(xsink);

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
      int ol = priv->length;
      resize(priv->length - len + n);
      // move trailing entries forward if necessary
      if (end != ol)
	 memmove(priv->entry + (end - len + n), priv->entry + end, sizeof(priv->entry) * (ol - end));
   }
   else if (len > n) // make list smaller
   {
      memmove(priv->entry + offset + n, priv->entry + offset + len, sizeof(priv->entry) * (priv->length - offset - n));
      // zero out trailing entries
      for (int i = priv->length - (len - n); i < priv->length; i++)
	 priv->entry[i] = NULL;
      // resize list
      resize(priv->length - (len - n));
   }

   // add in new entries
   if (!l || l->type != NT_LIST)
   {
      if (l)
	 l->ref();
      priv->entry[offset] = l;
   }
   else
      for (int i = 0; i < n; i++)
	 priv->entry[offset + i] = l->val.list->retrieve_entry(i) ? l->val.list->retrieve_entry(i)->RefSelf() : NULL;
}

int QoreList::size() const
{
   return priv->length;
}

bool QoreList::needsEval() const
{
   return priv->needs_eval;
}

void QoreList::clearNeedsEval()
{
   priv->needs_eval = false;
}

class QoreNode *QoreList::min() const
{
   class QoreNode *rv = NULL;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   class ExceptionSink xsink;

   for (int i = 0; i < priv->length; i++)
   {
      class QoreNode *v = priv->entry[i];

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

   for (int i = 0; i < priv->length; i++)
   {
      class QoreNode *v = priv->entry[i];

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

   for (int i = 0; i < priv->length; i++)
   {
      class QoreNode *v = priv->entry[i];

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

   for (int i = 0; i < priv->length; i++)
   {
      class QoreNode *v = priv->entry[i];

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
   l->resize(priv->length);
   for (int i = 0; i < priv->length; ++i)
   {
      class QoreNode *n = priv->entry[priv->length - i - 1];
      l->priv->entry[i] = n ? n->RefSelf() : NULL;
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
