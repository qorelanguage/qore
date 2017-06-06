/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ConstantList.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  constants can only be defined when parsing
  constants values will be substituted during the 2nd parse phase

  reads and writes are (must be) wrapped under the program-level parse lock

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

#ifndef _QORE_CONSTANTLIST_H

#define _QORE_CONSTANTLIST_H

#include <qore/common.h>
#include "qore/intern/ParseNode.h"

#include <map>
#include <string>

class qore_ns_private;
class qore_class_private;

// tricky structure that holds 2 types of pointers and a flag in the space of 1 pointer
// the flag is in the low bit since memory has to be aligned anyway we have at a few bits of space for flags
struct ClassNs {
   // if the low bit is set, then ptr is a qore_ns_priv*, otherwise it's a qore_class_private
   size_t ptr;

   DLLLOCAL ClassNs(qore_class_private* qc) : ptr((size_t)qc) {
   }

   DLLLOCAL ClassNs(qore_ns_private* ns) {
      ptr = ((size_t)ns) | (size_t)1;
   }

   DLLLOCAL ClassNs(const ClassNs& old) : ptr(old.ptr) {
   }

   DLLLOCAL qore_class_private* getClass() const {
      return (!(ptr & (size_t)1)) ? (qore_class_private*)ptr : 0;
   }

   DLLLOCAL qore_ns_private* getNs() const {
      return (ptr & (size_t)1) ? (qore_ns_private*)(ptr & ~(size_t)1) : 0;
   }

   DLLLOCAL bool isNs() const {
      return (bool)(ptr & (size_t)1);
   }

#ifdef DEBUG
   DLLLOCAL const char* getType() const {
      return isNs() ? "namespace" : "class";
   }

   DLLLOCAL const char* getName() const;
#endif
};

class RuntimeConstantRefNode;

class ConstantEntry : public QoreReferenceCounter {
   friend class ConstantEntryInitHelper;
   friend class RuntimeConstantRefNode;
   friend class ConstantList;

protected:
   AbstractQoreNode* saved_node;
   ClassAccess access;

   int scanValue(const AbstractQoreNode* n) const;

   DLLLOCAL ~ConstantEntry() {
      assert(!saved_node);
      assert(!node);
   }

public:
   QoreProgramLocation loc;
   ParseWarnOptions pwo;
   std::string name;
   const QoreTypeInfo* typeInfo;
   AbstractQoreNode* node;
   bool in_init : 1,  // being initialized
      pub : 1,        // public constant (modules only)
      init : 1,       // already initialized
      builtin : 1     // builtin vs user
      ;

   DLLLOCAL ConstantEntry(const QoreProgramLocation& loc, const char* n, AbstractQoreNode* v, const QoreTypeInfo* ti = 0, bool n_pub = false, bool n_init = false, bool n_builtin = false, ClassAccess n_access = Public);
   DLLLOCAL ConstantEntry(const ConstantEntry& old);

   DLLLOCAL void deref() {
      if (ROdereference())
         delete this;
   }

   DLLLOCAL void ref() {
      ROreference();
   }

   DLLLOCAL ConstantEntry* refSelf() {
      ref();
      return this;
   }

   DLLLOCAL void del(ExceptionSink* xsink);
   DLLLOCAL void del(QoreListNode& l);

   DLLLOCAL int parseInit(ClassNs ptr);

   DLLLOCAL AbstractQoreNode* get(const QoreTypeInfo*& constantTypeInfo, ClassNs ptr) {
      if (in_init) {
         parse_error("recursive constant reference found to constant '%s'", name.c_str());
         constantTypeInfo = nothingTypeInfo;
         return 0;
      }

      if (!init && parseInit(ptr)) {
         constantTypeInfo = nothingTypeInfo;
         return 0;
      }

      constantTypeInfo = typeInfo;
      return node;
   }

   DLLLOCAL const char* getName() const {
      return name.c_str();
   }

