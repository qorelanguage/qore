/*
  Object.cc

  thread-safe object definition

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

#include <qore/Qore.h>
#include <qore/intern/BuiltinMethod.h>
#include <qore/intern/QoreClassIntern.h>

#include <stdlib.h>
#include <assert.h>

#include <map>

#define OS_OK             0
#define OS_DELETED       -1

//#define QORE_DEBUG_OBJ_REFS 0

/*
  Qore internal class data is stored against the object with this data structure
  against its qore_classid_t (class ID).  In a class hierarchy, for private data
  that is actually a C++ subclass of Qore parent classes, then we save the same
  private data against the qore class IDs of the parent classes, but we set the
  flag to true, meaning that we will not delete the private data when
  the parent classes' destructors are run, but rather only when the subclass that
  actually owns data has its turn to destroy private object data. 

  So basically, the second boolean flag just means - does this class ID actually
  own the private data or not - if it's false, then it does not actually own the data,
  but is compatible with the data, so parent class builtin (C++) methods will get
  passed this private data as if it belonged to this class and as if it were saved
  directly to the object in the class' constructor.

  please note that this flag is called the "virtual" flag elsewhere in the code here,
  meaning that the data only "virtually" belongs to the class
 */
typedef std::pair<AbstractPrivateData *, bool> private_pair_t;

// mapping from qore class ID to the object data
typedef std::map<qore_classid_t, private_pair_t> keymap_t;

// for objects with multiple classes, private data has to be keyed
class KeyList {
   private:
      keymap_t keymap;

   public:
      DLLLOCAL AbstractPrivateData *getReferencedPrivateData(qore_classid_t key) const {
	 keymap_t::const_iterator i = keymap.find(key);
	 if (i == keymap.end())
	    return 0;

	 AbstractPrivateData *apd = i->second.first;
	 apd->ref();
	 return apd;
      }

      DLLLOCAL void addToString(QoreString *str) const {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	    str->sprintf("%d=<0x%08p>, ", i->first, i->second.first);
      }

      DLLLOCAL void derefAll(ExceptionSink *xsink) const {
	 for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	    if (!i->second.second)
	       i->second.first->deref(xsink);
      }

      DLLLOCAL AbstractPrivateData *getAndClearPtr(qore_classid_t key) {
	 keymap_t::iterator i = keymap.find(key);
	 if (i == keymap.end())
	    return 0;

	 assert(!i->second.second);
	 AbstractPrivateData *rv = i->second.first;
	 keymap.erase(i);
	 return rv;
      }

      DLLLOCAL void insert(qore_classid_t key, AbstractPrivateData *pd) {
	 assert(pd);
	 keymap.insert(std::make_pair(key, std::make_pair(pd, false)));
      }

      DLLLOCAL void insertVirtual(qore_classid_t key, AbstractPrivateData *pd) {
	 assert(pd);
	 if (keymap.find(key) == keymap.end())
	    keymap.insert(std::make_pair(key, std::make_pair(pd, true)));
      }
};

struct qore_object_private {
      const QoreClass *theclass;
      int status;
      mutable QoreThreadLock mutex;
#ifdef QORE_CLASS_SYNCHRONOUS
      mutable VRMutex *sync_vrm;
#endif
      // used for references, to ensure that assignments will not deadlock when the object is locked for update
      mutable QoreThreadLock ref_mutex;
      KeyList *privateData;
      QoreReferenceCounter tRefs;  // reference-references
      QoreHashNode *data;
      QoreProgram *pgm;
   bool system_object, delete_blocker_run, in_destructor;

