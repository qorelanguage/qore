/*
  QT_bigint.cc

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
#include <qore/QT_bigint.h>

class QoreNode *bigint_DefaultValue()
{
   Zero->ref();
   return Zero;
}

class QoreNode *bigint_ConvertTo(class QoreNode *n, class ExceptionSink *xsink)
{
   int64 i;

   if (n->type == NT_STRING)
      i = strtoll(n->val.String->getBuffer(), NULL, 10);
   else if (n->type == NT_FLOAT)
      i = (long long)n->val.floatval;
   else if (n->type == NT_DATE)
      i = n->val.date_time->year * 10000000000ll + 
	 n->val.date_time->month * 100000000ll +
	 n->val.date_time->day * 1000000ll + 
	 n->val.date_time->hour * 10000ll +
	 n->val.date_time->minute * 100ll +
	 n->val.date_time->second;
   else if (n->type == NT_BOOLEAN)
      i = n->val.boolval;
   else
      i = 0;

   return new QoreNode(i);
}

bool bigint_Compare(class QoreNode *l, class QoreNode *r)
{
   return (bool)(l->val.intval != r->val.intval);
}

class QoreString *bigint_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   return new QoreString(n->val.intval);
}
