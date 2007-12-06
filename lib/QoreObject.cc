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
#include <qore/intern/BuiltinMethod.h>

#include <stdlib.h>
#include <assert.h>

#include <map>

// if the second part of the pair is true, then the data is virtual
typedef std::pair<AbstractPrivateData *, bool> private_pair_t;
// mapping from qore class ID to the object data
typedef std::map<int, private_pair_t> keymap_t;

struct qore_object_private {
      class QoreClass *myclass;
      int status;
      bool system_object;
      class VRMutex g;
      class KeyList *privateData;
      class ReferenceObject tRefs;  // reference-references
      class QoreHash *data;
      class QoreProgram *pgm;

      DLLLOCAL qore_object_private(class QoreClass *oc, class QoreProgram *p, QoreHash *n_data) : 
	 myclass(oc), status(OS_OK), system_object(!p), privateData(0), data(n_data), pgm(p)
      {
	 printd(5, "QoreObject::QoreObject() this=%08p, pgm=%08p, class=%s, refs 0->1\n", this, p, oc->getName());
	 // instead of referencing the class, we reference the program, because the
	 // program contains the namespace that contains the class, and the class'
	 // methods may call functions in the program as well that could otherwise
	 // disappear when the program is deleted
	 if (p)
	 {
	    printd(5, "QoreObject::init() this=%08p (%s) calling QoreProgram::depRef() (%08p)\n", this, myclass->getName(), p);
	    p->depRef();
	 }
      }
      DLLLOCAL ~qore_object_private()
      {
	 assert(!pgm);
	 assert(!data);
	 assert(!privateData);
      }
};

// for objects with multiple classes, private data has to be keyed
class KeyList
{
   private:
      keymap_t keymap;

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

      DLLLOCAL void addToString(class QoreString *str) const
      {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	    str->sprintf("%d=<0x%08p>, ", i->first, i->second.first);
      }

      DLLLOCAL void derefAll(class ExceptionSink *xsink) const
      {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	    if (!i->second.second)
	       i->second.first->deref(xsink);
      }

      DLLLOCAL AbstractPrivateData *getAndClearPtr(int key)
      {
	 keymap_t::iterator i = keymap.find(key);
	 if (i == keymap.end())
	    return 0;

	 assert(!i->second.second);
	 class AbstractPrivateData *rv = i->second.first;
	 keymap.erase(i);
	 return rv;
      }

      DLLLOCAL void insert(int key, AbstractPrivateData *pd)
      {
	 keymap.insert(std::make_pair(key, std::make_pair(pd, false)));
      }

      DLLLOCAL void insertVirtual(int key, AbstractPrivateData *pd)
      {
	 if (keymap.find(key) == keymap.end())
	    keymap.insert(std::make_pair(key, std::make_pair(pd, true)));
      }

};

QoreObject::QoreObject(class QoreClass *oc, class QoreProgram *p) : priv(new qore_object_private(oc, p, new QoreHash()))
{
}

QoreObject::QoreObject(class QoreClass *oc, class QoreProgram *p, class QoreHash *h) : priv(new qore_object_private(oc, p, h))
{
}

QoreObject::~QoreObject()
{
   //tracein("QoreObject::~QoreObject()");
   printd(5, "QoreObject::~QoreObject() this=%08p, pgm=%08p, class=%s\n", this, priv->pgm, priv->myclass->getName());
   delete priv;
   //traceout("QoreObject::~QoreObject()");
}

class QoreClass *QoreObject::getClass() const 
{ 
   return priv->myclass; 
}

int QoreObject::getStatus() const 
{ 
   return priv->status; 
}

int QoreObject::isValid() const 
{ 
   return priv->status == OS_OK; 
}

class QoreProgram *QoreObject::getProgram() const
{
   return priv->pgm;
}

bool QoreObject::isSystemObject() const
{
   return priv->system_object;
}

void QoreObject::tRef()
{
   printd(5, "QoreObject::tRef(this=%08p) class=%s, tref %d->%d\n", this, priv->myclass->getName(), priv->tRefs.reference_count(), priv->tRefs.reference_count() + 1);
   priv->tRefs.ROreference();
}

void QoreObject::tDeref()
{
   printd(5, "QoreObject::tDeref(this=%08p) class=%s, tref %d->%d\n", this, priv->myclass->getName(), priv->tRefs.reference_count(), priv->tRefs.reference_count() - 1);
   if (priv->tRefs.ROdereference())
      delete this;
}

