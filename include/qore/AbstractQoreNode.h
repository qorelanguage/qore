/*
  AbstractQoreNode.h
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#ifndef _QORE_NODE_H

#define _QORE_NODE_H

#include <qore/common.h>
#include <qore/QoreReferenceCounter.h>
#include <qore/node_types.h>

#include <string>

#include <assert.h>

#define FMT_NONE   -1
#define FMT_NORMAL 0

class QoreString;

//! The base class for all value and parse types in Qore expression trees
/**
   Defines the interface for all value and parse types in Qore expression trees.  Default implementations are given for most virtual functions.
 */
class AbstractQoreNode : public QoreReferenceCounter
{
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AbstractQoreNode(const AbstractQoreNode&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AbstractQoreNode& operator=(const AbstractQoreNode&);

      //! default implementation, returns false
      /**
	 This function is called by the normal class function "getAsBool()"
       */
      DLLEXPORT virtual bool getAsBoolImpl() const { return false; }
      //! default implementation, returns 0
      /**
	 This function is called by the normal class function "getAsInt()"
       */
      DLLEXPORT virtual int getAsIntImpl() const { return 0; }
      //! default implementation, returns 0
      /**
	 This function is called by the normal class function "getAsBigInt()"
       */
      DLLEXPORT virtual int64 getAsBigIntImpl() const { return 0; }
      //! default implementation, returns 0.0
      /**
	 This function is called by the normal class function "getAsFloat()"
       */
      DLLEXPORT virtual double getAsFloatImpl() const { return 0.0; }

      //! evaluates the value and returns the result
      /** if a qore-language exception occurs, then the result returned must be 0.
	  the result of evaluation can also be 0 (equivalent to NOTHING) as well
	  without an exception.
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation (can be 0)
	  @see AbstractQoreNode::eval()
       */
      DLLEXPORT virtual AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const = 0;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  @see AbstractQoreNode::eval()
      */
      DLLEXPORT virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns a 64-bit integer value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual int64 bigIntEvalImpl(class ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns an integer value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual int integerEvalImpl(class ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns a boolean value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool boolEvalImpl(class ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns a floating-point value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual double floatEvalImpl(class ExceptionSink *xsink) const = 0;

      //! decrements the reference count
      /** deletes the object when the reference count = 0.  
	  The ExceptionSink argument is needed for those types that could throw an exception when they are deleted (ex: QoreObject)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return true if the object can be deleted, false if not (externally-managed)
       */
      DLLEXPORT virtual bool derefImpl(class ExceptionSink *xsink);

   protected:
      //! the type of the object
      /**
	 instead of using a virtual method to return a default type code for each implemented type, it's stored as an attribute of the base class.  This makes it possible to avoid making virtual function calls as a performance optimization in many cases, also it allows very fast type determination without makiing either a virtual function call or using dynamic_cast<> at the epense of more memory usage
       */
      qore_type_t type : 10;

      //! this is true for values, if false then either the type needs evaluation to produce a value or is a parse expression
      bool value : 1;

      //! if this is true then the type can be evaluated
      bool needs_eval_flag : 1;

      //! if this is set to true, then reference counting is turned off for objects of this class
      bool there_can_be_only_one : 1;

      //! default destructor does nothing
      /**
	 The destructor is protected because it should not be called directly, which also means that these objects cannot normally be created on the stack.  They are referenced counted, and the deref() function should be used to decrement the reference count rather than using the delete operator.  Because the QoreObject class at least could throw a Qore Exception when it is deleted, the deref() function takes an ExceptionSink argument by default as well. 
       */
      DLLEXPORT virtual ~AbstractQoreNode();

   public:
      //! constructor takes the type
      /** The type code for the class is passed as the argument to the constructor
	  @param t the Qore type code identifying this class in the Qore type system
	  @param n_there_can_be_only_one whereas this type is normally reference counted, if this is set to true, then referencing counting is turned off for this type.  This can only be turned on when the type represents a single value.
       */
      DLLEXPORT AbstractQoreNode(qore_type_t t, bool n_value, bool n_needs_eval, bool n_there_can_be_only_one = false);

      //! returns the boolean value of the object
      /**
	 calls getAsBoolImpl() if necessary (there is an optimization for the QoreBoolNode class) to return the boolean value of the object
       */
      DLLEXPORT bool getAsBool() const;

      //! returns the integer value of the object
      /**
	 calls getAsIntImpl() if necessary (there is an optimization for the QoreBigIntNode class) to return the integer value of the object
       */
      DLLEXPORT int getAsInt() const;

      //! returns the 64-bit integer value of the object
      /**
	 calls getAsBitIntImpl() if necessary (there is an optimization for the QoreBigIntNode class) to return the integer value of the object
       */
      DLLEXPORT int64 getAsBigInt() const;

      //! returns the float value of the object
      /**
	 calls getAsFloatImpl() if necessary (there is an optimization for the QoreFloatNode class) to return the floating-point value of the object
       */
      DLLEXPORT double getAsFloat() const;

      //! returns the value of the type converted to a string, default implementation: returns the empty string
      /** NOTE: do not use this function directly, use QoreStringValueHelper instead
	  @param del output parameter: if del is true, then the resulting QoreString pointer belongs to the caller (and must be deleted manually), if false it must not be
	  @return a QoreString pointer, use the del output parameter to determine ownership of the pointer
	  @see QoreStringValueHelper
       */
      DLLEXPORT virtual QoreString *getStringRepresentation(bool &del) const;

      //! concatentates the value of the type to an existing QoreString reference, default implementation does nothing
      /**
	 @param str a reference to a QoreString where the value of the type will be concatenated
       */
      DLLEXPORT virtual void getStringRepresentation(QoreString &str) const;

      //! returns the DateTime representation of this type (default implementation: returns ZeroDate, del = false)
      /** NOTE: Use the DateTimeValueHelper class instead of using this function directly
	  @param del output parameter: if del is true, then the returned DateTime pointer belongs to the caller (and must be deleted manually), if false, then it must not be
	  @see DateTimeValueHelper
       */
      DLLEXPORT virtual class DateTime *getDateTimeRepresentation(bool &del) const;

      //! assigns the date representation of a value to the DateTime reference passed, default implementation does nothing
      /** 
	  @param dt the DateTime reference to be assigned
       */
      DLLEXPORT virtual void getDateTimeRepresentation(DateTime &dt) const;

      //! concatenate the verbose string representation of the value (including all contained values for container types) to an existing QoreString
      /** used for %n and %N printf formatting
	  @param str the string representation of the type will be concatenated to this QoreString reference
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return -1 for exception raised, 0 = OK
      */
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const = 0;

      //! returns a QoreString giving the verbose string representation of the value (including all contained values for container types)
      /** used for %n and %N printf formatting
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const = 0;

      //! returns true if the object needs evaluation to return a value, false if not
      /** default implementation returns false
       */
      DLLLOCAL bool needs_eval() const
      {
	 return needs_eval_flag;
      }

      //! returns a copy of the object, the caller owns the reference count
      DLLEXPORT virtual AbstractQoreNode *realCopy() const = 0;

      //! tests for equality ("deep compare" including all contained values for container types) with possible type conversion (soft compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const = 0;

      //! tests for equality ("deep compare" including all contained values for container types) without type conversions (hard compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const = 0;

      //! returns the data type
      DLLLOCAL qore_type_t getType() const
      {
	 return type;
      }

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const = 0;

      //! evaluates the object and returns a value (or 0)
      /** return value requires a deref(xsink) (if not 0).  If needs_eval() returns false,
	  then this function just returns refSelf().  Otherwise evalImpl() is returned.
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation, if non-0, must be dereferenced manually
	  \Example
	  \code
	  ReferenceHolder<AbstractQoreNode> value(n->eval(xsink));
	  if (!value) // note that if a qore-language exception occured, then value = 0
	     return 0;
	  ...
	  return value.release();
	  \endcode
	  @see ReferenceHolder
      */
      DLLEXPORT AbstractQoreNode *eval(class ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  if needs_eval() is true, needs_deref = true, returns evalImpl()
	  otherwise needs_deref = false returns "this"
	  NOTE: do not use this function directly, use the QoreNodeEvalOptionalRefHolder class instead
	  @param needs_deref this is an output parameter, if needs_deref is true then the value returned must be dereferenced
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @see QoreNodeEvalOptionalRefHolder
      */
      DLLEXPORT AbstractQoreNode *eval(bool &needs_deref, class ExceptionSink *xsink) const;

      //! evaluates the object and returns a 64-bit integer value
      /** if needs_eval() returns true, then returns bigIntEvalImpl() otherwise returns getAsBigInt()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int64 bigIntEval(class ExceptionSink *xsink) const;

      //! evaluates the object and returns an integer value
      /** if needs_eval() returns true, then returns integerEvalImpl() otherwise returns getAsInteger()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT int integerEval(class ExceptionSink *xsink) const;

      //! evaluates the object and returns a boolean value
      /** if needs_eval() returns true, then returns boolEvalImpl() otherwise returns getAsBool()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT bool boolEval(class ExceptionSink *xsink) const;

      //! evaluates the object and returns a floating-point value
      /** if needs_eval() returns true, then returns floatEvalImpl() otherwise returns getAsFloat()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT double floatEval(class ExceptionSink *xsink) const;

      //! returns true if the node represents a value
      DLLLOCAL bool is_value() const
      {
	 return value;
      }

      //! decrements the reference count and calls derefImpl() if there_can_be_only_one is false, otherwise does nothing
      /** if there_can_be_only_one is false, calls derefImpl() and deletes the object when the reference count = 0.  
	  The ExceptionSink argument is needed for those types that could throw an exception when they are deleted (ex: QoreObject)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deref(class ExceptionSink *xsink);

      //! returns "this" with an incremented reference count
      DLLEXPORT AbstractQoreNode *refSelf() const;

      //! increments the reference count
      DLLEXPORT void ref() const;
};

//! The base class for all types in Qore expression trees that cannot throw an exception when deleted
/**
   This class adds the deref() function without an ExceptionSink argument.
 */
class SimpleQoreNode : public AbstractQoreNode 
{
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SimpleQoreNode& operator=(const SimpleQoreNode&);

   public:
      //! constructor takes the type and value arguments
      DLLLOCAL SimpleQoreNode(qore_type_t t, bool n_value, bool n_needs_eval, bool n_there_can_be_only_one = false) : AbstractQoreNode(t, n_value, n_needs_eval, n_there_can_be_only_one)
      {
      }

      //! copy constructor
      DLLEXPORT SimpleQoreNode(const SimpleQoreNode &) : AbstractQoreNode(type, value, needs_eval_flag, there_can_be_only_one)
      {
      }

      //! decrements the reference count and deletes the object when references = 0
      /**
	 This function is not virtual and should be used when possible for SimpleQoreNode objects
       */
      DLLEXPORT void deref();
};

//! base class non-referenced-counted parse and value types (where there_can_be_only_one is true)
class UniqueQoreNode : public AbstractQoreNode 
{
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL UniqueQoreNode& operator=(const UniqueQoreNode&);

   public:
      //! constructor takes the type argument
      DLLLOCAL UniqueQoreNode(qore_type_t t, bool value, bool needs_eval) : AbstractQoreNode(t, value, needs_eval, true)
      {
      }

      //! copy constructor
      DLLLOCAL UniqueQoreNode(const UniqueQoreNode &) : AbstractQoreNode(type, value, needs_eval_flag, true)
      {
      }
};

class SimpleValueQoreNode : public SimpleQoreNode
{
   private:

   protected:
      DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   public:
      DLLLOCAL SimpleValueQoreNode(qore_type_t t, bool n_there_can_be_only_one = false) : SimpleQoreNode(t, true, false, n_there_can_be_only_one)
      {
      }
};

class UniqueValueQoreNode : public UniqueQoreNode
{
   protected:
      DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   public:
      //! constructor takes the type argument
      DLLLOCAL UniqueValueQoreNode(qore_type_t t) : UniqueQoreNode(t, true, false)
      {
      }

};

//! for getting an integer number of seconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int getSecZeroInt(const AbstractQoreNode *a);

//! for getting an integer number of seconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getSecZeroBigInt(const AbstractQoreNode *a);

//! for getting an integer number of seconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int getSecMinusOneInt(const AbstractQoreNode *a);

//! for getting an integer number of seconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getSecMinusOneBigInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int getMsZeroInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getMsZeroBigInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int getMsMinusOneInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getMsMinusOneBigInt(const AbstractQoreNode *a);

//! for getting an integer number of microseconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int getMicroSecZeroInt(const AbstractQoreNode *a);

//! to check if an AbstractQoreNode object is NOTHING
static inline bool is_nothing(const AbstractQoreNode *n)
{
   if (!n || n->getType() == NT_NOTHING)
      return true;
   
   return false;
}

//! to deref an AbstractQoreNode (when the pointer may be 0)
static inline void discard(AbstractQoreNode *n, ExceptionSink *xsink)
{
   if (n)
      n->deref(xsink);
}

//! this class manages reference counts for the optional evaluation of AbstractQoreNode objects
/**
   This class can only be used on the stack (cannot be allocated dynamically).
   This class is designed to avoid atomic reference count increments and decrements whenever possible and to avoid an "eval()" call for types that do not require it (such as value types).  It is used extensively internally but normally should not need to be used outside of the qore library itself.
 */
class QoreNodeEvalOptionalRefHolder {
   private:
      AbstractQoreNode *val;
      ExceptionSink *xsink;
      bool needs_deref;

      DLLLOCAL void discard_intern()
      {
	 if (needs_deref && val)
	    val->deref(xsink);
      }

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreNodeEvalOptionalRefHolder(const QoreNodeEvalOptionalRefHolder&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreNodeEvalOptionalRefHolder& operator=(const QoreNodeEvalOptionalRefHolder&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      /** this function is not implemented in order to require objects of this type to be allocated on the stack.
       */
      DLLLOCAL void *operator new(size_t);

   public:
      //! constructor used to create a holder object
      DLLLOCAL QoreNodeEvalOptionalRefHolder(ExceptionSink *n_xsink) : val(0), xsink(n_xsink), needs_deref(false)
      {
      }

      //! constructor with a value that will call the class' eval(needs_deref) method
      DLLLOCAL QoreNodeEvalOptionalRefHolder(const AbstractQoreNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink)
      {
	 if (exp)
	    val = exp->eval(needs_deref, xsink);
	 else {
	    val = 0;
	    needs_deref = false;
	 }	    
      }

      //! discards any temporary value evaluated by the constructor or assigned by "assign()"
      DLLLOCAL ~QoreNodeEvalOptionalRefHolder()
      {
	 discard_intern();
      }
      
      //! discards any temporary value evaluated by the constructor or assigned by "assign()"
      DLLLOCAL void discard()
      {
	 discard_intern();
	 needs_deref = false;
	 val = 0;
      }

      //! assigns a new value to this holder object
      DLLLOCAL void assign(bool n_needs_deref, AbstractQoreNode *n_val)
      {
	 discard_intern();
	 needs_deref = n_needs_deref;
	 val = n_val;
      }

      //! returns a referenced value - the caller will own the reference
      DLLLOCAL AbstractQoreNode *getReferencedValue()
      {
	 if (needs_deref)
	    needs_deref = false;
	 else if (val)
	    val->ref();
	 return val;
      }

      //! returns the object being managed
      DLLLOCAL const AbstractQoreNode *operator->() const { return val; }

      //! returns the object being managed
      DLLLOCAL const AbstractQoreNode *operator*() const { return val; }

      //! returns true if a value is being held
      DLLLOCAL operator bool() const { return val != 0; }
};

#endif
