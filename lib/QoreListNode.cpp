/*
  QoreListNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols
  
  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/qore_list_private.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <qore/minitest.hpp>
#ifdef DEBUG_TESTS
#  include "tests/List_tests.cpp"
#endif

#include <algorithm>

#define LIST_BLOCK 20
#define LIST_PAD   15

typedef ReferenceHolder<QoreListNode> safe_qorelist_t;

//! For use on the stack only: allows QoreListNode to be allocated on the stack
/** the ExceptionSink object required for the deref() is passed to the constructor
    this class is not safe; it could be misused (for example, by calling ref() or refSelf())
    therefore it's a private class implemented only in this file
 */
class StackList : public QoreListNode {
private:
   ExceptionSink* xsink;
      
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t); 
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL StackList();
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL StackList(bool i);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL StackList(const StackList&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL StackList& operator=(const StackList&);
   
public:
   DLLLOCAL StackList(class ExceptionSink* xs) {
      xsink = xs;
   }
   DLLLOCAL ~StackList() {
      derefImpl(xsink);
   }
   DLLLOCAL AbstractQoreNode *getAndClear(qore_size_t i);
};

qore_size_t QoreListNode::check_offset(qore_offset_t offset) {
   if (offset < 0) {
      offset = priv->length + offset;
      return offset < 0 ? 0 : offset;
   }
   else if ((qore_size_t)offset > priv->length)
      return priv->length;

   return offset;
}

void QoreListNode::check_offset(qore_offset_t offset, qore_offset_t len, qore_size_t &n_offset, qore_size_t &n_len) {
   n_offset = check_offset(offset);
   if (len < 0) {
      len = priv->length + len - n_offset;
      if (len < 0)
	 n_len = 0;
      else
	 n_len = len;
      return;
   }
   n_len = len;
}

QoreListNode::QoreListNode() : AbstractQoreNode(NT_LIST, true, false), priv(new qore_list_private()) {
   //printd(5, "QoreListNode::QoreListNode() 1 this=%p ne=%d v=%d\n", this, needs_eval_flag, value);
}

QoreListNode::QoreListNode(bool i) : AbstractQoreNode(NT_LIST, !i, i), priv(new qore_list_private()) {
   //printd(5, "QoreListNode::QoreListNode() 2 this=%p ne=%d v=%d\n", this, needs_eval_flag, value);
}

QoreListNode::~QoreListNode() {
   delete priv;
}

AbstractQoreNode *QoreListNode::realCopy() const {
   return copy();
}

bool QoreListNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink* xsink) const {
   const QoreListNode *l = v && v->getType() == NT_LIST ? reinterpret_cast<const QoreListNode *>(v) : 0;
   if (!l)
      return false;

   if (l->size() != size())
      return false;
   for (qore_size_t i = 0; i < l->size(); i++)
      if (compareSoft(l->retrieve_entry(i), retrieve_entry(i), xsink) || *xsink)
         return false;
   return true;
}

bool QoreListNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink* xsink) const {
   const QoreListNode *l = v && v->getType() == NT_LIST ? reinterpret_cast<const QoreListNode *>(v) : 0;
   if (!l)
      return false;

   if (l->size() != size())
      return false;
   for (qore_size_t i = 0; i < l->size(); i++)
      if (compareHard(l->retrieve_entry(i), retrieve_entry(i), xsink) || *xsink)
         return false;
   return true;
}

// returns the type name as a c string
const char *QoreListNode::getTypeName() const {
   return getStaticTypeName();
}

/*
bool QoreListNode::is_value() const {
   return !priv->needs_eval;
}
*/

const AbstractQoreNode *QoreListNode::retrieve_entry(qore_size_t num) const {
   if (num >= priv->length)
      return 0;
   return priv->entry[num];
}

AbstractQoreNode *QoreListNode::retrieve_entry(qore_size_t num) {
   if (num >= priv->length)
      return 0;
   return priv->entry[num];
}

AbstractQoreNode *QoreListNode::get_referenced_entry(qore_size_t num) const {
   if (num >= priv->length)
      return 0;
   AbstractQoreNode *rv = priv->entry[num];
   return rv ? rv->refSelf() : 0;
}

