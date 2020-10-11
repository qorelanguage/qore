/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    QoreNumberNode.h

    Qore Programming Language

    Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORENUMBERNODE_H

#define _QORE_QORENUMBERNODE_H

class LocalVar;

/** @defgroup number_format_flags Number Format Flags
    used with QoreNumberNode::toString()
 */
//@{
//! number format bitfield: default
#define QORE_NF_DEFAULT    0
//! number format bitfield: scientific
#define QORE_NF_SCIENTIFIC (1 << 0)
//! number format bitfield: raw (unrounded)
#define QORE_NF_RAW        (1 << 1)
//@}

//! Qore's arbitrary-precision number value type, dynamically-allocated only, reference counted
class QoreNumberNode : public SimpleValueQoreNode {
    friend struct qore_number_private;

private:
    //! returns the value as a bool
    DLLLOCAL virtual bool getAsBoolImpl() const;

    //! returns the value as an int
    DLLLOCAL virtual int getAsIntImpl() const;

    //! returns the value as a 64-bit int
    DLLLOCAL virtual int64 getAsBigIntImpl() const;

    //! returns the value as a double
    DLLLOCAL virtual double getAsFloatImpl() const;

protected:
    //! the private implementation of the type
    struct qore_number_private* priv;

    //! the destructor is protected because it should not be called directly
    /**
        @see SimpleValueQoreNode::deref()
    */
    DLLEXPORT virtual ~QoreNumberNode();

    // private
    DLLLOCAL QoreNumberNode(struct qore_number_private* p);

public:
    //! creates a new number value from the node, if not possible then the new number will be assigned 0
    DLLEXPORT QoreNumberNode(const QoreValue n);

    //! creates a new number value and assigns the initial value to it
    /**
        @param f the value for the object
    */
    DLLEXPORT QoreNumberNode(double f);

    //! creates a new number value and assigns the initial value to it
    /**
        @param i the value for the object
    */
    DLLEXPORT QoreNumberNode(int64 i);

    //! creates a new number value and assigns the initial value to it
    /**
        @param str the value for the object
    */
    DLLEXPORT QoreNumberNode(const char* str);

    //! creates a new number value and assigns the initial value to it
    /**
        @param str the value for the object
        @param prec the initial precision for the number
    */
    DLLEXPORT QoreNumberNode(const char* str, unsigned prec);

    //! creates a new numbering-point value and assigns it to 0
    DLLEXPORT QoreNumberNode();

    //! creates a copy of the object
    DLLEXPORT QoreNumberNode(const QoreNumberNode& old);

    //! returns the number value converted to a string and sets del to true
    /** NOTE: do not use this function directly, use QoreStringValueHelper instead
        @param del output parameter: del is set to true, meaning that the resulting QoreString pointer belongs to the caller (and must be deleted manually)
        @return a QoreString pointer, use the del output parameter to determine ownership of the pointer
        @see QoreStringValueHelper
    */
    DLLEXPORT virtual QoreString* getStringRepresentation(bool& del) const;

    //! concatentates the number value to an existing QoreString reference, default implementation does nothing
    /**
        @param str a reference to a QoreString where the value of the type will be concatenated
    */
    DLLEXPORT virtual void getStringRepresentation(QoreString& str) const;

    //! returns the DateTime representation of this value and sets del to true
    /** The DateTime representation is calculated by converting the number value
        to an integer interpreted as the number of seconds offset from January 1, 1970.
        @note Use the DateTimeValueHelper class instead of using this function directly
        @param del output parameter: if del is true, then the returned DateTime pointer belongs to the caller (and must be deleted manually), if false, then it must not be
        @see DateTimeValueHelper
    */
    DLLEXPORT virtual class DateTime *getDateTimeRepresentation(bool& del) const;

