/*
 GlobalVariableList.cc
 
 Program QoreObject Definition
 
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
#include <qore/GlobalVariableList.h>
#include <qore/Variable.h>

#include <assert.h>

GlobalVariableList::GlobalVariableList()
{
}

GlobalVariableList::~GlobalVariableList()
{
   assert(vmap.empty());
}

void GlobalVariableList::import(class Var *var, class ExceptionSink *xsink, bool readonly)
{
   map_var_t::iterator i = vmap.find(var->getName());
   if (i == vmap.end())
      newVar(var, readonly);
   else
   {
      class Var *v = i->second;
      vmap.erase(i);
      v->makeReference(var, xsink, readonly);
      vmap[v->getName()] = v;
   }
}

// sets all non-imported variables to NULL (dereferences contents if any)
void GlobalVariableList::clear_all(class ExceptionSink *xsink)
{
   //printd(5, "GlobalVariableList::clear_all() this=%08p (size=%d)\n", this, vmap.size());
   map_var_t::reverse_iterator i = vmap.rbegin();
   
   while (i != vmap.rend())
   {
      if (!i->second->isImported())
      {
	 printd(5, "GlobalVariableList::clear_all() clearing '%s' (%08p)\n", i->first, i->second);
	 i->second->setValue(NULL, xsink);
      }
#ifdef DEBUG
      else printd(5, "GlobalVariableList::clear_all() skipping imported var '%s' (%08p)\n", i->first, i->second);
#endif
      i++;
   }
}

void GlobalVariableList::delete_all(class ExceptionSink *xsink)
{
   map_var_t::iterator i;
   while ((i = vmap.end()) != vmap.begin())
   {
      --i;
      class Var *v = i->second;
      vmap.erase(i);
      v->deref(xsink);
   }
}

class Var *GlobalVariableList::newVar(const char *name)
{
   class Var *var = new Var(name);
   vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::newVar(): %s (%08p) added\n", name, var);
   return var;
}

class Var *GlobalVariableList::newVar(class Var *v, bool readonly)
{
   class Var *var = new Var(v, readonly);
   vmap[var->getName()] = var;
   
   printd(5, "GlobalVariableList::newVar(): reference to %s (%08p) added\n", v->getName(), var);
   return var;
}

class Var *GlobalVariableList::findVar(const char *name)
{
   map_var_t::iterator i = vmap.find(name);
   if (i != vmap.end())
      return i->second;
   return NULL;
}

// used for resolving unflagged global variables
class Var *GlobalVariableList::checkVar(const char *name, int *new_var)
{
   tracein("GlobalVariableList::checkVar()");
   class Var *var;
   
   if (!(var = findVar(name)))
   {
      *new_var = 1;
      var = newVar(name);
   }
   traceout("GlobalVariableList::checkVar()");
   return var;
}

class QoreList *GlobalVariableList::getVarList() const
{
   QoreList *l = new QoreList();
   
   for (map_var_t::const_iterator i = vmap.begin(); i != vmap.end(); i++)
      l->push(new QoreNode(i->first));
   
   return l;
}
