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

void FunctionList::del() {
   for (fl_map_t::iterator i = begin(), e = end(); i != e; ++i)
      delete i->second;
   clear();
}

FunctionEntry* FunctionList::add(UserFunction* func) {
   QORE_TRACE("FunctionList::add()");

   assert(!findNode(func->getName()));
   
   FunctionEntry* n = new FunctionEntry(func);
   insert(std::make_pair(func->getName(), n));
   return n;
}

FunctionEntry* FunctionList::add(QoreProgram* pgm, UserFunction* func) {
   QORE_TRACE("FunctionList::add()");

   assert(!findNode(func->getName()));
   
   FunctionEntry* n = new FunctionEntry(pgm, func);
   insert(std::make_pair(func->getName(), n));
   return n;
}

FunctionEntry* FunctionList::add(QoreProgram* pgm, const char* new_name, UserFunction* func) {
   QORE_TRACE("FunctionList::add()");

   assert(!findNode(new_name));
   
   FunctionEntry* n = new FunctionEntry(pgm, new_name, func);
   insert(std::make_pair(new_name, n));
   return n;
}

FunctionEntry* FunctionList::findNode(const char* name) const {
   printd(5, "FunctionList::findNode(%s)\n", name);

   fl_map_t::const_iterator i = fl_map_t::find(name);
   if (i != end())
      return i->second;

   return 0;
}

UserFunction* FunctionList::find(const char* name, QoreProgram* &pgm, bool runtime) const {
   printd(5, "FunctionList::findFunction(%s) (UserFunction)\n", name);

   fl_map_t::const_iterator i = fl_map_t::find(name);
   if (i != end())
      return i->second->getFunction(pgm, runtime);

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
   return pgm
      ? new UserCallReferenceNode(func, pgm)
      : new LocalUserCallReferenceNode(func);
}
