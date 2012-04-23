/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ModuleInfo.h

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

#ifndef _QORE_MODULEINFO_H

#define _QORE_MODULEINFO_H

#include <dlfcn.h>

#include <string>
#include <map>

// user module parse options
#define USER_MOD_PO (PO_NO_TOP_LEVEL_STATEMENTS | PO_REQUIRE_PROTOTYPES | PO_REQUIRE_OUR | PO_IN_MODULE | PO_NO_EMBEDDED_LOGIC)

//! list of version numbers in order of importance (i.e. 1.2.3 = 1, 2, 3)
struct version_list_t : public std::vector<int> {
protected:
   QoreString ver;

public:
   DLLLOCAL version_list_t() {
   }

   DLLLOCAL version_list_t(const char* v) {
      set(v);
   }

   DLLLOCAL char set(const char* v);

   DLLLOCAL const char* operator*() const {
      return ver.getBuffer();
   }
};

class QoreAbstractModule {
private:
   // not implemented
   DLLLOCAL QoreAbstractModule(const QoreAbstractModule&);
   DLLLOCAL QoreAbstractModule& operator=(const QoreAbstractModule&);

protected:
   QoreString filename,
      name, 
      desc,
      author,
      url;

   //typedef std::map<std::string, std::string> strmap_t;
   //strmap_t strmap;

   DLLLOCAL QoreHashNode* getHashIntern() const {
      QoreHashNode* h = new QoreHashNode;

      h->setKeyValue("filename", new QoreStringNode(filename), 0);
      h->setKeyValue("name", new QoreStringNode(name), 0);
      h->setKeyValue("desc", new QoreStringNode(desc), 0);
      h->setKeyValue("version", new QoreStringNode(*version_list), 0);
      h->setKeyValue("author", new QoreStringNode(author), 0);
      if (!url.empty())
         h->setKeyValue("url", new QoreStringNode(url), 0);

      return h;
   }

public:
   version_list_t version_list;

   DLLLOCAL QoreAbstractModule(const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u) : filename(fn), name(n), desc(d), author(a), url(u), version_list(v) {
   }

   // for "builtin" modules
   DLLLOCAL virtual ~QoreAbstractModule() {
   }

   DLLLOCAL const char* getName() const {
      return name.getBuffer();
   }

   DLLLOCAL const char* getFileName() const {
      return filename.getBuffer();
   }

   DLLLOCAL const char* getDesc() const {
      return desc.getBuffer();
   }

   DLLLOCAL const char* getVersion() const {
      return* version_list;
   }

   DLLLOCAL const char* getURL() const {
      return url.getBuffer();
   }

   DLLLOCAL virtual bool isBuiltin() const = 0;
   DLLLOCAL virtual bool isUser() const = 0;
   DLLLOCAL virtual QoreHashNode* getHash() const = 0;
   DLLLOCAL virtual void addToProgram(QoreProgram* pgm, ExceptionSink& xsink) const = 0;
   DLLLOCAL virtual void issueParseCmd(QoreString &cmd) = 0;
};

class QoreModuleManager {
private:
   // not implemented
   DLLLOCAL QoreModuleManager(const QoreModuleManager&);
   // not implemented
   DLLLOCAL QoreModuleManager& operator=(const QoreModuleManager&);

protected:
   // recursive mutex; initialized in init()
   QoreThreadLock* mutex;

   // module blacklist
   typedef std::map<const char* , const char* , ltstr> bl_map_t;
   bl_map_t mod_blacklist;

   // module hash
   typedef std::map<const char*, QoreAbstractModule*, ltstr> module_map_t;
   module_map_t map;   

   DLLLOCAL QoreAbstractModule* findModuleUnlocked(const char* name) {
      module_map_t::iterator i = map.find(name);
      return i == map.end() ? 0 : i->second;
   }

   DLLLOCAL void loadModuleIntern(const char* name, QoreProgram* pgm, ExceptionSink& xsink) {
      AutoLocker sl(mutex); // make sure checking and loading are atomic

      loadModuleIntern(xsink, name, pgm);
   }

   DLLLOCAL void loadModuleIntern(ExceptionSink& xsink, const char* name, QoreProgram* pgm, mod_op_e op = MOD_OP_NONE, version_list_t* version = 0);

   DLLLOCAL void globDir(const char* dir);

