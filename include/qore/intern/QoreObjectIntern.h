/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreObjectIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QOREOBJECTINTERN_H

#define _QORE_QOREOBJECTINTERN_H

#include <stdlib.h>
#include <assert.h>

#include <map>
#include <set>
#include <vector>

//#define _QORE_CYCLE_CHECK 1
#ifdef _QORE_CYCLE_CHECK
#define QORE_DEBUG_OBJ_REFS 0
#define QRO_LVL 0
#else
#define QORE_DEBUG_OBJ_REFS 5
#define QRO_LVL 1
#endif

#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/RSection.h>
#include <qore/intern/RSet.h>

#define OS_OK            0
#define OS_DELETED      -1

// object access constants
#define QOA_OK           0
#define QOA_PRIV_ERROR   1
#define QOA_PUB_ERROR    2

class LValueHelper;

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
typedef std::pair<AbstractPrivateData*, bool> private_pair_t;

// mapping from qore class ID to the object data
typedef std::map<qore_classid_t, private_pair_t> keymap_t;

// for objects with multiple classes, private data has to be keyed
class KeyList {
private:
   keymap_t keymap;

public:
   DLLLOCAL AbstractPrivateData* getReferencedPrivateData(qore_classid_t key) const {
      keymap_t::const_iterator i = keymap.find(key);
      if (i == keymap.end())
	 return 0;

      AbstractPrivateData* apd = i->second.first;
      apd->ref();
      return apd;
   }

   DLLLOCAL void addToString(QoreString* str) const {
      for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	 str->sprintf("%d=<%p>, ", i->first, i->second.first);
   }

   DLLLOCAL void derefAll(ExceptionSink* xsink) const {
      for (keymap_t::const_iterator i = keymap.begin(), e = keymap.end(); i != e; ++i)
	 if (!i->second.second)
	    i->second.first->deref(xsink);
   }

   DLLLOCAL AbstractPrivateData* getAndClearPtr(qore_classid_t key) {
      keymap_t::iterator i = keymap.find(key);
      if (i == keymap.end() || i->second.second)
	 return 0;

      return i->second.first;
   }

   DLLLOCAL AbstractPrivateData* getAndRemovePtr(qore_classid_t key) {
      keymap_t::iterator i = keymap.find(key);
      if (i == keymap.end() || i->second.second)
	 return 0;

      AbstractPrivateData* rv = i->second.first;
      i->second.first = 0;
      return rv;
   }

   DLLLOCAL void insert(qore_classid_t key, AbstractPrivateData* pd) {
      assert(pd);
      assert(keymap.find(key) == keymap.end());
      keymap.insert(std::make_pair(key, std::make_pair(pd, false)));
   }

   DLLLOCAL void insertVirtual(qore_classid_t key, AbstractPrivateData* pd) {
      assert(pd);
      if (keymap.find(key) == keymap.end())
	 keymap.insert(std::make_pair(key, std::make_pair(pd, true)));
   }
};

class qore_object_private : public RObject {
public:
   const QoreClass* theclass;
   int status;

   // used for weak references, to ensure that assignments will not deadlock when the object is locked for update
   mutable QoreThreadLock ref_mutex;
   KeyList* privateData;
   // member data
   QoreHashNode* data;
   QoreProgram* pgm;

   bool system_object, delete_blocker_run, in_destructor;
   bool recursive_ref_found;

   QoreObject* obj;

   DLLLOCAL qore_object_private(QoreObject* n_obj, const QoreClass *oc, QoreProgram* p, QoreHashNode* n_data);

   DLLLOCAL ~qore_object_private();

