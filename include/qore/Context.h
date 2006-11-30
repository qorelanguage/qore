/*
  Context.h

  Qore Programming Language

  Copyright (C) David Nichols 2004

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

#ifndef QORE_CONTEXT_H

#define QORE_CONTEXT_H

#include <qore/common.h>
#include <stdio.h>

#define CM_WHERE_NODE           1
#define CM_NO_SORT              2
#define CM_SORT_ASCENDING       3
#define CM_SORT_DESCENDING      4
#define CM_SUMMARIZE_BY         5

class Context {
      void Sort(class QoreNode *sort, int sort_type = CM_SORT_ASCENDING);
      class ExceptionSink *sort_xsink;

   protected:
      ~Context();

   public:
      char *name;
      class QoreNode *value;
      // master row list needed for summary contexts
      int master_max_pos;
      int *master_row_list;
      int max_pos;
      int pos;
      int *row_list;
      int group_pos;
      int max_group_pos;
      struct node_row_list_s *group_values;
      class Context *next;
      int sub;
      
      Context(char *nme, class ExceptionSink *xsinkx, class QoreNode *exp,
	      class QoreNode *cond = NULL,
	      int sort_type = CM_NO_SORT, class QoreNode *sort = NULL,
	      class QoreNode *summary = NULL, int ignore_key = 0);
      class QoreNode *evalValue(char *field, class ExceptionSink *xsink);
      class QoreNode *getRow(class ExceptionSink *xsink);
      int next_summary();
      int check_condition(class QoreNode *cond, class ExceptionSink *xsinkx);
      inline void deref(class ExceptionSink *xsink);
};

class ComplexContextRef 
{
   public:
      char *name;
      char *member;
      int stack_offset;
      inline ComplexContextRef(char *str); 
      inline ComplexContextRef(char *n, char *m, int so); 
      inline ~ComplexContextRef();
      inline class ComplexContextRef *copy();
};

//static inline class QoreNode *getContextObjectValue(class QoreNode *o, char *member, class ExceptionSink *xsink);
inline class QoreNode *evalContextRef(char *key, class ExceptionSink *xsink);
class QoreNode *evalComplexContextRef(class ComplexContextRef *c, class ExceptionSink *xsink);
inline class QoreNode *evalContextRow(class ExceptionSink *xsink);

#include <qore/Statement.h>
#include <qore/QoreNode.h>
#include <qore/List.h>
#include <qore/Object.h>
#include <qore/support.h>
#include <qore/qore_thread.h>
#include <qore/Exception.h>

#include <stdlib.h>

inline ComplexContextRef::ComplexContextRef(char *str)
{
   char *c = strchr(str, ':');
   *c = '\0';
   name = strdup(str);
   member = strdup(c + 1);
   free(str);
}

inline ComplexContextRef::ComplexContextRef(char *n, char *m, int so)
{
   name = strdup(n);
   member = strdup(m);
   stack_offset = so;
}

inline ComplexContextRef::~ComplexContextRef()
{ 
   free(name); 
   free(member); 
}

inline class ComplexContextRef *ComplexContextRef::copy()
{
   return new ComplexContextRef(name, member, stack_offset);
}

struct node_row_list_s {
      class QoreNode *node;
      int *row_list;
      int num_rows;
      int allocated;
};

class Templist {
  public:
   class QoreNode *node;
   int pos;
};

inline ContextMod::ContextMod(int t, class QoreNode *n)
{
   next = NULL;
   type = t;
   c.exp = n;
}

inline ContextMod::~ContextMod()
{
   if (c.exp)
      c.exp->deref(NULL);
}

#define CONTEXT_MOD_BLOCK 5

inline ContextModList::ContextModList(ContextMod *cm)
{
   head = tail = cm;
   num = 1;
}

inline ContextModList::~ContextModList()
{
   while (head)
   {
      class ContextMod *w = head->next;
      delete head;
      head = w;
   }
}

inline void ContextModList::addContextMod(ContextMod *cm)
{
   tail->next = cm;
   tail = cm;
}

/*
static inline class QoreNode *getContextObjectValue(QoreNode *o, char *member, class ExceptionSink *xsink)
{
   Context *c = get_context_stack();
   while (c)
   {
      if (c->value && *(c->value) == o)
	 return c->evalValue(member, xsink);
      c = c->next;
   }
   return o->val.hash->evalKey(member, xsink);
}
*/
inline int Context::check_condition(class QoreNode *cond, class ExceptionSink *xsinkx)
{
   class QoreNode *val;
   int rc;

   tracein("Context::check_condition()");
   val = cond->eval(xsinkx);
   if (xsinkx->isEvent())
   {
      if (val) val->deref(xsinkx);
      return -1;
   }
   if (val)
   {
      rc = val->getAsInt();
      val->deref(xsinkx);
   }
   else
      rc = 0;
   traceout("Context::check_condition()");
   return rc;
}

inline void Context::deref(class ExceptionSink *xsink)
{
   if (!sub && value)
      value->deref(xsink);
   delete this;
}

inline class QoreNode *evalContextRef(char *key, class ExceptionSink *xsink)
{
   class Context *c = get_context_stack();
   return c->evalValue(key, xsink);
}

inline class QoreNode *evalContextRow(class ExceptionSink *xsink)
{
   class Context *c = get_context_stack();
   return c->getRow(xsink);
}

#endif
