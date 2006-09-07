/*
  ConstantList.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  constants can only be defined when parsing
  constants values will be substituted during the 2nd parse phase

  this structure can be safely read at all times, and writes are
  wrapped under the program-level parse lock

  NOTE: constants can only hold immediate values (no objects)

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

#ifndef _QORE_CONSTANTLIST_H

#define _QORE_CONSTANTLIST_H

#include <qore/config.h>
#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/Hash.h>

#include <string.h>
#include <stdlib.h>

#ifdef HAVE_QORE_HASH_MAP
#include <qore/hash_map.h>
#endif

class ConstantList
{
   private:
#ifdef HAVE_QORE_HASH_MAP
      hm_qn_t hm;

      inline void remove(hm_qn_t::iterator i)
      {
	 if (i->second)
	    i->second->deref(NULL);

	 char *c = i->first;
	 hm.erase(i);
	 free(c);	 
      }
#endif

      inline void deleteAll();

   public:
#ifndef HAVE_QORE_HASH_MAP
      class CNode *head, *tail;

      inline ConstantList();
#endif

      inline ~ConstantList();
      inline void add(char *name, class QoreNode *value);
      inline class QoreNode *find(char *name);
      inline class ConstantList *copy();
      inline void reset();
      inline void assimilate(class ConstantList *n, class ConstantList *otherlist, char *nsname);
      inline void assimilate(class ConstantList *n);
      inline void parseInit();
      inline Hash *getInfo();
};

inline ConstantList::~ConstantList()
{
   //tracein("ConstantList::~ConstantList()");
   deleteAll();
   //traceout("ConstantList::~ConstantList()");
}

#ifdef HAVE_QORE_HASH_MAP

//  NOTE: since constants cannot hold objects (only immediate values)
//  there is no need for an exception handler with the dereference
inline void ConstantList::deleteAll()
{
   hm_qn_t::iterator i;
   while ((i = hm.begin()) != hm.end())
      remove(i);
}

inline void ConstantList::reset()
{
   deleteAll();
}

inline void ConstantList::add(char *name, class QoreNode *value)
{
   // first check if the constant has already been defined
   if (hm.find(name) != hm.end())
   {
      parse_error("constant \"%s\" has already been defined", name);
      value->deref(NULL);
      return;
   }

   hm[strdup(name)] = value;
}

inline class QoreNode *ConstantList::find(char *name)
{
   hm_qn_t::iterator i = hm.find(name);
   if (i != hm.end())
      return i->second;

   return NULL;
}

inline class ConstantList *ConstantList::copy()
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
inline void ConstantList::assimilate(class ConstantList *n)
{
   hm_qn_t::iterator i;
   while ((i = n->hm.begin()) != n->hm.end())
   {
      // "move" data to new list
      hm[i->first] = i->second;
      n->hm.erase(i);
   }
}

// duplicate checking is done here
inline void ConstantList::assimilate(class ConstantList *n, class ConstantList *otherlist, char *nsname)
{
   // assimilate target list
   hm_qn_t::iterator i;
   while ((i = n->hm.begin()) != n->hm.end())
   {
      hm_qn_t::iterator j = otherlist->hm.find(i->first);
      if (j != otherlist->hm.end())
      {
	 parse_error("constant \"%s\" has already been defined in namespace \"%s\"",
		     i->first, nsname);
	 n->remove(i);
      }
      else
      {      
	 j = hm.find(i->first);
	 if (j != hm.end())
	 {
	    parse_error("constant \"%s\" is already pending for namespace \"%s\"",
			i->first, nsname);
	    n->remove(i);
	 }
	 else
	 {
	    // "move" data to new list
	    hm[i->first] = i->second;
	    n->hm.erase(i);
	 }
      }
   }
}

#include <qore/QoreType.h>

inline void ConstantList::parseInit()
{
   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); i++)
   {
      printd(5, "ConstantList::parseInit() %s\n", i->first);
      parseInitConstantValue(&i->second, 0);
      printd(5, "ConstantList::parseInit() constant %s resolved to %08p %s\n", 
	     i->first, i->second, i->second ? i->second->type->name : "NULL");
      if (!i->second)
	 i->second = nothing();
   }
}

inline Hash *ConstantList::getInfo()
{
   class Hash *h = new Hash();

   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); i++)
      h->setKeyValue(i->first, i->second->RefSelf(), NULL);

   return h;
}

#else
class CNode
{
   public:
      char *name;
      class QoreNode *val;
      class CNode *next;

      inline CNode(char *n, class QoreNode *v)
      {
	 name = strdup(n);
	 val = v;
	 next = NULL;
      }
      inline ~CNode();
};

/*
  NOTE: since constants cannot hold objects (only immediate values)
  there is no need for an exception handler with the dereference
 */
