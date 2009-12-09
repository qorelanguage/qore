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
#include <qore/intern/QoreClassIntern.h>

#include <stdlib.h>
#include <assert.h>

#include <map>

#define OS_OK             0
#define OS_DELETED       -1

#define QOA_OK          0
#define QOA_PRIV_ERROR  1
#define QOA_PUB_ERROR   2

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
	    str->sprintf("%d=<0x%p>, ", i->first, i->second.first);
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

class qore_object_private {
public:
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
      QoreObject *obj;

      DLLLOCAL qore_object_private(QoreObject *n_obj, const QoreClass *oc, QoreProgram *p, QoreHashNode *n_data) : 
	 theclass(oc), status(OS_OK), 
#ifdef QORE_CLASS_SYNCHRONOUS
	 sync_vrm(oc->has_synchronous_in_hierarchy() ? new VRMutex : 0),
#endif 
	 privateData(0), data(n_data), pgm(p), system_object(!p), delete_blocker_run(false), in_destructor(false),
	 obj(n_obj)
      {
#ifdef QORE_DEBUG_OBJ_REFS
	 printd(QORE_DEBUG_OBJ_REFS, "QoreObject::QoreObject() this=%p, pgm=%p, class=%s, references 0->1\n", this, p, oc->getName());
#endif
	 /* instead of referencing the class, we reference the program, because the
	    program contains the namespace that contains the class, and the class'
	    methods may call functions in the program as well that could otherwise
	    disappear when the program is deleted
	 */
	 if (p) {
	    printd(5, "QoreObject::init() this=%p (%s) calling QoreProgram::depRef() (%p)\n", this, theclass->getName(), p);
	    p->depRef();
	 }
      }

      DLLLOCAL ~qore_object_private() {
	 assert(!pgm);
	 assert(!data);
	 assert(!privateData);
      }

      DLLLOCAL QoreHashNode *getSlice(const QoreListNode *l, ExceptionSink *xsink) const {
	 AutoLocker al(mutex);

	 if (status == OS_DELETED) {
	    makeAccessDeletedObjectException(xsink, theclass->getName());
	    return 0;
	 }

	 bool has_public_members = theclass->runtimeHasPublicMembersInHierarchy();
	 bool private_access_ok = runtimeCheckPrivateClassAccess(theclass);

	 // check key list if necessary
	 if (has_public_members || !private_access_ok) {
	    ReferenceHolder<QoreListNode> nl(new QoreListNode, xsink);
	    ConstListIterator li(l);
	    while (li.next()) {
	       QoreStringValueHelper key(li.getValue(), QCS_DEFAULT, xsink);
	       if (*xsink)
		  return 0;

	       int rc = checkMemberAccessIntern(key->getBuffer(), has_public_members, private_access_ok);
	       if (!rc)
		  nl->push(new QoreStringNode(*key));
	       else if (rc == QOA_PUB_ERROR) {
		  doPublicException(key->getBuffer(), xsink);
		  return 0;
	       }
	    }
	 }

	 return data->getSlice(l, xsink);
      }

      DLLLOCAL int checkMemberAccessIntern(const char *mem, bool has_public_members, bool private_access_ok) const {
	 // check public access
	 if (has_public_members) {
	    bool priv_member;
	    if (!theclass->isPublicOrPrivateMember(mem, priv_member))
	       return QOA_PUB_ERROR;

	    if (priv_member && !private_access_ok)
	       return QOA_PRIV_ERROR;

	    return QOA_OK;
	 }

	 // if accessed outside the class and the member is a private member
	 return (!private_access_ok && theclass->isPrivateMember(mem)) ? QOA_PRIV_ERROR : QOA_OK;
      }

      DLLLOCAL int checkMemberAccess(const char *mem) const {
	 // check public access
	 if (theclass->runtimeHasPublicMembersInHierarchy()) {
	    bool priv_member;
	    if (!theclass->isPublicOrPrivateMember(mem, priv_member))
	       return QOA_PUB_ERROR;
	    
	    if (priv_member && !runtimeCheckPrivateClassAccess(theclass))
	       return QOA_PRIV_ERROR;

	    return QOA_OK;
	 }

	 // if accessed outside the class and the member is a private member
	 return (!runtimeCheckPrivateClassAccess(theclass) && theclass->isPrivateMember(mem)) ? QOA_PRIV_ERROR : QOA_OK;
      }

