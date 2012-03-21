/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ConstantList.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

  constants can only be defined when parsing
  constants values will be substituted during the 2nd parse phase

  reads and writes are (must be) wrapped under the program-level parse lock

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

#ifndef _QORE_CONSTANTLIST_H

#define _QORE_CONSTANTLIST_H

#ifdef _QORE_LIB_INTERN

#include <qore/common.h>

#include <map>
#include <string>

class LocalVar;

class ConstantEntry {
public:
   QoreProgramLocation loc;
   const QoreTypeInfo *typeInfo;
   AbstractQoreNode *node;
   bool init;

   DLLLOCAL ConstantEntry() : typeInfo(0), node(0), init(false) {}
   DLLLOCAL ConstantEntry(AbstractQoreNode *v, const QoreTypeInfo *ti = 0, bool n_init = false) : typeInfo(ti), node(v), init(n_init) {}

   DLLLOCAL ~ConstantEntry() {
      assert(!node);
   }

   DLLLOCAL void del(ExceptionSink* xsink);

   DLLLOCAL int parseInit(const char *name, QoreClass *class_context);
   DLLLOCAL AbstractQoreNode* get(const QoreTypeInfo*& constantTypeInfo) {
      constantTypeInfo = typeInfo;
      return node;
   }
};

typedef std::map<std::string, ConstantEntry*> cnemap_t;

class ConstantList {
   friend class ConstantListIterator;

private:
   // not implemented
   DLLLOCAL ConstantList& operator=(const ConstantList&);

   DLLLOCAL void clearIntern(ExceptionSink *xsink);
   DLLLOCAL int checkDup(const std::string &name, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname);

public:
   cnemap_t cnemap;

   DLLLOCAL ~ConstantList();

   DLLLOCAL ConstantList() {
      //fprintf(stderr, "XXX ConstantList::ConstantList() this=%p\n", this);
   }

   DLLLOCAL ConstantList(const ConstantList &old);

   // do not delete the object returned by this function
   DLLLOCAL cnemap_t::iterator add(const char *name, AbstractQoreNode *val, const QoreTypeInfo *typeInfo = 0);

   DLLLOCAL cnemap_t::iterator parseAdd(const char* name, AbstractQoreNode *val, const QoreTypeInfo *typeInfo = 0);

   DLLLOCAL AbstractQoreNode *find(const char *name, const QoreTypeInfo *&constantTypeInfo, QoreClass *class_context = 0);
   DLLLOCAL bool inList(const char *name) const;
   DLLLOCAL bool inList(const std::string &name) const;
   //DLLLOCAL ConstantList *copy();
   // assimilate the list without any duplicate checking
   DLLLOCAL void assimilate(ConstantList& n);
   // assimilate a constant list in a namespace with duplicate checking (also in pending list)
   DLLLOCAL void assimilate(ConstantList& n, ConstantList& otherlist, const char *nsname);

   // assimilate a constant list in a class constant list with duplicate checking (pub & priv + pending)
   DLLLOCAL void assimilate(ConstantList &n, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname);
   // add a constant to a list with duplicate checking (pub & priv + pending)
   DLLLOCAL void parseAdd(const std::string &name, AbstractQoreNode *val, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname);

   DLLLOCAL void parseInit(QoreClass *class_context = 0);
   DLLLOCAL QoreHashNode *getInfo();
   DLLLOCAL void parseDeleteAll();
   DLLLOCAL void deleteAll(ExceptionSink *xsink);
   DLLLOCAL void reset();

   DLLLOCAL bool empty() const {
      return cnemap.empty();
   }

   DLLLOCAL cnemap_t::iterator end() {
      return cnemap.end();
   }

   DLLLOCAL cnemap_t::const_iterator end() const {
      return cnemap.end();
   }
};

class ConstantListIterator {
protected:
   cnemap_t& cl;
   cnemap_t::iterator i;

public:
   DLLLOCAL ConstantListIterator(ConstantList &n_cl) : cl(n_cl.cnemap), i(cl.end()) {
   }

   DLLLOCAL bool next() {
      if (i == cl.end())
         i = cl.begin();
      else
         ++i;
      return i != cl.end();
   }

   DLLLOCAL const std::string& getName() const {
      return i->first;
   }

   DLLLOCAL AbstractQoreNode *getValue() const {
      return i->second->node;
   }

   DLLLOCAL ConstantEntry* getEntry() const {
      return i->second;
   }
};

#endif

#endif // _QORE_CONSTANTLIST_H
