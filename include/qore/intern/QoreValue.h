/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreValue.h

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

#ifndef _QORE_QOREVALUE_H

#define _QORE_QOREVALUE_H

/*
class AbstractLValue {
public:
   DLLLOCAL virtual ~AbstractLValue() {
   }

   DLLLOCAL virtual AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&n_typeInfo, ObjMap &omap, ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual AbstractQoreNode **getContainerValuePtr(AutoVLock *vl, const QoreTypeInfo *&n_typeInfo, ObjMap &omap, ExceptionSink* xsink) const = 0;

   DLLLOCAL virtual void assign(AbstractQoreNode *value, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual void assignBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual void assignFloat(double v, ExceptionSink* xsink) = 0;

   DLLLOCAL virtual int64 plusEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual double plusEqualsFloat(double v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 minusEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual double minusEqualsFloat(double v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 orEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 andEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 modulaEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 multiplyEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 divideEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 xorEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 shiftLeftEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 shiftRightEqualsBigInt(int64 v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual double multiplyEqualsFloat(double v, ExceptionSink* xsink) = 0;
   DLLLOCAL virtual double divideEqualsFloat(double v, ExceptionSink* xsink) = 0;

   DLLLOCAL virtual int64 postIncrement(ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 preIncrement(ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 postDecrement(ExceptionSink* xsink) = 0;
   DLLLOCAL virtual int64 preDecrement(ExceptionSink* xsink) = 0;

   DLLLOCAL virtual AbstractQoreNode* eval(ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual AbstractQoreNode* eval(bool &needs_deref, ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual int64 bigIntEval(ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual double floatEval(ExceptionSink* xsink) const = 0;

   DLLLOCAL virtual int64 removeBigInt(ExceptionSink* xsink) = 0;
   DLLLOCAL virtual double removeFloat(ExceptionSink* xsink) = 0;
   DLLLOCAL virtual AbstractQoreNode* remove(ExceptionSink* xsink, bool for_del) = 0;
};
*/

typedef unsigned char valtype_t;

#define QV_Bool  (valtype_t)0
#define QV_Int   (valtype_t)1
#define QV_Float (valtype_t)2
#define QV_Node  (valtype_t)3
#define QV_Ref   (valtype_t)4

union qore_value_u {
   bool b;
   int64 i;
   double f;
   AbstractQoreNode* n;
};

template <typename U>
class QoreValue {
protected:
   // returns the old value just in case it needs to be dereferenced outside a lock
   template <class T, typename t, int nt>
   DLLLOCAL T* ensureUnique(AbstractQoreNode*& old) {
      assert(type == QV_Node);
      assert(!old);

      if (!v.n)
         return reinterpret_cast<T*>(v.n = new T);

      if (v.n->getType() != nt) {
         t i = T::getValue(v.n);
	 old = v.n;
         return reinterpret_cast<T*>(v.n = new T(i));
      }

      if (v.n->is_unique())
         return reinterpret_cast<T*>(v.n);

      old = v.n;
      return reinterpret_cast<T*>((v.n = old->realCopy()));
   }

   DLLLOCAL void reset() {
      switch (type) {
         case QV_Bool: v.b = false; break;
         case QV_Int: v.i = 0; break;
         case QV_Float: v.f = 0.0; break;
         case QV_Node:
	    v.n = 0; break;
	 case QV_Ref:
	    break;
         default: assert(false);
      }
   }

public:
   U v;
   valtype_t type;

   DLLLOCAL QoreValue(valtype_t t) : type(t) {
      reset();
   }

   DLLLOCAL QoreValue(const QoreTypeInfo* typeInfo) {
      set(typeInfo);
   }

   DLLLOCAL void set(const QoreTypeInfo* typeInfo) {
      if (typeInfo == bigIntTypeInfo || typeInfo == softBigIntTypeInfo)
         set(QV_Int);
      else if (typeInfo == floatTypeInfo || typeInfo == softFloatTypeInfo)
         set(QV_Float);
      else if (typeInfo == boolTypeInfo || typeInfo == softBoolTypeInfo)
         set(QV_Bool);
      else
         set(QV_Node);
   }

   DLLLOCAL void set(valtype_t t) {
      type = t;
      reset();
   }

   DLLLOCAL bool optimized() const {
      return type != QV_Node && type != QV_Ref;
   }

