/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreValueList.h

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

#ifndef _QORE_QOREVALUELIST_H

#define _QORE_QOREVALUELIST_H

#include <qore/AbstractQoreNode.h>

class ResolvedCallReferenceNode;

//! This is the list container type in Qore, dynamically allocated only, reference counted
/**
   it is both a value type and can hold parse expressions as well (in which case it needs to be evaluated)
   the first element in the list is element 0
 */
class QoreValueList : public AbstractQoreNode {   
   friend struct qore_value_list_private;

private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreValueList(const QoreValueList&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreValueList& operator=(const QoreValueList&);

protected:
   //! this structure holds the private implementation for the type
   /** therefore changes to the implementation will not affect the C++ ABI 
    */
   struct qore_value_list_private *priv;

   //! the destructor is protected so it cannot be called directly
   /** use the deref(ExceptionSink) function to release the reference count
       @see AbstractQoreNode::deref()
       @see QoreValueList::derefImpl()
   */
   DLLEXPORT virtual ~QoreValueList();

   //! dereferences all elements of the list
   /** The ExceptionSink argument is needed for those types that could throw
       an exception when they are deleted (ex: QoreObject) - which could be 
       contained in the list
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return true if the object can be deleted, false if not (externally-managed)
   */
   DLLEXPORT virtual bool derefImpl(ExceptionSink* xsink);

   //! evaluates the list and returns a value (or 0)
   /** return value requires a deref(xsink)
       NOTE: if there is an exception, 0 will be returned
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT virtual AbstractQoreNode* evalImpl(ExceptionSink* xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode* evalImpl(bool &needs_deref, ExceptionSink* xsink) const;

   //! always returns 0
   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink* xsink) const;

   //! always returns 0
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink* xsink) const;

   //! always returns false
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink* xsink) const;

   //! always returns 0.0
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink* xsink) const;

public:
   DLLEXPORT QoreValueList();

   //! returns false unless perl-boolean-evaluation is enabled, in which case it returns false only when empty
   /** @return false unless perl-boolean-evaluation is enabled, in which case it returns false only when empty
    */
   DLLEXPORT virtual bool getAsBoolImpl() const;

   //! concatenate the verbose string representation of the list (including all contained values) to an existing QoreString
   /** used for %n and %N printf formatting
       @param str the string representation of the type will be concatenated to this QoreString reference
       @param foff for multi-line formatting offset, -1 = no line breaks
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @return -1 for exception raised, 0 = OK
   */
   DLLEXPORT int getAsString(QoreString &str, int foff, ExceptionSink* xsink) const;

   //! returns a QoreString giving the verbose string representation of the List (including all contained values)
   /** used for %n and %N printf formatting
       @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
       @param foff for multi-line formatting offset, -1 = no line breaks
       @param xsink if an error occurs, the Qore-language exception information will be added here
       NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
       @see QoreNodeAsStringHelper
   */
   DLLEXPORT QoreString* getAsString(bool &del, int foff, ExceptionSink* xsink) const;

   //! returns true if the list contains parse expressions and therefore needs evaluation to return a value, false if not
   //DLLEXPORT virtual bool needs_eval() const;

   //! performs a deep copy of the list and returns the new list
   DLLEXPORT virtual AbstractQoreNode* realCopy() const;

   //! tests for equality ("deep compare" including all contained values) with possible type conversion (soft compare)
   /**
      @param v the value to compare
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const;

   //! tests for equality ("deep compare" including all contained values) without type conversions (hard compare)
   /**
      @param v the value to compare
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const;

   //! returns the type name as a c string
   DLLEXPORT virtual const char* getTypeName() const;

   //! returns true if the list does not contain any parse expressions, otherwise returns false
   //DLLEXPORT virtual bool is_value() const;

   DLLLOCAL static const char* getStaticTypeName() {
      return "list";
   }

   //! returns the element at "index" (first element is index 0)
   /** the value is not referenced for the caller
       @param index the index of the element (first element is index 0)
       @return the value of the element at "index", not referenced for the caller
   */
   DLLEXPORT QoreValue retrieveEntry(size_t index);

   //! returns the element at "index" (first element is index 0)
   /** the value is not referenced for the caller
       @param index the index of the element (first element is index 0)
       @return the value of the element at "index", not referenced for the caller
   */
   DLLEXPORT const QoreValue retrieveEntry(size_t index) const;

   //! returns the element at "index" (first element is index 0), the caller owns the reference
   /**
      @param index the index of the element (first element is index 0)
      @return the value of the element at "index" with an incremented reference count for the caller
   */
   DLLEXPORT QoreValue getReferencedEntry(size_t index) const;

   /**
      @param index the index of the element (first element is index 0)
   */
   DLLEXPORT QoreValue& getEntryReference(size_t index);

   /**
      @param index the index of the element (first element is index 0)
   */
   DLLEXPORT QoreValue* getExistingEntryPtr(size_t index);

   //! sets the value of a list element
   /**
      if there is a value there already, it is dereferenced (hence "xsink" is needed to
      catch any exceptions)
      @param index the index of the element (first element is index 0)
      @param val the value to set, must be already referenced for the assignment (or 0)
      @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT void setEntry(size_t index, QoreValue val, ExceptionSink* xsink);

   DLLEXPORT void push(QoreValue val);
   DLLEXPORT void insert(QoreValue val);

   //! returns the last element of the list, the length is decremented by one, caller owns the reference
   /** if the list is empty the 0 is returned (NOTE: the last entry could also be 0 as well)
    */
   DLLEXPORT QoreValue pop();

   //! returns the first element of the list, all other entries are moved down to fill up the first position, caller owns the reference
   /** if the list is empty the 0 is returned (NOTE: the first entry could also be 0 as well)
       with the current implementation the execution time for this function is O(n) where n is the length of the list
   */
   DLLEXPORT QoreValue shift();

   //! appends the elements of "list" to this list
   DLLEXPORT void merge(const QoreValueList* list);

   /**
      @param index the index of the element (first element is index 0)
      @param xsink if an error occurs, the Qore-language exception information will be added here

      @note cannot be used when reference count > 1
   */
   //DLLEXPORT void popEntry(size_t index, ExceptionSink* xsink);

   //! evaluates the list and returns a value (or 0)
   /** return value requires a deref(xsink)
       if the list does not require evaluation then "refSelf()" is used to 
       return the same object with an incremented reference count
       NOTE: if the object requires evaluation and there is an exception, 0 will be returned
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreValueList* evalList(ExceptionSink* xsink) const;

   //! optionally evaluates the list
   /** return value requires a deref(xsink) if needs_deref is true
       NOTE: if the list requires evaluation and there is an exception, 0 will be returned
       NOTE: do not use this function directly, use the QoreValueListEvalOptionalRefHolder class instead
       @param needs_deref this is an output parameter, if needs_deref is true then the value returned must be dereferenced	  
       @param xsink if an error occurs, the Qore-language exception information will be added here
       @see QoreValueListEvalOptionalRefHolder
   */
   DLLEXPORT QoreValueList* evalList(bool &needs_deref, ExceptionSink* xsink) const;

   //! performs a deep copy of the list and returns the new list
   DLLEXPORT QoreValueList* copy() const;

   //! performs a deep copy of the list starting from element "offset" and returns the new list
   /** therefore element 0 of the new list is element "offset" in the source list
       @param index the index of the element (first element is index 0)
   */
   DLLEXPORT QoreValueList* copyListFrom(size_t index) const;

   //! returns a new list based on quicksorting the source list ("this")
   /** "soft" comparisons are made using OP_LOG_LT, meaning that the list can be made up of 
       different data types and still be sorted
   */
   DLLEXPORT QoreValueList* sort(ExceptionSink* xsink) const;

   //! returns a new list based on quicksorting the source list ("this") using the passed function reference to determine lexical order
   /** 
       @param fr the function reference to be executed for each comparison to give lexical order to the elements
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreValueList* sort(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const;

   //! returns a new list based on executing mergesort on the source list ("this")
   /** "soft" comparisons are made using OP_LOG_LT, meaning that the list can be made up of 
       different data types and still be sorted
   */
   DLLEXPORT QoreValueList* sortStable(ExceptionSink* xsink) const;

   //! returns a new list based on executing mergesort on the source list ("this") using the passed function reference to determine lexical order
   /** 
       @param fr the function reference to be executed for each comparison to give lexical order to the elements
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreValueList* sortStable(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const;

   //! returns a new list based on quicksorting the source list ("this") in descending order
   /** "soft" comparisons are made using OP_LOG_LT, meaning that the list can be made up of 
       different data types and still be sorted
   */
   DLLEXPORT QoreValueList* sortDescending(ExceptionSink* xsink) const;

   //! returns a new list based on quicksorting the source list ("this") in descending order, using the passed function reference to determine lexical order
   /** 
       @param fr the function reference to be executed for each comparison to give lexical order to the elements
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreValueList* sortDescending(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const;

   //! returns a new list based on executing mergesort on the source list ("this") in descending order
   /** "soft" comparisons are made using OP_LOG_LT, meaning that the list can be made up of 
       different data types and still be sorted
   */
   DLLEXPORT QoreValueList* sortDescendingStable(ExceptionSink* xsink) const;

   //! returns a new list based on executing mergesort on the source list ("this") in descending order, using the passed function reference to determine lexical order
   /** 
       @param fr the function reference to be executed for each comparison to give lexical order to the elements
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreValueList* sortDescendingStable(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const;

   //! returns the element having the lowest value (determined by calling OP_LOG_LT - the less-than "<" operator)
   /** so "soft" comparisons are made, meaning that the list can be made up of different types, and, as long
       as the comparisons are meaningful, the minimum value can be returned
   */
   DLLEXPORT QoreValue minValue(ExceptionSink* xsink) const;

   //! returns the element having the highest value (determined by calling OP_LOG_GT - the greater-than ">" operator)
   /** so "soft" comparisons are made, meaning that the list can be made up of different types, and, as long
       as the comparisons are meaningful, the maximum value can be returned
   */
   DLLEXPORT QoreValue maxValue(ExceptionSink* xsink) const;

   //! returns the element having the lowest value (determined by calling the function reference passed to give lexical order)
   /** 
       @param fr the function reference to be executed for each comparison to give lexical order to the elements
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreValue minValue(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const;

   //! returns the element having the highest value (determined by calling the function reference passed to give lexical order)
   /** 
       @param fr the function reference to be executed for each comparison to give lexical order to the elements
       @param xsink if an error occurs, the Qore-language exception information will be added here
   */
   DLLEXPORT QoreValue maxValue(const ResolvedCallReferenceNode* fr, ExceptionSink* xsink) const;

   //! truncates the list at position "offset" (first element is offset 0)
   /**
      @param offset the index of the element (first element is offset 0, negative offsets are offsets from the end of the list)
      @param xsink if an error occurs, the Qore-language exception information will be added here

      @return the elements removed from the list as a new list; if no elements are removed, 0 is returned
   */
   DLLEXPORT QoreValueList* splice(ptrdiff_t offset, ExceptionSink* xsink);

   //! removes "length" elements at position "offset" (first element is offset 0)
   /**
      @param offset the index of the element (first element is offset 0, negative offsets are offsets from the end of the list)
      @param length the number of elements to remove (negative numbers mean all except that many elements from the end)
      @param xsink if an error occurs, the Qore-language exception information will be added here

      @return the elements removed from the list as a new list; if no elements are removed, 0 is returned
   */
   DLLEXPORT QoreValueList* splice(ptrdiff_t offset, ptrdiff_t length, ExceptionSink* xsink);

   //! adds a single value or a list of values ("l") to list possition "offset", while removing "length" elements
   /** the "l" AbstractQoreNode (or each element if it is a QoreValueList) will be referenced for the assignment in the QoreValueList
    */
   /**
      @param offset the index of the element (first element is offset 0, negative offsets are offsets from the end of the list)
      @param length the number of elements to remove (negative numbers mean all except that many elements from the end)
      @param l the value or list of values to insert
      @param xsink if an error occurs, the Qore-language exception information will be added here

      @return the elements removed from the list as a new list; if no elements are removed, 0 is returned
   */
   DLLEXPORT QoreValueList* splice(ptrdiff_t offset, ptrdiff_t length, const QoreValue l, ExceptionSink* xsink);

   //! truncates the list at position "offset" (first element is offset 0) and returns any elements removed from the list as a new list
   /**
      @param offset the index of the element (first element is offset 0, negative offsets are offsets from the end of the list)
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return the elements removed from the list as a new list; if no elements are removed, an empty list is returned
   */
   DLLEXPORT QoreValueList* extract(ptrdiff_t offset, ExceptionSink* xsink);

   //! removes "length" elements at position "offset" (first element is offset 0) and returns any elements removed from the list as a new list
   /**
      @param offset the index of the element (first element is offset 0, negative offsets are offsets from the end of the list)
      @param length the number of elements to remove (negative numbers mean all except that many elements from the end)
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return the elements removed from the list as a new list; if no elements are removed, an empty list is returned
   */
   DLLEXPORT QoreValueList* extract(ptrdiff_t offset, ptrdiff_t length, ExceptionSink* xsink);

   //! adds a single value or a list of values ("l") to list possition "offset", while removing "length" elements and returns any elements removed from the list as a new list
   /** the "l" AbstractQoreNode (or each element if it is a QoreValueList) will be referenced for the assignment in the QoreValueList
    */
   /**
      @param offset the index of the element (first element is offset 0, negative offsets are offsets from the end of the list)
      @param length the number of elements to remove (negative numbers mean all except that many elements from the end)
      @param l the value or list of values to insert
      @param xsink if an error occurs, the Qore-language exception information will be added here
      @return the elements removed from the list as a new list; if no elements are removed, an empty list is returned
   */
   DLLEXPORT QoreValueList* extract(ptrdiff_t offset, ptrdiff_t length, const QoreValue l, ExceptionSink* xsink);

   //! returns the number of elements in the list
   /** return the number of elements in the list
    */
   DLLEXPORT size_t size() const;

   //! returns true if the list is empty
   /** return true if the list is empty
    */
   DLLEXPORT bool empty() const;

   //! returns a list with the order of the elements reversed
   DLLEXPORT QoreValueList* reverse() const;

   //! returns "this" with an incremented reference count
   DLLEXPORT QoreValueList* listRefSelf() const;

   //! swaps the value at the given position with the value given, caller owns any reference returned
   DLLEXPORT QoreValue swap(ptrdiff_t offset, QoreValue val);

   //! takes the value at the given position and replaces with NULL, only if the list position exists
   DLLEXPORT QoreValue takeExists(ptrdiff_t offset);
};

#include <qore/ReferenceHolder.h>

//! For use on the stack only: manages a QoreValueList reference count
/**
   @see ReferenceHolder
*/
typedef ReferenceHolder<QoreValueList> QoreValueListHolder;

//! For use on the stack only: iterates through a the elements of a QoreValueList
/**
   @code
   // iterate forward through the list
   ListIterator li(l);
   while (li.next()) {
      QoreStringValueHelper str(li.getValue());
      printf("%d: '%s'\n", li.index(), str->getBuffer());
   }
   @endcode
   @code
   // iterate backwards through the list
   ListIterator li(l);
   while (li.prev()) {
      QoreStringValueHelper str(li.getValue());
      printf("%d: '%s'\n", li.index(), str->getBuffer());
   }
   @endcode
   @see ConstListIterator
*/
class ValueListIterator {
protected:
   QoreValueList* l;
   ptrdiff_t pos;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t); 
   
public:
   //! initializes the iterator to the position given or, if omitted, just before the first element
   /** @param lst the list to iterate
       @param n_pos the starting position (-1 means just before the first element so that the initial call to next() or prev() will put the iterator on the first element)
   */
   DLLEXPORT ValueListIterator(QoreValueList* lst, size_t n_pos = -1);

