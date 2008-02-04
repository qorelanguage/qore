/*
  QoreListNode.cc

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

typedef ReferenceHolder<QoreListNode> safe_qorelist_t;

struct qore_list_private {
      AbstractQoreNode **entry;
      int length;
      int allocated;
      // FIXME: combine needs_eval, finalized, and vlist into a single char attribute
      bool needs_eval, finalized, vlist;

      DLLLOCAL qore_list_private(bool n_needs_eval)
      {
	 entry = 0;
	 length = 0;
	 allocated = 0;
	 needs_eval = n_needs_eval;
	 finalized = false;
	 vlist = false;
      }
      DLLLOCAL ~qore_list_private()
      {
	 assert(!length);

	 if (entry)
	    free(entry);
      }
};

void QoreListNode::check_offset(int &offset)
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

void QoreListNode::check_offset(int &offset, int &len)
{
   check_offset(offset);
   if (len < 0)
   {
      len = priv->length + len - offset;
      if (len < 0)
	 len = 0;
   }
}

QoreListNode::QoreListNode() : AbstractQoreNode(NT_LIST), priv(new qore_list_private(false))
{
}

QoreListNode::QoreListNode(bool i) : AbstractQoreNode(NT_LIST), priv(new qore_list_private(i))
{
}

QoreListNode::~QoreListNode()
{
   delete priv;
}

// default implementation returns false
bool QoreListNode::needs_eval() const
{
   return priv->needs_eval;
}

AbstractQoreNode *QoreListNode::realCopy() const
{
   return copy();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
/*
DLLLOCAL virtual int compare(const AbstractQoreNode *val) const
{
}
*/

bool QoreListNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const QoreListNode *l = dynamic_cast<const QoreListNode *>(v);
   if (!l)
      return false;

   if (l->size() != size())
      return false;
   for (int i = 0; i < l->size(); i++)
      if (compareSoft(l->retrieve_entry(i), retrieve_entry(i), xsink) || *xsink)
         return false;
   return true;
}

bool QoreListNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const QoreListNode *l = dynamic_cast<const QoreListNode *>(v);
   if (!l)
      return false;

   if (l->size() != size())
      return false;
   for (int i = 0; i < l->size(); i++)
      if (compareHard(l->retrieve_entry(i), retrieve_entry(i), xsink) || *xsink)
         return false;
   return true;
}

// returns the data type
const QoreType *QoreListNode::getType() const
{
   return NT_LIST;
}

// returns the type name as a c string
const char *QoreListNode::getTypeName() const
{
   return "list";
}

// eval(): return value requires a deref(xsink) if needs_deref is true
// default implementation = needs_deref = false, returns "this"
// note: do not use this function directly, use the QoreNodeEvalOptionalRefHolder class instead
AbstractQoreNode *QoreListNode::eval(bool &needs_deref, ExceptionSink *xsink) const
{
   return evalList(needs_deref, xsink);
}

// deletes the object when the reference count = 0
void QoreListNode::deref(ExceptionSink *xsink)
{
   //printd(5, "QoreListNode::deref() this=%08p %d -> %d\n", this, reference_count(), reference_count() - 1);
   if (ROdereference()) {
      deref_intern(xsink);
      delete this;
   }
}

bool QoreListNode::is_value() const
{
   return !priv->needs_eval;
}

const AbstractQoreNode *QoreListNode::retrieve_entry(int num) const
{
   if (num >= priv->length || num < 0)
      return NULL;
   return priv->entry[num];
}

AbstractQoreNode *QoreListNode::retrieve_entry(int num)
{
   if (num >= priv->length || num < 0)
      return NULL;
   return priv->entry[num];
}

int QoreListNode::getEntryAsInt(int num) const
{
   if (num >= priv->length || num < 0 || !priv->entry[num])
      return 0;
   return priv->entry[num]->getAsInt();
}

AbstractQoreNode **QoreListNode::get_entry_ptr(int num)
{
   if (num >= priv->length)
      resize(num + 1);
   return &priv->entry[num];
}