   DLLLOCAL AbstractQoreNode* assign(bool b) {
      switch (type) {
         case QV_Bool: v.b = false; return 0;
         case QV_Int: v.i = (int64)b; return 0;
         case QV_Float: v.f = (double)b; return 0;
         case QV_Node: {
	    AbstractQoreNode* rv = v.n;
	    v.n = get_bool_node(b);
	    return rv;
	 }
         default: assert(false);
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* assign(int64 i) {
      switch (type) {
         case QV_Bool: v.b = (bool)i; return 0;
         case QV_Int: v.i = i; return 0;
         case QV_Float: v.f = (double)i; return 0;
         case QV_Node: {
	    AbstractQoreNode* rv = v.n;
	    v.n = new QoreBigIntNode(i);
	    return rv;
	 }
         default: assert(false);
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* assign(double f) {
      switch (type) {
         case QV_Bool: v.b = (bool)f; return 0;
         case QV_Int: v.i = (int64)f; return 0;
         case QV_Float: v.f = f; return 0;
         case QV_Node: {
	    AbstractQoreNode* rv = v.n;
	    v.n = new QoreFloatNode(f);
	    return rv;
	 }
         default: assert(false);
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* assign(AbstractQoreNode* n) {
      switch (type) {
         case QV_Bool: {
	    if (n) {
	       v.b = n->getAsBool();
	       return n;
	    }
	    v.b = false;
	    return 0;
	 }
         case QV_Int: {
	    if (n) {
	       v.i = n->getAsBigInt();
	       return n;
	    }
	    v.i = 0;
	    return 0;
	 }
         case QV_Float: {
	    if (n) {
	       v.f = n->getAsFloat();
	       return n;
	    }
	    v.f = 0.0;
	    return 0;
	 }
         case QV_Node: {
	    AbstractQoreNode* rv = v.n;
	    v.n = n;
	    return rv;
	 }
         default:
	    assert(false);
      }
      return 0;
   }

   DLLLOCAL int64 getAsBool() const {
      switch (type) {
         case QV_Bool: return v.b;
         case QV_Int: return (bool)v.i;
         case QV_Float: return (bool)v.f;
         case QV_Node: return v.n ? v.n->getAsBool() : false;
         default: assert(false);
      }
      return false;
   }

   DLLLOCAL int64 getAsBigInt() const {
      switch (type) {
         case QV_Bool: return (int64)v.b;
         case QV_Int: return v.i;
         case QV_Float: return (int64)v.f;
         case QV_Node: return v.n ? v.n->getAsBigInt() : 0;
         default: assert(false);
      }
      return 0;
   }

   DLLLOCAL double getAsFloat() const {
      switch (type) {
         case QV_Bool: return (double)v.b;
         case QV_Int: return (double)v.i;
         case QV_Float: return v.f;
         case QV_Node: return v.n ? v.n->getAsFloat() : 0.0;
         default: assert(false);
      }
      return 0.0;
   }

   DLLLOCAL AbstractQoreNode* eval() const {
      switch (type) {
         case QV_Bool: return get_bool_node(v.b);
         case QV_Int: return new QoreBigIntNode(v.i);
         case QV_Float: return new QoreFloatNode(v.f);
         case QV_Node: return v.n ? v.n->refSelf() : 0;
         default: assert(false);
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode *eval(bool &needs_deref) const {
      if (type == QV_Node) {
         needs_deref = false;
	 return v.n;
      }
      needs_deref = true;

      switch (type) {
         case QV_Bool: return get_bool_node(v.b);
         case QV_Int: return new QoreBigIntNode(v.i);
         case QV_Float: return new QoreFloatNode(v.f);
         default: assert(false);
      }
      return 0;
   }

   DLLLOCAL qore_type_t getType() const {
      switch (type) {
         case QV_Bool: return NT_BOOLEAN;
         case QV_Int: return NT_INT;
         case QV_Float: return NT_FLOAT;
         case QV_Node: return v.n ? v.n->getType() : 0;
         default: assert(false);
      }
      return NT_NOTHING;      
   }

   DLLLOCAL const char* getTypeName() const {
      switch (type) {
	 case QV_Bool: return QoreBoolNode::getStaticTypeName();
	 case QV_Int: return QoreBigIntNode::getStaticTypeName();
	 case QV_Float: return QoreFloatNode::getStaticTypeName();
	 case QV_Node: return get_type_name(v.n);
         default: assert(false);
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode **getValuePtr(ExceptionSink* xsink) const {
      if (type == QV_Node)
         return (AbstractQoreNode**)&v.n;

      assert(false);
      xsink->raiseException("DEPRECATED-API", "this module uses a deprecated API that is no longer supported; please either update the module with a newer version that uses newer APIs that work with lvalues/references in newer versions of Qore or contact the developer to update the module");
      return 0;
   }

   DLLLOCAL AbstractQoreNode **getContainerValuePtr() const {
      return type == QV_Node ? (AbstractQoreNode**)&v.n : 0;
   }

   // lvalue operations
   DLLLOCAL int64 plusEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val += i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i += i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL double plusEqualsFloat(double f, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreFloatNode *vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f += f;
	    return vv->f;
	 }

	 case QV_Float:
	    return v.f += f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0.0;
   }

   DLLLOCAL int64 minusEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val -= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i -= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL double minusEqualsFloat(double f, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreFloatNode *vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f -= f;
	    return vv->f;
	 }

	 case QV_Float:
	    return v.f -= f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0.0;
   }

   DLLLOCAL int64 orEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val |= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i |= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 andEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val &= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i &= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 modulaEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    return i ? vv->val %= i : vv->val = 0;
	 }

	 case QV_Int:
	    return i ? v.i %= i : v.i = 0;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 multiplyEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val *= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i *= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 divideEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(i);

      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val /= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i /= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 xorEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val ^= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i ^= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val <<= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i <<= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 shiftRightEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val >>= i;
	    return vv->val;
	 }

	 case QV_Int:
	    return v.i >>= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }

      return 0;
   }

   DLLLOCAL int64 postIncrement(AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    int64 rv = vv->val;
	    ++vv->val;
	    return rv;
	 }

	 case QV_Float: {
	    int64 rv = v.f;
	    ++v.f;
	    return rv;
	 }

	 case QV_Int: {
	    int64 rv = v.i;
	    ++v.i;
	    return rv;
	 }

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }      

      return 0;
   }

   DLLLOCAL int64 preIncrement(AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    return ++vv->val;
	 }

	 case QV_Float:
	    return ++v.f;

	 case QV_Int:
	    return ++v.i;
	    
	 // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }      

      return 0;
   }

   DLLLOCAL int64 postDecrement(AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    int64 rv = vv->val;
	    --vv->val;
	    return rv;
	 }

	 case QV_Float: {
	    int64 rv = v.f;
	    --v.f;
	    return rv;
	 }

	 case QV_Int: {
	    int64 rv = v.i;
	    --v.i;
	    return rv;
	 }

	 // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }      

      return 0;
   }

