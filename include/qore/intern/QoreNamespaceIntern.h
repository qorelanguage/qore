/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespaceIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

#ifndef _QORE_QORENAMESPACEINTERN_H
#define _QORE_QORENAMESPACEINTERN_H

#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/intern/ConstantList.h>
#include <qore/intern/FunctionList.h>
#include <qore/intern/GlobalVariableList.h>

#include <map>
#include <vector>

class qore_root_ns_private;

struct GVEntryBase {
   NamedScope* name;
   Var* var;

   DLLLOCAL GVEntryBase(const NamedScope& n, Var* v) : name(new NamedScope(n)), var(v) {
   }

   DLLLOCAL GVEntryBase(char* n, const QoreTypeInfo* typeInfo, QoreParseTypeInfo* parseTypeInfo) : name(new NamedScope(n)), var(typeInfo ? new Var(name->getIdentifier(), typeInfo) : new Var(name->getIdentifier(), parseTypeInfo)) {
   }

   DLLLOCAL GVEntryBase(const GVEntryBase& old) : name(old.name), var(old.var) {
   }

   DLLLOCAL void clear() {
      delete name;
      if (var)
         var->deref(0);
   }

   DLLLOCAL Var* takeVar() {
      Var* rv = var;
      var = 0;
      return rv;
   }
};

template <class T>
struct GVList : public std::vector<T> {
   DLLLOCAL ~GVList() {
      clear();
   }

   DLLLOCAL void clear() {
      for (typename GVList<T>::iterator i = std::vector<T>::begin(), e = std::vector<T>::end(); i != e; ++i)
         (*i).clear();
      std::vector<T>::clear();
   }

   DLLLOCAL void zero() {
      std::vector<T>::clear();      
   }

   DLLLOCAL void assimilate(GVList<T>& l) {
      
   }
};

typedef GVList<GVEntryBase> gvblist_t;

class qore_ns_private {
private:
   // not implemented
   DLLLOCAL qore_ns_private(const qore_ns_private&);
   // not implemented
   DLLLOCAL qore_ns_private& operator=(const qore_ns_private&);

protected:
   // called from the root namespace constructor only
   DLLLOCAL qore_ns_private(QoreNamespace *n_ns) : constant(this), pendConstant(this), depth(0), root(true), pub(true), builtin(true), parent(0), class_handler(0), ns(n_ns) {
   }

public:
   std::string name;

   QoreClassList classList,       // committed class map
      pendClassList;              // pending class map
   ConstantList constant,         // committed constant map
      pendConstant;               // pending constant map
   QoreNamespaceList nsl,         // committed namespace map
      pendNSL;                    // pending namespace map
   FunctionList func_list;        // function map
   GlobalVariableList var_list;   // global variable map
   gvblist_t pend_gvblist;        // global variable declaration list

   // 0 = root namespace, ...
   unsigned depth;

   bool root,  // is this the root namespace?
      pub,     // is this namespace public (inherited by child programs or programs importing user modules)
      builtin; // is this namespace builtin?

   const qore_ns_private* parent;       // pointer to parent namespace (0 if this is the root namespace or an unattached namespace)
   q_ns_class_handler_t class_handler;   
   QoreNamespace *ns;

   // used with builtin namespaces
   DLLLOCAL qore_ns_private(QoreNamespace *n_ns, const char* n) : name(n), constant(this), pendConstant(this), depth(0), root(false), pub(true), builtin(true), parent(0), class_handler(0), ns(n_ns) {
   }

   // called when parsing
   DLLLOCAL qore_ns_private();

   DLLLOCAL qore_ns_private(const qore_ns_private &old, int64 po) 
      : name(old.name), 
        classList(old.classList, po, this), 
        constant(old.constant, this),
        pendConstant(this),
        nsl(old.nsl, po, *this),
        func_list(old.func_list, this, po),
        var_list(old.var_list, po),
        depth(old.depth),
        root(old.root),
        pub(old.builtin ? true : false),
        builtin(old.builtin),
        parent(0), class_handler(old.class_handler), ns(0) {
   }

   DLLLOCAL ~qore_ns_private() {
      printd(5, "qore_ns_private::~qore_ns_private() this: %p '%s'\n", this, name.c_str());
   }

   DLLLOCAL void getPath(std::string& str, bool anchored = false) const {
      const qore_ns_private* w = parent;
      while (w && (anchored || w->parent)) {
         str.insert(0, "::");
         str.insert(0, w->name);
         w = w->parent;
      }

      // append this namespace's name
      str += name;
   }

   DLLLOCAL static QoreNamespace* newNamespace(const qore_ns_private& old, int64 po) {
      qore_ns_private* p = new qore_ns_private(old, po);
      return new QoreNamespace(p);
   }

   // destroys the object and frees all associated memory
   DLLLOCAL void purge() {
      constant.reset();
      pendConstant.reset();

      classList.reset();
      pendClassList.reset();

      nsl.reset();
      pendNSL.reset();
   }

   DLLLOCAL qore_root_ns_private* getRoot() {
      qore_ns_private* w = this;
      while (w->parent)
         w = (qore_ns_private*)w->parent;

      return w->root ? reinterpret_cast<qore_root_ns_private*>(w) : 0;
   }

   // finds a local class in the committed class list, if not found executes the class handler
   DLLLOCAL QoreClass *findLoadClass(const char* cname) {
      //printd(5, "qore_ns_private::findLoadClass('%s') this: %p ('%s') class_handler: %p found: %d\n", cname, this, name.c_str(), class_handler, classList.find(cname));
      QoreClass *qc = classList.find(cname);
      if (!qc && class_handler)
	 qc = class_handler(ns, cname);
      return qc;
   }

   DLLLOCAL void clearConstants(QoreListNode& l);
   DLLLOCAL void clearData(ExceptionSink* xsink);
   DLLLOCAL void deleteData(ExceptionSink* xsink);

   DLLLOCAL void parseAssimilate(QoreNamespace* ns);
   DLLLOCAL void runtimeAssimilate(QoreNamespace* ns);

   DLLLOCAL void updateDepthRecursive(unsigned ndepth);

   DLLLOCAL int parseAddPendingClass(const NamedScope& n, QoreClass* oc);
   DLLLOCAL int parseAddPendingClass(QoreClass* oc);

   DLLLOCAL cnemap_t::iterator parseAddConstant(const char* name, AbstractQoreNode* value, bool pub);

   DLLLOCAL void parseAddConstant(const NamedScope& name, AbstractQoreNode* value, bool pub);

   DLLLOCAL int parseAddMethodToClass(const NamedScope& name, MethodVariantBase *qcmethod, bool static_flag);

   DLLLOCAL int checkImportFunction(const char* name, ExceptionSink *xsink) {
      //printd(5, "qore_ns_private::checkImportFunction(%s) this: %p\n", name, this);

      if (func_list.findNode(name)) {
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' already exists in this namespace", name);
         return -1;
      }

      return 0;
   }

   DLLLOCAL FunctionEntry* importFunction(ExceptionSink* xsink, QoreFunction* u, const char* new_name = 0) {
      const char* fn = new_name ? new_name : u->getName();
      if (checkImportFunction(fn, xsink))
         return 0;

      return func_list.import(fn, u, this);
   }

   DLLLOCAL int checkImportClass(const char* cname, ExceptionSink *xsink) {
      //printd(5, "qore_ns_private::checkImportClass(%s) this: %p\n", name, this);

      if (classList.find(cname)) {
         xsink->raiseException("CLASS-IMPORT-ERROR", "class '%s' already exists in namespace '%s'", cname, name.c_str());
         return -1;
      }
      if (pendClassList.find(cname)) {
         xsink->raiseException("CLASS-IMPORT-ERROR", "class '%s' is already pending in namespace '%s'", cname, name.c_str());
         return -1;
      }
      if (nsl.find(cname)) {
         xsink->raiseException("CLASS-IMPORT-ERROR", "a subnamespace named '%s' already exists in namespace '%s'", cname, name.c_str());
         return -1;
      }
      if (pendNSL.find(cname)) {
         xsink->raiseException("CLASS-IMPORT-ERROR", "a subnamespace named '%s' is already pending in namespace '%s'", cname, name.c_str());
         return -1;
      }

      return 0;
   }

