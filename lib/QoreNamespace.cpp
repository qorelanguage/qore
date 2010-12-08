/* -*- indent-tabs-mode: nil -*- */
/*
  QoreNamespace.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2010 David Nichols

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

#include <qore/intern/ErrnoConstants.h>
#include <qore/intern/TypeConstants.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/CallStack.h>
#include <qore/intern/QoreRegexBase.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/intern/AutoNamespaceList.h>
#include <qore/intern/ssl_constants.h>
#include <qore/intern/ConstantList.h>
#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/QoreSignal.h>
#include <qore/intern/QoreNamespaceIntern.h>

#include <qore/minitest.hpp>

// include files for default object classes
#include <qore/intern/QC_Socket.h>
#include <qore/intern/QC_SSLCertificate.h>
#include <qore/intern/QC_SSLPrivateKey.h>
#include <qore/intern/QC_Program.h>
#include <qore/intern/QC_File.h>
#include <qore/intern/QC_Dir.h>
#include <qore/intern/QC_GetOpt.h>
#include <qore/intern/QC_FtpClient.h>
#include <qore/intern/QC_HTTPClient.h>
#include <qore/intern/QC_XmlRpcClient.h>
#include <qore/intern/QC_JsonRpcClient.h>
#include <qore/intern/QC_TermIOS.h>
#include <qore/intern/QC_TimeZone.h>

// for initXmlNs()
#include <qore/intern/QC_XmlNode.h>

#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <assert.h>
#include <sys/socket.h> // for AF_INET and AF_INET6

// for Z_DEFAULT_COMPRESSION
#include <zlib.h>

#ifdef DEBUG_TESTS
// the #include "test/Namespace_tests.cpp" is on the bottom
#  include "tests/builtin_inheritance_tests.cpp"
#endif

#define MAX_RECURSION_DEPTH 20

StaticSystemNamespace staticSystemNamespace;

AutoNamespaceList ANSL;

QoreNamespace::QoreNamespace(const char *n) : priv(new qore_ns_private(this, n)) {
}

QoreNamespace::QoreNamespace() : priv(new qore_ns_private(this, "")) {
}

QoreNamespace::QoreNamespace(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl) : priv(new qore_ns_private(this, n, ocl, cl, nnsl)) {
}

void QoreNamespace::purge() {
   priv->purge();
}

QoreNamespace::~QoreNamespace() {
   //QORE_TRACE("QoreNamespace::~QoreNamespace()");
   delete priv;
}

const char *QoreNamespace::getName() const {
   return priv->name.c_str();
}

void QoreNamespace::setClassHandler(q_ns_class_handler_t class_handler) {
   priv->class_handler = class_handler;
}

// private function
QoreNamespace *QoreNamespace::resolveNameScope(const NamedScope *nscope) const {
   const QoreNamespace *sns = this;

   // find namespace
   for (unsigned i = 0; i < (nscope->size() - 1); i++)
      if (!(sns = sns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str()))) {
	 parse_error("namespace '%s' cannot be resolved while evaluating '%s' in constant declaration",
		     nscope->strlist[i].c_str(), nscope->ostr);
	 return 0;
      }
   return (QoreNamespace *)sns;
}

// private function
AbstractQoreNode *QoreNamespace::getConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv = priv->constant->find(cname, typeInfo);
   if (rv)
      return rv;

   rv = priv->pendConstant->find(cname, typeInfo);
   if (rv)
      return rv;

   rv = priv->classList->findConstant(cname, typeInfo);
   if (rv)
      return rv;

   return priv->pendClassList->findConstant(cname, typeInfo);
}

// only called while parsing before addition to namespace tree, no locking needed
void QoreNamespace::parseAddConstant(const NamedScope &nscope, AbstractQoreNode *value) {
   QoreNamespace *sns = resolveNameScope(&nscope);
   if (!sns)
      value->deref(0);
   else {
      const char *cname = nscope.strlist[nscope.size() - 1].c_str();
      if (sns->priv->constant->inList(cname)) {
	 parse_error("constant '%s' has already been defined", cname);
	 value->deref(0);
      }
      else 
	 sns->priv->pendConstant->parseAdd(cname, value);
   }
}

// public, only called in single-threaded initialization
void QoreNamespace::addSystemClass(QoreClass *oc) {
   QORE_TRACE("QoreNamespace::addSystemClass()");

#ifdef DEBUG
   if (priv->classList->add(oc))
      assert(false);
#else
   priv->classList->add(oc);
#endif
}

// public, only called when parsing for unattached namespaces
void QoreNamespace::addClass(const NamedScope *n, QoreClass *oc) {
   //printd(5, "QoreNamespace::addClass() adding ns=%s (%s, %p)\n", n->ostr, oc->getName(), oc);
   QoreNamespace *sns = resolveNameScope(n);
   if (!sns)
      delete oc;
   else
      if (sns->priv->classList->find(oc->getName())) {
	 parse_error("class '%s' already exists in namespace '%s'", oc->getName(), priv->name.c_str());
	 delete oc;
      }
      else if (sns->priv->pendClassList->add(oc)) {
	 parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), priv->name.c_str());
	 delete oc;
      }
}

void QoreNamespace::addNamespace(QoreNamespace *ns) {
   assert(!priv->classList->find(ns->priv->name.c_str()));
   assert(!priv->pendClassList->find(ns->priv->name.c_str()));
   ns->priv->parent = this;
   priv->nsl->add(ns);
}

void QoreNamespace::parseAddNamespace(QoreNamespace *ns) {
   //printd(5, "QoreNamespace::parseAddNamespace() this=%p '%s' adding %p '%s' (exists %p)\n", this, getName(), ns, ns->getName(), priv->nsl->find(ns->getName()));

   // raise an exception if namespace collides with an object name
   if (priv->classList->find(ns->getName())) {
      parse_error("namespace name '%s' collides with previously-defined class '%s'", ns->getName(), ns->getName());
      delete ns;
      return;
   }
   if (priv->pendClassList->find(ns->getName())) {
      parse_error("namespace name '%s' collides with pending class '%s'", ns->getName(), ns->getName());
      delete ns;
      return;
   }
   // see if a committed namespace with the same name already exists
   QoreNamespace *orig = priv->nsl->find(ns->getName());
   if (orig) {
      orig->assimilate(ns);
      return;
   }

   ns->priv->parent = this;
   priv->pendNSL->add(ns);
}

void QoreNamespace::parseInitConstants() {
   printd(5, "QoreNamespace::parseInitConstants() %s\n", priv->name.c_str());
   // do 2nd stage parse initialization on pending constants
   priv->pendConstant->parseInit();

   priv->pendNSL->parseInitConstants();
}

void QoreNamespace::parseInit() {
   printd(5, "QoreNamespace::parseInit() this=%p\n", this);

   // do 2nd stage parse initialization on committed classes
   priv->classList->parseInit();
   
   // do 2nd stage parse initialization on pending classes
   priv->pendClassList->parseInit();

   // do 2nd stage parse initialization on pending classes in pending lists of subnamespaces
   priv->nsl->parseInit();

   // do 2nd stage parse initialization on pending namespaces
   priv->pendNSL->parseInit();
}

void QoreNamespace::parseCommit() {
   // merge pending constant list
   priv->constant->assimilate(priv->pendConstant);

   // merge pending classes and commit pending changes to committed classes
   priv->classList->parseCommit(priv->pendClassList);

   // merge pending namespaces and repeat for all subnamespaces
   priv->nsl->parseCommit(priv->pendNSL);
}

void QoreNamespace::parseRollback() {
   printd(5, "QoreNamespace::parseRollback() %s %p\n", priv->name.c_str(), this);

   // delete pending constant list
   priv->pendConstant->parseDeleteAll();

   // delete pending changes to committed classes
   priv->classList->parseRollback();

   // delete pending classes
   priv->pendClassList->reset();

   // delete pending namespaces
   priv->pendNSL->reset();

   // do for all subnamespaces
   priv->nsl->parseRollback();
}

void QoreNamespaceList::deleteAll() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      delete i->second;
   nsmap.clear();
}

void QoreNamespaceList::assimilate(QoreNamespaceList *n) {
   for (nsmap_t::iterator i = n->nsmap.begin(), e = n->nsmap.end(); i != e; ++i) {
      assert(nsmap.find(i->first) == nsmap.end());
      nsmap[i->first] = i->second;
   }
   n->nsmap.clear();
}

void QoreNamespaceList::reset() {
   deleteAll();
}

void QoreNamespaceList::add(QoreNamespace *ns) {
   // if namespace is already registered, then assimilate
   QoreNamespace *ons;
   if ((ons = find(ns->priv->name.c_str()))) {
      //printd(5, "QoreNamespaceList::add() this=%p ns=%p (%s) assimilating with ons=%p (%s)\n", this, ns, ns->getName(), ons, ons->getName());
      ons->assimilate(ns);
      return;
   }
   nsmap[ns->priv->name] = ns;
}

QoreNamespace::QoreNamespace(const QoreNamespace &old, int64 po) : priv(new qore_ns_private(*old.priv, this, po)) {
}

QoreNamespace *QoreNamespace::copy(int po) const {
   //printd(5, "QoreNamespace::copy() (deprecated) this=%p po=%d %s\n", this, po, priv->name.c_str());
   return new QoreNamespace(*this, po);
}

QoreNamespace *QoreNamespace::copy(int64 po) const {
   //printd(5, "QoreNamespace::copy() this=%p po=%lld %s\n", this, po, priv->name.c_str());
   return new QoreNamespace(*this, po);
}

QoreNamespaceList *QoreNamespaceList::copy(int64 po, const QoreNamespace *parent) {
   //printd(5, "QoreNamespaceList::copy() this=%p po=%lld size=%d\n", this, po, nsmap.size());
   QoreNamespaceList *nsl = new QoreNamespaceList();

   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      QoreNamespace *ns = i->second->copy(po);
      ns->priv->parent = parent;
      nsl->nsmap[i->first] = ns;
   }

   return nsl;
}

void QoreNamespaceList::resolveCopy() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->classList->resolveCopy();
}

void QoreNamespaceList::parseInitConstants() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->parseInitConstants();
}

void QoreNamespaceList::deleteAllConstants(ExceptionSink *xsink) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->constant->deleteAll(xsink);
}

void QoreNamespaceList::parseInit() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->parseInit();
}

void QoreNamespaceList::parseCommit(QoreNamespaceList *l) {
   assimilate(l);

   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->parseCommit();
}

void QoreNamespaceList::parseRollback() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->parseRollback();
}

QoreClass *QoreNamespace::parseFindLocalClass(const char *cname) const {
   QoreClass *rv = priv->classList->find(cname);
   if (!rv)
      rv = priv->pendClassList->find(cname);
   return rv;
}

// public: only called during Qore initialization to setup
// system constant types directly in Qore system namespaces
// FIXME: change to addSystemConstant() to avoid confusion
void QoreNamespace::addConstant(const char *cname, AbstractQoreNode *val) {
   priv->constant->add(cname, val);
}

void QoreNamespace::addConstant(const char *cname, AbstractQoreNode *val, const QoreTypeInfo *typeInfo) {
   priv->constant->add(cname, val, typeInfo);
}

void QoreNamespace::addInitialNamespace(QoreNamespace *ns) {
   priv->nsl->add(ns);
}

QoreNamespace *QoreNamespace::findCreateNamespacePath(const char *nspath) {
   assert(nspath);
   NamedScope ns(nspath);

   // iterate through each level of the namespace path and find/create namespaces as needed
   QoreNamespaceList *nsl = priv->nsl;
   QoreNamespace *parent = this;
   QoreNamespace *tns = 0;
   for (unsigned i = 0; i < ns.size(); ++i) {
      const char *nsn = ns.strlist[i].c_str();
      tns = nsl->find(nsn);
      if (!tns) {
	 tns = new QoreNamespace(nsn);
	 tns->priv->parent = parent;
	 nsl->add(tns);
      }
      parent = tns;
      nsl = tns->priv->nsl;
   }

   return tns;
}

QoreClass *QoreNamespace::findLocalClass(const char *cname) const {
   return priv->classList->find(cname);
}

QoreNamespace *QoreNamespace::findLocalNamespace(const char *cname) const {
   return priv->nsl->find(cname);
}

const QoreNamespace *QoreNamespace::getParent() const {
   return priv->parent;
}

void QoreNamespace::deleteData(ExceptionSink *xsink) {
   // clear all constants
   priv->constant->deleteAll(xsink);
   // clear all constants and static class vars
   priv->classList->deleteClassData(xsink);
   // repeat for all subnamespaces
   priv->nsl->deleteData(xsink);
}

// QoreNamespaceList::parseResolveNamespace()
// does a recursive breadth-first search to resolve a namespace declaration
QoreNamespace *QoreNamespaceList::parseResolveNamespace(const NamedScope *name, unsigned *matched) {
   QORE_TRACE("QoreNamespaceList::parseResolveNamespace()");

   QoreNamespace *ns = 0;

   // search first level of all sub namespaces
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((ns = i->second->parseMatchNamespace(name, matched)))
	 return ns;
   }      

   // now search in all sub namespace lists
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((ns = i->second->priv->nsl->parseResolveNamespace(name, matched)))
	 break;
      if ((ns = i->second->priv->pendNSL->parseResolveNamespace(name, matched)))
	 break;
      //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);
   }

   return ns;
}

// QoreNamespaceList::parseFindConstantValue()
AbstractQoreNode *QoreNamespaceList::parseFindConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) {
   QORE_TRACE("QoreNamespaceList::parseFindConstantValue()");
   
   AbstractQoreNode *rv = 0;
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((rv = i->second->getConstantValue(cname, typeInfo)))
	 return rv;
   }

   // check all levels
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((rv = i->second->priv->nsl->parseFindConstantValue(cname, typeInfo)))
	 break;
      if ((rv = i->second->priv->pendNSL->parseFindConstantValue(cname, typeInfo)))
	 break;
   }

   return rv;
}

AbstractQoreNode *QoreNamespaceList::parseResolveBareword(const char *name, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv;
   for (nsmap_t::const_iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((rv = i->second->priv->parseResolveBareword(name, typeInfo)))
	 return rv;
   }
   return 0;
}

AbstractQoreNode *QoreNamespaceList::parseResolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv;
   for (nsmap_t::const_iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((rv = i->second->priv->parseResolveScopedReference(ns, m, typeInfo)))
	 return rv;
   }
   return 0;
}

/*
static void showNSL(QoreNamespaceList *nsl) {
   printd(5, "showNSL() dumping %p\n", nsl);
   for (int i = 0; i < nsl->num_namespaces; i++)
      printd(5, "showNSL()  %d: %p %s (list: %p)\n", i, nsl->nslist[i], nsl->nslist[i]->name, nsl->nslist[i]->nsl);
}
*/

