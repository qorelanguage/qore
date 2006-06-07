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
#include <qore/RMutex.h>

#include <stdio.h>

#define OS_OK            0
#define OS_DELETED       1
#define OS_BEING_DELETED 2
//#define OS_IN_COPY       4

class keyNode {
   public:
      int key;
      void *ptr;
      void *(* getReferencedPrivateDataFunction)(void *);
      class keyNode *next;

      inline keyNode(int k, void *p, void *(* grpdf)(void *))
      {
	 key = k;
	 ptr = p;
	 getReferencedPrivateDataFunction = grpdf;
	 next = NULL;
      }
};

// for objects with multiple classes, private data has to be keyed
class keyList {
      class keyNode *head, *tail;
      int len;

   public:
      inline keyList();
      inline ~keyList();
      inline class keyNode *find(int k);
      inline int insert(int k, void *ptr, void *(* grpdf)(void *));
      inline void *getReferencedPrivateData(int key);
      inline void *getPrivateData(int key);
      inline void addToString(class QoreString *str);
      //inline bool compare(class KeyList *k);
};

class Object : public ReferenceObject
{
   private:
      class QoreClass *type;
      int status;
      class RMutex g;
      class keyList *privateData;
      class ReferenceObject tRefs;  // reference-references
      // FIXME: the only reason this is a pointer is because of include file conflicts :-(
      class Hash *data;
      class QoreProgram *pgm;

      inline void init(class QoreClass *oc, class QoreProgram *p);

   protected:
      inline ~Object();

   public:
      inline Object(class QoreClass *oc, class QoreProgram *p);
      inline Object(class QoreClass *oc, class QoreProgram *p, class Hash *d);

      inline void instantiateLVar(lvh_t id);
      inline void uninstantiateLVar(class ExceptionSink *xsink);

      inline class QoreNode **getMemberValuePtr(class QoreString *key, class VLock *vl, class ExceptionSink *xsink);
      inline class QoreNode **getMemberValuePtr(char *key, class VLock *vl, class ExceptionSink *xsink);
      class QoreNode *getMemberValue(class QoreNode *member, class VLock *vl, class ExceptionSink *xsink);

      inline class QoreNode *getMemberValueNoMethod(class QoreString *key, class VLock *vl, class ExceptionSink *xsink);
      inline class QoreNode *getMemberValueNoMethod(char *key, class VLock *vl, class ExceptionSink *xsink);
      inline class QoreNode **getExistingValuePtr(class QoreString *mem, class VLock *vl, class ExceptionSink *xsink);
      inline class QoreNode **getExistingValuePtr(char *mem, class VLock *vl, class ExceptionSink *xsink);
      inline bool validInstanceOf(int cid);
      inline void setValue(char *key, class QoreNode *val, class ExceptionSink *xsink);
      //inline void setValue(class QoreString *key, class QoreNode *val, class ExceptionSink *xsink);
      inline class List *getMemberList(class ExceptionSink *xsink);
      inline void deleteMemberValue(class QoreString *key, class ExceptionSink *xsink);
      inline void deleteMemberValue(char *key, class ExceptionSink *xsink);
      inline int size();
      bool compareSoft(class Object *obj, class ExceptionSink *xsink);
      bool compareHard(class Object *obj);

      inline void merge(class Hash *h, class ExceptionSink *xsink);
      inline void assimilate(class Hash *h, class ExceptionSink *xsink);

      inline class QoreNode *evalFirstKeyValue(class ExceptionSink *xsink);
      class QoreNode *evalMember(class QoreNode *member, class ExceptionSink *xsink);
      inline class QoreNode *evalMemberNoMethod(char *mem, class ExceptionSink *xsink);
      inline class QoreNode *evalMemberExistence(char *mem, class ExceptionSink *xsink);
      inline class Hash *evalData(class ExceptionSink *xsink);

      void doDelete(class ExceptionSink *xsink);
      inline void doDeleteNoDestructor(class ExceptionSink *xsink);
      inline class QoreClass *getClass() { return type; }
      inline class QoreClass *getClass(int cid);
      inline int getStatus() { return status; }
      inline int isValid() { return status == OS_OK; }

      inline int setPrivate(int key, void *pd, void *(* grpdf)(void *));
      inline void *getReferencedPrivateData(int key);
      inline void *getPrivateData(int key);
      inline void *getAndClearPrivateData(int key);
      inline void addPrivateDataToString(class QoreString *str);

      inline class QoreNode *evalMethod(class QoreString *name, class QoreNode *args, class ExceptionSink *xsink);

      inline class QoreProgram *getProgram()
      {
	 return pgm;
      }
      inline bool isSystemObject()
      {
	 return pgm == NULL;
      }

      inline void ref();
      void dereference(class ExceptionSink *xsink);

      inline void tRef()
      {
	 tRefs.ROreference();
      }
      inline void tDeref()
      {
	 if (tRefs.ROdereference())
	    delete this;
      }
};

static inline void alreadyDeleted(class ExceptionSink *xsink, char *cmeth);

