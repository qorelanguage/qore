/*
  QT_binary.cc

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
#include <qore/intern/QT_binary.h>

bool binary_Compare(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink)
{
   return (bool)l->val.bin->compare(r->val.bin);
}

class QoreString *binary_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   QoreString *rv = new QoreString();
   rv->sprintf("binary object %08p (%d byte%s)", n->val.bin->getPtr(), n->val.bin->size(), n->val.bin->size() == 1 ? "" : "s");
   return rv;
}

void binary_DeleteContents(class QoreNode *n)
{
   delete n->val.bin;
}

class QoreNode *binary_Copy(class QoreNode *n, class ExceptionSink *xsink)
{
   return new QoreNode(n->val.bin->copy());
}
