/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_value_list_private.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREVALUELISTPRIVATE_H
#define _QORE_QOREVALUELISTPRIVATE_H

#define LIST_BLOCK 20
#define LIST_PAD   15

typedef ReferenceHolder<QoreValueList> safe_qorevaluelist_t;

/*
static QoreValueList* do_value_args(const QoreValue& e1, const QoreValue& e2) {
   QoreValueList* l = new QoreValueList;
   l->push(e1.refSelf());
   l->push(e2.refSelf());
   return l;
}
*/

class LValueHelper;

struct qore_value_list_private {
   QoreValue* entry = nullptr;
   qore_size_t length = 0;
   qore_size_t allocated = 0;
   unsigned obj_count = 0;
   const QoreTypeInfo* complexTypeInfo = nullptr;
   bool finalized : 1;
   bool vlist : 1;

   DLLLOCAL qore_value_list_private() : finalized(false), vlist(false) {
   }

   DLLLOCAL ~qore_value_list_private() {
      assert(!length);

      if (entry)
         free(entry);
   }

   DLLLOCAL void getTypeName(QoreString& str) const {
       if (complexTypeInfo)
          str.concat(QoreTypeInfo::getName(complexTypeInfo));
       else
          str.concat("list");
   }

   DLLLOCAL QoreValueList* getCopy() const {
      QoreValueList* l = new QoreValueList;
      if (complexTypeInfo)
         l->priv->complexTypeInfo = complexTypeInfo;
      return l;
   }

   DLLLOCAL QoreValueList* copy(const QoreTypeInfo* newComplexTypeInfo) const {
      QoreValueList* l = new QoreValueList;
      l->priv->complexTypeInfo = newComplexTypeInfo;
      copyIntern(*l->priv);
      return l;
   }

   // strip = copy without type information
   DLLLOCAL QoreValueList* copy(bool strip = false) const {
      QoreValueList* l = strip ? new QoreValueList : getCopy();
      copyIntern(*l->priv);
      return l;
   }

   DLLLOCAL void copyIntern(qore_value_list_private& l) const {
      l.reserve(length);
      for (qore_size_t i = 0; i < length; ++i)
         l.push(entry[i].refSelf());
   }

   DLLLOCAL void reserve(size_t num) {
      if (num < length)
         return;
      // make larger
      if (num >= allocated) {
         qore_size_t d = num >> 2;
         allocated = num + (d < LIST_PAD ? LIST_PAD : d);
         entry = (QoreValue*)realloc(entry, sizeof(QoreValue) * allocated);
      }
   }

   //DLLLOCAL int getLValue(size_t ind, LValueHelper& lvh, bool for_remove, ExceptionSink* xsink);

   DLLLOCAL void resize(size_t num) {
      if (num < length) { // make smaller
         //entry = (AbstractQoreNode **)realloc(entry, sizeof (AbstractQoreNode **) * num);
         length = num;
         return;
      }
      // make larger
      if (num >= length) {
         if (num >= allocated) {
            size_t d = num >> 2;
            allocated = num + (d < LIST_PAD ? LIST_PAD : d);
            entry = (QoreValue*)realloc(entry, sizeof(QoreValue) * allocated);
         }
         zeroEntries(length, num);
      }
      length = num;
   }

   DLLLOCAL void zeroEntries(size_t start, size_t end) {
      for (size_t i = start; i < end; ++i)
         entry[i] = QoreValue();
   }

   DLLLOCAL void removeEntry(QoreValue& v, QoreValueList*& rv) {
      if (v.hasNode()) {
         if (needs_scan(v.v.n))
            incScanCount(-1);
      }
      if (!rv)
         rv = new QoreValueList;
      rv->push(v);
   }

   DLLLOCAL size_t checkOffset(ptrdiff_t offset) {
      if (offset < 0) {
         offset = length + offset;
         return offset < 0 ? 0 : offset;
      }
      else if ((size_t)offset > length)
         return length;

      return offset;
   }

   DLLLOCAL void checkOffset(ptrdiff_t offset, ptrdiff_t len, size_t &n_offset, size_t &n_len) {
      n_offset = checkOffset(offset);
      if (len < 0) {
         len = length + len - n_offset;
         n_len = len < 0 ? 0 : len;
         return;
      }
      n_len = len;
   }

