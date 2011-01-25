/*
  Object.cpp

  thread-safe object definition

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/QoreObjectIntern.h>
#include <qore/intern/QoreHashNodeIntern.h>

void RecursiveSet::merge(RecursiveSet *rset) {
   assert(oset.size() >= rset->oset.size());

   for (obj_set_t::iterator i = rset->oset.begin(), e = rset->oset.end(); i != e; ++i)
      oset.insert(*i);
}

int qore_object_private::checkRecursive(ObjMap &omap, AutoVLock &vl, ExceptionSink *xsink) {
   // do lock handoff
   qore_object_recursive_lock_handoff_helper qolhm(this, vl);

   if (!qolhm)
      return 0;

   if (status == OS_DELETED)
      return 0;

   ObjectCycleHelper och(omap, obj);
   const char *key = omap.getKey(obj);
   if (key) {
      
      AbstractQoreNode *n = obj->getMemberValueNoMethod(key, &vl, xsink);
      // FIXME: wtf?
      if (*xsink)
	 return 0;
      omap.reset(obj, key);

      int rc = qoreCheckContainer(n, omap, vl, xsink);
      if (rc)
	 printd(0, "qore_object_private::checkRecursive() found recursive ref: obj=%p (%s) '%s' (key)\n", obj, theclass->getName(), key);
      else
	 printd(0, "qore_object_private::checkRecursive() checked obj=%p (%s) '%s' - no recursive ref (key)\n", obj, theclass->getName(), key);
	 
      
      return rc;
   }

   int rc = 0;

   HashIterator hi(data);
   while (hi.next()) {
      omap.reset(obj, hi.getKey());

      if (qoreCheckContainer(hi.getValue(), omap, vl, xsink)) {
	 printd(0, "qore_object_private::checkRecursive() found recursive ref: obj=%p (%s) '%s'\n", obj, theclass->getName(), hi.getKey());
	 ++rc;
      }
      else
	 printd(0, "qore_object_private::checkRecursive() checked obj=%p (%s) '%s' - no recursive ref\n", obj, theclass->getName(), hi.getKey());
   }

   return rc;
}

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **qore_object_private::getMemberValuePtr(const char *key, AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) const {
   // check for external access to private members
   if (checkMemberAccessGetTypeInfo(key, typeInfo, xsink))
      return 0;

   // do lock handoff
   qore_object_lock_handoff_helper qolhm(const_cast<qore_object_private *>(this), *vl);

   if (status == OS_DELETED)
      return 0;

   qolhm.stay_locked();

   HashMember *m = data->priv->findCreateMember(key);
   omap.set(obj, m->key);

   return &m->node;
}

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
   priv->doDeleteIntern(xsink);
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, new QoreHashNode)) {
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p, AbstractPrivateData *data) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, new QoreHashNode)) {
   assert(data);
   setPrivate(oc->getID(), data);
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p, QoreHashNode *h) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, h)) {
}

QoreObject::~QoreObject() {
   //QORE_TRACE("QoreObject::~QoreObject()");
   //printd(5, "QoreObject::~QoreObject() this=%p, pgm=%p, class=%s\n", this, priv->pgm, priv->theclass->getName());

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
   priv->tRef();
}

void QoreObject::tDeref() {
   priv->tDeref();
}

// helper function for QoreObject::evalBuiltinMethodWithPrivateData() variations
static void check_meth_eval(const QoreClass *cls, const char *mname, const QoreClass *mclass, ExceptionSink *xsink) {
   if (!xsink->isException()) {
      if (cls == mclass)
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", cls->getName(), mname);
      else
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of '%s') cannot be executed because the object has already been deleted", mclass->getName(), mname, cls->getName());
   }
}

AbstractQoreNode *QoreObject::evalBuiltinMethodWithPrivateData(const QoreMethod &method, const BuiltinNormalMethodVariantBase *meth, const QoreListNode *args, ExceptionSink *xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->getClass()->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->evalImpl(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p (%s) pd=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, priv->theclass->getName(), *pd, method.getClass()->getName(), method.getName(), method.getClass()->getID(), method.getClass()->getIDForMethod());
   check_meth_eval(priv->theclass, method.getName(), method.getClass(), xsink);
   return 0;
}

void QoreObject::evalCopyMethodWithPrivateData(const QoreClass &thisclass, const BuiltinCopyVariantBase *meth, QoreObject *self, ExceptionSink *xsink) {
   // get referenced object
   AbstractPrivateData *pd = getReferencedPrivateData(thisclass.getID(), xsink);

   if (pd) {
      meth->evalImpl(thisclass, self, this, pd, xsink);
      pd->deref(xsink);
      return;
   }

   check_meth_eval(priv->theclass, "copy", &thisclass, xsink);
}

// note that the lock is already held when this method is called
bool QoreObject::evalDeleteBlocker(qore_classid_t classid_for_method, BuiltinDeleteBlocker *meth) {
   // FIXME: eliminate reference counts for private data, private data should be destroyed after the destructor terminates

   // get referenced object
   ExceptionSink xsink;
   ReferenceHolder<AbstractPrivateData> pd(priv->privateData->getReferencedPrivateData(classid_for_method), &xsink);

   if (pd)
      return meth->eval(this, *pd);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p, method class ID=%d\n", this, classid_for_method);
   return false;
}

bool QoreObject::validInstanceOf(qore_classid_t cid) const {
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

AbstractQoreNode *QoreObject::evalMethodVariant(const QoreMethod &method, const QoreExternalMethodVariant *variant, const QoreListNode *args, ExceptionSink *xsink) {
   return method.evalNormalVariant(this, variant, args, xsink);
}

const QoreClass *QoreObject::getClass(qore_classid_t cid) const {
   if (cid == priv->theclass->getID())
      return priv->theclass;
   return priv->theclass->getClass(cid);
}

const QoreClass *QoreObject::getClass(qore_classid_t cid, bool &cpriv) const {
   return priv->theclass->getClass(cid, cpriv);
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

AbstractQoreNode *QoreObject::evalMember(const QoreString *member, ExceptionSink *xsink) {
   // make sure to convert string encoding if necessary to default character set
   TempEncodingHelper tstr(member, QCS_DEFAULT, xsink);
   if (!tstr)
      return 0;

   const char *mem = tstr->getBuffer();

   //printd(5, "QoreObject::evalMember() find_key(%s)=%p theclass=%s\n", mem, find_key(mem), theclass ? theclass->getName() : "NONE");


   int rc = priv->checkMemberAccess(mem);
   if (rc) {
      // run memberGate if it exists
      if (priv->theclass->hasMemberGate())
	 return priv->theclass->evalMemberGate(this, *tstr, xsink);

      if (rc == QOA_PRIV_ERROR)
	 priv->doPrivateException(mem, xsink);
      else
	 priv->doPublicException(mem, xsink);
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
   priv->doDeleteIntern(xsink);
}

void QoreObject::customRefIntern() const {
   if (!references)
      tRef();
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customRefIntern() this=%p class=%s references %d->%d\n", this, getClassName(), references, references + 1);
#endif
   ++references;
}

void QoreObject::customRef() const {
   AutoLocker al(priv->ref_mutex);
   customRefIntern();
}

void QoreObject::deleteBlockerRef() const {
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::deleteBlockerRef() this=%p class=%s references %d->%d\n", this, getClassName(), references, references + 1);
#endif
   AutoLocker al(priv->ref_mutex);
   ++references;
}

bool QoreObject::derefImpl(ExceptionSink *xsink) {
   // should never be called
   assert(false);
   return false;
}

// manages the custom dereference and executes the destructor if necessary
void QoreObject::customDeref(ExceptionSink *xsink) {
   {
      //printd(5, "QoreObject::customDeref() this=%p, class=%s references=%d->%d (trefs=%d) status=%d has_delete_blocker=%d delete_blocker_run=%d\n", this, getClassName(), references, references - 1, priv->tRefs.reference_count(), priv->status, priv->theclass->has_delete_blocker(), priv->delete_blocker_run);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customDeref(this=%p) class=%s: references %d->%d (rcnt=%d)\n", this, priv->status == OS_OK ? getClassName() : "<deleted>", references, references - 1, priv->rcount);
#endif
      {
	 AutoLocker slr(priv->ref_mutex);
	 if (--references)
	    return;
      }

      SafeLocker sl(priv->mutex);

      // if the destructor has already been run, then just run tDeref() which should delete the QoreObject
      if (priv->in_destructor || priv->status != OS_OK) {
	 sl.unlock();
	 tDeref();
	 return;
      }

      // if the scope deletion is blocked, then do not run the destructor
      if (!priv->delete_blocker_run && priv->theclass->has_delete_blocker()) {
	 if (priv->theclass->execDeleteBlocker(this, xsink)) {
	    //printd(5, "QoreObject::customDeref() this=%p class=%s blocking delete\n", this, getClassName());
	    priv->delete_blocker_run = true;
	    //printd(5, "Object lock %p unlocked (safe)\n", &priv->mutex);
	    return;
	 }
      }
      
      priv->in_destructor = true;

      //printd(5, "QoreObject::derefImpl() class=%s this=%p going out of scope\n", getClassName(), this);

      // mark status as in destructor
      priv->status = gettid();

      //printd(5, "Object lock %p unlocked (safe)\n", &priv->mutex);
   }

   priv->doDeleteIntern(xsink);
}

// this method is called when there is an exception in a constructor and the object should be deleted
void QoreObject::obliterate(ExceptionSink *xsink) {
   priv->obliterate(xsink);
}

/*
// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **QoreObject::getMemberValuePtr(const QoreString *key, AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink *xsink) const {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValuePtr(enc->getBuffer(), vl, typeInfo, xsink);
}
*/

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
   qore_object_lock_handoff_helper qolhm(const_cast<qore_object_private *>(priv), *vl);

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
   if (priv->checkMemberAccess(key, xsink))
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