   DLLLOCAL QoreClass* importClass(ExceptionSink* xsink, const QoreClass* c) {
      if (checkImportClass(c->getName(), xsink))
         return 0;

      QoreClass* nc = new QoreClass(*c);
      qore_class_private::setNamespace(nc, this);
      classList.add(nc);
      return nc;
   }

   DLLLOCAL const QoreFunction* runtimeFindFunction(const char* name) {
      return func_list.find(name, true);
   }

   DLLLOCAL const QoreFunction* findAnyFunction(const char* name) {
      return func_list.find(name, false);
   }

   DLLLOCAL QoreNamespace* findCreateNamespace(const char* nme);
   DLLLOCAL QoreNamespace* findCreateNamespacePath(const char* nspath);

   DLLLOCAL AbstractQoreNode *getConstantValue(const char* name, const QoreTypeInfo* &typeInfo);
   DLLLOCAL QoreClass *parseFindLocalClass(const char* name);
   DLLLOCAL qore_ns_private* parseAddNamespace(QoreNamespace *nns);

   DLLLOCAL void addModuleNamespace(qore_ns_private* nns, QoreModuleContext& qmc);
   DLLLOCAL void addCommitNamespaceIntern(qore_ns_private* nns);
   DLLLOCAL void addNamespace(qore_ns_private* nns);

   DLLLOCAL void parseInit();
   DLLLOCAL void parseInitConstants();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();

   DLLLOCAL const QoreClass* runtimeMatchClass(const NamedScope& nscope, const qore_ns_private*& rns) const;
   DLLLOCAL const qore_ns_private* runtimeMatchAddClass(const NamedScope& nscope, bool& fnd) const;

   DLLLOCAL const QoreFunction* runtimeMatchFunction(const NamedScope& nscope, const qore_ns_private*& rns) const;
   DLLLOCAL const qore_ns_private* runtimeMatchAddFunction(const NamedScope& nscope, bool& fnd) const;

   DLLLOCAL const QoreFunction* parseMatchFunction(const NamedScope& nscope, unsigned& match) const;

   DLLLOCAL QoreNamespace *resolveNameScope(const NamedScope& name) const;
   DLLLOCAL QoreNamespace *parseMatchNamespace(const NamedScope& nscope, unsigned& matched) const;
   DLLLOCAL QoreClass *parseMatchScopedClass(const NamedScope& name, unsigned& matched);
   DLLLOCAL QoreClass *parseMatchScopedClassWithMethod(const NamedScope& nscope, unsigned& matched);
   DLLLOCAL AbstractQoreNode *parseCheckScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo* &typeInfo, bool abr) const;

   DLLLOCAL AbstractQoreNode *parseResolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo* &typeInfo) const;

   DLLLOCAL AbstractQoreNode *parseFindLocalConstantValue(const char* cname, const QoreTypeInfo* &typeInfo);
   DLLLOCAL QoreNamespace *parseFindLocalNamespace(const char* nname);

   DLLLOCAL AbstractQoreNode *parseMatchScopedConstantValue(const NamedScope& name, unsigned& matched, const QoreTypeInfo*& typeInfo);

   DLLLOCAL FunctionEntry* addPendingVariantIntern(const char* fname, AbstractQoreFunctionVariant* v, bool& new_func);

   DLLLOCAL void addBuiltinVariant(const char* name, AbstractQoreFunctionVariant* v);
   DLLLOCAL void addBuiltinModuleVariant(const char* name, AbstractQoreFunctionVariant* v, QoreModuleContext& qmc);
   DLLLOCAL void addBuiltinVariantIntern(const char* name, AbstractQoreFunctionVariant* v);

   template <typename T, class B>
   DLLLOCAL void addBuiltinVariant(const char* name, T f, int64 flags, int64 functional_domain, const QoreTypeInfo* returnTypeInfo, unsigned num_params, va_list args) {
      //printd(5, "qore_ns_private::addBuiltinVariant('%s', %p, flags=%lld) BEFORE\n", name, f, flags);
      type_vec_t typeList;
      arg_vec_t defaultArgList;
      name_vec_t nameList;
      if (num_params)
         qore_process_params(num_params, typeList, defaultArgList, nameList, args);

      //printd(5, "qore_ns_private::addBuiltinVariant('%s', %p, flags=%lld, domain=%lld, ret=%s, num_params=%d, ...)\n", name, f, flags, functional_domain, returnTypeInfo->getName(), num_params);
      addBuiltinVariant(name, new B(f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList, nameList));
   }
   
   DLLLOCAL void scanMergeCommittedNamespace(const qore_ns_private& mns, QoreModuleContext& qmc) const;
   DLLLOCAL void copyMergeCommittedNamespace(const qore_ns_private& mns);

   DLLLOCAL void parseInitGlobalVars();

   DLLLOCAL void checkGlobalVarDecl(Var* v, const NamedScope& vname);
   DLLLOCAL void parseAddGlobalVarDecl(char *name, const QoreTypeInfo* typeInfo, QoreParseTypeInfo* parseTypeInfo, bool pub);

   DLLLOCAL void setPublic();

   DLLLOCAL static void addNamespace(QoreNamespace& ns, QoreNamespace* nns) {
      ns.priv->addNamespace(nns->priv);
   }

   DLLLOCAL static AbstractQoreNode* parseResolveClassConstant(QoreClass* qc, const char* name, const QoreTypeInfo*& typeInfo);

   DLLLOCAL static ConstantList& getConstantList(const QoreNamespace *ns) {
      return ns->priv->constant;
   }

   DLLLOCAL static const QoreFunction* runtimeFindFunction(QoreNamespace& ns, const char* name) {
      return ns.priv->runtimeFindFunction(name);
   }

   DLLLOCAL static QoreListNode* getUserFunctionList(QoreNamespace& ns) {
      return ns.priv->func_list.getList(); 
   }

   DLLLOCAL static void parseAddPendingClass(QoreNamespace& ns, const NamedScope& n, QoreClass* oc) {
      ns.priv->parseAddPendingClass(n, oc);
   }

   DLLLOCAL static void parseAddNamespace(QoreNamespace& ns, QoreNamespace *nns) {
      ns.priv->parseAddNamespace(nns);
   }

   DLLLOCAL static void parseAddConstant(QoreNamespace& ns, const NamedScope& name, AbstractQoreNode* value, bool pub) {
      ns.priv->parseAddConstant(name, value, pub);
   }

   DLLLOCAL static void parseRollback(QoreNamespace& ns) {
      ns.priv->parseRollback();
   }

   DLLLOCAL static void parseCommit(QoreNamespace& ns) {
      ns.priv->parseCommit();
   }

   DLLLOCAL static void purge(QoreNamespace& ns) {
      ns.priv->purge();
   }

   DLLLOCAL static qore_ns_private* get(QoreNamespace& ns) {
      return ns.priv;
   }

   DLLLOCAL static const qore_ns_private* get(const QoreNamespace& ns) {
      return ns.priv;
   }

   DLLLOCAL static bool isPublic(const QoreNamespace& ns) {
      return ns.priv->pub;
   }

   DLLLOCAL static bool isUserPublic(const QoreNamespace& ns) {
      return ns.priv->pub && !ns.priv->builtin;
   }
};

