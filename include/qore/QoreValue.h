/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreValue.h

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

#ifndef _QORE_QOREVALUE_H
#define _QORE_QOREVALUE_H

#include <assert.h>

typedef unsigned char valtype_t;

#define QV_Bool  (valtype_t)0
#define QV_Int   (valtype_t)1
#define QV_Float (valtype_t)2
#define QV_Node  (valtype_t)3
#define QV_Ref   (valtype_t)4

// forward reference
class AbstractQoreNode;
class QoreString;

union qore_value_u {
   bool b;
   int64 i;
   double f;
   AbstractQoreNode* n;
//   void* p;
};

namespace detail {
   //! used in QoreValue::get()
   template<typename Type>
   struct QoreValueCastHelper {
      typedef Type * Result;

      template<typename QV>
      static Result cast(QV *qv, valtype_t type) {
         assert(type == QV_Node);
         assert(dynamic_cast<Result>(qv->v.n));
         return reinterpret_cast<Result>(qv->v.n);
      }
   };

   //! used in QoreValue::get()
   template<>
   struct QoreValueCastHelper<bool> {
      typedef bool Result;

      template<typename QV>
      static bool cast(QV *qv, valtype_t type) {
         return qv->getAsBool();
      }
   };

   //! used in QoreValue::get()
   template<>
   struct QoreValueCastHelper<double> {
      typedef double Result;

      template<typename QV>
      static double cast(QV *qv, valtype_t type) {
         return qv->getAsFloat();
      }
   };

   //! used in QoreValue::get()
   template<>
   struct QoreValueCastHelper<int64> {
      typedef int64 Result;

      template<typename QV>
      static double cast(QV *qv, valtype_t type) {
         return qv->getAsBigInt();
      }
   };

   /*
   template<>
   struct QoreValueCastHelper<int> {
      typedef int Result;

      template<typename QV>
      static int cast(QV *qv, valtype_t type) {
         return qv->getAsBigInt();
      }
   };
   */
} // namespace detail

struct QoreValue {
   friend class ValueHolder;
   friend class ValueEvalRefHolder;
   template<typename> friend struct detail::QoreValueCastHelper;

protected:
   DLLEXPORT AbstractQoreNode* takeNodeIntern();

public:
   qore_value_u v;
   valtype_t type;

   //! creates with no value (ie NOTHING)
   DLLEXPORT QoreValue();

   DLLEXPORT QoreValue(bool b);

   DLLEXPORT QoreValue(int i);

   DLLEXPORT QoreValue(unsigned int i);

   DLLEXPORT QoreValue(long i);

   DLLEXPORT QoreValue(unsigned long i);

   DLLEXPORT QoreValue(int64 i);

   DLLEXPORT QoreValue(double f);

   // the QoreValue object takes the reference of the argument
   DLLEXPORT QoreValue(AbstractQoreNode* n);

   // does not reference n for any assignment
   // sanitizes n (increases the reference of n if necessary) - meaning:
   // if possible, the value is converted to an immediate value in place
   // (int, float, or bool)
   DLLEXPORT QoreValue(const AbstractQoreNode* n);

   DLLEXPORT QoreValue(const QoreValue& old);

   DLLEXPORT void swap(QoreValue& val);

   DLLEXPORT bool getAsBool() const;

   DLLEXPORT int64 getAsBigInt() const;

   DLLEXPORT double getAsFloat() const;

   DLLEXPORT QoreValue refSelf() const;

   DLLEXPORT AbstractQoreNode* getInternalNode();

   DLLEXPORT const AbstractQoreNode* getInternalNode() const;

   // the QoreValue object takes the reference of the argument
   DLLEXPORT AbstractQoreNode* assign(AbstractQoreNode* n);

   // the QoreValue object will increase the reference of n if necessary
   //returns 0 or the previously held AbstractQoreNode*
   DLLEXPORT AbstractQoreNode* assignAndSanitize(const AbstractQoreNode* n);

   DLLEXPORT AbstractQoreNode* assign(int64 n);

   DLLEXPORT AbstractQoreNode* assign(double n);

   DLLEXPORT AbstractQoreNode* assign(bool n);

   DLLEXPORT AbstractQoreNode* assignNothing();

   DLLEXPORT bool isEqualSoft(const QoreValue v, ExceptionSink* xsink) const;
   DLLEXPORT bool isEqualHard(const QoreValue v) const;

   // FIXME: remove with new API/ABI
   // converts pointers to efficient representations
   DLLEXPORT void sanitize();

   DLLEXPORT QoreValue& operator=(const QoreValue& n);

   // dereferences any contained AbstractQoreNode pointer and sets to 0; does not modify other values
   DLLEXPORT void discard(ExceptionSink* xsink);

   //! unconditionalls set the QoreValue to NOTHING (does not dereference any possible contained AbstractQoreNode ptr)
   DLLEXPORT void clear();

   DLLEXPORT int getAsString(QoreString& str, int format_offset, ExceptionSink *xsink) const;

   DLLEXPORT QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;

   //! returns a pointer to an object of the given class; takes the pointer from the object; the caller owns the reference returned
   /** will assert() in debug mode if the object does not contain a value of the requested type or if type != QV_Node
    */
   template<typename T>
   DLLLOCAL T* take() {
      assert(type == QV_Node);
      assert(dynamic_cast<T*>(v.n));
      T* rv = reinterpret_cast<T*>(v.n);
      v.n = 0;
      return rv;
   }

