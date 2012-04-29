/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  LocalVar.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

#ifndef _QORE_LOCALVAR_H

#define _QORE_LOCALVAR_H

#include <qore/intern/qore_thread_intern.h>

#include <qore/intern/QoreValue.h>

template <class T>
class VarStackPointerHelper {
   T* orig;

public:
   DLLLOCAL VarStackPointerHelper(T *v) : orig(v) {
      v->skip = true;
   }
   DLLLOCAL ~VarStackPointerHelper() {
      orig->skip = false;
   }
};

struct lvar_ref;

template <class T>
class LocalRefHelper {
protected:
   ObjectSubstitutionHelper osh;
   ProgramContextHelper pch;
   // used to skip the var entry in case it's a recursive reference
   VarStackPointerHelper<T> helper;
public:
   DLLLOCAL LocalRefHelper(T *val) : osh(val->val.v.ref->obj), pch(val->val.v.ref->pgm), helper(val) {
   }
};

template <class T>
class LValueRefHelper : public LocalRefHelper<T> {
protected:
   LValueHelper valp;

public:
   DLLLOCAL LValueRefHelper(T *val, ExceptionSink *xsink) : LocalRefHelper<T>(val), valp(val->val.v.ref->vexp, xsink) {
   }

   DLLLOCAL operator bool() const {
      return valp;
   }

   DLLLOCAL LValueHelper *operator->() {
      return &valp;
   }
};

struct lvar_ref {
   AbstractQoreNode* vexp;  // partially evaluated lvalue expression for references
   QoreObject *obj;         // for references to object members
   QoreProgram *pgm;
   bool is_vref : 1;

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      vexp->deref(xsink);
      if (obj)
         obj->tDeref();
   }

   DLLLOCAL lvar_ref(AbstractQoreNode* n_vexp, QoreObject *n_obj, QoreProgram *n_pgm);

   /*
   DLLLOCAL AbstractQoreNode** getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      ProgramContextHelper pch(pgm);

      if (obj) {
         ObjectSubstitutionHelper osh(obj);
         return get_var_value_ptr(vexp, vl, typeInfo, omap, xsink);
      }

      return get_var_value_ptr(vexp, vl, typeInfo, omap, xsink);
   }
    */
   /*
   template <class T>
   DLLLOCAL int64 plusEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL double plusEqualsFloat(double v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 minusEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL double minusEqualsFloat(double v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 orEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 andEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 modulaEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 multiplyEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 divideEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 xorEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 shiftRightEqualsBigInt(int64 v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 postIncrement(T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 preIncrement(T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 postDecrement(T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL int64 preDecrement(T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL double multiplyEqualsFloat(double v, T *vv, ExceptionSink *xsink);

   template <class T>
   DLLLOCAL double divideEqualsFloat(double v, T *vv, ExceptionSink *xsink);
   */
};

union qore_value_ref_u {
   bool b;
   int64 i;
   double f;
   AbstractQoreNode* n;
   lvar_ref* ref;
};

class VarValueBase {
protected:
   DLLLOCAL int checkFinalized(ExceptionSink *xsink) {
      if (finalized) {
         xsink->raiseException("DESTRUCTOR-ERROR", "illegal variable assignment after second phase of variable destruction");
         return -1;
      }
      return 0;
   }

public:
   QoreValue<qore_value_ref_u> val;
   const char *id;
   bool skip : 1;
   bool finalized : 1;

   DLLLOCAL VarValueBase(const char *n_id, valtype_t t = QV_Node, bool n_skip = false) : val(t), id(n_id), skip(n_skip), finalized(false) {
   }

   DLLLOCAL VarValueBase(const char* n_id, const QoreTypeInfo* varTypeInfo) : val(varTypeInfo), id(n_id), skip(false), finalized(false) {
   }

   DLLLOCAL VarValueBase() : val(QV_Bool), finalized(false) {
   }

   DLLLOCAL void del(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         //printd(5, "LocalVarValue::uninstantiate() this=%p uninstantiating local variable '%s' reference expression vexp=%p\n", this, id, val.v.ref->vexp);
         val.v.ref->uninstantiate(xsink);
         delete val.v.ref;
#ifdef DEBUG
         val.v.ref = 0;
#endif
         return;
      }

