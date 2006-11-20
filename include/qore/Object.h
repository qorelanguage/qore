/*
  Object.h

  thread-safe object definition

  references: how many variables are pointing at this object?

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

#ifndef _QORE_OBJECT_H

#define _QORE_OBJECT_H

#include <qore/common.h>
#include <qore/ReferenceObject.h>
#include <qore/VRMutex.h>

#include <stdio.h>

#define OS_OK            0
#define OS_DELETED       1
#define OS_BEING_DELETED 2
//#define OS_IN_COPY       4

typedef void (* q_private_t)(void *);

// note that any of the methods below that involve locking cannot be const methods
class Object : public ReferenceObject
{
   private:
      class QoreClass *myclass;
      int status;
      class VRMutex g;
      class KeyList *privateData;
      class ReferenceObject tRefs;  // reference-references
      // FIXME: the only reason this is a pointer is because of include file conflicts :-(
      class Hash *data;
      class QoreProgram *pgm;

      DLLLOCAL inline void init(class QoreClass *oc, class QoreProgram *p);
      // must only be called when inside the gate
      DLLLOCAL inline void doDeleteIntern(class ExceptionSink *xsink);
      DLLLOCAL void cleanup(class ExceptionSink *xsink, class Hash *td);
      
   protected:
      DLLLOCAL ~Object();

   public:
      DLLEXPORT Object(class QoreClass *oc, class QoreProgram *p);
      DLLEXPORT Object(class QoreClass *oc, class QoreProgram *p, class Hash *d);

      DLLEXPORT void instantiateLVar(lvh_t id);
      DLLEXPORT void uninstantiateLVar(class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getMemberValuePtr(class QoreString *key, class VLock *vl, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getMemberValuePtr(char *key, class VLock *vl, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *getMemberValueNoMethod(class QoreString *key, class VLock *vl, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *getMemberValueNoMethod(char *key, class VLock *vl, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getExistingValuePtr(class QoreString *mem, class VLock *vl, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode **getExistingValuePtr(char *mem, class VLock *vl, class ExceptionSink *xsink);
      DLLEXPORT bool validInstanceOf(int cid) const;
      DLLEXPORT void setValue(char *key, class QoreNode *val, class ExceptionSink *xsink);
      DLLEXPORT class List *getMemberList(class ExceptionSink *xsink);
      DLLEXPORT void deleteMemberValue(class QoreString *key, class ExceptionSink *xsink);
      DLLEXPORT void deleteMemberValue(char *key, class ExceptionSink *xsink);
      DLLEXPORT int size();
      DLLEXPORT bool compareSoft(class Object *obj, class ExceptionSink *xsink);
      DLLEXPORT bool compareHard(class Object *obj);
      DLLEXPORT void merge(class Hash *h, class ExceptionSink *xsink);
      DLLEXPORT void assimilate(class Hash *h, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *evalFirstKeyValue(class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *evalMember(class QoreNode *member, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *evalMemberNoMethod(char *mem, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *evalMemberExistence(char *mem, class ExceptionSink *xsink);
      DLLEXPORT class Hash *evalData(class ExceptionSink *xsink);
      DLLEXPORT void setPrivate(int key, void *pd, q_private_t pdref, q_private_t pdderef);
      DLLEXPORT class KeyNode *getReferencedPrivateDataNode(int key);
      DLLEXPORT void *getReferencedPrivateData(int key);
      DLLEXPORT void *getAndClearPrivateData(int key);
      DLLEXPORT void addPrivateDataToString(class QoreString *str);
      DLLEXPORT class QoreNode *evalMethod(class QoreString *name, class QoreNode *args, class ExceptionSink *xsink);
      DLLEXPORT void doDelete(class ExceptionSink *xsink);
      DLLEXPORT void ref();
      DLLEXPORT void dereference(class ExceptionSink *xsink);
      DLLEXPORT void obliterate(class ExceptionSink *xsink);
      DLLEXPORT class QoreClass *getClass(int cid) const;
      DLLLOCAL class QoreNode *evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, class QoreNode *args, class ExceptionSink *xsink);
      // called on old to acquire private data, copy method called on self
      DLLLOCAL void evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class Object *self, class ExceptionSink *xsink);

      DLLEXPORT inline class QoreClass *getClass() const 
      { 
	 return myclass; 
      }
      DLLEXPORT inline int getStatus() const 
      { 
	 return status; 
      }
      DLLEXPORT inline int isValid() const 
      { 
	 return status == OS_OK; 
      }
      DLLEXPORT inline class QoreProgram *getProgram() const
      {
	 return pgm;
      }
      DLLEXPORT inline bool isSystemObject() const
      {
	 return pgm == NULL;
      }

      DLLEXPORT inline void tRef()
      {
	 tRefs.ROreference();
      }
      DLLEXPORT inline void tDeref()
      {
	 if (tRefs.ROdereference())
	    delete this;
      }
};

#endif