// QoreNamespaceList::parseFindScopedConstantValue()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
AbstractQoreNode *QoreNamespaceList::parseFindScopedConstantValue(const NamedScope *name, unsigned *matched, const QoreTypeInfo *&typeInfo) {
   AbstractQoreNode *rv = 0;

   QORE_TRACE("QoreNamespaceList::parseFindScopedConstantValue()");
   printd(5, "QoreNamespaceList::parseFindScopedConstantValue(this=%p) target: %s\n", this, name->ostr);

   //showNSL(this);
   // see if a complete match can be found at the first level
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((rv = i->second->parseMatchScopedConstantValue(name, matched, typeInfo)))
	 return rv;
   }

   // now search all sub namespaces
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((rv = i->second->priv->nsl->parseFindScopedConstantValue(name, matched, typeInfo)))
	 break;
      if ((rv = i->second->priv->pendNSL->parseFindScopedConstantValue(name, matched, typeInfo)))
	 break;
   }

   return rv;
}

// QoreNamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
QoreClass *QoreNamespaceList::parseFindScopedClassWithMethod(const NamedScope *name, unsigned *matched) {
   QoreClass *oc = 0;

   // see if a complete match can be found at the first level
   for (nsmap_t::const_iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((oc = i->second->parseMatchScopedClassWithMethod(name, matched)))
	 return oc;
   }

   // now search all sub namespaces
   for (nsmap_t::const_iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((oc = i->second->priv->pendNSL->parseFindScopedClassWithMethod(name, matched)))
	 break;
      if ((oc = i->second->priv->nsl->parseFindScopedClassWithMethod(name, matched)))
	 break;
   }

   return oc;
}

QoreClass *QoreNamespaceList::parseFindClass(const char *ocname) {
   QoreClass *oc = 0;

   // see if a match can be found at the first level
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((oc = i->second->priv->findLoadClass(i->second, ocname)))
	 return oc;
      // check pending classes
      if ((oc = i->second->priv->pendClassList->find(ocname)))
	 return oc;
   }

   // check all levels
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((oc = i->second->priv->nsl->parseFindClass(ocname)))
	 break;
      if ((oc = i->second->priv->pendNSL->parseFindClass(ocname)))
	 break;
   }

   return oc;
}

// QoreNamespaceList::parseFindScopedClass()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
QoreClass *QoreNamespaceList::parseFindScopedClass(const NamedScope *name, unsigned *matched) {
   QoreClass *oc = 0;

   // see if a complete match can be found at the first level
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((oc = i->second->parseMatchScopedClass(name, matched)))
	 return oc;
   }

   // now search all sub namespaces
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((oc = i->second->priv->nsl->parseFindScopedClass(name, matched)))
	 break;
      if ((oc = i->second->priv->pendNSL->parseFindScopedClass(name, matched)))
	 break;
   }

   return oc;
}

void QoreNamespaceList::deleteData(ExceptionSink *xsink) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->deleteData(xsink);
}

// returns 0 for success, non-zero return value means error
int RootQoreNamespace::addMethodToClass(const NamedScope *scname, MethodVariantBase *qcmethod, bool static_flag) {
   std::auto_ptr<MethodVariantBase> v(qcmethod);

   // find class
   QoreClass *oc;

   const char *cname  = scname->strlist[scname->size() - 2].c_str();
   const char *method = scname->strlist[scname->size() - 1].c_str();

   // if there is no namespace specified, then just find class
   if (scname->size() == 2) {
      oc = rootFindClass(cname);
      if (!oc) {
	 parse_error("reference to undefined class '%s' while trying to add method '%s'", cname, method);
	 return -1;
      }
   }
   else {
      unsigned m = 0;
      oc = rootFindScopedClassWithMethod(scname, &m);
      if (!oc) {
	 if (m != (scname->size() - 2))
	    parse_error("cannot resolve namespace '%s' in '%s()'", scname->strlist[m].c_str(), scname->ostr);
	 else
	    parse_error("class '%s' does not exist", cname);
	 return -1;
      }
   }

   return oc->addUserMethod(method, v.release(), static_flag);
}

QoreClass *RootQoreNamespace::parseFindClass(const char *cname) const {
   QoreClass *oc = rootFindClass(cname);
   if (!oc)
      parse_error("reference to undefined class '%s'", cname);

   return oc;
}

QoreClass *RootQoreNamespace::parseFindScopedClass(const NamedScope *nscope) {
   QoreClass *oc;
   // if there is no namespace specified, then just find class
   if (nscope->size() == 1) {
      oc = rootFindClass(nscope->strlist[0].c_str());
      if (!oc)
	 parse_error("reference to undefined class '%s'", nscope->ostr);
   }
   else {
      unsigned m = 0;
      oc = rootFindScopedClass(nscope, &m);

      if (!oc) {
	 if (m != (nscope->size() - 1))
	    parse_error("cannot resolve namespace '%s' in '%s()'", nscope->strlist[m].c_str(), nscope->ostr);
	 else {
	    QoreString err;
	    err.sprintf("cannot find class '%s' in any namespace '", nscope->getIdentifier());
	    for (unsigned i = 0; i < (nscope->size() - 1); i++) {
	       err.concat(nscope->strlist[i].c_str());
	       if (i != (nscope->size() - 2))
		  err.concat("::");
	    }
	    err.concat("'");
	    parse_error(err.getBuffer());
	 }
      }
   }
   printd(5, "RootQoreNamespace::parseFindScopedClass('%s') returning %p\n", nscope->ostr, oc);
   return oc;
}

QoreClass *RootQoreNamespace::parseFindScopedClassWithMethod(const NamedScope *scname) {
   // must have at least 2 elements
   assert(scname->size() > 1);

   QoreClass *oc;

   unsigned m = 0;
   oc = rootFindScopedClassWithMethod(scname, &m);
   
   if (!oc) {
      if (m >= (scname->size() - 2))
	 parse_error("cannot resolve class '%s' in '%s()'", scname->strlist[m].c_str(), scname->ostr);
      else  {	 
	 QoreString err;
	 err.sprintf("cannot find class '%s' in any namespace '", scname->strlist[scname->size() - 2].c_str());
	 for (unsigned i = 0; i < (scname->size() - 2); i++) {
	    err.concat(scname->strlist[i].c_str());
	    if (i != (scname->size() - 3))
	       err.concat("::");
	 }
	 err.concat("'");
	 parse_error(err.getBuffer());
      }
   }
   
   printd(5, "RootQoreNamespace::parseFindScopedClassWithMethod('%s') returning %p\n", scname->ostr, oc);
   return oc;
}

