/*
  List.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

  FIXME: require derefence before delete

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

#ifndef _QORE_LIST_H

#define _QORE_LIST_H

class List {   
   protected:
      class QoreNode **entry;
      int length;
      int allocated;
      bool needs_eval;

      DLLLOCAL void resize(int num);
      DLLLOCAL void splice_intern(int offset, int length, class ExceptionSink *xsink);
      DLLLOCAL void splice_intern(int offset, int length, class QoreNode *l, class ExceptionSink *xsink);
      DLLLOCAL void check_offset(int &offset);
      DLLLOCAL void check_offset(int &offset, int &len);
      DLLLOCAL void deref_intern(class ExceptionSink *xisnk);
      // qsort sorts the list in-place (unstable)
      DLLLOCAL int qsort(class QoreProgram *pgm, class UserFunction *f, int left, int right, class ExceptionSink *xsink);
      // mergesort sorts the list in-place (stable)
      DLLLOCAL int mergesort(class QoreProgram *pgm, class UserFunction *f, class ExceptionSink *xsink);
      DLLEXPORT ~List();

   public:
      DLLEXPORT List();
      DLLEXPORT List(bool i);
      DLLEXPORT class QoreNode *retrieve_entry(int index) const;
      DLLEXPORT int getEntryAsInt(int index) const;
      DLLEXPORT class QoreNode **get_entry_ptr(int index);
      DLLEXPORT class QoreNode **getExistingEntryPtr(int index);
      DLLEXPORT void set_entry(int index, class QoreNode *val, class ExceptionSink *xsink);
      DLLEXPORT class QoreNode *eval_entry(int num, class ExceptionSink *xsink) const;
      DLLEXPORT void push(class QoreNode *val);
      DLLEXPORT void insert(class QoreNode *val);
      DLLEXPORT class QoreNode *pop();
      DLLEXPORT class QoreNode *shift();
      DLLEXPORT void merge(class List *list);
      DLLEXPORT int delete_entry(int index, class ExceptionSink *xsink);
      DLLEXPORT void pop_entry(int index, class ExceptionSink *xsink);
      DLLEXPORT void dereference(class ExceptionSink *xsink);
      DLLEXPORT void derefAndDelete(class ExceptionSink *xsink);
      DLLEXPORT class List *evalList(class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *evalFrom(int offset, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *copy() const;
      DLLEXPORT class List *copyList() const;
      DLLEXPORT class List *copyListFrom(int offset) const;
      DLLEXPORT class QoreNode *sort() const;
      DLLEXPORT class QoreNode *sortDescending() const;
      DLLEXPORT class QoreNode *sort(char *sort_function_name, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *sortStable() const;
      DLLEXPORT class QoreNode *sortDescendingStable() const;
      DLLEXPORT class QoreNode *sortStable(char *sort_function_name, class ExceptionSink *xsink) const;
      DLLEXPORT void splice(int offset, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, class ExceptionSink *xsink);
      DLLEXPORT void splice(int offset, int length, class QoreNode *l, class ExceptionSink *xsink);
      DLLEXPORT int size() const;
      DLLEXPORT bool needsEval() const;
      DLLEXPORT void clearNeedsEval();
};

class StackList : public List
{
private:
   class ExceptionSink *xsink;
   
   // none of these operators/methods are implemented - here to make sure they are not used
   DLLLOCAL void *operator new(size_t); 
   DLLLOCAL StackList();
   DLLLOCAL StackList(bool i);
   DLLLOCAL void deleteAndDeref(class ExceptionSink *xsink);
   
public:
   DLLEXPORT StackList(class ExceptionSink *xs)
   {
      xsink = xs;
   }
   DLLEXPORT ~StackList()
   {
      dereference(xsink);
   }
   DLLEXPORT class QoreNode *getAndClear(int i);
};

class TempList {
   private:
      class List *l;
      class ExceptionSink *xsink;

   public:
      DLLEXPORT TempList(class List *lst, class ExceptionSink *xs) : l(lst), xsink(xs)
      {
      }
      DLLEXPORT ~TempList()
      {
	 if (l)
	    l->derefAndDelete(xsink);
      }
      DLLEXPORT List *operator->(){ return l; };
      DLLEXPORT List *operator*() { return l; };
      DLLEXPORT operator bool() const { return l != 0; }
};

class ListIterator
{
   private:
      class List* l;
      int pos;
#ifdef DEBUG
      int m_list_size; // used to verify that the list is not changed during iteration
#endif
   
   public:
      DLLEXPORT ListIterator(class List *lst);
      DLLEXPORT bool next();
      DLLEXPORT class QoreNode *getValue() const;
      DLLEXPORT class QoreNode **getValuePtr() const;
      DLLEXPORT class QoreNode *eval(class ExceptionSink *xsink) const;
      DLLEXPORT bool last() const;
      //void setValue(class QoreNode *val, class ExceptionSink *xsink);
};

#endif