      DLLLOCAL qore_object_private(const QoreClass *oc, QoreProgram *p, QoreHashNode *n_data) : 
	 theclass(oc), status(OS_OK), 
#ifdef QORE_CLASS_SYNCHRONOUS
	 sync_vrm(oc->has_synchronous_in_hierarchy() ? new VRMutex : 0),
#endif 
	 privateData(0), data(n_data), pgm(p), system_object(!p), delete_blocker_run(false), in_destructor(false)
      {
#ifdef QORE_DEBUG_OBJ_REFS
	 printd(QORE_DEBUG_OBJ_REFS, "QoreObject::QoreObject() this=%08p, pgm=%08p, class=%s, references 0->1\n", this, p, oc->getName());
#endif
	 /* instead of referencing the class, we reference the program, because the
	    program contains the namespace that contains the class, and the class'
	    methods may call functions in the program as well that could otherwise
	    disappear when the program is deleted
	 */
	 if (p) {
	    printd(5, "QoreObject::init() this=%08p (%s) calling QoreProgram::depRef() (%08p)\n", this, theclass->getName(), p);
	    p->depRef();
	 }
      }

      DLLLOCAL ~qore_object_private() {
	 assert(!pgm);
	 assert(!data);
	 assert(!privateData);
      }

      // add virtual IDs for private data to class list
      void addVirtualPrivateData(qore_classid_t key, AbstractPrivateData *apd) {
	 // first get parent class corresponding to "key"
	 QoreClass *qc = theclass->getClass(key);
	 assert(qc);
	 BCSMList *sml = qc->getBCSMList();
	 if (!sml)
	    return;
	 
	 for (class_list_t::const_iterator i = sml->begin(), e = sml->end(); i != e; ++i)
	    if ((*i).second)
	       privateData->insertVirtual((*i).first->getID(), apd);
      }

      // called only during constructor execution, therefore no need for locking
      void setPrivate(qore_classid_t key, AbstractPrivateData *pd) { 
	 if (!privateData)
	    privateData = new KeyList;
	 privateData->insert(key, pd);
	 addVirtualPrivateData(key, pd);
      }
};

void QoreObject::externalDelete(qore_classid_t key, ExceptionSink *xsink) {
   {
      AutoLocker al(priv->mutex);

      if (priv->in_destructor || priv->status == OS_DELETED || !priv->privateData)
	 return;

      // remove the private data that's already been deleted
#ifdef DEBUG
      assert(priv->privateData->getAndClearPtr(key));
#else
      priv->privateData->getAndClearPtr(key);
#endif
      // mark status as in destructor
      priv->status = gettid();
   }

   // run the destructor
   doDeleteIntern(xsink);
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(oc, p, new QoreHashNode())) {
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p, AbstractPrivateData *data) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(oc, p, new QoreHashNode())) {
   assert(data);
   priv->setPrivate(oc->getID(), data);
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p, QoreHashNode *h) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(oc, p, h)) {
}

QoreObject::~QoreObject() {
   //QORE_TRACE("QoreObject::~QoreObject()");
   //printd(5, "QoreObject::~QoreObject() this=%08p, pgm=%08p, class=%s\n", this, priv->pgm, priv->theclass->getName());

   delete priv;
}

const QoreClass *QoreObject::getClass() const {
   return priv->theclass; 
}

const char *QoreObject::getClassName() const {
   return priv->theclass->getName(); 
}

int QoreObject::getStatus() const { 
   return priv->status; 
}

bool QoreObject::isValid() const {
   return priv->status == OS_OK; 
}

QoreProgram *QoreObject::getProgram() const {
   return priv->pgm;
}

bool QoreObject::isSystemObject() const {
   return priv->system_object;
}

void QoreObject::tRef() const {
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::tRef(this=%08p) class=%s: tref %d->%d\n", this, priv->theclass->getName(), priv->tRefs.reference_count(), priv->tRefs.reference_count() + 1);
#endif
   priv->tRefs.ROreference();
}

void QoreObject::tDeref() {
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::tDeref(this=%08p) class=%s: tref %d->%d\n", this, priv->theclass->getName(), priv->tRefs.reference_count(), priv->tRefs.reference_count() - 1);
#endif
   if (priv->tRefs.ROdereference())
      delete this;
}

