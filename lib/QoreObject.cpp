/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Object.cpp

  thread-safe object definition

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#include <qore/Qore.h>
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/QoreObjectIntern.h"
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/QoreClosureNode.h"

qore_object_private::qore_object_private(QoreObject* n_obj, const QoreClass* oc, QoreProgram* p, QoreHashNode* n_data) :
   RObject(n_obj->references, true),
   theclass(oc), data(n_data), pgm(p), system_object(!p),
   delete_blocker_run(false), in_destructor(false),
   recursive_ref_found(false),
   obj(n_obj) {
   //printd(5, "qore_object_private::qore_object_private() this: %p obj: %p '%s'\n", this, obj, oc->getName());
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::qore_object_private() this: %p obj: %p, pgm: %p, class: %s, references 0->1\n", this, obj, p, oc->getName());
#endif
   /* instead of referencing the class, we reference the program, because the
      program contains the namespace that contains the class, and the class's
      methods may call functions in the program as well that could otherwise
      disappear when the program is deleted
   */
   if (p) {
      printd(5, "qore_object_private::qore_object_private() obj: %p (%s) calling QoreProgram::ref() (%p)\n", obj, theclass->getName(), p);
      // make a weak reference to the Program - a strong reference (QoreProgram::ref()) could cause a recursive reference
      p->depRef();
   }
#ifdef DEBUG
   n_data->priv->is_obj = true;
#endif
   qore_class_private::get(*oc)->ref();
}

qore_object_private::~qore_object_private() {
   //printd(5, "qore_object_private::~qore_object_private() this: %p obj: %p '%s' pgm: %p\n", this, obj, theclass ? theclass->getName() : "<n/a>", pgm);
   assert(!cdmap);
   assert(!data);
   assert(!privateData);
   assert(!rset);
   qore_class_private::get(*const_cast<QoreClass*>(theclass))->deref();
   // release weak reference
   if (pgm)
      pgm->depDeref();
}

// returns true if a lock error has occurred and the transaction should be aborted or restarted; the rsection lock is held when this function is called
bool qore_object_private::scanMembersIntern(RSetHelper& rsh, QoreHashNode* odata) {
   assert(rml.checkRSectionExclusive());

   // we should never perform a scan while the object has "real references", such scans must be deferred until the last "real reference" has been removed
   if (rrefs) {
      bool invalidate = false;
      {
         AutoLocker al(rlck);
         if (rrefs) {
            invalidate = true;
            if (!deferred_scan)
               deferred_scan = true;
         }
      }
      if (invalidate) {
         removeInvalidateRSetIntern();
         return false;
      }
   }

   HashIterator hi(odata);
   while (hi.next()) {
#ifdef DEBUG
      if (get_node_type(hi.getValue()) == NT_OBJECT || get_node_type(hi.getValue()) == NT_RUNTIME_CLOSURE)
         printd(QRO_LVL, "RSetHelper::checkIntern() search %p '%s' key '%s' %p (%s)\n", obj, theclass->getName(), hi.getKey(), hi.getValue(), get_type_name(hi.getValue()));
#endif
      if (scanCheck(rsh, hi.getValue()))
          return true;
      printd(QRO_LVL, "RSetHelper::checkIntern() result %p '%s' key '%s' %p (%s)\n", obj, theclass->getName(), hi.getKey(), hi.getValue(), get_type_name(hi.getValue()));
   }

   return false;
}

// returns true if a lock error has occurred and the transaction should be aborted or restarted; the rsection lock is held when this function is called
bool qore_object_private::scanMembers(RSetHelper& rsh) {
   if (scanMembersIntern(rsh, data))
      return true;
   // scan internal members
   if (cdmap) {
      for (cdmap_t::iterator i = cdmap->begin(), e = cdmap->end(); i != e; ++i) {
         if (scanMembersIntern(rsh, i->second))
            return true;
      }
   }

   return false;
}

QoreHashNode* qore_object_private::copyData(ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, theclass->getName());
      return 0;
   }

   return data->copy();
}

void qore_object_private::merge(qore_object_private& o, AutoVLock& vl, ExceptionSink* xsink) {
   // saves source data to merge
   ReferenceHolder<QoreHashNode> new_data(xsink);

   // get the current class context for possible internal data
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = 0;

   // saves internal source data to merge
   ReferenceHolder<QoreHashNode> new_internal_data(xsink);

   {
      QoreSafeVarRWReadLocker sl(o.rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, o.theclass->getName());
         return;
      }

      if (!o.data->empty())
         new_data = o.data->copy();

      if (class_ctx && o.cdmap) {
         cdmap_t::iterator i = o.cdmap->find(class_ctx->getHash());
         if (i != o.cdmap->end()) {
            // see if the current object supports this class's data
            ClassAccess access;
            if (theclass->priv->getClass(*class_ctx, access))
               new_internal_data = i->second->copy();
         }
      }
   }

   bool check_recursive = false;

   // list for saving all overwritten values to be dereferenced outside the object lock
   ReferenceHolder<QoreListNode> holder(xsink);

   if (new_data || new_internal_data) {
      QoreAutoVarRWWriteLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return;
      }

      mergeIntern(xsink, *new_data, check_recursive, holder, class_ctx, *new_internal_data);
   }

   if (check_recursive) {
      RSetHelper orsh(*this);
   }
}

