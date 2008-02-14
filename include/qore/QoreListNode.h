/*
  QoreListNode.h

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

#ifndef _QORE_QORELISTNODE_H

#define _QORE_QORELISTNODE_H

#include <qore/AbstractQoreNode.h>

//! This is the list container type in Qore, dynamically allocated only, reference counted
/**
   it is both a value type and can hold parse expressions as well (in which case it needs to be evaluated)
 */
class QoreListNode : public AbstractQoreNode
{   
      friend class StackList;

   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreListNode(const QoreListNode&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreListNode& operator=(const QoreListNode&);

   protected:
      //! this structure holds the private implementation for the type
      /** therefore changes to the implementation will not affect the C++ ABI 
       */
      struct qore_list_private *priv;

      DLLLOCAL void resize(int num);
      DLLLOCAL void splice_intern(int offset, int length, class ExceptionSink *xsink);
      DLLLOCAL void splice_intern(int offset, int length, const AbstractQoreNode *l, class ExceptionSink *xsink);
      DLLLOCAL void check_offset(int &offset);
      DLLLOCAL void check_offset(int &offset, int &len);
      DLLLOCAL void deref_intern(class ExceptionSink *xisnk);

      //! qsort sorts the list in-place (unstable)
      DLLLOCAL int qsort(const class ResolvedFunctionReferenceNode *fr, int left, int right, bool ascending, class ExceptionSink *xsink);

      //! mergesort sorts the list in-place (stable)
      DLLLOCAL int mergesort(const class ResolvedFunctionReferenceNode *fr, bool ascending, class ExceptionSink *xsink);

      //! the destructor is protected so it cannot be called directly
      /**
	 use the deref(ExceptionSink) function to release the reference count
       */
      DLLEXPORT virtual ~QoreListNode();

   public:
      DLLEXPORT QoreListNode();

      //! concatenate the verbose string representation of the list (including all contained values) to an existing QoreString
      /** concatenate the verbose string representation of the list (including all contained values, for %n and %N in print formatting),
	  "foff" is for multi-line formatting offset, -1 = no line breaks
	  returns -1 for exception raised, 0 = OK
      */
      DLLEXPORT int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the List (including all contained values)
      /** get the verbose string representation of the list (for %n and %N), 
	  "foff" is for multi-line formatting offset, -1 = no line breaks
	  if "del" is true, then the returned QoreString * should be deleted, if false, then it must not be
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
      */
      DLLEXPORT QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! returns true if the list contains parse expressions and therefore needs evaluation to return a value, false if not
      DLLEXPORT virtual bool needs_eval() const;

      //! performs a deep copy of the list and returns the new list
      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      //! tests for equality ("deep compare" including all contained values) with possible type conversion (soft compare)
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality ("deep compare" including all contained values) without type conversions (hard compare)
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the data type
      DLLEXPORT virtual const QoreType *getType() const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      //! evaluates the object and returns a value (or 0)
      /** return value requires a deref(xsink)
	  if the object does not require evaluation then "refSelf()" is used to 
	  return the same object with an incremented reference count
	  NOTE: if the object requires evaluation and there is an exception, 0 will be returned
      */
      DLLEXPORT virtual class AbstractQoreNode *eval(class ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  NOTE: if the object requires evaluation and there is an exception, 0 will be returned
	  NOTE: do not use this function directly, use the QoreNodeEvalOptionalRefHolder class instead
      */
      DLLEXPORT virtual class AbstractQoreNode *eval(bool &needs_deref, class ExceptionSink *xsink) const;

      // decrements the reference count
      /** deletes the object when the reference count = 0.  The ExceptionSink 
	  argumentis needed for those types that could throw an exception when 
	  they are deleted (ex: QoreObject) - which could be contained in the list
       */
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);

      //! returns true if the list does not contain any parse expressions, otherwise returns false
      DLLEXPORT virtual bool is_value() const;

      //! returns the element at "index" (first element is index 0)
      DLLEXPORT AbstractQoreNode *retrieve_entry(int index);

      //! returns the element at "index" (first element is index 0)
      DLLEXPORT const AbstractQoreNode *retrieve_entry(int index) const;

      //! returns the value of element at "index" as an integer (first element is index 0)
      DLLEXPORT int getEntryAsInt(int index) const;

      DLLEXPORT AbstractQoreNode **get_entry_ptr(int index);
      DLLEXPORT AbstractQoreNode **getExistingEntryPtr(int index);
      DLLEXPORT void set_entry(int index, class AbstractQoreNode *val, class ExceptionSink *xsink);
      DLLEXPORT AbstractQoreNode *eval_entry(int num, class ExceptionSink *xsink) const;
      DLLEXPORT void push(class AbstractQoreNode *val);
      DLLEXPORT void insert(class AbstractQoreNode *val);
      DLLEXPORT AbstractQoreNode *pop();
      DLLEXPORT AbstractQoreNode *shift();
      DLLEXPORT void merge(const class QoreListNode *list);
      DLLEXPORT int delete_entry(int index, class ExceptionSink *xsink);
      DLLEXPORT void pop_entry(int index, class ExceptionSink *xsink);
      DLLEXPORT QoreListNode *evalList(class ExceptionSink *xsink) const;
      DLLEXPORT QoreListNode *evalList(bool &needs_deref, class ExceptionSink *xsink) const;
      DLLEXPORT QoreListNode *evalFrom(int offset, class ExceptionSink *xsink) const;
      DLLEXPORT QoreListNode *copy() const;
      DLLEXPORT QoreListNode *copyListFrom(int offset) const;
      DLLEXPORT QoreListNode *sort() const;
      DLLEXPORT QoreListNode *sort(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;
      DLLEXPORT QoreListNode *sortStable() const;
      DLLEXPORT QoreListNode *sortStable(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;
      DLLEXPORT QoreListNode *sortDescending() const;
      DLLEXPORT QoreListNode *sortDescending(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;
      DLLEXPORT QoreListNode *sortDescendingStable() const;
      DLLEXPORT QoreListNode *sortDescendingStable(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;

      //! returns the element having the lowest value (determined by calling OP_LOG_LT - the less-than "<" operator)
      /** so "soft" comparisons are made, meaning that the list can be made up of different types, and, as long
	  as the comparisons are meaningful, the minimum value can be returned
       */
      DLLEXPORT AbstractQoreNode *min() const;

      //! returns the element having the highest value (determined by calling OP_LOG_GT - the greater-than ">" operator)
      /** so "soft" comparisons are made, meaning that the list can be made up of different types, and, as long
	  as the comparisons are meaningful, the maximum value can be returned
       */
      DLLEXPORT AbstractQoreNode *max() const;

      //! returns the element having the lowest value (determined by calling the function reference passed to give lexical order)
      DLLEXPORT AbstractQoreNode *min(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;

      //! returns the element having the highest value (determined by calling the function reference passed to give lexical order)
      DLLEXPORT AbstractQoreNode *max(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;

      //! truncates the list at position "offset" (first element is offset 0)
      DLLEXPORT void splice(int offset, class ExceptionSink *xsink);

      //! removes "length" elements at position "offset" (first element is offset 0)
      DLLEXPORT void splice(int offset, int length, class ExceptionSink *xsink);

      //! adds a single value or a list of values ("l") to list possition "offset", while removing "length" elements
      /** the "l" AbstractQoreNode (or each element if it is a QoreListNode) will be referenced for the assignment in the QoreListNode
       */
      DLLEXPORT void splice(int offset, int length, const AbstractQoreNode *l, class ExceptionSink *xsink);

      //! returns the number of elements in the list
      DLLEXPORT int size() const;

      //! returns a list with the order of the elements reversed
      DLLEXPORT QoreListNode *reverse() const;

      //! returns "this" with an incremented reference count
      DLLEXPORT QoreListNode *listRefSelf() const;

      // needed only while parsing
      //! this function is not exported in the qore library
      DLLLOCAL QoreListNode(bool i);
      //! this function is not exported in the qore library
      DLLLOCAL bool isFinalized() const;
      //! this function is not exported in the qore library
      DLLLOCAL void setFinalized();
      //! this function is not exported in the qore library
      DLLLOCAL bool isVariableList() const;
      //! this function is not exported in the qore library
      DLLLOCAL void setVariableList();
      //! this function is not exported in the qore library
      DLLLOCAL void clearNeedsEval();
      //! this function is not exported in the qore library
      DLLLOCAL void clear();
};

//! For use on the stack only: allows QoreListNode to be allocated on the stack
/** the ExceptionSink object required for the deref() is passed to the constructor
 */
class StackList : public QoreListNode
{
   private:
      class ExceptionSink *xsink;
      
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t); 
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL StackList();
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL StackList(bool i);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL StackList(const StackList&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL StackList& operator=(const StackList&);
   
   public:
      DLLEXPORT StackList(class ExceptionSink *xs)
      {
	 xsink = xs;
      }
      DLLEXPORT ~StackList()
      {
	 deref_intern(xsink);
      }
      DLLEXPORT class AbstractQoreNode *getAndClear(int i);
};

//! For use on the stack only: manages a QoreListNode reference count
/** The QoreListNode object is dereferenced when this object is destroyed
 */
class TempList {
   private:
      QoreListNode *l;
      class ExceptionSink *xsink;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t); 
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL TempList(const TempList&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL TempList& operator=(const TempList&);

   public:
      DLLEXPORT TempList(QoreListNode *lst, class ExceptionSink *xs) : l(lst), xsink(xs)
      {
      }
      DLLEXPORT ~TempList()
      {
	 if (l)
	    l->deref(xsink);
      }
      DLLEXPORT QoreListNode *operator->(){ return l; };
      DLLEXPORT QoreListNode *operator*() { return l; };
      DLLEXPORT operator bool() const { return l != 0; }
      DLLLOCAL QoreListNode *release() { QoreListNode *rv = l; l = 0; return rv; }
};

//! For use on the stack only: iterates through a QoreListNode's elements
class ListIterator
{
   private:
      QoreListNode* l;
      int pos;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t); 
   
   public:
      DLLEXPORT ListIterator(QoreListNode *lst);
      DLLEXPORT bool next();
      DLLEXPORT AbstractQoreNode *getValue() const;
      DLLEXPORT AbstractQoreNode **getValuePtr() const;
      DLLEXPORT class AbstractQoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT bool first() const;
      DLLEXPORT bool last() const;
      //DLLEXPORT void setValue(class AbstractQoreNode *val, class ExceptionSink *xsink) const;
};

//! For use on the stack only: iterates through a const QoreListNode's elements
class ConstListIterator
{
   private:
      const QoreListNode* l;
      int pos;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t); 
   
   public:
      DLLEXPORT ConstListIterator(const QoreListNode *lst);
      DLLEXPORT bool next();
      DLLEXPORT const AbstractQoreNode *getValue() const;
      DLLEXPORT class AbstractQoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT bool first() const;
      DLLEXPORT bool last() const;
};

//! For use on the stack only: manages result of the optional evaluation of a QoreListNode
class QoreListNodeEvalOptionalRefHolder {
   private:
      QoreListNode *val;
      ExceptionSink *xsink;
      bool needs_deref;

      DLLLOCAL void discard_intern()
      {
	 if (needs_deref && val)
	    val->deref(xsink);
      }

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreListNodeEvalOptionalRefHolder(const QoreListNodeEvalOptionalRefHolder&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreListNodeEvalOptionalRefHolder& operator=(const QoreListNodeEvalOptionalRefHolder&);
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL void *operator new(size_t);

   public:
      //! initializes an empty object and saves the ExceptionSink object
      DLLLOCAL QoreListNodeEvalOptionalRefHolder(ExceptionSink *n_xsink) : xsink(n_xsink)
      {
	 needs_deref = false;
	 val = 0;
      }

      //! performs an optional evaluation of the list (sets the dereference flag)
      DLLLOCAL QoreListNodeEvalOptionalRefHolder(const QoreListNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink)
      {
	 needs_deref = false;
	 val = exp ? exp->evalList(needs_deref, xsink) : 0;
      }

      //! clears the object (dereferences the old object if necessary)
      DLLLOCAL ~QoreListNodeEvalOptionalRefHolder()
      {
	 discard_intern();
      }

      //! clears the object (dereferences the old object if necessary)
      DLLLOCAL void discard()
      {
	 discard_intern();
	 needs_deref = false;
	 val = 0;
      }

      //! assigns a new value and dereference flag to this object, dereferences the old object if necessary
      DLLLOCAL void assign(bool n_needs_deref, QoreListNode *n_val)
      {
	 discard_intern();
	 needs_deref = n_needs_deref;
	 val = n_val;
      }

      //! returns a referenced value - the caller will own the reference
      DLLLOCAL QoreListNode *getReferencedValue()
      {
	 if (needs_deref)
	    needs_deref = false;
	 else if (val)
	    val->ref();
	 return val;
      }

      //! takes the referenced value and leaves this object empty, value is referenced if necessary
      DLLLOCAL QoreListNode *takeReferencedValue()
      {
	 QoreListNode *rv = val;
	 if (val && !needs_deref)
	    rv->ref();
	 val = 0;
	 needs_deref = false;
	 return rv;
      }

      //! returns a pointer to the QoreListNode object being managed
      DLLLOCAL QoreListNode *operator->() { return val; }

      //! returns a pointer to the QoreListNode object being managed
      DLLLOCAL QoreListNode *operator*() { return val; }

      //! returns true if a QoreListNode object pointer is being managed, false if the pointer is 0
      DLLLOCAL operator bool() const { return val != 0; }
};

#endif
