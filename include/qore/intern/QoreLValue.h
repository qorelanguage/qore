/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreLValue.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#ifndef _QORE_INTERN_QORELVALUE_H

#define _QORE_INTERN_QORELVALUE_H

template <typename U = qore_value_u>
class QoreLValue {
protected:
   // returns the old value just in case it needs to be dereferenced outside a lock
   template <class T, typename t, int nt>
   DLLLOCAL T* ensureUnique(AbstractQoreNode*& old) {
      assert(type == QV_Node);
      assert(assigned);
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
      assert(!assigned || type != QV_Node || !v.n);
      if (assigned) {
         assigned = false;
         assert(!static_assignment);
      }
   }

public:
   U v;
   valtype_t type : 4;
   bool fixed_type : 1;
   bool assigned : 1;

   // true if the assigned value was not referenced for the assignment
   // and therefore should not be dereferenced
   bool static_assignment : 1;

   DLLLOCAL QoreLValue() : type(QV_Node), fixed_type(false), assigned(false), static_assignment(false) {
#ifdef DEBUG
      v.n = 0;
#endif
      reset();
   }

   DLLLOCAL QoreLValue(valtype_t t) : type(t), fixed_type(t != QV_Node), assigned(false), static_assignment(false) {
#ifdef DEBUG
      if (t == QV_Node)
         v.n = 0;
#endif
      reset();
   }

   // fixed_type is assigned in set()
   DLLLOCAL QoreLValue(const QoreTypeInfo* typeInfo) : assigned(false), static_assignment(false) {
#ifdef DEBUG
      type = QV_Bool;
#endif
      set(typeInfo);
   }

   DLLLOCAL QoreLValue(const QoreLValue<U>& old) : type(old.type), fixed_type(old.fixed_type), assigned(old.assigned), static_assignment(false) {
      if (!assigned)
         return;
      switch (old.type) {
         case QV_Bool: v.b = old.v.b; break;
         case QV_Int: v.i = old.v.i; break;
         case QV_Float: v.f = old.v.f; break;
         case QV_Node: v.n = old.v.n ? old.v.n->refSelf() : 0; break;
         default: assert(false);
         // no break
      }
   }

#ifdef DEBUG
   DLLLOCAL ~QoreLValue() {
      assert(!assigned || type != QV_Node || !v.n);
   }
#endif

   DLLLOCAL valtype_t getOptimizedType() const {
      return fixed_type ? type : QV_Node;
   }

   DLLLOCAL bool optimized() const {
      return type != QV_Node && type != QV_Ref;
   }

   DLLLOCAL const char* getFixedTypeName() const {
      if (!fixed_type)
         return "any";
      switch (type) {
         case QV_Int: return "int";
         case QV_Float: return "float";
         case QV_Bool: return "bool";
         default:
            assert(false);
      }
   }

   DLLLOCAL bool isInt() {
      if (!assigned) {
         if (type != QV_Int) {
            if (fixed_type)
               return false;
         }
         else
            type = QV_Int;
         assigned = true;
         v.i = 0;
         return true;
      }
      if (type == QV_Int)
         return true;
      if (type == QV_Node && v.n && v.n->getType() == NT_INT) {
         int64 i = reinterpret_cast<QoreBigIntNode*>(v.n)->val;
         v.n->deref(0);
         v.i = i;
         type = QV_Int;
         return true;
      }
      return false;
   }

   DLLLOCAL bool isFloat() {
      if (!assigned) {
         if (type != QV_Float) {
            if (fixed_type)
               return false;
         }
         else
            type = QV_Float;
         assigned = true;
         v.f = 0.0;
         return true;
      }
      if (type == QV_Float)
         return true;
      if (type == QV_Node && v.n && v.n->getType() == NT_FLOAT) {
         double f = reinterpret_cast<QoreFloatNode*>(v.n)->f;
         v.n->deref(0);
         v.f = f;
         type = QV_Float;
         return true;
      }
      return false;
   }

