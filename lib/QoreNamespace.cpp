/*
  Namespace.cpp

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
#include <qore/intern/QC_XmlNode.h>

#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <assert.h>
#include <sys/socket.h> // for AF_INET and AF_INET6

#ifdef DEBUG_TESTS
// the #include "test/Namespace_tests.cpp" is on the bottom
#  include "tests/builtin_inheritance_tests.cpp"
#endif

#define MAX_RECURSION_DEPTH 20

StaticSystemNamespace staticSystemNamespace;

AutoNamespaceList ANSL;

struct qore_ns_private {
   std::string name;

   QoreClassList      *classList;
   ConstantList       *constant;
   QoreNamespaceList  *nsl;

   // pending lists
   // FIXME: can be normal members
   QoreClassList      *pendClassList;
   ConstantList       *pendConstant;
   QoreNamespaceList  *pendNSL;

   QoreNamespace      *next;

   DLLLOCAL qore_ns_private(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl) :
      name(n), 
      classList(ocl), constant(cl), nsl(nnsl), 
      pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList), next(0)
   {
      assert(classList);
      assert(constant);
      assert(nsl);
   }
   DLLLOCAL qore_ns_private(const char *n) :
      name(n),
      classList(new QoreClassList), constant(new ConstantList), nsl(new QoreNamespaceList), 
      pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList), next(0)
   {
   }

   DLLLOCAL ~qore_ns_private() {
      printd(5, "QoreNamespace::~QoreNamespace() this=%08p '%s'\n", this, name.c_str());

      purge();
   }

   DLLLOCAL void purge() {
      delete constant;
      constant = 0;

      if (nsl)
	 nsl->deleteAllConstants();

      delete classList;
      classList = 0;

      delete nsl;
      nsl = 0;
	 
      delete pendConstant;
      pendConstant = 0;

      delete pendClassList;
      pendClassList = 0;

      delete pendNSL;
      pendNSL = 0;
   }
};

QoreNamespace::QoreNamespace(const char *n) : priv(new qore_ns_private(n)) {
}

QoreNamespace::QoreNamespace() : priv(new qore_ns_private("")) {
}

QoreNamespace::QoreNamespace(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl) : priv(new qore_ns_private(n, ocl, cl, nnsl)) {
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

// private function
QoreNamespace *QoreNamespace::resolveNameScope(NamedScope *nscope) const {
   const QoreNamespace *sns = this;

   // find namespace
   for (int i = 0; i < (nscope->elements - 1); i++)
      if (!(sns = sns->findNamespace(nscope->strlist[i])))
      {
	 parse_error("namespace '%s' cannot be resolved while evaluating '%s' in constant declaration",
		     nscope->strlist[i], nscope->ostr);
	 return 0;
      }
   return (QoreNamespace *)sns;
}

// private function
AbstractQoreNode *QoreNamespace::getConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv = priv->constant->find(cname, typeInfo);
   if (!rv)
      rv = priv->pendConstant->find(cname, typeInfo);

   return rv ? rv : 0;
}

// only called while parsing before addition to namespace tree, no locking needed
void QoreNamespace::addConstant(NamedScope *nscope, AbstractQoreNode *value) {
   QoreNamespace *sns = resolveNameScope(nscope);
   if (!sns)
      value->deref(0);
   else {
      const char *cname = nscope->strlist[nscope->elements - 1];
      if (sns->priv->constant->inList(cname)) {
	 parse_error("constant '%s' has already been defined", cname);
	 value->deref(0);
      }
      else 
	 sns->priv->pendConstant->add(cname, value);
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
void QoreNamespace::addClass(NamedScope *n, QoreClass *oc) {
   //printd(5, "QoreNamespace::addClass() adding ns=%s (%s, %08p)\n", n->ostr, oc->getName(), oc);
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
   priv->nsl->add(ns);
}

void QoreNamespace::parseAddNamespace(QoreNamespace *ns) {
   // raise an exception if namespace collides with an object name
   if (priv->classList->find(ns->priv->name.c_str())) {
      parse_error("namespace name '%s' collides with previously-defined class '%s'", ns->priv->name.c_str(), ns->priv->name.c_str());
      delete ns;
      return;
   }
   if (priv->pendClassList->find(ns->priv->name.c_str())) {
      parse_error("namespace name '%s' collides with pending class '%s'", ns->priv->name.c_str(), ns->priv->name.c_str());
      delete ns;
      return;
   }
   priv->pendNSL->add(ns);
}

void QoreNamespace::parseInitConstants() {
   printd(5, "QoreNamespace::parseInitConstants() %s\n", priv->name.c_str());
   // do 2nd stage parse initialization on pending constants
   priv->pendConstant->parseInit();

   priv->pendNSL->parseInitConstants();
}

void QoreNamespace::parseInit() {
   printd(5, "QoreNamespace::parseInit() this=%08p\n", this);

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
   printd(5, "QoreNamespace::parseRollback() %s %08p\n", priv->name.c_str(), this);

   // delete pending constant list
   priv->pendConstant->reset();

   // delete pending changes to committed classes
   priv->classList->parseRollback();

   // delete pending classes
   priv->pendClassList->reset();

   // delete pending namespaces
   priv->pendNSL->reset();

   // do for all subnamespaces
   priv->nsl->parseRollback();
}

QoreNamespaceList::QoreNamespaceList() {
   head = tail = 0;
}

void QoreNamespaceList::deleteAll() {
   while (head) {
      tail = head->priv->next;
      delete head;
      head = tail;
   }
}

QoreNamespaceList::~QoreNamespaceList() {
   deleteAll();
}

void QoreNamespaceList::assimilate(QoreNamespaceList *n) {
   // assimilate target list
   if (tail)
      tail->priv->next = n->head;
   else
      head = n->head;
   if (n->tail)
      tail = n->tail;
   
   // "zero" target list
   n->head = n->tail = 0;
}

void QoreNamespaceList::reset()
{
   deleteAll();
   head = tail = 0;
}

void QoreNamespaceList::add(QoreNamespace *ns) {
   // if namespace is already registered, then assimilate
   QoreNamespace *ons;
   if ((ons = find(ns->priv->name.c_str()))) {
      ons->assimilate(ns);
      return;
   }
   // otherwise append to list
   if (tail)
      tail->priv->next = ns;
   else
      head = ns;
   tail = ns;
}

QoreNamespace *QoreNamespaceList::find(const char *name) {
   QORE_TRACE("QoreNamespaceList::find()");
   printd(5, "QoreNamespaceList::find(%s)\n", name);

   QoreNamespace *w = head;

   while (w) {
      if (name == w->priv->name)
	 break;
      w = w->priv->next;
   }

   printd(5, "QoreNamespaceList::find(%s) returning %08p\n", name, w);

   return w;
}

QoreNamespace *QoreNamespace::copy(int po) const {
   //printd(5, "QoreNamespace::copy() (deprecated) this=%p po=%d %s\n", this, po, priv->name.c_str());
   return new QoreNamespace(priv->name.c_str(), priv->classList->copy(po), priv->constant->copy(), priv->nsl->copy(po));
}

QoreNamespace *QoreNamespace::copy(int64 po) const {
   //printd(5, "QoreNamespace::copy() this=%p po=%lld %s\n", this, po, priv->name.c_str());
   return new QoreNamespace(priv->name.c_str(), priv->classList->copy(po), priv->constant->copy(), priv->nsl->copy(po));
}

QoreNamespaceList *QoreNamespaceList::copy(int64 po) {
   //printd(5, "QoreNamespaceList::copy() this=%p po=%lld head=%p tail=%p\n", this, po, head, tail);
   QoreNamespaceList *nsl = new QoreNamespaceList();

   QoreNamespace *w = head;

   while (w) {
      nsl->add(w->copy(po));
      w = w->priv->next;
   }

   return nsl;
}

void QoreNamespaceList::resolveCopy() {
   QoreNamespace *w = head;

   while (w) {
      w->priv->classList->resolveCopy();
      w = w->priv->next;
   }
}

void QoreNamespaceList::parseInitConstants() {
   QoreNamespace *w = head;

   while (w) {
      w->parseInitConstants();
      w = w->priv->next;
   }
}

void QoreNamespaceList::deleteAllConstants() {
   QoreNamespace *w = head;

   while (w) {
      w->priv->constant->deleteAll();
      w = w->priv->next;
   }
}

void QoreNamespaceList::parseInit() {
   QoreNamespace *w = head;

   while (w) {
      w->parseInit();
      w = w->priv->next;
   }
}

void QoreNamespaceList::parseCommit(QoreNamespaceList *l) {
   assimilate(l);

   QoreNamespace *w = head;

   while (w) {
      w->parseCommit();
      w = w->priv->next;
   }
}

void QoreNamespaceList::parseRollback() {
   QoreNamespace *w = head;

   while (w) {
      w->parseRollback();
      w = w->priv->next;
   }
}

QoreNamespace *QoreNamespace::findNamespace(const char *nname) const {
   QoreNamespace *rv = priv->nsl->find(nname);
   if (!rv)
      rv = priv->pendNSL->find(nname);
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

static int parseInitConstantHash(QoreHashNode *h, int level) {
   RootQoreNamespace *rns = getRootNS();

   HashIterator hi(h);
   while (hi.next()) {
      const char *k = hi.getKey();
      AbstractQoreNode **value = hi.getValuePtr();

      if (rns->parseInitConstantValue(value, level))
	 return -1;

      ReferenceHolder<AbstractQoreNode> n(0);
      // resolve constant references in keys
      if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST) {
	 // FIXME: add new entry points to RootQoreNamespace so it's not necessary to create these temporary AbstractQoreNode values
	 if (k[0] == HE_TAG_CONST) {
	    n = new BarewordNode(strdup(k + 1));
	 }
	 else
	    n = new ConstantNode(strdup(k + 1));
	 if (rns->parseInitConstantValue(n.getPtrPtr(), level))
	    return -1;

	 if (n) {
	    QoreStringValueHelper str(*n);
	 
	    // reference value for new hash
	    (*value)->ref();
	    // not possible to have an exception here
	    // adds to end of hash key list so it won't invalidate up our iterator
	    // the string must be in QCS_DEFAULT
	    h->setKeyValue(str->getBuffer(), *value, 0);
	    // or here
	    hi.deleteKey(0);
	 }
      }
   }

   return 0;
}

// QoreNamespaceList::parseResolveNamespace()
// does a recursive breadth-first search to resolve a namespace declaration
QoreNamespace *QoreNamespaceList::parseResolveNamespace(NamedScope *name, int *matched) {
   QORE_TRACE("QoreNamespaceList::parseResolveNamespace()");

   QoreNamespace *w = head, *ns = 0;

   // search first level of all sub namespaces
   while (w) {
      if ((ns = w->parseMatchNamespace(name, matched)))
	 break;
      w = w->priv->next;
   }
      //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);

   if (!ns) {
      // now search in all sub namespace lists
      w = head;
      while (w) {
	 if ((ns = w->priv->nsl->parseResolveNamespace(name, matched)))
	    break;
	 if ((ns = w->priv->pendNSL->parseResolveNamespace(name, matched)))
	    break;
	 //printd(5, "1:%s matched=%d\n", nslist[i]->name, *matched);
	 w = w->priv->next;
      }
   }


   return ns;
}

// QoreNamespaceList::parseFindConstantValue()
AbstractQoreNode *QoreNamespaceList::parseFindConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) {
   QORE_TRACE("QoreNamespaceList::parseFindConstantValue()");
   
   AbstractQoreNode *rv = 0;
   QoreNamespace *w = head;
   // see if a match can be found at the first level
   while (w) {
      if ((rv = w->getConstantValue(cname, typeInfo)))
	 break;
      w = w->priv->next;
   }

   if (!rv) { // check all levels
      w = head;
      while (w) {
	 if ((rv = w->priv->nsl->parseFindConstantValue(cname, typeInfo)))
	    break;
	 if ((rv = w->priv->pendNSL->parseFindConstantValue(cname, typeInfo)))
	    break;
	 w = w->priv->next;
      }
   }

   return rv;
}

/*
static void showNSL(QoreNamespaceList *nsl)
{
   printd(5, "showNSL() dumping %08p\n", nsl);
   for (int i = 0; i < nsl->num_namespaces; i++)
      printd(5, "showNSL()  %d: %08p %s (list: %08p)\n", i, nsl->nslist[i], nsl->nslist[i]->name, nsl->nslist[i]->nsl);
}
*/

