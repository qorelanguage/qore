/*
 ClassRef.cc
 
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/ClassRef.h>
#include <qore/NamedScope.h>
#include <qore/Namespace.h>
#include <qore/QoreClass.h>

ClassRef::ClassRef(class NamedScope *n)
{
   cscope = n;
}

inline ClassRef::~ClassRef()
{
   if (cscope)
      delete cscope;
}

inline void ClassRef::resolve()
{
   if (cscope)
   {
      class QoreClass *qc = getRootNS()->parseFindScopedClass(cscope);
      if (qc)
	 cid = qc->getID();
      delete cscope;
      cscope = NULL;
   }
}

int ClassRef::getID() const
{
   return cid;
}
