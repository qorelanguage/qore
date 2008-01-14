/*
  QT_hash.cc

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

#include <qore/Qore.h>
#include <qore/intern/QT_hash.h>

class QoreNode *hash_DefaultValue()
{
   emptyHash->ref();
   return emptyHash;
}

class QoreNode *hash_ConvertTo(const QoreNode *n, class ExceptionSink *xsink)
{
   if (n && n->type == NT_OBJECT)
   {
      class QoreHash *h = n->val.object->evalData(xsink);
      if (!h)
	 return NULL;
      return new QoreNode(h);
   }
   return new QoreNode(new QoreHash());
}

bool hash_needs_eval(const QoreNode *n)
{
   return n->val.hash->needsEval();
}

class QoreNode *hash_Eval(const QoreNode *l, class ExceptionSink *xsink)
{
   if (!l->val.hash->needsEval())
      return l->RefSelf();

   return new QoreNode(l->val.hash->eval(xsink));
}

class QoreNode *hash_eval_opt_deref(bool &needs_deref, const QoreNode *n, class ExceptionSink *xsink)
{
   if (!n->val.hash->needsEval())
   {
      needs_deref = false;
      return const_cast<QoreNode *>(n);
   }

   needs_deref = true;
   return new QoreNode(n->val.hash->eval(xsink));
}

class QoreNode *hash_Copy(const QoreNode *l, class ExceptionSink *xsink)
{
   return new QoreNode(l->val.hash->copy());
}

bool hash_Compare(const QoreNode *l, const QoreNode *r, class ExceptionSink *xsink)
{
   return l->val.hash->compareHard(r->val.hash, xsink);
}

void hash_DeleteContents(class QoreNode *n)
{
   n->val.hash->derefAndDelete(NULL);
}
