/*
  QoreObject.h

  thread-safe object definition

  references: how many variables are pointing at this object?

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

#ifndef _QORE_QOREOBJECT_H

#define _QORE_QOREOBJECT_H

//! the implementation of Qore's object data type, reference counted, dynamically-allocated only
/** objects in Qore are unique unless copied explicitly (similar to Java)
    Builtin classes (those classes implemented in C++ as opposed to user classes implemented in the Qore language)
    can have "private data", which is data that maintains the state of the object per that class.  QoreObject
    objects store this data as well as any member data.
    
    objects have two levels of reference counts - one is for the existence of the c++ object (tRefs below)
    the other is for the scope of the object (the parent QoreReferenceCounter) - when this reaches 0 the
    object will have its destructor run (if it hasn't already been deleted)
    only when tRefs reaches 0, meaning that no more pointers are pointing to this object will the object
    actually be deleted

    @see QoreClass
*/
class QoreObject : public AbstractQoreNode
{
   private:
      //! the private implementation of the class
      struct qore_object_private *priv;

      // must only be called when inside the gate
      DLLLOCAL inline void doDeleteIntern(class ExceptionSink *xsink);
      DLLLOCAL void cleanup(class ExceptionSink *xsink, class QoreHashNode *td);
      DLLLOCAL void addVirtualPrivateData(AbstractPrivateData *apd);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreObject(const QoreObject&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreObject& operator=(const QoreObject&);

   protected:
      //! runs the destructor if necessary and dereferences all members
      /** Note that other objects could be deleted as well if they
	  are members of this object, any exceptions thrown there will also be added to "xsink"
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool derefImpl(class ExceptionSink *xsink);

      //! should never be called, does nothing
      /** in debugging builds calls to this function will abort
       */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

      //! should never be called, does nothing
      /** in debugging builds calls to this function will abort
       */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

      //! should never be called, does nothing
      /** in debugging builds calls to this function will abort
       */
      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;

      //! should never be called, does nothing
      /** in debugging builds calls to this function will abort
       */
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;

      //! should never be called, does nothing
      /** in debugging builds calls to this function will abort
       */
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;

      //! should never be called, does nothing
      /** in debugging builds calls to this function will abort
       */
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

      DLLLOCAL virtual ~QoreObject();

   public:
      //! creates an object as belonging to the given class, the QoreProgram object is referenced for the life of the object as well
      /**
	 @param oc the class of the object being created
	 @param p the QoreProgram object where the object "lives", this QoreProgram object is referenced for the life of the object to ensure that it is not deleted while the object still exists (for example, if the object is exported to a parent QoreProgram object)
       */
      DLLEXPORT QoreObject(const QoreClass *oc, class QoreProgram *p);

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

      //! performs a deep copy of the list and returns the new list
      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

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

      //! returns the type name as a c string
      DLLLOCAL static const char *getStaticTypeName()
      {
	 return "object";
      }

      //! returns true if this object is a valid instance of the classid passed
      /**
	 @param cid the class ID to check
       */
      DLLEXPORT bool validInstanceOf(qore_classid_t cid) const;

      //! sets the value of the given member to the given value
      /** the value must be already referenced for the assignment to the object
	  @param key the name of the member
	  @param val the value to set for the member (must be already referenced for the assignment, 0 is OK too)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void setValue(const char *key, class AbstractQoreNode *val, class ExceptionSink *xsink);

      //! returns the list of members, caller owns the list returned
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT class QoreListNode *getMemberList(class ExceptionSink *xsink) const;

      //! removes a member from the object, if the member's value is an object it is deleted as well (destructor is called)
      /**
	  @param key the name of the member to delete
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deleteMemberValue(const class QoreString *key, class ExceptionSink *xsink);

      //! removes a member from the object, if the member's value is an object it is deleted as well (destructor is called)
      /**
	  @param key the name of the member to delete, assumed to be in the default encoding (QCS_DEFAULT)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deleteMemberValue(const char *key, class ExceptionSink *xsink);

      //! returns the number of members of the object
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int size(class ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion of contained elements (soft compare)
      /**
	 @param obj the object to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT bool compareSoft(const class QoreObject *obj, class ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion of contained elements (hard compare)
      /**
	 @param obj the object to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT bool compareHard(const class QoreObject *obj, class ExceptionSink *xsink) const;

      //! returns the value of the given member with the reference count incremented, the caller owns the AbstractQoreNode (reference) returned
      /**
	 @param mem the name member to retrieve the value for
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT AbstractQoreNode *getReferencedMemberNoMethod(const char *mem, class ExceptionSink *xsink) const;

      //! retuns all member data of the object (or 0 if there's an exception), caller owns the QoreHashNode reference returned
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT class QoreHashNode *copyData(class ExceptionSink *xsink) const;

      //! copies all member data of the current object to the passed QoreHashNode
      /**
	 @param hash the hash to copy all data from
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void mergeDataToHash(class QoreHashNode *hash, class ExceptionSink *xsink);

      //! sets private data for the object against the class ID passed, used in C++ functions implementing Qore constructors
      /**
	 @param key the class ID of the class to set the private data for
	 @param pd the private data for the given class ID
       */
      DLLEXPORT void setPrivate(qore_classid_t key, AbstractPrivateData *pd);

      //! returns the private data corresponding to the class ID passed with an incremented reference count, caller owns the reference
      /**
	 @param key the class ID of the class to get the private data for
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT AbstractPrivateData *getReferencedPrivateData(qore_classid_t key, class ExceptionSink *xsink) const;

      //! evaluates the given method with the arguments passed and returns the return value, caller owns the AbstractQoreNode (reference) returned
      /**
	 @param name the name of the method to evalute
	 @param args the arguments for the method
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT AbstractQoreNode *evalMethod(const class QoreString *name, const class QoreListNode *args, class ExceptionSink *xsink);

      //! runs the destructor on the object (if it hasn't already been deleted)
      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void doDelete(class ExceptionSink *xsink);

      //! returns a pointer to a QoreClass object if the class ID passed is a valid class in the hierarchy
      /**
	 @param cid the class ID passed
	 @return a pointer to a QoreClass object if the class ID passed is a valid class in the hierarchy
       */
      DLLEXPORT const QoreClass *getClass(qore_classid_t cid) const;

      //! returns a pointer to the QoreClass of this object
      /**
	 @return a pointer to the QoreClass of this object
       */
      DLLEXPORT const QoreClass *getClass() const;

      //! returns the name of the class
      /**
	 @return the name of the class
       */
      DLLEXPORT const char *getClassName() const;

      //! returns true if the object is valid
      /**
	 @return true if the object is valid
       */
      DLLEXPORT bool isValid() const;

      //! returns the QoreProgram object associated with this object
      /** for system objects only (created with the system constructor) this will be 0
	  @return the QoreProgram object associated with this object
      */
      DLLEXPORT class QoreProgram *getProgram() const;

      //! returns true if the object is a system object (created with the system constructor)
      /**
	 @return true if the object is a system object (created with the system constructor)
      */
      DLLEXPORT bool isSystemObject() const;

      //! increments the existence reference count
      /** Will not prolong the scope of the object (use QoreObject::ref() to prolong the scope of the object).  
	  To derecrement the count, call QoreObject::tDeref()
	  @see QoreObject::tDeref()
	  @see QoreObject::ref()
       */
      DLLEXPORT void tRef() const;

      //! decrements the existence reference count, when it reaches 0 the object will be deleted
      /** To increment the existence reference count, call QoreObject::tRef()
	  @see QoreObject::tRef()
       */
      DLLEXPORT void tDeref();

      //! increments the scope count of the object
      /**  
       */
      DLLEXPORT void ref() const;

      DLLLOCAL int getStatus() const;

      //! returns the value of the member with an incremented reference count, or executes the memberGate() method and returns the value
      /**
	 @param member the name of the member to get the value for (or evaluate the memberGate() method against)
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *evalMember(const QoreString *member, class ExceptionSink *xsink);

      /**
	 @param h the hash to merge
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void merge(const class QoreHashNode *h, class ExceptionSink *xsink);

      DLLLOCAL class KeyNode *getReferencedPrivateDataNode(qore_classid_t key);

      /**
	 @param key the class key to use
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractPrivateData *getAndClearPrivateData(qore_classid_t key, class ExceptionSink *xsink);

      /**
	 @param meth the name of the method to evalute
	 @param args the arguments for the method
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, const class QoreListNode *args, class ExceptionSink *xsink);

      // called on old to acquire private data, copy method called on self (new copy)
      DLLLOCAL void evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class QoreObject *self, const char *class_name, class ExceptionSink *xsink);

      /**
	 @param str the string to concatenate to
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void addPrivateDataToString(class QoreString *str, class ExceptionSink *xsink) const;

      /**
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void obliterate(class ExceptionSink *xsink);

      /**
	 @param classID the ID of the class
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL void defaultSystemDestructor(qore_classid_t classID, class ExceptionSink *xsink);

      //! returns the pointer to the value of the member
      /** if the member exists, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
	  NOTE: the value returned is not referenced by this function, but rather the object is locked
	  @param key the name of the member
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *getMemberValueNoMethod(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns the pointer to the value of the member
      /**
	  NOTE: the value returned is not referenced by this function, but rather the object is locked
	  @param key the name of the member, assumed to be in the default encoding (QCS_DEFAULT)
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode *getMemberValueNoMethod(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member, so it can be set externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
	  @param key the name of the member
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getMemberValuePtr(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member, so it can be set or changed externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  if the object has a deleted status an exception will be thrown
	  @param key the name of the member, assumed to be in the default encoding (QCS_DEFAULT)
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getMemberValuePtr(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member only if it already exists, so it can be set externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
	  @param mem the name of the member
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getExistingValuePtr(const class QoreString *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member only if it already exists
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  if the object has a deleted status an exception will be thrown
	  @param mem the name of the member, assumed to be in the default encoding (QCS_DEFAULT)
	  @param vl the AutoVLock container for nested locking
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLLOCAL AbstractQoreNode **getExistingValuePtr(const char *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! creates the object with the initial data passed as "d", used by the copy constructor
      DLLLOCAL QoreObject(const QoreClass *oc, class QoreProgram *p, class QoreHashNode *d);
};

#endif