   //! initializes the iterator to the position given or, if omitted, just before the first element
   /** @param lst the list to iterate
       @param n_pos the starting position (-1 means just before the first element so that the initial call to next() or prev() will put the iterator on the first element)
   */
   DLLEXPORT ValueListIterator(QoreValueList& lst, size_t n_pos = -1);

   //! moves the iterator to the next element, returns true if the iterator is pointing to an element of the list
   /** if the iterator is on the last element, it moves to an invalid position before the first element and returns false
       note that a subsequent call to next() after it returns false will move the iterator to the first element again
       (assuming there is at least one element in the list)
       @return returns true if the iterator has been moved to point to a valid element of the list, false if there are no more elements to iterate
   */
   DLLEXPORT bool next();

   //! moves the iterator to the previous element, returns true if the iterator is pointing to an element of the list
   /** if the iterator is on the first element, it moves to an invalid position before the first element and returns false
       note that a subsequent call to prev() after it returns false will move the iterator to the last element again
       (assuming there is at least one element in the list)
       @return returns true if the iterator has been moved to point to a valid element of the list, false if there are no more elements to iterate
       @note after this function returns false, do not use the iterator until it points to a valid element, otherwise a crash will result
   */
   DLLEXPORT bool prev();

   //! sets the iterator to a specific position in the list
   /** In the case an invalid position is given (element not present in the list), the iterator will not be pointing to a valid element in the list
       @param n_pos the position in the list to set (first element is position 0)
       @return 0 for OK, -1 for invalid position
       @note if this function returns -1, do not use the iterator until it points to a valid element, otherwise a crash will result
   */
   DLLEXPORT int set(size_t n_pos);

