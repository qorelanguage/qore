/*
  Hash.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_HASH_H

#define _QORE_HASH_H

#include <qore/config.h>
#include <qore/hash_map.h>

#include <stdio.h>
#include <string.h>

// to maintain the order of inserts
class HashMember {
   public:
      class QoreNode *node;
      char *key;
      class HashMember *next;
      class HashMember *prev;
};

class HashIterator
{
   private:
      class HashMember *head;
      class HashMember *ptr;

   public:
      inline HashIterator(class HashMember *h) { ptr = NULL; head = h; }
      inline class HashMember *next() 
      { 
	 if (ptr) 
	    ptr = ptr->next;
	 else
	    ptr = head;
	 return ptr;
      }
      inline char *getKey()
      { 
	 if (!ptr)
	    return NULL;

	 return ptr->key;
      }
      inline class QoreString *getKeyString();
      inline class QoreNode *getValue()
      {
	 if (ptr)
	    return ptr->node;
	 return NULL;
      }
      inline class QoreNode **getValuePtr()
      {
	 if (ptr)
	    return &(ptr->node);
	 return NULL;
      }
      inline class QoreNode *eval(class ExceptionSink *xsink);
      //inline bool last() { return (bool)(ptr ? !ptr->next : false); } 
      //inline void setValue(class QoreNode *val, class ExceptionSink *xsink);
};

class Hash
{
   private:
      class HashMember *member_list;
      class HashMember *tail;
#ifdef HAVE_QORE_HASH_MAP
      hm_hm_t hm;
#else
      int num_elements;

      inline class HashMember *findKey(char *key);
#endif
      inline class QoreNode **newKeyValue(char *key, class QoreNode *value);
      inline void internDeleteKey(class HashMember *m, class ExceptionSink *xsink);

   public:
      bool needs_eval;

      inline Hash(bool ne = false);
      inline ~Hash();

      // deprecated APIs
      //inline class QoreNode *getKeyValue(class QoreString *key);
      //inline class QoreNode *getFirstKeyValue();

      inline char *getFirstKey() { if (member_list) return member_list->key; return NULL; }
      inline class QoreNode *getKeyValueExistence(char *key);
      inline class QoreNode *getKeyValueExistence(class QoreString *key, class ExceptionSink *xsink);
      inline class QoreNode *getKeyValue(class QoreString *key, class ExceptionSink *xsink);
      inline class QoreNode *getKeyValue(char *key);
      inline class Hash *copy();

      // APIs suitable for objects below
      inline class QoreNode **getKeyValuePtr(class QoreString *key, class ExceptionSink *xsink);
      inline class QoreNode **getKeyValuePtr(char *key);
      inline class QoreNode **getExistingValuePtr(class QoreString *key, class ExceptionSink *xsink);
      inline class QoreNode **getExistingValuePtr(char *key);
      inline void merge(class Hash *h, class ExceptionSink *xsink);
      inline void assimilate(class Hash *h, class ExceptionSink *xsink);
      inline class Hash *eval(class ExceptionSink *xsink);
      inline class QoreNode *evalKey(char *key, class ExceptionSink *xsink);
      inline class QoreNode *evalKeyExistence(char *key, class ExceptionSink *xsink);
      inline void setKeyValue(class QoreString *key, class QoreNode *value, class ExceptionSink *xsink);
      inline void setKeyValue(char *key, class QoreNode *value, class ExceptionSink *xsink);
      inline void deleteKey(class QoreString *key, class ExceptionSink *xsink);
      void deleteKey(char *key, class ExceptionSink *xsink);
      inline class List *getKeys();
      inline bool compareSoft(class Hash *h, class ExceptionSink *xsink);
      inline bool compareHard(class Hash *h);
      inline class QoreNode *evalFirstKeyValue(class ExceptionSink *xsink);
      inline class HashIterator *newIterator()
      {
	 return new HashIterator(member_list);
      }
#ifdef HAVE_QORE_HASH_MAP
      inline int size() { return hm.size(); }
#else
      inline int size() { return num_elements; }
#endif
      void dereference(class ExceptionSink *xsink);
};

#include <qore/QoreNode.h>
#include <qore/List.h>
#include <qore/QoreString.h>
#include <qore/Object.h>
#include <qore/charset.h>

#include <string.h>
#include <stdlib.h>

inline void Hash::internDeleteKey(class HashMember *om, class ExceptionSink *xsink)
{
   // dereference node if present
   if (om->node)
   {
      if (om->node->type == NT_OBJECT)
         om->node->val.object->doDelete(xsink);
      om->node->deref(xsink);
   }
   // remove key from list
   if (om->next)
      om->next->prev = om->prev;
   if (om->prev)
      om->prev->next = om->next;
   if (om == member_list)
      member_list = om->next;
   if (om == tail)
      tail = om->prev;
   // free string memory
   free(om->key);
   // free om memory
   delete om;

#ifndef HAVE_QORE_HASH_MAP
   num_elements--;
#endif
}

inline class QoreNode **Hash::getKeyValuePtr(QoreString *key, class ExceptionSink *xsink)
{
   if (key->getEncoding() && key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      QoreNode **rv = getKeyValuePtr(ns->getBuffer());
      delete ns;
      return rv;
   }
   return getKeyValuePtr(key->getBuffer());
}

inline void Hash::deleteKey(QoreString *key, ExceptionSink *xsink)
{
   if (key->getEncoding() && key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return;
      deleteKey(ns->getBuffer(), xsink);
      delete ns;
   }
   else
      deleteKey(key->getBuffer(), xsink);
}

inline class QoreNode *Hash::getKeyValueExistence(QoreString *key, class ExceptionSink *xsink)
{
   if (key->getEncoding() && key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      QoreNode *rv = getKeyValueExistence(ns->getBuffer());
      delete ns;
      return rv;
   }
   return getKeyValueExistence(key->getBuffer());
}

inline void Hash::setKeyValue(QoreString *key, class QoreNode *value, ExceptionSink *xsink)
{
   if (key->getEncoding() && key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return;
      setKeyValue(ns->getBuffer(), value, xsink);
      delete ns;
   }
   else
      setKeyValue(key->getBuffer(), value, xsink);
}

inline void Hash::setKeyValue(char *key, class QoreNode *value, ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::setKeyValue(char) key=NULL\n");
#endif
   class QoreNode **v = getKeyValuePtr(key);
   if (*v)
      (*v)->deref(xsink);
   *v = value;
}

inline class QoreNode **Hash::getExistingValuePtr(QoreString *key, class ExceptionSink *xsink)
{
   if (key->getEncoding() && key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      QoreNode **rv = getExistingValuePtr(ns->getBuffer());
      delete ns;
      return rv;
   }
   return getExistingValuePtr(key->getBuffer());
}

inline class QoreNode *Hash::getKeyValue(QoreString *key, class ExceptionSink *xsink)
{
   if (key->getEncoding() && key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      QoreNode *rv = getKeyValue(ns->getBuffer());
      delete ns;
      return rv;
   }
   return getKeyValue(key->getBuffer());
}

// this function should only be called when the key doesn't exist
inline class QoreNode **Hash::newKeyValue(char *key, class QoreNode *value)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::newKeyValue() key=NULL\n");
#endif

   class HashMember *om = new HashMember;
   om->node = value;
   om->next = NULL;
   om->prev = tail;
   om->key = strdup(key);
   if (tail)
      tail->next = om;
   else
      member_list = om;
   tail = om;

#ifdef HAVE_QORE_HASH_MAP
   hm[om->key] = om;
#else
   num_elements++;
#endif

   return &om->node;
}

// retrieve keys in order they were inserted
inline class List *Hash::getKeys()
{
   class List *list = new List();
   class HashMember *where = member_list;
   
   while (where)
   {
      list->push(new QoreNode(where->key));
      where = where->next;
   }
   return list;
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
// order is maintained
inline void Hash::merge(class Hash *h, class ExceptionSink *xsink)
{
   class HashMember *where = h->member_list;
   
   while (where)
   {
      setKeyValue(where->key, where->node ? where->node->RefSelf() : NULL, xsink);
      where = where->next;
   }
}

// adds all elements (already referenecd) from the hash passed, deletes the
// hased passed
// order is maintained
inline void Hash::assimilate(class Hash *h, ExceptionSink *xsink)
{
   class HashMember *where = h->member_list;
   
   while (where)
   {
      setKeyValue(where->key, where->node, xsink);
      where->node = NULL;
      where = where->next;
   }
   delete h;
}

// can only be used on hashes populated with parsed data - no objects can be present
// returns the same order
inline class Hash *Hash::copy()
{
   Hash *h = new Hash();

   // copy all members to new object
   class HashMember *where = member_list;
   while (where)
   {
      h->setKeyValue(where->key, where->node ? where->node->RefSelf() : NULL, NULL);
      where = where->next;
   }
   return h;
}

// returns a hash with the same order
inline class Hash *Hash::eval(ExceptionSink *xsink)
{
   class Hash *h = new Hash();

   class HashMember *where = member_list;
   while (where)
   {
      h->setKeyValue(where->key, where->node ? where->node->eval(xsink) : NULL, xsink);
      if (xsink->isEvent())
	 break;
      where = where->next;
   }
   return h;
}

inline class QoreNode *Hash::evalFirstKeyValue(class ExceptionSink *xsink)
{
   if (!member_list || !member_list->node)
      return NULL;
   
   return member_list->node->eval(xsink);
}

// hashes should always be empty by the time they are deleted 
// because object destructors need to be run...
inline Hash::~Hash()
{
#ifdef DEBUG
   if (member_list)
      run_time_error("Hash::~Hash() %08x not empty! elements=%d member_list=%08x\n", this, size(), member_list);
#endif
}

inline Hash::Hash(bool ne) 
{ 
   needs_eval = ne; 
   member_list = NULL; 
   tail = NULL; 
#ifndef HAVE_QORE_HASH_MAP
   num_elements = 0; 
#endif
}

inline class QoreNode *HashIterator::eval(class ExceptionSink *xsink)
{
   if (ptr && ptr->node)
      return ptr->node->eval(xsink);
   return NULL;
}

inline class QoreString *HashIterator::getKeyString()
{
   if (!ptr)
      return NULL;

   return new QoreString(QCS_DEFAULT, ptr->key);
}

/*
inline void HashIterator::setValue(class QoreNode *val, class ExceptionSink *xsink)
{
   if (!ptr)
      return;

   if (ptr->node)
      ptr->node->deref(xsink);
   ptr->node = val;
}
*/