   DLLLOCAL const std::string& getNameStr() const {
      return name;
   }

   DLLLOCAL bool isPublic() const {
      return pub;
   }

   DLLLOCAL bool isUserPublic() const {
      return pub && !builtin;
   }

   DLLLOCAL bool isSystem() const {
      return builtin;
   }

   DLLLOCAL bool isUser() const {
      return !builtin;
   }

   DLLLOCAL ClassAccess getAccess() const {
      return access;
   }
};

class ConstantEntryInitHelper {
protected:
   ConstantEntry &ce;

public:
   DLLLOCAL ConstantEntryInitHelper(ConstantEntry& n_ce) : ce(n_ce) {
      assert(!ce.in_init);
      assert(!ce.init);
      ce.in_init = true;
      //printd(5, "ConstantEntryInitHelper::ConstantEntryInitHelper() '%s'\n", ce.getName());
   }

   DLLLOCAL ~ConstantEntryInitHelper() {
      ce.in_init = false;
      ce.init = true;
      //printd(5, "ConstantEntryInitHelper::~ConstantEntryInitHelper() '%s'\n", ce.getName());
   }
};

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include "qore/intern/xxhash.h"

typedef HASH_MAP<const char*, ConstantEntry*, qore_hash_str, eqstr> cnemap_t;
#else
typedef std::map<const char*, ConstantEntry*, ltstr> cnemap_t;
#endif

class ConstantList {
   friend class ConstantListIterator;
   friend class ConstConstantListIterator;

private:
   // not implemented
   DLLLOCAL ConstantList& operator=(const ConstantList&);

   DLLLOCAL void clearIntern(ExceptionSink* xsink);

protected:
   // the object that owns the list (either a class or a namespace)
   ClassNs ptr;

public:
   cnemap_t cnemap;

   DLLLOCAL ~ConstantList();

   DLLLOCAL ConstantList(ClassNs p) : ptr(p) {
      //printd(5, "ConstantList::ConstantList() this: %p cls: %p ns: %p\n", this, ptr.getClass(), ptr.getNs());
   }

   DLLLOCAL ConstantList(const ConstantList& old, int64 po, ClassNs p);

   // do not delete the object returned by this function
   DLLLOCAL cnemap_t::iterator add(const char* name, AbstractQoreNode* val, const QoreTypeInfo* typeInfo = 0, ClassAccess access = Public);

   DLLLOCAL cnemap_t::iterator parseAdd(const QoreProgramLocation& loc, const char* name, AbstractQoreNode* val, const QoreTypeInfo* typeInfo = 0, bool pub = false, ClassAccess access = Public);

   DLLLOCAL ConstantEntry* findEntry(const char* name);

   DLLLOCAL AbstractQoreNode* parseFind(const char* name, const QoreTypeInfo*& constantTypeInfo, ClassAccess& access);

   DLLLOCAL AbstractQoreNode* parseFind(const char* name, const QoreTypeInfo*& constantTypeInfo) {
      ClassAccess access;
      return parseFind(name, constantTypeInfo, access);
   }

   DLLLOCAL AbstractQoreNode* find(const char* name, const QoreTypeInfo*& constantTypeInfo, ClassAccess& access);

   DLLLOCAL AbstractQoreNode* find(const char* name, const QoreTypeInfo*& constantTypeInfo) {
      ClassAccess access;
      return find(name, constantTypeInfo, access);
   }

   DLLLOCAL bool inList(const char* name) const;
   DLLLOCAL bool inList(const std::string& name) const;
   //DLLLOCAL ConstantList *copy();

   // assimilate the list without any duplicate checking
   DLLLOCAL void assimilate(ConstantList& n);

   // assimilate a constant list in a namespace with duplicate checking (also in pending list)
   DLLLOCAL void assimilate(ConstantList& n, ConstantList& otherlist, const char* type, const char* name);