int QoreListNode::getEntryAsInt(qore_size_t num) const {
   if (num >= priv->length || !priv->entry[num])
      return 0;
   return priv->entry[num]->getAsInt();
}

AbstractQoreNode **QoreListNode::get_entry_ptr(qore_size_t num) {
   if (num >= priv->length)
      resize(num + 1);
   return &priv->entry[num];
}

AbstractQoreNode **QoreListNode::getExistingEntryPtr(qore_size_t num) {
   assert(reference_count() == 1);
   if (num >= priv->length)
      return 0;
   return &priv->entry[num];
}

void QoreListNode::set_entry(qore_size_t index, AbstractQoreNode *val, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   // before we can entirely get rid of QoreNothingNode, try to convert pointers to NOTHING to 0
   if (val == &Nothing)
      val = 0;
   AbstractQoreNode **v = get_entry_ptr(index);
   if (*v) {
      if (get_container_obj(*v))
	 priv->incObjectCount(-1);

      (*v)->deref(xsink);
   }
   *v = val;

   if (get_container_obj(val))
      priv->incObjectCount(1);
}

AbstractQoreNode *QoreListNode::eval_entry(qore_size_t num, ExceptionSink* xsink) const {
   if (num >= priv->length)
      return 0;
   AbstractQoreNode *rv = priv->entry[num];
   if (rv)
      rv = rv->eval(xsink);
   return rv;
}

void QoreListNode::push(AbstractQoreNode* val) {
   assert(reference_count() == 1);
   AbstractQoreNode **v = get_entry_ptr(priv->length);
   *v = val;
   if (get_container_obj(val))
      priv->incObjectCount(1);
}

void QoreListNode::merge(const QoreListNode *list) {
   assert(reference_count() == 1);
   int start = priv->length;
   resize(priv->length + list->priv->length);
   for (qore_size_t i = 0; i < list->priv->length; i++) {
      AbstractQoreNode* p = list->priv->entry[i];
      if (p) {
	 priv->entry[start + i] = p->refSelf();
	 if (get_container_obj(p))
	    priv->incObjectCount(1);
      }
      else
	 priv->entry[start + i] = 0;
   }
}

// UNSAFE: remove this function
int QoreListNode::delete_entry(qore_size_t ind, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   if (reference_count() > 1) {
      xsink->raiseException("INVALID-UPDATE", "attempt to delete a list entry with references > 0");
      return -1;
   }

   if (ind >= priv->length)
      return -1;

   AbstractQoreNode *e = priv->entry[ind];
   if (get_container_obj(e))
      priv->incObjectCount(-1);
   
   if (e && e->getType() == NT_OBJECT)
      reinterpret_cast<QoreObject *>(e)->doDelete(xsink);

   if (e) {
      e->deref(xsink);
      priv->entry[ind] = 0;
   }

   // do not resize list
#if 0
   // resize list if deleting last element
   if (ind == (priv->length - 1))
      resize(ind);
#endif

   return 0;
}

// delete an priv->entry and move down the rest of the entries
void QoreListNode::pop_entry(qore_size_t ind, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   if (reference_count() > 1) {
      xsink->raiseException("INVALID-UPDATE", "attempt to delete a list entry with references > 0");
      return;
   }

   if (ind >= priv->length)
      return;

   AbstractQoreNode *e = priv->entry[ind];
   if (e && e->getType() == NT_OBJECT)
      reinterpret_cast<QoreObject *>(e)->doDelete(xsink);

   if (get_container_obj(e))
      priv->incObjectCount(-1);

   if (e) {
      e->deref(xsink);
      priv->entry[ind] = 0;
   }

   // resize list
   priv->length--;
   if (ind < priv->length)
      memmove(priv->entry + ind, priv->entry + ind + 1, sizeof(priv->entry) * (priv->length - ind));
   resize(priv->length);
}

void QoreListNode::insert(AbstractQoreNode *val) {
   assert(reference_count() == 1);
   resize(priv->length + 1);
   if (priv->length - 1)
      memmove(priv->entry + 1, priv->entry, sizeof(AbstractQoreNode *) * (priv->length - 1));
   priv->entry[0] = val;
   if (get_container_obj(val))
      priv->incObjectCount(1);
}