class QoreNode *QoreObject::evalBuiltinMethodWithPrivateData(class BuiltinMethod *meth, class QoreNode *args, class ExceptionSink *xsink)
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
   if (priv->myclass == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", priv->myclass->getName(), meth->getName());
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), meth->getName(), priv->myclass->getName());
   return NULL;
}

void QoreObject::evalCopyMethodWithPrivateData(class BuiltinMethod *meth, class QoreObject *self, const char *class_name, class ExceptionSink *xsink)
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
   if (priv->myclass == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::copy() cannot be executed because the object has already been deleted", priv->myclass->getName());
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::copy() (base class of '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), priv->myclass->getName());
}

void QoreObject::instantiateLVar(lvh_t id)
{
   ref();
   ::instantiateLVar(id, new QoreNode(this));
}

void QoreObject::uninstantiateLVar(class ExceptionSink *xsink)
{
   ::uninstantiateLVar(xsink);
}

void QoreObject::ref()
{
   printd(5, "QoreObject::ref(this=%08p) class=%s, %d->%d\n", this, priv->myclass->getName(), references, references + 1);
   ROreference();   // increment destructor-relevant references
}

bool QoreObject::validInstanceOf(int cid) const
{
   if (priv->status == OS_DELETED)
      return 0;

   return priv->myclass->getClass(cid);
}

class QoreNode *QoreObject::evalMethod(class QoreString *name, class QoreNode *args, class ExceptionSink *xsink)
{
   return priv->myclass->evalMethod(this, name->getBuffer(), args, xsink);
}

class QoreClass *QoreObject::getClass(int cid) const
{
   if (cid == priv->myclass->getID())
      return priv->myclass;
   return priv->myclass->getClass(cid);
}

class QoreNode *QoreObject::evalMember(class QoreNode *member, class ExceptionSink *xsink)
{
   // make sure to convert string encoding if necessary to default character set
   TempEncodingHelper tstr(member->val.String, QCS_DEFAULT, xsink);
   if (!tstr)
      return NULL;

   class QoreNode *rv = NULL;
   const char *mem = tstr->getBuffer();
   
   //printd(5, "QoreObject::evalMember() find_key(%s)=%08p myclass=%s\n", mem, find_key(mem), myclass ? myclass->getName() : "NONE");
   // if accessed outside the class and the member is a private member 
   class QoreObject *obj = getStackObject();
   printd(5, "QoreObject::evalMember(%s) obj=%08p class=%s ID=%d stack obj=%08p class=%s ID=%d isPrivateMember=%s\n", mem, this, priv->myclass->getName(), priv->myclass->getID(), obj, obj ? obj->priv->myclass->getName() : "(null)", obj ? obj->priv->myclass->getID() : -1, priv->myclass->isPrivateMember(mem) ? "true" : "false");
	  
   // if accessed outside the class and the member is a private member 
   if ((!obj || (obj && obj->priv->myclass->getID() != priv->myclass->getID())) && priv->myclass->isPrivateMember(mem))
   {
      if (priv->myclass->hasMemberGate()) // execute the member gate if it exists for private members
	 rv = priv->myclass->evalMemberGate(this, member, xsink);
      else
	 xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, priv->myclass->getName());
   }
   else
   {
      if (priv->g.enter(xsink) >= 0)
      {
	 if (priv->status == OS_DELETED)
	    priv->g.exit();
	 else
	 {
	    rv = priv->data->evalKeyExistence(mem, xsink);
	    priv->g.exit();
	    
	    // execute memberGate method for objects where no member exists
	    if (rv == (QoreNode *)-1)
	       rv = priv->myclass->evalMemberGate(this, member, xsink);
	 }
      }
   }

   return rv;
}

