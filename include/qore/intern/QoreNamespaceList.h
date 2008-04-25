/*
  Namespace.h

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

#ifndef _QORE_NAMESPACELIST_H

#define _QORE_NAMESPACELIST_H

class QoreNamespaceList
{
   private:
      DLLLOCAL void deleteAll();
      
   public:
      class QoreNamespace *head, *tail;

      DLLLOCAL QoreNamespaceList();
      DLLLOCAL ~QoreNamespaceList();
      DLLLOCAL class QoreNamespace *find(const char *name);
      DLLLOCAL void add(class QoreNamespace *ot);
      DLLLOCAL class QoreNamespaceList *copy(int po);
      DLLLOCAL void parseInitConstants();
      DLLLOCAL void parseInit();
      DLLLOCAL void parseCommit(class QoreNamespaceList *n);
      DLLLOCAL void parseRollback();
      DLLLOCAL void reset();
      DLLLOCAL void assimilate(class QoreNamespaceList *n);
      DLLLOCAL class QoreNamespace *parseResolveNamespace(class NamedScope *name, int *matched);
      DLLLOCAL class AbstractQoreNode *parseFindConstantValue(const char *cname);
      DLLLOCAL class AbstractQoreNode *parseFindScopedConstantValue(class NamedScope *name, int *matched);
      DLLLOCAL class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseFindScopedClass(class NamedScope *name, int *matched);
      DLLLOCAL class QoreClass *parseFindClass(const char *ocname);
      DLLLOCAL class QoreClass *parseFindChangeClass(const char *ocname);
};

#endif