// returns 0 for success, non-zero for error
int RootQoreNamespace::resolveBareword(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo) const {
   assert(*node && (*node)->getType() == NT_BAREWORD);
   BarewordNode *b = reinterpret_cast<BarewordNode *>(*node);

   QoreClass *pc = getParseClass();

   //printd(5, "RootQoreNamespace::resolveBareword(%s) pc=%p\n", b->str, pc);

   AbstractQoreNode *rv = 0;

   QoreProgram *pgm = getProgram();
   bool abr = pgm->getParseOptions64() & PO_ALLOW_BARE_REFS;

   if (abr) {
      bool in_closure;
      LocalVar *id = find_local_var(b->str, in_closure);
      if (id) {
         typeInfo = id->getTypeInfo();
         *node = new VarRefNode(b->takeString(), id, in_closure);
         b->deref();
         return 0;
      }
   }

   // if there is a current parse class context, then check it first
   if (pc) {
      // check for member reference
      if (abr) {
         if (!qore_class_private::parseResolveInternalMemberAccess(pc, b->str, typeInfo)) {
            *node = new SelfVarrefNode(b->takeString());
            b->deref();
            return 0;
         }
      }

      rv = qore_class_private::parseFindConstantValue(pc, b->str, typeInfo);
      if (rv)
	 rv->ref();
      else {
	 // check for class static var reference
	 const QoreClass *qc = 0;
	 QoreVarInfo *vi = qore_class_private::parseFindStaticVar(pc, b->str, qc, typeInfo);
	 if (vi) {
	    assert(qc);
	    *node = new StaticClassVarRefNode(b->str, *qc, *vi);
	    b->deref();
	    return 0;
	 }
      }
   }

   if (!rv) {
      // try to resolve a global variable
      if (abr) {
         Var *v = pgm->findGlobalVar(b->str);
         if (v) {
            *node = new GlobalVarRefNode(b->takeString(), v);
            b->deref();
            return 0;
         }
      }

      // try to resolve to constant, class constant, or static class variable
      rv = priv->parseResolveBareword(b->str, typeInfo);
   }

   if (rv) {
      b->deref();
      *node = rv;
      return 0;
   }

   //printd(5, "RootQoreNamespace::resolveBareword(%s) %p %s-> %p %s\n", b->str, *node, (*node)->getTypeName(), rv, get_type_name(rv));
   parse_error("cannot resolve bareword '%s' to any reachable object", b->str);
   return -1;
}

int RootQoreNamespace::resolveScopedReference(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo) const {
   assert(node && (*node)->getType() == NT_CONSTANT);
   ScopedRefNode *c = reinterpret_cast<ScopedRefNode *>(*node);

   if (c->scoped_ref->size() == 1) {
      AbstractQoreNode *rv = priv->parseResolveBareword(c->scoped_ref->ostr, typeInfo);
      if (rv) {
         c->deref();
         *node = rv;
         return 0;
      }

      //printd(5, "RootQoreNamespace::resolveBareword(%s) %p %s-> %p %s\n", b->str, *node, (*node)->getTypeName(), rv, get_type_name(rv));
      parse_error("cannot resolve bareword '%s' to any reachable object", c->scoped_ref->ostr);
      return -1;
   }

   unsigned m = 0;
   AbstractQoreNode *rv = priv->parseResolveScopedReference(*c->scoped_ref, m, typeInfo);
   if (rv) {
      c->deref();
      *node = rv;
      //printd(5, "RootQoreNamespace::resolveScopedReference(%s) %p %s-> %p %s\n", c->scoped_ref->ostr, *node, (*node)->getTypeName(), rv, rv->getTypeName());
      return 0;
   }

   NamedScope &ns = *c->scoped_ref;
   // raise parse exception
   if (m != (ns.size() - 1))
      parse_error("cannot find any namespace or class '%s' in '%s' with a reference to constant or static class variable '%s'", ns.strlist[m].c_str(), ns.ostr, ns.getIdentifier());
   else {
      QoreString err;
      err.sprintf("cannot resolve bareword '%s' to any reachable object in any namespace '", ns.getIdentifier());
      for (unsigned i = 0; i < (ns.size() - 1); i++) {
	 err.concat(ns.strlist[i].c_str());
	 if (i != (ns.size() - 2))
	    err.concat("::");
      }
      err.concat("'");
      parse_error(err.getBuffer());
   }

   //printd(5, "RootQoreNamespace::resolveScopedReference(%s) not found\n", c->scoped_ref->ostr);
   return -1;
}

AbstractQoreNode *RootQoreNamespace::findConstantValue(const char *cname, int level, const QoreTypeInfo *&typeInfo) const {
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH) {
      parse_error("recursive constant definitions too deep resolving '%s'", cname);
      return 0;
   }

   AbstractQoreNode *rv = rootFindConstantValue(cname, typeInfo);
   if (!rv)
      parse_error("constant '%s' cannot be resolved in any namespace", cname);
   else {
      // here we enforce PO_NO_TERMINAL_IO for stdin, stdout, and stderr constants
      if (rv->getType() == NT_OBJECT && getProgram()->getParseOptions() & PO_NO_TERMINAL_IO) {
	 QoreObject *o = reinterpret_cast<QoreObject *>(rv);
	 if (o->isSystemObject() && o->validInstanceOf(CID_FILE))
	    parseException("ILLEGAL-CONSTANT-ACCESS", "File I/O constants cannot be accessed with PO_NO_TERMINAL_IO");
      }
   }
   return rv;
}

// called in 2nd stage of parsing to resolve constant references
AbstractQoreNode *RootQoreNamespace::findConstantValue(const NamedScope *scname, int level, const QoreTypeInfo *&typeInfo) const {
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH) {
      parse_error("recursive constant definitions too deep resolving '%s'", scname->ostr);
      return 0;
   }

   AbstractQoreNode *rv;

   if (scname->size() == 1) {
      rv = rootFindConstantValue(scname->ostr, typeInfo);
      if (!rv) {
	 parse_error("constant '%s' cannot be resolved in any namespace", scname->ostr);
	 return 0;
      }
      return rv;
   }

   unsigned m = 0;
   rv = rootFindScopedConstantValue(scname, &m, typeInfo);
   if (rv)
      return rv;

   if (m != (scname->size() - 1))
      parse_error("cannot resolve namespace '%s' in constant reference '%s'", scname->strlist[m].c_str(), scname->ostr);
   else {
      QoreString err;
      err.sprintf("cannot find constant '%s' in any namespace '", scname->getIdentifier());
      for (unsigned i = 0; i < (scname->size() - 1); i++) {
	 err.concat(scname->strlist[i].c_str());
	 if (i != (scname->size() - 2))
	    err.concat("::");
      }
      err.concat("'");
      parse_error(err.getBuffer());
   }
   return 0;
}

// public, only called either in single-threaded initialization or
// while the program-level parse lock is held
void QoreNamespace::addClass(QoreClass *oc) {
   QORE_TRACE("QoreNamespace::addClass()");
   //printd(5, "QoreNamespace::addClass() adding str=%s (%p)\n", oc->name, oc);
   // raise an exception if object name collides with a namespace
   if (priv->nsl->find(oc->getName())) {
      parse_error("class name '%s' collides with previously-defined namespace '%s'", oc->getName(), oc->getName());
      delete oc;
   }
   else if (priv->pendNSL->find(oc->getName())) {
      parse_error("class name '%s' collides with pending namespace '%s'", oc->getName(), oc->getName());
      delete oc;
   }
   else if (priv->classList->find(oc->getName())) {
      parse_error("class '%s' already exists in namespace '%s'", oc->getName(), priv->name.c_str());
      delete oc;
   }
   else if (priv->pendClassList->add(oc)) {
      parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), priv->name.c_str());
      delete oc;
   }
}