   DLLLOCAL void plusEquals(const AbstractQoreNode* v, AutoVLock& vl, ExceptionSink* xsink) {
      if (!v)
         return;

      // do not need ensure_unique() for objects
      if (v->getType() == NT_OBJECT) {
         ReferenceHolder<QoreHashNode> h(const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(v))->copyData(xsink), xsink);
         if (h)
            merge(*h, vl, xsink);
      }
      else if (v->getType() == NT_HASH)
         merge(reinterpret_cast<const QoreHashNode* >(v), vl, xsink);
   }

   DLLLOCAL void merge(const QoreHashNode* h, AutoVLock& vl, ExceptionSink* xsink);

   DLLLOCAL int getLValue(const char* key, LValueHelper& lvh, bool internal, bool for_remove, ExceptionSink* xsink) const;

   DLLLOCAL AbstractQoreNode* *getMemberValuePtr(const char* key, AutoVLock *vl, const QoreTypeInfo*& typeInfo, ExceptionSink* xsink) const;

   DLLLOCAL QoreStringNode* firstKey(ExceptionSink* xsink) {
      QoreAutoVarRWReadLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return 0;
      }

      if (qore_class_private::runtimeCheckPrivateClassAccess(*theclass)) {
         const char* str = data->getFirstKey();
         //printd(5, "qore_object_private::firstKey() got %p (%s)\n", str, str ? str : "<null>");
         return !str ? 0 : new QoreStringNode(str);
      }

      // get first public member
      ConstHashIterator hi(data);
      while (hi.next()) {
         //printd(5, "qore_object_private::firstKey() checking '%s'\n", hi.getKey());
         if (!checkMemberAccessIntern(hi.getKey(), false, false))
            return new QoreStringNode(hi.getKey());
         //printd(5, "qore_object_private::firstKey() skipping '%s' (private)\n", hi.getKey());
      }

      return 0;
   }

   DLLLOCAL QoreStringNode* lastKey(ExceptionSink* xsink) {
      QoreAutoVarRWReadLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return 0;
      }

      if (qore_class_private::runtimeCheckPrivateClassAccess(*theclass)) {
         const char* str = data->getLastKey();
         return !str ? 0 : new QoreStringNode(str);
      }

      // get first public member
      ReverseConstHashIterator hi(data);
      while (hi.next()) {
         if (!checkMemberAccessIntern(hi.getKey(), false, false))
            return new QoreStringNode(hi.getKey());
      }

      return 0;
   }

   DLLLOCAL QoreHashNode* getSlice(const QoreListNode* l, ExceptionSink* xsink) const {
      QoreSafeVarRWReadLocker sl(rml);

      if (status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, theclass->getName());
	 return 0;
      }

      bool has_public_members = theclass->runtimeHasPublicMembersInHierarchy();
      bool private_access_ok = qore_class_private::runtimeCheckPrivateClassAccess(*theclass);

      // check key list if necessary
      if (has_public_members || !private_access_ok) {
	 ReferenceHolder<QoreListNode> nl(new QoreListNode, xsink);
	 ReferenceHolder<QoreListNode> mgl(theclass->hasMemberGate() ? new QoreListNode : 0, xsink);

	 ConstListIterator li(l);
	 while (li.next()) {
	    QoreStringValueHelper key(li.getValue(), QCS_DEFAULT, xsink);
	    if (*xsink)
	       return 0;

	    int rc = checkMemberAccessIntern(key->getBuffer(), has_public_members, private_access_ok);
	    if (!rc)
	       nl->push(new QoreStringNode(*key));
	    else {
	       if (theclass->hasMemberGate())
	          mgl->push(new QoreStringNode(*key));
	       else if (rc == QOA_PUB_ERROR) {
	          doPublicException(key->getBuffer(), xsink);
	          return 0;
	       }
	       else {
	          doPrivateException(key->getBuffer(), xsink);
	          return 0;
	       }
	    }
	 }

	 ReferenceHolder<QoreHashNode> rv(data->getSlice(*nl, xsink), xsink);
	 if (*xsink)
	    return 0;
	 if (mgl && !mgl->empty()) {
	    // unlock lock and execute memberGate() method for each method in the memger gate list (mgl)
	    sl.unlock();

	    ConstListIterator mgli(*mgl);
	    while (mgli.next()) {
	       const QoreStringNode* k = reinterpret_cast<const QoreStringNode*>(mgli.getValue());
               ValueHolder n(theclass->evalMemberGate(obj, k, xsink), xsink);
	       //AbstractQoreNode* n = theclass->evalMemberGate(obj, k, xsink);
	       if (*xsink)
	          return 0;
	       rv->setKeyValue(k->getBuffer(), n.getReferencedValue(), xsink);
	    }
	 }
	 return rv.release();
      }

      return data->getSlice(l, xsink);
   }

   DLLLOCAL int checkMemberAccessIntern(const char* mem, bool has_public_members, bool private_access_ok) const {
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

   DLLLOCAL int checkMemberAccess(const char* mem) const {
      // check public access
      if (theclass->runtimeHasPublicMembersInHierarchy()) {
         ClassAccess access;
         const qore_class_private* qc = qore_class_private::getMemberClass(*theclass, mem, access);
	 if (!qc)
	    return QOA_PUB_ERROR;

	 if ((access > Public) && !qc->runtimeCheckPrivateClassAccess())
	    return QOA_PRIV_ERROR;

	 return QOA_OK;
      }

      // if accessed outside the class and the member is a private member
      return (!qore_class_private::runtimeCheckPrivateClassAccess(*theclass) && theclass->isPrivateMember(mem)) ? QOA_PRIV_ERROR : QOA_OK;
   }

   DLLLOCAL int checkMemberAccess(const char* mem, ExceptionSink* xsink) const {
      int rc = checkMemberAccess(mem);
      if (!rc)
	 return 0;

      if (rc == QOA_PRIV_ERROR)
	 doPrivateException(mem, xsink);
      else
	 doPublicException(mem, xsink);
      return -1;
   }

   DLLLOCAL int checkMemberAccessGetTypeInfo(ExceptionSink* xsink, const char* mem, const QoreTypeInfo*& typeInfo, bool check_access = true) const {
      ClassAccess access;
      const QoreMemberInfo* mi = qore_class_private::runtimeGetMemberInfo(*theclass, mem, access);
      if (mi) {
	 if (access > Public && check_access && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass)) {
	    doPrivateException(mem, xsink);
	    return -1;
	 }

         typeInfo = mi->getTypeInfo();
	 return 0;
      }

      // member is not declared
      if (theclass->runtimeHasPublicMembersInHierarchy()) {
	 doPublicException(mem, xsink);
	 return -1;
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* takeMember(ExceptionSink* xsink, const char* mem, bool check_access = true);

   DLLLOCAL AbstractQoreNode* takeMember(LValueHelper& lvh, const char* mem);

   DLLLOCAL void takeMembers(QoreLValueGeneric& rv, LValueHelper& lvh, const QoreListNode* l);

   // lock not held on entry
   DLLLOCAL void doDeleteIntern(ExceptionSink* xsink) {
      printd(5, "qore_object_private::doDeleteIntern() execing destructor() obj=%p\n", obj);

      // increment reference count temporarily for destructor
      {
	 AutoLocker slr(ref_mutex);
	 ++obj->references;
      }

      theclass->execDestructor(obj, xsink);

      QoreHashNode* td;
      {
         QoreAutoVarRWWriteLocker al(rml);
	 assert(status != OS_DELETED);
	 assert(data);
	 status = OS_DELETED;
	 td = data;
	 data = 0;

         removeInvalidateRSet();
      }

      cleanup(xsink, td);

      obj->deref(xsink);
   }

   DLLLOCAL void cleanup(ExceptionSink* xsink, QoreHashNode* td) {
      if (privateData) {
         printd(5, "qore_object_private::cleanup() this: %p privateData: %p\n", this, privateData);
	 delete privateData;
#ifdef DEBUG
	 privateData = 0;
#endif
      }

      {
         QoreAutoVarRWWriteLocker al(rml);

         if (pgm) {
            printd(5, "qore_object_private::cleanup() obj=%p (%s) calling QoreProgram::depDeref() (%p)\n", obj, theclass->getName(), pgm);
            // release weak reference
            pgm->depDeref(xsink);
            pgm = 0;
         }
      }
      td->clear(xsink, true);
      td->deref(xsink);
   }

   // this method is called when there is an exception in a constructor and the object should be deleted
   DLLLOCAL void obliterate(ExceptionSink* xsink) {
      printd(5, "qore_object_private::obliterate() obj=%p class=%s %d->%d\n", obj, theclass->getName(), obj->references, obj->references - 1);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::obliterate() obj=%p class=%s: references %d->%d\n", obj, theclass->getName(), obj->references, obj->references - 1);
#endif

      {
	 AutoLocker slr(ref_mutex);
	 if (--obj->references)
	    return;
      }

      {
         QoreSafeVarRWWriteLocker sl(rml);

	 if (in_destructor || status != OS_OK) {
	    printd(5, "qore_object_private::obliterate() obj=%p data=%p in_destructor=%d status=%d\n", obj, data, in_destructor, status);
	    //printd(5, "Object lock %p unlocked (safe)\n", &rml);
	    sl.unlock();
	    tDeref();
	    return;
	 }

	 //printd(5, "Object lock %p locked   (safe)\n", &rml);
	 printd(5, "qore_object_private::obliterate() obj=%p class=%s\n", obj, theclass->getName());

	 status = OS_DELETED;
	 QoreHashNode* td = data;
	 data = 0;

         removeInvalidateRSet();

	 //printd(5, "Object lock %p unlocked (safe)\n", &rml);
	 sl.unlock();

	 if (privateData)
	    privateData->derefAll(xsink);

	 cleanup(xsink, td);
      }
      tDeref();
   }

   DLLLOCAL void doPrivateException(const char* mem, ExceptionSink* xsink) const {
      xsink->raiseException("PRIVATE-MEMBER", "'%s' is a private member of class '%s'", mem, theclass->getName());
   }

   DLLLOCAL void doPublicException(const char* mem, ExceptionSink* xsink) const {
      xsink->raiseException("INVALID-MEMBER", "'%s' is not a registered member of class '%s'", mem, theclass->getName());
   }

   DLLLOCAL virtual const char* getName() const {
      return theclass->getName();
   }

   DLLLOCAL virtual void deleteObject() {
      delete obj;
   }

   DLLLOCAL virtual bool isValidImpl() const {
      if (status != OS_OK || in_destructor) {
         printd(QRO_LVL, "qore_object_intern::isValidImpl() this: %p cannot delete graph obj '%s' status: %d in_destructor: %d\n", this, theclass->getName(), status, in_destructor);
         return false;
      }
      return true;
   }

   DLLLOCAL virtual bool scanMembers(RSetHelper& rsh);

   DLLLOCAL virtual bool needsScan() const {
      return (bool)getScanCount();
   }

   DLLLOCAL void setPrivate(qore_classid_t key, AbstractPrivateData* pd) {
      if (!privateData)
         privateData = new KeyList();
      //printd(5, "qore_object_private::setPrivate() this: %p 2:privateData: %p (%s) key: %d pd: %p\n", this, privateData, theclass->getName(), key, pd);
      privateData->insert(key, pd);
      addVirtualPrivateData(key, pd);
   }

   // add virtual IDs for private data to class list
   DLLLOCAL void addVirtualPrivateData(qore_classid_t key, AbstractPrivateData* apd) {
      // first get parent class corresponding to "key"
      QoreClass* qc = theclass->getClass(key);
      //printd(5, "qore_object_private::addVirtualPrivateData() this: %p privateData: %p key: %d apd: %p qc: %p '%s'\n", this, privateData, key, apd, qc, qc->getName());
      assert(qc);
      BCSMList* sml = qc->getBCSMList();
      //printd(5, "qore_object_private::addVirtualPrivateData() this: %p qc: %p '%s' sml: %p\n", this, qc, qc->getName(), sml);
      if (!sml)
	 return;

      for (class_list_t::const_iterator i = sml->begin(), e = sml->end(); i != e; ++i) {
         //printd(5, "qore_object_private::addVirtualPrivateData() this: %p i: %p '%s' key: %d virt: %s\n", this, i->first, i->first->getName(), i->first->getID(), i->second ? "true" : "false");
	 if (i->second)
	    privateData->insertVirtual(i->first->getID(), apd);
      }
   }

   DLLLOCAL unsigned getScanCount() const;

   DLLLOCAL void incScanCount(int dt);

   DLLLOCAL AbstractPrivateData* getAndRemovePrivateData(qore_classid_t key, ExceptionSink* xsink) {
      QoreSafeVarRWWriteLocker sl(rml);
      return privateData ? privateData->getAndRemovePtr(key) : 0;
   }

   DLLLOCAL AbstractPrivateData* getReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const;

   DLLLOCAL QoreValue evalBuiltinMethodWithPrivateData(const QoreMethod& method, const BuiltinNormalMethodVariantBase* meth, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink);

   /*
   DLLLOCAL static bool hackId(const QoreObject& obj) {
      if (!obj.priv->data)
         return false;
      const AbstractQoreNode* n = obj.priv->data->getKeyValue("name");
      if (n && n->getType() == NT_STRING && strstr(reinterpret_cast<const QoreStringNode*>(n)->getBuffer(), "http-test"))
         return true;
      return false;
   }
   */

   DLLLOCAL static QoreValue evalBuiltinMethodWithPrivateData(QoreObject& obj, const QoreMethod& method, const BuiltinNormalMethodVariantBase* meth, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) {
      return obj.priv->evalBuiltinMethodWithPrivateData(method, meth, args, rtflags, xsink);
   }

   DLLLOCAL static qore_object_private* get(QoreObject& obj) {
      return obj.priv;
   }

   DLLLOCAL static AbstractQoreNode* takeMember(QoreObject& obj, ExceptionSink* xsink, const char* mem, bool check_access = true) {
      return obj.priv->takeMember(xsink, mem, check_access);
   }

   DLLLOCAL static AbstractQoreNode* takeMember(QoreObject& obj, LValueHelper& lvh, const char* mem) {
      return obj.priv->takeMember(lvh, mem);
   }

   DLLLOCAL static void takeMembers(QoreObject& o, QoreLValueGeneric& rv, LValueHelper& lvh, const QoreListNode* l) {
      o.priv->takeMembers(rv, lvh, l);
   }

   DLLLOCAL static int getLValue(const QoreObject& obj, const char* key, LValueHelper& lvh, bool internal, bool for_remove, ExceptionSink* xsink) {
      return obj.priv->getLValue(key, lvh, internal, for_remove, xsink);
   }

   DLLLOCAL static AbstractQoreNode* *getMemberValuePtr(const QoreObject* obj, const char* key, AutoVLock *vl, const QoreTypeInfo*& typeInfo, ExceptionSink* xsink) {
      return obj->priv->getMemberValuePtr(key, vl, typeInfo, xsink);
   }

   DLLLOCAL static void plusEquals(QoreObject* obj, const AbstractQoreNode* v, AutoVLock& vl, ExceptionSink* xsink) {
      obj->priv->plusEquals(v, vl, xsink);
   }

   DLLLOCAL static QoreStringNode* firstKey(QoreObject* obj, ExceptionSink* xsink) {
      return obj->priv->firstKey(xsink);
   }

   DLLLOCAL static QoreStringNode* lastKey(QoreObject* obj, ExceptionSink* xsink) {
      return obj->priv->lastKey(xsink);
   }

   DLLLOCAL static unsigned getScanCount(const QoreObject& o) {
      return o.priv->getScanCount();
   }

   DLLLOCAL static void incScanCount(const QoreObject& o, int dt) {
      o.priv->incScanCount(dt);
   }
};

class qore_object_lock_handoff_helper {
private:
   qore_object_private* pobj;
   AutoVLock& vl;

public:
   DLLLOCAL qore_object_lock_handoff_helper(qore_object_private* n_pobj, AutoVLock& n_vl) : pobj(n_pobj), vl(n_vl) {
      if (pobj->obj == vl.getObject()) {
	 assert(vl.getRWL() == &pobj->rml);
	 vl.clear();
	 return;
      }

      // reference current object
      pobj->obj->tRef();

      // unlock previous lock and release from AutoVLock structure
      vl.del();

      // lock current object
      pobj->rml.wrlock();
   }

   DLLLOCAL ~qore_object_lock_handoff_helper() {
      // unlock if lock not saved in AutoVLock structure
      if (pobj) {
	 //printd(5, "Object lock %p unlocked (handoff)\n", &pobj->rml);
	 pobj->rml.unlock();
	 pobj->obj->tDeref();
      }
   }

   DLLLOCAL void stay_locked() {
      vl.set(pobj->obj, &pobj->rml);
      pobj = 0;
   }
};

class qore_object_recursive_lock_handoff_helper {
private:
   qore_object_private* pobj;
   bool locked;

public:
   DLLLOCAL qore_object_recursive_lock_handoff_helper(qore_object_private* n_pobj, AutoVLock& n_vl) : pobj(n_pobj) /*, vl(n_vl)*/ {
      // try to lock current object
      locked = !pobj->rml.trywrlock();
   }

   DLLLOCAL ~qore_object_recursive_lock_handoff_helper() {
      // unlock current object
      if (locked)
         pobj->rml.unlock();
   }

   DLLLOCAL operator bool() const {
      return locked;
   }
};

#endif
