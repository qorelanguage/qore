/*
  Function.cc

  Qore Programming language

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

#include <qore/config.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/Function.h>
#include <qore/DBI.h>
#include <qore/ql_io.h>
#include <qore/ql_time.h>
#include <qore/ql_lib.h>
#include <qore/ql_math.h>
#include <qore/ql_type.h>
#include <qore/ql_env.h>
#include <qore/ql_string.h>
#include <qore/ql_pwd.h>
#include <qore/ql_misc.h>
#include <qore/ql_xml.h>
#include <qore/ql_json.h>
#include <qore/ql_thread.h>
#include <qore/ql_crypto.h>
#include <qore/ql_object.h>
#include <qore/ql_file.h>

#ifdef DEBUG
#include <qore/ql_debug.h>
#endif // DEBUG

#include <string.h>

bool BuiltinFunctionList::init_done = false;
hm_bf_t BuiltinFunctionList::hm;
class LockedObject BuiltinFunctionList::mutex;
class BuiltinFunctionList builtinFunctions;

DLLLOCAL  BuiltinFunctionList::BuiltinFunctionList()
{
}

void BuiltinFunctionList::add(char *name, class QoreNode *(*f)(class QoreNode *, class ExceptionSink *xsink), int typ)
{
   if (init_done)
   {
      mutex.lock();
      hm[strdup(name)] = new BuiltinFunction(name, f, typ);
      mutex.unlock();
   }
   else
      hm[strdup(name)] = new BuiltinFunction(name, f, typ);
}

BuiltinFunctionList::~BuiltinFunctionList()
{
   //printd(5, "BuiltinFunctionList::~BuiltinFunctionList() this=%08p\n", this);
   hm_bf_t::iterator i;
   while ((i = hm.begin()) != hm.end())
   {
      //printd(5, "BuiltinFunctionList::~BuiltinFunctionList() deleting '%s()'\n", i->first);

      char *c = i->first;

      delete i->second;

      // erase hash entry
      hm.erase(i);

      // delete function
      free(c);
   }
}

class BuiltinFunction *BuiltinFunctionList::find(char *name)
{
   class BuiltinFunction *rv = NULL;
   if (init_done)
      mutex.lock();
   hm_bf_t::iterator i = hm.find(name);
   if (i != hm.end())
      rv = i->second;
   if (init_done)
      mutex.unlock();
   return rv;
}

inline int BuiltinFunctionList::size()
{
   return hm.size();
}

void BuiltinFunctionList::init()
{
   tracein("BuiltinFunctionList::init()");

   init_string_functions();
   init_io_functions();
   init_time_functions();
   init_lib_functions();
   init_misc_functions();
   init_type_functions();
   init_pwd_functions();
   init_math_functions();
   init_env_functions();
   init_xml_functions();
   init_json_functions();
   init_dbi_functions();
   init_thread_functions();
   init_crypto_functions();
   init_object_functions();
   init_file_functions();
#ifdef DEBUG
   init_debug_functions();
#endif
   init_done = true;

   traceout("BuiltinFunctionList::init()");
}