#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/QoreClass.h>
#include <qore/Context.h>
#include <qore/QoreString.h>
#include <qore/List.h>
#include <qore/ReferenceObject.h>
#include <qore/Variable.h>
#include <qore/Exception.h>

#include <stdlib.h>

static inline void alreadyDeleted(class ExceptionSink *xsink, char *cmeth)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s() cannot be executed because the object has already been deleted", cmeth);
}

inline keyList::keyList()
{
   head = tail = NULL;
   len = 0;
}

inline keyList::~keyList()
{
   while (head)
   {
      tail = head->next;
      delete head;
      head = tail;
   }
}

inline class keyNode *keyList::find(int k)
{
   keyNode *w = head;
   while (w)
   {
      if (w->key == k)
	 break;
      w = w->next;
   }
   return w;
}

inline void keyList::addToString(class QoreString *str)
{
   keyNode *w = head;
   while (w)
   {
      str->sprintf("%d=<0x%08x>, ", w->key, w->ptr);
      w = w->next;
   }
}

inline int keyList::insert(int k, void *ptr, void *(* grpdf)(void *))
{
   // see if key exists first, and return -1 if so
   if (find(k))
      return -1;

   class keyNode *n = new keyNode(k, ptr, grpdf);
   if (tail)
      tail->next = n;
   else
      head = n;
   tail = n;
   len++;
   return 0;
}

/*
// 0 = equal, 1 = not equal
inline bool keyList::compare(class KeyList *k)
{
   if (len != k->len)
      return 1;

   bool rc = 0;

   keyNode *w = head;
   while (w)
   {
      keyNode *kn = k->find(w->key);
      if (!kn || kn->ptr != w->ptr)
      {
	 rc = 1;
	 break;
      }

      w = w->next;
   }
   return rc;
}
*/

inline void *keyList::getReferencedPrivateData(int key)
{
   class keyNode *kn = find(key);
   if (!kn || !kn->getReferencedPrivateDataFunction || !kn->ptr)
      return NULL;

   return kn->getReferencedPrivateDataFunction(kn->ptr);
}

inline void *keyList::getPrivateData(int key)
{
   class keyNode *kn = find(key);
   if (!kn)
      return NULL;
   return kn->ptr;
}

static inline void makeAccessDeletedObjectException(class ExceptionSink *xsink, char *mem, char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to access member '%s' of an already-deleted object of class '%s'", mem, cname);
}

static inline void makeAccessDeletedObjectException(class ExceptionSink *xsink, char *cname)
{
   xsink->raiseException("OBJECT-ALREADY-DELETED", "attempt to an already-deleted object of class '%s'", cname);
}

inline void Object::init(class QoreClass *oc, class QoreProgram *p)
{
   status = OS_OK;

   type = oc; 
   type->ref();
   pgm = p;
   privateData = NULL;
}

inline Object::Object(class QoreClass *oc, class QoreProgram *p)
{
   init(oc, p);
   data = new Hash();
}

inline Object::Object(class QoreClass *oc, class QoreProgram *p, class Hash *h)
{
   init(oc, p);
   data = h;
}

inline Object::~Object()
{
   type->deref();
   //tracein("Object::~Object()");
   printd(5, "Object::~Object(this=%08x)\n", this);
#ifdef DEBUG
   if (data && data->size())
      run_time_error("Object::~Object() still has data members (%d)", data->size());
#endif
   if (privateData)
      delete privateData;
   if (data)
      delete data;
   //traceout("Object::~Object()");
}

inline void Object::instantiateLVar(lvh_t id)
{
   ref();
   ::instantiateLVar(id, new QoreNode(this));
}

inline void Object::uninstantiateLVar(class ExceptionSink *xsink)
{
   ::uninstantiateLVar(xsink);
}

// NOTE: caller must unlock
inline class QoreNode **Object::getMemberValuePtr(char *key, class VLock *vl, class ExceptionSink *xsink)
{

   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->add(&g);
   return data->getKeyValuePtr(key);
}

// NOTE: caller must unlock
inline class QoreNode **Object::getMemberValuePtr(QoreString *key, class VLock *vl, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->add(&g);
   return data->getKeyValuePtr(key, xsink);
}

// NOTE: caller must unlock
inline class QoreNode *Object::getMemberValueNoMethod(QoreString *key, class VLock *vl, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), type->name);
      return NULL;
   }

   QoreNode *rv = data->getKeyValue(key, xsink);
   if (rv)
      vl->add(&g);
   else
      g.exit();
   return rv;
}

// NOTE: caller must unlock
inline class QoreNode *Object::getMemberValueNoMethod(char *key, class VLock *vl, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, type->name);
      return NULL;
   }

   QoreNode *rv = data->getKeyValue(key);
   if (rv)
      vl->add(&g);
   else
      g.exit();
   return rv;
}

inline void Object::deleteMemberValue(QoreString *key, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), type->name);
      return;
   }

   data->deleteKey(key, xsink);
   g.exit();
}

inline void Object::deleteMemberValue(char *key, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, type->name);
      return;
   }

   data->deleteKey(key, xsink);
   g.exit();
}