AbstractQoreNode *QoreListNode::shift() {
   assert(reference_count() == 1);
   if (!priv->length)
      return 0;
   AbstractQoreNode *rv = priv->entry[0];
   qore_size_t pos = priv->length - 1;
   memmove(priv->entry, priv->entry + 1, sizeof(AbstractQoreNode *) * pos);
   priv->entry[pos] = 0;
   resize(pos);

   if (get_container_obj(rv))
      priv->incObjectCount(-1);

   return rv;
}

AbstractQoreNode *QoreListNode::pop() {
   assert(reference_count() == 1);
   if (!priv->length)
      return 0;
   AbstractQoreNode *rv = priv->entry[priv->length - 1];
   priv->entry[priv->length - 1] = 0;
   resize(priv->length - 1);

   if (get_container_obj(rv))
      priv->incObjectCount(-1);

   return rv;
}

AbstractQoreNode *QoreListNode::evalImpl(ExceptionSink* xsink) const {
   return eval_intern(xsink);
}

AbstractQoreNode *QoreListNode::evalImpl(bool &needs_deref, ExceptionSink* xsink) const {
   return evalList(needs_deref, xsink);
}

QoreListNode *QoreListNode::eval_intern(ExceptionSink* xsink) const {
   ReferenceHolder<QoreListNode> nl(new QoreListNode(), xsink);
   for (qore_size_t i = 0; i < priv->length; i++) {
      nl->push(priv->entry[i] && priv->entry[i]->getType() != NT_NOTHING ? priv->entry[i]->eval(xsink) : 0);
      if (*xsink)
	 return 0;
   }
   return nl.release();
}

QoreListNode *QoreListNode::evalList(ExceptionSink* xsink) const {
   if (value) {
      ref();
      return const_cast<QoreListNode *>(this);
   }

   return eval_intern(xsink);
}

QoreListNode *QoreListNode::evalList(bool &needs_deref, ExceptionSink* xsink) const {
   if (value) {
      needs_deref = false;
      return const_cast<QoreListNode *>(this);
   }
   needs_deref = true;
   return eval_intern(xsink);
}

int64 QoreListNode::bigIntEvalImpl(ExceptionSink* xsink) const {
   return 0;
}

int QoreListNode::integerEvalImpl(ExceptionSink* xsink) const {
   return 0;
}

bool QoreListNode::boolEvalImpl(ExceptionSink* xsink) const {
   return false;
}

double QoreListNode::floatEvalImpl(ExceptionSink* xsink) const {
   return 0.0;
}

QoreListNode *QoreListNode::copy() const {
   QoreListNode *nl = new QoreListNode();
   for (qore_size_t i = 0; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->refSelf() : 0);

   return nl;
}

QoreListNode *QoreListNode::copyListFrom(qore_size_t index) const {
   QoreListNode *nl = new QoreListNode();
   for (qore_size_t i = index; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->refSelf() : 0);

   return nl;
}

void QoreListNode::splice(qore_offset_t offset, ExceptionSink* xsink) {
   qore_size_t n_offset = check_offset(offset);
   if (n_offset == priv->length)
      return;

   splice_intern(n_offset, priv->length - n_offset, xsink);
}

void QoreListNode::splice(qore_offset_t offset, qore_offset_t len, ExceptionSink* xsink) {
   qore_size_t n_offset, n_len;
   check_offset(offset, len, n_offset, n_len);
   if (n_offset == priv->length)
      return;
   splice_intern(n_offset, n_len, xsink);
}

void QoreListNode::splice(qore_offset_t offset, qore_offset_t len, const AbstractQoreNode *l, ExceptionSink* xsink) {
   qore_size_t n_offset, n_len;
   check_offset(offset, len, n_offset, n_len);
   splice_intern(n_offset, n_len, l, xsink);
}

QoreListNode *QoreListNode::extract(qore_offset_t offset, ExceptionSink* xsink) {
   qore_size_t n_offset = check_offset(offset);
   if (n_offset == priv->length)
      return new QoreListNode;

   return splice_intern(n_offset, priv->length - n_offset, xsink, true);
}

