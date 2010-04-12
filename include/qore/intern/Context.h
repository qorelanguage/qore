/*
  Context.h

  Qore Programming Language

  Copyright (C) David Nichols 2004 - 2010

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

#ifdef _QORE_LIB_INTERN

#include <qore/common.h>

#define CM_WHERE_NODE           1
#define CM_SORT_ASCENDING       2
#define CM_SORT_DESCENDING      3
#define CM_SUMMARIZE_BY         4

class Context {
   DLLLOCAL void Sort(AbstractQoreNode *sort, int sort_type = CM_SORT_ASCENDING);

   ExceptionSink *sort_xsink;

protected:
   DLLLOCAL ~Context();

public:
   char *name;
   QoreHashNode *value;
   // master row list needed for summary contexts
   int master_max_pos;
   int *master_row_list;
   int max_pos;
   int pos;
   int *row_list;
   int group_pos;
   int max_group_pos;
   struct node_row_list_s *group_values;
   Context *next;
   int sub;
   
   DLLLOCAL Context(char *nme, ExceptionSink *xsinkx, AbstractQoreNode *exp,
		    AbstractQoreNode *cond = NULL,
		    int sort_type = -1, AbstractQoreNode *sort = NULL,
		    AbstractQoreNode *summary = NULL, int ignore_key = 0);
   DLLLOCAL AbstractQoreNode *evalValue(char *field, ExceptionSink *xsink);
   DLLLOCAL QoreHashNode *getRow(ExceptionSink *xsink);
   DLLLOCAL int next_summary();
   DLLLOCAL int check_condition(AbstractQoreNode *cond, ExceptionSink *xsinkx);
   DLLLOCAL void deref(ExceptionSink *xsink);
};

DLLLOCAL AbstractQoreNode *evalContextRef(char *key, ExceptionSink *xsink);
DLLLOCAL AbstractQoreNode *evalContextRow(ExceptionSink *xsink);

#endif

#endif
