/*
  AutoNamespaceList.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  namespaces are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should 
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef QORE_AUTONAMESPACELIST_H

#define QORE_AUTONAMESPACELIST_H

#include <qore/QoreThreadLock.h>
#include <qore/ModuleManager.h>

#include <qore/safe_dslist>

typedef safe_dslist<qore_module_ns_init_t> anslist_t;

class AutoNamespaceList : public QoreThreadLock, public anslist_t
{
   public:
      DLLLOCAL void add(qore_module_ns_init_t ns_init)
      {
	 lock();
	 push_back(ns_init);
	 unlock();
      }
      
      DLLLOCAL inline void init(class QoreNamespace *rns, class QoreNamespace *qns)
      {
	 for (anslist_t::iterator i = begin(); i != end(); i++)
	    (*i)(rns, qns);
      }
};

DLLLOCAL extern class AutoNamespaceList ANSL;

#endif
