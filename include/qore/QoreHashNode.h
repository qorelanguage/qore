/*
  QoreHashNode.h

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

#ifndef _QORE_QOREHASHNODE_H

#define _QORE_QOREHASHNODE_H

#include <qore/AbstractQoreNode.h>
#include <qore/common.h>
#include <qore/hash_map.h>

class HashMember;
class LocalVar;

//! This is the hash or associative list container type in Qore, dynamically allocated only, reference counted
/**
   it is both a value type and can hold parse expressions as well (in which case it needs to be evaluated)
   This type also maintains the insertion order as well as offering a hash-based lookup of string keys.
   The insertion order of keys is maintained in order to support consistent serialization and 
   deserialization to and from XML and JSON (and possibly others in the future).
 */
class QoreHashNode : public AbstractQoreNode {
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
      DLLEXPORT virtual bool derefImpl(ExceptionSink *xsink);

      //! evaluates the object and returns a value (or 0)
      /** return value requires a deref(xsink)
	  if the object requires evaluation and there is an exception, 0 will be returned
      */
      DLLEXPORT virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

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
      //! creates an empty hash
      DLLEXPORT QoreHashNode();

      //! concatenate the verbose string representation of the list (including all contained values) to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the List (including all contained values)
      /** used for %n and %N printf formatting
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

      //! performs a deep copy of the hash and returns the new hash
      /** @return a copy of the QoreHashNode
       */
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
      /** @return the type name as a c string
       */
      DLLEXPORT virtual const char *getTypeName() const;

      //! returns the type name
      /** @return the type name
       */
      DLLLOCAL static const char *getStaticTypeName() {
	 return "hash";
      }

      //! returns "this" with an incremented reference count
      /** @return "this" with an incremented reference count
       */
      DLLEXPORT QoreHashNode *hashRefSelf() const;

      //! returns the cstring value of the first key in the hash
      /** @return the cstring value of the first key in the hash
       */
      DLLEXPORT const char *getFirstKey() const;

      //! returns the cstring value of the last key in the hash
      /** @return the cstring value of the last key in the hash
       */
      DLLEXPORT const char *getLastKey() const;

      //! returns the value of the key (assumed to be in QCS_DEFAULT) if it exists and sets "exists" accordingly
      /** @param key the key to return the value for
	  @param exists output parameter: if true the key exists, if false the key does not exists (in this case the return value will always be 0)
	  @return the value of the key
       */
      DLLEXPORT AbstractQoreNode *getKeyValueExistence(const char *key, bool &exists);

      //! returns the value of the key (assumed to be in QCS_DEFAULT) if it exists and sets "exists" accordingly
      /** @param key the key to return the value for
	  @param exists output parameter: if true the key exists, if false the key does not exists (in this case the return value will always be 0)
	  @return the value of the key
       */
      DLLEXPORT const AbstractQoreNode *getKeyValueExistence(const char *key, bool &exists) const;

      //! returns the value of the key if it exists and sets "exists" accordingly
      /** Converts "key" to the default character encoding (QCS_DEFAULT) if necessary.
	  An exception could be thrown if the character encoding conversion fails.
	  @param key the key to return the value for
	  @param exists output parameter: if true the key exists, if false the key does not exists (in this case the return value will always be 0)
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the value of the key
       */
      DLLEXPORT AbstractQoreNode *getKeyValueExistence(const QoreString *key, bool &exists, ExceptionSink *xsink);

      //! returns the value of the key if it exists and sets "exists" accordingly
      /** Converts "key" to the default character encoding (QCS_DEFAULT) if necessary.
	  An exception could be thrown if the character encoding conversion fails.
	  @param key the key to return the value for
	  @param exists output parameter: if true the key exists, if false the key does not exists (in this case the return value will always be 0)
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the value of the key
       */
      DLLEXPORT const AbstractQoreNode *getKeyValueExistence(const QoreString *key, bool &exists, ExceptionSink *xsink) const;