   DLLLOCAL AbstractQoreNode* makeInt() {
      assert(!fixed_type || type == QV_Int);
      if (!assigned) {
         assigned = true;
         if (!type)
            type = QV_Int;
         v.i = 0;
         return 0;
      }
      switch (type) {
         case QV_Float: {
            int64 i = (int64)v.f;
            v.i = i;
            type = QV_Int;
            return 0;
         }

         case QV_Bool: {
            int64 i = (int64)v.b;
            v.i = i;
            type = QV_Int;
            return 0;
         }

         case QV_Node: {
            int64 i = v.n ? v.n->getAsBigInt() : 0;
            AbstractQoreNode* rv = v.n;
            v.i = i;
            type = QV_Int;
            return rv;
         }

         case QV_Int:
            break;

         default:
            assert(false);
      }

      return 0;
   }

   DLLLOCAL AbstractQoreNode* makeFloat() {
      assert(!fixed_type || type == QV_Float);
      if (!assigned) {
         assigned = true;
         if (!type)
            type = QV_Float;
         v.f = 0.0;
         return 0;
      }
      switch (type) {
         case QV_Int: {
            double f = v.i;
            v.f = f;
            type = QV_Float;
            return 0;
         }

         case QV_Bool: {
            double f = v.b;
            v.f = f;
            type = QV_Float;
            return 0;
         }

         case QV_Node: {
            double f = v.n ? v.n->getAsFloat() : 0.0;
            AbstractQoreNode* rv = v.n;
            v.f = f;
            type = QV_Float;
            return rv;
         }

         case QV_Float:
            break;

         default:
            assert(false);
      }

      return 0;
   }

   DLLLOCAL AbstractQoreNode* makeNumber() {
      assert(!fixed_type);
      if (!assigned) {
         assigned = true;
         if (!type)
            type = QV_Node;
         v.n = new QoreNumberNode;
         return 0;
      }
      switch (type) {
         case QV_Int: {
            QoreNumberNode* n = new QoreNumberNode(v.i);
            v.n = n;
            type = QV_Node;
            return 0;
         }

         case QV_Float: {
            QoreNumberNode* n = new QoreNumberNode(v.f);
            v.n = n;
            type = QV_Node;
            return 0;
         }

         case QV_Bool: {
            QoreNumberNode* n = new QoreNumberNode((int64)v.b);
            v.n = n;
            type = QV_Node;
            return 0;
         }

         case QV_Node: {
            if (v.n && v.n->getType() == NT_NUMBER)
               return 0;
            QoreNumberNode* n = new QoreNumberNode(v.n);
            AbstractQoreNode* rv = v.n;
            v.n = n;
            type = QV_Node;
            return rv;
         }

         default:
            assert(false);
      }

      return 0;
   }

   DLLLOCAL QoreValue takeValue() {
      if (!assigned)
         return QoreValue();

      assigned = false;
      assert(!static_assignment);

      switch (type) {
         case QV_Bool: return QoreValue(v.b);
         case QV_Int: return QoreValue(v.i);
         case QV_Float: return QoreValue(v.f);
         case QV_Node: return QoreValue(v.n);
         default: assert(false);
         // no break
      }
      return false;
   }

   DLLLOCAL QoreValue copyValue() const {
      if (!assigned)
         return QoreValue();

      switch (type) {
         case QV_Bool: return QoreValue(v.b);
         case QV_Int: return QoreValue(v.i);
         case QV_Float: return QoreValue(v.f);
         case QV_Node: return v.n ? QoreValue(v.n) : QoreValue();
         default: assert(false);
         // no break
      }
      return false;
   }

/*
   DLLLOCAL QoreValue eval(ExceptionSink* xsink) const {
      if (!assigned)
         return QoreValue();

      switch (type) {
         case QV_Bool: return QoreValue(v.b);
         case QV_Int: return QoreValue(v.i);
         case QV_Float: return QoreValue(v.f);
         case QV_Node: return v.n ? QoreValue(v.n->eval(xsink)) : QoreValue();
         default: assert(false);
         // no break
      }
      return false;
   }
*/

