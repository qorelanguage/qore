/*
  Object.cpp

  thread-safe object definition

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/QoreObjectIntern.h>
#include <qore/intern/QoreHashNodeIntern.h>

qore_object_private::qore_object_private(QoreObject* n_obj, const QoreClass *oc, QoreProgram* p, QoreHashNode* n_data) : 
   theclass(oc), status(OS_OK), 
   privateData(0), data(n_data), pgm(p), system_object(!p), 
   delete_blocker_run(false), in_destructor(false), pgm_ref(true), 
#ifdef DO_OBJ_RECURSIVE_CHECK
   recursive_ref_found(false),
   rscan(0),
   rcount(0), rwaiting(0), rcycle(0), rset(0),
#endif
   obj(n_obj) {
   //printd(5, "qore_object_private::qore_object_private() this: %p obj: %p '%s'\n", this, obj, oc->getName());
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "qore_object_private::qore_object_private() obj=%p, pgm=%p, class=%s, references 0->1\n", obj, p, oc->getName());
#endif
   /* instead of referencing the class, we reference the program, because the
      program contains the namespace that contains the class, and the class'
      methods may call functions in the program as well that could otherwise
      disappear when the program is deleted
   */
   if (p) {
      printd(5, "qore_object_private::qore_object_private() obj=%p (%s) calling QoreProgram::ref() (%p)\n", obj, theclass->getName(), p);
      //p->depRef();
      p->ref();
   }
#ifdef DEBUG
   n_data->priv->is_obj = true;
#endif
}

void qore_object_private::merge(const QoreHashNode* h, AutoVLock& vl, ExceptionSink* xsink) {
#ifdef DO_OBJ_RECURSIVE_CHECK
   bool check_recursive = false;
#endif

   // list for saving all overwritten values to be dereferenced outside the object lock
   ReferenceHolder<QoreListNode> holder(xsink);

   bool inclass = qore_class_private::runtimeCheckPrivateClassAccess(*theclass);

   {
      QoreAutoVarRWWriteLocker al(rml);

      if (status == OS_DELETED) {
	 makeAccessDeletedObjectException(xsink, theclass->getName());
	 return;
      }

      //printd(5, "qore_object_private::merge() obj=%p\n", obj);

      ConstHashIterator hi(h);
      while (hi.next()) {
	 const QoreTypeInfo *ti;

	 // check member status
	 if (checkMemberAccessGetTypeInfo(xsink, hi.getKey(), ti, !inclass))
	    return;
            
	 // check type compatibility and perform type translations, if any
	 ReferenceHolder<AbstractQoreNode> val(ti->acceptInputMember(hi.getKey(), hi.getReferencedValue(), xsink), xsink);
	 if (*xsink)
	    return;

#ifdef DO_OBJ_RECURSIVE_CHECK
	 AbstractQoreNode* nv = *val;
#endif
	 AbstractQoreNode *n = data->swapKeyValue(hi.getKey(), val.release());
#ifdef DO_OBJ_RECURSIVE_CHECK
	 if (!check_recursive && (is_container(n) || is_container(nv)))
	    check_recursive = true;
#endif

	 //printd(5, "QoreObject::merge() n=%p (rc=%d, type=%s)\n", n, n ? n->isReferenceCounted() : 0, get_type_name(n));
	 // if we are overwriting a value, then save it in the list for dereferencing after the lock is released
	 if (n && n->isReferenceCounted()) {
	    if (!holder)
	       holder = new QoreListNode;
	    holder->push(n);
	 }
      }
   }

#ifdef DO_OBJ_RECURSIVE_CHECK
   if (check_recursive) {
      ObjectRSetHelper orsh(*obj);
   }
#endif
}

#ifdef DO_OBJ_RECURSIVE_CHECK
unsigned qore_object_private::getObjectCount() {
   return data->priv->obj_count;
}

void qore_object_private::incObjectCount(int dt) {
   data->priv->incObjectCount(dt);
}
#endif

AbstractQoreNode *qore_object_private::takeMember(ExceptionSink* xsink, const char* key, bool check_access) {
   const QoreTypeInfo* mti = 0;
   if (checkMemberAccessGetTypeInfo(xsink, key, mti, check_access))
      return 0;

   QoreAutoVarRWWriteLocker al(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, key, theclass->getName());
      return 0;
   }

#ifdef QORE_ENFORCE_DEFAULT_LVALUE
   return data->swapKeyValue(key, mti->getDefaultValue());
#else
   return data->swapKeyValue(key, 0);
#endif
}

AbstractQoreNode* qore_object_private::takeMember(LValueHelper& lvh, const char* key) {
   bool check_access = !qore_class_private::runtimeCheckPrivateClassAccess(*theclass);

   const QoreTypeInfo* mti = 0;
   if (checkMemberAccessGetTypeInfo(lvh.vl.xsink, key, mti, check_access))
      return 0;

   QoreAutoVarRWWriteLocker al(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(lvh.vl.xsink, key, theclass->getName());
      return 0;
   }

   AbstractQoreNode* rv;
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
   rv = data->swapKeyValue(key, mti->getDefaultValue());
#else
   rv = data->swapKeyValue(key, 0);
#endif

#ifdef DO_OBJ_RECURSIVE_CHECK
   if (get_container_obj(rv)) {
      if (!getObjectCount())
	 lvh.setDelta(-1);
   }
#endif

   return rv;
}

