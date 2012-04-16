/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 FunctionList.h
 
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

#ifndef _QORE_FUNCTIONLIST_H

#define _QORE_FUNCTIONLIST_H

#include <qore/common.h>

#include <map>
#include <string>

//  function calls are handled with FunctionCallNode
class FunctionEntry {
   friend class FunctionList;

protected:
   QoreFunction* func;
   std::string name;

public:
   DLLLOCAL FunctionEntry(QoreFunction* u) : func(u) {
   }

   DLLLOCAL FunctionEntry(const char* new_name, QoreFunction* u) : func(u), name(new_name) {
   }

   DLLLOCAL FunctionEntry(const FunctionEntry& old) : func(old.func), name(old.name) {
      func->ref();
   }

   DLLLOCAL ~FunctionEntry() {
      func->deref();
   }

   DLLLOCAL QoreFunction* getFunction() const {
      return func;
   }

   DLLLOCAL QoreFunction* getFunction(bool runtime) const {
      if (runtime && func->committedEmpty())
	 return 0;
      return func;
   }

   DLLLOCAL const char* getName() const {
      return name.empty() ? func->getName() : name.c_str();
   }

   DLLLOCAL void parseInit() {
      func->parseInit();
   }

   DLLLOCAL void parseCommit() {
      func->parseCommit();
   }

   // returns -1 if the entry can be deleted
   DLLLOCAL int parseRollback() {
      // if there are no committed variants, then return -1 to erase the function entry entirely
      if (func->committedEmpty())
         return -1;

      // otherwise just roll back the pending variants
      func->parseRollback();
      return 0;
   }

   DLLLOCAL ResolvedCallReferenceNode* makeCallReference() const;

   DLLLOCAL bool isPublic() const {
      return func->hasModulePublic();
   }
};

class ModuleImportedFunctionEntry : public FunctionEntry {
public:
   DLLLOCAL ModuleImportedFunctionEntry(const FunctionEntry& old) : FunctionEntry(old.getName(), new QoreFunction(false, *(old.getFunction()))) {
   }
};

typedef std::map<const char* , FunctionEntry* , class ltstr> fl_map_t;

class FunctionList : public fl_map_t {
public:
   DLLLOCAL FunctionList() {
   }

   DLLLOCAL FunctionList(const FunctionList& old, int64 po);

   DLLLOCAL ~FunctionList() {
      del();
   }

   DLLLOCAL FunctionEntry* add(QoreFunction* func);
   DLLLOCAL FunctionEntry* import(QoreFunction* func);
   DLLLOCAL FunctionEntry* import(const char* new_name, QoreFunction* func);
   DLLLOCAL QoreFunction* find(const char* name, bool runtime) const;
   DLLLOCAL FunctionEntry* findNode(const char* name) const;

   DLLLOCAL void mergePublic(const FunctionList& src) {
      for (fl_map_t::const_iterator i = src.begin(), e = src.end(); i != e; ++i) {
         if (!i->second->isPublic())
            continue;

         assert(!findNode(i->first));
         //printd(5, "FunctionList::mergePublic() this: %p merging in %s (%p)\n", this, i->first, i->second);
         FunctionEntry* fe = new ModuleImportedFunctionEntry(*i->second);
         insert(fl_map_t::value_type(fe->getName(), fe));         
      }
   }

   DLLLOCAL void del();
   DLLLOCAL void parseInit();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();
   DLLLOCAL QoreListNode* getList();
   DLLLOCAL void assimilate(FunctionList& fl);
};

#endif