      discard(val.remove(true), xsink);
   }

   DLLLOCAL bool isRef() const {
      return val.type == QV_Ref;
   }
};

struct SkipHelper {
   VarValueBase *vvb;

   DLLLOCAL SkipHelper(VarValueBase *n_vvb) : vvb(n_vvb) {
      assert(!vvb->skip);
      vvb->skip = true;
   }

   DLLLOCAL ~SkipHelper() {
      vvb->skip = false;
   }
};

class LocalVarValue : public VarValueBase {
public:
   DLLLOCAL void set(const char *n_id, const QoreTypeInfo *typeInfo, AbstractQoreNode* value) {
      skip = false;
      id = n_id;

      if (typeInfo == bigIntTypeInfo || typeInfo == softBigIntTypeInfo)
         val.set(QV_Int);
      else if (typeInfo == floatTypeInfo || typeInfo == softFloatTypeInfo)
         val.set(QV_Float);
      else if (typeInfo == boolTypeInfo || typeInfo == softBoolTypeInfo)
         val.set(QV_Bool);
      else
         val.set(QV_Node);
      
      // no exception is possible here as there was no previous value
      // also since only basic value types could be returned, no exceptions can occur with the value passed either
      discard(val.assign(value), 0);
   }

   DLLLOCAL void set(const char *n_id, AbstractQoreNode* vexp, QoreObject *obj, QoreProgram *pgm) {
      assert(pgm);

      skip = false;
      id = n_id;

      val.set(QV_Ref);
      val.v.ref = new lvar_ref(vexp, obj, pgm);
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      del(xsink);
   }

   DLLLOCAL bool optimizedLocal() const {
      return val.optimized();
   }

