/*
  ReferenceHelper.cc

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

#include <qore/Qore.h>

ReferenceHelper::ReferenceHelper(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink)
{
   vp = get_var_value_ptr(ref->getExpression(), &vl, xsink);
}

ReferenceHelper::~ReferenceHelper()
{
}

AbstractQoreNode *ReferenceHelper::getUnique(ExceptionSink *xsink)
{
   if (!(*vp)) 
      return 0;
   
   if (!(*vp)->is_unique()) {
      AbstractQoreNode *old = *vp;
      (*vp) = old->realCopy();
      old->deref(xsink);
   }
   return *vp;
}

int ReferenceHelper::assign(AbstractQoreNode *val, ExceptionSink *xsink)
{
   if (*vp) {
      (*vp)->deref(xsink);
      if (*xsink) {
	 (*vp) = 0;
	 discard(val, xsink);
	 return -1;
      }
   }
   (*vp) = val;
   return 0;
}

void ReferenceHelper::swap(ReferenceHelper &other)
{
   AbstractQoreNode *t = *other.vp;
   *other.vp = *vp;
   *vp = t;
}

const AbstractQoreNode *ReferenceHelper::getValue() const
{
   return *vp;
}