void qore_object_private::merge(const QoreHashNode* h, AutoVLock& vl, ExceptionSink* xsink) {
   bool check_recursive = false;

   // list for saving all overwritten values to be dereferenced outside the object lock
   ReferenceHolder<QoreListNode> holder(xsink);

   // get the current class context for possible internal data
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = 0;

   if (!h->empty()) {
      QoreAutoVarRWWriteLocker al(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, theclass->getName());
         return;
      }

      mergeIntern(xsink, h, check_recursive, holder, class_ctx);
   }

   if (check_recursive) {
      RSetHelper orsh(*this);
   }
}

void qore_object_private::mergeIntern(ExceptionSink* xsink, const QoreHashNode* h, bool& check_recursive, ReferenceHolder<QoreListNode>& holder, const qore_class_private* class_ctx, const QoreHashNode* new_internal_data) {
   //printd(5, "qore_object_private::merge() obj: %p\n", obj);

   QoreHashNode* id = 0;

   if (h) {
      ConstHashIterator hi(h);
      while (hi.next()) {
         const QoreTypeInfo* ti;

         // check member status
         bool internal_member;
         if (checkMemberAccessGetTypeInfo(xsink, hi.getKey(), class_ctx, internal_member, ti))
            return;

         // check type compatibility and perform type translations, if any
         QoreValue qv(hi.getReferencedValue());
         QoreTypeInfo::acceptInputMember(ti, hi.getKey(), qv, xsink);
         ReferenceHolder<AbstractQoreNode> val(qv.takeNode(), xsink);
         if (*xsink)
            return;

         AbstractQoreNode* nv = *val;

         QoreHashNode* odata = internal_member ? (id ? id : (id = getCreateInternalData(class_ctx))) : data;

         AbstractQoreNode* n = odata->priv->swapKeyValue(hi.getKey(), val.release(), this);
         if (!check_recursive && (needs_scan(n) || needs_scan(nv)))
            check_recursive = true;

         //printd(5, "QoreObject::merge() n: %p (rc: %d, type: %s)\n", n, n ? n->isReferenceCounted() : 0, get_type_name(n));
         // if we are overwriting a value, then save it in the list for dereferencing after the lock is released
         if (n && n->isReferenceCounted()) {
            if (!holder)
               holder = new QoreListNode;
            holder->push(n);
         }
      }
   }

   // merge internal data if relevant & possible
   if (new_internal_data) {
      assert(class_ctx);
      assert(!new_internal_data->empty());

      if (!id)
         id = getCreateInternalData(class_ctx);

      ConstHashIterator hi(new_internal_data);
      while (hi.next()) {
         AbstractQoreNode* nv = hi.getReferencedValue();
         AbstractQoreNode* n = id->priv->swapKeyValue(hi.getKey(), nv, this);
         if (!check_recursive && (needs_scan(n) || needs_scan(nv)))
            check_recursive = true;

         //printd(5, "QoreObject::merge() n: %p (rc: %d, type: %s)\n", n, n ? n->isReferenceCounted() : 0, get_type_name(n));
         // if we are overwriting a value, then save it in the list for dereferencing after the lock is released
         if (n && n->isReferenceCounted()) {
            if (!holder)
               holder = new QoreListNode;
            holder->push(n);
         }
      }
   }
}

QoreHashNode* qore_object_private::getRuntimeMemberHash(ExceptionSink* xsink) const {
   // get the current class context for possible internal data
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = 0;

   QoreSafeVarRWReadLocker sl(rml);

   if (status == OS_DELETED)
      return 0;

   // return all member data if called inside the class
   if (class_ctx) {
      QoreHashNode* h = data->copy();
      const QoreHashNode* odata = getInternalData(class_ctx);
      if (odata)
         h->merge(odata, xsink);
      return h;
   }

   QoreHashNode* h = new QoreHashNode;

   ConstHashIterator hi(data);
   while (hi.next()) {
      if (theclass->isPrivateMember(hi.getKey()))
         continue;

      // not possible for an exception to happen here
      h->setKeyValue(hi.getKey(), hi.getReferencedValue(), xsink);
   }

   return h;
}

AbstractQoreNode* qore_object_private::takeMember(ExceptionSink* xsink, const char* key, bool check_access) {
   const QoreTypeInfo* mti = nullptr;

   // get the current class context for possible internal data
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = nullptr;
   bool internal_member;
   if (checkMemberAccessGetTypeInfo(xsink, key, class_ctx, internal_member, mti))
      return nullptr;

   QoreAutoVarRWWriteLocker al(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, key, theclass->getName());
      return nullptr;
   }

   QoreHashNode* odata = internal_member ? getCreateInternalData(class_ctx) : data;

#ifdef QORE_ENFORCE_DEFAULT_LVALUE
   return odata->priv->swapKeyValue(key, QoreTypeInfo::getDefaultQoreValue(mti).takeNode(), this);
#else
   return odata->priv->swapKeyValue(key, nullptr, this);
#endif
}

