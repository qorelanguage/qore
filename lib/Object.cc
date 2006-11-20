/*
  Object.cc

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

#include <qore/config.h>
#include <qore/Object.h>
#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/QoreClass.h>
#include <qore/Context.h>
#include <qore/QoreString.h>
#include <qore/List.h>
#include <qore/ReferenceObject.h>
#include <qore/Variable.h>
#include <qore/Exception.h>
#include <qore/QoreProgram.h>

#include <stdlib.h>

class KeyNode {
   private:
      int key;
      void *ptr;
      q_private_t f_ref, f_deref;

   public:
      class KeyNode *next;

      DLLLOCAL inline KeyNode(int k, void *p, q_private_t r, q_private_t d)
      {
	 key = k;
	 ptr = p;
	 f_ref = r;
	 f_deref = d;
	 next = NULL;
      }
      DLLLOCAL inline int getKey() const
      {
	 return key;
      }
      DLLLOCAL inline void *getPtr() const
      {
	 return ptr;
      }
      DLLLOCAL inline void *getAndClearPtr()
      {
	 void *rv = ptr;
	 ptr = NULL;
#ifdef DEBUG
	 f_ref = NULL;
	 f_deref = NULL;
#endif
	 return rv;
      }
      DLLLOCAL inline class KeyNode *refNode()
      {
	 if (!ptr)
	    return NULL;
	 f_ref(ptr);
	 return this;
      }
      DLLLOCAL inline void deref()
      {
	 f_deref(ptr);
      }
      DLLLOCAL inline void *refPtr()
      {
	 if (!ptr)
	    return NULL;
	 f_ref(ptr);
	 return ptr;
      }
      DLLLOCAL inline void derefPtr()
      {
	 f_deref(ptr);
      }
};

// for objects with multiple classes, private data has to be keyed
class KeyList {
      class KeyNode *head, *tail;
      int len;

   public:
      DLLLOCAL inline KeyList();
      DLLLOCAL inline ~KeyList();
      DLLLOCAL inline class KeyNode *find(int k) const;
      DLLLOCAL inline void insert(int k, void *ptr, q_private_t ref, q_private_t deref);
      DLLLOCAL inline class KeyNode *getReferencedPrivateDataNode(int key);
      DLLLOCAL inline void *getReferencedPrivateData(int key);
      DLLLOCAL inline void addToString(class QoreString *str) const;
      DLLLOCAL inline void derefAll();
};

inline KeyList::KeyList()
{
   head = tail = NULL;
   len = 0;
}

inline KeyList::~KeyList()
{
   while (head)
   {
      tail = head->next;
      delete head;
      head = tail;
   }
}

inline void KeyList::derefAll()
{
   KeyNode *w = head;
   while (w)
   {
      w->deref();
      w = w->next;
   }
}

inline class KeyNode *KeyList::find(int k) const
{
   KeyNode *w = head;
   while (w)
   {
      if (w->getKey() == k)
	 break;
      w = w->next;
   }
   return w;
}

DLLLOCAL inline void *KeyList::getReferencedPrivateData(int key)
{
   class KeyNode *kn = find(key);
   return kn ? kn->refPtr() : NULL;
}

inline void KeyList::addToString(class QoreString *str) const
{
   KeyNode *w = head;
   while (w)
   {
      str->sprintf("%d=<0x%08p>, ", w->getKey(), w->getPtr());
      w = w->next;
   }
}

inline void KeyList::insert(int k, void *ptr, q_private_t ref, q_private_t deref)
{
#ifdef DEBUG
   // see if key exists - should never happen
   if (find(k))
      run_time_error("KeyList::insert duplicate key=%d ptr=%08p", k, ptr);
#endif

   class KeyNode *n = new KeyNode(k, ptr, ref, deref);
   if (tail)
      tail->next = n;
   else
      head = n;
   tail = n;
   len++;
}

inline class KeyNode *KeyList::getReferencedPrivateDataNode(int key)
{
   class KeyNode *kn = find(key);
   if (!kn)
      return NULL;
   kn->refPtr();
   return kn;
}

inline void Object::init(class QoreClass *oc, class QoreProgram *p)
{
   status = OS_OK;

   myclass = oc; 
   pgm = p;
   // instead of referencing the class, we reference the program, because the
   // program contains the namespace that contains the class, and the class'
   // methods may call functions in the program as well that could otherwise
   // disappear when the program is deleted
   if (p)
   {
      printd(5, "Object::init() this=%08p (%s) calling QoreProgram::depRef() (%08p)\n", this, myclass->getName(), p);
      p->depRef();
   }
   privateData = NULL;
}

Object::Object(class QoreClass *oc, class QoreProgram *p)
{
   init(oc, p);
   data = new Hash();
}

Object::Object(class QoreClass *oc, class QoreProgram *p, class Hash *h)
{
   init(oc, p);
   data = h;
}

Object::~Object()
{
   //tracein("Object::~Object()");
   printd(5, "Object::~Object() this=%08p, pgm=%08p\n", this, pgm);
   //myclass->deref();
#ifdef DEBUG
   if (pgm)
      run_time_error("Object::~Object() still has pgm=%08p", pgm);
   if (data)
      run_time_error("Object::~Object() still has data=%08p", data);
   if (privateData)
      run_time_error("Object::~Object() still has privateData=%08p", privateData);
#endif
   //traceout("Object::~Object()");
}

class QoreNode *Object::evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, class QoreNode *args, class ExceptionSink *xsink)
{
   // get referenced object
   class KeyNode *kn = getReferencedPrivateDataNode(meth->myclass->getID());
   
   if (kn)
   {
      class QoreNode *rv = meth->evalMethod(this, kn->getPtr(), args, xsink);
      kn->deref();
      return rv;
   }

   if (myclass == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", myclass->getName(), meth->name);
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of object's class '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), meth->name, myclass->getName());
   return NULL;
}

void Object::evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class Object *self, class ExceptionSink *xsink)
{
   // get referenced object
   class KeyNode *kn = getReferencedPrivateDataNode(meth->myclass->getID());
   
   if (kn)
   {
      meth->evalCopy(self, this, kn->getPtr(), xsink);
      kn->deref();
      return;
   }

   if (myclass == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::copy() cannot be executed because the object has already been deleted", myclass->getName());
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::copy() (base class of object's class '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), myclass->getName());
}

void Object::instantiateLVar(lvh_t id)
{
   ref();
   ::instantiateLVar(id, new QoreNode(this));
}

void Object::uninstantiateLVar(class ExceptionSink *xsink)
{
   ::uninstantiateLVar(xsink);
}

void Object::ref()
{
   printd(5, "Object::ref(this=%08p) %d->%d\n", this, references, references + 1);
   //tRef();          // increment total references
   ROreference();   // increment destructor-relevant references
}

bool Object::validInstanceOf(int cid) const
{
   if (status == OS_DELETED)
      return 0;

   return myclass->getClass(cid);
}

class QoreNode *Object::evalMethod(class QoreString *name, class QoreNode *args, class ExceptionSink *xsink)
{
   return myclass->evalMethod(this, name->getBuffer(), args, xsink);
}

class QoreClass *Object::getClass(int cid) const
{
   if (cid == myclass->getID())
      return myclass;
   return myclass->getClass(cid);
}

class QoreNode *Object::evalMember(class QoreNode *member, class ExceptionSink *xsink)
{
   // make sure to convert string encoding if necessary to default character set
   class QoreString *tstr = member->val.String;
   if (tstr->getEncoding() != QCS_DEFAULT)
   {
      tstr = tstr->convertEncoding(QCS_DEFAULT, xsink);
      if (xsink->isEvent())
	 return NULL;
   }

   class QoreNode *rv;
   char *mem = tstr->getBuffer();
   
   //printd(5, "Object::evalMember() find_key(%s)=%08p myclass=%s\n", mem, find_key(mem), myclass ? myclass->getName() : "NONE");
   // if accessed outside the class and the member is a private member 
   class Object *obj = getStackObject();
   printd(5, "Object::evalMember(%s) obj=%08p class=%s ID=%d stack obj=%08p class=%s ID=%d isPrivateMember=%s\n", mem, this, myclass->getName(), myclass->getID(), obj, obj ? obj->myclass->getName() : "(null)", obj ? obj->myclass->getID() : -1, myclass->isPrivateMember(mem) ? "true" : "false");
	  
   // if accessed outside the class and the member is a private member 
   if ((!obj || (obj && obj->myclass->getID() != myclass->getID())) && myclass->isPrivateMember(mem))
   {
      if (myclass->hasMemberGate()) // execute the member gate if it exists for private members
	 rv = myclass->evalMemberGate(this, member, xsink);
      else
      {
	 xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, myclass->getName());
	 rv = NULL;
      }
   }
   else
   {
      g.enter();
      if (status == OS_DELETED)
      {
	 g.exit();
	 rv = NULL;
      }
      else
      {
	 rv = data->evalKeyExistence(mem, xsink);
	 g.exit();
	 
	 // execute memberGate method for objects where no member exists
	 if (rv == (QoreNode *)-1)
	    rv = myclass->evalMemberGate(this, member, xsink);
      }
   }
   if (tstr != member->val.String)
      delete tstr;

   return rv;
}

// 0 = equal, 1 = not equal
bool Object::compareSoft(class Object *obj, class ExceptionSink *xsink)
{
   if (obj->myclass != myclass)
      return 1;

   // to avoid deadlocks when an object is compared with itself
   if (this == obj)
      return 0;

   // objects are not equal if they have private data and they
   // do not point to the same objects
   if (privateData != obj->privateData)
      return 1;

   int rc;

   g.enter();
   obj->g.enter();
   if (status == OS_DELETED)
   {
      if (obj->status == OS_DELETED)
	 rc = 0;
      else
	 rc = 1;
   }
   else if (obj->status == OS_DELETED)
      rc = 1;
   else
      rc = data->compareSoft(obj->data, xsink);

   obj->g.exit();
   g.exit();
   return rc;
}

// 0 = equal, 1 = not equal
bool Object::compareHard(class Object *obj)
{
   if (obj->myclass != myclass)
      return 1;

   // to avoid deadlocks when an object is compared with itself
   if (this == obj)
      return 0;

   // objects are not equal if they have private data and they
   // do not point to the same objects
   if (privateData != obj->privateData)
      return 1;

   int rc;

   g.enter();
   obj->g.enter();

   if (status == OS_DELETED)
   {
      if (obj->status == OS_DELETED)
	 rc = 0;
      else
	 rc = 1;
   }
   else if (obj->status == OS_DELETED)
      rc = 1;
   else
      rc = data->compareHard(obj->data);

   obj->g.exit();
   g.exit();
   return rc;
}

// gate is already held
inline void Object::doDeleteIntern(class ExceptionSink *xsink)
{
   if (status == OS_DELETED)
   {
      g.exit();
      return;
   }
   if (status == OS_BEING_DELETED)
   {
      g.exit();
      xsink->raiseException("DOUBLE-DELETE-EXCEPTION", "destructor called from within destructor");
      return;
   }
   status = OS_BEING_DELETED;
   g.exit();
   
   printd(5, "Object::doDelete(this=%08p) calling destructor()\n", this);   
   myclass->execDestructor(this, xsink);
   
   g.enter();
   status = OS_DELETED;
   Hash *td = data;
   data = NULL;
   g.exit();

   cleanup(xsink, td);
}

// does a deep dereference and execs destructor if necessary
void Object::dereference(ExceptionSink *xsink)
{
   printd(5, "Object::dereference(this=%08p) class=%s %d->%d\n", this, myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      g.enter();
      printd(5, "Object::dereference() class=%s deleting this=%08p\n", myclass->getName(), this);
      if (status == OS_OK)
      {
	 // reference for destructor
	 ROreference();
	 doDeleteIntern(xsink);
	 ROdereference();
      }
      else
      {
	 g.exit();
	 printd(5, "Object::dereference() %08p data=%08p status=%d\n", this, data, status);
      }
      tDeref();
   }
}

// this method is called when there is an exception in a constructor and the object should be deleted
void Object::obliterate(ExceptionSink *xsink)
{
   printd(5, "Object::obliterate(this=%08p) class=%s %d->%d\n", this, myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      g.enter();
      printd(5, "Object::obliterate() class=%s deleting this=%08p\n", myclass->getName(), this);
      if (status == OS_OK)
      {
	 status = OS_DELETED;
	 Hash *td = data;
	 data = NULL;
	 g.exit();

	 if (privateData)
	    privateData->derefAll();

	 cleanup(xsink, td);
      }
      else
      {
	 g.exit();
	 printd(5, "Object::obliterate() %08p data=%08p status=%d\n", this, data, status);
      }
      tDeref();
   }
}

void Object::doDelete(class ExceptionSink *xsink)
{
   g.enter();
   doDeleteIntern(xsink);
}

// NOTE: caller must unlock
class QoreNode **Object::getMemberValuePtr(char *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->add(&g);
   return data->getKeyValuePtr(key);
}

// NOTE: caller must unlock
class QoreNode **Object::getMemberValuePtr(QoreString *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->add(&g);
   return data->getKeyValuePtr(key, xsink);
}

// NOTE: caller must unlock
class QoreNode *Object::getMemberValueNoMethod(QoreString *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
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
class QoreNode *Object::getMemberValueNoMethod(char *key, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
      return NULL;
   }

   QoreNode *rv = data->getKeyValue(key);
   if (rv)
      vl->add(&g);
   else
      g.exit();
   return rv;
}

void Object::deleteMemberValue(QoreString *key, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
      return;
   }

   data->deleteKey(key, xsink);
   g.exit();
}

void Object::deleteMemberValue(char *key, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
      return;
   }

   data->deleteKey(key, xsink);
   g.exit();
}

class List *Object::getMemberList(class ExceptionSink *xsink)
{
   class List *lst;
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return NULL;
   }
   lst = data->getKeys();
   g.exit();
   return lst;
}

void Object::setValue(char *key, class QoreNode *value, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
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
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
      return;
   }
   data->setKeyValue(key->getBuffer(), value, xsink);
   g.exit();
}
*/