AbstractQoreNode **QoreListNode::getExistingEntryPtr(int num)
{
   if (num >= priv->length || num < 0)
      return NULL;
   return &priv->entry[num];
}

void QoreListNode::set_entry(int index, AbstractQoreNode *val, ExceptionSink *xsink)
{
   AbstractQoreNode **v = get_entry_ptr(index);
   if (*v)
      (*v)->deref(xsink);
   *v = val;
}

AbstractQoreNode *QoreListNode::eval_entry(int num, ExceptionSink *xsink) const
{
   if (num >= priv->length || num < 0)
      return NULL;
   AbstractQoreNode *rv = priv->entry[num];
   if (rv)
      rv = rv->eval(xsink);
   return rv;
}

void QoreListNode::push(AbstractQoreNode *val)
{
   AbstractQoreNode **v = get_entry_ptr(priv->length);
   *v = val;
}

void QoreListNode::merge(const QoreListNode *list)
{
   int start = priv->length;
   resize(priv->length + list->priv->length);
   for (int i = 0; i < list->priv->length; i++)
   {
      if (list->priv->entry[i])
	 priv->entry[start + i] = list->priv->entry[i]->refSelf();
      else
	 priv->entry[start + i] = NULL;
   }
}

int QoreListNode::delete_entry(int ind, ExceptionSink *xsink)
{
   if (ind >= priv->length || ind < 0)
      return 1;

   QoreObject *o = dynamic_cast<QoreObject *>(priv->entry[ind]);
   if (o) 
      o->doDelete(xsink);

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
void QoreListNode::pop_entry(int ind, ExceptionSink *xsink)
{
   if (ind >= priv->length || ind < 0)
      return;

   QoreObject *o = dynamic_cast<QoreObject *>(priv->entry[ind]);
   if (o) 
      o->doDelete(xsink);

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

void QoreListNode::insert(AbstractQoreNode *val)
{
   resize(priv->length + 1);
   if (priv->length - 1)
      memmove(priv->entry + 1, priv->entry, sizeof(AbstractQoreNode *) * (priv->length - 1));
   priv->entry[0] = val;
}

AbstractQoreNode *QoreListNode::shift()
{
   if (!priv->length)
      return 0;
   AbstractQoreNode *rv = priv->entry[0];
   memmove(priv->entry, priv->entry + 1, sizeof(AbstractQoreNode *) * (priv->length - 1));
   priv->entry[priv->length - 1] = NULL;
   resize(priv->length - 1);
   return rv;
}

AbstractQoreNode *QoreListNode::pop()
{
   if (!priv->length)
      return NULL;
   AbstractQoreNode *rv = priv->entry[priv->length - 1];
   priv->entry[priv->length - 1] = NULL;
   resize(priv->length - 1);
   return rv;
}

AbstractQoreNode *QoreListNode::eval(ExceptionSink *xsink) const
{
   return evalList(xsink);
}

QoreListNode *QoreListNode::evalList(ExceptionSink *xsink) const
{
   if (!priv->needs_eval) {
      ref();
      return const_cast<QoreListNode *>(this);
   }

   ReferenceHolder<QoreListNode> nl(new QoreListNode(), xsink);
   for (int i = 0; i < priv->length; i++)
   {
      nl->push(priv->entry[i] ? priv->entry[i]->eval(xsink) : NULL);
      if (*xsink)
	 return 0;
   }
   return nl.release();
}

QoreListNode *QoreListNode::evalList(bool &needs_deref, ExceptionSink *xsink) const
{
   if (!priv->needs_eval) {
      needs_deref = false;
      return const_cast<QoreListNode *>(this);
   }
   QoreListNode *rv = evalList(xsink);
   if (rv)
      needs_deref = true;
   return rv;
}

QoreListNode *QoreListNode::evalFrom(int offset, ExceptionSink *xsink) const
{
   ReferenceHolder<QoreListNode> nl(new QoreListNode(), xsink);
   for (int i = offset; i < priv->length; i++)
   {
      nl->push(priv->entry[i] ? priv->entry[i]->eval(xsink) : NULL);
      if (*xsink)
	 return 0;
   }
   return nl.release();
}

QoreListNode *QoreListNode::copy() const
{
   QoreListNode *nl = new QoreListNode();
   for (int i = 0; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->refSelf() : NULL);

   return nl;
}

QoreListNode *QoreListNode::copyListFrom(int offset) const
{
   QoreListNode *nl = new QoreListNode();
   for (int i = offset; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->refSelf() : NULL);

   return nl;
}

void QoreListNode::splice(int offset, ExceptionSink *xsink)
{
   check_offset(offset);
   if (offset == priv->length)
      return;

   splice_intern(offset, priv->length - offset, xsink);
}

void QoreListNode::splice(int offset, int len, ExceptionSink *xsink)
{
   check_offset(offset, len);
   if (offset == priv->length)
      return;
   splice_intern(offset, len, xsink);
}

void QoreListNode::splice(int offset, int len, const AbstractQoreNode *l, ExceptionSink *xsink)
{
   check_offset(offset, len);
   splice_intern(offset, len, l, xsink);
}

static int compareListEntries(AbstractQoreNode *l, AbstractQoreNode *r)
{
   //printd(5, "compareListEntries(%08p, %08p) (%s %s)\n", l, r, l->type == NT_STRING ? l->val.String->getBuffer() : "?", r->type == NT_STRING ? r->val.String->getBuffer() : "?");

   // sort non-existant values last
   if (is_nothing(l))
      return 0;
   if (is_nothing(r))
      return 1;

   ExceptionSink xsink;
   int rc = (int)OP_LOG_LT->bool_eval(l, r, &xsink);
   //printd(5, "compareListEntries() returning %d\n", rc);
   return rc;
}

static int compareListEntriesDescending(AbstractQoreNode *l, AbstractQoreNode *r)
{
   return compareListEntries(l, r) ? 0 : 1;
}

QoreListNode *QoreListNode::sort() const
{
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->priv->entry, priv->length);
   std::sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntries);
   return rv;
}