AbstractQoreNode* qore_object_private::takeMember(LValueHelper& lvh, const char* key) {
   // get the current class context for possible internal data
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = nullptr;
   const QoreTypeInfo* mti = nullptr;

   bool internal_member;
   if (checkMemberAccessGetTypeInfo(lvh.vl.xsink, key, class_ctx, internal_member, mti))
      return nullptr;

   QoreAutoVarRWWriteLocker al(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(lvh.vl.xsink, key, theclass->getName());
      return nullptr;
   }

   QoreHashNode* odata = internal_member ? getCreateInternalData(class_ctx) : data;

   AbstractQoreNode* rv;
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
   rv = odata->priv->swapKeyValue(key, QoreTypeInfo::getDefaultValue(mti), this);
#else
   rv = odata->priv->swapKeyValue(key, nullptr, this);
#endif

   if (needs_scan(rv)) {
      if (!getScanCount())
         lvh.setDelta(-1);
   }

   return rv;
}

void qore_object_private::takeMembers(QoreLValueGeneric& rv, LValueHelper& lvh, const QoreListNode* l) {
   // get the current class context for possible internal data
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = nullptr;

   QoreHashNode* rvh = new QoreHashNode;
   // in case the lvalue cannot hold a hash, then dereference after the lock is released
   ReferenceHolder<> holder(rv.assignInitial(rvh), lvh.vl.xsink);

   QoreAutoVarRWWriteLocker al(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(lvh.vl.xsink, theclass->getName());
      return;
   }

   unsigned old_count = getScanCount();

   QoreHashNode* id = nullptr;

   ConstListIterator li(l);
   while (li.next()) {
      QoreStringValueHelper mem(li.getValue(), QCS_DEFAULT, lvh.vl.xsink);
      if (*lvh.vl.xsink)
         return;
      const char* key = mem->getBuffer();

      const QoreTypeInfo* mti = nullptr;
      bool internal_member;
      if (checkMemberAccessGetTypeInfo(lvh.vl.xsink, key, class_ctx, internal_member, mti))
         return;

      QoreHashNode* odata = internal_member ? (id ? id : (id = getCreateInternalData(class_ctx))) : data;

#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      AbstractQoreNode* n = odata->priv->swapKeyValue(key, QoreTypeInfo::getDefaultValue(mti), this);
#else
      AbstractQoreNode* n = odata->priv->swapKeyValue(key, 0, this);
#endif

      // note that no exception can occur here
      rvh->setKeyValue(key, n, lvh.vl.xsink);
      assert(!*lvh.vl.xsink);
   }

   if (old_count && !getScanCount())
      lvh.setDelta(-1);
}

void qore_object_private::mergeDataToHash(QoreHashNode* hash, ExceptionSink* xsink) const {
   // get the current class context for possible internal data
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = nullptr;

   QoreSafeVarRWReadLocker sl(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, theclass->getName());
      return;
   }

   if (class_ctx) {
      hash->merge(data, xsink);
      const QoreHashNode* odata = getInternalData(class_ctx);
      if (odata)
         hash->merge(odata, xsink);
      return;
   }

   ConstHashIterator hi(data);
   while (hi.next()) {
      if (theclass->isPrivateMember(hi.getKey()))
         continue;

      // not possible for an exception to happen here
      hash->setKeyValue(hi.getKey(), hi.getReferencedValue(), xsink);
   }
}

int qore_object_private::getLValue(const char* key, LValueHelper& lvh, const qore_class_private* class_ctx, bool for_remove, ExceptionSink* xsink) {
   const QoreTypeInfo* mti = nullptr;
   bool internal_member;
   if (checkMemberAccessGetTypeInfo(xsink, key, class_ctx, internal_member, mti))
      return -1;

   // do lock handoff
   qore_object_lock_handoff_helper qolhh(const_cast<qore_object_private*>(this), lvh.vl);

   if (status == OS_DELETED) {
      xsink->raiseException("OBJECT-ALREADY-DELETED", "write attempted to member \"%s\" in an already-deleted object", key);
      return -1;
   }

   qolhh.stayLocked();

   QoreHashNode* odata = internal_member ? getCreateInternalData(class_ctx) : data;

   //printd(5, "qore_object_private::getLValue() this: %p %s::%s type %s for_remove: %d int: %d odata: %p\n", this, theclass->getName(), key, QoreTypeInfo::getName(mti), for_remove, internal_member, odata);

   HashMember* m;
   if (for_remove) {
      m = odata->priv->findMember(key);
      if (!m)
         return -1;
   }
   else
      m = odata->priv->findCreateMember(key);

   lvh.setPtr(m->node, mti);

   return 0;
}

AbstractQoreNode* qore_object_private::getReferencedMemberNoMethod(const char* mem, ExceptionSink* xsink) const {
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx))
      class_ctx = 0;

   bool internal_member = class_ctx ? class_ctx->runtimeIsMemberInternal(mem) : false;

   QoreSafeVarRWReadLocker sl(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, theclass->getName());
      return 0;
   }

   const QoreHashNode* odata = internal_member ? getInternalData(class_ctx) : data;

   AbstractQoreNode* rv = odata ? odata->getReferencedKeyValue(mem) : 0;
   //printd(5, "qore_object_private::getReferencedMemberNoMethod() this: %p mem: %p (%s) xsink: %p internal: %d data->size(): %d rv: %p %s\n", this, mem, mem, xsink, internal_member, odata ? odata->size() : -1, rv, get_type_name(rv));
   return rv;
}

