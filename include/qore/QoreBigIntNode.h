/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreBigIntNode.h
  
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

#ifndef _QORE_BIGINTNODE_H

#define _QORE_BIGINTNODE_H

#include <qore/AbstractQoreNode.h>

class LocalVar;

//! this class implements Qore's 64-bit integer data type, reference-counted, dynamically-allocated only
class QoreBigIntNode : public SimpleValueQoreNode {
private:
   //! returns the value as a boolean
   DLLLOCAL virtual bool getAsBoolImpl() const;

   //! returns the value as an integer
   DLLLOCAL virtual int getAsIntImpl() const;

   //! returns the value as a 64-bit integer
   DLLLOCAL virtual int64 getAsBigIntImpl() const;

   //! returns the value as a float
   DLLLOCAL virtual double getAsFloatImpl() const;

protected:
   DLLEXPORT virtual ~QoreBigIntNode();

   // protected constructor for subclasses only
   DLLEXPORT QoreBigIntNode(qore_type_t t, int64 v);

public:
   //! value of the integer
   int64 val;

   //! creates a new integer with the value 0
   DLLEXPORT QoreBigIntNode();

   //! creates a new integer with the value of "v"
   /**
      @param v the value of the integer
   */
   DLLEXPORT QoreBigIntNode(int64 v);

   //! returns a string representing the integer and sets del to true
   /** NOTE: do not call this function directly, use QoreStringValueHelper instead
       @param del output parameter: always sets del to false
       @see QoreStringValueHelper
   */
   DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;

   //! concatentates the string representation of the integer to an existing QoreString reference
   /**
      @param str a reference to a QoreString where the value of the type will be concatenated
   */
   DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

   //! returns the DateTime representation of this integer (interpreted as an offset in seconds from January 1, 1970)
   /** NOTE: Use the DateTimeValueHelper class instead of using this function directly
       @param del output parameter: if del is true, then the returned DateTime pointer belongs to the caller (and must be deleted manually), if false, then it must not be
       @see DateTimeValueHelper
   */
   DLLEXPORT virtual DateTime *getDateTimeRepresentation(bool &del) const;

   //! assigns the date representation of this integer (interpreted as an offset in seconds from January 1, 1970) to the DateTime reference passed
   /** 
       @param dt the DateTime reference to be assigned
   */
   DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

   //! concatenates the value of the integer to an existing QoreString
   /** used for %n and %N printf formatting.  This implementation of the function never throws a Qore-language exception
       @param str the string representation of the type will be concatenated to this QoreString reference
       @param foff for multi-line formatting offset, -1 = no line breaks
       @param xsink is ignored
       @return always returns 0
   */
   DLLEXPORT virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;

   //! returns a QoreString representing the integer
   /** used for %n and %N printf formatting
       @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
       @param foff for multi-line formatting offset, -1 = no line breaks
       @param xsink is ignored
       NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
       @see QoreNodeAsStringHelper
   */
   DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

   DLLEXPORT virtual AbstractQoreNode *realCopy() const;

   //! tests for equality with the possibility of type conversion (soft compare)
   /** this implementation of the function does not throw any Qore-language exceptions
       @param v the value to compare
       @param xsink is ignored in this version of the function
   */
   DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

   //! tests for equality without the possibility of type conversion (hard compare)
   /** this implementation of the function does not throw any Qore-language exceptions
       @param v the value to compare
       @param xsink is ignored in this version of the function
   */
   DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

   //! returns the type name as a c string
   DLLEXPORT virtual const char *getTypeName() const;

   //! returns the type information
   DLLEXPORT virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   //! returns the type code (useful in templates)
   DLLLOCAL static const char *getStaticTypeName() {
      return "integer";
   }

   //! returns the type code (useful in templates)
   DLLLOCAL static qore_type_t getStaticTypeCode() {
      return NT_INT;
   }

   //! returns the integer value (useful in templates)
   DLLLOCAL static int64 getValue(AbstractQoreNode *v) {
      return v->getAsBigInt();
   }
};

#endif