QoreListNode *QoreListNode::sortDescending() const
{
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->priv->entry, priv->length);
   std::sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

QoreListNode *QoreListNode::sortDescending(const ResolvedFunctionReferenceNode *fr, ExceptionSink *xsink) const
{   
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->qsort(fr, 0, priv->length - 1, false, xsink))
	 return 0;

   return rv.release();
}

AbstractQoreNode *StackList::getAndClear(int i)
{
   if (i < 0 || i >= priv->length)
      return NULL;
   AbstractQoreNode *rv = priv->entry[i];
   priv->entry[i] = NULL;
   return rv;
}

static inline QoreListNode *do_args(AbstractQoreNode *e1, AbstractQoreNode *e2)
{
   QoreListNode *l = new QoreListNode();
   e1->ref();
   l->push(e1);
   e2->ref();
   l->push(e2);
   return l;
}

// mergesort for controlled and interruptible sorts (stable)
int QoreListNode::mergesort(const ResolvedFunctionReferenceNode *fr, bool ascending, ExceptionSink *xsink)
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
      AbstractQoreNode *l = left.priv->entry[li];
      AbstractQoreNode *r = right.priv->entry[ri];
      safe_qorelist_t args(do_args(l, r), xsink);
      ReferenceHolder<AbstractQoreNode> rv(fr->exec(*args, xsink), xsink);
      if (*xsink)
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
int QoreListNode::qsort(const ResolvedFunctionReferenceNode *fr, int left, int right, bool ascending, ExceptionSink *xsink)
{
   int l_hold = left;
   int r_hold = right;
   AbstractQoreNode *pivot = priv->entry[left];

   while (left < right)
   {
      while (true)
      {
	 safe_qorelist_t args(do_args(priv->entry[right], pivot), xsink);
	 ReferenceHolder<AbstractQoreNode> rv(fr->exec(*args, xsink), xsink);
	 if (*xsink)
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
	 safe_qorelist_t args(do_args(priv->entry[left], pivot), xsink);
	 ReferenceHolder<AbstractQoreNode> rv(fr->exec(*args, xsink), xsink);
	 if (*xsink)
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

QoreListNode *QoreListNode::sort(const ResolvedFunctionReferenceNode *fr, ExceptionSink *xsink) const
{   
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->qsort(fr, 0, priv->length - 1, true, xsink))
	 return 0;

   return rv.release();
}

QoreListNode *QoreListNode::sortStable() const
{
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->priv->entry, priv->length);
   std::stable_sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntries);
   return rv;
}