// helper function for QoreObject::evalBuiltinMethodWithPrivateData() variations
static AbstractQoreNode *check_meth_eval(const QoreClass *cls, const BuiltinMethod *meth, ExceptionSink *xsink) {
   if (xsink->isException())
      return 0;
   if (cls == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", cls->getName(), meth->getName());
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), meth->getName(), cls->getName());
   return 0;
}

AbstractQoreNode *QoreObject::evalBuiltinMethodWithPrivateData(BuiltinMethod *meth, const QoreListNode *args, ExceptionSink *xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->myclass->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->evalMethod(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%08p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
   return check_meth_eval(priv->theclass, meth, xsink);
}

AbstractQoreNode *QoreObject::evalBuiltinMethodWithPrivateData(const QoreMethod &method, BuiltinMethod *meth, const QoreListNode *args, ExceptionSink *xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->myclass->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->evalMethod(method, this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%08p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
   return check_meth_eval(priv->theclass, meth, xsink);
}

void QoreObject::evalCopyMethodWithPrivateData(const QoreClass &thisclass, BuiltinMethod *meth, QoreObject *self, bool new_calling_convention, ExceptionSink *xsink) {
   // get referenced object
   AbstractPrivateData *pd = getReferencedPrivateData(meth->myclass->getID(), xsink);

   if (pd) {
      meth->evalCopy(thisclass, self, this, pd, new_calling_convention, xsink);
      pd->deref(xsink);
      return;
   }

   if (xsink->isException())
      return;
   if (priv->theclass == meth->myclass)
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::copy() cannot be executed because the object has already been deleted", priv->theclass->getName());
   else
      xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::copy() (base class of '%s') cannot be executed because the object has already been deleted", meth->myclass->getName(), priv->theclass->getName());
}

// note that the lock is already held when this method is called
bool QoreObject::evalDeleteBlocker(BuiltinMethod *meth) {
   // FIXME: eliminate reference counts for private data, private data should be destroyed after the destructor terminates

   // get referenced object
   ExceptionSink xsink;
   ReferenceHolder<AbstractPrivateData> pd(priv->privateData->getReferencedPrivateData(meth->myclass->getIDForMethod()), &xsink);

   if (pd)
      return meth->evalDeleteBlocker(this, *pd);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%08p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
   return false;
}

bool QoreObject::validInstanceOf(qore_classid_t cid) const
{
   if (priv->status == OS_DELETED)
      return 0;

   return priv->theclass->getClass(cid);
}

AbstractQoreNode *QoreObject::evalMethod(const QoreString *name, const QoreListNode *args, ExceptionSink *xsink) {
   TempEncodingHelper tmp(name, QCS_DEFAULT, xsink);
   if (!tmp)
      return 0;

   return evalMethod(tmp->getBuffer(), args, xsink);
}

AbstractQoreNode *QoreObject::evalMethod(const char *name, const QoreListNode *args, ExceptionSink *xsink) {
   return priv->theclass->evalMethod(this, name, args, xsink);
}

AbstractQoreNode *QoreObject::evalMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink) {
   return method.eval(this, args, xsink);
}

const QoreClass *QoreObject::getClass(qore_classid_t cid) const
{
   if (cid == priv->theclass->getID())
      return priv->theclass;
   return priv->theclass->getClass(cid);
}

static bool in_class_call(qore_classid_t id) {
   QoreObject *obj = getStackObject();
   return (obj && obj->getClass()->getID() == id) ? true : false;
}

/*
class tAutoLocker : public AutoLocker {
   public:
      tAutoLocker(QoreThreadLock &m) : AutoLocker(m) {
	 printd(0, "got lock %p\n", lck);
      }
      ~tAutoLocker() {
	 printd(0, "released lock %p\n", lck);
      }
};

class tSafeLocker : public SafeLocker {
   public:
      tSafeLocker(QoreThreadLock &m) : SafeLocker(m) {
	 printd(0, "got lock %p\n", lck);
      }
      ~tSafeLocker() {
	 if (lck)
	    printd(0, "released lock %p\n", lck);
      }
      void unlock() {
	 printd(0, "released lock %p\n", lck);
	 SafeLocker::unlock();
      }
};
*/

void QoreObject::doPrivateException(const char *mem, ExceptionSink *xsink) const {
   xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, priv->theclass->getName());
}

