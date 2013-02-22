/*
 GlobalVariableList.cpp
 
 Program QoreObject Definition
 
 Qore Programming Language
 
 Copyright 2003 - 2013 David Nichols
 
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
#include <qore/intern/GlobalVariableList.h>

#include <assert.h>

// adds directly to committed list
Var* GlobalVariableList::import(Var* v, ExceptionSink* xsink, bool readonly) {
   map_var_t::iterator i = vmap.find(v->getName());
   if (i != vmap.end()) {
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "'%s' already exists in the target namespace", v->getName());
      return 0;
   }

   Var* var = new Var(v, readonly);
   pending_vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::import(): reference to %s (%p) added\n", v->getName(), var);
   return var;
}

// sets all non-imported variables to NULL (dereferences contents if any)
void GlobalVariableList::clearAll(ExceptionSink* xsink) {
   //printd(5, "GlobalVariableList::clear_all() this=%p (size=%d)\n", this, vmap.size());
   for (map_var_t::iterator i = vmap.begin(), e = vmap.end(); i != e; ++i) {
      i->second->clearLocal(xsink);
   }
}

void GlobalVariableList::deleteAll(ExceptionSink* xsink) {
   parseRollback();

   for (map_var_t::iterator i = vmap.begin(), e = vmap.end(); i != e; ++i)
      i->second->deref(xsink);
   vmap.clear();
}

Var* GlobalVariableList::runtimeCreateVar(const char* name, const QoreTypeInfo* typeInfo) {
   if (parseFindVar(name))
      return 0;

   Var* var = new Var(name, typeInfo);
   vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::runtimeCreateVar(): %s (%p) added (resolved type %s)\n", name, var, typeInfo->getName());
   return var;
}

Var* GlobalVariableList::parseCreatePendingVar(const char* name, const QoreTypeInfo* typeInfo) {
   assert(!parseFindVar(name));

   Var* var = new Var(name, typeInfo);
   pending_vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::parseCreatePendingVar(): %s (%p) added (resolved type %s)\n", name, var, typeInfo->getName());
   return var;
}

Var* GlobalVariableList::parseFindVar(const char* name) {
   map_var_t::iterator i = vmap.find(name);
   if (i != vmap.end())
      return i->second;

   i = pending_vmap.find(name);
   return i != pending_vmap.end() ? i->second : 0;
}

const Var* GlobalVariableList::parseFindVar(const char* name) const {
   map_var_t::const_iterator i = vmap.find(name);
   if (i != vmap.end())
      return i->second;

   i = pending_vmap.find(name);
   return i != pending_vmap.end() ? i->second : 0;
}

void GlobalVariableList::parseInit() {
   for (map_var_t::iterator i = pending_vmap.begin(); i != pending_vmap.end(); i++)
      i->second->parseInit();
}

void GlobalVariableList::parseCommit() {
   for (map_var_t::iterator i = pending_vmap.begin(), e = pending_vmap.end(); i != e; ++i) {
      assert(vmap.find(i->second->getName()) == vmap.end());
      vmap[i->second->getName()] = i->second;
   }

   pending_vmap.clear();
}

void GlobalVariableList::parseRollback() {
   for (map_var_t::iterator i = pending_vmap.begin(), e = pending_vmap.end(); i != e; ++i)
      i->second->deref(0);
   pending_vmap.clear();
}

QoreListNode *GlobalVariableList::getVarList() const {
   QoreListNode *l = new QoreListNode();
   
   for (map_var_t::const_iterator i = vmap.begin(); i != vmap.end(); i++)
      l->push(new QoreStringNode(i->first));
   
   return l;
}
