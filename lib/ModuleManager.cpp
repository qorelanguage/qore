/*
  ModuleManager.cpp

  Qore module manager

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
#include <qore/intern/ModuleInfo.h>
#include <qore/intern/AutoNamespaceList.h>
#include <qore/intern/QoreNamespaceIntern.h>
#include <qore/intern/QoreException.h>

#ifdef NEED_DLFCN_WRAPPER
extern "C" {
#endif
#include <dlfcn.h>
#ifdef NEED_DLFCN_WRAPPER
}
#endif

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

#include <deque>
#include <string>
#include <map>
#include <vector>
#include <set>

#define AUTO_MODULE_DIR MODULE_DIR "/auto"

static const qore_mod_api_compat_s qore_mod_api_list_l[] = { {0, 12}, {0, 11}, {0, 10}, {0, 9}, {0, 8}, {0, 7}, {0, 6}, {0, 5} };
#define QORE_MOD_API_LEN (sizeof(qore_mod_api_list_l)/sizeof(struct qore_mod_api_compat_s))

// public symbols
const qore_mod_api_compat_s *qore_mod_api_list = qore_mod_api_list_l;
const unsigned qore_mod_api_list_len = QORE_MOD_API_LEN;

QoreModuleManager QMM;
ModuleManager MM;

static bool show_errors = false;

typedef std::deque<std::string> strdeque_t;

QoreModuleDefContext::strset_t QoreModuleDefContext::vset;

void QoreModuleContext::error(const char* fmt, ...) {
   va_list args;
   if (!err)
      err = new QoreStringNode;
   else
      err->concat("; ");

   while (true) {
      va_start(args, fmt);
      int rc = err->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
}

void QoreModuleContext::commit() {
   rns->commitModule(*this);

   mcfl.mcfl_t::clear();
   mcnl.mcnl_t::clear();
}

class QoreModuleContextHelper : public QoreModuleContext {
public:
   DLLLOCAL QoreModuleContextHelper(QoreProgram* pgm) : QoreModuleContext(qore_root_ns_private::get(pgm ? *(pgm->getRootNS()) : staticSystemNamespace)) {
      set_module_context(this);
   }
   
   DLLLOCAL ~QoreModuleContextHelper() {
      set_module_context(0);
   }
};

class QoreModuleDefContextHelper : public QoreModuleDefContext {
protected:
   QoreModuleDefContext* old;

public:
   DLLLOCAL QoreModuleDefContextHelper() : old(set_module_def_context(this)) {
   }

   DLLLOCAL ~QoreModuleDefContextHelper() {
      set_module_def_context(old);
   }
};

//! non-thread-safe list of strings of directory names
/** a deque should require fewer memory allocations compared to a linked list
 */
class DirectoryList : public strdeque_t {
   public:
      DLLLOCAL void addDirList(const char *str);
};

static DirectoryList autoDirList, moduleDirList;

