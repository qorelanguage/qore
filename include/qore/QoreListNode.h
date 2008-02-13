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

//! This is the list container type in Qore
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
      /** concatenate the verbose string representation of the list (including all contained values, for %n and %N in print formatting), foff is for multi-line formatting offset, -1 = no line breaks
	  returns -1 for exception raised, 0 = OK
      */
      DLLEXPORT int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      //! returns a QoreString giving the verbose string representation of the List (including all contained values)
      /** get the verbose string representation of the list (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
	  if del is true, then the returned QoreString * should be deleted, if false, then it must not be
	  NOTE: Use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
      */
      DLLEXPORT QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! returns true if the list contains parse expressions and therefore needs evaluation to return a value, false if not
      DLLEXPORT virtual bool needs_eval() const;

      //! performs a deep copy of the list and returns the new list
      DLLEXPORT virtual class AbstractQoreNode *realCopy() const;

      //! tests for equality (including all contained values) with possible type conversion (soft compare)
      DLLEXPORT virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! tests for equality (including all contained values) without type conversions (hard compare)
      DLLEXPORT virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const;

      //! returns the data type
      DLLEXPORT virtual const QoreType *getType() const;

      //! returns the type name as a c string
      DLLEXPORT virtual const char *getTypeName() const;

      //! evaluates the object and returns a value (or 0)
      /** return value requires a deref(xsink)
	  default implementation = returns "refSelf()"
      */
      DLLEXPORT virtual class AbstractQoreNode *eval(class ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  default implementation = needs_deref = false, returns "this"
	  note: do not use this function directly, use the QoreNodeEvalOptionalRefHolder class instead
      */
      DLLEXPORT virtual class AbstractQoreNode *eval(bool &needs_deref, class ExceptionSink *xsink) const;

      // decrements the reference count
      /** deletes the object when the reference count = 0.  The ExceptionSink argument is needed for those types that could throw an exception when they are deleted (ex: QoreObject) - which could be contained in the list
       */
      DLLEXPORT virtual void deref(class ExceptionSink *xsink);

      //! returns true if the list does not contain any parse expressions, otherwise returns false
      DLLEXPORT virtual bool is_value() const;

      DLLEXPORT AbstractQoreNode *retrieve_entry(int index);
      DLLEXPORT const AbstractQoreNode *retrieve_entry(int index) const;
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

      //! returns the element having the lowest value (determined by calling OP_LOG_LT - the less-than operator)
      DLLEXPORT AbstractQoreNode *min() const;

      //! returns the element having the highest value (determined by calling OP_LOG_GT - the less-than operator)
      DLLEXPORT AbstractQoreNode *max() const;

      //! returns the element having the lowest value (determined by calling the function reference passed to give lexical order)
      DLLEXPORT AbstractQoreNode *min(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;

      //! returns the element having the highest value (determined by calling the function reference passed to give lexical order)
      DLLEXPORT AbstractQoreNode *max(const class ResolvedFunctionReferenceNode *fr, class ExceptionSink *xsink) const;
      DLLEXPORT void splice(int offset, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, class ExceptionSink *xsink);

      //! adds a single value or a list ("l") to list possition "offset", while removing "length" elements
      /** the "l" AbstractQoreNode will be referenced for the assignment in the QoreListNode
       */
      DLLEXPORT void splice(int offset, int length, const AbstractQoreNode *l, class ExceptionSink *xsink);

      //! returns the number of elements in the list
      DLLEXPORT int size() const;

      //! returns a list with the order of the elements reversed
      DLLEXPORT QoreListNode *reverse() const;

      //! returns "this" with an incremented reference count
      DLLEXPORT QoreListNode *listRefSelf() const;

      // needed only while parsing
      DLLLOCAL QoreListNode(bool i);
      DLLLOCAL bool isFinalized() const;
      DLLLOCAL void setFinalized();
      DLLLOCAL bool isVariableList() const;
      DLLLOCAL void setVariableList();
      DLLLOCAL void clearNeedsEval();
      DLLLOCAL void clear();
};

class StackList : public QoreListNode
{
   private:
      class ExceptionSink *xsink;
      
      // none of these operators/methods are implemented - here to make sure they are not used
      DLLLOCAL void *operator new(size_t); 
      DLLLOCAL StackList();
      DLLLOCAL StackList(bool i);
      DLLLOCAL StackList(const StackList&);
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

class TempList {
   private:
      QoreListNode *l;
      class ExceptionSink *xsink;

      // not implemented
      DLLLOCAL void *operator new(size_t); 
      DLLLOCAL TempList(const TempList&);
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

class ListIterator
{
   private:
      QoreListNode* l;
      int pos;
   
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

class ConstListIterator
{
   private:
      const QoreListNode* l;
      int pos;
   
   public:
      DLLEXPORT ConstListIterator(const QoreListNode *lst);
      DLLEXPORT bool next();
      DLLEXPORT const AbstractQoreNode *getValue() const;
      DLLEXPORT class AbstractQoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT bool first() const;
      DLLEXPORT bool last() const;
};

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

      // not implemented
      DLLLOCAL QoreListNodeEvalOptionalRefHolder(const QoreListNodeEvalOptionalRefHolder&);
      DLLLOCAL QoreListNodeEvalOptionalRefHolder& operator=(const QoreListNodeEvalOptionalRefHolder&);
      DLLLOCAL void *operator new(size_t);

   public:
      DLLLOCAL QoreListNodeEvalOptionalRefHolder(ExceptionSink *n_xsink) : xsink(n_xsink)
      {
	 needs_deref = false;
	 val = 0;
      }
      DLLLOCAL QoreListNodeEvalOptionalRefHolder(const QoreListNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink)
      {
	 needs_deref = false;
	 val = exp ? exp->evalList(needs_deref, xsink) : 0;
      }
      DLLLOCAL ~QoreListNodeEvalOptionalRefHolder()
      {
	 discard_intern();
      }
      DLLLOCAL void discard()
      {
	 discard_intern();
	 needs_deref = false;
	 val = 0;
      }
      DLLLOCAL void assign(bool n_needs_deref, QoreListNode *n_val)
      {
	 discard_intern();
	 needs_deref = n_needs_deref;
	 val = n_val;
      }
      // returns a referenced value - the caller will own the reference
      DLLLOCAL QoreListNode *getReferencedValue()
      {
	 if (needs_deref)
	    needs_deref = false;
	 else if (val)
	    val->ref();
	 return val;
      }
      // takes the referenced value and leaves this object empty, value is referenced if necessary
      DLLLOCAL QoreListNode *takeReferencedValue()
      {
	 QoreListNode *rv = val;
	 if (val && !needs_deref)
	    rv->ref();
	 val = 0;
	 needs_deref = false;
	 return rv;
      }
      DLLLOCAL QoreListNode *operator->() { return val; }
      DLLLOCAL QoreListNode *operator*() { return val; }
      DLLLOCAL operator bool() const { return val != 0; }
};

#endif
