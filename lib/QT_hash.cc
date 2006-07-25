/*
  QT_hash.cc

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
#include <qore/QoreType.h>
#include <qore/QT_hash.h>
#include <qore/Hash.h>

class QoreNode *hash_DefaultValue()
{
   emptyHash->ref();
   return emptyHash;
}

class QoreNode *hash_ConvertTo(class QoreNode *n, class ExceptionSink *xsink)
{
   if (n && n->type == NT_OBJECT)
   {
      class Hash *h = n->val.object->evalData(xsink);
      if (!h)
	 return NULL;
      return new QoreNode(h);
   }
   return new QoreNode(new Hash());
}

class QoreNode *hash_Eval(class QoreNode *l, class ExceptionSink *xsink)
{
   if (!l->val.hash->needs_eval)
      return l->RefSelf();

   return new QoreNode(l->val.hash->eval(xsink));
}

class QoreNode *hash_Copy(class QoreNode *l, class ExceptionSink *xsink)
{
   return new QoreNode(l->val.hash->eval(xsink));
}

bool hash_Compare(class QoreNode *l, class QoreNode *r)
{
   return l->val.hash->compareHard(r->val.hash);
}

class QoreString *hash_MakeString(class QoreNode *n, int foff, class ExceptionSink *xsink)
{
   QoreString *rv = new QoreString();
   if (!n->val.hash->size())
      rv->concat("<EMPTY HASH>");
   else
   {
      rv->concat("hash: ");
      if (foff != FMT_NONE)
	 rv->sprintf("(%d member%s)\n", n->val.hash->size(), n->val.hash->size() == 1 ? "" : "s");
      else
	 rv->concat('(');

      class HashIterator hi(n->val.hash);
      //class List *l = n->val.hash->getKeys();                                                                                                                       
      bool first = false;
      while (hi.next())
      {
	 if (first)
	    if (foff != FMT_NONE)
	       rv->concat('\n');
	    else
	       rv->concat(", ");
	 else
	    first = true;

	 if (foff != FMT_NONE)
	    indent(rv, foff + 2);

	 QoreString *elem = hi.getValue()->getAsString(foff != FMT_NONE ? foff + 2 : foff, xsink);
	 rv->sprintf("%s : %s", hi.getKey(), elem->getBuffer());
	 delete elem;
      }
	 
      if (foff == FMT_NONE)
	 rv->concat(')');
   }
   return rv;
}

void hash_DeleteContents(class QoreNode *n)
{
   delete n->val.hash;
}
