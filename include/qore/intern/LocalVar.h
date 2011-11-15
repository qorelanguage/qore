/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  LocalVar.h

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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

template <class T>
class VarStackPointerHelper {
   T *orig;

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
   DLLLOCAL LocalRefHelper(T *val) : osh(val->val.ref.obj), pch(val->val.ref.pgm), helper(val) {
   }
};

template <class T>
class LValueRefHelper : public LocalRefHelper<T> {
protected:
   LValueHelper valp;

public:
   DLLLOCAL LValueRefHelper(T *val, ExceptionSink *xsink) : LocalRefHelper<T>(val), valp(val->val.ref.vexp, xsink) {
   }

   DLLLOCAL operator bool() const {
      return valp;
   }

   DLLLOCAL LValueHelper *operator->() {
      return &valp;
   }
};

struct lvar_ref {
   AbstractQoreNode *vexp;  // partially evaluated lvalue expression for references
   QoreObject *obj;         // for references to object members
   QoreProgram *pgm;
   bool is_vref : 1;

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      vexp->deref(xsink);
      if (obj)
         obj->tDeref();
   }

   DLLLOCAL void assign(AbstractQoreNode *n_vexp, QoreObject *n_obj, QoreProgram *n_pgm);

   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      ProgramContextHelper pch(pgm);

      if (obj) {
         ObjectSubstitutionHelper osh(obj);
         return get_var_value_ptr(vexp, vl, typeInfo, omap, xsink);
      }

      return get_var_value_ptr(vexp, vl, typeInfo, omap, xsink);
   }

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
};

union lvar_u {
   AbstractQoreNode *value;
   lvar_ref ref;
   int64 val_int;
   double val_float;
   bool val_bool;

   template <class T, typename t, int nt>
   DLLLOCAL int value_ensureUnique(ExceptionSink *xsink) {
      if (!value) {
         value = new T;
         return 0;
      }

      if (value->getType() != NT_INT) {
         t i = T::getValue(value);
         value->deref(xsink);
         if (*xsink) {
            value = 0;
            return -1;
         }
         value = new T(i);
         return 0;
      }

      if (value->is_unique())
         return 0;

      AbstractQoreNode *old = value;
      value = old->realCopy();
      old->deref(xsink);
      return 0;
   }
  
   DLLLOCAL int64 value_plusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val += v;
      return vv->val;
   }

   DLLLOCAL double value_plusEqualsFloat(double v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreFloatNode, double, NT_FLOAT>(xsink))
         return 0;

      QoreFloatNode *vv = reinterpret_cast<QoreFloatNode *>(value);
      vv->f += v;
      return vv->f;
   }

   DLLLOCAL int64 value_minusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val -= v;
      return vv->val;
   }

   DLLLOCAL double value_minusEqualsFloat(double v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreFloatNode, double, NT_FLOAT>(xsink))
         return 0;

      QoreFloatNode *vv = reinterpret_cast<QoreFloatNode *>(value);
      vv->f -= v;
      return vv->f;
   }

   DLLLOCAL int64 value_orEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val |= v;
      return vv->val;
   }

   DLLLOCAL int64 value_andEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val &= v;
      return vv->val;
   }

   DLLLOCAL int64 value_modulaEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val %= v;
      return vv->val;
   }

   DLLLOCAL int64 value_multiplyEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val *= v;
      return vv->val;
   }

   DLLLOCAL int64 value_divideEqualsBigInt(int64 v, ExceptionSink *xsink) {
      assert(v);
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val /= v;
      return vv->val;
   }

   DLLLOCAL int64 value_xorEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val ^= v;
      return vv->val;
   }

   DLLLOCAL int64 value_shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val <<= v;
      return vv->val;
   }

   DLLLOCAL int64 value_shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      vv->val >>= v;
      return vv->val;
   }

   DLLLOCAL int64 value_postIncrement(ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      int64 rv = vv->val;
      ++vv->val;
      return rv;
   }

   DLLLOCAL int64 value_preIncrement(ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      return ++(reinterpret_cast<QoreBigIntNode *>(value)->val);
   }

   DLLLOCAL int64 value_postDecrement(ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      QoreBigIntNode *vv = reinterpret_cast<QoreBigIntNode *>(value);
      int64 rv = vv->val;
      --vv->val;
      return rv;
   }

   DLLLOCAL int64 value_preDecrement(ExceptionSink *xsink) {
      if (value_ensureUnique<QoreBigIntNode, int64, NT_INT>(xsink))
         return 0;

      return --(reinterpret_cast<QoreBigIntNode *>(value)->val);
   }

   DLLLOCAL double value_multiplyEqualsFloat(double v, ExceptionSink *xsink) {
      if (value_ensureUnique<QoreFloatNode, double, NT_FLOAT>(xsink))
         return 0;

      QoreFloatNode *vv = reinterpret_cast<QoreFloatNode *>(value);
      vv->f *= v;
      return vv->f;
   }

   DLLLOCAL double value_divideEqualsFloat(double v, ExceptionSink *xsink) {
      assert(v);
      if (value_ensureUnique<QoreFloatNode, double, NT_FLOAT>(xsink))
         return 0;

      QoreFloatNode *vv = reinterpret_cast<QoreFloatNode *>(value);
      vv->f /= v;
      return vv->f;
   }
};