   //DLLLOCAL bool isOptimized(const QoreTypeInfo*& varTypeInfo) const;
   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh);

   DLLLOCAL qore_type_t getValueType() const {
      assert(optimizedLocal());

      return val.getType();
   }

   DLLLOCAL const char* getValueTypeName() const {
      assert(optimizedLocal());

      return val.getTypeName();
   }

   /*
   DLLLOCAL AbstractQoreNode** getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      if (checkFinalized(xsink))
         return 0;

      if (val.type == QV_Ref) {
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<LocalVarValue> helper(this);
         
         return val.v.ref->getValuePtr(vl, typeInfo, omap, xsink);
      }

      return val.getValuePtr(xsink);
   }

   DLLLOCAL AbstractQoreNode** getContainerValuePtr(AutoVLock& vl, const QoreTypeInfo*& typeInfo, ObjMap& omap, ExceptionSink* xsink) {
      if (checkFinalized(xsink))
         return 0;

      if (val.type == QV_Ref) {
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<LocalVarValue> helper(this);

         return val.v.ref->getValuePtr(vl, typeInfo, omap, xsink);
      }

      return val.getContainerValuePtr();
   }
    */

   DLLLOCAL void finalize(ExceptionSink *xsink) {
      assert(!finalized);
      if (val.type == QV_Node) {
         discard(val.remove(true), xsink);
         finalized = true;
      }
   }

   /*
   // value is already referenced for assignment
   DLLLOCAL void assign(AbstractQoreNode* value, ExceptionSink *xsink) {
      ReferenceHolder<> value_holder(value, xsink);

      switch (val.type) {
         case QV_Ref:
            setValueRef(value_holder, xsink);
            break;

         case QV_Node:
            if (checkFinalized(xsink))
               return;
            // fall through here

         default:
            discard(val.assign(value_holder.release()), xsink);
            break;
      }
   }

   DLLLOCAL void setValueRef(ReferenceHolder<AbstractQoreNode> &value_holder, ExceptionSink *xsink) {
      assert(val.type == QV_Ref);

      LValueRefHelper<LocalVarValue> valp(this, xsink);
      if (!valp)
         return;

      valp->assign(value_holder.release());
   }

   DLLLOCAL void assignBigInt(int64 v, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         ReferenceHolder<AbstractQoreNode> value_holder(new QoreBigIntNode(v), xsink);
         setValueRef(value_holder, xsink);
         return;
      }

      discard(val.assign(v), xsink);
   }

   DLLLOCAL void assignFloat(double v, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         ReferenceHolder<AbstractQoreNode> value_holder(new QoreFloatNode(v), xsink);
         setValueRef(value_holder, xsink);
         return;
      }

      discard(val.assign(v), xsink);
   }

   DLLLOCAL int64 plusEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL double plusEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL int64 minusEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL double minusEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL int64 orEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 andEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 modulaEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 multiplyEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 divideEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 xorEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 postIncrement(ExceptionSink *xsink);

   DLLLOCAL int64 preIncrement(ExceptionSink *xsink);

   DLLLOCAL int64 postDecrement(ExceptionSink *xsink);

   DLLLOCAL int64 preDecrement(ExceptionSink *xsink);
*/

   DLLLOCAL AbstractQoreNode* eval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<LocalVarValue> helper(this);
         return val.v.ref->vexp->eval(xsink);
      }

      return val.eval();
   }

   DLLLOCAL AbstractQoreNode* eval(bool &needs_deref, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         needs_deref = true;
         LocalRefHelper<LocalVarValue> helper(this);
         return val.v.ref->vexp->eval(xsink);
      }
 
      return val.eval(needs_deref);
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<LocalVarValue> helper(this);
         return val.v.ref->vexp->bigIntEval(xsink);
      }

      return val.getAsBigInt();
   }

   DLLLOCAL int intEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<LocalVarValue> helper(this);
         return val.v.ref->vexp->integerEval(xsink);
      }

      return (int)val.getAsBigInt();
   }

   DLLLOCAL bool boolEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<LocalVarValue> helper(this);
         return val.v.ref->vexp->boolEval(xsink);
      }

      return val.getAsBool();
   }

   DLLLOCAL double floatEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<LocalVarValue> helper(this);
         return val.v.ref->vexp->floatEval(xsink);
      }

      return val.getAsFloat();
   }

   /*
   DLLLOCAL double multiplyEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL double divideEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL int64 removeBigInt(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LValueRefHelper<LocalVarValue> valp(this, xsink);
         if (!valp)
            return 0;
         // FIXME: implement removeBigInt
         ReferenceHolder<> tmp(valp->take_value(), xsink);
         return tmp ? tmp->getAsBigInt() : 0;
      }

      ReferenceHolder<> old(xsink);
      return val.removeBigInt(old.getRef());
   }

   DLLLOCAL double removeFloat(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LValueRefHelper<LocalVarValue> valp(this, xsink);
         if (!valp)
            return 0;
         // FIXME: implement removeFloat
         ReferenceHolder<> tmp(valp->take_value(), xsink);
         return tmp ? tmp->getAsFloat() : 0;
      }

      ReferenceHolder<> old(xsink);
      return val.removeFloat(old.getRef());
   }

   DLLLOCAL AbstractQoreNode* remove(ExceptionSink *xsink, bool for_del) {
      if (val.type == QV_Ref) {
         LValueRefHelper<LocalVarValue> valp(this, xsink);
         return !valp ? 0 : valp->take_value();
      }

      return val.remove(for_del);
   }
   */
};

/* NOTE: the proper threading behavior of this class depends on the fact that the
   type (reference or value) can never change.  also the reference expression
   and object will also not change.
   only the value operations are locked.
*/
struct ClosureVarValue : public VarValueBase, public QoreReferenceCounter, public QoreThreadLock {
public:
   const QoreTypeInfo* typeInfo; // type restriction for lvalue

   DLLLOCAL ClosureVarValue(const char *n_id, const QoreTypeInfo* varTypeInfo, AbstractQoreNode* value) : VarValueBase(n_id, varTypeInfo), typeInfo(varTypeInfo) {
      // also since only basic value types could be returned, no exceptions can occur with the value passed either
      discard(val.assign(value), 0);
   }