struct namespace_iterator_element {
   qore_ns_private* ns;
   nsmap_t::iterator i;
   bool committed;        // use committed or pending namespace list

   DLLLOCAL namespace_iterator_element(qore_ns_private* n_ns, bool n_committed) : 
      ns(n_ns), i(n_committed ? ns->nsl.nsmap.begin() : ns->pendNSL.nsmap.begin()), committed(n_committed) {
      assert(ns);
   }

   DLLLOCAL bool atEnd() const {
      return i == (committed ? ns->nsl.nsmap.end() : ns->pendNSL.nsmap.end());
   }

   DLLLOCAL QoreNamespace* next() {
      ++i;
      return atEnd() ? 0 : i->second;
   }
};

class QorePrivateNamespaceIterator {
protected:
   typedef std::vector<namespace_iterator_element> nsv_t;
   nsv_t nsv; // stack of namespaces
   qore_ns_private* root; // for starting over when done
   bool committed;        // use committed or pending namespace list

   DLLLOCAL void set(qore_ns_private* rns) {
      nsv.push_back(namespace_iterator_element(rns, committed));
      while (!(committed ? rns->nsl.empty() : rns->pendNSL.empty())) {
         rns = qore_ns_private::get(*((committed ? rns->nsl.nsmap.begin()->second : rns->pendNSL.nsmap.begin()->second)));
         nsv.push_back(namespace_iterator_element(rns, committed));
      }
   }

public:
   DLLLOCAL QorePrivateNamespaceIterator(qore_ns_private* rns, bool n_committed) : root(rns), committed(n_committed) {
      assert(rns);
   }

   DLLLOCAL bool next() {
      // reset when starting over
      if (nsv.empty()) {
         set(root);
         return true;
      }

      namespace_iterator_element* nie = &(nsv.back());

      // if the last element of the current namespace list has been iterated, take it off the stack
      if (nie->atEnd()) {
         nsv.pop_back();
         if (nsv.empty())
            return false;

         nie = &(nsv.back());
      }

      QoreNamespace* next = nie->next();
      if (next)
         set(qore_ns_private::get(*next));

      return true;
   }

   DLLLOCAL qore_ns_private* operator->() {
      return nsv.back().ns;
   }

   DLLLOCAL qore_ns_private* operator*() {
      return nsv.back().ns;
   }

   DLLLOCAL qore_ns_private* get() {
      return nsv.back().ns;
   }
};

struct NSOInfoBase {
   qore_ns_private* ns;

   DLLLOCAL NSOInfoBase(qore_ns_private* n_ns) : ns(n_ns) {
   }

   DLLLOCAL unsigned depth() const {
      return ns->depth;
   }
};

template <typename T>
struct NSOInfo : public NSOInfoBase {
   // object
   T* obj;

   DLLLOCAL NSOInfo(qore_ns_private* n_ns, T* n_obj) : NSOInfoBase(n_ns), obj(n_obj) {
   }

   DLLLOCAL void assign(qore_ns_private* n_ns, T* n_obj) {
      ns = n_ns;
      obj = n_obj;
   }
};

template <typename T>
class RootMap : public std::map<const char*, NSOInfo<T>, ltstr> {
private:
   // not implemented
   DLLLOCAL RootMap(const RootMap& old);
   // not implemented
   DLLLOCAL RootMap& operator=(const RootMap& m);

public:
   typedef NSOInfo<T> info_t;
   typedef std::map<const char*, NSOInfo<T>, ltstr> map_t;

   DLLLOCAL RootMap() {
   }

   DLLLOCAL void update(const char* name, qore_ns_private* ns, T* obj) {
      // get current lookup map entry for this object
      typename map_t::iterator i = this->find(name);
      if (i == this->end())
         this->insert(typename map_t::value_type(name, info_t(ns, obj)));
      else // if the old depth is > the new depth, then replace
         if (i->second.depth() > ns->depth)
            i->second.assign(ns, obj);
   }

   DLLLOCAL void update(typename map_t::const_iterator ni) {
      // get current lookup map entry for this object
      typename map_t::iterator i = this->find(ni->first);
      if (i == this->end()) {
         //printd(5, "RootMap::update(iterator) inserting '%s' new depth: %d\n", ni->first, ni->second.depth());
         this->insert(typename map_t::value_type(ni->first, ni->second));
      }
      else {
         // if the old depth is > the new depth, then replace
         if (i->second.depth() > ni->second.depth()) {
            //printd(5, "RootMap::update(iterator) replacing '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
            i->second = ni->second;      
         }
         //else
         //printd(5, "RootMap::update(iterator) ignoring '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
      }
   }

   T* findObj(const char* name) {
      typename map_t::iterator i = this->find(name);
      return i == this->end() ? 0 : i->second.obj;
   }
};

struct FunctionEntryInfo {
   FunctionEntry* obj;

   DLLLOCAL FunctionEntryInfo(FunctionEntry* o) : obj(o) {
   }

   DLLLOCAL unsigned depth() const {
      return getNamespace()->depth;
   }

   DLLLOCAL qore_ns_private* getNamespace() const {
      return obj->getFunction()->getNamespace();
   }

   DLLLOCAL void assign(FunctionEntry* n_obj) {
      obj = n_obj;
   }
};

typedef std::map<const char*, FunctionEntryInfo, ltstr> femap_t;
class FunctionEntryRootMap : public femap_t {
private:
   // not implemented
   DLLLOCAL FunctionEntryRootMap(const FunctionEntryRootMap& old);
   // not implemented
   DLLLOCAL FunctionEntryRootMap& operator=(const FunctionEntryRootMap& m);

public:
   DLLLOCAL FunctionEntryRootMap() {
   }

   DLLLOCAL void update(const char* name, FunctionEntry* obj) {
      // get current lookup map entry for this object
      femap_t::iterator i = find(name);
      if (i == end())
         insert(femap_t::value_type(name, FunctionEntryInfo(obj)));
      else // if the old depth is > the new depth, then replace
         if (i->second.depth() > obj->getFunction()->getNamespace()->depth)
            i->second.assign(obj);
   }

   DLLLOCAL void update(femap_t::const_iterator ni) {
      // get current lookup map entry for this object
      femap_t::iterator i = find(ni->first);
      if (i == end()) {
         //printd(5, "FunctionEntryRootMap::update(iterator) inserting '%s' new depth: %d\n", ni->first, ni->second.depth());
         insert(femap_t::value_type(ni->first, ni->second));
      }
      else {
         // if the old depth is > the new depth, then replace
         if (i->second.depth() > ni->second.depth()) {
            //printd(5, "FunctionEntryRootMap::update(iterator) replacing '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
            i->second = ni->second;
         }
         //else
         //printd(5, "FunctionEntryRootMap::update(iterator) ignoring '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
      }
   }

   FunctionEntry* findObj(const char* name) {
      femap_t::iterator i = find(name);
      return i == end() ? 0 : i->second.obj;
   }
};

class NamespaceMap {
   friend class NamespaceMapIterator;
   friend class ConstNamespaceMapIterator;

protected:
   // map from depth to namespace
   typedef std::multimap<unsigned, qore_ns_private*> nsdmap_t;
   // map from name to depth map
   typedef std::map<const char*, nsdmap_t, ltstr> nsmap_t;
   // map from namespace to depth for reindexing
   typedef std::map<qore_ns_private*, unsigned> nsrmap_t;

   nsmap_t nsmap;   // name to depth to namespace map
   nsrmap_t nsrmap; // namespace to depth map (for fast reindexing)

   // not implemented
   DLLLOCAL NamespaceMap(const NamespaceMap& old);
   // not implemented
   DLLLOCAL NamespaceMap& operator=(const NamespaceMap& m);

public:
   DLLLOCAL NamespaceMap() {
   }