    //! assigns the date representation of the value to the DateTime reference passed
    /** The DateTime representation is calculated by converting the number value
        to an integer interpreted as the number of seconds offset from January 1, 1970.
        @param dt the DateTime reference to be assigned
    */
    DLLEXPORT virtual void getDateTimeRepresentation(DateTime& dt) const;

    //! concatenate the string representation of the number value to an existing QoreString
    /** used for %n and %N printf formatting
        @param str the string representation of the type will be concatenated to this QoreString reference
        @param foff for multi-line formatting offset, ignored by this implementation of the function
        @param xsink ignored by this implementation of the function
        @return -1 for exception raised, 0 = OK
    */
    DLLEXPORT virtual int getAsString(QoreString& str, int foff, class ExceptionSink* xsink) const;

    //! returns a QoreString giving the string representation of the number value, sets del to true
    /** Used for %n and %N printf formatting.  Do not call this function directly; use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead
        @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
        @param foff for multi-line formatting offset, ignored by this implementation of the function
        @param xsink ignored by this implementation of the function
        @see QoreNodeAsStringHelper
    */
    DLLEXPORT virtual QoreString *getAsString(bool& del, int foff, class ExceptionSink* xsink) const;

    //! returns a copy of the object; the caller owns the reference count
    /**
        @return a copy of the object; the caller owns the reference count
    */
    DLLEXPORT virtual AbstractQoreNode* realCopy() const;

    //! tests for equality with possible type conversion (soft compare)
    /**
        @param v the value to compare
        @param xsink ignored by this implementation of the function
        @return true if the objects are equal, false if not
    */
    DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const;

    //! tests for equality without type conversions (hard compare)
    /**
        @param v the value to compare
        @param xsink ignored by this implementation of the function
        @return true if the objects are equal, false if not
    */
    DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const;

    //! returns the type name as a c string
    /**
        @return the type name as a c string
    */
    DLLEXPORT virtual const char* getTypeName() const;

    //! add the argument to this value and return the result
    DLLEXPORT QoreNumberNode* doPlus(const QoreNumberNode& n) const;

    //! subtract the argument from this value and return the result
    DLLEXPORT QoreNumberNode* doMinus(const QoreNumberNode& n) const;

    //! multiply the argument to this value and return the result
    DLLEXPORT QoreNumberNode* doMultiply(const QoreNumberNode& n) const;

    //! divide this value by the argument return the result (can throw a division-by-zero exception)
    DLLEXPORT QoreNumberNode* doDivideBy(const QoreNumberNode& n, ExceptionSink* xsink) const;

    //! divide this value by the argument return the result (can throw a division-by-zero exception)
    DLLEXPORT QoreNumberNode* doDivideBy(double d, ExceptionSink* xsink) const;

    //! divide this value by the argument return the result (can throw a division-by-zero exception)
    DLLEXPORT QoreNumberNode* doDivideBy(int64 i, ExceptionSink* xsink) const;

    //! returns the negative of the current object (this)
    DLLEXPORT QoreNumberNode* negate() const;

    //! returns true if the number is zero
    DLLEXPORT bool zero() const;

    //! returns -1 if the number is negative, 0 if zero, or 1 if the number is positive
    DLLEXPORT int sign() const;

    //! returns true if the current object is less than the argument
    DLLEXPORT bool lessThan(const QoreNumberNode& n) const;

    //! returns true if the current object is less than the argument
    DLLEXPORT bool lessThan(double n) const;

    //! returns true if the current object is less than the argument
    DLLEXPORT bool lessThan(int64 n) const;

    //! returns true if the current object is less than or equal to the argument
    DLLEXPORT bool lessThanOrEqual(const QoreNumberNode& n) const;

    //! returns true if the current object is less than or equal to the argument
    DLLEXPORT bool lessThanOrEqual(double n) const;

    //! returns true if the current object is less than or equal to the argument
    DLLEXPORT bool lessThanOrEqual(int64 n) const;

    //! returns true if the current object is greater than the argument
    DLLEXPORT bool greaterThan(const QoreNumberNode& n) const;

