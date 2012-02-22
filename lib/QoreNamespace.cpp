/* -*- indent-tabs-mode: nil -*- */
/*
  QoreNamespace.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2012 David Nichols

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
#include <qore/intern/QC_TermIOS.h>
#include <qore/intern/QC_TimeZone.h>

#include <qore/intern/QC_Datasource.h>
#include <qore/intern/QC_DatasourcePool.h>
#include <qore/intern/QC_SQLStatement.h>

#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <assert.h>

#include <memory>

#ifdef DEBUG_TESTS
// the #include "test/Namespace_tests.cpp" is on the bottom
#  include "tests/builtin_inheritance_tests.cpp"
#endif

DLLLOCAL void init_Datasource_constants(QoreNamespace& ns);
DLLLOCAL void init_File_constants(QoreNamespace& ns);
DLLLOCAL void init_Program_constants(QoreNamespace& ns);
DLLLOCAL void init_Socket_constants(QoreNamespace& ns);
DLLLOCAL void init_TermIOS_constants(QoreNamespace& ns);
DLLLOCAL void init_type_constants(QoreNamespace& ns);
DLLLOCAL void init_compression_constants(QoreNamespace& ns);
DLLLOCAL void init_crypto_constants(QoreNamespace& ns);
DLLLOCAL void init_misc_constants(QoreNamespace& ns);
DLLLOCAL void init_string_constants(QoreNamespace& ns);
DLLLOCAL void init_option_constants(QoreNamespace& ns);
DLLLOCAL void init_math_constants(QoreNamespace& ns);
DLLLOCAL void init_qore_constants(QoreNamespace& ns);
DLLLOCAL void init_errno_constants(QoreNamespace& ns);

DLLLOCAL void init_dbi_functions(QoreNamespace& ns);
DLLLOCAL void init_dbi_constants(QoreNamespace& ns);

#define MAX_RECURSION_DEPTH 20

StaticSystemNamespace staticSystemNamespace;

AutoNamespaceList ANSL;

QoreNamespace::QoreNamespace(const char *n) : priv(new qore_ns_private(this, n)) {
}

QoreNamespace::QoreNamespace(qore_ns_private* p) : priv(p) {
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

void QoreNamespace::addNamespace(QoreNamespace *ns) {
   assert(!priv->classList->find(ns->priv->name.c_str()));
   assert(!priv->pendClassList->find(ns->priv->name.c_str()));
   ns->priv->parent = priv;
   priv->nsl->add(ns);
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
      ons->priv->assimilate(ns);
      return;
   }
   nsmap[ns->priv->name] = ns;
}

QoreNamespace *QoreNamespace::copy(int po) const {
   //printd(5, "QoreNamespace::copy() (deprecated) this=%p po=%d %s\n", this, po, priv->name.c_str());
   return qore_ns_private::newNamespace(*priv, po);
}

QoreNamespace *QoreNamespace::copy(int64 po) const {
   //printd(5, "QoreNamespace::copy() this=%p po=%lld %s\n", this, po, priv->name.c_str());
   return qore_ns_private::newNamespace(*priv, po);
}

QoreNamespaceList *QoreNamespaceList::copy(int64 po, const qore_ns_private& parent) {
   //printd(5, "QoreNamespaceList::copy() this=%p po=%lld size=%d\n", this, po, nsmap.size());
   QoreNamespaceList *nsl = new QoreNamespaceList();

   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      QoreNamespace *ns = i->second->copy(po);
      ns->priv->parent = &parent;
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
      i->second->priv->parseInitConstants();
}

void QoreNamespaceList::deleteAllConstants(ExceptionSink *xsink) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->constant->deleteAll(xsink);
}

void QoreNamespaceList::parseInit() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->parseInit();
}

void QoreNamespaceList::parseCommit(QoreNamespaceList *l) {
   assimilate(l);

   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->parseCommit();
}

void QoreNamespaceList::parseRollback() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->parseRollback();
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
	 tns->priv->parent = parent->priv;
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
   return priv->parent->ns;
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
   //QORE_TRACE("QoreNamespaceList::parseResolveNamespace");
   //printd(5, "QoreNamespaceList::parseResolveNamespace() this=%p ns=%s matched=%d\n", this, name->ostr, *matched);

   QoreNamespace *ns = 0;

   // search first level of all sub namespaces
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((ns = i->second->priv->parseMatchNamespace(name, matched)))
	 return ns;
   }      

   // now search in all sub namespace lists
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      //printd(5, "QoreNamespaceList::parseResolveNamespace() this=%p ns=%s\n", this, i->second->getName());
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
   //QORE_TRACE("QoreNamespaceList::parseFindConstantValue()");
   //printd(5, "QoreNamespaceList::parseFindConstantValue() this=%p name='%s'\n", this, cname);

   AbstractQoreNode *rv = 0;
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      if ((rv = i->second->priv->getConstantValue(cname, typeInfo)))
	 return rv;
   }

   // check all levels
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i) {
      //printd(0, "QoreNamespaceList::parseFindConstantValue() this=%p name='%s' ns=%p '%s'\n", this, cname, i->second, i->second->getName());
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
      if ((rv = i->second->priv->parseMatchScopedConstantValue(name, matched, typeInfo)))
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
      if ((oc = i->second->priv->parseMatchScopedClassWithMethod(name, matched)))
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
      if ((oc = i->second->priv->parseMatchScopedClass(name, matched)))
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

RootQoreNamespace::RootQoreNamespace(qore_ns_private* p) : QoreNamespace(p) {
}

QoreNamespace *RootQoreNamespace::rootGetQoreNamespace() const {
   return qoreNS;
}

StaticSystemNamespace::StaticSystemNamespace() : RootQoreNamespace(new qore_ns_private(this, "")) {
}

// sets up the root namespace
void StaticSystemNamespace::init() {
   QORE_TRACE("StaticSystemNamespace::init()");

   priv->name = "";

   qoreNS = new QoreNamespace("Qore");

   qoreNS->addInitialNamespace(get_thread_ns(*qoreNS));

   QoreClass *File;

   // add system object types
   qoreNS->addSystemClass(initTimeZoneClass(*qoreNS));
   qoreNS->addSystemClass(initSSLCertificateClass(*qoreNS));
   qoreNS->addSystemClass(initSSLPrivateKeyClass(*qoreNS));
   qoreNS->addSystemClass(initSocketClass(*qoreNS));
   qoreNS->addSystemClass(initProgramClass(*qoreNS));

   qoreNS->addSystemClass(initTermIOSClass(*qoreNS));
   qoreNS->addSystemClass(File = initFileClass(*qoreNS));
   qoreNS->addSystemClass(initDirClass(*qoreNS));
   qoreNS->addSystemClass(initGetOptClass(*qoreNS));
   qoreNS->addSystemClass(initFtpClientClass(*qoreNS));

   // add HTTPClient namespace
   qoreNS->addSystemClass(initHTTPClientClass(*qoreNS));

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

   init_qore_constants(*qoreNS);

   // set up Option namespace for Qore options
   QoreNamespace* option = new QoreNamespace("Option");
   init_option_constants(*option);
   qoreNS->addInitialNamespace(option);

   // create Qore::SQL namespace
   QoreNamespace* sqlns = new QoreNamespace("SQL");

   sqlns->addSystemClass(initDatasourceClass(*sqlns));
   sqlns->addSystemClass(initDatasourcePoolClass(*sqlns));
   sqlns->addSystemClass(initSQLStatementClass(*sqlns));

   init_dbi_functions(*sqlns);
   init_dbi_constants(*sqlns);
   qoreNS->addInitialNamespace(sqlns);

   // create get Qore::Err namespace with ERRNO constants
   QoreNamespace* Err = new QoreNamespace("Err");
   init_errno_constants(*Err);
   qoreNS->addInitialNamespace(Err);

   QoreNamespace* tns = new QoreNamespace("Type");
   init_type_constants(*tns);
   qoreNS->addInitialNamespace(tns);

   init_Datasource_constants(*qoreNS);
   init_File_constants(*qoreNS);
   init_Program_constants(*qoreNS);
   init_Socket_constants(*qoreNS);
   init_TermIOS_constants(*qoreNS);
   init_type_constants(*qoreNS);
   init_compression_constants(*qoreNS);
   init_crypto_constants(*qoreNS);
   init_misc_constants(*qoreNS);
   init_string_constants(*qoreNS);
   init_math_constants(*qoreNS);

   builtinFunctions.init(*qoreNS);

   addInitialNamespace(qoreNS);

   // add all changes in loaded modules
   ANSL.init(this, qoreNS);
}

#ifdef DEBUG_TESTS
// moved down to allow to test internal classes
#  include "tests/Namespace_tests.cpp"
#endif

// returns 0 for success, non-zero return value means error
int qore_ns_private::rootAddMethodToClass(const NamedScope *scname, MethodVariantBase *qcmethod, bool static_flag) {
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

// returns 0 for success, non-zero for error
int qore_ns_private::rootResolveBareword(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo) const {
   assert(*node && (*node)->getType() == NT_BAREWORD);
   SimpleRefHolder<BarewordNode> b(reinterpret_cast<BarewordNode *>(*node));

   QoreClass *pc = getParseClass();

   //printd(5, "qore_ns_private::rootResolveBareword(%s) pc=%p (%s)\n", b->str, pc, pc ? pc->getName() : "<none>");

   AbstractQoreNode *rv = 0;

   QoreProgram *pgm = getProgram();
   bool abr = (bool)(pgm->getParseOptions64() & PO_ALLOW_BARE_REFS);

   if (abr) {
      bool in_closure;
      LocalVar *id = find_local_var(b->str, in_closure);
      if (id) {
         typeInfo = id->getTypeInfo();
         *node = new VarRefNode(b->takeString(), id, in_closure);
         return 0;
      }
   }

   // if there is a current parse class context, then check it first
   if (pc) {
      // check for member reference
      if (abr) {
         if (!qore_class_private::parseResolveInternalMemberAccess(pc, b->str, typeInfo)) {
            *node = new SelfVarrefNode(b->takeString());
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
            return 0;
         }
      }

      // try to resolve constant
      rv = rootFindConstantValue(b->str, typeInfo);
      if (rv)
         rv->ref();
   }

   if (rv) {
      *node = rv;
      return 0;
   }

   parse_error("cannot resolve bareword '%s' to any reachable object", b->str);

   //printd(5, "qore_ns_private::rootResolveBareword(%s) %p %s-> %p %s\n", b->str, *node, (*node)->getTypeName(), rv, get_type_name(rv));
   *node = &Nothing;
   //b.release();
   return -1;
}

int qore_ns_private::rootResolveScopedReference(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo) const {
   assert(node && (*node)->getType() == NT_CONSTANT);
   ScopedRefNode *c = reinterpret_cast<ScopedRefNode *>(*node);

   unsigned m = 0;
   AbstractQoreNode *n = resolveScopedReference(*c->scoped_ref, m, typeInfo);
   if (n) {
      //printd(5, "qore_ns_private::rootResolveScopedReference(%s) %p %s-> %p %s\n", c->scoped_ref->ostr, *node, (*node)->getTypeName(), n, get_type_name(n));
      c->deref();
      *node = n;
      return 0;
   }

   NamedScope &ns = *c->scoped_ref;

   if (ns.size() == 1) {
      //printd(5, "qore_ns_private::rootResolveBareword(%s) %p %s-> %p %s\n", b->str, *node, (*node)->getTypeName(), rv, get_type_name(rv));
      parse_error("cannot resolve bareword '%s' to any reachable definition", ns.ostr);
      return -1;
   }

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

   //printd(5, "qore_ns_private::rootResolveScopedReference(%s) not found\n", c->scoped_ref->ostr);
   return -1;
}

AbstractQoreNode *qore_ns_private::rootResolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo) const {
   if (ns.size() == 1)
      return parseResolveBareword(ns.ostr, typeInfo);

   // first try to resolve a global variable
   if (ns.size() == 2 && ns.strlist[0] == "") {
      Var *v = getProgram()->findGlobalVar(ns.getIdentifier());
      if (v)
         return new GlobalVarRefNode(strdup(ns.getIdentifier()), v);
   }

   return parseResolveScopedReference(ns, m, typeInfo);
}

// private
QoreClass *qore_ns_private::rootFindScopedClassWithMethod(const NamedScope *nscope, unsigned *matched) {
   QoreClass *oc;

   if (!(oc = parseMatchScopedClassWithMethod(nscope, matched))
       && !(oc = nsl->parseFindScopedClassWithMethod(nscope, matched)))
      oc = pendNSL->parseFindScopedClassWithMethod(nscope, matched);
   return oc;
}

QoreClass *qore_ns_private::parseFindClass(const char *cname) const {
   QoreClass *oc = rootFindClass(cname);
   if (!oc)
      parse_error("reference to undefined class '%s'", cname);

   return oc;
}

QoreClass *qore_ns_private::parseFindScopedClass(const NamedScope *nscope) {
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
   printd(5, "qore_ns_private::parseFindScopedClass('%s') returning %p\n", nscope->ostr, oc);
   return oc;
}

QoreClass *qore_ns_private::parseFindScopedClassWithMethod(const NamedScope *scname) {
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
   
   printd(5, "qore_ns_private::parseFindScopedClassWithMethod('%s') returning %p\n", scname->ostr, oc);
   return oc;
}

// only called with RootNS
AbstractQoreNode *qore_ns_private::rootFindConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv;
   if (!(rv = getConstantValue(cname, typeInfo))
       && (!(rv = nsl->parseFindConstantValue(cname, typeInfo))))
      rv = pendNSL->parseFindConstantValue(cname, typeInfo);
      
   return rv;
}

AbstractQoreNode *qore_ns_private::findConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) const {
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
AbstractQoreNode *qore_ns_private::findConstantValue(const NamedScope *scname, const QoreTypeInfo *&typeInfo) const {
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

// only called with RootNS
void qore_ns_private::rootAddClass(const NamedScope *nscope, QoreClass *oc) {
   QORE_TRACE("qore_ns_private::rootAddClass()");

   QoreNamespace *sns = rootResolveNamespace(nscope);

   if (sns) {
      //printd(5, "qore_ns_private::rootAddClass() '%s' adding %s:%p to %s:%p\n", nscope->ostr, oc->getName(), oc, sns->priv->name.c_str(), sns);
      sns->priv->addClass(oc);
   }
   else {
      //printd(5, "qore_ns_private::rootAddClass() class '%s' not added: '%s' namespace not found\n", oc->getName(), nscope->ostr);
      delete oc;
   }
}

void qore_ns_private::rootAddConstant(const NamedScope &nscope, AbstractQoreNode *value) {
   QoreNamespace *sns = rootResolveNamespace(&nscope);

   if (sns) {
      printd(5, "qore_ns_private::rootAddConstant() %s: adding %s to %s (value=%p type=%s)\n", nscope.ostr, 
	     nscope.getIdentifier(), sns->priv->name.c_str(), value, value ? value->getTypeName() : "(none)");
      sns->priv->pendConstant->parseAdd(nscope.strlist[nscope.size() - 1].c_str(), value);
   }
   else
      value->deref(0);
}

// public
QoreClass* qore_ns_private::rootFindClass(const char *ocname) const {
   QORE_TRACE("qore_ns_private::rootFindClass");

   // should only be called on the root namespace
   assert(name.empty() && !parent);

   QoreClass *oc;
   if (!(oc = classList->find(ocname))
       && !(oc = pendClassList->find(ocname))
       && !(oc = nsl->parseFindClass(ocname)))
      oc = pendNSL->parseFindClass(ocname);

   return oc;
}

// private
// will always be called with a namespace (nscope->size() > 1)
QoreClass *qore_ns_private::rootFindScopedClass(const NamedScope *nscope, unsigned *matched) {
   // should only be called on the root namespace
   assert(name.empty() && !parent);

   QoreClass *oc = parseMatchScopedClass(nscope, matched);
   if (!oc && !(oc = nsl->parseFindScopedClass(nscope, matched)))
      oc = pendNSL->parseFindScopedClass(nscope, matched);
   return oc;
}

// private, will always be called with nscope->size() > 1
AbstractQoreNode *qore_ns_private::rootFindScopedConstantValue(const NamedScope *nscope, unsigned *matched, const QoreTypeInfo *&typeInfo) const {
   // should only be called on the root namespace
   assert(name.empty() && !parent);

   AbstractQoreNode *rv = parseMatchScopedConstantValue(nscope, matched, typeInfo);
   if (!rv && !(rv = nsl->parseFindScopedConstantValue(nscope, matched, typeInfo)))
      rv = pendNSL->parseFindScopedConstantValue(nscope, matched, typeInfo);
   return rv;
}

QoreNamespace *qore_ns_private::rootResolveNamespace(const NamedScope *nscope) {
   // should only be called on the root namespace
   assert(name.empty() && !parent);

   if (nscope->size() == 1)
      return ns;

   QoreNamespace *fns;
   unsigned match = 0;

   if (!(fns = parseMatchNamespace(nscope, &match))
       && !(fns = nsl->parseResolveNamespace(nscope, &match))
       && !(fns = pendNSL->parseResolveNamespace(nscope, &match)))
      if (!fns)
         parse_error("cannot resolve namespace '%s' in '%s'", nscope->strlist[match].c_str(), nscope->ostr);

   return fns;
}

void qore_ns_private::parseInitConstants() {
   printd(5, "qore_ns_private::parseInitConstants() %s\n", name.c_str());
   // do 2nd stage parse initialization on pending constants
   pendConstant->parseInit();

   pendNSL->parseInitConstants();
}

void qore_ns_private::parseInit() {
   printd(5, "qore_ns_private::parseInit() this=%p ns=%s\n", this, ns);

   // do 2nd stage parse initialization on committed classes
   classList->parseInit();
   
   // do 2nd stage parse initialization on pending classes
   pendClassList->parseInit();

   // do 2nd stage parse initialization on pending classes in pending lists of subnamespaces
   nsl->parseInit();

   // do 2nd stage parse initialization on pending namespaces
   pendNSL->parseInit();
}

void qore_ns_private::parseCommit() {
   // merge pending constant list
   constant->assimilate(pendConstant);

   // merge pending classes and commit pending changes to committed classes
   classList->parseCommit(pendClassList);

   // merge pending namespaces and repeat for all subnamespaces
   nsl->parseCommit(pendNSL);
}

void qore_ns_private::parseRollback() {
   printd(5, "qore_ns_private::parseRollback() %s this=%p ns=%p\n", name.c_str(), this, ns);

   // delete pending constant list
   pendConstant->parseDeleteAll();

   // delete pending changes to committed classes
   classList->parseRollback();

   // delete pending classes
   pendClassList->reset();

   // delete pending namespaces
   pendNSL->reset();

   // do for all subnamespaces
   nsl->parseRollback();
}

void qore_ns_private::parseAddNamespace(QoreNamespace *nns) {
   std::auto_ptr<QoreNamespace> nnsh(nns);

   //printd(5, "qore_ns_private::parseAddNamespace() this=%p '%s' adding %p '%s' (exists %p)\n", this, getName(), ns, ns->getName(), priv->nsl->find(ns->getName()));

   // raise an exception if namespace collides with an object name
   if (classList->find(nns->getName())) {
      parse_error("namespace name '%s' collides with previously-defined class '%s'", ns->getName(), ns->getName());
      return;
   }

   if (pendClassList->find(nns->getName())) {
      parse_error("namespace name '%s' collides with pending class '%s'", ns->getName(), ns->getName());
      return;
   }

   nnsh.release();

   // see if a committed namespace with the same name already exists
   QoreNamespace *orig = nsl->find(nns->getName());
   if (orig) {
      orig->priv->assimilate(nns);
      return;
   }

   nns->priv->parent = this;
   pendNSL->add(nns);
}

// only called while parsing before addition to namespace tree, no locking needed
void qore_ns_private::parseAddConstant(const NamedScope &nscope, AbstractQoreNode *value) {
   ReferenceHolder<> vh(value, 0);

   QoreNamespace *sns = resolveNameScope(&nscope);
   if (!sns)
      return;

   const char *cname = nscope.strlist[nscope.size() - 1].c_str();
   if (sns->priv->constant->inList(cname)) {
      parse_error("constant '%s' has already been defined", cname);
      return;
   }

   sns->priv->pendConstant->parseAdd(cname, vh.release());
}

// public, only called either in single-threaded initialization or
// while the program-level parse lock is held
void qore_ns_private::addClass(QoreClass *oc) {
   std::auto_ptr<QoreClass> och(oc);

   //printd(5, "qore_ns_private::addClass() adding str=%s (%p)\n", oc->name, oc);
   // raise an exception if object name collides with a namespace
   if (nsl->find(oc->getName()))
      parse_error("class name '%s' collides with previously-defined namespace '%s'", oc->getName(), oc->getName());
   else if (pendNSL->find(oc->getName()))
      parse_error("class name '%s' collides with pending namespace '%s'", oc->getName(), oc->getName());
   else if (classList->find(oc->getName()))
      parse_error("class '%s' already exists in namespace '%s'", oc->getName(), name.c_str());
   else if (pendClassList->add(oc))
      parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), name.c_str());
   else
      och.release();
}

// public, only called when parsing for unattached namespaces
void qore_ns_private::addClass(const NamedScope *n, QoreClass *oc) {
   std::auto_ptr<QoreClass> och(oc);
   //printd(5, "QoreNamespace::addClass() adding ns=%s (%s, %p)\n", n->ostr, oc->getName(), oc);
   QoreNamespace *sns = resolveNameScope(n);
   if (!sns)
      return;
   
   if (sns->priv->classList->find(oc->getName()))
      parse_error("class '%s' already exists in namespace '%s'", oc->getName(), name.c_str());
   else if (sns->priv->pendClassList->add(oc))
      parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), name.c_str());
   else
      och.release();
}

void qore_ns_private::assimilate(QoreNamespace *ans) {
   // make sure there are no objects in the committed lists
   assert(ans->priv->nsl->empty());
   assert(ans->priv->constant->empty());
   assert(ans->priv->classList->empty());

   // assimilate pending constants
   // assimilate target list - if there were errors then the list will be deleted anyway
   pendConstant->assimilate(ans->priv->pendConstant, constant, name.c_str());

   // assimilate classes
   pendClassList->assimilate(ans->priv->pendClassList, classList, nsl, pendNSL, name.c_str());

   // assimilate sub namespaces
   for (nsmap_t::iterator i = ans->priv->pendNSL->nsmap.begin(), e = ans->priv->pendNSL->nsmap.end(); i != e; ++i) {
      // throw parse exception if name is already defined as a namespace or class
      if (nsl->find(i->second->priv->name.c_str()))
	 parse_error("subnamespace '%s' has already been defined in namespace '%s'", i->second->priv->name.c_str(), name.c_str());
      else if (pendNSL->find(i->second->priv->name.c_str()))
	 parse_error("subnamespace '%s' is already pending in namespace '%s'", i->second->priv->name.c_str(), name.c_str());
      else if (classList->find(i->second->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class has already been defined with this name",
		     i->second->priv->name.c_str(), name.c_str());
      else if (pendClassList->find(i->second->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class is already pending with this name",
		     i->second->priv->name.c_str(), name.c_str());
   }
   // assimilate target list
   pendNSL->assimilate(ans->priv->pendNSL);

   // delete source namespace
   delete ans;
}

QoreClass *qore_ns_private::parseFindLocalClass(const char *cname) const {
   QoreClass *rv = classList->find(cname);
   return rv ? rv : pendClassList->find(cname);
}

AbstractQoreNode *qore_ns_private::getConstantValue(const char *cname, const QoreTypeInfo *&typeInfo) const {
   AbstractQoreNode *rv = constant->find(cname, typeInfo);
   if (rv)
      return rv;

   return rv ? rv : pendConstant->find(cname, typeInfo);
   /*
   if (rv)
      return rv;

   rv = classList->findConstant(cname, typeInfo);
   if (rv)
      return rv;

   return pendClassList->findConstant(cname, typeInfo);
   */
}