bool QoreObject::checkExternalPrivateAccess(const char *mem) const {
   // if accessed outside the class and the member is a private member 
   return (!in_class_call(priv->theclass->getID()) && priv->theclass->isPrivateMember(mem)) ? true : false;
}

bool QoreObject::checkExternalPrivateAccess(const char *mem, ExceptionSink *xsink) const {
   if (!checkExternalPrivateAccess(mem))
      return false;
   doPrivateException(mem, xsink);
   return true;
}

AbstractQoreNode *QoreObject::evalMember(const QoreString *member, ExceptionSink *xsink) {
   // make sure to convert string encoding if necessary to default character set
   TempEncodingHelper tstr(member, QCS_DEFAULT, xsink);
   if (!tstr)
      return 0;

   const char *mem = tstr->getBuffer();

   //printd(5, "QoreObject::evalMember() find_key(%s)=%08p theclass=%s\n", mem, find_key(mem), theclass ? theclass->getName() : "NONE");

   if (checkExternalPrivateAccess(mem)) {
      // run memberGate if it exists
      if (priv->theclass->hasMemberGate())
	 return priv->theclass->evalMemberGate(this, *tstr, xsink);

      doPrivateException(mem, xsink);
      return 0;
   }

   AbstractQoreNode *rv;
   bool exists;
   {
      AutoLocker al(priv->mutex);

      if (priv->status == OS_DELETED)
	 return 0;

      rv = priv->data->getReferencedKeyValue(mem, exists);
   }

   // execute memberGate method for objects where no member exists
   if (!exists && priv->theclass->hasMemberGate()) {
      return priv->theclass->evalMemberGate(this, *tstr, xsink);
   }

   return rv;
}

// 0 = equal, 1 = not equal
bool QoreObject::compareSoft(const QoreObject *obj, ExceptionSink *xsink) const {
   // currently objects are only equal if they are the same object
   return !(this == obj);
}

// 0 = equal, 1 = not equal
bool QoreObject::compareHard(const QoreObject *obj, ExceptionSink *xsink) const {
   // currently objects are only equal if they are the same object
   return !(this == obj);
}

// lock not already held
void QoreObject::doDeleteIntern(ExceptionSink *xsink) {
   printd(5, "QoreObject::doDeleteIntern(this=%08p) execing destructor()\n", this);   
   priv->theclass->execDestructor(this, xsink);

   QoreHashNode *td;
   {
      AutoLocker al(priv->mutex);
      assert(priv->status != OS_DELETED);
      assert(priv->data);
      priv->status = OS_DELETED;
      td = priv->data;
      priv->data = 0;
   }
   cleanup(xsink, td);
}

void QoreObject::doDelete(ExceptionSink *xsink) {
   {
      AutoLocker al(priv->mutex);

      if (priv->status == OS_DELETED)
	 return;

      if (priv->in_destructor || priv->status > 0) {
	 xsink->raiseException("DOUBLE-DELETE-EXCEPTION", "destructor called from within destructor for class %s", getClassName());
	 return;
      }

      // mark status as in destructor
      priv->status = gettid();
   }
   doDeleteIntern(xsink);
}

void QoreObject::customRefIntern() const {
   if (!references)
      tRef();
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customRefIntern(this=%08p) class=%s: references %d->%d\n", this, getClassName(), references, references + 1);
#endif
   ++references;
}

void QoreObject::customRef() const {
   AutoLocker al(priv->ref_mutex);
   //AutoLocker al(priv->mutex);
   customRefIntern();
}

