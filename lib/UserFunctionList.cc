/*
 UserFunctionList.cc
 
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

#include <qore/Qore.h>
#include <qore/intern/UserFunctionList.h>
#include <qore/intern/Function.h>
#include <qore/intern/StatementBlock.h>

UserFunctionList::~UserFunctionList()
{
   parseRollback();
   del();
}

void UserFunctionList::del()
{
   hm_uf_t::iterator i = fmap.begin();
   while (i != fmap.end())
   {
      class UserFunction *uf = i->second;
      fmap.erase(i);
      i = fmap.begin();
      uf->deref();
   }
}

void UserFunctionList::add(class UserFunction *func)
{
   QORE_TRACE("UserFunctionList::add()");
   
   if (find(func->getName()))
      parse_error("user function '%s' has already been defined", func->getName());
   else
      pmap[func->getName()] = func;
   

}

class UserFunction *UserFunctionList::find(const char *name)
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
   return 0;
}

QoreListNode *UserFunctionList::getList()
{
   QORE_TRACE("UserFunctionList::getList()");
   
   QoreListNode *l = new QoreListNode();
   hm_uf_t::iterator i = fmap.begin();
   while (i != fmap.end())
   {
      l->push(new QoreStringNode(i->first));      
      i++;
   }
   

   return l;
}

// unlocked
void UserFunctionList::parseInit()
{
   QORE_TRACE("UserFunctionList::parseInit()");
   
   hm_uf_t::iterator i = pmap.begin();
   while (i != pmap.end())
   {
      // can (and must) be called if if w->statements is NULL
      i->second->statements->parseInit(i->second->params);
      i++;
   }
   

}

// unlocked
void UserFunctionList::parseCommit()
{
   hm_uf_t::iterator i = pmap.begin();
   while (i != pmap.end())
   {
      fmap[i->first] = i->second;
      pmap.erase(i);
      i = pmap.begin();
   }
}

// unlocked
void UserFunctionList::parseRollback()
{
   QORE_TRACE("UserFunctionList::parseRollback()");
   hm_uf_t::iterator i = pmap.begin();
   while (i != pmap.end())
   {
      class UserFunction *uf = i->second;
      pmap.erase(i);
      uf->deref();
      i = pmap.begin();
   }

}
