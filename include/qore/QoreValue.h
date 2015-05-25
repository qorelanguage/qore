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

typedef unsigned char valtype_t;

#define QV_Bool  (valtype_t)0
#define QV_Int   (valtype_t)1
#define QV_Float (valtype_t)2
#define QV_Node  (valtype_t)3
#define QV_Ref   (valtype_t)4

// forward reference
class AbstractQoreNode;

union qore_value_u {
   bool b;
   int64 i;
   double f;
   AbstractQoreNode* n;
   void* p;
};

struct QoreValue {
   friend class ValueEvalRefHolder;

protected:
   DLLEXPORT AbstractQoreNode* takeNodeIntern();
   
public:
   qore_value_u v;
   valtype_t type;

   //! creates with no value (ie NOTHING)
   DLLEXPORT QoreValue();

   DLLEXPORT QoreValue(bool b);

   DLLEXPORT QoreValue(int64 i);

   DLLEXPORT QoreValue(double f);

   DLLEXPORT QoreValue(AbstractQoreNode* n);

   DLLEXPORT QoreValue(const AbstractQoreNode* n);

   DLLEXPORT QoreValue(const QoreValue& old);

   DLLEXPORT void swap(QoreValue& val);

   DLLEXPORT bool getAsBool() const;

   DLLEXPORT int64 getAsBigInt() const;

   DLLEXPORT double getAsFloat() const;

   DLLEXPORT QoreValue refSelf() const;
   
   DLLEXPORT AbstractQoreNode* getInternalNode();

   DLLEXPORT const AbstractQoreNode* getInternalNode() const;

   DLLEXPORT AbstractQoreNode* assign(AbstractQoreNode* n);
   
   DLLEXPORT AbstractQoreNode* assign(int64 n);
   
   DLLEXPORT AbstractQoreNode* assign(double n);
   
   DLLEXPORT AbstractQoreNode* assign(bool n);

   DLLEXPORT AbstractQoreNode* assignNothing();

   DLLEXPORT bool isEqualSoft(const QoreValue v, ExceptionSink* xsink) const;
   DLLEXPORT bool isEqualHard(const QoreValue v) const;

   // FIXME: remove with new API/ABI
   // converts pointers to efficient reprensentations
   DLLEXPORT void sanitize();
   
   DLLEXPORT QoreValue& operator=(const QoreValue& n);
   
   DLLEXPORT void clearNode();
   
   DLLEXPORT void discard(ExceptionSink* xsink);

   template<typename T>
   DLLEXPORT T* get();
   
   template<typename T>
   DLLEXPORT const T* get() const;
   
   template<typename T>
   DLLEXPORT T* take();
   
   DLLEXPORT AbstractQoreNode* takeNode();

   DLLEXPORT AbstractQoreNode* takeIfNode();

   DLLEXPORT qore_type_t getType() const;

   DLLEXPORT const char* getTypeName() const;

   DLLEXPORT bool hasNode() const;
   
   DLLEXPORT bool isNothing() const;

   DLLEXPORT bool isNull() const;

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

   DLLEXPORT AbstractQoreNode* getReferencedValue();

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
   DLLEXPORT T* takeReferencedNode();

   // leaves the container empty
   DLLEXPORT AbstractQoreNode* getReferencedValue();

   DLLLOCAL AbstractQoreNode* takeNode(bool& nd) {
      if (v.type == QV_Node) {
         nd = needs_deref;
	 needs_deref = false;
         return v.takeNodeIntern();
      }
      nd = true;
      return v.takeNode();
   }

   DLLLOCAL QoreValue takeValue(bool& nd) {
      if (v.type == QV_Node) {
	 if (needs_deref) {
	    needs_deref = false;
	    nd = true;
	    return v.takeNodeIntern();
	 }
	 nd = false;
	 return v.takeNodeIntern();
      }
      nd = false;
      return v;
   }

   DLLEXPORT QoreValue takeReferencedValue();
};

#endif
