/*
  Qoreclasslist.cpp

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
#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/minitest.hpp>

#include <assert.h>

#ifdef DEBUG_TESTS
#  include "tests/QoreClassList_tests.cpp"
#endif

void QoreClassList::deleteAll() {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      delete i->second;

   hm.clear();
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

QoreClassList::QoreClassList(const QoreClassList& old, int64 po) {
   for (hm_qc_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i)
      if ((!(po & PO_NO_SYSTEM_CLASSES) && i->second->isSystem())
	  || (!(po & PO_NO_USER_CLASSES) && !i->second->isSystem()))
	 add(new QoreClass(*i->second));
}

void QoreClassList::resolveCopy() {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      i->second->resolveCopy();
}

void QoreClassList::parseInit() {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      i->second->parseInit();
}

void QoreClassList::parseRollback() {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      i->second->parseRollback();
}

void QoreClassList::parseCommit(QoreClassList& l) {
   assimilate(l);
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      i->second->parseCommit();
}

void QoreClassList::reset() {
   deleteAll();
}

void QoreClassList::assimilate(QoreClassList& n) {
   hm_qc_t::iterator i = n.hm.begin();
   while (i != n.hm.end()) {
      QoreClass *nc = i->second;
      n.hm.erase(i);      
      i = n.hm.begin();

      assert(!find(nc->getName()));
      printd(5, "QoreClassList::assimilate() this=%08p adding=%08p (%s)\n", this, nc, nc->getName());
      add(nc);
   }
}

void QoreClassList::assimilate(QoreClassList& n, QoreClassList& otherlist, QoreNamespaceList& nsl, QoreNamespaceList& pendNSL, const char *nsname) {
   hm_qc_t::iterator i = n.hm.begin();
   while (i != n.hm.end()) {
      if (otherlist.find(i->first)) {
	 parse_error("class '%s' has already been defined in namespace '%s'", i->first, nsname);
	 n.remove(i);
      }
      else if (find(i->first)) {
	 parse_error("class '%s' is already pending in namespace '%s'", i->first, nsname);
	 n.remove(i);
      }
      else if (nsl.find(i->first)) {
	 parse_error("cannot add class '%s' to existing namespace '%s' because a subnamespace has already been defined with this name", i->first, nsname);
	 n.remove(i);
      }
      else if (pendNSL.find(i->first)) {
	 parse_error("cannot add class '%s' to existing namespace '%s' because a pending subnamespace is already pending with this name", i->first, nsname);
	 n.remove(i);
      }
      else {
	 // "move" data to new list
	 hm[i->first] = i->second;
	 n.hm.erase(i);
      }
      i = n.hm.begin();
   }
}

QoreHashNode *QoreClassList::getInfo() {
   QoreHashNode *h = new QoreHashNode;
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      h->setKeyValue(i->first, i->second->getMethodList(), 0);
   return h;
}

AbstractQoreNode *QoreClassList::findConstant(const char *cname, const QoreTypeInfo *&typeInfo) {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      AbstractQoreNode *rv = qore_class_private::parseFindLocalConstantValue(i->second, cname, typeInfo);
      if (rv)
	 return rv;
   }

   return 0;
}

AbstractQoreNode *QoreClassList::parseResolveBareword(const char *name, const QoreTypeInfo *&typeInfo) {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      AbstractQoreNode *rv = qore_class_private::parseFindLocalConstantValue(i->second, name, typeInfo);
      if (rv)
	 return rv->refSelf();

      QoreVarInfo *vi = qore_class_private::parseFindLocalStaticVar(i->second, name, typeInfo);
      if (vi)
	 return new StaticClassVarRefNode(name, *(i->second), *vi);
   }

   return 0;
}

void QoreClassList::deleteClassData(ExceptionSink *xsink) {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      qore_class_private::deleteClassData(i->second, xsink);
   }
}