void QoreNamespace::assimilate(QoreNamespace *ns) {
   // make sure there are no objects in the committed lists
   assert(ns->priv->nsl->empty());
   assert(ns->priv->constant->empty());
   assert(ns->priv->classList->empty());

   // assimilate pending constants
   // assimilate target list - if there were errors then the list will be deleted anyway
   priv->pendConstant->assimilate(ns->priv->pendConstant, priv->constant, priv->name.c_str());

   // assimilate classes
   priv->pendClassList->assimilate(ns->priv->pendClassList, priv->classList, priv->nsl, priv->pendNSL, priv->name.c_str());

   // assimilate sub namespaces
   for (nsmap_t::iterator i = ns->priv->pendNSL->nsmap.begin(), e = ns->priv->pendNSL->nsmap.end(); i != e; ++i) {
      // throw parse exception if name is already defined
      if (priv->nsl->find(i->second->priv->name.c_str()))
	 parse_error("subnamespace '%s' has already been defined in namespace '%s'",
		     i->second->priv->name.c_str(), priv->name.c_str());
      else if (priv->pendNSL->find(i->second->priv->name.c_str()))
	 parse_error("subnamespace '%s' is already pending in namespace '%s'",
		     i->second->priv->name.c_str(), priv->name.c_str());
      else if (priv->classList->find(i->second->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class has already been defined with this name",
		     i->second->priv->name.c_str(), priv->name.c_str());
      else if (priv->pendClassList->find(i->second->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class is already pending with this name",
		     i->second->priv->name.c_str(), priv->name.c_str());
   }
   // assimilate target list
   priv->pendNSL->assimilate(ns->priv->pendNSL);

   // delete source namespace
   delete ns;
}

// QoreNamespace::parseMatchNamespace()
// will only be called if there is a match with the name and nscope->size() > 1
QoreNamespace *QoreNamespace::parseMatchNamespace(const NamedScope *nscope, unsigned *matched) const {
   // see if starting name matches this namespace
   if (!strcmp(nscope->strlist[0].c_str(), priv->name.c_str())) {
      const QoreNamespace *ns = this;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // check for a match of the structure in this namespace
      for (unsigned i = 1; i < (nscope->size() - 1); i++) {
	 ns = ns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str());
	 if (!ns)
	    break;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
      return (QoreNamespace *)ns;
   }

   return 0;
}

QoreClass *QoreNamespace::parseMatchScopedClassWithMethod(const NamedScope *nscope, unsigned *matched) {
   printd(5, "QoreNamespace::parseMatchScopedClassWithMethod(this=%p) %s class=%s (%s)\n", this, priv->name.c_str(), nscope->strlist[nscope->size() - 2].c_str(), nscope->ostr);

   QoreNamespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->size() > 2) {
      // if first namespace doesn't match, then return 0
      if (strcmp(nscope->strlist[0].c_str(), priv->name.c_str()))
	 return 0;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (unsigned i = 1; i < (nscope->size() - 2); i++) {	 
	 ns = ns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str());
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }

   // check last namespaces
   QoreClass *rv = ns->priv->findLoadClass(ns, nscope->strlist[nscope->size() - 2].c_str());
   if (!rv)
      rv = ns->priv->pendClassList->find(nscope->strlist[nscope->size() - 2].c_str());

   return rv;
}

QoreClass *QoreNamespace::parseMatchScopedClass(const NamedScope *nscope, unsigned *matched) {
   if (nscope->strlist[0] != priv->name)
      return 0;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   printd(5, "QoreNamespace::parseMatchScopedClass() matched %s in %s\n", priv->name.c_str(), nscope->ostr);

   QoreNamespace *ns = this;
   
   // if we need to follow the namespaces, then do so
   if (nscope->size() > 2) {
      for (unsigned i = 1; i < (nscope->size() - 1); i++) {
	 ns = ns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str());
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   QoreClass *rv = ns->priv->findLoadClass(ns, nscope->strlist[nscope->size() - 1].c_str());
   if (!rv)
      rv = ns->priv->pendClassList->find(nscope->strlist[nscope->size() - 1].c_str());
   return rv;
}

AbstractQoreNode *QoreNamespace::parseMatchScopedConstantValue(const NamedScope *nscope, unsigned *matched, const QoreTypeInfo *&typeInfo) const {
   printd(5, "QoreNamespace::parseMatchScopedConstantValue) trying to find %s in %s (%p) typeInfo=%p\n", 
	  nscope->getIdentifier(), priv->name.c_str(), getConstantValue(nscope->getIdentifier(), typeInfo));

   if (nscope->strlist[0] != priv->name) {
      // check for a class constant
      if (nscope->size() == 2) {
	 QoreClass *qc = parseFindLocalClass(nscope->strlist[0].c_str());
	 return qc ? qore_class_private::parseFindLocalConstantValue(qc, nscope->getIdentifier(), typeInfo) : 0;
      }

      return 0;
   }

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   const QoreNamespace *ns = this;

   // if we need to follow the namespaces, then do so
   if (nscope->size() > 2) {
      unsigned last = nscope->size() - 1;
      for (unsigned i = 1; i < last; i++) {
	 const char *oname = nscope->strlist[i].c_str();
	 ns = ns->priv->parseFindLocalNamespace(oname);
	 if (!ns) {
	    // if we are on the last element before the constant in the namespace path list,
	    // then check for a class constant
	    if (i == (last - 1)) {
	       QoreClass *qc = ns->parseFindLocalClass(oname);
	       return qc ? qore_class_private::parseFindLocalConstantValue(qc, nscope->getIdentifier(), typeInfo) : 0;
	    }
	    return 0;
	 }
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }

   return ns->getConstantValue(nscope->getIdentifier(), typeInfo);
}

QoreHashNode *QoreNamespace::getConstantInfo() const {
   return priv->constant->getInfo();
}

QoreHashNode *QoreNamespace::getClassInfo() const {
   return priv->classList->getInfo();
}

// returns a hash of namespace information
QoreHashNode *QoreNamespace::getInfo() const {
   QoreHashNode *h = new QoreHashNode();

   h->setKeyValue("constants", getConstantInfo(), 0);
   h->setKeyValue("classes", getClassInfo(), 0);

   if (!priv->nsl->nsmap.empty()) {
      QoreHashNode *nsh = new QoreHashNode;

      for (nsmap_t::iterator i = priv->nsl->nsmap.begin(), e = priv->nsl->nsmap.end(); i != e; ++i)
	 nsh->setKeyValue(i->second->priv->name.c_str(), i->second->getInfo(), 0);

      h->setKeyValue("subnamespaces", nsh, 0);
   }

   return h;
}

void QoreNamespace::setName(const char *nme) {
   assert(priv->name.empty());
   priv->name = nme;
}

// only called with RootNS
AbstractQoreNode *RootQoreNamespace::rootFindConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv;
   if (!(rv = getConstantValue(cname, typeInfo))
       && (!(rv = priv->nsl->parseFindConstantValue(cname, typeInfo))))
      rv = priv->pendNSL->parseFindConstantValue(cname, typeInfo);
      
   return rv;
}

// only called with RootNS
void RootQoreNamespace::rootAddClass(const NamedScope *nscope, QoreClass *oc) {
   QORE_TRACE("RootQoreNamespace::rootAddClass()");

   QoreNamespace *sns = rootResolveNamespace(nscope);

   if (sns) {
      printd(5, "RootQoreNamespace::rootAddClass() '%s' adding %s:%p to %s:%p\n", nscope->ostr, 
	     oc->getName(), oc, sns->priv->name.c_str(), sns);
      sns->addClass(oc);
   }
   else
      delete oc;
}

void RootQoreNamespace::rootAddConstant(const NamedScope &nscope, AbstractQoreNode *value) {
   QoreNamespace *sns = rootResolveNamespace(&nscope);

   if (sns) {
      printd(5, "RootQoreNamespace::rootAddConstant() %s: adding %s to %s (value=%p type=%s)\n", nscope.ostr, 
	     nscope.getIdentifier(), sns->priv->name.c_str(), value, value ? value->getTypeName() : "(none)");
      sns->priv->pendConstant->parseAdd(nscope.strlist[nscope.size() - 1].c_str(), value);
   }
   else
      value->deref(0);
}

// public
QoreClass *RootQoreNamespace::rootFindClass(const char *ocname) const {
   QORE_TRACE("RootQoreNamespace::rootFindClass");
   QoreClass *oc;
   if (!(oc = priv->classList->find(ocname))
       && !(oc = priv->pendClassList->find(ocname))
       && !(oc = priv->nsl->parseFindClass(ocname)))
      oc = priv->pendNSL->parseFindClass(ocname);

   return oc;
}

QoreNamespace *RootQoreNamespace::rootResolveNamespace(const NamedScope *nscope) {
   if (nscope->size() == 1)
      return this;

   QoreNamespace *ns;
   unsigned match = 0;

   if (!(ns = parseMatchNamespace(nscope, &match))
       && !(ns = priv->nsl->parseResolveNamespace(nscope, &match))
       && !(ns = priv->pendNSL->parseResolveNamespace(nscope, &match)))

   if (!ns)
      parse_error("cannot resolve namespace '%s' in '%s'", nscope->strlist[match].c_str(), nscope->ostr);

   return ns;
}

// private
QoreClass *RootQoreNamespace::rootFindScopedClassWithMethod(const NamedScope *nscope, unsigned *matched) {
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched))
       && !(oc = priv->nsl->parseFindScopedClassWithMethod(nscope, matched)))
      oc = priv->pendNSL->parseFindScopedClassWithMethod(nscope, matched);
   return oc;
}

// private
// will always be called with a namespace (nscope->size() > 1)
QoreClass *RootQoreNamespace::rootFindScopedClass(const NamedScope *nscope, unsigned *matched) {
   QoreClass *oc = parseMatchScopedClass(nscope, matched);
   if (!oc && !(oc = priv->nsl->parseFindScopedClass(nscope, matched)))
      oc = priv->pendNSL->parseFindScopedClass(nscope, matched);
   return oc;
}

// private, will always be called with nscope->size() > 1
AbstractQoreNode *RootQoreNamespace::rootFindScopedConstantValue(const NamedScope *nscope, unsigned *matched, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv = parseMatchScopedConstantValue(nscope, matched, typeInfo);
   if (!rv && !(rv = priv->nsl->parseFindScopedConstantValue(nscope, matched, typeInfo)))
      rv = priv->pendNSL->parseFindScopedConstantValue(nscope, matched, typeInfo);
   return rv;
}

void RootQoreNamespace::addQoreNamespace(QoreNamespace *qns) {
   addInitialNamespace(qns);
   qoreNS = qns;
}

static void addSignalConstants(class QoreNamespace *ns) {
   QoreHashNode *nh = new QoreHashNode();
   QoreHashNode *sh = new QoreHashNode();
#ifdef SIGHUP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGHUP), new QoreStringNode("SIGHUP"), 0);
   sh->setKeyValue("SIGHUP", new QoreBigIntNode(SIGHUP), 0);
   ns->addConstant("SIGHUP", new QoreBigIntNode(SIGHUP));
#endif
#ifdef SIGINT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINT), new QoreStringNode("SIGINT"), 0);
   sh->setKeyValue("SIGINT", new QoreBigIntNode(SIGINT), 0);
   ns->addConstant("SIGINT", new QoreBigIntNode(SIGINT));
#endif
#ifdef SIGQUIT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGQUIT), new QoreStringNode("SIGQUIT"), 0);
   sh->setKeyValue("SIGQUIT", new QoreBigIntNode(SIGQUIT), 0);
   ns->addConstant("SIGQUIT", new QoreBigIntNode(SIGQUIT));
#endif
#ifdef SIGILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGILL), new QoreStringNode("SIGILL"), 0);
   sh->setKeyValue("SIGILL", new QoreBigIntNode(SIGILL), 0);
   ns->addConstant("SIGILL", new QoreBigIntNode(SIGILL));
#endif
#ifdef SIGTRAP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTRAP), new QoreStringNode("SIGTRAP"), 0);
   sh->setKeyValue("SIGTRAP", new QoreBigIntNode(SIGTRAP), 0);
   ns->addConstant("SIGTRAP", new QoreBigIntNode(SIGTRAP));
#endif
#ifdef SIGABRT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGABRT), new QoreStringNode("SIGABRT"), 0);
   sh->setKeyValue("SIGABRT", new QoreBigIntNode(SIGABRT), 0);
   ns->addConstant("SIGABRT", new QoreBigIntNode(SIGABRT));
#endif
#ifdef SIGPOLL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPOLL), new QoreStringNode("SIGPOLL"), 0);
   sh->setKeyValue("SIGPOLL", new QoreBigIntNode(SIGPOLL), 0);
   ns->addConstant("SIGPOLL", new QoreBigIntNode(SIGPOLL));
#endif
#ifdef SIGIOT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIOT), new QoreStringNode("SIGIOT"), 0);
   sh->setKeyValue("SIGIOT", new QoreBigIntNode(SIGIOT), 0);
   ns->addConstant("SIGIOT", new QoreBigIntNode(SIGIOT));
#endif
#ifdef SIGEMT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGEMT), new QoreStringNode("SIGEMT"), 0);
   sh->setKeyValue("SIGEMT", new QoreBigIntNode(SIGEMT), 0);
   ns->addConstant("SIGEMT", new QoreBigIntNode(SIGEMT));
#endif
#ifdef SIGFPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGFPE), new QoreStringNode("SIGFPE"), 0);
   sh->setKeyValue("SIGFPE", new QoreBigIntNode(SIGFPE), 0);
   ns->addConstant("SIGFPE", new QoreBigIntNode(SIGFPE));
#endif
#ifdef SIGKILL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGKILL), new QoreStringNode("SIGKILL"), 0);
   sh->setKeyValue("SIGKILL", new QoreBigIntNode(SIGKILL), 0);
   ns->addConstant("SIGKILL", new QoreBigIntNode(SIGKILL));
#endif
#ifdef SIGBUS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGBUS), new QoreStringNode("SIGBUS"), 0);
   sh->setKeyValue("SIGBUS", new QoreBigIntNode(SIGBUS), 0);
   ns->addConstant("SIGBUS", new QoreBigIntNode(SIGBUS));
#endif
#ifdef SIGSEGV
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSEGV), new QoreStringNode("SIGSEGV"), 0);
   sh->setKeyValue("SIGSEGV", new QoreBigIntNode(SIGSEGV), 0);
   ns->addConstant("SIGSEGV", new QoreBigIntNode(SIGSEGV));