void qore_object_private::setValue(const char* key, AbstractQoreNode* val, ExceptionSink* xsink) {
   // get the current class context
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && (!qore_class_private::runtimeCheckPrivateClassAccess(*theclass, class_ctx) || !class_ctx->runtimeIsMemberInternal(key)))
      class_ctx = 0;

   setValueIntern(class_ctx, key, val, xsink);
}

// here if class_ctx is set it means that the member is an internal member and also that class_ctx is the current runtime class context
void qore_object_private::setValueIntern(const qore_class_private* class_ctx, const char* key, AbstractQoreNode* val, ExceptionSink* xsink) {
   AbstractQoreNode* old_value;

   // initial count (true = possible recursive cycle, false = no cycle possible)
   bool before;
   bool after = needs_scan(val);

   {
      QoreSafeVarRWWriteLocker sl(rml);

      if (status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, key, theclass->getName());
         return;
      }

      QoreHashNode* odata = class_ctx ? getCreateInternalData(class_ctx) : data;

      //printd(5, "qore_object_private::setValueIntern() obj: %p '%s' class_ctx: %p '%s' odata: %p\n", obj, key, class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a", odata);

      old_value = odata->takeKeyValue(key);

      before = needs_scan(old_value);

      qore_hash_private::get(*odata)->setKeyValue(key, val, this, xsink);

      // calculate and apply delta
      int dt = before ? (after ? 0 : -1) : (after ? 1 : 0);
      if (dt)
         incScanCount(dt);

      // only set before if there was an object requiring a scan and the current object might have had a recursive reference
      if (before && !mightHaveRecursiveReferences())
         before = false;
   }

   if (old_value) {
      old_value->deref(xsink);
   }

   // scan object if necessary
   if (before || after)
      RSetHelper rsh(*this);
}

// helper function for QoreObject::evalBuiltinMethodWithPrivateData() variations
static void check_meth_eval(const QoreClass* cls, const char* mname, const QoreClass* mclass, ExceptionSink* xsink) {
   if (!xsink->isException()) {
      if (cls == mclass)
         xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", cls->getName(), mname);
      else {
         xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of '%s') cannot be executed because the object has already been deleted", mclass->getName(), mname, cls->getName());
      }
   }
}

QoreValue qore_object_private::evalBuiltinMethodWithPrivateData(const QoreMethod& method, const BuiltinNormalMethodVariantBase* meth, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->getClass()->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->evalImpl(obj, *pd, args, rtflags, xsink);

   //printd(5, "qore_object_private::evalBuiltingMethodWithPrivateData() this: %p obj: %p (%s) pd: %p, call: %s::%s(), class ID: %d, method class ID: %d\n", this, obj, theclass->getName(), *pd, method.getClass()->getName(), method.getName(), method.getClass()->getID(), method.getClass()->getIDForMethod());
   check_meth_eval(theclass, method.getName(), method.getClass(), xsink);
   return QoreValue();
}

AbstractPrivateData* qore_object_private::getReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(rml);

   if (status == OS_DELETED || !privateData) {
      makeAccessDeletedObjectException(xsink, theclass->getName());
      return 0;
   }

   AbstractPrivateData* d = privateData->getReferencedPrivateData(key);
   if (!d)
      makeAccessDeletedObjectException(xsink, theclass->getName());

   return d;
}

AbstractPrivateData* qore_object_private::tryGetReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, theclass->getName());
      return 0;
   }

   if (!privateData)
      return 0;

   return privateData->getReferencedPrivateData(key);
}

void qore_object_private::setRealReference() {
   AutoLocker al(rlck);
   printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::setRealReference() this: %p '%s': references %d rrefs %d->%d\n", this, status == OS_OK ? getClassName() : "<deleted>", references.load(), rrefs, rrefs + 1);
   ++rrefs;
}

void qore_object_private::unsetRealReference() {
   AutoLocker al(rlck);
   printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::unsetRealReference() this: %p '%s': references %d rrefs %d->%d\n", this, status == OS_OK ? getClassName() : "<deleted>", references.load(), rrefs, rrefs - 1);
   derefRealIntern();
}

