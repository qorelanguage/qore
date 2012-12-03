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

ResolvedCallReferenceNode* FunctionEntry::makeCallReference() const {
   return new LocalFunctionCallReferenceNode(func);
}

FunctionList::FunctionList(const FunctionList& old, qore_ns_private* ns, int64 po) {
   bool no_user = po & PO_NO_INHERIT_USER_FUNC_VARIANTS;
   bool no_builtin = po & PO_NO_SYSTEM_FUNC_VARIANTS;
   for (fl_map_t::const_iterator i = old.begin(), e = old.end(); i != e; ++i) {
      QoreFunction* f = i->second->getFunction();
      if (!f->hasBuiltin()) {
         if (no_user || !f->hasUserPublic())
            continue;
      }
      else if (no_builtin && !f->hasUserPublic())
         continue;

      FunctionEntry* fe = new FunctionEntry(i->first, new QoreFunction(*f, po, ns));
      insert(std::make_pair(fe->getName(), fe));
      //if (!strcmp(i->first, "make_select_list2"))
      //if (f->hasUser())  printd(0, "FunctionList::FunctionList() this: %p copying fe: %p %s user: %d builtin: %d public: %d\n", this, i->second, i->first, f->hasUser(), f->hasBuiltin(), f->hasUserPublic());
   }
}

void FunctionList::del() {
   for (fl_map_t::iterator i = begin(), e = end(); i != e; ++i)
      delete i->second;
   clear();
   assert(empty());
}

FunctionEntry* FunctionList::add(QoreFunction* func) {
   QORE_TRACE("FunctionList::add()");
   assert(!findNode(func->getName()));
   assert(func->getNamespace());

   FunctionEntry* n = new FunctionEntry(func);
   insert(std::make_pair(func->getName(), n));
   return n;
}

FunctionEntry* FunctionList::import(QoreFunction* func, qore_ns_private* ns) {
   QORE_TRACE("FunctionList::import()");
   assert(!findNode(func->getName()));
   assert(func->getNamespace());

   // copy function entry for import and insert into map
   FunctionEntry* fe = new FunctionEntry(new QoreFunction(*func, 0, ns));
   insert(std::make_pair(fe->getName(), fe));
   return fe;
}

FunctionEntry* FunctionList::import(const char* new_name, QoreFunction* func, qore_ns_private* ns) {
   QORE_TRACE("FunctionList::add()");

   assert(!findNode(new_name));

   // copy function entry for import and insert into map
   FunctionEntry* fe = new FunctionEntry(new_name, new QoreFunction(*func, 0, ns, true));
   insert(std::make_pair(fe->getName(), fe));
   return fe;
}

FunctionEntry* FunctionList::findNode(const char* name) const {
   printd(5, "FunctionList::findNode(%s)\n", name);

   fl_map_t::const_iterator i = fl_map_t::find(name);
   return i != end() ? i->second : 0;
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

void FunctionList::assimilate(FunctionList& fl, qore_ns_private* ns) {
   for (fl_map_t::iterator i = fl.begin(), e = fl.end(); i != e;) {
      fl_map_t::const_iterator li = fl_map_t::find(i->first);
      if (li == end()) {
	 insert(fl_map_t::value_type(i->first, i->second));
	 i->second->updateNs(ns);
      }
      else {
	 li->second->getFunction()->parseAssimilate(*(i->second->getFunction()));
	 delete i->second;
      }

      fl.erase(i++);
   }   
}
