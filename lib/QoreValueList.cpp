/*
  QoreValueList.cpp

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

typedef ReferenceHolder<QoreValueList> safe_qorelist_t;

QoreValueList::QoreValueList() : AbstractQoreNode(NT_VALUE_LIST, true, false), priv(new qore_list_private) {
   //printd(5, "QoreValueList::QoreValueList() 1 this=%p ne=%d v=%d\n", this, needs_eval_flag, value);
}

QoreValueList::QoreValueList(bool i) : AbstractQoreNode(NT_VALUE_LIST, !i, i), priv(new qore_list_private) {
   //printd(5, "QoreValueList::QoreValueList() 2 this=%p ne=%d v=%d\n", this, needs_eval_flag, value);
}

QoreValueList::~QoreValueList() {
   delete priv;
}

AbstractQoreNode* QoreValueList::realCopy() const {
   return copy();
}

bool QoreValueList::is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const QoreValueList* l = v && v->getType() == NT_VALUE_LIST ? reinterpret_cast<const QoreValueList*>(v) : 0;
   if (!l)
      return false;

   if (l->size() != size())
      return false;
   for (size_t i = 0; i < l->size(); i++)
      if (compareSoft(l->retrieve_entry(i), retrieve_entry(i), xsink) || *xsink)
         return false;
   return true;
}

bool QoreValueList::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const QoreValueList* l = v && v->getType() == NT_VALUE_LIST ? reinterpret_cast<const QoreValueList*>(v) : 0;
   if (!l)
      return false;

   if (l->size() != size())
      return false;
   for (size_t i = 0; i < l->size(); i++)
      if (compareHard(l->retrieve_entry(i), retrieve_entry(i), xsink) || *xsink)
         return false;
   return true;
}

// returns the type name as a c string
const char* QoreValueList::getTypeName() const {
   return getStaticTypeName();
}

/*
bool QoreValueList::is_value() const {
   return !priv->needs_eval;
}
*/

const AbstractQoreNode* QoreValueList::retrieve_entry(size_t num) const {
   if (num >= priv->length)
      return 0;
   return priv->entry[num];
}

AbstractQoreNode* QoreValueList::retrieve_entry(size_t num) {
   if (num >= priv->length)
      return 0;
   return priv->entry[num];
}

AbstractQoreNode* QoreValueList::get_referenced_entry(size_t num) const {
   if (num >= priv->length)
      return 0;
   AbstractQoreNode* rv = priv->entry[num];
   return rv ? rv->refSelf() : 0;
}

int QoreValueList::getEntryAsInt(size_t num) const {
   if (num >= priv->length || !priv->entry[num])
      return 0;
   return priv->entry[num]->getAsInt();
}

AbstractQoreNode* *QoreValueList::get_entry_ptr(size_t num) {
   if (num >= priv->length)
      priv->resize(num + 1);
   return &priv->entry[num];
}

AbstractQoreNode* *QoreValueList::getExistingEntryPtr(size_t num) {
   assert(reference_count() == 1);
   if (num >= priv->length)
      return 0;
   return &priv->entry[num];
}

void QoreValueList::set_entry(size_t index, AbstractQoreNode* val, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   // before we can entirely get rid of QoreNothingNode, try to convert pointers to NOTHING to 0
   if (val == &Nothing)
      val = 0;
   AbstractQoreNode* *v = get_entry_ptr(index);
   if (*v) {
      if (get_container_obj(*v))
	 priv->incObjectCount(-1);

      (*v)->deref(xsink);
   }
   *v = val;

   if (get_container_obj(val))
      priv->incObjectCount(1);
}

AbstractQoreNode* QoreValueList::eval_entry(size_t num, ExceptionSink* xsink) const {
   if (num >= priv->length)
      return 0;
   AbstractQoreNode* rv = priv->entry[num];
   if (rv)
      rv = rv->eval(xsink);
   return rv;
}

void QoreValueList::push(QoreValue val) {
   assert(reference_count() == 1);
   priv->push(val);
}