inline CNode::~CNode()
{
   free(name);
   if (val)
      val->deref(NULL);
}

inline ConstantList::ConstantList()
{
   head = tail = NULL;
}

inline void ConstantList::deleteAll()
{
   while (head)
   {
      tail = head->next;
      delete head;
      head = tail;
   }
}

inline void ConstantList::reset()
{
   deleteAll();
   head = tail = NULL;
}

inline void ConstantList::add(char *name, class QoreNode *value)
{
   // first check if the constant has already been defined
   if (find(name))
   {
      parse_error("constant \"%s\" has already been defined", name);
      value->deref(NULL);
      return;
   }

   CNode *c = new CNode(name, value);
   if (tail)
      tail->next = c;
   else
      head = c;
   tail = c;
}

inline class QoreNode *ConstantList::find(char *name)
{
   class CNode *w = head;

   while (w)
   {
      if (!strcmp(name, w->name))
	 return w->val;
      w = w->next;
   }

   return NULL;
}

inline class ConstantList *ConstantList::copy()
{
   class ConstantList *ncl = new ConstantList();

   class CNode *w = head;

   while (w)
   {
      // reference value for new constant definition
      if (w->val)
	 w->val->ref();
      ncl->add(w->name, w->val);

      w = w->next;
   }

   return ncl;
}

inline void ConstantList::assimilate(class ConstantList *n)
{
   // assimilate target list
   if (tail)
      tail->next = n->head;
   else
      head = n->head;
   if (n->tail)
      tail = n->tail;
   
   // "zero" target list
   n->head = n->tail = NULL;
}

inline void ConstantList::assimilate(class ConstantList *n, class ConstantList *otherlist, char *nsname)
{
   CNode *cw = n->head;
   while (cw)
   {
      // throw parse exception if constant already defined
      if (otherlist->find(cw->name))
	 parse_error("constant \"%s\" has already been defined in namespace \"%s\"",
		     cw->name, nsname);
      else if (find(cw->name))
	 parse_error("constant \"%s\" is already pending for namespace \"%s\"",
		     cw->name, nsname);
      cw = cw->next;
   }

   assimilate(n);
}

#include <qore/QoreType.h>

inline void ConstantList::parseInit()
{
   //printd(5, "ConstantList::parseInit() head=%08p\n", head);
   class CNode *w = head;

   while (w)
   {
      printd(5, "ConstantList::parseInit() %s\n", w->name);
      parseInitConstantValue(&w->val, 0);
      printd(5, "ConstantList::parseInit() constant %s resolved to %08p %s\n", 
	     w->name, w->val, w->val ? w->val->type->name : "NULL");
      if (!w->val)
	 w->val = nothing();
      w = w->next;
   }
}

inline Hash *ConstantList::getInfo()
{
   class Hash *h = new Hash();

   class CNode *w = head;
   while (w)
   {
      h->setKeyValue(w->name, w->val->RefSelf(), NULL);
      w = w->next;
   }

   return h;
}

#endif // HAVE_QORE_HASH_MAP

#endif // _QORE_CONSTANTLIST_H
