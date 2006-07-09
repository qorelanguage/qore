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

#include <qore/StringList.h>
#include <qore/LockedObject.h>

#include <stdio.h>
#include <string.h>

typedef int (*qore_module_init_t)();
typedef void (*qore_module_ns_init_t)(class Namespace *, class Namespace *);
typedef void (*qore_module_delete_t)();

class ModuleInfo {
   private:
      char *filename, *name, *desc, *version, *author, *url;
      int api_major, api_minor;
      qore_module_init_t module_init;
      qore_module_ns_init_t module_ns_init;
      qore_module_delete_t module_delete;
      void *dlptr;

   public:
      class ModuleInfo *next;
      
      inline ModuleInfo(char *fn, char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, char *d, char *v, char *a, char *u, void *p)
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
      // for "builtin" modules
      inline ModuleInfo(char *feature, qore_module_delete_t del);
      ~ModuleInfo();
      inline char *getName()
      {
	 return name;
      }
      inline char *getFileName()
      {
	 return filename;
      }
      inline char *getDesc()
      {
	 return desc;
      }
      inline char *getVersion()
      {
	 return version;
      }
      inline char *getURL()
      {
	 return url;
      }
      inline int getAPIMajor()
      {
	 return api_major;
      }
      inline int getAPIMinor()
      {
	 return api_minor;
      }
      inline void ns_init(class Namespace *rns, class Namespace *qns)
      {
	 module_ns_init(rns, qns);
      }
      inline bool isBuiltin()
      {
	 return !dlptr;
      }
      class Hash *getHash();
};

class ModuleManager : public LockedObject
{
   private:
      class ModuleInfo *head;
      int num;
      bool show_errors;
      class StringList autoDirList, moduleDirList;

      inline void *getsym(char *path, void *ptr, char *sym);
      inline void addInternal(ModuleInfo *m)
      {
	 m->next = head;
	 head = m;
      }
      inline void addBuiltin(char *fn, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del);
      inline class ModuleInfo *add(char *fn, char *n, int major, int minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, char *d, char *v, char *a, char *u, void *p)
      {
	 class ModuleInfo *m = new ModuleInfo(fn, n, major, minor, init, ns_init, del, d, v, a, u, p);
	 lock();
	 addInternal(m);
	 num++;
	 unlock();
	 return m;
      }
      class ModuleInfo *loadModuleFromPath(char *path);
      inline class ModuleInfo *find(char *name)
      {
	 class ModuleInfo *w = head;
	 while (w)
	 {
	    if (!strcmp(name, w->getName()))
	       break;
	    w = w->next;
	 }
	 return w;
      }

   public:
      ModuleManager();
      // explicit initialization and autoloading
      void init(bool se);
      // explicit cleanup
      void cleanup();
      // retuns a list of module information hashes
      class List *getModuleList();
      // loads the named module, returns -1 for error
      int loadModule(char *name, class QoreProgram *pgm = NULL);
};

extern class ModuleManager MM;

#endif // _QORE_MODULEMANAGER_H
