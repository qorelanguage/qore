/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ModuleInfo.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_MODULEINFO_H

#define _QORE_MODULEINFO_H

#ifdef NEED_DLFCN_WRAPPER
extern "C" {
#endif
#include <dlfcn.h>
#ifdef NEED_DLFCN_WRAPPER
}
#endif

#include <string>
#include <map>
#include <deque>
#include <memory>
#include <vector>

// parse options set while parsing the module's header (init & del)
#define MOD_HEADER_PO (PO_LOCKDOWN & ~PO_NO_MODULES)

// initial user module parse options
#define USER_MOD_PO (PO_NO_TOP_LEVEL_STATEMENTS | PO_REQUIRE_PROTOTYPES | PO_REQUIRE_OUR | PO_IN_MODULE)

// module load options
#define QMLO_NONE     0
#define QMLO_INJECT   (1 << 0)
#define QMLO_REINJECT (1 << 1)
#define QMLO_PRIVATE  (1 << 2)
#define QMLO_RELOAD   (1 << 3)

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

   DLLLOCAL version_list_t& operator=(const char* v) {
      set(v);
      return *this;
   }

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
      url,
      license,
      orig_name;

   // link to associated modules (originals with reinjection, etc)
   QoreAbstractModule* prev, * next;

   bool priv : 1,
      injected : 1,
      reinjected : 1;

   DLLLOCAL QoreHashNode* getHashIntern(bool with_filename = true) const {
      QoreHashNode* h = new QoreHashNode;

      if (with_filename)
         h->setKeyValue("filename", new QoreStringNode(filename), 0);
      h->setKeyValue("name", new QoreStringNode(name), 0);
      h->setKeyValue("desc", new QoreStringNode(desc), 0);
      h->setKeyValue("version", new QoreStringNode(*version_list), 0);
      h->setKeyValue("author", new QoreStringNode(author), 0);
      if (!url.empty())
         h->setKeyValue("url", new QoreStringNode(url), 0);
      if (!license.empty())
         h->setKeyValue("license", new QoreStringNode(license), 0);
      if (!rmod.empty()) {
         QoreListNode* l = new QoreListNode;
         for (name_vec_t::const_iterator i = rmod.begin(), e = rmod.end(); i != e; ++i)
            l->push(new QoreStringNode(*i));
         h->setKeyValue("reexported-modules", l, 0);
      }
      h->setKeyValue("injected", get_bool_node(injected), 0);
      h->setKeyValue("reinjected", get_bool_node(injected), 0);

      return h;
   }

   DLLLOCAL virtual void addToProgramImpl(QoreProgram* pgm, ExceptionSink& xsink) const = 0;

   DLLLOCAL void set(const char* d, const char* v, const char* a, const char* u, const QoreString& l) {
      desc = d;
      author = a;
      url = u;
      license = l;
      version_list = v;
   }