QoreNamespace *qore_ns_private::resolveNameScope(const NamedScope *nscope) const {
   const QoreNamespace *sns = ns;

   // find namespace
   for (unsigned i = 0; i < (nscope->size() - 1); i++)
      if (!(sns = sns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str()))) {
	 parse_error("namespace '%s' cannot be resolved while evaluating '%s' in constant declaration",
		     nscope->strlist[i].c_str(), nscope->ostr);
	 return 0;
      }
   return (QoreNamespace *)sns;
}

// qore_ns_private::parseMatchNamespace()
// will only be called if there is a match with the name and nscope->size() > 1
QoreNamespace *qore_ns_private::parseMatchNamespace(const NamedScope *nscope, unsigned *matched) const {
   printd(5, "qore_ns_private::parseMatchNamespace() this=%p ns=%p '%s' ns=%s matched=%d\n", this, ns, name.c_str(), nscope->ostr, *matched);

   // see if starting name matches this namespace
   if (nscope->strlist[0] == name) {
      const QoreNamespace *fns = ns;

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // check for a match of the structure in this namespace
      for (unsigned i = 1; i < (nscope->size() - 1); i++) {
	 fns = fns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str());
	 if (!fns)
	    break;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
      return (QoreNamespace *)fns;
   }

   const QoreNamespace *fns = nsl->find(nscope->strlist[0]);
   if (!fns)
      fns = pendNSL->find(nscope->strlist[0]);

   return fns ? fns->priv->parseMatchNamespace(nscope, matched) : 0;
}