// 0 = equal, 1 = not equal
bool QoreObject::compareSoft(class QoreObject *obj, class ExceptionSink *xsink)
{
   if (obj->priv->myclass != priv->myclass)
      return 1;

   // to avoid deadlocks when an object is compared with itself
   if (this == obj)
      return 0;

   // objects are not equal if they have private data and they
   // do not point to the same objects
   if (priv->privateData != obj->priv->privateData)
      return 1;

   int rc;

   if (priv->g.enter(xsink) < 0)
      return 1;

   if (obj->priv->g.enter(xsink) < 0)
   {
      priv->g.exit();
      return 1;
   }
   if (priv->status == OS_DELETED)
   {
      if (obj->priv->status == OS_DELETED)
	 rc = 0;
      else
	 rc = 1;
   }
   else if (obj->priv->status == OS_DELETED)
      rc = 1;
   else
      rc = priv->data->compareSoft(obj->priv->data, xsink);

   obj->priv->g.exit();
   priv->g.exit();
   return rc;
}

// 0 = equal, 1 = not equal
bool QoreObject::compareHard(class QoreObject *obj, class ExceptionSink *xsink)
{
   if (obj->priv->myclass != priv->myclass)
      return 1;

   // to avoid deadlocks when an object is compared with itself
   if (this == obj)
      return 0;

   // objects are not equal if they have private data and they
   // do not point to the same objects
   if (priv->privateData != obj->priv->privateData)
      return 1;

   int rc;

   if (priv->g.enter(xsink) < 0)
      return 1;
   if (obj->priv->g.enter(xsink) < 0)
   {
      priv->g.exit();
      return 1;
   }

   if (priv->status == OS_DELETED)
   {
      if (obj->priv->status == OS_DELETED)
	 rc = 0;
      else
	 rc = 1;
   }
   else if (obj->priv->status == OS_DELETED)
      rc = 1;
   else
      rc = priv->data->compareHard(obj->priv->data, xsink);

   obj->priv->g.exit();
   priv->g.exit();
   return rc;
}

// gate is already held
inline void QoreObject::doDeleteIntern(class ExceptionSink *xsink)
{
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      return;
   }
   if (priv->status == OS_BEING_DELETED)
   {
      priv->g.exit();
      xsink->raiseException("DOUBLE-DELETE-EXCEPTION", "destructor called from within destructor");
      return;
   }
   priv->status = OS_BEING_DELETED;
   priv->g.exit();
   
   printd(5, "QoreObject::doDelete(this=%08p) calling destructor()\n", this);   
   priv->myclass->execDestructor(this, xsink);

   // FIXME: what the hell do we do if this happens?
   if (priv->g.enter(xsink) >= 0)
   {
      priv->status = OS_DELETED;
      QoreHash *td = priv->data;
      priv->data = NULL;
      priv->g.exit();
      cleanup(xsink, td);
   }
}

// does a deep dereference and execs destructor if necessary
void QoreObject::dereference(ExceptionSink *xsink)
{
   printd(5, "QoreObject::dereference(this=%08p) class=%s %d->%d\n", this, priv->myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      // FIXME: what the hell do we do if this happens?
      if (priv->g.enter(xsink) >= 0)
      {
	 printd(5, "QoreObject::dereference() class=%s deleting this=%08p\n", priv->myclass->getName(), this);
	 if (priv->status == OS_OK)
	 {
	    // reference for destructor
	    ROreference();
	    doDeleteIntern(xsink);
	    ROdereference();
	 }
	 else
	 {
	    priv->g.exit();
	    printd(5, "QoreObject::dereference() %08p class=%s data=%08p status=%d\n", this, priv->myclass->getName(), priv->data, priv->status);
	 }
	 tDeref();
      }
   }
}

// this method is called when there is an exception in a constructor and the object should be deleted
void QoreObject::obliterate(ExceptionSink *xsink)
{
   printd(5, "QoreObject::obliterate(this=%08p) class=%s %d->%d\n", this, priv->myclass->getName(), references, references - 1);
   if (ROdereference())
   {
      // FIXME: what the hell do we do if this happens?
      if (priv->g.enter(xsink) >= 0)
      {
	 printd(5, "QoreObject::obliterate() class=%s deleting this=%08p\n", priv->myclass->getName(), this);
	 if (priv->status == OS_OK)
	 {
	    priv->status = OS_DELETED;
	    QoreHash *td = priv->data;
	    priv->data = NULL;
	    priv->g.exit();
	    
	    if (priv->privateData)
	       priv->privateData->derefAll(xsink);

	    cleanup(xsink, td);
	 }
	 else
	 {
	    priv->g.exit();
	    printd(5, "QoreObject::obliterate() %08p data=%08p status=%d\n", this, priv->data, priv->status);
	 }
	 tDeref();
      }
   }
}