enum vvt_e { 
   VVT_Normal = 0,
   VVT_Ref    = 1,
   VVT_Int    = 2,
   VVT_Float  = 3,
   VVT_Bool   = 4,
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
   union lvar_u val;
   const char *id;
   vvt_e vvt : 3;
   bool skip : 1;
   bool finalized : 1;

   DLLLOCAL VarValueBase(const char *n_id, vvt_e n_vvt = VVT_Normal, bool n_skip = false) : id(n_id), vvt(n_vvt), skip(n_skip), finalized(false) {
   }
   DLLLOCAL VarValueBase() : finalized(false) {
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
   DLLLOCAL void set(const char *n_id, const QoreTypeInfo *typeInfo, AbstractQoreNode *value) {
      skip = false;
      id = n_id;
      if (typeInfo == bigIntTypeInfo || typeInfo == softBigIntTypeInfo) {
         vvt = VVT_Int;
         ReferenceHolder<AbstractQoreNode> tmp(value, 0);
         val.val_int = value ? value->getAsBigInt() : 0;
      }
#if 0
      else if (typeInfo == floatTypeInfo || typeInfo == softFloatTypeInfo) {
         vvt = VVT_Float;
         ReferenceHolder<AbstractQoreNode> tmp(value, 0);
         val.val_float = value ? value->getAsFloat() : 0.0;
      }
      else if (typeInfo == boolTypeInfo || typeInfo == softBoolTypeInfo) {
         vvt = VVT_Bool;
         ReferenceHolder<AbstractQoreNode> tmp(value, 0);
         val.val_bool = value ? value->getAsBool() : false;
      }
#endif
      else {
         vvt = VVT_Normal;
         val.value = value;
      }
   }

   DLLLOCAL void set(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm) {
      assert(pgm);
      vvt = VVT_Ref;
      skip = false;
      id = n_id;
      val.ref.assign(vexp, obj, pgm);
   }

   DLLLOCAL void uninstantiate(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            //printd(5, "LocalVarValue::uninstantiate() this=%p uninstantiating local variable '%s' val=%p\n", this, id, val.value);
            discard(val.value, xsink);
            break;

         case VVT_Ref:
            //printd(5, "LocalVarValue::uninstantiate() this=%p uninstantiating local variable '%s' reference expression vexp=%p\n", this, id, val.ref.vexp);
            val.ref.uninstantiate(xsink);
            break;

#ifdef DEBUG
         case VVT_Int:
         case VVT_Float:
         case VVT_Bool:
            break;

         default:
            assert(false);
#endif
      }
   }

   DLLLOCAL LocalVarValue* optimized(const QoreTypeInfo *&varTypeInfo) const;

