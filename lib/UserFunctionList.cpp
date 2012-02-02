/*
 UserFunctionList.cpp
 
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
#include <qore/intern/UserFunctionList.h>
#include <qore/intern/Function.h>
#include <qore/intern/StatementBlock.h>

UserFunctionList::~UserFunctionList() {
   parseRollback();
   del();
}

void UserFunctionList::del() {
   hm_uf_t::iterator i = fmap.begin();
   while (i != fmap.end()) {
      UserFunction *uf = i->second;
      fmap.erase(i);
      i = fmap.begin();
      uf->deref();
   }
}

void UserFunctionList::add(UserFunction *func) {
   QORE_TRACE("UserFunctionList::add()");
   
   assert(!find((func->getName())));
   fmap[func->getName()] = func;
}

UserFunction *UserFunctionList::find(const char *name) {
   printd(5, "UserFunctionList::find(%s)\n", name);
   hm_uf_t::iterator i = fmap.find(name);
   return i != fmap.end() ? i->second : 0;
}

QoreListNode *UserFunctionList::getList() {
   QORE_TRACE("UserFunctionList::getList()");
   
   QoreListNode *l = new QoreListNode();
   hm_uf_t::iterator i = fmap.begin();
   while (i != fmap.end()) {
      l->push(new QoreStringNode(i->first));      
      i++;
   }
   return l;
}

// unlocked
void UserFunctionList::parseInit() {
   QORE_TRACE("UserFunctionList::parseInit()");
   
   for (hm_uf_t::iterator i = fmap.begin(), e = fmap.end(); i != e; ++i)
      i->second->parseInit();
}

// unlocked
void UserFunctionList::parseCommit() {
   for (hm_uf_t::iterator i = fmap.begin(), e = fmap.end(); i != e; ++i)
      i->second->parseCommit();
}

// unlocked
void UserFunctionList::parseRollback() {
   QORE_TRACE("UserFunctionList::parseRollback()");

   for (hm_uf_t::iterator i = fmap.begin(), e = fmap.end(); i != e;) {
      if (i->second->committedEmpty()) {
	 i->second->deref();
	 fmap.erase(i++);
	 continue;
      }
      i->second->parseRollback();
      ++i;
   }
}
