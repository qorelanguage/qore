/*
  QoreHashNode.h

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

#ifndef _QORE_QOREHASHNODE_H

#define _QORE_QOREHASHNODE_H

#include <qore/AbstractQoreNode.h>
#include <qore/common.h>
#include <qore/hash_map.h>

// FIXME: use STL list instead
// to maintain the order of inserts
class HashMember {
   public:
      class AbstractQoreNode *node;
      char *key;
      class HashMember *next;
      class HashMember *prev;
};

class HashIterator
{
   private:
      class QoreHashNode *h;
      class HashMember *ptr;

      // not implemented
      DLLLOCAL HashIterator(const HashIterator&);
      DLLLOCAL HashIterator& operator=(const HashIterator&);

   public:
      DLLEXPORT HashIterator(class QoreHashNode *h);
      DLLEXPORT HashIterator(class QoreHashNode &h);
      DLLEXPORT class HashMember *next();
      DLLEXPORT const char *getKey() const;
      // caller owns QoreString returned
      DLLEXPORT class QoreString *getKeyString() const;
      DLLEXPORT class AbstractQoreNode *getValue() const;
      // deletes the key from the hash and returns the value, caller owns the reference
      DLLEXPORT class AbstractQoreNode *takeValueAndDelete();
      // deletes the key from the hash and dereferences the value
      DLLEXPORT void deleteKey(class ExceptionSink *xsink);
      DLLEXPORT class AbstractQoreNode **getValuePtr() const;
      DLLEXPORT class AbstractQoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT bool first() const;
      DLLEXPORT bool last() const;
      //DLLEXPORT void setValue(class AbstractQoreNode *val, class ExceptionSink *xsink);
};

class ConstHashIterator
{
   private:
      const QoreHashNode *h;
      class HashMember *ptr;

      // not implemented
      DLLLOCAL ConstHashIterator(const HashIterator&);
      DLLLOCAL ConstHashIterator& operator=(const HashIterator&);

   public:
      DLLEXPORT ConstHashIterator(const class QoreHashNode *h);
      DLLEXPORT ConstHashIterator(const class QoreHashNode &h);
      DLLEXPORT class HashMember *next();
      DLLEXPORT const char *getKey() const;
      // caller owns QoreString returned
      DLLEXPORT class QoreString *getKeyString() const;
      DLLEXPORT const AbstractQoreNode *getValue() const;
      DLLEXPORT AbstractQoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT bool first() const;
      DLLEXPORT bool last() const;
};

//! This is the hash or associative list container type in Qore, dynamically allocated only, reference counted
/**
   it is both a value type and can hold parse expressions as well (in which case it needs to be evaluated)
   This type also maintains the insertion order as well as offering a hash-based lookup of string keys.
   The insertion order of keys is maintained in order to support consistent serialization and 
   deserialization to and from XML and JSON (and others in the future)
 */
class QoreHashNode : public AbstractQoreNode
{
      friend class HashIterator;
      friend class ConstHashIterator;

   private:
      DLLLOCAL class AbstractQoreNode **newKeyValue(const char *key, class AbstractQoreNode *value);
      // does not touch the AbstractQoreNode value
      DLLLOCAL void internDeleteKey(class HashMember *m);
      DLLLOCAL void assimilate_intern(QoreHashNode *h, class ExceptionSink *xsink);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreHashNode(const QoreHashNode&);
      
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreHashNode& operator=(const QoreHashNode&);

  protected:
      class HashMember *member_list;
      class HashMember *tail;
      int len;
      hm_hm_t hm;
      bool needs_eval_flag;

      //! dereferences each element in the hash, does not reset member pointers
      DLLEXPORT void deref_intern(class ExceptionSink *xsink);
 
      //! deletes the object, cannot be called directly (use deref(ExceptionSink *) instead)
      DLLEXPORT virtual ~QoreHashNode();

   public:
      DLLEXPORT QoreHashNode();

      //! concatenate the verbose string representation of the hash (including all contained values) to an existing QoreString
      /** concatenate the verbose string representation of the hash (including all contained values, for %n and %N in print formatting), 
	  "foff" is for multi-line formatting offset, -1 = no line breaks
	  returns -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the hash (including all contained values)
      /** get the verbose string representation of the list (for %n and %N), 
	  "foff" is for multi-line formatting offset, -1 = no line breaks
	  if "del" is true, then the returned QoreString * should be deleted, if false, then it must not be
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! returns true if the hash contains parse expressions and therefore needs evaluation to return a value, false if not
      DLLEXPORT virtual bool needs_eval() const;

      //! performs a deep copy of the hash and returns the new hash
      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion (soft compare)
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) without type conversions (hard compare)
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the data type
      DLLEXPORT virtual const QoreType *getType() const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      //! evaluates the object and returns a value (or 0)
      /** return value requires a deref(xsink)
	  if the object does not require evaluation then "refSelf()" is used to 
	  return the same object with an incremented reference count
	  NOTE: if the object requires evaluation and there is an exception, 0 will be returned
      */
      DLLEXPORT virtual class AbstractQoreNode *eval(class ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  NOTE: if the object requires evaluation and there is an exception, 0 will be returned
	  NOTE: do not use this function directly, use the QoreNodeEvalOptionalRefHolder class instead
      */
      DLLEXPORT virtual class AbstractQoreNode *eval(bool &needs_deref, class ExceptionSink *xsink) const;

      // decrements the reference count
      /** deletes the object when the reference count = 0.  The ExceptionSink 
	  argumentis needed for those types that could throw an exception when 
	  they are deleted (ex: QoreObject) - which could be contained in the list
       */
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);

