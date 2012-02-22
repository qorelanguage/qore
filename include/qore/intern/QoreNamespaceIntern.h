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

struct qore_ns_private {
   std::string name;

   QoreClassList classList, pendClassList;
   ConstantList constant, pendConstant;
   QoreNamespaceList nsl, pendNSL;

   const qore_ns_private* parent;
   q_ns_class_handler_t class_handler;   
   QoreNamespace *ns;

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns, const char *n) 
      : name(n),
        parent(0), class_handler(0), ns(n_ns) {
   }

   DLLLOCAL qore_ns_private(const qore_ns_private &old, int64 po) 
      : name(old.name), 
        classList(old.classList, po), 
        constant(old.constant),
        nsl(old.nsl, po, *this),
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

   DLLLOCAL void addClass(const NamedScope *n, QoreClass *oc);
   DLLLOCAL void addClass(QoreClass *oc);

   DLLLOCAL void parseAddConstant(const NamedScope &name, AbstractQoreNode *value);

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
   DLLLOCAL AbstractQoreNode *rootFindScopedConstantValue(const NamedScope *name, unsigned *matched, const QoreTypeInfo *&typeInfo);
   DLLLOCAL RootQoreNamespace* copyRootNamespace(int64 po) {
      // should only be called on the root namespace
      assert(name.empty() && !parent);

      RootQoreNamespace* rv = new RootQoreNamespace(new qore_ns_private(*this, po));
      rv->qoreNS = rv->priv->nsl.find("Qore");
      rv->priv->ns = rv;
      assert(rv->qoreNS);
      return rv;
   }
   DLLLOCAL QoreClass *rootFindClass(const char *name);

   DLLLOCAL void rootAddClass(const NamedScope *name, QoreClass *oc);
   DLLLOCAL void rootAddConstant(const NamedScope &name, AbstractQoreNode *value);
   DLLLOCAL AbstractQoreNode *rootFindConstantValue(const char *name, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreClass *rootFindScopedClassWithMethod(const NamedScope *nscope, unsigned *matched);

   DLLLOCAL AbstractQoreNode *findConstantValue(const NamedScope *name, const QoreTypeInfo *&typeInfo);
   DLLLOCAL AbstractQoreNode *findConstantValue(const char *name, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreClass *parseFindClass(const char *name);
   DLLLOCAL QoreClass *parseFindScopedClass(const NamedScope *name);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethod(const NamedScope *name);

   // returns 0 for success, non-zero for error
   DLLLOCAL int rootResolveBareword(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo);
   // returns 0 for success, non-zero for error (parse exception thrown)
   DLLLOCAL int rootResolveScopedReference(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo);
   // does not throw parse exceptions
   DLLLOCAL AbstractQoreNode *rootResolveScopedReference(const NamedScope &ns, unsigned &m, const QoreTypeInfo *&typeInfo);
   // returns 0 for success, non-zero for error
   DLLLOCAL int rootAddMethodToClass(const NamedScope *name, MethodVariantBase *qcmethod, bool static_flag);

   // returns 0 for success, non-zero for error
   DLLLOCAL static int resolveBareword(AbstractQoreNode **node, const QoreTypeInfo *&typeInfo) {
      return getRootNS()->priv->rootResolveBareword(node, typeInfo);
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

   DLLLOCAL static AbstractQoreNode *rootFindConstantValue(RootQoreNamespace& rns, const char *name, const QoreTypeInfo *&typeInfo) {
      return rns.priv->rootFindConstantValue(name, typeInfo);
   }

   DLLLOCAL static AbstractQoreNode *findConstantValue(RootQoreNamespace& rns, const NamedScope *name, const QoreTypeInfo *&typeInfo) {
      return rns.priv->findConstantValue(name, typeInfo);
   }

   DLLLOCAL static AbstractQoreNode *findConstantValue(RootQoreNamespace& rns, const char *name, const QoreTypeInfo *&typeInfo) {
      return rns.priv->findConstantValue(name, typeInfo);
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

   DLLLOCAL static void rootAddConstant(const RootQoreNamespace& rns, const NamedScope &name, AbstractQoreNode *value) {
      rns.priv->rootAddConstant(name, value);
   }

   DLLLOCAL static QoreClass *rootFindClass(RootQoreNamespace& rns, const char *name) {
      return rns.priv->rootFindClass(name);
   }

   DLLLOCAL static RootQoreNamespace* copyRootNamespace(const RootQoreNamespace& rns, int64 po) {
      return rns.priv->copyRootNamespace(po);
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

   DLLLOCAL static QoreNamespace* newNamespace(const qore_ns_private& old, int64 po) {
      qore_ns_private* p = new qore_ns_private(old, po);
      QoreNamespace* rv = new QoreNamespace(p);
      p->ns = rv;
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

   DLLLOCAL static void parseInit(QoreNamespace& ns) {
      ns.priv->parseInit();
   }

   DLLLOCAL static void parseInitConstants(QoreNamespace& ns) {
      ns.priv->parseInitConstants();
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

#endif