      //! returns the value of the key if it exists
      /** Converts "key" to the default character encoding (QCS_DEFAULT) if necessary.
	  An exception could be thrown if the character encoding conversion fails.
	  @param key the key to return the value for
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the value of the key
       */
      DLLEXPORT AbstractQoreNode *getKeyValue(const QoreString *key, ExceptionSink *xsink);

      //! returns the value of the key if it exists
      /** Converts "key" to the default character encoding (QCS_DEFAULT) if necessary.
	  An exception could be thrown if the character encoding conversion fails.
	  @param key the key to return the value for
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return the value of the key
       */
      DLLEXPORT const AbstractQoreNode *getKeyValue(const QoreString *key, ExceptionSink *xsink) const;

      //! returns the value of the key (assumed to be in QCS_DEFAULT) if it exists
      /** @param key the key to return the value for
	  @return the value of the key
       */
      DLLEXPORT AbstractQoreNode *getKeyValue(const char *key);

      //! returns the value of the key (assumed to be in QCS_DEFAULT) if it exists
      /** @param key the key to return the value for
	  @return the value of the key
       */
      DLLEXPORT const AbstractQoreNode *getKeyValue(const char *key) const;

      //! returns the value of the key as an int64
      /** @param key the key to return the value for
	  @param found returns as true if the key exists, false if not
	  @return the value of the key as an int64
      */
      DLLEXPORT int64 getKeyAsBigInt(const char *key, bool &found) const;

      //! performs a deep copy of the hash and returns the new hash
      /** @return a copy of the current QoreHashNode
       */
      DLLEXPORT QoreHashNode *copy() const;

      //! returns a pointer to a pointer of the value of the key so the value may be set or changed externally
      /** Converts "key" to the default character encoding (QCS_DEFAULT) if necessary.
	  An exception could be thrown if the character encoding conversion fails.
	  The key hash entry is created if it does not already exist.
	  @param key the key to return the pointer to the value pointer for
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return a pointer to a pointer of the value of the key
       */
      DLLEXPORT AbstractQoreNode **getKeyValuePtr(const QoreString *key, ExceptionSink *xsink);

      //! returns a pointer to a pointer of the value of the key (assumed to be in QCS_DEFAULT) so the value may be set or changed externally
      /** The key hash entry is created if it does not already exist.
	  @param key the key to return the pointer to the value pointer for
	  @return a pointer to a pointer of the value of the key (assumed to be in QCS_DEFAULT)
       */
      DLLEXPORT AbstractQoreNode **getKeyValuePtr(const char *key);

      //! returns a pointer to a pointer of the value of the key only if the key already exists
      /** Converts "key" to the default character encoding (QCS_DEFAULT) if necessary.
	  An exception could be thrown if the character encoding conversion fails.
	  @param key the key to return the pointer to the value pointer for
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @return a pointer to a pointer of the value of the key, only if the key already exists, otherwise 0 is returned
       */
      DLLEXPORT AbstractQoreNode **getExistingValuePtr(const QoreString *key, ExceptionSink *xsink);

      //! returns a pointer to a pointer of the value of the key (assumed to be be in QCS_DEFAULT), only if the key already exists
      /** @param key the key to return the pointer to the value pointer for
	  @return a pointer to a pointer of the value of the key (assumed to be in QCS_DEFAULT), only if the key already exists, otherwise 0 is returned
       */
      DLLEXPORT AbstractQoreNode **getExistingValuePtr(const char *key);

      //! appends all key-value pairs of "h" to this hash
      /** Note that all keys and values of the QoreHashNode "h" are copied to this hash, values are referenced as necessary for the assigment to "this".
	  Qore-language exceptions could be thrown if the hash keys in "this" are overwritten with new values and the old value is an object that goes out of scope when dereferenced.
	  @param h the QoreHashNode to use to merge all keys to "this"
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
       */
      DLLEXPORT void merge(const QoreHashNode *h, ExceptionSink *xsink);