   //! returns a pointer to the value of the list element
   DLLEXPORT QoreValue getValue() const;

   //! returns a pointer to a pointer of the value of the list element, so it can be changed externally
   DLLEXPORT QoreValue* getValueReference() const;

   //! returns the current value with an incremented reference count
   DLLEXPORT QoreValue getReferencedValue() const;

   //! returns true when the iterator is pointing to the first element in the list
   DLLEXPORT bool first() const;

   //! returns true when the iterator is pointing to the last element in the list
   DLLEXPORT bool last() const;

   //DLLEXPORT void setValue(AbstractQoreNode* val, ExceptionSink* xsink) const;

   //! returns the current iterator position in the list or -1 if not pointing at a valid element
   DLLLOCAL size_t index() const { return pos; }

   //! returns the number of elements in the list
   DLLLOCAL size_t max() const { return l->size(); }

   //! returns the list
   DLLLOCAL QoreValueList* getList() { return l; }

   //! returns true if the list is empty
   DLLLOCAL bool empty() const {
      return l->empty();
   }

   //! returns true if the iterator is pointing to a valid element
   DLLLOCAL bool valid() const {
      return pos == -1 ? false : true;
   }
};

//! For use on the stack only: iterates through elements of a const QoreValueList
/**
   @code
   // iterate forward through the list
   ConstValueListIterator li(l);
   while (li.next()) {
      QoreStringValueHelper str(li.getValue());
      printf("%d: '%s'\n", li.index(), str->getBuffer());
   }
   @endcode
   @code
   // iterate backwards through the list
   ConstValueListIterator li(l);
   while (li.prev()) {
      QoreStringValueHelper str(li.getValue());
      printf("%d: '%s'\n", li.index(), str->getBuffer());
   }
   @endcode
   @see ValueListIterator
*/
class ConstValueListIterator {
protected:
   const QoreValueList* l;
   ptrdiff_t pos;

public:
   //! initializes the iterator to the position given or, if omitted, just before the first element
   /** @param lst the list to iterate
       @param n_pos the starting position (-1 means just before the first element so that the initial call to next() or prev() will put the iterator on the first element)
   */
   DLLEXPORT ConstValueListIterator(const QoreValueList* lst, size_t n_pos = -1);

