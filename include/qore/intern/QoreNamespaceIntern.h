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
#include <qore/intern/UserFunctionList.h>
#include <qore/intern/ImportedFunctionList.h>

#include <map>

class qore_ns_private {
public:
   std::string name;

   QoreClassList classList, pendClassList;
   ConstantList constant, pendConstant;
   QoreNamespaceList nsl, pendNSL;
   UserFunctionList user_func_list;
   ImportedFunctionList imported_func_list;

   // 0 = root namespace, ...
   unsigned depth;

   const qore_ns_private* parent;
   q_ns_class_handler_t class_handler;   
   QoreNamespace *ns;

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns, const char *n) : name(n), depth(0), parent(0), class_handler(0), ns(n_ns) {
   }

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns) : depth(0), parent(0), class_handler(0), ns(n_ns) {
   }

   DLLLOCAL qore_ns_private(const qore_ns_private &old, int64 po) 
      : name(old.name), 
        classList(old.classList, po), 
        constant(old.constant),        
        nsl(old.nsl, po, *this),
        depth(old.depth),
        parent(0), class_handler(old.class_handler), ns(0) {
   }		    

   DLLLOCAL ~qore_ns_private() {
      printd(5, "qore_ns_private::~qore_ns_private() this=%p '%s'\n", this, name.c_str());
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
   DLLLOCAL QoreClass *findLoadClass(QoreNamespace *cns, const char *cname) {
      QoreClass *qc = classList.find(cname);
      if (!qc && class_handler)
	 qc = class_handler(cns, cname);
      return qc;
   }

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
 
   DLLLOCAL void setName(const char *nme) {
      assert(name.empty());
      name = nme;
   }

   DLLLOCAL void assimilate(QoreNamespace* ns);

   DLLLOCAL void addClass(const NamedScope* n, QoreClass* oc);
   DLLLOCAL void addClass(QoreClass *oc);

   DLLLOCAL void parseAddConstant(const NamedScope& name, AbstractQoreNode* value);

   DLLLOCAL int checkImportUserFunction(const char* name, ExceptionSink *xsink) {
      //printd(0, "qore_ns_private::checkImportUserFunction(%s) this: %p\n", name, this);
         
      if (user_func_list.find(name)) {
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "user function '%s' already exists in this namespace", name);
         return -1;
      }

      if (imported_func_list.findNode(name)) {
         xsink->raiseException("FUNCTION-IMPORT-ERROR", "function '%s' has already been imported into this namespace", name);
         return -1;
      }

      return 0;
   }

   DLLLOCAL ImportedFunctionEntry* importUserFunction(QoreProgram* p, UserFunction* u, ExceptionSink* xsink) {
      if (checkImportUserFunction(u->getName(), xsink))
         return 0;

      return imported_func_list.add(p, u);
   }

   DLLLOCAL ImportedFunctionEntry* importUserFunction(QoreProgram* p, UserFunction* u, const char* new_name, ExceptionSink* xsink) {
      if (checkImportUserFunction(new_name, xsink))
         return 0;

      return imported_func_list.add(p, new_name, u);
   }

   DLLLOCAL UserFunction* findUserImportedFunction(const char* name, QoreProgram*& ipgm) {
      UserFunction* u = user_func_list.find(name);
      if (!u)
         u = imported_func_list.find(name, ipgm);

      return u;
   }

   DLLLOCAL const AbstractQoreFunction* parseResolveFunction(const char* fname, QoreProgram*& pgm) {
      QORE_TRACE("qore_ns_private::parseResolveFunction()");

      const AbstractQoreFunction *f;
      if ((f = user_func_list.find(fname))) {
         printd(5, "resolved user function call to %s\n", fname);
         return f;
      }

      if ((f = imported_func_list.find(fname, pgm))) {
         printd(5, "resolved imported function call to %s\n", fname);
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
         UserFunction* ufc;
         if ((ufc = user_func_list.find(fname))) {
            printd(5, "qore_ns_private::parseResolveCallReference() resolved function reference to user function %s (%p)\n", fname, ufc);
            return new LocalUserCallReferenceNode(ufc);
         }
      }
   
      {
         ImportedFunctionEntry* ifn;
         if ((ifn = imported_func_list.findNode(fname))) {
            printd(5, "qore_ns_private::parseResolveCallReference() resolved function reference to imported function %s (pgm=%p, func=%p)\n", fname, ifn->getProgram(), ifn->getFunction());
            return new UserCallReferenceNode(ifn->getFunction(), ifn->getProgram());
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

   DLLLOCAL void findCallFunction(const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      assert(!ufc);
      assert(!bfc);

      ufc = findUserImportedFunction(name, ipgm);
      if (!ufc)
         bfc = builtinFunctions.find(name);
   }

   DLLLOCAL AbstractQoreNode *getConstantValue(const char *name, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreClass *parseFindLocalClass(const char *name);
   DLLLOCAL void parseAddNamespace(QoreNamespace *nns);

   DLLLOCAL void parseInit();
   DLLLOCAL void parseInitConstants();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit();

   DLLLOCAL QoreNamespace *resolveNameScope(const NamedScope *name) const;
   DLLLOCAL QoreNamespace *parseMatchNamespace(const NamedScope *nscope, unsigned *matched);
   DLLLOCAL QoreClass *parseMatchScopedClass(const NamedScope *name, unsigned *matched);
   DLLLOCAL QoreClass *parseMatchScopedClassWithMethod(const NamedScope *nscope, unsigned *matched);
   DLLLOCAL AbstractQoreNode *parseCheckScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo);
   DLLLOCAL AbstractQoreNode *parseResolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo);
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

   // returns 0 for success, non-zero for error (parse exception thrown)
   DLLLOCAL int rootResolveScopedReference(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo);
   // does not throw parse exceptions
   DLLLOCAL AbstractQoreNode *rootResolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo);
   // returns 0 for success, non-zero for error
   DLLLOCAL int rootAddMethodToClass(const NamedScope *name, MethodVariantBase *qcmethod, bool static_flag);

   DLLLOCAL UserFunction* addPendingVariant(char* name, UserFunctionVariant* v, bool& new_func) {
      // check if an imported function already exists with this name
      if (imported_func_list.findNode(name)) {
         parse_error("function '%s' was been imported into this namespace; new variants cannot be added to imported functions", name);
         free(name);
         return 0;
      }

      UserFunction* u = user_func_list.find(name);
      if (!u) {
         u = new UserFunction(name);
         u->parseAddVariant(v);
         user_func_list.add(u);
         new_func = true;
         return u;
      }

      free(name);
      return u->parseAddVariant(v) ? 0 : u;
   }

   // returns 0 for success, non-zero for error (parse exception thrown)
   DLLLOCAL static int resolveScopedReference(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->priv->rootResolveScopedReference(node, typeInfo);
   }

   // does not throw parse exceptions
   DLLLOCAL static AbstractQoreNode *resolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->priv->rootResolveScopedReference(ns, m, typeInfo);
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

   DLLLOCAL static AbstractQoreNode *parseResolveBareword(QoreNamespace *ns, const char *bname, const QoreTypeInfo *&typeInfo) {
      return ns->priv->parseResolveBareword(bname, typeInfo);
   }

   DLLLOCAL static ConstantList& getConstantList(const QoreNamespace *ns) {
      return ns->priv->constant;
   }

   DLLLOCAL static UserFunction* findUserImportedFunction(QoreNamespace& ns, const char *name, QoreProgram*& ipgm) {
      return ns.priv->findUserImportedFunction(name, ipgm);
   }

   DLLLOCAL static void findCallFunction(QoreNamespace& ns, const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      return ns.priv->findCallFunction(name, ufc, ipgm, bfc);
   }

   DLLLOCAL static QoreListNode* getUserFunctionList(QoreNamespace& ns) {
      return ns.priv->user_func_list.getList(); 
   }

   DLLLOCAL static QoreNamespace* newNamespace(const qore_ns_private& old, int64 po) {
      qore_ns_private* p = new qore_ns_private(old, po);
      QoreNamespace* rv = new QoreNamespace(p);
      return rv;
   }

   DLLLOCAL static void addClass(QoreNamespace& ns, const NamedScope *n, QoreClass *oc) {
      ns.priv->addClass(n, oc);
   }

   DLLLOCAL static void addClass(QoreNamespace& ns, QoreClass *oc) {
      ns.priv->addClass(oc);
   }

   DLLLOCAL static void parseAddConstant(QoreNamespace& ns, const NamedScope &name, AbstractQoreNode *value) {
      ns.priv->parseAddConstant(name, value);
   }

   DLLLOCAL static void parseAddNamespace(QoreNamespace& ns, QoreNamespace *nns) {
      ns.priv->parseAddNamespace(nns);
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
public:
   typedef NSOInfo<T> info_t;
   typedef std::map<const char*, NSOInfo<T>, ltstr> map_t;

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

typedef RootMap<UserFunction> ufmap_t;
typedef RootMap<ImportedFunctionEntry> ifmap_t;

class qore_root_ns_private : public qore_ns_private {
protected:
   DLLLOCAL int addPendingVariant(qore_ns_private& ns, char* name, UserFunctionVariant* v) {
      // try to add function variant to given namespace
      bool new_func = false;
      UserFunction* u = ns.addPendingVariant(name, v, new_func);
      if (!u)
         return -1;

      if (new_func)
         pend_ufmap.update(u->getName(), &ns, u);
      else
         ufmap.update(u->getName(), &ns, u);
      return 0;      
   }

   DLLLOCAL UserFunction* runtimeFindUserFunctionIntern(const char* name) {
      return ufmap.findObj(name);
   }

   DLLLOCAL int importUserFunction(qore_ns_private& ns, QoreProgram* p, UserFunction* u, ExceptionSink* xsink) {
      ImportedFunctionEntry* ife = ns.importUserFunction(p, u, xsink);
      if (!ife)
         return -1;

      ifmap.update(ife->getName(), &ns, ife);
      return 0;
   }

   DLLLOCAL int importUserFunction(qore_ns_private& ns, QoreProgram *p, UserFunction *u, const char *new_name, ExceptionSink *xsink) {
      ImportedFunctionEntry* ife = ns.importUserFunction(p, u, new_name, xsink);
      if (!ife)
         return -1;

      ifmap.update(ife->getName(), &ns, ife);
      return 0;      
   }

   DLLLOCAL UserFunction* findImportedFunction(const char* name, QoreProgram*& ipgm) {
      ifmap_t::iterator i = ifmap.find(name);
      if (i == ifmap.end())
         return 0;

      ipgm = i->second.obj->getProgram();
      return i->second.obj->getFunction();
   }

   DLLLOCAL UserFunction* runtimeFindUserImportedFunctionIntern(const char* name, QoreProgram*& ipgm) {
      ufmap_t::iterator ui = ufmap.find(name);
      ifmap_t::iterator ii = ifmap.find(name);

      if (ui != ufmap.end()) {
         if (ii != ifmap.end()) {
            if (ui->second.depth() < ii->second.depth())
               return ui->second.obj;

            return ii->second.obj->getFunction(ipgm);
         }

         return ui->second.obj;
      }

      if (ii != ifmap.end())
         return ii->second.obj->getFunction(ipgm);

      //printd(0, "qore_root_ns_private::runtimeFindUserImportedFunctionIntern() this: %p %s not found ui: %d ii: %d\n", this, name, ui != ufmap.end(), ii != ifmap.end());
      return 0;
   }

   DLLLOCAL UserFunction* parseFindUserImportedFunctionIntern(const char* name, QoreProgram*& ipgm) {
      ufmap_t::iterator ui = ufmap.find(name);
      ufmap_t::iterator uip = pend_ufmap.find(name);
      ifmap_t::iterator ii = ifmap.find(name);

      if (ui != ufmap.end()) {
         if (ii != ifmap.end()) {
            if (uip != pend_ufmap.end()) {
               if (ui->second.depth() < ii->second.depth()) {
                  if (ui->second.depth() < uip->second.depth())
                     return ui->second.obj;

                  return uip->second.obj;
               }

               if (ii->second.depth() < uip->second.depth())
                  return ii->second.obj->getFunction(ipgm);
               return uip->second.obj;
            }
            if (ui->second.depth() < ii->second.depth())
               return ui->second.obj;

            return ii->second.obj->getFunction(ipgm);
         }

         if (uip != pend_ufmap.end()) {
            if (ui->second.depth() < uip->second.depth())
               return ui->second.obj;

            return uip->second.obj;
         }

         return ui->second.obj;
      }

      if (ii != ifmap.end()) {
         if (uip != pend_ufmap.end()) {
            if (ii->second.depth() < uip->second.depth())
               return ii->second.obj->getFunction(ipgm);
            return uip->second.obj;
         }
 
         return ii->second.obj->getFunction(ipgm);
      }

      if (uip != pend_ufmap.end())
         return uip->second.obj;

      /*
      if (!strcmp(name, "t")) {
         printd(0, "qore_root_ns_private::parseFindUserImportedFunctionIntern() this: %p %s ui: %d uip: %d ii:%d\n", this, name, ui != ufmap.end(), uip != pend_ufmap.end(), ii != ifmap.end());
         assert(false);
      }
      */

      return 0;
   }

   DLLLOCAL void findCallFunctionIntern(const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      assert(!ufc);
      assert(!bfc);

      ufc = runtimeFindUserImportedFunctionIntern(name, ipgm);
      if (!ufc)
         bfc = builtinFunctions.find(name);
   }

   DLLLOCAL const AbstractQoreFunction* parseResolveFunctionIntern(const char* fname, QoreProgram*& ipgm) {
      QORE_TRACE("qore_root_ns_private::parseResolveFunctionIntern()");

      const AbstractQoreFunction* f = parseFindUserImportedFunctionIntern(fname, ipgm);
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

      ufmap_t::iterator ui = ufmap.find(fname);
      ifmap_t::iterator ii = ifmap.find(fname);

      if (ui != ufmap.end()) {
         if (ii != ifmap.end()) {
            if (ui->second.depth() < ii->second.depth()) {
               return new LocalUserCallReferenceNode(ui->second.obj);
            }

            ImportedFunctionEntry* ifn = ii->second.obj;
            return new UserCallReferenceNode(ifn->getFunction(), ifn->getProgram());
         }

         return new LocalUserCallReferenceNode(ui->second.obj);
      }

      if (ii != ifmap.end()) {
         ImportedFunctionEntry* ifn = ii->second.obj;
         return new UserCallReferenceNode(ifn->getFunction(), ifn->getProgram());
      }
   
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
      //printd(0, "qore_root_ns_private::parseCommit() this: %p BEFORE newfunc: u: %d pend_u: %d\n", this, ufmap.find("newfunc") != ufmap.end(), pend_ufmap.find("newfunc") != pend_ufmap.end());

      // commit pending lookup entries
      for (ufmap_t::iterator i = pend_ufmap.begin(), e = pend_ufmap.end(); i != e; ++i)
         ufmap.update(i);

      pend_ufmap.clear();

      //printd(0, "qore_root_ns_private::parseCommit() this: %p AFTER  newfunc: u: %d pend_u: %d\n", this, ufmap.find("newfunc") != ufmap.end(), pend_ufmap.find("newfunc") != pend_ufmap.end());

      qore_ns_private::parseCommit();
   }

   DLLLOCAL void parseRollback() {
      // roll back pending lookup entries
      pend_ufmap.clear();

      qore_ns_private::parseRollback();
   }

   AbstractQoreNode* parseFindConstantValueIntern(const char* cname, const QoreTypeInfo *&typeInfo) {
      AbstractQoreNode* rv;

      if (!(rv = qore_ns_private::getConstantValue(cname, typeInfo))
          && (!(rv = nsl.parseFindConstantValue(cname, typeInfo))))
         rv = pendNSL.parseFindConstantValue(cname, typeInfo);
      
      return rv;
   }

   DLLLOCAL AbstractQoreNode *parseFindConstantValueIntern(const NamedScope *name, const QoreTypeInfo *&typeInfo);

   // returns 0 for success, non-zero for error
   DLLLOCAL int parseResolveBarewordIntern(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo);

   DLLLOCAL void parseAddConstantIntern(const NamedScope &name, AbstractQoreNode *value);

public:
   RootQoreNamespace* rns;
   QoreNamespace* qoreNS;

   ufmap_t ufmap,  // root function map
      pend_ufmap;  // pending lookup map (used only during parsing)
   ifmap_t ifmap;  // root imported function map

   DLLLOCAL qore_root_ns_private(RootQoreNamespace* n_rns) : qore_ns_private(n_rns), rns(n_rns), qoreNS(0) {
   }

   DLLLOCAL qore_root_ns_private(const qore_root_ns_private& old, int64 po) : qore_ns_private(old, po) {
      qoreNS = nsl.find("Qore");
      assert(qoreNS);
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

   DLLLOCAL static UserFunction* runtimeFindUserFunction(RootQoreNamespace& rns, const char* name) {
      return rns.rpriv->runtimeFindUserFunctionIntern(name);
   }

   DLLLOCAL static UserFunction* runtimeFindUserImportedFunction(RootQoreNamespace& rns, const char *name, QoreProgram*& ipgm) {
      return rns.rpriv->runtimeFindUserImportedFunctionIntern(name, ipgm);
   }

   DLLLOCAL static void findCallFunction(RootQoreNamespace& rns, const char* name, UserFunction*& ufc, QoreProgram*& ipgm, const BuiltinFunction*& bfc) {
      return rns.rpriv->findCallFunctionIntern(name, ufc, ipgm, bfc);
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

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const char *name, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo);
   }

   DLLLOCAL static AbstractQoreNode *parseFindConstantValue(const NamedScope *name, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->rpriv->parseFindConstantValueIntern(name, typeInfo);
   }
   // returns 0 for success, non-zero for error
   DLLLOCAL static int parseResolveBareword(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->rpriv->parseResolveBarewordIntern(node, typeInfo);
   }

   DLLLOCAL static void parseAddConstant(const NamedScope &name, AbstractQoreNode *value) {
      getRootNS()->rpriv->parseAddConstantIntern(name, value);
   }
};

#endif
