/*
  ModuleManager.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#include <qore/QoreThreadLock.h>
#include <qore/QoreString.h>

#include <vector>

/** @file ModuleManager.h
    provides definitions required to load qore modules
 */

#define QORE_MODULE_API_MAJOR 0 //!< the major number of the Qore module API implemented
#define QORE_MODULE_API_MINOR 8 //!< the minor number of the Qore module API implemented

//! element of qore_mod_api_list;
struct qore_mod_api_compat_s {
   unsigned char major;
   unsigned char minor;
};

//! list of module APIs this library supports
DLLEXPORT extern const qore_mod_api_compat_s *qore_mod_api_list;

//! number of elements in qore_mod_api_list;
DLLEXPORT extern const unsigned qore_mod_api_list_len;

class QoreNamespace;
class QoreStringNode;
class QoreListNode;
class ExceptionSink;

//! signature of the module constructor/initialization function
typedef QoreStringNode *(*qore_module_init_t)();

//! signature of the module namespace change/delta function
typedef void (*qore_module_ns_init_t)(QoreNamespace *root_ns, QoreNamespace *qore_ns);

//! signature of the module destructor function
typedef void (*qore_module_delete_t)();

//! list of version numbers in order of importance (i.e. 1.2.3 = 1, 2, 3)
class version_list_t : public std::vector<int> {
private:
   QoreString ver;
public:
   DLLLOCAL char set(const char *v);
   DLLLOCAL const char *getString() const { return ver.getBuffer(); }
};

class ModuleInfo;

enum mod_op_e { MOD_OP_NONE, MOD_OP_EQ, MOD_OP_GT, 
		MOD_OP_GE, MOD_OP_LT, MOD_OP_LE };

//! manages the loading of Qore modules from feature or path names.  Also manages adding module changes into QoreProgram objects.
/** in the case that a QoreProgram object is created before a module is loaded externally 
    (either through another QoreProgram object or through a direct call to the appropriate
    ModuleManager function), if the QoreProgram object then requests the feature, the
    ModuleManager will load in all namespace (class, constant, etc) changes into the 
    QoreProgram object.
    All members and methods are static; there will always only be one of these...
*/
class ModuleManager {
   private:
      DLLLOCAL static void add(ModuleInfo *m);
      DLLLOCAL static void addBuiltin(const char *fn, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del);
      DLLLOCAL static ModuleInfo *add(const char *fn, char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, char *d, char *v, char *a, char *u, void *p);
      DLLLOCAL static QoreStringNode *loadModuleIntern(const char *name, QoreProgram *pgm, mod_op_e op = MOD_OP_NONE, version_list_t *version = 0);
      DLLLOCAL static QoreStringNode *loadModuleFromPath(const char *path, const char *feature = 0, ModuleInfo **mi = 0, QoreProgram *pgm = 0);
      DLLLOCAL static ModuleInfo *find(const char *name);
      DLLLOCAL static void globDir(const char *dir);
      DLLLOCAL static QoreStringNode *parseLoadModuleIntern(const char *name, QoreProgram *pgm = 0);

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
      DLLEXPORT static QoreListNode *getModuleList();

      //! loads the named module at run time, returns -1 if an exception was raised, 0 for OK
      /** If the feature is already loaded, then the function returns immediately without raising an error.
	  The feature's namespace changes are added to the QoreProgram object if the feature is loaded.
	  @param name can be either a feature name or the full path to the module file
	  @param xsink if any errors are encountered loading the module, then a Qore-language "LOAD-MODULE-ERROR" exception is raised here
	  @return -1 if an exception was raised, 0 for OK
       */
      DLLEXPORT static int runTimeLoadModule(const char *name, ExceptionSink *xsink);

      //! loads the named module at parse time, returns a non-0 QoreStringNode pointer if an error occured, caller owns the QoreStringNode pointer's reference count returned if non-0
      /** if the feature is already loaded, then the function returns immediately without raising an error
	  The feature's namespace changes are added to the QoreProgram object if the feature is loaded and the pgm argument is non-zero.
	  @param name can be either a feature name or the full path to the module file
	  @param pgm the QoreProgram object in which to include all module additions (namespaces, classes, constants, etc) immediately
       */
      DLLLOCAL static QoreStringNode *parseLoadModule(const char *name, QoreProgram *pgm = 0);

      //! creates the ModuleManager object (private)
      /** private interface; not exported in the library's public API
       */
      DLLLOCAL ModuleManager();

      //! explicit initialization and autoloading (private)
      /** private interface; not exported in the library's public API
       */
      DLLLOCAL static void init(bool se);      

      //! explicit cleanup (private)
      /** private interface; not exported in the library's public API
       */
      DLLLOCAL static void cleanup();
};

//! the global ModuleManager object
DLLEXPORT extern ModuleManager MM;

DLLLOCAL static inline bool is_module_api_supported(int major, int minor) {
   for (unsigned i = 0; i < qore_mod_api_list_len; ++i)
      if (qore_mod_api_list[i].major == major && qore_mod_api_list[i].minor == minor)
	 return true;
   return false;
}

#endif // _QORE_MODULEMANAGER_H
