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
#include <qore/intern/qore_program_private.h>

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

// functions
#include <qore/intern/ql_time.h>
#include <qore/intern/ql_lib.h>
#include <qore/intern/ql_math.h>
#include <qore/intern/ql_type.h>
#include <qore/intern/ql_env.h>
#include <qore/intern/ql_string.h>
#include <qore/intern/ql_pwd.h>
#include <qore/intern/ql_misc.h>
#include <qore/intern/ql_list.h>
#include <qore/intern/ql_thread.h>
#include <qore/intern/ql_crypto.h>
#include <qore/intern/ql_object.h>
#include <qore/intern/ql_file.h>
#include <qore/intern/ql_compression.h>

#ifdef DEBUG
#include <qore/intern/ql_debug.h>
#endif // DEBUG

#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include <assert.h>

#include <memory>

#ifdef DEBUG_TESTS
// the #include "test/Namespace_tests.cpp" is on the bottom
#  include "tests/builtin_inheritance_tests.cpp"
#endif

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

StaticSystemNamespace staticSystemNamespace;

AutoNamespaceList ANSL;

DLLLOCAL void init_context_functions(QoreNamespace& ns);

QoreNamespace::QoreNamespace(const char* n) : priv(new qore_ns_private(this, n)) {
}

QoreNamespace::QoreNamespace(qore_ns_private* p) : priv(p) {
   p->ns = this;
}

QoreNamespace::~QoreNamespace() {
   //QORE_TRACE("QoreNamespace::~QoreNamespace()");
   delete priv;
}

const char* QoreNamespace::getName() const {
   return priv->name.c_str();
}

void QoreNamespace::setClassHandler(q_ns_class_handler_t class_handler) {
   priv->class_handler = class_handler;
}

// public, only called in single-threaded initialization
void QoreNamespace::addSystemClass(QoreClass* oc) {
   QORE_TRACE("QoreNamespace::addSystemClass()");

#ifdef DEBUG
   if (priv->classList.add(oc))
      assert(false);
#else
   priv->classList.add(oc);
#endif

   // see if namespace is attached to the root
   qore_root_ns_private* rns = priv->getRoot();
   if (!rns)
      return;

   rns->clmap.update(oc->getName(), priv, oc);
}

void QoreNamespace::addNamespace(QoreNamespace* ns) {
   priv->addNamespace(ns->priv);
}

void QoreNamespace::addInitialNamespace(QoreNamespace* ns) {
   priv->addNamespace(ns->priv);
}

void qore_ns_private::addModuleNamespace(qore_ns_private* nns, QoreModuleContext& qmc) {
   if (nsl.find(nns->name)) {
      std::string path;
      getPath(path, true);
      qmc.error("namespace '%s' already exists in '%s'", nns->name.c_str(), path.c_str());
      return;
   }

   if (pendNSL.find(nns->name)) {
      std::string path;
      getPath(path, true);
      qmc.error("namespace '%s' is already pending in '%s'", nns->name.c_str(), path.c_str());
      return;
   }

   if (classList.find(nns->name.c_str())) {
      std::string path;
      getPath(path, true);
      qmc.error("a class with the same name as the namespace ('%s') already exists in '%s'", nns->name.c_str(), path.c_str());
      return;
   }

   if (pendClassList.find(nns->name.c_str())) {
      std::string path;
      getPath(path, true);
      qmc.error("a class with the same name as the namespace ('%s') is already pending in '%s'", nns->name.c_str(), path.c_str());
      return;
   }

   qmc.mcnl.push_back(ModuleContextNamespaceCommit(this, nns));
}

void qore_ns_private::addCommitNamespaceIntern(qore_ns_private* nns) {
   assert(!classList.find(nns->name.c_str()));
   assert(!pendClassList.find(nns->name.c_str()));

   nsl.add(nns->ns, this);

   // see if namespace is attached to the root
   qore_root_ns_private* rns = getRoot();
   if (!rns)
      return;

   // rebuild indexes for objects in new namespace tree
   QorePrivateNamespaceIterator qpni(nns, true);
   while (qpni.next())
      rns->rebuildIndexes(qpni.get());
}

void qore_ns_private::addNamespace(qore_ns_private* nns) {
   QoreModuleContext* qmc = get_module_context();
   if (qmc)
      addModuleNamespace(nns, *qmc);
   else
      addCommitNamespaceIntern(nns);
}

void QoreNamespaceList::deleteAll() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      delete i->second;
   nsmap.clear();
}

void qore_ns_private::updateDepthRecursive(unsigned ndepth) {
   //printd(5, "qore_ns_private::updateDepthRecursive(ndepth: %d) this: %p '%s' curr depth: %d\n", ndepth, this, name.c_str(), depth);
   assert(depth <= ndepth);
   assert(!ndepth || !name.empty());

   if (depth < ndepth) {
      depth = ndepth;

      for (nsmap_t::iterator i = nsl.nsmap.begin(), e = nsl.nsmap.end(); i != e; ++i)
         i->second->priv->updateDepthRecursive(ndepth + 1);

      for (nsmap_t::iterator i = pendNSL.nsmap.begin(), e = pendNSL.nsmap.end(); i != e; ++i)
         i->second->priv->updateDepthRecursive(ndepth + 1);
   }
}

void qore_ns_private::addBuiltinModuleVariant(const char* fname, AbstractQoreFunctionVariant* v, QoreModuleContext& qmc) {
   SimpleRefHolder<AbstractQoreFunctionVariant> vh(v);

   FunctionEntry* fe = func_list.findNode(fname);

   if (fe)
      qmc.error("function '%s()' has already been declared in namespace '%s'", fname, name.c_str());
   else
      qmc.mcfl.push_back(ModuleContextFunctionCommit(this, fname, v));
}

void qore_ns_private::addBuiltinVariant(const char* fname, AbstractQoreFunctionVariant* v) {
   QoreModuleContext* qmc = get_module_context();
   if (qmc)
      addBuiltinModuleVariant(fname, v, *qmc);
   else
      addBuiltinVariantIntern(fname, v);
}

void qore_ns_private::addBuiltinVariantIntern(const char* fname, AbstractQoreFunctionVariant* v) {
   SimpleRefHolder<AbstractQoreFunctionVariant> vh(v);

   FunctionEntry* fe = func_list.findNode(fname);

   if (fe) {
      fe->getFunction()->addBuiltinVariant(vh.release());
      return;
   }

   QoreFunction* u = new QoreFunction(fname);
   u->addBuiltinVariant(vh.release());
   fe = func_list.add(u);

   // add to root function map if attached
   qore_root_ns_private* rns = getRoot();
   if (!rns)
      return;
   
   rns->fmap.update(fe->getName(), this, fe);
}

void QoreNamespaceList::assimilate(QoreNamespaceList& n, qore_ns_private* parent) {
   for (nsmap_t::iterator i = n.nsmap.begin(), e = n.nsmap.end(); i != e; ++i) {
      assert(nsmap.find(i->first) == nsmap.end());
      nsmap[i->first] = i->second;
      if (parent) {
         i->second->priv->parent = parent;
         i->second->priv->updateDepthRecursive(parent->depth + 1);
      }
   }
   n.nsmap.clear();
}

void QoreNamespaceList::reset() {
   deleteAll();
}

qore_ns_private* QoreNamespaceList::add(QoreNamespace* ns, qore_ns_private* parent) {
   // if namespace is already registered, then assimilate
   QoreNamespace* ons;
   if ((ons = find(ns->priv->name.c_str()))) {
      //printd(5, "QoreNamespaceList::add() this=%p ns=%p (%s) assimilating with ons=%p (%s)\n", this, ns, ns->getName(), ons, ons->getName());
      ons->priv->assimilate(ns);
      return ons->priv;
   }
   nsmap[ns->priv->name] = ns;
   ns->priv->parent = parent;
   ns->priv->updateDepthRecursive(parent->depth + 1);
   return ns->priv;
}

