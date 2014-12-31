/*
  Qoreclasslist.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/QoreNamespaceIntern.h>
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
   printd(5, "QCL::add() this: %p '%s' (%p)\n", this, oc->getName(), oc);

   if (find(oc->getName()))
      return 1;

   hm[oc->getName()] = oc;
   return 0;
}

QoreClass* QoreClassList::find(const char *name) {
   hm_qc_t::iterator i = hm.find(name);
   return i != hm.end() ? i->second : 0;
}

const QoreClass* QoreClassList::find(const char *name) const {
   hm_qc_t::const_iterator i = hm.find(name);
   return i != hm.end() ? i->second : 0;
}

QoreClassList::QoreClassList(const QoreClassList& old, int64 po, qore_ns_private* ns) {
   for (hm_qc_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i) {
      if (!i->second->isSystem()) {
         //printd(5, "QoreClassList::QoreClassList() this: %p c: %p '%s' po & PO_NO_USER_CLASSES: %s pub: %s\n", this, i->second, i->second->getName(), po & PO_NO_USER_CLASSES ? "true": "false", qore_class_private::isPublic(*i->second) ? "true": "false");
         if (po & PO_NO_USER_CLASSES || !qore_class_private::isPublic(*i->second))
            continue;
      }
      else
         if (po & PO_NO_SYSTEM_CLASSES)
            continue;
      QoreClass* qc = new QoreClass(*i->second);
      qore_class_private::setNamespace(qc, ns);
      add(qc);
   }
}

void QoreClassList::mergeUserPublic(const QoreClassList& old, qore_ns_private* ns) {
   for (hm_qc_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i) {
      if (!qore_class_private::isUserPublic(*i->second))
	 continue;

      QoreClass* qc = new QoreClass(*i->second);
      qore_class_private::setNamespace(qc, ns);
      add(qc);
   }
}

void QoreClassList::resolveCopy() {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      qore_class_private::resolveCopy(*(i->second));
}

void QoreClassList::parseInit() {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      //printd(5, "QoreClassList::parseInit() this: %p initializing %p '%s'\n", this, i->second, i->first);
      qore_class_private::parseInit(*(i->second));
   }
}

void QoreClassList::parseRollback() {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      qore_class_private::parseRollback(*(i->second));
}

void QoreClassList::parseCommit(QoreClassList& l) {
   assimilate(l);
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      //printd(5, "QoreClassList::parseCommit() this: %p qc: %p '%s' pub: %d\n", this, i->second, i->second->getName(), qore_class_private::isPublic(*i->second));
      qore_class_private::parseCommit(*(i->second));
   }
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
      add(nc);
   }
}

void QoreClassList::assimilate(QoreClassList& n, qore_ns_private& ns) {
   hm_qc_t::iterator i = n.hm.begin();
   while (i != n.hm.end()) {
      if (ns.classList.find(i->first)) {
	 parse_error("class '%s' has already been defined in namespace '%s'", i->first, ns.name.c_str());
	 n.remove(i);
      }
      else if (find(i->first)) {
	 parse_error("class '%s' is already pending in namespace '%s'", i->first, ns.name.c_str());
	 n.remove(i);
      }
      else if (ns.nsl.find(i->first)) {
	 parse_error("cannot add class '%s' to existing namespace '%s' because a subnamespace has already been defined with this name", i->first, ns.name.c_str());
	 n.remove(i);
      }
      else if (ns.pendNSL.find(i->first)) {
	 parse_error("cannot add class '%s' to existing namespace '%s' because a pending subnamespace is already pending with this name", i->first, ns.name.c_str());
	 n.remove(i);
      }
      else {
         //printd(5, "QoreClassList::assimilate() this: %p adding: %p '%s::%s'\n", this, i->second, ns.name.c_str(), i->second->getName());

         // "move" data to new list
	 hm[i->first] = i->second;
	 qore_class_private::setNamespace(i->second, &ns);
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

void QoreClassList::clearConstants(QoreListNode& l) {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      qore_class_private::clearConstants(i->second, l);
   }
}

void QoreClassList::clear(ExceptionSink *xsink) {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      qore_class_private::clear(i->second, xsink);
   }
}

void QoreClassList::deleteClassData(ExceptionSink *xsink) {
   for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      qore_class_private::deleteClassData(i->second, xsink);
   }
}

bool ClassListIterator::isPublic() const {
   return qore_class_private::isPublic(*i->second);
}

bool ConstClassListIterator::isPublic() const {
   return qore_class_private::isPublic(*i->second);
}

bool ClassListIterator::isUserPublic() const {
   return qore_class_private::isUserPublic(*i->second);
}

bool ConstClassListIterator::isUserPublic() const {
   return qore_class_private::isUserPublic(*i->second);
}
