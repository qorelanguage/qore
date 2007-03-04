/*
  QT_boolean.cc

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
#include <qore/QT_boolean.h>

class QoreNode *boolean_DefaultValue()
{
   False->ref();
   return False;
}

class QoreNode *boolean_ConvertTo(class QoreNode *n, class ExceptionSink *xsink)
{
   class QoreNode *rv;

   if (n->type == NT_INT)
      rv = n->val.intval ? boolean_true() : boolean_false();
   else if (n->type == NT_FLOAT)
      rv = n->val.floatval ? boolean_true() : boolean_false();
   else if (n->type == NT_STRING)
      rv = strtoll(n->val.String->getBuffer(), NULL, 10) ? boolean_true() : boolean_false();
   else if (n->type == NT_DATE)
      rv = n->val.date_time->getEpochSeconds() ? boolean_true() : boolean_false();
   else
      rv = boolean_false();

   return rv;
}

bool boolean_Compare(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink)
{
   return (bool)(l->val.boolval != r->val.boolval);
}

class QoreString *boolean_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   return new QoreString((char *)(n->val.boolval ? "True" : "False"));
}