void QoreObject::doDelete(class ExceptionSink *xsink)
{
   // FIXME: what the hell do we do if this happens?
   if (priv->g.enter(xsink) < 0)
      return;
   doDeleteIntern(xsink);
}

// NOTE: caller must unlock
class QoreNode **QoreObject::getMemberValuePtr(const char *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;

   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      return NULL;
   }

   vl->push(&priv->g);
   return priv->data->getKeyValuePtr(key);
}

// NOTE: caller must unlock
class QoreNode **QoreObject::getMemberValuePtr(QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;

   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      return NULL;
   }

   vl->push(&priv->g);
   return priv->data->getKeyValuePtr(key, xsink);
}

// NOTE: caller must unlock
class QoreNode *QoreObject::getMemberValueNoMethod(QoreString *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;

   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), priv->myclass->getName());
      return NULL;
   }

   QoreNode *rv = priv->data->getKeyValue(key, xsink);
   if (!rv)
      priv->g.exit();
   else
      vl->push(&priv->g);
   return rv;
}

// NOTE: caller must unlock
class QoreNode *QoreObject::getMemberValueNoMethod(const char *key, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;

   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, key, priv->myclass->getName());
      return NULL;
   }

   QoreNode *rv = priv->data->getKeyValue(key);
   if (!rv)
      priv->g.exit();
   else
      vl->push(&priv->g);
   return rv;
}

void QoreObject::deleteMemberValue(QoreString *key, ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return;

   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, key->getBuffer(), priv->myclass->getName());
      return;
   }

   priv->data->deleteKey(key, xsink);
   priv->g.exit();
}

void QoreObject::deleteMemberValue(const char *key, ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, key, priv->myclass->getName());
      return;
   }

   priv->data->deleteKey(key, xsink);
   priv->g.exit();
}

class QoreList *QoreObject::getMemberList(class ExceptionSink *xsink)
{
   class QoreList *lst;
   if (priv->g.enter(xsink) < 0)
      return NULL;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, priv->myclass->getName());
      return NULL;
   }
   lst = priv->data->getKeys();
   priv->g.exit();
   return lst;
}

void QoreObject::setValue(const char *key, class QoreNode *value, ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, key, priv->myclass->getName());
      return;
   }
   priv->data->setKeyValue(key, value, xsink);
   priv->g.exit();
}

int QoreObject::size(class ExceptionSink *xsink)
{
   int rc;

   if (priv->g.enter(xsink) < 0)
      return 0;
   if (priv->status == OS_DELETED)
      rc = 0;
   else
      rc = priv->data->size();
   priv->g.exit();
   return rc;
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
void QoreObject::merge(class QoreHash *h, ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, priv->myclass->getName());
      return;
   }
   priv->data->merge(h, xsink);
   priv->g.exit();
}

// adds all elements (already referenced) from the hash passed, deletes the
// hash passed
void QoreObject::assimilate(class QoreHash *h, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, priv->myclass->getName());
      return;
   }
   priv->data->assimilate(h, xsink);
   priv->g.exit();
}

/*
// to be called only in builtin constructors - no locking necessary
inline void QoreObject::doDeleteNoDestructor(class ExceptionSink *xsink)
{
   priv->status = OS_DELETED;
   data->dereference(xsink);
}
*/

class QoreNode *QoreObject::evalFirstKeyValue(class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, priv->myclass->getName());
      return NULL;
   }
   class QoreNode *rv = priv->data->evalFirstKeyValue(xsink);
   priv->g.exit();
   return rv;
}

class QoreNode *QoreObject::evalMemberNoMethod(const char *mem, class ExceptionSink *xsink)
{
   printd(5, "QoreObject::evalMemberNoMethod(this=%08p, mem=%08p (%s), xsink=%08p, data->size()=%d)\n",
	  this, mem, mem, xsink, priv->data ? priv->data->size() : -1);
   if (priv->g.enter(xsink) < 0)
      return NULL;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, mem, priv->myclass->getName());
      return NULL;
   }
   class QoreNode *rv = priv->data->evalKey(mem, xsink);
   priv->g.exit();
   return rv;
}

