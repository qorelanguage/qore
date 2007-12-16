/*
  QoreHash.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include <qore/Qore.h>
#include <qore/minitest.hpp>

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>

#ifdef DEBUG_TESTS
#  include "tests/Hash_tests.cc"
#endif

HashIterator::HashIterator(class QoreHash *qh) 
{
   h = qh;
   ptr = NULL;
}

HashIterator::HashIterator(class QoreHash &qh) 
{
   h = &qh;
   ptr = NULL;
}

class QoreNode *HashIterator::eval(class ExceptionSink *xsink) const
{
   if (ptr && ptr->node)
      return ptr->node->eval(xsink);
   return NULL;
}

class QoreString *HashIterator::getKeyString() const
{
   if (!ptr)
      return NULL;
   
   return new QoreString(ptr->key, QCS_DEFAULT);
}

/*
 void HashIterator::setValue(class QoreNode *val, class ExceptionSink *xsink)
 {
    if (!ptr)
       return;
    
    if (ptr->node)
       ptr->node->deref(xsink);
    ptr->node = val;
 }
 */

class HashMember *HashIterator::next() 
{ 
   if (ptr) 
      ptr = ptr->next;
   else
      ptr = h->member_list;
   return ptr;
}

const char *HashIterator::getKey() const
{ 
   if (!ptr)
      return NULL;
   
   return ptr->key;
}

class QoreNode *HashIterator::getValue() const
{
   if (ptr)
      return ptr->node;
   return NULL;
}

class QoreNode *HashIterator::takeValueAndDelete()
{
   class QoreNode *rv;
   if (ptr)
   { 
      rv = ptr->node;
      ptr->node = NULL;
      class HashMember *w = ptr;
      ptr = ptr->prev;
      h->internDeleteKey(w);
   }
   else
      rv = NULL;
   return rv;
}

class QoreNode **HashIterator::getValuePtr() const
{
   if (ptr)
      return &(ptr->node);
   return NULL;
}

bool HashIterator::last() const 
{ 
   return (bool)(ptr ? !ptr->next : false); 
} 

bool HashIterator::first() const 
{ 
   return (bool)(ptr ? !ptr->prev : false); 
} 

const char *QoreHash::getFirstKey() const 
{ 
   return member_list ? member_list->key :NULL; 
}

const char *QoreHash::getLastKey() const 
{
   return tail ? tail->key : NULL; 
}

// NOTE: does not delete the value, this must be done by the called before this call
void QoreHash::internDeleteKey(class HashMember *om)
{
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
}

// this function should only be called when the key doesn't exist
class QoreNode **QoreHash::newKeyValue(const char *key, class QoreNode *value)
{
   assert(key);

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

   hm[om->key] = om;

   return &om->node;
}

class QoreNode **QoreHash::getKeyValuePtr(const QoreString *key, class ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
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

void QoreHash::deleteKey(const QoreString *key, ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
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

class QoreNode *QoreHash::takeKeyValue(const QoreString *key, ExceptionSink *xsink)
{
   class QoreNode *rv;
   if (key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      rv = takeKeyValue(ns->getBuffer());
      delete ns;
   }
   else
      rv = takeKeyValue(key->getBuffer());
   return rv;
}

class QoreNode *QoreHash::getKeyValueExistence(const QoreString *key, class ExceptionSink *xsink) const
{
   if (key->getEncoding() != QCS_DEFAULT)
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

void QoreHash::setKeyValue(const QoreString *key, class QoreNode *value, ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
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

void QoreHash::setKeyValue(const char *key, class QoreNode *value, ExceptionSink *xsink)
{
   assert(key);
   class QoreNode **v = getKeyValuePtr(key);
   //printd(5, "Hash::setKeyValue(%s, %08p) v=%08p *v=%08p\n", key, value, v, *v);
   if (*v)
      (*v)->deref(xsink);
   *v = value;
}

class QoreNode **QoreHash::getExistingValuePtr(const QoreString *key, class ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
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

class QoreNode *QoreHash::getKeyValue(const QoreString *key, class ExceptionSink *xsink) const
{
   if (key->getEncoding() != QCS_DEFAULT)
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

// retrieve keys in order they were inserted
class QoreList *QoreHash::getKeys() const
{
   class QoreList *list = new QoreList();
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
void QoreHash::merge(const class QoreHash *h, class ExceptionSink *xsink)
{
   class HashMember *where = h->member_list;
   
   while (where)
   {
      setKeyValue(where->key, where->node ? where->node->RefSelf() : NULL, xsink);
      where = where->next;
   }
}

// adds all elements (already referenecd) from the hash passed, deletes the
// hash passed; order is maintained
void QoreHash::assimilate(class QoreHash *h, ExceptionSink *xsink)
{
   class HashMember *where = h->member_list;
   
   while (where)
   {
      setKeyValue(where->key, where->node, xsink);
      where->node = NULL;
      class HashMember *n = where->next;
      free(where->key);
      delete where;
      where = n;
   }
#ifdef DEBUG
   h->member_list = NULL;
#endif
   delete h;
}

// can only be used on hashes populated with parsed data - no objects can be present
// returns the same order
class QoreHash *QoreHash::copy() const
{
   QoreHash *h = new QoreHash();

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
class QoreHash *QoreHash::eval(ExceptionSink *xsink) const
{
   class QoreHash *h = new QoreHash();

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

class QoreNode *QoreHash::evalFirstKeyValue(class ExceptionSink *xsink) const
{
   if (!member_list || !member_list->node)
      return NULL;
   
   return member_list->node->eval(xsink);
}

// hashes should always be empty by the time they are deleted 
// because object destructors need to be run...
QoreHash::~QoreHash()
{
   assert(!member_list);
}

QoreHash::QoreHash(bool ne) 
{ 
   needs_eval = ne; 
   member_list = NULL; 
   tail = NULL; 
}


class QoreNode *QoreHash::evalKey(const char *key, class ExceptionSink *xsink) const
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end() && i->second->node)
      return i->second->node->eval(xsink);

   return NULL;
}

class QoreNode *QoreHash::evalKeyExistence(const char *key, class ExceptionSink *xsink) const
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end())
   {
      if (i->second->node)
	 return i->second->node->eval(xsink);
      
      return NULL;
   }
   return (QoreNode *)-1;
}

class QoreNode **QoreHash::getKeyValuePtr(const char *key)
{
   assert(key);

   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end())
      return &i->second->node;

   return newKeyValue(key, NULL);
}

class QoreNode *QoreHash::getKeyValue(const char *key) const
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end())
      return i->second->node;

   return NULL;
}

class QoreNode *QoreHash::getKeyValueExistence(const char *key) const
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end())
      return i->second->node;

   return (QoreNode *)-1;
}

