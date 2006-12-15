/*
 UserFunctionList.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006 David Nichols
 
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/UserFunctionList.h>
#include <qore/List.h>
#include <qore/support.h>
#include <qore/Function.h>

UserFunctionList::~UserFunctionList()
{
   parseRollback();
   del();
}

void UserFunctionList::del()
{
   hm_uf_t::iterator i;
   while ((i = fmap.begin()) != fmap.end())
   {
      class UserFunction *uf = i->second;
      fmap.erase(i);
      uf->deref();
   }
}

void UserFunctionList::add(class UserFunction *func)
{
   tracein("UserFunctionList::add()");
   
   if (find(func->name))
      parse_error("user function \"%s\" has already been defined", func->name);
   else
      pmap[func->name] = func;
   
   traceout("UserFunctionList::add()");
}

class UserFunction *UserFunctionList::find(char *name)
{
   printd(5, "UserFunctionList::find(%s)\n", name);
   // first look in pending functions
   hm_uf_t::iterator i = pmap.find(name);
   if (i != pmap.end())
      return i->second;
   
   i = fmap.find(name);
   if (i != fmap.end())
      return i->second;
   
   //printd(5, "UserFunctionList::find(%s) returning %08p\n", name, w);
   return NULL;
}

class List *UserFunctionList::getList()
{
   tracein("UserFunctionList::getList()");
   
   class List *l = new List();
   hm_uf_t::iterator i = fmap.begin();
   while (i != fmap.end())
   {
      l->push(new QoreNode(i->first));      
      i++;
   }
   
   traceout("UserFunctionList::getList()");
   return l;
}

// unlocked
void UserFunctionList::parseInit()
{
   tracein("UserFunctionList::parseInit()");
   
   hm_uf_t::iterator i = pmap.begin();
   while (i != pmap.end())
   {
      // can (and must) be called if if w->statements is NULL
      i->second->statements->parseInit(i->second->params);
      i++;
   }
   
   traceout("UserFunctionList::parseInit()");
}

// unlocked
void UserFunctionList::parseCommit()
{
   hm_uf_t::iterator i;
   while ((i = pmap.begin()) != pmap.end())
   {
      fmap[i->first] = i->second;
      pmap.erase(i);
   }
}

// unlocked
void UserFunctionList::parseRollback()
{
   tracein("UserFunctionList::parseRollback()");
   hm_uf_t::iterator i;
   while ((i = pmap.begin()) != pmap.end())
   {
      class UserFunction *uf = i->second;
      pmap.erase(i);
      uf->deref();
   }
   traceout("UserFunctionList::parseRollback()");
}
