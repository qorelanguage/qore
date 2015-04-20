/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreFloatNode.h
  
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

#ifndef _QORE_QOREFLOATNODE_H

#define _QORE_QOREFLOATNODE_H

class LocalVar;
class QoreTypeInfo;

//! Qore's floating-point value type, dynamically-allocated only, reference counted
class QoreFloatNode : public SimpleValueQoreNode {
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
   //! the destructor is protected because it should not be called directly
   /**
      @see SimpleValueQoreNode::deref()
   */
   DLLEXPORT virtual ~QoreFloatNode();

public:
   //! the value of the type
   double f;

   //! creates a new floating-point value and assigns the initial value to it
   /**
      @param n_f the value for the object
   */
   DLLEXPORT QoreFloatNode(double n_f);

   //! creates a new floating-point value and assigns 0.0 to it
   DLLEXPORT QoreFloatNode();

   //! returns the floating-point value converted to a string and sets del to true
   /** NOTE: do not use this function directly, use QoreStringValueHelper instead
       @param del output parameter: del is set to true, meaning that the resulting QoreString pointer belongs to the caller (and must be deleted manually)
       @return a QoreString pointer, use the del output parameter to determine ownership of the pointer
       @see QoreStringValueHelper
   */
   DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;

   //! concatentates the floating-point value to an existing QoreString reference, default implementation does nothing
   /**
      @param str a reference to a QoreString where the value of the type will be concatenated
   */
   DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

   //! returns the DateTime representation of this value and sets del to true
   /** The DateTime representation is calculated by converting the floating-point value 
       to an integer interpreted as the number of seconds offset from January 1, 1970.
       @note Use the DateTimeValueHelper class instead of using this function directly
       @param del output parameter: if del is true, then the returned DateTime pointer belongs to the caller (and must be deleted manually), if false, then it must not be
       @see DateTimeValueHelper
   */
   DLLEXPORT virtual class DateTime *getDateTimeRepresentation(bool &del) const;

   //! assigns the date representation of the value to the DateTime reference passed
   /** The DateTime representation is calculated by converting the floating-point value 
       to an integer interpreted as the number of seconds offset from January 1, 1970.
       @param dt the DateTime reference to be assigned
   */
   DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

   //! concatenate the string representation of the floating-point value to an existing QoreString
   /** used for %n and %N printf formatting
       @param str the string representation of the type will be concatenated to this QoreString reference
       @param foff for multi-line formatting offset, ignored by this implementation of the function
       @param xsink ignored by this implementation of the function
       @return -1 for exception raised, 0 = OK
   */
   DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

   //! returns a QoreString giving the string representation of the floating-point value, sets del to true
   /** Used for %n and %N printf formatting.  Do not call this function directly; use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead
       @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
       @param foff for multi-line formatting offset, ignored by this implementation of the function
       @param xsink ignored by this implementation of the function
       @see QoreNodeAsStringHelper
   */
   DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

   //! returns a copy of the object; the caller owns the reference count
   /**
      @return a copy of the object; the caller owns the reference count
   */
   DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

   //! tests for equality with possible type conversion (soft compare)
   /**
      @param v the value to compare
      @param xsink ignored by this implementation of the function
      @return true if the objects are equal, false if not
   */
   DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

   //! tests for equality without type conversions (hard compare)
   /**
      @param v the value to compare
      @param xsink ignored by this implementation of the function
      @return true if the objects are equal, false if not
   */
   DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

   //! returns the type name as a c string
   /**
      @return the type name as a c string
   */
   DLLEXPORT virtual const char *getTypeName() const;

   //! returns the type information
   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   //! returns the type name (useful in templates)
   /**
      @return the type name as a c string
   */
   DLLLOCAL static const char *getStaticTypeName() {
      return "float";
   }

   //! returns the type code (useful in templates)
   DLLLOCAL static qore_type_t getStaticTypeCode() {
      return NT_FLOAT;
   }

   //! returns the value of the argument (useful in templates)
   DLLLOCAL static double getValue(AbstractQoreNode *v) {
      return v->getAsFloat();
   }
};

#endif
