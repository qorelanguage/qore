/*
  ReferenceHolder.h

  Smart pointer like class that dereferences
  obtained pointer to a QoreReferenceCounter in its destructor.

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#ifndef QORE_REFERENCE_HOLDER_H_
#define QORE_REFERENCE_HOLDER_H_

#include <stdlib.h>

//! a templated class to manage a reference count of an object that can throw a Qore-language exception when dereferenced
/** the destructor will call deref(ExceptionSink *)
    @code
    ReferenceHolder<QoreQFont> holder(self->getReferencedPrivateData(CID_QFONT, xsink), xsink);
    // the call to deref(ExceptionSink *) is automatic when the object goes out of scope
    if (*xsink)
       return 0;
    return holder->styleHint();
    @endcode
*/
template<typename T>
class ReferenceHolder {
   private:
      DLLLOCAL ReferenceHolder(const ReferenceHolder&); // not implemented
      DLLLOCAL ReferenceHolder& operator=(const ReferenceHolder&); // not implemented
      DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed
      
      T* p;
      ExceptionSink* xsink;

   public:
      //! creates an empty ReferenceHolder
      DLLLOCAL ReferenceHolder(ExceptionSink* xsink_) : p(0), xsink(xsink_) {}

      //! populates with object with data and the ExceptionSink pointer
      DLLLOCAL ReferenceHolder(T* p_, ExceptionSink* xsink_) : p(p_), xsink(xsink_) {}

      //! calls deref(ExceptionSink *) on the pointer being managed if not 0
      DLLLOCAL ~ReferenceHolder() { if (p) p->deref(xsink);}

      //! returns the pointer being managed
      DLLLOCAL T* operator->() { return p; }

      //! returns the pointer being managed
      DLLLOCAL T* operator*() { return p; }

      //! assigns a new pointer to the holder, dereferences the current pointer if any
      DLLLOCAL void operator=(T *nv)
      {
	 if (p)
	    p->deref(xsink);
	 p = nv;
      }

      //! releases the pointer to the caller
      DLLLOCAL T *release()
      {
	 T *rv = p;
	 p = 0;
	 return rv;
      }

      //! returns true if a non-0 pointer is being managed
      DLLLOCAL operator bool() const { return p != 0; }
      
      //! returns a pointer to the pointer being managed
      DLLLOCAL T **getPtrPtr() { return &p; }
};

//! manages a reference count of a pointer to a class that takes a simple "deref()" call with no arguments
/**
   SimpleRefHolder<QoreStringNode> str(new QoreStringNode(QCS_UTF8));
   // QoreString::concatUnicode() can raise a Qore-language exception if the code is invalid
   str->concatUnicode(code, xsink);
   return *xsink ? str.release() : 0;
 */
template<typename T>
class SimpleRefHolder
{
   private:
      DLLLOCAL SimpleRefHolder(const SimpleRefHolder&); // not implemented
      DLLLOCAL SimpleRefHolder& operator=(const SimpleRefHolder&); // not implemented
      DLLLOCAL void* operator new(size_t); // not implemented, make sure it is not new'ed
      
      T* p;

   public:
      DLLLOCAL SimpleRefHolder() : p(0) {}
      DLLLOCAL SimpleRefHolder(T* p_) : p(p_) {}
      DLLLOCAL ~SimpleRefHolder() { if (p) p->deref(); }
      
      DLLLOCAL T* operator->() { return p; }
      DLLLOCAL T* operator*() { return p; }
      DLLLOCAL void operator=(T *nv)
      {
         if (p)
            p->deref();
         p = nv;
      }
      DLLLOCAL T *release()
      {
         T *rv = p;
         p = 0;
         return rv;
      }
      DLLLOCAL operator bool() const { return p != 0; }
};

#endif

// EOF


