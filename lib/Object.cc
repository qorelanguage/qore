/*
  Object.cc

  thread-safe object definition

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

#include <qore/Qore.h>
#include <qore/BuiltinMethod.h>

#include <stdlib.h>
#include <assert.h>

#include <map>

// actual object data and set of super/metaclass IDs
typedef std::pair<AbstractPrivateData *, int_set_t *> abstract_data_record_t;
// mapping from qore class ID to the object data and super/metaclass ID set
typedef std::map<int, abstract_data_record_t> keymap_t;
// mapping from qore super/metaclass ID to object data
typedef std::multimap<int, AbstractPrivateData *> metakeymap_t;

// for objects with multiple classes, private data has to be keyed
class KeyList
{
   private:
      keymap_t keymap;
      metakeymap_t metakeymap;

   public:
      DLLLOCAL AbstractPrivateData *getReferencedPrivateData(int key) const
      {
	 keymap_t::const_iterator i = keymap.find(key);
	 if (i == keymap.end())
	    return 0;

	 AbstractPrivateData *apd = i->second.first;
	 apd->ref();
	 return apd;
      }

      DLLLOCAL AbstractPrivateData *getReferencedPrivateDataFromMetaClass(int metakey) const
      {
	 metakeymap_t::const_iterator i = metakeymap.find(metakey);
	 if (i == metakeymap.end())
	    return getReferencedPrivateData(metakey);

	 AbstractPrivateData *apd = i->second;
	 apd->ref();
	 return apd;
      }

      DLLLOCAL void addToString(class QoreString *str) const
      {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	    str->sprintf("%d=<0x%08p>, ", i->first, i->second.first);
      }

      DLLLOCAL void derefAll(class ExceptionSink *xsink) const
      {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	 {
	    i->second.first->deref(xsink);
	    if (i->second.second)
	       delete i->second.second;
	 }
      }

      DLLLOCAL AbstractPrivateData *getAndClearPtr(int key)
      {
	 keymap_t::iterator i = keymap.find(key);
	 if (i == keymap.end())
	    return 0;

	 class AbstractPrivateData *rv = i->second.first;
	 // if there are meta mappings, then delete them too
	 if (i->second.second)
	 {
	    for (int_set_t::iterator mi = i->second.second->begin(), e = i->second.second->end(); mi != e; ++mi)
	    {
	       metakeymap_t::iterator j = metakeymap.find(*mi);
	       assert(j != metakeymap.end());
	       while (j != metakeymap.end() && j->first == *mi)
		  if (j->second == rv) {
		     metakeymap.erase(j);
		     break;
		  }
		  else
		     ++j;
	    }
	    delete i->second.second;
	 }
	 keymap.erase(i);
	 return rv;
      }

      DLLLOCAL void insert(int key, AbstractPrivateData *pd)
      {
	 keymap.insert(std::make_pair(key, std::make_pair(pd, (int_set_t *)0)));
      }

      DLLLOCAL void insert(int key, int metakey, AbstractPrivateData *pd)
      {
	 assert(metakey);
	 // make new meta key set with one key
	 int_set_t *metakeyset = new int_set_t;
	 metakeyset->insert(metakey);
	 keymap.insert(std::make_pair(key, std::make_pair(pd, metakeyset)));

	 metakeymap.insert(std::make_pair(metakey, pd));
      }

      DLLLOCAL void insert(int key, int_set_t *metakeyset, AbstractPrivateData *pd)
      {
	 keymap.insert(std::make_pair(key, std::make_pair(pd, metakeyset)));

	 for (int_set_t::iterator i = metakeyset->begin(), e = metakeyset->end(); i != e; ++i)
	 {
	    assert(*i);
	    metakeymap.insert(std::make_pair(*i, pd));
	 }
      }

};

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
   system_object = !pgm;
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
   assert(!pgm);
   assert(!data);
   assert(!privateData);
   //traceout("Object::~Object()");
}

class QoreClass *Object::getClass() const 
{ 
   return myclass; 
}

int Object::getStatus() const 
{ 
   return status; 
}

int Object::isValid() const 
{ 
   return status == OS_OK; 
}

class QoreProgram *Object::getProgram() const
{
   return pgm;
}

bool Object::isSystemObject() const
{
   return system_object;
}

void Object::tRef()
{
   printd(5, "Object::tRef(this=%08p) tref %d->%d\n", this, tRefs.reference_count(), tRefs.reference_count() + 1);
   tRefs.ROreference();
}

void Object::tDeref()
{
   printd(5, "Object::tDeref(this=%08p) tref %d->%d\n", this, tRefs.reference_count(), tRefs.reference_count() - 1);
   if (tRefs.ROdereference())
      delete this;
}

class QoreNode *Object::evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, class QoreNode *args, class ExceptionSink *xsink)
{
   // get referenced object
   class AbstractPrivateData *pd = getReferencedPrivateData(meth->myclass->getIDForMethod(), xsink);
   
   if (pd)
   {
      class QoreNode *rv = meth->evalMethod(this, pd, args, xsink);
      pd->deref(xsink);
      return rv;
   }

   if (xsink->isException())
      return NULL;
   if (myclass == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", myclass->getName(), meth->getName());
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of object's class '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), meth->getName(), myclass->getName());
   return NULL;
}

void Object::evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class Object *self, const char *class_name, class ExceptionSink *xsink)
{
   // get referenced object
   class AbstractPrivateData *pd = getReferencedPrivateData(meth->myclass->getID(), xsink);
   
   if (pd)
   {
      meth->evalCopy(self, this, pd, class_name, xsink);
      pd->deref(xsink);
      return;
   }

   if (xsink->isException())
      return;
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
   TempEncodingHelper tstr(member->val.String, QCS_DEFAULT, xsink);
   if (!tstr)
      return NULL;

   class QoreNode *rv = NULL;
   const char *mem = tstr->getBuffer();
   
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
	 xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, myclass->getName());
   }
   else
   {
      if (g.enter(xsink) >= 0)
      {
	 if (status == OS_DELETED)
	    g.exit();
	 else
	 {
	    rv = data->evalKeyExistence(mem, xsink);
	    g.exit();
	    
	    // execute memberGate method for objects where no member exists
	    if (rv == (QoreNode *)-1)
	       rv = myclass->evalMemberGate(this, member, xsink);
	 }
      }
   }

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

   if (g.enter(xsink) < 0)
      return 1;

   if (obj->g.enter(xsink) < 0)
   {
      g.exit();
      return 1;
   }
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
bool Object::compareHard(class Object *obj, class ExceptionSink *xsink)
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

   if (g.enter(xsink) < 0)
      return 1;
   if (obj->g.enter(xsink) < 0)
   {
      g.exit();
      return 1;
   }

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
      rc = data->compareHard(obj->data, xsink);

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

   // FIXME: what the hell do we do if this happens?
   if (g.enter(xsink) >= 0)
   {
      status = OS_DELETED;
      Hash *td = data;
      data = NULL;
      g.exit();
      cleanup(xsink, td);
   }
}

// does a deep dereference and execs destructor if necessary
void Object::dereference(ExceptionSink *xsink)
{
   printd(5, "Object::dereference(this=%08p) class=%s %d->%d\n", this, myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      // FIXME: what the hell do we do if this happens?
      if (g.enter(xsink) >= 0)
      {
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
}

// this method is called when there is an exception in a constructor and the object should be deleted
void Object::obliterate(ExceptionSink *xsink)
{
   printd(5, "Object::obliterate(this=%08p) class=%s %d->%d\n", this, myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      // FIXME: what the hell do we do if this happens?
      if (g.enter(xsink) >= 0)
      {
	 printd(5, "Object::obliterate() class=%s deleting this=%08p\n", myclass->getName(), this);
	 if (status == OS_OK)
	 {
	    status = OS_DELETED;
	    Hash *td = data;
	    data = NULL;
	    g.exit();
	    
	    if (privateData)
	       privateData->derefAll(xsink);

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
}

void Object::doDelete(class ExceptionSink *xsink)
{
   // FIXME: what the hell do we do if this happens?
   if (g.enter(xsink) < 0)
      return;
   doDeleteIntern(xsink);
}

// NOTE: caller must unlock
class QoreNode **Object::getMemberValuePtr(const char *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->push(&g);
   return data->getKeyValuePtr(key);
}

// NOTE: caller must unlock
class QoreNode **Object::getMemberValuePtr(QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      return NULL;
   }

   vl->push(&g);
   return data->getKeyValuePtr(key, xsink);
}

// NOTE: caller must unlock
class QoreNode *Object::getMemberValueNoMethod(QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
      return NULL;
   }

   QoreNode *rv = data->getKeyValue(key, xsink);
   if (!rv)
      g.exit();
   else
      vl->push(&g);
   return rv;
}

// NOTE: caller must unlock
class QoreNode *Object::getMemberValueNoMethod(const char *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return NULL;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
      return NULL;
   }

   QoreNode *rv = data->getKeyValue(key);
   if (!rv)
      g.exit();
   else
      vl->push(&g);
   return rv;
}

void Object::deleteMemberValue(QoreString *key, ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return;

   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), myclass->getName());
      return;
   }

   data->deleteKey(key, xsink);
   g.exit();
}

void Object::deleteMemberValue(const char *key, ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return;
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
   if (g.enter(xsink) < 0)
      return NULL;
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

void Object::setValue(const char *key, class QoreNode *value, ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return;
   if (status == OS_DELETED)
   {
      g.exit();
      makeAccessDeletedObjectException(xsink, key, myclass->getName());
      return;
   }
   data->setKeyValue(key, value, xsink);
   g.exit();
}

int Object::size(class ExceptionSink *xsink)
{
   int rc;

   if (g.enter(xsink) < 0)
      return 0;
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
   if (g.enter(xsink) < 0)
      return;
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
   if (g.enter(xsink) < 0)
      return;
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
   if (g.enter(xsink) < 0)
      return NULL;
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

class QoreNode *Object::evalMemberNoMethod(const char *mem, class ExceptionSink *xsink)
{
   printd(5, "Object::evalMemberNoMethod(this=%08p, mem=%08p (%s), xsink=%08p, data->size()=%d)\n",
	  this, mem, mem, xsink, data ? data->size() : -1);
   if (g.enter(xsink) < 0)
      return NULL;
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
class QoreNode *Object::evalMemberExistence(const char *mem, class ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
      return NULL;
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
   if (g.enter(xsink) < 0)
      return NULL;
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
class QoreNode **Object::getExistingValuePtr(class QoreString *mem, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
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
      vl->push(&g);
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
class QoreNode **Object::getExistingValuePtr(const char *mem, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (g.enter(xsink) < 0)
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
      vl->push(&g);
   return rv;
}

AbstractPrivateData *Object::getReferencedPrivateData(int key, class ExceptionSink *xsink)
{ 
   AbstractPrivateData *rv = NULL;

   if (g.enter(xsink) < 0)
      return NULL;
   if (status != OS_DELETED && privateData)
      rv = privateData->getReferencedPrivateData(key);
   g.exit();

   return rv;
}

AbstractPrivateData *Object::getReferencedPrivateDataFromMetaClass(int metakey, class ExceptionSink *xsink)
{ 
   AbstractPrivateData *rv = NULL;

   if (g.enter(xsink) < 0)
      return NULL;
   if (status != OS_DELETED && privateData)
      rv = privateData->getReferencedPrivateDataFromMetaClass(metakey);
   g.exit();

   return rv;
}

AbstractPrivateData *Object::getAndClearPrivateData(int key, class ExceptionSink *xsink)
{ 
   AbstractPrivateData *rv = NULL;
   if (g.enter(xsink) < 0)
      return NULL;
   if (privateData)
      rv = privateData->getAndClearPtr(key);
   g.exit(); 
   return rv;
}

// called only during constructor execution, therefore no need for locking
void Object::setPrivate(int key, AbstractPrivateData *pd)
{ 
   if (!privateData)
      privateData = new KeyList();
   privateData->insert(key, pd);
}

void Object::setPrivate(int key, int metakey, AbstractPrivateData *pd)
{ 
   if (!privateData)
      privateData = new KeyList();
   privateData->insert(key, metakey, pd);
}

void Object::setPrivate(int key, int_set_t *metakeyset, AbstractPrivateData *pd)
{ 
   if (!privateData)
      privateData = new KeyList();
   privateData->insert(key, metakeyset, pd);
}

void Object::addPrivateDataToString(class QoreString *str, class ExceptionSink *xsink)
{
   str->concat('(');
   if (g.enter(xsink) < 0)
      return;
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
      printd(5, "Object::cleanup() this=%08p (%s) calling QoreProgram::depDeref() (%08p)\n", this, myclass->getName(), pgm);
      pgm->depDeref(xsink);
#ifdef DEBUG
      pgm = NULL;
#endif
   }
   
   td->derefAndDelete(xsink);
}

void Object::defaultSystemDestructor(int classID, class ExceptionSink *xsink)
{
   AbstractPrivateData *pd = getAndClearPrivateData(classID, xsink);
   printd(5, "Object::defaultSystemDestructor() this=%08p class=%s private_data=%08p\n", this, myclass->getName(), pd); 
   if (pd)
      pd->deref(xsink);
}
