/*
  ModuleInfo.h

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

#ifndef _QORE_MODULEINFO_H

#define _QORE_MODULEINFO_H

class ModuleInfo {
   private:
      char *filename;
      const char *name, *desc, *version, *author, *url;
      int api_major, api_minor;
      qore_module_init_t module_init;
      qore_module_ns_init_t module_ns_init;
      qore_module_delete_t module_delete;
      const void *dlptr;

      // not implemented
      DLLLOCAL ModuleInfo(const ModuleInfo&);
      DLLLOCAL ModuleInfo& operator=(const ModuleInfo&);

   public:
      version_list_t version_list;

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
      DLLLOCAL void ns_init(QoreNamespace *rns, QoreNamespace *qns) const;
      DLLLOCAL bool isBuiltin() const;
      DLLLOCAL QoreHashNode *getHash() const;
      DLLLOCAL const void *getPtr() const { return dlptr; }
};

#endif