   DLLLOCAL QoreValue getReferencedValue() const {
      if (!assigned)
         return QoreValue();

      switch (type) {
         case QV_Bool: return QoreValue(v.b);
         case QV_Int: return QoreValue(v.i);
         case QV_Float: return QoreValue(v.f);
         case QV_Node: return QoreValue(v.n ? v.n->refSelf() : 0);
         default: assert(false);
            // no break
      }
      return QoreValue();
   }

   // FIXME: eliminate
   DLLLOCAL AbstractQoreNode* getReferencedNodeValue() const {
      if (!assigned)
         return 0;

      switch (type) {
         case QV_Bool: return get_bool_node(v.b);
         case QV_Int: return new QoreBigIntNode(v.i);
         case QV_Float: return new QoreFloatNode(v.f);
         case QV_Node: return v.n ? v.n->refSelf() : 0;
         default: assert(false);
            // no break
      }
      return 0;
   }

   DLLLOCAL bool isContainer() const {
      if (!assigned || type != QV_Node || !v.n)
         return false;

      return is_container(v.n);
   }

   DLLLOCAL bool isObjectContainer() const {
      if (!assigned || type != QV_Node || !v.n)
         return false;

      return get_container_obj(v.n);
   }

   DLLLOCAL AbstractQoreNode* getReferencedContainerValue() const {
      if (!assigned)
         return 0;

      if (type != QV_Node || !v.n)
         return 0;

      return v.n->refSelf();
   }

   // never call when a lock is held
   DLLLOCAL QoreValue getReferencedValue(bool& needs_deref) const {
      if (!assigned) {
         needs_deref = false;
         return QoreValue();
      }

      if (type == QV_Node) {
         needs_deref = false;
         return QoreValue(v.n);
      }

      needs_deref = false;

      switch (type) {
         case QV_Bool: return QoreValue(v.b);
         case QV_Int: return QoreValue(v.i);
         case QV_Float: return QoreValue(v.f);
         default: assert(false);
         // no break
      }
      return QoreValue();
   }

   /*
   DLLLOCAL AbstractQoreNode* getReferencedNodeValue(bool& needs_deref, bool in_lock = false) const {
      if (!assigned) {
         needs_deref = false;
         return 0;
      }

      if (type == QV_Node) {
         if (!in_lock || !v.n) {
            needs_deref = false;
            return v.n;
         }
         needs_deref = true;
         return v.n->refSelf();
      }
      needs_deref = true;

      switch (type) {
         case QV_Bool: return get_bool_node(v.b);
         case QV_Int: return new QoreBigIntNode(v.i);
         case QV_Float: return new QoreFloatNode(v.f);
         default: assert(false);
         // no break
      }
      return 0;
   }
   */