AbstractQoreNode *QoreObject::takeMember(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return takeMember(enc->getBuffer(), xsink);
}

AbstractQoreNode *QoreObject::takeMember(const char *key, ExceptionSink *xsink) {
   // check for external access to private members
   if (priv->checkMemberAccess(key, xsink))
      return 0;

   AbstractQoreNode *v;
   {
      AutoLocker al(priv->mutex);

      if (priv->status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, key, priv->theclass->getName());
	 return 0;
      }
      
      v = priv->data->takeKeyValue(key);
   }

   return v;
}

void QoreObject::removeMember(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   removeMember(enc->getBuffer(), xsink);
}

void QoreObject::removeMember(const char *key, ExceptionSink *xsink) {
   discard(takeMember(key, xsink), xsink);
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
   return priv->getSlice(value_list, xsink);
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

   printd(5, "QoreObject::getReferencedMemberNoMethod(this=%p, mem=%p (%s), xsink=%p, data->size()=%d)\n",
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
   bool inclass = runtimeCheckPrivateClassAccess(priv->theclass);

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
// only called for deletes - typeinfo not needed
AbstractQoreNode **QoreObject::getExistingValuePtr(const char *mem, AutoVLock *vl, ExceptionSink *xsink) const {
   // check for illegal access
   if (priv->checkMemberAccess(mem, xsink))
      return 0;

   // do lock handoff
   qore_object_lock_handoff_helper qolhm(const_cast<qore_object_private *>(priv), *vl);

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

AbstractPrivateData *QoreObject::getAndClearPrivateData(qore_classid_t key, ExceptionSink *xsink) {
   AutoLocker al(priv->mutex);

   if (priv->privateData)
      return priv->privateData->getAndClearPtr(key);

   return 0;
}

// called only during constructor execution, therefore no need for locking
void QoreObject::setPrivate(qore_classid_t key, AbstractPrivateData *pd) { 
   if (!priv->privateData)
      priv->privateData = new KeyList();
   priv->privateData->insert(key, pd);
   priv->addVirtualPrivateData(key, pd);
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

void QoreObject::defaultSystemDestructor(qore_classid_t classID, ExceptionSink *xsink) {
   AbstractPrivateData *pd = getAndClearPrivateData(classID, xsink);
   printd(5, "QoreObject::defaultSystemDestructor() this=%p class=%s private_data=%p\n", this, priv->theclass->getName(), pd); 
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
   QoreContainerHelper cch(this);
   if (!cch) {
      str.sprintf("{ERROR: recursive reference to object %p (class %s)}", this, getClassName());
      return 0;
   }

   QoreHashNodeHolder h(copyData(xsink), xsink);
   if (*xsink)
      return -1;

   if (foff == FMT_YAML_SHORT) {
      str.sprintf("{<%s object>", getClassName());
      if (!h->empty()) {
	 str.concat(": ");
	 ConstHashIterator hi(*h);
      
	 while (hi.next()) {
	    str.sprintf("%s: ", hi.getKey());
	    const AbstractQoreNode *n = hi.getValue();
	    if (!n) n = &Nothing;
	    if (n->getAsString(str, foff, xsink))
	       return -1;
	    if (!hi.last())
	       str.concat(", ");
	 }
      }
      str.concat('}');
      return 0;      
   }

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
      str.concat('(');
      if (foff != FMT_NONE)
         str.sprintf("%d member%s)\n", h->size(), h->size() == 1 ? "" : "s");

      //bool private_access_ok = runtimeCheckPrivateClassAccess(priv->theclass);

      ConstHashIterator hi(*h);
      while (hi.next()) {
	 // skip private members when accessed outside the class
	 //if (!private_access_ok && priv->checkMemberAccessIntern(hi.getKey(), false, false) == QOA_PRIV_ERROR)
	 //   continue;

         if (foff != FMT_NONE)
            str.addch(' ', foff + 2);

         str.sprintf("%s : ", hi.getKey());

	 const AbstractQoreNode *n = hi.getValue();
	 if (!n) n = &Nothing;
	 if (n->getAsString(str, foff != FMT_NONE ? foff + 2 : foff, xsink))
	    return -1;

         if (!hi.last()) {
            if (foff != FMT_NONE)
               str.concat('\n');
            else
               str.concat(", ");
	 }
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
bool QoreObject::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const QoreObject *o = dynamic_cast<const QoreObject *>(v);
   if (!o)
      return false;
   return !compareSoft(o, xsink);
}

bool QoreObject::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const QoreObject *o = dynamic_cast<const QoreObject *>(v);
   if (!o)
      return false;
   return !compareHard(o, xsink);
}

// returns the type name as a c string
const char *QoreObject::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode *QoreObject::evalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

AbstractQoreNode *QoreObject::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

int64 QoreObject::bigIntEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

int QoreObject::integerEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0;
}

bool QoreObject::boolEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return false;
}

double QoreObject::floatEvalImpl(ExceptionSink *xsink) const {
   assert(false);
   return 0.0;
}

bool QoreObject::hasMemberNotification() const {
   return priv->theclass->hasMemberNotification();
}

void QoreObject::execMemberNotification(const char *member, ExceptionSink *xsink) {
   priv->theclass->execMemberNotification(this, member, xsink);
}

AbstractQoreNode **QoreObject::getMemberValuePtrForInitialization(const char *member) {
   return priv->data->getKeyValuePtr(member);
}