   DLLLOCAL ClosureVarValue(const char *n_id, AbstractQoreNode* vexp, QoreObject *obj, QoreProgram *pgm) : VarValueBase(n_id, QV_Ref), typeInfo(0) {
      val.v.ref = new lvar_ref(vexp, obj, pgm);
   }

   DLLLOCAL void ref() { ROreference(); }

   DLLLOCAL void deref(ExceptionSink *xsink) { if (ROdereference()) { del(xsink); delete this; } }

   //DLLLOCAL bool isOptimized(const QoreTypeInfo*& varTypeInfo) const;
   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;
   DLLLOCAL void remove(LValueRemoveHelper& lvrh);

   /*
   DLLLOCAL void assignBigInt(int64 v, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         // FIXME: implement assignFloat for LValueRefHelper
         ReferenceHolder<AbstractQoreNode> value_holder(new QoreBigIntNode(v), xsink);
         setValueRef(value_holder, xsink);
         return;
      }

      // dereference old value outside the lock
      AbstractQoreNode* old;
      {
         AutoLocker al(this);
         old = val.assign(v);
      }
      discard(old, xsink);
   }

   DLLLOCAL void assignFloat(double v, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         // FIXME: implement assignFloat for LValueRefHelper
         ReferenceHolder<AbstractQoreNode> value_holder(new QoreFloatNode(v), xsink);
         setValueRef(value_holder, xsink);
         return;
      }

      // dereference old value outside the lock
      AbstractQoreNode* old;
      {
         AutoLocker al(this);
         old = val.assign(v);
      }
      discard(old, xsink);
   }
*/
   /*
   DLLLOCAL AbstractQoreNode** getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<ClosureVarValue> helper(this);

         return val.v.ref->getValuePtr(vl, typeInfo, omap, xsink);
      }

      lock();
      vl->set(this);

      if (checkFinalized(xsink))
         return 0;

      return val.getValuePtr(xsink);
   }

   DLLLOCAL AbstractQoreNode** getContainerValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         // skip this entry in case it's a recursive reference
         VarStackPointerHelper<ClosureVarValue> helper(this);

         return val.v.ref->getValuePtr(vl, typeInfo, omap, xsink);
      }

      lock();
      vl->set(this);

      if (checkFinalized(xsink))
         return 0;

      return val.getContainerValuePtr();
   }
*/

   DLLLOCAL void finalize(ExceptionSink *xsink) {
      SafeLocker sl(this);
      if (!finalized && val.type != QV_Ref) {
         AbstractQoreNode* dr = val.remove(true);
         finalized = true;
         sl.unlock();
         discard(dr, xsink);         
      }
   }

   /*
   DLLLOCAL void setValueRef(ReferenceHolder<AbstractQoreNode> &value_holder, ExceptionSink *xsink) {
      LValueRefHelper<ClosureVarValue> valp(this, xsink);
      if (!valp)
         return;

      valp->assign(value_holder.release());
   }

   // value is already referenced for assignment
   DLLLOCAL void assign(AbstractQoreNode* value, ExceptionSink *xsink) {
      ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

      if (val.type == QV_Ref) {
         setValueRef(value_holder, xsink);
         return;
      }

      // make sure and dereference the old value outside the lock
      AbstractQoreNode* dr = 0;
      {
         AutoLocker al(this);

         if (checkFinalized(xsink))
            return;

         dr = val.assign(value_holder.release());
      }
      discard(dr, xsink);
   }
*/

