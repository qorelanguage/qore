/*
  QT_string.cc

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
#include <qore/QT_string.h>
#include <qore/QoreString.h>

class QoreNode *string_DefaultValue()
{
   NullString->ref();
   return NullString;
}

class QoreNode *string_Copy(class QoreNode *n, class ExceptionSink *xsink)
{
   return new QoreNode(n->val.String->copy());
}

class QoreNode *string_ConvertTo(class QoreNode *n, class ExceptionSink *xsink)
{
   class QoreNode *rv;
   
   if (n->type == NT_INT)
      rv = new QoreNode(new QoreString(n->val.intval));
   else if (n->type == NT_FLOAT)
      rv = new QoreNode(new QoreString(n->val.floatval));
   else if (n->type == NT_DATE)
      rv = new QoreNode(new QoreString(n->val.date_time));
   else if (n->type == NT_BOOLEAN)
      rv = new QoreNode(new QoreString(n->val.boolval));
   else
   {
      NullString->ref();
      rv = NullString;
   }

   return rv;
}

bool string_Compare(class QoreNode *l, class QoreNode *r)
{
   return l->val.String->compare(r->val.String);
}

class QoreString *string_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   QoreString *rv = new QoreString(n->val.String->getEncoding(), "");
   rv->sprintf("\"%s\"", n->val.String->getBuffer());
   return rv;
}

void string_DeleteContents(class QoreNode *n)
{
   delete n->val.String;
}
