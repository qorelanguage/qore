/*
  ModuleManager.cc

  Qore module manager

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/ModuleManager.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/List.h>
#include <qore/Hash.h>
#include <qore/module.h>
#include <qore/QoreLib.h>
#include <qore/AutoNamespaceList.h>
#include <qore/QoreProgram.h>

#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <glob.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define AUTO_MODULE_DIR MODULE_DIR "/auto"

class ModuleManager MM;

#ifdef QORE_MONOLITHIC
// for non-shared builds of the qore library, initialize all optional components here
# ifdef TIBCO
#  include "../modules/TIBCO/tibco-module.h"
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

ModuleManager::ModuleManager()
{
   head = NULL;
   num = 0;
}

void ModuleManager::init(bool se)
{
   show_errors = se;

   // set up auto-load list from QORE_AUTO_MODULE_DIR
   autoDirList.addDirList(getenv("QORE_AUTO_MODULE_DIR"));

   // setup module directory list from QORE_MODULE_DIR
   moduleDirList.addDirList(getenv("QORE_MODULE_DIR"));

   // append standard directories to the end of the list
   autoDirList.append(strdup(AUTO_MODULE_DIR));
   moduleDirList.append(strdup(MODULE_DIR));

#ifdef QORE_MONOLITHIC
# ifdef NCURSES
   ncurses_module_init();
# endif
# ifdef ORACLE
   oracle_module_init();     // init Oracle DBI driver
# endif
# ifdef QORE_MYSQL
   qore_mysql_module_init(); // init MySQL DBI driver
# endif
# ifdef TIBCO
   tibco_module_init();      // init TIBCO module
# endif
   // autoload modules
#else
   // try to load all modules in each directory in the autoDirList
   QoreString gstr;

   class StringNode *w = autoDirList.getHead();
   while (w)
   {
      // make new string for glob
      gstr.terminate(0);
      gstr.concat(w->str);
      gstr.concat("/*.qmod");

      glob_t globbuf;
      if (!glob(gstr.getBuffer(), 0, NULL, &globbuf))
      {
	 for (int i = 0; i < (int)globbuf.gl_pathc; i++)
	    loadModuleFromPath(globbuf.gl_pathv[i]);
      }
      else
	 printd(1, "ModuleManager::init(): glob returned an error: %s\n", strerror(errno));
      globfree(&globbuf);
      w = w->next;
   }
#endif
}

#ifndef QORE_MONOLITHIC
inline void *ModuleManager::getsym(char *path, void *ptr, char *sym)
{
   void *sp = dlsym(ptr, sym);
   if (!sp)
   {
      if (show_errors)
	 printf("error loading qore module '%s': missing symbol '%s' in module\n", path, sym);
      printd(5, "ModuleManager::getsym() '%s': missing symbol '%s'\n", path, sym);
      dlclose(ptr);
   }
   return sp;
}
#endif

int ModuleManager::loadModule(char *name, class QoreProgram *pgm)
{
   // if the feature already exists in this program, then return
   if (pgm && !pgm->featureList.find(name))
      return 0;

   // if the feature already exists, then load the namespace changes into this program and register the feature
   class ModuleInfo *mi;
   if ((mi = find(name)))
   {
      if (pgm)
      {
	 mi->ns_init(pgm->getRootNS(), pgm->getQoreNS());
	 pgm->featureList.append(mi->getName());
      }
      return 0;
   }

#ifndef QORE_MONOLITHIC
   // otherwise, try to find module in the module path
   QoreString str;
   struct stat sb;

   class StringNode *w = moduleDirList.getHead();
   while (w)
   {
      str.terminate(0);
      str.sprintf("%s/%s.qmod", w->str, name);
      //printd(5, "ModuleManager::loadModule(%s) trying %s\n", name, str.getBuffer());

      if (!stat(str.getBuffer(), &sb))
      {
	 if ((mi = loadModuleFromPath(str.getBuffer())))	    
	 {
	    if (pgm)
	    {
	       mi->ns_init(pgm->getRootNS(), pgm->getQoreNS());
	       pgm->featureList.append(mi->getName());
	    }
	    return 0;
	 }
	 break;
      }
      w = w->next;
   }
#endif
   return -1;
}

#ifndef QORE_MONOLITHIC
class ModuleInfo *ModuleManager::loadModuleFromPath(char *path)
{
   void *ptr = dlopen(path, RTLD_LAZY);
   if (!ptr)
   {
      if (show_errors)
	 printf("error loading qore module '%s': %s\n", path, dlerror());

      printd(5, "ModuleManager::loadModuleFromPath(%s): %s\n", path, dlerror());
      return NULL;
   }
   // get module name
   char *name = (char *)getsym(path, ptr, "qore_module_name");
   if (!name)
      return NULL;
   // see if a module with this name is already registered
   class ModuleInfo *mi;
   if ((mi = find(name)))
   {
      if (show_errors)
	 printf("error loading qore module '%s': '%s' already registered by '%s'", path, name, mi->getFileName());
      printd(5, "ModuleManager::loadModulefromPath(%s): '%s' already registered by '%s'", path, name, mi->getFileName());
      dlclose(ptr);
      return NULL;
   }
   // get qore module API major number
   int *api_major = (int *)getsym(path, ptr, "qore_module_api_major");
   if (!api_major)
      return NULL;
   if (*api_major != QORE_MODULE_API_MAJOR)
   {
      if (show_errors)
	 printf("error loading qore module '%s': registering '%s': claims API major=%d, %d required", path, name, *api_major, QORE_MODULE_API_MAJOR);
      printd(5, "ModuleManager::loadModuleFromPath(%s): '%s': claims API major=%d, %d required", path, name, *api_major, QORE_MODULE_API_MAJOR);
   }

   // get initialization function
   qore_module_init_t *module_init = (qore_module_init_t *)getsym(path, ptr, "qore_module_init");
   if (!module_init)
      return NULL;

   // get namespace initialization function
   qore_module_ns_init_t *module_ns_init = (qore_module_ns_init_t *)getsym(path, ptr, "qore_module_ns_init");
   if (!module_ns_init)
      return NULL;

   // get deletion function
   qore_module_delete_t *module_delete = (qore_module_delete_t *)getsym(path, ptr, "qore_module_delete");
   if (!module_delete)
      return NULL;

   // get qore module API minor number
   int *api_minor = (int *)getsym(path, ptr, "qore_module_api_minor");
   if (!api_major)
      return NULL;

   // get qore module description
   char *desc = (char *)getsym(path, ptr, "qore_module_description");
   if (!desc)
      return NULL;

   // get qore module version
   char *version = (char *)getsym(path, ptr, "qore_module_version");
   if (!version)
      return NULL;

   // get qore module author
   char *author = (char *)getsym(path, ptr, "qore_module_author");
   if (!author)
      return NULL;

   // get qore module URL (optional)
   char *url = (char *)getsym(path, ptr, "qore_module_url");

   printd(5, "ModuleManager::loadModuleFromPath(%s) %s: calling module_init@%08x\n", path, name, *module_init);

   char *err;
   // run initialization
   if ((err = (*module_init)()))
   {
      if (show_errors)
	 printf("error initializing qore module '%s' registering '%s': %s\n", path, name, err);
      printd(5, "ModuleManager::loadModuleFromPath(%s): '%s': qore_module_init returned error: %s", path, name, err);
      return NULL;
   }

   mi = MM.add(path, name, *api_major, *api_minor, *module_init, *module_ns_init, *module_delete, desc, version, author, url, ptr);
   // add to auto namespace list
   ANSL.add(*module_ns_init);
   printd(5, "ModuleManager::loadModuleFromPath(%s) registered '%s'\n", path, name);
   return mi;
}
#endif

void ModuleManager::cleanup()
{
   tracein("ModuleManager::cleanup()");

#ifndef QORE_MONOLITHIC

   while (head)
   {
      ModuleInfo *w = head->next;
      delete head;
      head = w;
   }

#else
# ifdef TIBCO
   tibco_module_delete();       // delete tibco module
# endif
# ifdef ORACLE
   oracle_module_delete();      // delete oracle DBI driver
# endif
# ifdef QORE_MYSQL
   qore_mysql_module_delete();  // delete MySQL DBI driver
# endif
# ifdef NCURSES
   ncurses_module_delete();
# endif
#endif    // QORE_MONOLITHIC

   traceout("ModuleManager::cleanup()");
}

class List *ModuleManager::getModuleList()
{
   if (!head)
      return NULL;

   class List *l = new List();
   class ModuleInfo *w = head;
   while (w)
   {
      l->push(new QoreNode(w->getHash()));
      w = w->next;
   }
   return l;
}

ModuleInfo::~ModuleInfo()
{
   printd(5, "ModuleInfo::~ModuleInfo() '%s': %s (%08x) calling module_delete=%08x\n", name, filename, dlptr, module_delete);
   module_delete();
#ifndef DEBUG
   // do not close modules when debugging
   dlclose(dlptr);
#endif
   free(filename);
}

class Hash *ModuleInfo::getHash()
{
   class Hash *h = new Hash();
   h->setKeyValue("filename", new QoreNode(filename), NULL);
   h->setKeyValue("name", new QoreNode(name), NULL);
   h->setKeyValue("desc", new QoreNode(desc), NULL);
   h->setKeyValue("version", new QoreNode(version), NULL);
   h->setKeyValue("author", new QoreNode(author), NULL);
   h->setKeyValue("api_major", new QoreNode((int64)api_major), NULL);
   h->setKeyValue("api_minor", new QoreNode((int64)api_minor), NULL);
   if (url)
      h->setKeyValue("url", new QoreNode(url), NULL);
   return h;
}