    //! returns true if the current object is greater than the argument
    DLLEXPORT bool greaterThan(double n) const;

    //! returns true if the current object is greater than the argument
    DLLEXPORT bool greaterThan(int64 n) const;

    //! returns true if the current object is greater than or equal to the argument
    DLLEXPORT bool greaterThanOrEqual(const QoreNumberNode& n) const;

    //! returns true if the current object is greater than or equal to the argument
    DLLEXPORT bool greaterThanOrEqual(double n) const;

    //! returns true if the current object is greater than or equal to the argument
    DLLEXPORT bool greaterThanOrEqual(int64 n) const;

    //! returns true if the current object is equal to the argument
    DLLEXPORT bool equals(const QoreNumberNode& n) const;

    //! returns true if the current object is equal to the argument
    DLLEXPORT bool equals(double n) const;

    //! returns true if the current object is equal to the argument
    DLLEXPORT bool equals(int64 n) const;

    //! returns a pointer to this with the reference count incremented
    DLLEXPORT QoreNumberNode* numberRefSelf() const;

    //! concatenates the string value corresponding to the number to the string given
    /** @param str the string to append to
        @param fmt a bitfield of @ref number_format_flags "number format flags"
    */
    DLLEXPORT void toString(QoreString& str, int fmt = QORE_NF_DEFAULT) const;

    //! returns the precision of the number
    DLLEXPORT unsigned getPrec() const;

    //! returns true if the number is NaN
    DLLEXPORT bool nan() const;

    //! returns true if the number is +/-inf
    DLLEXPORT bool inf() const;

    //! returns true if the number is an ordinary number (neither NaN nor an infinity)
    DLLEXPORT bool ordinary() const;

    //! returns the type information
    DLLLOCAL virtual void parseInit(QoreValue& val, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo);

    //! returns the representation of the value as a number if possible (otherwise returns 0), caller owns the reference returned
    DLLEXPORT static QoreNumberNode* toNumber(const AbstractQoreNode* v);

    //! returns the representation of the value as a number if possible (otherwise returns 0), caller owns the reference returned
    DLLEXPORT static QoreNumberNode* toNumber(const QoreValue v);

    //! returns the type name (useful in templates)
    /**
        @return the type name as a c string
    */
    DLLLOCAL static const char* getStaticTypeName() {
        return "number";
    }

    //! returns the type code (useful in templates)
    DLLLOCAL static qore_type_t getStaticTypeCode() {
        return NT_NUMBER;
    }

    static const qore_type_t TYPE = NT_NUMBER;
};

//! manages conversions of a QoreValue to a QoreNumberNode
class QoreNumberNodeHelper {
private:
   const QoreNumberNode* num;
   bool del;

   DLLLOCAL QoreNumberNodeHelper(const QoreNumberNodeHelper&); // not implemented
   DLLLOCAL QoreNumberNodeHelper& operator=(const QoreNumberNodeHelper&); // not implemented
   DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed

public:
   //! converts the argument to a QoreNumberNode if necessary
   DLLLOCAL QoreNumberNodeHelper(const QoreValue n);

   //! destroys the object and dereferences the pointer being managed if it was a temporary object
   DLLLOCAL ~QoreNumberNodeHelper();

   //! returns the object being managed
   /**
      @return the object being managed
   */
   DLLLOCAL const QoreNumberNode* operator->() { return num; }

   //! returns the object being managed
   /**
      @return the object being managed
   */
   DLLLOCAL const QoreNumberNode* operator*() { return num; }

   //! returns a referenced value and leaves the current object empty; the caller will own the reference
   /**
      The number is referenced if necessary (if it was a temporary value)
      @return the number, where the caller will own the reference count
   */
   DLLEXPORT QoreNumberNode* getReferencedValue();

   //! returns true if the referenced being managed is temporary
   DLLLOCAL bool is_temp() const {
      return del;
   }
};

#endif