// does a "soft" compare (values of different types are converted if necessary and then compared)
// 0 = equal, 1 = not equal
bool QoreHash::compareSoft(class QoreHash *h, class ExceptionSink *xsink) const
{
   if (h->hm.size() != hm.size())
      return 1;

   for (hm_hm_t::const_iterator i = hm.begin(); i != hm.end(); i++)
   {
      hm_hm_t::const_iterator j = h->hm.find(i->first);
      if (j == h->hm.end())
	 return 1;

      if (::compareSoft(i->second->node, j->second->node, xsink))
	 return 1;
   }

   return 0;
}

// does a "hard" compare (types must be exactly the same)
// 0 = equal, 1 = not equal
bool QoreHash::compareHard(class QoreHash *h, class ExceptionSink *xsink) const
{
   if (h->hm.size() != hm.size())
      return 1;

   for (hm_hm_t::const_iterator i = hm.begin(); i != hm.end(); i++)
   {
      hm_hm_t::const_iterator j = h->hm.find(i->first);
      if (j == h->hm.end())
	 return 1;

      if (::compareHard(i->second->node, j->second->node, xsink))
	 return 1;
   }

   return 0;
}

class QoreNode **QoreHash::getExistingValuePtr(const char *key)
{
   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end())
      return &i->second->node;
   
   return NULL;
}

inline void QoreHash::deref_intern(class ExceptionSink *xsink)
{
   class HashMember *where = member_list;
   while (where)
   {
#if 0
      printd(5, "Hash::deref_intern() %s=%08p type=%s references=%d\n",
	     where->key ? where->key : "(null)",
	     where->node,
	     where->node ? where->node->type->getName() : "(null)",
	     where->node ? where->node->reference_count() : 0);
#endif
      class HashMember *om = where;
      if (where->node)
	 where->node->deref(xsink);
      where = where->next;
      if (om->key)
	 free(om->key);
      delete om;
   }
}

void QoreHash::dereference(class ExceptionSink *xsink)
{
   deref_intern(xsink);
   member_list = NULL;
   tail = NULL;
   hm.clear();
}

void QoreHash::derefAndDelete(class ExceptionSink *xsink)
{
   deref_intern(xsink);
#ifdef DEBUG
   member_list = NULL;
#endif
   delete this;
}

void QoreHash::deleteKey(const char *key, ExceptionSink *xsink)
{
   assert(key);

   hm_hm_t::iterator i = hm.find(key);

   if (i == hm.end())
      return;

   class HashMember *m = i->second;

   hm.erase(i);

   // dereference node if present
   if (m->node)
   {
      if (m->node->type == NT_OBJECT)
         m->node->val.object->doDelete(xsink);
      m->node->deref(xsink);
   }

   internDeleteKey(m);
}

class QoreNode *QoreHash::takeKeyValue(const char *key)
{
   assert(key);

   hm_hm_t::iterator i = hm.find(key);

   if (i == hm.end())
      return NULL;

   class HashMember *m = i->second;

   hm.erase(i);

   class QoreNode *rv = m->node;

   internDeleteKey(m);

   return rv;
}

int QoreHash::size() const 
{ 
   return hm.size(); 
}

bool QoreHash::needsEval() const
{
   return needs_eval;
}

void QoreHash::clearNeedsEval()
{
   needs_eval = false;
}