public:
   version_list_t version_list;
   // list of dependent modules to reexport
   name_vec_t rmod;

   // for binary modules
   DLLLOCAL QoreAbstractModule(const char* cwd, const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u, const QoreString& l) : filename(fn), name(n), desc(d), author(a), url(u), license(l), prev(0), next(0), priv(false), injected(false), reinjected(false), version_list(v) {
      q_normalize_path(filename, cwd);
   }

   // for user modules
   DLLLOCAL QoreAbstractModule(const char* cwd, const char* fn, const char* n, unsigned load_opt) : filename(fn), name(n), prev(0), next(0), priv(load_opt & QMLO_PRIVATE), injected(load_opt & QMLO_INJECT), reinjected(load_opt & QMLO_REINJECT) {
      q_normalize_path(filename, cwd);
   }

   DLLLOCAL virtual ~QoreAbstractModule() {
      //printd(5, "QoreAbstractModule::~QoreAbstractModule() this: %p name: %s\n", this, name.getBuffer());
      if (next) {
         assert(next->prev == this);
         next->prev = prev;
      }
      if (prev) {
         assert(prev->next == this);
         prev->next = next;
      }
   }

   DLLLOCAL const char* getName() const {
      return name.getBuffer();
   }

   DLLLOCAL const char* getFileName() const {
      return filename.getBuffer();
   }

   DLLLOCAL const QoreString& getFileNameStr() const {
      return filename;
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

   DLLLOCAL const char* getOrigName() const {
      return orig_name.empty() ? 0 : orig_name.getBuffer();
   }

   DLLLOCAL void resetName() {
      assert(!orig_name.empty());
      name = orig_name;
      orig_name.clear();
   }

   DLLLOCAL bool isInjected() const {
      return injected;
   }

   DLLLOCAL bool isReInjected() const {
      return reinjected;
   }

   DLLLOCAL void addModuleReExport(const char* m) {
      rmod.push_back(m);
   }

   DLLLOCAL void reexport(ExceptionSink& xsink, QoreProgram* pgm) const;

   DLLLOCAL void addToProgram(QoreProgram* pgm, ExceptionSink& xsink) const {
      addToProgramImpl(pgm, xsink);
      if (!xsink)
         reexport(xsink, pgm);
   }

   DLLLOCAL bool equalTo(const QoreAbstractModule* m) const {
      assert(name == m->name);
      return filename == m->filename;
   }

   DLLLOCAL bool isPath(const char* p) const {
      return filename == p;
   }

   DLLLOCAL void rename(const QoreString& n) {
      assert(orig_name.empty());
      name = n;
   }

   DLLLOCAL void setOrigName(const char* n) {
      assert(orig_name.empty());
      orig_name = n;
   }

   DLLLOCAL bool isPrivate() const {
      return priv;
   }

   DLLLOCAL void setPrivate(bool p = true) {
      assert(priv != p);
      priv = p;
   }

   DLLLOCAL void setLink(QoreAbstractModule* n) {
      assert(!next);
      assert(!n->prev);
      next = n;
      n->prev = this;
   }

   DLLLOCAL QoreAbstractModule* getNext() const {
      return next;
   }

   DLLLOCAL virtual bool isBuiltin() const = 0;
   DLLLOCAL virtual bool isUser() const = 0;
   DLLLOCAL virtual QoreHashNode* getHash(bool with_filename = true) const = 0;
   DLLLOCAL virtual void issueParseCmd(QoreString &cmd) = 0;
};

// list/dequeue of strings
typedef std::deque<std::string> strdeque_t;

//! non-thread-safe unique list of strings of directory names
/** a deque should require fewer memory allocations compared to a linked list.
    the set is used for uniqueness
 */
class UniqueDirectoryList {
protected:
   typedef std::set<std::string> strset_t;

   strdeque_t dlist;
   strset_t dset;

public:
   DLLLOCAL void addDirList(const char* str);

   DLLLOCAL bool push_back(const char* str) {
      if (dset.find(str) != dset.end())
         return false;

      dlist.push_back(str);
      dset.insert(str);
      return true;
   }

   DLLLOCAL bool empty() const {
      return dlist.empty();
   }

   DLLLOCAL strdeque_t::const_iterator begin() const {
      return dlist.begin();
   }

   DLLLOCAL strdeque_t::const_iterator end() const {
      return dlist.end();
   }

   DLLLOCAL void appendPath(QoreString& str) const {
      if (dlist.empty()) {
         str.concat("<empty>");
         return;
      }
      for (strdeque_t::const_iterator i = dlist.begin(), e = dlist.end(); i != e; ++i) {
         str.concat((*i).c_str());
         str.concat(':');
      }
      str.terminate(str.size() - 1);
   }
};

class QoreModuleContextHelper : public QoreModuleContext {
public:
   DLLLOCAL QoreModuleContextHelper(const char* name, QoreProgram* pgm, ExceptionSink& xsink);
   DLLLOCAL ~QoreModuleContextHelper();
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

class QoreUserModuleDefContextHelper;
class QoreUserModule;

typedef std::set<std::string> strset_t;
typedef std::map<std::string, strset_t> md_map_t;

class ModMap {
private:
   DLLLOCAL ModMap(const ModMap &);
   DLLLOCAL ModMap& operator=(const ModMap&);

protected:
   md_map_t map;

public:
   DLLLOCAL ModMap() {
   }

