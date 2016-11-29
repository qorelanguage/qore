/*
  QoreClosureParseNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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
#include "qore/intern/QoreClassIntern.h"

QoreClosureParseNode::QoreClosureParseNode(UserClosureFunction* n_uf, bool n_lambda) : ParseNode(NT_CLOSURE), uf(n_uf), lambda(n_lambda), in_method(false) {
}

QoreClosureNode* QoreClosureParseNode::evalClosure() const {
   return new QoreClosureNode(this);
}

QoreObjectClosureNode* QoreClosureParseNode::evalObjectClosure() const {
   QoreObject* o;
   const qore_class_private* c_ctx;
   runtime_get_object_and_class(o, c_ctx);
   return new QoreObjectClosureNode(o, c_ctx, this);
}

QoreValue QoreClosureParseNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   return in_method ? (AbstractQoreNode*)evalObjectClosure() : (AbstractQoreNode*)evalClosure();
}

int QoreClosureParseNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.sprintf("parsed closure (%slambda, %p)", lambda ? "" : "non-", this);
   return 0;
}

QoreString* QoreClosureParseNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = true;
   QoreString* rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode* QoreClosureParseNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   if (oflag) {
      in_method = true;
      uf->setClassType(oflag->getTypeInfo());
   }
   uf->parseInit();
   uf->parseCommit();
   typeInfo = runTimeClosureTypeInfo;
   return this;
}

const char* QoreClosureParseNode::getTypeName() const {
   return getStaticTypeName();
}

QoreValue QoreClosureParseNode::exec(const QoreClosureBase& closure_base, QoreProgram* pgm, const QoreListNode* args, QoreObject* self, const qore_class_private* class_ctx, ExceptionSink* xsink) const {
   return uf->evalClosure(closure_base, pgm, args, self, class_ctx, xsink);
}

QoreClosureBase* QoreClosureParseNode::evalBackground(ExceptionSink* xsink) const {
   cvv_vec_t* cvv = thread_get_all_closure_vars();

   if (in_method) {
      QoreObject* o;
      const qore_class_private* c_ctx;
      runtime_get_object_and_class(o, c_ctx);
      return new QoreObjectClosureNode(o, c_ctx, this, cvv);
   }

   return new QoreClosureNode(this, cvv);
}
