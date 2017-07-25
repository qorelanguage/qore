/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_list_private.h

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

#ifndef _QORE_QORELISTPRIVATE_H
#define _QORE_QORELISTPRIVATE_H

typedef ReferenceHolder<QoreListNode> safe_qorelist_t;

inline QoreListNode* do_args(AbstractQoreNode* e1, AbstractQoreNode* e2) {
   QoreListNode* l = new QoreListNode;
   e1->ref();
   l->push(e1);
   e2->ref();
   l->push(e2);
   return l;
}

struct qore_list_private {
   AbstractQoreNode** entry = nullptr;
   qore_size_t length = 0;
   qore_size_t allocated = 0;
   unsigned obj_count = 0;
   const QoreTypeInfo* complexTypeInfo = nullptr;
   bool finalized : 1;
   bool vlist : 1;

   DLLLOCAL qore_list_private() : finalized(false), vlist(false) {
   }

   DLLLOCAL ~qore_list_private() {
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

   DLLLOCAL QoreListNode* getCopy() const {
      QoreListNode* l = new QoreListNode;
      if (complexTypeInfo)
         l->priv->complexTypeInfo = complexTypeInfo;
      return l;
   }

   DLLLOCAL QoreListNode* copy(const QoreTypeInfo* newComplexTypeInfo) const {
      QoreListNode* l = new QoreListNode;
      l->priv->complexTypeInfo = newComplexTypeInfo;
      copyIntern(*l->priv);
      return l;
   }

   // strip = copy without type information
   DLLLOCAL QoreListNode* copy(bool strip = false) const {
      QoreListNode* l = strip ? new QoreListNode : getCopy();
      copyIntern(*l->priv);
      return l;
   }

   DLLLOCAL void copyIntern(qore_list_private& l) const {
      l.reserve(length);
      for (qore_size_t i = 0; i < length; ++i)
         l.push(entry[i] ? entry[i]->refSelf() : nullptr);
   }

   DLLLOCAL void reserve(size_t num);

   DLLLOCAL void push(AbstractQoreNode* val) {
      AbstractQoreNode** v = getEntryPtr(length);
      *v = val;
      if (needs_scan(val))
          incScanCount(1);
   }

   DLLLOCAL AbstractQoreNode** getEntryPtr(qore_size_t num) {
      if (num >= length)
          resize(num + 1);
      return &entry[num];
   }

   DLLLOCAL void resize(size_t num);

   DLLLOCAL int getLValue(size_t ind, LValueHelper& lvh, bool for_remove, ExceptionSink* xsink);

   DLLLOCAL void incScanCount(int dt) {
      assert(dt);
      assert(obj_count || (dt > 0));
      //printd(5, "qore_list_private::incScanCount() this: %p dt: %d: %d -> %d\n", this, dt, obj_count, obj_count + dt);
      obj_count += dt;
   }

   DLLLOCAL static const qore_list_private* get(const QoreListNode& l) {
      return l.priv;
   }

   DLLLOCAL static qore_list_private* get(QoreListNode& l) {
      return l.priv;
   }

   DLLLOCAL static unsigned getScanCount(const QoreListNode& l) {
      return l.priv->obj_count;
   }

   DLLLOCAL static void incScanCount(const QoreListNode& l, int dt) {
      l.priv->incScanCount(dt);
   }
};

#endif