void QoreObject::deleteBlockerRef() const {
    customRefIntern();
}

bool QoreObject::derefImpl(ExceptionSink *xsink) {
   // should never be called
   assert(false);
   return false;
}

// manages the custom dereference and executes the destructor if necessary
void QoreObject::customDeref(ExceptionSink *xsink) {
   {
      printd(5, "QoreObject::customDeref() this=%08p, class=%s references=%d->%d status=%d has_delete_blocker=%d delete_blocker_run=%d\n", this, getClassName(), references, references - 1, priv->status, priv->theclass->has_delete_blocker(), priv->delete_blocker_run);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customDeref(this=%08p) class=%s: references %d->%d\n", this, getClassName(), references, references - 1);
#endif
      {
	 AutoLocker slr(priv->ref_mutex);
	 if (--references)
	    return;
      }

      SafeLocker sl(priv->mutex);

      if (priv->in_destructor || priv->status != OS_OK) {
	 sl.unlock();
	 tDeref();
	 return;
      }

      // if the scope deletion is blocked, then do not run the destructor
      if (!priv->delete_blocker_run && priv->theclass->has_delete_blocker()) {
	 if (priv->theclass->execDeleteBlocker(this, xsink)) {
	    //printd(5, "QoreObject::derefImpl() this=%08p class=%s blocking delete\n", this, getClassName());
	    priv->delete_blocker_run = true;
	    //printd(5, "Object lock %08p unlocked (safe)\n", &priv->mutex);
	    return;
	 }
      }

      priv->in_destructor = true;

      //printd(5, "QoreObject::derefImpl() class=%s this=%08p going out of scope\n", getClassName(), this);

      // mark status as in destructor
      priv->status = gettid();

      //printd(5, "Object lock %08p unlocked (safe)\n", &priv->mutex);
   }

   doDeleteIntern(xsink);

   tDeref();
}

// this method is called when there is an exception in a constructor and the object should be deleted
void QoreObject::obliterate(ExceptionSink *xsink)
{
   printd(5, "QoreObject::obliterate(this=%08p) class=%s %d->%d\n", this, priv->theclass->getName(), references, references - 1);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "QoreObject::obliterate(this=%08p) class=%s: references %d->%d\n", this, getClassName(), references, references - 1);
#endif

   {
      AutoLocker slr(priv->ref_mutex);
      if (--references)
	 return;
   }
   
   {
      SafeLocker sl(priv->mutex);

      if (priv->in_destructor || priv->status != OS_OK) {
	 printd(5, "QoreObject::obliterate() %08p data=%08p in_destructor=%d status=%d\n", this, priv->data, priv->in_destructor, priv->status);
	 //printd(0, "Object lock %08p unlocked (safe)\n", &priv->mutex);
         sl.unlock();
         tDeref();
         return;
      }

      //printd(5, "Object lock %08p locked   (safe)\n", &priv->mutex);
      printd(5, "QoreObject::obliterate() class=%s deleting this=%08p\n", priv->theclass->getName(), this);

      priv->status = OS_DELETED;
      QoreHashNode *td = priv->data;
      priv->data = 0;
      //printd(0, "Object lock %08p unlocked (safe)\n", &priv->mutex);
      sl.unlock();
	 
      if (priv->privateData)
	 priv->privateData->derefAll(xsink);
	 
      cleanup(xsink, td);
   }
   tDeref();
}

class qore_object_lock_handoff_manager {
   private:
      QoreObject *self;
      AutoVLock *vl;

   public:
      DLLLOCAL qore_object_lock_handoff_manager(QoreObject *n_self, AutoVLock *n_vl) : self(n_self), vl(n_vl) {
	 // reference current object
	 self->tRef();

	 // unlock previous lock and release from AutoVLock structure
	 vl->del();

	 //printd(0, "Object lock %08p locked   (handoff by %s - vlock by %s)\n", &self->priv->m, who, vl->getWho());
	 // lock current object
	 self->priv->mutex.lock();
      }