   DLLLOCAL void update(qore_ns_private* ns) {
      // if this namespace is already indexed, then reindex
      nsrmap_t::iterator ri = nsrmap.find(ns);
      if (ri != nsrmap.end()) {
         // if the depth is the same, then do nothing
         if (ns->depth == ri->second)
            return;

         // otherwise get the depth -> namespace map under this name
         nsmap_t::iterator i = nsmap.find(ns->name.c_str());
         assert(i != nsmap.end());

         // now get the namespace entry
         nsdmap_t::iterator di = i->second.find(ri->second);
         assert(di != i->second.end());

         // remove from depth -> namespace map
         i->second.erase(di);

         // remove from reverse map
         nsrmap.erase(ri);

         // add new entry to depth -> namespace map
         i->second.insert(nsdmap_t::value_type(ns->depth, ns));

         return;
      }
      else {
         // insert depth -> ns map entry
         nsmap_t::iterator i = nsmap.find(ns->name.c_str());
         if (i == nsmap.end())
            i = nsmap.insert(nsmap_t::value_type(ns->name.c_str(), nsdmap_t())).first;

         i->second.insert(nsdmap_t::value_type(ns->depth, ns));
      }

      // add new entry to reverse map
      nsrmap.insert(nsrmap_t::value_type(ns, ns->depth));
   }

   DLLLOCAL void commit(NamespaceMap& pend) {
      // commit entries
      for (nsrmap_t::iterator i = pend.nsrmap.begin(), e = pend.nsrmap.end(); i != e; ++i)
         update(i->first);
      pend.clear();
   }

   DLLLOCAL void clear() {
      nsmap.clear();
      nsrmap.clear();
   }
};

class NamespaceMapIterator {
protected:
   NamespaceMap::nsmap_t::iterator mi;
   NamespaceMap::nsdmap_t::iterator i;
   bool valid;

public:
   DLLLOCAL NamespaceMapIterator(NamespaceMap& nsm, const char* name) : mi(nsm.nsmap.find(name)), valid(mi != nsm.nsmap.end()) {
      if (valid)
         i = mi->second.end();
   }

   DLLLOCAL bool next() {
      if (!valid)
         return false;

      if (i == mi->second.end())
         i = mi->second.begin();
      else
         ++i;

      return i != mi->second.end();      
   }

   DLLLOCAL qore_ns_private* get() {
      return i->second;
   }
};

class ConstNamespaceMapIterator {
protected:
   NamespaceMap::nsmap_t::const_iterator mi;
   NamespaceMap::nsdmap_t::const_iterator i;
   bool valid;

public:
   DLLLOCAL ConstNamespaceMapIterator(const NamespaceMap& nsm, const char* name) : mi(nsm.nsmap.find(name)), valid(mi != nsm.nsmap.end()) {
      if (valid)
         i = mi->second.end();
   }

   DLLLOCAL bool next() {
      if (!valid)
         return false;

      if (i == mi->second.end())
         i = mi->second.begin();
      else
         ++i;

      return i != mi->second.end();      
   }

   DLLLOCAL const qore_ns_private* get() {
      return i->second;
   }
};

typedef FunctionEntryRootMap fmap_t;

typedef RootMap<ConstantEntry> cnmap_t;

typedef RootMap<QoreClass> clmap_t;

typedef RootMap<Var> varmap_t;

struct GVEntry : public GVEntryBase {
   qore_ns_private* ns;

   DLLLOCAL GVEntry(qore_ns_private* n_ns, const NamedScope& n, Var* v) : GVEntryBase(n, v), ns(n_ns) {
   }

   DLLLOCAL GVEntry(const GVEntry& old) : GVEntryBase(old), ns(old.ns) {
   }

   DLLLOCAL GVEntry(const GVEntryBase& old, qore_ns_private* n_ns) : GVEntryBase(old), ns(n_ns) {
   }
};

typedef GVList<GVEntry> gvlist_t;

class qore_root_ns_private : public qore_ns_private {
   friend class qore_ns_private;

protected:
   DLLLOCAL int addPendingVariantIntern(qore_ns_private& ns, const char* name, AbstractQoreFunctionVariant* v) {
      // try to add function variant to given namespace
      bool new_func = false;
      FunctionEntry* fe = ns.addPendingVariantIntern(name, v, new_func);
      if (!fe)
         return -1;

      assert(fe->getNamespace() == &ns);

      if (new_func) {
         fmap_t::iterator i = fmap.find(fe->getName());
         // only add to pending map if either not in the committed map or the depth is higher in the committed map
         if (i == fmap.end() || i->second.depth() > ns.depth)
            pend_fmap.update(fe->getName(), fe);
      }

      return 0;      
   }

   DLLLOCAL int addPendingVariantIntern(qore_ns_private& ns, const NamedScope& nscope, AbstractQoreFunctionVariant* v) {
      assert(nscope.size() > 1);
      SimpleRefHolder<AbstractQoreFunctionVariant> vh(v);

      QoreNamespace* fns = ns.ns;
      for (unsigned i = 0; i < nscope.size() - 1; ++i) {
         fns = fns->priv->parseFindLocalNamespace(nscope[i]);
         if (!fns) {
            parse_error("cannot find namespace '%s::' in '%s()' as a child of namespace '%s::'", nscope[i], nscope.ostr, ns.name.c_str());
            return -1;
         }
      }

      return addPendingVariantIntern(*fns->priv, nscope.getIdentifier(), vh.release());
   }

   // performed at runtime
   DLLLOCAL int importClass(ExceptionSink *xsink, qore_ns_private& ns, const QoreClass *c) {
      QoreClass* nc = ns.importClass(xsink, c);
      if (!nc)
         return -1;

      //printd(5, "qore_root_ns_private::importClass() this: %p ns: %p '%s' (depth %d) func: %p %s\n", this, &ns, ns.name.c_str(), ns.depth, u, c->getName());

      clmap.update(nc->getName(), &ns, nc);
      return 0;
   }

   // performed at runtime
   DLLLOCAL int importFunction(ExceptionSink *xsink, qore_ns_private& ns, QoreFunction *u, const char* new_name = 0) {
      FunctionEntry* fe = ns.importFunction(xsink, u, new_name);
      if (!fe)
         return -1;

      assert(fe->getNamespace() == &ns);

      //printd(5, "qore_root_ns_private::importFunction() this: %p ns: %p '%s' (depth %d) func: %p %s\n", this, &ns, ns.name.c_str(), ns.depth, u, fe->getName());

      fmap.update(fe->getName(), fe);
      return 0;
   }

   DLLLOCAL bool runtimeExistsFunctionIntern(const char* name) {
      return fmap.find(name) != fmap.end();
   }

   DLLLOCAL const QoreClass* runtimeFindClassIntern(const char* name, const qore_ns_private*& ns) {
      clmap_t::iterator i = clmap.find(name);

      if (i != clmap.end()) {
         ns = i->second.ns;
         //printd(5, "qore_root_ns_private::runtimeFindClassIntern() this: %p %s found in ns: '%s' depth: %d\n", this, name, ns->name.c_str(), ns->depth);
         return i->second.obj;
      }

      return 0;
   }

   DLLLOCAL const QoreClass* runtimeFindClassIntern(const NamedScope& name, const qore_ns_private*& ns);

   DLLLOCAL const QoreFunction* runtimeFindFunctionIntern(const char* name, const qore_ns_private*& ns) {
      fmap_t::iterator i = fmap.find(name);

      if (i != fmap.end()) {
         ns = i->second.getNamespace();
         //printd(5, "qore_root_ns_private::runtimeFindFunctionIntern() this: %p %s found in ns: '%s' depth: %d\n", this, name, ns->name.c_str(), ns->depth);
         return i->second.obj->getFunction();
      }

      //printd(5, "qore_root_ns_private::runtimeFindFunctionIntern() this: %p %s not found i: %d\n", this, name, i != fmap.end());
      return 0;
   }

