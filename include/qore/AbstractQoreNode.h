/*
  AbstractQoreNode.h
  
  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_ABSTRACTQORENODE_H

#define _QORE_ABSTRACTQORENODE_H

#include <qore/common.h>
#include <qore/QoreReferenceCounter.h>
#include <qore/node_types.h>

#include <string>

#include <assert.h>

#define FMT_NONE   -1
#define FMT_NORMAL 0

class LocalVar;

//! The base class for all value and parse types in Qore expression trees
/**
   Defines the interface for all value and parse types in Qore expression trees.  Default implementations are given for most virtual functions.
 */
class AbstractQoreNode : public QoreReferenceCounter {
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AbstractQoreNode(const AbstractQoreNode&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL AbstractQoreNode& operator=(const AbstractQoreNode&);

      //! default implementation, returns false
      /** This function is called by the normal class function "getAsBool()"
	  @return the value of the object interpreted as a boolean
       */
      DLLEXPORT virtual bool getAsBoolImpl() const { return false; }

      //! default implementation, returns 0
      /** This function is called by the normal class function "getAsInt()"
	  @return the value of the object interpreted as an integer
       */
      DLLEXPORT virtual int getAsIntImpl() const { return 0; }

      //! default implementation, returns 0
      /** This function is called by the normal class function "getAsBigInt()"
	  @return the value of the object interpreted as a 64-bit integer
       */
      DLLEXPORT virtual int64 getAsBigIntImpl() const { return 0; }

      //! default implementation, returns 0.0
      /** This function is called by the normal class function "getAsFloat()"
	  @return the value of the object interpreted as a floating-point number
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
      DLLEXPORT virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const = 0;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  @param needs_deref output parameter: if true then the return value requires a deref()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation (can be 0)
	  @see AbstractQoreNode::eval()
      */
      DLLEXPORT virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns a 64-bit integer value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation, interpreted as a 64-bit integer
       */
      DLLEXPORT virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns an integer value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation, interpreted as an integer
       */
      DLLEXPORT virtual int integerEvalImpl(ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns a boolean value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation, interpreted as a bool
       */
      DLLEXPORT virtual bool boolEvalImpl(ExceptionSink *xsink) const = 0;

      //! evaluates the object and returns a floating-point value
      /** only called if needs_eval returns true
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation, interpreted as a double
       */
      DLLEXPORT virtual double floatEvalImpl(ExceptionSink *xsink) const = 0;

      //! decrements the reference count
      /** deletes the object when the reference count = 0.  
	  The ExceptionSink argument is needed for those types that could throw an exception when they are deleted (ex: QoreObject)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return true if the object can be deleted, false if not (externally-managed)
       */
      DLLEXPORT virtual bool derefImpl(ExceptionSink *xsink);

      //! special processing when the object's reference count transitions from 0-1
      /** only called when custom_reference_handlers = true.
	  The default implementation does nothing (calls assert(false) in debug
	  mode)
       */
      DLLEXPORT virtual void customRef() const;

      /** only called when custom_reference_handlers = true.
	  The default implementation does nothing (calls assert(false) in debug
	  mode)
       */
      DLLEXPORT virtual void customDeref(ExceptionSink *xsink);

   protected:
      //! the type of the object
      /**
	 instead of using a virtual method to return a default type code for each implemented type, it's stored as an attribute of the base class.  This makes it possible to avoid making virtual function calls as a performance optimization in many cases, also it allows very fast type determination without makiing either a virtual function call or using dynamic_cast<> at the expense of more memory usage
       */
      qore_type_t type : 11;

      //! this is true for values, if false then either the type needs evaluation to produce a value or is a parse expression
      bool value : 1;

      //! if this is true then the type can be evaluated
      bool needs_eval_flag : 1;

      //! if this is set to true, then reference counting is turned off for objects of this class
      bool there_can_be_only_one : 1;

      //! set to one for objects that need custom reference handlers
      bool custom_reference_handlers : 1;

      //! default destructor does nothing
      /**
	 The destructor is protected because it should not be called directly, which also means that these objects cannot normally be created on the stack.  They are referenced counted, and the deref() function should be used to decrement the reference count rather than using the delete operator.  Because the QoreObject class at least could throw a Qore Exception when it is deleted, AbstractQoreNode::deref() takes an ExceptionSink pointer argument by default as well. 
       */
      DLLEXPORT virtual ~AbstractQoreNode();

   public:
      //! constructor takes the type
      /** The type code for the class is passed as the argument to the constructor
	  @param t the Qore type code identifying this class in the Qore type system
	  @param n_value determines if this is a value type or not
	  @param n_needs_eval determines if the type needs evaluation when AbstractQoreNode::eval() is called
	  @param n_there_can_be_only_one whereas this type is normally reference counted, if this is set to true, then referencing counting is turned off for this type.  This can only be turned on when the type represents a single value.
	  @param n_custom_reference_handlers if true then the class implements its own reference handlers
       */
      DLLEXPORT AbstractQoreNode(qore_type_t t, bool n_value, bool n_needs_eval, bool n_there_can_be_only_one = false, bool n_custom_reference_handlers = false);

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
      DLLEXPORT virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const = 0;

      //! returns a QoreString giving the verbose string representation of the value (including all contained values for container types)
      /** Used for %n and %N printf formatting.  Do not call this function directly; use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead
	  @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
	  @param foff for multi-line formatting offset, -1 = no line breaks
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @see QoreNodeAsStringHelper
      */
      DLLEXPORT virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const = 0;

      //! returns true if the object needs evaluation to return a value, false if not
      /** default implementation returns false
	  @return true if the type supports evaluation of this object, false if not
       */
      DLLLOCAL bool needs_eval() const {
	 return needs_eval_flag;
      }

      //! returns a copy of the object; the caller owns the reference count
      /**
	 @return a copy of the object; the caller owns the reference count
       */
      DLLEXPORT virtual AbstractQoreNode *realCopy() const = 0;

      //! tests for equality ("deep compare" including all contained values for container types) with possible type conversion (soft compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
	 @return true if the objects are equal, false if not
       */
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const = 0;

      //! tests for equality ("deep compare" including all contained values for container types) without type conversions (hard compare)
      /**
	 @param v the value to compare
	 @param xsink if an error occurs, the Qore-language exception information will be added here
	 @return true if the objects are equal, false if not
       */
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const = 0;

      //! returns the data type
      /**
	 @return the data type of the object
       */
      DLLLOCAL qore_type_t getType() const {
	 return type;
      }

      //! returns the type name as a c string
      /**
	 @return the type name as a c string
       */
      DLLEXPORT virtual const char *getTypeName() const = 0;

      //! for use by parse types to initialize them for execution during stage 1 parsing
      /** @note: not yet universally used
	  This function should only be overridden by types that can appear in
	  the parse tree (i.e. are recognized by the parser)
	  @param oflag non-zero if initialized within class code
	  @param pflag bitfield parse flag
	  @param lvids the number of new local variables declared in this node
	  @return new object
       */
      DLLEXPORT virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids) { return this; }

      //! evaluates the object and returns a value (or 0)
      /** return value requires a deref(xsink) (if not 0).  If needs_eval() returns false,
	  then this function just returns refSelf().  Otherwise evalImpl() is returned.
	  @code
	  ReferenceHolder<AbstractQoreNode> value(n->eval(xsink));
	  if (!value) // note that if a qore-language exception occured, then value = 0
	     return 0;
	  ...
	  return value.release();
	  @endcode
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation, if non-0, must be dereferenced manually
	  @see ReferenceHolder
      */
      DLLEXPORT AbstractQoreNode *eval(ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  if needs_eval() is true, needs_deref = true, returns evalImpl()
	  otherwise needs_deref = false returns "this"
	  NOTE: do not use this function directly, use the QoreNodeEvalOptionalRefHolder class instead
	  @param needs_deref this is an output parameter, if needs_deref is true then the value returned must be dereferenced
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @see QoreNodeEvalOptionalRefHolder
      */
      DLLEXPORT AbstractQoreNode *eval(bool &needs_deref, ExceptionSink *xsink) const;

      //! evaluates the object and returns a 64-bit integer value
      /** if needs_eval() returns true, then returns bigIntEvalImpl() otherwise returns getAsBigInt()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation of the object
       */
      DLLEXPORT int64 bigIntEval(ExceptionSink *xsink) const;

      //! evaluates the object and returns an integer value
      /** if needs_eval() returns true, then returns integerEvalImpl() otherwise returns getAsInteger()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation of the object
       */
      DLLEXPORT int integerEval(ExceptionSink *xsink) const;

      //! evaluates the object and returns a boolean value
      /** if needs_eval() returns true, then returns boolEvalImpl() otherwise returns getAsBool()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation of the object
       */
      DLLEXPORT bool boolEval(ExceptionSink *xsink) const;

      //! evaluates the object and returns a floating-point value
      /** if needs_eval() returns true, then returns floatEvalImpl() otherwise returns getAsFloat()
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation of the object
       */
      DLLEXPORT double floatEval(ExceptionSink *xsink) const;

      //! returns true if the node represents a value
      /**
	 @return true if the object is a value, false if not
       */
      DLLLOCAL bool is_value() const {
	 return value;
      }

      //! decrements the reference count and calls derefImpl() if there_can_be_only_one is false, otherwise does nothing
      /** if there_can_be_only_one is false, calls derefImpl() and deletes the object when the reference count = 0.  
	  The ExceptionSink argument is needed for those types that could throw an exception when they are deleted (ex: QoreObject)
	  @param xsink if an error occurs, the Qore-language exception information will be added here
       */
      DLLEXPORT void deref(ExceptionSink *xsink);

      //! returns "this" with an incremented reference count
      /**
	 @return "this" with an incremented reference count
       */
      DLLEXPORT AbstractQoreNode *refSelf() const;

      //! increments the reference count
      DLLEXPORT void ref() const;

      //! returns true if the object is reference-counted
      DLLEXPORT bool isReferenceCounted() const { return there_can_be_only_one; }
};

//! The base class for all types in Qore expression trees that cannot throw an exception when deleted
/**
   This class adds the deref() function without an ExceptionSink argument.
 */
class SimpleQoreNode : public AbstractQoreNode {
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL SimpleQoreNode& operator=(const SimpleQoreNode&);

   public:
      //! constructor takes the type and value arguments
      DLLLOCAL SimpleQoreNode(qore_type_t t, bool n_value, bool n_needs_eval, bool n_there_can_be_only_one = false) : AbstractQoreNode(t, n_value, n_needs_eval, n_there_can_be_only_one) { }

      //! copy constructor
      DLLEXPORT SimpleQoreNode(const SimpleQoreNode &) : AbstractQoreNode(type, value, needs_eval_flag, there_can_be_only_one) { }

      //! decrements the reference count and deletes the object when references = 0
      /**
	 This function is not virtual and should be used when possible for SimpleQoreNode objects
       */
      DLLEXPORT void deref();
};

//! base class for simple value types
class SimpleValueQoreNode : public SimpleQoreNode {
   private:

   protected:
      //! should never be called for value types
      /** in debugging builds of the library, calls to this function will abort 
       */
      DLLEXPORT virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

      //! should never be called for value types
      /** in debugging builds of the library, calls to this function will abort 
       */
      DLLEXPORT virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

      //! should never be called for value types
      /** in debugging builds of the library, calls to this function will abort 
       */
      DLLEXPORT virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;

      //! should never be called for value types
      /** in debugging builds of the library, calls to this function will abort 
       */
      DLLEXPORT virtual int integerEvalImpl(ExceptionSink *xsink) const;

      //! should never be called for value types
      /** in debugging builds of the library, calls to this function will abort 
       */
      DLLEXPORT virtual bool boolEvalImpl(ExceptionSink *xsink) const;

      //! should never be called for value types
      /** in debugging builds of the library, calls to this function will abort 
       */
      DLLEXPORT virtual double floatEvalImpl(ExceptionSink *xsink) const;

   public:
      //! creates the object by assigning the type code and setting the "value" flag, unsetting the "needs_eval" flag, and setting "there_can_be_only_one"
      DLLLOCAL SimpleValueQoreNode(qore_type_t t, bool n_there_can_be_only_one = false) : SimpleQoreNode(t, true, false, n_there_can_be_only_one) { }

      DLLLOCAL SimpleValueQoreNode(const SimpleValueQoreNode &v) : SimpleQoreNode(type, true, false, there_can_be_only_one) { }
};

//! this class is for value types that will exists only once in the Qore library, reference counting is disabled
/** these types must be statically allocated
 */
class UniqueValueQoreNode : public SimpleValueQoreNode {
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL UniqueValueQoreNode& operator=(const UniqueValueQoreNode&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t);

   protected:

   public:
      //! constructor takes the type argument
      DLLLOCAL UniqueValueQoreNode(qore_type_t t) : SimpleValueQoreNode(t, true) { }

      //! copy constructor
      DLLLOCAL UniqueValueQoreNode(const UniqueValueQoreNode &) : SimpleValueQoreNode(type, true) { }

      //! returns itself; objects of this type are not reference-counted and only deleted manually (by static destruction)
      DLLEXPORT virtual AbstractQoreNode *realCopy() const;
};

#endif