void qore_object_private::customDeref(bool real, ExceptionSink* xsink) {

   {
      //printd(5, "qore_object_private::customDeref() this: %p '%s' references: %d->%d (trefs: %d) status: %d has_delete_blocker: %d delete_blocker_run: %d\n", this, getClassName(), references, references - 1, tRefs.reference_count(), status, theclass->has_delete_blocker(), delete_blocker_run);

      printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::customDeref() this: %p '%s': references %d->%d rrefs %d->%d\n", this, status == OS_OK ? getClassName() : "<deleted>", references.load(), references.load() - 1, rrefs, rrefs - (real ? 1 : 0));

      robject_dereference_helper qodh(this, real);
      int ref_copy = qodh.getRefs();

      // in case this is the last reference (even in recursive cases), ref_copy will remain equal to references throughout this code
      // in other cases, the references value could change in another thread

      bool rrf = false;
      if (ref_copy) {
         while (true) {
            bool recalc = false;
            {
               QoreSafeRSectionReadLocker sl(rml);

               if (in_destructor || status != OS_OK || recursive_ref_found) {
                  return;
               }

               // rset can be changed unless the rsection is acquired
               sl.acquireRSection();

               printd(QRO_LVL, "qore_object_private::customDeref() this: %p '%s' rset: %p (valid: %d) rcount: %d refs: %d/%d rrefs: %d (deferred: %d do_scan: %d)\n", this, getClassName(), rset, RSet::isValid(rset), rcount, ref_copy, references.load(), rrefs, deferred_scan, qodh.doScan());

               int rc;
               RSet* rs = rset;

               if (!rs) {
                  if (rcount == ref_copy) {
                     // this must be true if we really are dealing with an object with no more valid (non-recursive) references
                     assert(references.load() == ref_copy);
                     rc = 1;
                  }
                  else {
                     if (qodh.deferredScan()) {
                        printd(QRO_LVL, "qore_object_private::customDeref() this: %p '%s' deferred scan set; rescanning\n", this, getClassName());
                        rc = -1;
                     }
                     else {
                        printd(QRO_LVL, "qore_object_private::customDeref() this: %p '%s' no deferred scan\n", this, getClassName());
                        return;
                     }
                  }
               }
               else
                  rc = rs->canDelete(ref_copy, rcount);

               if (!rc)
                  return;

               if (rc == -1) {
                  printd(QRO_LVL, "qore_object_private::customDeref() this: %p '%s' invalid rset, recalculating\n", this, getClassName());
                  recalc = true;
               }
            }
            if (recalc) {
               if (qodh.doScan()) {
                  // recalculate rset immediately
                  RSetHelper rsh(*this);
                  continue;
               }
               else {
                  return;
               }
            }

	    printd(QRO_LVL, "qore_object_private::customDeref() this: %p rcount/refs: %d/%d collecting object (%s) with only recursive references\n", this, rcount, ref_copy, getClassName());

            qodh.willDelete();
            rrf = true;
            break;
         }
      }

      QoreSafeVarRWWriteLocker sl(rml);

      if (rrf)
         recursive_ref_found = true;

      // if the destructor has already been run, then just run tDeref() which should delete the QoreObject
      if (in_destructor || status != OS_OK) {
         sl.unlock();
         //printd(5, "qore_object_private::customDeref() this: %p obj: %p %s deleting\n", this, obj, getClassName());
         qodh.finalDeref(this);
         return;
      }

      // if the scope deletion is blocked, then do not run the destructor
      if (!delete_blocker_run && theclass->has_delete_blocker()) {
         if (theclass->execDeleteBlocker(obj, xsink)) {
            //printd(5, "qore_object_private::customDeref() this: %p class: %s blocking delete\n", this, getClassName());
            delete_blocker_run = true;
            //printd(5, "Object lock %p unlocked (safe)\n", &rml);
            return;
         }
      }

      in_destructor = true;

      //printd(5, "qore_object_private::customDeref() class: %s this: %p going out of scope\n", getClassName(), this);

      // mark status as in destructor
      status = gettid();

      //printd(5, "Object lock %p unlocked (safe)\n", &rml);
   }

   doDeleteIntern(xsink);
}

int qore_object_private::startCall(const char* mname, ExceptionSink* xsink) {
   AutoLocker al(rlck);
   if (status == OS_DELETED) {
      xsink->raiseException("OBJECT-ALREADY-DELETED", "cannot call method '%s()' on an object that has already been deleted", mname);
      return -1;
   }

   customRefIntern(true);
   return 0;
}

void qore_object_private::endCall(ExceptionSink* xsink) {
   //printd(5, "qore_object_private::endCall() this: %p obj: %p '%s' calling customDeref()\n", this, obj, theclass->getName());
   customDeref(true, xsink);
}

void QoreObject::externalDelete(qore_classid_t key, ExceptionSink* xsink) {
   {
      QoreAutoVarRWWriteLocker al(priv->rml);

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

QoreObject::QoreObject(const QoreClass* oc, QoreProgram* p) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, new QoreHashNode)) {
}

QoreObject::QoreObject(const QoreClass* oc, QoreProgram* p, AbstractPrivateData* data) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, new QoreHashNode)) {
   assert(data);
   priv->setPrivate(oc->getID(), data);
}

QoreObject::QoreObject(const QoreClass* oc, QoreProgram* p, QoreHashNode* h) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, h)) {
}

QoreObject::~QoreObject() {
   //QORE_TRACE("QoreObject::~QoreObject()");
   //printd(5, "QoreObject::~QoreObject() this: %p, pgm: %p, class: %s\n", this, priv->pgm, priv->theclass->getName());
   delete priv;
}

const QoreClass* QoreObject::getClass() const {
   return priv->theclass;
}

const char*QoreObject::getClassName() const {
   return priv->theclass->getName();
}

int QoreObject::getStatus() const {
   return priv->status;
}

bool QoreObject::isValid() const {
   return priv->status != OS_DELETED;
}