   template<typename T>
   DLLLOCAL typename detail::QoreValueCastHelper<T>::Result get() {
      return detail::QoreValueCastHelper<T>::cast(this, type);
   }

   template<typename T>
   DLLLOCAL typename detail::QoreValueCastHelper<const T>::Result get() const {
      return detail::QoreValueCastHelper<const T>::cast(this, type);
   }

   //! returns a referenced AbstractQoreNode pointer; leaving the "this" untouched; the caller owns the reference returned
   DLLEXPORT AbstractQoreNode* getReferencedValue() const;

   //! returns a referenced AbstractQoreNode pointer leaving "this" empty (value is taken from "this"); the caller owns the reference returned
   DLLEXPORT AbstractQoreNode* takeNode();

   //! returns a referenced AbstractQoreNode pointer only if the contained value is an AbstractQoreNode pointer, in which case "this" is left empty (the value is taken from "this"); returns 0 if the object does not contain an AbstractQoreNode pointer (type != QV_Node)
   DLLEXPORT AbstractQoreNode* takeIfNode();

   //! returns the type of value contained
   DLLEXPORT qore_type_t getType() const;

   //! returns a string type description of the value contained (ex: "nothing" for a null AbstractQoreNode pointer)
   DLLEXPORT const char* getTypeName() const;

   //! returns true if the object contains a non-null AbstractQoreNode pointer (ie type == QV_Node && v.n is not 0)
   DLLEXPORT bool hasNode() const;

   //! returns true if the object contains NOTHING
   DLLEXPORT bool isNothing() const;

   //! returns true if the object contains NULL
   DLLEXPORT bool isNull() const;

   //! returns true if the object contains NOTHING or NULL
   DLLEXPORT bool isNullOrNothing() const;
};

class ValueHolderBase {
protected:
   QoreValue v;
   ExceptionSink* xsink;

public:
   DLLLOCAL ValueHolderBase(ExceptionSink* xs) : xsink(xs) {
   }

   DLLLOCAL ValueHolderBase(QoreValue n_v, ExceptionSink* xs) : v(n_v), xsink(xs) {
   }

   //! returns the value being managed
   DLLLOCAL QoreValue* operator->() { return &v; }

   //! returns the value being managed
   DLLLOCAL QoreValue& operator*() { return v; }
};

class ValueHolder : public ValueHolderBase {
public:
   DLLLOCAL ValueHolder(ExceptionSink* xs) : ValueHolderBase(xs) {
   }

   DLLLOCAL ValueHolder(QoreValue n_v, ExceptionSink* xs) : ValueHolderBase(n_v, xs) {
   }

   DLLEXPORT ~ValueHolder();

   //! returns a referenced AbstractQoreNode ptr; caller owns the referenced; the current object is left empty
   DLLEXPORT AbstractQoreNode* getReferencedValue();

   //! returns a QoreValue object and leaves the current object empty; the caller owns any referenced contained in the return value
   DLLEXPORT QoreValue release();

   DLLLOCAL QoreValue& operator=(QoreValue nv) {
      v.discard(xsink);
      v = nv;
      return v;
   }

   //! returns true if holding an AbstractQoreNode reference
   DLLLOCAL operator bool() const {
      return v.type == QV_Node && v.v.n;
   }
};

class ValueOptionalRefHolder : public ValueHolderBase {
protected:
   bool needs_deref;

   DLLLOCAL ValueOptionalRefHolder(ExceptionSink* xs) : ValueHolderBase(xs), needs_deref(false) {
   }

   // not implemented
   DLLLOCAL QoreValue& operator=(QoreValue& nv);

public:
   DLLLOCAL ValueOptionalRefHolder(QoreValue n_v, bool nd, ExceptionSink* xs) : ValueHolderBase(n_v, xs), needs_deref(nd) {
   }

   DLLEXPORT ~ValueOptionalRefHolder();

   //! returns true if the value is temporary (needs dereferencing)
   DLLLOCAL bool isTemp() const { return needs_deref; }
};

class ValueEvalRefHolder : public ValueOptionalRefHolder {
protected:

public:
   DLLEXPORT ValueEvalRefHolder(const AbstractQoreNode* exp, ExceptionSink* xs);

   template<typename T>
   DLLLOCAL T* takeReferencedNode() {
      T* rv = v.take<T>();
      if (needs_deref)
         needs_deref = false;
      else
         rv->ref();

      return rv;
   }

   // leaves the container empty
   DLLEXPORT AbstractQoreNode* getReferencedValue();

   DLLLOCAL AbstractQoreNode* takeNode(bool& nd) {
      if (v.type == QV_Node) {
         nd = needs_deref;
         return v.takeNodeIntern();
      }
      nd = true;
      return v.takeNode();
   }

   DLLLOCAL QoreValue takeValue(bool& nd) {
      if (v.type == QV_Node) {
         nd = needs_deref;
	 return v.takeNodeIntern();
      }
      nd = false;
      return v;
   }

   DLLEXPORT QoreValue takeReferencedValue();
};

#endif
