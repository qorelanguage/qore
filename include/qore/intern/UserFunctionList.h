/*
 UserFunctionList.h
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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

#ifndef _QORE_USERFUNCTIONLIST_H

#define _QORE_USERFUNCTIONLIST_H

#include <qore/hash_map.h>

// all read and write access to this list is done within the program object's parse lock
class UserFunctionList
{
private:
   hm_uf_t fmap, pmap;   // maps of functions for quick lookups

public:
   DLLLOCAL UserFunctionList() {}
   DLLLOCAL ~UserFunctionList();
   DLLLOCAL void del();
   DLLLOCAL class UserFunction *find(const char *name);
   DLLLOCAL void add(class UserFunction *func);
   DLLLOCAL void parseInit();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();
   DLLLOCAL class QoreListNode *getList();
};

#endif
