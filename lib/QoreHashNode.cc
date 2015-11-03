/*
  QoreHashNode.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

static const char *qore_hash_type_name = "hash";

// FIXME: use STL list instead
// to maintain the order of inserts
class HashMember {
   public:
      AbstractQoreNode *node;
      char *key;
      class HashMember *next;
      class HashMember *prev;
};

struct qore_hash_private {
   class HashMember *member_list;
   class HashMember *tail;
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

   DLLLOCAL AbstractQoreNode **getKeyValuePtr(const char *key) {
      assert(key);	 
      hm_hm_t::iterator i = hm.find(key);
	 
      // if the key already exists, then return the value ptr
      if (i != hm.end())
	 return &i->second->node;

      // otherwise create the new hash entry
      HashMember *om = new HashMember;
      om->node = 0;
      om->next = 0;
      om->prev = tail;
      om->key = strdup(key);
      assert(hm.find(om->key) == hm.end());

      if (tail)
	 tail->next = om;
      else
	 member_list = om;
      tail = om;
	 
      // add to the map
      hm[om->key] = om;

      // increment the hash size
      ++len;

      // return the new value ptr
      return &om->node;
   }

   // NOTE: does not delete the value, this must be done by the called before this call
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

      // free string memory
      free(om->key);

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

QoreHashNode::QoreHashNode(bool ne) : AbstractQoreNode(NT_HASH, !ne, ne), priv(new qore_hash_private) {
}

QoreHashNode::QoreHashNode() : AbstractQoreNode(NT_HASH, true, false), priv(new qore_hash_private) { 
}

QoreHashNode::~QoreHashNode() {
   delete priv;
}

AbstractQoreNode *QoreHashNode::realCopy() const {
   return copy();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreHashNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   if (!v || v->getType() != NT_HASH)
      return false;

   return !compareSoft(reinterpret_cast<const QoreHashNode *>(v), xsink);
}

bool QoreHashNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   if (!v || v->getType() != NT_HASH)
      return false;

   return !compareHard(reinterpret_cast<const QoreHashNode *>(v), xsink);
}

const char *QoreHashNode::getTypeName() const {
   return qore_hash_type_name;
}

const char *QoreHashNode::getFirstKey() const  { 
   return priv->member_list ? priv->member_list->key :0; 
}

const char *QoreHashNode::getLastKey() const {
   return priv->tail ? priv->tail->key : 0; 
}

AbstractQoreNode **QoreHashNode::getKeyValuePtr(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;
   
   return priv->getKeyValuePtr(tmp->getBuffer());
}

AbstractQoreNode **QoreHashNode::getKeyValuePtr(const char *key) {
   return priv->getKeyValuePtr(key);
}

int64 QoreHashNode::getKeyAsBigInt(const char *key, bool &found) const {
   return priv->getKeyAsBigInt(key, found);
}

void QoreHashNode::deleteKey(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   priv->deleteKey(tmp->getBuffer(), xsink);
}

void QoreHashNode::removeKey(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   priv->removeKey(tmp->getBuffer(), xsink);
}

AbstractQoreNode *QoreHashNode::takeKeyValue(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return priv->takeKeyValue(tmp->getBuffer());
}

AbstractQoreNode *QoreHashNode::getKeyValueExistence(const QoreString *key, bool &exists, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return getKeyValueExistence(tmp->getBuffer(), exists);
}

const AbstractQoreNode *QoreHashNode::getKeyValueExistence(const QoreString *key, bool &exists, ExceptionSink *xsink) const {
   return const_cast<QoreHashNode *>(this)->getKeyValueExistence(key, exists, xsink);
}

void QoreHashNode::setKeyValue(const QoreString *key, AbstractQoreNode *val, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   setKeyValue(tmp->getBuffer(), val, xsink);
}

void QoreHashNode::setKeyValue(const char *key, AbstractQoreNode *val, ExceptionSink *xsink) {
   AbstractQoreNode **v = priv->getKeyValuePtr(key);
   //printd(5, "QoreHashNode::setKeyValue(%s, %08p) v=%08p *v=%08p\n", key, val, v, *v);
   if (*v)
      (*v)->deref(xsink);
   *v = val;
}

AbstractQoreNode *QoreHashNode::swapKeyValue(const QoreString *key, AbstractQoreNode *val, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return swapKeyValue(tmp->getBuffer(), val);
}

AbstractQoreNode *QoreHashNode::swapKeyValue(const char *key, AbstractQoreNode *val) {
   AbstractQoreNode **v = priv->getKeyValuePtr(key);
   //printd(5, "QoreHashNode::setKeyValue(%s, %08p) v=%08p *v=%08p\n", key, val, v, *v);
   AbstractQoreNode *rv = *v;
   *v = val;
   return rv;
}

AbstractQoreNode **QoreHashNode::getExistingValuePtr(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return getExistingValuePtr(tmp->getBuffer());
}

AbstractQoreNode *QoreHashNode::getKeyValue(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return getKeyValue(tmp->getBuffer());
}

const AbstractQoreNode *QoreHashNode::getKeyValue(const QoreString *key, ExceptionSink *xsink) const {
   return const_cast<QoreHashNode *>(this)->getKeyValue(key, xsink);
}

// retrieve keys in order they were inserted
QoreListNode *QoreHashNode::getKeys() const {
   QoreListNode *list = new QoreListNode();
   HashMember *where = priv->member_list;
   
   while (where) {
      list->push(new QoreStringNode(where->key));
      where = where->next;
   }
   return list;
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
// order is maintained
void QoreHashNode::merge(const class QoreHashNode *h, ExceptionSink *xsink) {
   HashMember *where = h->priv->member_list;
   
   while (where) {
      setKeyValue(where->key, where->node ? where->node->refSelf() : 0, xsink);
      where = where->next;
   }
}

// returns the same order
QoreHashNode *QoreHashNode::copy() const {
   QoreHashNode *h = new QoreHashNode();

   // copy all members to new object
   class HashMember *where = priv->member_list;
   while (where) {
      //printd(5, "QoreHashNode::copy() this=%p node=%p key='%s'\n", this, where->node, where->key);
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
AbstractQoreNode *QoreHashNode::evalImpl(ExceptionSink *xsink) const
{
   QoreHashNodeHolder h(new QoreHashNode(), xsink);

   HashMember *where = priv->member_list;
   while (where) {
      h->setKeyValue(where->key, where->node ? where->node->eval(xsink) : 0, xsink);
      if (*xsink)
	 return 0;
      where = where->next;
   }

   return h.release();
}

// returns a hash with the same order
AbstractQoreNode *QoreHashNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   if (value) {
      needs_deref = false;
      return const_cast<QoreHashNode *>(this);
   }

   needs_deref = true;
   return QoreHashNode::evalImpl(xsink);
}

int64 QoreHashNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

int QoreHashNode::integerEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

bool QoreHashNode::boolEvalImpl(ExceptionSink *xsink) const {
   return false;
}

double QoreHashNode::floatEvalImpl(ExceptionSink *xsink) const {
   return 0.0;
}

AbstractQoreNode *QoreHashNode::getFirstKeyValue() const {
   return priv->member_list && priv->member_list->node ? priv->member_list->node : 0;
}

AbstractQoreNode *QoreHashNode::evalKeyValue(const QoreString *key, class ExceptionSink *xsink) const {
   TempEncodingHelper k(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   hm_hm_t::const_iterator i = priv->hm.find(k->getBuffer());

   if (i != priv->hm.end() && i->second->node)
      return i->second->node->refSelf();

   return 0;
}

AbstractQoreNode *QoreHashNode::getReferencedKeyValue(const char *key) const
{
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end() && i->second->node)
      return i->second->node->refSelf();

   return 0;
}

AbstractQoreNode *QoreHashNode::getReferencedKeyValue(const char *key, bool &exists) const
{
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end())
   {
      exists = true;
      if (i->second->node)
	 return i->second->node->refSelf();
      
      return 0;
   }
   exists = false;
   return 0;
}

AbstractQoreNode *QoreHashNode::getKeyValue(const char *key)
{
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end())
      return i->second->node;

   return 0;
}

const AbstractQoreNode *QoreHashNode::getKeyValue(const char *key) const
{
   return const_cast<QoreHashNode *>(this)->getKeyValue(key);
}

AbstractQoreNode *QoreHashNode::getKeyValueExistence(const char *key, bool &exists)
{
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end()) {
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
   if (h->priv->len != priv->len)
      return 1;

   ConstHashIterator hi(this);
   while (hi.next()) {
      hm_hm_t::const_iterator j = h->priv->hm.find(hi.getKey());
      if (j == h->priv->hm.end())
	 return 1;

      if (::compareSoft(hi.getValue(), j->second->node, xsink))
	 return 1;
   }
/*
   for (hm_hm_t::const_iterator i = priv->hm.begin(); i != priv->hm.end(); i++)
   {
      hm_hm_t::const_iterator j = h->priv->hm.find(i->first);
      if (j == h->priv->hm.end())
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
   if (h->priv->len != priv->len)
      return 1;

   ConstHashIterator hi(this);
   while (hi.next()) {
      hm_hm_t::const_iterator j = h->priv->hm.find(hi.getKey());
      if (j == h->priv->hm.end())
	 return 1;

      if (::compareHard(hi.getValue(), j->second->node, xsink))
	 return 1;
   }
/*
   for (hm_hm_t::const_iterator i = priv->hm.begin(); i != priv->hm.end(); i++)
   {
      hm_hm_t::const_iterator j = h->priv->hm.find(i->first);
      if (j == h->priv->hm.end())
	 return 1;

      if (::compareHard(i->second->node, j->second->node, xsink))
	 return 1;
   }
*/
   return 0;
}

