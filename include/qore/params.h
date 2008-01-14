/*
  params.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#ifndef _QORE_PARAMS_H

#define _QORE_PARAMS_H

#include <qore/QoreNode.h>

static inline int num_params(const QoreNode *n)
{
   if (!n)
      return 0;
   return n->val.list->size();
}

static inline QoreNode *get_param(const QoreNode *n, int i)
{
   if (!n) return NULL;
   class QoreNode *p = n->val.list->retrieve_entry(i);
   return is_nothing(p) ? NULL : p;
}

static inline QoreNode *test_param(const QoreNode *n, class QoreType *type, int i)
{
   if (!n) return NULL;
   QoreNode *p = n->val.list->retrieve_entry(i);
   if (is_nothing(p)) return NULL;
   return (p->type == type) ? p : NULL;
}

static inline QoreStringNode *test_string_param(const QoreNode *n, int i)
{
   if (!n) return 0;
   QoreNode *p = n->val.list->retrieve_entry(i);
   if (!p) return 0;
   return dynamic_cast<QoreStringNode *>(p);
}

/*
// this will return only valid objects of the passed class ID
// if an object is returned it will be locked and the caller
// is reponsible for releasing the lock (exiting the gate)
static inline QoreNode *test_object_param(const QoreNode *n, int cid, int i, class RMutex **gp)
{
   if (!n) return NULL;
   QoreNode *p = n->val.list->retrieve_entry(i);
   if (!p) return NULL;
   return (p->type == NT_OBJECT && p->val.object->validInstanceOf(cid, gp)) ? p : NULL;
}
*/

static inline int test_nothing_param(const QoreNode *n, int i)
{
   if (!n) return 1;
   return is_nothing(n->val.list->retrieve_entry(i));
}

#endif