   DLLLOCAL const QoreFunction* runtimeFindFunctionIntern(const NamedScope& name, const qore_ns_private*& ns);

   DLLLOCAL FunctionEntry* parseFindFunctionEntryIntern(const char* name) {
      {
         // try to check in current namespace first
         qore_ns_private* nscx = parse_get_ns();
         if (nscx) {
            FunctionEntry* fe = nscx->func_list.findNode(name);
            if (fe)
               return fe;
         }
      }

      fmap_t::iterator i = fmap.find(name);
      fmap_t::iterator ip = pend_fmap.find(name);

      if (i != fmap.end()) {
         if (ip != pend_fmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj;

            return ip->second.obj;
         }
 
         return i->second.obj;
      }

      if (ip != pend_fmap.end())
         return ip->second.obj;

      return 0;
   }

   DLLLOCAL QoreFunction* parseFindFunctionIntern(const char* name) {
      FunctionEntry* fe = parseFindFunctionEntryIntern(name);
      return !fe ? 0 : fe->getFunction();
   }

   DLLLOCAL const QoreFunction* parseResolveFunctionIntern(const char* fname) {
      QORE_TRACE("qore_root_ns_private::parseResolveFunctionIntern()");

      const QoreFunction* f = parseFindFunctionIntern(fname);
      if (!f)
         // cannot find function, throw exception
         parse_error("function '%s()' cannot be found", fname);

      return f;
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL AbstractCallReferenceNode* parseResolveCallReferenceIntern(UnresolvedProgramCallReferenceNode* fr);

   DLLLOCAL void parseCommit() {
      // commit pending function lookup entries
      for (fmap_t::iterator i = pend_fmap.begin(), e = pend_fmap.end(); i != e; ++i)
         fmap.update(i);
      pend_fmap.clear();

      // commit pending constant lookup entries
      for (cnmap_t::iterator i = pend_cnmap.begin(), e = pend_cnmap.end(); i != e; ++i)
         cnmap.update(i);
      pend_cnmap.clear();
      
      // commit pending class lookup entries
      for (clmap_t::iterator i = pend_clmap.begin(), e = pend_clmap.end(); i != e; ++i)
         clmap.update(i);
      pend_clmap.clear();

      // commit pending global variable lookup entries
      for (varmap_t::iterator i = pend_varmap.begin(), e = pend_varmap.end(); i != e; ++i)
         varmap.update(i);
      pend_varmap.clear();

      // commit pending namespace entries
      nsmap.commit(pend_nsmap);

      qore_ns_private::parseCommit();
   }

   DLLLOCAL void parseRollback() {
      // roll back pending lookup entries
      pend_fmap.clear();
      pend_cnmap.clear();
      pend_clmap.clear();
      pend_varmap.clear();
      pend_nsmap.clear();

      // roll back pending global variables
      pend_gvlist.clear();

      qore_ns_private::parseRollback();
   }

   DLLLOCAL ConstantEntry* parseFindOnlyConstantEntryIntern(const char* cname, qore_ns_private*& ns) {
      {
         // first try to look in current namespace context
         qore_ns_private* nscx = parse_get_ns();
         if (nscx) {
            ConstantEntry* ce = nscx->constant.findEntry(cname);
            if (!ce)
               ce = nscx->pendConstant.findEntry(cname);
            if (ce) {
               ns = nscx;
               return ce;
            }
         }
      }

      // look up in global constant map
      cnmap_t::iterator i = cnmap.find(cname);
      cnmap_t::iterator ip = pend_cnmap.find(cname);

      if (i != cnmap.end()) {
         if (ip != pend_cnmap.end()) {
            if (i->second.depth() < ip->second.depth()) {
               ns = i->second.ns;
               return i->second.obj;
            }

            ns = ip->second.ns;
            return ip->second.obj;;
         }

         ns = i->second.ns;
         return i->second.obj;;
      }

      if (ip != pend_cnmap.end()) {
         ns = ip->second.ns;
         return ip->second.obj;;
      }

      return 0;
   }

   DLLLOCAL AbstractQoreNode* parseFindOnlyConstantValueIntern(const char* cname, const QoreTypeInfo*& typeInfo) {
      qore_ns_private* ns;
      ConstantEntry *ce = parseFindOnlyConstantEntryIntern(cname, ns);
      if (!ce)
         return 0;

      //printd(5, "qore_root_ns_private::parseFindOnlyConstantValueIntern() const: %s ns: %p %s\n", cname, ns, ns->name.c_str());

      NamespaceParseContextHelper nspch(ns);
      return ce->get(typeInfo, this);
   }

   DLLLOCAL AbstractQoreNode* parseFindConstantValueIntern(const char* cname, const QoreTypeInfo*& typeInfo, bool error) {
      // look up class constants first
      QoreClass* pc = getParseClass();
      if (pc) {
         AbstractQoreNode* rv = qore_class_private::parseFindConstantValue(pc, cname, typeInfo);
         if (rv)
            return rv;
      }

      AbstractQoreNode* rv = parseFindOnlyConstantValueIntern(cname, typeInfo);

      if (rv)
         return rv;

      if (error)
         parse_error("constant '%s' cannot be resolved in any namespace", cname);      

      return 0;
   }

   DLLLOCAL ResolvedCallReferenceNode* runtimeGetCallReference(const char* fname, ExceptionSink* xsink) {
      fmap_t::iterator i = fmap.find(fname);
      if (i == fmap.end()) {
         xsink->raiseException("NO-SUCH-FUNCTION", "callback function '%s()' does not exist", fname);
         return 0;
      }

      return i->second.obj->makeCallReference();
   }

   DLLLOCAL QoreClass *parseFindScopedClassIntern(const QoreProgramLocation& loc, const NamedScope& name);
   DLLLOCAL QoreClass *parseFindScopedClassIntern(const NamedScope& name, unsigned& matched);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethodInternError(const NamedScope& name, bool error);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethodIntern(const NamedScope& name, unsigned& matched);

   DLLLOCAL QoreClass* parseFindClassIntern(const char* cname) {
      {
         // try to check in current namespace first
         qore_ns_private* nscx = parse_get_ns();
         if (nscx) {
            QoreClass* qc = nscx->parseFindLocalClass(cname);
            if (qc)
               return qc;
         }
      }

      clmap_t::iterator i = clmap.find(cname);
      clmap_t::iterator ip = pend_clmap.find(cname);

      if (i != clmap.end()) {
         if (ip != pend_clmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj;
            return ip->second.obj;
         }

         return i->second.obj;
      }

      if (ip != pend_clmap.end())
         return ip->second.obj;

      return 0;
   }

   DLLLOCAL QoreClass* runtimeFindClass(const char* name) {
      clmap_t::iterator i = clmap.find(name);
      return i != clmap.end() ? i->second.obj : 0;
   }

   DLLLOCAL QoreNamespace* runtimeFindNamespaceForAddFunction(const NamedScope& name, ExceptionSink* xsink) {
      //printd(5, "QoreNamespaceIntern::runtimeFindNamespaceForAddFunction() this: %p name: %s (%s)\n", this, name.ostr, name[0]);
      bool fnd = false;

      // iterate all namespaces with the initial name and look for the match
      NamespaceMapIterator nmi(nsmap, name[0]);
      while (nmi.next()) {
         const qore_ns_private* rv = nmi.get()->runtimeMatchAddFunction(name, fnd);
         //printd(5, "QoreNamespaceIntern::runtimeFindNamespaceForAddFunction() this: %p name: %s ns: %p '%s' rv: %p fnd: %d\n", this, name.ostr, nmi.get(), nmi.get()->name.c_str(), rv, fnd);
         if (rv)
            return const_cast<QoreNamespace*>(rv->ns);
      }
      
      if (fnd)
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "target function '%s' already exists in the given namespace", name.ostr);
      else
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "target namespace in '%s' does not exist", name.ostr);
      return 0;      
   }

   DLLLOCAL QoreNamespace *runtimeFindNamespaceForAddClass(const NamedScope& name, ExceptionSink* xsink) {
      bool fnd = false;

      // iterate all namespaces with the initial name and look for the match
      NamespaceMapIterator nmi(nsmap, name.strlist[0].c_str());
      while (nmi.next()) {
         const qore_ns_private* rv = nmi.get()->runtimeMatchAddClass(name, fnd);
         if (rv)
            return const_cast<QoreNamespace*>(rv->ns);
      }

      if (fnd)
         xsink->raiseException("CLASS-IMPORT-ERROR", "target class '%s' already exists in the given namespace", name.ostr);
      else
         xsink->raiseException("CLASS-IMPORT-ERROR", "target namespace in '%s' does not exist", name.ostr);
      return 0;
   }

   DLLLOCAL void addConstant(qore_ns_private& ns, const char* cname, AbstractQoreNode *value, const QoreTypeInfo* typeInfo);

   DLLLOCAL AbstractQoreNode *parseFindConstantValueIntern(const NamedScope& name, const QoreTypeInfo*& typeInfo, bool error);

   DLLLOCAL AbstractQoreNode* parseResolveBarewordIntern(const QoreProgramLocation& loc, const char* bword, const QoreTypeInfo*& typeInfo);

   DLLLOCAL AbstractQoreNode *parseResolveScopedReferenceIntern(const NamedScope& name, const QoreTypeInfo* &typeInfo);

   DLLLOCAL void parseAddConstantIntern(QoreNamespace& ns, const NamedScope& name, AbstractQoreNode* value, bool pub);

   DLLLOCAL void parseAddClassIntern(const NamedScope& name, QoreClass *oc);

   DLLLOCAL qore_ns_private *parseResolveNamespaceIntern(const NamedScope& nscope, qore_ns_private* sns, const QoreProgramLocation* loc = 0);
   DLLLOCAL qore_ns_private *parseResolveNamespace(const NamedScope& nscope, qore_ns_private* sns, const QoreProgramLocation* loc = 0);
   DLLLOCAL qore_ns_private *parseResolveNamespace(const NamedScope& nscope);

   DLLLOCAL const QoreFunction* parseResolveFunctionIntern(const NamedScope& nscope);

   DLLLOCAL Var* parseAddResolvedGlobalVarDefIntern(const NamedScope& name, const QoreTypeInfo* typeInfo);
   DLLLOCAL Var* parseAddGlobalVarDefIntern(const NamedScope& name, QoreParseTypeInfo* typeInfo);

   DLLLOCAL Var* parseCheckImplicitGlobalVarIntern(const QoreProgramLocation& loc, const NamedScope& name, const QoreTypeInfo* typeInfo);

   DLLLOCAL Var* parseFindGlobalVarIntern(const NamedScope& vname) {
      assert(vname.size() > 1);

      Var* rv = 0;
      unsigned match = 0;

      {
         // try to check in current namespace first
         qore_ns_private* nscx = parse_get_ns();
         if (nscx && nscx->name == vname[0]) {
            QoreNamespace* vns = nscx->parseMatchNamespace(vname, match);
            if (vns && (rv = vns->priv->var_list.parseFindVar(vname.getIdentifier())))
               return rv;
         }
      }

      // iterate all namespaces with the initial name and look for the match
      {
         NamespaceMapIterator nmi(nsmap, vname[0]);
         while (nmi.next()) {
            QoreNamespace* vns = nmi.get()->parseMatchNamespace(vname, match);
            if (vns && (rv = vns->priv->var_list.parseFindVar(vname.getIdentifier())))
               return rv;
         }
      }

      {
         NamespaceMapIterator nmi(pend_nsmap, vname[0]);
         while (nmi.next()) {
            QoreNamespace* vns = nmi.get()->parseMatchNamespace(vname, match);
            if (vns && (rv = vns->priv->var_list.parseFindVar(vname.getIdentifier())))
               return rv;
         }
      }

      return rv;
   }

   DLLLOCAL Var* parseFindGlobalVarIntern(const char* vname) {
      {
         // try to check in current namespace first
         qore_ns_private* nscx = parse_get_ns();
         if (nscx) {
            Var* v = nscx->var_list.parseFindVar(vname);
            if (v)
               return v;
         }

         //printd(5, "qore_root_ns_private::parseFindGlobalVarIntern() this: %p '%s' nscx: %p ('%s') varmap: %d pend_varmap: %d\n", this, vname, nscx, nscx ? nscx->name.c_str() : "n/a", varmap.find(vname) != varmap.end(), pend_varmap.find(vname) != pend_varmap.end());
      }

      varmap_t::iterator i = varmap.find(vname);
      varmap_t::iterator ip = pend_varmap.find(vname);

      if (i != varmap.end()) {
         if (ip != pend_varmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj;
            return ip->second.obj;
         }

         return i->second.obj;
      }

      if (ip != pend_varmap.end())
         return ip->second.obj;

      return 0;
   }

   DLLLOCAL Var* runtimeFindGlobalVar(const char* vname, const qore_ns_private*& vns) const {
      varmap_t::const_iterator i = varmap.find(vname);
      if (i != varmap.end()) {
         assert(i->second.ns);
         vns = i->second.ns;
         return i->second.obj;
      }
      return 0;
   }

   DLLLOCAL void importGlobalVariable(qore_ns_private& tns, Var* v, bool readonly, ExceptionSink* xsink) {
      Var* var = tns.var_list.import(v, xsink, readonly);
      if (!var)
         return;

      varmap.update(var->getName(), &tns, var);
   }

   DLLLOCAL Var* runtimeCreateVar(qore_ns_private& vns, const char* vname, const QoreTypeInfo* typeInfo) {
      Var* v = vns.var_list.runtimeCreateVar(vname, typeInfo);

      if (v)
         varmap.update(v->getName(), &vns, v);
      return v;
   }

   DLLLOCAL void parseResolveGlobalVarsIntern();

   // returns 0 for success, non-zero for error
   DLLLOCAL int parseAddMethodToClassIntern(const NamedScope& name, MethodVariantBase *qcmethod, bool static_flag);

   DLLLOCAL static void rebuildConstantIndexes(cnmap_t& cnmap, ConstantList& cl, qore_ns_private* ns) {
      ConstantListIterator cli(cl);
      while (cli.next())
         cnmap.update(cli.getName().c_str(), ns, cli.getEntry());
   }

   DLLLOCAL static void rebuildClassIndexes(clmap_t& clmap, QoreClassList& cl, qore_ns_private* ns) {
      ClassListIterator cli(cl);
      while (cli.next())
         clmap.update(cli.getName(), ns, cli.get());
   }

   DLLLOCAL void rebuildIndexes(qore_ns_private* ns) {
      // process function indexes
      for (fl_map_t::iterator i = ns->func_list.begin(), e = ns->func_list.end(); i != e; ++i) {
         assert(i->second->getFunction()->getNamespace() == ns);

         fmap.update(i->first, i->second);
         //printd(5, "qore_root_ns_private::rebuildIndexes() this: %p ns: %p func %s\n", this, ns, i->first);
      }

      // process variable indexes
      for (map_var_t::iterator i = ns->var_list.vmap.begin(), e = ns->var_list.vmap.end(); i != e; ++i)
         varmap.update(i->first, ns, i->second);

      // process constant indexes
      rebuildConstantIndexes(cnmap, ns->constant, ns);

      // process class indexes
      rebuildClassIndexes(clmap, ns->classList, ns);

      // reindex namespace
      nsmap.update(ns);
   }

   DLLLOCAL void parseRebuildIndexes(qore_ns_private* ns) {
      //printd(5, "qore_root_ns_private::parseRebuildIndexes() this: %p ns: %p (%s) depth %d\n", this, ns, ns->name.c_str(), ns->depth);
      
      // process function indexes
      for (fl_map_t::iterator i = ns->func_list.begin(), e = ns->func_list.end(); i != e; ++i) {
         assert(i->second->getFunction()->getNamespace() == ns);
         pend_fmap.update(i->first, i->second);
      }

      // process pending variable indexes
      for (map_var_t::iterator i = ns->var_list.pending_vmap.begin(), e = ns->var_list.pending_vmap.end(); i != e; ++i)
         pend_varmap.update(i->first, ns, i->second);

      // process variable indexes
      for (map_var_t::iterator i = ns->var_list.vmap.begin(), e = ns->var_list.vmap.end(); i != e; ++i)
         varmap.update(i->first, ns, i->second);

      // process pending constant indexes
      rebuildConstantIndexes(pend_cnmap, ns->pendConstant, ns);

      // process constant indexes
      rebuildConstantIndexes(cnmap, ns->constant, ns);

      // process pending class indexes
      rebuildClassIndexes(pend_clmap, ns->pendClassList, ns);

      // process class indexes
      rebuildClassIndexes(clmap, ns->classList, ns);

      // reindex namespace
      pend_nsmap.update(ns);
   }

   DLLLOCAL void parseAddNamespaceIntern(QoreNamespace *nns) {
      qore_ns_private *ns = qore_ns_private::parseAddNamespace(nns);
      if (!ns)
         return;

      // add all objects to the new (or assimilated) namespace

      //printd(5, "qore_root_ns_private::parseAddNamespaceIntern() this: %p ns: %p\n", this, ns);

      // take global variable decls
      for (unsigned i = 0; i < ns->pend_gvblist.size(); ++i) {
         //printd(5, "qore_root_ns_private::parseAddNamespaceIntern() merging global var decl '%s::%s' into the root list\n", ns->name.c_str(), ns->pend_gvblist[i].name->ostr);
         pend_gvlist.push_back(GVEntry(ns->pend_gvblist[i], ns));
      }
      ns->pend_gvblist.zero();

      QorePrivateNamespaceIterator qpni(ns, false);
      while (qpni.next())
         parseRebuildIndexes(qpni.get());
   }

   DLLLOCAL void rebuildAllIndexes() {
      // rebuild root indexes - only for committed objects
      QorePrivateNamespaceIterator qpni(this, true);
      while (qpni.next())
         rebuildIndexes(qpni.get());
   }