QoreClass *qore_ns_private::parseMatchScopedClass(const NamedScope *nscope, unsigned *matched) {
   printd(5, "qore_ns_private::parseMatchScopedClass() this=%p ns=%p '%s' nscope='%s' matched=%d\n", ns, name.c_str(), nscope->ostr, *matched);

   if (nscope->strlist[0] != name) {
      QoreNamespace *fns = nsl->find(nscope->strlist[0]);
      if (!fns)
         fns = pendNSL->find(nscope->strlist[0]);      
      return fns ? fns->priv->parseMatchScopedClass(nscope, matched) : 0;
   }

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   printd(5, "qore_ns_private::parseMatchScopedClass() matched %s in %s\n", name.c_str(), nscope->ostr);

   QoreNamespace *fns = ns;
   
   // if we need to follow the namespaces, then do so
   if (nscope->size() > 2) {
      for (unsigned i = 1; i < (nscope->size() - 1); i++) {
	 fns = fns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str());
	 if (!fns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }
   QoreClass *rv = fns->priv->findLoadClass(ns, nscope->strlist[nscope->size() - 1].c_str());
   if (!rv)
      rv = fns->priv->pendClassList->find(nscope->strlist[nscope->size() - 1].c_str());
   return rv;
}

QoreClass *qore_ns_private::parseMatchScopedClassWithMethod(const NamedScope *nscope, unsigned *matched) {
   printd(5, "qore_ns_private::parseMatchScopedClassWithMethod() this=%p ns=%p '%s' class=%s (%s)\n", this, ns, name.c_str(), nscope->strlist[nscope->size() - 2].c_str(), nscope->ostr);

   QoreNamespace* fns = ns;
   // if we need to follow the namespaces, then do so
   if (nscope->size() > 2) {
      // if first namespace doesn't match, then return 0
      if (nscope->strlist[0] != name) {
         fns = nsl->find(nscope->strlist[0]);
         if (!fns)
            fns = pendNSL->find(nscope->strlist[0]);
	 return fns ? fns->priv->parseMatchScopedClassWithMethod(nscope, matched) : 0;
      }

      // mark first namespace as matched
      if (!(*matched))
	 *matched = 1;

      // otherwise search the rest of the namespaces
      for (unsigned i = 1; i < (nscope->size() - 2); i++) {	 
	 fns = fns->priv->parseFindLocalNamespace(nscope->strlist[i].c_str());
	 if (!fns)
	    return 0;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }

   // check last namespaces
   QoreClass *rv = fns->priv->findLoadClass(ns, nscope->strlist[nscope->size() - 2].c_str());
   if (!rv)
      rv = fns->priv->pendClassList->find(nscope->strlist[nscope->size() - 2].c_str());

   return rv;
}

AbstractQoreNode *qore_ns_private::parseMatchScopedConstantValue(const NamedScope *nscope, unsigned *matched, const QoreTypeInfo *&typeInfo) const {
   printd(5, "qore_ns_private::parseMatchScopedConstantValue) trying to find %s in %s (%p) typeInfo=%p\n", 
	  nscope->getIdentifier(), name.c_str(), getConstantValue(nscope->getIdentifier(), typeInfo));

   if (nscope->strlist[0] != name) {
      // check for a class constant
      if (nscope->size() == 2) {
	 QoreClass *qc = parseFindLocalClass(nscope->strlist[0].c_str());
         AbstractQoreNode *rv = qc ? qore_class_private::parseFindLocalConstantValue(qc, nscope->getIdentifier(), typeInfo) : 0;
         if (rv)
            return rv;
      }
      const QoreNamespace *fns = nsl->find(nscope->strlist[0]);
      if (!fns)
         fns = pendNSL->find(nscope->strlist[0]);
      return fns ? fns->priv->parseMatchScopedConstantValue(nscope, matched, typeInfo) : 0;
   }

   // mark first namespace as matched
   if (!(*matched))
      *matched = 1;

   const QoreNamespace *fns = ns;

   // if we need to follow the namespaces, then do so
   if (nscope->size() > 2) {
      unsigned last = nscope->size() - 1;
      for (unsigned i = 1; i < last; i++) {
	 const char *oname = nscope->strlist[i].c_str();
         const QoreNamespace *nns = fns->priv->parseFindLocalNamespace(oname);
	 if (!nns) {
	    // if we are on the last element before the constant in the namespace path list,
	    // then check for a class constant
	    if (i == (last - 1)) {
	       QoreClass *qc = fns->priv->parseFindLocalClass(oname);
	       return qc ? qore_class_private::parseFindLocalConstantValue(qc, nscope->getIdentifier(), typeInfo) : 0;
	    }
	    return 0;
	 }
         fns = nns;
	 if (i >= (*matched))
	    (*matched) = i + 1;
      }
   }

   return fns->priv->getConstantValue(nscope->getIdentifier(), typeInfo);
}

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
            QoreClass *qc = pns->priv->parseFindLocalClass(cname);
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
      priv->purge();
   }
}
