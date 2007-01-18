/*
 ImportedFunctionList.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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
#include <qore/ImportedFunctionList.h>
#include <qore/support.h>

#include <string.h>

ImportedFunctionNode::ImportedFunctionNode(class QoreProgram *p, class UserFunction *u)
{
   pgm = p;
   func = u;
}

ImportedFunctionList::ImportedFunctionList()
{
}

ImportedFunctionList::~ImportedFunctionList()
{
   ifn_map_t::iterator i;
   while ((i = begin()) != end())
   {
      class ImportedFunctionNode *n = i->second;
      erase(i);
      delete n;
   }
}

void ImportedFunctionList::add(class QoreProgram *pgm, class UserFunction *func)
{
   tracein("ImportedFunctionList::add()");
   
   ImportedFunctionNode *n = new ImportedFunctionNode(pgm, func);
   insert(std::make_pair(func->getName(), n));
   
   traceout("ImportedFunctionList::add()");
}

class ImportedFunctionNode *ImportedFunctionList::findNode(char *name) const
{
   printd(5, "ImportedFunctionList::findNode(%s)\n", name);

   ifn_map_t::const_iterator i = ifn_map_t::find(name);
   if (i != end())
      return i->second;

   return NULL;
}

class UserFunction *ImportedFunctionList::find(char *name) const
{
   printd(5, "ImportedFunctionList::findFunction(%s) (UserFunction)\n", name);

   ifn_map_t::const_iterator i = ifn_map_t::find(name);
   if (i != end())
      return i->second->func;

   return NULL;
}