#ifdef HAVE_QORE_HASH_MAP
inline class QoreNode *Hash::evalKey(char *key, class ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::evalKey() key=NULL\n");
#endif
   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end() && i->second->node)
      return i->second->node->eval(xsink);

   return NULL;
}

inline class QoreNode *Hash::evalKeyExistence(char *key, class ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::evalKeyExistence() key=NULL\n");
#endif
   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end())
   {
      if (i->second->node)
	 return i->second->node->eval(xsink);
      
      return NULL;
   }
   return (QoreNode *)-1;
}

inline class QoreNode **Hash::getKeyValuePtr(char *key)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::getKeyValuePtr() key=NULL\n");
#endif
   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end())
      return &i->second->node;

   return newKeyValue(key, NULL);
}

inline class QoreNode *Hash::getKeyValue(char *key)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::getKeyValue() key=NULL\n");
#endif

   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end())
      return i->second->node;

   return NULL;
}

inline class QoreNode *Hash::getKeyValueExistence(char *key)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::getKeyValueExistence() key=NULL\n");
#endif

   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end())
      return i->second->node;

   return (QoreNode *)-1;
}

// does a "soft" compare (values of different types are converted if necessary and then compared)
// 0 = equal, 1 = not equal
inline bool Hash::compareSoft(class Hash *h, class ExceptionSink *xsink)
{
   if (h->hm.size() != hm.size())
      return 1;

   for (hm_hm_t::iterator i = hm.begin(); i != hm.end(); i++)
   {
      hm_hm_t::iterator j = h->hm.find(i->first);
      if (j == h->hm.end())
	 return 1;

      if (::compareSoft(i->second->node, j->second->node, xsink))
	 return 1;
   }

   return 0;
}