QoreProgram* QoreObject::getProgram() const {
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

void QoreObject::evalCopyMethodWithPrivateData(const QoreClass &thisclass, const BuiltinCopyVariantBase* meth, QoreObject* self, ExceptionSink* xsink) {
   // get referenced object
   AbstractPrivateData* pd = getReferencedPrivateData(thisclass.getID(), xsink);

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

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this: %p, method class ID: %d\n", this, classid_for_method);
   return false;
}

bool QoreObject::validInstanceOf(qore_classid_t cid) const {
   if (priv->status == OS_DELETED)
      return 0;

   return priv->theclass->getClass(cid);
}

bool QoreObject::validInstanceOf(const QoreClass& qc) const {
   if (priv->status == OS_DELETED)
      return 0;

   bool p = false;
   return priv->theclass->getClass(qc, p);
}

QoreValue QoreObject::evalMethodValue(const QoreString* name, const QoreListNode* args, ExceptionSink* xsink) {
   TempEncodingHelper tmp(name, QCS_DEFAULT, xsink);
   if (!tmp)
      return QoreValue();

   return evalMethodValue(tmp->getBuffer(), args, xsink);
}

QoreValue QoreObject::evalMethodValue(const char* name, const QoreListNode* args, ExceptionSink* xsink) {
   return priv->theclass->evalMethod(this, name, args, xsink);
}

AbstractQoreNode* QoreObject::evalMethod(const QoreString* name, const QoreListNode* args, ExceptionSink* xsink) {
   TempEncodingHelper tmp(name, QCS_DEFAULT, xsink);
   if (!tmp)
      return 0;

   return evalMethod(tmp->getBuffer(), args, xsink);
}

AbstractQoreNode* QoreObject::evalMethod(const char* name, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(priv->theclass->evalMethod(this, name, args, xsink), xsink);
   return *xsink ? 0 : rv.getReferencedValue();
}

int64 QoreObject::bigIntEvalMethod(const char* name, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(priv->theclass->evalMethod(this, name, args, xsink), xsink);
   return *xsink ? 0 : rv->getAsBigInt();
}

int QoreObject::intEvalMethod(const char* name, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(priv->theclass->evalMethod(this, name, args, xsink), xsink);
   return *xsink ? 0 : rv->getAsBigInt();
}

bool QoreObject::boolEvalMethod(const char* name, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(priv->theclass->evalMethod(this, name, args, xsink), xsink);
   return *xsink ? false : rv->getAsBool();
}

double QoreObject::floatEvalMethod(const char* name, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(priv->theclass->evalMethod(this, name, args, xsink), xsink);
   return *xsink ? 0.0 : rv->getAsFloat();
}

QoreValue QoreObject::evalMethodValue(const QoreMethod& method, const QoreListNode* args, ExceptionSink* xsink) {
   return qore_method_private::eval(method, xsink, this, args);
}

AbstractQoreNode* QoreObject::evalMethod(const QoreMethod& method, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(qore_method_private::eval(method, xsink, this, args), xsink);
   return *xsink ? 0 : rv.getReferencedValue();
}

int64 QoreObject::bigIntEvalMethod(const QoreMethod& method, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(qore_method_private::eval(method, xsink, this, args), xsink);
   return *xsink ? 0 : rv->getAsBigInt();
}

int QoreObject::intEvalMethod(const QoreMethod& method, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(qore_method_private::eval(method, xsink, this, args), xsink);
   return *xsink ? 0 : rv->getAsBigInt();
}

bool QoreObject::boolEvalMethod(const QoreMethod& method, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(qore_method_private::eval(method, xsink, this, args), xsink);
   return *xsink ? false : rv->getAsBool();
}

double QoreObject::floatEvalMethod(const QoreMethod& method, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(qore_method_private::eval(method, xsink, this, args), xsink);
   return *xsink ? 0.0 : rv->getAsFloat();
}

AbstractQoreNode* QoreObject::evalMethodVariant(const QoreMethod& method, const QoreExternalMethodVariant* variant, const QoreListNode* args, ExceptionSink* xsink) {
   ValueHolder rv(qore_method_private::evalNormalVariant(method, xsink, this, variant, args), xsink);
   return *xsink ? 0 : rv.getReferencedValue();
}

const QoreClass* QoreObject::getClass(qore_classid_t cid) const {
   if (cid == priv->theclass->getID())
      return priv->theclass;
   return priv->theclass->getClass(cid);
}

const QoreClass* QoreObject::getClass(qore_classid_t cid, bool& cpriv) const {
   return priv->theclass->getClass(cid, cpriv);
}

QoreValue QoreObject::evalMember(const QoreString* member, ExceptionSink* xsink) {
   // make sure to convert string encoding if necessary to default character set
   TempEncodingHelper tstr(member, QCS_DEFAULT, xsink);
   if (!tstr)
      return QoreValue();

   const char* mem = tstr->getBuffer();

   //printd(5, "QoreObject::evalMember() find_key(%s): %p theclass: %s\n", mem, find_key(mem), theclass ? theclass->getName() : "NONE");

   // get the current class context
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*priv->theclass, class_ctx))
      class_ctx = 0;
   bool internal_member;
   int rc = priv->checkMemberAccess(mem, class_ctx, internal_member);
   if (rc) {
      // run memberGate if it exists
      if (priv->theclass->hasMemberGate())
         return priv->theclass->evalMemberGate(this, *tstr, xsink);

      if (rc == QOA_PRIV_ERROR)
         priv->doPrivateException(mem, xsink);
      else
         priv->doPublicException(mem, xsink);
      return QoreValue();
   }

   AbstractQoreNode* rv;
   bool exists;
   {
      QoreAutoVarRWReadLocker al(priv->rml);

      if (priv->status == OS_DELETED)
         return QoreValue();

      const QoreHashNode* odata = internal_member ? priv->getInternalData(class_ctx) : priv->data;
      if (!odata) {
         rv = 0;
         exists = false;
      }
      else
         rv = odata->getReferencedKeyValue(mem, exists);
   }

   // execute memberGate method for objects where no member exists
   if (!exists && priv->theclass->hasMemberGate()) {
      assert(!rv);
      return priv->theclass->evalMemberGate(this, *tstr, xsink);
   }

   return rv;
}

