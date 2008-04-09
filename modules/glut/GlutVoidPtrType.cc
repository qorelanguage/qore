/*
  GlutVoidPtrType.cc

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

#include "GlutVoidPtrType.h"

qore_type_t NT_GLUTPTR = -1;

QoreString *GlutVoidPtrType::getStringRepresentation(bool &del) const
{
   del = true;
   QoreString *str = new QoreString();
   getStringRepresentation(*str);
   return str;
}

void GlutVoidPtrType::getStringRepresentation(QoreString &str) const
{
   str.sprintf("glut_void_ptr=%08p", ptr);
}

bool GlutVoidPtrType::getAsBoolImpl() const
{
   return false;
}

int GlutVoidPtrType::getAsIntImpl() const
{
   return 0;
}

int64 GlutVoidPtrType::getAsBigIntImpl() const
{
   return 0;
}

double GlutVoidPtrType::getAsFloatImpl() const
{
   return 0.0;
}

QoreString *GlutVoidPtrType::getAsString(bool &del, int foff, class ExceptionSink *xsink) const
{
   return getStringRepresentation(del);
}

int GlutVoidPtrType::getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const
{
   getStringRepresentation(str);
   return 0;
}

class AbstractQoreNode *GlutVoidPtrType::realCopy() const
{
   return new GlutVoidPtrType(ptr);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool GlutVoidPtrType::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return GlutVoidPtrType::is_equal_hard(v, xsink);
}

bool GlutVoidPtrType::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const GlutVoidPtrType *ps = dynamic_cast<const GlutVoidPtrType *>(v);
   if (!ps)
      return false;

   return ps->ptr == ptr;
}

// returns the type name as a c string
const char *GlutVoidPtrType::getTypeName() const
{
   return "GlutVoidPtr";
}

void addGlutVoidPtrType()
{
   // add types for enums
   NT_GLUTPTR = get_next_type_id();
}