   DLLLOCAL void discard(ExceptionSink* xsink) {
      if (!assigned)
         return;

      if (type == QV_Node && v.n)
         v.n->deref(xsink);
   }

#ifdef DEBUG
   DLLLOCAL bool hasValue() const {
      if (!assigned)
         return false;
      switch (type) {
         case QV_Bool: return v.b;
         case QV_Int: return (bool)v.i;
         case QV_Float: return (bool)v.f;
         case QV_Node: return (bool)v.n;
         default: assert(false);
         // no break
      }
      return false;
   }
#endif

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
      assert(!assigned || type != QV_Node || !v.n);
      type = t;
      fixed_type = (t != QV_Node);
#ifdef DEBUG
      if (t == QV_Node)
         v.n = 0;
#endif
      reset();
   }

   // NOTE: destructive for "val":
   DLLLOCAL AbstractQoreNode* assignAssume(QoreValue& val) {
      if (fixed_type) {
         if (!assigned)
            assigned = true;
         switch (type) {
            case QV_Bool: v.b = val.getAsBool(); break;
            case QV_Int: v.i = val.getAsBigInt(); break;
            case QV_Float: v.f = val.getAsFloat(); break;
            default: assert(false);
               // no break
         }
         return val.takeIfNode();
      }

      AbstractQoreNode* rv;
      if (assigned)
         rv = (type == QV_Node) ? v.n : 0;
      else {
         assigned = true;
         rv = 0;
      }

      switch (val.type) {
         case QV_Bool: v.b = val.v.b; if (type != QV_Bool) type = QV_Bool; break;
         case QV_Int: v.i = val.v.i; if (type != QV_Int) type = QV_Int; break;
         case QV_Float: v.f = val.v.f; if (type != QV_Float) type = QV_Float; break;
         case QV_Node: v.n = val.takeNode(); if (type != QV_Node) type = QV_Node; break;
         default: assert(false);
            // no break
      }

      return rv;
   }

   DLLLOCAL AbstractQoreNode* assign(bool b) {
      if (fixed_type) {
         if (!assigned)
            assigned = true;
         switch (type) {
            case QV_Bool: v.b = false; return 0;
            case QV_Int: v.i = (int64)b; return 0;
            case QV_Float: v.f = (double)b; return 0;
            default: assert(false);
               // no break
         }
      }

      AbstractQoreNode* rv;
      if (assigned) {
         if (type == QV_Node)
            rv = v.n;
         else
            rv = 0;
      }
      else {
         assigned = true;
         rv = 0;
      }

      v.b = b;
      type = QV_Bool;

      return rv;
   }

   DLLLOCAL AbstractQoreNode* assign(int64 i) {
      if (fixed_type) {
         if (!assigned)
            assigned = true;
         switch (type) {
            case QV_Bool: v.b = (bool)i; return 0;
            case QV_Int: v.i = i; return 0;
            case QV_Float: v.f = (double)i; return 0;
            default: assert(false);
               // no break
         }
      }

      AbstractQoreNode* rv;
      if (assigned) {
         if (type == QV_Node)
            rv = v.n;
         else
            rv = 0;
      }
      else {
         assigned = true;
         rv = 0;
      }

      v.i = i;
      type = QV_Int;

      return rv;
   }

   DLLLOCAL AbstractQoreNode* assign(double f) {
      if (fixed_type) {
         if (!assigned)
            assigned = true;
         switch (type) {
            case QV_Bool: v.b = (bool)f; return 0;
            case QV_Int: v.i = (int64)f; return 0;
            case QV_Float: v.f = f; return 0;
            default: assert(false);
               // no break
         }
      }

      AbstractQoreNode* rv;
      if (assigned) {
         if (type == QV_Node)
            rv = v.n;
         else
            rv = 0;
      }
      else {
         assigned = true;
         rv = 0;
      }

      v.f = f;
      type = QV_Float;

      return rv;
   }

#ifdef QORE_ENFORCE_DEFAULT_LVALUE
   DLLLOCAL void assignSetInitialSwap(QoreLValue<U>& n, QoreValue& val) {
      assert(!assigned);
      assigned = true;
      type = n.type;
      if (n.static_assignment) {
         static_assignment = n.static_assignment;
         n.static_assignment = false;
      }
      switch (n.type) {
         case QV_Bool: v.b = n.v.b; n.v.b = val.v.b; break;
         case QV_Int: v.i = n.v.i; n.v.i = val.v.i; break;
         case QV_Float: v.f = n.v.f; n.v.f = val.v.f; break;
         case QV_Node: v.n = n.v.n; n.v.n = val.takeNode(); break;
         default: assert(false);
         // no break
      }
   }
#else
   DLLLOCAL void assignSetTakeInitial(QoreLValue<U>& n) {
      assert(!assigned);
      if (!n.assigned)
         return;
      assigned = true;
      type = n.type;
      if (n.static_assignment) {
         static_assignment = n.static_assignment;
         n.static_assignment = false;
      }
      switch (n.type) {
         case QV_Bool: v.b = n.v.b; n.v.b = false; break;
         case QV_Int: v.i = n.v.i; n.v.i = 0; break;
         case QV_Float: v.f = n.v.f; n.v.f = 0; break;
         case QV_Node: v.n = n.v.n; n.v.n = 0; break;
         default: assert(false);
         // no break
      }
      n.assigned = false;
   }
