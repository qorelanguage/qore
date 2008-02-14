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
    objects have two levels of reference counts - one is for the existence of the c++ object (tRefs below)
    the other is for the scope of the object (the parent ReferenceObject) - when this reaches 0 the
    object will have its destructor run (if it hasn't already been deleted)
    only when tRefs reaches 0, meaning that no more pointers are pointing to this object will the object
    actually be deleted
*/
class QoreObject : public AbstractQoreNode
{
   private:
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
      DLLLOCAL virtual ~QoreObject();

   public:
      //! creates an object as belonging to the given class, the QoreProgram object is referenced for the life of the object as well
      DLLEXPORT QoreObject(const QoreClass *oc, class QoreProgram *p);

      //! concatenate the verbose string representation of the list (including all contained values) to an existing QoreString
      /** used for %n and %N printf formatting, "foff" is for multi-line formatting offset, -1 = no line breaks
	  returns -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the List (including all contained values)
      /** used for %n and %N printf formatting, "foff" is for multi-line formatting offset, -1 = no line breaks
	  if "del" is true, then the returned QoreString * should be deleted, if false, then it must not be
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! performs a deep copy of the list and returns the new list
      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion (soft compare)
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) without type conversions (hard compare)
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the data type
      DLLEXPORT virtual const QoreType *getType() const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      // decrements the reference count
      /** deletes the object when the reference count = 0.  The ExceptionSink 
	  argument is needed for those types that could throw an exception when 
	  they are deleted (ex: QoreObject) - which could be contained in this object as well
       */
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);

      //! returns true if the list does not contain any parse expressions, otherwise returns false
      DLLEXPORT virtual bool is_value() const;

      DLLEXPORT bool validInstanceOf(int cid) const;
      DLLEXPORT void setValue(const char *key, class AbstractQoreNode *val, class ExceptionSink *xsink);

      //! returns the list of members, caller owns the list returned
      DLLEXPORT class QoreListNode *getMemberList(class ExceptionSink *xsink) const;

      DLLEXPORT void deleteMemberValue(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT void deleteMemberValue(const char *key, class ExceptionSink *xsink);

      //! returns the number of members of the object
      DLLEXPORT int size(class ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion of contained elements (soft compare)
      DLLEXPORT bool compareSoft(const class QoreObject *obj, class ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion of contained elements (hard compare)
      DLLEXPORT bool compareHard(const class QoreObject *obj, class ExceptionSink *xsink) const;

      //! returns the value of the given member with the reference count incremented, the caller owns the AbstractQoreNode (reference) returned
      DLLEXPORT AbstractQoreNode *getReferencedMemberNoMethod(const char *mem, class ExceptionSink *xsink) const;

      //! retuns all member data of the object (or 0 if there's an exception), caller owns the QoreHashNode reference returned
      DLLEXPORT class QoreHashNode *copyData(class ExceptionSink *xsink) const;

      //! copies all member data of the current object to the passed QoreHashNode
      DLLEXPORT void mergeDataToHash(class QoreHashNode *hash, class ExceptionSink *xsink);

      //! sets private data to the passed key, used in Qore-language constructors
      DLLEXPORT void setPrivate(int key, AbstractPrivateData *pd);

      //! returns the private data corresponding to the key passed with an incremented reference count, caller owns the reference
      DLLEXPORT AbstractPrivateData *getReferencedPrivateData(int key, class ExceptionSink *xsink) const;

      //! evaluates the given method with the arguments passed and returns the return value, caller owns the AbstractQoreNode (reference) returned
      DLLEXPORT AbstractQoreNode *evalMethod(const class QoreString *name, const class QoreListNode *args, class ExceptionSink *xsink);

      DLLEXPORT const QoreClass *getClass(int cid) const;
      DLLEXPORT const QoreClass *getClass() const;
      DLLEXPORT const char *getClassName() const;
      DLLEXPORT int getStatus() const;
      DLLEXPORT int isValid() const;
      DLLEXPORT class QoreProgram *getProgram() const;
      DLLEXPORT bool isSystemObject() const;
      DLLEXPORT void tRef() const;
      DLLEXPORT void tDeref();
      DLLEXPORT void ref() const;

      //! returns the value of the member with an incremented reference count, or executes the memberGate() method and returns the value
      DLLLOCAL AbstractQoreNode *evalMember(const QoreString *member, class ExceptionSink *xsink);

      DLLLOCAL void instantiateLVar(lvh_t id);
      DLLLOCAL void uninstantiateLVar(class ExceptionSink *xsink);
      DLLLOCAL void merge(const class QoreHashNode *h, class ExceptionSink *xsink);
      DLLLOCAL class KeyNode *getReferencedPrivateDataNode(int key);
      DLLLOCAL AbstractPrivateData *getAndClearPrivateData(int key, class ExceptionSink *xsink);
      DLLLOCAL AbstractQoreNode *evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, const class QoreListNode *args, class ExceptionSink *xsink);
      // called on old to acquire private data, copy method called on self (new copy)
      DLLLOCAL void evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class QoreObject *self, const char *class_name, class ExceptionSink *xsink);
      DLLLOCAL void addPrivateDataToString(class QoreString *str, class ExceptionSink *xsink) const;
      DLLLOCAL void obliterate(class ExceptionSink *xsink);
      DLLLOCAL void doDelete(class ExceptionSink *xsink);
      DLLLOCAL void defaultSystemDestructor(int classID, class ExceptionSink *xsink);

      //! returns the pointer to the value of the member
      /** if the member exists, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
       */
      DLLLOCAL AbstractQoreNode *getMemberValueNoMethod(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns the pointer to the value of the member, "key" must be in QCS_DEFAULT encoding
      DLLLOCAL AbstractQoreNode *getMemberValueNoMethod(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member, so it can be set externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
       */
      DLLLOCAL AbstractQoreNode **getMemberValuePtr(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member, so it can be set externally, "key" must be in QCS_DEFAULT encoding
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  if the object has a deleted status an exception will be thrown
       */
      DLLLOCAL AbstractQoreNode **getMemberValuePtr(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member only if it already exists, so it can be set externally
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  an exception will be thrown if the character encoding conversion fails
	  also if the object has a deleted status an exception will be thrown
       */
      DLLLOCAL AbstractQoreNode **getExistingValuePtr(const class QoreString *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! returns a pointer to a pointer to the value of the member only if it already exists, "key" must be in QCS_DEFAULT encoding
      /** if no exception occurs, the lock is held and added to the AutoVLock "vl", otherwise the lock is released
	  if the object has a deleted status an exception will be thrown
       */
      DLLLOCAL AbstractQoreNode **getExistingValuePtr(const char *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;

      //! creates the object with the initial data passed as "d", used by the copy constructor
      DLLLOCAL QoreObject(const QoreClass *oc, class QoreProgram *p, class QoreHashNode *d);
};

#endif
