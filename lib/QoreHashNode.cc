/*
  QoreHashNode.cc

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
#include <stdio.h>

#ifdef DEBUG_TESTS
#  include "tests/Hash_tests.cc"
#endif

const char *qore_hash_type_name = "hash";

// FIXME: use STL list instead
// to maintain the order of inserts
class HashMember {
   public:
      class AbstractQoreNode *node;
      char *key;
      class HashMember *next;
      class HashMember *prev;
};

QoreHashNode::QoreHashNode(bool ne) : AbstractQoreNode(NT_HASH)
{ 
   needs_eval_flag = ne; 
   member_list = NULL; 
   tail = NULL; 
   len = 0;
}

QoreHashNode::QoreHashNode() : AbstractQoreNode(NT_HASH)
{ 
   needs_eval_flag = false; 
   member_list = NULL; 
   tail = NULL; 
   len = 0;
}

// hashes should always be empty by the time they are deleted 
// because object destructors need to be run...
QoreHashNode::~QoreHashNode()
{
   assert(!member_list);
}

AbstractQoreNode *QoreHashNode::realCopy() const
{
   return copy();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreHashNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   if (!v || v->getType() != NT_HASH)
      return false;

   return !compareSoft(reinterpret_cast<const QoreHashNode *>(v), xsink);
}

bool QoreHashNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   if (!v || v->getType() != NT_HASH)
      return false;

   return !compareHard(reinterpret_cast<const QoreHashNode *>(v), xsink);
}

// returns the data type
const QoreType *QoreHashNode::getType() const
{
   return NT_HASH;
}

const char *QoreHashNode::getTypeName() const
{
   return qore_hash_type_name;
}


const char *QoreHashNode::getFirstKey() const 
{ 
   return member_list ? member_list->key :NULL; 
}

const char *QoreHashNode::getLastKey() const 
{
   return tail ? tail->key : NULL; 
}

// NOTE: does not delete the value, this must be done by the called before this call
void QoreHashNode::internDeleteKey(class HashMember *om)
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
   --len;
}

// this function should only be called when the key doesn't exist
AbstractQoreNode **QoreHashNode::newKeyValue(const char *key, AbstractQoreNode *value)
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

   ++len;
   return &om->node;
}

AbstractQoreNode **QoreHashNode::getKeyValuePtr(const QoreString *key, ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      AbstractQoreNode **rv = getKeyValuePtr(ns->getBuffer());
      delete ns;
      return rv;
   }
   return getKeyValuePtr(key->getBuffer());
}

void QoreHashNode::deleteKey(const QoreString *key, ExceptionSink *xsink)
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

AbstractQoreNode *QoreHashNode::takeKeyValue(const QoreString *key, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
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

AbstractQoreNode *QoreHashNode::getKeyValueExistence(const QoreString *key, bool &exists, ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      AbstractQoreNode *rv = getKeyValueExistence(ns->getBuffer(), exists);
      delete ns;
      return rv;
   }
   return getKeyValueExistence(key->getBuffer(), exists);
}

const AbstractQoreNode *QoreHashNode::getKeyValueExistence(const QoreString *key, bool &exists, ExceptionSink *xsink) const
{
   return const_cast<QoreHashNode *>(this)->getKeyValueExistence(key, exists, xsink);
}

void QoreHashNode::setKeyValue(const QoreString *key, AbstractQoreNode *value, ExceptionSink *xsink)
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

void QoreHashNode::setKeyValue(const char *key, AbstractQoreNode *value, ExceptionSink *xsink)
{
   assert(key);
   AbstractQoreNode **v = getKeyValuePtr(key);
   //printd(5, "QoreHashNode::setKeyValue(%s, %08p) v=%08p *v=%08p\n", key, value, v, *v);
   if (*v)
      (*v)->deref(xsink);
   *v = value;
}

AbstractQoreNode **QoreHashNode::getExistingValuePtr(const QoreString *key, ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      AbstractQoreNode **rv = getExistingValuePtr(ns->getBuffer());
      delete ns;
      return rv;
   }
   return getExistingValuePtr(key->getBuffer());
}

AbstractQoreNode *QoreHashNode::getKeyValue(const QoreString *key, ExceptionSink *xsink)
{
   if (key->getEncoding() != QCS_DEFAULT)
   {
      QoreString *ns = key->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
      AbstractQoreNode *rv = getKeyValue(ns->getBuffer());
      delete ns;
      return rv;
   }
   return getKeyValue(key->getBuffer());
}

const AbstractQoreNode *QoreHashNode::getKeyValue(const QoreString *key, ExceptionSink *xsink) const
{
   return const_cast<QoreHashNode *>(this)->getKeyValue(key, xsink);
}

// retrieve keys in order they were inserted
QoreListNode *QoreHashNode::getKeys() const
{
   QoreListNode *list = new QoreListNode();
   class HashMember *where = member_list;
   
   while (where)
   {
      list->push(new QoreStringNode(where->key));
      where = where->next;
   }
   return list;
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
// order is maintained
void QoreHashNode::merge(const class QoreHashNode *h, ExceptionSink *xsink)
{
   class HashMember *where = h->member_list;
   
   while (where)
   {
      setKeyValue(where->key, where->node ? where->node->refSelf() : NULL, xsink);
      where = where->next;
   }
}

// returns the same order
QoreHashNode *QoreHashNode::copy() const
{
   QoreHashNode *h = new QoreHashNode();

   // copy all members to new object
   class HashMember *where = member_list;
   while (where)
   {
      h->setKeyValue(where->key, where->node ? where->node->refSelf() : 0, 0);
      where = where->next;
   }
   return h;
}

QoreHashNode *QoreHashNode::hashRefSelf() const
{
   ref();
   return const_cast<QoreHashNode *>(this);
}

// returns a hash with the same order
AbstractQoreNode *QoreHashNode::eval(ExceptionSink *xsink) const
{
   if (!needs_eval_flag)
      return hashRefSelf();

   QoreHashNodeHolder h(new QoreHashNode(), xsink);

   HashMember *where = member_list;
   while (where) {
      h->setKeyValue(where->key, where->node ? where->node->eval(xsink) : 0, xsink);
      if (*xsink)
	 return 0;
      where = where->next;
   }

   return h.release();
}

AbstractQoreNode *QoreHashNode::eval(bool &needs_deref, ExceptionSink *xsink) const
{
   if (!needs_eval_flag) {
      needs_deref = false;
      return const_cast<QoreHashNode *>(this);
   }

   needs_deref = true;
   AbstractQoreNode *rv = eval(xsink);
   if (rv)
      return rv;

   needs_deref = false;
   return 0;
}

bool QoreHashNode::is_value() const
{
   return !needs_eval_flag;
}

AbstractQoreNode *QoreHashNode::getFirstKeyValue() const
{
   return member_list && member_list->node ? member_list->node : 0;
}

AbstractQoreNode *QoreHashNode::evalKeyValue(const QoreString *key, class ExceptionSink *xsink) const
{
   TempEncodingHelper k(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   hm_hm_t::const_iterator i = hm.find(k->getBuffer());

   if (i != hm.end() && i->second->node)
      return i->second->node->refSelf();

   return 0;
}

AbstractQoreNode *QoreHashNode::getReferencedKeyValue(const char *key) const
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end() && i->second->node)
      return i->second->node->refSelf();

   return 0;
}

AbstractQoreNode *QoreHashNode::getReferencedKeyValue(const char *key, bool &exists) const
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end())
   {
      exists = true;
      if (i->second->node)
	 return i->second->node->refSelf();
      
      return 0;
   }
   exists = false;
   return 0;
}

AbstractQoreNode **QoreHashNode::getKeyValuePtr(const char *key)
{
   assert(key);

   hm_hm_t::iterator i = hm.find(key);

   if (i != hm.end())
      return &i->second->node;

   return newKeyValue(key, NULL);
}

AbstractQoreNode *QoreHashNode::getKeyValue(const char *key)
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end())
      return i->second->node;

   return NULL;
}

const AbstractQoreNode *QoreHashNode::getKeyValue(const char *key) const
{
   return const_cast<QoreHashNode *>(this)->getKeyValue(key);
}

AbstractQoreNode *QoreHashNode::getKeyValueExistence(const char *key, bool &exists)
{
   assert(key);

   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end()) {
      exists = true;
      return i->second->node;
   }

   exists = false;
   return 0;
}

const AbstractQoreNode *QoreHashNode::getKeyValueExistence(const char *key, bool &exists) const
{
   return const_cast<QoreHashNode *>(this)->getKeyValueExistence(key, exists);
}

// does a "soft" compare (values of different types are converted if necessary and then compared)
// 0 = equal, 1 = not equal
bool QoreHashNode::compareSoft(const QoreHashNode *h, ExceptionSink *xsink) const
{
   if (h->len != len)
      return 1;

   ConstHashIterator hi(this);
   while (hi.next()) {
      hm_hm_t::const_iterator j = h->hm.find(hi.getKey());
      if (j == h->hm.end())
	 return 1;

      if (::compareSoft(hi.getValue(), j->second->node, xsink))
	 return 1;
   }
/*
   for (hm_hm_t::const_iterator i = hm.begin(); i != hm.end(); i++)
   {
      hm_hm_t::const_iterator j = h->hm.find(i->first);
      if (j == h->hm.end())
	 return 1;

      if (::compareSoft(i->second->node, j->second->node, xsink))
	 return 1;
   }
*/
   return 0;
}