// QoreNamespaceList::parseFindScopedConstantValue()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
AbstractQoreNode *QoreNamespaceList::parseFindScopedConstantValue(NamedScope *name, int *matched, const QoreTypeInfo *&typeInfo) {
   AbstractQoreNode *rv = 0;

   QORE_TRACE("QoreNamespaceList::parseFindScopedConstantValue()");
   printd(5, "QoreNamespaceList::parseFindScopedConstantValue(this=%08p) target: %s\n", this, name->ostr);

   //showNSL(this);
   // see if a complete match can be found at the first level
   QoreNamespace *w = head;
   while (w) {
      if ((rv = w->parseMatchScopedConstantValue(name, matched, typeInfo)))
	 break;
      w = w->priv->next;
   }

   if (!rv) { // now search all sub namespaces
      w = head;
      while (w) {
	 if ((rv = w->priv->nsl->parseFindScopedConstantValue(name, matched, typeInfo)))
	    break;
	 if ((rv = w->priv->pendNSL->parseFindScopedConstantValue(name, matched, typeInfo)))
	    break;
	 w = w->priv->next;
      }
   }

   return rv;
}

// QoreNamespaceList::parseFindScopedClassWithMethod()
// does a recursive breadth-first search to resolve a namespace containing the given class name
// note: is only called with a namespace specifier
QoreClass *QoreNamespaceList::parseFindScopedClassWithMethod(NamedScope *name, int *matched) const {
   QoreClass *oc = 0;

   // see if a complete match can be found at the first level
   QoreNamespace *w = head;
   while (w) {
      if ((oc = w->parseMatchScopedClassWithMethod(name, matched)))
	 break;
      w = w->priv->next;
   }

   if (!oc) { // now search all sub namespaces
      w = head;
      while (w) {
	 if ((oc = w->priv->pendNSL->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 if ((oc = w->priv->nsl->parseFindScopedClassWithMethod(name, matched)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}

QoreClass *QoreNamespaceList::parseFindClass(const char *ocname) {
   QoreClass *oc = 0;

   // see if a match can be found at the first level
   QoreNamespace *w = head;
   while (w) {
      if ((oc = w->priv->classList->find(ocname)))
	 break;
      // check pending classes
      if ((oc = w->priv->pendClassList->find(ocname)))
	 break;

      w = w->priv->next;
   }

   if (!oc) { // check all levels
      w = head;
      while (w) {
	 if ((oc = w->priv->nsl->parseFindClass(ocname)))
	    break;
	 if ((oc = w->priv->pendNSL->parseFindClass(ocname)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}

// QoreNamespaceList::parseFindScopedClass()
// does a recursive breadth-first search to resolve a namespace with the given object type
// note: is only called with a namespace specifier
QoreClass *QoreNamespaceList::parseFindScopedClass(NamedScope *name, int *matched) {
   QoreClass *oc = 0;

   // see if a complete match can be found at the first level
   QoreNamespace *w = head;
   while (w) {
      if ((oc = w->parseMatchScopedClass(name, matched)))
	 break;
      w = w->priv->next;
   }

   if (!oc) { // now search all sub namespaces
      w = head;
      while (w) {
	 if ((oc = w->priv->nsl->parseFindScopedClass(name, matched)))
	    break;
	 if ((oc = w->priv->pendNSL->parseFindScopedClass(name, matched)))
	    break;
	 w = w->priv->next;
      }
   }

   return oc;
}

// returns 0 for success, non-zero return value means error
int RootQoreNamespace::addMethodToClass(NamedScope *scname, MethodVariantBase *qcmethod, bool static_flag) {
   std::auto_ptr<MethodVariantBase> v(qcmethod);

   // find class
   QoreClass *oc;

   const char *cname  = scname->strlist[scname->elements - 2];
   const char *method = scname->strlist[scname->elements - 1];

   // if there is no namespace specified, then just find class
   if (scname->elements == 2) {
      oc = rootFindClass(cname);
      if (!oc) {
	 parse_error("reference to undefined class '%s' while trying to add method '%s'", cname, method);
	 return -1;
      }
   }
   else {
      int m = 0;
      oc = rootFindScopedClassWithMethod(scname, &m);
      if (!oc) {
	 if (m != (scname->elements - 2))
	    parse_error("cannot resolve namespace '%s' in '%s()'", scname->strlist[m], scname->ostr);
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

QoreClass *RootQoreNamespace::parseFindScopedClass(NamedScope *nscope) const {
   QoreClass *oc;
   // if there is no namespace specified, then just find class
   if (nscope->elements == 1) {
      oc = rootFindClass(nscope->strlist[0]);
      if (!oc)
	 parse_error("reference to undefined class '%s'", nscope->ostr);
   }
   else {
      int m = 0;
      oc = rootFindScopedClass(nscope, &m);

      if (!oc) {
	 if (m != (nscope->elements - 1))
	    parse_error("cannot resolve namespace '%s' in '%s()'", nscope->strlist[m], nscope->ostr);
	 else {
	    QoreString err;
	    err.sprintf("cannot find class '%s' in any namespace '", nscope->getIdentifier());
	    for (int i = 0; i < (nscope->elements - 1); i++) {
	       err.concat(nscope->strlist[i]);
	       if (i != (nscope->elements - 2))
		  err.concat("::");
	    }
	    err.concat("'");
	    parse_error(err.getBuffer());
	 }
      }
   }
   printd(5, "RootQoreNamespace::parseFindScopedClass('%s') returning %08p\n", nscope->ostr, oc);
   return oc;
}

QoreClass *RootQoreNamespace::parseFindScopedClassWithMethod(NamedScope *scname) const {
   // must have at least 2 elements
   assert(scname->elements > 1);

   QoreClass *oc;

   int m = 0;
   oc = rootFindScopedClassWithMethod(scname, &m);
   
   if (!oc) {
      if (m >= (scname->elements - 2))
	 parse_error("cannot resolve class '%s' in '%s()'", scname->strlist[m], scname->ostr);
      else  {	 
	 QoreString err;
	 err.sprintf("cannot find class '%s' in any namespace '", scname->strlist[scname->elements - 2]);
	 for (int i = 0; i < (scname->elements - 2); i++) {
	    err.concat(scname->strlist[i]);
	    if (i != (scname->elements - 3))
	       err.concat("::");
	 }
	 err.concat("'");
	 parse_error(err.getBuffer());
      }
   }
   
   printd(5, "RootQoreNamespace::parseFindScopedClassWithMethod('%s') returning %08p\n", scname->ostr, oc);
   return oc;
}

// returns 0 for success, non-zero for error
int RootQoreNamespace::parseInitConstantValue(AbstractQoreNode **val, int level) {
   if (!(*val))
      return 0;

   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH) {
      parse_error("maximum recursion level exceeded resolving constant definition");
      return -1;
   }

   //printd(5, "constant %08p resolving type '%s'\n", val, *val ? (*val)->getTypeName() : "null");

   while (true) {
      // type info here is ignored because the node has not yet been initialized anyway
      const QoreTypeInfo *typeInfo = 0;

      qore_type_t vtype = (*val)->getType();
      if (vtype == NT_BAREWORD) {
	 //printd(5, "constant %08p has recursive definition '%s'\n", val, reinterpret_cast<BarewordNode *>(*val)->str);
	 if (resolveSimpleConstant(val, level + 1, typeInfo))
	    return -1;
      }
      else if (vtype == NT_CONSTANT) {
	 //printd(5, "constant %08p has recursive definition '%s'\n", val, reinterpret_cast<ConstantNode *>(*val)->scoped_ref->ostr);
	 if (resolveScopedConstant(val, level + 1, typeInfo))
	    return -1;
      }
      else
	 break;
      //printd(5, "constant %08p resolved to type '%s'\n", val, *val ? (*val)->getTypeName() : "null");
   }

   qore_type_t vtype = (*val)->getType();

   // if it's an expression or container type, then 
   // 1) initialize each element
   // 2) evaluate in case it contains immediate expressions
   if (vtype == NT_TREE || vtype == NT_LIST || vtype == NT_HASH) {
      if (vtype == NT_LIST) {
	 QoreListNode *l = reinterpret_cast<QoreListNode *>(*val);
	 for (unsigned i = 0; i < l->size(); i++) {
	    if (parseInitConstantValue(l->get_entry_ptr(i), level))
	       return -1;
	 }
      }
      else if (vtype == NT_HASH) {
	 QoreHashNode *h = reinterpret_cast<QoreHashNode *>(*val);
	 if (parseInitConstantHash(h, level))
	    return -1;
      }
      else if (vtype == NT_TREE) {
	 QoreTreeNode *tree =reinterpret_cast<QoreTreeNode *>(*val);
	 if (parseInitConstantValue(&(tree->left), level))
	    return -1;
	 if (tree->right)
	    if (parseInitConstantValue(&(tree->right), level))
	       return -1;
      }

      //printd(5, "evaluating constant expression %08p\n", *val);
      ExceptionSink xsink;
      AbstractQoreNode *n = (*val)->eval(&xsink);
      (*val)->deref(&xsink);
      *val = n;
   }

   return 0;
}

// returns 0 for success, non-zero for error
int RootQoreNamespace::resolveSimpleConstant(AbstractQoreNode **node, int level, const QoreTypeInfo *&typeInfo) const {
   assert(*node && (*node)->getType() == NT_BAREWORD);
   BarewordNode *b = reinterpret_cast<BarewordNode *>(*node);
   printd(5, "RootQoreNamespace::resolveSimpleConstant(%s, %d)\n", b->str, level);

   // if constant is not found, then a parse error will be raised
   AbstractQoreNode *rv = findConstantValue(b->str, level, typeInfo);

   printd(5, "RootQoreNamespace::resolveSimpleConstant(%s, %d) %08p %s-> %08p %s\n", 
	  b->str, level, *node, (*node)->getTypeName(), rv, rv ? rv->getTypeName() : "n/a");

   b->deref();
   // here we put &True in the tree, so a value will be there - 
   // nulls cannot appear in the parse tree
   *node = rv ? rv->refSelf() : &True;

   if (!rv)
      return -1;

   return 0;
}

int RootQoreNamespace::resolveScopedConstant(AbstractQoreNode **node, int level, const QoreTypeInfo *&typeInfo) const {
   assert(node && (*node)->getType() == NT_CONSTANT);
   ConstantNode *c = reinterpret_cast<ConstantNode *>(*node);

   // if constant is not found, then a parse error will be raised
   AbstractQoreNode *rv = findConstantValue(c->scoped_ref, level, typeInfo);

   c->deref();
   // here we put &True in the tree, so a value will be there - 
   // nulls cannot appear in the parse tree
   *node = rv ? rv->refSelf() : &True;

   if (!rv) {
      //printd(5, "RootQoreNamespace::resolveScopedConstant(%s, %d) findConstantValue() returned 0\n", c->scoped_ref->ostr, level);
      return -1;
   }

   //printd(5, "RootQoreNamespace::resolveScopedConstant(%s, %d) %08p %s-> %08p %s\n", c->scoped_ref->ostr, level, *node, (*node)->getTypeName(), rv, rv->getTypeName());
   return 0;
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
AbstractQoreNode *RootQoreNamespace::findConstantValue(NamedScope *scname, int level, const QoreTypeInfo *&typeInfo) const {
   // check recurse level and throw an error if it's too deep
   if (level >= MAX_RECURSION_DEPTH) {
      parse_error("recursive constant definitions too deep resolving '%s'", scname->ostr);
      return 0;
   }

   AbstractQoreNode *rv;

   if (scname->elements == 1) {
      rv = rootFindConstantValue(scname->ostr, typeInfo);
      if (!rv) {
	 parse_error("constant '%s' cannot be resolved in any namespace", scname->ostr);
	 return 0;
      }
      return rv;
   }

   int m = 0;
   rv = rootFindScopedConstantValue(scname, &m, typeInfo);
   if (rv)
      return rv;

   if (m != (scname->elements - 1))
      parse_error("cannot resolve namespace '%s' in '%s'", scname->strlist[m], scname->ostr);
   else {
      QoreString err;
      err.sprintf("cannot find constant '%s' in any namespace '", scname->getIdentifier());
      for (int i = 0; i < (scname->elements - 1); i++) {
	 err.concat(scname->strlist[i]);
	 if (i != (scname->elements - 2))
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
   //printd(5, "QoreNamespace::addClass() adding str=%s (%08p)\n", oc->name, oc);
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
   // assimilate pending constants
   // assimilate target list - if there were errors then the list will be deleted anyway
   priv->pendConstant->assimilate(ns->priv->pendConstant, priv->constant, priv->name.c_str());

   // assimilate classes
   priv->pendClassList->assimilate(ns->priv->pendClassList, priv->classList, priv->nsl, priv->pendNSL, priv->name.c_str());

   // assimilate sub namespaces
   QoreNamespace *nw = ns->priv->pendNSL->head;
   while (nw) {
      // throw parse exception if name is already defined
      if (priv->nsl->find(nw->priv->name.c_str()))
	 parse_error("subnamespace '%s' has already been defined in namespace '%s'",
		     nw->priv->name.c_str(), priv->name.c_str());
      else if (priv->pendNSL->find(nw->priv->name.c_str()))
	 parse_error("subnamespace '%s' is already pending in namespace '%s'",
		     nw->priv->name.c_str(), priv->name.c_str());
      else if (priv->classList->find(nw->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class has already been defined with this name",
		     nw->priv->name.c_str(), priv->name.c_str());
      else if (priv->pendClassList->find(nw->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class is already pending with this name",
		     nw->priv->name.c_str(), priv->name.c_str());

      nw = nw->priv->next;
   }
   // assimilate target list
   priv->pendNSL->assimilate(ns->priv->pendNSL);

   // delete source namespace
   delete ns;
}

// QoreNamespace::parseMatchNamespace()
// will only be called if there is a match with the name and nscope->elements > 1
QoreNamespace *QoreNamespace::parseMatchNamespace(NamedScope *nscope, int *matched) const {
   // see if starting name matches this namespace
   if (!strcmp(nscope->strlist[0], priv->name.c_str())) {
      const QoreNamespace *ns = this;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // check for a match of the structure in this namespace
      for (int i = 1; i < (nscope->elements - 1); i++) {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    break;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
      return (QoreNamespace *)ns;
   }

   return 0;
}

QoreClass *QoreNamespace::parseMatchScopedClassWithMethod(NamedScope *nscope, int *matched) const {
   printd(5, "QoreNamespace::parseMatchScopedClassWithMethod(this=%08p) %s class=%s (%s)\n", this, priv->name.c_str(), nscope->strlist[nscope->elements - 2], nscope->ostr);

   const QoreNamespace *ns = this;
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2) {
      // if first namespace doesn't match, then return 0
      if (strcmp(nscope->strlist[0], priv->name.c_str()))
	 return 0;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (int i = 1; i < (nscope->elements - 2); i++) {	 
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   // check last namespaces
   QoreClass *rv = ns->priv->pendClassList->find(nscope->strlist[nscope->elements - 2]);
   if (!rv)
      rv = ns->priv->classList->find(nscope->strlist[nscope->elements - 2]);

   return rv;
}

QoreClass *QoreNamespace::parseMatchScopedClass(NamedScope *nscope, int *matched) const {
   if (strcmp(nscope->strlist[0], priv->name.c_str()))
      return 0;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   printd(5, "QoreNamespace::parseMatchScopedClass() matched %s in %s\n", priv->name.c_str(), nscope->ostr);

   const QoreNamespace *ns = this;
   
   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2) {
      for (int i = 1; i < (nscope->elements - 1); i++) {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   QoreClass *rv = ns->priv->classList->find(nscope->strlist[nscope->elements - 1]);
   if (!rv)
      rv = ns->priv->pendClassList->find(nscope->strlist[nscope->elements - 1]);
   return rv;
}

AbstractQoreNode *QoreNamespace::parseMatchScopedConstantValue(NamedScope *nscope, int *matched, const QoreTypeInfo *&typeInfo) const {
   printd(5, "QoreNamespace::parseMatchScopedConstantValue() trying to find %s in %s (%p) typeInfo=%p\n", 
	  nscope->getIdentifier(), priv->name.c_str(), getConstantValue(nscope->getIdentifier(), typeInfo));

   if (strcmp(nscope->strlist[0], priv->name.c_str()))
      return 0;

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   const QoreNamespace *ns = this;

   // if we need to follow the namespaces, then do so
   if (nscope->elements > 2) {
      for (int i = 1; i < (nscope->elements - 1); i++) {
	 ns = ns->findNamespace(nscope->strlist[i]);
	 if (!ns)
	    return 0;
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

   if (priv->nsl->head) {
      QoreHashNode *nsh = new QoreHashNode();
      
      QoreNamespace *w = priv->nsl->head;
      while (w) {
	 nsh->setKeyValue(w->priv->name.c_str(), w->getInfo(), 0);
	 w = w->priv->next;
      }

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
void RootQoreNamespace::rootAddClass(NamedScope *nscope, QoreClass *oc) {
   QORE_TRACE("RootQoreNamespace::rootAddClass()");

   QoreNamespace *sns = rootResolveNamespace(nscope);

   if (sns) {
      printd(5, "RootQoreNamespace::rootAddClass() '%s' adding %s:%08p to %s:%08p\n", nscope->ostr, 
	     oc->getName(), oc, sns->priv->name.c_str(), sns);
      sns->addClass(oc);
   }
   else
      delete oc;
}

void RootQoreNamespace::rootAddConstant(NamedScope *nscope, AbstractQoreNode *value) {
   QoreNamespace *sns = rootResolveNamespace(nscope);

   if (sns) {
      printd(5, "RootQoreNamespace::rootAddConstant() %s: adding %s to %s (value=%08p type=%s)\n", nscope->ostr, 
	     nscope->getIdentifier(), sns->priv->name.c_str(), value, value ? value->getTypeName() : "(none)");
      sns->priv->pendConstant->add(nscope->strlist[nscope->elements - 1], value);
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

QoreNamespace *RootQoreNamespace::rootResolveNamespace(NamedScope *nscope) {
   if (nscope->elements == 1)
      return this;

   QoreNamespace *ns;
   int match = 0;

   if (!(ns = parseMatchNamespace(nscope, &match))
       && !(ns = priv->nsl->parseResolveNamespace(nscope, &match))
       && !(ns = priv->pendNSL->parseResolveNamespace(nscope, &match)))

   if (!ns)
      parse_error("cannot resolve namespace '%s' in '%s'", nscope->strlist[match], nscope->ostr);

   return ns;
}

// private
QoreClass *RootQoreNamespace::rootFindScopedClassWithMethod(NamedScope *nscope, int *matched) const {
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched))
       && !(oc = priv->nsl->parseFindScopedClassWithMethod(nscope, matched)))
      oc = priv->pendNSL->parseFindScopedClassWithMethod(nscope, matched);
   return oc;
}

// private
// will always be called with a namespace (nscope->elements > 1)
QoreClass *RootQoreNamespace::rootFindScopedClass(NamedScope *nscope, int *matched) const {
   QoreClass *oc = parseMatchScopedClass(nscope, matched);
   if (!oc && !(oc = priv->nsl->parseFindScopedClass(nscope, matched)))
      oc = priv->pendNSL->parseFindScopedClass(nscope, matched);
   return oc;
}

// private, will always be called with nscope->elements > 1
AbstractQoreNode *RootQoreNamespace::rootFindScopedConstantValue(NamedScope *nscope, int *matched, const QoreTypeInfo *&typeInfo) const {
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

   // add system object types
   qoreNS->addSystemClass(initSocketClass());
   qoreNS->addSystemClass(initSSLCertificateClass());
   qoreNS->addSystemClass(initSSLPrivateKeyClass());
   qoreNS->addSystemClass(initProgramClass());
   QoreClass *TermIOS, *File;
   qoreNS->addSystemClass(TermIOS = initTermIOSClass());
   qoreNS->addSystemClass(File = initFileClass(TermIOS));
   qoreNS->addSystemClass(initDirClass(File));
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

   // math constants
   qoreNS->addConstant("M_PI",          new QoreFloatNode(3.14159265358979323846));

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

#ifdef HAVE_TIMEGM
   option->addConstant("HAVE_TIMEGM",   &True);
#else
   option->addConstant("HAVE_TIMEGM",   &False);
#endif

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
		   staticSystemNamespace.priv->constant->copy(),
		   staticSystemNamespace.priv->nsl->copy(po)) {
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
   return new RootQoreNamespace(priv->classList->copy(po), priv->constant->copy(), priv->nsl->copy(po));
}

#ifdef DEBUG_TESTS
// moved down to allow to test internal classes
#  include "tests/Namespace_tests.cpp"
#endif

