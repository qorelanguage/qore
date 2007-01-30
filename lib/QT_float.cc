/*
  QT_float.cc

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
#include <qore/QT_float.h>

class QoreNode *float_DefaultValue()
{
   ZeroFloat->ref();
   return ZeroFloat;
}

class QoreNode *float_ConvertTo(class QoreNode *n, class ExceptionSink *xsink)
{
   double f;

   if (n->type == NT_STRING)
      f = atof(n->val.String->getBuffer());
   else if (n->type == NT_INT)
      f = (double)n->val.intval;
   else if (n->type == NT_DATE)
      f = (double)n->val.date_time->getEpochSeconds();
   else if (n->type == NT_BOOLEAN)
      f = (double)n->val.boolval;
   else
      f = 0.0;

   return new QoreNode(f);
}

bool float_Compare(class QoreNode *l, class QoreNode *r)
{
   return (bool)(l->val.floatval != r->val.floatval);
}

class QoreString *float_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   return new QoreString(n->val.floatval);
}