   //! initializes the iterator to the position given or, if omitted, just before the first element
   /** @param lst the list to iterate
       @param n_pos the starting position (-1 means just before the first element so that the initial call to next() or prev() will put the iterator on the first element)
   */
   DLLEXPORT ConstValueListIterator(const QoreValueList& lst, size_t n_pos = -1);

   //! moves the iterator to the next element, returns true if the iterator is pointing to an element of the list
   /** if the iterator is on the last element, it moves to an invalid position before the first element and returns false
       note that a subsequent call to next() after it returns false will move the iterator to the first element again
       (assuming there is at least one element in the list)
       @return returns true if the iterator has been moved to point to a valid element of the list, false if there are no more elements to iterate
   */
   DLLEXPORT bool next();

   //! moves the iterator to the previous element, returns true if the iterator is pointing to an element of the list
   /** if the iterator is on the first element, it moves to an invalid position before the first element and returns false
       note that a subsequent call to prev() after it returns false will move the iterator to the last element again
       (assuming there is at least one element in the list)
       @return returns true if the iterator has been moved to point to a valid element of the list, false if there are no more elements to iterate
       @note after this function returns false, do not use the iterator until it points to a valid element, otherwise a crash will result
   */
   DLLEXPORT bool prev();

   //! sets the iterator to a specific position in the list
   /** In the case an invalid position is given (element not present in the list), the iterator will not be pointing to a valid element in the list
       @param n_pos the position in the list to set (first element is position 0)
       @return 0 for OK, -1 for invalid position
       @note if this function returns -1, do not use the iterator until it points to a valid element, otherwise a crash will result
   */
   DLLEXPORT int set(size_t n_pos);

