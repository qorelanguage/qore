/*
  ReferenceHolder.h

  Smart pointer like class that dereferences
  obtained pointer to a ReferenceObject in its destructor.

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

#include <qore/Exception.h>

//-----------------------------------------------------------------------------
// Example of use:
//
// ReferenceHolder<TibcoClass> holder(self->getReferencedPrivateData(...));
//
// holder->a_tibco_function();
//
// - deref() is automatic when the object goes out of scope
//
// Or
// ExceptionSink xsink;
// ReferenceHolder<TibcoClass> holder(self->getReferencePrivateData(...), &xsink);
//
template<typename T>
class ReferenceHolder
{
private:
  ReferenceHolder(const ReferenceHolder&); // not implemented
  ReferenceHolder& operator=(const ReferenceHolder&); // not implemented
  void* operator new(unsigned); // not implemented, make sure it is not new'ed

  T* p;
  ExceptionSink* xsink;
public:
  ReferenceHolder(T* p_, ExceptionSink* xsink_ = 0) : p(p_), xsink(xsink_) {}
  ~ReferenceHolder() { if(p) p->deref(xsink);}

  T* operator->() { return p; }
  operator bool() const { return p != 0; }
};

#endif

// EOF


