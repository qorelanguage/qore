/*
  Namespace.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

class NamespaceList;
class RootNamespace;

class Namespace
{
      friend class NamespaceList;
      friend class RootNamespace;

  private:
      char *name;

      class ConstantList   *constant;
      class QoreClassList  *classList;
      class NamespaceList  *nsl;
      class Namespace      *next;

      // pending lists
      class ConstantList   *pendConstant;
      class QoreClassList  *pendClassList;
      class NamespaceList  *pendNSL;

      DLLLOCAL class QoreNode *parseMatchScopedConstantValue(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseMatchScopedClass(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched) const;
      DLLLOCAL class Namespace *parseMatchNamespace(class NamedScope *nscope, int *matched) const;
      DLLLOCAL inline void init();
      DLLLOCAL void assimilate(class Namespace *ns);
      DLLLOCAL inline class Namespace *findNamespace(char *name) const;
      DLLLOCAL inline class Namespace *resolveNameScope(class NamedScope *name) const;
      DLLLOCAL inline class QoreNode *getConstantValue(char *name) const;
      DLLLOCAL Namespace(char *n, QoreClassList *ocl, ConstantList *cl, NamespaceList *nnsl);

   public:
      DLLEXPORT Namespace(char *n);
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
      DLLLOCAL void setName(char *nme);

      DLLEXPORT void addConstant(char *name, class QoreNode *value);
      DLLEXPORT void addSystemClass(class QoreClass *oc);
      DLLEXPORT void addInitialNamespace(class Namespace *ns);
      DLLEXPORT class Namespace *copy(int po = 0) const;
      // info
      DLLEXPORT class Hash *getClassInfo() const;
      DLLEXPORT class Hash *getConstantInfo() const;
      DLLEXPORT class Hash *getInfo() const;
      DLLEXPORT char *getName() const
      {
	 return name;
      }
};

class RootNamespace : public Namespace
{
   private:
      class Namespace *qoreNS;

      DLLLOCAL inline class Namespace *rootResolveNamespace(class NamedScope *nscope);
      DLLLOCAL inline void addQoreNamespace(class Namespace *qns);
      // private constructor
      DLLLOCAL RootNamespace(QoreClassList *ocl, ConstantList *cl, NamespaceList *nnsl);

   public:
      DLLLOCAL RootNamespace(class Namespace **QoreNS);
      DLLLOCAL ~RootNamespace();

      DLLLOCAL class RootNamespace *copy(int po = 0) const;
      DLLLOCAL class QoreClass *rootFindClass(char *name) const;
      DLLLOCAL class QoreClass *rootFindChangeClass(char *name);
      DLLLOCAL class QoreNode *rootFindConstantValue(char *name) const;
      DLLLOCAL class QoreNode *rootFindScopedConstantValue(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *rootFindScopedClass(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched) const;
      DLLLOCAL void rootAddClass(class NamedScope *name, class QoreClass *oc);
      DLLLOCAL void rootAddConstant(class NamedScope *name, class QoreNode *value);
      DLLLOCAL class QoreNode *findConstantValue(class NamedScope *name, int level) const;
      DLLLOCAL class QoreNode *findConstantValue(char *name, int level) const;
      DLLLOCAL class QoreClass *parseFindClass(char *name) const;
      DLLLOCAL class QoreClass *parseFindScopedClass(class NamedScope *name) const;
      DLLLOCAL class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveSimpleConstant(class QoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int parseInitConstantValue(class QoreNode **, int level);
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveScopedConstant(class QoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int addMethodToClass(class NamedScope *name, class Method *qcmethod);
      DLLEXPORT inline class Namespace *rootGetQoreNamespace() const
      {
	 return qoreNS;
      }
};

#endif // QORE_NAMESPACE_H