// it's OK to return NULL here to duplicate the behaviour of NOTHING
class QoreNode *QoreObject::evalMemberExistence(const char *mem, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, mem, priv->myclass->getName());
      return NULL;
   }
   class QoreNode *rv = priv->data->evalKeyExistence(mem, xsink);
   priv->g.exit();
   return rv;
}

class QoreHash *QoreObject::evalData(class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;
   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      // need to return an empty hash here
      return new QoreHash();
   }
   class QoreHash *rv = priv->data->eval(xsink);
   priv->g.exit();
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
class QoreNode **QoreObject::getExistingValuePtr(class QoreString *mem, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;

   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, mem->getBuffer(), priv->myclass->getName());
      return NULL;
   }
   class QoreNode **rv = priv->data->getExistingValuePtr(mem, xsink);
   if (!rv)
      priv->g.exit();
   else
      vl->push(&priv->g);
   return rv;
}

// NOTE: caller must unlock lock
// we check if the object is already locked
class QoreNode **QoreObject::getExistingValuePtr(const char *mem, class AutoVLock *vl, class ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return NULL;

   if (priv->status == OS_DELETED)
   {
      priv->g.exit();
      makeAccessDeletedObjectException(xsink, mem, priv->myclass->getName());
      return NULL;
   }
   class QoreNode **rv = priv->data->getExistingValuePtr(mem);
   if (!rv)
      priv->g.exit();
   else
      vl->push(&priv->g);
   return rv;
}

AbstractPrivateData *QoreObject::getReferencedPrivateData(int key, class ExceptionSink *xsink)
{ 
   AbstractPrivateData *rv = NULL;

   if (priv->g.enter(xsink) < 0)
      return NULL;
   if (priv->status != OS_DELETED && priv->privateData)
      rv = priv->privateData->getReferencedPrivateData(key);
   priv->g.exit();

   return rv;
}

AbstractPrivateData *QoreObject::getAndClearPrivateData(int key, class ExceptionSink *xsink)
{ 
   AbstractPrivateData *rv = NULL;
   if (priv->g.enter(xsink) < 0)
      return NULL;
   if (priv->privateData)
      rv = priv->privateData->getAndClearPtr(key);
   priv->g.exit(); 
   return rv;
}

// add virtual IDs for private data to class list
void QoreObject::addVirtualPrivateData(AbstractPrivateData *apd)
{
   class BCSMList *sml = priv->myclass->getBCSMList();
   if (!sml)
      return;

   for (class_list_t::const_iterator i = sml->begin(), e = sml->end(); i != e; ++i)
      if ((*i).second)
	 priv->privateData->insertVirtual((*i).first->getID(), apd);
}

// called only during constructor execution, therefore no need for locking
void QoreObject::setPrivate(int key, AbstractPrivateData *pd)
{ 
   if (!priv->privateData)
      priv->privateData = new KeyList();
   priv->privateData->insert(key, pd);
   addVirtualPrivateData(pd);
}

void QoreObject::addPrivateDataToString(class QoreString *str, class ExceptionSink *xsink)
{
   str->concat('(');
   if (priv->g.enter(xsink) < 0)
      return;
   if (priv->status == OS_OK && priv->privateData)
   {
      priv->privateData->addToString(str);
      str->terminate(str->strlen() - 2);
   }
   else
      str->concat("<NO PRIVATE DATA>");
   priv->g.exit();
   str->concat(')');
}

void QoreObject::cleanup(class ExceptionSink *xsink, class QoreHash *td)
{
   if (priv->privateData)
   {
      delete priv->privateData;
#ifdef DEBUG
      priv->privateData = NULL;
#endif
   }
   
   if (priv->pgm)
   {
      printd(5, "QoreObject::cleanup() this=%08p (%s) calling QoreProgram::depDeref() (%08p)\n", this, priv->myclass->getName(), priv->pgm);
      priv->pgm->depDeref(xsink);
#ifdef DEBUG
      priv->pgm = NULL;
#endif
   }
   
   td->derefAndDelete(xsink);
}

void QoreObject::defaultSystemDestructor(int classID, class ExceptionSink *xsink)
{
   AbstractPrivateData *pd = getAndClearPrivateData(classID, xsink);
   printd(5, "QoreObject::defaultSystemDestructor() this=%08p class=%s private_data=%08p\n", this, priv->myclass->getName(), pd); 
   if (pd)
      pd->deref(xsink);
}