#endif
#ifdef SIGSYS
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSYS), new QoreStringNode("SIGSYS"), 0);
   sh->setKeyValue("SIGSYS", new QoreBigIntNode(SIGSYS), 0);
   ns->addConstant("SIGSYS", new QoreBigIntNode(SIGSYS));
#endif
#ifdef SIGPIPE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPIPE), new QoreStringNode("SIGPIPE"), 0);
   sh->setKeyValue("SIGPIPE", new QoreBigIntNode(SIGPIPE), 0);
   ns->addConstant("SIGPIPE", new QoreBigIntNode(SIGPIPE));
#endif
#ifdef SIGALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGALRM), new QoreStringNode("SIGALRM"), 0);
   sh->setKeyValue("SIGALRM", new QoreBigIntNode(SIGALRM), 0);
   ns->addConstant("SIGALRM", new QoreBigIntNode(SIGALRM));
#endif
#ifdef SIGTERM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTERM), new QoreStringNode("SIGTERM"), 0);
   sh->setKeyValue("SIGTERM", new QoreBigIntNode(SIGTERM), 0);
   ns->addConstant("SIGTERM", new QoreBigIntNode(SIGTERM));
#endif
#ifdef SIGURG
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGURG), new QoreStringNode("SIGURG"), 0);
   sh->setKeyValue("SIGURG", new QoreBigIntNode(SIGURG), 0);
   ns->addConstant("SIGURG", new QoreBigIntNode(SIGURG));
#endif
#ifdef SIGSTOP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTOP), new QoreStringNode("SIGSTOP"), 0);
   sh->setKeyValue("SIGSTOP", new QoreBigIntNode(SIGSTOP), 0);
   ns->addConstant("SIGSTOP", new QoreBigIntNode(SIGSTOP));
#endif
#ifdef SIGTSTP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTSTP), new QoreStringNode("SIGTSTP"), 0);
   sh->setKeyValue("SIGTSTP", new QoreBigIntNode(SIGTSTP), 0);
   ns->addConstant("SIGTSTP", new QoreBigIntNode(SIGTSTP));
#endif
#ifdef SIGCONT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCONT), new QoreStringNode("SIGCONT"), 0);
   sh->setKeyValue("SIGCONT", new QoreBigIntNode(SIGCONT), 0);
   ns->addConstant("SIGCONT", new QoreBigIntNode(SIGCONT));
#endif
#ifdef SIGCHLD
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCHLD), new QoreStringNode("SIGCHLD"), 0);
   sh->setKeyValue("SIGCHLD", new QoreBigIntNode(SIGCHLD), 0);
   ns->addConstant("SIGCHLD", new QoreBigIntNode(SIGCHLD));
#endif
#ifdef SIGTTIN
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTIN), new QoreStringNode("SIGTTIN"), 0);
   sh->setKeyValue("SIGTTIN", new QoreBigIntNode(SIGTTIN), 0);
   ns->addConstant("SIGTTIN", new QoreBigIntNode(SIGTTIN));
#endif
#ifdef SIGTTOU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTTOU), new QoreStringNode("SIGTTOU"), 0);
   sh->setKeyValue("SIGTTOU", new QoreBigIntNode(SIGTTOU), 0);
   ns->addConstant("SIGTTOU", new QoreBigIntNode(SIGTTOU));
#endif
#ifdef SIGIO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGIO), new QoreStringNode("SIGIO"), 0);
   sh->setKeyValue("SIGIO", new QoreBigIntNode(SIGIO), 0);
   ns->addConstant("SIGIO", new QoreBigIntNode(SIGIO));
#endif
#ifdef SIGXCPU
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXCPU), new QoreStringNode("SIGXCPU"), 0);
   sh->setKeyValue("SIGXCPU", new QoreBigIntNode(SIGXCPU), 0);
   ns->addConstant("SIGXCPU", new QoreBigIntNode(SIGXCPU));
#endif
#ifdef SIGXFSZ
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXFSZ), new QoreStringNode("SIGXFSZ"), 0);
   sh->setKeyValue("SIGXFSZ", new QoreBigIntNode(SIGXFSZ), 0);
   ns->addConstant("SIGXFSZ", new QoreBigIntNode(SIGXFSZ));
#endif
#ifdef SIGVTALRM
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGVTALRM), new QoreStringNode("SIGVTALRM"), 0);
   sh->setKeyValue("SIGVTALRM", new QoreBigIntNode(SIGVTALRM), 0);
   ns->addConstant("SIGVTALRM", new QoreBigIntNode(SIGVTALRM));
#endif
#ifdef SIGPROF
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPROF), new QoreStringNode("SIGPROF"), 0);
   sh->setKeyValue("SIGPROF", new QoreBigIntNode(SIGPROF), 0);
   ns->addConstant("SIGPROF", new QoreBigIntNode(SIGPROF));
#endif
#ifdef SIGWINCH
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGWINCH), new QoreStringNode("SIGWINCH"), 0);
   sh->setKeyValue("SIGWINCH", new QoreBigIntNode(SIGWINCH), 0);
   ns->addConstant("SIGWINCH", new QoreBigIntNode(SIGWINCH));
#endif
#ifdef SIGINFO
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGINFO), new QoreStringNode("SIGINFO"), 0);
   sh->setKeyValue("SIGINFO", new QoreBigIntNode(SIGINFO), 0);
   ns->addConstant("SIGINFO", new QoreBigIntNode(SIGINFO));
#endif
#ifdef SIGUSR1
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR1), new QoreStringNode("SIGUSR1"), 0);
   sh->setKeyValue("SIGUSR1", new QoreBigIntNode(SIGUSR1), 0);
   ns->addConstant("SIGUSR1", new QoreBigIntNode(SIGUSR1));
#endif
#ifdef SIGUSR2
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGUSR2), new QoreStringNode("SIGUSR2"), 0);
   sh->setKeyValue("SIGUSR2", new QoreBigIntNode(SIGUSR2), 0);
   ns->addConstant("SIGUSR2", new QoreBigIntNode(SIGUSR2));
#endif
#ifdef SIGSTKFLT
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGSTKSZ), new QoreStringNode("SIGSTKFLT"), 0);
   sh->setKeyValue("SIGSTKFLT", new QoreBigIntNode(SIGSTKFLT), 0);
   ns->addConstant("SIGSTKFLT", new QoreBigIntNode(SIGSTKFLT));
#endif
#ifdef SIGCLD
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCHLD), new QoreStringNode("SIGCLD"), 0);
   sh->setKeyValue("SIGCLD", new QoreBigIntNode(SIGCLD), 0);
   ns->addConstant("SIGCLD", new QoreBigIntNode(SIGCLD));
#endif
#ifdef SIGPWR
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGPWR), new QoreStringNode("SIGPWR"), 0);
   sh->setKeyValue("SIGPWR", new QoreBigIntNode(SIGPWR), 0);
   ns->addConstant("SIGPWR", new QoreBigIntNode(SIGPWR));
#endif
#ifdef SIGLOST
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGLOST), new QoreStringNode("SIGLOST"), 0);
   sh->setKeyValue("SIGLOST", new QoreBigIntNode(SIGLOST), 0);
   ns->addConstant("SIGLOST", new QoreBigIntNode(SIGLOST));
#endif
#ifdef SIGWAITING
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGWAITING), new QoreStringNode("SIGWAITING"), 0);
   sh->setKeyValue("SIGWAITING", new QoreBigIntNode(SIGWAITING), 0);
   ns->addConstant("SIGWAITING", new QoreBigIntNode(SIGWAITING));
#endif
#ifdef SIGLWP
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGLWP), new QoreStringNode("SIGLWP"), 0);
   sh->setKeyValue("SIGLWP", new QoreBigIntNode(SIGLWP), 0);
   ns->addConstant("SIGLWP", new QoreBigIntNode(SIGLWP));
#endif
#ifdef SIGFREEZE
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGFREEZE), new QoreStringNode("SIGFREEZE"), 0);
   sh->setKeyValue("SIGFREEZE", new QoreBigIntNode(SIGFREEZE), 0);
   ns->addConstant("SIGFREEZE", new QoreBigIntNode(SIGFREEZE));
#endif
#ifdef SIGTHAW
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGTHAW), new QoreStringNode("SIGTHAW"), 0);
   sh->setKeyValue("SIGTHAW", new QoreBigIntNode(SIGTHAW), 0);
   ns->addConstant("SIGTHAW", new QoreBigIntNode(SIGTHAW));
#endif
#ifdef SIGCANCEL
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGCANCEL), new QoreStringNode("SIGCANCEL"), 0);
   sh->setKeyValue("SIGCANCEL", new QoreBigIntNode(SIGCANCEL), 0);
   ns->addConstant("SIGCANCEL", new QoreBigIntNode(SIGCANCEL));
#endif
#ifdef SIGXRES
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGXRES), new QoreStringNode("SIGXRES"), 0);
   sh->setKeyValue("SIGXRES", new QoreBigIntNode(SIGXRES), 0);
   ns->addConstant("SIGXRES", new QoreBigIntNode(SIGXRES));
#endif
#ifdef SIGJVM1
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGJVM1), new QoreStringNode("SIGJVM1"), 0);
   sh->setKeyValue("SIGJVM1", new QoreBigIntNode(SIGJVM1), 0);
   ns->addConstant("SIGJVM1", new QoreBigIntNode(SIGJVM1));
#endif
#ifdef SIGJVM2
   nh->setKeyValue(MAKE_STRING_FROM_SYMBOL(SIGJVM2), new QoreStringNode("SIGJVM2"), 0);
   sh->setKeyValue("SIGJVM2", new QoreBigIntNode(SIGJVM2), 0);
   ns->addConstant("SIGJVM2", new QoreBigIntNode(SIGJVM2));
#endif
   
   ns->addConstant("SignalToName", nh);
   ns->addConstant("NameToSignal", sh);
}