void qore_object_private::takeMembers(QoreLValueGeneric& rv, LValueHelper& lvh, const QoreListNode* l) {
   bool check_access = !qore_class_private::runtimeCheckPrivateClassAccess(*theclass);

   QoreHashNode* rvh = new QoreHashNode;
   rv.assignInitial(rvh);

   QoreAutoVarRWWriteLocker al(rml);

   if (status == OS_DELETED) {
      makeAccessDeletedObjectException(lvh.vl.xsink, theclass->getName());
      return;
   }

#ifdef DO_OBJ_RECURSIVE_CHECK
   unsigned old_count = getObjectCount();
#endif
   
   ConstListIterator li(l);
   while (li.next()) {
      QoreStringValueHelper mem(li.getValue(), QCS_DEFAULT, lvh.vl.xsink);
      if (*lvh.vl.xsink)
	 return;
      const char* key = mem->getBuffer();
	 
      const QoreTypeInfo* mti = 0;
      if (checkMemberAccessGetTypeInfo(lvh.vl.xsink, key, mti, check_access))
	 return;
      
#ifdef QORE_ENFORCE_DEFAULT_LVALUE
      AbstractQoreNode* n = data->swapKeyValue(key, mti->getDefaultValue());
#else
      AbstractQoreNode* n = data->swapKeyValue(key, 0);
#endif
	    
      // note that no exception can occur here
      rvh->setKeyValue(key, n, lvh.vl.xsink);
      assert(!*lvh.vl.xsink);
   }

#ifdef DO_OBJ_RECURSIVE_CHECK
   if (old_count && !getObjectCount())
      lvh.setDelta(-1);   
#endif
}

int qore_object_private::getLValue(const char* key, LValueHelper& lvh, bool internal, bool for_remove, ExceptionSink* xsink) const {
   const QoreTypeInfo* mti = 0;
   if (checkMemberAccessGetTypeInfo(xsink, key, mti, !internal))
      return -1;

   // do lock handoff
   AutoVLock& vl = lvh.getAutoVLock();
   qore_object_lock_handoff_helper qolhm(const_cast<qore_object_private *>(this), vl);

   if (status == OS_DELETED) {
      xsink->raiseException("OBJECT-ALREADY-DELETED", "write attempted to member \"%s\" in an already-deleted object", key);
      return -1;
   }

   qolhm.stay_locked();

   //printd(5, "qore_object_private::getLValue() this: %p %s::%s type %s\n", this, theclass->getName(), key, mti->getName());
   // save lvalue type info
   lvh.setTypeInfo(mti);

   HashMember* m;
   if (for_remove) {
      m = data->priv->findMember(key);
      if (!m)
         return -1;
   }
   else
      m = data->priv->findCreateMember(key);
   lvh.setPtr(m->node);
   return 0;
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
   setPrivate(oc->getID(), data);
}

QoreObject::QoreObject(const QoreClass* oc, QoreProgram* p, QoreHashNode* h) : AbstractQoreNode(NT_OBJECT, false, false, false, true), priv(new qore_object_private(this, oc, p, h)) {
}

QoreObject::~QoreObject() {
   //QORE_TRACE("QoreObject::~QoreObject()");
   //printd(5, "QoreObject::~QoreObject() this=%p, pgm=%p, class=%s\n", this, priv->pgm, priv->theclass->getName());

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
   return priv->status == OS_OK; 
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

// helper function for QoreObject::evalBuiltinMethodWithPrivateData() variations
static void check_meth_eval(const QoreClass *cls, const char *mname, const QoreClass *mclass, ExceptionSink* xsink) {
   if (!xsink->isException()) {
      if (cls == mclass)
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() cannot be executed because the object has already been deleted", cls->getName(), mname);
      else
	 xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::%s() (base class of '%s') cannot be executed because the object has already been deleted", mclass->getName(), mname, cls->getName());
   }
}

AbstractQoreNode *QoreObject::evalBuiltinMethodWithPrivateData(const QoreMethod &method, const BuiltinNormalMethodVariantBase *meth, const QoreListNode *args, ExceptionSink* xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->getClass()->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->evalImpl(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p (%s) pd=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, priv->theclass->getName(), *pd, method.getClass()->getName(), method.getName(), method.getClass()->getID(), method.getClass()->getIDForMethod());
   check_meth_eval(priv->theclass, method.getName(), method.getClass(), xsink);
   return 0;
}

int64 QoreObject::bigIntEvalBuiltinMethodWithPrivateData(const QoreMethod &method, const BuiltinNormalMethodVariantBase *meth, const QoreListNode *args, ExceptionSink* xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->getClass()->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->bigIntEvalImpl(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p (%s) pd=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, priv->theclass->getName(), *pd, method.getClass()->getName(), method.getName(), method.getClass()->getID(), method.getClass()->getIDForMethod());
   check_meth_eval(priv->theclass, method.getName(), method.getClass(), xsink);
   return 0;
}

int QoreObject::intEvalBuiltinMethodWithPrivateData(const QoreMethod &method, const BuiltinNormalMethodVariantBase *meth, const QoreListNode *args, ExceptionSink* xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->getClass()->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->intEvalImpl(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p (%s) pd=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, priv->theclass->getName(), *pd, method.getClass()->getName(), method.getName(), method.getClass()->getID(), method.getClass()->getIDForMethod());
   check_meth_eval(priv->theclass, method.getName(), method.getClass(), xsink);
   return 0;
}

bool QoreObject::boolEvalBuiltinMethodWithPrivateData(const QoreMethod &method, const BuiltinNormalMethodVariantBase *meth, const QoreListNode *args, ExceptionSink* xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->getClass()->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->boolEvalImpl(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p (%s) pd=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, priv->theclass->getName(), *pd, method.getClass()->getName(), method.getName(), method.getClass()->getID(), method.getClass()->getIDForMethod());
   check_meth_eval(priv->theclass, method.getName(), method.getClass(), xsink);
   return 0;
}

double QoreObject::floatEvalBuiltinMethodWithPrivateData(const QoreMethod &method, const BuiltinNormalMethodVariantBase *meth, const QoreListNode *args, ExceptionSink* xsink) {
   // get referenced object
   ReferenceHolder<AbstractPrivateData> pd(getReferencedPrivateData(meth->getClass()->getIDForMethod(), xsink), xsink);

   if (pd)
      return meth->floatEvalImpl(this, *pd, args, xsink);

   //printd(5, "QoreObject::evalBuiltingMethodWithPrivateData() this=%p (%s) pd=%p, call=%s::%s(), class ID=%d, method class ID=%d\n", this, priv->theclass->getName(), *pd, method.getClass()->getName(), method.getName(), method.getClass()->getID(), method.getClass()->getIDForMethod());
   check_meth_eval(priv->theclass, method.getName(), method.getClass(), xsink);
   return 0;
}

void QoreObject::evalCopyMethodWithPrivateData(const QoreClass &thisclass, const BuiltinCopyVariantBase *meth, QoreObject *self, ExceptionSink* xsink) {
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

bool QoreObject::validInstanceOf(const QoreClass& qc) const {
   if (priv->status == OS_DELETED)
      return 0;

   bool p = false;
   return priv->theclass->getClass(qc, p);
}

AbstractQoreNode *QoreObject::evalMethod(const QoreString *name, const QoreListNode *args, ExceptionSink* xsink) {
   TempEncodingHelper tmp(name, QCS_DEFAULT, xsink);
   if (!tmp)
      return 0;

   return evalMethod(tmp->getBuffer(), args, xsink);
}

AbstractQoreNode *QoreObject::evalMethod(const char *name, const QoreListNode *args, ExceptionSink* xsink) {
   return priv->theclass->evalMethod(this, name, args, xsink);
}

int64 QoreObject::bigIntEvalMethod(const char *name, const QoreListNode *args, ExceptionSink* xsink) {
   return priv->theclass->bigIntEvalMethod(this, name, args, xsink);
}

int QoreObject::intEvalMethod(const char *name, const QoreListNode *args, ExceptionSink* xsink) {
   return priv->theclass->bigIntEvalMethod(this, name, args, xsink);
}

bool QoreObject::boolEvalMethod(const char *name, const QoreListNode *args, ExceptionSink* xsink) {
   return priv->theclass->boolEvalMethod(this, name, args, xsink);
}

double QoreObject::floatEvalMethod(const char *name, const QoreListNode *args, ExceptionSink* xsink) {
   return priv->theclass->floatEvalMethod(this, name, args, xsink);
}

AbstractQoreNode *QoreObject::evalMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink* xsink) {
   return qore_method_private::eval(method, this, args, xsink);
}

int64 QoreObject::bigIntEvalMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink* xsink) {
   return qore_method_private::bigIntEval(method, this, args, xsink);
}

