/*
  CallStack.cc

  Qore Programming Language

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

#include <qore/config.h>
#include <qore/common.h>

#include <qore/qore_thread.h>
#include <qore/QoreType.h>
#include <qore/QoreString.h>
#include <qore/List.h>
#include <qore/Hash.h>
#include <qore/support.h>
#include <qore/QoreClass.h>
#include <qore/Object.h>

CallNode::CallNode(char *f, int t, class Object *o)
{
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

void CallNode::objectDeref(class ExceptionSink *xsink)
{
   if (obj)
   {
      printd(5, "CallNode::~CallNode() popping class=%s obj=%08p\n", obj->getClass()->getName(), obj);
      // deref object
      obj->dereference(xsink);
   }
}

extern char *file_names[];
class Hash *CallNode::getInfo() const
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
   h->setKeyValue("line",     new QoreNode((int64)start_line), NULL);
   h->setKeyValue("endline",  new QoreNode((int64)end_line), NULL);
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

CallStack::CallStack()
{
   tail = NULL;
}

CallStack::~CallStack()
{
   while (tail)
   {
      class CallNode *c = tail->prev;
      delete tail;
      tail = c;
   }
}

void CallStack::push(char *f, int t, class Object *o)
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

void CallStack::pop(class ExceptionSink *xsink)
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

class List *CallStack::getCallStack() const
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

void CallStack::substituteObjectIfEqual(class Object *o)
{
   if (!tail->obj && tail->prev && tail->prev->obj == o)
   {
      tail->obj = o;
      o->ref();
   }
}

class Object *CallStack::getStackObject() const
{
   if (!tail)
      return NULL;
   return tail->obj;
}

class Object *CallStack::substituteObject(class Object *o)
{
   class Object *ro = tail->obj;
   tail->obj = o;
   return ro;
}

bool CallStack::inMethod(char *name, class Object *o) const
{
   if (!tail)
      return false;
   return tail->func == name && tail->obj == o;
}

class Object *CallStack::getPrevStackObject()
{
   class CallNode *w = tail;
   while (w)
   {
      if (w->obj)
         return w->obj;
      w = w->prev;
   }
   return NULL;
}