// sets up the root namespace
void StaticSystemNamespace::init() {
   QORE_TRACE("StaticSystemNamespace::init()");

   priv->name = "";

   qoreNS = new QoreNamespace("Qore");

   qoreNS->addInitialNamespace(get_thread_ns());

   QoreClass *TermIOS, *SSLCert, *File, *SSLPrivKey;

   // add system object types
   qoreNS->addSystemClass(initTimeZoneClass());
   qoreNS->addSystemClass(SSLCert = initSSLCertificateClass());
   qoreNS->addSystemClass(SSLPrivKey = initSSLPrivateKeyClass());
   qoreNS->addSystemClass(initSocketClass(SSLCert, SSLPrivKey));
   qoreNS->addSystemClass(initProgramClass());
   qoreNS->addSystemClass(TermIOS = initTermIOSClass());
   qoreNS->addSystemClass(File = initFileClass(TermIOS));
   qoreNS->addSystemClass(initDirClass());
   qoreNS->addSystemClass(initGetOptClass());
   qoreNS->addSystemClass(initFtpClientClass());

   // add Xml namespace
   qoreNS->addInitialNamespace(initXmlNs());

   // add HTTPClient namespace
   QoreClass *http_client_class;
   qoreNS->addSystemClass((http_client_class = initHTTPClientClass()));
   qoreNS->addSystemClass(initXmlRpcClientClass(http_client_class));
   qoreNS->addSystemClass(initJsonRpcClientClass(http_client_class));

   // add signal constants
   addSignalConstants(qoreNS);

#ifdef DEBUG_TESTS
{ // tests
   QoreClass* base = initBuiltinInheritanceTestBaseClass();
   qoreNS->addSystemClass(base);
   qoreNS->addSystemClass(initBuiltinInheritanceTestDescendant1(base));
   // hierarchy with 3 levels
   QoreClass* desc2 = initBuiltinInheritanceTestDescendant2(base);
   qoreNS->addSystemClass(desc2);
   QoreClass* desc3 = initBuiltinInheritanceTestDescendant3(desc2);
   qoreNS->addSystemClass(desc3);
// BUGBUG : this fails. When desc2 is placed in the next line all is OK
   QoreClass* desc4 = initBuiltinInheritanceTestDescendant4(desc3);
   qoreNS->addSystemClass(desc4);

   QoreClass* base2 = initBuiltinInheritanceTestBase2Class();
   qoreNS->addSystemClass(base2);
// BUGBUG - the function actually fails to deal with two base classes, see the 
// code in tests/builtin_inheritance_tests.cpp
   QoreClass* desc_multi = initBuiltinInheritanceTestDescendantMulti(base2, base);
   qoreNS->addSystemClass(desc_multi);
}
#endif

   // add ssl socket constants
   addSSLConstants(qoreNS);

   // add boolean constants for true and false
   qoreNS->addConstant("True",          boolean_true());
   qoreNS->addConstant("False",         boolean_false());

   // add File object constants for stdin (0), stdout (1), stderr (2)
   qoreNS->addConstant("stdin",         File->execSystemConstructor(0));
   qoreNS->addConstant("stdout",        File->execSystemConstructor(1));
   qoreNS->addConstant("stderr",        File->execSystemConstructor(2));

   // add constants for exception types
   qoreNS->addConstant("ET_System",     new QoreStringNode("System"));
   qoreNS->addConstant("ET_User",       new QoreStringNode("User"));

   // create constants for call types
   qoreNS->addConstant("CT_User",       new QoreBigIntNode(CT_USER));
   qoreNS->addConstant("CT_Builtin",    new QoreBigIntNode(CT_BUILTIN));
   qoreNS->addConstant("CT_NewThread",  new QoreBigIntNode(CT_NEWTHREAD));
   qoreNS->addConstant("CT_Rethrow",    new QoreBigIntNode(CT_RETHROW));

   // create constants for version and platform information
   qoreNS->addConstant("VersionString", new QoreStringNode(qore_version_string));
   qoreNS->addConstant("VersionMajor",  new QoreBigIntNode(qore_version_major));
   qoreNS->addConstant("VersionMinor",  new QoreBigIntNode(qore_version_minor));
   qoreNS->addConstant("VersionSub",    new QoreBigIntNode(qore_version_sub));
   qoreNS->addConstant("Build",         new QoreBigIntNode(qore_build_number));
   qoreNS->addConstant("PlatformCPU",   new QoreStringNode(TARGET_ARCH));
   qoreNS->addConstant("PlatformOS",    new QoreStringNode(TARGET_OS));

   // constants for build info
   qoreNS->addConstant("BuildHost",     new QoreStringNode(qore_build_host));
   qoreNS->addConstant("Compiler",      new QoreStringNode(qore_cplusplus_compiler));
   qoreNS->addConstant("CFLAGS",        new QoreStringNode(qore_cflags));
   qoreNS->addConstant("LDFLAGS",       new QoreStringNode(qore_ldflags));

   // add constants for regex() function options
   qoreNS->addConstant("RE_Caseless",   new QoreBigIntNode(PCRE_CASELESS));
   qoreNS->addConstant("RE_DotAll",     new QoreBigIntNode(PCRE_DOTALL));
   qoreNS->addConstant("RE_Extended",   new QoreBigIntNode(PCRE_EXTENDED));
   qoreNS->addConstant("RE_MultiLine",  new QoreBigIntNode(PCRE_MULTILINE));
   // note that the following constant is > 32-bits so it can't collide with PCRE constants
   qoreNS->addConstant("RE_Global",     new QoreBigIntNode(QRE_GLOBAL));

   // network constants
   qoreNS->addConstant("AF_INET",       new QoreBigIntNode(AF_INET));
   qoreNS->addConstant("AF_INET6",      new QoreBigIntNode(AF_INET6));
   qoreNS->addConstant("AF_UNIX",       new QoreBigIntNode(AF_UNIX));
#ifdef AF_LOCAL
   qoreNS->addConstant("AF_LOCAL",      new QoreBigIntNode(AF_LOCAL)); // POSIX synonym for AF_UNIX
#else
   qoreNS->addConstant("AF_LOCAL",      new QoreBigIntNode(AF_UNIX));
#endif

   // zlib constants
   qoreNS->addConstant("Z_DEFAULT_COMPRESSION", new QoreBigIntNode(Z_DEFAULT_COMPRESSION));

   // math constants
   qoreNS->addConstant("M_PI",          new QoreFloatNode(3.14159265358979323846));

   // system constants
#ifdef WORDS_BIGENDIAN
   qoreNS->addConstant("MACHINE_MSB",   &True);
#else
   qoreNS->addConstant("MACHINE_MSB",   &False);
#endif

   // warning constants
   qoreNS->addConstant("WARN_NONE",                      new QoreBigIntNode(QP_WARN_NONE));
   qoreNS->addConstant("WARN_WARNING_MASK_UNCHANGED",    new QoreBigIntNode(QP_WARN_WARNING_MASK_UNCHANGED));
   qoreNS->addConstant("WARN_DUPLICATE_LOCAL_VARS",      new QoreBigIntNode(QP_WARN_DUPLICATE_LOCAL_VARS));
   qoreNS->addConstant("WARN_UNKNOWN_WARNING",           new QoreBigIntNode(QP_WARN_UNKNOWN_WARNING));
   qoreNS->addConstant("WARN_UNDECLARED_VAR",            new QoreBigIntNode(QP_WARN_UNDECLARED_VAR));
   qoreNS->addConstant("WARN_DUPLICATE_GLOBAL_VARS",     new QoreBigIntNode(QP_WARN_DUPLICATE_GLOBAL_VARS));
   qoreNS->addConstant("WARN_UNREACHABLE_CODE",          new QoreBigIntNode(QP_WARN_UNREACHABLE_CODE));
   qoreNS->addConstant("WARN_NONEXISTENT_METHOD_CALL",   new QoreBigIntNode(QP_WARN_NONEXISTENT_METHOD_CALL));
   qoreNS->addConstant("WARN_INVALID_OPERATION",         new QoreBigIntNode(QP_WARN_INVALID_OPERATION));
   qoreNS->addConstant("WARN_CALL_WITH_TYPE_ERRORS",     new QoreBigIntNode(QP_WARN_CALL_WITH_TYPE_ERRORS));
   qoreNS->addConstant("WARN_RETURN_VALUE_IGNORED",      new QoreBigIntNode(QP_WARN_RETURN_VALUE_IGNORED));
   qoreNS->addConstant("WARN_DEPRECATED",                new QoreBigIntNode(QP_WARN_DEPRECATED));
   qoreNS->addConstant("WARN_EXCESS_ARGS",               new QoreBigIntNode(QP_WARN_EXCESS_ARGS));
   qoreNS->addConstant("WARN_DUPLICATE_HASH_KEY",        new QoreBigIntNode(QP_WARN_DUPLICATE_HASH_KEY));
   qoreNS->addConstant("WARN_UNREFERENCED_VARIABLE",     new QoreBigIntNode(QP_WARN_UNREFERENCED_VARIABLE));
   qoreNS->addConstant("WARN_ALL",                       new QoreBigIntNode(QP_WARN_ALL));

   // event constants
   QoreHashNode *qesm = new QoreHashNode;
   qesm->setKeyValue("1", new QoreStringNode("SOCKET"), 0);
   qesm->setKeyValue("2", new QoreStringNode("HTTPCLIENT"), 0);
   qesm->setKeyValue("3", new QoreStringNode("FTPCLIENT"), 0);
   qesm->setKeyValue("4", new QoreStringNode("FILE"), 0);
   qoreNS->addConstant("EVENT_SOURCE_MAP", qesm);

   qoreNS->addConstant("SOURCE_SOCKET", new QoreBigIntNode(QORE_SOURCE_SOCKET));
   qoreNS->addConstant("SOURCE_HTTPCLIENT", new QoreBigIntNode(QORE_SOURCE_HTTPCLIENT));
   qoreNS->addConstant("SOURCE_FTPCLIENT", new QoreBigIntNode(QORE_SOURCE_FTPCLIENT));
   qoreNS->addConstant("SOURCE_FILE", new QoreBigIntNode(QORE_SOURCE_FILE));

   QoreHashNode *qsam = new QoreHashNode;
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_PACKET_READ), new QoreStringNode("PACKET_READ"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_PACKET_SENT), new QoreStringNode("PACKET_SENT"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_CONTENT_LENGTH), new QoreStringNode("HTTP_CONTENT_LENGTH"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_CHUNKED_START), new QoreStringNode("HTTP_CHUNKED_START"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_CHUNKED_END), new QoreStringNode("HTTP_CHUNKED_END"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_REDIRECT), new QoreStringNode("HTTP_REDIRECT"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_CHANNEL_CLOSED), new QoreStringNode("CHANNEL_CLOSED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_DELETED), new QoreStringNode("DELETED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_FTP_SEND_MESSAGE), new QoreStringNode("FTP_SEND_MESSAGE"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_FTP_MESSAGE_RECEIVED), new QoreStringNode("FTP_MESSAGE_RECEIVED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HOSTNAME_LOOKUP), new QoreStringNode("HOSTNAME_LOOKUP"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HOSTNAME_RESOLVED), new QoreStringNode("HOSTNAME_RESOLVED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_SEND_MESSAGE), new QoreStringNode("HTTP_SEND_MESSAGE"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_MESSAGE_RECEIVED), new QoreStringNode("HTTP_MESSAGE_RECEIVED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_FOOTERS_RECEIVED), new QoreStringNode("HTTP_FOOTERS_RECEIVED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED), new QoreStringNode("HTTP_CHUNKED_DATA_RECEIVED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_HTTP_CHUNK_SIZE), new QoreStringNode("HTTP_CHUNK_SIZE"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_CONNECTING), new QoreStringNode("CONNECTING"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_CONNECTED), new QoreStringNode("CONNECTED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_START_SSL), new QoreStringNode("START_SSL"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_SSL_ESTABLISHED), new QoreStringNode("SSL_ESTABLISHED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_OPEN_FILE), new QoreStringNode("OPEN_FILE"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_FILE_OPENED), new QoreStringNode("FILE_OPENED"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_DATA_READ), new QoreStringNode("DATA_READ"), 0);
   qsam->setKeyValue(MAKE_STRING_FROM_SYMBOL(QORE_EVENT_DATA_WRITTEN), new QoreStringNode("DATA_WRITTEN"), 0);
   qoreNS->addConstant("EVENT_MAP", qsam);

   qoreNS->addConstant("EVENT_PACKET_READ", new QoreBigIntNode(QORE_EVENT_PACKET_READ));
   qoreNS->addConstant("EVENT_PACKET_SENT", new QoreBigIntNode(QORE_EVENT_PACKET_SENT));
   qoreNS->addConstant("EVENT_HTTP_CONTENT_LENGTH", new QoreBigIntNode(QORE_EVENT_HTTP_CONTENT_LENGTH));
   qoreNS->addConstant("EVENT_HTTP_CHUNKED_START", new QoreBigIntNode(QORE_EVENT_HTTP_CHUNKED_START));
   qoreNS->addConstant("EVENT_HTTP_CHUNKED_END", new QoreBigIntNode(QORE_EVENT_HTTP_CHUNKED_END));
   qoreNS->addConstant("EVENT_HTTP_REDIRECT", new QoreBigIntNode(QORE_EVENT_HTTP_REDIRECT));
   qoreNS->addConstant("EVENT_CHANNEL_CLOSED", new QoreBigIntNode(QORE_EVENT_CHANNEL_CLOSED));
   qoreNS->addConstant("EVENT_DELETED", new QoreBigIntNode(QORE_EVENT_DELETED));
   qoreNS->addConstant("EVENT_FTP_SEND_MESSAGE", new QoreBigIntNode(QORE_EVENT_FTP_SEND_MESSAGE));
   qoreNS->addConstant("EVENT_FTP_MESSAGE_RECEIVED", new QoreBigIntNode(QORE_EVENT_FTP_MESSAGE_RECEIVED));
   qoreNS->addConstant("EVENT_HOSTNAME_LOOKUP", new QoreBigIntNode(QORE_EVENT_HOSTNAME_LOOKUP));
   qoreNS->addConstant("EVENT_HOSTNAME_RESOLVED", new QoreBigIntNode(QORE_EVENT_HOSTNAME_RESOLVED));
   qoreNS->addConstant("EVENT_HTTP_SEND_MESSAGE", new QoreBigIntNode(QORE_EVENT_HTTP_SEND_MESSAGE));
   qoreNS->addConstant("EVENT_HTTP_MESSAGE_RECEIVED", new QoreBigIntNode(QORE_EVENT_HTTP_MESSAGE_RECEIVED));
   qoreNS->addConstant("EVENT_HTTP_FOOTERS_RECEIVED", new QoreBigIntNode(QORE_EVENT_HTTP_FOOTERS_RECEIVED));
   qoreNS->addConstant("EVENT_HTTP_CHUNKED_DATA_RECEIVED", new QoreBigIntNode(QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED));
   qoreNS->addConstant("EVENT_HTTP_CHUNK_SIZE", new QoreBigIntNode(QORE_EVENT_HTTP_CHUNK_SIZE));
   qoreNS->addConstant("EVENT_CONNECTING", new QoreBigIntNode(QORE_EVENT_CONNECTING));
   qoreNS->addConstant("EVENT_CONNECTED", new QoreBigIntNode(QORE_EVENT_CONNECTED));
   qoreNS->addConstant("EVENT_START_SSL", new QoreBigIntNode(QORE_EVENT_START_SSL));
   qoreNS->addConstant("EVENT_SSL_ESTABLISHED", new QoreBigIntNode(QORE_EVENT_SSL_ESTABLISHED));
   qoreNS->addConstant("EVENT_OPEN_FILE", new QoreBigIntNode(QORE_EVENT_OPEN_FILE));
   qoreNS->addConstant("EVENT_FILE_OPENED", new QoreBigIntNode(QORE_EVENT_FILE_OPENED));
   qoreNS->addConstant("EVENT_DATA_READ", new QoreBigIntNode(QORE_EVENT_DATA_READ));
   qoreNS->addConstant("EVENT_DATA_WRITTEN", new QoreBigIntNode(QORE_EVENT_DATA_WRITTEN));
   //qoreNS->addConstant("EVENT_", new QoreBigIntNode(QORE_EVENT_));

   // setup terminal mode constants
   // input modes
   qoreNS->addConstant("IGNBRK", new QoreBigIntNode(IGNBRK));
   qoreNS->addConstant("BRKINT", new QoreBigIntNode(BRKINT));
   qoreNS->addConstant("IGNPAR", new QoreBigIntNode(IGNPAR));
   qoreNS->addConstant("PARMRK", new QoreBigIntNode(PARMRK));
   qoreNS->addConstant("INPCK", new QoreBigIntNode(INPCK));
   qoreNS->addConstant("ISTRIP", new QoreBigIntNode(ISTRIP));
   qoreNS->addConstant("INLCR", new QoreBigIntNode(INLCR));
   qoreNS->addConstant("IGNCR", new QoreBigIntNode(IGNCR));
   qoreNS->addConstant("ICRNL", new QoreBigIntNode(ICRNL));
   qoreNS->addConstant("IXON", new QoreBigIntNode(IXON));
   qoreNS->addConstant("IXOFF", new QoreBigIntNode(IXOFF));
   qoreNS->addConstant("IXANY", new QoreBigIntNode(IXANY));
#ifdef IMAXBEL
   qoreNS->addConstant("IMAXBEL", new QoreBigIntNode(IMAXBEL));
#endif
#ifdef IUCLC
   qoreNS->addConstant("IUCLC", new QoreBigIntNode(IUCLC));
#endif

   // output modes
   qoreNS->addConstant("OPOST", new QoreBigIntNode(OPOST));
   qoreNS->addConstant("ONLCR", new QoreBigIntNode(ONLCR));
#ifdef OXTABS
   qoreNS->addConstant("OXTABS", new QoreBigIntNode(OXTABS));
#endif
#ifdef ONOEOT
   qoreNS->addConstant("ONOEOT", new QoreBigIntNode(ONOEOT));
#endif
   qoreNS->addConstant("OCRNL", new QoreBigIntNode(OCRNL));
#ifdef OLCUC
   qoreNS->addConstant("OLCUC", new QoreBigIntNode(OLCUC));
#endif
   qoreNS->addConstant("ONOCR", new QoreBigIntNode(ONOCR));
   qoreNS->addConstant("ONLRET", new QoreBigIntNode(ONLRET));

   // control modes
   qoreNS->addConstant("CSIZE", new QoreBigIntNode(CSIZE));
   qoreNS->addConstant("CS5", new QoreBigIntNode(CS5));
   qoreNS->addConstant("CS6", new QoreBigIntNode(CS6));
   qoreNS->addConstant("CS7", new QoreBigIntNode(CS7));
   qoreNS->addConstant("CS8", new QoreBigIntNode(CS8));
   qoreNS->addConstant("CSTOPB", new QoreBigIntNode(CSTOPB));
   qoreNS->addConstant("CREAD", new QoreBigIntNode(CREAD));
   qoreNS->addConstant("PARENB", new QoreBigIntNode(PARENB));
   qoreNS->addConstant("PARODD", new QoreBigIntNode(PARODD));
   qoreNS->addConstant("HUPCL", new QoreBigIntNode(HUPCL));
   qoreNS->addConstant("CLOCAL", new QoreBigIntNode(CLOCAL));
#ifdef CCTS_OFLOW
   qoreNS->addConstant("CCTS_OFLOW", new QoreBigIntNode(CCTS_OFLOW));
#endif
#ifdef CRTSCTS
   qoreNS->addConstant("CRTSCTS", new QoreBigIntNode(CRTSCTS));
#endif
#ifdef CRTS_IFLOW
   qoreNS->addConstant("CRTS_IFLOW", new QoreBigIntNode(CRTS_IFLOW));
#endif
#ifdef MDMBUF
   qoreNS->addConstant("MDMBUF", new QoreBigIntNode(MDMBUF));
#endif

   // local modes
#ifdef ECHOKE
   qoreNS->addConstant("ECHOKE", new QoreBigIntNode(ECHOKE));
#endif
   qoreNS->addConstant("ECHOE", new QoreBigIntNode(ECHOE));
   qoreNS->addConstant("ECHO", new QoreBigIntNode(ECHO));
   qoreNS->addConstant("ECHONL", new QoreBigIntNode(ECHONL));
#ifdef ECHOPRT
   qoreNS->addConstant("ECHOPRT", new QoreBigIntNode(ECHOPRT));
#endif
#ifdef ECHOCTL
   qoreNS->addConstant("ECHOCTL", new QoreBigIntNode(ECHOCTL));
#endif
   qoreNS->addConstant("ISIG", new QoreBigIntNode(ISIG));
   qoreNS->addConstant("ICANON", new QoreBigIntNode(ICANON));
#ifdef ALTWERASE
   qoreNS->addConstant("ALTWERASE", new QoreBigIntNode(ALTWERASE));
#endif
   qoreNS->addConstant("IEXTEN", new QoreBigIntNode(IEXTEN));
#ifdef EXTPROC
   qoreNS->addConstant("EXTPROC", new QoreBigIntNode(EXTPROC));
#endif
   qoreNS->addConstant("TOSTOP", new QoreBigIntNode(TOSTOP));
#ifdef FLUSHO
   qoreNS->addConstant("FLUSHO", new QoreBigIntNode(FLUSHO));
#endif
#ifdef NOKERNINFO
   qoreNS->addConstant("NOKERNINFO", new QoreBigIntNode(NOKERNINFO));
#endif
#ifdef PENDIN
   qoreNS->addConstant("PENDIN", new QoreBigIntNode(PENDIN));
#endif
   qoreNS->addConstant("NOFLSH", new QoreBigIntNode(NOFLSH));
   
   // control characters
   qoreNS->addConstant("VEOF", new QoreBigIntNode(VEOF));
   qoreNS->addConstant("VEOL", new QoreBigIntNode(VEOL));
#ifdef VEOL2
   qoreNS->addConstant("VEOL2", new QoreBigIntNode(VEOL2));
#endif
   qoreNS->addConstant("VERASE", new QoreBigIntNode(VERASE));
#ifdef VWERASE
   qoreNS->addConstant("VWERASE", new QoreBigIntNode(VWERASE));
#endif
   qoreNS->addConstant("VKILL", new QoreBigIntNode(VKILL));
#ifdef VREPRINT
   qoreNS->addConstant("VREPRINT", new QoreBigIntNode(VREPRINT));
#endif
   qoreNS->addConstant("VINTR", new QoreBigIntNode(VINTR));
   qoreNS->addConstant("VQUIT", new QoreBigIntNode(VQUIT));
   qoreNS->addConstant("VSUSP", new QoreBigIntNode(VSUSP));
#ifdef VDSUSP
   qoreNS->addConstant("VDSUSP", new QoreBigIntNode(VDSUSP));
#endif
   qoreNS->addConstant("VSTART", new QoreBigIntNode(VSTART));
   qoreNS->addConstant("VSTOP", new QoreBigIntNode(VSTOP));
#ifdef VLNEXT
   qoreNS->addConstant("VLNEXT", new QoreBigIntNode(VLNEXT));
#endif
#ifdef VDISCARD
   qoreNS->addConstant("VDISCARD", new QoreBigIntNode(VDISCARD));
#endif
   qoreNS->addConstant("VMIN", new QoreBigIntNode(VMIN));
   qoreNS->addConstant("VTIME", new QoreBigIntNode(VTIME));
#ifdef VSTATUS
   qoreNS->addConstant("VSTATUS", new QoreBigIntNode(VSTATUS));
#endif

   // terminal setting actions
   qoreNS->addConstant("TCSANOW", new QoreBigIntNode(TCSANOW));
   qoreNS->addConstant("TCSADRAIN", new QoreBigIntNode(TCSADRAIN));
   qoreNS->addConstant("TCSAFLUSH", new QoreBigIntNode(TCSAFLUSH));
#ifdef TCSASOFT
   qoreNS->addConstant("TCSASOFT", new QoreBigIntNode(TCSASOFT));
#endif

   // set up Option namespace for Qore options
   QoreNamespace *option = new QoreNamespace("Option");

   // add constant for features found with configure
#ifdef HAVE_ATOMIC_MACROS
   option->addConstant("HAVE_ATOMIC_OPERATIONS", &True);
#else
   option->addConstant("HAVE_ATOMIC_OPERATIONS", &False);
#endif

#ifdef HAVE_CHECK_STACK_POS
   option->addConstant("HAVE_STACK_GUARD", &True);
#else
   option->addConstant("HAVE_STACK_GUARD", &False);
#endif

#ifdef HAVE_SIGNAL_HANDLING
   option->addConstant("HAVE_SIGNAL_HANDLING", &True);
#else
   option->addConstant("HAVE_SIGNAL_HANDLING", &False);
#endif

#ifdef DEBUG
   option->addConstant("HAVE_LIBRARY_DEBUGGING", &True);
#else
   option->addConstant("HAVE_LIBRARY_DEBUGGING", &False);
#endif

#ifdef QORE_RUNTIME_THREAD_STACK_TRACE
   option->addConstant("HAVE_RUNTIME_THREAD_STACK_TRACE", &True);
#else
   option->addConstant("HAVE_RUNTIME_THREAD_STACK_TRACE", &False);
#endif

#ifdef HAVE_ROUND
   option->addConstant("HAVE_ROUND",    &True);
#else
   option->addConstant("HAVE_ROUND",    &False);
#endif

   // HAVE_TIMEGM is always true now; we don't use the system function anyway anymore
   option->addConstant("HAVE_TIMEGM",   &True);

#ifdef HAVE_SETEUID
   option->addConstant("HAVE_SETEUID",  &True);
#else
   option->addConstant("HAVE_SETEUID",  &False);
#endif

#ifdef HAVE_SETEGID
   option->addConstant("HAVE_SETEGID",  &True);
#else
   option->addConstant("HAVE_SETEGID",  &False);
#endif

#ifdef HAVE_XMLTEXTREADERSETSCHEMA
   option->addConstant("HAVE_PARSEXMLWITHSCHEMA",  &True);
#else
   option->addConstant("HAVE_PARSEXMLWITHSCHEMA",  &False);
#endif

#ifdef HAVE_XMLTEXTREADERRELAXNGSETSCHEMA
   option->addConstant("HAVE_PARSEXMLWITHRELAXNG",  &True);
#else
   option->addConstant("HAVE_PARSEXMLWITHRELAXNG",  &False);
#endif

#if !defined(OPENSSL_NO_SHA256) && defined(HAVE_OPENSSL_SHA512)
   option->addConstant("HAVE_SHA224",  &True);
   option->addConstant("HAVE_SHA256",  &True);
#else
   option->addConstant("HAVE_SHA224",  &False);
   option->addConstant("HAVE_SHA256",  &False);
#endif

#if !defined(OPENSSL_NO_SHA512) && defined(HAVE_OPENSSL_SHA512)
   option->addConstant("HAVE_SHA384",  &True);
   option->addConstant("HAVE_SHA512",  &True);
#else
   option->addConstant("HAVE_SHA384",  &False);
   option->addConstant("HAVE_SHA512",  &False);
#endif

#ifndef OPENSSL_NO_MDC2
   option->addConstant("HAVE_MDC2",  &True);
#else
   option->addConstant("HAVE_MDC2",  &False);
#endif

#ifndef OPENSSL_NO_MD2
   option->addConstant("HAVE_MD2",  &True);
#else
   option->addConstant("HAVE_MD2",  &False);
#endif

#ifndef OPENSSL_NO_RC5
   option->addConstant("HAVE_RC5",  &True);
#else
   option->addConstant("HAVE_RC5",  &False);
#endif

   qoreNS->addInitialNamespace(option);

   // create Qore::SQL namespace
   qoreNS->addInitialNamespace(getSQLNamespace());

   // create get Qore::Err namespace with ERRNO constants
   qoreNS->addInitialNamespace(get_errno_ns());

   // create Qore::Type namespace with type constants
   qoreNS->addInitialNamespace(get_type_ns());

   // add file constants
   addFileConstants(qoreNS);

   // add parse option constants to Qore namespace
   addProgramConstants(qoreNS);

   addQoreNamespace(qoreNS);

   // add all changes in loaded modules
   ANSL.init(this, qoreNS);
}

