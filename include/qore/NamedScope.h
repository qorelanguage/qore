/*
  NamedScope.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  NamedScopes are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should 
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef QORE_NAMEDSCOPE_H

#define QORE_NAMEDSCOPE_H

// for parsing namespace/class scope resolution
class NamedScope
{
   private:
      int allocated;

   public:
      int elements;
      char **strlist;
      char *ostr;

      NamedScope(char *str);
      inline ~NamedScope();
      inline char *getIdentifier() { return strlist[elements - 1]; }
      inline class NamedScope *copy();
      inline void fixBCCall()
      {
	 // fix last string pointer
	 char *str = strlist[elements - 1];
	 memmove(str, str + 2, strlen(str) - 1);
      }
};

inline NamedScope::~NamedScope()
{
   for (int i = 0; i < elements; i++)
      free(strlist[i]);
   free(strlist);
   free(ostr);
}

inline class NamedScope *NamedScope::copy()
{
   return new NamedScope(ostr);
}

#endif // QORE_NAMEDSCOPE_H