#endif

   // note: destructive for "n"
   // returns any AbstractQoreNode no longer needed (because it was converted to a base type)
   DLLLOCAL AbstractQoreNode* assignAssumeInitial(QoreValue& n, bool is_static_assignment = false) {
      assert(!assigned);
      assert(!static_assignment);
      //printd(5, "QoreLValue::assignAssumeInitial() this: %p n: %s sa: %d\n", this, n.getTypeName(), is_static_assignment);
      if (is_static_assignment)
         static_assignment = true;
      return assignAssume(n);
   }

   DLLLOCAL void assignInitial(bool n) {
      assert(!assigned);
      assigned = true;
      if (fixed_type) {
         switch (type) {
            case QV_Bool: v.b = n; return;
            case QV_Int: v.i = (int64)n; return;
            case QV_Float: v.f = (double)n; return;
            default: assert(false);
               // no break
         }
      }
      type = QV_Bool;
      v.b = n;
   }

   DLLLOCAL void assignInitial(int64 n) {
      assert(!assigned);
      assigned = true;
      if (fixed_type) {
         switch (type) {
            case QV_Bool: v.b = (bool)n; return;
            case QV_Int: v.i = n; return;
            case QV_Float: v.f = (double)n; return;
            default: assert(false);
               // no break
         }
      }
      type = QV_Int;
      v.i = n;
   }

   DLLLOCAL void assignInitial(double n) {
      assert(!assigned);
      assigned = true;
      if (fixed_type) {
         switch (type) {
            case QV_Bool: v.b = (bool)n; return;
            case QV_Int: v.i = (int64)n; return;
            case QV_Float: v.f = n; return;
            default: assert(false);
               // no break
         }
      }
      type = QV_Float;
      v.f = n;
   }

   DLLLOCAL void assignInitial(AbstractQoreNode* n) {
      assert(!assigned);
      assigned = true;
      if (fixed_type) {
         switch (type) {
            case QV_Bool: v.b = n ? n->getAsBool() : false; return;
            case QV_Int: v.i = n ? n->getAsBigInt() : 0; return;
            case QV_Float: v.f = n ? n->getAsFloat() : 0; return;
            default: assert(false);
               // no break
         }
      }
      type = QV_Node;
      v.n = n;
   }

   DLLLOCAL AbstractQoreNode* assign(AbstractQoreNode* n) {
      if (fixed_type) {
         if (!assigned)
            assigned = true;

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
               // no break
         }
         return 0;
      }

      AbstractQoreNode* rv;
      if (assigned) {
         if (type == QV_Node)
            rv = v.n;
         else
            rv = 0;
      }
      else {
         assigned = true;
         rv = 0;
      }

      qore_type_t nt = get_node_type(n);
      switch (nt) {
         case NT_BOOLEAN: v.b = reinterpret_cast<QoreBoolNode*>(n)->getValue(); if (type != QV_Bool) type = QV_Bool; n->deref(0); break;
         case NT_INT: v.b = reinterpret_cast<QoreBigIntNode*>(n)->val; if (type != QV_Int) type = QV_Int; n->deref(0); break;
         case NT_FLOAT: v.b = reinterpret_cast<QoreFloatNode*>(n)->f; if (type != QV_Float) type = QV_Float; n->deref(0); break;
         default: {
            v.n = n;
            if (type != QV_Node)
               type = QV_Node;
            break;
         }
      }

      return rv;
   }

   DLLLOCAL bool exists() const {
      return assigned && (type != QV_Node || !is_nothing(v.n));
   }

   DLLLOCAL bool getAsBool() const {
      if (assigned) {
         switch (type) {
            case QV_Bool: return v.b;
            case QV_Int: return (bool)v.i;
            case QV_Float: return (bool)v.f;
            case QV_Node: return v.n ? v.n->getAsBool() : false;
            default: assert(false);
               // no break
         }
      }
      return false;
   }

   DLLLOCAL int64 getAsBigInt() const {
      if (assigned) {
         switch (type) {
            case QV_Bool: return (int64)v.b;
            case QV_Int: return v.i;
            case QV_Float: return (int64)v.f;
            case QV_Node: return v.n ? v.n->getAsBigInt() : 0;
            default: assert(false);
               // no break
         }
      }
      return 0;
   }

   DLLLOCAL double getAsFloat() const {
      if (assigned) {
         switch (type) {
            case QV_Bool: return (double)v.b;
            case QV_Int: return (double)v.i;
            case QV_Float: return v.f;
            case QV_Node: return v.n ? v.n->getAsFloat() : 0.0;
            default: assert(false);
               // no break
         }
      }
      return 0.0;
   }

   DLLLOCAL AbstractQoreNode* getInternalNode() const {
      return assigned && type == QV_Node ? v.n : 0;
   }

   /*
   DLLLOCAL AbstractQoreNode* eval() const {
      if (assigned) {
         switch (type) {
            case QV_Bool: return get_bool_node(v.b);
            case QV_Int: return new QoreBigIntNode(v.i);
            case QV_Float: return new QoreFloatNode(v.f);
            case QV_Node: return v.n ? v.n->refSelf() : 0;
            default: assert(false);
               // no break
         }
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* eval(bool &needs_deref, bool in_lock = false) const {
      if (!assigned) {
         needs_deref = false;
         return 0;
      }

      if (type == QV_Node) {
         if (!in_lock || !v.n) {
            needs_deref = false;
            return v.n;
         }
         needs_deref = true;
         return v.n->refSelf();
      }
      needs_deref = true;

      switch (type) {
         case QV_Bool: return get_bool_node(v.b);
         case QV_Int: return new QoreBigIntNode(v.i);
         case QV_Float: return new QoreFloatNode(v.f);
         default: assert(false);
         // no break
      }
      return 0;
   }
   */

   DLLLOCAL qore_type_t getType() const {
      if (assigned)
         switch (type) {
            case QV_Bool: return NT_BOOLEAN;
            case QV_Int: return NT_INT;
            case QV_Float: return NT_FLOAT;
            case QV_Node: return v.n ? v.n->getType() : 0;
            default: assert(false);
               // no break
         }
      return NT_NOTHING;
   }

   DLLLOCAL const char* getTypeName() const {
      if (assigned)
         switch (type) {
            case QV_Bool: return QoreBoolNode::getStaticTypeName();
            case QV_Int: return QoreBigIntNode::getStaticTypeName();
            case QV_Float: return QoreFloatNode::getStaticTypeName();
            case QV_Node: return get_type_name(v.n);
            default: assert(false);
               // no break
         }
      return "NOTHING";
   }

   DLLLOCAL AbstractQoreNode** getValuePtr(ExceptionSink* xsink) const {
      if (!fixed_type) {
         if (type != QV_Node)
            type = QV_Node;
         if (!assigned) {
            assigned = true;
            v.n = 0;
         }
         return (AbstractQoreNode**)&v.n;
      }

      assert(false);
      xsink->raiseException("DEPRECATED-API", "this module uses a deprecated API that is no longer supported; please either update the module with a newer version that uses newer APIs that work with lvalues/references in newer versions of Qore or contact the developer to update the module");
      return 0;
   }

   /*
   DLLLOCAL AbstractQoreNode** getContainerValuePtr() {
      if (!fixed_type) {
         if (type != QV_Node) {
            type = QV_Node;
         }
         if (!assigned) {
            assigned = true;
            v.n = 0;
         }
         return (AbstractQoreNode**)&v.n;
      }
      return 0;
   }
   */

   // lvalue operations
   DLLLOCAL int64 plusEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
         case QV_Node: {
            if (!assigned) {
               assigned = true;
               type = QV_Int;
               return v.i = i;
            }

            QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
            vv->val += i;
            return vv->val;
         }
