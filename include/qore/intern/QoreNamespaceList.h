/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespaceList.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

#include <map>

typedef std::map<std::string, QoreNamespace*> nsmap_t;

class qore_ns_private;

class QoreNamespaceList {
private:
   DLLLOCAL void deleteAll();

   // not implemented
   DLLLOCAL QoreNamespaceList(const QoreNamespaceList& old);
   // not implemented
   DLLLOCAL QoreNamespaceList& operator=(const QoreNamespaceList& nsl);

public:
   nsmap_t nsmap;

   DLLLOCAL QoreNamespaceList() {
   }

   DLLLOCAL QoreNamespaceList(const QoreNamespaceList& old, int64 po, const qore_ns_private& parent);

   DLLLOCAL ~QoreNamespaceList() {
      deleteAll();
   }

   DLLLOCAL QoreNamespace *find(const char *name) {
      nsmap_t::iterator i = nsmap.find(name);
      return i == nsmap.end() ? 0 : i->second;
   }
   DLLLOCAL QoreNamespace *find(const std::string &name) {
      nsmap_t::iterator i = nsmap.find(name);
      return i == nsmap.end() ? 0 : i->second;
   }

   // do not delete the pointer returned from this function
   DLLLOCAL qore_ns_private* add(QoreNamespace *ot, qore_ns_private* parent);

   DLLLOCAL void resolveCopy();
   DLLLOCAL void parseInitConstants();
   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit(QoreNamespaceList& n);
   DLLLOCAL void parseRollback();
   DLLLOCAL void deleteAllConstants(ExceptionSink *xsink);
   DLLLOCAL void reset();
   DLLLOCAL void assimilate(QoreNamespaceList& n, qore_ns_private* parent);

   DLLLOCAL QoreNamespace *parseResolveNamespace(const NamedScope *name, unsigned *matched);
   DLLLOCAL AbstractQoreNode *parseFindConstantValue(const char *cname, const QoreTypeInfo *&typeInfo);
   DLLLOCAL AbstractQoreNode *parseFindScopedConstantValue(const NamedScope *name, unsigned *matched, const QoreTypeInfo *&typeInfo);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethod(const NamedScope *name, unsigned *matched);
   DLLLOCAL QoreClass *parseFindScopedClass(const NamedScope *name, unsigned *matched);
   DLLLOCAL QoreClass *parseFindClass(const char *ocname);
   DLLLOCAL void deleteData(ExceptionSink *xsink);

   //DLLLOCAL AbstractQoreNode *parseResolveBareword(const char *name, const QoreTypeInfo *&typeInfo) const;

   DLLLOCAL bool empty() const {
      return nsmap.empty();
   }

   DLLLOCAL qore_size_t size() const {
      return nsmap.size();
   }
};

/*
class QoreNamespaceListIterator {
protected:
   nsmap_t& nsmap;
   nsmap_t::iterator i;

public:
   DLLLOCAL QoreNamespaceListIterator(QoreNamespaceList* nsl) : nsmap(nsl->nsmap), i(nsmap.begin()) {
   }

   DLLLOCAL QoreNamespaceListIterator(QoreNamespaceList& nsl) : nsmap(nsl.nsmap), i(nsmap.begin()) {
   }

   DLLLOCAL bool next() {
      if (i == nsmap.end())
         i = nsmap.begin();
      else
         ++i;
      return (i == nsmap.end()) ? false : true;
   }

   DLLLOCAL QoreNamespace* operator->() {
      return i->second;
   }
};

class QoreNamespaceListConstIterator {
protected:
   const nsmap_t& nsmap;
   nsmap_t::const_iterator i;

public:
   DLLLOCAL QoreNamespaceListConstIterator(const QoreNamespaceList* nsl) : nsmap(nsl->nsmap), i(nsmap.end()) {
   }

   DLLLOCAL QoreNamespaceListConstIterator(const QoreNamespaceList& nsl) : nsmap(nsl.nsmap), i(nsmap.end()) {
   }

   DLLLOCAL bool next() {
      if (i == nsmap.end())
         i = nsmap.begin();
      else
         ++i;
      return (i == nsmap.end()) ? false : true;
   }

   DLLLOCAL const QoreNamespace* operator->() const {
      return i->second;
   }
};
*/

#endif