// 0 = equal, 1 = not equal
bool QoreObject::compareSoft(const QoreObject* obj, ExceptionSink* xsink) const {
   // currently objects are only equal if they are the same object
   return !(this == obj);
}

// 0 = equal, 1 = not equal
bool QoreObject::compareHard(const QoreObject* obj, ExceptionSink* xsink) const {
   // currently objects are only equal if they are the same object
   return !(this == obj);
}

void QoreObject::doDelete(ExceptionSink* xsink) {
   {
      QoreAutoVarRWWriteLocker al(priv->rml);

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

void qore_object_private::customRefIntern(bool real) {
   if (!references.load())
      tRef();
   printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::customRefIntern() this: %p obj: %p '%s' references %d->%d rrefs: %d->%d\n", this, obj, getClassName(), references.load(), references.load() + 1, rrefs, rrefs + (real ? 1 : 0));
   ++references;
   if (real)
      ++rrefs;
}

void QoreObject::customRef() const {
   AutoLocker al(priv->rlck);
   priv->customRefIntern(false);
}

void QoreObject::deleteBlockerRef() const {
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::deleteBlockerRef() this: %p '%s' references %d->%d\n", this, getClassName(), references.load(), references.load() + 1);
#endif
   AutoLocker al(priv->rlck);
   ++references;
}

bool QoreObject::derefImpl(ExceptionSink* xsink) {
   // should never be called
   assert(false);
   return false;
}

void QoreObject::realRef() {
   AutoLocker al(priv->rlck);
   priv->customRefIntern(true);
}

void QoreObject::realDeref(ExceptionSink* xsink) {
   priv->customDeref(true, xsink);
}

// manages the custom dereference and executes the destructor if necessary
void QoreObject::customDeref(ExceptionSink* xsink) {
   priv->customDeref(false, xsink);
}

// this method is called when there is an exception in a constructor and the object should be deleted
void QoreObject::obliterate(ExceptionSink* xsink) {
   priv->obliterate(xsink);
}

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode* QoreObject::getMemberValueNoMethod(const QoreString* key, AutoVLock *vl, ExceptionSink* xsink) const {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValueNoMethod(enc->getBuffer(), vl, xsink);
}

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode* QoreObject::getMemberValueNoMethod(const char* key, AutoVLock *vl, ExceptionSink* xsink) const {
   // do lock handoff
   qore_object_lock_handoff_helper qolhm(const_cast<qore_object_private*>(priv), *vl);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, key, priv->theclass->getName());
      return 0;
   }

   AbstractQoreNode* rv = priv->data->getKeyValue(key);
   if (rv && rv->isReferenceCounted()) {
      qolhm.stayLocked();
   }
   return rv;
}

void QoreObject::deleteMemberValue(const QoreString* key, ExceptionSink* xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   deleteMemberValue(enc->getBuffer(), xsink);
}

void QoreObject::deleteMemberValue(const char* key, ExceptionSink* xsink) {
   // get the current class context
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*priv->theclass, class_ctx))
      class_ctx = 0;
   bool internal_member;

   // check for external access to private members
   if (priv->checkMemberAccess(key, class_ctx, internal_member, xsink))
      return;

   AbstractQoreNode* v;
   {
      QoreSafeVarRWWriteLocker sl(priv->rml);

      if (priv->status == OS_DELETED) {
         makeAccessDeletedObjectException(xsink, key, priv->theclass->getName());
         return;
      }

      QoreHashNode* odata = internal_member ? priv->getInternalData(class_ctx) : priv->data;
      v = odata ? odata->takeKeyValue(key) : 0;
   }

   if (!v)
      return;

   if (v->getType() == NT_OBJECT)
      reinterpret_cast<QoreObject*>(v)->doDelete(xsink);
   v->deref(xsink);
}

AbstractQoreNode* QoreObject::takeMember(const QoreString* key, ExceptionSink* xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return priv->takeMember(xsink, enc->getBuffer());
}

AbstractQoreNode* QoreObject::takeMember(const char* key, ExceptionSink* xsink) {
   return priv->takeMember(xsink, key);
}

void QoreObject::removeMember(const QoreString* key, ExceptionSink* xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   removeMember(enc->getBuffer(), xsink);
}

void QoreObject::removeMember(const char* key, ExceptionSink* xsink) {
   discard(takeMember(key, xsink), xsink);
}

QoreListNode* QoreObject::getMemberList(ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return 0;
   }

   return priv->data->getKeys();
}

QoreHashNode* QoreObject::getSlice(const QoreListNode* value_list, ExceptionSink* xsink) const {
   return priv->getSlice(value_list, xsink);
}

void QoreObject::setValue(const char* key, AbstractQoreNode* val, ExceptionSink* xsink) {
   priv->setValue(key, val, xsink);
}

int QoreObject::size(ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED)
      return 0;

   return priv->data->size();
}

int64 QoreObject::getMemberAsBigInt(const char* mem, bool& found, ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->theclass->getName());
      return 0;
   }

   return priv->data->getKeyAsBigInt(mem, found);
}

