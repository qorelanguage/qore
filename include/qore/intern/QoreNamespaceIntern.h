/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespaceIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2012 David Nichols

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

#ifndef _QORE_QORENAMESPACEINTERN_H
#define _QORE_QORENAMESPACEINTERN_H

#include <qore/intern/QoreClassList.h>
#include <qore/intern/QoreNamespaceList.h>
#include <qore/intern/ConstantList.h>
#include <qore/intern/FunctionList.h>

#include <map>
#include <vector>

class qore_ns_private {
private:
   // not implemented
   DLLLOCAL qore_ns_private(const qore_ns_private&);
   // not implemented
   DLLLOCAL qore_ns_private& operator=(const qore_ns_private&);

public:
   std::string name;

   QoreClassList classList, pendClassList;
   ConstantList constant, pendConstant;
   QoreNamespaceList nsl, pendNSL;
   FunctionList func_list;

   // 0 = root namespace, ...
   unsigned depth;

   bool root;

   const qore_ns_private* parent;       // pointer to parent namespace (0 if this is the root namespace or an unattached namespace)
   q_ns_class_handler_t class_handler;   
   QoreNamespace *ns;

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns, const char *n) : name(n), depth(0), root(false), parent(0), class_handler(0), ns(n_ns) {
   }

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns) : depth(0), root(false), parent(0), class_handler(0), ns(n_ns) {      
   }

   DLLLOCAL qore_ns_private(const qore_ns_private &old, int64 po) 
      : name(old.name), 
        classList(old.classList, po), 
        constant(old.constant),        
        nsl(old.nsl, po, *this),
        depth(old.depth),
        root(old.root),
        parent(0), class_handler(old.class_handler), ns(0) {
   }

   DLLLOCAL ~qore_ns_private() {
      printd(5, "qore_ns_private::~qore_ns_private() this=%p '%s'\n", this, name.c_str());
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

   // finds a local class in the committed class list, if not found executes the class handler
   DLLLOCAL QoreClass *findLoadClass(const char *cname) {
      QoreClass *qc = classList.find(cname);
      if (!qc && class_handler)
	 qc = class_handler(ns, cname);
      return qc;
   }

/*
   DLLLOCAL AbstractQoreNode *parseResolveBareword(const char *bname, const QoreTypeInfo *&typeInfo) {
      AbstractQoreNode *rv = constant.find(bname, typeInfo);
      if (rv)
         return rv->refSelf();

      rv = pendConstant.find(bname, typeInfo);
      if (rv)
         return rv->refSelf();

      rv = classList.parseResolveBareword(bname, typeInfo);
      if (!rv) {
         rv = pendClassList.parseResolveBareword(bname, typeInfo);
         if (!rv) {
            rv = nsl.parseResolveBareword(bname, typeInfo);
            if (!rv)
               rv = pendNSL.parseResolveBareword(bname, typeInfo);
         }
      }
      return rv;
   }
*/
 
   DLLLOCAL void setName(const char *nme) {
      assert(name.empty());
      name = nme;
   }

   DLLLOCAL void assimilate(QoreNamespace* ns);

   DLLLOCAL void updateDepthRecursive(unsigned ndepth);

   DLLLOCAL void addClass(const NamedScope* n, QoreClass* oc);
   DLLLOCAL void addClass(QoreClass *oc);

   DLLLOCAL clmap_t::iterator parseAddConstant(const char* name, AbstractQoreNode* value);

   DLLLOCAL void parseAddConstant(const NamedScope& name, AbstractQoreNode* value);

   DLLLOCAL int checkImportUserFunction(const char* name, ExceptionSink *xsink) {
      //printd(0, "qore_ns_private::checkImportUserFunction(%s) this: %p\n", name, this);

      if (func_list.findNode(name)) {
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' already exists in this namespace", name);
         return -1;
      }

      return 0;
   }

   DLLLOCAL FunctionEntry* importUserFunction(QoreProgram* p, UserFunction* u, ExceptionSink* xsink) {
      if (checkImportUserFunction(u->getName(), xsink))
         return 0;

      return func_list.add(p, u);
   }

   DLLLOCAL FunctionEntry* importUserFunction(QoreProgram* p, UserFunction* u, const char* new_name, ExceptionSink* xsink) {
      if (checkImportUserFunction(new_name, xsink))
         return 0;

      return func_list.add(p, new_name, u);
   }

   DLLLOCAL UserFunction* runtimeFindFunction(const char* name, QoreProgram*& ipgm) {
      return func_list.find(name, ipgm, true);
   }

   DLLLOCAL const AbstractQoreFunction* parseResolveFunction(const char* fname, QoreProgram*& pgm) {
      QORE_TRACE("qore_ns_private::parseResolveFunction()");

      const AbstractQoreFunction *f;
      if ((f = func_list.find(fname, pgm, false))) {
         printd(5, "resolved function call to %s\n", fname);
         return f;
      }

      if ((f = builtinFunctions.find(fname))) {
         printd(5, "resolved builtin function call to %s\n", fname);
         return f;
      }

      // cannot find function, throw exception
      parse_error("function '%s()' cannot be found", fname);

      return 0;
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL AbstractCallReferenceNode* parseResolveCallReference(UnresolvedProgramCallReferenceNode* fr) {
      std::auto_ptr<UnresolvedProgramCallReferenceNode> fr_holder(fr);
      char* fname = fr->str;

      {
         FunctionEntry* fn;
         if ((fn = func_list.findNode(fname))) {
            printd(5, "qore_ns_private::parseResolveCallReference() resolved function reference to function %s (pgm=%p, func=%p)\n", fname, fn->getProgram(), fn->getFunction());
            return fn->makeCallReference();
         }
      }
   
      const BuiltinFunction* bfc;
      if ((bfc = builtinFunctions.find(fname))) {
         printd(5, "qore_ns_private::parseResolveCallReference() resolved function reference to builtin function to %s\n", fname);
      
         // check parse options to see if access is allowed
         if (bfc->getUniqueFunctionality() & getProgram()->getParseOptions64())
            parse_error("parse options do not allow access to builtin function '%s'", fname);
         else 
            return new BuiltinCallReferenceNode(bfc);
      }
      else
         // cannot find function, throw exception
         parse_error("reference to function '%s()' cannot be resolved", fname);

      return fr_holder.release();
   }

   DLLLOCAL void runtimeFindCallFunction(const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      assert(!ufc);
      assert(!bfc);

      ufc = runtimeFindFunction(name, ipgm);
      if (!ufc)
         bfc = builtinFunctions.find(name);
   }

   DLLLOCAL QoreNamespace* findCreateNamespacePath(const char* nspath);

   DLLLOCAL AbstractQoreNode *getConstantValue(const char *name, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreClass *parseFindLocalClass(const char *name);
   DLLLOCAL qore_ns_private* parseAddNamespace(QoreNamespace *nns);

   DLLLOCAL void addNamespace(qore_ns_private* nns);

   DLLLOCAL void parseInit();
   DLLLOCAL void parseInitConstants();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();

   DLLLOCAL QoreNamespace *resolveNameScope(const NamedScope *name) const;
   DLLLOCAL QoreNamespace *parseMatchNamespace(const NamedScope *nscope, unsigned *matched);
   DLLLOCAL QoreClass *parseMatchScopedClass(const NamedScope *name, unsigned *matched);
   DLLLOCAL QoreClass *parseMatchScopedClassWithMethod(const NamedScope *nscope, unsigned *matched);
   DLLLOCAL AbstractQoreNode *parseCheckScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo);

   DLLLOCAL AbstractQoreNode *parseFindLocalConstantValue(const char *cname, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreNamespace *parseFindLocalNamespace(const char *nname);

   DLLLOCAL AbstractQoreNode *parseMatchScopedConstantValue(const NamedScope *name, unsigned *matched, const QoreTypeInfo *&typeInfo);

   DLLLOCAL QoreNamespace *rootResolveNamespace(const NamedScope *nscope);
   DLLLOCAL QoreClass *rootFindScopedClass(const NamedScope *name, unsigned *matched);
   DLLLOCAL QoreClass *rootFindClass(const char *name);

   DLLLOCAL void rootAddClass(const NamedScope *name, QoreClass *oc);
   DLLLOCAL QoreClass *rootFindScopedClassWithMethod(const NamedScope *nscope, unsigned *matched);

   DLLLOCAL QoreClass *parseFindClass(const char *name);
   DLLLOCAL QoreClass *parseFindScopedClass(const NamedScope *name);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethod(const NamedScope *name);

   DLLLOCAL int rootAddMethodToClass(const NamedScope *name, MethodVariantBase *qcmethod, bool static_flag);

   DLLLOCAL FunctionEntry* addPendingVariant(char* name, UserFunctionVariant* v, bool& new_func) {
      FunctionEntry* fe = func_list.findNode(name);

      if (!fe) {
         UserFunction* u = new UserFunction(name);
         u->parseAddVariant(v);
         fe = func_list.add(u);
         new_func = true;
         return fe;
      }

      // see if the function was imported; cannot add variants to an imported function
      if (fe->getProgram()) {
         parse_error("function '%s' was been imported into this namespace; new variants cannot be added to imported functions", name);
         free(name);
         return 0;
      }

      free(name);
      return fe->getFunction()->parseAddVariant(v) ? 0 : fe;
   }

   // returns 0 for success, non-zero for error
   DLLLOCAL static int addMethodToClass(const NamedScope *name, MethodVariantBase *qcmethod, bool static_flag) {
      return getRootNS()->priv->rootAddMethodToClass(name, qcmethod, static_flag);
   }

   DLLLOCAL static QoreClass *rootFindScopedClassWithMethod(RootQoreNamespace& rns, const NamedScope *nscope, unsigned *matched) {
      return rns.priv->rootFindScopedClassWithMethod(nscope, matched);
   }

   DLLLOCAL static QoreClass *parseFindClass(RootQoreNamespace& rns, const char *name) {
      return rns.priv->parseFindClass(name);
   }

   DLLLOCAL static QoreClass *parseFindScopedClass(RootQoreNamespace& rns, const NamedScope *name) {
      return rns.priv->parseFindScopedClass(name);
   }

   DLLLOCAL static QoreClass *parseFindScopedClassWithMethod(RootQoreNamespace& rns, const NamedScope *name) {
      return rns.priv->parseFindScopedClassWithMethod(name);
   }

   DLLLOCAL static void rootAddClass(const RootQoreNamespace& rns, const NamedScope *name, QoreClass *oc) {
      rns.priv->rootAddClass(name, oc);
   }

   DLLLOCAL static QoreClass *rootFindClass(RootQoreNamespace& rns, const char *name) {
      return rns.priv->rootFindClass(name);
   }

   DLLLOCAL static const AbstractQoreFunction* parseResolveFunction(QoreNamespace& ns, const char* fname, QoreProgram*& pgm) {
      return ns.priv->parseResolveFunction(fname, pgm);
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL static AbstractCallReferenceNode* parseResolveCallReference(QoreNamespace& ns, UnresolvedProgramCallReferenceNode* fr) {
      return ns.priv->parseResolveCallReference(fr);
   }

   DLLLOCAL static void setName(const QoreNamespace& ns, const char *nme) {
      ns.priv->setName(nme);
   }

/*
   DLLLOCAL static AbstractQoreNode *parseResolveBareword(QoreNamespace *ns, const char *bname, const QoreTypeInfo *&typeInfo) {
      return ns->priv->parseResolveBareword(bname, typeInfo);
   }
*/

   DLLLOCAL static ConstantList& getConstantList(const QoreNamespace *ns) {
      return ns->priv->constant;
   }

   DLLLOCAL static UserFunction* runtimeFindFunction(QoreNamespace& ns, const char *name, QoreProgram*& ipgm) {
      return ns.priv->runtimeFindFunction(name, ipgm);
   }

   DLLLOCAL static void runtimeFindCallFunction(QoreNamespace& ns, const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      return ns.priv->runtimeFindCallFunction(name, ufc, ipgm, bfc);
   }

   DLLLOCAL static QoreListNode* getUserFunctionList(QoreNamespace& ns) {
      return ns.priv->func_list.getList(); 
   }

   DLLLOCAL static void addClass(QoreNamespace& ns, const NamedScope *n, QoreClass *oc) {
      ns.priv->addClass(n, oc);
   }

   DLLLOCAL static void addClass(QoreNamespace& ns, QoreClass *oc) {
      ns.priv->addClass(oc);
   }

   DLLLOCAL static void parseAddNamespace(QoreNamespace& ns, QoreNamespace *nns) {
      ns.priv->parseAddNamespace(nns);
   }

   DLLLOCAL static void parseAddConstant(QoreNamespace& ns, const NamedScope& name, AbstractQoreNode* value) {
      ns.priv->parseAddConstant(name, value);
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
};

struct namespace_iterator_element {
   qore_ns_private* ns;
   nsmap_t::iterator i;

   DLLLOCAL namespace_iterator_element(qore_ns_private* n_ns) : ns(n_ns), i(ns->nsl.nsmap.begin()) {
   }

   DLLLOCAL bool atEnd() const {
      return i == ns->nsl.nsmap.end();
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

   DLLLOCAL void set(qore_ns_private* rns) {
      nsv.push_back(rns);
      while (!rns->nsl.empty()) {
         rns = qore_ns_private::get(*(rns->nsl.nsmap.begin()->second));
         nsv.push_back(rns);
      }
   }

public:
   DLLLOCAL QorePrivateNamespaceIterator(qore_ns_private* rns) : root(rns) {
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
         //printd(0, "RootMap::update(iterator) inserting '%s' new depth: %d\n", ni->first, ni->second.depth());
         this->insert(typename map_t::value_type(ni->first, ni->second));
      }
      else {
         // if the old depth is > the new depth, then replace
         if (i->second.depth() > ni->second.depth()) {
            //printd(0, "RootMap::update(iterator) replacing '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
            i->second = ni->second;      
         }
         //else
         //printd(0, "RootMap::update(iterator) ignoring '%s' current depth: %d new depth: %d\n", ni->first, i->second.depth(), ni->second.depth());
      }
   }

   T* findObj(const char* name) {
      typename map_t::iterator i = this->find(name);
      return i == this->end() ? 0 : i->second.obj;
   }
};

typedef RootMap<FunctionEntry> fmap_t;

typedef RootMap<ConstantEntry> cmap_t;

class qore_root_ns_private : public qore_ns_private {
   friend class qore_ns_private;

protected:
   DLLLOCAL int addPendingVariant(qore_ns_private& ns, char* name, UserFunctionVariant* v) {
      // try to add function variant to given namespace
      bool new_func = false;
      FunctionEntry* fe = ns.addPendingVariant(name, v, new_func);
      if (!fe)
         return -1;

      if (new_func) {
         fmap_t::iterator i = fmap.find(fe->getName());
         // only add to pending map if either not in the committed map or the depth is higher in the committed map
         if (i == fmap.end() || i->second.depth() > ns.depth)
            pend_fmap.update(fe->getName(), &ns, fe);
      }

      return 0;      
   }

   // performed at runtime
   DLLLOCAL int importUserFunction(qore_ns_private& ns, QoreProgram* p, UserFunction* u, ExceptionSink* xsink) {
      FunctionEntry* fe = ns.importUserFunction(p, u, xsink);
      if (!fe)
         return -1;

      fmap.update(fe->getName(), &ns, fe);
      return 0;
   }

   // performed at runtime
   DLLLOCAL int importUserFunction(qore_ns_private& ns, QoreProgram *p, UserFunction *u, const char *new_name, ExceptionSink *xsink) {
      FunctionEntry* fe = ns.importUserFunction(p, u, new_name, xsink);
      if (!fe)
         return -1;

      fmap.update(fe->getName(), &ns, fe);
      return 0;      
   }

   DLLLOCAL bool runtimeExistsFunctionIntern(const char* name) {
      return fmap.find(name) != fmap.end();
   }

   DLLLOCAL UserFunction* runtimeFindFunctionIntern(const char* name, QoreProgram*& ipgm) {
      fmap_t::iterator i = fmap.find(name);

      if (i != fmap.end())
         return i->second.obj->getFunction(ipgm);

      //printd(0, "qore_root_ns_private::runtimeFindFunctionIntern() this: %p %s not found i: %d\n", this, name, i != fmap.end());
      return 0;
   }

   DLLLOCAL UserFunction* parseFindFunctionIntern(const char* name, QoreProgram*& ipgm) {
      fmap_t::iterator i = fmap.find(name);
      fmap_t::iterator ip = pend_fmap.find(name);

      if (i != fmap.end()) {
         if (ip != pend_fmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj->getFunction(ipgm);

            return ip->second.obj->getFunction(ipgm);
         }
 
         return i->second.obj->getFunction(ipgm);
      }

      if (ip != pend_fmap.end())
         return ip->second.obj->getFunction(ipgm);

      return 0;
   }

   DLLLOCAL void runtimeFindCallFunctionIntern(const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      assert(!ufc);
      assert(!bfc);

      ufc = runtimeFindFunctionIntern(name, ipgm);
      if (!ufc)
         bfc = builtinFunctions.find(name);
   }

   DLLLOCAL const AbstractQoreFunction* parseResolveFunctionIntern(const char* fname, QoreProgram*& ipgm) {
      QORE_TRACE("qore_root_ns_private::parseResolveFunctionIntern()");

      const AbstractQoreFunction* f = parseFindFunctionIntern(fname, ipgm);
      if (!f) {
         f = builtinFunctions.find(fname);

         // cannot find function, throw exception
         if (!f)
            parse_error("function '%s()' cannot be found", fname);
      }

      return f;
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL AbstractCallReferenceNode* parseResolveCallReferenceIntern(UnresolvedProgramCallReferenceNode* fr) {
      std::auto_ptr<UnresolvedProgramCallReferenceNode> fr_holder(fr);
      char* fname = fr->str;

      fmap_t::iterator i = fmap.find(fname);
      fmap_t::iterator ip = pend_fmap.find(fname);

      if (i != fmap.end()) {
         if (ip != pend_fmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj->makeCallReference();

            return ip->second.obj->makeCallReference();
         }

         return i->second.obj->makeCallReference();
      }

      if (ip != pend_fmap.end())
         return ip->second.obj->makeCallReference();
   
      const BuiltinFunction* bfc;
      if ((bfc = builtinFunctions.find(fname))) {
         printd(5, "qore_root_ns_private::parseResolveCallReference() resolved function reference to builtin function to %s\n", fname);
      
         // check parse options to see if access is allowed
         if (bfc->getUniqueFunctionality() & getProgram()->getParseOptions64())
            parse_error("parse options do not allow access to builtin function '%s'", fname);
         else 
            return new BuiltinCallReferenceNode(bfc);
      }
      else
         // cannot find function, throw exception
         parse_error("reference to function '%s()' cannot be resolved", fname);

      return fr_holder.release();
   }

   DLLLOCAL void parseCommit() {
      // commit pending function lookup entries
      for (fmap_t::iterator i = pend_fmap.begin(), e = pend_fmap.end(); i != e; ++i)
         fmap.update(i);
      pend_fmap.clear();

      // commit pending constant lookup entries
      for (cmap_t::iterator i = pend_cmap.begin(), e = pend_cmap.end(); i != e; ++i)
         cmap.update(i);
      pend_cmap.clear();
      
      qore_ns_private::parseCommit();
   }

   DLLLOCAL void parseRollback() {
      // roll back pending lookup entries
      pend_fmap.clear();
      pend_cmap.clear();

      qore_ns_private::parseRollback();
   }

   AbstractQoreNode* parseFindOnlyConstantValueIntern(const char* cname, const QoreTypeInfo*& typeInfo) {
      // look up in global constant map
      cmap_t::iterator i = cmap.find(cname);
      cmap_t::iterator ip = pend_cmap.find(cname);

      if (i != cmap.end()) {
         if (ip != pend_cmap.end()) {
            if (i->second.depth() < ip->second.depth())
               return i->second.obj->get(typeInfo);
            return ip->second.obj->get(typeInfo);
         }

         return i->second.obj->get(typeInfo);
      }

      if (ip != pend_cmap.end())
         return ip->second.obj->get(typeInfo);

      return 0;
   }

   AbstractQoreNode* parseFindConstantValueIntern(const char* cname, const QoreTypeInfo*& typeInfo, bool error) {
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

   DLLLOCAL ResolvedCallReferenceNode* runtimeGetCallReference(const char *name, ExceptionSink* xsink) {
      fmap_t::iterator i = fmap.find(name);
      if (i == fmap.end()) {
         xsink->raiseException("NO-SUCH-FUNCTION", "callback function '%s()' does not exist", name);
         return 0;
      }

      return i->second.obj->makeCallReference();
   }

   DLLLOCAL AbstractQoreNode *parseFindConstantValueIntern(const NamedScope *name, const QoreTypeInfo*& typeInfo, bool error);

   // returns 0 for success, non-zero for error
   DLLLOCAL AbstractQoreNode* parseResolveBarewordIntern(const char* bword, const QoreTypeInfo*& typeInfo);

   DLLLOCAL void parseAddConstantIntern(QoreNamespace& ns, const NamedScope& name, AbstractQoreNode* value);

   DLLLOCAL void rebuildIndexes(qore_ns_private* ns) {
      // process function indexes
      for (fl_map_t::iterator i = ns->func_list.begin(), e = ns->func_list.end(); i != e; ++i)
         fmap.update(i->first, ns, i->second);

      // process constant indexes
      ConstantListIterator cli(ns->constant);
      while (cli.next()) {
         cmap.update(cli.getName().c_str(), ns, cli.getEntry());
      }
   }

   DLLLOCAL void parseRebuildIndexes(qore_ns_private* ns) {
      //printd(5, "qore_root_ns_private::parseRebuildIndexes() this: %p ns: %p (%s) depth %d\n", this, ns, ns->name.c_str(), ns->depth);
      
      // process function indexes
      for (fl_map_t::iterator i = ns->func_list.begin(), e = ns->func_list.end(); i != e; ++i)
         pend_fmap.update(i->first, ns, i->second);

      // process pending constant indexes
      {
         ConstantListIterator cli(ns->pendConstant);
         while (cli.next()) {
            //printd(5, "qore_root_ns_private::parseRebuildIndexes() this: %p processing constant %s depth %d\n", this, cli.getName().c_str(), ns->depth);
            pend_cmap.update(cli.getName().c_str(), ns, cli.getEntry());
         }
      }

      // process constant indexes
      {
         ConstantListIterator cli(ns->constant);
         while (cli.next()) {
            //printd(5, "qore_root_ns_private::parseRebuildIndexes() this: %p processing constant %s depth %d\n", this, cli.getName().c_str(), ns->depth);
            cmap.update(cli.getName().c_str(), ns, cli.getEntry());
         }
      }
   }

   DLLLOCAL void parseAddNamespaceIntern(QoreNamespace *nns) {
      qore_ns_private *ns = qore_ns_private::parseAddNamespace(nns);

      //printd(5, "qore_root_ns_private::parseAddNamespaceIntern() this: %p ns: %p\n", this, ns);

      // add all objects to the new (or assimilated) namespace
      if (ns) {
         QorePrivateNamespaceIterator qpni(ns);
         while (qpni.next())
            parseRebuildIndexes(qpni.get());
      }
   }

public:
   RootQoreNamespace* rns;
   QoreNamespace* qoreNS;

   fmap_t fmap,    // root function map
      pend_fmap;   // root pending function map (only used during parsing)

   cmap_t cmap,    // root constant map
      pend_cmap;   // root pending constant map (used only during parsing)
   
   DLLLOCAL qore_root_ns_private(RootQoreNamespace* n_rns) : qore_ns_private(n_rns), rns(n_rns), qoreNS(0) {
      root = true;
   }

   DLLLOCAL qore_root_ns_private(const qore_root_ns_private& old, int64 po) : qore_ns_private(old, po) {
      qoreNS = nsl.find("Qore");
      assert(qoreNS);

      // rebuild root indexes
      QorePrivateNamespaceIterator qpni(this);
      while (qpni.next())
         rebuildIndexes(qpni.get());
   }

   DLLLOCAL ~qore_root_ns_private() {
   }

   DLLLOCAL RootQoreNamespace* copy(int64 po) {
      qore_root_ns_private* p = new qore_root_ns_private(*this, po);
      RootQoreNamespace* rv = new RootQoreNamespace(p);
      return rv;
   }

   DLLLOCAL static RootQoreNamespace* copy(const RootQoreNamespace& rns, int64 po) {
      return rns.rpriv->copy(po);
   }

   DLLLOCAL static int addPendingVariant(QoreNamespace& ns, char *name, UserFunctionVariant *v) {
      return getRootNS()->rpriv->addPendingVariant(*ns.priv, name, v);
   }

   DLLLOCAL static int importUserFunction(RootQoreNamespace& rns, QoreNamespace& ns, QoreProgram* p, UserFunction* u, ExceptionSink* xsink) {
      return rns.rpriv->importUserFunction(*ns.priv, p, u, xsink);
   }

   DLLLOCAL static int importUserFunction(RootQoreNamespace& rns, QoreNamespace& ns, QoreProgram *p, UserFunction *u, const char *new_name, ExceptionSink *xsink) {
      return rns.rpriv->importUserFunction(*ns.priv, p, u, new_name, xsink);
   }

   DLLLOCAL static UserFunction* runtimeFindFunction(RootQoreNamespace& rns, const char *name, QoreProgram*& ipgm) {
      return rns.rpriv->runtimeFindFunctionIntern(name, ipgm);
   }

   DLLLOCAL static bool runtimeExistsFunction(RootQoreNamespace& rns, const char *name) {
      return rns.rpriv->runtimeExistsFunctionIntern(name);
   }

   DLLLOCAL static void runtimeFindCallFunction(RootQoreNamespace& rns, const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      return rns.rpriv->runtimeFindCallFunctionIntern(name, ufc, ipgm, bfc);
   }

   DLLLOCAL static const AbstractQoreFunction* parseResolveFunction(const char* fname, QoreProgram*& pgm) {
      return getRootNS()->rpriv->parseResolveFunctionIntern(fname, pgm);
   }

   // called during parsing (plock already grabbed)
   DLLLOCAL static AbstractCallReferenceNode* parseResolveCallReference(UnresolvedProgramCallReferenceNode* fr) {
      return getRootNS()->rpriv->parseResolveCallReferenceIntern(fr);
   }

   DLLLOCAL static void parseInit() {
      qore_ns_private* p = getRootNS()->priv;
      p->parseInitConstants();
      p->parseInit();
   }

   DLLLOCAL static void parseCommit(RootQoreNamespace& rns) {
      rns.rpriv->parseCommit();
   }

   DLLLOCAL static void parseRollback(RootQoreNamespace& rns) {
      rns.rpriv->parseRollback();
   }

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const char *name, const QoreTypeInfo *&typeInfo, bool error) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo, error);
   }

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const NamedScope *name, const QoreTypeInfo *&typeInfo, bool error) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo, error);
   }
   // returns 0 for success, non-zero for error
   DLLLOCAL static AbstractQoreNode* parseResolveBareword(const char* bword, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->rpriv->parseResolveBarewordIntern(bword, typeInfo);
   }

   DLLLOCAL static void parseAddConstant(QoreNamespace& ns, const NamedScope &name, AbstractQoreNode *value) {
      getRootNS()->rpriv->parseAddConstantIntern(ns, name, value);
   }

   DLLLOCAL static void parseAddNamespace(QoreNamespace *nns) {
      getRootNS()->rpriv->parseAddNamespaceIntern(nns);
   }

   DLLLOCAL static ResolvedCallReferenceNode* runtimeGetCallReference(RootQoreNamespace& rns, const char *name, ExceptionSink* xsink) {
      return rns.rpriv->runtimeGetCallReference(name, xsink);
   }

   DLLLOCAL static void runtimeModuleRebuildIndexes(RootQoreNamespace& rns) {
      // rebuild root indexes
      QorePrivateNamespaceIterator qpni(rns.priv);
      while (qpni.next())
         rns.rpriv->rebuildIndexes(qpni.get());
   }
};

#endif
