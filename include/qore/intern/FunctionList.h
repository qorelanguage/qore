/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 FunctionList.h
 
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

#ifndef _QORE_FUNCTIONLIST_H

#define _QORE_FUNCTIONLIST_H

#include <qore/common.h>

#include <map>
#include <string>

class qore_ns_private;

//  function calls are handled with FunctionCallNode
class FunctionEntry {
   friend class FunctionList;

private:
   // not implemented
   DLLLOCAL FunctionEntry(const FunctionEntry& old);

protected:
   QoreFunction* func;
   std::string name;

public:
   DLLLOCAL FunctionEntry(QoreFunction* u) : func(u) {
   }

   DLLLOCAL FunctionEntry(const char* new_name, QoreFunction* u) : func(u), name(new_name) {
   }

   DLLLOCAL ~FunctionEntry() {
      func->deref();
   }

   DLLLOCAL qore_ns_private* getNamespace() const {
      return func->getNamespace();
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
      return func->hasPublic();
   }

   DLLLOCAL bool isUserPublic() const {
      return func->hasUserPublic();
   }

   DLLLOCAL void updateNs(qore_ns_private* ns) {
      func->updateNs(ns);
   }
};

class ModuleImportedFunctionEntry : public FunctionEntry {
public:
   //DLLLOCAL ModuleImportedFunctionEntry(const FunctionEntry& old, qore_ns_private* ns) : FunctionEntry(old.getName(), new QoreFunction(false, *(old.getFunction()), ns)) {
   //}
   DLLLOCAL ModuleImportedFunctionEntry(const FunctionEntry& old, qore_ns_private* ns) : FunctionEntry(old.getName(), new QoreFunction(*(old.getFunction()), PO_NO_SYSTEM_FUNC_VARIANTS, ns)) {
   }
};

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include <qore/intern/xxhash.h>

typedef HASH_MAP<const char*, FunctionEntry*, qore_hash_str, eqstr> fl_map_t;
#else
typedef std::map<const char*, FunctionEntry*, ltstr> fl_map_t;
#endif

class FunctionList : public fl_map_t {
public:
   DLLLOCAL FunctionList() {
   }

   DLLLOCAL FunctionList(const FunctionList& old, qore_ns_private* ns, int64 po);

   DLLLOCAL ~FunctionList() {
      del();
   }

   DLLLOCAL FunctionEntry* add(QoreFunction* func);
   DLLLOCAL FunctionEntry* import(QoreFunction* func, qore_ns_private* ns);
   DLLLOCAL FunctionEntry* import(const char* new_name, QoreFunction* func, qore_ns_private* ns);
   DLLLOCAL QoreFunction* find(const char* name, bool runtime) const;
   DLLLOCAL FunctionEntry* findNode(const char* name) const;

   DLLLOCAL void mergeUserPublic(const FunctionList& src, qore_ns_private* ns) {
      for (fl_map_t::const_iterator i = src.begin(), e = src.end(); i != e; ++i) {
         if (!i->second->isUserPublic())
            continue;

         assert(!findNode(i->first));
         FunctionEntry* fe = new ModuleImportedFunctionEntry(*i->second, ns);
         //printd(5, "FunctionList::mergePublic() this: %p merging in %s (%p)\n", this, i->first, fe);
         assert(!fe->isUserPublic());
         insert(fl_map_t::value_type(fe->getName(), fe));
      }
   }

   DLLLOCAL void del();
   DLLLOCAL void parseInit();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();
   DLLLOCAL QoreListNode* getList();
   DLLLOCAL void assimilate(FunctionList& fl, qore_ns_private* ns);
};

#endif
