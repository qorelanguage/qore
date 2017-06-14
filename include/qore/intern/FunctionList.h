/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FunctionList.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_FUNCTIONLIST_H

#define _QORE_FUNCTIONLIST_H

#include <qore/common.h>

#include "qore/intern/Function.h"

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

   DLLLOCAL qore_ns_private* getNamespace() const;

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

   DLLLOCAL ResolvedCallReferenceNode* makeCallReference(const QoreProgramLocation& loc) const;

   DLLLOCAL bool isPublic() const {
      return func->hasPublic();
   }

   DLLLOCAL bool isUserPublic() const {
      return func->hasUserPublic();
   }

   DLLLOCAL bool hasBuiltin() const {
      return func->hasBuiltin();
   }

   DLLLOCAL void updateNs(qore_ns_private* ns);
};

class ModuleImportedFunctionEntry : public FunctionEntry {
public:
   DLLLOCAL ModuleImportedFunctionEntry(const FunctionEntry& old, qore_ns_private* ns);
};

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include "qore/intern/xxhash.h"

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
   DLLLOCAL FunctionEntry* import(const char* new_name, QoreFunction* func, qore_ns_private* ns, bool inject);
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

   // returns the number of functions imported
   DLLLOCAL int importSystemFunctions(const FunctionList& src, qore_ns_private* ns, ExceptionSink* xsink);

   DLLLOCAL void del();
   DLLLOCAL void parseInit();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();
   DLLLOCAL QoreListNode* getList();
   DLLLOCAL void assimilate(FunctionList& fl, qore_ns_private* ns);
};

#endif