QoreListNode *QoreListNode::extract(qore_offset_t offset, qore_offset_t len, ExceptionSink* xsink) {
   qore_size_t n_offset, n_len;
   check_offset(offset, len, n_offset, n_len);
   if (n_offset == priv->length)
      return new QoreListNode;
   return splice_intern(n_offset, n_len, xsink, true);
}

QoreListNode *QoreListNode::extract(qore_offset_t offset, qore_offset_t len, const AbstractQoreNode *l, ExceptionSink* xsink) {
   qore_size_t n_offset, n_len;
   check_offset(offset, len, n_offset, n_len);
   return splice_intern(n_offset, n_len, l, xsink, true);
}

static int compareListEntries(AbstractQoreNode *l, AbstractQoreNode *r) {
   //printd(5, "compareListEntries(%p, %p) (%s %s)\n", l, r, l->getType() == NT_STRING ? l->val.String->getBuffer() : "?", r->getType() == NT_STRING ? r->val.String->getBuffer() : "?");

   // sort non-existant values last
   if (is_nothing(l))
      return 0;
   if (is_nothing(r))
      return 1;

   ExceptionSink xsink;
   ValueHolder v(OP_LOG_LT->eval(l, r, true, &xsink), &xsink);
   //printd(5, "compareListEntries() returning %d\n", (int)v->getAsBool());
   return (int)v->getAsBool();
}

static int compareListEntriesDescending(AbstractQoreNode *l, AbstractQoreNode *r) {
   return compareListEntries(l, r) ? 0 : 1;
}

QoreListNode *QoreListNode::sort() const {
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%p priv->length=%d\n", rv->priv->entry, priv->length);
   std::sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntries);
   return rv;
}

QoreListNode *QoreListNode::sortDescending() const {
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%p priv->length=%d\n", rv->priv->entry, priv->length);
   std::sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

QoreListNode *QoreListNode::sortDescending(const ResolvedCallReferenceNode *fr, ExceptionSink* xsink) const {   
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->qsort(fr, 0, priv->length - 1, false, xsink))
	 return 0;

   return rv.release();
}

AbstractQoreNode *StackList::getAndClear(qore_size_t i) {
   assert(reference_count() == 1);
   if (i >= priv->length)
      return 0;
   AbstractQoreNode *rv = priv->entry[i];
   priv->entry[i] = 0;

   if (get_container_obj(rv))
      priv->incObjectCount(-1);

   return rv;
}

static inline QoreListNode *do_args(AbstractQoreNode *e1, AbstractQoreNode *e2) {
   QoreListNode *l = new QoreListNode();
   e1->ref();
   l->push(e1);
   e2->ref();
   l->push(e2);
   return l;
}

