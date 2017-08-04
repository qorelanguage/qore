/*
  CallStack.cpp

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
#include "qore/intern/QoreHashNodeIntern.h"

// a read-write lock is used in an inverted fashion to provide thread-safe
// access to call stacks: writing to each call stack is performed within
// the read lock, reading all threads' stacks is performed in the write lock
#include <qore/QoreRWLock.h>

#ifdef _Q_WINDOWS
QoreRWLock *thread_stack_lock;
#else
QoreRWLock thread_stack_lock;
#endif

CallNode::CallNode(const char *f, int t, QoreObject* o, const qore_class_private* c) : func(f), loc(RunTimeLocation), type(t), obj(o), cls(c) {
}

QoreHashNode* CallNode::getInfo() const {
   QoreHashNode* h = new QoreHashNode(hashdeclCallStackInfo, nullptr);
   QoreStringNode* str = new QoreStringNode;
   if (cls) {
      str->concat(cls->name.c_str());
      str->concat("::");
   }
   str->concat(func);

   qore_hash_private* ph = qore_hash_private::get(*h);

   ph->setKeyValueIntern("function", str);
   ph->setKeyValueIntern("line",     new QoreBigIntNode(loc.start_line));
   ph->setKeyValueIntern("endline",  new QoreBigIntNode(loc.end_line));
   ph->setKeyValueIntern("file",     new QoreStringNode(loc.file));
   ph->setKeyValueIntern("source",   loc.source ? new QoreStringNode(loc.source) : 0);
   ph->setKeyValueIntern("offset",   new QoreBigIntNode(loc.offset));
   ph->setKeyValueIntern("typecode", new QoreBigIntNode(type));
   // CT_RETHROW is only aded manually
   switch (type) {
      case CT_USER:
         ph->setKeyValueIntern("type",  new QoreStringNode("user"));
         break;
      case CT_BUILTIN:
         ph->setKeyValueIntern("type",  new QoreStringNode("builtin"));
         break;
      case CT_NEWTHREAD:
         ph->setKeyValueIntern("type",  new QoreStringNode("new-thread"));
         break;
   }
   return h;
}

CallStack::CallStack() {
   tail = 0;
}

CallStack::~CallStack() {
   while (tail) {
      CallNode *c = tail->prev;
      delete tail;
      tail = c;
   }
}

void CallStack::push(CallNode *c) {
   QORE_TRACE("CallStack::push()");
   c->next = 0;
   c->prev = tail;
   QoreAutoRWReadLocker l(thread_stack_lock);
   if (tail)
      tail->next = c;
   tail = c;
}

void CallStack::pop(ExceptionSink *xsink) {
   QORE_TRACE("CallStack::pop()");
   QoreAutoRWReadLocker l(thread_stack_lock);
   tail = tail->prev;
   if (tail)
      tail->next = 0;
}

QoreListNode* CallStack::getCallStack() const {
   QoreListNode* l = new QoreListNode(hashdeclCallStackInfo->getTypeInfo());
   CallNode* c = tail;
   while (c) {
      l->push(c->getInfo());
      c = c->prev;
   }
   return l;
}

/*
void CallStack::substituteObjectIfEqual(QoreObject *o) {
   if (!tail->obj && tail->prev && tail->prev->obj == o) {
      tail->obj = o;
      o->ref();
   }
}

QoreObject *CallStack::getStackObject() const {
   if (!tail)
      return 0;
   return tail->obj;
}

QoreObject *CallStack::substituteObject(QoreObject *o) {
   QoreObject *ro = tail->obj);
   tail->obj = o;
   return ro;
}
*/