QoreListNode *QoreListNode::sortDescendingStable() const
{
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%08p priv->length=%d\n", rv->priv->entry, priv->length);
   std::stable_sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

QoreListNode *QoreListNode::sortDescendingStable(const ResolvedFunctionReferenceNode *fr, ExceptionSink *xsink) const
{   
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->mergesort(fr, false, xsink))
	 return 0;

   return rv.release();
}

QoreListNode *QoreListNode::sortStable(const ResolvedFunctionReferenceNode *fr, ExceptionSink *xsink) const
{   
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->mergesort(fr, true, xsink))
	 return 0;

   return rv.release();
}

// does a deep dereference
void QoreListNode::deref_intern(ExceptionSink *xsink)
{
   for (int i = 0; i < priv->length; i++)
      if (priv->entry[i])
         priv->entry[i]->deref(xsink);
#ifdef DEBUG
   priv->length = 0;
#endif
}

void QoreListNode::resize(int num)
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
      //priv->entry = (AbstractQoreNode **)realloc(priv->entry, sizeof (AbstractQoreNode **) * num);
      priv->length = num;
      return;
   }
   // make larger
   if (num >= priv->allocated)
   {
      int d = num >> 2;
      priv->allocated = num + (d < LIST_PAD ? LIST_PAD : d);
      //priv->allocated = num + LIST_PAD;
      priv->entry = (AbstractQoreNode **)realloc(priv->entry, sizeof (AbstractQoreNode *) * priv->allocated);
      for (int i = priv->length; i < priv->allocated; i++)
	 priv->entry[i] = NULL;
   }
   priv->length = num;
}

void QoreListNode::splice_intern(int offset, int len, ExceptionSink *xsink)
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

void QoreListNode::splice_intern(int offset, int len, const AbstractQoreNode *l, ExceptionSink *xsink)
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
      n = (reinterpret_cast<const QoreListNode *>(l))->size();
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
   const QoreListNode *lst = dynamic_cast<const QoreListNode *>(l);
   if (!lst) {
      priv->entry[offset] = l ? l->refSelf() : 0;
      return;
   }
   for (int i = 0; i < n; i++) {
      const AbstractQoreNode *n = lst->retrieve_entry(i);
      priv->entry[offset + i] = n ? n->refSelf() : 0;
   }
}

int QoreListNode::size() const
{
   return priv->length;
}

void QoreListNode::clearNeedsEval()
{
   priv->needs_eval = false;
}

AbstractQoreNode *QoreListNode::min() const
{
   AbstractQoreNode *rv = NULL;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   ExceptionSink xsink;

   for (int i = 0; i < priv->length; i++)
   {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 if (OP_LOG_LT->bool_eval(v, rv, &xsink))
	    rv = v;
	 assert(!xsink);
      }
   }
   return rv ? rv->refSelf() : NULL;
}

AbstractQoreNode *QoreListNode::max() const
{
   AbstractQoreNode *rv = NULL;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   ExceptionSink xsink;

   for (int i = 0; i < priv->length; i++)
   {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 if (OP_LOG_GT->bool_eval(v, rv, &xsink))
	    rv = v;
	 assert(!xsink);
      }
   }
   return rv ? rv->refSelf() : NULL;
}

AbstractQoreNode *QoreListNode::min(const ResolvedFunctionReferenceNode *fr, ExceptionSink *xsink) const
{
   AbstractQoreNode *rv = NULL;

   for (int i = 0; i < priv->length; i++)
   {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 safe_qorelist_t args(do_args(v, rv), xsink);
	 ReferenceHolder<AbstractQoreNode> result(fr->exec(*args, xsink), xsink);
	 if (*xsink)
	    return NULL;
	 if (*result ? result->getAsInt() < 0 : false)
	    rv = v;
      }
   }
   return rv ? rv->refSelf() : NULL;
}

