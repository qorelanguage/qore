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

static inline int num_params(const QoreListNode *n)
{
   if (!n)
      return 0;
   return n->size();
}

static inline QoreNode *get_param(const QoreListNode *n, int i)
{
   if (!n) return NULL;
   class QoreNode *p = n->retrieve_entry(i);
   return is_nothing(p) ? NULL : p;
}

static inline QoreNode *test_param(const QoreListNode *n, class QoreType *type, int i)
{
   if (!n) return NULL;
   QoreNode *p = n->retrieve_entry(i);
   if (is_nothing(p)) return NULL;
   return (p->type == type) ? p : NULL;
}

static inline BinaryNode *test_binary_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<BinaryNode *>(n->retrieve_entry(i));
}

static inline QoreStringNode *test_string_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<QoreStringNode *>(n->retrieve_entry(i));
}

static inline QoreObject *test_object_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<QoreObject *>(n->retrieve_entry(i));
}

static inline DateTimeNode *test_date_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<DateTimeNode *>(n->retrieve_entry(i));
}

static inline QoreHashNode *test_hash_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<QoreHashNode *>(n->retrieve_entry(i));
}

static inline QoreListNode *test_list_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<QoreListNode *>(n->retrieve_entry(i));
}

static inline ResolvedFunctionReferenceNode *test_funcref_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<ResolvedFunctionReferenceNode *>(n->retrieve_entry(i));
}

static inline ReferenceNode *test_reference_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<ReferenceNode *>(n->retrieve_entry(i));
}

/*
// this will return only valid objects of the passed class ID
// if an object is returned it will be locked and the caller
// is reponsible for releasing the lock (exiting the gate)
static inline QoreNode *test_object_param(const QoreListNode *n, int cid, int i, class RMutex **gp)
{
   if (!n) return NULL;
   QoreNode *p = n->retrieve_entry(i);
   if (!p) return NULL;
   return (p->type == NT_OBJECT && p->val.object->validInstanceOf(cid, gp)) ? p : NULL;
}
*/

static inline bool test_nothing_param(const QoreListNode *n, int i)
{
   if (!n) return true;
   return is_nothing(n->retrieve_entry(i));
}

#endif
