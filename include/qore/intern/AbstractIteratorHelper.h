/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AbstractIteratorHelper.h
 
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

#ifndef _QORE_ABSTRACTITERATORHELPER_H

#define _QORE_ABSTRACTITERATORHELPER_H

#include <qore/intern/QoreClassIntern.h>

class AbstractIteratorHelper {
protected:
   DLLLOCAL static const QoreExternalMethodVariant* getCheckVariant(const char* op, const QoreMethod* m, ExceptionSink* xsink) {
      const MethodVariantBase* variant = reinterpret_cast<const MethodVariantBase*>(m->getFunction()->findVariant(0, false, xsink));
      // this could throw an exception if the variant is builtin and has functional flags not allowed in the current pgm, for example
      if (*xsink)
         return 0;
      // we must have a variant here because we have an instance of AbstractIterator
      assert(variant);
      if (variant->isPrivate()) {
         // check for access to the class holding the private method
         if (!qore_class_private::runtimeCheckPrivateClassAccess(*(variant->method()->getClass()))) {
            QoreString opstr(op);
            opstr.toupr();
            opstr.concat("-ITERATOR-ERROR");
            xsink->raiseException(opstr.getBuffer(), "cannot access private %s::next() iterator method with the %s", variant->method()->getClass()->getName(), op);
            return 0;
         }
      }
      return reinterpret_cast<const QoreExternalMethodVariant*>(variant);
   }

public:
   QoreObject* obj;
   const QoreMethod* nextMethod;
   const QoreExternalMethodVariant* nextVariant;
   const QoreMethod* getValueMethod;
   const QoreExternalMethodVariant* getValueVariant;
   bool valid;

   DLLLOCAL AbstractIteratorHelper(ExceptionSink* xsink, const char* op, QoreObject* o, bool fwd = true, bool get_value = true) : obj(0), nextMethod(0), nextVariant(0), getValueMethod(0), getValueVariant(0), valid(false) {
      bool priv;
      const QoreClass* qc = o->getClass()->getClass(fwd ? *QC_ABSTRACTITERATOR : *QC_ABSTRACTBIDIRECTIONALITERATOR, priv);
      if (!qc)
         return;

      obj = o;
      // get "next" method if accessible
      nextMethod = qore_class_private::get(*o->getClass())->runtimeFindCommittedMethod(fwd ? "next" : "prev", priv);
      // method must be found because we have an instance of AbstractIterator/AbstractBidirectionalIterator
      assert(nextMethod);
      nextVariant = getCheckVariant(op, nextMethod, xsink);
      if (!nextVariant)
         return;
      if (get_value) {
         getValueMethod = qore_class_private::get(*o->getClass())->runtimeFindCommittedMethod("getValue", priv);
         // method must be found because we have an instance of AbstractIterator
         assert(getValueMethod);
         getValueVariant = getCheckVariant(op, getValueMethod, xsink);
         if (!getValueVariant)
            return;
      }
      valid = true;
   }

   DLLLOCAL operator bool() const {
      return valid;
   }

   DLLLOCAL bool next(ExceptionSink* xsink) {
      assert(nextMethod);
      assert(nextVariant);
      return nextMethod->boolEvalNormalVariant(obj, nextVariant, 0, xsink);
   }

   DLLLOCAL AbstractQoreNode* getValue(ExceptionSink* xsink) {
      assert(getValueMethod);
      assert(getValueVariant);
      return getValueMethod->evalNormalVariant(obj, getValueVariant, 0, xsink);
   }

   /*
   DLLLOCAL QoreObject* getReferencedObject() const {
      obj->ref();
      return obj;
   }
   */
};

#endif
