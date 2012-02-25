/*
 ImportedFunctionList.cpp
 
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
#include <qore/intern/ImportedFunctionList.h>

#include <string.h>

ImportedFunctionList::ImportedFunctionList() {
}

ImportedFunctionList::~ImportedFunctionList() {
   ifn_map_t::iterator i;
   while ((i = begin()) != end()) {
      ImportedFunctionEntry *n = i->second;
      erase(i);
      delete n;
   }
}

ImportedFunctionEntry* ImportedFunctionList::add(QoreProgram *pgm, UserFunction *func) {
   QORE_TRACE("ImportedFunctionList::add()");
   
   ImportedFunctionEntry *n = new ImportedFunctionEntry(pgm, func);
   insert(std::make_pair(func->getName(), n));
   return n;
}

ImportedFunctionEntry* ImportedFunctionList::add(QoreProgram *pgm, const char *new_name, UserFunction *func) {
   QORE_TRACE("ImportedFunctionList::add()");
   
   ImportedFunctionEntry *n = new ImportedFunctionEntry(pgm, new_name, func);
   insert(std::make_pair(n->getName(), n));
   return n;
}

ImportedFunctionEntry *ImportedFunctionList::findNode(const char *name) const {
   printd(5, "ImportedFunctionList::findNode(%s)\n", name);

   ifn_map_t::const_iterator i = ifn_map_t::find(name);
   if (i != end())
      return i->second;

   return 0;
}

UserFunction *ImportedFunctionList::find(const char *name, QoreProgram *&pgm) const {
   printd(5, "ImportedFunctionList::findFunction(%s) (UserFunction)\n", name);

   ifn_map_t::const_iterator i = ifn_map_t::find(name);
   if (i != end()) {
      pgm = i->second->getProgram();
      return i->second->getFunction();
   }

   return 0;
}
