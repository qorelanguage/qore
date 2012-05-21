/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreClassList.h

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

#ifndef _QORE_QORECLASSLIST_H

#define _QORE_QORECLASSLIST_H

#include <qore/common.h>

#include <qore/QoreClass.h>

#include <string.h>
#include <stdlib.h>

#include <map>

typedef std::map<const char*, QoreClass *, ltstr> hm_qc_t;

class QoreNamespaceList;

class QoreClassList {
   friend class ClassListIterator;
   friend class ConstClassListIterator;
      
private:
   hm_qc_t hm;        // hash_map for name lookups
      
   DLLLOCAL void deleteAll();
   DLLLOCAL void assimilate(QoreClassList& n);

   DLLLOCAL void remove(hm_qc_t::iterator i) {
      QoreClass *qc = i->second;
      //printd(5, "QCL::remove() this=%08p '%s' (%08p)\n", this, qc->getName(), qc);
      hm.erase(i);
      delete qc;
   }

public:
   DLLLOCAL QoreClassList() {}
   DLLLOCAL ~QoreClassList();
   DLLLOCAL QoreClassList(const QoreClassList& old, int64 po, qore_ns_private* ns);
   
   DLLLOCAL void mergePublic(const QoreClassList& old, qore_ns_private* ns);

   DLLLOCAL int add(QoreClass *ot);
   DLLLOCAL QoreClass *find(const char *name);
   DLLLOCAL const QoreClass *find(const char *name) const;
   DLLLOCAL void resolveCopy();
   DLLLOCAL void parseInit();
   DLLLOCAL void parseRollback();
   DLLLOCAL void parseCommit(QoreClassList& n);
   DLLLOCAL void reset();
   DLLLOCAL void assimilate(QoreClassList& n, qore_ns_private& ns);
   DLLLOCAL QoreHashNode *getInfo();

   DLLLOCAL AbstractQoreNode *findConstant(const char *cname, const QoreTypeInfo *&typeInfo);

   DLLLOCAL AbstractQoreNode *parseResolveBareword(const char *name, const QoreTypeInfo *&typeInfo);

   DLLLOCAL bool empty() const {
      return hm.empty();
   }

   DLLLOCAL void clearStaticVars(ExceptionSink* xsink);
   DLLLOCAL void deleteClassData(ExceptionSink* xsink);
};

class ClassListIterator {
protected:
   hm_qc_t& cl;
   hm_qc_t::iterator i;

public:
   DLLLOCAL ClassListIterator(QoreClassList& n_cl) : cl(n_cl.hm), i(cl.end()) {
   }

   DLLLOCAL bool next() {
      if (i == cl.end())
         i = cl.begin();
      else
         ++i;
      return i != cl.end();
   }

   DLLLOCAL const char* getName() const {
      return i->first;
   }

   DLLLOCAL QoreClass* get() const {
      return i->second;
   }

   DLLLOCAL bool isPublic() const;
};

class ConstClassListIterator {
protected:
   const hm_qc_t& cl;
   hm_qc_t::const_iterator i;

public:
   DLLLOCAL ConstClassListIterator(const QoreClassList& n_cl) : cl(n_cl.hm), i(cl.end()) {
   }

   DLLLOCAL bool next() {
      if (i == cl.end())
         i = cl.begin();
      else
         ++i;
      return i != cl.end();
   }

   DLLLOCAL const char* getName() const {
      return i->first;
   }

   DLLLOCAL const QoreClass* get() const {
      return i->second;
   }

   DLLLOCAL bool isPublic() const;
};

#endif // _QORE_QORECLASSLIST_H
