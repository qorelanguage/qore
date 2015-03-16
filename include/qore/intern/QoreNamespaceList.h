/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespaceList.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  namespaces are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should 
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef _QORE_NAMESPACELIST_H

#define _QORE_NAMESPACELIST_H

#include <map>

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include <qore/intern/xxhash.h>

typedef HASH_MAP<std::string, QoreNamespace*> nsmap_t;
#else
typedef std::map<std::string, QoreNamespace*> nsmap_t;
#endif

class qore_ns_private;
class qore_root_ns_private;

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

   DLLLOCAL QoreNamespace *find(const char* name) {
      nsmap_t::iterator i = nsmap.find(name);
      return i == nsmap.end() ? 0 : i->second;
   }

   DLLLOCAL QoreNamespace *find(const std::string &name) {
      nsmap_t::iterator i = nsmap.find(name);
      return i == nsmap.end() ? 0 : i->second;
   }
   DLLLOCAL const QoreNamespace* find(const std::string& name) const {
      nsmap_t::const_iterator i = nsmap.find(name);
      return i == nsmap.end() ? 0 : i->second;
   }

   // do not delete the pointer returned from this function
   DLLLOCAL qore_ns_private* parseAdd(QoreNamespace* ot, qore_ns_private* parent);

   DLLLOCAL qore_ns_private* runtimeAdd(QoreNamespace* ot, qore_ns_private* parent);

   DLLLOCAL void resolveCopy();
   DLLLOCAL void parseInitConstants();

   DLLLOCAL void parseInitGlobalVars();
   DLLLOCAL void clearConstants(QoreListNode& l);
   DLLLOCAL void clearData(ExceptionSink* sink);
   DLLLOCAL void deleteGlobalVars(ExceptionSink* sink);

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit(QoreNamespaceList& n);
   DLLLOCAL void parseRollback();
   DLLLOCAL void deleteAllConstants(ExceptionSink *xsink);
   DLLLOCAL void reset();

   DLLLOCAL void parseAssimilate(QoreNamespaceList& n, qore_ns_private* parent);
   DLLLOCAL void runtimeAssimilate(QoreNamespaceList& n, qore_ns_private* parent);

   DLLLOCAL void deleteData(ExceptionSink *xsink);

   DLLLOCAL bool empty() const {
      return nsmap.empty();
   }

   DLLLOCAL qore_size_t size() const {
      return nsmap.size();
   }
};

#endif
