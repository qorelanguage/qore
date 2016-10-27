/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreObjectIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#include "qore/intern/VRMutex.h"

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

// per-class internal data
typedef std::map<char*, QoreHashNode*, ltstr> cdmap_t;

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

class VRMutex;

class qore_object_private : public RObject {
public:
   const QoreClass* theclass;
   int status = OS_OK;

   KeyList* privateData = 0;
   // member data
   QoreHashNode* data;
   QoreProgram* pgm;
   cdmap_t* cdmap = 0;

   // used for garbage collection
   mutable unsigned obj_count = 0;

   mutable VRMutex gate;

   // number of calls currently in progress
   int call_count = 0;

   // flag to force a scan after a call
   mutable bool scan_after_call = false;

   bool system_object, delete_blocker_run, in_destructor;
   bool recursive_ref_found;

   QoreObject* obj;

   DLLLOCAL qore_object_private(QoreObject* n_obj, const QoreClass *oc, QoreProgram* p, QoreHashNode* n_data);

   DLLLOCAL ~qore_object_private();

   DLLLOCAL void plusEquals(const AbstractQoreNode* v, AutoVLock& vl, ExceptionSink* xsink) {
      if (!v)
         return;

      // do not need ensure_unique() for objects
      if (v->getType() == NT_OBJECT)
         merge(*const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(v))->priv, vl, xsink);
      else if (v->getType() == NT_HASH)
         merge(reinterpret_cast<const QoreHashNode*>(v), vl, xsink);
   }

   DLLLOCAL void merge(qore_object_private& o, AutoVLock& vl, ExceptionSink* xsink);

   DLLLOCAL void merge(const QoreHashNode* h, AutoVLock& vl, ExceptionSink* xsink);

   DLLLOCAL void mergeIntern(ExceptionSink* xsink, const QoreHashNode* h, bool& check_recursive, ReferenceHolder<QoreListNode>& holder, const qore_class_private* class_ctx, const QoreHashNode* new_internal_data = 0);

   DLLLOCAL QoreHashNode* copyData(ExceptionSink* xsink) const;

   DLLLOCAL int getLValue(const char* key, LValueHelper& lvh, const qore_class_private* class_ctx, bool for_remove, ExceptionSink* xsink);

   DLLLOCAL AbstractQoreNode** getMemberValuePtr(const char* key, AutoVLock* vl, const QoreTypeInfo*& typeInfo, ExceptionSink* xsink) const;

   DLLLOCAL QoreStringNode* firstKey(ExceptionSink* xsink) {
      // get the current class context
      const qore_class_private* class_ctx = runtime_get_class();
      if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
         class_ctx = 0;

      QoreAutoVarRWReadLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return 0;
      }

      if (class_ctx) {
         const char* str = data->getFirstKey();
         //printd(5, "qore_object_private::firstKey() got %p (%s)\n", str, str ? str : "<null>");
         return !str ? 0 : new QoreStringNode(str);
      }

      // get first accessible non-internal member
      ConstHashIterator hi(data);
      while (hi.next()) {
         //printd(5, "qore_object_private::firstKey() checking '%s'\n", hi.getKey());
         bool internal_member;
         if (!checkMemberAccessIntern(hi.getKey(), false, class_ctx, internal_member))
            return new QoreStringNode(hi.getKey());
         //printd(5, "qore_object_private::firstKey() skipping '%s' (private)\n", hi.getKey());
      }

      return 0;
   }

   DLLLOCAL QoreStringNode* lastKey(ExceptionSink* xsink) {
      // get the current class context
      const qore_class_private* class_ctx = runtime_get_class();
      if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
         class_ctx = 0;

      QoreAutoVarRWReadLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return 0;
      }

      if (class_ctx) {
         const char* str = data->getLastKey();
         return !str ? 0 : new QoreStringNode(str);
      }

      // get last accessible non-internal member
      ReverseConstHashIterator hi(data);
      while (hi.next()) {
         bool internal_member;
         if (!checkMemberAccessIntern(hi.getKey(), false, class_ctx, internal_member))
            return new QoreStringNode(hi.getKey());
      }

      return 0;
   }

   DLLLOCAL QoreHashNode* getSlice(const QoreListNode* l, ExceptionSink* xsink) const {
      // get the current class context
      const qore_class_private* class_ctx = runtime_get_class();
      if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
         class_ctx = 0;
      bool has_public_members = theclass->runtimeHasPublicMembersInHierarchy();

      QoreSafeVarRWReadLocker sl(rml);

      if (status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, theclass->getName());
	 return 0;
      }


      ReferenceHolder<QoreListNode> nl(new QoreListNode, xsink);
      ReferenceHolder<QoreListNode> int_nl(xsink);
      ReferenceHolder<QoreListNode> mgl(theclass->hasMemberGate() ? new QoreListNode : 0, xsink);

      ConstListIterator li(l);
      while (li.next()) {
         QoreStringValueHelper key(li.getValue(), QCS_DEFAULT, xsink);
         if (*xsink)
            return 0;

         bool internal_member;
         int rc = checkMemberAccessIntern(key->getBuffer(), has_public_members, class_ctx, internal_member);
         if (!rc) {
            if (internal_member) {
               if (!int_nl)
                  int_nl = new QoreListNode;
               int_nl->push(new QoreStringNode(*key));
            }
            else
               nl->push(new QoreStringNode(*key));
         }
         else {
            if (mgl)
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
      // get internal members
      if (int_nl) {
         assert(class_ctx);
         const QoreHashNode* odata = getInternalData(class_ctx);
         if (odata) {
            ConstListIterator li(*int_nl);
            while (li.next()) {
               const char* k = reinterpret_cast<const QoreStringNode*>(li.getValue())->c_str();
               bool exists;
               const AbstractQoreNode* v = odata->getKeyValueExistence(k, exists);
               if (!exists)
                  continue;
               rv->setKeyValue(k, v ? v->refSelf() : 0, xsink);
               if (*xsink)
                  return 0;
            }
         }
      }
      if (mgl && !mgl->empty()) {
         // unlock lock and execute memberGate() method for each method in the member gate list (mgl)
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

   DLLLOCAL int checkMemberAccessIntern(const char* mem, bool has_public_members, const qore_class_private* class_ctx, bool& internal_member) const {
      ClassAccess access;
      const qore_class_private* qc = qore_class_private::runtimeGetMemberClass(*theclass, mem, access, class_ctx, internal_member);
      if (!qc)
         return has_public_members ? QOA_PUB_ERROR : QOA_OK;
      // if internal_member is true, then private access has already been verified
      if (internal_member)
         return QOA_OK;

      return ((access > Public) && !class_ctx) ? QOA_PRIV_ERROR : QOA_OK;
   }

   // must be called in the object read lock
   DLLLOCAL const QoreHashNode* getInternalData(const qore_class_private* class_ctx) const {
      if (!cdmap)
         return 0;
      cdmap_t::const_iterator i = cdmap->find(class_ctx->getHash());
      return i != cdmap->end() ? i->second : 0;
   }

   // must be called in the object read lock
   DLLLOCAL QoreHashNode* getInternalData(const qore_class_private* class_ctx) {
      if (!cdmap)
         return 0;
      cdmap_t::iterator i = cdmap->find(class_ctx->getHash());
      return i != cdmap->end() ? i->second : 0;
   }

   // must be called in the object write lock
   DLLLOCAL QoreHashNode* getCreateInternalData(const qore_class_private* class_ctx) {
      if (cdmap) {
         cdmap_t::iterator i = cdmap->find(class_ctx->getHash());
         if (i != cdmap->end())
            return i->second;
      }
      else
         cdmap = new cdmap_t;

      QoreHashNode* id = new QoreHashNode;
      cdmap->insert(cdmap_t::value_type(class_ctx->getHash(), id));
      return id;
   }

   DLLLOCAL void setValue(const char* key, AbstractQoreNode* val, ExceptionSink* xsink);

   DLLLOCAL void setValueIntern(const qore_class_private* class_ctx, const char* key, AbstractQoreNode* val, ExceptionSink* xsink);

   DLLLOCAL int checkMemberAccess(const char* mem, const qore_class_private* class_ctx, bool& internal_member) const {
      ClassAccess access;
      const qore_class_private* qc = qore_class_private::runtimeGetMemberClass(*theclass, mem, access, class_ctx, internal_member);
      if (!qc)
         return theclass->runtimeHasPublicMembersInHierarchy() ? QOA_PUB_ERROR : QOA_OK;
      // if internal_member is true, then private access has already been verified
      if (internal_member)
         return QOA_OK;

      return ((access > Public) && !class_ctx) ? QOA_PRIV_ERROR : QOA_OK;
   }

   DLLLOCAL int checkMemberAccess(const char* mem, const qore_class_private* class_ctx, bool& internal_member, ExceptionSink* xsink) const {
      int rc = checkMemberAccess(mem, class_ctx, internal_member);
      if (!rc)
	 return 0;

      if (rc == QOA_PRIV_ERROR)
	 doPrivateException(mem, xsink);
      else
	 doPublicException(mem, xsink);
      return -1;
   }

   DLLLOCAL int checkMemberAccessGetTypeInfo(ExceptionSink* xsink, const char* mem, const qore_class_private* class_ctx, bool& internal_member, const QoreTypeInfo*& typeInfo) const {
      ClassAccess access;
      const QoreMemberInfo* mi = qore_class_private::runtimeGetMemberInfo(*theclass, mem, access, class_ctx, internal_member);
      if (mi) {
	 if (access > Public && !class_ctx) {
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

   DLLLOCAL AbstractQoreNode* getReferencedMemberNoMethod(const char* mem, ExceptionSink* xsink) const;

   // lock not held on entry
   DLLLOCAL void doDeleteIntern(ExceptionSink* xsink) {
      printd(5, "qore_object_private::doDeleteIntern() execing destructor() obj: %p\n", obj);

      // increment reference count temporarily for destructor
      {
	 AutoLocker slr(ref_mutex);
	 ++obj->references;
      }

      theclass->execDestructor(obj, xsink);

      cdmap_t* cdm;
      QoreHashNode* td;
      {
         QoreAutoVarRWWriteLocker al(rml);
	 assert(status != OS_DELETED);
	 assert(data);
	 status = OS_DELETED;

         cdm = cdmap;
         cdmap = 0;

	 td = data;
	 data = 0;

         removeInvalidateRSetIntern();
      }

      cleanup(xsink, td, cdm);

      obj->deref(xsink);
   }

   DLLLOCAL void cleanup(ExceptionSink* xsink, QoreHashNode* td, cdmap_t* cdm) {
      if (privateData) {
         printd(5, "qore_object_private::cleanup() this: %p privateData: %p\n", this, privateData);
	 delete privateData;
#ifdef DEBUG
	 privateData = 0;
#endif
      }

      td->clear(xsink, true);
      td->deref(xsink);

      if (cdm) {
         for (auto& i : *cdm) {
            i.second->clear(xsink, true);
            i.second->deref(xsink);
         }
         delete cdm;
      }
   }

   // this method is called when there is an exception in a constructor and the object should be deleted
   DLLLOCAL void obliterate(ExceptionSink* xsink) {
      printd(5, "qore_object_private::obliterate() obj: %p class: %s %d->%d\n", obj, theclass->getName(), obj->references, obj->references - 1);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::obliterate() obj: %p class: %s: references %d->%d\n", obj, theclass->getName(), obj->references, obj->references - 1);
#endif

      {
	 AutoLocker slr(ref_mutex);
	 if (--obj->references)
	    return;
      }

      {
         QoreSafeVarRWWriteLocker sl(rml);

	 if (in_destructor || status != OS_OK) {
	    printd(5, "qore_object_private::obliterate() obj: %p data: %p in_destructor: %d status: %d\n", obj, data, in_destructor, status);
	    //printd(5, "Object lock %p unlocked (safe)\n", &rml);
	    sl.unlock();
	    tDeref();
	    return;
	 }

	 //printd(5, "Object lock %p locked   (safe)\n", &rml);
	 printd(5, "qore_object_private::obliterate() obj: %p class: %s\n", obj, theclass->getName());

	 status = OS_DELETED;
         cdmap_t* cdm = cdmap;
         cdmap = 0;
	 QoreHashNode* td = data;
	 data = 0;

         removeInvalidateRSetIntern();

	 //printd(5, "Object lock %p unlocked (safe)\n", &rml);
	 sl.unlock();

	 if (privateData)
	    privateData->derefAll(xsink);

	 cleanup(xsink, td, cdm);
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
         printd(QRO_LVL, "qore_object_intern::isValidImpl() this: %p cannot delete graph obj status: %d in_destructor: %d\n", this, status, in_destructor);
         return false;
      }
      return true;
   }

   DLLLOCAL virtual bool scanMembersIntern(RSetHelper& rsh, QoreHashNode* odata);

   DLLLOCAL virtual bool scanMembers(RSetHelper& rsh);

   DLLLOCAL virtual bool needsScan() const {
      if (!getScanCount() || status != OS_OK)
         return false;
      AutoLocker al(ref_mutex);
      if (status != OS_OK)
         return false;
      if (getScanCount()) {
         if (call_count) {
            if (!scan_after_call)
               scan_after_call = true;
            return false;
         }
         return true;
      }
      return false;
   }

   DLLLOCAL void mergeDataToHash(QoreHashNode* hash, ExceptionSink* xsink) const;

   DLLLOCAL void setPrivate(qore_classid_t key, AbstractPrivateData* pd) {
      if (!privateData)
         privateData = new KeyList();
      //printd(5, "qore_object_private::setPrivate() this: %p 2:privateData: %p (%s) key: %d pd: %p\n", this, privateData, theclass->getName(), key, pd);
      privateData->insert(key, pd);
      addVirtualPrivateData(key, pd);
   }

   static void breakit() {}

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

   DLLLOCAL AbstractPrivateData* getAndRemovePrivateData(qore_classid_t key, ExceptionSink* xsink) {
      QoreSafeVarRWWriteLocker sl(rml);
      return privateData ? privateData->getAndRemovePtr(key) : 0;
   }

   DLLLOCAL AbstractPrivateData* getReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const;

   DLLLOCAL AbstractPrivateData* tryGetReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const;

   DLLLOCAL QoreValue evalBuiltinMethodWithPrivateData(const QoreMethod& method, const BuiltinNormalMethodVariantBase* meth, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink);

   // no locking necessary; if class_ctx is non-null, an internal member is being initialized
   AbstractQoreNode** getMemberValuePtrForInitialization(const char* member, const qore_class_private* class_ctx) {
      QoreHashNode* odata = class_ctx ? getCreateInternalData(class_ctx) : data;
      //printd(5, "qore_object_private::getMemberValuePtrForInitialization() this: %p mem: '%s' class_ctx: %p %s odata: %p\n", this, member, class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a", odata);
      return odata->getKeyValuePtr(member);
   }

   //! retuns member data of the object (or 0 if there's an exception), private members are excluded if called outside the class, caller owns the QoreHashNode reference returned
   /**
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return member data of the object
   */
   DLLLOCAL QoreHashNode* getRuntimeMemberHash(ExceptionSink* xsink) const;

   DLLLOCAL void incScanCount(int dt) {
      assert(dt);
      assert(obj_count || dt > 0);
      //printd(5, "qore_object_private::incScanCount() this: %p dt: %d: %d -> %d\n", this, dt, obj_count, obj_count + dt);
      obj_count += dt;
   }

   DLLLOCAL unsigned getScanCount() const {
      return obj_count;
   }

   DLLLOCAL VRMutex* getGate() const {
      return &gate;
   }

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

   DLLLOCAL void customDeref(bool do_scan, ExceptionSink* xsink);

   DLLLOCAL int startCall(const char* mname, ExceptionSink* xsink);

   DLLLOCAL void endCall(ExceptionSink* xsink);

   DLLLOCAL const char* getClassName() const {
      return theclass->getName();
   }

   DLLLOCAL static QoreValue evalBuiltinMethodWithPrivateData(QoreObject& obj, const QoreMethod& method, const BuiltinNormalMethodVariantBase* meth, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) {
      return obj.priv->evalBuiltinMethodWithPrivateData(method, meth, args, rtflags, xsink);
   }

   DLLLOCAL static qore_object_private* get(QoreObject& obj) {
      return obj.priv;
   }

   DLLLOCAL static const qore_object_private* get(const QoreObject& obj) {
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

   DLLLOCAL static int getLValue(const QoreObject& obj, const char* key, LValueHelper& lvh, const qore_class_private* class_ctx, bool for_remove, ExceptionSink* xsink) {
      return obj.priv->getLValue(key, lvh, class_ctx, for_remove, xsink);
   }

   DLLLOCAL static AbstractQoreNode** getMemberValuePtr(const QoreObject* obj, const char* key, AutoVLock *vl, const QoreTypeInfo*& typeInfo, ExceptionSink* xsink) {
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