      //! sets the value of "key" to "value"
      /** A Qore-language exception could be thrown converting the key string's encoding to QCS_DEFAULT, or if the given key has a current value and it's a QoreObject that goes out of scope when dereferenced (the object's destructor could throw an exception)
	  @param key the key to set the value for
	  @param value the value to assign to the key, must be already referenced for the assignment
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @note the assignment is made even if an exception occurs when dereferencing the old value
       */
      DLLEXPORT void setKeyValue(const QoreString *key, AbstractQoreNode *value, ExceptionSink *xsink);

      //! sets the value of "key" to "value"
      /** A Qore-language exception could be thrown if the given key has a current value and it's a QoreObject that goes out of scope when dereferenced (the object's destructor could throw an exception).
	  @param key the key to set the value for (assumed to be in QCS_DEFAULT encoding)
	  @param value the value to assign to the key, must be already referenced for the assignment
	  @param xsink if an error occurs, the Qore-language exception information will be added here	  
	  @note the assignment is made even if an exception occurs when dereferencing the old value
       */
      DLLEXPORT void setKeyValue(const char *key, AbstractQoreNode *value, ExceptionSink *xsink);

      //! sets the value of "key" to "value" and returns the old value (0 if not present or if already 0), caller owns any reference count returned
      /** A Qore-language exception could be thrown converting the key string's encoding to QCS_DEFAULT
	  @param key the key to set the value for
	  @param value the value to assign to the key, must be already referenced for the assignment
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the old value of the key (0 if not present or if the old value was already 0), caller owns any reference count returned
       */
      DLLEXPORT AbstractQoreNode *swapKeyValue(const QoreString *key, AbstractQoreNode *value, ExceptionSink *xsink);

      //! sets the value of "key" to "value" and returns the old value (0 if not present or if already 0), caller owns any reference count returned
      /** @param key the key to set the value for (assumed to be in QCS_DEFAULT encoding)
	  @param value the value to assign to the key, must be already referenced for the assignment
	  @return the old value of the key (0 if not present or if the old value was already 0), caller owns any reference count returned
       */
      DLLEXPORT AbstractQoreNode *swapKeyValue(const char *key, AbstractQoreNode *value);

      //! performs a delete operation on the value of the given key
      /** The delete operation means a simple dereference for all types except QoreObject, on this type the destructor will be run immediately.
	  A Qore-language exception could occur eitherin converting the key string's encoding to QCS_DEFAULT, or in the destructor of a deleted object.
	  @param key the key of the value to delete
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deleteKey(const QoreString *key, ExceptionSink *xsink);

      //! performs a delete operation on the value of the given key
      /** the delete operation means a simple dereference for all types except QoreObject, on this type the destructor will be run immediately
	  @param key the key of the value to delete
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deleteKey(const char *key, ExceptionSink *xsink);

      //! "takes" the value of the key from the hash and removes the key from the hash and returns the value, caller owns the reference count returned
      /** @param key the key of the value to return
	  @param xsink if an error occurs converting the key string to QCS_DEFAULT encoding, the Qore-language exception information will be added here
	  @return the value of the key, caller owns the reference count returned
       */
      DLLEXPORT AbstractQoreNode *takeKeyValue(const QoreString *key, ExceptionSink *xsink);

      //! "takes" the value of the key from the hash and removes the key from the hash and returns the value, caller owns the reference count returned
      /** @param key the key of the value to return
	  @return the value of the key, caller owns the reference count returned
       */
      DLLEXPORT AbstractQoreNode *takeKeyValue(const char *key);

      //! returns a QoreListNode of QoreStringNodes representing all keys in the hash, caller owns the reference count returned
      /** to iterate through a hash, use HashIterator or ConstHashIterator
	  @return a QoreListNode of QoreStringNodes representing all keys in the hash, caller owns the reference count returned
       */
      DLLEXPORT QoreListNode *getKeys() const;