   DLLLOCAL AbstractQoreNode* eval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<ClosureVarValue> helper(this);
         return val.v.ref->vexp->eval(xsink);
      }

      AutoLocker al(this);
      return val.eval();
   }

   DLLLOCAL AbstractQoreNode* eval(bool& needs_deref, ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<ClosureVarValue> helper(this);
         return val.v.ref->vexp->eval(needs_deref, xsink);
      }

      AutoLocker al(this);
      return val.eval(needs_deref);
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<ClosureVarValue> helper(this);
         return val.v.ref->vexp->bigIntEval(xsink);
      }

      AutoLocker al(this);
      return val.getAsBigInt();
   }

   DLLLOCAL int intEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<ClosureVarValue> helper(this);
         return val.v.ref->vexp->integerEval(xsink);
      }

      AutoLocker al(this);
      return (int)val.getAsBigInt();
   }

   DLLLOCAL bool boolEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<ClosureVarValue> helper(this);
         return val.v.ref->vexp->boolEval(xsink);
      }

      AutoLocker al(this);
      return val.getAsBool();
   }

   DLLLOCAL double floatEval(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LocalRefHelper<ClosureVarValue> helper(this);
         return val.v.ref->vexp->floatEval(xsink);
      }

      AutoLocker al(this);
      return val.getAsFloat();
   }

   /*
   DLLLOCAL int64 plusEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL double plusEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL int64 minusEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL double minusEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL int64 orEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 andEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 modulaEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 multiplyEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 divideEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 xorEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink);

   DLLLOCAL int64 postIncrement(ExceptionSink *xsink);

   DLLLOCAL int64 preIncrement(ExceptionSink *xsink);

   DLLLOCAL int64 postDecrement(ExceptionSink *xsink);

   DLLLOCAL int64 preDecrement(ExceptionSink *xsink);

   DLLLOCAL double multiplyEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL double divideEqualsFloat(double v, ExceptionSink *xsink);

   DLLLOCAL int64 removeBigInt(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LValueRefHelper<ClosureVarValue> valp(this, xsink);
         if (!valp)
            return 0;
         // FIXME: implement removeBigInt
         ReferenceHolder<> tmp(valp->take_value(), xsink);
         return tmp ? tmp->getAsBigInt() : 0;
      }

      ReferenceHolder<> old(xsink);      
      AutoLocker al(this);
      return val.removeBigInt(old.getRef());
   }

   DLLLOCAL double removeFloat(ExceptionSink *xsink) {
      if (val.type == QV_Ref) {
         LValueRefHelper<ClosureVarValue> valp(this, xsink);
         if (!valp)
            return 0;
         // FIXME: implement removeBigInt
         ReferenceHolder<> tmp(valp->take_value(), xsink);
         return tmp ? tmp->getAsFloat() : 0.0;
      }

      ReferenceHolder<> old(xsink);      
      AutoLocker al(this);
      return val.removeFloat(old.getRef());
   }

   DLLLOCAL AbstractQoreNode* remove(ExceptionSink *xsink, bool for_del) {
      if (val.type == QV_Ref) {
         LValueRefHelper<ClosureVarValue> valp(this, xsink);
         return !valp ? 0 : valp->take_value();
      }

      AutoLocker al(this);
      return val.remove(for_del);
   }
   */
};

// now shared between parent and child Program objects for top-level local variables with global scope
class LocalVar {
private:
   std::string name;
   bool closure_use, needs_value_instantiation;
   const QoreTypeInfo *typeInfo;

   DLLLOCAL LocalVarValue *get_var() const {
      return thread_find_lvar(name.c_str());
   }

public:
   DLLLOCAL LocalVar(const char *n_name, const QoreTypeInfo *ti) : name(n_name), closure_use(false), needs_value_instantiation(ti->hasType() ? true : false), typeInfo(ti) {
   }

   DLLLOCAL LocalVar(const LocalVar &old) : name(old.name), closure_use(old.closure_use), needs_value_instantiation(old.needs_value_instantiation), typeInfo(old.typeInfo) {
   }

   DLLLOCAL ~LocalVar() {
   }

   DLLLOCAL void instantiate() const {
      //printd(0, "LocalVar::instantiate() this=%p '%s'\n", this, name.c_str());
      //instantiate(typeInfo->getDefaultValue());
      instantiate(0);
   }

   DLLLOCAL void instantiate(AbstractQoreNode* value) const {
      //printd(5, "LocalVar::instantiate(%p) this=%p '%s' value type=%s closure_use=%s pgm=%p\n", value, this, name.c_str(), get_type_name(value), closure_use ? "true" : "false", getProgram());

      if (!closure_use) {
         LocalVarValue *val = thread_instantiate_lvar();
         val->set(name.c_str(), typeInfo, value);
      }
      else
         thread_instantiate_closure_var(name.c_str(), typeInfo, value);
   }