void DirectoryList::addDirList(const char *str) {
   if (!str)
      return;

   // duplicate string for invasive searches
   QoreString plist(str);
   str = (char *)plist.getBuffer();

   // add each directory
   while (char *p = (char *)strchr(str, ':')) {
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
      // don't match ':' as the second character in a path as a path separator
      if (p == str + 1) {
	 p = (char *)strchr(p + 1, ':');
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

static void add_prefix(QoreStringNode* err, const char* name) {
   // add "error loading module 'module-name': " to beginning of the string
   err->prepend("': ");
   err->prepend(name);
   err->prepend("error loading module '");
}

QoreStringNode* QoreBuiltinModule::addToProgram(QoreProgram* pgm) const {
   QoreModuleContextHelper qmc(pgm);

   RootQoreNamespace* rns = pgm->getRootNS();
   QoreNamespace* qns = pgm->getQoreNS();

   module_ns_init(rns, qns);

   QoreStringNode* err = qmc.takeError();
   if (err) {
      // rollback all module changes
      qmc.rollback();
      add_prefix(err, name.getBuffer());
      return err;
   }

   // commit all module changes
   qmc.commit();
   pgm->addFeature(name.getBuffer());

   return 0;
}

QoreStringNode* QoreUserModule::addToProgram(QoreProgram* tpgm) const {
   //printd(5, "QoreUserModule::addToProgram() tpgm po: %llx pgm dom: %llx\n", tpgm->getParseOptions64(), qore_program_private::getDomain(*pgm));
   // first check the module's functional domain
   if (tpgm->getParseOptions64() & qore_program_private::getDomain(*pgm))
      return new QoreStringNode("this module implements functionality restricted in the Program object trying to import the module");

   QoreModuleContextHelper qmc(tpgm);

   RootQoreNamespace* rns = tpgm->getRootNS();
   qore_root_ns_private::scanMergeCommittedNamespace(*rns, *(pgm->getRootNS()), qmc);

   QoreStringNode* err = qmc.takeError();
   if (err) {
      // rollback all module changes
      qmc.rollback();
      add_prefix(err, name.getBuffer());
      return err;
   }

   // commit all module changes
   qore_root_ns_private::copyMergeCommittedNamespace(*rns, *(pgm->getRootNS()));
   tpgm->addFeature(name.getBuffer());

   return 0;
}

void QoreBuiltinModule::issueParseCmd(QoreString &cmd) {
   if (!module_parse_cmd) {
      parseException("PARSE-COMMAND-ERROR", "module '%s' loaded from '%s' has not registered a parse command handler", name.getBuffer(), filename.getBuffer());
      return;
   }

   ExceptionSink *pxsink = getProgram()->getParseExceptionSink();
   // if parse exceptions have been disabled, then skip issuing the command
   if (!pxsink)
      return;

   module_parse_cmd(cmd, pxsink);
}

ModuleManager::ModuleManager() {
}

// to add a directory to the module directory search list, can only be called before init()
void ModuleManager::addModuleDir(const char *dir) {
   moduleDirList.push_back(dir);
}

// to add a directory to the auto module directory search list, can only be called before init()
void ModuleManager::addAutoModuleDir(const char *dir) {
   autoDirList.push_back(dir);
}

// to add a list of directories to the module directory search list, can only be called before init()
void ModuleManager::addModuleDirList(const char *strlist) {
   moduleDirList.addDirList(strlist);
}

// to add a list of directories to the auto module directory search list, can only be called before init()
void ModuleManager::addAutoModuleDirList(const char *strlist) {
   autoDirList.addDirList(strlist);
}

// see if name has "-api-<digit(s)>.<digit(s)>" at the end
static bool has_extension(const char *name) {
   const char *p = strstr(name, "-api-");
   if (!p)
      return false;
   p += 5;
   while (isdigit(*p)) ++p;
   if (*p != '.')
      return false;
   return isdigit(*(p + 1));
}

void QoreModuleManager::globDir(const char *dir) {
   // first check modules with extensions indicating compatible apis
   for (unsigned mi = 0; mi <= qore_mod_api_list_len; ++mi) {
      QoreString gstr(dir);
      QoreString ext;

      // make new string for glob
      if (mi < qore_mod_api_list_len)
	 ext.sprintf("-api-%d.%d.qmod", qore_mod_api_list[mi].major, qore_mod_api_list[mi].minor);
      else
	 ext.concat(".qmod");

      gstr.concat("/*");
      gstr.concat(&ext);
 
      glob_t globbuf;
      if (!glob(gstr.getBuffer(), 0, 0, &globbuf)) {
	 for (int i = 0; i < (int)globbuf.gl_pathc; i++) {
	    char *name = q_basename(globbuf.gl_pathv[i]);
	    ON_BLOCK_EXIT(free, name);

	    // see if the name has an api in it (if on the generic case)
	    // and skip it if it does
	    if (mi == qore_mod_api_list_len && has_extension(name))
	       continue;

	    // delete extension from module name for feature matching
	    unsigned len = strlen(name);
	    if (len == ext.strlen()) {
	       printd(5, "QoreModuleManager::globDir() %s has no name, just an extension\n", name);
	       continue;
	    }
	    name[len - ext.strlen()] = '\0';
	    printd(5, "QoreModuleManager::globDir() found %s (%s)\n", globbuf.gl_pathv[i], name);
	    QoreStringNodeHolder errstr(loadBinaryModuleFromPath(globbuf.gl_pathv[i], name));
	    if (errstr && show_errors)
	       fprintf(stderr, "error loading %s\n", errstr->getBuffer());
	 }
      }
      else
	 printd(5, "QoreModuleManager::globDir(): glob(%s) returned an error: %s\n", gstr.getBuffer(), strerror(errno));
      globfree(&globbuf);
   }
}

void QoreModuleManager::init(bool se) {
   static const char *qt_blacklist_string = "because it was implemented with faulty namespace handling that does not work with newer versions of Qore; use the 'qt4' module based in libsmoke instead; it is much more complete";

   // setup possible user module keys
   QoreModuleDefContext::vset.insert("name");
   QoreModuleDefContext::vset.insert("desc");
   QoreModuleDefContext::vset.insert("version");
   QoreModuleDefContext::vset.insert("author");
   QoreModuleDefContext::vset.insert("url");

   mutex = new QoreThreadLock(&ma_recursive);

   // initialize blacklist
   // add old QT modules to blacklist
   mod_blacklist.insert(std::make_pair((const char *)"qt-core", qt_blacklist_string));
   mod_blacklist.insert(std::make_pair((const char *)"qt-gui", qt_blacklist_string));
   mod_blacklist.insert(std::make_pair((const char *)"qt-svn", qt_blacklist_string));
   mod_blacklist.insert(std::make_pair((const char *)"qt-opengl", qt_blacklist_string));

   show_errors = se;

   // set up auto-load list from QORE_AUTO_MODULE_DIR (if it hasn't already been manually set up)
   if (autoDirList.empty()) {
      autoDirList.addDirList(getenv("QORE_AUTO_MODULE_DIR"));

      // append standard directories to the end of the list
      //autoDirList.push_back(AUTO_MODULE_DIR);

      QoreString str(MODULE_DIR);
      str.concat("/auto");
      //printd(5, "adding auto module dir %s\n", str.getBuffer());
      autoDirList.push_back(str.getBuffer());
   }

   // setup module directory list from QORE_MODULE_DIR (if it hasn't already been manually set up)
   if (moduleDirList.empty()) {
      moduleDirList.addDirList(getenv("QORE_MODULE_DIR"));

      // append standard directories to the end of the list
      // moduleDirList.push_back(MODULE_DIR);
      
      // append qore module directory
      moduleDirList.push_back(MODULE_DIR);
   }

   // autoload modules
   // try to load all modules in each directory in the autoDirList
   QoreString gstr;

   DirectoryList::iterator w = autoDirList.begin();
   while (w != autoDirList.end()) {
      globDir((*w).c_str());
      ++w;
   }
}

int ModuleManager::runTimeLoadModule(const char *name, ExceptionSink *xsink) {
   return QMM.runTimeLoadModule(name, xsink);
}

int QoreModuleManager::runTimeLoadModule(const char *name, ExceptionSink *xsink) {
   QoreProgram *pgm = getProgram();

   // grab the parse lock
   SafeLocker sl(pgm->getParseLock());

   QoreStringNode *err = parseLoadModuleIntern(name, pgm);
   sl.unlock();
   if (err) {
      xsink->raiseException("LOAD-MODULE-ERROR", err);
      return -1;
   }
   return 0;
}

static const char *get_op_string(mod_op_e op) {
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

static QoreStringNode *check_qore_version(const char *name, mod_op_e op, version_list_t& version) {
   unsigned max = version.size() > 3 ? version.size() : 3;
   for (unsigned i = 0; i < max; ++i) {
      int mv = (!i ? QORE_VERSION_MAJOR : 
		(i == 1 ? QORE_VERSION_MINOR : 
		 (i == 2 ? QORE_VERSION_SUB : 0)));
      int rv = (i >= version.size() ? 0 : version[i]);
      int res = check_component(op, mv, rv, i == (max - 1));
      if (res == MVC_FAIL) {
	 QoreStringNode *err = new QoreStringNode;
	 err->sprintf("feature '%s' is built in, but the following version requirement is not satisfied: Qore library %s %s %s", name, QORE_VERSION, get_op_string(op), *version);
	 return err;
      }
      if (res == MVC_FINAL_OK)
	 break;
   }
   return 0;
}

static QoreStringNode *check_module_version(QoreAbstractModule* mi, mod_op_e op, version_list_t& version) {
   unsigned max = version.size() > mi->version_list.size() ? version.size() : mi->version_list.size();
   //printd(5, "check_module_version(%s %s %s) max=%d vs=%d ms=%d\n", mi->getVersion(), get_op_string(op), version->getString(), max, version->size(), mi->version_list.size());
   for (unsigned i = 0; i < max; ++i) {
      int mv = (i >= mi->version_list.size() ? 0 : mi->version_list[i]);
      int rv = (i >= version.size() ? 0 : version[i]);
      int res = check_component(op, mv, rv, i == (max - 1));
      if (res == MVC_FAIL) {
	 QoreStringNode *err = new QoreStringNode;
	 err->sprintf("loaded module '%s' does not satisfy the following requirement: %s %s %s", mi->getName(), mi->getVersion(), get_op_string(op), *version);
	 return err;
      }
      if (res == MVC_FINAL_OK)
	 break;
   }
   return 0;
}

static QoreStringNode* qore_check_load_module_intern(QoreAbstractModule *mi, mod_op_e op, version_list_t* version, QoreProgram* pgm) {
   // check version if necessary
   if (version) {
      QoreStringNode *err = check_module_version(mi, op, *version);
      if (err)
	 return err;
   }

   return pgm ? mi->addToProgram(pgm) : 0;
}

QoreStringNode* QoreModuleManager::loadModuleIntern(const char *name, QoreProgram *pgm, mod_op_e op, version_list_t *version) {
   assert(!version || (version && op != MOD_OP_NONE));

   // check for special "qore" feature
   if (!strcmp(name, "qore")) {
      if (version)
	 return check_qore_version(name, op, *version); 
      return 0;
   }

   // if the feature already exists in this program, then return
   if (pgm && pgm->checkFeature(name)) {
      // check version if necessary
      if (version) {
	 QoreAbstractModule* mi = findModuleUnlocked(name);
	 // if no module is found, then this is a builtin feature
	 if (!mi)
	    return check_qore_version(name, op, *version);
	 return check_module_version(mi, op, *version);
      }
      return 0;
   }

   // check if parse options allow loading any modules at all
   if (pgm && (pgm->getParseOptions64() & PO_NO_MODULES)) {
      QoreStringNode* err = new QoreStringNode;
      err->sprintf("cannot load modules ('%s' requested) into the current Program object because PO_NO_MODULES is set", name);
      return err;
   }   

   // if the feature already exists, then load the namespace changes into this program and register the feature
   QoreAbstractModule* mi = findModuleUnlocked(name);

   if (mi)
      return qore_check_load_module_intern(mi, op, version, pgm);

   //printd(5, "QoreModuleManager::loadModuleIntern() this: %p name: %s not found\n", this, name);

   QoreStringNode *errstr;

   // see if this is actually a path
   if (strchr(name, QORE_DIR_SEP)) {
      // see if it's a user or binary module
      size_t len = strlen(name);
      errstr = (len > 3 && !strcasecmp(".qm", name + len - 3))
	 ? loadUserModuleFromPath(name, 0, &mi)
	 : loadBinaryModuleFromPath(name, 0, &mi, pgm);

      if (errstr)
	 return errstr;

      assert(mi);
      return qore_check_load_module_intern(mi, op, version, pgm);
   }

   // otherwise, try to find module in the module path
   QoreString str;
   struct stat sb;

   DirectoryList::iterator w = moduleDirList.begin();
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
	    
	 //printd(0, "ModuleManager::loadModule(%s) trying binary module: %s\n", name, str.getBuffer());
	 if (!stat(str.getBuffer(), &sb)) {
	    printd(5, "ModuleManager::loadModule(%s) found binary module: %s\n", name, str.getBuffer());
	    if ((errstr = loadBinaryModuleFromPath(str.getBuffer(), name, &mi, pgm)))
	       return errstr;

	    return qore_check_load_module_intern(mi, op, version, pgm);
	 }

	 // build path to user module
	 str.clear();
	 str.sprintf("%s" QORE_DIR_SEP_STR "%s.qm", (*w).c_str(), name);
	    
	 //printd(0, "ModuleManager::loadModule(%s) trying user module: %s\n", name, str.getBuffer());
	 if (!stat(str.getBuffer(), &sb)) {
	    printd(0, "ModuleManager::loadModule(%s) found user module: %s\n", name, str.getBuffer());
	    if ((errstr = loadUserModuleFromPath(str.getBuffer(), name, &mi)))
	       return errstr;

	    assert(mi);
	    return qore_check_load_module_intern(mi, op, version, pgm);
	 }
      }

      w++;
   }
   
   errstr = new QoreStringNode;
   errstr->sprintf("feature '%s' is not builtin and no module with this name could be found in the module path", name);
   return errstr;
}

QoreStringNode *ModuleManager::parseLoadModule(const char *name, QoreProgram *pgm) {
   return QMM.parseLoadModule(name, pgm);
}

QoreStringNode *QoreModuleManager::parseLoadModule(const char *name, QoreProgram *pgm) {
   //printd(5, "ModuleManager::parseLoadModule(name=%s, pgm=%p)\n", name, pgm);

   char *p = strchrs(name, "<>=");
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
	 QoreStringNode *err = new QoreStringNode;
	 err->sprintf("cannot parse module operator '%s'; expecting one of: '<', '<=', '=', '>=', or '>'", op.getBuffer());
	 return err;
      }

      version_list_t iv;
      char ec = iv.set(p);
      if (ec) {
	 QoreStringNode *err = new QoreStringNode;
	 err->sprintf("only numeric digits and '.' characters are allowed in module/feature version specifications, got '%c'", ec);
	 return err;
      }

      if (!iv.size())
	 return new QoreStringNode("empty version specification given in feature/module request");

      AutoLocker al(mutex); // make sure checking and loading are atomic
      return loadModuleIntern(str.getBuffer(), pgm, mo, &iv);
   }

   AutoLocker sl(mutex); // make sure checking and loading are atomic
   return loadModuleIntern(name, pgm);
}

