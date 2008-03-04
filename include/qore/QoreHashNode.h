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

class HashMember;

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
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreHashNode(const QoreHashNode&);
      
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreHashNode& operator=(const QoreHashNode&);

  protected:
      //! private implementation of the class
      struct qore_hash_private *priv;

      //! dereferences all elements of the hash
      /** The ExceptionSink argument is needed for those types that could throw
	  an exception when they are deleted (ex: QoreObject) - which could be 
	  contained in the hash
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return true if the object can be deleted, false if not (externally-managed)
       */
      DLLEXPORT virtual bool derefImpl(class ExceptionSink *xsink);

      //! evaluates the object and returns a value (or 0)
      /** return value requires a deref(xsink)
	  if the object requires evaluation and there is an exception, 0 will be returned
      */
      DLLEXPORT virtual AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  @see AbstractQoreNode::eval()
      */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

      //! always returns 0
      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;

      //! always returns 0
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;

      //! always returns false
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;

      //! always returns 0.0
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;
 
      //! deletes the object, cannot be called directly (use deref(ExceptionSink *) instead)
      /** @see AbstractQoreNode::deref()
	  @see QoreHashNode::derefImpl()
       */
      DLLEXPORT virtual ~QoreHashNode();

   public:
      DLLEXPORT QoreHashNode();

      //! concatenate the verbose string representation of the list (including all contained values) to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the List (including all contained values)
      /** used for %n and %N printf formatting
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! returns true if the hash contains parse expressions and therefore needs evaluation to return a value, false if not
      //DLLEXPORT virtual bool needs_eval() const;

      //! performs a deep copy of the hash and returns the new hash
      DLLEXPORT virtual AbstractQoreNode *realCopy() const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion (soft compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) without type conversions (hard compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      //! returns true if the hash does not contain any parse expressions, otherwise returns false
      //DLLEXPORT virtual bool is_value() const;

      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "hash";
      }

      //! returns "this" with an incremented reference count
      DLLEXPORT QoreHashNode *hashRefSelf() const;

      DLLEXPORT const char *getFirstKey() const;
      DLLEXPORT const char *getLastKey() const;

      //! returns the value of the key (must be in QCS_DEFAULT) if it exists and sets "exists" accordingly
      DLLEXPORT AbstractQoreNode *getKeyValueExistence(const char *key, bool &exists);

      //! returns the value of the key (must be in QCS_DEFAULT) if it exists and sets "exists" accordingly
      DLLEXPORT const AbstractQoreNode *getKeyValueExistence(const char *key, bool &exists) const;

      //! returns the value of the key if it exists and sets "exists" accordingly
      /** converts "key" to the default character encoding (QCS_DEFAULT) if necessary
	  an exception could be thrown if the character encoding conversion fails
       */
      DLLEXPORT AbstractQoreNode *getKeyValueExistence(const class QoreString *key, bool &exists, class ExceptionSink *xsink);

      //! returns the value of the key if it exists and sets "exists" accordingly
      /** converts "key" to the default character encoding (QCS_DEFAULT) if necessary
	  an exception could be thrown if the character encoding conversion fails
       */
      DLLEXPORT const AbstractQoreNode *getKeyValueExistence(const class QoreString *key, bool &exists, class ExceptionSink *xsink) const;

      //! returns the value of the key if it exists
      /** converts "key" to the default character encoding (QCS_DEFAULT) if necessary
	  an exception could be thrown if the character encoding conversion fails
       */
      DLLEXPORT AbstractQoreNode *getKeyValue(const class QoreString *key, class ExceptionSink *xsink);

      //! returns the value of the key if it exists
      /** converts "key" to the default character encoding (QCS_DEFAULT) if necessary
	  an exception could be thrown if the character encoding conversion fails
       */
      DLLEXPORT const AbstractQoreNode *getKeyValue(const class QoreString *key, class ExceptionSink *xsink) const;

      //! returns the value of the key (must be in QCS_DEFAULT) if it exists
      DLLEXPORT AbstractQoreNode *getKeyValue(const char *key);

      //! returns the value of the key (must be in QCS_DEFAULT) if it exists
      DLLEXPORT const AbstractQoreNode *getKeyValue(const char *key) const;

      //! performs a deep copy of the hash and returns the new hash
      DLLEXPORT QoreHashNode *copy() const;

      //! returns a pointer to a pointer of the value of the key so the value may be set or changed externally
      /** converts "key" to the default character encoding (QCS_DEFAULT) if necessary
	  an exception could be thrown if the character encoding conversion fails
       */
      DLLEXPORT AbstractQoreNode **getKeyValuePtr(const class QoreString *key, class ExceptionSink *xsink);

      //! returns a pointer to a pointer of the value of the key (must be in QCS_DEFAULT) so the value may be set or changed externally
      DLLEXPORT AbstractQoreNode **getKeyValuePtr(const char *key);

      //! returns a pointer to a pointer of the value of the key only if the key already exists
      /** converts "key" to the default character encoding (QCS_DEFAULT) if necessary
	  an exception could be thrown if the character encoding conversion fails
       */
      DLLEXPORT AbstractQoreNode **getExistingValuePtr(const class QoreString *key, class ExceptionSink *xsink);

      //! returns a pointer to a pointer of the value of the key (must be in QCS_DEFAULT), only if the key already exists
      DLLEXPORT AbstractQoreNode **getExistingValuePtr(const char *key);

      //! appends all key-value pairs of "h" to this hash
      DLLEXPORT void merge(const class QoreHashNode *h, class ExceptionSink *xsink);

      DLLEXPORT void setKeyValue(const class QoreString *key, AbstractQoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void setKeyValue(const char *key, AbstractQoreNode *value, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT void deleteKey(const char *key, class ExceptionSink *xsink);
      // "takes" the value of the key from the hash and removes the key from the hash and returns the value
      DLLEXPORT AbstractQoreNode *takeKeyValue(const class QoreString *key, class ExceptionSink *xsink);
      // "takes" the value of the key from the hash and removes the key from the hash and returns the value
      DLLEXPORT AbstractQoreNode *takeKeyValue(const char *key);
      DLLEXPORT class QoreListNode *getKeys() const;
      DLLEXPORT bool compareSoft(const QoreHashNode *h, class ExceptionSink *xsink) const;
      DLLEXPORT bool compareHard(const QoreHashNode *h, class ExceptionSink *xsink) const;

      //! returns the number of members in the hash, executes in constant time
      /** return the number of members in the hash
       */
      DLLEXPORT qore_size_t size() const;

      DLLLOCAL QoreHashNode(bool ne);
      DLLLOCAL void clear(ExceptionSink *xsink);

      //! sets "needs_eval" to false and "value" to true
      DLLLOCAL void clearNeedsEval();

      DLLLOCAL AbstractQoreNode *evalKeyValue(const QoreString *key, class ExceptionSink *xsink) const;

      // "key" is always passed in the default character encoding
      DLLLOCAL AbstractQoreNode *getReferencedKeyValue(const char *key) const;

      // "key" is always passed in the default character encoding
      DLLLOCAL AbstractQoreNode *getReferencedKeyValue(const char *key, bool &exists) const;

      DLLLOCAL AbstractQoreNode *getFirstKeyValue() const;
};

#include <qore/ReferenceHolder.h>

//! For use on the stack only: manages a QoreHashNode reference count
/**
   @see ReferenceHolder
 */
typedef ReferenceHolder<QoreHashNode> QoreHashNodeHolder;

//! iterator class for QoreHashNode, to be only created on the stack
/**
   @code
   HashIterator hi(h);
   while (hi.next()) {
      QoreStringValueHelper str(hi.getValue());
      printf("key: '%s', value: '%s'\n", hi.getKey(), str->getBuffer());
   }
   @endcode
 */
class HashIterator
{
   private:
      class QoreHashNode *h;
      class HashMember *ptr;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL HashIterator(const HashIterator&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL HashIterator& operator=(const HashIterator&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void* operator new(size_t);

   public:
      //! initializes the iterator with the passed hash
      DLLEXPORT HashIterator(QoreHashNode *h);

      //! initializes the iterator with the passed hash
      DLLEXPORT HashIterator(QoreHashNode &h);

      //! moves to the next element, returns false when there are no more elements to iterate
      /** also moves to the first element if the object has just been initialized after a complete iteration
	  (assuming there is at least one element in the hash)
       */
      DLLEXPORT bool next();

      //! returns the current key
      DLLEXPORT const char *getKey() const;

      //! returns a QoreString for the current key, the caller owns QoreString returned
      DLLEXPORT class QoreString *getKeyString() const;

      //! returns the value of the current key
      DLLEXPORT AbstractQoreNode *getValue() const;

      //! deletes the key from the hash and returns the value, caller owns the reference
      DLLEXPORT AbstractQoreNode *takeValueAndDelete();

      //! deletes the key from the hash and dereferences the value
      /** the pointer is moved to the previous element (or before the beginning) 
	  so that the next call to next() will put the pointer on the element after
	  the one being deleted
      */
      DLLEXPORT void deleteKey(class ExceptionSink *xsink);

      //! returns a pointer to a pointer to the current value so the value of the key may be manipulated externally
      DLLEXPORT AbstractQoreNode **getValuePtr() const;

      //! returns the value of the current key with an incremented reference count
      DLLEXPORT AbstractQoreNode *getReferencedValue() const;

      //! returns true if on the first key of the hash
      DLLEXPORT bool first() const;

      //! returns true if on the last key of the hash
      DLLEXPORT bool last() const;

      //DLLEXPORT void setValue(AbstractQoreNode *val, class ExceptionSink *xsink);
};

//! constant iterator class for QoreHashNode, to be only created on the stack
/**
   @code
   ConstHashIterator hi(h);
   while (hi.next()) {
      QoreStringValueHelper str(hi.getValue());
      printf("key: '%s', value: '%s'\n", hi.getKey(), str->getBuffer());
   }
   @endcode
*/
class ConstHashIterator
{
   private:
      const QoreHashNode *h;
      class HashMember *ptr;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ConstHashIterator(const HashIterator&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL ConstHashIterator& operator=(const HashIterator&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void* operator new(size_t);

   public:
      //! initializes the iterator with the passed hash
      DLLEXPORT ConstHashIterator(const QoreHashNode *h);

      //! initializes the iterator with the passed hash
      DLLEXPORT ConstHashIterator(const QoreHashNode &h);

      //! moves to the next element, returns false when there are no more elements to iterate
      /** also moves to the first element if the object has just been initialized after a complete iteration
	  (assuming there is at least one element in the hash)
       */
      DLLEXPORT bool next();

      //! returns the current key
      DLLEXPORT const char *getKey() const;

      //! returns a QoreString for the current key, the caller owns QoreString returned
      DLLEXPORT class QoreString *getKeyString() const;

      //! returns the value of the current key
      DLLEXPORT const AbstractQoreNode *getValue() const;

      //! returns the value of the current key with an incremented reference count
      DLLEXPORT AbstractQoreNode *getReferencedValue() const;

      //! returns true if on the first key of the hash
      DLLEXPORT bool first() const;

      //! returns true if on the last key of the hash
      DLLEXPORT bool last() const;
};

#endif // _QORE_HASH_H