      DLLLOCAL ~qore_object_lock_handoff_manager() {
	 // unlock if lock not saved in AutoVLock structure
	 if (self) {
	    //printd(0, "Object lock %08p unlocked (handoff)\n", &self->priv->mutex);
	    self->priv->mutex.unlock();
	    self->tDeref();
	 }
      }

      DLLLOCAL void stay_locked() {
	 vl->set(self, &self->priv->mutex);
	 self = 0;
      }
};

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **QoreObject::getMemberValuePtr(const char *key, AutoVLock *vl, ExceptionSink *xsink) const {
   // check for external access to private members
   if (checkExternalPrivateAccess(key, xsink))
      return 0;

   // do lock handoff
   qore_object_lock_handoff_manager qolhm(const_cast<QoreObject *>(this), vl);

   if (priv->status == OS_DELETED)
      return 0;

   qolhm.stay_locked();
   return priv->data->getKeyValuePtr(key);
}

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **QoreObject::getMemberValuePtr(const QoreString *key, AutoVLock *vl, ExceptionSink *xsink) const {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValuePtr(enc->getBuffer(), vl, xsink);
}

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode *QoreObject::getMemberValueNoMethod(const QoreString *key, AutoVLock *vl, ExceptionSink *xsink) const {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValueNoMethod(enc->getBuffer(), vl, xsink);
}

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode *QoreObject::getMemberValueNoMethod(const char *key, AutoVLock *vl, ExceptionSink *xsink) const {
   // do lock handoff
   qore_object_lock_handoff_manager qolhm(const_cast<QoreObject *>(this), vl);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, key, priv->theclass->getName());
      return 0;
   }

   AbstractQoreNode *rv = priv->data->getKeyValue(key);
   if (rv && rv->isReferenceCounted()) {
      qolhm.stay_locked();
   }
   return rv;
}

void QoreObject::deleteMemberValue(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   deleteMemberValue(enc->getBuffer(), xsink);
}

void QoreObject::deleteMemberValue(const char *key, ExceptionSink *xsink) {
   // check for external access to private members
   if (checkExternalPrivateAccess(key, xsink))
      return;

   AbstractQoreNode *v;
   {
      AutoLocker al(priv->mutex);

      if (priv->status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, key, priv->theclass->getName());
	 return;
      }
      
      v = priv->data->takeKeyValue(key);
   }

   if (!v)
      return;

   if (v->getType() == NT_OBJECT)
      reinterpret_cast<QoreObject *>(v)->doDelete(xsink);
   v->deref(xsink);
}

void QoreObject::removeMember(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   removeMember(enc->getBuffer(), xsink);
}

void QoreObject::removeMember(const char *key, ExceptionSink *xsink) {
   // check for external access to private members
   if (checkExternalPrivateAccess(key, xsink))
      return;

   AbstractQoreNode *v;
   {
      AutoLocker al(priv->mutex);

      if (priv->status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, key, priv->theclass->getName());
	 return;
      }
      
      v = priv->data->takeKeyValue(key);
   }

   if (!v)
      return;

   v->deref(xsink);
}

QoreListNode *QoreObject::getMemberList(ExceptionSink *xsink) const {
   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return 0;
   }

   return priv->data->getKeys();
}

QoreHashNode *QoreObject::getSlice(const QoreListNode *value_list, ExceptionSink *xsink) const {
   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return 0;
   }

   bool inclass = in_class_call(priv->theclass->getID());
   // return all member data requested if called in the class
   if (inclass)
      return priv->data->getSlice(value_list, xsink);

   // otherwise build a new list of only public members
   ReferenceHolder<QoreListNode> nl(new QoreListNode, xsink);

   ConstListIterator li(value_list);
   while (li.next()) {
      QoreStringValueHelper key(li.getValue(), QCS_DEFAULT, xsink);
      if (*xsink)
	 return 0;

      if (priv->theclass->isPrivateMember(key->getBuffer()))
	 continue;

      nl->push(new QoreStringNode(*(*key)));
   }

   return priv->data->getSlice(*nl, xsink);
}

