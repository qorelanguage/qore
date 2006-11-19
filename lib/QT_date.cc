/*
  QT_date.cc

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
#include <qore/QT_date.h>
#include <qore/DateTime.h>

class QoreNode *date_DefaultValue()
{
   ZeroDate->ref();
   return ZeroDate;
}

class QoreNode *date_ConvertTo(class QoreNode *n, class ExceptionSink *xsink)
{
   if (n->type == NT_INT)
      return new QoreNode(new DateTime(n->val.intval));

   if (n->type == NT_FLOAT)
      return new QoreNode(new DateTime((int64)(n->val.floatval)));
   
   if (n->type == NT_STRING)
      return new QoreNode(new DateTime(n->val.String->getBuffer()));
   
   if (n->type == NT_BOOLEAN)
      return new QoreNode(new DateTime((int64)(n->val.boolval)));

   ZeroDate->ref();
   return ZeroDate;
}

class QoreNode *date_Copy(class QoreNode *l, class ExceptionSink *xsink)
{
   return new QoreNode(new DateTime(*(l->val.date_time)));
}

bool date_Compare(class QoreNode *l, class QoreNode *r)
{
   return (bool)!l->val.date_time->isEqual(r->val.date_time);
}

class QoreString *date_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   class QoreString *str = new QoreString();
   n->val.date_time->getString(str);
   return str;
}

void date_DeleteContents(class QoreNode *n)
{
   delete n->val.date_time;
}