QoreNamespace* QoreNamespace::copy(int po) const {
   //printd(5, "QoreNamespace::copy() (deprecated) this=%p po=%d %s\n", this, po, priv->name.c_str());
   return qore_ns_private::newNamespace(*priv, po);
}

QoreNamespace* QoreNamespace::copy(int64 po) const {
   //printd(5, "QoreNamespace::copy() this=%p po=%lld %s\n", this, po, priv->name.c_str());
   return qore_ns_private::newNamespace(*priv, po);
}

QoreNamespaceList::QoreNamespaceList(const QoreNamespaceList& old, int64 po, const qore_ns_private& parent) {
   //printd(5, "QoreNamespaceList::QoreNamespaceList(old=%p) this=%p po=%lld size=%d\n", &old, this, po, nsmap.size());
   nsmap_t::iterator last = nsmap.begin();
   for (nsmap_t::const_iterator i = old.nsmap.begin(), e = old.nsmap.end(); i != e; ++i) {
      QoreNamespace* ns = i->second->copy(po);
      ns->priv->parent = &parent;
      assert(ns->priv->depth);
      last = nsmap.insert(last, nsmap_t::value_type(i->first, ns));
   }
}

void QoreNamespaceList::resolveCopy() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->classList.resolveCopy();
}

void QoreNamespaceList::parseInitGlobalVars(int64 po) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->parseInitGlobalVars(po);
}

void QoreNamespaceList::clearGlobalVars(ExceptionSink* xsink) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->clearGlobalVars(xsink);
}

void QoreNamespaceList::deleteGlobalVars(ExceptionSink* xsink) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->deleteGlobalVars(xsink);
}

void QoreNamespaceList::parseInitConstants() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->parseInitConstants();
}

void QoreNamespaceList::deleteAllConstants(ExceptionSink *xsink) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->constant.deleteAll(xsink);
}

void QoreNamespaceList::parseInit() {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->priv->parseInit();
}