   //! returns a pointer to the value of the list element
   DLLEXPORT const QoreValue getValue() const;

   //! returns the current value with an incremented reference count
   DLLEXPORT QoreValue getReferencedValue() const;

   //! returns true when the iterator is pointing to the first element in the list
   DLLEXPORT bool first() const;

   //! returns true when the iterator is pointing to the last element in the list
   DLLEXPORT bool last() const;

   //! returns the current iterator position in the list or -1 if not pointing at a valid element
   DLLLOCAL size_t index() const { return pos; }

   //! returns the number of elements in the list
   DLLLOCAL size_t max() const { return l->size(); }

   //! returns the list
   DLLLOCAL const QoreValueList* getList() const { return l; }

   //! returns true if the list is empty
   DLLLOCAL bool empty() const {
      return l->empty();
   }

   //! returns true if the iterator is pointing to a valid element
   DLLLOCAL bool valid() const {
      return pos == -1 ? false : true;
   }

   //! resets the iterator to its initial state
   DLLEXPORT void reset();
};

//! For use on the stack only: manages result of the optional evaluation of a QoreValueList
class QoreValueListEvalOptionalRefHolder {
private:
   QoreValueList* val;
   ExceptionSink* xsink;
   bool needs_deref;

   DLLLOCAL void discard_intern() {
      if (needs_deref && val)
         val->deref(xsink);
   }