      //! returns true if the hash does not contain any parse expressions, otherwise returns false
      DLLEXPORT virtual bool is_value() const;

      //! returns "this" with an incremented reference count
      DLLEXPORT QoreHashNode *hashRefSelf() const;

      DLLEXPORT const char *getFirstKey() const;
      DLLEXPORT const char *getLastKey() const;

      // returns (AbstractQoreNode *)-1 if the key doesn't exist
      // FIXME: this is not a good function
      DLLEXPORT AbstractQoreNode *getKeyValueExistence(const char *key);

      // returns (AbstractQoreNode *)-1 if the key doesn't exist
      // FIXME: this is not a good function
      DLLEXPORT const AbstractQoreNode *getKeyValueExistence(const char *key) const;

      // returns (AbstractQoreNode *)-1 if the key doesn't exist
      // FIXME: this is not a good function
      DLLEXPORT AbstractQoreNode *getKeyValueExistence(const class QoreString *key, class ExceptionSink *xsink);

      // returns (AbstractQoreNode *)-1 if the key doesn't exist
      // FIXME: this is not a good function
      DLLEXPORT const AbstractQoreNode *getKeyValueExistence(const class QoreString *key, class ExceptionSink *xsink) const;

      DLLEXPORT AbstractQoreNode *getKeyValue(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT const AbstractQoreNode *getKeyValue(const class QoreString *key, class ExceptionSink *xsink) const;
      DLLEXPORT AbstractQoreNode *getKeyValue(const char *key);
      DLLEXPORT const AbstractQoreNode *getKeyValue(const char *key) const;

      //! performs a deep copy of the hash and returns the new hash
      DLLEXPORT QoreHashNode *copy() const;

      DLLEXPORT AbstractQoreNode **getKeyValuePtr(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT AbstractQoreNode **getKeyValuePtr(const char *key);
      DLLEXPORT AbstractQoreNode **getExistingValuePtr(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT AbstractQoreNode **getExistingValuePtr(const char *key);
      DLLEXPORT void merge(const class QoreHashNode *h, class ExceptionSink *xsink);
      DLLEXPORT void assimilate(class QoreHashNode *h, class ExceptionSink *xsink);

      // FIXME: change to const QoreString * so encodings can be taken into consideration
      DLLEXPORT class AbstractQoreNode *evalKey(const char *key, class ExceptionSink *xsink) const;

      // FIXME: change to const QoreString * so encodings can be taken into consideration
      DLLEXPORT class AbstractQoreNode *evalKeyExistence(const char *key, class ExceptionSink *xsink) const;

      DLLEXPORT void setKeyValue(const class QoreString *key, class AbstractQoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void setKeyValue(const char *key, class AbstractQoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(const char *key, class ExceptionSink *xsink);
      // "takes" the value of the key from the hash and removes the key from the hash and returns the value
      DLLEXPORT class AbstractQoreNode *takeKeyValue(const class QoreString *key, class ExceptionSink *xsink);
      // "takes" the value of the key from the hash and removes the key from the hash and returns the value
      DLLEXPORT class AbstractQoreNode *takeKeyValue(const char *key);
      DLLEXPORT class QoreListNode *getKeys() const;
      DLLEXPORT bool compareSoft(const QoreHashNode *h, class ExceptionSink *xsink) const;
      DLLEXPORT bool compareHard(const QoreHashNode *h, class ExceptionSink *xsink) const;
      DLLEXPORT class AbstractQoreNode *evalFirstKeyValue(class ExceptionSink *xsink) const;
      DLLEXPORT int size() const;

      DLLLOCAL QoreHashNode(bool ne);
      DLLLOCAL void clear(ExceptionSink *xsink);
      DLLLOCAL void clearNeedsEval();
};

class StackHash : public QoreHashNode
{
   private:
      class ExceptionSink *xsink;
   
      // none of these operators/methods are implemented - here to make sure they are not used
      DLLLOCAL void *operator new(size_t); 
      DLLLOCAL StackHash();
      DLLLOCAL StackHash(bool i);
   
   public:
      DLLEXPORT StackHash(class ExceptionSink *xs)
      {
	 xsink = xs;
      }
      DLLEXPORT ~StackHash()
      {
	 deref_intern(xsink);
	 member_list = 0;
      }
};

class TempQoreHashNode {
  private:
   QoreHashNode *h;
   ExceptionSink *xsink;

   DLLLOCAL TempQoreHashNode(const TempQoreHashNode&); // not implemented
   DLLLOCAL TempQoreHashNode& operator=(const TempQoreHashNode&); // not implemented
   DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed

  public:
   DLLLOCAL TempQoreHashNode(QoreHashNode *nh, ExceptionSink *xs) : h(nh), xsink(xs)
   {
   }
   DLLLOCAL TempQoreHashNode(ExceptionSink *xs) : h(0), xsink(xs)
   {
   }
   DLLLOCAL ~TempQoreHashNode()
   {
      if (h)
	 h->deref(xsink);
   }
   DLLLOCAL QoreHashNode *operator->() { return h; }
   DLLLOCAL QoreHashNode *operator*() { return h; }
   DLLLOCAL void operator=(QoreHashNode *nv) { if (h) h->deref(xsink); h = nv; }
   DLLLOCAL QoreHashNode *release() 
   {
      QoreHashNode *rv = h;
      h = 0;
      return rv;
   }
};

#endif // _QORE_HASH_H
