/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreValue.h

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

#ifndef _QORE_QOREVALUE_H
#define _QORE_QOREVALUE_H

#include <assert.h>

typedef unsigned char valtype_t;

//! @defgroup QoreValue type constants
/** the possible values for QoreValue::type
 */
//@{
#define QV_Bool  (valtype_t)0  //!< for boolean values
#define QV_Int   (valtype_t)1  //!< for integer values
#define QV_Float (valtype_t)2  //!< for floating-point values
#define QV_Node  (valtype_t)3  //!< for heap-allocated values
#define QV_Ref   (valtype_t)4  //!< for references (when used with lvalues)
//@}

// forward reference
class AbstractQoreNode;
class QoreString;

//! this is the union that stores values in QoreValue
union qore_value_u {
   bool b;               //!< for boolean values
   int64 i;              //!< for integer values
   double f;             //!< for double values
   AbstractQoreNode* n;  //!< for all heap-allocated values
};

//! namespace for implementation details of QoreValue functions
namespace detail {
   //! used in QoreValue::get()
   template<typename Type>
   struct QoreValueCastHelper {
      typedef Type * Result;

      template<typename QV>
      static Result cast(QV *qv, valtype_t type) {
         assert(type == QV_Node);
         assert(!qv->v.n || dynamic_cast<Result>(qv->v.n));
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
      static int64 cast(QV *qv, valtype_t type) {
         return qv->getAsBigInt();
      }
   };
} // namespace detail

//! The main value class in Qore, designed to be passed by value
struct QoreValue {
   friend class ValueHolder;
   friend class ValueOptionalRefHolder;
   template<typename> friend struct detail::QoreValueCastHelper;

protected:
   //! returns the internal AbstractQoreNode pointer, does not check that type == QV_Node, leaves the object empty
   DLLEXPORT AbstractQoreNode* takeNodeIntern();

public:
   //! the actual value is stored here
   qore_value_u v;
   //! indicates the value that the union is holding
   valtype_t type;

   //! creates with no value (i.e. @ref QoreNothingNode)
   DLLEXPORT QoreValue();

   //! creates as a bool
   DLLEXPORT QoreValue(bool b);

   //! creates as an int
   DLLEXPORT QoreValue(int i);

   //! creates as an int
   DLLEXPORT QoreValue(unsigned int i);

   //! creates as an int
   DLLEXPORT QoreValue(long i);

   //! creates as an int
   DLLEXPORT QoreValue(unsigned long i);

   //! creates as an int
   DLLEXPORT QoreValue(unsigned long long i);

   //! creates as an int
   DLLEXPORT QoreValue(int64 i);

   //! creates as a double
   DLLEXPORT QoreValue(double f);

   //! the QoreValue object takes the reference of the argument passed
   DLLEXPORT QoreValue(AbstractQoreNode* n);

   //! creates as the given object; does not reference n for the assignment to this object
   /** sanitizes n (increases the reference of n if necessary), meaning that
       if possible, the value is converted to an immediate value in place
       (int, float, or bool)

       if getType() == QV_Node after this assignment, then the node must be referenced for the assignment
   */
   DLLEXPORT QoreValue(const AbstractQoreNode* n);

   //! copies the value, in case type == QV_Node, no additional references are made in this function
   DLLEXPORT QoreValue(const QoreValue& old);

   //! exchanges the values
   DLLEXPORT void swap(QoreValue& val);

   //! returns the value as a bool
   DLLEXPORT bool getAsBool() const;

   //! returns the value as an int
   DLLEXPORT int64 getAsBigInt() const;

   //! returns the value as a float
   DLLEXPORT double getAsFloat() const;

   //! references the contained value if type == QV_Node
   DLLEXPORT void ref() const;

   //! references the contained value if type == QV_Node, returns itself
   DLLEXPORT QoreValue refSelf() const;

   //! returns any AbstractQoreNode value held; if type != QV_Node, returns NULL
   DLLEXPORT AbstractQoreNode* getInternalNode();

   //! returns any AbstractQoreNode value held; if type != QV_Node, returns NULL
   DLLEXPORT const AbstractQoreNode* getInternalNode() const;

   //! the QoreValue object takes the reference of the argument
   /** @param n the new node value of the object, sets type to QV_Node
       @return any node value held before; if type != QV_Node before the assignment, returns NULL
    */
   DLLEXPORT AbstractQoreNode* assign(AbstractQoreNode* n);

   //! sets the value of the object and returns any node value held previously
   /** @param n the new value of the object
       @return any node value held before; if type != QV_Node before the assignment, returns NULL
    */
   DLLEXPORT AbstractQoreNode* assignAndSanitize(const QoreValue n);

