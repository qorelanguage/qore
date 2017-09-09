/*
  ReferenceArgumentHelper.cpp

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

#include <qore/Qore.h>

#include <qore/ReferenceArgumentHelper.h>

struct lvih_intern {
   LocalVar lv;
   ExceptionSink* xsink;
   ReferenceNode* ref;

   DLLLOCAL lvih_intern(AbstractQoreNode* val, const QoreTypeInfo* typeInfo, ExceptionSink* xs) : lv("ref_arg_helper", typeInfo), xsink(xs) {
      printd(5, "ReferenceArgumentHelper::ReferenceArgumentHelper() instantiating %p (val: %p type: '%s') \n", &lv, val, val ? val->getTypeName() : "n/a");
      lv.instantiate(val);
      VarRefNode* vr = new VarRefNode(get_runtime_location(), strdup("ref_arg_helper"), VT_LOCAL);
      vr->ref.id = &lv;
      ref = new ReferenceNode(vr, typeInfo, nullptr, vr, nullptr);
   }

   DLLLOCAL ~lvih_intern() {
      ref->deref(nullptr);
      lv.uninstantiate(xsink);
   }

   DLLLOCAL AbstractQoreNode* getOutputValue() {
      // there will be no locking here, because it's our temporary local "variable"
      ExceptionSink xsink2;
      LValueRemoveHelper vp(ref, &xsink2, true);

      // no exception should be possible here
      assert(!xsink2);
      if (!vp)
         return nullptr;

      // take output value from our temporary "variable" and return it
      return vp.removeNode();
   }

   DLLLOCAL AbstractQoreNode* getArg() {
      return ref->refSelf();
   }
};

ReferenceArgumentHelper::ReferenceArgumentHelper(AbstractQoreNode *val, ExceptionSink *xsink) : priv(new lvih_intern(val, nullptr, xsink)) {
}

ReferenceArgumentHelper::ReferenceArgumentHelper(AbstractQoreNode *val, const QoreTypeInfo* typeInfo, ExceptionSink *xsink) : priv(new lvih_intern(val, typeInfo, xsink)) {
}

ReferenceArgumentHelper::~ReferenceArgumentHelper() {
   delete priv;
}

AbstractQoreNode* ReferenceArgumentHelper::getArg() const {
   return priv->getArg();
}

AbstractQoreNode* ReferenceArgumentHelper::getOutputValue() {
   return priv->getOutputValue();
}