AbstractQoreNode* QoreObject::getReferencedMemberNoMethod(const char* mem, ExceptionSink* xsink) const {
   return priv->getReferencedMemberNoMethod(mem, xsink);
}

QoreHashNode* QoreObject::copyData(ExceptionSink* xsink) const {
   return priv->copyData(xsink);
}

// unlocking the lock is managed with the AutoVLock object
// we check if the object is already locked
AbstractQoreNode** QoreObject::getExistingValuePtr(const QoreString* mem, AutoVLock *vl, ExceptionSink* xsink) const {
   TempEncodingHelper enc(mem, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getExistingValuePtr(enc->getBuffer(), vl, xsink);
}

// unlocking the lock is managed with the AutoVLock object
// we check if the object is already locked
// only called for deletes - typeinfo not needed
AbstractQoreNode** QoreObject::getExistingValuePtr(const char* mem, AutoVLock *vl, ExceptionSink* xsink) const {
   // get the current class context
   const qore_class_private* class_ctx = runtime_get_class();
   if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*priv->theclass, class_ctx))
      class_ctx = 0;
   bool internal_member;

   // check for illegal access
   if (priv->checkMemberAccess(mem, class_ctx, internal_member, xsink))
      return 0;

   // do lock handoff
   qore_object_lock_handoff_helper qolhm(const_cast<qore_object_private*>(priv), *vl);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->theclass->getName());
      return 0;
   }

   QoreHashNode* odata = internal_member ? const_cast<QoreHashNode*>(priv->getInternalData(class_ctx)) : priv->data;

   AbstractQoreNode** rv = odata ? odata->getExistingValuePtr(mem) : 0;
   if (rv) {
      qolhm.stayLocked();
   }

   return rv;
}

AbstractPrivateData* QoreObject::getReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const {
   return priv->getReferencedPrivateData(key, xsink);
}

AbstractPrivateData* QoreObject::tryGetReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const {
   return priv->tryGetReferencedPrivateData(key, xsink);
}

AbstractPrivateData* QoreObject::getAndClearPrivateData(qore_classid_t key, ExceptionSink* xsink) {
   QoreSafeVarRWWriteLocker sl(priv->rml);

   if (priv->privateData)
      return priv->privateData->getAndClearPtr(key);

   return 0;
}

// called only during constructor execution, therefore no need for locking
void QoreObject::setPrivate(qore_classid_t key, AbstractPrivateData* pd) {
   priv->setPrivate(key, pd);
}

void QoreObject::addPrivateDataToString(QoreString* str, ExceptionSink* xsink) const {
   str->concat('(');
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_OK && priv->privateData) {
      priv->privateData->addToString(str);
      str->terminate(str->strlen() - 2);
   }
   else
      str->concat("<NO PRIVATE DATA>");

   str->concat(')');
}

void QoreObject::defaultSystemDestructor(qore_classid_t classID, ExceptionSink* xsink) {
   AbstractPrivateData* pd = getAndClearPrivateData(classID, xsink);
   printd(5, "QoreObject::defaultSystemDestructor() this: %p class: %s private_data: %p\n", this, priv->theclass->getName(), pd);
   if (pd)
      pd->deref(xsink);
}

QoreString* QoreObject::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = false;

   TempString rv(new QoreString());
   if (getAsString(*(*rv), foff, xsink))
      return 0;

   del = true;
   return rv.release();
}

int QoreObject::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
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
            const AbstractQoreNode* n = hi.getValue();
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

      // FIXME: encapsulation error; private members are included in the string returned
      /*
      const qore_class_private* class_ctx = runtime_get_class();
      if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*priv->theclass, class_ctx))
         class_ctx = 0;
      */

      ConstHashIterator hi(*h);
      while (hi.next()) {
         // skip private members when accessed outside the class
         //if (!class_ctx && priv->checkMemberAccessIntern(hi.getKey(), false, false) == QOA_PRIV_ERROR)
         //   continue;

         if (foff != FMT_NONE)
            str.addch(' ', foff + 2);

         str.sprintf("%s : ", hi.getKey());

         const AbstractQoreNode* n = hi.getValue();
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

AbstractQoreNode* QoreObject::realCopy() const {
   return refSelf();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than the argument
bool QoreObject::is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const QoreObject* o = dynamic_cast<const QoreObject*>(v);
   if (!o)
      return false;
   return !compareSoft(o, xsink);
}

bool QoreObject::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   const QoreObject* o = dynamic_cast<const QoreObject*>(v);
   if (!o)
      return false;
   return !compareHard(o, xsink);
}

// returns the type name as a c string
const char* QoreObject::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode* QoreObject::evalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

AbstractQoreNode* QoreObject::evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

int64 QoreObject::bigIntEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

int QoreObject::integerEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

bool QoreObject::boolEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return false;
}

double QoreObject::floatEvalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0.0;
}

bool QoreObject::hasMemberNotification() const {
   return priv->theclass->hasMemberNotification();
}

void QoreObject::execMemberNotification(const char* member, ExceptionSink* xsink) {
   priv->theclass->execMemberNotification(this, member, xsink);
}

bool QoreObject::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   if (runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;

   QoreSafeVarRWReadLocker sl(priv->rml);
   return priv->status != OS_DELETED;
}