QoreStringNode* QoreModuleManager::loadUserModuleFromPath(const char *path, const char *feature, QoreAbstractModule **mip) {
   QoreAbstractModule *mi = 0;
   if (mip)
      *mip = 0;

   ExceptionSink xsink;
   ReferenceHolder<QoreProgram> pgm(new QoreProgram(USER_MOD_PO), &xsink);

   QoreModuleDefContextHelper qmd;
   pgm->parseFile(path, &xsink);

   if (xsink) {
      QoreException* qe = xsink.catchException();

      QoreException* oqe = qe;

      QoreStringNode* rv = new QoreStringNodeMaker("failed to load user module '%s':\n", feature ? feature : path);
      while (qe) {
	 QoreStringNodeValueHelper err(qe->err);
	 QoreStringNodeValueHelper desc(qe->desc);

	 rv->concat(" * ");
	 if (!qe->file.empty())
	    rv->sprintf("%s:", qe->file.c_str());
	 if (qe->start_line)
	    rv->sprintf("%d-%d: ", qe->start_line, qe->end_line);
	 rv->sprintf("%s: %s\n", err->getBuffer(), desc->getBuffer());

	 qe = qe->next;
      }

      oqe->del(&xsink);

      return rv;
   }

   const char* name = qmd.get("name");

   if (!name)
      return new QoreStringNodeMaker("module '%s': no feature name present in module", path);

   if (feature && strcmp(feature, name))
      return new QoreStringNodeMaker("module '%s': provides feature '%s', expecting feature '%s', skipping, rename module to %s.qm to load", path, name, feature, name);

   // get qore module description
   const char* desc = qmd.get("desc");
   if (!desc)
      return new QoreStringNodeMaker("module '%s': feature '%s': missing description", path, name);

   // get qore module version
   const char* version = qmd.get("version");
   if (!version)
      return new QoreStringNodeMaker("module '%s': feature '%s': missing version", path, name);

   // get qore module author
   const char* author = qmd.get("author");
   if (!author)
      return new QoreStringNodeMaker("module '%s': feature '%s': missing author", path, name);
 
   const char* url = qmd.get("url");

   // xxx add to auto namespace list
   //ANSL.add(*pgm);

   mi = new QoreUserModule(path, name, desc, version, author, url, pgm.release());
   QMM.addModule(mi);

   qoreFeatureList.push_back(name);

   if (mip)
      *mip = mi;
   return 0;
}

