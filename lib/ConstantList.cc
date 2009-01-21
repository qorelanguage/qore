/*
 ConstantList.cc
 
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
#include <qore/intern/ConstantList.h>

#include <string.h>
#include <stdlib.h>

void ConstantList::remove(hm_qn_t::iterator i)
{
   if (i->second)
      i->second->deref(0);
   
   const char *c = i->first;
   hm.erase(i);
   free((char *)c);	 
}

ConstantList::~ConstantList()
{
   //QORE_TRACE("ConstantList::~ConstantList()");
   deleteAll();

}

//  NOTE: since constants cannot hold objects (only immediate values)
//  there is no need for an exception handler with the dereference
void ConstantList::deleteAll()
{
   hm_qn_t::iterator i = hm.begin();
   while (i != hm.end()) {
      remove(i);
      i = hm.begin();
   }
}

void ConstantList::reset()
{
   deleteAll();
}

void ConstantList::add(const char *name, AbstractQoreNode *value)
{
   // first check if the constant has already been defined
   if (hm.find(name) != hm.end())
   {
      parse_error("constant \"%s\" has already been defined", name);
      value->deref(0);
      return;
   }
   
   hm[strdup(name)] = value;
}

AbstractQoreNode *ConstantList::find(const char *name)
{
   hm_qn_t::iterator i = hm.find(name);
   if (i != hm.end())
      return i->second;
   
   return 0;
}

class ConstantList *ConstantList::copy()
{
   class ConstantList *ncl = new ConstantList();
   
   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); i++)
   {
      // reference value for new constant definition
      if (i->second)
	 i->second->ref();
      ncl->add(i->first, i->second);
   }
   
   return ncl;
}

// no duplicate checking is done here
void ConstantList::assimilate(class ConstantList *n)
{
   hm_qn_t::iterator i = n->hm.begin();
   while (i != n->hm.end())
   {
      // "move" data to new list
      hm[i->first] = i->second;
      n->hm.erase(i);
      i = n->hm.begin();
   }
}

// duplicate checking is done here
void ConstantList::assimilate(class ConstantList *n, class ConstantList *otherlist, const char *nsname)
{
   // assimilate target list
   hm_qn_t::iterator i = n->hm.begin();
   while (i != n->hm.end())
   {
      hm_qn_t::iterator j = otherlist->hm.find(i->first);
      if (j != otherlist->hm.end())
      {
	 parse_error("constant \"%s\" has already been defined in namespace \"%s\"", i->first, nsname);
	 n->remove(i);
      }
      else
      {      
	 j = hm.find(i->first);
	 if (j != hm.end())
	 {
	    parse_error("constant \"%s\" is already pending for namespace \"%s\"", i->first, nsname);
	    n->remove(i);
	 }
	 else
	 {
	    // "move" data to new list
	    hm[i->first] = i->second;
	    n->hm.erase(i);
	 }
      }
      i = n->hm.begin();
   }
}

void ConstantList::parseInit() {
   RootQoreNamespace *rns = getRootNS();
   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); i++) {
      printd(5, "ConstantList::parseInit() %s\n", i->first);
      rns->parseInitConstantValue(&i->second, 0);
      printd(5, "ConstantList::parseInit() constant %s resolved to %08p %s\n", 
	     i->first, i->second, i->second ? i->second->getTypeName() : "NULL");
      if (i->second)
	  process_node(&i->second, 0, 0);
      if (!i->second)
	 i->second = nothing();
   }
}

QoreHashNode *ConstantList::getInfo()
{
   QoreHashNode *h = new QoreHashNode();

   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); i++)
      h->setKeyValue(i->first, i->second->refSelf(), 0);

   return h;
}