#endif

         case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = i;
            }
            return v.i += i;

            // to avoid warnings about missing enum values
         default:
            assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL double plusEqualsFloat(double f, AbstractQoreNode*& old) {
      assert(type == QV_Float);

      switch (type) {
#if 0
	 case QV_Node: {
            if (!assigned) {
               assigned = true;
               type = QV_Float;
               return v.f = f;
            }

	    QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f += f;
	    return vv->f;
	 }
#endif

	 case QV_Float:
            if (!assigned) {
               assigned = true;
               return v.f = f;
            }
	    return v.f += f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0.0;
   }

   DLLLOCAL int64 minusEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            if (!assigned) {
               assigned = true;
               type = QV_Int;
               return v.i = -i;
            }

	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val -= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = -i;
            }
	    return v.i -= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL double minusEqualsFloat(double f, AbstractQoreNode*& old) {
      assert(type == QV_Float);

      switch (type) {
#if 0
	 case QV_Node: {
            if (!assigned) {
               assigned = true;
               type = QV_Float;
               return v.f = f;
            }

	    QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f -= f;
	    return vv->f;
	 }
#endif

	 case QV_Float:
            if (!assigned) {
               assigned = true;
               return v.f = -f;
            }
	    return v.f -= f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0.0;
   }

   DLLLOCAL int64 orEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            if (!assigned) {
               assigned = true;
               type = QV_Int;
               return v.i = i;
            }

	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val |= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = i;
            }
	    return v.i |= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 andEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            if (!assigned) {
               assigned = true;
               type = QV_Int;
               return v.i = 0;
            }
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val &= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = 0;
            }
	    return v.i &= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
	    // no break
     }

      return 0;
   }

   DLLLOCAL int64 modulaEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    return i ? vv->val %= i : vv->val = 0;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = 0;
            }
	    return i ? v.i %= i : v.i = 0;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 multiplyEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val *= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = 0;
            }
	    return v.i *= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 divideEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);
      assert(i);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val /= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = 0;
            }
	    return v.i /= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 xorEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val ^= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = i;
            }
	    return v.i ^= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
	    // no break
    }

      return 0;
   }

   DLLLOCAL int64 shiftLeftEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val <<= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = 0;
            }
	    return v.i <<= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 shiftRightEqualsBigInt(int64 i, AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    vv->val >>= i;
	    return vv->val;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = 0;
            }
	    return v.i >>= i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 postIncrementBigInt(AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    int64 rv = vv->val;
	    ++vv->val;
	    return rv;
	 }

	 case QV_Float: {
            if (!assigned) {
               assigned = true;
               v.f = 1.0;
               return 0;
            }
	    int64 rv = (int64)v.f;
	    ++v.f;
	    return rv;
	 }