// mergesort for controlled and interruptible sorts (stable)
int QoreListNode::mergesort(const ResolvedCallReferenceNode *fr, bool ascending, ExceptionSink* xsink) {
   //printd(5, "List::mergesort() ENTER this=%p, pgm=%p, f=%p priv->length=%d\n", this, pgm, f, priv->length);
   
   if (priv->length <= 1)
      return 0;

   // separate list into two equal-sized lists
   StackList left(xsink), right(xsink);
   qore_size_t mid = priv->length / 2;
   {
      qore_size_t i;
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
   qore_size_t li = 0, ri = 0;
   while ((li < left.priv->length) && (ri < right.priv->length)) {
      AbstractQoreNode *l = left.priv->entry[li];
      AbstractQoreNode *r = right.priv->entry[ri];
      safe_qorelist_t args(do_args(l, r), xsink);
      ValueHolder rv(fr->execValue(*args, xsink), xsink);
      if (*xsink)
	 return -1;
      int rc = (int)rv->getAsBigInt();
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

   //printd(5, "List::mergesort() EXIT this=%p, priv->length=%d\n", this, priv->length);

   return 0;
}

// quicksort for controlled and interruptible sorts (unstable)
// I am so smart that I did not comment this code
// and now I don't know how it works anymore
int QoreListNode::qsort(const ResolvedCallReferenceNode *fr, qore_size_t left, qore_size_t right, bool ascending, ExceptionSink* xsink) {
   qore_size_t l_hold = left;
   qore_size_t r_hold = right;
   AbstractQoreNode *pivot = priv->entry[left];

   while (left < right) {
      while (true) {
	 safe_qorelist_t args(do_args(priv->entry[right], pivot), xsink);
	 ValueHolder rv(fr->execValue(*args, xsink), xsink);
	 if (*xsink)
	    return -1;
	 int rc = (int)rv->getAsBigInt();
	 if ((left < right)
	     && ((rc >= 0 && ascending)
		 || (rc < 0 && !ascending)))
	    right--;
	 else
	    break;
      }

      if (left != right) {
	 priv->entry[left] = priv->entry[right];
	 left++;
      }

      while (true) {
	 safe_qorelist_t args(do_args(priv->entry[left], pivot), xsink);
	 ValueHolder rv(fr->execValue(*args, xsink), xsink);
	 if (*xsink)
	    return -1;
	 int rc = (int)rv->getAsBigInt();
	 if ((left < right) 
	     && ((rc <= 0 && ascending)
		 || (rc > 0 && !ascending)))
	    left++;
	 else
	    break;
      }
      
      if (left != right) {
	 priv->entry[right] = priv->entry[left];
	 right--;
      }
   }
   priv->entry[left] = pivot;
   qore_size_t t_left = left;
   left = l_hold;
   right = r_hold;
   int rc = 0;
   if (left < t_left)
      rc = qsort(fr, left, t_left - 1, ascending, xsink);
   if (!rc && right > t_left)
      rc = qsort(fr, t_left + 1, right, ascending, xsink);
   return rc;
}

QoreListNode *QoreListNode::sort(const ResolvedCallReferenceNode *fr, ExceptionSink* xsink) const {
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->qsort(fr, 0, priv->length - 1, true, xsink))
	 return 0;

   return rv.release();
}

QoreListNode *QoreListNode::sortStable() const {
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%p priv->length=%d\n", rv->priv->entry, priv->length);
   std::stable_sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntries);
   return rv;
}

QoreListNode *QoreListNode::sortDescendingStable() const {
   QoreListNode *rv = copy();
   //printd(5, "List::sort() priv->entry=%p priv->length=%d\n", rv->priv->entry, priv->length);
   std::stable_sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

QoreListNode *QoreListNode::sortDescendingStable(const ResolvedCallReferenceNode *fr, ExceptionSink* xsink) const {
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->mergesort(fr, false, xsink))
	 return 0;

   return rv.release();
}

QoreListNode *QoreListNode::sortStable(const ResolvedCallReferenceNode *fr, ExceptionSink* xsink) const {
   ReferenceHolder<QoreListNode> rv(copy(), xsink);
   if (priv->length)
      if (rv->mergesort(fr, true, xsink))
	 return 0;

   return rv.release();
}

// does a deep dereference
bool QoreListNode::derefImpl(ExceptionSink* xsink) {
   for (qore_size_t i = 0; i < priv->length; i++)
      if (priv->entry[i])
         priv->entry[i]->deref(xsink);
#ifdef DEBUG
   priv->length = 0;
#endif
   return true;
}

void QoreListNode::resize(qore_size_t num) {
   if (num < priv->length) { // make smaller
      //priv->entry = (AbstractQoreNode **)realloc(priv->entry, sizeof (AbstractQoreNode **) * num);
      priv->length = num;
      return;
   }
   // make larger
   if (num >= priv->allocated) {
      qore_size_t d = num >> 2;
      priv->allocated = num + (d < LIST_PAD ? LIST_PAD : d);
      priv->entry = (AbstractQoreNode **)realloc(priv->entry, sizeof (AbstractQoreNode *) * priv->allocated);
      for (qore_size_t i = priv->length; i < priv->allocated; i++)
	 priv->entry[i] = 0;
   }
   priv->length = num;
}