void QoreNamespaceList::parseCommit(QoreNamespaceList& l) {
   assimilate(l, 0);

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
void QoreNamespace::addConstant(const char* cname, AbstractQoreNode* val) {
   return addConstant(cname, val, 0);
}

void QoreNamespace::addConstant(const char* cname, AbstractQoreNode* val, const QoreTypeInfo* typeInfo) {
   // see if namespace is attached to the root
   qore_root_ns_private* rns = priv->getRoot();
   if (!rns) {
      priv->constant.add(cname, val, typeInfo);
      return;
   }

   qore_root_ns_private::addConstant(*rns, *priv, cname, val, typeInfo);
}

QoreNamespace* QoreNamespace::findCreateNamespacePath(const char* nspath) {
   return priv->findCreateNamespacePath(nspath);
}

QoreNamespace* qore_ns_private::findCreateNamespace(const char* nsn) {
   QoreNamespace* ns = nsl.find(nsn);
   if (!ns)
      nsl.add((ns = new QoreNamespace(nsn)), this);
   return ns;
}

QoreNamespace* qore_ns_private::findCreateNamespacePath(const char* nspath) {
   assert(nspath);
   NamedScope nscope(nspath);

   // iterate through each level of the namespace path and find/create namespaces as needed
   QoreNamespace* nns = ns;
   for (unsigned i = 0; i < nscope.size(); ++i)
      nns = nns->priv->findCreateNamespace(nscope[i]);

   return nns;
}

QoreClass* QoreNamespace::findLocalClass(const char* cname) const {
   return priv->classList.find(cname);
}

QoreNamespace* QoreNamespace::findLocalNamespace(const char* cname) const {
   return priv->nsl.find(cname);
}

const QoreNamespace* QoreNamespace::getParent() const {
   return priv->parent ? priv->parent->ns : 0;
}

void QoreNamespace::deleteData(ExceptionSink *xsink) {
   // clear all constants
   priv->constant.deleteAll(xsink);
   // clear all constants and static class vars
   priv->classList.deleteClassData(xsink);
   // clear all user functions
   priv->func_list.del();

   // repeat for all subnamespaces
   priv->nsl.deleteData(xsink);
}

void QoreNamespaceList::deleteData(ExceptionSink *xsink) {
   for (nsmap_t::iterator i = nsmap.begin(), e = nsmap.end(); i != e; ++i)
      i->second->deleteData(xsink);
}

/*
static void showNSL(QoreNamespaceList *nsl) {
   printd(5, "showNSL() dumping %p\n", nsl);
   for (int i = 0; i < nsl.num_namespaces; i++)
      printd(5, "showNSL()  %d: %p %s (list: %p)\n", i, nsl.nslist[i], nsl.nslist[i]->name, nsl.nslist[i]->nsl);
}
*/

QoreHashNode* QoreNamespace::getConstantInfo() const {
   return priv->constant.getInfo();
}

QoreHashNode* QoreNamespace::getClassInfo() const {
   return priv->classList.getInfo();
}

// returns a hash of namespace information
QoreHashNode* QoreNamespace::getInfo() const {
   QoreHashNode* h = new QoreHashNode();

   h->setKeyValue("constants", getConstantInfo(), 0);
   h->setKeyValue("classes", getClassInfo(), 0);

   if (!priv->nsl.nsmap.empty()) {
      QoreHashNode* nsh = new QoreHashNode;

      for (nsmap_t::iterator i = priv->nsl.nsmap.begin(), e = priv->nsl.nsmap.end(); i != e; ++i)
	 nsh->setKeyValue(i->second->priv->name.c_str(), i->second->getInfo(), 0);

      h->setKeyValue("subnamespaces", nsh, 0);
   }

   return h;
}

void QoreNamespace::addBuiltinVariant(const char* name, q_func_t f, int64 code_flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   priv->addBuiltinVariant<q_func_t, BuiltinFunctionVariant>(name, f, code_flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void QoreNamespace::addBuiltinVariant(const char* name, q_func_int64_t f, int64 code_flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   priv->addBuiltinVariant<q_func_int64_t, BuiltinFunctionBigIntVariant>(name, f, code_flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void QoreNamespace::addBuiltinVariant(const char* name, q_func_double_t f, int64 code_flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   priv->addBuiltinVariant<q_func_double_t, BuiltinFunctionFloatVariant>(name, f, code_flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void QoreNamespace::addBuiltinVariant(const char* name, q_func_bool_t f, int64 code_flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   priv->addBuiltinVariant<q_func_bool_t, BuiltinFunctionBoolVariant>(name, f, code_flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

RootQoreNamespace::RootQoreNamespace(qore_root_ns_private* p) : QoreNamespace(p), rpriv(p) {
   p->rns = this;
}

RootQoreNamespace::~RootQoreNamespace() {
   delete rpriv;
   // make sure priv is not deleted (again)
   priv = 0;
}

QoreNamespace* RootQoreNamespace::rootGetQoreNamespace() const {
   return rpriv->qoreNS;
}

StaticSystemNamespace::StaticSystemNamespace() : RootQoreNamespace(new qore_root_ns_private(this)) {
}

// sets up the root namespace
void StaticSystemNamespace::init() {
   QORE_TRACE("StaticSystemNamespace::init()");

   rpriv->qoreNS = new QoreNamespace("Qore");
   QoreNamespace& qns = *rpriv->qoreNS;

   qore_ns_private::addNamespace(qns, get_thread_ns(qns));

   // add system object types
   qns.addSystemClass(initTimeZoneClass(qns));
   qns.addSystemClass(initSSLCertificateClass(qns));
   qns.addSystemClass(initSSLPrivateKeyClass(qns));
   qns.addSystemClass(initSocketClass(qns));
   qns.addSystemClass(initProgramClass(qns));

   qns.addSystemClass(initTermIOSClass(qns));
   qns.addSystemClass(initFileClass(qns));
   qns.addSystemClass(initDirClass(qns));
   qns.addSystemClass(initGetOptClass(qns));
   qns.addSystemClass(initFtpClientClass(qns));

   // add HTTPClient namespace
   qns.addSystemClass(initHTTPClientClass(qns));

#ifdef DEBUG_TESTS
   { // tests
      QoreClass* base = initBuiltinInheritanceTestBaseClass();
      qns.addSystemClass(base);
      qns.addSystemClass(initBuiltinInheritanceTestDescendant1(base));
      // hierarchy with 3 levels
      QoreClass* desc2 = initBuiltinInheritanceTestDescendant2(base);
      qns.addSystemClass(desc2);
      QoreClass* desc3 = initBuiltinInheritanceTestDescendant3(desc2);
      qns.addSystemClass(desc3);
// BUGBUG : this fails. When desc2 is placed in the next line all is OK
      QoreClass* desc4 = initBuiltinInheritanceTestDescendant4(desc3);
      qns.addSystemClass(desc4);

      QoreClass* base2 = initBuiltinInheritanceTestBase2Class();
      qns.addSystemClass(base2);
// BUGBUG - the function actually fails to deal with two base classes, see the 
// code in tests/builtin_inheritance_tests.cpp
      QoreClass* desc_multi = initBuiltinInheritanceTestDescendantMulti(base2, base);
      qns.addSystemClass(desc_multi);
   }
#endif

   init_qore_constants(qns);

   // set up Option namespace for Qore options
   QoreNamespace* option = new QoreNamespace("Option");
   init_option_constants(*option);
   qore_ns_private::addNamespace(qns, option);

   // create Qore::SQL namespace
   QoreNamespace* sqlns = new QoreNamespace("SQL");

   sqlns->addSystemClass(initDatasourceClass(*sqlns));
   sqlns->addSystemClass(initDatasourcePoolClass(*sqlns));
   sqlns->addSystemClass(initSQLStatementClass(*sqlns));

   init_dbi_functions(*sqlns);
   init_dbi_constants(*sqlns);
   qore_ns_private::addNamespace(qns, sqlns);

   // create get Qore::Err namespace with ERRNO constants
   QoreNamespace* Err = new QoreNamespace("Err");
   init_errno_constants(*Err);
   qore_ns_private::addNamespace(qns, Err);
   
   QoreNamespace* tns = new QoreNamespace("Type");
   init_type_constants(*tns);
   qore_ns_private::addNamespace(qns, tns);

   init_type_constants(qns);
   init_compression_constants(qns);
   init_crypto_constants(qns);
   init_misc_constants(qns);
   init_string_constants(qns);
   init_math_constants(qns);

   init_string_functions(qns);
   init_time_functions(qns);
   init_lib_functions(qns);
   init_misc_functions(qns);
   init_list_functions(qns);
   init_type_functions(qns);
   init_pwd_functions(qns);
   init_math_functions(qns);
   init_env_functions(qns);
   init_thread_functions(qns);
   init_crypto_functions(qns);
   init_object_functions(qns);
   init_file_functions(qns);
   init_compression_functions(qns);
   init_context_functions(qns);
#ifdef DEBUG
   init_debug_functions(qns);
#endif

   qore_ns_private::addNamespace(*this, rpriv->qoreNS);

   // add all changes in loaded modules
   ANSL.init(this, rpriv->qoreNS);
}

#ifdef DEBUG_TESTS
// moved down to allow to test internal classes
#  include "tests/Namespace_tests.cpp"
#endif

// returns 0 for success, non-zero return value means error
int qore_root_ns_private::parseAddMethodToClassIntern(const NamedScope& scname, MethodVariantBase *qcmethod, bool static_flag) {
   std::auto_ptr<MethodVariantBase> v(qcmethod);

   // find class
   QoreClass* oc = parseFindScopedClassWithMethodInternError(scname, true);
   if (!oc)
      return -1;

   return oc->addUserMethod(scname.getIdentifier(), v.release(), static_flag);
}

// returns 0 for success, non-zero for error
AbstractQoreNode* qore_root_ns_private::parseResolveBarewordIntern(const char* bword, const QoreTypeInfo *&typeInfo) {
   QoreClass* pc = getParseClass();

   //printd(5, "qore_root_ns_private::parseResolveBarewordIntern(%s) pc=%p (%s)\n", b->str, pc, pc ? pc->getName() : "<none>");

   QoreProgram *pgm = getProgram();
   bool abr = (bool)(pgm->getParseOptions64() & PO_ALLOW_BARE_REFS);

   // if bare refs are enabled, first look for a local variablee
   if (abr) {
      bool in_closure;
      LocalVar *id = find_local_var(bword, in_closure);
      if (id) {
         typeInfo = id->getTypeInfo();
         return new VarRefNode(strdup(bword), id, in_closure);
      }
   }

   // if there is a current parse class context, then check for class objects
   if (pc) {
      // if bare refs are enabled, check for member reference first
      if (abr && !qore_class_private::parseResolveInternalMemberAccess(pc, bword, typeInfo))
         return new SelfVarrefNode(strdup(bword));

      // now try to find a class constant with this name
      AbstractQoreNode* rv = qore_class_private::parseFindConstantValue(pc, bword, typeInfo);
      if (rv)
         return rv->refSelf();

      // now check for class static var reference
      const QoreClass* qc = 0;
      QoreVarInfo *vi = qore_class_private::parseFindStaticVar(pc, bword, qc, typeInfo);
      if (vi) {
         assert(qc);
         return new StaticClassVarRefNode(bword, *qc, *vi);
      }
   }

   // try to resolve a global variable
   if (abr) {
      Var* v = parseFindGlobalVar(bword);
      if (v)
         return new GlobalVarRefNode(strdup(bword), v);
   }

   // try to resolve constant
   // first try to find a namespace context
   AbstractQoreNode* rv;

   qore_ns_private* nscx = parse_get_ns();
   //printd(5, "qore_root_ns_private::parseResolveBarewordIntern() bword: %s nscx: %p (%s)\n", bword, nscx, nscx ? nscx->name.c_str() : "n/a");
   if (nscx && (rv = nscx->getConstantValue(bword, typeInfo))) {
      //printd(5, "qore_root_ns_private::parseResolveBarewordIntern() bword: %s nscx: %p (%s) got rv: %p (%s)\n", bword, nscx, nscx ? nscx->name.c_str() : "n/a", rv, get_type_name(rv));
      return rv->refSelf();
   }

   rv = parseFindOnlyConstantValueIntern(bword, typeInfo);

   if (rv)
      return rv->refSelf();

   parse_error("cannot resolve bareword '%s' to any reachable object", bword);

   //printd(5, "qore_root_ns_private::parseResolveBarewordIntern(%s) %p %s\n", bword, rv, get_type_name(rv));
   return 0;
}

AbstractQoreNode *qore_root_ns_private::parseResolveScopedReferenceIntern(const NamedScope& nscope, const QoreTypeInfo *&typeInfo) {
   assert(nscope.size() > 1);

   unsigned m = 0;
   AbstractQoreNode* rv = 0;

   bool abr = (bool)(getParseOptions() & PO_ALLOW_BARE_REFS);

   {
      // try to check in current namespace first
      qore_ns_private* nscx = parse_get_ns();
      //printd(5, "qore_root_ns_private::parseResolveScopedReferenceIntern(%s) ns: %p (%s)\n", nscope.ostr, nscx, nscx ? nscx->name.c_str() : "n/a");
      if (nscx) {
         QoreNamespace* ns = nscx->parseFindLocalNamespace(nscope[0]);
         if (ns && (rv = ns->priv->parseCheckScopedReference(nscope, m, typeInfo, abr)))
            return rv;
      }
   }

   // iterate all namespaces with the initial name and look for the match
   {
      NamespaceMapIterator nmi(nsmap, nscope[0]);
      while (nmi.next()) {
         //printd(5, "qore_root_ns_private::parseResolveScopedReferenceIntern(%s) ns: %p (%s)\n", nscope.ostr, nmi.get(), nmi.get()->name.c_str());
         if ((rv = nmi.get()->parseCheckScopedReference(nscope, m, typeInfo, abr)))
            return rv;
      }
   }

   {
      NamespaceMapIterator nmi(pend_nsmap, nscope[0]);
      while (nmi.next()) {
         //printd(5, "qore_root_ns_private::parseResolveScopedReferenceIntern(%s) ns: %p (%s)\n", nscope.ostr, nmi.get(), nmi.get()->name.c_str());
         if ((rv = nmi.get()->parseCheckScopedReference(nscope, m, typeInfo, abr)))
            return rv;
      }
   }

   // now look for class constants if there is only a single namespace or class name in the beginning
   if (nscope.size() == 2) {
      QoreClass* qc = parseFindClassIntern(nscope[0], false);
      if (qc) {
         rv = parseResolveClassConstant(qc, nscope.getIdentifier(), typeInfo);
         if (rv)
            return rv;
      }
   }

   // raise parse exception
   if (m != (nscope.size() - 1))
      parse_error("cannot find any namespace or class '%s' in '%s' providing a constant or static class variable '%s'", nscope[m], nscope.ostr, nscope.getIdentifier());
   else {
      QoreString err;
      err.sprintf("cannot resolve bareword '%s' to any reachable object in any namespace '", nscope.getIdentifier());
      for (unsigned i = 0; i < (nscope.size() - 1); i++) {
         err.concat(nscope[i]);
         if (i != (nscope.size() - 2))
            err.concat("::");
      }
      err.concat("'");
      parse_error(err.getBuffer());
   }

   //printd(5, "RootQoreNamespace::resolveScopedReference(%s) not found\n", nscope.ostr);
   return 0;
}

// private
QoreClass* qore_root_ns_private::parseFindScopedClassWithMethodIntern(const NamedScope& nscope, unsigned& matched) {
   assert(nscope.size() > 2);

   QoreClass* oc;

   // iterate all namespaces with the initial name and look for the match
   {
      NamespaceMapIterator nmi(nsmap, nscope[0]);
      while (nmi.next()) {
         if ((oc = nmi.get()->parseMatchScopedClassWithMethod(nscope, matched)))
            return oc;
      }
   }

   {
      NamespaceMapIterator nmi(pend_nsmap, nscope[0]);
      while (nmi.next()) {
         if ((oc = nmi.get()->parseMatchScopedClassWithMethod(nscope, matched)))
            return oc;
      }
   }

   return 0;
}

QoreClass* qore_root_ns_private::parseFindScopedClassIntern(const NamedScope& nscope, unsigned& matched) {
   assert(nscope.size() > 1);

   // iterate all namespaces with the initial name and look for the match
   {
      NamespaceMapIterator nmi(nsmap, nscope[0]);
      while (nmi.next()) {
         QoreClass* oc;
         //printd(5, "qore_root_ns_private::parseResolveScopedReferenceIntern(%s) ns: %p (%s)\n", nscope.ostr, nmi.get(), nmi.get()->name.c_str());
         if ((oc = nmi.get()->parseMatchScopedClass(nscope, matched)))
            return oc;
      }
   }

   {
      NamespaceMapIterator nmi(pend_nsmap, nscope[0]);
      while (nmi.next()) {
         QoreClass* oc;
         //printd(5, "qore_root_ns_private::parseResolveScopedReferenceIntern(%s) ns: %p (%s)\n", nscope.ostr, nmi.get(), nmi.get()->name.c_str());
         if ((oc = nmi.get()->parseMatchScopedClass(nscope, matched)))
            return oc;
      }
   }

   return 0;
}

QoreClass* qore_root_ns_private::parseFindScopedClassIntern(const NamedScope& nscope) {
   QoreClass* oc;
   // if there is no namespace specified, then just find class
   if (nscope.size() == 1)
      return parseFindClassIntern(nscope.getIdentifier(), true);

   unsigned m = 0;
   oc = parseFindScopedClassIntern(nscope, m);

   if (!oc) {
      if (m != (nscope.size() - 1))
         parse_error("cannot resolve namespace '%s' in '%s'", nscope[m], nscope.ostr);
      else {
         QoreString err;
         err.sprintf("cannot find class '%s' in any namespace '", nscope.getIdentifier());
         for (unsigned i = 0; i < (nscope.size() - 1); i++) {
            err.concat(nscope[i]);
            if (i != (nscope.size() - 2))
               err.concat("::");
         }
         err.concat("'");
         parse_error(err.getBuffer());
      }
   }

   printd(5, "qore_root_ns_private::parseFindScopedClassIntern('%s') returning %p\n", nscope.ostr, oc);
   return oc;
}

QoreClass* qore_root_ns_private::parseFindScopedClassWithMethodInternError(const NamedScope& scname, bool error) {
   // must have at least 2 elements
   assert(scname.size() > 1);

   QoreClass* oc;

   if (scname.size() == 2) {
      oc = parseFindClassIntern(scname[0], false);
      if (oc)
         return oc;

      if (error)
         parse_error("reference to undefined class '%s' in '%s()'", scname[0], scname.ostr);
      return 0;
   }

   unsigned m = 0;
   oc = parseFindScopedClassWithMethodIntern(scname, m);
   if (!oc && error) {
      if (m >= (scname.size() - 2))
         parse_error("cannot resolve class '%s' in '%s()'", scname[m], scname.ostr);
      else  {	 
         QoreString err;
         err.sprintf("cannot find class '%s' in any namespace '", scname[scname.size() - 2]);
         for (unsigned i = 0; i < (scname.size() - 2); i++) {
            err.concat(scname.strlist[i].c_str());
            if (i != (scname.size() - 3))
               err.concat("::");
         }
         err.concat("'");
         parse_error(err.getBuffer());
      }
   }

   printd(5, "qore_ns_private::parseFindScopedClassWithMethodIntern('%s') returning %p\n", scname.ostr, oc);
   return oc;
}

// called in 2nd stage of parsing to resolve constant references
AbstractQoreNode* qore_root_ns_private::parseFindConstantValueIntern(const NamedScope& scname, const QoreTypeInfo *&typeInfo, bool error) {
   if (scname.size() == 1)
      return parseFindConstantValueIntern(scname.ostr, typeInfo, error);

   AbstractQoreNode* rv;
   unsigned m = 0;

   // iterate all namespaces with the initial name and look for the match
   {
      NamespaceMapIterator nmi(nsmap, scname[0]);
      while (nmi.next()) {
         //printd(5, "qore_root_ns_private::parseResolveScopedReferenceIntern(%s) ns: %p (%s)\n", nscope.ostr, nmi.get(), nmi.get()->name.c_str());
         if ((rv = nmi.get()->parseMatchScopedConstantValue(scname, m, typeInfo)))
            return rv;
      }
   }

   {
      NamespaceMapIterator nmi(pend_nsmap, scname[0]);
      while (nmi.next()) {
         //printd(5, "qore_root_ns_private::parseResolveScopedReferenceIntern(%s) ns: %p (%s)\n", nscope.ostr, nmi.get(), nmi.get()->name.c_str());
         if ((rv = nmi.get()->parseMatchScopedConstantValue(scname, m, typeInfo)))
            return rv;
      }
   }

   // look for a class constant if there are only 2 elements in the scope list
   if (scname.size() == 2) {
      QoreClass* qc = parseFindClassIntern(scname[0], false);
      if (qc) {
         rv = parseResolveClassConstant(qc, scname.getIdentifier(), typeInfo);
         if (rv)
            return rv;
      }
   }

   if (!error)
      return 0;

   if (m != (scname.size() - 1))
      parse_error("cannot resolve namespace '%s' in constant reference '%s'", scname[m], scname.ostr);
   else {
      QoreString err;
      err.sprintf("cannot find constant '%s' in any namespace '", scname.getIdentifier());
      for (unsigned i = 0; i < (scname.size() - 1); i++) {
	 err.concat(scname[i]);
	 if (i != (scname.size() - 2))
	    err.concat("::");
      }
      err.concat("'");
      parse_error(err.getBuffer());
   }

   return 0;
}

// only called with RootNS
void qore_root_ns_private::parseAddClassIntern(const NamedScope& nscope, QoreClass* oc) {
   QORE_TRACE("qore_root_ns_private::parseAddClassIntern()");

   QoreNamespace* sns = parseResolveNamespace(nscope);

   if (sns) {
      //printd(5, "qore_root_ns_private::parseAddClassIntern() '%s' adding %s:%p to %s:%p\n", nscope.ostr, oc->getName(), oc, sns->priv->name.c_str(), sns);
      // add to pending class map if add was successful
      if (!sns->priv->parseAddPendingClass(oc))
         pend_clmap.update(oc->getName(), sns->priv, oc);
   }
   else {
      //printd(5, "qore_root_ns_private::parseAddClassIntern() class '%s' not added: '%s' namespace not found\n", oc->getName(), nscope.ostr);
      delete oc;
   }
}

void qore_root_ns_private::addConstant(qore_ns_private& ns, const char* cname, AbstractQoreNode *val, const QoreTypeInfo* typeInfo) {
   cnemap_t::iterator i = ns.constant.add(cname, val, typeInfo);
   if (i == ns.constant.end())
      return;

   cnmap.update(i->first, &ns, i->second);
}

void qore_root_ns_private::parseAddConstantIntern(QoreNamespace& ns, const NamedScope& name, AbstractQoreNode* value) {
   ReferenceHolder<> vh(value, 0);

   QoreNamespace* sns = ns.priv->resolveNameScope(name);
   if (!sns)
      return;

   const char* cname = name.strlist[name.size() - 1].c_str();
   cnemap_t::iterator i = sns->priv->parseAddConstant(cname, vh.release());
   if (i == sns->priv->pendConstant.end())
      return;

   pend_cnmap.update(i->first, sns->priv, i->second);
}

QoreNamespace* qore_root_ns_private::parseResolveNamespace(const NamedScope& nscope) {
   if (nscope.size() == 1)
      return ns;

   QoreNamespace* fns = 0;
   unsigned match = 0;

   // try to check in current namespace first
   qore_ns_private* nscx = parse_get_ns();
   if (nscx) {
      fns = nscx->parseFindLocalNamespace(nscope[0]);
      if (ns && (fns = ns->priv->parseMatchNamespace(nscope, match)))
         return fns;
   }

   // iterate all namespaces with the initial name and look for the match
   {
      NamespaceMapIterator nmi(nsmap, nscope[0]);
      while (nmi.next()) {
         if ((fns = nmi.get()->parseMatchNamespace(nscope, match)))
            return fns;
      }
   }

   {
      NamespaceMapIterator nmi(pend_nsmap, nscope[0]);
      while (nmi.next()) {
         if ((fns = nmi.get()->parseMatchNamespace(nscope, match)))
            return fns;
      }
   }

   parse_error("cannot resolve namespace '%s' in '%s'", nscope[match], nscope.ostr);
   return 0;
}

const QoreFunction* qore_root_ns_private::runtimeFindFunctionIntern(const NamedScope& name, const qore_ns_private*& ns) {
   assert(name.size() > 1);

   // iterate all namespaces with the initial name and look for the match
   const QoreFunction* f = 0;
   NamespaceMapIterator nmi(nsmap, name.strlist[0].c_str());
   while (nmi.next()) {
      if ((f = nmi.get()->runtimeMatchFunction(name, ns)))
         return f;
   }

   return 0;
}

const QoreFunction* qore_root_ns_private::parseResolveFunctionIntern(const NamedScope& nscope) {
   assert(nscope.size() > 1);

   const QoreFunction* f = 0;
   unsigned match = 0;

   {
      // try to check in current namespace first
      qore_ns_private* nscx = parse_get_ns();
      if (nscx) {
         QoreNamespace* ns = nscx->parseFindLocalNamespace(nscope[0]);
         if (ns && (f = ns->priv->parseMatchFunction(nscope, match)))
            return f;
      }
   }

   // iterate all namespaces with the initial name and look for the match
   {
      NamespaceMapIterator nmi(nsmap, nscope[0]);
      while (nmi.next()) {
         if ((f = nmi.get()->parseMatchFunction(nscope, match)))
            return f;
      }
   }

   {
      NamespaceMapIterator nmi(pend_nsmap, nscope[0]);
      while (nmi.next()) {
         if ((f = nmi.get()->parseMatchFunction(nscope, match)))
            return f;
      }
   }

   return 0;   
}

AbstractCallReferenceNode* qore_root_ns_private::parseResolveCallReferenceIntern(UnresolvedProgramCallReferenceNode* fr) {
   std::auto_ptr<UnresolvedProgramCallReferenceNode> fr_holder(fr);
   char* fname = fr->str;

   FunctionEntry* fe = parseFindFunctionEntryIntern(fname);
   if (fe) {
      // check parse options to see if access is allowed
      if (!qore_program_private::parseAddDomain(getProgram(), fe->getFunction()->getUniqueFunctionality())) 
         return fe->makeCallReference();
      parse_error("parse options do not allow access to function '%s'", fname);
   }
   else // cannot find function, throw exception
      parse_error("reference to function '%s()' cannot be resolved", fname);

   return fr_holder.release();
}

Var* qore_root_ns_private::parseAddResolvedGlobalVarDefIntern(const NamedScope& vname, const QoreTypeInfo *typeInfo) {
   bool new_var = false;

   qore_ns_private* tns;
   // assume the root namespace for all unscoped global variables
   if (vname.size() == 1)
      tns = this;
   else {
      QoreNamespace* rns = parseResolveNamespace(vname);
      if (rns)
         tns = rns->priv;
   }

   //printd(5, "qore_root_ns_private::parseAddResolvedGlobalVarDefIntern() this: %p vname: '%s' resolved ns to %p '%s::'\n", this, vname.ostr, tns, tns->name.c_str());

   Var* v = tns->var_list.parseFindCreateVar(vname.getIdentifier(), typeInfo, new_var);
   
   if (!new_var)
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_DUPLICATE_GLOBAL_VARS, "DUPLICATE-GLOBAL-VARIABLE", "global variable '%s' has already been declared", vname.ostr);
   else {
      if (checkParseOption(PO_NO_GLOBAL_VARS))
         parse_error("illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", vname.ostr);

      pend_varmap.update(v->getName(), tns, v);
      //printd(5, "qore_root_ns_private::parseAddResolvedGlobalVarDef() this: %p adding %p '%s' (%s) to '%s::'\n", this, v, vname.ostr, typeInfo->getName(), name.c_str());
   }
   
   return v;
}

Var *qore_root_ns_private::parseAddGlobalVarDefIntern(const NamedScope& vname, QoreParseTypeInfo *typeInfo) {
   bool new_var = false;

   qore_ns_private* tns;
   // assume the root namespace for all unscoped global variables
   if (vname.size() == 1)
      tns = this;
   else {
      QoreNamespace* rns = parseResolveNamespace(vname);
      if (rns)
         tns = rns->priv;
   }

   Var* v = tns->var_list.parseFindCreateVar(vname.getIdentifier(), typeInfo, new_var);

   if (!new_var)
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_DUPLICATE_GLOBAL_VARS, "DUPLICATE-GLOBAL-VARIABLE", "global variable '%s' has already been declared", vname.ostr);
   else {
      if (checkParseOption(PO_NO_GLOBAL_VARS))
         parse_error("illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", vname.ostr);

      pend_varmap.update(v->getName(), tns, v);
   }

   return v;
}

Var *qore_root_ns_private::parseCheckImplicitGlobalVarIntern(const NamedScope& vname, const QoreTypeInfo *typeInfo) {
   Var* rv;

   qore_ns_private* tns;
   // assume the root namespace for all unscoped global variables
   if (vname.size() == 1) {      
      rv = parseFindGlobalVarIntern(vname.ostr);
      tns = this;
   }
   else {
      QoreNamespace* rns = parseResolveNamespace(vname);
      tns = rns ? rns->priv : this;
      rv = tns->var_list.parseFindVar(vname.getIdentifier());
   }

   //printd(5, "qore_root_ns_private::parseCheckImplicitGlobalVar() this: %p '%s' rv: %p (omq: %p)\n", this, vname.ostr, rv, parseFindGlobalVarIntern("omq"));
   if (!rv) {
      // check for errors & warnings for implicit global variables
      QoreProgram* pgm = getProgram();
      int64 po = pgm->getParseOptions64();

      // check if unflagged global vars are allowed
      if (po & PO_REQUIRE_OUR)
	 parseException("UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' must first be declared with 'our' (conflicts with parse option REQUIRE_OUR)", vname.ostr);
      else if (po & PO_NO_GLOBAL_VARS) // check if new global variables are allowed to be created at all
	 parseException("ILLEGAL-GLOBAL-VARIABLE", "illegal reference to new global variable '%s' (conflicts with parse option NO_GLOBAL_VARS)", vname.ostr);
      else
	 qore_program_private::makeParseWarning(pgm, QP_WARN_UNDECLARED_VAR, "UNDECLARED-GLOBAL-VARIABLE", "global variable '%s' should be explicitly declared with 'our'", vname.ostr);

      assert(!tns->var_list.parseFindVar(vname.getIdentifier()));
      rv = tns->var_list.parseCreatePendingVar(vname.getIdentifier(), typeInfo);
      pend_varmap.update(rv->getName(), this, rv);
   }
   else
      rv->checkAssignType(typeInfo);

   return rv;
}

void qore_ns_private::parseInitConstants() {
   printd(5, "qore_ns_private::parseInitConstants() %s\n", name.c_str());

   NamespaceParseContextHelper nspch(this);

   // do 2nd stage parse initialization on pending constants
   pendConstant.parseInit();

   nsl.parseInitConstants();
   pendNSL.parseInitConstants();
}

void qore_ns_private::parseInit() {
   printd(5, "qore_ns_private::parseInit() this=%p ns=%p\n", this, ns);

   // do 2nd stage parse initialization on committed classes
   classList.parseInit();
      
   // do 2nd stage parse initialization on pending classes
   pendClassList.parseInit();

   {
      NamespaceParseContextHelper nspch(this);

      // do 2nd stage parse initialization on user functions
      func_list.parseInit();
   }
   
   // do 2nd stage parse initialization in subnamespaces
   nsl.parseInit();

   // do 2nd stage parse initialization on pending subnamespaces
   pendNSL.parseInit();
}

void qore_ns_private::parseCommit() {
   // commit pending global vars
   var_list.parseCommit();

   // merge pending user functions
   func_list.parseCommit();

   // merge pending constant list
   constant.assimilate(pendConstant);

   // merge pending classes and commit pending changes to committed classes
   classList.parseCommit(pendClassList);

   // merge pending namespaces and repeat for all subnamespaces
   nsl.parseCommit(pendNSL);
}

void qore_ns_private::parseRollback() {
   printd(5, "qore_ns_private::parseRollback() %s this=%p ns=%p\n", name.c_str(), this, ns);

   // delete pending global variables
   var_list.parseRollback();

   // delete pending user functions
   func_list.parseRollback();

   // delete pending constant list
   pendConstant.parseDeleteAll();

   // delete pending changes to committed classes
   classList.parseRollback();

   // delete pending classes
   pendClassList.reset();

   // delete pending namespaces
   pendNSL.reset();

   // do for all subnamespaces
   nsl.parseRollback();
}

qore_ns_private *qore_ns_private::parseAddNamespace(QoreNamespace* nns) {
   std::auto_ptr<QoreNamespace> nnsh(nns);

   //printd(5, "qore_ns_private::parseAddNamespace() this=%p '%s' adding %p '%s' (exists %p)\n", this, getName(), ns, ns->getName(), priv->nsl.find(ns->getName()));

   // raise an exception if namespace collides with an object name
   if (classList.find(nns->getName())) {
      parse_error("namespace name '%s' collides with previously-defined class '%s'", ns->getName(), ns->getName());
      return 0;
   }

   if (pendClassList.find(nns->getName())) {
      parse_error("namespace name '%s' collides with pending class '%s'", ns->getName(), ns->getName());
      return 0;
   }

   nnsh.release();
   
   // see if a committed namespace with the same name already exists
   QoreNamespace* orig = nsl.find(nns->getName());
   if (orig) {
      orig->priv->assimilate(nns);
      return orig->priv;
   }

   return pendNSL.add(nns, this);
}

// only called while parsing before addition to namespace tree, no locking needed
cnemap_t::iterator qore_ns_private::parseAddConstant(const char* cname, AbstractQoreNode* value) {
   ReferenceHolder<> vh(value, 0);

   if (constant.inList(cname)) {
      std::string path;
      getPath(path, true);
      parse_error("constant '%s' has already been defined in '%s'", cname, path.c_str());
      return pendConstant.end();
   }

   return pendConstant.parseAdd(cname, vh.release());
}

// only called while parsing before addition to namespace tree, no locking needed
void qore_ns_private::parseAddConstant(const NamedScope& nscope, AbstractQoreNode* value) {
   ReferenceHolder<> vh(value, 0);

   QoreNamespace *sns = resolveNameScope(nscope);
   if (!sns)
      return;

   sns->priv->parseAddConstant(nscope[nscope.size() - 1], vh.release());
}

// public, only called either in single-threaded initialization or
// while the program-level parse lock is held
int qore_ns_private::parseAddPendingClass(QoreClass* oc) {
   std::auto_ptr<QoreClass> och(oc);

   //printd(5, "qore_ns_private::parseAddPendingClass() adding str=%s (%p)\n", oc->name, oc);
   // raise an exception if object name collides with a namespace
   if (nsl.find(oc->getName())) {
      parse_error("class name '%s' collides with previously-defined namespace '%s'", oc->getName(), oc->getName());
      return -1;
   }

   if (pendNSL.find(oc->getName())) {
      parse_error("class name '%s' collides with pending namespace '%s'", oc->getName(), oc->getName());
      return -1;
   }

   if (classList.find(oc->getName())) {
      parse_error("class '%s' already exists in namespace '%s'", oc->getName(), name.c_str());
      return -1;
   }

   if (pendClassList.add(oc)) {
      parse_error("class '%s' is already pending in namespace '%s'", oc->getName(), name.c_str());
      return -1;
   }

   qore_class_private::setNamespace(oc, this);

   och.release();
   return 0;
}

// public, only called when parsing unattached namespaces
int qore_ns_private::parseAddPendingClass(const NamedScope& n, QoreClass* oc) {
   std::auto_ptr<QoreClass> och(oc);

   //printd(5, "qore_ns_private::parseAddPendingClass() adding ns=%s (%s, %p)\n", n.ostr, oc->getName(), oc);
   QoreNamespace* sns = resolveNameScope(n);
   if (!sns)
      return -1;

   return sns->priv->parseAddPendingClass(och.release());
}

/*
void qore_ns_private::scanMergeCommittedNamespace(const qore_ns_private& mns, QoreModuleContext& qmc) const {
   // make sure there are no objects in the pending lists
   assert(mns.pendNSL.empty());
   assert(mns.pendConstant.empty());
   assert(mns.pendClassList.empty());
   
   // check constants
   {
      ConstConstantListIterator cli(mns.constant);
      while (cli.next()) {
         if (constant.inList(cli.getName()))
            qmc.error("duplicate constant %s::%s", name.c_str(), cli.getName().c_str());
         if (pendConstant.inList(cli.getName()))
            qmc.error("duplicate pending constant %s::%s", name.c_str(), cli.getName().c_str());
      }
   }

   // check classes
   {
      ConstClassListIterator cli(mns.classList);
      while (cli.next()) {
         if (classList.find(cli.getName()))
            qmc.error("duplicate class %s::%s", name.c_str(), cli.getName());
         if (pendClassList.find(cli.getName()))
            qmc.error("duplicate pending class %s::%s", name.c_str(), cli.getName());            
      }
   }

   // check functions
   for (fl_map_t::const_iterator i = mns.func_list.begin(), e = mns.func_list.end(); i != e; ++i) {
      if (func_list.findNode(i->first))
         qmc.error("duplicate function %s::%s()", name.c_str(), i->first);
   }

   // check subnamespaces
   for (nsmap_t::const_iterator i = mns.nsl.nsmap.begin(), e = mns.nsl.nsmap.end(); i != e; ++i) {
      // see if a class with the same name is present
      if (classList.find(i->first.c_str())) {
         qmc.error("namespace '%s::%s' clashes with an existing class of the same name", name.c_str(), i->first.c_str());
         continue;
      }
      // see if a pending class with the same name is present
      if (pendClassList.find(i->first.c_str())) {
         qmc.error("namespace '%s::%s' clashes with a pending class of the same name", name.c_str(), i->first.c_str());
         continue;
      }
      // see if a subnamespace with the same name exists
      const QoreNamespace* cns = nsl.find(i->first);
      if (!cns)
         cns = pendNSL.find(i->first);
      if (cns) {
         cns->priv->scanMergeCommittedNamespace(*(i->second->priv), qmc);
         continue;
      }
   }
}

void qore_ns_private::copyMergeCommittedNamespace(const qore_ns_private& mns) {
   // add constants
   {
      ConstConstantListIterator cli(mns.constant);
      while (cli.next()) {
      }
   }

   // add classes
   {
      ConstClassListIterator cli(mns.classList);
      while (cli.next()) {
      }
   }

   // add functions
   for (fl_map_t::const_iterator i = mns.func_list.begin(), e = mns.func_list.end(); i != e; ++i) {
   }

   // add sub namespaces
   for (nsmap_t::const_iterator i = mns.nsl.nsmap.begin(), e = mns.nsl.nsmap.end(); i != e; ++i) {
      QoreNamespace* nns = findCreateNamespace(i->first.c_str());
      i->second->priv->scanMergeCommittedNamespace(*(i->second->priv));
   }
}
*/

void qore_ns_private::assimilate(QoreNamespace* ans) {
   qore_ns_private* pns = ans->priv;
   // make sure there are no objects in the committed lists
   assert(pns->nsl.empty());
   assert(pns->constant.empty());
   assert(pns->classList.empty());

   // assimilate pending constants
   // assimilate target list - if there were errors then the list will be deleted anyway
   pendConstant.assimilate(pns->pendConstant, constant, name.c_str());

   // assimilate classes
   pendClassList.assimilate(pns->pendClassList, *this);

   // assimilate pending functions
   func_list.assimilate(pns->func_list);

   // assimilate sub namespaces
   for (nsmap_t::iterator i = pns->pendNSL.nsmap.begin(), e = pns->pendNSL.nsmap.end(); i != e; ++i) {
      // throw parse exception if name is already defined as a namespace or class
      if (nsl.find(i->second->priv->name.c_str()))
	 parse_error("subnamespace '%s' has already been defined in namespace '%s'", i->second->priv->name.c_str(), name.c_str());
      else if (pendNSL.find(i->second->priv->name.c_str()))
	 parse_error("subnamespace '%s' is already pending in namespace '%s'", i->second->priv->name.c_str(), name.c_str());
      else if (classList.find(i->second->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class has already been defined with this name",
		     i->second->priv->name.c_str(), name.c_str());
      else if (pendClassList.find(i->second->priv->name.c_str()))
	 parse_error("cannot add namespace '%s' to existing namespace '%s' because a class is already pending with this name",
		     i->second->priv->name.c_str(), name.c_str());
   }

   // assimilate target namespace list
   pendNSL.assimilate(pns->pendNSL, this);

   // delete source namespace
   delete ans;
}

QoreClass* qore_ns_private::parseFindLocalClass(const char* cname) {
   QoreClass* rv = classList.find(cname);
   return rv ? rv : pendClassList.find(cname);
}

AbstractQoreNode* qore_ns_private::getConstantValue(const char* cname, const QoreTypeInfo *&typeInfo) {
   AbstractQoreNode* rv = constant.find(cname, typeInfo);
   if (rv)
      return rv;

   return pendConstant.find(cname, typeInfo);
}

QoreNamespace* qore_ns_private::resolveNameScope(const NamedScope& nscope) const {
   const QoreNamespace* sns = ns;

   // find namespace
   for (unsigned i = 0; i < (nscope.size() - 1); i++)
      if (!(sns = sns->priv->parseFindLocalNamespace(nscope[i]))) {
	 parse_error("namespace '%s' cannot be resolved while evaluating '%s' in constant declaration", nscope[i], nscope.ostr);
	 return 0;
      }
   return (QoreNamespace* )sns;
}

const QoreFunction* qore_ns_private::parseMatchFunction(const NamedScope& nscope, unsigned& match) const {
   assert(nscope.strlist[0] == name);
   const QoreNamespace* fns = ns;

   assert(name == nscope[0]);

   // mark first namespace as matched
   if (!match)
      match = 1;

   // check for a match of the structure in this namespace
   for (unsigned i = 1; i < (nscope.size() - 1); i++) {
      //printd(5, "qore_ns_private::parseMatchFunction('%s') curr: %p '%s' element %d %s found: %p\n", nscope.ostr, fns, fns->getName(), i, nscope[i], fns->priv->parseFindLocalNamespace(nscope[i]));
      fns = fns->priv->parseFindLocalNamespace(nscope[i]);
      if (!fns)
         return 0;
      if (i >= match)
         match = i + 1;
   }

   return fns->priv->func_list.find(nscope.getIdentifier(), false);
}

const QoreFunction* qore_ns_private::runtimeMatchFunction(const NamedScope& nscope, const qore_ns_private*& rns) const {
   assert(name == nscope[0]);

   const QoreNamespace* fns = ns;
   // check for a match of the structure in this namespace
   for (unsigned i = 1; i < (nscope.size() - 1); i++) {
      fns = fns->priv->nsl.find(nscope[i]);
      if (!fns)
         return 0;
   }
   rns = fns->priv;
   return rns->func_list.find(nscope.getIdentifier(), true);
}

// qore_ns_private::parseMatchNamespace()
// will only be called if there is a match with the name and nscope.size() > 1
QoreNamespace* qore_ns_private::parseMatchNamespace(const NamedScope& nscope, unsigned& matched) const {
   printd(5, "qore_ns_private::parseMatchNamespace() this=%p ns=%p '%s' ns=%s matched=%d\n", this, ns, name.c_str(), nscope.ostr, matched);

   assert(nscope[0] == name);
   const QoreNamespace* fns = ns;

   // mark first namespace as matched
   if (!matched)
      matched = 1;

   // check for a match of the structure in this namespace
   for (unsigned i = 1; i < (nscope.size() - 1); i++) {
      fns = fns->priv->parseFindLocalNamespace(nscope[i]);
      if (!fns)
         break;
      if (i >= matched)
         matched = i + 1;
   }
   return (QoreNamespace* )fns;
}

QoreClass* qore_ns_private::parseMatchScopedClass(const NamedScope& nscope, unsigned& matched) {
   printd(5, "qore_ns_private::parseMatchScopedClass() this=%p ns=%p '%s' nscope='%s' matched=%d\n", this, ns, name.c_str(), nscope.ostr, matched);

   if (nscope[0] != name) {
      QoreNamespace* fns = nsl.find(nscope[0]);
      if (!fns)
         fns = pendNSL.find(nscope[0]);      
      return fns ? fns->priv->parseMatchScopedClass(nscope, matched) : 0;
   }

   // mark first namespace as matched
   if (!matched)
      matched = 1;

   printd(5, "qore_ns_private::parseMatchScopedClass() matched %s in %s\n", name.c_str(), nscope.ostr);

   QoreNamespace* fns = ns;
   
   // if we need to follow the namespaces, then do so
   if (nscope.size() > 2) {
      for (unsigned i = 1; i < (nscope.size() - 1); i++) {
	 fns = fns->priv->parseFindLocalNamespace(nscope[i]);
	 if (!fns)
	    return 0;
	 if (i >= matched)
	    matched = i + 1;
      }
   }
   QoreClass* rv = fns->priv->findLoadClass(nscope[nscope.size() - 1]);
   if (!rv)
      rv = fns->priv->pendClassList.find(nscope[nscope.size() - 1]);
   return rv;
}

QoreClass* qore_ns_private::parseMatchScopedClassWithMethod(const NamedScope& nscope, unsigned& matched) {
   assert(nscope.size() > 2);
   assert(nscope.strlist[0] == name);

   printd(5, "qore_ns_private::parseMatchScopedClassWithMethod() this=%p ns=%p '%s' class=%s (%s)\n", this, ns, name.c_str(), nscope[nscope.size() - 2], nscope.ostr);

   QoreNamespace* fns = ns;

   // mark first namespace as matched
   if (!matched)
      matched = 1;

   // search the rest of the namespaces
   for (unsigned i = 1; i < (nscope.size() - 2); i++) {	 
      fns = fns->priv->parseFindLocalNamespace(nscope[i]);
      if (!fns)
         return 0;
      if (i >= matched)
         matched = i + 1;
   }

   // now get class from final namespace
   QoreClass* rv = fns->priv->findLoadClass(nscope[nscope.size() - 2]);
   if (!rv)
      rv = fns->priv->pendClassList.find(nscope[nscope.size() - 2]);

   return rv;
}

AbstractQoreNode* qore_ns_private::parseResolveClassConstant(QoreClass* qc, const char* name, const QoreTypeInfo*& typeInfo) {
   AbstractQoreNode* rv = qore_class_private::parseFindConstantValue(qc, name, typeInfo, true);
   if (rv)
      return rv->refSelf();
   const QoreClass* aqc;
   QoreVarInfo *vi = qore_class_private::parseFindStaticVar(qc, name, aqc, typeInfo, true);
   if (vi) {
      typeInfo = vi->getTypeInfo();
      return new StaticClassVarRefNode(name, *qc, *vi);
   }
   return 0;
}

AbstractQoreNode* qore_ns_private::parseMatchScopedConstantValue(const NamedScope& nscope, unsigned& matched, const QoreTypeInfo*& typeInfo) {
   printd(5, "qore_ns_private::parseMatchScopedConstantValue) trying to find %s in %s (%p) typeInfo=%p\n", nscope.getIdentifier(), name.c_str(), getConstantValue(nscope.getIdentifier(), typeInfo));

   assert(nscope[0] == name);

   // mark first namespace as matched
   if (!matched)
      matched = 1;

   const QoreNamespace* fns = ns;

   // if we need to follow the namespaces, then do so
   if (nscope.size() > 2) {
      unsigned last = nscope.size() - 1;
      for (unsigned i = 1; i < last; i++) {
	 const char* oname = nscope[i];
         const QoreNamespace* nns = fns->priv->parseFindLocalNamespace(oname);
	 if (!nns) {
	    // if we are on the last element before the constant in the namespace path list,
	    // then check for a class constant
	    if (i == (last - 1)) {
	       QoreClass* qc = fns->priv->parseFindLocalClass(oname);
	       return qc ? qore_class_private::parseFindLocalConstantValue(qc, nscope.getIdentifier(), typeInfo) : 0;
	    }
	    return 0;
	 }
         fns = nns;
	 if (i >= matched)
	    matched = i + 1;
      }
   }

   return fns->priv->getConstantValue(nscope.getIdentifier(), typeInfo);
}

AbstractQoreNode* qore_ns_private::parseCheckScopedReference(const NamedScope& nsc, unsigned& matched, const QoreTypeInfo*& typeInfo, bool abr) const {
   const QoreNamespace* pns = ns;

   matched = 1;

   // follow the namespaces
   unsigned last = nsc.size() - 1;
   for (unsigned i = 1; i < last; ++i) {
      QoreNamespace* nns = pns->priv->parseFindLocalNamespace(nsc.strlist[i].c_str());

      if (!nns) {
         // if we have matched all namespaces except the last one, check if it's a class
         // and try to resolve a class constant or static class variable
         if (i == (last - 1)) {
            const char* cname = nsc.strlist[last - 1].c_str();
            QoreClass* qc = pns->priv->parseFindLocalClass(cname);
            //printd(5, "qore_ns_private::parseCheckScopedReference() this=%p '%s' nsc=%s checking for class '%s' qc=%p\n", this, name.c_str(), nsc.ostr, cname, qc);
            if (qc) 
               return parseResolveClassConstant(qc, nsc.getIdentifier(), typeInfo);
         }
         return 0;
      }

      pns = nns;
      if (i >= matched)
         matched = i + 1;      
   }

   // matched all namespaces, now try to find a constant
   AbstractQoreNode* rv = pns->priv->parseFindLocalConstantValue(nsc.getIdentifier(), typeInfo);
   if (!rv && abr) {
      Var* v = pns->priv->var_list.parseFindVar(nsc.getIdentifier());
      if (v) {
         typeInfo = v->getTypeInfo();
         return new GlobalVarRefNode(strdup(nsc.ostr), v);
      }
   }
      
   return rv ? rv->refSelf() : 0;
}

AbstractQoreNode* qore_ns_private::parseFindLocalConstantValue(const char* cname, const QoreTypeInfo *&typeInfo) {
   AbstractQoreNode* rv = constant.find(cname, typeInfo);
   return rv ? rv : pendConstant.find(cname, typeInfo);
}

QoreNamespace* qore_ns_private::parseFindLocalNamespace(const char* nname) {
   QoreNamespace* rv = nsl.find(nname);
   return rv ? rv : pendNSL.find(nname);
}

void StaticSystemNamespace::purge() {
   ExceptionSink xsink;
   deleteData(&xsink);
   priv->purge();
}