   DLLLOCAL bool optimizedLocal() const {
      return vvt != VVT_Normal && vvt != VVT_Ref;
   }

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      assert(optimizedLocal());
      switch (vvt) {
         case VVT_Int:
            return bigIntTypeInfo;
         case VVT_Float:
            return floatTypeInfo;
         case VVT_Bool:
            return boolTypeInfo;
      }
      assert(false);
      return 0;
   }

   DLLLOCAL qore_type_t getValueType() const {
      assert(optimizedLocal());
      //assert(vvt != VVT_Ref);

      switch (vvt) {
         /*
         case VVT_Normal:
            return get_node_type(val.value);
         */

         case VVT_Int:
            return NT_INT;

         case VVT_Float:
            return NT_FLOAT;

         case VVT_Bool:
            return NT_BOOLEAN;
      }
      assert(false);
      return NT_NOTHING;
   }

   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      if (checkFinalized(xsink))
         return 0;

      if (vvt == VVT_Normal)
         return const_cast<AbstractQoreNode **>(&val.value);

      if (vvt != VVT_Ref) {
         assert(false);
         xsink->raiseException("DEPRECATED-API", "this module uses a deprecated API that is no longer supported; please either update the module with a newer version that uses newer APIs that work with lvalues/references in newer versions of Qore or contact the developer to update the module");
         return 0;
      }

      // skip this entry in case it's a recursive reference
      VarStackPointerHelper<LocalVarValue> helper(this);

      return val.ref.getValuePtr(vl, typeInfo, omap, xsink);
   }

   DLLLOCAL void finalize(ExceptionSink *xsink) {
      assert(!finalized);
      if (vvt == VVT_Normal) {
         if (val.value) {
            discard(val.value, xsink);
            val.value = 0;
         }
         finalized = true;
      }
   }

   // value is already referenced for assignment
   DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink) {
      ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

      switch (vvt) {
         case VVT_Normal:
            if (checkFinalized(xsink))
               return;
            setValueNormal(value_holder, xsink);
            break;

         case VVT_Ref:
            setValueRef(value_holder, xsink);
            break;

         case VVT_Int:
            val.val_int = value_holder ? value_holder->getAsBigInt() : 0;
            break;

         case VVT_Float:
            val.val_float = value_holder ? value_holder->getAsFloat() : 0.0;
            break;

         case VVT_Bool:
            val.val_bool = value_holder ? value_holder->getAsBool() : false;
            break;

#ifdef DEBUG
         default:
            assert(false);
#endif
      }
   }

   DLLLOCAL void setValueNormal(ReferenceHolder<AbstractQoreNode> &value_holder, ExceptionSink *xsink) {
      assert(vvt == VVT_Normal);
      if (val.value)
         val.value->deref(xsink);
      val.value = value_holder.release();
   }

   DLLLOCAL void setValueRef(ReferenceHolder<AbstractQoreNode> &value_holder, ExceptionSink *xsink) {
      assert(vvt == VVT_Ref);

      LValueRefHelper<LocalVarValue> valp(this, xsink);
      if (!valp)
         return;

      valp->assign(value_holder.release());
   }

   DLLLOCAL void assignBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Int:
            val.val_int = v;
            break;

         case VVT_Float:
            val.val_float = v;
            break;

         case VVT_Ref: {
            ReferenceHolder<AbstractQoreNode> value_holder(new QoreBigIntNode(v), xsink);
            setValueRef(value_holder, xsink);
            break;
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }      
   }

   DLLLOCAL void assignFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Int:
            val.val_int = v;
            break;

         case VVT_Float:
            val.val_float = v;
            break;

         case VVT_Ref: {
            ReferenceHolder<AbstractQoreNode> value_holder(new QoreFloatNode(v), xsink);
            setValueRef(value_holder, xsink);
            break;
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }      
   }

   DLLLOCAL int64 plusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_plusEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int += v;

         case VVT_Ref:
            return val.ref.plusEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL double plusEqualsFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_plusEqualsFloat(v, xsink);

         case VVT_Float:
            return val.val_float += v;

         case VVT_Ref:
            return val.ref.plusEqualsFloat<LocalVarValue>(v, this, xsink);
      }

#ifdef DEBUG
      assert(false);