public:
   RootQoreNamespace* rns;
   QoreNamespace* qoreNS;

   fmap_t fmap,         // root function map
      pend_fmap;        // root pending function map (only used during parsing)

   cnmap_t cnmap,       // root constant map
      pend_cnmap;       // root pending constant map (used only during parsing)
   
   clmap_t clmap,       // root class map
      pend_clmap;       // root pending class map (used only during parsing)

   varmap_t varmap,     // root variable map
      pend_varmap;      // root pending variable map (used only during parsing)
   
   NamespaceMap nsmap,  // root namespace map
      pend_nsmap;       // root pending namespace map (used only during parsing)

   // unresolved pending global variable list - only used in the 1st stage of parsing (data read in to tree)
   gvlist_t pend_gvlist;

   DLLLOCAL qore_root_ns_private(RootQoreNamespace* n_rns) : qore_ns_private(n_rns), rns(n_rns), qoreNS(0) {
      assert(root);
      assert(pub);
      // add initial namespace to committed map
      nsmap.update(this);
   }

   DLLLOCAL qore_root_ns_private(const qore_root_ns_private& old, int64 po) : qore_ns_private(old, po) {
      qoreNS = nsl.find("Qore");
      assert(qoreNS);

      // always set the module public flag to true in the root namespace
      pub = true;

      // rebuild root indexes - only for committed objects
      rebuildAllIndexes();
   }

   DLLLOCAL ~qore_root_ns_private() {
   }

   DLLLOCAL RootQoreNamespace* copy(int64 po) {
      qore_root_ns_private* p = new qore_root_ns_private(*this, po);
      RootQoreNamespace* rv = new RootQoreNamespace(p);
      return rv;
   }

   DLLLOCAL qore_ns_private* getQore() {
      return qoreNS->priv;
   }

   DLLLOCAL const qore_ns_private* getQore() const {
      return qoreNS->priv;
   }

   DLLLOCAL void commitModule(QoreModuleContext& qmc) {
      for (unsigned j = 0; j < qmc.mcnl.size(); ++j) {
         ModuleContextNamespaceCommit& mc = qmc.mcnl[j];
         mc.parent->addCommitNamespaceIntern(mc.nns);
      }

      for (unsigned j = 0; j < qmc.mcfl.size(); ++j) {
         ModuleContextFunctionCommit& mc = qmc.mcfl[j];
         mc.parent->addBuiltinVariantIntern(mc.name, mc.v);
      }
   }

   DLLLOCAL static RootQoreNamespace* copy(const RootQoreNamespace& rns, int64 po) {
      return rns.rpriv->copy(po);
   }

   DLLLOCAL static int addPendingVariant(qore_ns_private& nsp, const char* name, AbstractQoreFunctionVariant* v) {
      return getRootNS()->rpriv->addPendingVariantIntern(nsp, name, v);
   }

   DLLLOCAL static int addPendingVariant(qore_ns_private& nsp, const NamedScope& name, AbstractQoreFunctionVariant* v) {
      return getRootNS()->rpriv->addPendingVariantIntern(nsp, name, v);
   }

   DLLLOCAL static int importFunction(RootQoreNamespace& rns, ExceptionSink *xsink, QoreNamespace& ns, QoreFunction *u, const char* new_name = 0) {
      return rns.rpriv->importFunction(xsink, *ns.priv, u, new_name);
   }

   DLLLOCAL static int importClass(RootQoreNamespace& rns, ExceptionSink* xsink, QoreNamespace& ns, const QoreClass *c) {
      return rns.rpriv->importClass(xsink, *ns.priv, c);
   }

   DLLLOCAL static const QoreClass* runtimeFindClass(RootQoreNamespace& rns, const char* name, const qore_ns_private*& ns) {
      if (strstr(name, "::")) {
         NamedScope nscope(name);
         return rns.rpriv->runtimeFindClassIntern(nscope, ns);
      }
      return rns.rpriv->runtimeFindClassIntern(name, ns);
   }

   DLLLOCAL static const QoreFunction* runtimeFindFunction(RootQoreNamespace& rns, const char* name, const qore_ns_private*& ns) {
      if (strstr(name, "::")) {
         NamedScope nscope(name);
         return rns.rpriv->runtimeFindFunctionIntern(nscope, ns); 
      }
      return rns.rpriv->runtimeFindFunctionIntern(name, ns);
   }

   DLLLOCAL static bool runtimeExistsFunction(RootQoreNamespace& rns, const char* name) {
      return rns.rpriv->runtimeExistsFunctionIntern(name);
   }

   DLLLOCAL static void addConstant(qore_root_ns_private& rns, qore_ns_private& ns, const char* cname, AbstractQoreNode *value, const QoreTypeInfo* typeInfo) {
      rns.addConstant(ns, cname, value, typeInfo);
   }

   DLLLOCAL static const QoreFunction* parseResolveFunction(const char* fname) {
      return getRootNS()->rpriv->parseResolveFunctionIntern(fname);
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL static AbstractCallReferenceNode* parseResolveCallReference(UnresolvedProgramCallReferenceNode* fr) {
      return getRootNS()->rpriv->parseResolveCallReferenceIntern(fr);
   }

   DLLLOCAL static void parseResolveGlobalVars() {
      getRootNS()->rpriv->parseResolveGlobalVarsIntern();
   }

   DLLLOCAL static void parseInit() {
      qore_ns_private* p = getRootNS()->priv;
      p->parseInitGlobalVars();
      p->parseInitConstants();
      p->parseInit();
   }

   DLLLOCAL static void parseCommit(RootQoreNamespace& rns) {
      rns.rpriv->parseCommit();
   }

   DLLLOCAL static void parseRollback(RootQoreNamespace& rns) {
      rns.rpriv->parseRollback();
   }

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const char* name, const QoreTypeInfo* &typeInfo, bool error) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo, error);
   }

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const NamedScope& name, const QoreTypeInfo* &typeInfo, bool error) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo, error);
   }

   DLLLOCAL static AbstractQoreNode* parseResolveBareword(const QoreProgramLocation& loc, const char* bword, const QoreTypeInfo* &typeInfo) {
      return getRootNS()->rpriv->parseResolveBarewordIntern(loc, bword, typeInfo);
   }

   DLLLOCAL static AbstractQoreNode *parseResolveScopedReference(const NamedScope& name, const QoreTypeInfo*& typeInfo) {
      return getRootNS()->rpriv->parseResolveScopedReferenceIntern(name, typeInfo);
   }

   DLLLOCAL static QoreClass *parseFindClass(const QoreProgramLocation& loc, const char* name) {
      QoreClass* qc = getRootNS()->rpriv->parseFindClassIntern(name);
      if (!qc)
         parse_error(loc, "reference to undefined class '%s'", name);
      return qc;
   }

   DLLLOCAL static QoreClass *parseFindScopedClass(const QoreProgramLocation& loc, const NamedScope& name) {
      return getRootNS()->rpriv->parseFindScopedClassIntern(loc, name);
   }

   DLLLOCAL static QoreClass *parseFindScopedClassWithMethod(const NamedScope& name, bool error) {
      return getRootNS()->rpriv->parseFindScopedClassWithMethodInternError(name, error);
   }

   DLLLOCAL static void parseAddConstant(QoreNamespace& ns, const NamedScope &name, AbstractQoreNode *value, bool pub) {
      getRootNS()->rpriv->parseAddConstantIntern(ns, name, value, pub);
   }

   // returns 0 for success, non-zero for error
   DLLLOCAL static int parseAddMethodToClass(const NamedScope& name, MethodVariantBase *qcmethod, bool static_flag) {
      return getRootNS()->rpriv->parseAddMethodToClassIntern(name, qcmethod, static_flag);
   }

   DLLLOCAL static void parseAddClass(const NamedScope& name, QoreClass *oc) {
      getRootNS()->rpriv->parseAddClassIntern(name, oc);
   }

   DLLLOCAL static void parseAddNamespace(QoreNamespace *nns) {
      getRootNS()->rpriv->parseAddNamespaceIntern(nns);
   }

   DLLLOCAL static const QoreFunction* parseResolveFunction(const NamedScope& nscope) {
      return getRootNS()->rpriv->parseResolveFunctionIntern(nscope);
   }

   DLLLOCAL static ResolvedCallReferenceNode* runtimeGetCallReference(RootQoreNamespace& rns, const char* name, ExceptionSink* xsink) {
      return rns.rpriv->runtimeGetCallReference(name, xsink);
   }

   DLLLOCAL static Var* parseAddResolvedGlobalVarDef(const NamedScope& vname, const QoreTypeInfo* typeInfo) {
      return getRootNS()->rpriv->parseAddResolvedGlobalVarDefIntern(vname, typeInfo);
   }

   DLLLOCAL static Var* parseAddGlobalVarDef(const NamedScope& vname, QoreParseTypeInfo* typeInfo) {
      return getRootNS()->rpriv->parseAddGlobalVarDefIntern(vname, typeInfo);
   }

   DLLLOCAL static Var* parseCheckImplicitGlobalVar(const QoreProgramLocation& loc, const NamedScope& name, const QoreTypeInfo* typeInfo) {
      return getRootNS()->rpriv->parseCheckImplicitGlobalVarIntern(loc, name, typeInfo);
   }

   DLLLOCAL static Var* parseFindGlobalVar(const char* vname) {
      return getRootNS()->rpriv->parseFindGlobalVarIntern(vname);
   }

   DLLLOCAL static Var* parseFindGlobalVar(const NamedScope& nscope) {
      return getRootNS()->rpriv->parseFindGlobalVarIntern(nscope);
   }

   DLLLOCAL static void scanMergeCommittedNamespace(const RootQoreNamespace& ns, const RootQoreNamespace& mns, QoreModuleContext& qmc) {
      ns.priv->scanMergeCommittedNamespace(*(mns.priv), qmc);
   }

   DLLLOCAL static void copyMergeCommittedNamespace(RootQoreNamespace& ns, const RootQoreNamespace& mns) {
      ns.priv->copyMergeCommittedNamespace(*(mns.priv));

      // rebuild root indexes - only for committed objects
      ns.rpriv->rebuildAllIndexes();
   }

   DLLLOCAL static Var* runtimeFindGlobalVar(const RootQoreNamespace& rns, const char* vname, const qore_ns_private*& vns) {
      return rns.rpriv->runtimeFindGlobalVar(vname, vns);
   }

   DLLLOCAL static Var* runtimeCreateVar(RootQoreNamespace& rns, QoreNamespace& vns, const char* vname, const QoreTypeInfo* typeInfo) {
      return rns.rpriv->runtimeCreateVar(*vns.priv, vname, typeInfo);
   } 

   DLLLOCAL static void importGlobalVariable(RootQoreNamespace& rns, QoreNamespace& tns, Var* v, bool readonly, ExceptionSink* xsink) {
      return rns.rpriv->importGlobalVariable(*tns.priv, v, readonly, xsink);
   }

   DLLLOCAL static void runtimeModuleRebuildIndexes(RootQoreNamespace& rns) {
      // rebuild root indexes
      QorePrivateNamespaceIterator qpni(rns.priv, true);
      while (qpni.next())
         rns.rpriv->rebuildIndexes(qpni.get());
   }

   DLLLOCAL static QoreClass* runtimeFindClass(RootQoreNamespace& rns, const char* name) {
      return rns.rpriv->runtimeFindClass(name);
   }

   DLLLOCAL static QoreNamespace* runtimeFindNamespaceForAddFunction(RootQoreNamespace& rns, const NamedScope& name, ExceptionSink* xsink) {
      return rns.rpriv->runtimeFindNamespaceForAddFunction(name, xsink);
   }

   DLLLOCAL static QoreNamespace* runtimeFindNamespaceForAddClass(RootQoreNamespace& rns, const NamedScope& name, ExceptionSink* xsink) {
      return rns.rpriv->runtimeFindNamespaceForAddClass(name, xsink);
   }

   DLLLOCAL static qore_root_ns_private* get(RootQoreNamespace& rns) {
      return rns.rpriv;
   }

   DLLLOCAL static const qore_root_ns_private* get(const RootQoreNamespace& rns) {
      return rns.rpriv;
   }

   DLLLOCAL static qore_ns_private* getQore(RootQoreNamespace& rns) {
      return rns.rpriv->qoreNS->priv;
   }

   DLLLOCAL static const qore_ns_private* getQore(const RootQoreNamespace& rns) {
      return rns.rpriv->qoreNS->priv;
   }

   DLLLOCAL static void clearConstants(RootQoreNamespace& ns, QoreListNode& l) {
      ns.priv->clearConstants(l);
      ns.rpriv->cnmap.clear();
   }

   DLLLOCAL static void clearData(RootQoreNamespace& ns, ExceptionSink* xsink) {
      ns.priv->clearData(xsink);
   }
};

#endif
