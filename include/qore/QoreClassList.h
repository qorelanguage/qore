/*
  QoreClassList.h

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

#ifndef _QORE_QORECLASSLIST_H

#define _QORE_QORECLASSLIST_H

#include <qore/config.h>
#include <qore/QoreClass.h>

#include <string.h>
#include <stdlib.h>

#include <qore/hash_map.h>

class QoreClassList
{
   private:
      inline void deleteAll();
      inline void assimilate(QoreClassList *n);

   public:
      hm_qc_t hm;        // hash_map for name lookups
      
      inline void remove(hm_qc_t::iterator i)
      {
	 class QoreClass *qc = i->second;
	 //printd(5, "QCL::remove() this=%08p '%s' (%08p)\n", this, qc->getName(), qc);
         hm.erase(i);
	 qc->nderef();
      }

      inline QoreClassList() {}
      inline ~QoreClassList();
      inline int add(class QoreClass *ot);
      inline class QoreClass *find(char *name);
      inline class QoreClass *findChange(char *name);
      inline class QoreClassList *copy(int po);
      inline void parseInit();
      inline void parseRollback();
      inline void parseCommit(QoreClassList *n);
      inline void reset();
      inline void assimilate(QoreClassList *n, QoreClassList *otherlist, class NamespaceList *nsl, class NamespaceList *pendNSL, char *nsname);
      inline class Hash *getInfo();
};

#include <qore/QoreProgram.h>
#include <qore/Namespace.h>
#include <qore/Hash.h>

inline void QoreClassList::deleteAll()
{
   hm_qc_t::iterator i;
   while ((i = hm.begin()) != hm.end())
      remove(i);
}

inline QoreClassList::~QoreClassList()
{
   deleteAll();
}

inline int QoreClassList::add(class QoreClass *oc)
{
   if (find(oc->getName()))
      return 1;

   //printd(5, "QCL::add() this=%08p '%s' (%08p)\n", this, oc->getName(), oc);

   hm[oc->getName()] = oc;
   return 0;
}

inline class QoreClass *QoreClassList::find(char *name)
{
   hm_qc_t::iterator i = hm.find(name);
   if (i != hm.end())
      return i->second;
   return NULL;
}

inline class QoreClass *QoreClassList::findChange(char *name)
{
   hm_qc_t::iterator i = hm.find(name);
   if (i != hm.end())
   {
      class QoreClass *nc;
      if (!i->second->is_unique())
      {
	 nc = i->second;
	 hm.erase(i);
	 nc = nc->copyAndDeref();
	 hm[nc->getName()] = nc;
      }
      else
	 nc = i->second;
      return nc;
   }
   return NULL;
}

inline class QoreClassList *QoreClassList::copy(int po)
{
   class QoreClassList *nocl = new QoreClassList();

   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      if ((!(po & PO_NO_SYSTEM_CLASSES) && i->second->isSystem())
	  || (!(po & PO_NO_USER_CLASSES) && !i->second->isSystem()))
	 nocl->add(i->second->getReference());
   return nocl;
}

inline void QoreClassList::parseInit()
{
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseInit();
}

inline void QoreClassList::parseRollback()
{
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseRollback();
}

inline void QoreClassList::parseCommit(class QoreClassList *l)
{
   assimilate(l);
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseCommit();
}

inline void QoreClassList::reset()
{
   deleteAll();
}

inline void QoreClassList::assimilate(class QoreClassList *n)
{
   hm_qc_t::iterator i;
   while ((i = n->hm.begin()) != n->hm.end())
   {
      class QoreClass *nc = i->second;
      n->hm.erase(i);      
#ifdef DEBUG
      class QoreClass *c;
      if ((c = find(nc->getName())))
	 run_time_error("QoreClassList::assimilate() this=%08p DUPLICATE CLASS %08p (%s)\n", this, nc, nc->getName());
#endif
      printd(5, "QoreClassList::assimilate() this=%08p adding=%08p (%s)\n", this, nc, nc->getName());
      add(nc);
   }
}

inline void QoreClassList::assimilate(QoreClassList *n, QoreClassList *otherlist, class NamespaceList *nsl, class NamespaceList *pendNSL, char *nsname)
{
   hm_qc_t::iterator i;
   while ((i = n->hm.begin()) != n->hm.end())
   {
      if (otherlist->find(i->first))
      {
	 parse_error("class '%s' has already been defined in namespace '%s'", i->first, nsname);
	 n->remove(i);
      }
      else if (find(i->first))
      {
	 parse_error("class '%s' is already pending in namespace '%s'", i->first, nsname);
	 n->remove(i);
      }
      else if (nsl->find(i->first))
      {
	  parse_error("cannot add class '%s' to existing namespace '%s' because a subnamespace has already been defined with this name", i->first, nsname);
	  n->remove(i);
      }
      else if (pendNSL->find(i->first))
      {
	 parse_error("cannot add class '%s' to existing namespace '%s' because a pending subnamespace is already pending with this name", i->first, nsname);
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

inline class Hash *QoreClassList::getInfo()
{
   class Hash *h = new Hash();
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      h->setKeyValue(i->first, new QoreNode(i->second->getMethodList()), NULL);
   return h;
}

#endif // _QORE_QORECLASSLIST_H
