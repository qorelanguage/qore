/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHashNodeIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREHASHNODEINTERN_H

#define _QORE_QOREHASHNODEINTERN_H

#include <qore/qlist>

// to maintain the order of inserts
class HashMember {
public:
   AbstractQoreNode* node;
   std::string key;

   DLLLOCAL HashMember(const char* n_key) : node(0), key(n_key) {
   }

   DLLLOCAL ~HashMember() {
   }
};

typedef qlist<HashMember*> qhlist_t;

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include "qore/intern/xxhash.h"

typedef HASH_MAP<const char*, qhlist_t::iterator, qore_hash_str, eqstr> hm_hm_t;
#else
typedef std::map<const char*, qhlist_t::iterator, ltstr> hm_hm_t;
#endif

class qore_hash_private {
public:
   qhlist_t member_list;
   hm_hm_t hm;
   // either hashdecl or complexTypeInfo can be set, but not both
   const TypedHashDecl* hashdecl = nullptr;
   const QoreTypeInfo* complexTypeInfo = nullptr;
   unsigned obj_count = 0;
#ifdef DEBUG
   bool is_obj = false;
#endif

   DLLLOCAL qore_hash_private() {
   }

   // hashes should always be empty by the time they are deleted
   // because object destructors need to be run...
   DLLLOCAL ~qore_hash_private() {
      assert(member_list.empty());
   }

   DLLLOCAL int64 getKeyAsBigInt(const char* key, bool &found) const {
      assert(key);
      hm_hm_t::const_iterator i = hm.find(key);

      if (i != hm.end()) {
         found = true;
         return (*(i->second))->node ? (*(i->second))->node->getAsBigInt() : 0;
      }

      found = false;
      return 0;
   }

   DLLLOCAL bool getKeyAsBool(const char* key, bool& found) const {
      assert(key);
      hm_hm_t::const_iterator i = hm.find(key);

      if (i != hm.end()) {
         found = true;
         return (*(i->second))->node ? (*(i->second))->node->getAsBool() : false;
      }

      found = false;
      return false;
   }

   DLLLOCAL bool existsKey(const char* key) const {
      assert(key);
      return hm.find(key) != hm.end();
   }

   DLLLOCAL bool existsKeyValue(const char* key) const {
      assert(key);
      hm_hm_t::const_iterator i = hm.find(key);
      if (i == hm.end())
         return false;
      return !is_nothing((*(i->second))->node);
   }

   DLLLOCAL HashMember* findMember(const char* key) {
      assert(key);
      hm_hm_t::iterator i = hm.find(key);
      return i != hm.end() ? (*(i->second)) : nullptr;
   }

   DLLLOCAL HashMember* findCreateMember(const char* key) {
      // otherwise create the new hash entry
      HashMember* om = findMember(key);
      if (om)
	 return om;

      om = new HashMember(key);
      member_list.push_back(om);

      // add to the map
      qhlist_t::iterator i = member_list.end();
      --i;
      hm[om->key.c_str()] = i;

      // return the new member
      return om;
   }

   DLLLOCAL AbstractQoreNode** getKeyValuePtr(const char* key) {
      return &findCreateMember(key)->node;
   }

   // NOTE: does not delete the value, this must be done by the caller before this call
   // also does not delete map entry; must be done outside this call
   DLLLOCAL void internDeleteKey(qhlist_t::iterator i) {
      HashMember* om = *i;

      member_list.erase(i);

      // free om memory
      delete om;
   }

   DLLLOCAL void deleteKey(const char* key, ExceptionSink *xsink) {
      assert(key);

      hm_hm_t::iterator i = hm.find(key);

      if (i == hm.end())
         return;

      qhlist_t::iterator li = i->second;
      hm.erase(i);

      // dereference node if present
      if ((*li)->node) {
         if (needs_scan((*li)->node))
            incScanCount(-1);

         if ((*li)->node->getType() == NT_OBJECT)
            reinterpret_cast<QoreObject*>((*li)->node)->doDelete(xsink);
         (*li)->node->deref(xsink);
      }

      internDeleteKey(li);
   }