   DLLLOCAL QoreAbstractModule* loadBinaryModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature = 0, QoreProgram* pgm = 0);
   DLLLOCAL QoreAbstractModule* loadUserModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature = 0, QoreProgram* pgm = 0);

public:
   DLLLOCAL QoreModuleManager() : mutex(0) {
   }

   DLLLOCAL ~QoreModuleManager() {
      delete mutex;
   }

   DLLLOCAL void init(bool se);
   DLLLOCAL void delUser();
   DLLLOCAL void cleanup();
   DLLLOCAL void issueParseCmd(const char* mname, QoreProgram* pgm, QoreString &cmd);

   DLLLOCAL void addModule(QoreAbstractModule* m) {
      map.insert(std::make_pair(m->getName(), m));
   }

   DLLLOCAL QoreAbstractModule* findModule(const char* name) {
      AutoLocker al(mutex);
      return findModuleUnlocked(name);
   }

   DLLLOCAL void parseLoadModule(const char* name, QoreProgram* pgm, ExceptionSink& xsink);
   DLLLOCAL int runTimeLoadModule(const char* name, ExceptionSink& xsink);

   DLLLOCAL QoreHashNode* getModuleHash();
   DLLLOCAL QoreListNode* getModuleList();


};

DLLLOCAL extern QoreModuleManager QMM;

class QoreBuiltinModule : public QoreAbstractModule {
protected:
   unsigned api_major, api_minor;
   qore_module_init_t module_init;
   qore_module_ns_init_t module_ns_init;
   qore_module_delete_t module_delete;
   qore_module_parse_cmd_t module_parse_cmd;
   const void* dlptr;

public:
   DLLLOCAL QoreBuiltinModule(const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u, unsigned major, unsigned minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, qore_module_parse_cmd_t pcmd, const void* p) : QoreAbstractModule(fn, n, d, v, a, u), api_major(major), api_minor(minor), module_init(init), module_ns_init(ns_init), module_delete(del), module_parse_cmd(pcmd), dlptr(p) {
   }

   DLLLOCAL virtual ~QoreBuiltinModule() {
      printd(5, "ModuleInfo::~ModuleInfo() '%s': %s calling module_delete=%08p\n", name.getBuffer(), filename.getBuffer(), module_delete);
      module_delete();
      if (dlptr) {
         printd(5, "calling dlclose(%08p)\n", dlptr);
#ifndef DEBUG
         // do not close modules when debugging
         dlclose((void* )dlptr);
#endif
      }
   }

   DLLLOCAL unsigned getAPIMajor() const {
      return api_major;
   }

   DLLLOCAL unsigned getAPIMinor() const {
      return api_minor;
   }

   DLLLOCAL virtual bool isBuiltin() const {
      return true;
   }

   DLLLOCAL virtual bool isUser() const {
      return false;
   }

   DLLLOCAL QoreHashNode* getHash() const {
      QoreHashNode* h = getHashIntern();
   
      h->setKeyValue("user", &False, 0);
      h->setKeyValue("api_major", new QoreBigIntNode(api_major), 0);
      h->setKeyValue("api_minor", new QoreBigIntNode(api_minor), 0);
   
      return h;
   }

   DLLLOCAL const void* getPtr() const {
      return dlptr;
   }

   DLLLOCAL virtual void addToProgram(QoreProgram* pgm, ExceptionSink& xsink) const;
   DLLLOCAL virtual void issueParseCmd(QoreString &cmd);
};

class QoreUserModule : public QoreAbstractModule {
protected:
   QoreProgram* pgm;
   QoreClosureParseNode* del; // deletion closure

public:
   DLLLOCAL QoreUserModule(const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u, QoreProgram* p, QoreClosureParseNode* dl) : QoreAbstractModule(fn, n, d, v, a, u), pgm(p), del(dl) {
   }

   DLLLOCAL virtual ~QoreUserModule();

   DLLLOCAL virtual bool isBuiltin() const {
      return false;
   }

   DLLLOCAL virtual bool isUser() const {
      return true;
   }

   DLLLOCAL virtual QoreHashNode* getHash() const {
      return getHashIntern();
   }

   DLLLOCAL virtual void addToProgram(QoreProgram* pgm, ExceptionSink& xsink) const;

   DLLLOCAL virtual void issueParseCmd(QoreString &cmd) {
      parseException("PARSE-COMMAND-ERROR", "module '%s' loaded from '%s' is a user module; only builtin modules can support parse commands", name.getBuffer(), filename.getBuffer());
   }
};

#endif
