/*
  QoreNode.cc

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
#include <qore/QoreNode.h>
#include <qore/Variable.h>
#include <qore/support.h>
#include <qore/List.h>
#include <qore/Hash.h>

static inline QoreNode *crlr_list_copy(QoreNode *n, ExceptionSink *xsink)
{
   // if it's not an immediate list, then there can't be any
   // variable references in it at any level, so return copy
   if (!n->val.list->needs_eval)
      return n->RefSelf();

   // otherwise process each list element
   List *l = new List(1);
   for (int i = 0; i < n->val.list->size(); i++)
      l->push(copy_and_resolve_lvar_refs(n->val.list->retrieve_entry(i), xsink));
   return new QoreNode(l);   
}

static inline QoreNode *crlr_hash_copy(QoreNode *n, ExceptionSink *xsink)
{
   // if it's not an immediate hash, then there can't be any
   // variable references in it at any level, so return copy
   if (!n->val.hash->needs_eval)
      return n->RefSelf();

   Hash *h = new Hash(1);
   HashIterator hi(n->val.hash);
   while (hi.next())
      h->setKeyValue(hi.getKey(), copy_and_resolve_lvar_refs(hi.getValue(), xsink), xsink);
   return new QoreNode(h);
}

static inline QoreNode *crlr_tree_copy(QoreNode *n, ExceptionSink *xsink)
{
   QoreNode *nn = new QoreNode(NT_TREE);
   nn->val.tree.op = n->val.tree.op;
   nn->val.tree.left = copy_and_resolve_lvar_refs(n->val.tree.left, xsink);
   if (n->val.tree.right)
      nn->val.tree.right = copy_and_resolve_lvar_refs(n->val.tree.right, xsink);
   else
      nn->val.tree.right = NULL;
   return nn;
}

static inline QoreNode *crlr_fcall_copy(QoreNode *n, ExceptionSink *xsink)
{
   QoreNode *nn = new QoreNode(NT_FUNCTION_CALL);
   QoreNode *na;
   if (n->val.fcall->args)
      na = copy_and_resolve_lvar_refs(n->val.fcall->args, xsink);
   else
      na = NULL;

   switch (n->val.fcall->type)
   {
      case FC_USER:
	 nn->val.fcall = new FunctionCall(n->val.fcall->f.ufunc, na);
	 break;
      case FC_BUILTIN:
	 nn->val.fcall = new FunctionCall(n->val.fcall->f.bfunc, na);
	 break;
      case FC_SELF:
	 nn->val.fcall = new FunctionCall(n->val.fcall->f.sfunc->func, na);
	 break;
      case FC_UNRESOLVED:
	 nn->val.fcall = new FunctionCall(strdup(n->val.fcall->f.c_str), na);
	 break;
      case FC_IMPORTED:
	 nn->val.fcall = new FunctionCall(n->val.fcall->f.ifunc->pgm, n->val.fcall->f.ifunc->func, na);
	 break;
      case FC_METHOD:
	 nn->val.fcall = new FunctionCall(strdup(n->val.fcall->f.c_str), na);
	 nn->val.fcall->type = FC_METHOD;
	 break;
   }
   return nn;
}

static inline class QoreNode *eval_notnull(class QoreNode *n, ExceptionSink *xsink)
{
   n = n->eval(xsink);
   if (!xsink->isEvent() && !n)
      return nothing();
   return n;
}

class QoreNode *copy_and_resolve_lvar_refs(class QoreNode *n, ExceptionSink *xsink)
{
   if (!n) return NULL;

   if (n->type == NT_LIST)
      return crlr_list_copy(n, xsink);

   if (n->type == NT_HASH)
      return crlr_hash_copy(n, xsink);

   if (n->type == NT_TREE)
      return crlr_tree_copy(n, xsink);

   if (n->type == NT_FUNCTION_CALL)
      return crlr_fcall_copy(n, xsink);

   // must make sure to return a value here or it could cause a segfault - parse expressions expect non-NULL values for the operands
   if (n->type == NT_FIND || n->type == NT_SELF_VARREF)
      return eval_notnull(n, xsink);

   if (n->type == NT_VARREF && n->val.vref->type == VT_LOCAL)
      return eval_notnull(n, xsink);

   return n->RefSelf();
}