int QoreObject::intEvalMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink* xsink) {
   return qore_method_private::intEval(method, this, args, xsink);
}

bool QoreObject::boolEvalMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink* xsink) {
   return qore_method_private::boolEval(method, this, args, xsink);
}

double QoreObject::floatEvalMethod(const QoreMethod &method, const QoreListNode *args, ExceptionSink* xsink) {
   return qore_method_private::floatEval(method, this, args, xsink);
}

AbstractQoreNode *QoreObject::evalMethodVariant(const QoreMethod &method, const QoreExternalMethodVariant *variant, const QoreListNode *args, ExceptionSink* xsink) {
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

AbstractQoreNode *QoreObject::evalMember(const QoreString *member, ExceptionSink* xsink) {
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
      QoreAutoVarRWReadLocker al(priv->rml);

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
bool QoreObject::compareSoft(const QoreObject *obj, ExceptionSink* xsink) const {
   // currently objects are only equal if they are the same object
   return !(this == obj);
}

// 0 = equal, 1 = not equal
bool QoreObject::compareHard(const QoreObject *obj, ExceptionSink* xsink) const {
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

void QoreObject::customRefIntern() const {
   if (!references)
      tRef();
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customRefIntern() this: %p '%s' references %d->%d\n", this, getClassName(), references, references + 1);
#endif
   ++references;
}

void QoreObject::customRef() const {
   AutoLocker al(priv->ref_mutex);
   customRefIntern();
}

void QoreObject::deleteBlockerRef() const {
#ifdef QORE_DEBUG_OBJ_REFS
   printd(QORE_DEBUG_OBJ_REFS, "QoreObject::deleteBlockerRef() this: %p '%s' references %d->%d\n", this, getClassName(), references, references + 1);
#endif
   AutoLocker al(priv->ref_mutex);
   ++references;
}

bool QoreObject::derefImpl(ExceptionSink* xsink) {
   // should never be called
   assert(false);
   return false;
}

// manages the custom dereference and executes the destructor if necessary
void QoreObject::customDeref(ExceptionSink* xsink) {
   {
      //printd(5, "QoreObject::customDeref() this: %p '%s' references: %d->%d (trefs: %d) status: %d has_delete_blocker: %d delete_blocker_run: %d\n", this, getClassName(), references, references - 1, priv->tRefs.reference_count(), priv->status, priv->theclass->has_delete_blocker(), priv->delete_blocker_run);

#ifdef QORE_DEBUG_OBJ_REFS
      printd(QORE_DEBUG_OBJ_REFS, "QoreObject::customDeref() this: %p '%s': references %d->%d\n", this, priv->status == OS_OK ? getClassName() : "<deleted>", references, references - 1);
#endif

#ifdef DO_OBJ_RECURSIVE_CHECK
      int ref_copy;
      {
	 AutoLocker slr(priv->ref_mutex);
	 ref_copy = --references;
      }

      bool rrf = false;
      if (ref_copy) {
	 while (true) {
	    bool recalc = false;
	    {
	       QoreSafeVarRWReadLocker sl(priv->rml);

	       if (priv->in_destructor || priv->status != OS_OK || priv->recursive_ref_found) {
		  return;
	       }

	       printd(QRO_LVL, "QoreObject::customDeref() this: %p '%s' rset: %p rcount: %d references: %d\n", this, getClassName(), priv->rset, priv->rcount, references);

	       int rc;
	       ObjectRSet* rs;
	       rs = priv->rset;
	       if (!rs) {
		  if (priv->rcount == references)
		     rc = 1;
		  else
		     return;
	       }
	       else
		  rc = rs->canDelete();

	       if (!rc)
		  return;
	       
	       if (rc == -1) {
		  printd(QRO_LVL, "QoreObject::customDeref() this: %p '%s' invalid rset, recalculating\n", this, getClassName());
		  recalc = true;
	       }
	    }
	    if (recalc) {
	       // recalculate rset
	       ObjectRSetHelper rsh(*this);
	       continue;
	    }

	    // output in any case even when debugging is disabled
	    print_debug(0, "QoreObject::customDeref() this: %p rcount/refs: %d collecting object (%s) with only recursive references\n", this, (int)ref_copy, getClassName());

	    rrf = true;
	    break;
	 }
      }

      QoreSafeVarRWWriteLocker sl(priv->rml);

      if (rrf)
	 priv->recursive_ref_found = true;

      // if the destructor has already been run, then just run tDeref() which should delete the QoreObject
      if (priv->in_destructor || priv->status != OS_OK) {
	 sl.unlock();
	 if (!ref_copy)
	    priv->tDeref();
	 return;
      }
#else
      {
	 AutoLocker slr(priv->ref_mutex);
	 if (--references)
	    return;
      }

      QoreSafeVarRWWriteLocker sl(priv->rml);

      // if the destructor has already been run, then just run tDeref() which should delete the QoreObject
      if (priv->in_destructor || priv->status != OS_OK) {
	 sl.unlock();
	 priv->tDeref();
	 return;
      }
#endif

      // if the scope deletion is blocked, then do not run the destructor
      if (!priv->delete_blocker_run && priv->theclass->has_delete_blocker()) {
	 if (priv->theclass->execDeleteBlocker(this, xsink)) {
	    //printd(5, "QoreObject::customDeref() this: %p class: %s blocking delete\n", this, getClassName());
	    priv->delete_blocker_run = true;
	    //printd(5, "Object lock %p unlocked (safe)\n", &priv->rml);
	    return;
	 }
      }
      
      priv->in_destructor = true;

      //printd(5, "QoreObject::derefImpl() class: %s this: %p going out of scope\n", getClassName(), this);

      // mark status as in destructor
      priv->status = gettid();

      //printd(5, "Object lock %p unlocked (safe)\n", &priv->rml);
   }

   priv->doDeleteIntern(xsink);
}

// this method is called when there is an exception in a constructor and the object should be deleted
void QoreObject::obliterate(ExceptionSink* xsink) {
   priv->obliterate(xsink);
}

/*
// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode **QoreObject::getMemberValuePtr(const QoreString *key, AutoVLock *vl, const QoreTypeInfo *&typeInfo, ExceptionSink* xsink) const {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValuePtr(enc->getBuffer(), vl, typeInfo, xsink);
}
*/

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode *QoreObject::getMemberValueNoMethod(const QoreString *key, AutoVLock *vl, ExceptionSink* xsink) const {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getMemberValueNoMethod(enc->getBuffer(), vl, xsink);
}

// unlocking the lock is managed with the AutoVLock object
AbstractQoreNode *QoreObject::getMemberValueNoMethod(const char *key, AutoVLock *vl, ExceptionSink* xsink) const {
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

void QoreObject::deleteMemberValue(const QoreString *key, ExceptionSink* xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   deleteMemberValue(enc->getBuffer(), xsink);
}

void QoreObject::deleteMemberValue(const char *key, ExceptionSink* xsink) {
   // check for external access to private members
   if (priv->checkMemberAccess(key, xsink))
      return;

   AbstractQoreNode *v;
   {
      QoreSafeVarRWWriteLocker sl(priv->rml);

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

AbstractQoreNode *QoreObject::takeMember(const QoreString *key, ExceptionSink* xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return priv->takeMember(xsink, enc->getBuffer());
}

AbstractQoreNode *QoreObject::takeMember(const char *key, ExceptionSink* xsink) {
   return priv->takeMember(xsink, key);
}

void QoreObject::removeMember(const QoreString *key, ExceptionSink* xsink) {
   TempEncodingHelper enc(key, QCS_DEFAULT, xsink);
   if (!enc)
      return;

   removeMember(enc->getBuffer(), xsink);
}

void QoreObject::removeMember(const char *key, ExceptionSink* xsink) {
   discard(takeMember(key, xsink), xsink);
}

QoreListNode *QoreObject::getMemberList(ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return 0;
   }

   return priv->data->getKeys();
}

QoreHashNode *QoreObject::getSlice(const QoreListNode *value_list, ExceptionSink* xsink) const {
   return priv->getSlice(value_list, xsink);
}

void QoreObject::setValue(const char *key, AbstractQoreNode *val, ExceptionSink* xsink) {
   AbstractQoreNode *old_value;

   {
      QoreSafeVarRWWriteLocker sl(priv->rml);

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

int QoreObject::size(ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED)
      return 0;

   return priv->data->size();
}

int64 QoreObject::getMemberAsBigInt(const char *mem, bool &found, ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->theclass->getName());
      return 0;
   }

   return priv->data->getKeyAsBigInt(mem, found);
}

AbstractQoreNode *QoreObject::getReferencedMemberNoMethod(const char *mem, ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   printd(5, "QoreObject::getReferencedMemberNoMethod(this=%p, mem=%p (%s), xsink=%p, data->size()=%d)\n",
	  this, mem, mem, xsink, priv->data ? priv->data->size() : -1);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, mem, priv->theclass->getName());
      return 0;
   }

   return priv->data->getReferencedKeyValue(mem);
}

QoreHashNode *QoreObject::copyData(ExceptionSink* xsink) const {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return 0;
   }

   return priv->data->copy();
}

QoreHashNode *QoreObject::getRuntimeMemberHash(ExceptionSink* xsink) const {
   bool inclass = qore_class_private::runtimeCheckPrivateClassAccess(*(priv->theclass));

   QoreSafeVarRWReadLocker sl(priv->rml);

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

void QoreObject::mergeDataToHash(QoreHashNode *hash, ExceptionSink* xsink) {
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED) {
      makeAccessDeletedObjectException(xsink, priv->theclass->getName());
      return;
   }

   hash->merge(priv->data, xsink);
}

// unlocking the lock is managed with the AutoVLock object
// we check if the object is already locked
AbstractQoreNode **QoreObject::getExistingValuePtr(const QoreString *mem, AutoVLock *vl, ExceptionSink* xsink) const {
   TempEncodingHelper enc(mem, QCS_DEFAULT, xsink);
   if (!enc)
      return 0;

   return getExistingValuePtr(enc->getBuffer(), vl, xsink);
}

// unlocking the lock is managed with the AutoVLock object
// we check if the object is already locked
// only called for deletes - typeinfo not needed
AbstractQoreNode **QoreObject::getExistingValuePtr(const char *mem, AutoVLock *vl, ExceptionSink* xsink) const {
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

AbstractPrivateData *QoreObject::getReferencedPrivateData(qore_classid_t key, ExceptionSink* xsink) const { 
   QoreSafeVarRWReadLocker sl(priv->rml);

   if (priv->status == OS_DELETED || !priv->privateData) {
      makeAccessDeletedObjectException(xsink, getClassName());
      return 0;
   }

   return priv->privateData->getReferencedPrivateData(key);
}

AbstractPrivateData *QoreObject::getAndClearPrivateData(qore_classid_t key, ExceptionSink* xsink) {
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
   AbstractPrivateData *pd = getAndClearPrivateData(classID, xsink);
   printd(5, "QoreObject::defaultSystemDestructor() this=%p class=%s private_data=%p\n", this, priv->theclass->getName(), pd); 
   if (pd)
      pd->deref(xsink);
}

QoreString *QoreObject::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
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

      //bool private_access_ok = qore_class_private::runtimeCheckPrivateClassAccess(*(priv->theclass));

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
bool QoreObject::is_equal_soft(const AbstractQoreNode *v, ExceptionSink* xsink) const {
   const QoreObject *o = dynamic_cast<const QoreObject *>(v);
   if (!o)
      return false;
   return !compareSoft(o, xsink);
}

bool QoreObject::is_equal_hard(const AbstractQoreNode *v, ExceptionSink* xsink) const {
   const QoreObject *o = dynamic_cast<const QoreObject *>(v);
   if (!o)
      return false;
   return !compareHard(o, xsink);
}

// returns the type name as a c string
const char *QoreObject::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode *QoreObject::evalImpl(ExceptionSink* xsink) const {
   assert(false);
   return 0;
}

AbstractQoreNode *QoreObject::evalImpl(bool &needs_deref, ExceptionSink* xsink) const {
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

AbstractQoreNode **QoreObject::getMemberValuePtrForInitialization(const char* member) {
   return priv->data->getKeyValuePtr(member);
}

bool QoreObject::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   if (runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;

   QoreSafeVarRWReadLocker sl(priv->rml);
   return priv->status != OS_DELETED;
}

#ifdef DO_OBJ_RECURSIVE_CHECK
bool ObjectRSetHelper::checkIntern(AbstractQoreNode* n) {
   if (!get_container_obj(n))
      return false;

   switch (get_node_type(n)) {
      case NT_OBJECT:
	 return checkIntern(*reinterpret_cast<QoreObject*>(n));

      case NT_LIST: {
	 QoreListNode* l = reinterpret_cast<QoreListNode*>(n);
	 ListIterator li(l);
	 while (li.next()) {
	    if (checkIntern(li.getValue()))
	       return true;
	 }

	 return false;
      }

      case NT_HASH: {
	 QoreHashNode* h = reinterpret_cast<QoreHashNode*>(n);
	 HashIterator hi(h);
	 while (hi.next()) {
	    if (checkIntern(hi.getValue()))
	       return true;
	 }

	 return false;
      }
   }

   return false;
}

int ObjectRSetHelper::removeInvalidate(ObjectRSet* ors, int tid) {
   // get a list of objects to be invalidated
   obj_vec_t rovec;
   // first grab all rsection locks
   for (obj_set_t::iterator ri = ors->begin(), re = ors->end(); ri != re; ++ri) {		     
      // if we already have the rsection lock, then ignore; already processed (either in fomap or tr_out)
      if ((*ri)->priv->rml.hasRSectionLock(tid))
	 continue;

#ifdef DEBUG
      bool hl = (*ri)->priv->rml.hasRSectionLock();
#endif
      if ((*ri)->priv->rml.tryRSectionLockNotifyWaitRead(&notifier)) {
	 printd(QRO_LVL, "ObjectRSetHelper::removeInvalidate() obj %p '%s' cannot enter rsection: tid: %d\n", *ri, (*ri)->getClassName(), (*ri)->priv->rml.rSectionTid());

	 // release other rsection locks
	 for (unsigned i = 0; i < rovec.size(); ++i) {
	    rovec[i]->priv->rml.rSectionUnlock();
	    deccnt();
	 }
	 return ORS_LOCK_ERROR;
      }
#ifdef DEBUG
      if (!hl)
	 inccnt();
#endif

      // check object status; do not scan invalid objects or objects being deleted
      if ((*ri)->priv->in_destructor || (*ri)->priv->status != OS_OK) {
	 (*ri)->priv->rml.rSectionUnlock();
	 deccnt();
	 continue;
      }
      
      rovec.push_back(*ri);
   }

   // now that we have all write locks, we have to check the RObjectSet status again
   if (!ors->active()) {
      // release other rsection locks and take no action
      for (unsigned i = 0; i < rovec.size(); ++i) {
	 rovec[i]->priv->rml.rSectionUnlock();
	 deccnt();
      }
      return 0;
   }

   // invalidate old rset when transaction is committed
   tr_invalidate.insert(ors);

   // now process old rset
   for (unsigned i = 0; i < rovec.size(); ++i) {
      assert(rovec[i]->priv->rml.hasRSectionLock());
      assert(rovec[i]->priv->rset == ors);
      
      if (rovec[i]->priv->status == OS_OK)
	 tr_out.insert(rovec[i]);
   }

   return 0;
}

bool ObjectRSetHelper::checkIntern(QoreObject& obj) {
#ifdef DEBUG
   bool hl = obj.priv->rml.hasRSectionLock();
#endif
   if (obj.priv->rml.tryRSectionLockNotifyWaitRead(&notifier)) {
      printd(QRO_LVL, "ObjectRSetHelper::checkIntern() + obj %p '%s' cannot enter rsection: rsection tid: %d\n", &obj, obj.getClassName(), obj.priv->rml.rSectionTid());
      //assert(strcmp(obj.getClassName(), "HttpListener"));
      return true;
   }
#ifdef DEBUG
   if (!hl)
      inccnt();
#endif

   // check object status; do not scan invalid objects or objects being deleted
   if (obj.priv->in_destructor || obj.priv->status != OS_OK) {
      obj.priv->rml.rSectionUnlock();
      deccnt();
      return false;
   }

   // see if the object has been scanned
   omap_t::iterator fi = fomap.find(&obj);
   if (fi != fomap.end()) {
      printd(QRO_LVL, "ObjectRSetHelper::checkIntern() + found obj %p '%s' rcount: %d in_cycle: %d ok: %d\n", &obj, obj.getClassName(), fi->second.rcount, fi->second.in_cycle, fi->second.ok);
      //printd(QRO_LVL, "ObjectRSetHelper::checkIntern() found obj %p '%s' incrementing rcount: %d -> %d\n", &obj, obj.getClassName(), fi->second.rcount, fi->second.rcount + 1);

      if (fi->second.ok) {
	 assert(!fi->second.in_cycle);
	 printd(QRO_LVL, " + %p '%s' already scanned and ok\n", &obj, obj.getClassName());
	 return false;
      }

      if (fi->second.in_cycle) {
	 assert(fi->second.rset);
	 // check if this object is part of the current cycle already - if
	 // 1) it's already in the current scan vector, or
	 // 2) the parent object of the current object is already a part of the recursive set
	 if (inCurrentSet(fi) || (ovec.size() && fi->second.rset->find(ovec.back()->first) != fi->second.rset->end())) {
	    printd(QRO_LVL, " + recursive obj %p '%s' already finalized and in current cycle (rcount: %d -> %d)\n", &obj, obj.getClassName(), fi->second.rcount, fi->second.rcount + 1);
	    assert(!fi->second.ok);
	    ++fi->second.rcount;
	 }
	 else
	    printd(QRO_LVL, " + recursive obj %p '%s' already finalized but not in current cycle\n", &obj, obj.getClassName());
	 return false;
      }

      if (!inCurrentSet(fi)) {
	 printd(QRO_LVL, " + recursive obj %p '%s' not in current cycle\n", &obj, obj.getClassName());
	 return false;
      }

      // finalize current objects immediately
      ObjectRSet* rset = fi->second.rset;
#ifdef DEBUG
      if (rset)
	 printd(QRO_LVL, " + %p '%s': reusing ObjectRSet: %p\n", &obj, obj.getClassName(), rset);
#endif
      int tid = gettid();

      int i = (int)ovec.size() - 1;
      while (i >= 0) {
	 assert(i >= 0);

	 // get iterator to object record
	 omap_t::iterator oi = ovec[i];

	 //printd(QRO_LVL, "  + %p %s rcount: %d in_cycle: %d ok: %d (rset: %p) %s\n", o, o->getClassName(), oi->second.rcount, oi->second.in_cycle, oi->second.ok, oi->second.rset, oi->first == &obj ? "" : "->");

	 //printd(QRO_LVL, " + obj %p '%s': marking %d(/%d): %p '%s' current rcount: %d\n", &obj, obj.getClassName(), i, (int)ovec.size(), ovec[i]->first, ovec[i]->first->getClassName(), ovec[i]->second.rcount);

	 //printd(QRO_LVL, "ObjectRSetHelper::checkIntern() done search obj %p '%s': matches found: %d, marking with new rset\n", &obj, obj.getClassName(), frvec.size() - fpos);

	 // merge rsets
	 if (!oi->second.rset) {
	    if (!rset) {
	       rset = new ObjectRSet;
	       printd(QRO_LVL, " + %p '%s': rcycle: %d second.rset: %p new ObjectRSet: %p\n", oi->first, oi->first->getClassName(), obj.priv->rcycle, oi->second.rset, rset);
	    }
	    else // ensure that the current object is not in the rset
	       assert(rset->find(oi->first) == rset->end());

	    // queue mark object as finalized
	    oi->second.finalize(rset);
	    assert(oi->second.rset);

	    // insert into new rset
	    rset->insert(oi->first);

	    // queue any nodes not scanned for rset invalidation
	    if (oi->first->priv->rset) {
	       assert(rset != oi->first->priv->rset);
	       if (oi->first->priv->rset->active() && removeInvalidate(oi->first->priv->rset, tid))
		  return true;
	    }

	    printd(QRO_LVL, " + %p '%s': finalized with rset: %p (rcount: %d)\n", oi->first, oi->first->getClassName(), rset, oi->second.rcount);

	    assert(!oi->second.in_cycle);
	    oi->second.in_cycle = true;
	    assert(oi->second.rset);
	    // increment rcount
	    printd(QRO_LVL, " + %p '%s': rcycle: %d second.rset: %p final: %d ok: %d rcount: %d -> %d\n", oi->first, oi->first->getClassName(), obj.priv->rcycle, oi->second.rset, oi->second.in_cycle, oi->second.ok, oi->second.rcount, oi->second.rcount + 1);
	    ++oi->second.rcount;
	 }
	 else {
	    printd(QRO_LVL, " + %p '%s': already finalized with rset: %p (size: %d, rcount: %d) current rset: %p (size: %d)\n", oi->first, oi->first->getClassName(), oi->second.rset, (int)oi->second.rset->size(), oi->second.rcount, rset ? rset : 0, rset ? (int)rset->size() : 0);

	    if (i > 0 && oi->first != &obj && !ovec[i-1]->second.in_cycle) {
	       printd(QRO_LVL, " + %p '%s': parent not yet in cycle (rcount: %d -> %d)\n", &obj, obj.getClassName(), fi->second.rcount, fi->second.rcount + 1);
	       ++oi->second.rcount;
	    }

	    if (rset == oi->second.rset) {
	       printd(QRO_LVL, " + %p '%s': rset: %p (%d) already in set\n", oi->first, oi->first->getClassName(), rset, (int)rset->size());
	    }
	    else {
	       if (!rset)
		  rset = oi->second.rset;
	       else {
		  assert(rset != oi->second.rset);
		  // here we have to merge rsets
		  if (rset->size() > oi->second.rset->size()) {
		     printd(QRO_LVL, " + %p '%s': rset: %p (%d) assimilating %p (%d)\n", oi->first, oi->first->getClassName(), rset, (int)rset->size(), oi->second.rset, (int)oi->second.rset->size());
		     // merge oi->second.rset into rset and retag objects
		     ObjectRSet *old_rset = oi->second.rset;
		     for (obj_set_t::iterator i = old_rset->begin(), e = old_rset->end(); i != e; ++i) {
			rset->insert(*i);
			omap_t::iterator noi = fomap.find(*i);
			assert(noi != fomap.end());
			noi->second.rset = rset;
		     }
		     delete old_rset;
		  }
		  else {
		     printd(QRO_LVL, " + %p '%s': oi->second.rset: %p (%d) assimilating %p (%d)\n", oi->first, oi->first->getClassName(), oi->second.rset, (int)oi->second.rset->size(), rset, (int)rset->size());
		     // merge rset into oi->second.rset and retag objects
		     ObjectRSet *old_rset = rset;
		     rset = oi->second.rset;
		     for (obj_set_t::iterator i = old_rset->begin(), e = old_rset->end(); i != e; ++i) {
			rset->insert(*i);
			omap_t::iterator noi = fomap.find(*i);
			assert(noi != fomap.end());
			noi->second.rset = rset;
		     }
		     delete old_rset;
		  }
	       }
	    }
	 }

	 /*
	 if (oi->second.in_cycle) {
	    printd(QRO_LVL, " + %p '%s': rcycle: %d second.rset: %p final: %d ok: %d already in cycle \n", oi->first, oi->first->getClassName(), obj.priv->rcycle, oi->second.rset, oi->second.in_cycle, oi->second.ok);

	    // check if parent is not in an rcycle and increment the rcount of this object if not
	    if (oi->first != &obj) {
	       assert(i > 0);
	       omap_t::iterator pi = ovec[i - 1];
	       if (!pi->second.in_cycle) {
		  printd(QRO_LVL, " + %p '%s': rcount: %d -> %d\n", oi->first, oi->first->getClassName(), oi->second.rcount, oi->second.rcount + 1);
		  ++oi->second.rcount;
	       }
	    }
	 }
	 else {
	    oi->second.in_cycle = true;
	    assert(oi->second.rset);
	    // increment rcount
	    printd(QRO_LVL, " + %p '%s': rcycle: %d second.rset: %p final: %d ok: %d rcount: %d -> %d\n", oi->first, oi->first->getClassName(), obj.priv->rcycle, oi->second.rset, oi->second.in_cycle, oi->second.ok, oi->second.rcount, oi->second.rcount + 1);
	    ++oi->second.rcount;
	 }
	 */

	 if (oi->first == &obj)
	    break;

	 --i;
	 //frvec.pop_back();
      }

      return false;
   }
   else {
      printd(QRO_LVL, "ObjectRSetHelper::checkIntern() + adding new obj %p '%s' setting rcount = 0 (current: %d rset: %p)\n", &obj, obj.getClassName(), obj.priv->rcount, obj.priv->rset);

      // insert into total scanned object set
      fi = fomap.insert(fi, omap_t::value_type(&obj, RSetStat()));

      // check if the object should be iterated
      if (!qore_object_private::getObjectCount(obj)) {
	 printd(QRO_LVL, "ObjectRSetHelper::checkIntern() obj %p '%s' will not be iterated since object count is 0\n", &obj, obj.getClassName());
	 fi->second.ok = true;
	 assert(!fi->second.rset);
	 return false;
      }
   }

   // push on current vector chain
   ovec.push_back(fi);

   // save high water mark in rset
   //size_t fpos = frvec.size();

   // remove from invalidation set if present
   tr_out.erase(&obj);

   // recursively check data members
   HashIterator hi(obj.priv->data);
   while (hi.next()) {
#ifdef DEBUG
      if (get_node_type(hi.getValue()) == NT_OBJECT)
	 printd(QRO_LVL, "ObjectRSetHelper::checkIntern() search %p '%s' key '%s' %p (%s)\n", &obj, obj.getClassName(), hi.getKey(), hi.getValue(), get_type_name(hi.getValue()));
#endif
      if (checkIntern(hi.getValue()))
	 return true;
      printd(QRO_LVL, "ObjectRSetHelper::checkIntern() result %p '%s' key '%s' %p (%s)\n", &obj, obj.getClassName(), hi.getKey(), hi.getValue(), get_type_name(hi.getValue()));
   }

   // remove from current vector chain
   ovec.pop_back();

   return false;
}

class ObjectRScanHelper {
public:
   qore_object_private& obj;
   int rcycle;

   DLLLOCAL ObjectRScanHelper(qore_object_private& o) : obj(o), rcycle(o.rcycle) {
      AutoLocker al(obj.rlck);
      while (obj.rscan) {
	 ++obj.rwaiting;
	 obj.rdone.wait(obj.rlck);
	 --obj.rwaiting;
      }
      obj.rscan = gettid();
   }

   DLLLOCAL ~ObjectRScanHelper() {
      AutoLocker al(obj.rlck);
      assert(obj.rscan == gettid());
      if (obj.rwaiting)
	 obj.rdone.signal();
      obj.rscan = 0;
   }

   DLLLOCAL bool needScan() const {
      return rcycle == obj.rcycle;
   }
};

ObjectRSetHelper::ObjectRSetHelper(QoreObject& obj) {
#ifdef DEBUG
   lcnt = 0;
#endif
   printd(QRO_LVL, "ObjectRSetHelper::ObjectRSetHelper() this: %p (%p) ENTER\n", this, &obj);

   ObjectRScanHelper rsh(*obj.priv);

   while (true) {
      if (checkIntern(obj)) {
	 rollback();
	 // wait for foreign transaction to finish if necessary
	 notifier.wait();

	 if (!rsh.needScan()) {
	    printd(QRO_LVL, "ObjectRSetHelper::ObjectRSetHelper() this: %p (%p: %s) TRANSACTION COMPLETE IN ANOTHER THREAD\n", this, &obj, obj.getClassName());
	    return;
	 }
	 printd(QRO_LVL, "ObjectRSetHelper::ObjectRSetHelper() this: %p (%p: %s) RESTARTING TRANSACTION: %d\n", this, &obj, obj.getClassName(), obj.priv->rcycle);
	 continue;
      }

      break;
   }

   commit(obj);

   printd(QRO_LVL, "ObjectRSetHelper::ObjectRSetHelper() this: %p (%p) EXIT\n", this, &obj);
}

void ObjectRSetHelper::commit(QoreObject& obj) {
#ifdef DEBUG
   bool has_obj = false;
#endif

   // invalidate rsets
   for (rs_set_t::iterator i = tr_invalidate.begin(), e = tr_invalidate.end(); i != e; ++i) {      
      (*i)->invalidate();
   }

   // unlock rsection
   for (obj_set_t::iterator i = tr_out.begin(), e = tr_out.end(); i != e; ++i) {
      assert(fomap.find(*i) == fomap.end());
      (*i)->priv->rml.rSectionUnlock();
      deccnt();
   }

   // finalize graph - exit rsection
#ifdef DEBUG
   // DEBUG
   for (omap_t::iterator i = fomap.begin(), e = fomap.end(); i != e; ++i) {
      QoreObject* tobj = i->first;
      ObjectRSet* rs = i->second.rset;
      int rcount = i->second.rcount;
      assert(tr_out.find(tobj) == tr_out.end());
      tobj->priv->setRSet(rs, rcount);

      if (tobj == &obj)
	 has_obj = true;
   }

   for (omap_t::iterator i = fomap.begin(), e = fomap.end(); i != e; ++i) {
      ObjectRSet* rs = i->second.rset;
      if (!rs)
	 continue;
      for (obj_set_t::iterator ri = rs->begin(), re = rs->end(); ri != re; ++ri) {
	 QoreObject* o = (*ri);
	 assert(o->priv->rset == rs);
      }
   }

   for (omap_t::iterator i = fomap.begin(), e = fomap.end(); i != e; ++i) {
      i->first->priv->rml.rSectionUnlock();
      deccnt();
   }
#else
   for (omap_t::iterator i = fomap.begin(), e = fomap.end(); i != e; ++i) {
      i->first->priv->setRSet(i->second.rset, i->second.rcount);
   }
   for (omap_t::iterator i = fomap.begin(), e = fomap.end(); i != e; ++i) {
      i->first->priv->rml.rSectionUnlock();
   }
#endif

   assert(fomap.empty() || has_obj);
   assert(!lcnt);
}

void ObjectRSetHelper::rollback() {
   for (omap_t::iterator i = fomap.begin(), e = fomap.end(); i != e; ++i) {
      if (i->second.rset) {
	 ObjectRSet* r = i->second.rset;
	 if (r->pop())
	    delete r;
      }
      i->first->priv->rml.rSectionUnlock();
      deccnt();
   }

   // exit rsection of objects in tr_out
   for (obj_set_t::iterator i = tr_out.begin(), e = tr_out.end(); i != e; ++i) {
      assert(fomap.find(*i) == fomap.end());
      (*i)->priv->rml.rSectionUnlock();
      deccnt();
   }

   fomap.clear();
   //frvec.clear();
   ovec.clear();
   tr_out.clear();
   tr_invalidate.clear();

   assert(!lcnt);

#ifdef _POSIX_PRIORITY_SCHEDULING
   sched_yield();
#endif
}

int ObjectRSet::canDelete() {
   if (!valid)
      return -1;
   if (in_del)
      return 1;

   {
      QoreAutoRWReadLocker al(rwl);
      if (!valid)
	 return -1;
      if (in_del)
	 return 1;

      for (obj_set_t::iterator i = begin(), e = end(); i != e; ++i) {
	 if ((*i)->priv->rcount != (*i)->references) {
	    printd(QRO_LVL, "ObjectRSet::canDelete() this: %p cannot delete graph obj %p '%s' rcount: %d refs: %d\n", this, *i, (*i)->getClassName(), (*i)->priv->rcount, (*i)->references);
	    return 0;
	 }
	 if ((*i)->priv->status != OS_OK || (*i)->priv->in_destructor) {
	    printd(QRO_LVL, "ObjectRSet::canDelete() this: %p cannot delete graph obj %p '%s' status: %d in_destructor: %d\n", this, *i, (*i)->getClassName(), (*i)->priv->status, (*i)->priv->in_destructor);
	    return 0;
	 }
	 printd(QRO_LVL, "ObjectRSet::canDelete() this: %p can delete graph obj %p '%s' rcount: %d refs: %d\n", this, *i, (*i)->getClassName(), (*i)->priv->rcount, (*i)->references);
      }
   }
   printd(QRO_LVL, "ObjectRSet::canDelete() this: %p can delete all objects in graph\n", this);

   QoreAutoRWWriteLocker al(rwl);
   in_del = true;
   return 1;
}
#endif