inline class List *Object::getMemberList(class ExceptionSink *xsink)
{
   class List *lst;
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, type->name);
      return NULL;
   }
   lst = data->getKeys();
   g.exit();
   return lst;
}

inline void Object::setValue(char *key, class QoreNode *value, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, type->name);
      return;
   }
   data->setKeyValue(key, value, xsink);
   g.exit();
}

/*
// FIXME: convert to default character set
inline void Object::setValue(class QoreString *key, class QoreNode *value, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), type->name);
      return;
   }
   data->setKeyValue(key->getBuffer(), value, xsink);
   g.exit();
}
*/

inline int Object::size()
{
   int rc;

   g.enter();
   if (status == OS_DELETED)
      rc = 0;
   else
      rc = data->size();
   g.exit();
   return rc;
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
inline void Object::merge(class Hash *h, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, type->name);
      return;
   }
   data->merge(h, xsink);
   g.exit();
}

// adds all elements (already referenced) from the hash passed, deletes the
// hash passed
inline void Object::assimilate(class Hash *h, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, type->name);
      return;
   }
   data->assimilate(h, xsink);
   g.exit();
}

// to be called only in builtin constructors - no locking necessary
inline void Object::doDeleteNoDestructor(class ExceptionSink *xsink)
{
   status = OS_DELETED;
   data->dereference(xsink);
}

inline void Object::ref()
{
   printd(5, "Object::ref(this=%08x) %d->%d\n", this, references, references + 1);
   tRef();          // increment total references
   ROreference();   // incremebet destructor-relevant references
}

inline class QoreNode *Object::evalFirstKeyValue(class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, type->name);
      return NULL;
   }
   class QoreNode *rv = data->evalFirstKeyValue(xsink);
   g.exit();
   return rv;
}

inline class QoreNode *Object::evalMemberNoMethod(char *mem, class ExceptionSink *xsink)
{
   printd(5, "Object::evalMemberNoMethod(this=%08x, mem=%08x (%s), xsink=%08x, data->size()=%d)\n",
	  this, mem, mem, xsink, data ? data->size() : -1);
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, type->name);
      return NULL;
   }
   class QoreNode *rv = data->evalKey(mem, xsink);
   g.exit();
   return rv;
}

// it's OK to return NULL here to duplicate the behaviour of NOTHING
inline class QoreNode *Object::evalMemberExistence(char *mem, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, type->name);
      return NULL;
   }
   class QoreNode *rv = data->evalKeyExistence(mem, xsink);
   g.exit();
   return rv;
}

// need to return an empty hash here
inline class Hash *Object::evalData(class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      return new Hash();
   }
   class Hash *rv = data->eval(xsink);
   g.exit();
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
inline class QoreNode **Object::getExistingValuePtr(class QoreString *mem, class VLock *vl, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem->getBuffer(), type->name);
      return NULL;
   }
   class QoreNode **rv = data->getExistingValuePtr(mem, xsink);
   if (!rv)
      g.exit();
   else
      vl->add(&g);
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
inline class QoreNode **Object::getExistingValuePtr(char *mem, class VLock *vl, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, type->name);
      return NULL;
   }
   class QoreNode **rv = data->getExistingValuePtr(mem);
   if (!rv)
      g.exit();
   else
      vl->add(&g);
   return rv;
}

inline bool Object::validInstanceOf(int cid)
{
   if (status == OS_DELETED)
      return 0;

   return type->getClass(cid);
}

inline void *Object::getReferencedPrivateData(int key) 
{ 
   void *rv = NULL;

   g.enter();
   if (status != OS_DELETED && privateData)
      rv = privateData->getReferencedPrivateData(key);
   g.exit();
   return rv;
}

inline void *Object::getPrivateData(int key)
{ 
   void *rv;
   g.enter();
   if (privateData)
      rv = privateData->getPrivateData(key);
   else
      rv = NULL;
   g.exit();
   return rv;
}

inline void *Object::getAndClearPrivateData(int key)
{ 
   void *rv = NULL;
   g.enter();
   if (privateData)
   {
      class keyNode *kn = privateData->find(key);
      if (kn)
      {
	 rv = kn->ptr;
	 kn->ptr = NULL;
      }
   }
   g.exit(); 
   return rv;
}

inline int Object::setPrivate(int key, void *pd, void *(*grpdf)(void *))
{ 
   g.enter();
   if (!privateData)
      privateData = new keyList();
   int rc = privateData->insert(key, pd, grpdf);
   g.exit();
   return rc;
}

inline void Object::addPrivateDataToString(class QoreString *str)
{
   str->concat('(');
   g.enter();
   if (privateData)
   {
      privateData->addToString(str);
      str->terminate(str->strlen() - 2);
   }
   else
      str->concat("<NO PRIVATE DATA>");
   g.exit();
   str->concat(')');
}

inline class QoreNode *Object::evalMethod(class QoreString *name, class QoreNode *args, class ExceptionSink *xsink)
{
   return type->evalMethod(this, name->getBuffer(), args, xsink);
}

inline class QoreClass *Object::getClass(int cid) 
{

   if (cid == type->getID())
      return type;
   return type->getClass(cid);
}

#endif
