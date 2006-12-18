/*
  VRMutex.h

  recursive lock object

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_VRMUTEX_H

#define _QORE_VRMUTEX_H

#include <qore/LockedObject.h>
#include <qore/QoreCondition.h>

// class for tiered locking and deadlock detection
class VRMutex : public LockedObject, public QoreCondition
{
   private:
      int tid, count, waiting;
      class VLock *vl;

   public:
      DLLLOCAL VRMutex();
      DLLLOCAL int enter(class VLock *, class ExceptionSink *);
      DLLLOCAL void enter();
      DLLLOCAL int exit();
};

// VLNode and VLock are for nested locks when updating variables and objects
class VLNode {
   public:
      class VRMutex *g;
      class VLNode *next;

      DLLLOCAL VLNode(class VRMutex *gate);
};

// for locking
class VLock {
   private:
      class VLNode *head;
      class VLNode *tail;

   public:
      class VRMutex *waiting_on;
      int tid;

      DLLLOCAL VLock();
      DLLLOCAL ~VLock();
      DLLLOCAL void add(class VRMutex *g);
      DLLLOCAL void del();
      DLLLOCAL class VRMutex *find(class VRMutex *g);
};

#endif
