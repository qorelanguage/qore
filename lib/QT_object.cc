/*
  QT_object.cc

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

#include <qore/config.h>
#include <qore/QoreType.h>
#include <qore/QT_object.h>
#include <qore/Object.h>

// there is no object_DeleteContents, because they will be taken care of by QoreNode::~QoreNode

/*
class QoreNode *object_Eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->val.object->doCopy(xsink);
}
*/

class QoreNode *object_Copy(class QoreNode *n, class ExceptionSink *xsink)
{
   return n->RefSelf();
}

bool object_Compare(class QoreNode *l, class QoreNode *r)
{
   return l->val.object->compareHard(r->val.object);
}

class QoreString *object_MakeString(class QoreNode *n, int foff, class ExceptionSink *xsink)
{
   Hash *h = n->val.object->evalData(xsink);
   if (xsink->isEvent())
   {
      if (h)
      {
	 h->derefAndDelete(xsink);
	 return NULL;
      }
   }
   QoreString *rv = new QoreString();
   rv->sprintf("class %s: ", n->val.object->getClass()->getName());

   if (foff != FMT_NONE)
   {
      n->val.object->addPrivateDataToString(rv);
      rv->concat(' ');
   }
   if (!h->size())
      rv->concat("<NO MEMBERS>");
   else
   {
      if (foff != FMT_NONE)
	 rv->sprintf(" (%d member%s)\n", h->size(), h->size() == 1 ? "" : "s");
      else
	 rv->concat('(');

      class HashIterator hi(h);
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
	    rv->addch(' ', foff + 2);

	 QoreString *elem = hi.getValue()->getAsString(foff != FMT_NONE ? foff + 2 : foff, xsink);
	 rv->sprintf("%s : %s", hi.getKey(), elem->getBuffer());
	 delete elem;
      }
      if (foff == FMT_NONE)
	 rv->concat(')');
   }
   h->derefAndDelete(xsink);

   return rv;
}