// does a "hard" compare (types must be exactly the same)
// 0 = equal, 1 = not equal
bool QoreHashNode::compareHard(const QoreHashNode *h, ExceptionSink *xsink) const
{
   if (h->len != len)
      return 1;

   ConstHashIterator hi(this);
   while (hi.next()) {
      hm_hm_t::const_iterator j = h->hm.find(hi.getKey());
      if (j == h->hm.end())
	 return 1;

      if (::compareHard(hi.getValue(), j->second->node, xsink))
	 return 1;
   }
/*
   for (hm_hm_t::const_iterator i = hm.begin(); i != hm.end(); i++)
   {
      hm_hm_t::const_iterator j = h->hm.find(i->first);
      if (j == h->hm.end())
	 return 1;

      if (::compareHard(i->second->node, j->second->node, xsink))
	 return 1;
   }
*/
   return 0;
}

AbstractQoreNode **QoreHashNode::getExistingValuePtr(const char *key)
{
   hm_hm_t::const_iterator i = hm.find(key);

   if (i != hm.end())
      return &i->second->node;
   
   return NULL;
}

bool QoreHashNode::derefImpl(ExceptionSink *xsink)
{
   //printd(5, "QoreHashNode::derefImpl() this=%08p member_list=%08p\n", this, member_list);
   class HashMember *where = member_list;
   while (where)
   {
#if 0
      printd(5, "QoreHashNode::derefImpl() this=%08p %s=%08p type=%s references=%d\n", this,
	     where->key ? where->key : "(null)",
	     where->node, where->node ? where->node->getTypeName() : "(null)",
	     where->node ? where->node->reference_count() : 0);
#endif
      if (where->node)
	 where->node->deref(xsink);
      HashMember *om = where;
      where = where->next;
      if (om->key)
	 free(om->key);
      delete om;
   }
#ifdef DEBUG
   member_list = 0;
#endif
   return true;
}

