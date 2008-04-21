/*
  ModuleManager.cc

  Qore module manager

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <glob.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include <deque>
#include <string>
#include <map>

#define AUTO_MODULE_DIR MODULE_DIR "/auto"

ModuleManager MM;

typedef std::map<const char *, ModuleInfo *, class ltstr> module_map_t;
static module_map_t map;

static bool show_errors = false;
static QoreThreadLock mutex;

#ifdef QORE_MONOLITHIC
// for non-shared builds of the qore library, initialize all optional components here
# ifdef TIBRV
#  include "../modules/tibrv/tibrv-module.h"
# endif
# ifdef TIBAE
#  include "../modules/tibae/tibae-module.h"
# endif
# ifdef ORACLE
#  include "../modules/oracle/oracle-module.h"
# endif
# ifdef QORE_MYSQL
#  include "../modules/mysql/qore-mysql-module.h"
# endif
# ifdef NCURSES
# include "../modules/ncurses/ncurses-module.h"
# endif
#endif

typedef std::deque<std::string> strdeque_t;

//! non-thread-safe list of strings of directory names
/** a deque should require fewer memory allocations compared to a linked list
 */
class DirectoryList : public strdeque_t
{
   public:
      DLLLOCAL void addDirList(const char *str);
};

static DirectoryList autoDirList, moduleDirList;

