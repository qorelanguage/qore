/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreProgram.h
 
 Program QoreObject Definition
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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

#ifndef _QORE_GLOBALVARIABLELIST_H

#define _QORE_GLOBALVARIABLELIST_H

#include <map>

class Var;

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include <qore/intern/xxhash.h>

typedef HASH_MAP<const char*, Var*, qore_hash_str, eqstr> map_var_t;
#else
typedef std::map<const char*, Var*, ltstr> map_var_t;
#endif

// this is a "grow-only" container
// all reading and writing is done within the parse lock on the containing program object
class GlobalVariableList {
protected:
   // xxx DLLLOCAL Var* parseCreatePendingVar(const char* name, QoreParseTypeInfo* typeInfo);

public:
   map_var_t vmap, pending_vmap;

   DLLLOCAL GlobalVariableList() {
   }

   DLLLOCAL GlobalVariableList(const GlobalVariableList& old, int64 po) {
      // don't inherit any global vars if the appropriate flag is not already set
      if ((po & PO_NO_INHERIT_GLOBAL_VARS))
         return;

      map_var_t::iterator last = vmap.begin();
      for (map_var_t::const_iterator i = old.vmap.begin(), e = old.vmap.end(); i != e; ++i) {
         //printd(5, "GlobalVariableList::GlobalVariableList() this: %p v: %p '%s' pub: %d\n", this, i->second, i->second->getName(), i->second->isPublic());
         if (!i->second->isPublic())
            continue;
         Var* v = new Var(const_cast<Var*>(i->second));
         last = vmap.insert(last, map_var_t::value_type(v->getName(), v));
      }
   }

   DLLLOCAL void mergePublic(const GlobalVariableList& old) {
      map_var_t::iterator last = vmap.begin();
      for (map_var_t::const_iterator i = old.vmap.begin(), e = old.vmap.end(); i != e; ++i) {
         if (!i->second->isPublic())
            continue;
         Var* v = new Var(const_cast<Var*>(i->second));
         last = vmap.insert(last, map_var_t::value_type(v->getName(), v));
      }
   }

   DLLLOCAL ~GlobalVariableList() {
      assert(vmap.empty());
   }

   DLLLOCAL void clearAll(ExceptionSink *xsink);
   DLLLOCAL void deleteAll(ExceptionSink* xsink);

   // called at runtime
   // returns a non-0 Var* if a new variable was created, 0 if not (because it already existed - exception raised)
   DLLLOCAL Var* import(Var* var, ExceptionSink* xsink, bool readonly = false);

   DLLLOCAL Var* runtimeCreateVar(const char* name, const QoreTypeInfo* typeInfo);

   DLLLOCAL Var* parseFindVar(const char* name);
   DLLLOCAL Var* parseCreatePendingVar(const char* name, const QoreTypeInfo* typeInfo);
   DLLLOCAL const Var* parseFindVar(const char* name) const;

   DLLLOCAL void parseAdd(Var* v) {
      assert(!parseFindVar(v->getName()));
      pending_vmap[v->getName()] = v;
   }

   // xxx DLLLOCAL Var* parseFindCreateVar(const char* name, QoreParseTypeInfo* typeInfo, bool& new_var);
   // xxx DLLLOCAL Var* parseFindCreateVar(const char* name, const QoreTypeInfo* typeInfo, bool& new_var);

   DLLLOCAL Var* runtimeFindVar(const char* name) {
      map_var_t::iterator i = vmap.find(name);
      return i != vmap.end() ? i->second : 0;
   }

   DLLLOCAL QoreListNode* getVarList() const;

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();
};

#endif
