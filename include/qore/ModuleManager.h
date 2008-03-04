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
#include <qore/QoreThreadLock.h>

#define QORE_MODULE_API_MAJOR 0
#define QORE_MODULE_API_MINOR 4

//! signature of the module constructor/initialization function
typedef class QoreStringNode *(*qore_module_init_t)();

//! signature of the module namespace change/delta function
typedef void (*qore_module_ns_init_t)(class QoreNamespace *, class QoreNamespace *);

//! signature of the module destructor function
typedef void (*qore_module_delete_t)();

class ModuleInfo;

//! manages the loading of Qore modules from feature or path names.  Also manages adding module changes into QoreProgram objects.
/** in the case that a QoreProgram object is created before a module is loaded externally 
    (either through another QoreProgram object or through a direct call to the appropriate
    ModuleManager function), if the QoreProgram object then requests the feature, the
    ModuleManager will load in all namespace (class, constant, etc) changes into the 
    QoreProgram object.
    All members and methods are static; there will always only be one of these...
*/
class ModuleManager
{
   private:
      DLLLOCAL static bool show_errors;
      DLLLOCAL static class QoreThreadLock mutex;
      
      DLLLOCAL static void add(ModuleInfo *m);
      DLLLOCAL static void addBuiltin(const char *fn, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del);
      DLLLOCAL static ModuleInfo *add(const char *fn, char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, char *d, char *v, char *a, char *u, void *p);
      DLLLOCAL static class QoreStringNode *loadModuleFromPath(const char *path, const char *feature = 0, ModuleInfo **mi = 0);
      DLLLOCAL static ModuleInfo *find(const char *name);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ModuleManager(const ModuleManager&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ModuleManager& operator=(const ModuleManager&);

   public:
      //! to add a single directory to the QORE_MODULE_DIR list, can only be called before the library initialization function qore_init()
      /**
	 @param dir the directory path to add to the list
       */
      DLLEXPORT static void addModuleDir(const char *dir);

      //! to add a single directory to the QORE_AUTO_MODULE_DIR list, can only be called before the library initialization function qore_init()
      /**
	 @param dir the directory path to add to the list
       */
      DLLEXPORT static void addAutoModuleDir(const char *dir);

      //! to add a list of directories separated by ':' characters to the QORE_MODULE_DIR list, can only be called before the library initialization function qore_init()
      /**
	 @param strlist a list of directories separated by ':' characters to add to the QORE_MODULE_DIR list
       */
      DLLEXPORT static void addModuleDirList(const char *strlist);

      //! to add a list of directories separated by ':' characters to the QORE_AUTO_MODULE_DIR list, can only be called before the library initialization function qore_init()
      /**
	 @param strlist a list of directories separated by ':' characters to add to the QORE_AUTO_MODULE_DIR list
       */
      DLLEXPORT static void addAutoModuleDirList(const char *strlist);

      //! retuns a list of module information hashes, caller owns the list reference returned
      DLLEXPORT static class QoreListNode *getModuleList();

      //! loads the named module at parse time, returns a non-0 QoreStringNode if an error occured, caller owns the QoreStringNode pointer's reference count returned if non-0
      /** if the feature is already loaded, then the function returns immediately without raising an error
	  @param name can be either a feature name or the full path to the module file
	  @param pgm the QoreProgram object in which to include all module additions (namespaces, classes, constants, etc) immediately
       */
      DLLEXPORT static class QoreStringNode *parseLoadModule(const char *name, class QoreProgram *pgm = 0);

      //! loads the named module at run time, returns -1 if an exception was raised, 0 for OK
      /** if the feature is already loaded, then the function returns immediately without raising an error
	  @param name can be either a feature name or the full path to the module file
	  @param xsink if any errors are encountered loading the module, then a Qore-language "LOAD-MODULE-ERROR" exception is raised here
	  @return -1 if an exception was raised, 0 for OK
       */
      DLLEXPORT static int runTimeLoadModule(const char *name, class ExceptionSink *xsink);

      //! creates the ModuleManager object
      DLLLOCAL ModuleManager();

      //! explicit initialization and autoloading
      DLLLOCAL static void init(bool se);      

      //! explicit cleanup
      DLLLOCAL static void cleanup();
};

DLLEXPORT extern class ModuleManager MM;

#endif // _QORE_MODULEMANAGER_H