void DirectoryList::addDirList(const char *str)
{
   if (!str)
      return;

   // duplicate string for invasive searches
   QoreString plist(str);
   str = (char *)plist.getBuffer();

   // add each directory
   while (char *p = (char *)strchr(str, ':'))
   {
      if (p != str)
      {
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

ModuleInfo::ModuleInfo(const char *fn, const char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, const char *d, const char *v, const char *a, const char *u, const void *p)
{
   filename = strdup(fn);
   name = n;
   api_major = major;
   api_minor = minor;
   module_init = init;
   module_ns_init = ns_init;
   module_delete = del;
   desc = d;
   version = v;
   author = a;
   url = u;
   dlptr = p;
}

// builtin module info node - when features are compiled into the library
ModuleInfo::ModuleInfo(const char *fn, qore_module_delete_t del)
{
   filename = "<builtin>";
   name = fn;
   api_major = QORE_MODULE_API_MAJOR;
   api_minor = QORE_MODULE_API_MINOR;
   module_init = 0;
   module_ns_init = 0;
   module_delete = del;
   version = desc = author = url = "<builtin>";
   dlptr = 0;
}

ModuleInfo::~ModuleInfo()
{
   printd(5, "ModuleInfo::~ModuleInfo() '%s': %s calling module_delete=%08p\n", name, filename, module_delete);
   module_delete();
   if (dlptr)
   {
      printd(5, "calling dlclose(%08p)\n", dlptr);
#ifndef DEBUG
      // do not close modules when debugging
      dlclose((void *)dlptr);
#endif
      free(filename);
   }
}

const char *ModuleInfo::getName() const
{
   return name;
}

const char *ModuleInfo::getFileName() const
{
   return filename;
}

const char *ModuleInfo::getDesc() const
{
   return desc;
}

const char *ModuleInfo::getVersion() const
{
   return version;
}

const char *ModuleInfo::getURL() const
{
   return url;
}

int ModuleInfo::getAPIMajor() const
{
   return api_major;
}

int ModuleInfo::getAPIMinor() const
{
   return api_minor;
}

void ModuleInfo::ns_init(class QoreNamespace *rns, class QoreNamespace *qns) const
{
   module_ns_init(rns, qns);
}

bool ModuleInfo::isBuiltin() const
{
   return !dlptr;
}

QoreHashNode *ModuleInfo::getHash() const
{
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("filename", new QoreStringNode(filename), 0);
   h->setKeyValue("name", new QoreStringNode(name), 0);
   h->setKeyValue("desc", new QoreStringNode(desc), 0);
   h->setKeyValue("version", new QoreStringNode(version), 0);
   h->setKeyValue("author", new QoreStringNode(author), 0);
   h->setKeyValue("api_major", new QoreBigIntNode(api_major), 0);
   h->setKeyValue("api_minor", new QoreBigIntNode(api_minor), 0);
   if (url)
      h->setKeyValue("url", new QoreStringNode(url), 0);
   return h;
}

ModuleManager::ModuleManager()
{
}

void ModuleManager::add(ModuleInfo *m)
{
   map.insert(std::make_pair(m->getName(), m));
}

class ModuleInfo *ModuleManager::add(const char *fn, char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, char *d, char *v, char *a, char *u, void *p)
{
   class ModuleInfo *m = new ModuleInfo(fn, n, major, minor, init, ns_init, del, d, v, a, u, p);
   add(m);
   return m;
}

class ModuleInfo *ModuleManager::find(const char *name)
{
   module_map_t::iterator i = map.find(name);
   if (i == map.end())
      return 0;

   return i->second;
}

void ModuleManager::addBuiltin(const char *fn, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del)
{
   QoreStringNodeHolder str(init());
   if (str)
   {
      fprintf(stderr, "WARNING! cannot initialize builtin feature '%s': %s\n", fn, str->getBuffer());
      return;
   }
   // "num" is not incremented here - only incremented for real modules
   add(new ModuleInfo(fn, del));
   ANSL.add(ns_init);
}

// to add a directory to the module directory search list, can only be called before init()
void ModuleManager::addModuleDir(const char *dir)
{
   moduleDirList.push_back(dir);
}

// to add a directory to the auto module directory search list, can only be called before init()
void ModuleManager::addAutoModuleDir(const char *dir)
{
   autoDirList.push_back(dir);
}

// to add a list of directories to the module directory search list, can only be called before init()
void ModuleManager::addModuleDirList(const char *strlist)
{
   moduleDirList.addDirList(strlist);
}

// to add a list of directories to the auto module directory search list, can only be called before init()
void ModuleManager::addAutoModuleDirList(const char *strlist)
{
   autoDirList.addDirList(strlist);
}

void ModuleManager::init(bool se)
{
   show_errors = se;

   // set up auto-load list from QORE_AUTO_MODULE_DIR (if it hasn't already been manually set up)
   if (autoDirList.empty())
   {
      autoDirList.addDirList(getenv("QORE_AUTO_MODULE_DIR"));
      // append standard directories to the end of the list
      autoDirList.push_back(AUTO_MODULE_DIR);
   }

   // setup module directory list from QORE_MODULE_DIR (if it hasn't already been manually set up)
   if (moduleDirList.empty())
   {
      moduleDirList.addDirList(getenv("QORE_MODULE_DIR"));
      // append standard directories to the end of the list
      moduleDirList.push_back(MODULE_DIR);
   }

#ifdef QORE_MONOLITHIC
# ifdef NCURSES
   addBuiltin("ncurses", ncurses_module_init, ncurses_module_ns_init, ncurses_module_delete);
# endif
# ifdef ORACLE
   addBuiltin("oracle", oracle_module_init, oracle_module_ns_init, oracle_module_delete);
# endif
# ifdef QORE_MYSQL
   addBuiltin("mysql", qore_mysql_module_init, qore_mysql_module_ns_init, qore_mysql_module_delete);
# endif
# ifdef TIBRV
   addBuiltin("tibrv", tibrv_module_init, tibrv_module_ns_init, tibrv_module_delete);
# endif
# ifdef TIBAE
   addBuiltin("tibae", tibae_module_init, tibae_module_ns_init, tibae_module_delete);
# endif
#endif
   // autoload modules
   // try to load all modules in each directory in the autoDirList
   QoreString gstr;

   DirectoryList::iterator w = autoDirList.begin();
   while (w != autoDirList.end())
   {
      // make new string for glob
      gstr.clear();
      gstr.concat((*w).c_str());
      gstr.concat("/*.qmod");

      glob_t globbuf;
      if (!glob(gstr.getBuffer(), 0, 0, &globbuf))
      {
	 for (int i = 0; i < (int)globbuf.gl_pathc; i++)
	 {
	    char *name = q_basename(globbuf.gl_pathv[i]);
	    // delete ".qmod" from module name for feature matching
	    char *p = strrchr(name, '.');
	    if (p)
	      *p = '\0';
	    QoreStringNodeHolder errstr(loadModuleFromPath(globbuf.gl_pathv[i], name));
	    if (errstr && show_errors)
	       fprintf(stderr, "error loading %s\n", errstr->getBuffer());
	    free(name);
	 }
      }
      else
	 printd(1, "ModuleManager::init(): glob(%s) returned an error: %s\n", gstr.getBuffer(), strerror(errno));
      globfree(&globbuf);
      w++;
   }
}

int ModuleManager::runTimeLoadModule(const char *name, ExceptionSink *xsink)
{
   QoreProgram *pgm = getProgram();

   // grab the parse lock
   SafeLocker sl(pgm->getParseLock());

   QoreStringNode *err = parseLoadModule(name, pgm);
   sl.unlock();
   if (err) {
      xsink->raiseException("LOAD-MODULE-ERROR", err);
      return -1;
   }
   return 0;
}

QoreStringNode *ModuleManager::loadModuleIntern(const char *name, QoreProgram *pgm)
{
   // if the feature already exists in this program, then return
   if (pgm && !pgm->checkFeature(name))
      return 0;

   // if the feature already exists, then load the namespace changes into this program and register the feature
   ModuleInfo *mi = find(name);
   if (mi) {
      if (pgm) {
	 mi->ns_init(pgm->getRootNS(), pgm->getQoreNS());
	 pgm->addFeature(mi->getName());
      }
      return 0;
   }

   QoreStringNode *errstr;

   // see if this is actually a path
   if (name[0] == '/') {
      if ((errstr = loadModuleFromPath(name, 0, &mi, pgm)))
	 return errstr;
      
      if (pgm) {
	 mi->ns_init(pgm->getRootNS(), pgm->getQoreNS());
	 pgm->addFeature(mi->getName());
      }
      return 0;
   }

   // otherwise, try to find module in the module path
   QoreString str;
   struct stat sb;

   DirectoryList::iterator w = moduleDirList.begin();
   while (w != moduleDirList.end()) {
      str.clear();
      str.sprintf("%s/%s.qmod", (*w).c_str(), name);
      //printd(5, "ModuleManager::loadModule(%s) trying %s\n", name, str.getBuffer());

      if (!stat(str.getBuffer(), &sb)) {
	 if ((errstr = loadModuleFromPath(str.getBuffer(), name, &mi, pgm)))
	    return errstr;

	 if (pgm) {
	    mi->ns_init(pgm->getRootNS(), pgm->getQoreNS());
	    pgm->addFeature(mi->getName());
	 }
	 return 0;
      }
      w++;
   }
   
   errstr = new QoreStringNode;
   errstr->sprintf("feature '%s' is not builtin and no module with this name could be found in the module path", name);
   return errstr;
}

QoreStringNode *ModuleManager::parseLoadModule(const char *name, QoreProgram *pgm)
{
   SafeLocker sl(&mutex); // make sure checking and loading are atomic

   return loadModuleIntern(name, pgm);
}

QoreStringNode *ModuleManager::loadModuleFromPath(const char *path, const char *feature, class ModuleInfo **mip, QoreProgram *pgm)
{
   class ModuleInfo *mi = 0;
   if (mip)
      *mip = 0;

   QoreStringNode *str = 0;
   void *ptr = dlopen(path, RTLD_LAZY|RTLD_GLOBAL);
   if (!ptr) {
      str = new QoreStringNode();
      str->sprintf("error loading qore module '%s': %s", path, dlerror());
      printd(5, "%s\n", str->getBuffer());
      return str;
   }
   // get module name
   char *name = (char *)dlsym(ptr, "qore_module_name");
   if (!name) {
      str = new QoreStringNode();
      str->sprintf("module '%s': no feature name present in module", path);
      dlclose(ptr);
      printd(5, "%s\n", str->getBuffer());
      return str;
   }
   // ensure provided feature matches with expected feature
   if (feature && strcmp(feature, name)) {
      str = new QoreStringNode();
      str->sprintf("module '%s': provides feature '%s', expecting feature '%s', skipping, rename module to %s.qmod to load", path, name, feature, feature);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // see if a module with this name is already registered
   if ((mi = find(name))) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s' already registered by '%s'", path, name, mi->getFileName());
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }
   // get qore module API major number
   int *api_major = (int *)dlsym(ptr, "qore_module_api_major");
   if (!api_major) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': no qore module API major number", path, name);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module API minor number
   int *api_minor = (int *)dlsym(ptr, "qore_module_api_minor");
   if (!api_minor) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': no qore module API minor number", path, name);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   if (*api_major != QORE_MODULE_API_MAJOR || (!(*api_major) && *api_minor != QORE_MODULE_API_MINOR)) {
      str = new QoreStringNode();
#if QORE_MODULE_API_MAJOR > 0
      str->sprintf("module '%s': feature '%s': API mismatch, module supports API %d.%d, however the minimum supported version is %d.0 is required", path, name, *api_major, *api_minor, QORE_MODULE_API_MAJOR);
#else
      str->sprintf("module '%s': feature '%s': API mismatch, module supports API %d.%d, however %d.%d is required", path, name, *api_major, *api_minor, QORE_MODULE_API_MAJOR, QORE_MODULE_API_MINOR);
#endif
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get initialization function
   qore_module_init_t *module_init = (qore_module_init_t *)dlsym(ptr, "qore_module_init");
   if (!module_init) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': claims API major=%d, %d required", path, name, *api_major, QORE_MODULE_API_MAJOR);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get namespace initialization function
   qore_module_ns_init_t *module_ns_init = (qore_module_ns_init_t *)dlsym(ptr, "qore_module_ns_init");
   if (!module_ns_init) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': missing namespace init method", path, name);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get deletion function
   qore_module_delete_t *module_delete = (qore_module_delete_t *)dlsym(ptr, "qore_module_delete");
   if (!module_delete) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': missing delete method", path, name);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module description
   char *desc = (char *)dlsym(ptr, "qore_module_description");
   if (!desc) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': missing description", path, name);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module version
   char *version = (char *)dlsym(ptr, "qore_module_version");
   if (!version) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': missing version", path, name);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module author
   char *author = (char *)dlsym(ptr, "qore_module_author");
   if (!author) {
      str = new QoreStringNode();
      str->sprintf("module '%s': feature '%s': missing author", path, name);
      dlclose(ptr);
      printd(5, "ModuleManager::loadModuleFromPath() error: %s\n", str->getBuffer());
      return str;
   }

   // get qore module URL (optional)
   char *url = (char *)dlsym(ptr, "qore_module_url");

   char **dep_list = (char **)dlsym(ptr, "qore_module_dependencies");
   if (dep_list) {
      char *dep = dep_list[0];
      //printd(5, "dep_list=%08p (0=%s)\n", dep_list, dep);
      for (int i = 0; dep; dep = dep_list[++i]) {
	 //printd(5, "loading module dependency=%s\n", dep);
	 str = loadModuleIntern(dep, pgm);
	 if (str) {
	    str->replace(0, 0, "\": ");
	    str->replace(0, 0, dep);
	    str->replace(0, 0, "error loading module dependency \"");
	    return str;
	 }
      }
   }

   printd(5, "ModuleManager::loadModuleFromPath(%s) %s: calling module_init@%08p\n", path, name, *module_init);

   str = (*module_init)();
   // run initialization
   if (str) {
      printd(5, "ModuleManager::loadModuleFromPath(%s): '%s': qore_module_init returned error: %s", path, name, str->getBuffer());
      QoreString desc;
      desc.sprintf("module '%s': feature '%s': ", path, name);
      dlclose(ptr);
      // insert text at beginning of string
      str->replace(0, 0, desc.getBuffer());
      return str;
   }

   mi = MM.add(path, name, *api_major, *api_minor, *module_init, *module_ns_init, *module_delete, desc, version, author, url, ptr);
   // add to auto namespace list
   ANSL.add(*module_ns_init);
   qoreFeatureList.push_back(name);
   printd(5, "ModuleManager::loadModuleFromPath(%s) registered '%s'\n", path, name);
   if (mip)
      *mip = mi;
   return 0;
}

void ModuleManager::cleanup()
{
   tracein("ModuleManager::cleanup()");

   module_map_t::iterator i;
   while ((i = map.begin()) != map.end())
   {
      class ModuleInfo *m = i->second;
      map.erase(i);
      delete m;
   }

   traceout("ModuleManager::cleanup()");
}

QoreListNode *ModuleManager::getModuleList()
{
   QoreListNode *l = 0;
   AutoLocker al(&mutex);
   if (!map.empty())
   {
      l = new QoreListNode();
      for (module_map_t::iterator i = map.begin(); i != map.end(); i++)
	 if (!i->second->isBuiltin())
	    l->push(i->second->getHash());
   }
   return l;
}