   //! sets the value of the object and returns any node value held previously
   /** @param n the new value of the object; sets type to QV_Int
       @return any node value held before; if type != QV_Node before the assignment, returns NULL
    */
   DLLEXPORT AbstractQoreNode* assign(int64 n);

   //! sets the value of the object and returns any node value held previously
   /** @param n the new value of the object; sets type to QV_Float
       @return any node value held before; if type != QV_Node before the assignment, returns NULL
    */
   DLLEXPORT AbstractQoreNode* assign(double n);

   //! sets the value of the object and returns any node value held previously
   /** @param n the new value of the object; sets type to QV_Bool
       @return any node value held before; if type != QV_Node before the assignment, returns NULL
    */
   DLLEXPORT AbstractQoreNode* assign(bool n);

   //! sets the value of the object to @ref QoreNothingNode and returns any node value held previously
   /** sets type to QV_Node

       @return any node value held before; if type != QV_Node before the assignment, returns NULL
    */
   DLLEXPORT AbstractQoreNode* assignNothing();

   //! returns trus if the argument value is equal to the current value with type conversions
   DLLEXPORT bool isEqualSoft(const QoreValue v, ExceptionSink* xsink) const;

   //! returns trus if the argument value is equal to the current value without any type conversions
   DLLEXPORT bool isEqualHard(const QoreValue v) const;

   //! converts any node pointers to efficient representations if possible and dereferences the node value contained
   DLLEXPORT void sanitize();

   //! assigns a new value
   DLLEXPORT QoreValue& operator=(const QoreValue& n);

   //! dereferences any contained AbstractQoreNode pointer and sets to 0; does not modify other values
   DLLEXPORT void discard(ExceptionSink* xsink);

   //! unconditionally set the QoreValue to @ref QoreNothingNode (does not dereference any possible contained AbstractQoreNode ptr)
   DLLEXPORT void clear();

   //! appends the string value of the contained node to the string argument with optional formatting
   DLLEXPORT int getAsString(QoreString& str, int format_offset, ExceptionSink *xsink) const;

   //! returns the string value with optional formatting of the contained node
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

   //! returns the value as the given type
   /** @note that if a pointer type is given and the object does not contain a node (i.e. type != QV_Node), then this call will cause a segfault, however it is always legal to cast to simple types (int64, bool, float), in which case type conversions are performed
    */
   template<typename T>
   DLLLOCAL typename detail::QoreValueCastHelper<T>::Result get() {
      return detail::QoreValueCastHelper<T>::cast(this, type);
   }

   //! returns the value as the given type
   /** @note that if a pointer type is given and the object does not contain a node (i.e. type != QV_Node), then this call will cause a segfault, however it is always legal to cast to simple types (int64, bool, float), in which case type conversions are performed
    */
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

   //! returns the type of the value
   /** @since %Qore 0.8.13
   */
   DLLEXPORT const QoreTypeInfo* getTypeInfo() const;

   //! returns the type of value contained
   DLLEXPORT qore_type_t getType() const;

   //! returns a string type description of the value contained (ex: \c "nothing" for a null AbstractQoreNode pointer)
   DLLEXPORT const char* getTypeName() const;

   //! returns a string type description of the full type of the value contained (ex: \c "nothing" for a null AbstractQoreNode pointer); differs from the return value of getTypeName() for complex types (ex: \c "hash<string, int>")
   DLLEXPORT const char* getFullTypeName() const;

   //! returns true if the object contains a non-null AbstractQoreNode pointer (ie type == QV_Node && v.n is not 0)
   DLLEXPORT bool hasNode() const;

   //! returns true if the object contains NOTHING
   DLLEXPORT bool isNothing() const;

   //! returns true if the object contains NULL
   DLLEXPORT bool isNull() const;

   //! returns true if the object contains NOTHING or NULL
   DLLEXPORT bool isNullOrNothing() const;
};

//! base class for holding a QoreValue object
class ValueHolderBase {
protected:
   //! the vlaue held
   QoreValue v;
   //! for possible Qore-language exceptions
   ExceptionSink* xsink;

public:
   //! creates an ampty object
   DLLLOCAL ValueHolderBase(ExceptionSink* xs) : xsink(xs) {
   }

   //! creates the object with the given value
   DLLLOCAL ValueHolderBase(QoreValue n_v, ExceptionSink* xs) : v(n_v), xsink(xs) {
   }

   //! returns the value being managed
   DLLLOCAL QoreValue* operator->() { return &v; }

