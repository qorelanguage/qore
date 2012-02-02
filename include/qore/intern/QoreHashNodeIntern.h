/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreHashNodeIntern.h

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

#ifndef _QORE_QOREHASHNODEINTERN_H

#define _QORE_QOREHASHNODEINTERN_H

// FIXME: use STL list instead
// to maintain the order of inserts
class HashMember {
public:
   AbstractQoreNode *node;
   char *key;
   HashMember *next;
   HashMember *prev;

   DLLLOCAL HashMember(HashMember *n_prev, char *n_key) : node(0), key(n_key), next(0), prev(n_prev) {
   }
   DLLLOCAL ~HashMember() {
      if (key)
         free(key);
   }
};

typedef std::map<const char*, HashMember *, ltstr> hm_hm_t;

struct qore_hash_private {
   HashMember *member_list, *tail;
   qore_size_t len;
   hm_hm_t hm;

   DLLLOCAL qore_hash_private() : member_list(0), tail(0), len(0) {
   }
      
   // hashes should always be empty by the time they are deleted 
   // because object destructors need to be run...
   DLLLOCAL ~qore_hash_private() {
      assert(!member_list);
   }

   DLLLOCAL int64 getKeyAsBigInt(const char *key, bool &found) const {
      assert(key);	 
      hm_hm_t::const_iterator i = hm.find(key);

      if (i != hm.end()) {
         found = true;
         return i->second->node ? i->second->node->getAsBigInt() : 0;
      }

      found = false;
      return 0;
   }

   DLLLOCAL bool getKeyAsBool(const char *key, bool &found) const {
      assert(key);	 
      hm_hm_t::const_iterator i = hm.find(key);

      if (i != hm.end()) {
         found = true;
         return i->second->node ? i->second->node->getAsBool() : false;
      }

      found = false;
      return false;
   }

   DLLLOCAL bool existsKey(const char *key) const {
      assert(key);	 
      return hm.find(key) != hm.end();
   }

   DLLLOCAL bool existsKeyValue(const char *key) const {
      assert(key);	 
      hm_hm_t::const_iterator i = hm.find(key);
      if (i == hm.end())
         return false;
      return !is_nothing(i->second->node);
   }

   DLLLOCAL HashMember *findMember(const char *key) {
      assert(key);
      hm_hm_t::iterator i = hm.find(key);
      return i != hm.end() ? i->second : 0;
   }

   DLLLOCAL HashMember *findCreateMember(const char *key) {
      // otherwise create the new hash entry
      HashMember *om = findMember(key);
      if (om)
	 return om;

      om = new HashMember(tail, strdup(key));
      if (tail)
         tail->next = om;
      else
         member_list = om;
      tail = om;

      // add to the map
      hm[om->key] = om;

      // increment the hash size
      ++len;

      // return the new member
      return om;
   }

   DLLLOCAL AbstractQoreNode **getKeyValuePtr(const char *key) {
      return &findCreateMember(key)->node;
   }

   // NOTE: does not delete the value, this must be done by the caller before this call
   // also does not delete map entry; must be done outside this call
   DLLLOCAL void internDeleteKey(HashMember *om) {
      // remove key from the linked list
      if (om->next)
         om->next->prev = om->prev;
      if (om->prev)
         om->prev->next = om->next;
      if (om == member_list)
         member_list = om->next;
      if (om == tail)
         tail = om->prev;

      // free om memory
      delete om;

      // decrement the hash size
      --len;
   }

   DLLLOCAL void deleteKey(const char *key, ExceptionSink *xsink) {
      assert(key);
      
      hm_hm_t::iterator i = hm.find(key);
      
      if (i == hm.end())
         return;
      
      HashMember *m = i->second;
      
      hm.erase(i);
      
      // dereference node if present
      if (m->node) {
         if (m->node->getType() == NT_OBJECT)
            reinterpret_cast<QoreObject *>(m->node)->doDelete(xsink);
         m->node->deref(xsink);
      }
      
      internDeleteKey(m);
   }

   // removes the value and dereferences it, without performing a delete on it
   DLLLOCAL void removeKey(const char *key, ExceptionSink *xsink) {
      assert(key);
      
      hm_hm_t::iterator i = hm.find(key);
      
      if (i == hm.end())
         return;
      
      HashMember *m = i->second;
      
      hm.erase(i);
      
      // dereference node if present
      if (m->node)
         m->node->deref(xsink);
      
      internDeleteKey(m);
   }

   DLLLOCAL AbstractQoreNode *takeKeyValue(const char *key) {
      assert(key);

      hm_hm_t::iterator i = hm.find(key);
      
      if (i == hm.end())
         return 0;
      
      HashMember *m = i->second;
      
      hm.erase(i);
      
      AbstractQoreNode *rv = m->node;
      
      internDeleteKey(m);
      
      return rv;
   }
};

#endif