#endif
      return 0.0;
   }

   DLLLOCAL int64 minusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_minusEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int -= v;

         case VVT_Ref:
            return val.ref.minusEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL double minusEqualsFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_minusEqualsFloat(v, xsink);

         case VVT_Float:
            return val.val_float += v;

         case VVT_Ref:
            return val.ref.minusEqualsFloat<LocalVarValue>(v, this, xsink);
      }

#ifdef DEBUG
      assert(false);
#endif
      return 0.0;
   }

   DLLLOCAL int64 orEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_orEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int |= v;

         case VVT_Ref:
            return val.ref.orEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 andEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_andEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int &= v;

         case VVT_Ref:
            return val.ref.andEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 modulaEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_modulaEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int %= v;

         case VVT_Ref:
            return val.ref.modulaEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 multiplyEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_multiplyEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int *= v;

         case VVT_Ref:
            return val.ref.multiplyEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 divideEqualsBigInt(int64 v, ExceptionSink *xsink) {
      assert(v);
      switch (vvt) {
         case VVT_Normal:
            return val.value_minusEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int /= v;

         case VVT_Ref:
            return val.ref.minusEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 xorEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_minusEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int ^= v;

         case VVT_Ref:
            return val.ref.minusEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_shiftLeftEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int <<= v;

         case VVT_Ref:
            return val.ref.shiftLeftEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_shiftRightEqualsBigInt(v, xsink);

         case VVT_Int:
            return val.val_int >>= v;

         case VVT_Ref:
            return val.ref.shiftRightEqualsBigInt<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 postIncrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_postIncrement(xsink);

         case VVT_Int: {
            int64 rv = val.val_int;
            ++val.val_int;
            return rv;
         }

         case VVT_Ref:
            return val.ref.postIncrement<LocalVarValue>(this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 preIncrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_preIncrement(xsink);

         case VVT_Int:
            return ++val.val_int;

         case VVT_Ref:
            return val.ref.preIncrement<LocalVarValue>(this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 postDecrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_postDecrement(xsink);

         case VVT_Int: {
            int64 rv = val.val_int;
            --val.val_int;
            return rv;
         }

         case VVT_Ref:
            return val.ref.postDecrement<LocalVarValue>(this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 preDecrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_preIncrement(xsink);

         case VVT_Int:
            return --val.val_int;

         case VVT_Ref:
            return val.ref.preIncrement<LocalVarValue>(this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value ? val.value->refSelf() : 0;
         case VVT_Int:
            return new QoreBigIntNode(val.val_int);
         case VVT_Float:
            return new QoreFloatNode(val.val_float);
         case VVT_Bool:
            return get_bool_node(val.val_bool);
         case VVT_Ref: {
            LocalRefHelper<LocalVarValue> helper(this);
            return val.ref.vexp->eval(xsink);
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         needs_deref = false;
         return val.value;
      }

      needs_deref = true;

      switch (vvt) {
         case VVT_Int:
            return new QoreBigIntNode(val.val_int);
         case VVT_Float:
            return new QoreFloatNode(val.val_float);
         case VVT_Bool:
            return get_bool_node(val.val_bool);
         case VVT_Ref: {
            LocalRefHelper<LocalVarValue> helper(this);
            return val.ref.vexp->eval(xsink);
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }
      return 0;
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value ? val.value->getAsBigInt() : 0;
         case VVT_Int:
            return val.val_int;
         case VVT_Float:
            return val.val_float;
         case VVT_Bool:
            return val.val_bool;
         case VVT_Ref: {
            ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
            return rv ? rv->getAsBigInt() : 0;
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }
      return 0;
   }

   DLLLOCAL int intEval(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value ? val.value->getAsInt() : 0;
         case VVT_Int:
            return val.val_int;
         case VVT_Float:
            return val.val_float;
         case VVT_Bool:
            return val.val_bool;
         case VVT_Ref: {
            ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
            return rv ? rv->getAsBigInt() : 0;
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }
      return 0;
   }

   DLLLOCAL bool boolEval(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value ? val.value->getAsBool() : false;
         case VVT_Int:
            return val.val_int;
         case VVT_Float:
            return val.val_float;
         case VVT_Bool:
            return val.val_bool;
         case VVT_Ref: {
            ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
            return rv ? rv->getAsBool() : false;
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }
      return false;
   }

   DLLLOCAL double floatEval(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value ? val.value->getAsFloat() : 0.0;
         case VVT_Int:
            return val.val_int;
         case VVT_Float:
            return val.val_float;
         case VVT_Bool:
            return val.val_bool;
         case VVT_Ref: {
            ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
            return rv ? rv->getAsFloat() : 0.0;
         }

#ifdef DEBUG
         default:
            assert(false);
#endif
      }
      return 0.0;
   }

   DLLLOCAL double multiplyEqualsFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal:
            return val.value_multiplyEqualsFloat(v, xsink);

         case VVT_Int:
            return val.val_int *= v;

         case VVT_Ref:
            return val.ref.multiplyEqualsFloat<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL double divideEqualsFloat(double v, ExceptionSink *xsink) {
      assert(v);

      switch (vvt) {
         case VVT_Normal:
            return val.value_divideEqualsFloat(v, xsink);

         case VVT_Int:
            return val.val_int /= v;

         case VVT_Ref:
            return val.ref.divideEqualsFloat<LocalVarValue>(v, this, xsink);
      }      

#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL bool isRef() const {
      return vvt == VVT_Ref;
   }
};

/* NOTE: the proper threading behavior of this class depends on the fact that the
   type (reference or value) can never change.  also the reference expression
   and object will also not change.
   only the value operations are locked.
*/
struct ClosureVarValue : public VarValueBase, public QoreReferenceCounter, public QoreThreadLock {
private:
   DLLLOCAL void del(ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         //printd(5, "ClosureVarValue::del() this=%p uninstantiating closure variable '%s' val=%p\n", this, id, val->val.value);
         discard(val.value, xsink);
         return;
      }
      else {
         //printd(5, "ClosureVarValue::del() this=%p uninstantiating closure variable '%s' reference expression vexp=%p\n", this, id, val->val.ref.vexp);
         val.ref.uninstantiate(xsink);
      }
   }

public:
   DLLLOCAL ClosureVarValue(const char *n_id, AbstractQoreNode *value) : VarValueBase(n_id) {
      val.value = value;
   }

   DLLLOCAL ClosureVarValue(const char *n_id, AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm) : VarValueBase(n_id, VVT_Ref) {
      val.ref.assign(vexp, obj, pgm);
   }

   DLLLOCAL void ref() { ROreference(); }

   DLLLOCAL void deref(ExceptionSink *xsink) { if (ROdereference()) { del(xsink); delete this; } }

   DLLLOCAL bool isRef() const {
      return vvt == VVT_Ref;
   }

   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         lock();

         if (checkFinalized(xsink))
            return 0;

         vl->set(this);
         return const_cast<AbstractQoreNode **>(&val.value);
      }
      assert(vvt == VVT_Ref);

      // skip this entry in case it's a recursive reference
      VarStackPointerHelper<ClosureVarValue> helper(this);

      return val.ref.getValuePtr(vl, typeInfo, omap, xsink);
   }

   DLLLOCAL void finalize(ExceptionSink *xsink) {
      SafeLocker sl(this);
      if (!finalized && vvt == VVT_Normal) {
         AbstractQoreNode *dr = val.value;
         val.value = 0;
         finalized = true;
         sl.unlock();
         discard(dr, xsink);         
      }
   }

   DLLLOCAL void setValueNormal(ReferenceHolder<AbstractQoreNode> &value_holder, ExceptionSink *xsink) {
      // make sure and dereference the old value outside the lock
      AbstractQoreNode *dr = 0;
      {
         AutoLocker al(this);

         if (checkFinalized(xsink))
            return;

         if (val.value)
            dr = val.value;
         val.value = value_holder.release();
      }
      discard(dr, xsink);
   }

   DLLLOCAL void setValueRef(ReferenceHolder<AbstractQoreNode> &value_holder, ExceptionSink *xsink) {
      LValueRefHelper<ClosureVarValue> valp(this, xsink);
      if (!valp)
         return;

      valp->assign(value_holder.release());
   }

   // value is already referenced for assignment
   DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink) {
      ReferenceHolder<AbstractQoreNode> value_holder(value, xsink);

      if (vvt == VVT_Normal) {
         setValueNormal(value_holder, xsink);
         return;
      }
      assert(vvt == VVT_Ref);

      setValueRef(value_holder, xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         AutoLocker al(this);
         return val.value ? val.value->refSelf() : 0;
      }
      assert(vvt == VVT_Ref);
      LocalRefHelper<ClosureVarValue> helper(this);
      return val.ref.vexp->eval(xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) {
      needs_deref = true;
      return eval(xsink);
   }

   DLLLOCAL int64 bigIntEval(ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         AutoLocker al(this);
         return val.value ? val.value->getAsBigInt() : 0;
      }
      assert(vvt == VVT_Ref);
         
      ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
      return rv ? rv->getAsBigInt() : 0;
   }

   DLLLOCAL int intEval(ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         AutoLocker al(this);
         return val.value ? val.value->getAsInt() : 0;
      }
      assert(vvt == VVT_Ref);

      ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
      return rv ? rv->getAsInt() : 0;
   }

   DLLLOCAL bool boolEval(ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         AutoLocker al(this);
         return val.value ? val.value->getAsBool() : false;
      }
      assert(vvt == VVT_Ref);

      ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
      return rv ? rv->getAsBool() : 0;
   }

   DLLLOCAL double floatEval(ExceptionSink *xsink) {
      if (vvt == VVT_Normal) {
         AutoLocker al(this);
         return val.value ? val.value->getAsFloat() : 0.0;
      }
      assert(vvt == VVT_Ref);

      ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
      return rv ? rv->getAsFloat() : 0.0;
   }

   DLLLOCAL int64 plusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_plusEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.plusEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL double plusEqualsFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_plusEqualsFloat(v, xsink);
         }

         case VVT_Ref:
            return val.ref.plusEqualsFloat<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0.0;
   }

   DLLLOCAL int64 minusEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_minusEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.minusEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL double minusEqualsFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_minusEqualsFloat(v, xsink);
         }

         case VVT_Ref:
            return val.ref.minusEqualsFloat<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0.0;
   }

   DLLLOCAL int64 orEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_orEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.orEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 andEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_andEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.andEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 modulaEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_modulaEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.modulaEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 multiplyEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_multiplyEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.multiplyEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 divideEqualsBigInt(int64 v, ExceptionSink *xsink) {
      assert(v);
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_divideEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.divideEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 xorEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_xorEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.xorEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_shiftLeftEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.shiftLeftEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 shiftRightEqualsBigInt(int64 v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_shiftRightEqualsBigInt(v, xsink);
         }

         case VVT_Ref:
            return val.ref.shiftRightEqualsBigInt<ClosureVarValue>(v, this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 postIncrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_postIncrement(xsink);
         }

         case VVT_Ref:
            return val.ref.postIncrement<ClosureVarValue>(this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 preIncrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_preIncrement(xsink);
         }

         case VVT_Ref:
            return val.ref.preIncrement<ClosureVarValue>(this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 postDecrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_postDecrement(xsink);
         }

         case VVT_Ref:
            return val.ref.postDecrement<ClosureVarValue>(this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL int64 preDecrement(ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_preDecrement(xsink);
         }

         case VVT_Ref:
            return val.ref.preDecrement<ClosureVarValue>(this, xsink);
      }      
#ifdef DEBUG
      assert(false);
#endif
      return 0;
   }

   DLLLOCAL double multiplyEqualsFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_multiplyEqualsFloat(v, xsink);
         }

         case VVT_Ref:
            return val.ref.multiplyEqualsFloat<ClosureVarValue>(v, this, xsink);
      }
