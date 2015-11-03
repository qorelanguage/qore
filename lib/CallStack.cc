/*
  CallStack.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/CallStack.h>

// a read-write lock is used in an inverted fashion to provide thread-safe
// access to call stacks: writing to each call stack is performed within
// the read lock, reading all threads' stacks is performed in the write lock
#include <qore/QoreRWLock.h>

QoreRWLock thread_stack_lock;

CallNode::CallNode(const char *f, int t, QoreObject *o) {
   func = f;
   type = t;
   file_name   = get_pgm_file();
   get_pgm_counter(start_line, end_line);
   obj = o;
   if (obj)
      obj->ref();
#ifdef DEBUG
   if (obj)
      printd(5, "CallNode::CallNode() pushing class=%08p '%s' (name=%08p) obj=%08p\n", obj->getClass(), obj->getClass()->getName(), obj->getClass()->getName(), obj);
#endif
}

void CallNode::objectDeref(ExceptionSink *xsink) {
   if (obj) {
      printd(5, "CallNode::~CallNode() popping class=%s obj=%08p\n", obj->getClass()->getName(), obj);
      // deref object
      obj->deref(xsink);
   }
}

extern char *file_names[];
QoreHashNode *CallNode::getInfo() const {
   QoreHashNode *h = new QoreHashNode();
   // FIXME: add class name
   QoreStringNode *str = new QoreStringNode();
   if (obj) {
      str->concat(obj->getClass()->getName());
      str->concat("::");
   }
   str->concat(func);

   h->setKeyValue("function", str, 0);
   h->setKeyValue("line",     new QoreBigIntNode(start_line), 0);
   h->setKeyValue("endline",  new QoreBigIntNode(end_line), 0);
   h->setKeyValue("file",     new QoreStringNode(file_name), 0);
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
      class CallNode *c = tail->prev;
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
   QoreListNode *l = new QoreListNode();
   CallNode *c = tail;
   while (c) {
      l->push(c->getInfo());
      c = c->prev;
   }
   return l;
}

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
   QoreObject *ro = tail->obj;
   tail->obj = o;
   return ro;
}
