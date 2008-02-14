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

// objects have two levels of reference counts - one is for the existence of the c++ object (tRefs below)
// the other is for the scope of the object (the parent ReferenceObject) - when this reaches 0 the
// object will have its destructor run (if it hasn't already been deleted)
// only when tRefs reaches 0, meaning that no more pointers are pointing to this object will the object
// actually be deleted

// note that any of the methods below that involve locking cannot be const methods
class QoreObject : public AbstractQoreNode
{
   private:
      struct qore_object_private *priv;

      // must only be called when inside the gate
      DLLLOCAL inline void doDeleteIntern(class ExceptionSink *xsink);
      DLLLOCAL void cleanup(class ExceptionSink *xsink, class QoreHashNode *td);
      DLLLOCAL void addVirtualPrivateData(AbstractPrivateData *apd);

      // not implemented
      DLLLOCAL QoreObject(const QoreObject&);
      DLLLOCAL QoreObject& operator=(const QoreObject&);

   protected:
      DLLLOCAL virtual ~QoreObject();

   public:
      DLLEXPORT QoreObject(const QoreClass *oc, class QoreProgram *p);
      DLLEXPORT QoreObject(const QoreClass *oc, class QoreProgram *p, class QoreHashNode *d);

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      // performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
      // the "val" passed
      //DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
      // the type passed must always be equal to the current type
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      // returns the data type
      DLLEXPORT virtual const QoreType *getType() const;
      // returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;
      // deletes the object when the reference count = 0
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);
      // returns true if the node represents a value (default implementation)
      DLLEXPORT virtual bool is_value() const;
      
      DLLEXPORT class AbstractQoreNode **getMemberValuePtr(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;
      DLLEXPORT class AbstractQoreNode **getMemberValuePtr(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;
      DLLEXPORT class AbstractQoreNode *getMemberValueNoMethod(const class QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink) const;
      DLLEXPORT class AbstractQoreNode *getMemberValueNoMethod(const char *key, class AutoVLock *vl, class ExceptionSink *xsink) const;
      DLLEXPORT class AbstractQoreNode **getExistingValuePtr(const class QoreString *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;
      DLLEXPORT class AbstractQoreNode **getExistingValuePtr(const char *mem, class AutoVLock *vl, class ExceptionSink *xsink) const;
      DLLEXPORT bool validInstanceOf(int cid) const;
      DLLEXPORT void setValue(const char *key, class AbstractQoreNode *val, class ExceptionSink *xsink);
      // caller owns the list returned
      DLLEXPORT class QoreListNode *getMemberList(class ExceptionSink *xsink) const;
      DLLEXPORT void deleteMemberValue(const class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT void deleteMemberValue(const char *key, class ExceptionSink *xsink);
      DLLEXPORT int size(class ExceptionSink *xsink) const;
      DLLEXPORT bool compareSoft(const class QoreObject *obj, class ExceptionSink *xsink) const;
      DLLEXPORT bool compareHard(const class QoreObject *obj, class ExceptionSink *xsink) const;
      // caller owns the AbstractQoreNode (reference) returned
      DLLEXPORT class AbstractQoreNode *evalFirstKeyValue(class ExceptionSink *xsink) const;
      // caller owns the AbstractQoreNode (reference) returned
      DLLEXPORT class AbstractQoreNode *evalMemberNoMethod(const char *mem, class ExceptionSink *xsink) const;
      // caller owns the AbstractQoreNode (reference) returned
      DLLEXPORT class AbstractQoreNode *evalMemberExistence(const char *mem, class ExceptionSink *xsink) const;
      // caller owns the QoreHashNode returned
      DLLEXPORT class QoreHashNode *copyData(class ExceptionSink *xsink) const;
      // caller owns the QoreHashNode returned
      DLLEXPORT class QoreHashNode *copyDataNode(class ExceptionSink *xsink) const;
      // copies all data to the passed QoreHashNode
      DLLEXPORT void mergeDataToHash(class QoreHashNode *hash, class ExceptionSink *xsink);
      // sets private data to the passed key, used in constructors
      DLLEXPORT void setPrivate(int key, AbstractPrivateData *pd);
      DLLEXPORT AbstractPrivateData *getReferencedPrivateData(int key, class ExceptionSink *xsink) const;
      // caller owns the AbstractQoreNode (reference) returned
      DLLEXPORT class AbstractQoreNode *evalMethod(const class QoreString *name, const class QoreListNode *args, class ExceptionSink *xsink);
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

      DLLLOCAL class AbstractQoreNode *evalMember(const QoreString *member, class ExceptionSink *xsink);
      DLLLOCAL void instantiateLVar(lvh_t id);
      DLLLOCAL void uninstantiateLVar(class ExceptionSink *xsink);
      DLLLOCAL void merge(const class QoreHashNode *h, class ExceptionSink *xsink);
      DLLLOCAL void assimilate(class QoreHashNode *h, class ExceptionSink *xsink);
      DLLLOCAL class KeyNode *getReferencedPrivateDataNode(int key);
      DLLLOCAL AbstractPrivateData *getAndClearPrivateData(int key, class ExceptionSink *xsink);
      DLLLOCAL class AbstractQoreNode *evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, const class QoreListNode *args, class ExceptionSink *xsink);
      // called on old to acquire private data, copy method called on self (new copy)
      DLLLOCAL void evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class QoreObject *self, const char *class_name, class ExceptionSink *xsink);
      DLLLOCAL void addPrivateDataToString(class QoreString *str, class ExceptionSink *xsink) const;
      DLLLOCAL void obliterate(class ExceptionSink *xsink);
      DLLLOCAL void doDelete(class ExceptionSink *xsink);
      DLLLOCAL void defaultSystemDestructor(int classID, class ExceptionSink *xsink);
};

#endif
