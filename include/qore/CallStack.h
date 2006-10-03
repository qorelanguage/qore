/*
  CallStack.h

  QORE programming language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_CALLSTACK_H

#define _QORE_CALLSTACK_H

#define CT_USER      0
#define CT_BUILTIN   1
#define CT_NEWTHREAD 2
#define CT_RETHROW   3

class CallNode {
   public:
      char *func;
      char *file_name;
      int line_number;
      int type;
      class Object *obj;
      class CallNode *next;
      class CallNode *prev;

      inline CallNode(char *f, int t, class Object *o);
      inline void objectDeref(class ExceptionSink *xsink);
      inline class Hash *getInfo();
};

class CallStack {
   private:
      class CallNode *tail;

   public:
      inline CallStack();
      inline ~CallStack();
      inline void push(char *f, int t, class Object *o);
      inline void pop(class ExceptionSink *xsink);
      inline class List *getCallStack();
      inline class Object *getStackObject()
      {
	 if (!tail)
	    return NULL;
	 return tail->obj;
      }
#ifdef DEBUG
      class Object *getPrevStackObject();
#endif
      inline void substituteObjectIfEqual(class Object *o);
      inline class Object *substituteObject(class Object *o)
      {
	 class Object *ro = tail->obj;
	 tail->obj = o;
	 return ro;
      }
      inline bool inMethod(char *name, class Object *o)
      {
	 if (!tail)
	    return false;
	 return tail->func == name && tail->obj == o;
      }
};

#include <qore/qore_thread.h>
#include <qore/QoreType.h>
#include <qore/QoreString.h>
#include <qore/List.h>
#include <qore/Hash.h>
#include <qore/support.h>
#include <qore/QoreClass.h>
#ifdef DEBUG
#include <qore/Object.h>
#endif

inline CallNode::CallNode(char *f, int t, class Object *o)
{
   func = f;
   type = t;
   file_name   = get_pgm_file();
   line_number = get_pgm_counter();
   obj = o;
   if (obj)
      obj->ref();
#ifdef DEBUG
   if (obj)
      printd(5, "CallNode::CallNode() pushing class=%08p '%s' (name=%08p) obj=%08p\n", obj->getClass(), obj->getClass()->getName(), obj->getClass()->getName(), obj);
#endif
}

inline void CallNode::objectDeref(class ExceptionSink *xsink)
{
   if (obj)
   {
      printd(5, "CallNode::~CallNode() popping class=%s obj=%08p\n", obj->getClass()->getName(), obj);
      // deref object
      obj->dereference(xsink);
   }
}

extern char *file_names[];
inline class Hash *CallNode::getInfo()
{
   class Hash *h = new Hash();
   // FIXME: add class name
   class QoreString *str = new QoreString();
   if (obj)
   {
      str->concat(obj->getClass()->getName());
      str->concat("::");
   }
   str->concat(func);

   h->setKeyValue("function", new QoreNode(str), NULL);
   h->setKeyValue("line",     new QoreNode((int64)line_number), NULL);
   h->setKeyValue("file",     new QoreNode(file_name), NULL);
   h->setKeyValue("typecode", new QoreNode((int64)type), NULL);
   // CT_RETHROW is only aded manually
   switch (type)
   {
      case CT_USER:
	 h->setKeyValue("type",  new QoreNode("user"), NULL);
	 break;
      case CT_BUILTIN:
	 h->setKeyValue("type",  new QoreNode("builtin"), NULL);
	 break;
      case CT_NEWTHREAD:
	 h->setKeyValue("type",  new QoreNode("new-thread"), NULL);
	 break;
   }
   return h;
}

inline CallStack::CallStack()
{
   tail = NULL;
}

inline CallStack::~CallStack()
{
   while (tail)
   {
      class CallNode *c = tail->prev;
      delete tail;
      tail = c;
   }
}

inline void CallStack::push(char *f, int t, class Object *o)
{
   tracein("CallStack::push()");
   CallNode *c = new CallNode(f, t, o);
   c->next = NULL;
   c->prev = tail;
   if (tail)
      tail->next = c;
   tail = c;
   traceout("CallStack::push()");
}

inline void CallStack::pop(class ExceptionSink *xsink)
{
   tracein("CallStack::pop()");
   CallNode *c = tail;
   tail = tail->prev;
   if (tail)
      tail->next = NULL;
   c->objectDeref(xsink);
   delete c;
   traceout("CallStack::pop()");
}

inline class List *CallStack::getCallStack()
{
   class List *l = new List();
   CallNode *c = tail;
   while (c)
   {
      l->push(new QoreNode(c->getInfo()));
      c = c->prev;
   }
   return l;
}

inline void CallStack::substituteObjectIfEqual(class Object *o)
{
   if (!tail->obj && tail->prev && tail->prev->obj == o)
   {
      tail->obj = o;
      o->ref();
   }
}

#endif // _QORE_CALLSTACK_H