   DLLLOCAL void eval_intern(const QoreValueList* exp) {
      if (exp)
         val = exp->evalList(needs_deref, xsink);
      else {
         val = 0;
         needs_deref = false;
      }
   }

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreValueListEvalOptionalRefHolder(const QoreValueListEvalOptionalRefHolder&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreValueListEvalOptionalRefHolder& operator=(const QoreValueListEvalOptionalRefHolder&);
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL void *operator new(size_t);

public:
   //! initializes an empty object and saves the ExceptionSink object
   DLLLOCAL QoreValueListEvalOptionalRefHolder(ExceptionSink* n_xsink) : xsink(n_xsink) {
      needs_deref = false;
      val = 0;
   }

   //! performs an optional evaluation of the list (sets the dereference flag)
   DLLLOCAL QoreValueListEvalOptionalRefHolder(const QoreValueList* exp, ExceptionSink* n_xsink) : xsink(n_xsink) {
      eval_intern(exp);
   }

   //! clears the object (dereferences the old object if necessary)
   DLLLOCAL ~QoreValueListEvalOptionalRefHolder() {
      discard_intern();
   }

   //! clears the object (dereferences the old object if necessary)
   DLLLOCAL void discard() {
      discard_intern();
      needs_deref = false;
      val = 0;
   }

   //! assigns a new value by executing the given list and dereference flag to this object, dereferences the old object if necessary
   DLLLOCAL void assignEval(const QoreValueList* exp) {
      discard_intern();
      eval_intern(exp);
   }

   //! assigns a new value and dereference flag to this object, dereferences the old object if necessary
   DLLLOCAL void assign(bool n_needs_deref, QoreValueList* n_val) {
      discard_intern();
      needs_deref = n_needs_deref;
      val = n_val;
   }

   //! returns true if the object contains a temporary (evaluated) value that needs a dereference
   DLLLOCAL bool needsDeref() const {
      return needs_deref;
   }

   //! returns a referenced value - the caller will own the reference
   /**
      The list is referenced if necessary (if it was a temporary value)
      @return the list value, where the caller will own the reference count
   */
   DLLLOCAL QoreValueList* getReferencedValue() {
      if (needs_deref)
         needs_deref = false;
      else if (val)
         val->ref();
      return val;
   }

   //! will create a unique list so the list can be edited
   DLLLOCAL void edit() {
      if (!val) {
         val = new QoreValueList;
         needs_deref = true;
      }
      else if (!needs_deref || !val->is_unique()) {
         val = val->copy();
         needs_deref = true;
      }
   }

   //! returns a pointer to the QoreValueList object being managed
   /**
      if you need a referenced value, use getReferencedValue()
      @return a pointer to the QoreValueList object being managed (or 0 if none)
   */
   DLLLOCAL const QoreValueList* operator->() const { return val; }

   //! returns a pointer to the QoreValueList object being managed
   DLLLOCAL const QoreValueList* operator*() const { return val; }

   //! returns true if a QoreValueList object pointer is being managed, false if the pointer is 0
   DLLLOCAL operator bool() const { return val != 0; }
};

#endif
