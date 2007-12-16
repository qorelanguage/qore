/*
  BuiltinFunctionList.h

  Qore programming language

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

#ifndef _QORE_BUILTINFUNCTIONLIST_H

#define _QORE_BUILTINFUNCTIONLIST_H

#include <qore/common.h>
#include <qore/hash_map.h>
#include <qore/Restrictions.h>
#include <qore/LockedObject.h>

DLLLOCAL void init_builtin_functions();

// there is only one of these, therefore we have static members and methods
class BuiltinFunctionList
{
   private:
      DLLLOCAL static bool init_done;
      DLLLOCAL static hm_bf_t hm;
      DLLLOCAL static class LockedObject mutex;

      // not implemented
      DLLLOCAL BuiltinFunctionList(const BuiltinFunctionList&);
      DLLLOCAL BuiltinFunctionList& operator=(const BuiltinFunctionList&);
      DLLLOCAL void *operator new(size_t);

   public:
      DLLLOCAL BuiltinFunctionList();
      DLLLOCAL ~BuiltinFunctionList();
      DLLLOCAL void clear();

      // The function assumes that the 'name' string is ALWAYS character literal (aka "my_foo") which stays
      // valid during the whole lifetime of the session. If this assumption is not true the name would
      // need to be cloned.
      DLLEXPORT static void add(const char *name, q_func_t f, int typ = QDOM_DEFAULT);
      DLLEXPORT static class BuiltinFunction *find(const char *name);
      DLLEXPORT static int size();

      DLLLOCAL static void init();
};

DLLEXPORT extern class BuiltinFunctionList builtinFunctions;

#endif // _QORE_BUILTINFUNCTIONLIST_H