#endif

	 case QV_Int: {
            if (!assigned) {
               assigned = true;
               v.i = 1;
               return 0;
            }
	    int64 rv = (int64)v.i;
	    ++v.i;
	    return rv;
	 }

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 preIncrementBigInt(AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    return ++vv->val;
	 }

	 case QV_Float:
            if (!assigned) {
               assigned = true;
               v.f = 1.0;
               return 1;
            }
	    return (int64)++v.f;
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = 1;
            }
	    return ++v.i;

	 // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 postDecrementBigInt(AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    return vv->val--;
	 }

	 case QV_Float: {
            if (!assigned) {
               assigned = true;
               v.f = -1.0;
               return 0;
            }
	    int64 rv = (int64)v.f;
	    --v.f;
	    return rv;
	 }
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               v.i = -1;
               return 0;
            }
	    return v.i--;

	 // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL int64 preDecrementBigInt(AbstractQoreNode*& old) {
      assert(type == QV_Int);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreBigIntNode* vv = ensureUnique<QoreBigIntNode, int64, NT_INT>(old);
	    return --vv->val;
	 }

	 case QV_Float:
            if (!assigned) {
               assigned = true;
               v.f = -1.0;
               return -1;
            }
	    return (int64)--v.f;
#endif

	 case QV_Int:
            if (!assigned) {
               assigned = true;
               return v.i = -1;
            }
	    return --v.i;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }
      return 0;
   }

   DLLLOCAL double postIncrementFloat(AbstractQoreNode*& old) {
      assert(type == QV_Float);

      switch (type) {
#if 0
         case QV_Node: {
            assert(false);
            QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
            return vv->f++;
         }

         case QV_Int: {
            assert(false);
            double rv = v.i;
            ++v.i;
            return rv;
         }
#endif

         case QV_Float:
            if (!assigned) {
               assigned = true;
               v.f = 1.0;
               return 0.0;
            }
            return v.f++;

            // to avoid warnings about missing enum values
         default:
            assert(false);
            // no break
      }

      return 0.0;
   }

   DLLLOCAL double preIncrementFloat(AbstractQoreNode*& old) {
      assert(type == QV_Float);

      switch (type) {
#if 0
         case QV_Node: {
            assert(false);
            QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
            return ++vv->f;
         }

         case QV_Int:
            assert(false);
            return (double)++v.i;
#endif

         case QV_Float:
            if (!assigned) {
               assigned = true;
               return v.f = 1.0;
            }
            return ++v.f;

         // to avoid warnings about missing enum values
         default:
            assert(false);
            // no break
      }

      return 0.0;
   }

   DLLLOCAL double postDecrementFloat(AbstractQoreNode*& old) {
      assert(type == QV_Float);

      switch (type) {
#if 0
         case QV_Node: {
            assert(false);
            QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
            return vv->f--;
         }

         case QV_Int: {
            assert(false);
            double rv = v.i;
            --v.i;
            return rv;
         }
#endif

         case QV_Float:
            if (!assigned) {
               assigned = true;
               v.f = -1.0;
               return 0.0;
            }
            return v.f--;

         // to avoid warnings about missing enum values
         default:
            assert(false);
            // no break
      }

      return 0.0;
   }

   DLLLOCAL double preDecrementFloat(AbstractQoreNode*& old) {
      assert(type == QV_Float);

      switch (type) {
#if 0
         case QV_Node: {
            assert(false);
            QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
            return --vv->f;
         }

         case QV_Int:
            assert(false);
            return --v.i;
#endif

         case QV_Float:
            if (!assigned) {
               assigned = true;
               return v.f = -1.0;
            }
            return --v.f;

            // to avoid warnings about missing enum values
         default:
            assert(false);
            // no break
      }
      return 0;
   }

   DLLLOCAL double multiplyEqualsFloat(double f, AbstractQoreNode*& old) {
      assert(type == QV_Float);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f *= f;
	    return vv->f;
	 }

	 case QV_Int:
            assert(false);
	    return v.i *= (int64)f;