QoreListNode *QoreListNode::splice_intern(qore_size_t offset, qore_size_t len, ExceptionSink* xsink, bool extract) {
   assert(reference_count() == 1);

   //printd(5, "splice_intern(offset=%d, len=%d, priv->length=%d)\n", offset, len, priv->length);
   qore_size_t end;
   if (len > (priv->length - offset)) {
      end = priv->length;
      len = priv->length - offset;
   }
   else
      end = offset + len;

   QoreListNode *rv = extract ? new QoreListNode : 0;

   // dereference all entries that will be removed or add to return value list
   for (qore_size_t i = offset; i < end; i++) {
      if (rv) 
	 rv->push(priv->entry[i]);
      else if (priv->entry[i]) {
	 if (get_container_obj(priv->entry[i]))
	    priv->incObjectCount(-1);
	 priv->entry[i]->deref(xsink);
      }
   }

   // move down entries if necessary
   if (end != priv->length) {
      memmove(priv->entry + offset, priv->entry + end, sizeof(priv->entry) * (priv->length - end));
      // zero out trailing entries
      for (qore_size_t i = priv->length - len; i < priv->length; i++)
	 priv->entry[i] = 0;
   }
   else // set last priv->entry to 0
      priv->entry[end - 1] = 0;

   resize(priv->length - len);

   return rv;
}

QoreListNode *QoreListNode::splice_intern(qore_size_t offset, qore_size_t len, const AbstractQoreNode *l, ExceptionSink* xsink, bool extract) {
   assert(reference_count() == 1);

   //printd(5, "splice_intern(offset=%d, len=%d, priv->length=%d)\n", offset, len, priv->length);
   qore_size_t end;
   if (len > (priv->length - offset)) {
      end = priv->length;
      len = priv->length - offset;
   }
   else
      end = offset + len;

   QoreListNode *rv = extract ? new QoreListNode : 0;

   // dereference all entries that will be removed or add to return value list
   for (qore_size_t i = offset; i < end; i++) {
      if (rv)
	 rv->push(priv->entry[i]);
      else if (priv->entry[i]) {
	 if (get_container_obj(priv->entry[i]))
	    priv->incObjectCount(-1);
	 priv->entry[i]->deref(xsink);
      }
   }

   // get number of entries to insert
   qore_size_t n;
   if (!l)
      n = 1;
   else if (l->getType() == NT_LIST)
      n = (reinterpret_cast<const QoreListNode *>(l))->size();
   else
      n = 1;
   // difference
   if (n > len) { // make bigger
      qore_size_t ol = priv->length;
      resize(priv->length - len + n);
      // move trailing entries forward if necessary
      if (end != ol)
	 memmove(priv->entry + (end - len + n), priv->entry + end, sizeof(priv->entry) * (ol - end));
   }
   else if (len > n) { // make list smaller
      memmove(priv->entry + offset + n, priv->entry + offset + len, sizeof(priv->entry) * (priv->length - offset - n));
      // zero out trailing entries
      for (qore_size_t i = priv->length - (len - n); i < priv->length; ++i)
	 priv->entry[i] = 0;
      // resize list
      resize(priv->length - (len - n));
   }

   // add in new entries
   if (!l || l->getType() != NT_LIST) {
      if (l) {
	 if (get_container_obj(l))
	    priv->incObjectCount(1);
	 priv->entry[offset] = l->refSelf();
      }
      else
	 priv->entry[offset] = 0;
   }
   else {
      const QoreListNode *lst = reinterpret_cast<const QoreListNode *>(l);
      for (qore_size_t i = 0; i < n; ++i) {
	 const AbstractQoreNode *v = lst->retrieve_entry(i);
	 if (v) {
	    priv->entry[offset + i] = v->refSelf();
	    if (get_container_obj(v))
	       priv->incObjectCount(1);
	 }
	 else
	    priv->entry[offset + i] = 0;
      }
   }

   return rv;
}

qore_size_t QoreListNode::size() const {
   return priv->length;
}

bool QoreListNode::empty() const {
   return !priv->length;
}

void QoreListNode::clearNeedsEval() {
   value = true;
   needs_eval_flag = false;
}

void QoreListNode::setNeedsEval() {
   value = false;
   needs_eval_flag = true;
}