void QoreObject::setValue(const char *key, AbstractQoreNode *val, ExceptionSink *xsink) {
   AbstractQoreNode *old_value;

   {
      AutoLocker al(priv->mutex);

      if (priv->status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, key, priv->theclass->getName());
	 return;
      }

      old_value = priv->data->takeKeyValue(key);

      priv->data->setKeyValue(key, val, xsink);
   }

   if (old_value)
      old_value->deref(xsink);
}

int QoreObject::size(ExceptionSink *xsink) const
{
   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED)
      return 0;

   return priv->data->size();
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
void QoreObject::merge(const QoreHashNode *h, ExceptionSink *xsink)
{
   // list for saving all overwritten values to be dereferenced outside the object lock
   ReferenceHolder<QoreListNode> holder(xsink);

   {
      AutoLocker al(priv->mutex);

      if (priv->status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, priv->theclass->getName());
	 return;
      }

      ConstHashIterator hi(h);
      while (hi.next()) {
	 AbstractQoreNode *n = priv->data->swapKeyValue(hi.getKey(), hi.getReferencedValue());
	 // if we are overwriting a value, then save it in the list for dereferencing after the lock is released
	 if (n && n->isReferenceCounted()) {
	    if (!holder)
	       holder = new QoreListNode();
	    holder->push(n);
	 }
      }
   }
}

int64 QoreObject::getMemberAsBigInt(const char *mem, bool &found, ExceptionSink *xsink) const {
   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->theclass->getName());
      return 0;
   }

   return priv->data->getKeyAsBigInt(mem, found);
}

AbstractQoreNode *QoreObject::getReferencedMemberNoMethod(const char *mem, ExceptionSink *xsink) const {
   AutoLocker al(priv->mutex);

   printd(5, "QoreObject::getReferencedMemberNoMethod(this=%08p, mem=%08p (%s), xsink=%08p, data->size()=%d)\n",
	  this, mem, mem, xsink, priv->data ? priv->data->size() : -1);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->theclass->getName());
      return 0;
   }

   return priv->data->getReferencedKeyValue(mem);
}

QoreHashNode *QoreObject::copyData(ExceptionSink *xsink) const {
   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return 0;
   }

   return priv->data->copy();
}

QoreHashNode *QoreObject::getRuntimeMemberHash(ExceptionSink *xsink) const {
   bool inclass = in_class_call(priv->theclass->getID());

   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED)
      return 0;

   // return all member data if called inside the class
   if (inclass)
      return priv->data->copy();

   QoreHashNode *h = new QoreHashNode;

   ConstHashIterator hi(priv->data);
   while (hi.next()) {
      if (priv->theclass->isPrivateMember(hi.getKey()))
	 continue;

      // not possible for an exception to happen here
      h->setKeyValue(hi.getKey(), hi.getReferencedValue(), xsink);
   }

   return h;
}

void QoreObject::mergeDataToHash(QoreHashNode *hash, ExceptionSink *xsink) {
   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return;
   }

   hash->merge(priv->data, xsink);
}

// unlocking the lock is managed with the AutoVLock object
// we check if the object is already locked
AbstractQoreNode **QoreObject::getExistingValuePtr(const QoreString *mem, AutoVLock *vl, ExceptionSink *xsink) const {
   TempEncodingHelper enc(mem, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getExistingValuePtr(enc->getBuffer(), vl, xsink);
}

// unlocking the lock is managed with the AutoVLock object
// we check if the object is already locked
AbstractQoreNode **QoreObject::getExistingValuePtr(const char *mem, AutoVLock *vl, ExceptionSink *xsink) const {
   // check for illegal access
   if (checkExternalPrivateAccess(mem, xsink))
      return 0;

   // do lock handoff
   qore_object_lock_handoff_manager qolhm(const_cast<QoreObject *>(this), vl);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->theclass->getName());
      return 0;
   }

   AbstractQoreNode **rv = priv->data->getExistingValuePtr(mem);
   if (rv) {
      qolhm.stay_locked();
   }

   return rv;
}