AbstractQoreNode **QoreHashNode::getExistingValuePtr(const char *key)
{
   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end())
      return &i->second->node;
   
   return 0;
}

bool QoreHashNode::derefImpl(ExceptionSink *xsink)
{
   //printd(5, "QoreHashNode::derefImpl() this=%08p priv->member_list=%08p\n", this, priv->member_list);
   class HashMember *where = priv->member_list;
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
   priv->member_list = 0;
#endif
   return true;
}

void QoreHashNode::clear(ExceptionSink *xsink) {
   assert(is_unique());
   derefImpl(xsink);
   priv->member_list = 0;
   priv->tail = 0;
   priv->hm.clear();
}

void QoreHashNode::deleteKey(const char *key, ExceptionSink *xsink) {
   priv->deleteKey(key, xsink);
}

void QoreHashNode::removeKey(const char *key, ExceptionSink *xsink) {
   return priv->removeKey(key, xsink);
}

AbstractQoreNode *QoreHashNode::takeKeyValue(const char *key) {
   return priv->takeKeyValue(key);
}

qore_size_t QoreHashNode::size() const { 
   return priv->len; 
}

bool QoreHashNode::empty() const {
   return !priv->len;
}

void QoreHashNode::clearNeedsEval() {
   value = true;
   needs_eval_flag = false;
}