   DLLLOCAL int64 preDecrement(AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreBigIntNode *vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    return --vv->val;
	 }

	 case QV_Float:
	    return --v.f;

	 case QV_Int:
	    return --v.i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }      
      return 0;
   }

   DLLLOCAL double multiplyEqualsFloat(double f, AbstractQoreNode*& old) {
      switch (type) {
	 case QV_Node: {
	    QoreFloatNode *vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f *= f;
	    return vv->f;
	 }

	 case QV_Float:
	    return v.f *= f;

	 case QV_Int:
	    return v.i *= (int64)f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }      

      return 0;
   }

   DLLLOCAL double divideEqualsFloat(double f, AbstractQoreNode*& old) {
      assert(f);

      switch (type) {
	 case QV_Node: {
	    QoreFloatNode *vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f /= f;
	    return vv->f;
	 }

	 case QV_Float:
	    return v.f /= f;

	 case QV_Int:
	    return v.i /= (int64)f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
      }      

      return 0;
   }

   DLLLOCAL int64 removeBigInt(AbstractQoreNode*& old) {
      assert(!old);

      switch (type) {
         case QV_Bool: {
            int64 rv = (int64)v.b;
            v.b = false;
            return rv;
         }
         case QV_Int: {
            int64 rv = v.i;
            v.i = 0;
            return rv;
         }
         case QV_Float: {
            int64 rv = (int64)v.f;
            v.f = 0.0;
            return rv;
         }
         case QV_Node: {
            int64 rv;
            if (v.n) {
               rv = v.n->getAsBigInt();
	       old = v.n;
               v.n = 0;
            }
            else
               rv = 0;
            return rv;
         }
         default:
            assert(false);
      }
      return 0;
   }

   DLLLOCAL double removeFloat(AbstractQoreNode*& old) {
      assert(!old);
      switch (type) {
         case QV_Bool: {
            double rv = (double)v.b;
            v.b = false;
            return rv;
         }
         case QV_Int: {
            double rv = (double)v.i;
            v.i = 0;
            return rv;
         }
         case QV_Float: {
            double rv = v.f;
            v.f = 0.0;
            return rv;
         }
         case QV_Node: {
            double rv;
            if (v.n) {
               rv = v.n->getAsFloat();
	       old = v.n;
               v.n = 0;
            }
            else
               rv = 0.0;
            return rv;
         }
         default:
            assert(false);
      }
      return 0.0;
   }

   DLLLOCAL AbstractQoreNode* remove(bool for_del) {
      switch (type) {
         case QV_Bool: {
            bool rv = v.b;
            v.b = false;
            return for_del ? 0 : get_bool_node(rv);
         }
         case QV_Int: {
            int64 rv = v.i;
            v.i = 0;
            return for_del ? 0 : new QoreBigIntNode(rv);
         }
         case QV_Float: {
            double rv = v.f;
            v.f = 0.0;
            return for_del ? 0 : new QoreFloatNode(rv);
         }
         case QV_Node: {
            AbstractQoreNode* rv = v.n;
            v.n = 0;
            return rv;
         }
         default:
            assert(false);
      }
      return 0;
   }
};

#endif