   // removes the value and dereferences it, without performing a delete on it
   DLLLOCAL void removeKey(const char* key, ExceptionSink *xsink) {
      assert(key);

      hm_hm_t::iterator i = hm.find(key);

      if (i == hm.end())
         return;

      qhlist_t::iterator li = i->second;
      hm.erase(i);

      // dereference node if present
      if ((*li)->node) {
         if (needs_scan((*li)->node))
            incScanCount(-1);
         (*li)->node->deref(xsink);
      }

      internDeleteKey(li);
   }

   DLLLOCAL AbstractQoreNode* takeKeyValue(const char* key) {
      assert(key);

      hm_hm_t::iterator i = hm.find(key);

      if (i == hm.end())
         return 0;

      qhlist_t::iterator li = i->second;
      hm.erase(i);

      AbstractQoreNode *rv = (*li)->node;
      internDeleteKey(li);

      if (needs_scan(rv))
         incScanCount(-1);

      return rv;
   }

   DLLLOCAL const char* getFirstKey() const  {
      return member_list.empty() ? nullptr : member_list.front()->key.c_str();
   }

   DLLLOCAL const char* getLastKey() const {
      return member_list.empty() ? nullptr : member_list.back()->key.c_str();
   }

   DLLLOCAL QoreListNode* getKeys() const {
      QoreListNode* list = new QoreListNode;

      for (qhlist_t::const_iterator i = member_list.begin(), e = member_list.end(); i != e; ++i) {
         list->push(new QoreStringNode((*i)->key));
      }
      return list;
   }

   DLLLOCAL void merge(const qore_hash_private& h, ExceptionSink* xsink);

   DLLLOCAL int getLValue(const char* key, LValueHelper& lvh, bool for_remove, ExceptionSink* xsink);

   DLLLOCAL QoreHashNode* getCopy() const {
      QoreHashNode* h = new QoreHashNode;
      if (hashdecl)
         h->priv->hashdecl = hashdecl;
      if (complexTypeInfo)
         h->priv->complexTypeInfo = complexTypeInfo;
      return h;
   }

   DLLLOCAL QoreHashNode* copy() const {
      QoreHashNode* h = getCopy();

      // copy all members to new object
      for (auto& i : member_list) {
         hash_assignment_priv ha(*h->priv, i->key.c_str());
         ha.swap(i->node ? i->node->refSelf() : nullptr);
      }
      return h;
   }

   DLLLOCAL AbstractQoreNode* evalImpl(ExceptionSink* xsink) const {
      QoreHashNodeHolder h(getCopy(), xsink);

      for (qhlist_t::const_iterator i = member_list.begin(), e = member_list.end(); i != e; ++i) {
         h->setKeyValue((*i)->key, (*i)->node ? (*i)->node->eval(xsink) : nullptr, xsink);
         if (*xsink)
            return nullptr;
      }

      return h.release();
   }

   DLLLOCAL void setKeyValue(const std::string& key, AbstractQoreNode* val, ExceptionSink* xsink) {
      hash_assignment_priv ha(*this, key.c_str());
      ha.assign(val, xsink);
   }

   DLLLOCAL void setKeyValue(const char* key, AbstractQoreNode* val, qore_object_private* o, ExceptionSink* xsink) {
      hash_assignment_priv ha(*this, key, false, o);
      ha.assign(val, xsink);
   }

   DLLLOCAL bool derefImpl(ExceptionSink* xsink, bool reverse = false) {
      if (reverse) {
         for (qhlist_t::reverse_iterator i = member_list.rbegin(), e = member_list.rend(); i != e; ++i) {
            if ((*i)->node)
               (*i)->node->deref(xsink);
            delete *i;
         }
      } else {
         for (qhlist_t::iterator i = member_list.begin(), e = member_list.end(); i != e; ++i) {
            if ((*i)->node)
               (*i)->node->deref(xsink);
            delete *i;
         }
      }

      member_list.clear();
      hm.clear();
      obj_count = 0;
      return true;
   }

