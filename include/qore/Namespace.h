/*
  Namespace.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

  namespaces are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should 
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef QORE_NAMESPACE_H

#define QORE_NAMESPACE_H

#include <string.h>
#include <stdlib.h>

#include <string>

class RootNamespace;

class Namespace
{
      friend class NamespaceList;
      friend class RootNamespace;

  private:
      struct qore_ns_private *priv; // private implementation

      DLLLOCAL class QoreNode *parseMatchScopedConstantValue(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseMatchScopedClass(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched) const;
      DLLLOCAL class Namespace *parseMatchNamespace(class NamedScope *nscope, int *matched) const;
      DLLLOCAL void assimilate(class Namespace *ns);
      DLLLOCAL class Namespace *findNamespace(const char *name) const;
      DLLLOCAL class Namespace *resolveNameScope(class NamedScope *name) const;
      DLLLOCAL class QoreNode *getConstantValue(const char *name) const;
      DLLLOCAL Namespace(const char *n, class QoreClassList *ocl, class ConstantList *cl, class NamespaceList *nnsl);
      DLLLOCAL Namespace(class QoreClassList *ocl, class ConstantList *cl, class NamespaceList *nnsl);

   public:
      DLLEXPORT Namespace(const char *n);
      DLLEXPORT ~Namespace();

      // parse-only interfaces are not exported
      DLLLOCAL Namespace();
      DLLLOCAL void addClass(class NamedScope *n, class QoreClass *oc);
      DLLLOCAL void addConstant(class NamedScope *name, class QoreNode *value);
      DLLLOCAL void addClass(class QoreClass *oc);
      DLLLOCAL void addNamespace(class Namespace *ns);
      DLLLOCAL void parseInit();
      DLLLOCAL void parseInitConstants();
      DLLLOCAL void parseRollback();
      DLLLOCAL void parseCommit();
      DLLLOCAL void setName(const char *nme);

      DLLEXPORT void addConstant(const char *name, class QoreNode *value);
      DLLEXPORT void addSystemClass(class QoreClass *oc);
      DLLEXPORT void addInitialNamespace(class Namespace *ns);
      DLLEXPORT class Namespace *copy(int po = 0) const;
      // info
      DLLEXPORT class QoreHash *getClassInfo() const;
      DLLEXPORT class QoreHash *getConstantInfo() const;
      DLLEXPORT class QoreHash *getInfo() const;
      DLLEXPORT const char *getName() const;
};

class RootNamespace : public Namespace
{
   private:
      class Namespace *qoreNS;

      DLLLOCAL class Namespace *rootResolveNamespace(class NamedScope *nscope);
      DLLLOCAL void addQoreNamespace(class Namespace *qns);
      // private constructor
      DLLLOCAL RootNamespace(class QoreClassList *ocl, class ConstantList *cl, class NamespaceList *nnsl);

   public:
      DLLEXPORT class Namespace *rootGetQoreNamespace() const;

      DLLLOCAL RootNamespace(class Namespace **QoreNS);
      DLLLOCAL ~RootNamespace();
      DLLLOCAL class RootNamespace *copy(int po = 0) const;
      DLLLOCAL class QoreClass *rootFindClass(const char *name) const;
      DLLLOCAL class QoreClass *rootFindChangeClass(const char *name);
      DLLLOCAL class QoreNode *rootFindConstantValue(const char *name) const;
      DLLLOCAL class QoreNode *rootFindScopedConstantValue(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *rootFindScopedClass(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched) const;
      DLLLOCAL void rootAddClass(class NamedScope *name, class QoreClass *oc);
      DLLLOCAL void rootAddConstant(class NamedScope *name, class QoreNode *value);
      DLLLOCAL class QoreNode *findConstantValue(class NamedScope *name, int level) const;
      DLLLOCAL class QoreNode *findConstantValue(const char *name, int level) const;
      DLLLOCAL class QoreClass *parseFindClass(const char *name) const;
      DLLLOCAL class QoreClass *parseFindScopedClass(class NamedScope *name) const;
      DLLLOCAL class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveSimpleConstant(class QoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int parseInitConstantValue(class QoreNode **, int level);
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveScopedConstant(class QoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int addMethodToClass(class NamedScope *name, class QoreMethod *qcmethod, class BCAList *bcal);
};

#endif // QORE_NAMESPACE_H