   DLLLOCAL void instantiate_object(QoreObject *value) const {
      //printd(5, "LocalVar::instantiate_object(%p) this=%p '%s'\n", value, this, name.c_str());
      instantiate(value);
      value->ref();
   }

   DLLLOCAL void instantiate(AbstractQoreNode* vexp, QoreObject *obj, QoreProgram *pgm) {
      //printd(5, "LocalVar::instantiate(%p, %p) this=%p '%s'\n", vexp, obj, this, name.c_str());

      if (!closure_use) {
         LocalVarValue *val = thread_instantiate_lvar();
         val->set(name.c_str(), vexp, obj, pgm);
      }
      else
         thread_instantiate_closure_var(name.c_str(), vexp, obj, pgm);
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) const  {
      //printd(5, "LocalVar::uninstantiate() this=%p '%s' closure_use=%s pgm=%p\n", this, name.c_str(), closure_use ? "true" : "false", getProgram());

      if (!closure_use) {
         thread_uninstantiate_lvar(xsink);
      }
      else
         thread_uninstantiate_closure_var(xsink);
   }

   /*
   DLLLOCAL AbstractQoreNode** getValuePtr(AutoVLock *vl, const QoreTypeInfo *&n_typeInfo, ObjMap &omap, ExceptionSink *xsink) const {
      // typeInfo is set here, but in case the variable is a reference, the actual
      // typeInfo structure will be set from the referenced value
      n_typeInfo = typeInfo;
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->getValuePtr(vl, n_typeInfo, omap, xsink);
      }
      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->getValuePtr(vl, n_typeInfo, omap, xsink);
   }

   DLLLOCAL AbstractQoreNode** getContainerValuePtr(AutoVLock *vl, const QoreTypeInfo *&n_typeInfo, ObjMap &omap, ExceptionSink *xsink) const {
      // typeInfo is set here, but in case the variable is a reference, the actual
      // typeInfo structure will be set from the referenced value
      n_typeInfo = typeInfo;
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->getContainerValuePtr(vl, n_typeInfo, omap, xsink);
      }
      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->getContainerValuePtr(vl, n_typeInfo, omap, xsink);
   }
*/