// does a "hard" compare (types must be exactly the same)
// 0 = equal, 1 = not equal
inline bool Hash::compareHard(class Hash *h)
{
   if (h->hm.size() != hm.size())
      return 1;

   for (hm_hm_t::iterator i = hm.begin(); i != hm.end(); i++)
   {
      hm_hm_t::iterator j = h->hm.find(i->first);
      if (j == h->hm.end())
	 return 1;

      if (::compareHard(i->second->node, j->second->node))
	 return 1;
   }

   return 0;
}

inline class QoreNode **Hash::getExistingValuePtr(char *key)
{
   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end())
      return &i->second->node;
   
   return NULL;
}

#else // HAVE_QORE_HASH_MAP is not defined (no hash_map)

inline class HashMember *Hash::findKey(char *key)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::findKey() key=NULL\n");
#endif
   class HashMember *where = member_list;

   while (where)
   {
      //printd(5, "Hash::findKey(%s) == %s? (where=%08x, next=%08x)\n", key, where->key, where, where->next);
      if (!strcmp(where->key, key))
	 break;
      where = where->next;
   }
   //printd(5, "O:fk() where=%08x\n", where);
   return where;
}

inline class QoreNode *Hash::evalKey(char *key, class ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::evalKey() key=NULL\n");
#endif
   class HashMember *om;

   //printd(5, "Hash::evalKey(%s)\n", key);
   if ((om = findKey(key)))
   {
      if (om->node)
	 return om->node->eval(xsink);
      return NULL;
   }
   return NULL;
}