int Object::size()
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
void Object::merge(class Hash *h, ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return;
   }
   data->merge(h, xsink);
   g.exit();
}

// adds all elements (already referenced) from the hash passed, deletes the
// hash passed
void Object::assimilate(class Hash *h, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return;
   }
   data->assimilate(h, xsink);
   g.exit();
}

/*
// to be called only in builtin constructors - no locking necessary
inline void Object::doDeleteNoDestructor(class ExceptionSink *xsink)
{
   status = OS_DELETED;
   data->dereference(xsink);
}
*/

class QoreNode *Object::evalFirstKeyValue(class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, myclass->getName());
      return NULL;
   }
   class QoreNode *rv = data->evalFirstKeyValue(xsink);
   g.exit();
   return rv;
}

class QoreNode *Object::evalMemberNoMethod(char *mem, class ExceptionSink *xsink)
{
   printd(5, "Object::evalMemberNoMethod(this=%08p, mem=%08p (%s), xsink=%08p, data->size()=%d)\n",
	  this, mem, mem, xsink, data ? data->size() : -1);
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, myclass->getName());
      return NULL;
   }
   class QoreNode *rv = data->evalKey(mem, xsink);
   g.exit();
   return rv;
}

// it's OK to return NULL here to duplicate the behaviour of NOTHING
class QoreNode *Object::evalMemberExistence(char *mem, class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, myclass->getName());
      return NULL;
   }
   class QoreNode *rv = data->evalKeyExistence(mem, xsink);
   g.exit();
   return rv;
}