   DLLLOCAL ~ModMap() {
   }

   DLLLOCAL bool addDep(const char* l, const char* r) {
      md_map_t::iterator i = map.lower_bound(l);
      if (i == map.end() || i->first != l)
         i = map.insert(i, md_map_t::value_type(l, strset_t()));
      else if (i->second.find(r) != i->second.end())
         return true;
      i->second.insert(r);
      return false;
   }

   DLLLOCAL md_map_t::iterator begin() {
      return map.begin();
   }

   DLLLOCAL md_map_t::iterator end() {
      return map.end();
   }

   DLLLOCAL md_map_t::iterator find(const char* n) {
      return map.find(n);
   }

   DLLLOCAL md_map_t::iterator find(const std::string& n) {
      return map.find(n);
   }

   DLLLOCAL void erase(md_map_t::iterator i) {
      map.erase(i);
   }

   DLLLOCAL void clear() {
      map.clear();
   }

   DLLLOCAL bool empty() const {
      return map.empty();
   }

#ifdef DEBUG
   DLLLOCAL void show() {
      for (md_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i) {
         QoreString str("[");
         for (strset_t::iterator si = i->second.begin(), se = i->second.end(); si != se; ++si)
            str.sprintf("'%s',", (*si).c_str());
         str.concat("]");

         printd(0, " + rmd_map '%s' -> %s\n", i->first.c_str(), str.getBuffer());
      }
   }
#endif
};

class QoreModuleManager {
   friend class QoreAbstractModule;

private:
   // not implemented
   DLLLOCAL QoreModuleManager(const QoreModuleManager&);
   // not implemented
   DLLLOCAL QoreModuleManager& operator=(const QoreModuleManager&);

protected:
   // recursive mutex; initialized in init()
   QoreThreadLock* mutex;

   // user module dependency map: module -> dependents
   ModMap md_map;
   // user module dependent map: dependent -> module
   ModMap rmd_map;

   // module blacklist
   typedef std::map<const char*, const char*, ltstr> bl_map_t;
   bl_map_t mod_blacklist;

   // module hash
   typedef std::map<const char*, QoreAbstractModule*, ltstr> module_map_t;
   module_map_t map;

   // set of user modules with no dependencies
   strset_t umset;

   // list of module directories
   UniqueDirectoryList moduleDirList;

   DLLLOCAL QoreAbstractModule* findModuleUnlocked(const char* name) {
      module_map_t::iterator i = map.find(name);
      return i == map.end() ? 0 : i->second;
   }

   DLLLOCAL void loadModuleIntern(const char* name, QoreProgram* pgm, ExceptionSink& xsink) {
      AutoLocker sl(mutex); // make sure checking and loading are atomic

      loadModuleIntern(xsink, name, pgm);
   }

   DLLLOCAL void loadModuleIntern(ExceptionSink& xsink, const char* name, QoreProgram* pgm, bool reexport = false, mod_op_e op = MOD_OP_NONE, version_list_t* version = 0, const char* src = 0, QoreProgram* mpgm = 0, unsigned load_opt = QMLO_NONE);

   DLLLOCAL QoreAbstractModule* loadBinaryModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature = 0, QoreProgram* pgm = 0, bool reexport = false);
   DLLLOCAL QoreAbstractModule* loadUserModuleFromPath(ExceptionSink& xsink, const char* path, const char* feature = 0, QoreProgram* tpgm = 0, bool reexport = false, QoreProgram* pgm = 0, QoreProgram* path_pgm = 0, unsigned load_opt = QMLO_NONE);
   DLLLOCAL QoreAbstractModule* loadUserModuleFromSource(ExceptionSink& xsink, const char* path, const char* feature, QoreProgram* tpgm, const char* src, bool reexport, QoreProgram* pgm = 0);
   DLLLOCAL QoreAbstractModule* setupUserModule(ExceptionSink& xsink, std::auto_ptr<QoreUserModule>& mi, QoreUserModuleDefContextHelper& qmd, unsigned load_opt = QMLO_NONE);