   //! returns the value being managed
   DLLLOCAL QoreValue& operator*() { return v; }
};

//! holds an object and dereferences it in the destructor
class ValueHolder : public ValueHolderBase {
public:
   //! creates an empty object
   DLLLOCAL ValueHolder(ExceptionSink* xs) : ValueHolderBase(xs) {
   }

   //! creates the object with the given value
   DLLLOCAL ValueHolder(QoreValue n_v, ExceptionSink* xs) : ValueHolderBase(n_v, xs) {
   }

   //! dereferences any contained node
   DLLEXPORT ~ValueHolder();

   //! returns a referenced AbstractQoreNode ptr; caller owns the reference; the current object is left empty
   DLLEXPORT AbstractQoreNode* getReferencedValue();

   //! returns a QoreValue object and leaves the current object empty; the caller owns any reference contained in the return value
   DLLEXPORT QoreValue release();

   //! assigns the object, any currently-held value is dereferenced before the assignment
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

//! allows storing a value and setting a boolean flag that indicates if the value should be dereference in the destructor or not
class ValueOptionalRefHolder : public ValueHolderBase {
private:
   // not implemented
   DLLLOCAL QoreValue& operator=(QoreValue& nv);

protected:
   //! flag indicating if the value should be dereferenced in the destructor or not
   bool needs_deref;

public:
   //! creates the object with the given values
   DLLLOCAL ValueOptionalRefHolder(QoreValue n_v, bool nd, ExceptionSink* xs) : ValueHolderBase(n_v, xs), needs_deref(nd) {
   }

   //! creates an empty object
   DLLLOCAL ValueOptionalRefHolder(ExceptionSink* xs) : ValueHolderBase(xs), needs_deref(false) {
   }

   DLLEXPORT ~ValueOptionalRefHolder();

   //! returns true if the value is temporary (needs dereferencing)
   DLLLOCAL bool isTemp() const { return needs_deref; }

   //! sets needs_deref = false
   DLLLOCAL void clearTemp() {
      if (needs_deref)
         needs_deref = false;
   }

   //! returns true if holding an AbstractQoreNode reference
   DLLLOCAL operator bool() const {
      return v.type == QV_Node && v.v.n;
   }

   //! assigns a new non-temporary value
   DLLLOCAL void setValue(QoreValue nv) {
      if (needs_deref) {
         v.discard(xsink);
         needs_deref = false;
      }
      v = nv;
   }

   //! assigns a new value
   DLLLOCAL void setValue(QoreValue nv, bool temp) {
      if (needs_deref)
         v.discard(xsink);
      if (needs_deref != temp)
         needs_deref = temp;
      v = nv;
   }

   // ensures that the held value is referenced
   /** if needs_deref is false and an AbstractQoreNode* is contained, then the value is referenced and needs_deref is set to true
    */
   DLLEXPORT void ensureReferencedValue();

   //! returns the stored node value and leaves the current object empty
   template<typename T>
   DLLLOCAL T* takeReferencedNode() {
      T* rv = v.take<T>();
      if (needs_deref)
         needs_deref = false;
      else
         rv->ref();

      return rv;
   }

   //! returns a referenced AbstractQoreNode ptr; caller owns the reference; the current object is left empty
   DLLEXPORT AbstractQoreNode* getReferencedValue();

   //! returns the stored AbstractQoreNode pointer and sets the dereference flag as an output variable
   DLLLOCAL AbstractQoreNode* takeNode(bool& nd) {
      if (v.type == QV_Node) {
         nd = needs_deref;
         return v.takeNodeIntern();
      }
      nd = true;
      return v.takeNode();
   }

   //! returns the stored value and sets the dereference flag as an output variable
   DLLLOCAL QoreValue takeValue(bool& nd) {
      if (v.type == QV_Node) {
         nd = needs_deref;
	 return v.takeNodeIntern();
      }
      nd = false;
      return v;
   }

   //! returns the stored value which must be dereferenced if it is a node object (i.e. type == QV_Node)
   DLLLOCAL void takeValueFrom(ValueOptionalRefHolder& val) {
      if (needs_deref)
         v.discard(xsink);
      v = val.takeValue(needs_deref);
   }

   //! returns a QoreValue after incrementing the reference count of any node value stored
   DLLEXPORT QoreValue takeReferencedValue();

   // FIXME: remove with new API/ABI
   //! converts pointers to efficient representations and manages the reference count
   DLLEXPORT void sanitize();
};

//! evaluates an AbstractQoreNode and dereferences the stored value in the destructor
class ValueEvalRefHolder : public ValueOptionalRefHolder {
protected:

public:
   //! evaluates the AbstractQoreNode argument
   DLLEXPORT ValueEvalRefHolder(const AbstractQoreNode* exp, ExceptionSink* xs);
};

#endif