AbstractPrivateData *QoreObject::getReferencedPrivateData(qore_classid_t key, ExceptionSink *xsink) const { 
   AutoLocker al(priv->mutex);

   if (priv->status == OS_DELETED || !priv->privateData) {
      makeAccessDeletedObjectException(xsink, getClassName());
      return 0;
   }

   return priv->privateData->getReferencedPrivateData(key);
}

AbstractPrivateData *QoreObject::getAndClearPrivateData(qore_classid_t key, ExceptionSink *xsink)
{
   AutoLocker al(priv->mutex);

   if (priv->privateData)
      return priv->privateData->getAndClearPtr(key);

   return 0;
}

// called only during constructor execution, therefore no need for locking
void QoreObject::setPrivate(qore_classid_t key, AbstractPrivateData *pd) { 
   priv->setPrivate(key, pd);
}

void QoreObject::addPrivateDataToString(QoreString *str, ExceptionSink *xsink) const {
   str->concat('(');
   AutoLocker al(priv->mutex);

   if (priv->status == OS_OK && priv->privateData) {
      priv->privateData->addToString(str);
      str->terminate(str->strlen() - 2);
   }
   else
      str->concat("<NO PRIVATE DATA>");

   str->concat(')');
}

void QoreObject::cleanup(ExceptionSink *xsink, QoreHashNode *td) {
   if (priv->privateData) {
      delete priv->privateData;
#ifdef DEBUG
      priv->privateData = 0;
#endif
   }
   
   if (priv->pgm) {
      printd(5, "QoreObject::cleanup() this=%08p (%s) calling QoreProgram::depDeref() (%08p)\n", this, priv->theclass->getName(), priv->pgm);
      priv->pgm->depDeref(xsink);
#ifdef DEBUG
      priv->pgm = 0;
#endif
   }

   td->deref(xsink);
}

void QoreObject::defaultSystemDestructor(qore_classid_t classID, ExceptionSink *xsink) {
   AbstractPrivateData *pd = getAndClearPrivateData(classID, xsink);
   printd(5, "QoreObject::defaultSystemDestructor() this=%08p class=%s private_data=%08p\n", this, priv->theclass->getName(), pd); 
   if (pd)
      pd->deref(xsink);
}

QoreString *QoreObject::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;

   TempString rv(new QoreString());
   if (getAsString(*(*rv), foff, xsink))
      return 0;

   del = true;
   return rv.release();
}

int QoreObject::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   QoreHashNodeHolder h(copyData(xsink), xsink);
   if (*xsink)
      return -1;

   str.sprintf("class %s: ", priv->theclass->getName());

   if (foff != FMT_NONE) {
      addPrivateDataToString(&str, xsink);
      if (*xsink)
         return -1;

      str.concat(' ');
   }
   if (!h->size())
      str.concat("<NO MEMBERS>");
   else {
      QoreContainerHelper cch(this);
      if (!cch) {
	 str.concat("(ERROR: recursive reference)");
	 return 0;
      }

      str.concat('(');
      if (foff != FMT_NONE)
         str.sprintf("%d member%s)\n", h->size(), h->size() == 1 ? "" : "s");

      HashIterator hi(*h);

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

AbstractQoreNode *QoreObject::realCopy() const {
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
   return priv->theclass->hasMemberNotification();
}

void QoreObject::execMemberNotification(const char *member, ExceptionSink *xsink)
{
   priv->theclass->execMemberNotification(this, member, xsink);
}

#ifdef QORE_CLASS_SYNCHRONOUS
VRMutex *QoreObject::getClassSyncLock()
{
   return priv->sync_vrm;
}
#endif
