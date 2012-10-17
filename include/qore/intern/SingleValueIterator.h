/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SingleValueIterator.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

#ifndef _QORE_SINGLEVALUEITERATOR_H

#define _QORE_SINGLEVALUEITERATOR_H

// the c++ object
class SingleValueIterator : public QoreIteratorBase {
protected:
   AbstractQoreNode* val;
   bool validp;

public:
   DLLLOCAL SingleValueIterator(const AbstractQoreNode* v) : val(!is_nothing(v) ? v->refSelf() : 0), validp(false) {
   }

   DLLLOCAL SingleValueIterator(const SingleValueIterator& old) : val(old.val ? old.val->refSelf() : 0), validp(old.validp) {
   }

   DLLLOCAL bool next() {
      if (!val)
         return false;
      return (validp = !validp);
   }

   DLLLOCAL int checkValid(ExceptionSink* xsink) const {
      if (!validp) {
         xsink->raiseException("ITERATOR-ERROR", "the %s is not pointing at a valid element; make sure %s::next() returns True before calling this method", getName(), getName());
         return -1;
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode* getValue() {
      return val ? val->refSelf() : 0;
   }

   DLLLOCAL bool valid() const {
      return validp;
   }

   DLLLOCAL void reset() {
      if (validp)
         validp = false;
   }

   using QoreIteratorBase::deref;
   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference()) {
         if (val)
            val->deref(xsink);
         delete this;
      }
   }

   DLLLOCAL virtual const char* getName() const { return "SingleValueIterator"; }
};

#endif // _QORE_SINGLEVALUEITERATOR_H
