/*
 GlobalVariableList.cpp
 
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

#include <qore/Qore.h>
#include <qore/intern/GlobalVariableList.h>

#include <assert.h>

GlobalVariableList::GlobalVariableList() {
}

GlobalVariableList::~GlobalVariableList() {
   assert(vmap.empty());
}

// adds directly to committed list
void GlobalVariableList::import(Var *var, ExceptionSink *xsink, bool readonly) {
   map_var_t::iterator i = vmap.find(var->getName());
   if (i == vmap.end())
      newVar(var, readonly);
   else {
      Var *v = i->second;
      vmap.erase(i);
      v->makeReference(var, xsink, readonly);
      vmap[v->getName()] = v;
   }
}

// sets all non-imported variables to NULL (dereferences contents if any)
void GlobalVariableList::clear_all(ExceptionSink *xsink) {
   //printd(5, "GlobalVariableList::clear_all() this=%08p (size=%d)\n", this, vmap.size());
   map_var_t::reverse_iterator i = vmap.rbegin();
   
   while (i != vmap.rend()) {
      if (!i->second->isImported()) {
	 printd(5, "GlobalVariableList::clear_all() clearing '%s' (%08p)\n", i->first, i->second);
	 i->second->setValue(0, xsink);
      }
#ifdef DEBUG
      else printd(5, "GlobalVariableList::clear_all() skipping imported var '%s' (%08p)\n", i->first, i->second);
#endif
      ++i;
   }
}

void GlobalVariableList::delete_all(ExceptionSink *xsink) {
   parseRollback();

   for (map_var_t::iterator i = vmap.begin(), e = vmap.end(); i != e; ++i)
      i->second->deref(xsink);
   vmap.clear();
}

Var *GlobalVariableList::newVar(const char *name, QoreParseTypeInfo *typeInfo) {
   Var *var = new Var(name, typeInfo);
   pending_vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::newVar(): %s (%08p) added (parseType %s)\n", name, var, typeInfo->getName());
   return var;
}

Var *GlobalVariableList::newVar(const char *name, const QoreTypeInfo *typeInfo) {
   Var *var = new Var(name, typeInfo);
   pending_vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::newVar(): %s (%08p) added (resolved type %s)\n", name, var, typeInfo->getName());
   return var;
}

Var *GlobalVariableList::newVar(Var *v, bool readonly) {
   Var *var = new Var(v, readonly);
   pending_vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::newVar(): reference to %s (%08p) added\n", v->getName(), var);
   return var;
}

Var *GlobalVariableList::findVar(const char *name) {
   map_var_t::iterator i = vmap.find(name);
   if (i != vmap.end())
      return i->second;

   i = pending_vmap.find(name);
   return i != pending_vmap.end() ? i->second : 0;
}

const Var *GlobalVariableList::findVar(const char *name) const {
   map_var_t::const_iterator i = vmap.find(name);
   if (i != vmap.end())
      return i->second;

   i = pending_vmap.find(name);
   return i != pending_vmap.end() ? i->second : 0;
}

// used for resolving unflagged global variables
Var *GlobalVariableList::checkVar(const char *name, QoreParseTypeInfo *typeInfo, int *new_var) {
   QORE_TRACE("GlobalVariableList::checkVar()");
   Var *var;
   
   if (!(var = findVar(name))) {
      *new_var = 1;
      var = newVar(name, typeInfo);
   }
   else // check if a new type has been declared for this global variable
      var->parseCheckAssignType(typeInfo);

   return var;
}

// used for resolving unflagged global variables
Var *GlobalVariableList::checkVar(const char *name, const QoreTypeInfo *typeInfo, int *new_var) {
   QORE_TRACE("GlobalVariableList::checkVar()");
   Var *var;

   if (!(var = findVar(name))) {
      *new_var = 1;
      var = newVar(name, typeInfo);
   }
   else // check if a new type has been declared for this global variable
      var->checkAssignType(typeInfo);

   return var;
}

void GlobalVariableList::parseInit(int64 parse_options) {
   bool needs_type = (bool)(parse_options & PO_REQUIRE_TYPES);
   for (map_var_t::iterator i = pending_vmap.begin(); i != pending_vmap.end(); i++) {
      if (needs_type && !i->second->hasTypeInfo())
	 parse_error("global variable '%s' declared without type information, but parse options require all declarations to have type information", i->second->getName());
      i->second->parseInit();
   }
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