RootQoreNamespace::RootQoreNamespace(QoreNamespace *&QoreNS, int64 po) 
   : QoreNamespace("", 
		   staticSystemNamespace.priv->classList->copy(po),
		   new ConstantList(*staticSystemNamespace.priv->constant),
		   staticSystemNamespace.priv->nsl->copy(po, this)) {
   qoreNS = QoreNS = priv->nsl->find("Qore");
   assert(QoreNS);
}

// private constructor
RootQoreNamespace::RootQoreNamespace(QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl) : QoreNamespace("", ocl, cl, nnsl) {
   qoreNS = priv->nsl->find("Qore");
   // resolve all copied classes to the new classes
   priv->classList->resolveCopy();
   priv->nsl->resolveCopy();
}

// private constructor; used by StaticSystemNamespace
RootQoreNamespace::RootQoreNamespace() : QoreNamespace("") {
}

RootQoreNamespace::~RootQoreNamespace() {
   // first delete all contained classes and other objects
   purge();
}

QoreNamespace *RootQoreNamespace::rootGetQoreNamespace() const {
   return qoreNS;
}

RootQoreNamespace *RootQoreNamespace::copy(int64 po) const {
   //printd(5, "RootQoreNamespace::copy() this=%p po=%lld\n", this, po);
   return new RootQoreNamespace(priv->classList->copy(po), new ConstantList(*priv->constant), priv->nsl->copy(po, this));
}

