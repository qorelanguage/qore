/*
  ModuleManager.h

  Qore Programming Language

  Copyright (C) 2003,2004,2005 David Nichols

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

#ifndef _QORE_MODULEMANAGER_H

#define _QORE_MODULEMANAGER_H

#include <qore/common.h>
#include <qore/StringList.h>
#include <qore/LockedObject.h>

#include <stdio.h>
#include <string.h>

#include <map>

#define QORE_MODULE_API_MAJOR 0
#define QORE_MODULE_API_MINOR 4

typedef class QoreString *(*qore_module_init_t)();
typedef void (*qore_module_ns_init_t)(class Namespace *, class Namespace *);
typedef void (*qore_module_delete_t)();

class ModuleInfo {
   private:
      char *filename;
      const char *name, *desc, *version, *author, *url;
      int api_major, api_minor;
      qore_module_init_t module_init;
      qore_module_ns_init_t module_ns_init;
      qore_module_delete_t module_delete;
      const void *dlptr;

   public:
      DLLLOCAL ModuleInfo(const char *fn, const char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, const char *d, const char *v, const char *a, const char *u, const void *p);
      // for "builtin" modules
      DLLLOCAL ModuleInfo(const char *feature, qore_module_delete_t del);
      DLLLOCAL ~ModuleInfo();
      DLLLOCAL const char *getName() const;
      DLLLOCAL const char *getFileName() const;
      DLLLOCAL const char *getDesc() const;
      DLLLOCAL const char *getVersion() const;
      DLLLOCAL const char *getURL() const;
      DLLLOCAL int getAPIMajor() const;
      DLLLOCAL int getAPIMinor() const;
      DLLLOCAL void ns_init(class Namespace *rns, class Namespace *qns) const;
      DLLLOCAL bool isBuiltin() const;
      DLLLOCAL class Hash *getHash() const;
};

typedef std::map<const char *, ModuleInfo *, class ltstr> module_map_t;

// all members and methods are static; there will always only be one of these...
class ModuleManager
{
   private:
      DLLLOCAL static bool show_errors;
      DLLLOCAL static class StringList autoDirList, moduleDirList;
      DLLLOCAL static class LockedObject mutex;
      DLLLOCAL static module_map_t map;
      
      DLLLOCAL static void add(ModuleInfo *m);
      DLLLOCAL static void addBuiltin(const char *fn, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del);
      DLLLOCAL static class ModuleInfo *add(const char *fn, char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, char *d, char *v, char *a, char *u, void *p);
      DLLLOCAL static class QoreString *loadModuleFromPath(const char *path, const char *feature = NULL, class ModuleInfo **mi = NULL);
      DLLLOCAL static class ModuleInfo *find(const char *name);

   public:
      // to add a directory to the QORE_MODULE_DIR list, can only be called before init()
      DLLEXPORT static void addModuleDir(const char *dir);
      // to add a directory to the QORE_AUTO_MODULE_DIR list, can only be called before init()
      DLLEXPORT static void addAutoModuleDir(const char *dir);
      // to add a directory to the QORE_MODULE_DIR list, can only be called before init()
      DLLEXPORT static void addModuleDirList(const char *strlist);
      // to add a directory to the QORE_AUTO_MODULE_DIR list, can only be called before init()
      DLLEXPORT static void addAutoModuleDirList(const char *strlist);
      // retuns a list of module information hashes
      DLLEXPORT static class List *getModuleList();
      // loads the named module at parse time, returns a non-NULL QoreString if an error occured
      DLLEXPORT static class QoreString *parseLoadModule(const char *name, class QoreProgram *pgm = NULL);
      // loads the named module at run time, returns -1 if an exception was raised, 0 for OK
      DLLEXPORT static int runTimeLoadModule(const char *name, class ExceptionSink *xsink);

      // creates the ModuleManager object
      DLLLOCAL ModuleManager();
      // explicit initialization and autoloading
      DLLLOCAL static void init(bool se);      
      // explicit cleanup
      DLLLOCAL static void cleanup();
};

DLLEXPORT extern class ModuleManager MM;

#endif // _QORE_MODULEMANAGER_H
