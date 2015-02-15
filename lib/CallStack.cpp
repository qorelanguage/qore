/*
  CallStack.cpp

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

// a read-write lock is used in an inverted fashion to provide thread-safe
// access to call stacks: writing to each call stack is performed within
// the read lock, reading all threads' stacks is performed in the write lock
#include <qore/QoreRWLock.h>

#ifdef _Q_WINDOWS
QoreRWLock *thread_stack_lock;
#else
QoreRWLock thread_stack_lock;
#endif

CallNode::CallNode(const char *f, int t, ClassObj o) : func(f), loc(RunTimeLocation), type(t), obj(o) {
   QoreObject *qo = o.getObj();
   if (qo) {
      qo->ref();
#ifdef DEBUG
      printd(5, "CallNode::CallNode() pushing class=%p '%s' (name=%p) obj=%p\n", qo->getClass(), qo->getClass()->getName(), qo->getClass()->getName(), qo);
#endif
   }
}

void CallNode::objectDeref(ExceptionSink *xsink) {
   QoreObject *qo = obj.getObj();
   if (qo) {
      printd(5, "CallNode::~CallNode() popping class=%s obj=%p\n", qo->getClass()->getName(), qo);
      // deref object
      qo->deref(xsink);
   }
}

QoreHashNode* CallNode::getInfo() const {
   QoreHashNode* h = new QoreHashNode;
   // FIXME: add class name
   QoreStringNode *str = new QoreStringNode;
   if (obj) {
      str->concat(obj.getClass()->name.c_str());
      str->concat("::");
   }
   str->concat(func);

   h->setKeyValue("function", str, 0);
   h->setKeyValue("line",     new QoreBigIntNode(loc.start_line), 0);
   h->setKeyValue("endline",  new QoreBigIntNode(loc.end_line), 0);
   h->setKeyValue("file",     new QoreStringNode(loc.file), 0);
   h->setKeyValue("source",   loc.source ? new QoreStringNode(loc.source) : 0, 0);
   h->setKeyValue("offset",   new QoreBigIntNode(loc.offset), 0);
   h->setKeyValue("typecode", new QoreBigIntNode(type), 0);
   // CT_RETHROW is only aded manually
   switch (type) {
      case CT_USER:
	 h->setKeyValue("type",  new QoreStringNode("user"), 0);
	 break;
      case CT_BUILTIN:
	 h->setKeyValue("type",  new QoreStringNode("builtin"), 0);
	 break;
      case CT_NEWTHREAD:
	 h->setKeyValue("type",  new QoreStringNode("new-thread"), 0);
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
   CallNode *c;
   {
      QoreAutoRWReadLocker l(thread_stack_lock);
      c = tail;
      tail = tail->prev;
      if (tail)
	 tail->next = 0;
   }
   c->objectDeref(xsink);
}

QoreListNode *CallStack::getCallStack() const {
   QoreListNode *l = new QoreListNode;
   CallNode *c = tail;
   while (c) {
      l->push(c->getInfo());
      c = c->prev;
   }
   return l;
}

void CallStack::substituteObjectIfEqual(QoreObject *o) {
   if (!tail->obj.getObj() && tail->prev && tail->prev->obj.getObj() == o) {
      tail->obj = o;
      o->ref();
   }
}

QoreObject *CallStack::getStackObject() const {
   if (!tail)
      return 0;
   return tail->obj.getObj();
}

QoreObject *CallStack::substituteObject(QoreObject *o) {
   QoreObject *ro = tail->obj.getObj();
   tail->obj = o;
   return ro;
}
