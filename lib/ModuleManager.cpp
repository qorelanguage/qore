/*
  ModuleManager.cpp

  Qore module manager

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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
#include <qore/intern/qore_program_private.h>
#include <qore/intern/ModuleInfo.h>
#include <qore/intern/QoreNamespaceIntern.h>
#include <qore/intern/QoreException.h>

#include <errno.h>
#include <string.h>

#ifdef HAVE_GLOB_H
#include <glob.h>
#else
#include <qore/intern/glob.h>
#endif

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <string>
#include <map>
#include <vector>
#include <set>

static const qore_mod_api_compat_s qore_mod_api_list_l[] = { {0, 19}, {0, 18}, {0, 17}, {0, 16}, {0, 15}, {0, 14}, {0, 13}, {0, 12}, {0, 11}, {0, 10}, {0, 9}, {0, 8}, {0, 7}, {0, 6}, {0, 5} };
#define QORE_MOD_API_LEN (sizeof(qore_mod_api_list_l)/sizeof(struct qore_mod_api_compat_s))

// public symbols
const qore_mod_api_compat_s* qore_mod_api_list = qore_mod_api_list_l;
const unsigned qore_mod_api_list_len = QORE_MOD_API_LEN;

QoreModuleManager QMM;
ModuleManager MM;

static bool show_errors = false;

QoreModuleDefContext::strset_t QoreModuleDefContext::vset;

ModuleReExportHelper::ModuleReExportHelper(QoreAbstractModule* mi, bool reexp) : m(set_reexport(mi, reexp, reexport)) {
   //printd(5, "ModuleReExportHelper::ModuleReExportHelper() %p '%s' (reexp: %d) to %p '%s' (reexp: %d)\n", mi, mi ? mi->getName() : "n/a", reexp, m, m ? m->getName() : "n/a", reexport);
   if (m && mi && reexp) {
      //printd(5, "ModuleReExportHelper::ModuleReExportHelper() adding '%s' to '%s'\n", mi->getName(), m->getName());
      m->addModuleReExport(mi->getName());
   }
}

ModuleReExportHelper::~ModuleReExportHelper() {
   set_reexport(m, reexport);
}

void QoreAbstractModule::reexport(ExceptionSink& xsink, QoreProgram* pgm) const {
   // import also any modules that should be reexported from the loaded module
   for (name_vec_t::const_iterator i = rmod.begin(), e = rmod.end(); i != e; ++i) {
      QMM.loadModuleIntern(xsink, i->c_str(), pgm);
   }
}

void QoreModuleContext::error(const char* fmt, ...) {
   va_list args;
   QoreStringNode* err = new QoreStringNodeMaker("module '%s': ", name);

   while (true) {
      va_start(args, fmt);
      int rc = err->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }   

   xsink.raiseExceptionArg("MODULE-LOAD-ERROR", new QoreStringNode(name), err);
}

void QoreModuleContext::commit() {
   rns->commitModule(*this);

   mcfl.mcfl_t::clear();
   mcnl.mcnl_t::clear();
}

void QoreModuleDefContext::set(const char* key, const AbstractQoreNode* val) {
   qore_type_t t = get_node_type(val);

   // special handling for "init" and "del"
   if (!strcmp(key, "init")) {
      if (init_c)
	 parse_error("module key 'init' was given multiple times");
      else // check type when code is committed
	 init_c = val->refSelf();
   }
   else if (!strcmp(key, "del")) {
      if (del_c)
	 parse_error("module key 'del' was given multiple times");
      else // check type when code is committed
	 del_c = val->refSelf();
   }
   else if (vset.find(key) == vset.end())
      parse_error("module key '%s' is invalid", key);
   else if (vmap.find(key) != vmap.end())
      parse_error("module key '%s' was given multiple times", key);
   else if (t != NT_STRING)
      parse_error("module key '%s' assigned type '%s' (expecting 'string')", key, get_type_name(val));
   else
      vmap[key] = reinterpret_cast<const QoreStringNode*>(val)->getBuffer();
}

// called only during parsing
void QoreModuleDefContext::parseInit() {
   if (init_c)
      initClosure(init_c, "init");
   if (del_c)
      initClosure(del_c, "del");
}

void QoreModuleDefContext::initClosure(AbstractQoreNode*& c, const char* n) {
   // initialize closure
   int lvids = 0;
   const QoreTypeInfo* typeInfo = 0;
   // check for local variables at the top level - this can only happen if the expresion is not a closure
   c = c->parseInit(0, PO_LOCKDOWN, lvids, typeInfo);
   if (lvids) {
      parseException("ILLEGAL-LOCAL-VAR", "local variables may not be declared in module '%s' code", n);
      // discard variables immediately
      for (int i = 0; i < lvids; ++i)
         pop_local_var();
   }

   qore_type_t t = get_node_type(c);
   if (t != NT_CLOSURE && t != NT_FUNCREF)
      parse_error("the module '%s' key must be assigned to a closure or call reference (got type '%s')", n, get_type_name(c));
}

int QoreModuleDefContext::init(QoreProgram& pgm, ExceptionSink& xsink) {
   if (!init_c)
      return 0;

   {
      ProgramThreadCountContextHelper tch(&xsink, &pgm, true);
      if (xsink)
         return -1;

      ReferenceHolder<> cn(reinterpret_cast<QoreClosureParseNode*>(init_c)->eval(&xsink), &xsink);
      assert(!xsink);
      assert(cn->getType() == NT_RUNTIME_CLOSURE || cn->getType() == NT_FUNCREF);
      ReferenceHolder<> tmp(reinterpret_cast<ResolvedCallReferenceNode*>(*cn)->exec(0, &xsink), &xsink);
   }

   return xsink ? -1 : 0;
}

QoreClosureParseNode* QoreModuleDefContext::takeDel() {
   if (!del_c)
      return 0;

   assert(dynamic_cast<QoreClosureParseNode*>(del_c));
   QoreClosureParseNode* rv = reinterpret_cast<QoreClosureParseNode*>(del_c);
   del_c = 0;
   return rv;
}

QoreModuleContextHelper::QoreModuleContextHelper(const char* name, QoreProgram* pgm, ExceptionSink& xsink)
   : QoreModuleContext(name, qore_root_ns_private::get(pgm ? *(pgm->getRootNS()) : *staticSystemNamespace), xsink) {
   set_module_context(this);
}

QoreModuleContextHelper::~QoreModuleContextHelper() {
   set_module_context(0);
}

void UniqueDirectoryList::addDirList(const char* str) {
   if (!str)
      return;

   // duplicate string for invasive searches
   QoreString plist(str);
   str = (char*)plist.getBuffer();

   // add each directory
   while (char* p = (char*)strchr(str, ':')) {
#ifdef _Q_WINDOWS
      // don't match ':' as the second character in a path as a path separator
      if (p == str + 1) {
	 p = (char*)strchr(p + 1, ':');
	 if (!p)
	    break;
      }
#endif
      if (p != str) {
	 *p = '\0';
	 // add string to list
	 push_back(str);
      }
      str = p + 1;
   }

   // add last directory
   if (*str)
      push_back(str);
}

static QoreStringNode* loadModuleError(const char* name, ExceptionSink& xsink) {
   QoreStringNode* rv = new QoreStringNodeMaker("failed to load module '%s':\n", name);
   qore_es_private::appendList(xsink, *rv);
   xsink.clear();
   return rv;
}

void QoreBuiltinModule::addToProgramImpl(QoreProgram* pgm, ExceptionSink& xsink) const {
   QoreModuleContextHelper qmc(name.getBuffer(), pgm, xsink);

   RootQoreNamespace* rns = pgm->getRootNS();
   QoreNamespace* qns = pgm->getQoreNS();

   module_ns_init(rns, qns);

   if (qmc.hasError()) {
      // rollback all module changes
      qmc.rollback();
      return;
   }

   // commit all module changes
   qmc.commit();
   pgm->addFeature(name.getBuffer());
}

QoreUserModule::~QoreUserModule() {
   assert(pgm);
   ExceptionSink xsink;
   if (del) {
      ProgramThreadCountContextHelper tch(&xsink, pgm, true);
      if (!xsink) {
         ReferenceHolder<> cn(del->eval(&xsink), &xsink);
         assert(!xsink);
	 assert(cn->getType() == NT_RUNTIME_CLOSURE || cn->getType() == NT_FUNCREF);
	 ReferenceHolder<> tmp(reinterpret_cast<ResolvedCallReferenceNode*>(*cn)->exec(0, &xsink), &xsink);
         del->deref(&xsink);
      }
   }
   pgm->waitForTerminationAndDeref(&xsink);
}

void QoreUserModule::addToProgramImpl(QoreProgram* tpgm, ExceptionSink& xsink) const {
   //printd(5, "QoreUserModule::addToProgram() tpgm po: %llx pgm dom: %llx\n", tpgm->getParseOptions64(), qore_program_private::getDomain(*pgm));
   // first check the module's functional domain
   int64 dom = qore_program_private::getDomain(*pgm);
   if (tpgm->getParseOptions64() & dom) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s' implements functionality restricted in the Program object trying to import the module (%xd)", name.getBuffer(), tpgm->getParseOptions64() & dom);
      return;
   }

   QoreModuleContextHelper qmc(name.getBuffer(), tpgm, xsink);
   ProgramThreadCountContextHelper ptcch(&xsink, tpgm, false);
   if (xsink) {
      // rollback all module changes
      qmc.rollback();
      return;
   }

   RootQoreNamespace* rns = tpgm->getRootNS();
   qore_root_ns_private::scanMergeCommittedNamespace(*rns, *(pgm->getRootNS()), qmc);

   if (qmc.hasError()) {
      // rollback all module changes
      qmc.rollback();
      return;
   }

   // commit all module changes
   qore_root_ns_private::copyMergeCommittedNamespace(*rns, *(pgm->getRootNS()));
   qore_program_private::addUserFeature(*tpgm, name.getBuffer());
   //tpgm->addUserFeature(name.getBuffer());

   // add domain to current Program's domain
   qore_program_private::runtimeAddDomain(*tpgm, dom);

   QMM.trySetUserModuleDependency(this);
}

void QoreBuiltinModule::issueParseCmd(QoreString& cmd) {
   if (!module_parse_cmd) {
      parseException("PARSE-COMMAND-ERROR", "module '%s' loaded from '%s' has not registered a parse command handler", name.getBuffer(), filename.getBuffer());
      return;
   }

   ExceptionSink* pxsink = getProgram()->getParseExceptionSink();
   // if parse exceptions have been disabled, then skip issuing the command
   if (!pxsink)
      return;

   module_parse_cmd(cmd, pxsink);
}

ModuleManager::ModuleManager() {
}

// to add a directory to the module directory search list, can only be called before init()
void ModuleManager::addModuleDir(const char* dir) {
   QMM.addModuleDir(dir);
}

// no longer supported; throws an exception
void ModuleManager::addAutoModuleDir(const char* dir) {
   printd(0, "ModuleManager::addAutoModuleDir() support for auto module directories was removed in 0.8.4\n");
   assert(false);
}

// to add a list of directories to the module directory search list, can only be called before init()
void ModuleManager::addModuleDirList(const char* strlist) {
   QMM.addModuleDirList(strlist);
}

// no longer supported; throws an exception
void ModuleManager::addAutoModuleDirList(const char* strlist) {
   printd(0, "ModuleManager::addAutoModuleDir() support for auto module directories was removed in 0.8.4\n");
   assert(false);
}

void QoreModuleManager::init(bool se) {
   static const char* qt_blacklist_string = "because it was implemented with faulty namespace handling that does not work with newer versions of Qore; use the 'qt4' module based in libsmoke instead; it is much more complete";

   // setup possible user module keys
   QoreModuleDefContext::vset.insert("desc");
   QoreModuleDefContext::vset.insert("version");
   QoreModuleDefContext::vset.insert("author");
   QoreModuleDefContext::vset.insert("url");
   QoreModuleDefContext::vset.insert("license");

   mutex = new QoreThreadLock(&ma_recursive);

   // initialize blacklist
   // add old QT modules to blacklist
   mod_blacklist.insert(std::make_pair((const char*)"qt-core", qt_blacklist_string));
   mod_blacklist.insert(std::make_pair((const char*)"qt-gui", qt_blacklist_string));
   mod_blacklist.insert(std::make_pair((const char*)"qt-svn", qt_blacklist_string));
   mod_blacklist.insert(std::make_pair((const char*)"qt-opengl", qt_blacklist_string));

   show_errors = se;

   // setup module directory list from QORE_MODULE_DIR (if it hasn't already been manually set up)
   if (moduleDirList.empty())
      QoreModuleManager::addStandardModulePaths();
}

void QoreModuleManager::addStandardModulePaths() {
   moduleDirList.addDirList(getenv("QORE_MODULE_DIR"));

   // append version-specifc user module directory
   moduleDirList.push_back(USER_MODULE_VER_DIR);

   // append version-specifc module directory
   if (strcmp(MODULE_VER_DIR, USER_MODULE_VER_DIR))
      moduleDirList.push_back(MODULE_VER_DIR);

   // append user-module directory
   moduleDirList.push_back(USER_MODULE_DIR);

   // append qore module directory
   if (strcmp(MODULE_DIR, USER_MODULE_DIR))
      moduleDirList.push_back(MODULE_DIR);
}

void ModuleManager::addStandardModulePaths() {
   QMM.addStandardModulePaths();
}

int ModuleManager::runTimeLoadModule(const char* name, ExceptionSink* xsink) {
   assert(name);
   assert(xsink);
   return QMM.runTimeLoadModule(*xsink, name, getProgram());
}

int ModuleManager::runTimeLoadModule(const char* name, QoreProgram* pgm, ExceptionSink* xsink) {
   assert(name);
   assert(xsink);
   return QMM.runTimeLoadModule(*xsink, name, pgm);
}

int QoreModuleManager::runTimeLoadModule(ExceptionSink& xsink, const char* name, QoreProgram* pgm, QoreProgram* mpgm, bool inject, bool reinject, bool priv) {
   // grab the parse lock
   ProgramRuntimeParseContextHelper pah(&xsink, pgm);
   if (xsink)
      return -1;
   
   AutoLocker al2(mutex);               // grab global module lock
   loadModuleIntern(xsink, name, pgm, false, MOD_OP_NONE, 0, 0, mpgm, inject, reinject, priv);
   return xsink ? -1 : 0;
}

static const char* get_op_string(mod_op_e op) {
   if (op == MOD_OP_LT) return "<";
   if (op == MOD_OP_LE) return "<=";
   if (op == MOD_OP_EQ) return "=";
   if (op == MOD_OP_GE) return ">=";
   assert(op == MOD_OP_GT);
   return ">";
}

#define MVC_FAIL     0
#define MVC_OK       1
#define MVC_FINAL_OK 2
int check_component(mod_op_e op, int mod_ver, int req_ver, bool last) {
   // "promote" operator if not comparing last element
   if (!last) {
      if (op == MOD_OP_LT) op = MOD_OP_LE;
      else if (op == MOD_OP_GT) op = MOD_OP_GE;
   }
   //printd(5, "check_component(%d %s %d)\n", mod_ver, get_op_string(op), req_ver);
   if (op == MOD_OP_LT)
      return mod_ver < req_ver ? MVC_FINAL_OK : MVC_FAIL;
   if (op == MOD_OP_LE) 
      return mod_ver < req_ver ? MVC_FINAL_OK : (mod_ver == req_ver ? MVC_OK : MVC_FAIL);
   if (op == MOD_OP_EQ)
      return mod_ver == req_ver ? MVC_OK : MVC_FAIL;
   if (op == MOD_OP_GE)
      return mod_ver > req_ver ? MVC_FINAL_OK : (mod_ver == req_ver ? MVC_OK : MVC_FAIL);
   assert(op == MOD_OP_GT);
   return mod_ver > req_ver ? MVC_FINAL_OK : MVC_FAIL;
}

static void check_qore_version(const char* name, mod_op_e op, version_list_t& version, ExceptionSink& xsink) {
   unsigned max = version.size() > 3 ? version.size() : 3;
   for (unsigned i = 0; i < max; ++i) {
      int mv = (!i ? QORE_VERSION_MAJOR : 
		(i == 1 ? QORE_VERSION_MINOR : 
		 (i == 2 ? QORE_VERSION_SUB : 0)));
      int rv = (i >= version.size() ? 0 : version[i]);
      int res = check_component(op, mv, rv, i == (max - 1));
      if (res == MVC_FAIL) {
	 xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "feature '%s' is built in, but the following version requirement is not satisfied: Qore library %s %s %s", name, QORE_VERSION, get_op_string(op), *version);
	 return;
      }
      if (res == MVC_FINAL_OK)
	 break;
   }
}

static void check_module_version(QoreAbstractModule* mi, mod_op_e op, version_list_t& version, ExceptionSink& xsink) {
   unsigned max = version.size() > mi->version_list.size() ? version.size() : mi->version_list.size();
   //printd(5, "check_module_version(%s %s %s) max=%d vs=%d ms=%d\n", mi->getVersion(), get_op_string(op), version->getString(), max, version->size(), mi->version_list.size());
   for (unsigned i = 0; i < max; ++i) {
      int mv = (i >= mi->version_list.size() ? 0 : mi->version_list[i]);
      int rv = (i >= version.size() ? 0 : version[i]);
      int res = check_component(op, mv, rv, i == (max - 1));
      if (res == MVC_FAIL) {
	 xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(mi->getName()), "loaded module '%s' does not satisfy the following requirement: %s %s %s", mi->getName(), mi->getVersion(), get_op_string(op), *version);
	 return;
      }
      if (res == MVC_FINAL_OK)
	 break;
   }
}

static void qore_check_load_module_intern(QoreAbstractModule* mi, mod_op_e op, version_list_t* version, QoreProgram* pgm, ExceptionSink& xsink) {
   // check version if necessary
   if (version) {
      check_module_version(mi, op, *version, xsink);
      if (xsink)
	 return;
   }

   if (pgm)
      mi->addToProgram(pgm, xsink);
}

void QoreModuleManager::getUniqueName(QoreString& nname, const char* name, const char* suffix) {
   int ver = 1;
   while (true) {
      nname.sprintf("%s!!%s%d", name, suffix, ver++);
      if (!findModuleUnlocked(nname.getBuffer()))
	 break;
      nname.clear();
   }
}

void QoreModuleManager::reinjectModule(QoreAbstractModule* mi) {
   // handle reinjections here
   QoreString nname;
   getUniqueName(nname, mi->getName(), "!!orig-");
   module_map_t::iterator i = map.find(mi->getName());
   assert(i != map.end());
   map.erase(i);
   mi->rename(nname);
   addModule(mi);
}

void QoreModuleManager::loadModuleIntern(ExceptionSink& xsink, const char* name, QoreProgram* pgm, bool reexport, mod_op_e op, version_list_t* version, const char* src, QoreProgram* mpgm, bool inject, bool reinject, bool priv) {
   assert(!version || (version && op != MOD_OP_NONE));

   ReferenceHolder<QoreProgram> pholder(mpgm, &xsink);
   
   // check for special "qore" feature
   if (!strcmp(name, "qore")) {
      if (version)
	 check_qore_version(name, op, *version, xsink); 
      return;
   }

   // if the feature already exists in this program, then return
   if (pgm && pgm->checkFeature(name)) {
      if (inject)
	 xsink.raiseException("LOAD-MODULE-ERROR", "cannot load module '%s' for injection because the module has already been loaded", name);

      QoreAbstractModule* mi = findModuleUnlocked(name);

      // check version if necessary
      if (version) {
	 // if no module is found, then this is a builtin feature
	 if (!mi)
	    check_qore_version(name, op, *version, xsink);
	 else
	    check_module_version(mi, op, *version, xsink);
      }
	 
      trySetUserModuleDependency(mi);
      return;
   }

   // check if parse options allow loading any modules at all
   if (pgm && (pgm->getParseOptions64() & PO_NO_MODULES)) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "cannot load modules ('%s' requested) into the current Program object because PO_NO_MODULES is set", name);
      return;
   }   

   // if the feature already exists, then load the namespace changes into this program and register the feature
   QoreAbstractModule* mi = findModuleUnlocked(name);

   if (mi) {
      if (!reinject) {
	 if (inject)
	    xsink.raiseException("LOAD-MODULE-ERROR", "cannot load module '%s' for injection because the module has already been loaded; to reinject a module, call Program::loadApplyToUserModule() with the reinject flag set to True", name);
	 else
	    qore_check_load_module_intern(mi, op, version, pgm, xsink);
	 return;
      }
   }

   //printd(5, "QoreModuleManager::loadModuleIntern() this: %p name: %s not found\n", this, name);

   // see if we are loading a user module from explicit source
   if (src) {
      mi = loadUserModuleFromSource(xsink, name, name, pgm, src, reexport, pholder.release());
      if (xsink) {
	 assert(!mi);
	 return;
      }
      assert(mi);
      qore_check_load_module_intern(mi, op, version, pgm, xsink);
      return;
   }

   // see if this is actually a path
   if (q_find_first_path_sep(name)) {
      // see if it's a user or binary module
      size_t len = strlen(name);
      if (len > 5 && !strcasecmp(".qmod", name + len - 5)) {
	 if (mpgm) {
	    xsink.raiseException("LOAD-MODULE-ERROR", "cannot load a binary module with a Program container");
	    return;
	 }
	 if (reinject) {
	    xsink.raiseException("LOAD-MODULE-ERROR", "cannot reinject module '%s' because reinjection is not currently supported for binary modules", name);
	    return;
	 }

	 mi = loadBinaryModuleFromPath(xsink, name, 0, pgm, reexport);
      }
      else {
	 QoreString n(name);
	 qore_offset_t i = n.rfind('.');
	 if (i > 0)
	    n.terminate(i);
#ifdef _Q_WINDOWS
	 i = n.rfindAny("\\/");
#else
	 i = n.rfind(QORE_DIR_SEP);
#endif
	 if (i >= 0)
	    n.replace(0, i + 1, (const char*)0);

	 mi = loadUserModuleFromPath(xsink, name, n.getBuffer(), pgm, reexport, pholder.release(), inject, reinject, reinject ? mpgm : 0, priv);
      }

      if (xsink) {
	 assert(!mi);
	 return;
      }

      assert(mi);
      qore_check_load_module_intern(mi, op, version, pgm, xsink);
      return;
   }

   // otherwise, try to find module in the module path
   QoreString str;
   struct stat sb;

   strdeque_t::const_iterator w = moduleDirList.begin();
   while (w != moduleDirList.end()) {
      // try to find module with supported api tags
      for (unsigned ai = 0; ai <= qore_mod_api_list_len; ++ai) {
	 // build path to binary module
	 str.clear();
	 str.sprintf("%s" QORE_DIR_SEP_STR "%s", (*w).c_str(), name);

	 // make new extension string
	 if (ai < qore_mod_api_list_len)
	    str.sprintf("-api-%d.%d.qmod", qore_mod_api_list[ai].major, qore_mod_api_list[ai].minor);
	 else
	    str.concat(".qmod");
	    
	 //printd(5, "ModuleManager::loadModule(%s) trying binary module: %s\n", name, str.getBuffer());
	 if (!stat(str.getBuffer(), &sb)) {
	    printd(5, "ModuleManager::loadModule(%s) found binary module: %s\n", name, str.getBuffer());
	    if (mpgm) {
	       xsink.raiseException("LOAD-MODULE-ERROR", "cannot load a binary module with a Program container");
	       return;
	    }

	    mi = loadBinaryModuleFromPath(xsink, str.getBuffer(), name, pgm, reexport);
	    if (xsink) {
	       assert(!mi);
	       return;
	    }

	    assert(mi);
	    qore_check_load_module_intern(mi, op, version, pgm, xsink);
	    return;
	 }

	 // build path to user module
	 str.clear();
	 str.sprintf("%s" QORE_DIR_SEP_STR "%s.qm", (*w).c_str(), name);
	    
	 //printd(5, "ModuleManager::loadModule(%s) trying user module: %s\n", name, str.getBuffer());
	 if (!stat(str.getBuffer(), &sb)) {
	    printd(5, "ModuleManager::loadModule(%s) found user module: %s\n", name, str.getBuffer());
	    mi = loadUserModuleFromPath(xsink, str.getBuffer(), name, pgm, reexport, pholder.release(), inject, reinject, reinject ? mpgm : 0, priv);
	    if (xsink) {
	       assert(!mi);
	       return;
	    }

	    assert(mi);
	    qore_check_load_module_intern(mi, op, version, pgm, xsink);
	    return;
	 }
      }

      ++w;
   }

   QoreStringNode* desc = new QoreStringNodeMaker("feature '%s' is not builtin and no module with this name could be found in the module path: ", name);
   moduleDirList.appendPath(*desc);
   xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), desc);
}

void ModuleManager::registerUserModuleFromSource(const char* name, const char* src, QoreProgram* pgm, ExceptionSink* xsink) {
   QMM.registerUserModuleFromSource(name, src, pgm, *xsink);
}

QoreStringNode* ModuleManager::parseLoadModule(const char* name, QoreProgram* pgm) {
   ExceptionSink xsink;

   QMM.parseLoadModule(xsink, name, pgm);

   if (!xsink)
      return 0;

   return loadModuleError(name, xsink);
}

void QoreModuleManager::parseLoadModule(ExceptionSink& xsink, const char* name, QoreProgram* pgm, bool reexport) {
   //printd(5, "ModuleManager::parseLoadModule(name: %s, pgm: %p, reexport: %d)\n", name, pgm, reexport);

   char* p = strchrs(name, "<>=");
   if (p) {
      QoreString str(name, p - name);
      str.trim();

      QoreString op;
      do {
	 if (!isblank(*p))
	    op.concat(*p);
	 ++p;
      } while (*p == '<' || *p == '>' || *p == '=' || isblank(*p));

      // get version operator
      mod_op_e mo;

      if (!op.compare("<"))
	 mo = MOD_OP_LT;
      else if (!op.compare("<="))
	 mo = MOD_OP_LE;
      else if (!op.compare("=") || !op.compare("=="))
	 mo = MOD_OP_EQ;
      else if (!op.compare(">="))
	 mo = MOD_OP_GE;
      else if (!op.compare(">"))
	 mo = MOD_OP_GT;
      else {
	 xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "cannot parse module operator '%s'; expecting one of: '<', '<=', '=', '>=', or '>'", op.getBuffer());
	 return;
      }

      version_list_t iv;
      char ec = iv.set(p);
      if (ec) {
	 xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "only numeric digits and '.' characters are allowed in module/feature version specifications, got '%c'", ec);
	 return;
      }

      if (!iv.size()) {
	 xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "empty version specification given in feature/module request");
	 return;
      }

      AutoLocker al(mutex); // make sure checking and loading are atomic
      loadModuleIntern(xsink, str.getBuffer(), pgm, reexport, mo, &iv);
      return;
   }

   AutoLocker al(mutex); // make sure checking and loading are atomic
   loadModuleIntern(xsink, name, pgm, reexport);
}

void QoreModuleManager::registerUserModuleFromSource(const char* name, const char* src, QoreProgram* pgm, ExceptionSink& xsink) {
   AutoLocker al(mutex); // make sure checking and loading are atomic
   loadModuleIntern(xsink, name, pgm, false, MOD_OP_NONE, 0, src);
}

// const char* path, const char* feature, ReferenceHolder<QoreProgram>& pgm
QoreAbstractModule* QoreModuleManager::setupUserModule(ExceptionSink& xsink, std::auto_ptr<QoreUserModule>& mi, QoreUserModuleDefContextHelper& qmd, bool inject, bool reinject) {
   // see if a module with this name is already registered
   QoreAbstractModule* omi = findModuleUnlocked(mi->getName());
   if (omi)
      qmd.setDuplicate();
   else if (reinject) {
      xsink.raiseException("LOAD-MODULE-ERROR", "cannot reinject module '%s' because it has not yet been loaded", mi->getName());
      return 0;
   }

   //printd(5, "QoreModuleManager::setupUserModule() '%s' omi: %p\n", mi->getName(), omi);
   
   if (xsink)
      return 0;

   const char* name = qmd.get("name");

   if (!name) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': no feature name present in module", mi->getFileName());
      return 0;
   }

   //printd(5, "QoreModuleManager::setupUserModule() path: %s name: %s feature: %s\n", mi->getFileName(), name, mi->getName());

   if (strcmp(mi->getName(), name)) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': provides feature '%s', expecting feature '%s', skipping, rename module to %s.qm to load", mi->getFileName(), name, mi->getName(), name);
      return 0;
   }

   // see if a module with this name is already registered
   if (omi) {
      if (!reinject) {
	 // if the module is the same, then do not return an error unless trying to inject
	 if (mi->equalTo(omi)) {
	    if (inject) {
	       xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s' already loaded therefore cannot be used for injection unless the reinject flag is set", mi->getFileName(), name);
	       return 0;
	    }
	    return omi;
	 }
	 xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s' already registered by '%s'", mi->getFileName(), name, omi->getFileName());
	 return 0;
      }
   }
   
   // get qore module description
   const char* desc = qmd.get("desc");
   if (!desc) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing description", mi->getFileName(), name);
      return 0;
   }

   // get qore module version
   const char* version = qmd.get("version");
   if (!version) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing version", mi->getFileName(), name);
      return 0;
   }

   // get qore module author
   const char* author = qmd.get("author");
   if (!author) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing author", mi->getFileName(), name);
      return 0;
   }
 
   const char* url = qmd.get("url");

   const char* license = qmd.get("license");
   QoreString license_str(license ? license : "unknown");

   // init & run module initialization code if any
   if (qmd.init(*mi->getProgram(), xsink))
      return 0;

   mi->set(desc, version, author, url, license_str, qmd.takeDel());

   // handle reinjections here
   if (omi) {
      assert(reinject);
      reinjectModule(omi);
      mi->setOrigName(omi->getName());
   }
   else if (mi->isPrivate()) {
      // rename to unique name
      QoreString nname;
      getUniqueName(nname, mi->getName(), "!!private-");
      mi->rename(nname);
      name = mi->getName();
   }
   
   omi = mi.release();
   addModule(omi);
   trySetUserModule(name);

   return omi;
}

QoreAbstractModule* QoreModuleManager::loadUserModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature, QoreProgram* tpgm, bool reexport, QoreProgram* pgm, bool inject, bool reinject, QoreProgram* path_pgm, bool priv) {
   assert(feature);
   //printd(5, "QoreModuleManager::loadUserModuleFromPath() path: %s feature: %s tpgm: %p\n", path, feature, tpgm);

   QoreParseCountContextHelper pcch;

   // parse options for the module
   int64 po = USER_MOD_PO;
   // add in parse options from the current program, if any, disabling style and types options already set with USER_MOD_PO
   if (tpgm)
      po |= (tpgm->getParseOptions64() & ~(PO_FREE_OPTIONS|PO_REQUIRE_TYPES));

   QoreString tpath;

   QoreProgram* p = tpgm ? tpgm : path_pgm;
   const char* td = p ? p->parseGetScriptDir() : 0;

   // calculate path from relative path if possible
   if ((tpgm || path_pgm) && path[0] == '.') {
      //printd(5, "QoreModuleManager::loadUserModuleFromPath() path: '%s' feature: '%s' tpgm: %p td: '%s'\n", path, feature, p, td ? td : "n/a");
      if (!qore_find_file_in_path(tpath, path, td))
	 path = tpath.getBuffer();
   }

   if (pgm)
      qore_program_private::forceReplaceParseOptions(*pgm, po);
   else
      pgm = new QoreProgram(po);

   //printd(5, "QoreModuleManager::loadUserModuleFromPath(%s) tpgm: %p po: "QLLD" allow-injection: %s tpgm allow-injection: %s pgm allow-injection: %s\n", path, tpgm, po, po & PO_ALLOW_INJECTION ? "true" : "false", (tpgm ? tpgm->getParseOptions64() & PO_ALLOW_INJECTION : 0) ? "true" : "false", pgm->getParseOptions64() & PO_ALLOW_INJECTION ? "true" : "false");
   
   std::auto_ptr<QoreUserModule> mi(new QoreUserModule(td, path, feature, pgm, priv));

   ModuleReExportHelper mrh(mi.get(), reexport);

   QoreUserModuleDefContextHelper qmd(feature, xsink);
   mi->getProgram()->parseFile(path, &xsink, &xsink, QP_WARN_MODULES);

   return setupUserModule(xsink, mi, qmd, inject, reinject);
}

QoreAbstractModule* QoreModuleManager::loadUserModuleFromSource(ExceptionSink& xsink, const char* path, const char* feature, QoreProgram* tpgm, const char* src, bool reexport, QoreProgram* pgm) {
   assert(feature);
   //printd(5, "QoreModuleManager::loadUserModuleFromSource() path: %s feature: %s tpgm: %p\n", path, feature, tpgm);

   QoreParseCountContextHelper pcch;

   // parse options for the module
   int64 po = USER_MOD_PO;
   // add in parse options from the current program, if any, disabling style and types options already set with USER_MOD_PO
   if (tpgm)
      po |= (tpgm->getParseOptions64() & ~(PO_FREE_OPTIONS|PO_REQUIRE_TYPES));

   if (pgm)
      qore_program_private::forceReplaceParseOptions(*pgm, po);
   else
      pgm = new QoreProgram(po);
   
   std::auto_ptr<QoreUserModule> mi(new QoreUserModule(0, path, feature, pgm, false));

   ModuleReExportHelper mrh(mi.get(), reexport);

   QoreUserModuleDefContextHelper qmd(feature, xsink);
   mi->getProgram()->parse(src, path, &xsink, &xsink, QP_WARN_MODULES);

   return setupUserModule(xsink, mi, qmd);
}

struct DLHelper {
   void* ptr;

   DLLLOCAL DLHelper(void* p) : ptr(p) {
   }

   DLLLOCAL ~DLHelper() {
      if (ptr)
	 dlclose(ptr);
   }

   DLLLOCAL void* release() {
      void* rv = ptr;
      ptr = 0;
      return rv;
   }
};

QoreAbstractModule* QoreModuleManager::loadBinaryModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature, QoreProgram* pgm, bool reexport) {
   QoreAbstractModule* mi = 0;

   void* ptr = dlopen(path, RTLD_LAZY|RTLD_GLOBAL);
   if (!ptr) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(path), "error loading qore module '%s': %s", path, dlerror());
      return 0;
   }

   DLHelper dlh(ptr);

   // get module name
   const char* name = (const char*)dlsym(ptr, "qore_module_name");
   if (!name) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(path), "module '%s': no feature name present in module", path);
      return 0;
   }

   // ensure provided feature matches with expected feature
   if (feature && strcmp(feature, name)) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': provides feature '%s', expecting feature '%s', skipping, rename module to %s.qmod to load", path, name, feature, name);
      return 0;
   }

   // see if a module with this name is already registered
   if ((mi = findModuleUnlocked(name))) {
      // if the module is the same, then do not return an error
      if (mi->isPath(path))
	 return mi;
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s' already registered by '%s'", path, name, mi->getFileName());
      return 0;
   }

   // check if it's been blacklisted
   bl_map_t::const_iterator i = mod_blacklist.find(name);
   if (i != mod_blacklist.end()) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': '%s' is blacklisted %s", path, name, i->second);
      return 0;
   }

   // get qore module API major number
   int* api_major = (int*)dlsym(ptr, "qore_module_api_major");
   if (!api_major) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': no qore module API major number", path, name);
      return 0;
   }

   // get qore module API minor number
   int* api_minor = (int*)dlsym(ptr, "qore_module_api_minor");
   if (!api_minor) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': no qore module API minor number", path, name);
      return 0;
   }

   if (!is_module_api_supported(*api_major, *api_minor)) {
      QoreStringNode* str = new QoreStringNodeMaker("module '%s': feature '%s': API mismatch, module supports API %d.%d, however only version", path, name, *api_major, *api_minor);

      if (qore_mod_api_list_len > 1)
	 str->concat('s');
      // add all supported api pairs to the string
      for (unsigned j = 0; j < qore_mod_api_list_len; ++j) {
	 str->sprintf(" %d.%d", qore_mod_api_list[j].major, qore_mod_api_list[j].minor);
	 if (j != qore_mod_api_list_len - 1) {
	    if (qore_mod_api_list_len > 2) {
	       if (j != qore_mod_api_list_len - 2)
		  str->concat(",");
	       else
		  str->concat(", and");
	    }
	    else
	       str->concat(" and");
	 }
	 if (j == qore_mod_api_list_len - 1) {
	    str->concat(' ');
	    if (qore_mod_api_list_len > 1)
	       str->concat("are");
	    else
	       str->concat("is");
	    str->concat(" supported");
	 }
      }

      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), str);
      return 0;
   }

   // get license type
   qore_license_t* module_license = (qore_license_t*)dlsym(ptr, "qore_module_license");
   if (!module_license) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing qore_module_license symbol", path, name);
      return 0;
   }

   // get optional license string
   const char* module_license_str = (const char*)dlsym(ptr, "qore_module_license_str");

   qore_license_t license = *module_license;
   QoreString license_str;
   if (module_license_str)
      license_str = module_license_str;

   //printd(5, "module_license_str: '%s' license_str: '%s'\n", module_license_str ? module_license_str : "n/a", license_str.getBuffer());

   switch (license) {
      case QL_GPL: if (!module_license_str) license_str = "GPL"; break;
      case QL_LGPL: if (!module_license_str) license_str = "LGPL"; break;
      case QL_MIT: if (!module_license_str) license_str = "MIT"; break;
      default:
	 xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': invalid qore_module_license symbol (%d)", path, name, license);
	 return 0;
   }

   if (qore_license != QL_GPL && license == QL_GPL) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': qore library initialized with non-GPL license, but module requires GPL", path, name);
      return 0;
   }

   // get initialization function
   qore_module_init_t* module_init = (qore_module_init_t*)dlsym(ptr, "qore_module_init");
   if (!module_init) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing module init method", path, name);
      return 0;
   }

   // get namespace initialization function
   qore_module_ns_init_t* module_ns_init = (qore_module_ns_init_t*)dlsym(ptr, "qore_module_ns_init");
   if (!module_ns_init) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing namespace init method", path, name);
      return 0;
   }

   // get deletion function
   qore_module_delete_t* module_delete = (qore_module_delete_t*)dlsym(ptr, "qore_module_delete");
   if (!module_delete) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing delete method", path, name);
      return 0;
   }

   // get parse command function
   qore_module_parse_cmd_t* pcmd = (qore_module_parse_cmd_t*)dlsym(ptr, "qore_module_parse_cmd");

   // get qore module description
   const char* desc = (const char*)dlsym(ptr, "qore_module_description");
   if (!desc) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing description", path, name);
      return 0;
   }

   // get qore module version
   const char* version = (const char*)dlsym(ptr, "qore_module_version");
   if (!version) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing version", path, name);
      return 0;
   }

   // get qore module author
   const char* author = (const char*)dlsym(ptr, "qore_module_author");
   if (!author) {
      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': missing author", path, name);
      return 0;
   }

   // get qore module URL (optional)
   const char* url = (const char*)dlsym(ptr, "qore_module_url");

   const char** dep_list = (const char**)dlsym(ptr, "qore_module_dependencies");
   if (dep_list) {
      const char* dep = dep_list[0];
      //printd(5, "dep_list=%p (0=%s)\n", dep_list, dep);
      for (int j = 0; dep; dep = dep_list[++j]) {
	 //printd(5, "loading module dependency=%s\n", dep);
	 loadModuleIntern(xsink, dep, pgm);
	 if (xsink)
	    return 0;
      }
   }

   printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s) %s: calling module_init@%p\n", path, name, *module_init);

   // this is needed for backwards-compatibility for modules that add builtin functions in the module initilization code
   QoreModuleContextHelper qmc(name, pgm, xsink);
   QoreStringNode* str = (*module_init)();
   if (str) {
      // rollback all module changes
      qmc.rollback();

      xsink.raiseExceptionArg("LOAD-MODULE-ERROR", new QoreStringNode(name), "module '%s': feature '%s': initialization error: %s", path, name, str->getBuffer());
      str->deref();
      return 0;
   }

   if (qmc.hasError()) {
      // rollback all module changes
      qmc.rollback();
      return 0;
   }

   // commit all module changes - to the current program or to the static namespace
   qmc.commit();

   mi = new QoreBuiltinModule(0, path, name, desc, version, author, url, license_str, *api_major, *api_minor, *module_init, *module_ns_init, *module_delete, pcmd ? *pcmd : 0, dlh.release());
   QMM.addModule(mi);

   ModuleReExportHelper mrh(mi, reexport);

   printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s) registered '%s'\n", path, name);
   return mi;
}

void QoreModuleManager::delOrig(QoreAbstractModule* mi) {
   const char *n = mi->getOrigName();
   //printd(5, "QoreModuleManager::delOrig() mi: %p '%s' orig: %s\n", mi, mi->getName(), n ? n : "n/a");
   if (!n)
      return;

   module_map_t::iterator i = map.find(n);
   if (i == map.end())
      return;

   mi = i->second;
   delOrig(mi);
   map.erase(i);
   delete mi;
}

// deletes only user modules
void QoreModuleManager::delUser() {
   // first delete user modules in dependency order
   while (!umset.empty()) {
      strset_t::iterator ui = umset.begin();
      module_map_t::iterator i = map.find((*ui).c_str());
      assert(i != map.end());
      QoreAbstractModule* m = i->second;
      assert(m->isUser());
      
      delOrig(m);
      //printd(5, "QoreModuleManager::delUser() deleting '%s' (%s) %p\n", (*ui).c_str(), i->first, m);
      umset.erase(ui);
      removeUserModuleDependency(i->first);
      
      map.erase(i);
      delete m;
   }

#ifdef DEBUG   
   for (module_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i) {
      if (i->second->isUser()) {
	 printd(0, "QoreModuleManager::delUser() '%s' %p not yet removed\n", i->second->getName(), i->second);

	 for (md_map_t::iterator i = md_map.begin(), e = md_map.end(); i != e; ++i) {
	    QoreString str("[");
	    for (strset_t::iterator si = i->second.begin(), se = i->second.end(); si != se; ++si)
	       str.sprintf("'%s',", (*si).c_str());
	    str.concat("]");
	    
	    printd(0, " + md_map '%s' -> %s\n", i->first.c_str(), str.getBuffer());
	 }

	 rmd_map.show();

	 assert(false);
      }
   }
#endif
   
   assert(md_map.empty());
   assert(rmd_map.empty());
}

void QoreModuleManager::cleanup() {
   QORE_TRACE("ModuleManager::cleanup()");

   module_map_t::iterator i;
   while ((i = map.begin()) != map.end()) {
      QoreAbstractModule* m = i->second;
      map.erase(i);
      delete m;
   }
}

void QoreModuleManager::issueParseCmd(const char* mname, QoreProgram* pgm, QoreString& cmd) {
   ExceptionSink xsink;

   AutoLocker al(mutex); // make sure checking and loading are atomic
   loadModuleIntern(xsink, mname, pgm);

   if (xsink) {
      parseException("PARSE-COMMAND-ERROR", loadModuleError(mname, xsink));
      return;
   }

   QoreAbstractModule* mi = findModule(mname);
   assert(mi);

   mi->issueParseCmd(cmd);
}

QoreHashNode* ModuleManager::getModuleHash() {
   return QMM.getModuleHash();
}

QoreHashNode* QoreModuleManager::getModuleHash() {
   bool with_filename = !(runtime_get_parse_options() & PO_NO_EXTERNAL_INFO);
   QoreHashNode* h = new QoreHashNode;
   AutoLocker al(mutex);
   for (module_map_t::const_iterator i = map.begin(); i != map.end(); ++i)
      h->setKeyValue(i->second->getName(), i->second->getHash(with_filename), 0);
   return h;   
}

QoreListNode* ModuleManager::getModuleList() {
   return QMM.getModuleList();
}

QoreListNode* QoreModuleManager::getModuleList() {
   bool with_filename = !(runtime_get_parse_options() & PO_NO_EXTERNAL_INFO);
   QoreListNode* l = new QoreListNode;
   AutoLocker al(mutex);
   for (module_map_t::const_iterator i = map.begin(); i != map.end(); ++i)
      l->push(i->second->getHash(with_filename));
   return l;
}

char version_list_t::set(const char* v) {
   ver = v;

   // set version list
   ver.trim();

   char* a;
   char* p = a = (char*)ver.getBuffer();
   while (*p) {
      if (*p == '.') {
	 char save = *p;
	 *p = '\0';
	 push_back(atoi(a));
	 //printd(5, "this=%p a=%s\n", this, a);
	 *p = save;
	 a = p + 1;
      }
      else if (!isdigit(*p))
	 return *p;
      ++p;
   }
   //printd(5, "this=%p a=%s FINAL\n", this, a);
   push_back(atoi(a));
   return 0;
}