   DLLLOCAL void reinjectModule(QoreAbstractModule* mi);
   DLLLOCAL void delOrig(QoreAbstractModule* mi);
   DLLLOCAL void getUniqueName(QoreString& nname, const char* name, const char* prefix);

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
      map.insert(module_map_t::value_type(m->getName(), m));
   }

   DLLLOCAL QoreAbstractModule* findModule(const char* name) {
      AutoLocker al(mutex);
      return findModuleUnlocked(name);
   }

   DLLLOCAL void parseLoadModule(ExceptionSink& xsink, const char* name, QoreProgram* pgm, bool reexport = false);
   DLLLOCAL int runTimeLoadModule(ExceptionSink& xsink, const char* name, QoreProgram* pgm, QoreProgram* mpgm = 0, unsigned load_opt = QMLO_NONE);

   DLLLOCAL QoreHashNode* getModuleHash();
   DLLLOCAL QoreListNode* getModuleList();

   DLLLOCAL void addModuleDir(const char* dir) {
      OptLocker al(mutex);
      moduleDirList.push_back(dir);
   }

   DLLLOCAL void addModuleDirList(const char* strlist) {
      OptLocker al(mutex);
      moduleDirList.addDirList(strlist);
   }

   DLLLOCAL void addStandardModulePaths();

   DLLLOCAL void registerUserModuleFromSource(const char* name, const char* src, QoreProgram *pgm, ExceptionSink& xsink);

   DLLLOCAL void trySetUserModuleDependency(const QoreAbstractModule* mi) {
      if (!mi->isUser())
         return;

      const char* old_name = get_user_module_context_name();
      if (old_name)
         setUserModuleDependency(mi->getName(), old_name);
      trySetUserModule(mi->getName());
   }

   DLLLOCAL void trySetUserModule(const char* name) {
      md_map_t::iterator i = md_map.find(name);
      if (i == md_map.end()) {
         umset.insert(name);
         //printd(5, "QoreModuleManager::trySetUserModule('%s') UMSET SET: rmd_map: empty\n", name);
      }
#ifdef DEBUG
      /*
      else {
	 QoreString str("[");
	 for (strset_t::iterator si = i->second.begin(), se = i->second.end(); si != se; ++si)
	    str.sprintf("'%s',", (*si).c_str());
	 str.concat("]");
         //printd(5, "QoreModuleManager::trySetUserModule('%s') UMSET NOT SET: md_map: %s\n", name, str.getBuffer());
      }
      */
#endif
   }

   DLLLOCAL void setUserModuleDependency(const char* name, const char* dep) {
      //printd(5, "QoreModuleManager::setUserModuleDependency('%s' -> '%s')\n", name, dep);
      if (md_map.addDep(name, dep))
         return;
      rmd_map.addDep(dep, name);

      strset_t::iterator ui = umset.find(name);
      if (ui != umset.end()) {
         umset.erase(ui);
         //printd(5, "QoreModuleManager::setUserModuleDependency('%s' -> '%s') REMOVED '%s' FROM UMMSET\n", name, dep, name);
      }
   }

   DLLLOCAL void removeUserModuleDependency(const char* name, const char* orig_name = 0) {
      //printd(5, "QoreModuleManager::removeUserModuleDependency() name: '%s' orig: '%s'\n", name, orig_name ? orig_name : "n/a");
      md_map_t::iterator i = rmd_map.find(name);
      if (i == rmd_map.end() && orig_name)
         i = rmd_map.find(orig_name);
      if (i != rmd_map.end()) {
         // remove dependents
         for (strset_t::iterator si = i->second.begin(), se = i->second.end(); si != se; ++si) {
            md_map_t::iterator di = md_map.find(*si);
            assert(di != md_map.end());

            strset_t::iterator dsi = di->second.find(i->first);
            assert(dsi != di->second.end());
            di->second.erase(dsi);
            if (di->second.empty()) {
               //printd(5, "QoreModuleManager::removeUserModuleDependency('%s') '%s' now empty, ADDING TO UMMSET: '%s'\n", name, i->first.c_str(), (*si).c_str());
               //md_map.erase(di);
               assert(umset.find(*si) == umset.end());
               umset.insert(*si);
            }
         }
         // remove from dep map
         rmd_map.erase(i);
      }

      i = md_map.find(name);
      if (i != md_map.end())
         md_map.erase(i);
      if (orig_name) {
         i = md_map.find(orig_name);
         if (i != md_map.end())
            md_map.erase(i);
      }
   }
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

   DLLLOCAL virtual void addToProgramImpl(QoreProgram* pgm, ExceptionSink& xsink) const;

