/*
  BuiltinFunctionList.h

  Qore programming language

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

#ifndef _QORE_BUILTINFUNCTIONLIST_H

#define _QORE_BUILTINFUNCTIONLIST_H

#include <qore/config.h>
#include <qore/hash_map.h>
#include <qore/Restrictions.h>
#include <qore/hash_map.h>
#include <qore/LockedObject.h>

#include <string.h>

class BuiltinFunctionList : public LockedObject
{
   private:
      bool init_done;
      hm_bf_t hm;
   
      inline void add_locked(char *name, class QoreNode *(*f)(class QoreNode *, class ExceptionSink *xsink), int typ);
      
   public:
      inline BuiltinFunctionList() : init_done(false) {}

      ~BuiltinFunctionList();
      
      void add(char *name, class QoreNode *(*f)(class QoreNode *, class ExceptionSink *xsink), int typ = QDOM_DEFAULT);

      inline class BuiltinFunction *find(char *name);

      inline int size()
      {
	 return hm.size();
      }

      void init();
      //void cleanup();
};

extern class BuiltinFunctionList builtinFunctions;

void init_builtin_functions();

#include <qore/Function.h>

inline class BuiltinFunction *BuiltinFunctionList::find(char *name)
{
   class BuiltinFunction *rv = NULL;
   if (init_done)
      lock();
   hm_bf_t::iterator i = hm.find(name);
   if (i != hm.end())
      rv = i->second;
   if (init_done)
      unlock();
   return rv;
}

#endif // _QORE_BUILTINFUNCTIONLIST_H