      //! does a deep "soft" compare of all hash elements (types may be converted for the comparison) and returns true if the hashes are equal
      /** @note that if the hashes have a different number or names of keys then the comparison fails immediately
	  @return true if the hashes have the same number and names of keys and all elements are equal (types may be converted for the comparison)
       */
      DLLEXPORT bool compareSoft(const QoreHashNode *h, ExceptionSink *xsink) const;

      //! does a deep "hard" compare of all hash elements (no type conversions are performed) and returns true if the hashes are equal
      /** @note that if the hashes have a different number of keys then the comparison fails immediately
	  @return true if the hashes have the same number and values of keys and all elements are equal and of the same type (no type conversions are performed)
       */
      DLLEXPORT bool compareHard(const QoreHashNode *h, ExceptionSink *xsink) const;

      //! returns the number of members in the hash, executes in constant time
      /** return the number of members in the hash
       */
      DLLEXPORT qore_size_t size() const;

      //! returns true if the hash has no members, false if not
      /** return true if the hash has no members, false if not
       */
      DLLEXPORT bool empty() const;

      DLLLOCAL QoreHashNode(bool ne);
      DLLLOCAL void clear(ExceptionSink *xsink);

      //! sets "needs_eval" to false and "value" to true
      DLLLOCAL void clearNeedsEval();

      //! sets "needs_eval" to true and "value" to false
      DLLLOCAL void setNeedsEval();

      DLLLOCAL AbstractQoreNode *evalKeyValue(const QoreString *key, ExceptionSink *xsink) const;

      // returns a new hash consisting of just the members of value_list
      DLLLOCAL QoreHashNode *getSlice(const QoreListNode *value_list, ExceptionSink *xsink) const;

      // "key" is always passed in the default character encoding
      DLLLOCAL AbstractQoreNode *getReferencedKeyValue(const char *key) const;

      // "key" is always passed in the default character encoding
      DLLLOCAL AbstractQoreNode *getReferencedKeyValue(const char *key, bool &exists) const;

      DLLLOCAL AbstractQoreNode *getFirstKeyValue() const;

      //! removes the given key from the hash and derefences its value, if any
      /** A Qore-language exception could occur either in converting the key string's encoding to QCS_DEFAULT, or when dereferencing the contained value
	  @param key the key of the value to delete
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void removeKey(const QoreString *key, ExceptionSink *xsink);

      //! removes the given key from the hash and derefences its value, if any
      /** A Qore-language exception could occur when dereferencing the contained value
	  @param key the key of the value to delete, must be in QCS_DEFAULT encoding
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void removeKey(const char *key, ExceptionSink *xsink);
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
class HashIterator {
   private:
      class QoreHashNode *h;
      HashMember *ptr;

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
      DLLEXPORT QoreString *getKeyString() const;

      //! returns the value of the current key
      DLLEXPORT AbstractQoreNode *getValue() const;

      //! deletes the key from the hash and returns the value, caller owns the reference
      DLLEXPORT AbstractQoreNode *takeValueAndDelete();

      //! deletes the key from the hash and dereferences the value
      /** the pointer is moved to the previous element (or before the beginning) 
	  so that the next call to next() will put the pointer on the element after
	  the one being deleted
      */
      DLLEXPORT void deleteKey(ExceptionSink *xsink);

      //! returns a pointer to a pointer to the current value so the value of the key may be manipulated externally
      DLLEXPORT AbstractQoreNode **getValuePtr() const;

      //! returns the value of the current key with an incremented reference count
      DLLEXPORT AbstractQoreNode *getReferencedValue() const;

      //! returns true if on the first key of the hash
      DLLEXPORT bool first() const;

      //! returns true if on the last key of the hash
      DLLEXPORT bool last() const;

      //DLLEXPORT void setValue(AbstractQoreNode *val, ExceptionSink *xsink);
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
class ConstHashIterator {
   private:
      const QoreHashNode *h;
      HashMember *ptr;

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
      DLLEXPORT QoreString *getKeyString() const;

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