inline class QoreNode *Hash::evalKeyExistence(char *key, class ExceptionSink *xsink)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::evalKeyExistence() key=NULL\n");
#endif
   class HashMember *om;

   //printd(5, "Hash::getKeyValue(%s)\n", key);
   if ((om = findKey(key)))
   {
      if (om->node)
	 return om->node->eval(xsink);
      return NULL;
   }
   return (QoreNode *)-1;
}

inline class QoreNode **Hash::getKeyValuePtr(char *key)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::getKeyValuePtr() key=NULL\n");
#endif
   class HashMember *om;

   if ((om = findKey(key)))
      return &om->node;
   return newKeyValue(key, NULL);
}

inline class QoreNode *Hash::getKeyValue(char *key)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::getKeyValue() key=NULL\n");
#endif
   class HashMember *om;

   //printd(5, "Hash::getKeyValue(%s)\n", key);
   if ((om = findKey(key)))
      return om->node;
   return NULL;
}

/*
inline class QoreNode *Hash::getKeyValue(class QoreString *key)
{
   return getKeyValue(key->getBuffer());
}

inline class QoreNode *Hash::getFirstKeyValue()
{
   if (!member_list)
      return NULL;
   return member_list->node;
}
*/

inline class QoreNode *Hash::getKeyValueExistence(char *key)
{
#ifdef DEBUG
   if (!key) run_time_error("Hash::getKeyValueExistence() key=NULL\n");
#endif
   class HashMember *om;

   //printd(5, "Hash::getKeyValueExistence(%s)\n", key);
   if ((om = findKey(key)))
      return om->node;
   return (QoreNode *)-1;
}

// does a "soft" compare (values of different types are converted if necessary and then compared)
// 0 = equal, 1 = not equal
inline bool Hash::compareSoft(class Hash *h, class ExceptionSink *xsink)
{
   if (h->num_elements != num_elements)
      return 1;
   class HashMember *where = member_list;
   while (where)
   {
      class QoreNode *n = h->getKeyValueExistence(where->key);
      if (n == (QoreNode *)-1)
	 return 1;
      if (::compareSoft(where->node, n, xsink))
	 return 1;
      where = where->next;
   }
   return 0;
}

// does a "hard" compare (types must be exactly the same)
// 0 = equal, 1 = not equal
inline bool Hash::compareHard(class Hash *h)
{
   if (h->num_elements != num_elements)
      return 1;
   class HashMember *where = member_list;
   while (where)
   {
      class QoreNode *n = h->getKeyValueExistence(where->key);
      if (n == (QoreNode *)-1)
	 return 1;
      if (::compareHard(where->node, n))
	 return 1;
      where = where->next;
   }
   return 0;
}

inline class QoreNode **Hash::getExistingValuePtr(char *key)
{
   class HashMember *om;

   if ((om = findKey(key)))
      return &om->node;
   
   return NULL;
}

#endif // HAVE_QORE_HASH_MAP

#endif // _QORE_HASH_H