void QoreHashNode::clear(ExceptionSink *xsink)
{
   assert(is_unique());
   derefImpl(xsink);
   member_list = 0;
   tail = 0;
   hm.clear();
}

void QoreHashNode::deleteKey(const char *key, ExceptionSink *xsink)
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
      if (m->node->getType() == NT_OBJECT)
	 reinterpret_cast<QoreObject *>(m->node)->doDelete(xsink);
      m->node->deref(xsink);
   }

   internDeleteKey(m);
}

AbstractQoreNode *QoreHashNode::takeKeyValue(const char *key)
{
   assert(key);

   hm_hm_t::iterator i = hm.find(key);

   if (i == hm.end())
      return NULL;

   class HashMember *m = i->second;

   hm.erase(i);

   AbstractQoreNode *rv = m->node;

   internDeleteKey(m);

   return rv;
}

int QoreHashNode::size() const 
{ 
   return len; 
}

bool QoreHashNode::needs_eval() const
{
   return needs_eval_flag;
}

void QoreHashNode::clearNeedsEval()
{
   needs_eval_flag = false;
}

int QoreHashNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   if (!size()) {
      str.concat(&EmptyHashString);
      return 0;
   }
   str.concat("hash: ");
   if (foff != FMT_NONE) {
      int elements = size();
      str.sprintf("(%d member%s)\n", elements, elements == 1 ? "" : "s");
   }
   else
      str.concat('(');
   
   class ConstHashIterator hi(this);
   
   bool first = false;
   while (hi.next()) {
      if (first)
	 if (foff != FMT_NONE)
	    str.concat('\n');
	 else
	    str.concat(", ");
      else
	 first = true;
      
      if (foff != FMT_NONE)
	 str.addch(' ', foff + 2);

      str.sprintf("%s : ", hi.getKey());

      const AbstractQoreNode *n = hi.getValue();
      if (!n) n = &Nothing;
      if (n->getAsString(str, foff != FMT_NONE ? foff + 2 : foff, xsink))
	 return -1;
   }
   
   if (foff == FMT_NONE)
      str.concat(')');

   return 0;
}