   /*
   // value is already referenced for assignment
   DLLLOCAL void assign(AbstractQoreNode* value, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         val->assign(value, xsink);
         return;
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      val->assign(value, xsink);
   }

   DLLLOCAL void assignBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         val->assignBigInt(v, xsink);
         return;
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      val->assignBigInt(v, xsink);
      return;
   }

   DLLLOCAL void assignFloat(double v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         val->assignFloat(v, xsink);
         return;
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      val->assignFloat(v, xsink);
   }
*/
   DLLLOCAL AbstractQoreNode* eval(ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->eval(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->eval(xsink);
   }

   DLLLOCAL AbstractQoreNode* eval(bool &needs_deref, ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->eval(needs_deref, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->eval(needs_deref, xsink);
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->bigIntEval(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->bigIntEval(xsink);
   }

   DLLLOCAL int intEval(ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->intEval(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->intEval(xsink);
   }

   DLLLOCAL bool boolEval(ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->boolEval(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->boolEval(xsink);
   }

   DLLLOCAL double floatEval(ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->floatEval(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->floatEval(xsink);
   }
/*
   DLLLOCAL int64 plusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->plusEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->plusEqualsBigInt(v, xsink);      
   }

   DLLLOCAL double plusEqualsFloat(double v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->plusEqualsFloat(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->plusEqualsFloat(v, xsink);      
   }

   DLLLOCAL int64 minusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->minusEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->minusEqualsBigInt(v, xsink);      
   }

   DLLLOCAL double minusEqualsFloat(double v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->minusEqualsFloat(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->minusEqualsFloat(v, xsink);      
   }

   DLLLOCAL int64 orEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->orEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->orEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 andEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->andEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->andEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 modulaEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->modulaEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->modulaEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 multiplyEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->multiplyEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->multiplyEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 divideEqualsBigInt(int64 v, ExceptionSink *xsink) {
      assert(v);
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->divideEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->divideEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 xorEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->xorEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->xorEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->shiftLeftEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->shiftLeftEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->shiftRightEqualsBigInt(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->shiftRightEqualsBigInt(v, xsink);      
   }

   DLLLOCAL int64 postIncrement(ExceptionSink *xsink) {
      return !closure_use ? get_var()->LocalVarValue::postIncrement(xsink) : thread_find_closure_var(name.c_str())->ClosureVarValue::postIncrement(xsink);
   }

   DLLLOCAL int64 preIncrement(ExceptionSink *xsink) {
      return !closure_use ? get_var()->LocalVarValue::preIncrement(xsink) : thread_find_closure_var(name.c_str())->ClosureVarValue::preIncrement(xsink);
   }

   DLLLOCAL int64 postDecrement(ExceptionSink *xsink) {
      return !closure_use ? get_var()->LocalVarValue::postDecrement(xsink) : thread_find_closure_var(name.c_str())->ClosureVarValue::postDecrement(xsink);
   }

   DLLLOCAL int64 preDecrement(ExceptionSink *xsink) {
      return !closure_use ? get_var()->LocalVarValue::preDecrement(xsink) : thread_find_closure_var(name.c_str())->ClosureVarValue::preDecrement(xsink);
   }

   DLLLOCAL double multiplyEqualsFloat(double v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->multiplyEqualsFloat(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->multiplyEqualsFloat(v, xsink);      
   }

   DLLLOCAL double divideEqualsFloat(double v, ExceptionSink* xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->divideEqualsFloat(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->divideEqualsFloat(v, xsink);      
   }

   DLLLOCAL int64 removeBigInt(ExceptionSink* xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->removeBigInt(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->removeBigInt(xsink);      
   }

   DLLLOCAL double removeFloat(ExceptionSink* xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->removeFloat(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->removeFloat(xsink);      
   }

   DLLLOCAL AbstractQoreNode* remove(ExceptionSink* xsink, bool for_del) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->remove(xsink, for_del);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->remove(xsink, for_del);
   }
*/

   DLLLOCAL const char *getName() const {
      return name.c_str();
   }

   DLLLOCAL const std::string &getNameStr() const {
      return name;
   }

   DLLLOCAL void setClosureUse() {
      closure_use = true;
   }

   DLLLOCAL bool closureUse() const { 
      return closure_use;
   }

   DLLLOCAL bool isRef() const {
      return !closure_use ? get_var()->isRef() : thread_find_closure_var(name.c_str())->isRef();
   }

   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const {
      if (!closure_use) {
         lvh.setTypeInfo(typeInfo);
         return get_var()->getLValue(lvh, for_remove);
      }

      return thread_find_closure_var(name.c_str())->getLValue(lvh, for_remove);
   }

   DLLLOCAL void remove(LValueRemoveHelper& lvrh) {
      if (!closure_use)
         return get_var()->remove(lvrh);

      return thread_find_closure_var(name.c_str())->remove(lvrh);
   }

   /*
   DLLLOCAL bool isOptimized(const QoreTypeInfo*& varTypeInfo) const {
      if (!closure_use) {
         varTypeInfo = typeInfo;
         return get_var()->isOptimized(varTypeInfo);
      }

      return thread_find_closure_var(name.c_str())->isOptimized(varTypeInfo);
   }
    */

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL qore_type_t getValueType() const {
      return !closure_use ? get_var()->val.getType() : thread_find_closure_var(name.c_str())->val.getType();
   }

   DLLLOCAL const char* getValueTypeName() const {
      return !closure_use ? get_var()->val.getTypeName() : thread_find_closure_var(name.c_str())->val.getTypeName();
   }

   DLLLOCAL bool needsValueInstantiation() const {
      return needs_value_instantiation;
   }

   DLLLOCAL void unsetNeedsValueInstantiation() {
      if (needs_value_instantiation)
         needs_value_instantiation = false;
   }

#if 0
   DLLLOCAL bool needsAssignmentAtInstantiation() const {
      return needs_value_instantiation && !typeInfo->hasDefaultValue() ? true : false;
   }
#endif
};

typedef LocalVar* lvar_ptr_t;

#endif