public:
   DLLLOCAL QoreBuiltinModule(const char* cwd, const char* fn, const char* n, const char* d, const char* v, const char* a, const char* u, const QoreString& l, unsigned major, unsigned minor, qore_module_init_t init, qore_module_ns_init_t ns_init, qore_module_delete_t del, qore_module_parse_cmd_t pcmd, const void* p) : QoreAbstractModule(cwd, fn, n, d, v, a, u, l), api_major(major), api_minor(minor), module_init(init), module_ns_init(ns_init), module_delete(del), module_parse_cmd(pcmd), dlptr(p) {
   }

   DLLLOCAL virtual ~QoreBuiltinModule() {
      printd(5, "QoreBuiltinModule::~QoreBuiltinModule() '%s': %s calling module_delete: %p\n", name.getBuffer(), filename.getBuffer(), module_delete);
      module_delete();
      if (dlptr) {
         printd(5, "calling dlclose(%p)\n", dlptr);
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

   DLLLOCAL QoreHashNode* getHash(bool with_filename = true) const {
      QoreHashNode* h = getHashIntern(with_filename);

      h->setKeyValue("user", &False, 0);
      h->setKeyValue("api_major", new QoreBigIntNode(api_major), 0);
      h->setKeyValue("api_minor", new QoreBigIntNode(api_minor), 0);

      return h;
   }

   DLLLOCAL const void* getPtr() const {
      return dlptr;
   }

   DLLLOCAL virtual void issueParseCmd(QoreString &cmd);
};

class QoreUserModule : public QoreAbstractModule {
protected:
   QoreProgram* pgm;
   QoreClosureParseNode* del; // deletion closure

   DLLLOCAL virtual void addToProgramImpl(QoreProgram* pgm, ExceptionSink& xsink) const;

public:
   DLLLOCAL QoreUserModule(const char* cwd, const char* fn, const char* n, QoreProgram* p, unsigned load_opt) : QoreAbstractModule(cwd, fn, n, load_opt), pgm(p), del(0) {
   }

   DLLLOCAL void set(const char* d, const char* v, const char* a, const char* u, const QoreString& l, QoreClosureParseNode* dl) {
      QoreAbstractModule::set(d, v, a, u, l);
      del = dl;
   }

   DLLLOCAL QoreProgram* getProgram() const {
      return pgm;
   }

   DLLLOCAL virtual ~QoreUserModule();

   DLLLOCAL virtual bool isBuiltin() const {
      return false;
   }

   DLLLOCAL virtual bool isUser() const {
      return true;
   }

   DLLLOCAL virtual QoreHashNode* getHash(bool with_filename = true) const {
      return getHashIntern(with_filename);
   }

   DLLLOCAL virtual void issueParseCmd(QoreString &cmd) {
      parseException("PARSE-COMMAND-ERROR", "module '%s' loaded from '%s' is a user module; only builtin modules can support parse commands", name.getBuffer(), filename.getBuffer());
   }
};

class QoreUserModuleDefContextHelper : public QoreModuleDefContextHelper {
protected:
   const char* old_name;

   qore_program_private* pgm;
   int64 po;

   ExceptionSink& xsink;
   bool dup;

public:
   DLLLOCAL QoreUserModuleDefContextHelper(const char* name, QoreProgram* p, ExceptionSink& xs);

   DLLLOCAL ~QoreUserModuleDefContextHelper() {
      const char* name = set_user_module_context_name(old_name);

      if (xsink && !dup)
         QMM.removeUserModuleDependency(name);
   }

   DLLLOCAL void setDuplicate() {
      assert(!dup);
      dup = true;
   }

   DLLLOCAL void setNameInit(const char* name);

   DLLLOCAL void close();
};

#endif