AbstractQoreNode *QoreListNode::max(const ResolvedFunctionReferenceNode *fr, ExceptionSink *xsink) const
{
   AbstractQoreNode *rv = NULL;

   for (int i = 0; i < priv->length; i++)
   {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else
      {
	 safe_qorelist_t args(do_args(v, rv), xsink);
	 ReferenceHolder<AbstractQoreNode> result(fr->exec(*args, xsink), xsink);
	 if (*xsink)
	    return NULL;
	 if (*result ? result->getAsInt() > 0 : false)
	    rv = v;
      }
   }
   return rv ? rv->refSelf() : NULL;
}

QoreListNode *QoreListNode::reverse() const
{
   QoreListNode *l = new QoreListNode();
   l->resize(priv->length);
   for (int i = 0; i < priv->length; ++i)
   {
      AbstractQoreNode *n = priv->entry[priv->length - i - 1];
      l->priv->entry[i] = n ? n->refSelf() : NULL;
   }
   return l;
}

int QoreListNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   if (!size()) {
      str.concat(&EmptyListString);
      return 0;
   }
   str.concat("list: ");
   if (foff != FMT_NONE)
      str.sprintf("(%d element%s)\n", priv->length, priv->length == 1 ? "" : "s");
   else
      str.concat('(');

   for (int i = 0; i < priv->length; ++i)
   {
      if (foff != FMT_NONE)
      {
	 str.addch(' ', foff + 2);
	 str.sprintf("[%d]=", i);
      }
      
      AbstractQoreNode *n = priv->entry[i];
      if (!n) n = Nothing;
      if (n->getAsString(str, foff != FMT_NONE ? foff + 2 : foff, xsink))
	 return -1;
      
      if (i != (priv->length - 1))
	 if (foff != FMT_NONE)
	    str.concat('\n');
	 else
	    str.concat(", ");
   }
   if (foff == FMT_NONE)
      str.concat(')');

   return 0;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreListNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = false;
   if (!priv->length)
      return &EmptyListString;
      
   TempString rv(new QoreString());
   if (getAsString(*(*rv), foff, xsink))
      return 0;

   del = true;
   return rv.release();
}

ListIterator::ListIterator(QoreListNode *lst) : l(lst), pos(-1)
{ 
}

bool ListIterator::next() 
{
   if (l->size() == 0) return false; // empty
   if (++pos >= l->size()) return false; // finished
   return true;
}

AbstractQoreNode *ListIterator::getValue() const
{
   if (pos < 0)
      return NULL;
   return l->retrieve_entry(pos);
}

AbstractQoreNode **ListIterator::getValuePtr() const
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

AbstractQoreNode *ListIterator::eval(ExceptionSink *xsink) const
{
   // QoreListNode::eval_entry() checks if the offset is < 0 already
   return l->eval_entry(pos, xsink);
}

ConstListIterator::ConstListIterator(const QoreListNode *lst) : l(lst), pos(-1)
{ 
}

bool ConstListIterator::next() 
{
   if (l->size() == 0) return false; // empty
   if (++pos >= l->size()) return false; // finished
   return true;
}

const AbstractQoreNode *ConstListIterator::getValue() const
{
   if (pos < 0)
      return NULL;
   return l->retrieve_entry(pos);
}

bool ConstListIterator::last() const
{
   return (bool)(pos == (l->size() - 1)); 
} 

bool ConstListIterator::first() const
{
   return !pos; 
} 

AbstractQoreNode *ConstListIterator::eval(ExceptionSink *xsink) const
{
   // QoreListNode::eval_entry() checks if the offset is < 0 already
   return l->eval_entry(pos, xsink);
}

bool QoreListNode::isFinalized() const
{
   return priv->finalized;
}

void QoreListNode::setFinalized()
{
   priv->finalized = true;
}

bool QoreListNode::isVariableList() const
{
   return priv->vlist;
}

void QoreListNode::setVariableList()
{
   priv->vlist = true;
}
