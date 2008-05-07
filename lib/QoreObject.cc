/*
  Object.cc

  thread-safe object definition

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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
#include <qore/intern/QoreClassIntern.h>

#include <stdlib.h>
#include <assert.h>

#include <map>

#define OS_OK             0
#define OS_DELETED       -1

// if the second part of the pair is true, then the data is virtual
typedef std::pair<AbstractPrivateData *, bool> private_pair_t;
// mapping from qore class ID to the object data
typedef std::map<qore_classid_t, private_pair_t> keymap_t;

class KeyList;

struct qore_object_private {
      const QoreClass *myclass;
      int status;
      unsigned call_count;
      mutable VRMutex g;
      KeyList *privateData;
      QoreReferenceCounter tRefs;  // reference-references
      QoreHashNode *data;
      QoreProgram *pgm;
      bool system_object, delete_blocker_run;

      DLLLOCAL qore_object_private(const QoreClass *oc, QoreProgram *p, QoreHashNode *n_data) : 
	 myclass(oc), status(OS_OK), call_count(0), privateData(0), data(n_data), pgm(p),
	 system_object(!p), delete_blocker_run(false)
      {
	 printd(5, "QoreObject::QoreObject() this=%08p, pgm=%08p, class=%s, refs 0->1\n", this, p, oc->getName());
	 // instead of referencing the class, we reference the program, because the
	 // program contains the namespace that contains the class, and the class'
	 // methods may call functions in the program as well that could otherwise
	 // disappear when the program is deleted
	 if (p) {
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
      DLLLOCAL AbstractPrivateData *getReferencedPrivateData(qore_classid_t key) const
      {
	 keymap_t::const_iterator i = keymap.find(key);
	 if (i == keymap.end())
	    return 0;

	 AbstractPrivateData *apd = i->second.first;
	 apd->ref();
	 return apd;
      }

      DLLLOCAL void addToString(QoreString *str) const
      {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	    str->sprintf("%d=<0x%08p>, ", i->first, i->second.first);
      }

      DLLLOCAL void derefAll(ExceptionSink *xsink) const
      {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	    if (!i->second.second)
	       i->second.first->deref(xsink);
      }

      DLLLOCAL AbstractPrivateData *getAndClearPtr(qore_classid_t key)
      {
	 keymap_t::iterator i = keymap.find(key);
	 if (i == keymap.end())
	    return 0;

	 assert(!i->second.second);
	 AbstractPrivateData *rv = i->second.first;
	 keymap.erase(i);
	 return rv;
      }

      DLLLOCAL void insert(qore_classid_t key, AbstractPrivateData *pd)
      {
	 keymap.insert(std::make_pair(key, std::make_pair(pd, false)));
      }

      DLLLOCAL void insertVirtual(qore_classid_t key, AbstractPrivateData *pd)
      {
	 if (keymap.find(key) == keymap.end())
	    keymap.insert(std::make_pair(key, std::make_pair(pd, true)));
      }

};

class AutoVRMutex {
   protected:
      VRMutex *vrm;

      DLLLOCAL AutoVRMutex(VRMutex *n_vrm) : vrm(n_vrm) {}

   public:
      DLLLOCAL AutoVRMutex(VRMutex *n_vrm, ExceptionSink *xsink) : vrm(n_vrm)
      {
	 if (vrm->enter(xsink) < 0)
	    vrm = 0;
      }
      DLLLOCAL ~AutoVRMutex()
      {
	 if (vrm)
	    vrm->exit();
      }
      DLLLOCAL operator bool() const
      {
	 return vrm != 0;
      }
      DLLLOCAL void exit()
      {
	 vrm->exit();
	 vrm = 0;
      }
      DLLLOCAL void stay_locked()
      {
	 vrm = 0;
      }
};

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p) : AbstractQoreNode(NT_OBJECT, false, false), priv(new qore_object_private(oc, p, new QoreHashNode()))
{
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p, QoreHashNode *h) : AbstractQoreNode(NT_OBJECT, false, false), priv(new qore_object_private(oc, p, h))
{
}

QoreObject::~QoreObject()
{
   //tracein("QoreObject::~QoreObject()");
   //printd(5, "QoreObject::~QoreObject() this=%08p, pgm=%08p, class=%s\n", this, priv->pgm, priv->myclass->getName());
   delete priv;
}

int QoreObject::check_access(ExceptionSink *xsink) const
{
   if (!priv->status)
      return 0;

   QoreObject *o = getStackObject();
   if (isSystemObject() || o && o == this)
      return 0;

   int tid = gettid();
   //printd(5, "check_access() o=%08p this=%08p tid=%d status=%d\n", o, this, tid, priv->status);
   if (priv->status != tid)
      xsink->raiseException("OBJECT-ACCESS-ERROR", "this object (class '%s') is being deleted by TID %d and can no longer be accessed outside the class (access attempted from TID %d)", getClassName(), priv->status, tid);
   else
      xsink->raiseException("OBJECT-ACCESS-ERROR", "this object (class '%s') is being deleted and can no longer be accessed outside the class", getClassName(), priv->status);
   return -1;
}

const QoreClass *QoreObject::getClass() const 
{ 
   return priv->myclass; 
}

const char *QoreObject::getClassName() const 
{ 
   return priv->myclass->getName(); 
}

int QoreObject::getStatus() const 
{ 
   return priv->status; 
}

bool QoreObject::isValid() const 
{ 
   return priv->status == OS_OK; 
}

QoreProgram *QoreObject::getProgram() const
{
   return priv->pgm;
}

bool QoreObject::isSystemObject() const
{
   return priv->system_object;
}

void QoreObject::tRef() const
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

AbstractQoreNode *QoreObject::evalBuiltinMethodWithPrivateData(BuiltinMethod *meth, const QoreListNode *args, ExceptionSink *xsink)
{
   // get referenced object
   AbstractPrivateData *pd = getReferencedPrivateData(meth->myclass->getIDForMethod(), xsink);

   if (pd) {
      AbstractQoreNode *rv = meth->evalMethod(this, pd, args, xsink);
      pd->deref(xsink);
      return rv;
   }

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%08p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
   if (xsink->isException())
      return 0;
   if (priv->myclass == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", priv->myclass->getName(), meth->getName());
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), meth->getName(), priv->myclass->getName());
   return 0;
}

void QoreObject::evalCopyMethodWithPrivateData(BuiltinMethod *meth, QoreObject *self, const char *class_name, ExceptionSink *xsink)
{
   // get referenced object
   AbstractPrivateData *pd = getReferencedPrivateData(meth->myclass->getID(), xsink);

   if (pd) {
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

bool QoreObject::evalDeleteBlocker(BuiltinMethod *meth)
{
   // FIXME: eliminate reference counts for private data, private data should be destroyed after the destructor terminates

   // get referenced object
   ExceptionSink xsink;
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->myclass->getIDForMethod(), &xsink), &xsink);

   if (pd)
      return meth->evalDeleteBlocker(this, *pd);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%08p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
   return false;
}

bool QoreObject::validInstanceOf(qore_classid_t cid) const
{
   if (priv->status == OS_DELETED)
      return 0;

   return priv->myclass->getClass(cid);
}

AbstractQoreNode *QoreObject::evalMethod(const QoreString *name, const QoreListNode *args, ExceptionSink *xsink)
{
   return priv->myclass->evalMethod(this, name->getBuffer(), args, xsink);
}

const QoreClass *QoreObject::getClass(qore_classid_t cid) const
{
   if (cid == priv->myclass->getID())
      return priv->myclass;
   return priv->myclass->getClass(cid);
}

AbstractQoreNode *QoreObject::evalMember(const QoreString *member, ExceptionSink *xsink)
{
   // make sure to convert string encoding if necessary to default character set
   TempEncodingHelper tstr(member, QCS_DEFAULT, xsink);
   if (!tstr)
      return 0;

   const char *mem = tstr->getBuffer();
   
   //printd(5, "QoreObject::evalMember() find_key(%s)=%08p myclass=%s\n", mem, find_key(mem), myclass ? myclass->getName() : "NONE");
   // if accessed outside the class and the member is a private member 
   QoreObject *obj = getStackObject();
   printd(5, "QoreObject::evalMember(%s) obj=%08p class=%s ID=%d stack obj=%08p class=%s ID=%d isPrivateMember=%s\n", mem, this, priv->myclass->getName(), priv->myclass->getID(), obj, obj ? obj->priv->myclass->getName() : "(null)", obj ? obj->priv->myclass->getID() : -1, priv->myclass->isPrivateMember(mem) ? "true" : "false");

   // if accessed outside the class and the member is a private member 
   if ((!obj || (obj && obj->priv->myclass->getID() != priv->myclass->getID())) && priv->myclass->isPrivateMember(mem)) {
      if (priv->myclass->hasMemberGate()) // execute the member gate if it exists for private members
	 return priv->myclass->evalMemberGate(this, *tstr, xsink);
      xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, priv->myclass->getName());
      return 0;
   }

   AbstractQoreNode *rv;
   bool exists;
   {
      AutoVRMutex h(&priv->g, xsink);
      if (!h)
	 return 0;
      
      if (priv->status == OS_DELETED || check_access(xsink))
	 return 0;
      
      rv = priv->data->getReferencedKeyValue(mem, exists);
   }
   
   // execute memberGate method for objects where no member exists
   if (!exists)
      rv = priv->myclass->evalMemberGate(this, *tstr, xsink);

   return rv;
}

// 0 = equal, 1 = not equal
bool QoreObject::compareSoft(const QoreObject *obj, ExceptionSink *xsink) const
{
   // currently objects are only equal if they are the same object
   return !(this == obj);

#if 0
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

   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 1;

   if (check_access(xsink))
      return 1;

   AutoVRMutex vo(&obj->priv->g);
   if (!vo)
      return 1;

   if (obj->check_access(xsink))
      return 1;

   if (priv->status == OS_DELETED) {
      if (obj->priv->status == OS_DELETED)
	 rc = 0;
      else
	 rc = 1;
   }
   else if (obj->priv->status == OS_DELETED)
      return 1;

   return priv->data->compareSoft(obj->priv->data, xsink);
#endif
}

// 0 = equal, 1 = not equal
bool QoreObject::compareHard(const QoreObject *obj, ExceptionSink *xsink) const
{
   // currently objects are only equal if they are the same object
   return !(this == obj);
}

// gate is already held
void QoreObject::doDeleteIntern(ExceptionSink *xsink)
{
   if (priv->status == OS_DELETED) {
      priv->g.exit();
      return;
   }
   if (priv->status > 0) {
      priv->g.exit();
      xsink->raiseException("DOUBLE-DELETE-EXCEPTION", "destructor called from within destructor");
      return;
   }
   priv->status = gettid();
   priv->g.exit();

   printd(5, "QoreObject::doDelete(this=%08p) calling destructor()\n", this);   
   priv->myclass->execDestructor(this, xsink);

   // once the status is set with the TID where the destructor is being run
   // the VRMutex should not be able to be acquired from any other thread and
   // therefore there should be no possibility of a deadlock
   AutoVRMutex v(&priv->g, xsink);
   assert(v);

   priv->status = OS_DELETED;
   QoreHashNode *td = priv->data;
   priv->data = 0;
   cleanup(xsink, td);
}

// does a deep dereference and execs destructor if necessary
bool QoreObject::derefImpl(ExceptionSink *xsink)
{
   printd(5, "QoreObject::derefImpl() this=%08p, class=%s references=0 status=%d has_delete_blocker=%d delete_blocker_run=%d\n", this, getClassName(), priv->status, priv->myclass->has_delete_blocker(), priv->delete_blocker_run);

   // if the scope deletion is blocked, then do not run the destructor
   if (!priv->delete_blocker_run && priv->myclass->execDeleteBlocker(this, xsink)) {
      //printd(5, "QoreObject::derefImpl() this=%08p class=%s blocking delete\n", this, getClassName());
      priv->delete_blocker_run = true;
      return false;
   }

   // there cannot be a deadlock here because the object is going out of scope
#ifdef DEBUG
   assert(priv->g.enter(xsink) >= 0);
#else
   priv->g.enter(xsink);
#endif

   //printd(5, "QoreObject::derefImpl() class=%s this=%08p going out of scope\n", getClassName(), this);
   if (priv->status == OS_OK) {
      // FIXME: this is stupid
      // reference for destructor
      ROreference();
      doDeleteIntern(xsink);
      if (ROdereference())
	 tDeref();

      return false;
   }

   priv->g.exit();
   printd(5, "QoreObject::derefImpl() %08p class=%s data=%08p status=%d\n", this, getClassName(), priv->data, priv->status);
   assert(!reference_count());
   tDeref();

   return false;
}

// this method is called when there is an exception in a constructor and the object should be deleted
void QoreObject::obliterate(ExceptionSink *xsink)
{
   printd(5, "QoreObject::obliterate(this=%08p) class=%s %d->%d\n", this, priv->myclass->getName(), references, references - 1);

   if (ROdereference()) {
      AutoVRMutex v(&priv->g, xsink);
      // there cannot be a deadlock here because the object is going out of scope
      assert(v);

      printd(5, "QoreObject::obliterate() class=%s deleting this=%08p\n", priv->myclass->getName(), this);

      if (priv->status == OS_OK) {
	 priv->status = OS_DELETED;
	 QoreHashNode *td = priv->data;
	 priv->data = 0;
	 v.exit();

	 if (priv->privateData)
	    priv->privateData->derefAll(xsink);

	 cleanup(xsink, td);
      }
      else {
	 v.exit();
	 printd(5, "QoreObject::obliterate() %08p data=%08p status=%d\n", this, priv->data, priv->status);
      }
      tDeref();
   }
}

void QoreObject::doDelete(ExceptionSink *xsink)
{
   if (priv->g.enter(xsink) < 0)
      return;
   doDeleteIntern(xsink);
}

// NOTE: caller must unlock
AbstractQoreNode **QoreObject::getMemberValuePtr(const char *key, AutoVLock *vl, ExceptionSink *xsink) const
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (priv->status == OS_DELETED || check_access(xsink))
      return 0;

   vl->push(&priv->g);
   v.stay_locked();
   return priv->data->getKeyValuePtr(key);
}

// NOTE: caller must unlock
AbstractQoreNode **QoreObject::getMemberValuePtr(const QoreString *key, AutoVLock *vl, ExceptionSink *xsink) const
{
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValuePtr(enc->getBuffer(), vl, xsink);
}

// NOTE: caller must unlock
AbstractQoreNode *QoreObject::getMemberValueNoMethod(const QoreString *key, AutoVLock *vl, ExceptionSink *xsink) const
{
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValueNoMethod(enc->getBuffer(), vl, xsink);
}

// NOTE: caller must unlock (with AutoVLock)
AbstractQoreNode *QoreObject::getMemberValueNoMethod(const char *key, AutoVLock *vl, ExceptionSink *xsink) const
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (check_access(xsink))
      return 0;

   if (priv->status == OS_DELETED) {
      v.exit();
      makeAccessDeletedObjectException(xsink, key, priv->myclass->getName());
      return 0;
   }

   AbstractQoreNode *rv = priv->data->getKeyValue(key);
   if (rv && rv->isReferenceCounted()) {
      vl->push(&priv->g);
      v.stay_locked();
   }
   return rv;
}

void QoreObject::deleteMemberValue(const QoreString *key, ExceptionSink *xsink)
{
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   deleteMemberValue(enc->getBuffer(), xsink);
}

void QoreObject::deleteMemberValue(const char *key, ExceptionSink *xsink)
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return;

   if (check_access(xsink))
      return;

   if (priv->status == OS_DELETED) {
      v.exit();
      makeAccessDeletedObjectException(xsink, key, priv->myclass->getName());
      return;
   }

   priv->data->deleteKey(key, xsink);
}

QoreListNode *QoreObject::getMemberList(ExceptionSink *xsink) const
{
   QoreListNode *lst;
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (check_access(xsink))
      return 0;

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->myclass->getName());
      return 0;
   }
   lst = priv->data->getKeys();
   return lst;
}

void QoreObject::setValue(const char *key, AbstractQoreNode *value, ExceptionSink *xsink)
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return;

   if (check_access(xsink))
      return;

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, key, priv->myclass->getName());
      return;
   }
   priv->data->setKeyValue(key, value, xsink);
}

int QoreObject::size(ExceptionSink *xsink) const
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (check_access(xsink))
      return 0;

   if (priv->status == OS_DELETED)
      return 0;

   return priv->data->size();
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
void QoreObject::merge(const QoreHashNode *h, ExceptionSink *xsink)
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return;

   if (check_access(xsink))
      return;

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->myclass->getName());
      return;
   }
   priv->data->merge(h, xsink);
}

AbstractQoreNode *QoreObject::getReferencedMemberNoMethod(const char *mem, ExceptionSink *xsink) const
{
   printd(5, "QoreObject::getReferencedMemberNoMethod(this=%08p, mem=%08p (%s), xsink=%08p, data->size()=%d)\n",
	  this, mem, mem, xsink, priv->data ? priv->data->size() : -1);
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (check_access(xsink))
      return 0;

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->myclass->getName());
      return 0;
   }
   return priv->data->getReferencedKeyValue(mem);
}

QoreHashNode *QoreObject::copyData(ExceptionSink *xsink) const
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (check_access(xsink))
      return 0;

   if (priv->status == OS_DELETED)
      return 0;
   
   return priv->data->copy();
}

void QoreObject::mergeDataToHash(QoreHashNode *hash, ExceptionSink *xsink)
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return;

   if (check_access(xsink))
      return;

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->myclass->getName());
      return;
   }
   hash->merge(priv->data, xsink);
}

// NOTE: caller must unlock lock
// we check if the object is already locked
AbstractQoreNode **QoreObject::getExistingValuePtr(const QoreString *mem, AutoVLock *vl, ExceptionSink *xsink) const
{
   TempEncodingHelper enc(mem, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getExistingValuePtr(enc->getBuffer(), vl, xsink);
}

// NOTE: caller must unlock lock
// we check if the object is already locked
AbstractQoreNode **QoreObject::getExistingValuePtr(const char *mem, AutoVLock *vl, ExceptionSink *xsink) const
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (check_access(xsink))
      return 0;

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->myclass->getName());
      return 0;
   }

   AbstractQoreNode **rv = priv->data->getExistingValuePtr(mem);
   if (rv) {
      vl->push(&priv->g);
      v.stay_locked();
   }

   return rv;
}

AbstractPrivateData *QoreObject::getReferencedPrivateData(qore_classid_t key, ExceptionSink *xsink) const
{ 
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (priv->status != OS_DELETED && priv->privateData)
      return priv->privateData->getReferencedPrivateData(key);

   return 0;
}

AbstractPrivateData *QoreObject::getAndClearPrivateData(qore_classid_t key, ExceptionSink *xsink)
{
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return 0;

   if (priv->privateData)
      return priv->privateData->getAndClearPtr(key);

   return 0;
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
void QoreObject::setPrivate(qore_classid_t key, AbstractPrivateData *pd)
{ 
   if (!priv->privateData)
      priv->privateData = new KeyList();
   priv->privateData->insert(key, pd);
   addVirtualPrivateData(pd);
}

void QoreObject::addPrivateDataToString(QoreString *str, ExceptionSink *xsink) const
{
   str->concat('(');
   AutoVRMutex v(&priv->g, xsink);
   if (!v)
      return;

   if (priv->status == OS_OK && priv->privateData) {
      priv->privateData->addToString(str);
      str->terminate(str->strlen() - 2);
   }
   else
      str->concat("<NO PRIVATE DATA>");

   str->concat(')');
}

void QoreObject::cleanup(ExceptionSink *xsink, QoreHashNode *td)
{
   if (priv->privateData) {
      delete priv->privateData;
#ifdef DEBUG
      priv->privateData = 0;
#endif
   }
   
   if (priv->pgm) {
      printd(5, "QoreObject::cleanup() this=%08p (%s) calling QoreProgram::depDeref() (%08p)\n", this, priv->myclass->getName(), priv->pgm);
      priv->pgm->depDeref(xsink);
#ifdef DEBUG
      priv->pgm = 0;
#endif
   }
   
   td->deref(xsink);
}

void QoreObject::defaultSystemDestructor(qore_classid_t classID, ExceptionSink *xsink)
{
   AbstractPrivateData *pd = getAndClearPrivateData(classID, xsink);
   printd(5, "QoreObject::defaultSystemDestructor() this=%08p class=%s private_data=%08p\n", this, priv->myclass->getName(), pd); 
   if (pd)
      pd->deref(xsink);
}

QoreString *QoreObject::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = false;

   TempString rv(new QoreString());
   if (getAsString(*(*rv), foff, xsink))
      return 0;

   del = true;
   return rv.release();
}

int QoreObject::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   QoreHashNodeHolder h(copyData(xsink), xsink);
   if (*xsink)
      return -1;

   str.sprintf("class %s: ", priv->myclass->getName());

   if (foff != FMT_NONE) {
      addPrivateDataToString(&str, xsink);
      if (*xsink)
         return -1;

      str.concat(' ');
   }
   if (!h->size())
      str.concat("<NO MEMBERS>");
   else {
      if (foff != FMT_NONE)
         str.sprintf(" (%d member%s)\n", h->size(), h->size() == 1 ? "" : "s");
      else
         str.concat('(');

      class HashIterator hi(*h);

      bool first = false;
      while (hi.next()) {
         if (first)
            if (foff != FMT_NONE)
               str.concat('\n');
            else
               str.concat(", ");
         else
            first = true;

         if (foff != FMT_NONE)
            str.addch(' ', foff + 2);

         str.sprintf("%s : ", hi.getKey());

	 AbstractQoreNode *n = hi.getValue();
	 if (!n) n = &Nothing;
	 if (n->getAsString(str, foff != FMT_NONE ? foff + 2 : foff, xsink))
	    return -1;
      }
      if (foff == FMT_NONE)
         str.concat(')');
   }

   return 0;
}

AbstractQoreNode *QoreObject::realCopy() const
{
   return refSelf();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool QoreObject::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const QoreObject *o = dynamic_cast<const QoreObject *>(v);
   if (!o)
      return false;
   return !compareSoft(o, xsink);
}

bool QoreObject::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const QoreObject *o = dynamic_cast<const QoreObject *>(v);
   if (!o)
      return false;
   return !compareHard(o, xsink);
}

// returns the type name as a c string
const char *QoreObject::getTypeName() const
{
   return getStaticTypeName();
}

AbstractQoreNode *QoreObject::evalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

AbstractQoreNode *QoreObject::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

int64 QoreObject::bigIntEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

int QoreObject::integerEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0;
}

bool QoreObject::boolEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return false;
}

double QoreObject::floatEvalImpl(ExceptionSink *xsink) const
{
   assert(false);
   return 0.0;
}

bool QoreObject::hasMemberNotification() const
{
   return priv->myclass->hasMemberNotification();
}

void QoreObject::execMemberNotification(const char *member, ExceptionSink *xsink)
{
   if (priv->status == OS_OK)
      priv->myclass->execMemberNotification(this, member, xsink);
}
