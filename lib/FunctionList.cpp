/*
 FunctionList.cpp
 
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
#include <qore/intern/FunctionList.h>

#include <string.h>

FunctionList::FunctionList(const FunctionList& old, int64 po) {
   bool no_user = !(po & PO_INHERIT_USER_FUNC_VARIANTS);
   bool no_builtin = po & PO_NO_SYSTEM_FUNC_VARIANTS;
   for (fl_map_t::const_iterator i = old.begin(), e = old.end(); i != e; ++i) {
      QoreFunction* f = i->second->getFunction();
      if (no_user && !f->hasBuiltin())
	 continue;
      if (no_builtin && !f->hasUser())
	 continue;

      FunctionEntry* fe = new FunctionEntry(i->first, new QoreFunction(*f, po));
      insert(std::make_pair(fe->getName(), fe));
      //printd(0, "FunctionList::FunctionList() this: %p copying function %s user: %d builtin: %d\n", this, i->first, f->hasUser(), f->hasBuiltin());
   }
}

void FunctionList::del() {
   for (fl_map_t::iterator i = begin(), e = end(); i != e; ++i)
      delete i->second;
   clear();
}

FunctionEntry* FunctionList::add(QoreFunction* func) {
   QORE_TRACE("FunctionList::add()");

   assert(!findNode(func->getName()));
   
   FunctionEntry* n = new FunctionEntry(func);
   insert(std::make_pair(func->getName(), n));
   return n;
}

FunctionEntry* FunctionList::import(QoreFunction* func) {
   QORE_TRACE("FunctionList::import()");

   assert(!findNode(func->getName()));

   // copy function entry for import and insert into map
   FunctionEntry* fe = new FunctionEntry(new QoreFunction(*func));
   insert(std::make_pair(fe->getName(), fe));
   return fe;
}

FunctionEntry* FunctionList::import(const char* new_name, QoreFunction* func) {
   QORE_TRACE("FunctionList::add()");

   assert(!findNode(new_name));

   // copy function entry for import and insert into map
   FunctionEntry* fe = new FunctionEntry(new_name, new QoreFunction(*func));
   insert(std::make_pair(fe->getName(), fe));
   return fe;
}

FunctionEntry* FunctionList::findNode(const char* name) const {
   printd(5, "FunctionList::findNode(%s)\n", name);

   fl_map_t::const_iterator i = fl_map_t::find(name);
   if (i != end())
      return i->second;

   return 0;
}

QoreFunction* FunctionList::find(const char* name, bool runtime) const {
   printd(5, "FunctionList::findFunction(%s) (QoreFunction)\n", name);

   fl_map_t::const_iterator i = fl_map_t::find(name);
   if (i != end())
      return i->second->getFunction(runtime);

   return 0;
}

QoreListNode* FunctionList::getList() {
   QoreListNode* l = new QoreListNode;

   for (fl_map_t::iterator i = begin(), e = end(); i != e; ++i)
      l->push(new QoreStringNode(i->first));      

   return l;
}

void FunctionList::parseInit() {
   for (fl_map_t::iterator i = begin(), e = end(); i != e; ++i)
      i->second->parseInit();
}

void FunctionList::parseCommit() {
   // commit pending variants in all functions
   for (fl_map_t::iterator i = begin(), e = end(); i != e; ++i)
      i->second->parseCommit();
}

void FunctionList::parseRollback() {
   for (fl_map_t::iterator i = begin(), e = end(); i != e;) {
      if (i->second->parseRollback()) {
	 delete i->second;
	 erase(i++);
	 continue;
      }

      ++i;
   }
}

ResolvedCallReferenceNode* FunctionEntry::makeCallReference() const {
   return new LocalFunctionCallReferenceNode(func);
}
