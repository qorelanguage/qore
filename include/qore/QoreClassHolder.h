/*
  QoreClassHolder.h

  Smart pointer like class that dereferences
  obtained pointer to a QoreClass in destructor.

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

#ifndef QORE_CLASS_HOLDER_H_
#define QORE_CLASS_HOLDER_H_

//-----------------------------------------------------------------------------
// Example of use:
//
// QoreClassHolder<TibcoClass> holder = self->getReferencedPrivateData(...);
// holder->a_tibco_function();
// - deref() is automatic

template<typename T>
class QoreClassHolder
{
private:
  QoreClassHolder(const QoreClassHolder&); // not implemented
  QoreClassHolder& operator=(const QoreClassHolder&); // not implemented

  T* p;
public:
  QoreClassHolder(T* p_) : p(p_)
  ~QoreClassHolder() { p->deref(); }

  T* operator->() { return p; }
};

#endif

// EOF


