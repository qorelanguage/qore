/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_value_list_private.h

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

#ifndef _QORE_QOREVALUELISTPRIVATE_H
#define _QORE_QOREVALUELISTPRIVATE_H

static inline QoreValueList* do_args(const QoreValue& e1, const QoreValue& e2) {
   QoreValueList* l = new QoreValueList;
   l->push(e1.refSelf());
   l->push(e2.refSelf());
   return l;
}

struct qore_value_list_private {
   QoreValue* entry;
   qore_size_t length;
   qore_size_t allocated;
   unsigned obj_count;
   bool finalized : 1;
   bool vlist : 1;

   DLLLOCAL qore_list_private() : entry(0), length(0), allocated(0), obj_count(0), finalized(false), vlist(false) {
   }

   DLLLOCAL ~qore_list_private() {
      assert(!length);

      if (entry)
	 free(entry);
   }

   DLLLOCAL void resize(size_t num) {
      if (num < length) { // make smaller
         //entry = (AbstractQoreNode **)realloc(priv->entry, sizeof (AbstractQoreNode **) * num);
         length = num;
         return;
      }
      // make larger
      if (num >= allocated) {
         size_t d = num >> 2;
         allocated = num + (d < LIST_PAD ? LIST_PAD : d);
         entry = (QoreValue*)realloc(entry, sizeof(QoreValue*) * allocated);
         zeroEntries(length, num);
      }
      length = num;
   }

   DLLLOCAL void zeroEntries(size_t start, size_t end) {
      for (size_t i = start; i < end; ++i)
         entry[i] = QoreValue();
   }

   DLLLOCAL void removeEntry(QoreValue& v, QoreValueList* rv) {
      if (v.hasNode()) {
         if (get_container_obj(v.v.n))
            incObjectCount(-1);
         if (!rv)
            v.v.n->deref(xsink);
      }
      if (rv)
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
         n_len = len < 0 ? o : len;
         return;
      }
      n_len = len;
   }

   DLLLOCAL QoreValueList* spliceIntern(size_t offset, size_t len, ExceptionSink* xsink, bool extract) {
      assert(reference_count() == 1);

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

   DLLLOCAL QoreValueList* spliceIntern(size_t offset, size_t len, const AbstractQoreNode* l, ExceptionSink* xsink, bool extract) {
      assert(reference_count() == 1);

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
      size_t n;
      if (!l)
         n = 1;
      else if (l->getType() == NT_VALUE_LIST)
         n = (reinterpret_cast<const QoreValueList*>(l))->size();
      else
         n = 1;
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
      if (!l || l->getType() != NT_VALUE_LIST) {
         if (l) {
            if (get_container_obj(l))
               incObjectCount(1);
            entry[offset].assign(l->refSelf());
         }
         else
            entry[offset].assignNothing();
      }
      else {
         const QoreValueList* lst = reinterpret_cast<const QoreValueList*>(l);
         for (size_t i = 0; i < n; ++i) {
            const QoreValue v = lst->retrieveEntry(i);
            if (v.isNode() && get_container_obj(v.v.n))
               incObjectCount(1);
            entry[offset + i].assign(v.refSelf());
         }
      }

      return rv;
   }

   DLLLOCAL void push(QoreValue val) {
      getEntryReference(length) = val;
      if (val.isNode() && get_container_obj(val.v.n))
         incObjectCount(1);
   }
   
   DLLLOCAL QoreValue getAndClear(size_t i) {
      if (i >= length)
         return QoreValue();
      QoreValue rv = entry[i];
      entry[i] = QoreValue();

      if (rv.isNode() && get_container_obj(rv.v.n))
         incObjectCount(-1);

      return rv;
   }

   // mergesort for controlled and interruptible sorts (stable)
   DLLLOCAL int mergesort(const ResolvedCallReferenceNode* fr, bool ascending, ExceptionSink* xsink) {
      //printd(5, "List::mergesort() ENTER this: %p, pgm: %p, f: %p length: %d\n", this, pgm, f, length);
   
      if (length <= 1)
         return 0;

      // separate list into two equal-sized lists
      ReferenceHolder<QoreValueList> left(new QoreValueList, xsink);
      ReferenceHolder<QoreValueList> right(new QoreValueList, xsink);
      size_t mid = length / 2;
      {
         size_t i = 0;
         for (; i < mid; i++)
            left->push(entry[i]);
         for (; i < length; i++)
            right->push(entry[i]);
      }

      // set length to 0 - the temporary lists own the entry references now
      length = 0;

      // mergesort the two lists
      if (left->priv->mergesort(fr, ascending, xsink) || right->priv->mergesort(fr, ascending, xsink))
         return -1;

      // merge the resulting lists
      // use offsets and StackList::getAndClear() to avoid moving a lot of memory around
      size_t li = 0, ri = 0;
      while ((li < left.length) && (ri < right.length)) {
         QoreValue& l = left.entry[li];
         QoreValue& r = right.entry[ri];
         safe_qorelist_t args(do_args(l, r), xsink);
         ValueHolder rv(fr->execValue(*args, xsink), xsink);
         if (*xsink)
            return -1;
         int rc = (int)rv->getAsBigInt();
         if ((ascending && rc <= 0)
             || (!ascending && rc > 0))
            push(left->priv->getAndClear(li++));
         else
            push(right->priv->getAndClear(ri++));
      }

      // only one list will have entries left...
      while (li < left->length)
         push(left->getAndClear(li++));
      while (ri < right->length)
         push(right->getAndClear(ri++));

      //printd(5, "List::mergesort() EXIT this: %p, length: %d\n", this, length);

      return 0;
   }

   // quicksort for controlled and interruptible sorts (unstable)
   // I am so smart that I did not comment this code
   // and now I don't know how it works anymore
   DLLLOCAL int qsort(const ResolvedCallReferenceNode* fr, size_t left, size_t right, bool ascending, ExceptionSink* xsink) {
      size_t l_hold = left;
      size_t r_hold = right;
      AbstractQoreNode* pivot = entry[left];

      while (left < right) {
         while (true) {
            safe_qorelist_t args(do_args(entry[right], pivot), xsink);
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
            entry[left] = entry[right];
            left++;
         }

         while (true) {
            safe_qorelist_t args(do_args(entry[left], pivot), xsink);
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
            entry[right] = entry[left];
            right--;
         }
      }
      entry[left] = pivot;
      size_t t_left = left;
      left = l_hold;
      right = r_hold;
      int rc = 0;
      if (left < t_left)
         rc = qsort(fr, left, t_left - 1, ascending, xsink);
      if (!rc && right > t_left)
         rc = qsort(fr, t_left + 1, right, ascending, xsink);
      return rc;
   }
   
   DLLLOCAL void incObjectCount(int dt) {
      assert(dt);
      assert(obj_count || (dt > 0));
      //printd(5, "qore_list_private::incObjectCount() this: %p dt: %d: %d -> %d\n", this, dt, obj_count, obj_count + dt);
      obj_count += dt;
   }

   DLLLOCAL static unsigned getObjectCount(const QoreListNode& l) {
      return l.priv->obj_count;
   }

   DLLLOCAL static void incObjectCount(const QoreListNode& l, int dt) {
      l.priv->incObjectCount(dt);
   }
};

#endif