      DLLLOCAL int checkMemberAccess(const char *mem, ExceptionSink *xsink) const {
	 int rc = checkMemberAccess(mem);
	 if (!rc)
	    return false;

	 if (rc == QOA_PRIV_ERROR)
	    doPrivateException(mem, xsink);
	 else
	    doPublicException(mem, xsink);
	 return true;
      }

      // lock not already held
      DLLLOCAL void doDeleteIntern(ExceptionSink *xsink) {
	 printd(5, "QoreObject::doDeleteIntern() execing destructor()\n");   
	 theclass->execDestructor(obj, xsink);

	 QoreHashNode *td;
	 {
	    AutoLocker al(mutex);
	    assert(status != OS_DELETED);
	    assert(data);
	    status = OS_DELETED;
	    td = data;
	    data = 0;
	 }
	 cleanup(xsink, td);
      }

      DLLLOCAL void cleanup(ExceptionSink *xsink, QoreHashNode *td) {
	 if (privateData) {
	    delete privateData;
#ifdef DEBUG
	    privateData = 0;
#endif
	 }
   
	 if (pgm) {
	    printd(5, "QoreObject::cleanup() obj=%p (%s) calling QoreProgram::depDeref() (%p)\n", obj, theclass->getName(), pgm);
	    pgm->depDeref(xsink);
#ifdef DEBUG
	    pgm = 0;
#endif
	 }

	 td->deref(xsink);
      }

      // this method is called when there is an exception in a constructor and the object should be deleted
      DLLLOCAL void obliterate(ExceptionSink *xsink) {
	 printd(5, "QoreObject::obliterate() obj=%p class=%s %d->%d\n", obj, theclass->getName(), obj->references, obj->references - 1);

#ifdef QORE_DEBUG_OBJ_REFS
	 printd(QORE_DEBUG_OBJ_REFS, "QoreObject::obliterate(obj=%p) class=%s: references %d->%d\n", obj, getClassName(), obj->references, obj->references - 1);
#endif
	 
	 {
	    AutoLocker slr(ref_mutex);
	    if (--obj->references)
	       return;
	 }
   
	 {
	    SafeLocker sl(mutex);
	    
	    if (in_destructor || status != OS_OK) {
	       printd(5, "QoreObject::obliterate() %p data=%p in_destructor=%d status=%d\n", obj, data, in_destructor, status);
	       //printd(0, "Object lock %p unlocked (safe)\n", &mutex);
	       sl.unlock();
	       tDeref();
	       return;
	    }
	    
	    //printd(5, "Object lock %p locked   (safe)\n", &mutex);
	    printd(5, "QoreObject::obliterate() class=%s deleting obj=%p\n", theclass->getName(), obj);
	    
	    status = OS_DELETED;
	    QoreHashNode *td = data;
	    data = 0;
	    //printd(0, "Object lock %p unlocked (safe)\n", &mutex);
	    sl.unlock();
	    
	    if (privateData)
	       privateData->derefAll(xsink);
	    
	    cleanup(xsink, td);
	 }
	 tDeref();
      }

      DLLLOCAL void doPrivateException(const char *mem, ExceptionSink *xsink) const {
	 xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, theclass->getName());
      }

      DLLLOCAL void doPublicException(const char *mem, ExceptionSink *xsink) const {
	 xsink->raiseException("INVALID-MEMBER", "'%s' is not a registered member of class '%s'", mem, theclass->getName());
      }

      DLLLOCAL void tRef() const {
#ifdef QORE_DEBUG_OBJ_REFS
	 printd(QORE_DEBUG_OBJ_REFS, "QoreObject::tRef() obj=%p class=%s: tref %d->%d\n", obj, theclass->getName(), tRefs.reference_count(), tRefs.reference_count() + 1);
#endif
	 tRefs.ROreference();
      }

      DLLLOCAL void tDeref() {
#ifdef QORE_DEBUG_OBJ_REFS
	 printd(QORE_DEBUG_OBJ_REFS, "QoreObject::tDeref() obj=%p class=%s: tref %d->%d\n", obj, theclass->getName(), tRefs.reference_count(), tRefs.reference_count() - 1);
#endif
	 if (tRefs.ROdereference())
	    delete obj;
      }

      // add virtual IDs for private data to class list
      DLLLOCAL void addVirtualPrivateData(AbstractPrivateData *apd) {
	 BCSMList *sml = theclass->getBCSMList();
	 if (!sml)
	    return;

	 for (class_list_t::const_iterator i = sml->begin(), e = sml->end(); i != e; ++i)
	    if ((*i).second)
	       privateData->insertVirtual((*i).first->getID(), apd);
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
   priv->doDeleteIntern(xsink);
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, new QoreHashNode())) {
}