class Hash *Object::evalData(class ExceptionSink *xsink)
{
   g.enter();
   if (status == OS_DELETED)
   {
      g.exit();
      // need to return an empty hash here
      return new Hash();
   }
   class Hash *rv = data->eval(xsink);
   g.exit();
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
class QoreNode **Object::getExistingValuePtr(class QoreString *mem, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem->getBuffer(), myclass->getName());
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
class QoreNode **Object::getExistingValuePtr(char *mem, class VLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(vl, xsink))
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, mem, myclass->getName());
      return NULL;
   }
   class QoreNode **rv = data->getExistingValuePtr(mem);
   if (!rv)
      g.exit();
   else
      vl->add(&g);
   return rv;
}

class KeyNode *Object::getReferencedPrivateDataNode(int key) 
{ 
   class KeyNode *rv = NULL;

   g.enter();
   if (status != OS_DELETED && privateData)
      rv = privateData->getReferencedPrivateDataNode(key);
   g.exit();
   return rv;
}

void *Object::getReferencedPrivateData(int key)
{ 
   void *rv = NULL;

   g.enter();
   if (status != OS_DELETED && privateData)
      rv = privateData->getReferencedPrivateData(key);
   g.exit();

   return rv;
}

void *Object::getAndClearPrivateData(int key)
{ 
   void *rv = NULL;
   g.enter();
   if (privateData)
   {
      class KeyNode *kn = privateData->find(key);
      if (kn)
	 rv = kn->getAndClearPtr();
   }
   g.exit(); 
   return rv;
}

void Object::setPrivate(int key, void *pd, q_private_t pdref, q_private_t pdderef)
{ 
   g.enter();
   if (!privateData)
      privateData = new KeyList();
   privateData->insert(key, pd, pdref, pdderef);
   g.exit();
}

void Object::addPrivateDataToString(class QoreString *str)
{
   str->concat('(');
   g.enter();
   if (status == OS_OK && privateData)
   {
      privateData->addToString(str);
      str->terminate(str->strlen() - 2);
   }
   else
      str->concat("<NO PRIVATE DATA>");
   g.exit();
   str->concat(')');
}

void Object::cleanup(class ExceptionSink *xsink, class Hash *td)
{
   if (privateData)
   {
      delete privateData;
#ifdef DEBUG
      privateData = NULL;
#endif
   }
   
   if (pgm)
   {
      printd(5, "Object::cleaup() this=%08p (%s) calling QoreProgram::depDeref() (%08p)\n", this, myclass->getName(), pgm);
      pgm->depDeref(xsink);
#ifdef DEBUG
      pgm = NULL;
#endif
   }
   
   td->derefAndDelete(xsink);
}

