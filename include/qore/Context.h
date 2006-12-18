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

#include <qore/config.h>
#include <qore/common.h>

#define CM_WHERE_NODE           1
#define CM_SORT_ASCENDING       2
#define CM_SORT_DESCENDING      3
#define CM_SUMMARIZE_BY         4

class Context {
      DLLLOCAL void Sort(class QoreNode *sort, int sort_type = CM_SORT_ASCENDING);

      class ExceptionSink *sort_xsink;

   protected:
      DLLLOCAL ~Context();

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
      
      DLLLOCAL Context(char *nme, class ExceptionSink *xsinkx, class QoreNode *exp,
		       class QoreNode *cond = NULL,
		       int sort_type = -1, class QoreNode *sort = NULL,
		       class QoreNode *summary = NULL, int ignore_key = 0);
      DLLLOCAL class QoreNode *evalValue(char *field, class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *getRow(class ExceptionSink *xsink);
      DLLLOCAL int next_summary();
      DLLLOCAL int check_condition(class QoreNode *cond, class ExceptionSink *xsinkx);
      DLLLOCAL inline void deref(class ExceptionSink *xsink);
};

class ComplexContextRef 
{
   public:
      char *name;
      char *member;
      int stack_offset;

      DLLLOCAL ComplexContextRef(char *str); 
      DLLLOCAL ComplexContextRef(char *n, char *m, int so); 
      DLLLOCAL ~ComplexContextRef();
      DLLLOCAL class ComplexContextRef *copy();
};

DLLLOCAL class QoreNode *evalContextRef(char *key, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *evalComplexContextRef(class ComplexContextRef *c, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *evalContextRow(class ExceptionSink *xsink);

#endif