QoreStringNode* QoreModuleManager::loadBinaryModuleFromPath(const char *path, const char *feature, QoreAbstractModule **mip, QoreProgram *pgm) {
   QoreAbstractModule *mi = 0;
   if (mip)
      *mip = 0;

   void *ptr = dlopen(path, RTLD_LAZY|RTLD_GLOBAL);
   if (!ptr)
      return new QoreStringNodeMaker("error loading qore module '%s': %s", path, dlerror());

   // get module name
   const char *name = (const char *)dlsym(ptr, "qore_module_name");
   if (!name) {
      dlclose(ptr);
      return new QoreStringNodeMaker("module '%s': no feature name present in module", path);
   }

   QoreStringNode *str = 0;
   // ensure provided feature matches with expected feature
   if (feature && strcmp(feature, name)) {
      str = new QoreStringNodeMaker("module '%s': provides feature '%s', expecting feature '%s', skipping, rename module to %s.qmod to load", path, name, feature, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // see if a module with this name is already registered
   if ((mi = findModuleUnlocked(name))) {
      str = new QoreStringNodeMaker("module '%s': feature '%s' already registered by '%s'", path, name, mi->getFileName());
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // check if it's been blacklisted
   bl_map_t::const_iterator i = mod_blacklist.find(name);
   if (i != mod_blacklist.end()) {
      str = new QoreStringNodeMaker("module '%s': '%s' is blacklisted %s", path, name, i->second);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module API major number
   int *api_major = (int *)dlsym(ptr, "qore_module_api_major");
   if (!api_major) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': no qore module API major number", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module API minor number
   int *api_minor = (int *)dlsym(ptr, "qore_module_api_minor");
   if (!api_minor) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': no qore module API minor number", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   if (!is_module_api_supported(*api_major, *api_minor)) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': API mismatch, module supports API %d.%d, however only version", path, name, *api_major, *api_minor);

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

      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get license type
   qore_license_t *module_license = (qore_license_t *)dlsym(ptr, "qore_module_license");
   if (!module_license) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': missing qore_module_license symbol", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   qore_license_t license = *module_license;
   if (license != QL_GPL && license != QL_LGPL) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': invalid qore_module_license symbol (%d)", path, name, license);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   if (qore_license != QL_GPL && license == QL_GPL) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': qore library initialized with LGPL license, but module requires GPL", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get initialization function
   qore_module_init_t *module_init = (qore_module_init_t *)dlsym(ptr, "qore_module_init");
   if (!module_init) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': claims API major=%d, %d required", path, name, *api_major, QORE_MODULE_API_MAJOR);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get namespace initialization function
   qore_module_ns_init_t *module_ns_init = (qore_module_ns_init_t *)dlsym(ptr, "qore_module_ns_init");
   if (!module_ns_init) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': missing namespace init method", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get deletion function
   qore_module_delete_t *module_delete = (qore_module_delete_t *)dlsym(ptr, "qore_module_delete");
   if (!module_delete) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': missing delete method", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get parse command function
   qore_module_parse_cmd_t *pcmd = (qore_module_parse_cmd_t *)dlsym(ptr, "qore_module_parse_cmd");

   // get qore module description
   char *desc = (char *)dlsym(ptr, "qore_module_description");
   if (!desc) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': missing description", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module version
   char *version = (char *)dlsym(ptr, "qore_module_version");
   if (!version) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': missing version", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module author
   char *author = (char *)dlsym(ptr, "qore_module_author");
   if (!author) {
      str = new QoreStringNodeMaker("module '%s': feature '%s': missing author", path, name);
      dlclose(ptr);
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module URL (optional)
   char *url = (char *)dlsym(ptr, "qore_module_url");

   char **dep_list = (char **)dlsym(ptr, "qore_module_dependencies");
   if (dep_list) {
      char *dep = dep_list[0];
      //printd(5, "dep_list=%08p (0=%s)\n", dep_list, dep);
      for (int j = 0; dep; dep = dep_list[++j]) {
	 //printd(5, "loading module dependency=%s\n", dep);
	 str = loadModuleIntern(dep, pgm);
	 if (str) {
	    str->replace(0, 0, "': ");
	    str->replace(0, 0, dep);
	    str->replace(0, 0, "' module dependency '");
	    str->replace(0, 0, name);
	    str->replace(0, 0, "error loading '");
	    return str;
	 }
      }
   }

   printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s) %s: calling module_init@%08p\n", path, name, *module_init);

   // this is needed for backwards-compatibility for modules that add builtin functions in the module initilization code
   QoreModuleContextHelper qmc(pgm);
   str = (*module_init)();
   QoreStringNode* err = qmc.takeError();
   if (err) {
      if (str) {
	 str->concat("; ");
	 str->concat(err->getBuffer(), err->size());
	 err->deref();
      }
      else
	 str = err;
   }
   
   if (str) {
      // rollback all module changes
      qmc.rollback();
      add_prefix(str, mi->getName());
      return str;
   }

   // commit all module changes - to the current program or to the static namespace
   qmc.commit();

   // run initialization
   if (str) {
      printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s): '%s': qore_module_init returned error: %s", path, name, str->getBuffer());
      QoreString edesc;
      edesc.sprintf("module '%s': feature '%s': ", path, name);
      dlclose(ptr);
      // insert text at beginning of string
      str->prepend(edesc.getBuffer());
      return str;
   }

   mi = new QoreBuiltinModule(path, name, desc, version, author, url, *api_major, *api_minor, *module_init, *module_ns_init, *module_delete, pcmd ? *pcmd : 0, ptr);
   QMM.addModule(mi);

   // add to auto namespace list
   ANSL.add(*module_ns_init);
   qoreFeatureList.push_back(name);
   printd(5, "QoreModuleManager::loadBinaryModuleFromPath(%s) registered '%s'\n", path, name);
   if (mip)
      *mip = mi;
   return 0;
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

void QoreModuleManager::issueParseCmd(const char *mname, QoreProgram *pgm, QoreString &cmd) {
   QoreStringNode *err = ModuleManager::parseLoadModule(mname, pgm);
   if (err) {
      parseException("PARSE-COMMAND-ERROR", err);
      return;
   }
   
   QoreAbstractModule* mi = findModule(mname);
   assert(mi);

   mi->issueParseCmd(cmd);
}

QoreHashNode *ModuleManager::getModuleHash() {
   return QMM.getModuleHash();
}

QoreHashNode *QoreModuleManager::getModuleHash() {
   QoreHashNode *h = new QoreHashNode;
   AutoLocker al(mutex);
   for (module_map_t::const_iterator i = map.begin(); i != map.end(); i++)
      h->setKeyValue(i->second->getName(), i->second->getHash(), 0);
   return h;   
}

QoreListNode *ModuleManager::getModuleList() {
   return QMM.getModuleList();
}

QoreListNode *QoreModuleManager::getModuleList() {
   QoreListNode *l = new QoreListNode;
   AutoLocker al(mutex);
   for (module_map_t::const_iterator i = map.begin(); i != map.end(); i++)
      l->push(i->second->getHash());
   return l;
}

char version_list_t::set(const char *v) {
   ver = v;

   // set version list
   ver.trim();

   char *a;
   char *p = a = (char *)ver.getBuffer();
   while (*p) {
      if (*p == '.') {
	 char save = *p;
	 *p = '\0';
	 push_back(atoi(a));
	 //printd(0, "this=%p a=%s\n", this, a);
	 *p = save;
	 a = p + 1;
      }
      else if (!isdigit(*p))
	 return *p;
      p++;
   }
   //printd(0, "this=%p a=%s FINAL\n", this, a);
   push_back(atoi(a));
   return 0;
}