AbstractQoreNode *QoreListNode::min() const {
   AbstractQoreNode *rv = 0;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   ExceptionSink xsink;

   for (qore_size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else {
	 ValueHolder vh(OP_LOG_LT->eval(v, rv, true, &xsink), &xsink);
	 if (vh->getAsBool())
	    rv = v;
	 assert(!xsink);
      }
   }
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode *QoreListNode::max() const {
   AbstractQoreNode *rv = 0;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   ExceptionSink xsink;

   for (qore_size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else {
	 ValueHolder vh(OP_LOG_GT->eval(v, rv, true, &xsink), &xsink);
	 if (vh->getAsBool())
	    rv = v;
	 assert(!xsink);
      }
   }
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode *QoreListNode::min(const ResolvedCallReferenceNode *fr, ExceptionSink* xsink) const {
   AbstractQoreNode *rv = 0;

   for (qore_size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else {
	 safe_qorelist_t args(do_args(v, rv), xsink);
	 ValueHolder result(fr->execValue(*args, xsink), xsink);
	 if (*xsink)
	    return 0;
	 if (result->getAsBigInt() < 0)
	    rv = v;
      }
   }
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode *QoreListNode::max(const ResolvedCallReferenceNode *fr, ExceptionSink* xsink) const {
   AbstractQoreNode *rv = 0;

   for (qore_size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode *v = priv->entry[i];

      if (!rv)
	 rv = v;
      else {
	 safe_qorelist_t args(do_args(v, rv), xsink);
	 ValueHolder result(fr->execValue(*args, xsink), xsink);
	 if (*xsink)
	    return 0;
	 if (result->getAsBigInt() > 0)
	    rv = v;
      }
   }
   return rv ? rv->refSelf() : 0;
}

QoreListNode *QoreListNode::reverse() const {
   QoreListNode *l = new QoreListNode();
   l->resize(priv->length);
   for (qore_size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode *n = priv->entry[priv->length - i - 1];
      l->priv->entry[i] = n ? n->refSelf() : 0;
   }
   return l;
}

int QoreListNode::getAsString(QoreString &str, int foff, ExceptionSink* xsink) const {
   QoreContainerHelper cch(this);
   if (!cch) {
      str.sprintf("[ERROR: recursive reference to list %p]", this);
      return 0;
   }

   if (foff == FMT_YAML_SHORT) {
      str.concat('[');
      ConstListIterator li(this);
      while (li.next()) {
	 const AbstractQoreNode *n = li.getValue();
	 if (!n) n = &Nothing;
	 if (n->getAsString(str, foff, xsink))
	    return -1;
	 if (!li.last())
	    str.concat(", ");
      }
      str.concat(']');
      return 0;
   }

   if (!size()) {
      str.concat(&EmptyListString);
      return 0;
   }
   str.concat("list: (");

   if (foff != FMT_NONE)
      str.sprintf("%d element%s)\n", priv->length, priv->length == 1 ? "" : "s");

   for (qore_size_t i = 0; i < priv->length; ++i) {
      if (foff != FMT_NONE) {
	 str.addch(' ', foff + 2);
	 str.sprintf("[%d]=", i);
      }
      
      AbstractQoreNode *n = priv->entry[i];
      if (!n) n = &Nothing;
      if (n->getAsString(str, foff != FMT_NONE ? foff + 2 : foff, xsink))
	 return -1;
      
      if (i != (priv->length - 1)) {
	 if (foff != FMT_NONE)
	    str.concat('\n');
	 else
	    str.concat(", ");
      }
   }
   if (foff == FMT_NONE)
      str.concat(')');

   return 0;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *QoreListNode::getAsString(bool &del, int foff, ExceptionSink* xsink) const {
   del = false;
   if (!priv->length && foff != FMT_YAML_SHORT)
      return &EmptyListString;
      
   TempString rv(new QoreString);
   if (getAsString(*(*rv), foff, xsink))
      return 0;

   del = true;
   return rv.release();
}

bool QoreListNode::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   if (runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;
   return !empty();
}

ListIterator::ListIterator(QoreListNode *lst, qore_size_t n_pos) : l(lst) {
   set(n_pos);
}

ListIterator::ListIterator(QoreListNode &lst, qore_size_t n_pos) : l(&lst) {
   set(n_pos);
}

bool ListIterator::next() {
   if (++pos == (qore_offset_t)l->size()) {
      pos = -1;
      return false; // finished
   }
   return true;
}

bool ListIterator::prev() {
   if (l->empty())
      return false; // empty
   if (pos == -1) {
      pos = l->size() - 1;
      return true;
   }
   if (!pos) {
      pos = -1;
      return false; // finished
   }
   --pos;
   return true;
}

int ListIterator::set(qore_size_t n_pos) {
   if (n_pos >= l->size()) {
      pos = -1;
      return -1;
   }
   pos = n_pos;
   return 0;
}

AbstractQoreNode *ListIterator::getValue() const {
   return l->retrieve_entry(pos);
}

AbstractQoreNode *ListIterator::getReferencedValue() const {
   AbstractQoreNode *rv = l->retrieve_entry(pos);
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode *ListIterator::takeValue() {
   if (l->is_unique()) {
      AbstractQoreNode **p = l->get_entry_ptr(pos);
      AbstractQoreNode *rv = *p;
      *p = 0;
      return rv;
   }
   AbstractQoreNode *rv = l->retrieve_entry(pos);
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode **ListIterator::getValuePtr() const {
   if (pos > (qore_offset_t)l->size())
      return 0;
   return l->get_entry_ptr(pos);
}

bool ListIterator::last() const {
   return (bool)(pos == (qore_offset_t)(l->size() - 1));
} 

bool ListIterator::first() const {
   return !pos; 
} 

ConstListIterator::ConstListIterator(const QoreListNode *lst, qore_size_t n_pos) : l(lst) {
   set(n_pos);
}

ConstListIterator::ConstListIterator(const QoreListNode &lst, qore_size_t n_pos) : l(&lst) {
   set(n_pos);
}

bool ConstListIterator::next() {
   if (++pos == (qore_offset_t)l->size()) {
      pos = -1;
      return false; // finished
   }
   return true;
}

bool ConstListIterator::prev() {
   if (l->empty())
      return false; // empty
   if (pos == -1) {
      pos = l->size() - 1;
      return true;
   }
   if (!pos) {
      pos = -1;
      return false; // finished
   }
   --pos;
   return true;
}

int ConstListIterator::set(qore_size_t n_pos) {
   if (n_pos >= l->size()) {
      pos = -1;
      return -1;
   }
   pos = n_pos;
   return 0;
}

const AbstractQoreNode *ConstListIterator::getValue() const {
   return l->retrieve_entry(pos);
}

AbstractQoreNode *ConstListIterator::getReferencedValue() const {
   const AbstractQoreNode *rv = l->retrieve_entry(pos);
   return rv ? rv->refSelf() : 0;
}

bool ConstListIterator::last() const {
   return (bool)(pos == (qore_offset_t)(l->size() - 1));
} 

bool ConstListIterator::first() const {
   return !pos;
} 

void ConstListIterator::reset() {
   pos = -1;
}

bool QoreListNode::isFinalized() const {
   return priv->finalized;
}

void QoreListNode::setFinalized() {
   priv->finalized = true;
}

bool QoreListNode::isVariableList() const {
   return priv->vlist;
}

void QoreListNode::setVariableList() {
   priv->vlist = true;
}

QoreListNode *QoreListNode::listRefSelf() const {
   ref();
   return const_cast<QoreListNode *>(this);
}

QoreListNode *QoreListNode::parseInitList(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = listTypeInfo;

   QoreListNodeParseInitHelper li(this, oflag, pflag, lvids);
   while (li.next()) {
      const QoreTypeInfo *argTypeInfo = 0;
      li.parseInit(argTypeInfo);
   }

   //printd(0, "QoreListNode::parseInit() this=%p ne=%d v=%d\n", this, needs_eval_flag, value);

   return this;
}

AbstractQoreNode *QoreListNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   return parseInitList(oflag, pflag, lvids, typeInfo);
}

AbstractQoreNode *QoreListNode::swap(qore_offset_t offset, AbstractQoreNode *val) {
   AbstractQoreNode **ptr = get_entry_ptr(offset);
   AbstractQoreNode *rv = *ptr;
   *ptr = val;
   return rv;
}

AbstractQoreNode *QoreListNode::takeExists(qore_offset_t offset) {
   AbstractQoreNode **ptr = getExistingEntryPtr(offset);
   if (!ptr)
      return 0;
   AbstractQoreNode *rv = *ptr;
   *ptr = 0;
   return rv;
}
