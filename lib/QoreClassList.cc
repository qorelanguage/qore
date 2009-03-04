/*
  QoreClassList.cc

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
#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/minitest.hpp>

#include <assert.h>

#ifdef DEBUG_TESTS
#  include "tests/QoreClassList_tests.cc"
#endif

void QoreClassList::deleteAll() {
   hm_qc_t::iterator i = hm.begin();
   while (i != hm.end()) {
      remove(i);
      i = hm.begin();
   }
}

QoreClassList::~QoreClassList() {
   deleteAll();
}

int QoreClassList::add(QoreClass *oc) {
   printd(5, "QCL::add() this=%08p '%s' (%08p)\n", this, oc->getName(), oc);

   if (find(oc->getName()))
      return 1;

   hm[oc->getName()] = oc;
   return 0;
}

QoreClass *QoreClassList::find(const char *name) {
   hm_qc_t::iterator i = hm.find(name);
   if (i != hm.end())
      return i->second;
   return 0;
}

QoreClass *QoreClassList::findChange(const char *name) {
   hm_qc_t::iterator i = hm.find(name);
   if (i != hm.end()) {
      QoreClass *nc;
      if (!i->second->is_unique()) {
	 nc = i->second;
	 hm.erase(i);
	 nc = nc->copyAndDeref();
	 hm[nc->getName()] = nc;
      }
      else
	 nc = i->second;
      return nc;
   }
   return 0;
}

QoreClassList *QoreClassList::copy(int po) {
   QoreClassList *nocl = new QoreClassList();

   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      if ((!(po & PO_NO_SYSTEM_CLASSES) && i->second->isSystem())
	  || (!(po & PO_NO_USER_CLASSES) && !i->second->isSystem()))
	 nocl->add(i->second->getReference());
   return nocl;
}

void QoreClassList::parseInit() {
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseInit();
}

void QoreClassList::parseRollback() {
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseRollback();
}

void QoreClassList::parseCommit(QoreClassList *l) {
   assimilate(l);
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      i->second->parseCommit();
}

void QoreClassList::reset() {
   deleteAll();
}

void QoreClassList::assimilate(QoreClassList *n) {
   hm_qc_t::iterator i = n->hm.begin();
   while (i != n->hm.end()) {
      QoreClass *nc = i->second;
      n->hm.erase(i);      
      i = n->hm.begin();

      assert(!find(nc->getName()));
      printd(5, "QoreClassList::assimilate() this=%08p adding=%08p (%s)\n", this, nc, nc->getName());
      add(nc);
   }
}

void QoreClassList::assimilate(QoreClassList *n, QoreClassList *otherlist, class QoreNamespaceList *nsl, class QoreNamespaceList *pendNSL, const char *nsname) {
   hm_qc_t::iterator i = n->hm.begin();
   while (i != n->hm.end()) {
      if (otherlist->find(i->first)) {
	 parse_error("class '%s' has already been defined in namespace '%s'", i->first, nsname);
	 n->remove(i);
      }
      else if (find(i->first)) {
	 parse_error("class '%s' is already pending in namespace '%s'", i->first, nsname);
	 n->remove(i);
      }
      else if (nsl->find(i->first)) {
	 parse_error("cannot add class '%s' to existing namespace '%s' because a subnamespace has already been defined with this name", i->first, nsname);
	 n->remove(i);
      }
      else if (pendNSL->find(i->first)) {
	 parse_error("cannot add class '%s' to existing namespace '%s' because a pending subnamespace is already pending with this name", i->first, nsname);
	 n->remove(i);
      }
      else {
	 // "move" data to new list
	 hm[i->first] = i->second;
	 n->hm.erase(i);
      }
      i = n->hm.begin();
   }
}

QoreHashNode *QoreClassList::getInfo() {
   QoreHashNode *h = new QoreHashNode();
   for (hm_qc_t::iterator i = hm.begin(); i != hm.end(); i++)
      h->setKeyValue(i->first, i->second->getMethodList(), 0);
   return h;
}
