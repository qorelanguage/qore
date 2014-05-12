/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHashNodeIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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
#include <qore/intern/xxhash.h>

typedef HASH_MAP<const char*, qhlist_t::iterator, qore_hash_str, eqstr> hm_hm_t;
#else
typedef std::map<const char*, qhlist_t::iterator, ltstr> hm_hm_t;
#endif

class qore_hash_private {
public:
   qhlist_t member_list;
   hm_hm_t hm;

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
      return i != hm.end() ? (*(i->second)) : 0;
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

   DLLLOCAL AbstractQoreNode **getKeyValuePtr(const char* key) {
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
      if ((*li)->node)
         (*li)->node->deref(xsink);
      
      internDeleteKey(li);
   }

   DLLLOCAL AbstractQoreNode *takeKeyValue(const char* key) {
      assert(key);

      hm_hm_t::iterator i = hm.find(key);
      
      if (i == hm.end())
         return 0;
      
      qhlist_t::iterator li = i->second;      
      hm.erase(i);
      
      AbstractQoreNode *rv = (*li)->node;
      internDeleteKey(li);
      return rv;
   }

   DLLLOCAL const char* getFirstKey() const  {
      return member_list.empty() ? 0 : member_list.front()->key.c_str(); 
   }
   
   DLLLOCAL const char* getLastKey() const {
      return member_list.empty() ? 0 : member_list.back()->key.c_str(); 
   }

   DLLLOCAL QoreListNode* getKeys() const {
      QoreListNode* list = new QoreListNode;

      for (qhlist_t::const_iterator i = member_list.begin(), e = member_list.end(); i != e; ++i) {
         list->push(new QoreStringNode((*i)->key));
      }
      return list;
   }

   DLLLOCAL void merge(const qore_hash_private& h, ExceptionSink* xsink) {
      for (qhlist_t::const_iterator i = h.member_list.begin(), e = h.member_list.end(); i != e; ++i) {
         setKeyValue((*i)->key, (*i)->node ? (*i)->node->refSelf() : 0, xsink);
      }
   }

   DLLLOCAL QoreHashNode* copy() const {
      QoreHashNode* h = new QoreHashNode;

      // copy all members to new object
      for (qhlist_t::const_iterator i = member_list.begin(), e = member_list.end(); i != e; ++i) {
         //printd(5, "QoreHashNode::copy() this=%p node=%p key='%s'\n", this, where->node, where->key);
         h->setKeyValue((*i)->key, (*i)->node ? (*i)->node->refSelf() : 0, 0);
      }
      return h;
   }

   DLLLOCAL AbstractQoreNode* evalImpl(ExceptionSink* xsink) const {
      QoreHashNodeHolder h(new QoreHashNode(), xsink);

      for (qhlist_t::const_iterator i = member_list.begin(), e = member_list.end(); i != e; ++i) {
         h->setKeyValue((*i)->key, (*i)->node ? (*i)->node->eval(xsink) : 0, 0);
         if (*xsink)
            return 0;
      }

      return h.release();
   }

   DLLLOCAL void setKeyValue(const std::string& key, AbstractQoreNode* val, ExceptionSink* xsink) {
      hash_assignment_priv ha(*this, key.c_str());
      ha.assign(val, xsink);
   }

   DLLLOCAL bool derefImpl(ExceptionSink* xsink) {
      for (qhlist_t::iterator i = member_list.begin(), e = member_list.end(); i != e; ++i) {
         if ((*i)->node)
            (*i)->node->deref(xsink);
         delete *i;
      }

      member_list.clear();
      hm.clear();
      return true;
   }

   DLLLOCAL void clear(ExceptionSink* xsink) {
      derefImpl(xsink);
   }

   DLLLOCAL size_t size() const {
      return member_list.size();
   }

   DLLLOCAL bool empty() const {
      return member_list.empty();
   }

   DLLLOCAL static AbstractQoreNode* getFirstKeyValue(const QoreHashNode* h) { 
      return h->priv->member_list.empty() ? 0 : h->priv->member_list.front()->node;
   }  
   
   DLLLOCAL static AbstractQoreNode* getLastKeyValue(const QoreHashNode* h) {
      return h->priv->member_list.empty() ? 0 : h->priv->member_list.back()->node;
   }
};

#endif