void QoreHashNode::setNeedsEval() {
   value = false;
   needs_eval_flag = true;
}

int QoreHashNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   if (!size()) {
      str.concat(&EmptyHashString);
      return 0;
   }
   str.concat("hash: ");

   QoreContainerHelper cch(this);
   if (!cch) {
      str.concat("(ERROR: recursive reference)");
      return 0;
   }

   str.concat('(');
   if (foff != FMT_NONE) {
      qore_size_t elements = size();
      str.sprintf("%lu member%s)\n", elements, elements == 1 ? "" : "s");
   }
   
   ConstHashIterator hi(this);
   
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

QoreString *QoreHashNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   qore_size_t elements = size();
   if (!elements)
      return &EmptyHashString;

   TempString rv(new QoreString());
   rv->concat("hash: ");
   if (foff != FMT_NONE)
      rv->sprintf("(%lu member%s)\n", elements, elements == 1 ? "" : "s");
   else
      rv->concat('(');

   ConstHashIterator hi(this);

   bool first = false;
   while (hi.next()) {
      //printd(5, "QoreHashNode::getAsString() h=%p key=%s v=%p\n", this, hi.getKey(), hi.getValue());

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

QoreHashNode *QoreHashNode::getSlice(const QoreListNode *value_list, ExceptionSink *xsink) const {
   ReferenceHolder<QoreHashNode> rv(new QoreHashNode(), xsink);

   ConstListIterator li(value_list);
   while (li.next()) {
      QoreStringValueHelper key(li.getValue(), QCS_DEFAULT, xsink);
      if (*xsink)
	 return 0;

      bool exists;
      const AbstractQoreNode *v = getKeyValueExistence(key->getBuffer(), exists);
      if (!exists)
	 continue;
      rv->setKeyValue(key->getBuffer(), v ? v->refSelf() : 0, xsink);
      if (*xsink)
	 return 0;
   }
   return rv.release();
}

HashIterator::HashIterator(QoreHashNode *qh) {
   h = qh;
   ptr = 0;
}

HashIterator::HashIterator(QoreHashNode &qh) {
   h = &qh;
   ptr = 0;
}

AbstractQoreNode *HashIterator::getReferencedValue() const {
   return ptr && ptr->node ? ptr->node->refSelf() : 0;
}

QoreString *HashIterator::getKeyString() const {
   if (!ptr)
      return 0;
   
   return new QoreString(ptr->key, QCS_DEFAULT);
}

/*
void HashIterator::setValue(AbstractQoreNode *val, ExceptionSink *xsink) {
    if (!ptr)
       return;
    
    if (ptr->node)
       ptr->node->deref(xsink);
    ptr->node = val;
}
*/

bool HashIterator::next() { 
   if (ptr) 
      ptr = ptr->next;
   else
      ptr = h->priv->member_list;
   return ptr;
}

const char *HashIterator::getKey() const { 
   if (!ptr)
      return 0;
   
   return ptr->key;
}

AbstractQoreNode *HashIterator::getValue() const {
   if (ptr)
      return ptr->node;
   return 0;
}

AbstractQoreNode *HashIterator::takeValueAndDelete() {
   if (ptr) { 
      AbstractQoreNode *rv = ptr->node;
      ptr->node = 0;
      HashMember *w = ptr;
      ptr = ptr->prev;

      // remove key from map before deleting hash member with key pointer
      hm_hm_t::iterator i = h->priv->hm.find(w->key);
      assert(i != h->priv->hm.end());
      h->priv->hm.erase(i);

      h->priv->internDeleteKey(w);
      return rv;
   }

   return 0;
}

void HashIterator::deleteKey(ExceptionSink *xsink) {
   if (!ptr)
      return;

   ptr->node->deref(xsink);
   ptr->node = 0;
   HashMember *w = ptr;
   ptr = ptr->prev;
   
   // remove key from map before deleting hash member with key pointer
   hm_hm_t::iterator i = h->priv->hm.find(w->key);
   assert(i != h->priv->hm.end());
   h->priv->hm.erase(i);

   h->priv->internDeleteKey(w);
}

AbstractQoreNode **HashIterator::getValuePtr() const {
   if (ptr)
      return &(ptr->node);
   return 0;
}

bool HashIterator::last() const { 
   return (bool)(ptr ? !ptr->next : false); 
} 

bool HashIterator::first() const { 
   return (bool)(ptr ? !ptr->prev : false); 
} 

ConstHashIterator::ConstHashIterator(const QoreHashNode *qh) {
   h = qh;
   ptr = 0;
}

ConstHashIterator::ConstHashIterator(const QoreHashNode &qh) {
   h = &qh;
   ptr = 0;
}

AbstractQoreNode *ConstHashIterator::getReferencedValue() const {
   return ptr && ptr->node ? ptr->node->refSelf() : 0;
}

QoreString *ConstHashIterator::getKeyString() const {
   if (!ptr)
      return 0;
   
   return new QoreString(ptr->key, QCS_DEFAULT);
}

bool ConstHashIterator::next() { 
   if (ptr) 
      ptr = ptr->next;
   else
      ptr = h->priv->member_list;
   return ptr;
}

const char *ConstHashIterator::getKey() const { 
   if (!ptr)
      return 0;
   
   return ptr->key;
}

const AbstractQoreNode *ConstHashIterator::getValue() const {
   if (ptr)
      return ptr->node;
   return 0;
}

bool ConstHashIterator::last() const { 
   return (bool)(ptr ? !ptr->next : false); 
} 

bool ConstHashIterator::first() const {
   return (bool)(ptr ? !ptr->prev : false); 
} 