   DLLLOCAL AbstractQoreNode* swapKeyValue(const char* key, AbstractQoreNode* val, qore_object_private* o) {
      //printd(0, "qore_hash_private::swapKeyValue() this: %p key: %s val: %p (%s) deprecated API called\n", this, key, val, get_node_type(val));
      //assert(false);
      hash_assignment_priv ha(*this, key, false, o);
      return ha.swap(val);
   }

   DLLLOCAL void clear(ExceptionSink* xsink, bool reverse) {
      derefImpl(xsink, reverse);
   }

   DLLLOCAL size_t size() const {
      return member_list.size();
   }

   DLLLOCAL bool empty() const {
      return member_list.empty();
   }

   DLLLOCAL void incScanCount(int dt) {
      assert(!is_obj);
      assert(dt);
      assert(obj_count || dt > 0);
      //printd(5, "qore_hash_private::incScanCount() this: %p dt: %d: %d -> %d\n", this, dt, obj_count, obj_count + dt);
      obj_count += dt;
   }

   DLLLOCAL const QoreTypeInfo* getValueTypeInfo() const {
      return complexTypeInfo ? QoreTypeInfo::getUniqueReturnComplexHash(complexTypeInfo) : nullptr;
   }

   DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
      if (hashdecl)
          return hashdecl->getTypeInfo();
      if (complexTypeInfo)
          return complexTypeInfo;
      return hashTypeInfo;
   }

   DLLLOCAL const TypedHashDecl* getHashDecl() const {
      return hashdecl;
   }

   DLLLOCAL static QoreHashNode* newHashDecl(const TypedHashDecl* hd) {
      QoreHashNode* rv = new QoreHashNode;
      rv->priv->hashdecl = hd;
      return rv;
   }

   DLLLOCAL static qore_hash_private* get(QoreHashNode& h) {
      return h.priv;
   }

   DLLLOCAL static const qore_hash_private* get(const QoreHashNode& h) {
      return h.priv;
   }

   // returns -1 if no checks are needed or if an error is raised, 0 if OK to check
   DLLLOCAL static int parseInitHashInitialization(const QoreProgramLocation& loc, LocalVar *oflag, int pflag, int& lvids, QoreListNode* args, const QoreTypeInfo*& argTypeInfo, const AbstractQoreNode*& arg);

   DLLLOCAL static int parseInitComplexHashInitialization(const QoreProgramLocation& loc, LocalVar *oflag, int pflag, QoreListNode* args, const QoreTypeInfo* vti);

   DLLLOCAL static void parseCheckComplexHashInitialization(const QoreProgramLocation& loc, const QoreTypeInfo* typeInfo, const QoreTypeInfo* expTypeInfo, const AbstractQoreNode* exp, const char* context_action, bool strict_check = true);

   DLLLOCAL static void parseCheckTypedAssignment(const QoreProgramLocation& loc, const AbstractQoreNode* arg, const QoreTypeInfo* vti, const char* context_action, bool strict_check = true);

   DLLLOCAL static QoreHashNode* newComplexHash(const QoreTypeInfo* typeInfo, const QoreListNode* args, ExceptionSink* xsink);

   DLLLOCAL static QoreHashNode* newComplexHashFromHash(const QoreTypeInfo* typeInfo, QoreHashNode* init, ExceptionSink* xsink);

   DLLLOCAL static unsigned getScanCount(const QoreHashNode& h) {
      assert(!h.priv->is_obj);
      return h.priv->obj_count;
   }

   DLLLOCAL static void incScanCount(const QoreHashNode& h, int dt) {
      assert(!h.priv->is_obj);
      h.priv->incScanCount(dt);
   }

   DLLLOCAL static AbstractQoreNode* getFirstKeyValue(const QoreHashNode* h) {
      return h->priv->member_list.empty() ? nullptr : h->priv->member_list.front()->node;
   }

   DLLLOCAL static AbstractQoreNode* getLastKeyValue(const QoreHashNode* h) {
      return h->priv->member_list.empty() ? nullptr : h->priv->member_list.back()->node;
   }
};

#endif
