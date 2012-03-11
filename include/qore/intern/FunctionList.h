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

//  functions are handled with FunctionCallNode; the program is set and the function is executed
class FunctionEntry {
   friend class FunctionList;

protected:
   QoreProgram* pgm;
   QoreFunction* func;
   std::string name;

public:
   DLLLOCAL FunctionEntry(QoreFunction* u) : pgm(0), func(u) {
   }

   DLLLOCAL FunctionEntry(QoreProgram* p, QoreFunction* u) : pgm(p), func(u) {
   }

   DLLLOCAL FunctionEntry(QoreProgram* p, const char* new_name, QoreFunction* u) : pgm(p), func(u), name(new_name) {
   }

   DLLLOCAL FunctionEntry(const FunctionEntry &ife) : pgm(ife.pgm), func(ife.func), name(ife.name) {
   }

   DLLLOCAL ~FunctionEntry() {
      // only dereference if not imported
      if (!pgm)
	 func->deref();
   }

   DLLLOCAL QoreProgram* getProgram() {
      return pgm;
   }

   DLLLOCAL QoreFunction* getFunction() {
      return func;
   }

   DLLLOCAL QoreFunction* getFunction(QoreProgram*& ipgm) {
      if (pgm)
         ipgm = pgm;
      return func;
   }

   DLLLOCAL QoreFunction* getFunction(QoreProgram*& ipgm, bool runtime) {
      if (runtime && func->committedEmpty())
	 return 0;
      if (pgm)
	 ipgm = pgm;
      return func;
   }

   DLLLOCAL const char* getName() const {
      return name.empty() ? func->getName() : name.c_str();
   }

   DLLLOCAL void parseInit() {
      // only parse init non-imported user functions
      if (!pgm)
           func->parseInit();
   }

   DLLLOCAL void parseCommit() {
      // only commit non-imported user functions
      if (!pgm)
         func->parseCommit();
   }

   // returns -1 if the entry can be deleted
   DLLLOCAL int parseRollback() {
      // only rollback non-imported user functions
      if (pgm)
	 return 0;

      // if there are no committed variants, then return -1 to erase the function entry entirely
      if (func->committedEmpty())
         return -1;

      // otherwise just roll back the pending variants
      func->parseRollback();
      return 0;
   }

   ResolvedCallReferenceNode* makeCallReference() const;
};

typedef std::map<const char* , FunctionEntry* , class ltstr> fl_map_t;

class FunctionList : public fl_map_t {
public:
   DLLLOCAL FunctionList() {
   }

   DLLLOCAL ~FunctionList() {
      del();
   }

   DLLLOCAL FunctionEntry* add(QoreFunction* func);
   DLLLOCAL FunctionEntry* add(QoreProgram* pgm, QoreFunction* func);
   DLLLOCAL FunctionEntry* add(QoreProgram* pgm, const char* new_name, QoreFunction* func);
   DLLLOCAL QoreFunction* find(const char* name, QoreProgram* &pgm, bool runtime) const;
   DLLLOCAL FunctionEntry* findNode(const char* name) const;
   DLLLOCAL void del();
   DLLLOCAL void parseInit();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();
   DLLLOCAL QoreListNode* getList();
};

#endif
