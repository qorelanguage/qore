/*
  AutoNamespaceList.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/LockedObject.h>
#include <qore/ModuleManager.h>

class ANSNode {
   private:
      qore_module_ns_init_t ns_init;

   public:
      class ANSNode *next;

      inline ANSNode(qore_module_ns_init_t ni)
      {
	 ns_init = ni;
	 next = NULL;
      }
      inline void init(class Namespace *rns, class Namespace *qns)
      {
	 ns_init(rns, qns);
      }
};

class AutoNamespaceList : public LockedObject
{
   private:
      class ANSNode *head, *tail;

   public:
      inline AutoNamespaceList()
      {
	 head = tail = NULL;
      }

      inline ~AutoNamespaceList()
      {
	 while (head)
	 {
	    tail = head->next;
	    delete head;
	    head = tail;
	 }	 
      }

      inline void add(qore_module_ns_init_t ns_init)
      {
	 ANSNode *n = new ANSNode(ns_init);
	 lock();
	 if (tail)
	    tail->next = n;
	 else
	    head = n;
	 tail = n;
	 unlock();
      }
      
      inline void init(class Namespace *rns, class Namespace *qns)
      {
	 class ANSNode *w = head;

	 while (w)
	 {
	    w->init(rns, qns);
	    w = w->next;
	 }
      }
};

extern class AutoNamespaceList ANSL;

#endif