#ifdef DEBUG_TESTS
// moved down to allow to test internal classes
#  include "tests/Namespace_tests.cpp"
#endif

AbstractQoreNode *qore_ns_private::parseCheckScopedReference(const NamedScope &nsc, unsigned &matched, const QoreTypeInfo *&typeInfo) const {
   const QoreNamespace *pns = ns;

   // follow the namespaces
   unsigned last = nsc.size() - 1;
   for (unsigned i = 0; i < last; ++i) {
      QoreNamespace *nns;
      if (!i)
         nns = (name == nsc.strlist[0]) ? ns : 0;
      else
         nns = pns->priv->parseFindLocalNamespace(nsc.strlist[i].c_str());

      if (!nns) {
         // if we have matched all namespaces except the last one, check if it's a class
         // and try to resolve a class constant or static class variable
         if (i == (last - 1)) {
            const char *cname = nsc.strlist[last - 1].c_str();
            QoreClass *qc = pns->parseFindLocalClass(cname);
            //printd(0, "qore_ns_private::parseCheckScopedReference() this=%p '%s' nsc=%s checking for class '%s' qc=%p\n", this, name.c_str(), nsc.ostr, cname, qc);
            if (qc) {
               AbstractQoreNode *rv = qore_class_private::parseFindConstantValue(qc, nsc.getIdentifier(), typeInfo, true);
               if (rv)
                  return rv->refSelf();
               const QoreClass *aqc;
               QoreVarInfo *vi = qore_class_private::parseFindStaticVar(qc, nsc.getIdentifier(), aqc, typeInfo, true);
               if (vi) {
                  typeInfo = vi->getTypeInfo();
                  return new StaticClassVarRefNode(nsc.getIdentifier(), *qc, *vi);
               }
            }
         }
         return 0;
      }

      pns = nns;
      if (i >= matched)
         matched = i + 1;      
   }

   // matched all namespaces, now try to find a constant
   AbstractQoreNode *rv = pns->priv->parseFindLocalConstantValue(nsc.getIdentifier(), typeInfo);
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode *qore_ns_private::parseResolveScopedReference(const NamedScope &nsc, unsigned &matched, const QoreTypeInfo *&typeInfo) const {
   //printd(0, "qore_ns_private::parseResolveScopedReference() this=%p '%s' nsc=%s m=%d\n", this, name.c_str(), nsc.ostr, matched);
   assert(nsc.size() > 1);

   AbstractQoreNode *rv = parseCheckScopedReference(nsc, matched, typeInfo);
   if (rv)
      return rv;

   rv = nsl->parseResolveScopedReference(nsc, matched, typeInfo);
   return rv ? rv : pendNSL->parseResolveScopedReference(nsc, matched, typeInfo);
}

AbstractQoreNode *qore_ns_private::parseFindLocalConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv = constant->find(cname, typeInfo);
   return rv ? rv : pendConstant->find(cname, typeInfo);
}

QoreNamespace *qore_ns_private::parseFindLocalNamespace(const char *nname) const {
   QoreNamespace *rv = nsl->find(nname);
   return rv ? rv : pendNSL->find(nname);
}

void StaticSystemNamespace::purge() {                                                                                                                                                          
   if (priv->nsl) {                                                                                                                                                              
      ExceptionSink xsink;                                                                                                                                                       
      deleteData(&xsink);                                                                                                                                                        
      QoreNamespace::purge();                                                                                                                                                    
   }                                                                                                                                                                             
}                                                                                                                                                                                
