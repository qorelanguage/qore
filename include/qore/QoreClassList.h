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

#ifdef HAVE_QORE_HASH_MAP
#include <qore/hash_map.h>
#else
class QCNode {
   public:
      class QoreClass *c;
      class QCNode *next;

      inline QCNode(class QoreClass *nc)
      {
	 c = nc;
	 next = NULL;
      }
};
#endif

class QoreClassList
{
   private:
      inline void deleteAll();
      inline void assimilate(QoreClassList *n);

   public:
#ifdef HAVE_QORE_HASH_MAP
      hm_qc_t hm;        // hash_map for name lookups
      
      inline void remove(hm_qc_t::iterator i)
      {
	 class QoreClass *qc = i->second;
         hm.erase(i);
	 qc->nderef();
      }
#else
      class QCNode *head, *tail;

      inline class QoreClass *find(int cid);
#endif

      inline QoreClassList();
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

inline QoreClassList::QoreClassList()
{
#ifndef HAVE_QORE_HASH_MAP
   head = tail = NULL;
#endif
}

inline void QoreClassList::deleteAll()
{
#ifdef HAVE_QORE_HASH_MAP
   hm_qc_t::iterator i;
   while ((i = hm.begin()) != hm.end())
      remove(i);

#else
   while (head)
   {
      tail = head->next;
      head->c->nderef();
      head = tail;
   }
#endif
}

inline QoreClassList::~QoreClassList()
{
   deleteAll();
}

inline int QoreClassList::add(class QoreClass *oc)
{
   if (find(oc->name))
      return 1;

#ifdef HAVE_QORE_HASH_MAP
   hm[oc->name] = oc;
#else
   class QCNode *w = new QCNode(oc);
   if (tail)
      tail->next = w;
   else
      head = w;
   tail = w;
#endif

   return 0;
}

inline class QoreClass *QoreClassList::find(char *name)
{
#ifdef HAVE_QORE_HASH_MAP
   hm_qc_t::iterator i = hm.find(name);
   if (i != hm.end())
      return i->second;
   return NULL;
#else
   class QCNode *w = head;

   while (w)
   {
      //printd(5, "QoreClassList::find() this=%08x arg=%s %s\n", this, name, w->name);
      if (!strcmp(name, w->c->name))
	 return w->c;
      w = w->next;
   }
   return NULL;
#endif
}

inline class QoreClass *QoreClassList::findChange(char *name)
{
#ifdef HAVE_QORE_HASH_MAP
   hm_qc_t::iterator i = hm.find(name);
   if (i != hm.end())
   {
      class QoreClass *nc;
      if (!i->second->is_unique())
      {
	 nc = i->second;
	 hm.erase(i);
	 nc = nc->copyAndDeref();
	 hm[nc->name] = nc;
      }
      else
	 nc = i->second;
      return nc;
   }
   return NULL;
#else
   class QCNode *w = head;

   while (w)
   {
      //printd(5, "QoreClassList::find() this=%08x arg=%s %s\n", this, name, w->name);
      if (!strcmp(name, w->c->name))
      {
	 if (!w->c->is_unique())
	    w->c = w->c->copyAndDeref();
	 return w->c;
      }
      w = w->next;
   }
   return NULL;
#endif
}

#ifndef HAVE_QORE_HASH_MAP
inline class QoreClass *QoreClassList::find(int cid)
{
   class QCNode *w = head;

   while (w)
   {
      //printd(5, "QoreClassList::find() this=%08x arg=%s %s\n", this, name, w->name);
      if (cid == w->c->getID())
	 return w->c;
      w = w->next;
   }
   return NULL;
}
#endif

inline class QoreClassList *QoreClassList::copy(int po)
{
   class QoreClassList *nocl = new QoreClassList();

#ifdef HAVE_QORE_HASH_MAP
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      if ((!(po & PO_NO_SYSTEM_CLASSES) && i->second->isSystem())
	  || (!(po & PO_NO_USER_CLASSES) && !i->second->isSystem()))
	 nocl->add(i->second->getReference());
#else
   class QCNode *w = head;
   while (w)
   {
      if ((!(po & PO_NO_SYSTEM_CLASSES) && w->c->isSystem())
	  || (!(po & PO_NO_USER_CLASSES) && !w->c->isSystem()))
	 nocl->add(w->c->getReference());
      w = w->next;
   }
#endif
   return nocl;
}

inline void QoreClassList::parseInit()
{
#ifdef HAVE_QORE_HASH_MAP
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseInit();
#else
   class QCNode *w = head;
   while (w)
   {
      w->c->parseInit();
      w = w->next;
   }
#endif
}

inline void QoreClassList::parseRollback()
{
#ifdef HAVE_QORE_HASH_MAP
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseRollback();
#else
   class QCNode *w = head;
   while (w)
   {
      w->c->parseRollback();
      w = w->next;
   }
#endif
}

inline void QoreClassList::parseCommit(class QoreClassList *l)
{
   assimilate(l);
#ifdef HAVE_QORE_HASH_MAP
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseCommit();
#else
   class QCNode *w = head;
   while (w)
   {
      w->c->parseCommit();
      w = w->next;
   }
#endif
}

inline void QoreClassList::reset()
{
   deleteAll();
#ifndef HAVE_QORE_HASH_MAP
   head = tail = NULL;
#endif
}

inline void QoreClassList::assimilate(class QoreClassList *n)
{
#ifdef HAVE_QORE_HASH_MAP
   hm_qc_t::iterator i;
   while ((i = n->hm.begin()) != n->hm.end())
   {
      class QoreClass *nc = i->second;
      n->hm.erase(i);      
#ifdef DEBUG
      class QoreClass *c;
      if ((c = find(nc->name)))
	 run_time_error("QoreClassList::assimilate() this=%08x DUPLICATE CLASS %08x (%s)\n", this, nc, nc->name);
#endif
      printd(5, "QoreClassList::assimilate() this=%08x adding=%08x (%s)\n", this, nc, nc->name);
      add(nc);
   }
#else
   QCNode *w = n->head;

   while (w)
   {
      QCNode *nx = w->next;

      class QoreClass *c;
#ifdef DEBUG
      if ((c = find(w->getID())))
	 run_time_error("QoreClassList::assimilate() this=%08x DUPLICATE CLASS %08x (%s)\n", this, nx, nx->name);
#endif
      printd(5, "QoreClassList::assimilate() this=%08x adding=%08x (%s)\n", this, w->c, w->c->name);
      w->next = NULL;
      add(w->c);

      w = nx;
   }
   // "zero" target list
   n->head = n->tail = NULL;
#endif
}

#ifdef HAVE_QORE_HASH_MAP
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
#else
inline void QoreClassList::assimilate(QoreClassList *n, QoreClassList *otherlist, class NamespaceList *nsl, class NamespaceList *pendNSL, char *nsname)
{
   QCNode *w = n->head;

   while (w)
   {
      if (otherlist->find(w->c->name))
	 parse_error("class '%s' has already been defined in namespace '%s'", w->c->name, nsname);
      else if (find(w->c->name))
	 parse_error("class '%s' is already pending in namespace '%s'", w->c->name, nsname);
      else if (nsl->find(w->c->name))
	  parse_error("cannot add class '%s' to existing namespace '%s' because a subnamespace has already been defined with this name", w->c->name, nsname);
       else if (pendNSL->find(w->c->name))
	 parse_error("cannot add class '%s' to existing namespace '%s' because a pending subnamespace has already been defined with this name", w->c->name, nsname);
      w = w->next;
   }
   // if there are any errors the list will be deleted anyway
   assimilate(n);
}
#endif

inline class Hash *QoreClassList::getInfo()
{
   class Hash *h = new Hash();
#ifdef HAVE_QORE_HASH_MAP
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      h->setKeyValue(i->first, new QoreNode(i->second->getMethodList()), NULL);
#else
   class QCNode *w = head;
   while (w)
   {
      h->setKeyValue(w->c->name, new QoreNode(w->c->getMethodList()), NULL);
      w = w->next;
   }

#endif
   return h;
}

#endif // _QORE_QORECLASSLIST_H
