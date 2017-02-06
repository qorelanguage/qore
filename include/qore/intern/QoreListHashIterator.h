/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreListHashIterator.h

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

#ifndef _QORE_QORELISTHASHITERATOR_H

#define _QORE_QORELISTHASHITERATOR_H

#include "qore/intern/QoreListIterator.h"

#include <assert.h>

extern QoreClass* QC_LISTHASHITERATOR;

// the c++ object
class QoreListHashIterator : public QoreListIterator {
protected:
   DLLLOCAL virtual ~QoreListHashIterator() {
   }

   DLLLOCAL const QoreHashNode* checkHash(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;
      const AbstractQoreNode* n = getValue();
      if (get_node_type(n) != NT_HASH) {
         xsink->raiseException("ITERATOR-ERROR", "The %s object is not a list of hashes, element " QSD " (starting with 0) is type '%s' instead (expected 'hash')", getName(), index(), get_type_name(n));
         return 0;
      }
      return static_cast<const QoreHashNode*>(n);
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValueIntern(const QoreHashNode* h, const char* key, ExceptionSink* xsink) const {
      bool exists = false;
      const AbstractQoreNode* n = h->getKeyValueExistence(key, exists);
      if (!exists) {
         xsink->raiseException("LISTHASHITERATOR-ERROR", "key '%s' does not exist in the hash at element " QSD " (starting with 0)", key, index());
         return 0;
      }
      return n ? n->refSelf() : 0;
   }

public:
   DLLLOCAL QoreListHashIterator(const QoreListNode* n_l) : QoreListIterator(n_l) {
   }

   DLLLOCAL QoreListHashIterator(const QoreListHashIterator& old) : QoreListIterator(old) {
   }

   DLLLOCAL AbstractQoreNode* getReferencedKeyValue(const char* key, ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      const QoreHashNode* h = checkHash(xsink);
      if (!h)
         return 0;
      return getReferencedKeyValueIntern(h, key, xsink);
   }

   DLLLOCAL QoreHashNode* getRow(ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      const QoreHashNode* h = checkHash(xsink);
      return h ? h->hashRefSelf() : 0;
   }

   DLLLOCAL QoreHashNode* getSlice(const QoreListNode* args, ExceptionSink* xsink) const {
      if (checkPtr(xsink))
         return 0;

      const QoreHashNode* h = checkHash(xsink);
      if (!h)
         return 0;

      ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);

      ConstListIterator li(args);
      while (li.next()) {
         QoreStringValueHelper str(li.getValue(), QCS_UTF8, xsink);
         if (*xsink)
            return 0;
         const char* key = str->getBuffer();
         AbstractQoreNode* n = getReferencedKeyValueIntern(h, key, xsink);
         if (*xsink)
            return 0;
         rv->setKeyValue(key, n, xsink);
         // cannot have an exception here
         assert(!*xsink);
      }

      return rv.release();
   }

   DLLLOCAL virtual const char* getName() const {
      return "ListHashIterator";
   }
};

#endif // _QORE_QORELISTHASHITERATOR_H
