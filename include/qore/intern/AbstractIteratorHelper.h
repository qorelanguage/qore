/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 AbstractIteratorHelper.h
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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

#ifndef _QORE_ABSTRACTITERATORHELPER_H

#define _QORE_ABSTRACTITERATORHELPER_H

#include <qore/intern/QoreClassIntern.h>

extern QoreClass* QC_ABSTRACTITERATOR;
extern QoreClass* QC_ABSTRACTBIDIRECTIONALITERATOR;

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
      nextMethod = qore_class_private::get(*o->getClass())->findMethod(fwd ? "next" : "prev", priv);
      // method must be found because we have an instance of AbstractIterator/AbstractBidirectionalIterator
      assert(nextMethod);
      nextVariant = getCheckVariant(op, nextMethod, xsink);
      if (!nextVariant)
         return;
      if (get_value) {
         getValueMethod = qore_class_private::get(*o->getClass())->findMethod("getValue", priv);
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