#endif

	 case QV_Float:
            if (!assigned) {
               assigned = true;
               return v.f = 0;
            }
	    return v.f *= f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL double divideEqualsFloat(double f, AbstractQoreNode*& old) {
      assert(type == QV_Float);
      assert(f);

      switch (type) {
#if 0
	 case QV_Node: {
            assert(false);
	    QoreFloatNode* vv = ensureUnique<QoreFloatNode, double, NT_FLOAT>(old);
	    vv->f /= f;
	    return vv->f;
	 }

	 case QV_Int:
            assert(false);
	    return v.i /= (int64)f;
#endif

	 case QV_Float:
            if (!assigned) {
               assigned = true;
               return v.f = 0;
            }
	    return v.f /= f;

	    // to avoid warnings about missing enum values
	 default:
	    assert(false);
            // no break
      }

      return 0;
   }

   DLLLOCAL QoreValue remove(bool& was_static_assignment) {
      assert(!was_static_assignment);

      if (!assigned)
         return QoreValue();
      assigned = false;
      if (static_assignment) {
         static_assignment = false;
         was_static_assignment = true;
      }

      switch (type) {
         case QV_Bool:
            return v.b;
         case QV_Int:
            return v.i;
         case QV_Float:
            return v.f;
         case QV_Node:
            return v.n;
         default:
            assert(false);
            // no break
      }
      return QoreValue();
   }

   // ignore any current value and sets the lvalue to unassigned
   DLLLOCAL void unassignIgnore() {
      if (assigned) {
         assigned = false;
         if (static_assignment)
            static_assignment = false;
      }
   }

   DLLLOCAL AbstractQoreNode* removeNode(bool for_del) {
      if (!assigned)
         return 0;
      assigned = false;
      assert(!static_assignment);

      switch (type) {
         case QV_Bool:
            return for_del ? 0 : get_bool_node(v.b);
         case QV_Int:
            return for_del ? 0 : new QoreBigIntNode(v.i);
         case QV_Float:
            return for_del ? 0 : new QoreFloatNode(v.f);
         case QV_Node:
            return v.n;
         default:
            assert(false);
            // no break
      }
      return 0;
   }
};

typedef QoreLValue<> QoreLValueGeneric;

#endif