   DLLLOCAL QoreValueList* spliceIntern(size_t offset, size_t len, ExceptionSink* xsink, bool extract = false) {
      //printd(5, "spliceIntern(offset: %d, len: %d, length: %d)\n", offset, len, length);
      size_t end;
      if (len > (length - offset)) {
         end = length;
         len = length - offset;
      }
      else
         end = offset + len;

      QoreValueList* rv = extract ? new QoreValueList : 0;

      // dereference all entries that will be removed or add to return value list
      for (size_t i = offset; i < end; i++)
         removeEntry(entry[i], rv);

      // move down entries if necessary
      if (end != length) {
         memmove(entry + offset, entry + end, sizeof(QoreValue) * (length - end));
         // zero out trailing entries
         zeroEntries(length - len, length);
      }
      else // set last entry to 0
         entry[end - 1] = QoreValue();

      resize(length - len);

      return rv;
   }

   DLLLOCAL QoreValueList* spliceIntern(size_t offset, size_t len, const QoreValue l, ExceptionSink* xsink, bool extract = false) {
      //printd(5, "spliceIntern(offset: %d, len: %d, length: %d)\n", offset, len, length);
      size_t end;
      if (len > (length - offset)) {
         end = length;
         len = length - offset;
      }
      else
         end = offset + len;

      QoreValueList* rv = extract ? new QoreValueList : 0;

      // dereference all entries that will be removed or add to return value list
      for (size_t i = offset; i < end; i++)
         removeEntry(entry[i], rv);

      // get number of entries to insert
      size_t n = l.getType() == NT_VALUE_LIST ? l.get<const QoreValueList>()->size() : 1;
      // difference
      if (n > len) { // make bigger
         size_t ol = length;
         resize(length - len + n);
         // move trailing entries forward if necessary
         if (end != ol)
            memmove(entry + (end - len + n), entry + end, sizeof(QoreValue) * (ol - end));
      }
      else if (len > n) { // make list smaller
         memmove(entry + offset + n, entry + offset + len, sizeof(QoreValue) * (length - offset - n));
         // zero out trailing entries
         zeroEntries(length - (len - n), length);
         // resize list
         resize(length - (len - n));
      }

      // add in new entries
      if (l.getType() != NT_VALUE_LIST) {
         entry[offset] = l.refSelf();
         if (l.hasNode() && needs_scan(l.getInternalNode()))
            incScanCount(1);
      }
      else {
         const QoreValueList* lst = l.get<const QoreValueList>();
         for (size_t i = 0; i < n; ++i) {
            const QoreValue v = lst->retrieveEntry(i);
            if (v.hasNode() && needs_scan(v.v.n))
               incScanCount(1);
            entry[offset + i] = v.refSelf();
         }
      }

      return rv;
   }

   DLLLOCAL QoreValue& getEntryReference(size_t num) {
      if (num >= length)
         resize(num + 1);
      return entry[num];
   }

   DLLLOCAL void push(QoreValue val) {
      getEntryReference(length) = val;
      if (val.hasNode() && needs_scan(val.v.n))
         incScanCount(1);
   }

   DLLLOCAL QoreValue getAndClear(size_t i) {
      if (i >= length)
         return QoreValue();
      QoreValue rv = entry[i];
      entry[i] = QoreValue();

      if (rv.hasNode() && needs_scan(rv.v.n))
         incScanCount(-1);

      return rv;
   }

   DLLLOCAL QoreValueList* eval(ExceptionSink* xsink) const {
      ReferenceHolder<QoreValueList> nl(new QoreValueList, xsink);
      for (size_t i = 0; i < length; i++) {
         QoreValue v = entry[i];
         nl->push(v.hasNode() ? v.getInternalNode()->eval(xsink) : v);
         if (*xsink)
            return 0;
      }
      return nl.release();
   }

   // mergesort for controlled and interruptible sorts (stable)
   DLLLOCAL int mergesort(const ResolvedCallReferenceNode* fr, bool ascending, ExceptionSink* xsink);

   // quicksort for controlled and interruptible sorts (unstable)
   // I am so smart that I did not comment this code
   // and now I don't know how it works anymore
   DLLLOCAL int qsort(const ResolvedCallReferenceNode* fr, size_t left, size_t right, bool ascending, ExceptionSink* xsink);

   DLLLOCAL void incScanCount(int dt) {
      assert(dt);
      assert(obj_count || (dt > 0));
      //printd(5, "qore_value_list_private::incScanCount() this: %p dt: %d: %d -> %d\n", this, dt, obj_count, obj_count + dt);
      obj_count += dt;
   }