void QoreValueList::merge(const QoreValueList* list) {
   assert(reference_count() == 1);
   int start = priv->length;
   priv->resize(priv->length + list->priv->length);
   for (size_t i = 0; i < list->priv->length; i++) {
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

// delete an priv->entry and move down the rest of the entries
void QoreValueList::pop_entry(size_t ind, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   if (reference_count() > 1) {
      xsink->raiseException("INVALID-UPDATE", "attempt to delete a list entry with references > 0");
      return;
   }

   if (ind >= priv->length)
      return;

   AbstractQoreNode* e = priv->entry[ind];
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
   priv->resize(priv->length);
}

void QoreValueList::insert(AbstractQoreNode* val) {
   assert(reference_count() == 1);
   priv->resize(priv->length + 1);
   if (priv->length - 1)
      memmove(priv->entry + 1, priv->entry, sizeof(AbstractQoreNode* ) * (priv->length - 1));
   priv->entry[0] = val;
   if (get_container_obj(val))
      priv->incObjectCount(1);
}

AbstractQoreNode* QoreValueList::shift() {
   assert(reference_count() == 1);
   if (!priv->length)
      return 0;
   AbstractQoreNode* rv = priv->entry[0];
   size_t pos = priv->length - 1;
   memmove(priv->entry, priv->entry + 1, sizeof(AbstractQoreNode* ) * pos);
   priv->entry[pos] = 0;
   priv->resize(pos);

   if (get_container_obj(rv))
      priv->incObjectCount(-1);

   return rv;
}

AbstractQoreNode* QoreValueList::pop() {
   assert(reference_count() == 1);
   if (!priv->length)
      return 0;
   AbstractQoreNode* rv = priv->entry[priv->length - 1];
   priv->entry[priv->length - 1] = 0;
   priv->resize(priv->length - 1);

   if (get_container_obj(rv))
      priv->incObjectCount(-1);

   return rv;
}

AbstractQoreNode* QoreValueList::evalImpl(ExceptionSink* xsink) const {
   return eval_intern(xsink);
}

AbstractQoreNode* QoreValueList::evalImpl(bool &needs_deref, ExceptionSink* xsink) const {
   return evalList(needs_deref, xsink);
}

QoreValueList* QoreValueList::eval_intern(ExceptionSink* xsink) const {
   ReferenceHolder<QoreValueList> nl(new QoreValueList(), xsink);
   for (size_t i = 0; i < priv->length; i++) {
      nl->push(priv->entry[i] && priv->entry[i]->getType() != NT_NOTHING ? priv->entry[i]->eval(xsink) : 0);
      if (*xsink)
	 return 0;
   }
   return nl.release();
}

QoreValueList* QoreValueList::evalList(ExceptionSink* xsink) const {
   if (value) {
      ref();
      return const_cast<QoreValueList*>(this);
   }

   return eval_intern(xsink);
}

QoreValueList* QoreValueList::evalList(bool &needs_deref, ExceptionSink* xsink) const {
   if (value) {
      needs_deref = false;
      return const_cast<QoreValueList*>(this);
   }
   needs_deref = true;
   return eval_intern(xsink);
}

int64 QoreValueList::bigIntEvalImpl(ExceptionSink* xsink) const {
   return 0;
}

int QoreValueList::integerEvalImpl(ExceptionSink* xsink) const {
   return 0;
}

bool QoreValueList::boolEvalImpl(ExceptionSink* xsink) const {
   return false;
}

double QoreValueList::floatEvalImpl(ExceptionSink* xsink) const {
   return 0.0;
}

QoreValueList* QoreValueList::copy() const {
   QoreValueList* nl = new QoreValueList();
   for (size_t i = 0; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->refSelf() : 0);

   return nl;
}

QoreValueList* QoreValueList::copyListFrom(size_t index) const {
   QoreValueList* nl = new QoreValueList();
   for (size_t i = index; i < priv->length; i++)
      nl->push(priv->entry[i] ? priv->entry[i]->refSelf() : 0);

   return nl;
}

void QoreValueList::splice(ptrdiff_t offset, ExceptionSink* xsink) {
   size_t n_offset = priv->checkOffset(offset);
   if (n_offset == priv->length)
      return;

   priv->spliceIntern(n_offset, priv->length - n_offset, xsink);
}

void QoreValueList::splice(ptrdiff_t offset, ptrdiff_t len, ExceptionSink* xsink) {
   size_t n_offset, n_len;
   priv->checkOffset(offset, len, n_offset, n_len);
   if (n_offset == priv->length)
      return;
   priv->spliceIntern(n_offset, n_len, xsink);
}

void QoreValueList::splice(ptrdiff_t offset, ptrdiff_t len, const AbstractQoreNode* l, ExceptionSink* xsink) {
   size_t n_offset, n_len;
   priv->checkOffset(offset, len, n_offset, n_len);
   priv->spliceIntern(n_offset, n_len, l, xsink);
}

QoreValueList* QoreValueList::extract(ptrdiff_t offset, ExceptionSink* xsink) {
   size_t n_offset = priv->checkOffset(offset);
   if (n_offset == priv->length)
      return new QoreValueList;

   return priv->spliceIntern(n_offset, priv->length - n_offset, xsink, true);
}

QoreValueList* QoreValueList::extract(ptrdiff_t offset, ptrdiff_t len, ExceptionSink* xsink) {
   size_t n_offset, n_len;
   priv->checkOffset(offset, len, n_offset, n_len);
   if (n_offset == priv->length)
      return new QoreValueList;
   return priv->spliceIntern(n_offset, n_len, xsink, true);
}

QoreValueList* QoreValueList::extract(ptrdiff_t offset, ptrdiff_t len, const AbstractQoreNode* l, ExceptionSink* xsink) {
   size_t n_offset, n_len;
   priv->checkOffset(offset, len, n_offset, n_len);
   return priv->spliceIntern(n_offset, n_len, l, xsink, true);
}

static int compareListEntries(AbstractQoreNode* l, AbstractQoreNode* r) {
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

static int compareListEntriesDescending(AbstractQoreNode* l, AbstractQoreNode* r) {
   return compareListEntries(l, r) ? 0 : 1;
}

QoreValueList* QoreValueList::sort() const {
   QoreValueList* rv = copy();
   //printd(5, "List::sort() priv->entry: %p priv->length: %d\n", rv->priv->entry, priv->length);
   std::sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntries);
   return rv;
}

QoreValueList* QoreValueList::sortDescending() const {
   QoreValueList* rv = copy();
   //printd(5, "List::sort() priv->entry: %p priv->length: %d\n", rv->priv->entry, priv->length);
   std::sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

QoreValueList* QoreValueList::sortDescending(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const {   
   ReferenceHolder<QoreValueList> rv(copy(), xsink);
   if (priv->length)
      if (rv->qsort(fr, 0, priv->length - 1, false, xsink))
	 return 0;

   return rv.release();
}

AbstractQoreNode* StackList::getAndClear(size_t i) {
   assert(reference_count() == 1);
   if (i >= priv->length)
      return 0;
   AbstractQoreNode* rv = priv->entry[i];
   priv->entry[i] = 0;

   if (get_container_obj(rv))
      priv->incObjectCount(-1);

   return rv;
}

QoreValueList* QoreValueList::sort(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const {
   ReferenceHolder<QoreValueList> rv(copy(), xsink);
   if (priv->length)
      if (rv->qsort(fr, 0, priv->length - 1, true, xsink))
	 return 0;

   return rv.release();
}

QoreValueList* QoreValueList::sortStable() const {
   QoreValueList* rv = copy();
   //printd(5, "List::sort() priv->entry: %p priv->length: %d\n", rv->priv->entry, priv->length);
   std::stable_sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntries);
   return rv;
}

QoreValueList* QoreValueList::sortDescendingStable() const {
   QoreValueList* rv = copy();
   //printd(5, "List::sort() priv->entry: %p priv->length: %d\n", rv->priv->entry, priv->length);
   std::stable_sort(rv->priv->entry, rv->priv->entry + priv->length, compareListEntriesDescending);
   return rv;
}

QoreValueList* QoreValueList::sortDescendingStable(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const {
   ReferenceHolder<QoreValueList> rv(copy(), xsink);
   if (priv->length)
      if (rv->mergesort(fr, false, xsink))
	 return 0;

   return rv.release();
}

QoreValueList* QoreValueList::sortStable(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const {
   ReferenceHolder<QoreValueList> rv(copy(), xsink);
   if (priv->length)
      if (rv->mergesort(fr, true, xsink))
	 return 0;

   return rv.release();
}

// does a deep dereference
bool QoreValueList::derefImpl(ExceptionSink* xsink) {
   for (size_t i = 0; i < priv->length; i++)
      if (priv->entry[i])
         priv->entry[i]->deref(xsink);
#ifdef DEBUG
   priv->length = 0;
#endif
   return true;
}

void QoreValueList::resize(size_t num) {
   if (num < priv->length) { // make smaller
      //priv->entry = (AbstractQoreNode* *)realloc(priv->entry, sizeof (AbstractQoreNode* *) * num);
      priv->length = num;
      return;
   }
   // make larger
   if (num >= priv->allocated) {
      size_t d = num >> 2;
      priv->allocated = num + (d < LIST_PAD ? LIST_PAD : d);
      priv->entry = (AbstractQoreNode* *)realloc(priv->entry, sizeof (AbstractQoreNode* ) * priv->allocated);
      for (size_t i = priv->length; i < priv->allocated; i++)
	 priv->entry[i] = 0;
   }
   priv->length = num;
}

size_t QoreValueList::size() const {
   return priv->length;
}

bool QoreValueList::empty() const {
   return !priv->length;
}

void QoreValueList::clearNeedsEval() {
   value = true;
   needs_eval_flag = false;
}

void QoreValueList::setNeedsEval() {
   value = false;
   needs_eval_flag = true;
}

AbstractQoreNode* QoreValueList::min() const {
   AbstractQoreNode* rv = 0;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   ExceptionSink xsink;

   for (size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode* v = priv->entry[i];

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

AbstractQoreNode* QoreValueList::max() const {
   AbstractQoreNode* rv = 0;
   // it's not possible for an exception to be raised here, but
   // we need an exception sink anyway
   ExceptionSink xsink;

   for (size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode* v = priv->entry[i];

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

AbstractQoreNode* QoreValueList::min(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const {
   AbstractQoreNode* rv = 0;

   for (size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode* v = priv->entry[i];

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

AbstractQoreNode* QoreValueList::max(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const {
   AbstractQoreNode* rv = 0;

   for (size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode* v = priv->entry[i];

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

QoreValueList* QoreValueList::reverse() const {
   QoreValueList* l = new QoreValueList();
   l->priv->resize(priv->length);
   for (size_t i = 0; i < priv->length; ++i) {
      AbstractQoreNode* n = priv->entry[priv->length - i - 1];
      l->priv->entry[i] = n ? n->refSelf() : 0;
   }
   return l;
}

int QoreValueList::getAsString(QoreString &str, int foff, ExceptionSink* xsink) const {
   QoreContainerHelper cch(this);
   if (!cch) {
      str.sprintf("[ERROR: recursive reference to list %p]", this);
      return 0;
   }

   if (foff == FMT_YAML_SHORT) {
      str.concat('[');
      ConstValueListIterator li(this);
      while (li.next()) {
	 const AbstractQoreNode* n = li.getValue();
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

   for (size_t i = 0; i < priv->length; ++i) {
      if (foff != FMT_NONE) {
	 str.addch(' ', foff + 2);
	 str.sprintf("[%d]=", i);
      }
      
      AbstractQoreNode* n = priv->entry[i];
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
QoreString *QoreValueList::getAsString(bool &del, int foff, ExceptionSink* xsink) const {
   del = false;
   if (!priv->length && foff != FMT_YAML_SHORT)
      return &EmptyListString;
      
   TempString rv(new QoreString);
   if (getAsString(*(*rv), foff, xsink))
      return 0;

   del = true;
   return rv.release();
}

bool QoreValueList::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   if (runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;
   return !empty();
}

ValueListIterator::ValueListIterator(QoreValueList* lst, size_t n_pos) : l(lst) {
   set(n_pos);
}

ValueListIterator::ValueListIterator(QoreValueList &lst, size_t n_pos) : l(&lst) {
   set(n_pos);
}

bool ValueListIterator::next() {
   if (++pos == (ptrdiff_t)l->size()) {
      pos = -1;
      return false; // finished
   }
   return true;
}

bool ValueListIterator::prev() {
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

int ValueListIterator::set(size_t n_pos) {
   if (n_pos >= l->size()) {
      pos = -1;
      return -1;
   }
   pos = n_pos;
   return 0;
}

AbstractQoreNode* ValueListIterator::getValue() const {
   return l->retrieve_entry(pos);
}

AbstractQoreNode* ValueListIterator::getReferencedValue() const {
   AbstractQoreNode* rv = l->retrieve_entry(pos);
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode* ValueListIterator::takeValue() {
   if (l->is_unique()) {
      AbstractQoreNode* *p = l->get_entry_ptr(pos);
      AbstractQoreNode* rv = *p;
      *p = 0;
      return rv;
   }
   AbstractQoreNode* rv = l->retrieve_entry(pos);
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode* *ValueListIterator::getValuePtr() const {
   if (pos > (ptrdiff_t)l->size())
      return 0;
   return l->get_entry_ptr(pos);
}

bool ValueListIterator::last() const {
   return (bool)(pos == (ptrdiff_t)(l->size() - 1));
} 

bool ValueListIterator::first() const {
   return !pos; 
} 

ConstValueListIterator::ConstValueListIterator(const QoreValueList* lst, size_t n_pos) : l(lst) {
   set(n_pos);
}

ConstValueListIterator::ConstValueListIterator(const QoreValueList &lst, size_t n_pos) : l(&lst) {
   set(n_pos);
}

bool ConstValueListIterator::next() {
   if (++pos == (ptrdiff_t)l->size()) {
      pos = -1;
      return false; // finished
   }
   return true;
}

bool ConstValueListIterator::prev() {
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

int ConstValueListIterator::set(size_t n_pos) {
   if (n_pos >= l->size()) {
      pos = -1;
      return -1;
   }
   pos = n_pos;
   return 0;
}

const AbstractQoreNode* ConstValueListIterator::getValue() const {
   return l->retrieve_entry(pos);
}

AbstractQoreNode* ConstValueListIterator::getReferencedValue() const {
   const AbstractQoreNode* rv = l->retrieve_entry(pos);
   return rv ? rv->refSelf() : 0;
}

bool ConstValueListIterator::last() const {
   return (bool)(pos == (ptrdiff_t)(l->size() - 1));
} 

bool ConstValueListIterator::first() const {
   return !pos;
} 

void ConstValueListIterator::reset() {
   pos = -1;
}

bool QoreValueList::isFinalized() const {
   return priv->finalized;
}

void QoreValueList::setFinalized() {
   priv->finalized = true;
}

bool QoreValueList::isVariableList() const {
   return priv->vlist;
}

void QoreValueList::setVariableList() {
   priv->vlist = true;
}

QoreValueList* QoreValueList::listRefSelf() const {
   ref();
   return const_cast<QoreValueList*>(this);
}

QoreValueList* QoreValueList::parseInitList(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   typeInfo = listTypeInfo;

   QoreValueListParseInitHelper li(this, oflag, pflag, lvids);
   while (li.next()) {
      const QoreTypeInfo *argTypeInfo = 0;
      li.parseInit(argTypeInfo);
   }

   //printd(0, "QoreValueList::parseInit() this: %p ne: %d v: %d\n", this, needs_eval_flag, value);

   return this;
}

AbstractQoreNode* QoreValueList::parseInit(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   return parseInitList(oflag, pflag, lvids, typeInfo);
}

AbstractQoreNode* QoreValueList::swap(ptrdiff_t offset, AbstractQoreNode* val) {
   AbstractQoreNode* *ptr = get_entry_ptr(offset);
   AbstractQoreNode* rv = *ptr;
   *ptr = val;
   return rv;
}

AbstractQoreNode* QoreValueList::takeExists(ptrdiff_t offset) {
   AbstractQoreNode* *ptr = getExistingEntryPtr(offset);
   if (!ptr)
      return 0;
   AbstractQoreNode* rv = *ptr;
   *ptr = 0;
   return rv;
}