QoreString *QoreHashNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = false;
   int elements = size();
   if (!elements)
      return &EmptyHashString;

   TempString rv(new QoreString());
   rv->concat("hash: ");
   if (foff != FMT_NONE)
      rv->sprintf("(%d member%s)\n", elements, elements == 1 ? "" : "s");
   else
      rv->concat('(');
   
   class ConstHashIterator hi(this);
   
   bool first = false;
   while (hi.next())
   {
      if (first)
	 if (foff != FMT_NONE)
	    rv->concat('\n');
	 else
	    rv->concat(", ");
      else
	 first = true;
      
      if (foff != FMT_NONE)
	 rv->addch(' ', foff + 2);
      
      QoreNodeAsStringHelper elem(hi.getValue(), foff != FMT_NONE ? foff + 2 : foff, xsink);
      if (*xsink)
	 return 0;
      rv->sprintf("%s : %s", hi.getKey(), elem->getBuffer());
   }
   
   if (foff == FMT_NONE)
      rv->concat(')');

   del = true;
   return rv.release();
}

HashIterator::HashIterator(class QoreHashNode *qh) 
{
   h = qh;
   ptr = NULL;
}

HashIterator::HashIterator(class QoreHashNode &qh) 
{
   h = &qh;
   ptr = NULL;
}

AbstractQoreNode *HashIterator::getReferencedValue() const
{
   return ptr && ptr->node ? ptr->node->refSelf() : 0;
}

class QoreString *HashIterator::getKeyString() const
{
   if (!ptr)
      return NULL;
   
   return new QoreString(ptr->key, QCS_DEFAULT);
}

/*
 void HashIterator::setValue(AbstractQoreNode *val, ExceptionSink *xsink)
 {
    if (!ptr)
       return;
    
    if (ptr->node)
       ptr->node->deref(xsink);
    ptr->node = val;
 }
 */

bool HashIterator::next() 
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

AbstractQoreNode *HashIterator::getValue() const
{
   if (ptr)
      return ptr->node;
   return NULL;
}

AbstractQoreNode *HashIterator::takeValueAndDelete()
{
   AbstractQoreNode *rv;
   if (ptr)
   { 
      rv = ptr->node;
      ptr->node = NULL;
      class HashMember *w = ptr;
      ptr = ptr->prev;

      // remove key from map before deleting hash member with key pointer
      hm_hm_t::iterator i = h->hm.find(w->key);
      assert(i != h->hm.end());
      h->hm.erase(i);

      h->internDeleteKey(w);
   }
   else
      rv = NULL;
   return rv;
}

void HashIterator::deleteKey(ExceptionSink *xsink)
{
   if (!ptr)
      return;

   ptr->node->deref(xsink);
   ptr->node = 0;
   class HashMember *w = ptr;
   ptr = ptr->prev;
   
   // remove key from map before deleting hash member with key pointer
   hm_hm_t::iterator i = h->hm.find(w->key);
   assert(i != h->hm.end());
   h->hm.erase(i);

   h->internDeleteKey(w);
}

AbstractQoreNode **HashIterator::getValuePtr() const
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

ConstHashIterator::ConstHashIterator(const QoreHashNode *qh) 
{
   h = qh;
   ptr = NULL;
}

ConstHashIterator::ConstHashIterator(const QoreHashNode &qh) 
{
   h = &qh;
   ptr = NULL;
}

AbstractQoreNode *ConstHashIterator::getReferencedValue() const
{
   return ptr && ptr->node ? ptr->node->refSelf() : 0;
}

QoreString *ConstHashIterator::getKeyString() const
{
   if (!ptr)
      return NULL;
   
   return new QoreString(ptr->key, QCS_DEFAULT);
}

bool ConstHashIterator::next() 
{ 
   if (ptr) 
      ptr = ptr->next;
   else
      ptr = h->member_list;
   return ptr;
}

const char *ConstHashIterator::getKey() const
{ 
   if (!ptr)
      return NULL;
   
   return ptr->key;
}

const AbstractQoreNode *ConstHashIterator::getValue() const
{
   if (ptr)
      return ptr->node;
   return NULL;
}

bool ConstHashIterator::last() const 
{ 
   return (bool)(ptr ? !ptr->next : false); 
} 

bool ConstHashIterator::first() const 
{ 
   return (bool)(ptr ? !ptr->prev : false); 
} 