#ifdef DEBUG
      assert(false);
#endif
      return 0.0;
   }

   DLLLOCAL double divideEqualsFloat(double v, ExceptionSink *xsink) {
      switch (vvt) {
         case VVT_Normal: {
            AutoLocker al(this);
            return val.value_divideEqualsFloat(v, xsink);
         }

         case VVT_Ref:
            return val.ref.divideEqualsFloat<ClosureVarValue>(v, this, xsink);
      }
#ifdef DEBUG
      assert(false);
#endif
      return 0.0;
   }
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

   DLLLOCAL LocalVarValue *optimized(const QoreTypeInfo *&varTypeInfo) const {
      if (closure_use)
         return 0;
      
      LocalVarValue *rv = get_var()->optimized(varTypeInfo);
      if (rv && !varTypeInfo)
         varTypeInfo = typeInfo;
      return rv;
   }

   DLLLOCAL void instantiate() const {
      //printd(0, "LocalVar::instantiate() this=%p '%s'\n", this, name.c_str());
      //instantiate(typeInfo->getDefaultValue());
      instantiate(0);
   }

   DLLLOCAL void instantiate(AbstractQoreNode *value) const {
      //printd(5, "LocalVar::instantiate(%p) this=%p '%s' value type=%s closure_use=%s pgm=%p\n", value, this, name.c_str(), get_type_name(value), closure_use ? "true" : "false", getProgram());

      if (!closure_use) {
         LocalVarValue *val = thread_instantiate_lvar();
         val->set(name.c_str(), typeInfo, value);
      }
      else
         thread_instantiate_closure_var(name.c_str(), value);
   }

   DLLLOCAL void instantiate_object(QoreObject *value) const {
      //printd(5, "LocalVar::instantiate_object(%p) this=%p '%s'\n", value, this, name.c_str());
      instantiate(value);
      value->ref();
   }

   DLLLOCAL void instantiate(AbstractQoreNode *vexp, QoreObject *obj, QoreProgram *pgm) {
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

   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&n_typeInfo, ObjMap &omap, ExceptionSink *xsink) const {
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

   // value is already referenced for assignment
   DLLLOCAL void setValue(AbstractQoreNode *value, ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         val->setValue(value, xsink);
         return;
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      val->setValue(value, xsink);
   }

   DLLLOCAL void assignBigInt(int64 v, ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         val->assignBigInt(v, xsink);
         return;
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      val->setValue(new QoreBigIntNode(v), xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(ExceptionSink *xsink) const {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->eval(xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->eval(xsink);
   }

   DLLLOCAL AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) const {
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
      return !closure_use ? get_var()->postIncrement(xsink) : thread_find_closure_var(name.c_str())->postIncrement(xsink);
   }

   DLLLOCAL int64 preIncrement(ExceptionSink *xsink) {
      return !closure_use ? get_var()->preIncrement(xsink) : thread_find_closure_var(name.c_str())->preIncrement(xsink);
   }

   DLLLOCAL int64 postDecrement(ExceptionSink *xsink) {
      return !closure_use ? get_var()->postDecrement(xsink) : thread_find_closure_var(name.c_str())->postDecrement(xsink);
   }

   DLLLOCAL int64 preDecrement(ExceptionSink *xsink) {
      return !closure_use ? get_var()->preDecrement(xsink) : thread_find_closure_var(name.c_str())->preDecrement(xsink);
   }

   DLLLOCAL double multiplyEqualsFloat(double v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->multiplyEqualsFloat(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->multiplyEqualsFloat(v, xsink);      
   }

   DLLLOCAL double divideEqualsFloat(double v, ExceptionSink *xsink) {
      if (!closure_use) {
         LocalVarValue *val = get_var();
         return val->divideEqualsFloat(v, xsink);
      }

      ClosureVarValue *val = thread_find_closure_var(name.c_str());
      return val->divideEqualsFloat(v, xsink);      
   }

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

   DLLLOCAL const QoreTypeInfo *getTypeInfo() const {
      return typeInfo;
   }

   DLLLOCAL bool needsValueInstantiation() const {
      return needs_value_instantiation;
   }

   DLLLOCAL void unsetNeedsValueInstantiation() {
      if (needs_value_instantiation)
         needs_value_instantiation = false;
   }

   DLLLOCAL bool needsAssignmentAtInstantiation() const {
      return needs_value_instantiation && !typeInfo->hasDefaultValue() ? true : false;
   }
};

typedef LocalVar *lvar_ptr_t;

#endif