   DLLLOCAL static qore_value_list_private* get(QoreValueList& l) {
      return l.priv;
   }

   DLLLOCAL static const qore_value_list_private* get(const QoreValueList& l) {
      return l.priv;
   }

   DLLLOCAL static unsigned getScanCount(const QoreValueList& l) {
      return l.priv->obj_count;
   }

   DLLLOCAL static void incScanCount(const QoreValueList& l, int dt) {
      l.priv->incScanCount(dt);
   }
};

//! For use on the stack only: manages result of the optional evaluation of a QoreValueList
class QoreValueListEvalOptionalRefHolder {
private:
   QoreValueList* val;
   ExceptionSink* xsink;
   bool needs_deref;

   DLLLOCAL void discardIntern() {
      if (needs_deref && val)
         val->deref(xsink);
   }

   DLLLOCAL void evalIntern(const QoreValueList* exp) {
      if (exp)
         val = exp->evalList(needs_deref, xsink);
      else {
         val = 0;
         needs_deref = false;
      }
   }

   DLLLOCAL void evalIntern(const QoreListNode* exp);

   //! will create a unique list so the list can be edited
   DLLLOCAL void editIntern() {
      if (!val) {
         val = new QoreValueList;
         needs_deref = true;
      }
      else if (!needs_deref || !val->is_unique()) {
         val = val->copy();
         needs_deref = true;
      }
   }

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreValueListEvalOptionalRefHolder(const QoreValueListEvalOptionalRefHolder&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreValueListEvalOptionalRefHolder& operator=(const QoreValueListEvalOptionalRefHolder&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void* operator new(size_t);

public:
   //! initializes an empty object and saves the ExceptionSink object
   DLLLOCAL QoreValueListEvalOptionalRefHolder(ExceptionSink* n_xsink) : val(0), xsink(n_xsink), needs_deref(false) {
   }

   //! performs an optional evaluation of the list (sets the dereference flag)
   DLLLOCAL QoreValueListEvalOptionalRefHolder(const QoreValueList* exp, ExceptionSink* n_xsink) : xsink(n_xsink) {
      evalIntern(exp);
   }

   //! clears the object (dereferences the old object if necessary)
   DLLLOCAL ~QoreValueListEvalOptionalRefHolder() {
      discardIntern();
   }

   //! clears the object (dereferences the old object if necessary)
   DLLLOCAL void discard() {
      discardIntern();
      needs_deref = false;
      val = 0;
   }

   //! assigns a new value by executing the given list and dereference flag to this object, dereferences the old object if necessary
   DLLLOCAL void assignEval(const QoreValueList* exp) {
      discardIntern();
      evalIntern(exp);
   }

   //! assigns a new value by executing the given list and dereference flag to this object, dereferences the old object if necessary
   DLLLOCAL void assignEval(const QoreListNode* exp) {
      discardIntern();
      evalIntern(exp);
   }

   //! assigns a new value and dereference flag to this object, dereferences the old object if necessary
   DLLLOCAL void assign(bool n_needs_deref, QoreValueList* n_val) {
      discardIntern();
      needs_deref = n_needs_deref;
      val = n_val;
   }

   //! returns true if the object contains a temporary (evaluated) value that needs a dereference
   DLLLOCAL bool needsDeref() const {
      return needs_deref;
   }

   //! returns a referenced value - the caller will own the reference
   /**
      The list is referenced if necessary (if it was a temporary value)
      @return the list value, where the caller will own the reference count
   */
   DLLLOCAL QoreValueList* getReferencedValue() {
      if (needs_deref)
         needs_deref = false;
      else if (val)
         val->ref();
      return val;
   }

   DLLLOCAL QoreValue& getEntryReference(size_t index) {
      editIntern();
      return val->getEntryReference(index);
   }

   DLLLOCAL size_t size() const {
      return val ? val->size() : 0;
   }

   //! returns a pointer to the QoreValueList object being managed
   /**
      if you need a referenced value, use getReferencedValue()
      @return a pointer to the QoreValueList object being managed (or 0 if none)
   */
   DLLLOCAL const QoreValueList* operator->() const { return val; }

   //! returns a pointer to the QoreValueList object being managed
   DLLLOCAL const QoreValueList* operator*() const { return val; }

   //! returns true if a QoreValueList object pointer is being managed, false if the pointer is 0
   DLLLOCAL operator bool() const { return val != 0; }
};

#endif