QoreObject::QoreObject(const QoreClass *oc, QoreProgram *p, AbstractPrivateData *data) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, new QoreHashNode())) {
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
      return reinterpret_cast<BuiltinNormalMethod*>(meth)->eval(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
   return check_meth_eval(priv->theclass, meth, xsink);
}

AbstractQoreNode *QoreObject::evalBuiltinMethodWithPrivateData(const QoreMethod &method, BuiltinMethod *meth, const QoreListNode *args, ExceptionSink *xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->myclass->getIDForMethod(), xsink), xsink);

   if (pd)
      return reinterpret_cast<BuiltinNormalMethod2*>(meth)->eval(method, this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
   return check_meth_eval(priv->theclass, meth, xsink);
}

void QoreObject::evalCopyMethodWithPrivateData(const QoreClass &thisclass, BuiltinMethod *meth, QoreObject *self, bool new_calling_convention, ExceptionSink *xsink) {
   // get referenced object
   AbstractPrivateData *pd = getReferencedPrivateData(meth->myclass->getID(), xsink);

   if (pd) {
      if (new_calling_convention)
	 reinterpret_cast<BuiltinCopy2*>(meth)->eval(thisclass, self, this, pd, xsink);
      else
	 reinterpret_cast<BuiltinCopy*>(meth)->eval(thisclass, self, this, pd, xsink);
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
      return reinterpret_cast<BuiltinDeleteBlocker*>(meth)->eval(this, *pd);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, meth->myclass->getName(), meth->getName(), meth->myclass->getID(), meth->myclass->getIDForMethod());
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
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customRefIntern(this=%p) class=%s: references %d->%d\n", this, getClassName(), references, references + 1);
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
      printd(5, "QoreObject::customDeref() this=%p, class=%s references=%d->%d status=%d has_delete_blocker=%d delete_blocker_run=%d\n", this, getClassName(), references, references - 1, priv->status, priv->theclass->has_delete_blocker(), priv->delete_blocker_run);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customDeref(this=%p) class=%s: references %d->%d\n", this, getClassName(), references, references - 1);
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
	    //printd(5, "QoreObject::derefImpl() this=%p class=%s blocking delete\n", this, getClassName());
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

   tDeref();
}

// this method is called when there is an exception in a constructor and the object should be deleted
void QoreObject::obliterate(ExceptionSink *xsink) {
   priv->obliterate(xsink);
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

	 //printd(0, "Object lock %p locked   (handoff by %s - vlock by %s)\n", &self->priv->m, who, vl->getWho());
	 // lock current object
	 self->priv->mutex.lock();
      }

      DLLLOCAL ~qore_object_lock_handoff_manager() {
	 // unlock if lock not saved in AutoVLock structure
	 if (self) {
	    //printd(0, "Object lock %p unlocked (handoff)\n", &self->priv->mutex);
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
   if (priv->checkMemberAccess(key, xsink))
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

void QoreObject::removeMember(const QoreString *key, ExceptionSink *xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   removeMember(enc->getBuffer(), xsink);
}

void QoreObject::removeMember(const char *key, ExceptionSink *xsink) {
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
AbstractQoreNode **QoreObject::getExistingValuePtr(const char *mem, AutoVLock *vl, ExceptionSink *xsink) const {
   // check for illegal access
   if (priv->checkMemberAccess(mem, xsink))
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
   priv->addVirtualPrivateData(pd);
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

#ifdef QORE_CLASS_SYNCHRONOUS
VRMutex *QoreObject::getClassSyncLock() {
   return priv->sync_vrm;
}
#endif

AbstractQoreNode **QoreObject::getMemberValuePtrForInitialization(const char *member) {
   return priv->data->getKeyValuePtr(member);
}
