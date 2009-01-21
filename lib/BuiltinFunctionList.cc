/*
  Function.cc

  Qore Programming language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/intern/ql_io.h>
#include <qore/intern/ql_time.h>
#include <qore/intern/ql_lib.h>
#include <qore/intern/ql_math.h>
#include <qore/intern/ql_type.h>
#include <qore/intern/ql_env.h>
#include <qore/intern/ql_string.h>
#include <qore/intern/ql_pwd.h>
#include <qore/intern/ql_misc.h>
#include <qore/intern/ql_list.h>
#include <qore/intern/ql_xml.h>
#include <qore/intern/ql_json.h>
#include <qore/intern/ql_thread.h>
#include <qore/intern/ql_crypto.h>
#include <qore/intern/ql_object.h>
#include <qore/intern/ql_file.h>
#include <qore/intern/ql_bzip.h>

#ifdef DEBUG
#include <qore/intern/ql_debug.h>
#endif // DEBUG

#include <string.h>
#include <assert.h>

bool BuiltinFunctionList::init_done = false;
hm_bf_t BuiltinFunctionList::hm;
class QoreThreadLock BuiltinFunctionList::mutex;
class BuiltinFunctionList builtinFunctions;

BuiltinFunctionList::BuiltinFunctionList()
{
}

BuiltinFunctionList::~BuiltinFunctionList()
{
//   assert(hm.empty());
}

void BuiltinFunctionList::add(const char *name, q_func_t f, int typ)
{
   if (init_done)
   {
      mutex.lock();
      // version with cloning the name: hm[strdup(name)] = new BuiltinFunction(name, f, typ);
      hm[name] = new BuiltinFunction(name, f, typ);
      mutex.unlock();
   } else {
      //version with cloning the name:  hm[strdup(name)] = new BuiltinFunction(name, f, typ);
      hm[name] = new BuiltinFunction(name, f, typ);
   }
}

void BuiltinFunctionList::clear()
{
   //printd(5, "BuiltinFunctionList::~BuiltinFunctionList() this=%08p\n", this);
   hm_bf_t::iterator i = hm.begin();
   while (i != hm.end())
   {
      //printd(5, "BuiltinFunctionList::~BuiltinFunctionList() deleting '%s()'\n", i->first);
      // char *c = (char *)i->first; - was used for deleting the cloned function name

      // delete function
      delete i->second;

      // erase hash entry
      hm.erase(i);

      i = hm.begin();

      // delete name
      // free(c); - uncomment if the names are cloned, uncomment the 'c' declaration above too
   }
}

const BuiltinFunction *BuiltinFunctionList::find(const char *name)
{
   const BuiltinFunction *rv = 0;
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
   QORE_TRACE("BuiltinFunctionList::init()");

   init_string_functions();
   init_io_functions();
   init_time_functions();
   init_lib_functions();
   init_misc_functions();
   init_list_functions();
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
   init_bzip_functions();
#ifdef DEBUG
   init_debug_functions();
#endif
   init_done = true;


}