   // copy all user/public elements of the source list to the target, assuming no duplicates
   DLLLOCAL void mergeUserPublic(const ConstantList& src);

   DLLLOCAL int importSystemConstants(const ConstantList& src, ExceptionSink* xsink);

   // add a constant to a list with duplicate checking (pub & priv + pending)
   DLLLOCAL void parseAdd(const QoreProgramLocation& loc, const std::string& name, AbstractQoreNode* val, ConstantList& committed, ClassAccess access, const char* cname);

   DLLLOCAL void parseInit();
   DLLLOCAL QoreHashNode* getInfo();
   DLLLOCAL void parseDeleteAll();
   DLLLOCAL void clear(QoreListNode& l);
   DLLLOCAL void deleteAll(ExceptionSink* xsink);
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

   DLLLOCAL void setAccess(ClassAccess access) {
      for (auto& i : cnemap)
         i.second->access = access;
   }
};

class ConstantListIterator {
protected:
   cnemap_t& cl;
   cnemap_t::iterator i;

public:
   DLLLOCAL ConstantListIterator(ConstantList& n_cl) : cl(n_cl.cnemap), i(cl.end()) {
   }

   DLLLOCAL bool next() {
      if (i == cl.end())
         i = cl.begin();
      else
         ++i;
      return i != cl.end();
   }

   DLLLOCAL const std::string& getName() const {
      return i->second->getNameStr();
   }

   DLLLOCAL AbstractQoreNode* getValue() const {
      return i->second->node;
   }

   DLLLOCAL ConstantEntry* getEntry() const {
      return i->second;
   }

   DLLLOCAL ClassAccess getAccess() const {
      return i->second->getAccess();
   }

   DLLLOCAL const bool isPublic() const {
      return i->second->isPublic();
   }

   DLLLOCAL const bool isUserPublic() const {
      return i->second->isUserPublic();
   }
};

class ConstConstantListIterator {
protected:
   const cnemap_t& cl;
   cnemap_t::const_iterator i;

public:
   DLLLOCAL ConstConstantListIterator(const ConstantList& n_cl) : cl(n_cl.cnemap), i(cl.end()) {
   }

   DLLLOCAL bool next() {
      if (i == cl.end())
         i = cl.begin();
      else
         ++i;
      return i != cl.end();
   }

   DLLLOCAL const std::string& getName() const {
      return i->second->getNameStr();
   }

   DLLLOCAL const AbstractQoreNode* getValue() const {
      return i->second->node;
   }

   DLLLOCAL const ConstantEntry* getEntry() const {
      return i->second;
   }

   DLLLOCAL const bool isPublic() const {
      return i->second->isPublic();
   }

   DLLLOCAL const bool isUserPublic() const {
      return i->second->isUserPublic();
   }
};

class RuntimeConstantRefNode : public ParseNode {
protected:
   ConstantEntry* ce;

   virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
      return this;
   }

   virtual const QoreTypeInfo* getTypeInfo() const {
      assert(ce->saved_node);
      return getTypeInfoForValue(ce->saved_node);
   }

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
      assert(ce->saved_node);
      return ce->saved_node->evalValue(needs_deref, xsink);
   }

   DLLLOCAL ~RuntimeConstantRefNode() {
      ce->deref();
   }

public:
   DLLLOCAL RuntimeConstantRefNode(const QoreProgramLocation& loc, ConstantEntry* n_ce) : ParseNode(loc, NT_RTCONSTREF, true, false), ce(n_ce) {
      assert(ce->saved_node);
   }

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
      assert(ce->saved_node);
      return ce->saved_node->getAsString(str, foff, xsink);
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      assert(ce->saved_node);
      return ce->saved_node->getAsString(del, foff, xsink);
   }

   DLLLOCAL virtual const char* getTypeName() const {
      return ce->saved_node ? ce->saved_node->getTypeName() : "nothing";
   }
};

#endif // _QORE_CONSTANTLIST_H
